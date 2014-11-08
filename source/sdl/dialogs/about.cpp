#include "profile.h"
#include "../gui/menu.h"
#include "about.h"
#include <SDL_image.h>
#include "raine.h"
#include "version.h"
#include "files.h" // get_shared
#include "history.h"
#include "sdl/dialogs/messagebox.h"
#include "gui/tfont.h"
#include "control.h"
#include "sdl/control_internal.h"
#include <SDL_sound.h>
#include "sdl/SDL_gfx/SDL_gfxPrimitives.h"
#include "games.h"

class TAbout_menu : public TBitmap_menu
{
  private:
    UINT8 *CYC;
    int palette_size;

  public:
  TAbout_menu(char *mytitle, menu_item_t *myitem, char *path);
  ~TAbout_menu();
  virtual void update_fg_layer(int nb_to_update);
};

// A special TStatic class to be able to display these special sequences for
// commands...
class TMoveStatic : public TStatic
{
    public:
    TMoveStatic(menu_item_t *my_menu) : TStatic(my_menu) {}
    virtual void disp(SDL_Surface *s, TFont *font, int x, int y, int w, int h,
  int fg, int bg, int xoptions);
};

static int filled_poly;
static void polytab(SDL_Surface *sf,int ox,int oy,int w,int h,int mw,int mh,
	int nb,Sint16 *tabx,Sint16 *taby, int col)
{
    int n;
    for (n=0; n<nb; n++) {
	tabx[n] = tabx[n]*w/mw+ox;
	taby[n] = taby[n]*h/mh+oy;
    }
    if (filled_poly)
	filledPolygonColor(sf,tabx,taby,nb,col);
    else
	polygonColor(sf,tabx,taby,nb,col);
}

static void poly(SDL_Surface *sf,int ox,int oy,int w,int h, int mw,int mh,
	int col, ...)
{
    va_list ap;
#define PMAX 16
    Sint16 tabx[PMAX],taby[PMAX];
    va_start(ap,col);
    int x,y;
    int nb = 0;
    do {
	x = va_arg(ap,int);
	y = va_arg(ap,int);
	if (x >= 0) {
	    if (nb == PMAX) {
		printf("poly overflow !\n");
		exit(1);
	    }
	    tabx[nb] = x;
	    taby[nb++] = y;
	}
    } while (x >= 0);
    va_end(ap);
    polytab(sf,ox,oy,w,h,mw,mh,nb,tabx,taby,col);
}

static void mirror(int nb, Sint16 *tab, Sint16 *res)
{
    int min = 1000, max = 0;
    int n;
    for (n=0; n<nb; n++) {
	if (tab[n] < min) min = tab[n];
	if (tab[n] > max) max = tab[n];
    }
    for (n=0; n<nb; n++)
	res[n] = (max+min)-tab[n];
}

