/* Font support : we'll use first the fonts from SDL_gfx
 * they are extremely easy to use : constant char width & height, with
 * a default 8x8 font available if we don't load any font
 * It might be interesting to use ttf fonts one day, so we'll try to keep
 * this as generic as possible */

#include <string.h>
#include <stdio.h>
#include "compat.h"
#include "raine.h"
#include "SDL_gfx/SDL_gfxPrimitives.h"
#include "tfont.h"
#include <dirent.h>
#include "confile.h"
#include "menu.h"
#include "display_sdl.h"
#include "dialogs/fsel.h"

#if SDL==2
#define stringColor(surf,x,y,s,col) stringColor(rend,x,y,s,col)
#define boxColor(surf,x,y,w,h,col) boxColor(rend,x,y,w,h,col)
#define sdl_screen NULL
#endif

int min_font_size = 8;
int max_font_size = 30;

static int do_font(int sel) {
    char res[FILENAME_MAX];
    char *exts[] = { "ttf", NULL };
    char temp[FILENAME_MAX];
    strcpy(temp,jap_font);
    char *p = strrchr(temp,SLASH[0]);
    if (p) {
	*p = 0;
	if (p > temp && p[-1] == SLASH[0]) p[-1] = 0; // for windows and its stupide double \\ !
    }
    strcpy(res,jap_font);
    fsel(temp,exts, res, _("Select true type font"));
    if (res[0] && strcmp(jap_font,res) && !stricmp(&res[strlen(res)-3],"ttf")) {
	if (*jap_font) free(jap_font);
	jap_font = strdup(res);
    }

    return 1;
}

static int bidon;

// These lists of 3 values are ranges : min, max, step
static menu_item_t font_options[] =
{
  { _("Minimum font size"), NULL, &min_font_size, 3, {8, 20, 1} },
  { _("Maximum font size"), NULL, &max_font_size, 3, {20, 40, 1} },
  { _("Japanese font..."), &do_font, &bidon, 1, {0}, { jap_font } },
};

int add_fonts_gui_options(menu_item_t *menu) {
  menu[0] = font_options[0];
  menu[1] = font_options[1];
  font_options[2].values_list_label[0] = jap_font;
  menu[2] = font_options[2];
  return 3;
}

static void get_font_dimensions(char *s, unsigned int *width, unsigned int *height) {
  while ((*s < '0' || *s > '9') && *s)
    s++;
  *width = atoi(s);
  while ((*s >= '0' && *s <= '9')) // find separator
    s++;
  while ((*s < '0' || *s > '9') && *s) // find next number
    s++;
  *height = atoi(s);
}

TFont::TFont(int size, char *font) {
  loaded_font = NULL;
}

TFont::TFont(char *myfont) {
  loaded_font = NULL;
  load_font(myfont);
}

void TFont::select_ideal_font(int ideal_width, int ideal_height) {
  unsigned int bestw = 0, besth=0;
  char *fdir = (*get_shared_hook)("fonts");
  DIR *dir = opendir(fdir);
  loaded_font = NULL;
  if (dir) {
    char selected[256];
    unsigned int w,h;
    struct dirent *entry;
    // 1st look for height
    while ((entry = readdir(dir))) {
      get_font_dimensions(entry->d_name,&w,&h);
      if (abs((int)h-ideal_height) < abs((int)besth - ideal_height))
	besth = h;
    }
    rewinddir(dir);
    // Now look for the best width with this height
    while ((entry = readdir(dir))) {
      get_font_dimensions(entry->d_name,&w,&h);
      if (h == besth && (abs((int)w-ideal_width) < abs((int)bestw-ideal_width))) {
	strcpy(selected, entry->d_name);
	bestw = w;
      }
    }
    closedir(dir);
    TFont::load_font(selected);
  }
}

TFont::TFont(int ideal_width, int ideal_height) {
  select_ideal_font(ideal_width,ideal_height);
}

void TFont::load_font(char *myfont) {
  FILE *f;
  unsigned int len,width,height;
  get_font_dimensions(myfont,&width,&height);
  if (!width || !height) { // load default font
    charWidth = charHeight = 0;
    if (loaded_font) {
      free(loaded_font);
      loaded_font = NULL;
    }
    return;
  }
  if (strchr(myfont,'/') == 0) {
    char tpath[1024];
    sprintf(tpath,"fonts/%s",myfont);
    f = fopen((*get_shared_hook)(tpath),"rb");
  } else
    f = fopen((*get_shared_hook)(myfont),"rb");

  if (f) {
    fseek(f,0L,SEEK_END);
    len = ftell(f);
    fseek(f,0L,SEEK_SET);
    if (loaded_font)
      free(loaded_font);

    loaded_font = (char*)malloc(len);
    if (loaded_font) {
      fread(loaded_font,len,1,f);
      charWidth = width;
      charHeight = height;
      gfxPrimitivesSetFont(loaded_font,charWidth,charHeight);
    } else {
      charWidth = charHeight = 0;
      // in this case CharWidth = CharHeight = 8, see get_font_height...
    }
    fclose(f);
  }
}

