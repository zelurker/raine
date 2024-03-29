#include "raine.h"
#include "alpha.h"
#include "emudx.h"

/* Alpha blending support - mmx */
/* This module just handles global variables required by the mmx functions */

static UINT32 alpha,dalpha;
UINT8 alphatable[256];

/* The CAPITALS for the global variables are kept because they were like that
   in the original code and I'd like to change it as little as possible */

#ifdef DARWIN
#undef RAINE_UNIX
#endif

UINT64 ALPHA64, ALPHABY4, COLORKEY64;

UINT64 MASKRED = 0xF800F800F800F800ll;
UINT64 MASKGREEN = 0x07E007E007E007E0ll;
UINT64 MASKBLUE = 0x001F001F001F001Fll;

UINT64 ADD64 = 0x0040004000400040ll;

void set_alpha(UINT32 my_alpha) {
  alpha = my_alpha;
  dalpha = 255-alpha;
}

/* May Microsoft be cursed for the eternity with its stupid _ everywhere !!! */

void init_alpha(UINT32 my_alpha) {
    set_alpha(my_alpha);

#ifndef NO_ASM
  asm(
      "movd %0,%%mm2    \n" // Copy ALPHA into %mm2
      "punpcklwd %%mm2,%%mm2 \n" // Unpack %mm2 - 0000 0000 00aa 00aa
      "punpckldq %%mm2,%%mm2 \n" // Unpack %mm2 - 00aa 00aa 00aa 00aa
      "movq %%mm2,%3  \n" // Save the result into ALPHA64
      "psrlw  $2,%%mm2      \n" // Divide each ALPHA value by 4
      "movq %%mm2,%4 \n" // Save the result to ALPHABY4

      "movd %2,%%mm4 \n" // Copy ColorKey into %%mm4
      "punpcklwd %%mm4,%%mm4 \n" // Unpack %%mm4 - 0000 0000 cccc cccc
      "punpckldq %%mm4,%%mm4 \n" // Unpack %%mm4 - cccc cccc cccc cccc
      "movq %%mm4,%1 \n" // Save the result into COLORKEY64
      "finit \n" : : "m"(alpha),"m"(COLORKEY64),"m"(emudx_transp),"m"(ALPHA64),"m"(ALPHABY4)
      );
#endif
}

void blend_16(UINT16 *dest, UINT16 src) {
    UINT8 rd,gd,bd,r,g,b;
#ifndef SDL
    rd = getr16(*dest);
    gd = getg16(*dest);
    bd = getb16(*dest);
    r = getr16(src);
    g = getg16(src);
    b = getb16(src);
    *dest = makecol16(
#else
    SDL_GetRGB(*dest,color_format,&rd,&gd,&bd);
    SDL_GetRGB(src,color_format,&r,&g,&b);
    *dest = SDL_MapRGB(color_format,
#endif
	    ((rd * dalpha) >> 8) + ((r * alpha) >> 8),
	    ((gd * dalpha) >> 8) + ((g * alpha) >> 8),
	    ((bd * dalpha) >> 8) + ((b * alpha) >> 8));
}

void blend50_16(UINT16 *dest, UINT16 src) {
    UINT8 rd,gd,bd,r,g,b;
#ifndef SDL
    rd = getr16(*dest);
    gd = getg16(*dest);
    bd = getb16(*dest);
    r = getr16(src);
    g = getg16(src);
    b = getb16(src);
    *dest = makecol16(
#else
    SDL_GetRGB(*dest,color_format,&rd,&gd,&bd);
    SDL_GetRGB(src,color_format,&r,&g,&b);
    *dest = SDL_MapRGB(color_format,
#endif
	    rd/2+r/2,
	    gd/2+g/2,
	    bd/2+b/2);
}

void blend_32(UINT32 *dest, UINT32 src) {
    UINT8 rd,gd,bd,r,g,b;
#ifndef SDL
    rd = getr32(*dest);
    gd = getg32(*dest);
    bd = getb32(*dest);
    r = getr32(src);
    g = getg32(src);
    b = getb32(src);
    *dest = makecol32(
#else
    SDL_GetRGB(*dest,color_format,&rd,&gd,&bd);
    SDL_GetRGB(src,color_format,&r,&g,&b);
    *dest = SDL_MapRGB(color_format,
#endif
	    ((rd * dalpha) >> 8) + ((r * alpha) >> 8),
	    ((gd * dalpha) >> 8) + ((g * alpha) >> 8),
	    ((bd * dalpha) >> 8) + ((b * alpha) >> 8));
}

void blend50_32(UINT32 *dest, UINT32 src) {
    UINT8 rd,gd,bd,r,g,b;
#ifndef SDL
    rd = getr32(*dest);
    gd = getg32(*dest);
    bd = getb32(*dest);
    r = getr32(src);
    g = getg32(src);
    b = getb32(src);
    *dest = makecol32(
#else
    SDL_GetRGB(*dest,color_format,&rd,&gd,&bd);
    SDL_GetRGB(src,color_format,&r,&g,&b);
    *dest = SDL_MapRGB(color_format,
#endif
	    rd/2+r/2,
	    gd/2+g/2,
	    bd/2+b/2);
}

void blend_8(UINT8 *dest, UINT8 src) {
    // This one is here just for the _Rot functions, it's impossible to do
    // blending in 8bpp, so just assign src to dest !
    *dest = src;
}

