/******************************************************************************/
/*                                                                            */
/*                               RAINE GAME LISTS                             */
/*                                                                            */
/******************************************************************************/

#include "raine.h"
#include "games.h"
#include "neocd/neocd.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include "files.h"

struct GAME_MAIN *current_game;
int game_count;

// Notice, it's too early for _() to work, but never mind, it allows to mark the strings as translatable
// the array is re-affected later in init_lang
char *game_type[NB_GAME_TYPE] =
{ _("All"),
  _("Breakout"),
  _("Shoot'em up"),
  _("Beat'em up"),
  _("Puzzle"),
  _("Platform"),
  _("Misc"),
  _("Sport"),
  _("Adult"),
  _("Race"),
  _("Quizz"),
  _("Maze"),
  _("Fight"),
  _("Hack and slash"),
};

char *company_name[] =
{
   "Unknown",
   "Bootleg",
   "Capcom",
   "Data East",
   "Irem",
   "Jaleco",
   "Konami",
   "Nichibutsu",
   "Sega",
   "SNK",
   "Taito",
   "Tecmo",
   "Toaplan",
   "Banpresto",
   "UPL",
   "Namco",
   "NTC",
   "Visco",
   "Face",
   "Seibu",
   "Sammy",
   "Video System",
   "Alpha Denshi",
   "Nazca",
   "Technos",
   "NMK",
   "Sunsoft",
   "Aicom",
   "Yumekobo",
   "Hudson Soft",
   "Tad",
   "Comad",
   "Seta",
   "Excellent System",
   "Kaneko",
   "Raizing",
   "Cave",
   "Zeus",
   "Gaelco",
   "Psikyo",
   "East Technology",
   "Williams",
   "Hot-B",
   "Marble",
   "Wintechno",
   "Allumer",
   "Athena",
   "Yang Cheng",
   "Midway",
   "Nintendo",
   "Mitchell",
   "SemiCom",
   "ESD",
   "Spacy Industrial, Ltd.",
   "Electronic Devices",
   "NIX",
   "ADK",
   "Hack",
   "Takumi",
   "Viccom",
   "Eighting",
   "Saurus",
   "Eleven / Gavaking",
   "Noise Factory",
   "Eolith / SNK",
   "Mega / Playmore",
   "Evoga / Playmore",
   "Eolith / Playmore",
   "Noise Factory / Atlus",
   "Aiky / Taito",
   "SNK / Playmore",
   "Kyle Hodgetts",
   "F2 System",
   "Atari",
};

const int nb_companies = sizeof(company_name)/4;

#include "driver.c"

#ifndef RAINE_WIN32
// st_atime is to say the least unreliable in windows, it's really a bad idea to initialize this based on that
static void init_recent() {
    int n,nb = 0;
    for (n=0; n<game_count; n++) {
	if (!game_list[n]->last_played) {
	    struct stat buf;
	    char s[60];
	    snprintf(s,60,"%ssavedata/%s.hi",dir_cfg.exe_path,game_list[n]->main_name);
	    int ret = stat(s,&buf);
	    if (ret < 0) {
		snprintf(s,60,"%ssavedata/%s.epr",dir_cfg.exe_path,game_list[n]->main_name);
		ret = stat(s,&buf);
	    }
	    if (!ret) {
		game_list[n]->last_played = buf.st_atim.tv_sec;
		nb++;
	    }
	}
    }
}
#endif

static void read_game_stats() {
    FILE *f = fopen(get_shared("savedata" SLASH "stats"),"r");
    unsigned long found_last_played = 0;
    if (f) {
	while (!feof(f)) {
	    char buf[80];
	    buf[0] = 0;
	    myfgets(buf,80,f);
	    char *s = strchr(buf,'=');
	    if (s) {
		*s = 0;
		GAME_MAIN *game = find_game(buf);
		if (!game) {
		    printf("read_game_stats: game %s not found\n",buf);
		} else {
		    char *s2 = strchr(s+1,',');
		    if (!s2) {
			printf("read_game_stats: game %s, bad format\n",buf);
		    } else {
			*s2 = 0;
			char *s3 = strchr(s2+1,',');
			if (s3) {
			    *s3 = 0;
#ifdef __x86_64__
			    game->last_played = atoi(s3+1);
#else
			    game->last_played = atol(s3+1);
#endif
			    if (!found_last_played)
				found_last_played = game->last_played;
			}
			game->nb_loaded = atoi(s+1);
			game->time_played = atoi(s2+1);
		    }
		}
	    }
	}
	fclose(f);
    }
#ifndef RAINE_WIN32
    // Don't try to initialize anything in windows, st_atime is unreliable !
    if (!found_last_played)
	// call this if no "most recent" was saved yet
	init_recent();
#endif

}

void init_game_list(void)
{
   GAME_MAIN *swap;
   int i,j;

   /*

   count the games

   */

   game_count = sizeof(game_list) / sizeof(GAME_MAIN *);

   /*

   sort game_list by long_name (bubble sort)

   */

   for(i = game_count-1; i > 0; i --){

      for(j = 1; j <= i; j ++){

         if( stricmp(game_list[j-1]->long_name, game_list[j]->long_name) > 0){

            swap           = game_list[j-1];
            game_list[j-1] = game_list[j];
            game_list[j]   = swap;

         }

      }

   }

   /*

   no game is loaded

   */

   current_game = NULL;

   read_game_stats();
}

char *game_company_name(UINT8 company_id)
{
   return company_name[company_id];
}

int is_current_game(char *main_name)
{
    if (!current_game) return 0;
   return (!stricmp(current_game->main_name, main_name));
}

int is_neocd()
{
    if (!current_game) return 0;
    // A way to recognize a neocd game : no roms !
    // There shouldn't ever be any other driver without roms !
    return current_game->rom_list == NULL;
}