void TMoveStatic::disp(SDL_Surface *sf, TFont *font, int x, int y, int w, int h,
  int myfg, int mybg, int xoptions) {
    int fg = myfg, bg = mybg;
    char *s = (char*)menu->label;
    char *old = s;
    int old_min = min_font_size;
    min_font_size = 1;
    font->set_utf(is_utf);
    // All the translations are taken from http://home.comcast.net/~plotor/command.html
    while (*s) {
	if (*s != '_' && *s != '^' && *s != '@') {
	    s++;
	    continue;
	}
	char pre = *s;
	if (s > old) {
	    // Eventually display what's before
	    *s = 0;
	    int w,h;
	    font->dimensions(old,&w,&h);
	    font->surf_string(sf,x,y,old,fg,bg);
	    x += w;
	    *s = pre;
	}
	s++;
	font->dimensions("_A",&w,&h);
	int r;
	if (w < h) r = w; else r = h;
	r/=2;
	int col = 0;
	char str[4];
	TFont *f0 = NULL;
	str[0] = *s;
	str[1] = 0;
	if (pre == '_') {
	    switch(*s) {
	    case 'A':
	    case 'a':
	    case 'S':
	    case '5':	col = mymakecol(255,64,64); break;
	    case 'B':
	    case 'b':	col = mymakecol(255,238,0); break;
	    case 'C':
	    case 'c':	col = mymakecol(0,255,64); break;
	    case 'D':
	    case 'd':	col = mymakecol(0,170,255); break;
	    case 'P':
	    case 'e':	col = mymakecol(255,0,170); break;
	    case 'K':
	    case 'Z':
	    case 'f':	col = mymakecol(170,0,255); break;
	    case 'g': col = mymakecol(0,255,204); break;
	    case 'i': col = mymakecol(255,160,0); break;
	    case 'G': col = mymakecol(0,170,255); break;
	    case 'H':
	    case 'h': col = mymakecol(255,0,255); break;
	    case 'j': col = mymakecol(190,190,190); break;
	    }
	    if (*s >= 'a' && *s <= 'j')
		sprintf(str,"%d",*s-'a'+1);
	    else if (*s == 'L')
		sprintf(str,">>"); // too many drawings !!!
	    else if (*s == 'M')
		sprintf(str,"<<");
	    else if (*s == 'X')
		sprintf(str,"TAP");
	    else if (*s == '^')
		sprintf(str,"AIR");
	    else if (*s == '?')
		sprintf(str,"DIR");
	    else if (*s == 'S')
		sprintf(str,"St"); // tss...
	    else if (*s == '.')
		sprintf(str,"...");

	    filled_poly = 1;

	} else if (pre == '^') {
	    filled_poly = 0;
	    switch (*s) {
	    case 'S':
	    case 'E': col = mymakecol(255,238,0); break;
	    case 's':
	    case 'F': col = mymakecol(255,160,0); break;
	    case 'G':	col = mymakecol(255,64,64); break;
	    case 'H': col = mymakecol(190,190,190); break;
	    case 'I': col = mymakecol(0,255,204); break;
	    case 'J':	col = mymakecol(0,170,255); break;
	    case 'T':	col = mymakecol(170,0,255); break;
	    case 'W':
	    case 'U':	col = mymakecol(255,0,170); break;
	    case 'V':	col = mymakecol(170,0,255); break;
	    }
	    if (*s >= 'E' && *s <= 'J') {
		if (!strncmp(current_game->main_name,"sf",2) ||
			!strncmp(current_game->main_name,"msh",3)) {
		    // Street fighter games
		    char *keys[] = { "lp","mp","sp","lk","mk","sk" };
		    sprintf(str,"%s",keys[*s-'E']);
		} else
		    sprintf(str,"b%d",*s-'E'+1); // button n for other games
	    } else if (*s == 'T')
		sprintf(str,"3K");
	    else if (*s == 'U')
		sprintf(str,"3P");
	    else if (*s == 'V')
		sprintf(str,"2K");
	    else if (*s == 'W')
		sprintf(str,"2P");
	    else if (*s == 'S')
		sprintf(str,"SE"); // ?!
	    else if (*s == 'M')
		sprintf(str,"MAX");
	} else if (pre == '@') {
	    if (!strncmp(s,"W-button",8)) {
		sprintf(str,"W");
		col = mymakecol(255,238,0);
		s += 7;
	    }
	}

	if (col)
	    filledCircleColor(sf, x+w/2, y+h/2, r, col);

	if (strlen(str) > 2) {
	    // Try to find a font size which fits in this space !
	    f0 = font;
	    int h = f0->get_font_height()/2;
	    do {
		font = new TFont_ttf(h,"VeraMono.ttf");
		if (h <= 3) break;
		int w0,h0;
		font->dimensions(str,&w0,&h0);
		if (w0 > w) {
		    h--;
		    delete font;
		    font = NULL;
		}
	    } while (!font);
	}
	// The coordinates below are supposed to be on & 10x9 matrix, except
	// that the picture I am using has clearly been resized and so it's
	// only an approximation...

	// For the arrows they are rotated and mirrored, so I do the rotation/
	// mirror instead of risking more errors with more coordinates...
	Sint16 kx[13] = {1,3,6,4,4,7,7,9,9,7,4,2,2};
	Sint16 ky[13] = {3,1,3,3,5,5,3,3,5,7,7,5,3};
	Sint16 mkx[13],mky[13];
	mirror(13,kx,mkx);
	mirror(13,ky,mky);

	Sint16 ox[10] = {1,3,3,5,5,7,7,5,3,3};
	Sint16 oy[10] = {6,4,5,5,1,1,5,7,7,8};
	Sint16 mox[10],moy[10];
	mirror(10,ox,mox);
	mirror(10,oy,moy);

	if (pre == '@') {
	    // Very special case, W Button, 1 letter.
	    font->surf_string(sf,x+w/4,y,str,(col ? 0 : fg),bg);
	    goto end_loop;
	}

	if (*s == '1')
	    poly(sf,x,y,w,h,10,9,mymakecol(255,255,255),
		    6,1,
		    2,6,
		    1,5,
		    1,8,
		    4,8,
		    3,7,
		    8,2,
		    -1,-1);
	else if (*s == '2')
	    poly(sf,x,y,w,h,10,9,mymakecol(255,255,255),
		    2,1,
		    2,6,
		    1,6,
		    3,8,
		    5,6,
		    4,6,
		    4,1,
		    -1,-1);
	else if (*s == '3')
	    poly(sf,x,y,w,h,10,9,mymakecol(255,255,255),
		    0,2,
		    5,8,
		    4,9,
		    8,9,
		    8,5,
		    7,6,
		    3,1,
		    -1,-1);
	else if (*s == '4')
	    poly(sf,x,y,w,h,10,9,mymakecol(255,255,255),
		    3,2,
		    0,5,
		    3,7,
		    3,6,
		    9,6,
		    9,3,
		    3,3,
		    -1,-1);
	else if (*s == '4')
	    poly(sf,x,y,w,h,10,9,mymakecol(255,255,255),
		    3,2,
		    0,5,
		    3,7,
		    3,6,
		    9,6,
		    9,3,
		    3,3,
		    -1,-1);
	else if (*s == '6')
	    poly(sf,x,y,w,h,10,9,mymakecol(255,255,255),
		    1,3,
		    6,3,
		    6,2,
		    9,5,
		    6,7,
		    6,6,
		    1,6,
		    -1,-1);
	else if (*s == '7')
	    poly(sf,x,y,w,h,10,9,mymakecol(255,255,255),
		    1,1,
		    1,5,
		    2,4,
		    8,9,
		    9,7,
		    4,2,
		    5,1,
		    -1,-1);
	else if (*s == '8')
	    poly(sf,x,y,w,h,10,9,mymakecol(255,255,255),
		    2,8,
		    2,3,
		    1,3,
		    3,1,
		    5,3,
		    4,3,
		    4,8,
		    -1,-1);
	else if (*s == '9')
	    poly(sf,x,y,w,h,10,9,mymakecol(255,255,255),
		    8,1,
		    8,5,
		    7,4,
		    1,9,
		    0,7,
		    5,2,
		    4,1,
		    -1,-1);
	else if (*s == 'k')
	    polytab(sf,x,y,w,h,10,9,13,kx,ky,mymakecol(255,255,255));
	else if (*s == 'm') // horizontal mirror of k (10-x)
	    polytab(sf,x,y,w,h,10,9,13,mkx,ky,mymakecol(255,255,255));
	else if (*s == 'l') // vertical mirror of m (8-y)
	    polytab(sf,x,y,w,h,10,9,13,mkx,mky,mymakecol(255,255,255));
	else if (*s == 'n') // horizontal mirror of l (10-x)
	    polytab(sf,x,y,w,h,10,9,13,kx,mky,mymakecol(255,255,255));
	else if (*s == 'o')
	    polytab(sf,x,y,w,h,10,9,10,ox,oy,mymakecol(255,255,255));
	else if (*s == 'p')
	    polytab(sf,x,y,w,h,10,9,10,moy,ox,mymakecol(255,255,255));
	else if (*s == 'q')
	    polytab(sf,x,y,w,h,10,9,10,mox,moy,mymakecol(255,255,255));
	else if (*s == 'r')
	    polytab(sf,x,y,w,h,10,9,10,oy,mox,mymakecol(255,255,255));
	else if (*s == 's')
	    polytab(sf,x,y,w,h,10,9,10,mox,oy,mymakecol(255,255,255));
	else if (*s == 't')
	    polytab(sf,x,y,w,h,10,9,10,oy,ox,mymakecol(255,255,255));
	else if (*s == 'u')
	    polytab(sf,x,y,w,h,10,9,10,ox,moy,mymakecol(255,255,255));
	else if (*s == 'v')
	    polytab(sf,x,y,w,h,10,9,10,moy,mox,mymakecol(255,255,255));
	else if (*s == 'w')
	    poly(sf,x,y,w,h,10,9,mymakecol(255,255,255),
		    3,8,
		    1,6,
		    1,3,
		    3,1,
		    6,1,
		    8,3,
		    8,5,
		    10,5,
		    7,7,
		    5,5,
		    7,5,
		    5,3,
		    3,3,
		    3,6,
		    5,7,
		    4,8,
		    -1,-1);
	else if (str[1] == 0)
	    font->surf_string(sf,x+w/4,y,str,(col ? 0 : fg),bg);
	else
	    font->surf_string(sf,x,y,str,(col ? 0 : fg),bg);
	if (f0) {
	    delete font;
	    font = f0;
	}
end_loop:
	s++;
	old = s;
	x += w;
    }

    if (*old)
	font->surf_string(sf,x,y,old,fg,bg);
    min_font_size = old_min;
}

