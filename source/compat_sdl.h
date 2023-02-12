#if !defined(COMPAT_SDL_H) && SDL == 2
#define COMPAT_SDL_H

#include <SDL.h>
#include "compat.h"

#define sdl_create_bitmap(w, h) sdl_create_bitmap_ex(display_cfg.bpp,w,h)
#define SDL_WM_SetCaption(title1,title2) SDL_SetWindowTitle(win,title1)
#define SDL_WM_GrabInput SDL_SetRelativeMouseMode
#define SDL_GRAB_ON SDL_TRUE
#define SDL_GRAB_OFF SDL_FALSE
struct al_bitmap *surface_to_bmp(SDL_Surface *s);

// Misc
static inline SDL_Surface *get_surface_from_bmp(al_bitmap *bmp) {
  return (SDL_Surface *)bmp->extra;
}

extern SDL_PixelFormat *color_format;
extern SDL_Window *win;
#define screen sdl_screen
extern SDL_Surface *sdl_screen;
extern SDL_Renderer *rend;

// lovely callback to avoid the violent name collision between allegro and
// windows about BITMAP. I think BITMAP has probably been removed from allegro
// now, I should check that one day... !
void save_png_surf_rev(char *name, SDL_Surface *s);

typedef SDL_Color PALETTE[256];

extern PALETTE pal;			// 256 Colour palette for the game

#define stretch_blit(src,dst,x1,y1,w1,h1,x2,y2,w2,h2)                  \
{  SDL_Surface *mysrc;                                                 \
   if ((x1) || (y1)) {                                                 \
     SDL_Rect r;                                                       \
     int myw = (w1), myh = (h1);                                       \
     r.x = x1; r.y = y1; r.w = myw; r.h = myh;                         \
     SDL_PixelFormat *fmt = get_surface_from_bmp(src)->format;         \
     mysrc = SDL_CreateRGBSurface(SDL_SWSURFACE,myw,myh,               \
       fmt->BitsPerPixel,fmt->Rmask,fmt->Gmask,fmt->Bmask,fmt->Amask); \
     SDL_BlitSurface(get_surface_from_bmp(src),&r,mysrc,NULL);         \
   } else                                                              \
     mysrc = get_surface_from_bmp(src);                                \
   if (mysrc->w == w2 && mysrc->h == h2)                               \
     SDL_BlitSurface(mysrc,NULL,get_surface_from_bmp(dst),NULL);       \
   else {                                                              \
     SDL_Surface *scaled = rotozoomSurfaceXY(mysrc, 0.0,               \
      (w2)*1.0/(w1),(h2)*1.0/(h1),0);                                  \
     SDL_BlitSurface(scaled, NULL, get_surface_from_bmp(dst), NULL);   \
     SDL_FreeSurface(scaled);                                          \
   }                                                                   \
   if ((x1) || (y1)) SDL_FreeSurface(mysrc);                           \
}



#define blit(src,dst,x1,y1,x2,y2,w1,h1) \
{ \
  SDL_Rect r1 = {x1,y1,w1,h1},r2 = {x2,y2,w1,h1}; \
  SDL_BlitSurface(get_surface_from_bmp(src),&r1,get_surface_from_bmp(dst),&r2);}

#define rectfill(bmp,x1,y1,x2,y2,coul) \
{ \
  SDL_Rect r; \
  r.x = x1; r.y = y1; r.w = (x2)-(x1); r.h =(y2)-(y1); \
  SDL_FillRect(get_surface_from_bmp(bmp),&r,coul); \
}

#endif
