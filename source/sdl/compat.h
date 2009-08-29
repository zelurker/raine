#ifndef SDL_COMPAT_H
#define SDL_COMPAT_H

#ifdef __cplusplus
extern "C" {
#endif
// Compatibility layer with allegro

#include <SDL.h>
#include "deftypes.h"

#ifndef WINAPI
// windows.h has a wonderfull name collision with BITMAP !
// usually windows.h is not included, so we assume that we don't need both
// declarations at the same time !
typedef struct BITMAP            /* a bitmap structure */
{
   int w, h;                     /* width and height in pixels */
   int clip;                     /* flag if clipping is turned on */
   int cl, cr, ct, cb;           /* clip left, right, top and bottom values */
   void *vtable;           /* drawing functions */
   void *write_bank;             /* C func on some machines, asm on i386 */
   void *read_bank;              /* C func on some machines, asm on i386 */
   void *dat;                    /* the memory we allocated for the bitmap */
   unsigned long id;             /* for identifying sub-bitmaps */
   void *extra;                  /* points to a structure with more info */
   int x_ofs;                    /* horizontal offset (for sub-bitmaps) */
   int y_ofs;                    /* vertical offset (for sub-bitmaps) */
   int seg;                      /* bitmap segment */
   Uint8 *line[0];
} BITMAP;

// Bitmap functions
BITMAP *sdl_create_bitmap_ex(int bpp, int w, int h);
void clear_bitmap(BITMAP *bitmap);
#define sdl_create_bitmap(w, h) sdl_create_bitmap_ex(display_cfg.bpp,w,h)
struct BITMAP *surface_to_bmp(SDL_Surface *s);
void acquire_bitmap(BITMAP *bmp);
void release_bitmap(BITMAP *bmp);

void destroy_bitmap(struct BITMAP *bmp);
// clear is normally useless... Hope so at least (if called after creation)
#define clear(x)

#define RGB SDL_Color

// Misc
int  save_pcx(const char *filename, BITMAP *bmp, const RGB
	      *pal);
static inline SDL_Surface *get_surface_from_bmp(BITMAP *bmp) {
  return (SDL_Surface *)bmp->extra;
}

extern BITMAP *screen;
struct BITMAP *sdl_create_sub_bitmap(struct BITMAP *src, int x, int y, int w, int h);
UINT16 bytes_per_pixel(BITMAP *screen);

#endif
#ifdef __GLIBC__
#if __GLIBC__ >= 2
#define stricmp strcasecmp
#define strnicmp strncasecmp
#endif
#elif defined(DARWIN)
#define stricmp strcasecmp
#define strnicmp strncasecmp
#endif

typedef SDL_Color PALETTE[256];

// SDLK_LAST_KEY is around 334 right now, let's take some margin and choose
// 512 values for the array key...
extern Uint8 key[0x300];
// A few scan codes directly used (berk !)
#define KEY_F7 SDLK_F7
#define KEY_F8 SDLK_F8
#define KEY_J  SDLK_j
#define KEY_K  SDLK_k
#define KEY_I  SDLK_i
#define KEY_O  SDLK_o

void sdl_init();
void sdl_done();


// Mouse... Very different now... Just declare the vars, and see later...
extern int mouse_x,mouse_y,mouse_b;
void set_mouse_range(int x1, int y1, int x2, int y2); // does nothing !

// Strings
#ifndef RAINE_WIN32
void strlwr(char *s);
#endif
void put_backslash(char *s);
char *get_filename(char *f);

// Joystick
void init_joys(); // Does nothing for now

// GUI
void StartGUI(); 

// Files
int exists(char *filename);

// defined in sdl/control.c, but only for the sdl version
#define get_mouse_mickeys my_get_mouse_mickeys
void my_get_mouse_mickeys(int *mx, int *my);

#define allegro_message printf

#ifdef RAINE_DEBUG
   #define ASSERT(condition)  if (!(condition)) { fprintf(stderr,"assertion failed file %s line %d\n",__FILE__, __LINE__); exit(1); } 
#else
   #define ASSERT(condition)
#endif

// Allegro -> SDL_gfx primitives
#define line(bitmap,x1,y1,x2,y2,color) \
  lineColor(get_surface_from_bmp(bitmap),x1,y1,x2,y2,color)


extern SDL_Surface *sdl_screen;
extern SDL_PixelFormat *color_format;
extern SDL_Overlay *sdl_overlay;

void setup_gfx_modes();

void sdl_create_overlay( int w, int h);
#define bitmap_color_depth(b) bytes_per_pixel(b)*8

#define create_bitmap_ex sdl_create_bitmap_ex
#define create_bitmap(w, h) sdl_create_bitmap_ex(display_cfg.bpp,w,h)

#ifdef __cplusplus
}
#endif
#endif

