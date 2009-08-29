#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "raine.h"
#include "neocd.h"
#include "cache.h"
#include "cdrom.h"
#include "sasound.h"
#include "savegame.h"
#include "newmem.h"
#include "files.h"
#include "starhelp.h"

#ifdef PSP
#define printf	pspDebugScreenPrintf
#endif

typedef struct {
  char name[15];
  unsigned int offset, len, crc;
} file_entry;

static file_entry *cache[7];
static int used[7];
static int max[7];

void get_cache_origin(int type, int offset, char **name, int *nb) {
  if (cache[type]) {
    file_entry *list = cache[type];
    int n = 0;
    while (n < used[type] && list[n].offset+list[n].len < offset)
      n++;
    if (list[n].offset < offset && list[n].offset+list[n].len > offset) {
      *name = list[n].name;
      *nb = offset-list[n].offset;
    }
  }
}

static UINT8 *get_src(int type, int offset) {
  switch(type) {
    case PRG_TYPE: return RAM + offset; break;
    case Z80_TYPE: return Z80ROM + offset; break;
    case FIX_TYPE: return neogeo_fix_memory + offset; break;
    case SPR_TYPE: return GFX + offset; break;
    case PCM_TYPE: return PCMROM + offset; break;
  }
  return NULL;
}

void put_override(int type, char *name) {
  if (cache[type]) {
    file_entry *list = cache[type];
    int n = 0;
    while (n < used[type] && strcasecmp(list[n].name,name))
      n++;
    if (n< used[type]) { // found
      char filename[256];
      sprintf(filename,"%soverride", dir_cfg.exe_path);
      mkdir_rwx(filename);
      sprintf(&filename[strlen(filename)],SLASH "%s", name);
      UINT8 *src = get_src(type,list[n].offset);
      if (type == PRG_TYPE) 
        ByteSwap(src,list[n].len);
      printf("write %s len %d\n",list[n].name,list[n].len);
      FILE *f = fopen(filename,"wb");
      fwrite(src,1,list[n].len,f);
      fclose(f);
      if (type == PRG_TYPE) 
        ByteSwap(src,list[n].len);
    }
  }
}

int file_cache(char *filename, int offset, int size,int type) {
  int use;
  char *s = strrchr(filename,'/');
  if (s)
    filename = s+1; // no path here !

#ifdef DEBUG_CACHE
  if (type == 2)
    printf("looking for %s offset %x size %x\n",filename,offset,size);
#endif

  if (cache[type]) {
    file_entry *list = cache[type];
    int n = 0;
    while (n < used[type] && list[n].offset < offset)
      n++;
    if (n == used[type]) { // reached the end of list
      use = used[type]++;
      if (used[type] >= max[type]) {
	// I use >= on purpose because I want more room for an eventual insertion
	// see memmove further
	max[type]+=32;
	cache[type] = realloc(cache[type],sizeof(file_entry)*max[type]);
      }
    } else { // found an offset >= this one
      int end;
      while (n < used[type] && list[n].offset == offset && list[n].len > size)
	n++; // leave the bigger ones, go forward...
      if (n < used[type] && list[n].offset == offset && list[n].len == size &&
	  !strcmp(list[n].name,filename)) {
	// already have it !
	int old_crc = list[n].crc;
	// printf("!!! already have it !!!\n");
	/* Since this one is going to be reloaded, we remove the ones
	 * overwriten just after it */
	n++;
	end = offset+size;
	while (n < used[type] && list[n].offset >= offset && list[n].offset < end) {
	  /* We don't test the end here. The idea is that a new file overwrites
	   * the start of an old one. In this case we consider the whole old
	   * file to be bad... */
#ifdef DEBUG_CACHE
	  if (type == 2) {
	    printf("remove %s %x %x\n",list[n].name,list[n].offset,list[n].len);
	  }
#endif
	  memmove(&list[n],&list[n+1],sizeof(file_entry)*(used[type]-(n+1)));
	  used[type]--;
	}
	n--;
	cache_set_crc(offset,size,type);
	if (list[n].crc != old_crc) {
	  print_debug("cache: %s: crc differ (%x & %x)\n",filename,old_crc,list[n].crc);
	  return 0;
	}
	print_debug("cache: %s: same crc %x\n",filename,old_crc);
	return 0;
      }
      end = offset+size;
      // First : skip eventually biger files loaded here (we overwrite the beg so we
      // should be loaded after them
      while (n < used[type] && list[n].offset == offset && list[n].offset+list[n].len > end) {
#ifdef DEBUG_CACHE
	if (type == 2) {
	  printf("skip %s %x %x\n",list[n].name,list[n].offset,list[n].len);
	}
#endif
	n++;
      }

      // remove those which are overwriten (if there are some)
      // We also remove the ones partially overwritten, because I don't think
      // the neocd would keep accessing some data partially overwritten
      // and it will make the cache handling much easier
      while (n < used[type] && list[n].offset >= offset && list[n].offset < end) {
#ifdef DEBUG_CACHE
	if (type == 2) {
	  printf("remove %s %x %x\n",list[n].name,list[n].offset,list[n].len);
	}
#endif
	memmove(&list[n],&list[n+1],sizeof(file_entry)*(used[type]-(n+1)));
	used[type]--;
      }
      // otherwise insertion needed at this place
      if (n < used[type]) {
#ifdef DEBUG_CACHE
	if (type == 2)
	  printf("insertion before %s %x %x\n",list[n].name,list[n].offset,list[n].len);
#endif
	memmove(&list[n+1],&list[n],(used[type]-n)*sizeof(file_entry));
      }
      use = n;
      used[type]++;
      if (used[type] >= max[type]) {
	max[type]+=32;
	cache[type] = realloc(cache[type],sizeof(file_entry)*max[type]);
      }
    }
  } else { // first allocation
    if (!max[type])
      max[type] = 32;
    cache[type] = malloc(sizeof(file_entry)*max[type]);
    used[type] = 1;
    use = 0;
  }
  strcpy(cache[type][use].name,filename);
  cache[type][use].offset = offset;
  cache[type][use].len = size;
  return 0; // not already cached, new file
}

  int get_used(int type) {
    if (type < 7)
      return used[type];
    return -1;
  }

