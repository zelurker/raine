#include "../gui/menu.h"
#include "../gui.h"
#include "messagebox.h"

static int retval,start_buttons;

static int button_proc(int sel) {
  retval = sel - start_buttons+1;
  return 1; // exit menu
}

static char *cherche(char *s, int len, char c) {
    // Search a char backwards in a string, from len to 0
    for (int n=len; n>=0; n--) {
	if (s[n] == c)
	    return &s[n];
    }
    return NULL;
}

int MessageBox(char *title, char *the_content, char *mybtn) {
  menu_item_t *menu;
  int nb_lines = 0;
  char *content = strdup(the_content);
  char *s = content, *start;
  int nb_btn = 0;
  char buttons[10240];
  if (mybtn) strncpy(buttons,mybtn,10240);
  buttons[10239] = 0;

  if (mybtn) {
    nb_btn = 1;
    char *s = buttons;
    while ((s = strchr(s,'|'))) {
      s++;
      nb_btn++;
    }
  }

  start = s = content;
  if (!strstr(s,"\n")) {
      // Cut the lines if they are longer than say 80 chars
      char *b;
      while (strlen(s) > 80 && (b = cherche(s,80,' '))) {
	  *b = "\n"[0];
	  s = b+1;
      }
      s = start;
  }
  while ((s = strstr(s,"\n"))) {
    nb_lines++;
    s++;
    start = s;
  }
  if (*start) {
    nb_lines++; // last line not finished by an eol
  }
  menu = (menu_item_t *)malloc(sizeof(menu_item_t)*(nb_btn+nb_lines+1));
  memset(menu,0,sizeof(menu_item_t)*(nb_lines+nb_btn+1));
  nb_lines = 0;
  s = content;
  start = content;
  while ((s = strstr(s,"\n"))) {
    *s = 0;
    menu[nb_lines].label = start;
    nb_lines++;
    s++;
    start = s;
  }
  if (*start) { // last line not finished by an eol
    menu[nb_lines++].label = start;
  }
  if (nb_btn) {
    retval = 0;
    start_buttons = nb_lines;
    s = buttons;
    char *start = s;
    do {
      s = strchr(s,'|');
      if (s) {
	*s = 0;
	s++;
      }
      menu[nb_lines].label = start;
      menu[nb_lines].menu_func = &button_proc;
      nb_lines++;
      start = s;
    } while (s);
  }

  TDialog *mbox = new TDialog(title,menu);
  mbox->execute();
  delete mbox;
  free(menu);
  free(content);
  return retval;
}

