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

static int goto_url(int sel) {
#ifdef RAINE_UNIX
    // at least when using compiz, you can't switch to any other window while
    // in fullscreen mode, so it's better to leave it first !
    if (display_cfg.fullscreen)
	toggle_fullscreen();
#endif
    if (menu[sel].values_list_label[0]) {
	char cmd[2048];
#ifdef RAINE_UNIX
	sprintf(cmd,"www-browser \"%s\" &",menu[sel].values_list_label[0]);
#else
	// windows
	sprintf(cmd,"explorer \"%s\"",menu[sel].values_list_label[0]);
#endif
	system(cmd);
    }
    return 0;
}
  
static int about_game(int sel) {
  int nb_lines = 10;
  menu = (menu_item_t *)malloc(sizeof(menu_item_t)*nb_lines);
  int used = 0;
  unsigned int maxlen = sdl_screen->w/(min_font_size); // rough approximation

  if (history) {
    char *s = history;
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
		  menu[used].menu_func = &goto_url;
		  goto end_loop;
	      }
	  }
      }
      if (strlen(s) > maxlen) {
	char start;
	if (s > history) {
	  start = s[-1];
	  s[-1] = 0;
	}
	char old = s[maxlen];
	s[maxlen] = 0;
	char *sp = strrchr(s,' ');
	s[maxlen] = old;
	if (s>history) {
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
  } 

  if (used) {
    menu[used].label = NULL;

    TMenu *about_menu = new TMenu("About this game",menu);
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

static menu_item_t about_items[] =
{
  { EMUNAME" "VERSION" (c)1998-2010 " HANDLE, NULL, NULL },
  { "Compiled on " __DATE__ " (" __TIME__ ")", NULL, NULL },
  { "gcc", NULL, NULL },
  { "cpu", NULL, NULL },
  { "SDL", },
  { " ", NULL, NULL },
  { "http://rainemu.swishparty.co.uk/", NULL, NULL },
  { " ", NULL, NULL, },
  { "CPU emulators:", NULL, NULL },
  {    "Starscream 0.26r4 by Neill Corlett", },
  {    "MZ80 3.4raine by Neill Bradley" },
  {    "M6502 1.6raine2 by Neill Bradley" },
  {    "UAE 68020 Emulator : old hacked asm version" },
  {    "MCU 68705: statically recompiled code by Richard Mitton" },
  { "About this game...", &about_game },
  { NULL, NULL, NULL },
};

static TAbout_menu *about_menu;
static char gcc_version[30];
static char about_cpu[64];

int do_about(int sel) {
  if (!about_menu) {
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
    static char about_sdl[80];
    const SDL_version *version = SDL_Linked_Version();
    const SDL_version *img = IMG_Linked_Version();
    const SDL_version *ttf = TTF_Linked_Version();
    sprintf(about_sdl,"Linked with SDL-%d.%d.%d, SDL_image-%d.%d.%d, SDL_ttf-%d.%d.%d",version->major,version->minor,version->patch,
      img->major,img->minor,img->patch,
      ttf->major,ttf->minor,ttf->patch);

    about_items[4].label = about_sdl;
  }
#ifdef RDTSC_PROFILE
  if (cycles_per_second) {
    sprintf(about_cpu,"CPU: %s at %d MHz",raine_cpu_model,cycles_per_second/1000000);
  }
#endif
  about_menu->execute();
  return 0;
}
