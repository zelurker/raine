#include "gameinc.h"

char *history = NULL;

void hist_open() {
  FILE *f;
  char str[256];
  int size = 1024;
  int used = 0;
  if (history) {
    free(history);
    history = NULL;
  }
  f = fopen (get_shared("history.dat"), "r");
  if (f) {
    while (!feof(f)) {
      fgets(str,255,f);
      if (!strncmp(str,"$info=",6)) {
	char *s = strstr(str,current_game->main_name);
	int len = strlen(current_game->main_name);
	if (s && (*(s-1)=='=' || *(s-1)==',' || *(s-1) == ' ') &&
	    (*(s+len)==',' || *(s+len)==' ')) { // found it...
	  history = malloc(size);
	  *history = 0;
	  do {
	    fgets(str,255,f);
	    len = strlen(str)-1;
	    int found_lf = 0;
	    while (str[len]<32 && len>=0) {// crlf ?
	      str[len--] = 0;
	      found_lf = 1;
	    }
	    if (found_lf)
	      len+=2; // include final character
	    else
	      len++;
	    // The use of strncmp is to be able to handle crlfs files in unix
	    if (!strncmp(str,"$end",4) || feof(f))
	      break;
	    if ((str[0]!='$' || strncmp(str,"$bio",4))) {
	      if (used + len >= size) {
		size += 1024;
		history = realloc(history,size);
		if (!history)
		  break;
	      }
	      if (found_lf)
		sprintf(&history[used],"%s\n",str);
	      else
		sprintf(&history[used],"%s",str);
	      used += len;
	    } 
	  } while (1);
	}
      }
    } // while ...
    fclose(f);
  } 
}
