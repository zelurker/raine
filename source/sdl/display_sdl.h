// extensions for SDL

#ifndef DISPLAY_SDL_H
#define DISPLAY_SDL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "deftypes.h"

typedef struct {
    int sync;
    int overlay;
    int info;
    char *vendor,*renderer,*version;
    struct {
	int dbuf,fsaa_buffers,fsaa_samples,accel,vbl;
    } infos;
} togl_options;

extern togl_options ogl;

extern int prefered_yuv_format;
#ifdef DARWIN
extern int overlays_workarounds;
#endif
extern UINT32 screen_flags; // flags of the game screen, before gui adjustement
extern const SDL_VideoInfo *video_info;

void resize();
int lock_surface(SDL_Surface *s);
void adjust_gui_resolution();

#ifdef __cplusplus
}
#endif

#endif

