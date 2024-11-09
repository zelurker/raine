#include "gameinc.h"
#include "files.h"
#ifdef SDL
#include "gui/menuitem.h"
/* Can now read also command.dat which has almost the same format as history.dat
 * in this case this menu_commands is filled with the sub-section names
 * and commands_buff contains the associated text */
menu_item_t *menu_commands;
#endif
#include "history.h"
#ifdef HAVE_6502
#include "m6502hlp.h"
#endif
#include "sasound.h"
#include "config.h"
#include "sh2.h"

int nb_commands;
char **commands_buff;
char *history = NULL,*driver_info = NULL;
static int size;
static int used;
static char *commands;

static void hist_add(const char *format, ...)
{
    char str[4096];
    va_list ap;
    va_start(ap,format);
    vsnprintf(str,4096,format,ap);
    str[4095] = 0;
    va_end(ap);
    int len = strlen(str)+1;
    while (used + len >= size) {
	size += 1024;
	commands = realloc(commands,size);
	if (!commands)
	    return;
    }
    sprintf(&commands[used],"%s\n",str);
    used += len;
}

void hist_open(char *name,const char *game) {
  FILE *f;
#define LINE 1024
  char str[LINE+1];
  int old_size = 0,old_use;
  char *old_commands;
  size = 1024;
  used = 0;
  commands = NULL;
  if (strcmp(name,"history.dat")) { // not history.dat -> free old commands
      done_commands();
  }
  f = fopen (get_shared(name), "r");
  while (f && !feof(f)) {
      myfgets(str,LINE,f);
      // The use of strncmp is to be able to handle crlfs files in unix
      if (!strncmp(str,"$info=",6)) {
	  if (commands) break; // End of current game
	  const char *target = game;
	  char *s = strstr(str,target);
	  int len = strlen(target);
	  while (s) {
	      // $info can contain a list of clones, so search the game name inside
	      if (s && (*(s-1)=='=' || *(s-1)==',' || *(s-1) == ' ') &&
		      (*(s+len)==',' || *(s+len)==' ' || s[len]==0)) { // found it
		  commands = malloc(size);
		  *commands = 0;
		  break;
	      }
	      s = strstr(s+1,target);
	  }
      } else if (!commands)
	  continue;
      // After this line we are parsing data for the current loaded game
      else if (!strncmp(str,"$cmd",4)) { // subsection
	  // Specific to command.dat
	  myfgets(str,LINE,f);
#ifdef SDL
	  menu_commands = realloc(menu_commands,sizeof(menu_item_t)*(nb_commands+2));
	  memset(&menu_commands[nb_commands],0,sizeof(menu_item_t)*2);
#endif
	  commands_buff = realloc(commands_buff,sizeof(char*)*(nb_commands+1));
	  // Also clear last element after this one
	  if (str[0] == '[') { // name between useless [] apparently...
	      char *s = &str[strlen(str)-1];
	      *s-- = 0;
	      while (*s == '-') // and a line of - !
		  *s-- = 0;
#ifdef SDL
	      menu_commands[nb_commands].label = strdup(str+1);
#endif
	  } else {
	      // In case some entry doesn't have these []...
	      char *s = &str[strlen(str)-1];
	      while (*s == '-') // and a line of - !
		  *s-- = 0;
#ifdef SDL
	      menu_commands[nb_commands].label = strdup(str);
#endif
	  }
	  if (old_size) {
	      fatal_error("history: subsection inside subsection !");
	  }
	  old_size = size;
	  old_use = used;
	  used = 0;
	  old_commands = commands;
	  size = 1024;
	  commands = malloc(size);
	  nb_commands++;
      } else if (!strncmp(str,"$end",4) || feof(f)) {
	  if (old_size) { // inside subsection, return to main...
	      commands_buff[nb_commands-1] = commands;
	      commands = old_commands;
	      size = old_size;
	      old_size = 0;
	      used = old_use;
	  } else
	      break; // end of main section in this case !
      } else if((str[0]!='$' || strncmp(str,"$bio",4))) {
	  char *q = str;
	  while ((q = strstr(q,"\xc2\x97"))) {
	      // A stupid utf8 sequence which can't apparently be rendered by
	      // Vera.. it's simply a kind of '-', but on 2 bytes.
	      // I hate utf8 !!!
	      *q++ = '-';
	      memmove(q,q+1,strlen(q+1)+1);
	  }
	  hist_add(str);
      }
  } // while ...
  if (f) fclose(f);
  if (!strcmp(name,"history.dat")) {
      if (current_game) {
	  if (history)
	      free(history);
	  history = commands;
	  size = 1024;
	  used = 0;
	  commands = malloc(size);
	  *commands = 0;

	  hist_add("\nGame: %s",current_game->long_name);
	  if (current_game->long_name_jpn)
	      hist_add("Japenese name (requires a ttf font with jap chars):\n%s",current_game->long_name_jpn);
	  hist_add("Company: %s", game_company_name(current_game->company_id));

	  hist_add("Year: %4d", current_game->year);

	  /*

	     cpu %s %d

*/

	  hist_add("CPU:");
	  if (StarScreamEngine)
	      hist_add("   M68000 x %d",StarScreamEngine);
	  if (MZ80Engine)
	      hist_add("   Z80 x %d",MZ80Engine);
#ifdef HAVE_6502
	  if (M6502Engine)
	      hist_add("   M6502 x %d",M6502Engine);
#endif
#ifndef NO020
	  if(MC68020)
	      hist_add("   M68020 x 1");
#endif
	  if (sh2Engine)
	      hist_add("  SH2 x %d",sh2Engine);

	  if (current_game->sound) {
	      hist_add("\nAudio:");
	      int j;
	      for( j = 0; j < SndMachine->control_max; j++ )
		  hist_add("   %s",get_sound_chip_name( SndMachine->init[j] ));
	  }

	  /*

	     video %d x %d

*/

	  const VIDEO_INFO *vid_info = current_game->video;

	  hist_add("\nVideo: %d x %d", vid_info->screen_x, vid_info->screen_y);

	  if(vid_info->flags & VIDEO_ROTATABLE){

	      switch(VIDEO_ROTATE( vid_info->flags )){
	      case VIDEO_ROTATE_NORMAL:
		  hist_add(" horizontal");
		  break;
	      case VIDEO_ROTATE_90:
		  hist_add(" vertical");
		  break;
	      case VIDEO_ROTATE_180:
		  hist_add(" horizontal");
		  break;
	      case VIDEO_ROTATE_270:
		  hist_add(" vertical");
		  break;
	      }

	  }

	  /*

	     palette %s

*/

	  if(current_colour_mapper)
	      hist_add("\nPalette: %s", current_colour_mapper->name);

	  /*

	     tiles %s

*/

	  if(tile_list_count){
	      hist_add("\nTiles:");
	      int i;
	      for( i = 0; i < tile_list_count; i ++){
		  hist_add("   %-14s %6d tiles", tile_type[tile_list[i].type], tile_list[i].count);
	      }
	  }

	  /*

	     archive %s

*/

	  hist_add("\nArchive:");
	  const DIR_INFO *dir_list = current_game->dir_list;
	  int romof = 0;
	  if (dir_list) {
	      while(dir_list->maindir){
		  if((!IS_ROMOF(dir_list->maindir)) && (!IS_CLONEOF(dir_list->maindir)))
		      hist_add("   %s", dir_list->maindir);
		  dir_list++;
	      }

	      /*

		 romof %s [optional]

*/


	      find_romof(current_game->dir_list, &romof);

	      if (romof) { // Shows only the 1st romof...
		  hist_add("Romof:");
		  hist_add("   %s", romof_list[0] );
	      }

	      /*

		 cloneof %s [optional]

*/

	      int cloneof = 0;

	      dir_list = current_game->dir_list;
	      while(dir_list->maindir){
		  if(IS_CLONEOF(dir_list->maindir)){
		      if(!cloneof)
			  hist_add("Cloneof:");
		      hist_add("   %s", (dir_list->maindir) + 1 );
		      cloneof ++;
		  }
		  dir_list ++;
	      }
	  }

	  hist_add("\nRoms:");

	  const ROM_INFO *rom_list = current_game->rom_list;
	  while(rom_list && rom_list->name){
	      if (strcmp(rom_list->name,REGION_EMPTY) && *rom_list->name) {
		  hist_add("   %-12s %7d bytes", rom_list->name, rom_list->size);
	      }
	      rom_list++;
	  }

	  if (load_region[REGION_CPU1]) { // Check the parent if we have defined regions
	      int region;
	      for (region=1; region < REGION_MAX; region++) {
		  if (load_region[region]) {
		      int found = 0,i;
		      rom_list = current_game->rom_list;

		      while(rom_list->name && !found){
			  if (rom_list->region == region) {
			      found = 1;
			  }
			  rom_list++;
		      }

		      for (i=0; i<romof; i++) {
			  if (!found) {
			      GAME_MAIN *game_romof = find_game(romof_list[i]);
			      rom_list = game_romof->rom_list;
			      while(rom_list->name){
				  if (rom_list->region == region) {
				      hist_add("   %-12s %7d bytes", rom_list->name, rom_list->size);
				      found = 1;
				  }
				  rom_list++;
			      }
			  }
		      }
		  }
	      }
	  }

	  hist_add("");
	  hist_add("Driver: %s",current_game->source_file);

	  if (driver_info)
	      free(driver_info);
	  driver_info = commands;
      }


  } else {
      // commands.dat, throw the buffer away, keep only the menus inside
      if (commands)
	  free(commands);
  }
}

void done_commands() {
    int n;
    for (n=0; n<nb_commands; n++) {
	free(commands_buff[n]);
#ifdef SDL
	free((void*)menu_commands[n].label);
#endif
    }
    if (commands_buff) {
	free(commands_buff);
	commands_buff = NULL;
    }
#ifdef SDL
    if (menu_commands)
	free(menu_commands);
    menu_commands = NULL;
#endif
    nb_commands = 0;
}

