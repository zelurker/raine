#include "widget.h"
#include "SDL_gfx/SDL_gfxPrimitives.h"
#include "compat.h"
#include "gui/tconsole.h" // get ansilen from there...

#if SDL == 2
#define rectangleColor(s,x,y,w,h,col) rectangleColor(rend,x,y,w,h,col)
#define boxColor(s,x,y,w,h,col) boxColor(rend,x,y,w,h,col)
#endif

/* Basic widgets for TMenu, see menu.cpp comments at the top for an
 * introduction to the gui.
 *
 * These basic widgets are :
 * TStatic : a simple string in the menu, eventually linked to a command
 * TOptions : a title associated with a list of values (either strings or
 * numbers, see the definition of menu_item_t in menu.cpp.
 * TProgressBar : a progression bar used in raine to load roms.
 */

static int ansi_color[] =
{
  0,
  0xff0000, // red
  0x00ff00, // green
  0xffff00, // yellow
  0x0000ff, // blue
  0xff00ff, // magenta
  0x00ffff, // cyan
  0xffffff, // white
};

static void process_piece(TFont *font, char *s, int *width, int *height) {
  int w0,h0;
  font->dimensions(s,&w0,&h0);
  *width += w0;
  if (*height < h0) *height = h0;
}

static void ansi_font_dim(TFont *font, char *str, int *width, int *height) {
  char *s = str,*old = str;
  *width = 0; *height = 0;
  while ((s = strstr(old,"\E["))) {
    char oldc = *s;
    *s = 0;
    process_piece(font,old,width,height);
    *s = oldc;
    while (*s != 'm' && *s != 0)
      s++;
    if (*s == 'm') {
      old = s+1;
    } else
      return;
  }
  process_piece(font,old,width,height);
}

// this time really returns if a valid utf8 sequence was found, so if it returns 0
// it's ascii, latin or anything else
// follow the instructions from http://www.fileformat.info/info/unicode/utf8.htm
int test_utf(const unsigned char *s) {
    const unsigned char *e = &s[strlen((const char*)s)];
    while (*s && s<e) {
	if (*s >= 0xc2 && *s <= 0xdf) {
	    if (s[1] >= 0x80 && s[1] <= 0xbf)
		return 1;
	    return 0;
	} else if (*s >= 0xe0 && *s <= 0xef) {
	    if (s[1] >= 0x80 && s[1] <= 0xbf &&
		    s[2] >= 0x80 && s[2] <= 0xbf)
		return 1;
	    return 0;
	} else if (*s >= 0xf0 && *s <= 0xff) {
	    if (s[1] >= 0x80 && s[1] <= 0xbf &&
		    s[2] >= 0x80 && s[2] <= 0xbf &&
		    s[3] >= 0x80 && s[3] <= 0xbf)
		return 1;
	    return 0;
	}
	s++;
    }
    return 0;
}

TStatic::TStatic(menu_item_t *my_menu) {
    menu = my_menu;
}

int TStatic::get_len() {
  return strlen(menu->label);
}

int TStatic::can_be_selected() {
  if (menu->menu_func)
    return 1;
  return 0;
}

int TStatic::can_draw_selection() {
  // Tells if we want to be redraw all the time to update the background
  // when selection is upon us (default = yes always)
  return 1;
}

int TStatic::get_len_max_options() {
  return 0;
}

int TStatic::get_width(TFont *font) {
  int w,h;
  ansi_font_dim(font,(char*)menu->label,&w,&h);
  return w;
}

int TStatic::get_width_max_options(TFont *font) {
    int w,h;
    font->dimensions("W",&w,&h);
  return w*get_len_max_options();
}

int TStatic::get_list_index() {
  return 0;
}

int TStatic::get_height(TFont *font) {
  int w,h;
  ansi_font_dim(font,(char*)menu->label,&w,&h);
  return h;
}

void TStatic::disp(SDL_Surface *sf, TFont *font, int x, int y, int w, int h,
  int myfg, int mybg, int xoptions) {
  int fg = myfg, bg = mybg;
  char *s = (char*)menu->label;
  char *old = s;
  while ((s = strstr(s,"\E["))) {
    if (s > menu->label) {
      *s = 0;
      int w,h;
      font->dimensions(old,&w,&h);
      font->surf_string(sf,x,y,old,fg,bg,w);
      x += w;
      *s = 27;
    }
    int col = 0;
    s+=2;
    do {
      while (*s >= '0' && *s <= '9') {
	col = (col*10) + (*s - '0');
	s++;
      }
      if (*s == 'm' || *s == ';') {
	if (col >= 30 && col <= 37)
	  fg = (ansi_color[col - 30]<<8) | 255;
	else if (col >= 40 && col <= 47)
	  bg = (ansi_color[col - 40]<<8) | 255;
	else if (col == 1)
	    font->set_style(TTF_STYLE_BOLD);
	else if (col == 0) {
	  fg = myfg;
	  bg = mybg;
	  font->set_style(TTF_STYLE_NORMAL);
	} else if (col == 39)
	  fg = myfg;
	else if (col == 49)
	  bg = mybg;
      }
      s++;
      old = s;
    } while (s[-1] != 'm' && s[-1] != 0);
  }

  font->surf_string(sf,x,y,old,fg,bg,w);
}

void TStatic::next_list_item() {}
void TStatic::prev_list_item() {}

