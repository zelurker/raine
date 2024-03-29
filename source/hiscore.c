/*	hiscore.c
**	generalized high score save/restore support

Taken from xmame 37b7.1
*/

#include "gameinc.h"
#include "hiscore.h"
#include "arpro.h"
#include "conf-cpu.h"
#include "files.h"

#define MAX_CONFIG_LINE_SIZE 48

// #define VERBOSE 1

#if VERBOSE || defined(RAINE_DEBUG)
#define LOG(x)	fprintf(stderr,x)
#else
#define LOG(x)
#endif

static char *db_filename = "hiscore.dat"; /* high score definition file */
#define MAX_NCHECK 16
static UINT32 adr_check[MAX_NCHECK];
static int ncheck;

static struct
{
	int hiscores_have_been_loaded;

	struct mem_range
	{
		UINT32 cpu, addr, num_bytes, start_value, end_value;
		struct mem_range *next;
	} *mem_range;
} state;

/*****************************************************************************/

void computer_writemem_byte(int cpu, int addr, int value)
{
  gen_cpu_write_byte(addr,value);
}

int computer_readmem_byte(int cpu, int addr)
{
  return gen_cpu_read_byte(addr);
}

int hiscores_loaded() {
    return state.hiscores_have_been_loaded;
}

/*****************************************************************************/

static void copy_to_memory (int cpu, int addr, const UINT8 *source, int num_bytes)
{
	int i;
	for (i=0; i<num_bytes; i++)
	{
		computer_writemem_byte (cpu, addr+i, source[i]);
		if (source[i] && ncheck < MAX_NCHECK)
		    adr_check[ncheck++] = addr+i;
	}
}

static void copy_from_memory (int cpu, int addr, UINT8 *dest, int num_bytes)
{
	int i;
	for (i=0; i<num_bytes; i++)
	{
		dest[i] = computer_readmem_byte (cpu, addr+i);
	}
}

/*****************************************************************************/

/*	hexstr2num extracts and returns the value of a hexadecimal field from the
	character buffer pointed to by pString.

	When hexstr2num returns, *pString points to the character following
	the first non-hexadecimal digit, or NULL if an end-of-string marker
	(0x00) is encountered.

*/
static UINT32 hexstr2num (const char **pString)
{
	const char *string = *pString;
	if (*string == '@') {
	    // New format
	    const char *s = strchr(string,',');
	    /* Normal syntax of this @ tag is @maincpu,program,field,field...
	     * But I added an old entry to some hiscore.dat, which reads like that :
	     * @:field:field...
	     * I should have checked it better, but now that it's done (and it's old !), let's handle it...
	     * it's for viewpoint */
	    if (s)
		s = strchr(s+1,',');
	    else
		s = strchr(string,':');
	    if (s) {
		*pString = s+1;
		return 0;
	    }
	}

	UINT32 result = 0;
	if (string)
	{
		for(;;)
		{
			char c = *string++;
			int digit;

			if (c>='0' && c<='9')
			{
				digit = c-'0';
			}
			else if (c>='a' && c<='f')
			{
				digit = 10+c-'a';
			}
			else if (c>='A' && c<='F')
			{
				digit = 10+c-'A';
			}
			else
			{
				/* not a hexadecimal digit */
				/* safety check for premature EOL */
				if (!c) string = NULL;
				break;
			}
			result = result*16 + digit;
		}
		*pString = string;
	}
	return result;
}

/*	given a line in the hiscore.dat file, determine if it encodes a
	memory range (or a game name).
	For now we assume that CPU number is always a decimal digit, and
	that no game name starts with a decimal digit.
*/
static int is_mem_range (const char *pBuf)
{
	char c;
	if (*pBuf == '@') {// new format
	    const char *quote = pBuf;
	    for (int n=1; n<=5 && quote; n++) // at least 5 , on the line...
		quote = strchr(quote+1,',');
	    return quote != NULL;
	}
	for(;;)
	{
		c = *pBuf++;
		if (c == 0) return 0; /* premature EOL */
		if (c == ':') break;
	}
	c = *pBuf; /* character following first ':' */

	return	(c>='0' && c<='9') ||
			(c>='a' && c<='f') ||
			(c>='A' && c<='F');
}

