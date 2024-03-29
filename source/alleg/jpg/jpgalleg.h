
#ifdef __cplusplus
extern "C" {
#endif
/*
 *      JPGalleg: JPEG image decoding routines for Allegro
 *
 *      version 1.1, by Angelo Mottola, May/June 2000
 *
 *      Include file
 */

#ifndef JPGALLEG_H
#define JPGALLEG_H

#include <allegro.h>

/* load_memory_jpg:
 *  Decodes a JPG image from a block of memory data.
 */
al_bitmap *load_memory_jpg(void *data, RGB *pal);

/* load_jpg:
 *  Decodes a JPG image from a standard JPG file.
 */
al_bitmap *load_jpg(char *filename, RGB *pal);

#endif

#ifdef __cplusplus
}
#endif