class TMoves_menu : public TMenu
{
    public:
	TMoves_menu(char *mytitle, menu_item_t *myitem) : TMenu(mytitle,myitem)
	{
	    font_name = "VeraMono.ttf";
	}
	void create_child(int n) {
	    if (!menu[n].value_int)
		child[n] = new TMoveStatic(&menu[n]);
	    else
		TMenu::create_child(n);
	}
};

TAbout_menu::TAbout_menu(char *mytitle, menu_item_t *myitem, char *path) :
  TBitmap_menu(mytitle,myitem,path) {

  /* This cycling color is inspired by the picture given by NoLogic on the
   * rainemu forum. The idea is to allow a cycling of color on any paletized
   * picture, but with a minimum of restrictions : color 0 is *always*
   * transparent, and the unused colors must either be set to black or not
   * appear on the picture */

#define NB_STEPS (palette_size <= 64 ? 128/palette_size : 2) // Number of steps from 1 color to another

   if (bmp == NULL || bmp->format->palette == NULL) {
     CYC = NULL;
     MessageBox("Error","No raine logo, or it's a true color bitmap");
   } else {
     palette_size = bmp->format->palette->ncolors;
     SDL_Color *colors = bmp->format->palette->colors;
     int n;
     for (n=1; n<palette_size; n++) {
       if (colors[n].r == 0 && colors[n].g == 0 && colors[n].b == 0)
	 break;
     }
     palette_size = n;
     // printf("preparation on %d colors, total size %d colorkey %d\n",palette_size,4*palette_size*NB_STEPS*palette_size,bmp->format->colorkey);
     if (bmp->format->colorkey != 0) {
       MessageBox("Erreur","raine_logo.png must have the 1st color of the palette\n(color 0) as transparent");
       CYC = NULL;
     } else {
       CYC = (UINT8 *)malloc(4*palette_size*NB_STEPS*(palette_size-1));

       int start;
       for (int n=0; n<palette_size-1; n++) {
	 for (int step=0; step<NB_STEPS; step++) {
	   start = step*palette_size*4+(n*NB_STEPS*palette_size*4);
	   int i;
	   for (i=1; i<palette_size-1; i++) {
	     CYC[start+i*4+0] = colors[i].r*((NB_STEPS-1)-step)/(NB_STEPS-1)+
	       colors[(i+1)].r*step/(NB_STEPS-1);
	     CYC[start+i*4+1] = colors[i].g*((NB_STEPS-1)-step)/(NB_STEPS-1)+
	       colors[(i+1)].g*step/(NB_STEPS-1);
	     CYC[start+i*4+2] = colors[i].b*((NB_STEPS-1)-step)/(NB_STEPS-1)+
	       colors[(i+1)].b*step/(NB_STEPS-1);
	   }
	   // last color of the palette takes color 1, not color 0
	   CYC[start+i*4+0] = colors[i].r*((NB_STEPS-1)-step)/(NB_STEPS-1)+
	     colors[(+1)].r*step/(NB_STEPS-1);
	   CYC[start+i*4+1] = colors[i].g*((NB_STEPS-1)-step)/(NB_STEPS-1)+
	     colors[(+1)].g*step/(NB_STEPS-1);
	   CYC[start+i*4+2] = colors[i].b*((NB_STEPS-1)-step)/(NB_STEPS-1)+
	     colors[(+1)].b*step/(NB_STEPS-1);
	 }
	 memcpy(colors,&CYC[start],palette_size*4);
       }
     }
   }
}

