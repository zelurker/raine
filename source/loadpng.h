/* loadpng.h */
/* This file is hereby placed in the public domain. */
#ifndef _included_loadpng_h_
#define _included_loadpng_h_

#include "unzip.h"
#include "compat_sdl.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Choose zlib compression level for saving file.
 * Default is Z_BEST_COMPRESSION.
 */
extern int _png_compression_level;


/* Load a PNG from disk. */
extern al_bitmap *load_png(const char *filename, RGB *pal);
extern al_bitmap *load_png_from_zip(unzFile uf, RGB *pal);

/* Save a bitmap to disk in PNG format. */
extern int save_png(const char *filename, al_bitmap *bmp, const RGB *pal);

#ifdef __cplusplus
}
#endif

#endif /* _included_loadpng_h */