void clear_file_cache() {
  int n;
  for (n=0; n<7; n++) {
    if (cache[n]) {
      free(cache[n]);
      cache[n] = NULL;
      used[n] = 0;
      max[n] = 32;
    }
  }
}

void cache_set_crc(int offset,int size,int type) {
  /* The problem of neocd is that everything is loaded in ram, which means that it can
     be changed dynamically after it's loaded. Most of the time this is only changed
     by accessing the upload area so we could probably only test the upload area.
     But the 68k and z80 prg memory, can be changed directly or with some patches
     for the z80. So the best way is to add a crc */
  if (cache[type]) {
    file_entry *list = cache[type];
    int n = 0;
    UINT8 *src=NULL;
    unsigned int crc;

    while (n < used[type] && (list[n].offset != offset || list[n].len != size))
      n++;

    /* Normally this function is called AFTER file_cache, which means that this loop
       will always find the correct file in the cache, we just need to compute the crc
       here */

    switch(type) {
      case PRG_TYPE: src= RAM + offset; break;
      case Z80_TYPE: src= Z80ROM + offset; break;
      case FIX_TYPE: src= neogeo_fix_memory + offset; break;
      case SPR_TYPE: src= GFX + offset; break;
      case PCM_TYPE: src= PCMROM + offset; break;
    }

    crc = 0;
    /* If the size is not a multiple of 4, then we loose precision, a few bytes are
       ignored at the end of the file. Oh well it should be enough just to check
       if an area has changed anyway... ! */
    do {
      crc += *((int *)src); // an xor here loops on 0 if the rom is filled
      // with the same value... a simple add should be safer (avoid complexity)
      size -= 4;
      src += 4;
    } while (size>=4);
    list[n].crc = crc;
  }
}

#define MAX_UPLOAD 64

static void cache_load_spr(UINT8 *buff, int len) {
  /* Actually the cache isn't directly loaded from the savegame, we use
   * the filenames and uploads areas from the savegame and feed them directly
   * to the existing cache which existed before the load */
  file_entry *tcache = (file_entry *)buff;
  int nb_upload = 0, upload_offset[MAX_UPLOAD],upload_size[MAX_UPLOAD],n;
  int old_cd = cdrom_speed;
  cdrom_speed = 0;
  while (len > 0) {
    if (!strcmp(tcache->name,"upload")) {
      upload_offset[nb_upload] = tcache->offset;
      upload_size[nb_upload++] = tcache->len;
      if (nb_upload == MAX_UPLOAD) {
	MessageBox("error","cache_load_spr: max_upload overflow, expect trouble","ok");
	return;
      }
      len -= tcache->len;
    } else {
      neogeo_cdrom_load_spr_file(tcache->name, tcache->offset/2);
    }
    tcache++;
    len -= sizeof(file_entry);
  }
  buff = (UINT8 *)tcache;
  for (n = 0; n<nb_upload; n++) {
    print_debug("restoring spr upload %x,%x\n",upload_offset[n],
	upload_size[n]);
    memcpy(GFX+upload_offset[n],buff,upload_size[n]);
    UINT8 *usage_ptr = video_spr_usage + (upload_offset[n]>>8);
    int i;
    for (i=0; i<upload_size[n]; i+= 256) {
      int res = 0;
      int j;
      for (j=0; j<256; j++) {
	if (buff[i+j])
	  res++;
      }
      if (res == 0) // all transp
	usage_ptr[i/256] = 0;
      else if (res == 256)
	usage_ptr[i/256] = 2; // all solid
      else
	usage_ptr[i/256] = 1; // semi
    }
    buff += upload_size[n];
  }
  cdrom_speed = old_cd;
}