TFont::~TFont() {
  if (loaded_font) {
    free(loaded_font);
    loaded_font = NULL;
  }
}

void TFont::dimensions(const char *s, int *w, int *h) {
    if (!*s) {
	*w = 0;
	*h = 0;
	return;
    }
  if (loaded_font) {
    *w = strlen(s)*charWidth;
    *h = charHeight;
  } else {
    *w = strlen(s)*8;
    *h = 8;
  }
}

void TFont::put_string(int x, int y, const char *s, int color) {
  surf_string_tr(sdl_screen,x,y,s,color);
}

void TFont::put_string(int x, int y, const char *s, int color, int bgcolor) {
  surf_string(sdl_screen,x,y,s,color,bgcolor,0);
}

void TFont::surf_string_tr(SDL_Surface *surf,int x, int y, const char *s, int color,int w) {
  if (loaded_font)
    gfxPrimitivesSetFont(loaded_font,charWidth,charHeight);
  if (charWidth*strlen(s) > (unsigned)w && w!=0) {
      int len = w/charWidth;
      char *dup = strdup(s);
      dup[len+1] = 0;
      stringColor(surf,x,y,dup,color);
      free(dup);
  } else
      stringColor(surf,x,y,s,color);
}

void TFont::surf_string(SDL_Surface *surf,int x, int y, const char *s, int color, int bgcolor, int w) {
  if (charWidth*strlen(s) > (unsigned)w && w!=0) {
      int len = w/charWidth;
      char *dup = strdup(s);
      dup[len+1] = 0;
      boxColor(surf,x,y,charWidth*len+x-1,charHeight+y-1,bgcolor);
      surf_string_tr(surf,x,y,dup,color);
      free(dup);
  } else {
      boxColor(surf,x,y,charWidth*strlen(s)+x-1,charHeight+y-1,bgcolor);
      surf_string_tr(surf,x,y,s,color);
  }
}

// SDL_ttf

#include <SDL_ttf.h>

TFont_ttf::TFont_ttf(int size, char *font) :
 TFont::TFont(size,font) {
  if (size < min_font_size) size = min_font_size;
  if (size > max_font_size) size = max_font_size;
  ttf = NULL;
  charWidth = charHeight = size; // for loading...
  load_font(font);
  dimensions("M",&charWidth,&charHeight);
}

TFont_ttf::~TFont_ttf() {
  if (ttf) TTF_CloseFont(ttf);
}

void TFont_ttf::dimensions(const char *s,int *w, int *h) {
    if (!*s) {
	*w = 0;
	*h = 0;
	return;
    }

  if (ttf) {
      int is_utf = test_utf((const unsigned char*)s);
      if (is_utf)
	  TTF_SizeUTF8(ttf,s,w,h);
      else
	  TTF_SizeText(ttf,s,w,h);
  } else {
    TFont::dimensions(s,w,h);
  }
}

void TFont_ttf::put_string(int x, int y, const char *s, int color) {
  surf_string_tr(sdl_screen,x,y,s,color);
}

void TFont_ttf::put_string(int x, int y, const char *s, int color, int bgcolor) {
  surf_string(sdl_screen,x,y,s,color,bgcolor,0);
}