TAbout_menu::~TAbout_menu() {
  if (CYC) free(CYC);
}

void TAbout_menu::update_fg_layer(int nb_to_update) {
  static unsigned int tt = 0;
  if (nb_to_update >= 0 && bmp && CYC) {
     int indice = (tt*palette_size*4);
     // printf("indice %d\n",indice);
     if (indice >= (4*palette_size*NB_STEPS*(palette_size-1))) {
       indice = 0;
       tt = 0;
     }
     SDL_SetColors(bmp,(SDL_Color*)&CYC[indice],0,palette_size);
     tt++;
    SDL_Rect dest;

    dest.x = fgdst.x+(fg_layer->w-bmp->w)/2;
    dest.y = fgdst.y+HMARGIN;
    dest.w = bmp->w;
    dest.h = bmp->h;
    SDL_BlitSurface(bmp,NULL,sdl_screen,&dest);
    do_update(&dest);
  }
  TBitmap_menu::update_fg_layer(nb_to_update);
}

static menu_item_t *menu;

static void goto_url(char *url) {
#ifdef RAINE_UNIX
    // at least when using compiz, you can't switch to any other window while
    // in fullscreen mode, so it's better to leave it first !
    if (display_cfg.fullscreen)
	toggle_fullscreen();
#endif
    char cmd[2048];
#ifdef RAINE_UNIX
    // Now use xdg-open instead of www-browser
    // normally xdg-open knows about urls and lanches the best application...
    sprintf(cmd,"xdg-open \"%s\" &",url);
#else
    // windows
    sprintf(cmd,"explorer \"%s\"",url);
#endif
    system(cmd);
}