void cache_save_spr(UINT8 **buff, int *len) {
  int upload_size = 0;
  int n;
  for (n=0; n<used[SPR_TYPE]; n++) {
    if (!strcmp(cache[SPR_TYPE][n].name,"upload")) {
      upload_size += cache[SPR_TYPE][n].len;
    }
  }
  *len = used[SPR_TYPE]*sizeof(file_entry);
  *buff = AllocateMem(*len + upload_size);
  memcpy(*buff,cache[SPR_TYPE],*len);
  if (upload_size) {
    UINT8 *tmp = *buff + *len;
    for (n=0; n<used[SPR_TYPE]; n++) {
      if (!strcmp(cache[SPR_TYPE][n].name,"upload")) {
	memcpy(tmp,GFX + cache[SPR_TYPE][n].offset,cache[SPR_TYPE][n].len);
	tmp += cache[SPR_TYPE][n].len;
      }
    }
  }
  *len += upload_size;
}

static void cache_load_fix(UINT8 *buff, int len) {
  file_entry *tcache = (file_entry *)buff;
  int nb_upload = 0, upload_offset[MAX_UPLOAD],upload_size[MAX_UPLOAD],n;
  int old_cd = cdrom_speed;
  cdrom_speed = 0;
  while (len > 0) {
    if (!strcmp(tcache->name,"upload")) {
      upload_offset[nb_upload] = tcache->offset;
      upload_size[nb_upload++] = tcache->len;
      if (nb_upload == MAX_UPLOAD) {
	MessageBox("error","cache_load_fix: max_upload overflow, expect trouble","ok");
	return;
      }
      len -= tcache->len;
    } else {
      neogeo_cdrom_load_fix_file(tcache->name, tcache->offset);
    }
    tcache++;
    len -= sizeof(file_entry);
  }
  buff = (UINT8 *)tcache;
  for (n = 0; n<nb_upload; n++) {
    print_debug("restoring fix upload %x,%x\n",upload_offset[n],
	upload_size[n]);
    memcpy(neogeo_fix_memory+upload_offset[n],buff,upload_size[n]);
    UINT8 *usage_ptr = video_fix_usage + (upload_offset[n]>>5);
    int i;
    for (i=0; i<upload_size[n]; i+= 32) {
      int res = 0;
      int j;
      for (j=0; j<32; j++) {
	if (buff[i+j])
	  res++;
      }
      if (res == 0) // all transp
	usage_ptr[i/32] = 0;
      else if (res == 32)
	usage_ptr[i/32] = 2; // all solid
      else
	usage_ptr[i/32] = 1; // semi
    }
    buff += upload_size[n];
  }
  cdrom_speed = old_cd;
}

void cache_save_fix(UINT8 **buff, int *len) {
  int upload_size = 0;
  int n;
  for (n=0; n<used[FIX_TYPE]; n++) {
    if (!strcmp(cache[FIX_TYPE][n].name,"upload")) {
      upload_size += cache[FIX_TYPE][n].len;
    }
  }
  *len = used[FIX_TYPE]*sizeof(file_entry);
  *buff = AllocateMem(*len + upload_size);
  memcpy(*buff,cache[FIX_TYPE],*len);
  if (upload_size) {
    UINT8 *tmp = *buff + *len;
    for (n=0; n<used[FIX_TYPE]; n++) {
      if (!strcmp(cache[FIX_TYPE][n].name,"upload")) {
	memcpy(tmp,GFX + cache[FIX_TYPE][n].offset,cache[FIX_TYPE][n].len);
	tmp += cache[FIX_TYPE][n].len;
      }
    }
  }
  *len += upload_size;
}

