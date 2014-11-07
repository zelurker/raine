#include "gameinc.h"
#include "files.h"
#ifdef SDL
#include "gui/menuitem.h"
/* Can now read also command.dat which has almost the same format as history.dat
 * in this case this menu_commands is filled with the sub-section names
 * and commands_buff contains the associated text */
menu_item_t *menu_commands;
#endif

int nb_commands;
char **commands_buff;
char *history = NULL;

void hist_open(char *name) {
  FILE *f;
  char str[256];
  int size = 1024;
  int len;
  int used = 0;
  int old_size = 0,old_use;
  char *old_commands;
  char *commands = NULL;
  if (strcmp(name,"history.dat")) { // not history.dat -> free old commands
      int n;
      for (n=0; n<nb_commands; n++)
	  free(commands_buff[n]);
      nb_commands = 0;
  }
  f = fopen (get_shared(name), "r");
  if (!f) return;
  while (!feof(f)) {
      myfgets(str,255,f);
      len = strlen(str)+1;
      // The use of strncmp is to be able to handle crlfs files in unix
      if (!strncmp(str,"$info=",6)) {
	  if (commands) break; // End of current game
	  char *s = strstr(str,current_game->main_name);
	  int len = strlen(current_game->main_name);
	  // $info can contain a list of clones, so search the game name inside
	  if (s && (*(s-1)=='=' || *(s-1)==',' || *(s-1) == ' ') &&
		  (*(s+len)==',' || *(s+len)==' ' || s[len]==0)) { // found it
	      commands = malloc(size);
	      *commands = 0;
	  }
      } else if (!commands)
	  continue;
      // After this line we are parsing data for the current loaded game
      else if (!strncmp(str,"$cmd",4)) { // subsection
	  // Specific to command.dat
	  myfgets(str,255,f);
	  menu_commands = realloc(menu_commands,sizeof(menu_item_t)*(nb_commands+2));
	  commands_buff = realloc(commands_buff,sizeof(char*)*(nb_commands+1));
	  // Also clear last element after this one
	  memset(&menu_commands[nb_commands],0,sizeof(menu_item_t)*2);
	  if (str[0] == '[') { // name between useless [] apparently...
	      char *s = &str[strlen(str)-1];
	      *s-- = 0;
	      while (*s == '-') // and a line of - !
		  *s-- = 0;
	      menu_commands[nb_commands].label = strdup(str+1);
	  } else {
	      // In case some entry doesn't have these []...
	      char *s = &str[strlen(str)-1];
	      while (*s == '-') // and a line of - !
		  *s-- = 0;
	      menu_commands[nb_commands].label = strdup(str);
	  }
	  if (old_size) {
	      printf("history: subsection inside subsection !\n");
	      exit(1);
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
	  if (used + len >= size) {
	      size += 1024;
	      commands = realloc(commands,size);
	      if (!commands)
		  break;
	  }
	  sprintf(&commands[used],"%s\n",str);
	  used += len;
      }
  } // while ...
  fclose(f);
  if (!strcmp(name,"history.dat")) {
      if (history)
	  free(history);
      history = commands;
  } else {
      // commands.dat, throw the buffer away, keep only the menus inside
      if (commands)
	  free(commands);
  }
}