/*	matching_game_name is used to skip over lines until we find <gamename>: */
static int matching_game_name (const char *pBuf, const char *name)
{
	while (*name)
	{
		if (*name++ != *pBuf++) return 0;
	}
	return (*pBuf == ':');
}

/*****************************************************************************/

/* safe_to_load checks the start and end values of each memory range */
static int safe_to_load (void)
{
	struct mem_range *mem_range = state.mem_range;
	while (mem_range)
	{
		if ((UINT32)(computer_readmem_byte (mem_range->cpu, mem_range->addr)) !=
			mem_range->start_value)
		{
#ifdef RAINE_DEBUG
		    printf("hiscores: bad addr %x\n",mem_range->addr);
#endif
			return 0;
		}
		if ((UINT32)(computer_readmem_byte (mem_range->cpu, mem_range->addr + mem_range->num_bytes - 1)) !=
			mem_range->end_value)
		{
#ifdef RAINE_DEBUG
		    printf("hiscores: bad end %x\n",mem_range->addr + mem_range->num_bytes - 1);
#endif
			return 0;
		}
		mem_range = mem_range->next;
	}
	return 1;
}

/* hs_free disposes of the mem_range linked list */
static void hs_free (void)
{
	struct mem_range *mem_range = state.mem_range;
	while (mem_range)
	{
		struct mem_range *next = mem_range->next;
		free (mem_range);
		mem_range = next;
	}
	state.mem_range = NULL;
}

static char str[256];
static char *myget_filename() {

   snprintf(str,256,"%ssavedata" SLASH "%s%s.hi", dir_cfg.exe_path, current_game->main_name,(is_neocd() ? "-neocd" : ""));
   return str;
}

void hs_load (void)
{
  FILE *f = fopen(myget_filename(), "rb");
  //  LOG("have_been_loaded %s\n",str);
  state.hiscores_have_been_loaded = 1;
  LOG(("hs_load\n"));
  if (f)
    {
      struct mem_range *mem_range = state.mem_range;
      LOG(("hiscores: loading...\n"));
      while (mem_range)
	{
	  UINT8 *data = malloc (mem_range->num_bytes);
	  if (data)
	    {
	      /*	this buffer will almost certainly be small
			enough to be dynamically allocated, but let's
			avoid memory trashing just in case
	      */
	      int red = fread (data, 1,mem_range->num_bytes,f);
	      if (red < mem_range->num_bytes) {
		  print_debug("hiscores: invalid read, expected %d, got %d\n",mem_range->num_bytes,red);
	      } else
		  copy_to_memory (mem_range->cpu, mem_range->addr, data, mem_range->num_bytes);
	      free (data);
	    }
	  mem_range = mem_range->next;
	}
      fclose (f);
    }
}

static void hs_save (void)
{
    if (is_neocd() && ReadLongSc(&RAM[0x11e]) > 0xc00000) {
	LOG("hiscores: neocd in bios, can't save hiscores anymore\n");
	return;
    }

    if (!state.hiscores_have_been_loaded)
	return;

  FILE *f = fopen(myget_filename(), "wb");
  if (f)
    {
      struct mem_range *mem_range = state.mem_range;
      LOG(("hiscores: saving...\n"));
      while (mem_range)
	{
	  UINT8 *data = malloc (mem_range->num_bytes);
	  if (data)
	    {
	      /*	this buffer will almost certainly be small
			enough to be dynamically allocated, but let's
			avoid memory trashing just in case
	      */
	      copy_from_memory (mem_range->cpu, mem_range->addr, data, mem_range->num_bytes);
	      fwrite(data, mem_range->num_bytes,1,f);
	      free(data);
	    }
	  mem_range = mem_range->next;
	}
      fclose(f);
    }
  state.hiscores_have_been_loaded = 0;
}