static void cache_load_pcm(UINT8 *buff, int len) {
  file_entry *tcache = (file_entry *)buff;
  int nb_upload = 0, upload_offset[MAX_UPLOAD],upload_size[MAX_UPLOAD],n;
  int old_cd = cdrom_speed;
  cdrom_speed = 0;
  while (len > 0) {
    if (!strcmp(tcache->name,"upload")) {
      printf("upload in pcm area, getting crazy !\n");
      upload_offset[nb_upload] = tcache->offset;
      upload_size[nb_upload++] = tcache->len;
      if (nb_upload == MAX_UPLOAD) {
	MessageBox("error","cache_load_pcm: max_upload overflow, expect trouble","ok");
	return;
      }
      len -= tcache->len;
    } else {
      neogeo_cdrom_load_pcm_file(tcache->name, tcache->offset);
    }
    tcache++;
    len -= sizeof(file_entry);
  }
  buff = (UINT8 *)tcache;
  for (n = 0; n<nb_upload; n++) {
    print_debug("restoring pcm upload %x,%x\n",upload_offset[n],
	upload_size[n]);
    memcpy(PCMROM+upload_offset[n],buff,upload_size[n]);
    buff += upload_size[n];
  }
  cdrom_speed = old_cd;
}

void cache_save_pcm(UINT8 **buff, int *len) {
  int n;
  int upload_size = 0;
  for (n=0; n<used[PCM_TYPE]; n++) {
    if (!strcmp(cache[PCM_TYPE][n].name,"upload")) {
      upload_size += cache[PCM_TYPE][n].len;
    }
  }
  *len = used[PCM_TYPE]*sizeof(file_entry);
  *buff = AllocateMem(*len + upload_size);
  memcpy(*buff,cache[PCM_TYPE],*len);
  if (upload_size) {
    UINT8 *tmp = *buff + *len;
    for (n=0; n<used[PCM_TYPE]; n++) {
      if (!strcmp(cache[PCM_TYPE][n].name,"upload")) {
	memcpy(tmp,PCMROM + cache[PCM_TYPE][n].offset,cache[PCM_TYPE][n].len);
	tmp += cache[PCM_TYPE][n].len;
      }
    }
  }
  *len += upload_size;
}

// the prg handling is the easiest one because I don't handle uploads there
// I need it only for the translator, so that I am able to find where some
// piece of data comes from
// (it would be cool to be able to save only the ram which isn't directly loaded
// from cd, but it would require not to save directly RAM, which would be messy)
/* Actually it wasn't so easy because of the crcs :
 * when we save the cache, we save the crcs
 * if we restore this area like the others we call file_cache, but never
 * cache_set_crc because we don't know if the file has been overwriten or not.
 * The consequence is that the status of the crc is unknown ! if the same
 * file is loaded after this by the game it will believe the correct crc is
 * the one file_cache has put here : 0 most of the time !!!
 * So the correct way is to import everything into the cache, crcs included,
 * without changing anything to the ram area. */
static void cache_load_prg(UINT8 *buff, int len) {
  file_entry *tcache = (file_entry *)buff;
  int old_cd = cdrom_speed;
  cdrom_speed = 0;
  used[PRG_TYPE] = 0;
  while (len > 0) {
    if (used[PRG_TYPE] >= max[PRG_TYPE]) {
      // I use >= on purpose because I want more room for an eventual insertion
      // see memmove further
      max[PRG_TYPE]+=32;
      cache[PRG_TYPE] = realloc(cache[PRG_TYPE],sizeof(file_entry)*max[PRG_TYPE]);
    }
    cache[PRG_TYPE][used[PRG_TYPE]++] = *tcache; // copy everything

    tcache++;
    len -= sizeof(file_entry);
  }
  cdrom_speed = old_cd;
}

void cache_save_prg(UINT8 **buff, int *len) {
  *len = used[PRG_TYPE]*sizeof(file_entry);
  *buff = AllocateMem(*len);
  memcpy(*buff,cache[PRG_TYPE],*len);
}

void prepare_cache_save() {
  AddSaveDynCallbacks(0,cache_load_spr,cache_save_spr);
  AddSaveDynCallbacks(1,cache_load_fix,cache_save_fix);
  AddSaveDynCallbacks(2,cache_load_pcm,cache_save_pcm);
  // Actually it doesn't work for prg, the ram can be changed directly
  // and some programs seem to store their data in the middle of the code
  // so if you save it here, all the data is erased when you restore a
  // savegame !!!
  AddSaveDynCallbacks(3,cache_load_prg,cache_save_prg);
}

