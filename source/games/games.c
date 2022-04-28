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

struct GAME_MAIN *current_game;
int game_count;

char *game_type[NB_GAME_TYPE] =
{ "All",
  "Breakout",
  "Shoot'em up",
  "Beat'em up",
  "Puzzle",
  "Platform",
  "Misc",
  "Sport",
  "Adult",
  "Race",
  "Quizz",
  "Maze",
  "Fight",
  "Hack and slash",
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