/*****************************************************************************/
/* public API */

/* call hs_open once after loading a game */
void hs_open ()
{
  FILE *f;
  f = fopen (get_shared(db_filename), "r");
  state.mem_range = NULL;
  ncheck = 0;

  if (f)
    {
      char buffer[MAX_CONFIG_LINE_SIZE];
      enum { FIND_NAME, FIND_DATA, FETCH_DATA } mode;
      mode = FIND_NAME;

      while (fgets (buffer, MAX_CONFIG_LINE_SIZE, f))
	{
	  if (mode==FIND_NAME)
	    {
	      if (matching_game_name (buffer, current_game->main_name))
		{
		  mode = FIND_DATA;
		  LOG(("hs config found!\n"));
		}
	    }
	  else if (is_mem_range (buffer))
	    {
	      const char *pBuf = buffer;
	      struct mem_range *mem_range = malloc(sizeof(struct mem_range));
	      if (mem_range)
		{
		  mem_range->cpu = hexstr2num (&pBuf);
		  mem_range->addr = hexstr2num (&pBuf);
		  mem_range->num_bytes = hexstr2num (&pBuf);
		  mem_range->start_value = hexstr2num (&pBuf);
		  mem_range->end_value = hexstr2num (&pBuf);

		  mem_range->next = NULL;
		  {
		    struct mem_range *last = state.mem_range;
		    while (last && last->next) last = last->next;
		    if (last == NULL)
		      {
			state.mem_range = mem_range;
		      }
		    else
		      {
			last->next = mem_range;
		      }
		  }

		  mode = FETCH_DATA;
		}
	      else
		{
		  hs_free();
		  break;
		}
	    }
	  else
	    {
				/* line is a game name */
	      if (mode == FETCH_DATA) break;
	    }
	}
      fclose (f);
    }
}

/* call hs_init when emulation starts, and when the game is reset */
void hs_init (void)
{
  struct mem_range *mem_range = state.mem_range;
  state.hiscores_have_been_loaded = 0;

  while (mem_range)
    {
      computer_writemem_byte(
			     mem_range->cpu,
			     mem_range->addr,
			     ~mem_range->start_value
			     );

      computer_writemem_byte(
			     mem_range->cpu,
			     mem_range->addr + mem_range->num_bytes-1,
			     ~mem_range->end_value
			     );
      mem_range = mem_range->next;
    }
  if (state.mem_range)
    {
      LOG("hiscores: range ready\n");
      if (!state.hiscores_have_been_loaded)
	{

	  if (safe_to_load()) {
	    hs_load();
	    LOG("hiscores loaded\n");
	  } else {
	    LOG("hiscores not safe to load\n");
	  }
	}
    }
}

/* call hs_update periodically (i.e. once per frame) */
void hs_update (void)
{
  if (state.mem_range)
    {
      if (!state.hiscores_have_been_loaded)
	{
	  if (safe_to_load()) {
	    hs_load();
	    LOG("hiscores loaded\n");
	  } else {
	    LOG("hiscores not safe\n");
	  }
	} else if (ncheck) {
	    int nzero = 0;
	    int n;
	    for (n=0; n<ncheck; n++) {
		if (gen_cpu_read_byte(adr_check[n]) == 0)
		    nzero++;
	    }
	    if (nzero == ncheck) { // area was cleared, probably in service mode...
		state.hiscores_have_been_loaded = 0;
		ncheck = 0;
	    }
	}
    }
}

/* call hs_close when done playing game */
void hs_close (void)
{
  if (state.hiscores_have_been_loaded) hs_save();
  hs_free();
}

int current_game_has_hiscores()
{
  return (state.mem_range != NULL);
}