static int menu_goto_url(int sel) {
    if (menu[sel].values_list_label[0])
	goto_url(menu[sel].values_list_label[0]);
    return 0;
}

static int do_command = 0;

static int about_game(int sel) {
  int nb_lines = 10;
  menu = (menu_item_t *)malloc(sizeof(menu_item_t)*nb_lines);
  int used = 0;
  unsigned int maxlen = sdl_screen->w/(min_font_size); // rough approximation
  char *buff;
  if (do_command)
      buff = commands_buff[sel];
  else
      buff = history;

  if (buff) {
    char *s = buff;
    char *nl;
    char previous;
    while ((nl = strchr(s,'\n'))) {
      char *q;
      previous = *nl;
      *nl = 0;
      if ((q=strstr(s,"<a href="))) {
	  /* Basic parsing for urls. We assume everything is lowercase with
	   * only 1 space and not more, and everything on 1 line of text.
	   * I don't was a general html parser here, just something to parse
	   * basic urls */
	  q+=8;
	  char *end;
	  char old;
	  if (*q == '"') {
	      q++;
	      end = strchr(q,'"');
	  } else
	      end = strchr(q,' ');
	  if (end) {
	      old = *end;
	      *end = 0;
	      menu[used].values_list_label[0] = strdup(q);
	      *end = old;
	      q = end+1;
	  } else
	      menu[used].values_list_label[0] = NULL;
	  q = strchr(q,'>');
	  if (q) {
	      q++;
	      end = strstr(q,"</a>");
	      if (end) {
		  old = *end;
		  *end = 0;
		  menu[used].label = strdup(q);
		  *end = old;
		  menu[used].menu_func = &menu_goto_url;
		  goto end_loop;
	      }
	  }
      }
      if (strlen(s) > maxlen) {
	char start;
	if (s > buff) {
	  start = s[-1];
	  s[-1] = 0;
	}
	char old = s[maxlen];
	s[maxlen] = 0;
	char *sp = strrchr(s,' ');
	s[maxlen] = old;
	if (s>buff) {
	  s[-1] = start;
	}
	if (!sp)
	  sp = strchr(s+maxlen,' ');
	*nl = previous;
	if (sp && sp <= s+maxlen)
	  nl = sp;
	else
	  nl = s+maxlen;

	previous = *nl;
	*nl = 0;
      }
      while ((q = strchr(s,0x92)))
	*q = 0x27; // fix the stupid non standard ' code from krosoft

      menu[used].label = strdup(s);
      menu[used].menu_func = NULL;
      menu[used].values_list_label[0] = NULL;
end_loop:
      menu[used].value_int = NULL;
      used++;
      if (used == nb_lines) {
	nb_lines += 10;
	menu = (menu_item_t *)realloc(menu,sizeof(menu_item_t)*nb_lines);
      }
      *nl = previous;
      if (previous == ' ' || previous == '\n')
	s = nl+1;
      else
	s = nl;
    }
  } else {
      char *hist = get_shared("history.dat");
      if (!strcmp(hist,"history.dat")) { // No path info added -> no file !
	  int ret = MessageBox("Warning","history.dat not found\nDownload it from http://www.arcade-history.com/index.php?page=download\nand install it in your raine directory\n"
#ifdef RAINE_UNIX
		  "in linux ~/.raine or /usr/share/games/raine\n"
#endif
		  ,"Open this page now !|Later maybe...");
	  if (ret == 1)
	      goto_url("http://www.arcade-history.com/index.php?page=download");
      }
  }

  if (used) {
    menu[used].label = NULL;

    TMenu *about_menu;
    if (!do_command)
	about_menu = new TMenu("About this game",menu);
    else
	about_menu = new TMoves_menu((char*)menu_commands[sel].label,menu);
    about_menu->execute();
    delete about_menu;

    for (int n=0; n<used; n++) {
      free((void*)menu[n].label);
      if (menu[n].values_list_label[0])
	  free(menu[n].values_list_label[0]);
    }
  }
  free(menu);

  return 0;
}

