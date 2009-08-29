// extensions for SDL

#ifndef DISPLAY_SDL_H
#define DISPLAY_SDL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "deftypes.h"

extern int prefered_yuv_format;
extern UINT32 screen_flags; // flags of the game screen, before gui adjustement
extern const SDL_VideoInfo *video_info;

void resize();
int lock_surface(SDL_Surface *s);
void adjust_gui_resolution();

#ifdef __cplusplus
}
#endif

#endif

