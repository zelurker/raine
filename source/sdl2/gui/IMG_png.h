#ifndef IMG_PNG_H
#define IMG_PNG_H

#ifdef __cplusplus
extern "C" {
#endif

int raine_InitPNG();
void raine_QuitPNG();
SDL_Surface *raine_LoadPNG_RW(SDL_RWops *src);

#ifdef __cplusplus
}
#endif

#endif
