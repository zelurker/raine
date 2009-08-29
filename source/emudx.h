#ifndef __EMUDX_H
#define __EMUDX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "unzip.h"

typedef unzFile emudx_file;
char *exists_emudx_file(const char *db_filename);
emudx_file open_emudx_file(const char *name);

void close_emudx(emudx_file dat);
BITMAP *emudx_bitmap(emudx_file dat, int nb);

void load_emudx(const char *name, int tiles, int sprites,
		int start_sprites32, int sprites32, int first_char, int last_char,
		int width, int height,
		int transp,void (*draw_emudx)());
void free_emudx();

extern BITMAP **emudx_tiles;
extern UINT8 *emudx_sprites,*emudx_sprites32,*emudx_chars;
extern UINT32 emudx_nb_tiles,emudx_transp;

#define set_emudx_password() packfile_password("pacsass")

#ifdef SDL
extern void load_message(char *);
#include "sdl/SDL_gfx/SDL_rotozoom.h"

#define load_explicit_progress(a,b) load_progress("emudx",(a)*100/(b))

#define stretch_blit(src,dst,x1,y1,w1,h1,x2,y2,w2,h2) \
{  SDL_Surface *mysrc; \
   if ((x1) || (y1)) { \
     SDL_Rect r; \
     int myw = (w1), myh = (h1); \
     r.x = x1; r.y = y1; r.w = myw; r.h = myh; \
     SDL_PixelFormat *fmt = get_surface_from_bmp(src)->format; \
     mysrc = SDL_CreateRGBSurface(SDL_SWSURFACE,myw,myh, \
       fmt->BitsPerPixel,fmt->Rmask,fmt->Gmask,fmt->Bmask,fmt->Amask); \
     SDL_BlitSurface(get_surface_from_bmp(src),&r,mysrc,NULL); \
   } else \
     mysrc = get_surface_from_bmp(src); \
   if (mysrc->w == w2 && mysrc->h == h2) \
     SDL_BlitSurface(mysrc,NULL,get_surface_from_bmp(dst),NULL); \
   else { \
     SDL_Surface *scaled = rotozoomSurfaceXY(mysrc, 0.0, \
      (w2)*1.0/(w1),(h2)*1.0/(h1),0); \
     SDL_BlitSurface(scaled, NULL, get_surface_from_bmp(dst), NULL); \
     SDL_FreeSurface(scaled); \
   } \
   if ((x1) || (y1)) SDL_FreeSurface(mysrc); \
}


#define blit(src,dst,x1,y1,x2,y2,w1,h1) \
{ \
  SDL_Rect r1,r2; \
  r1.x = x1; r1.y = y1; r1.w = w1; r1.h = h1; \
  r2.x = x2; r2.y = y2; r2.w = w1; r2.h = h1; \
  SDL_BlitSurface(get_surface_from_bmp(src),&r1,get_surface_from_bmp(dst),&r2);}

#define rectfill(bmp,x1,y1,x2,y2,coul) \
{ \
  SDL_Rect r; \
  r.x = x1; r.y = y1; r.w = (x2)-(x1); r.h =(y2)-(y1); \
  SDL_FillRect(get_surface_from_bmp(bmp),&r,coul); \
}

#endif

#ifdef __cplusplus
}
#endif

#endif