void TFont_ttf::surf_string_tr(SDL_Surface *surf,int x, int y, const char *s, int color,int w) {
    if (!s[0]) return;
  if (!ttf)
    return TFont::surf_string_tr(surf,x,y,s,color,w);
  SDL_Rect dest;
  SDL_Color sc;
#if SDL == 2
  sc.b = (color >> 16) & 0xff;
  sc.g = (color >> 8) & 0xff;
  sc.r = (color >> 0) & 0xff;
  sc.a = 0xff;
#else
  sc.b = (color >> 8) & 0xff;
  sc.g = (color >> 16) & 0xff;
  sc.r = (color >> 24) & 0xff;
#endif
  SDL_Surface *sf;
  int is_utf = test_utf((const unsigned char*)s);
  if (is_utf)
      sf = TTF_RenderUTF8_Solid(ttf,s,sc );
  else
      sf = TTF_RenderText_Solid(ttf,s,sc );
  dest.x = x; dest.y = y;
  dest.w = (w ? MIN(sf->w,w) : sf->w); dest.h = sf->h;

  if (w && w < sf->w) {
      SDL_Rect src;
      src.w = w;
      src.h = sf->h;
      src.x = src.y = 0;
#if SDL==2
      SDL_Texture *tex = SDL_CreateTextureFromSurface(rend,sf);
      SDL_RenderCopy(rend,tex,&src,&dest);
      SDL_DestroyTexture(tex);
#else
      SDL_BlitSurface(sf,&src,surf,&dest);
#endif
  } else {
#if SDL==2
      SDL_Texture *tex = SDL_CreateTextureFromSurface(rend,sf);
      SDL_RenderCopy(rend,tex,NULL,&dest);
      SDL_DestroyTexture(tex);
#else
      SDL_BlitSurface(sf,NULL,surf,&dest);
#endif
  }
  SDL_FreeSurface(sf);
}

void TFont_ttf::surf_string(SDL_Surface *surf,int x, int y, const char *s, int color, int bgcolor, int w) {
    if (!s[0]) return;
  if (!ttf)
    return TFont::surf_string(surf,x,y,s,color,bgcolor,w);
  if (!bgcolor) // 0 is totally transparent in sdl_gfx -> no bg
    return surf_string_tr(surf,x,y,s,color,w);
  SDL_Rect dest;
  SDL_Color sc,bg;
#if SDL == 2
  bg.a = 255;
  sc.a = 255;
  sc.b = (color >> 16) & 0xff;
  sc.g = (color >> 8) & 0xff;
  sc.r = (color >> 0) & 0xff;
  bg.b = (bgcolor >> 16) & 0xff;
  bg.g = (bgcolor >> 8) & 0xff;
  bg.r = (bgcolor >> 0) & 0xff;
#else
  sc.b = (color >> 8) & 0xff;
  sc.g = (color >> 16) & 0xff;
  sc.r = (color >> 24) & 0xff;
  bg.b = (bgcolor >> 8) & 0xff;
  bg.g = (bgcolor >> 16) & 0xff;
  bg.r = (bgcolor >> 24) & 0xff;
#endif
  SDL_Surface *sf;
  int is_utf = test_utf((const unsigned char*)s);
  if (is_utf)
      sf = TTF_RenderUTF8_Shaded(ttf,s,sc,bg);
  else
      sf = TTF_RenderText_Shaded(ttf,s,sc,bg);
  // SDL_SetColorKey(sf,SDL_SRCCOLORKEY | SDL_RLEACCEL,0);
  dest.x = x; dest.y = y;
  dest.w = (w ? MIN(w,sf->w) : sf->w); dest.h = sf->h;
  if (w && w < sf->w) {
      SDL_Rect src;
      src.w = w;
      src.h = sf->h;
      src.x = src.y = 0;
#if SDL==2
      SDL_Texture *tex = SDL_CreateTextureFromSurface(rend,sf);
      SDL_RenderCopy(rend,tex,&src,&dest);
      SDL_DestroyTexture(tex);
#else
      SDL_BlitSurface(sf,&src,surf,&dest);
#endif
  } else {
#if SDL==2
      SDL_Texture *tex = SDL_CreateTextureFromSurface(rend,sf);
      SDL_RenderCopy(rend,tex,NULL,&dest);
      SDL_DestroyTexture(tex);
#else
      SDL_BlitSurface(sf,NULL,surf,&dest);
#endif
  }
  SDL_FreeSurface(sf);
}

void TFont_ttf::load_font(char *myfont) {
  if (!strstr(myfont,"ttf"))
    myfont = "Vera.ttf";
  char tpath[1024];
  if (myfont[0] == '/')
      ttf = TTF_OpenFont(myfont,charHeight*3/2);
  else {
      sprintf(tpath,"fonts/%s",myfont);
      ttf = TTF_OpenFont((*get_shared_hook)(tpath),charHeight*3/2);
  }
  if ( ttf == NULL ) {
    fprintf(stderr, "Couldn't load %d pt font from %s: %s\n",
	charHeight, myfont, SDL_GetError());
    // falling back to bitmap font
    select_ideal_font(charWidth,charHeight);
  } else {
    TTF_SetFontStyle(ttf, TTF_STYLE_NORMAL);
    dimensions("ABCDEF",&charWidth,&charHeight);
    charWidth /= 6;
  }
}