void TProgressBar::disp(SDL_Surface *s, TFont *font, int x, int y, int w,int h,
  int fg, int bg, int xoptions) {
  x = (s->w-w)/2;
  rectangleColor(s,x,y,x+w-1,y+h-1,mymakecol(255,255,255));
  int pcent;
  int prev = x+1;
  for (pcent=5; pcent<=*(menu->value_int); pcent += 5) {
    int cur = x+(pcent * (w-2)/100);
    int color = 255*pcent/100;
    boxColor(s,prev,y+1,cur,y+h-2,mymakecol(color,0,0));
    prev = cur;
  }
  char buff[8];
  sprintf(buff,"%d %%",*(menu->value_int));
  int myw,myh;
  font->dimensions(buff,&myw,&myh);
  font->surf_string_tr(s,x+(w-myw)/2,y+1,buff,mymakecol(255,255,255));
}

int TProgressBar::get_width(TFont *font) {
#if SDL==2
    return desktop->w/2;
#endif
  return sdl_screen->w/2;
}

int TProgressBar::get_height(TFont *font) {
  int h = TStatic::get_height(font);
  h+=2;
  return h;
}

int TOptions::get_list_index() {
  // return the index of the selected value in the list
  int index;
  for (index=0; index<menu->values_list_size; index++) {
    if (menu->values_list[index] == *(menu->value_int))
      break;
  }
  return index;
}

int TOptions::get_len_max_options() {
  unsigned int len_max_options = 0;
  if (menu->values_list_label[0] &&
	  !strcmp(menu->values_list_label[0],"hidden"))
      return 0;
  if (menu->value_int && menu->values_list_size) {
      for (int l=0; l<menu->values_list_size; l++) {
	  unsigned int len2;
	  if (!menu->values_list_label[l]) {
	      char s2[20];
	      sprintf(s2,"%d",menu->values_list[l]);
	      len2 = strlen(s2);
	  } else
	      len2 = ansilen(menu->values_list_label[l]);
	  if (len2 > len_max_options) {
	      len_max_options = len2;
	  }
      }
  }
  return len_max_options;
}

int TOptions::get_width_max_options(TFont *font) {
  int w,h,width_max_options = 0;
  if (menu->values_list_label[0] &&
	  !strcmp(menu->values_list_label[0],"hidden"))
      return 0;
  if (menu->value_int && menu->values_list_size) {
      for (int l=0; l<menu->values_list_size; l++) {
	  if (!menu->values_list_label[l]) {
	      char s2[20];
	      sprintf(s2,"%d",menu->values_list[l]);
	      font->dimensions(s2,&w,&h);
	  } else
	      ansi_font_dim(font,menu->values_list_label[l],&w,&h);
	  if (w > width_max_options)
	      width_max_options = w;
      }
  }
  return width_max_options;
}

void TOptions::disp(SDL_Surface *s, TFont *font, int x, int y, int w, int h,
  int fg, int bg, int xoptions) {
  TStatic::disp(s,font,x,y,w,h,fg,bg,xoptions);
  if (menu->values_list_label[0] &&
	  !strcmp(menu->values_list_label[0],"hidden"))
      return;

  int index = 0;
  if (menu->values_list_size != 3 || menu->values_list_label[0]) {
    index = get_list_index();
    if (index >= menu->values_list_size) { // not found !
      // It isn't supposed to happen, but I'd better handle it anyway, just
      // in case
      index = 0;
      *(menu->value_int) = menu->values_list[0];
    }
  }
  char *disp_string,buff[10];
  if (menu->values_list_label[index]) {
    disp_string = menu->values_list_label[index];
  } else {
    // no labels -> display the int value directly
    sprintf(buff,"%d",*(menu->value_int));
    disp_string = buff;
  }
  char *old = menu->label;
  menu->label = disp_string;
  TStatic::disp(s,font,xoptions,y,w,h,fg,bg,xoptions);
  menu->label = old;
  // font->surf_string(s,xoptions,y,disp_string,fg,bg,w);
}

/* About cycling of next_list_item and prev_list_item : this is mandatory with
 * the mouse, or it would be impossible to return to the start of the list
 * (only 1 button to browse since the right button is to exit and the middle
 * one is not convinient at all for this */
void TOptions::next_list_item() {
  if (menu->values_list_size == 3 &&
	  (!menu->values_list_label[0] ||
	   !strcmp(menu->values_list_label[0],"hidden"))) {
    // special interval list
    *(menu->value_int) += menu->values_list[2];
    if (*(menu->value_int) > menu->values_list[1])
      *(menu->value_int) = menu->values_list[0];
  } else {
    int index = get_list_index();
    if (index < menu->values_list_size - 1)
      index++;
    else
      index = 0; // cycling
    *(menu->value_int) = menu->values_list[index];
  }
}

void TOptions::prev_list_item() {
  if (menu->values_list_size == 3 &&
	  (!menu->values_list_label[0] ||
	   !strcmp(menu->values_list_label[0],"hidden"))) {
    // special interval list
    *(menu->value_int) -= menu->values_list[2];
    if (*(menu->value_int) < menu->values_list[0])
      *(menu->value_int) = menu->values_list[1];
  } else {
    int index = get_list_index();
    if (index > 0)
      index--;
    else
      index = menu->values_list_size - 1; // cycling
    *(menu->value_int) = menu->values_list[index];
  }
}

int TOptions::handle_key_buff(char *buff) {
    // If we are on a list of values, then use the keys to browse
    // the list
    int index = strlen(buff);
    int n;
    for (n=0; n<menu->values_list_size; n++) {
	if (menu->values_list_label[n] &&
		!strnicmp(menu->values_list_label[n],buff,index))
	    break;
    }
    if (n<menu->values_list_size) { // found
	*(menu->value_int) = n;
	return 1;
    }
    return 0;
}