int show_moves(int sel) {
    do_command = 1;
    int n;
    for (n=0; n<nb_commands; n++)
	menu_commands[n].menu_func = &about_game;
    TMenu *menu = new TMenu("Special moves...",menu_commands);
    menu->execute();
    delete menu;
    do_command = 0;
    return 0;
}

static menu_item_t about_items[] =
{
  { EMUNAME" "VERSION" (c)1998-2014 " HANDLE, NULL, NULL },
  { "Compiled on " __DATE__ " (" __TIME__ ")", NULL, NULL },
  { "gcc", NULL, NULL },
  { "cpu", NULL, NULL },
  { "SDL", },
  { "sound" },
  { " ", NULL, NULL },
  { "http://rainemu.swishparty.co.uk/", NULL, NULL },
  { " ", NULL, NULL, },
  { "CPU emulators:", NULL, NULL },
  {    "Starscream 0.26r4 by Neill Corlett", },
  {    "MZ80 3.4raine2 by Neill Bradley" },
  {    "M6502 1.6raine2 by Neill Bradley" },
  {    "UAE 68020 Emulator : old hacked asm version from UAE" },
  {    "MCU 68705: statically recompiled code by Richard Mitton" },
  { "About this game...", &about_game },
  { NULL, NULL, NULL },
};

static TAbout_menu *about_menu;
static char gcc_version[30];
static char about_cpu[64];

int do_about(int sel) {
    char *dir = get_shared("bitmaps");
    char path[256];
#ifdef __GNUC__
    {
	int major = __GNUC__;
	int minor = __GNUC_MINOR__;
#ifdef __GNUC_PATCHLEVEL__
	int patchlevel = __GNUC_PATCHLEVEL__;
#else
	int patchlevel = 0;
#endif
	sprintf(gcc_version,"with gcc %d.%d.%d",major,minor,patchlevel);
    }
#else
    sprintf(gcc_version,"with an unknown gcc ???");
#endif
    about_items[2].label = gcc_version;
    sprintf(path,"%s/raine_logo.png",dir);
    about_menu = new TAbout_menu("About...",about_items, path);
    sprintf(about_cpu, "CPU: %s", raine_cpu_model);
    about_items[3].label = about_cpu;
    char about_sdl[80],about_sound[80];
    const SDL_version *version = SDL_Linked_Version();
    const SDL_version *img = IMG_Linked_Version();
    const SDL_version *ttf = TTF_Linked_Version();
    Sound_Version sound;
    Sound_GetLinkedVersion(&sound);
    sprintf(about_sdl,"Linked with SDL-%d.%d.%d, SDL_image-%d.%d.%d, SDL_ttf-%d.%d.%d",version->major,version->minor,version->patch,
	    img->major,img->minor,img->patch,
	    ttf->major,ttf->minor,ttf->patch);
    sprintf(about_sound,"SDL_sound-%d.%d.%d",
	    sound.major,sound.minor,sound.patch);

    about_items[4].label = about_sdl;
    about_items[5].label = about_sound;
#ifdef RDTSC_PROFILE
  if (cycles_per_second) {
    sprintf(about_cpu,"CPU: %s at %d MHz",raine_cpu_model,cycles_per_second/1000000);
  }
#endif
  about_menu->execute();
  delete about_menu;
  about_menu = NULL;
  return 0;
}