/*

Return the name of the parent if current game is a clone
else return current game name

*/

const char *parent_name() {
  char *dir;
  const DIR_INFO* dlist = current_game->dir_list;
  if (!dlist) return current_game->main_name;
  while (dlist->maindir) {
    dir = dlist->maindir;
    if( IS_ROMOF(dir) ){

      GAME_MAIN *game_romof;

      game_romof = find_game(dir+1);
      if (game_romof)
	return game_romof->main_name;
    }
    dlist++;
  }
  return current_game->main_name;
}

int is_clone(struct GAME_MAIN *current_game) {
  const DIR_INFO* dlist = current_game->dir_list;
  char *dir;
  while (dlist->maindir) {
    dir = dlist->maindir;
    if(( IS_ROMOF(dir) || IS_CLONEOF(dir)) && strcmp(dir+1,"neogeo")){
	return 1;
    }
    dlist++;
  }
  return 0;
}

/* This balanced binary tree algorythm is taken from
 * http://www.geeksforgeeks.org/sorted-array-to-balanced-bst/
 * nice and short ! */

#define elem char*

/* A Binary Tree node */
struct TNode
{
    elem data;
    struct TNode* left;
    struct TNode* right;
};

static struct TNode* newNode(elem data)
{
    struct TNode* node = (struct TNode*)
                         malloc(sizeof(struct TNode));
    node->data = data;
    node->left = NULL;
    node->right = NULL;

    return node;
}

static int get_string_from_tree(struct TNode *nd, char* value) {
    while (nd) {
	int i = stricmp(value,nd->data);
	if (i == 0) return 1;
	if (i < 0)
	    nd = nd->left;
	else
	    nd = nd->right;
    }
    return 0;
}

/* A function that constructs Balanced Binary Search Tree from a sorted array */
static struct TNode* sortedArrayToBST(elem arr[], int start, int end)
{
    /* Base Case */
    if (start > end)
      return NULL;

    /* Get the middle element and make it root */
    int mid = (start + end)/2;
    struct TNode *root = newNode(arr[mid]);

    /* Recursively construct the left subtree and make it
       left child of root */
    root->left =  sortedArrayToBST(arr, start, mid-1);

    /* Recursively construct the right subtree and make it
       right child of root */
    root->right = sortedArrayToBST(arr, mid+1, end);

    return root;
}

static int alloc_cache, nb_cache;
static struct TNode **cache;

static int cmpstringp(const void *p1, const void *p2)
{
    /* The actual arguments to this function are "pointers to
       pointers to char", but strcmp(3) arguments are "pointers
       to char", hence the following cast plus dereference */

    return stricmp(* (char * const *) p1, * (char * const *) p2);
}

static struct TNode *dir2cache(char *dirname) {
    DIR *dir = opendir(dirname);
    if (!dir) return NULL;
    struct dirent *dent;
    int nb = 0, nb_alloc = 0;
    char **names = NULL;
    while ((dent = readdir(dir))) {
	if (dent->d_name[0] == '.')
	    continue;
	if (nb == nb_alloc) {
	    nb_alloc += 10;
	    names = realloc(names,sizeof(char*)*nb_alloc);
	}
	names[nb++] = strdup(dent->d_name);
    }
    qsort(names,nb,sizeof(char*),cmpstringp);
    struct TNode *root = sortedArrayToBST(names, 0, nb-1);
    free(names);
    return root;
}

void add_cache(int ta) {
    if (ta >= nb_cache) {
	if (ta >= alloc_cache) {
	    alloc_cache += 10;
	    cache = realloc(cache,sizeof(struct TNode)*alloc_cache);
	}
	cache[nb_cache++] = dir2cache(dir_cfg.rom_dir[ta]);
    }
}

static void build_cache() {
    int ta;
    for(ta = 0; dir_cfg.rom_dir[ta]; ta ++){
	if(dir_cfg.rom_dir[ta][0]){
	    add_cache(ta);
	}
    }
}

static void free_tree(struct TNode *node) {
    if (node) {
	if (node->left)
	    free_tree(node->left);
	if (node->right)
	    free_tree(node->right);
	free(node->data);
	free(node);
    }
}

void del_cache(int i) {
    free_tree(cache[i]);
    cache[i] = NULL;
    nb_cache--;
    if (nb_cache > i)
	memmove(&cache[i],&cache[i+1],(nb_cache-i)*sizeof(struct TNode*));
}

void free_cache() {
    while (nb_cache > 0)
	del_cache(nb_cache-1);
    free(cache);
    alloc_cache = 0;
    cache = NULL;
}

void update_cache(int i) {
    free_tree(cache[i]);
    cache[i] = dir2cache(dir_cfg.rom_dir[i]);
}

static int rom_exists(int ta,char *filename) {
    int ret = get_string_from_tree(cache[ta],filename);
    return ret;
}

int game_exists(GAME_MAIN **my_game_list,int num)
{
   const DIR_INFO *dir_list;
   char str[256];
   UINT32 ta;

   build_cache();
   dir_list = my_game_list[num]->dir_list;

   while( (dir_list->maindir) ){

      if((dir_list->maindir[0] != '#')&&(dir_list->maindir[0] != '$')){

	 for(ta = 0; dir_cfg.rom_dir[ta]; ta ++){

	    if(dir_cfg.rom_dir[ta][0]){

	       sprintf(str,"%s.zip", dir_list->maindir);
	       if((rom_exists(ta,str))) return 1;

	       sprintf(str,"%s.7z", dir_list->maindir);
	       if((rom_exists(ta,str))) return 1;

	       if((rom_exists(ta,dir_list->maindir))) return 1;

	    }

	 }

      }

      dir_list++;
   }

   return 0;

}

