#ifndef SDL_COMPAT_H
#define SDL_COMPAT_H

#ifdef __cplusplus
extern "C" {
#endif
// Compatibility layer with allegro

#include "deftypes.h"

typedef struct al_bitmap            /* a bitmap structure */
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
   unsigned char *line[0];
} al_bitmap;

extern UINT32 sdl2_color_format;

// Bitmap functions
#define fatal_error(format, ...) sdl_fatal_error(__FILE__,__func__,__LINE__, format, ## __VA_ARGS__)
void sdl_fatal_error(const char *file, const char*func, int line, char *format, ...);
al_bitmap *sdl_create_bitmap_ex(int bpp, int w, int h);
void clear_bitmap(al_bitmap *bitmap);
void acquire_bitmap(al_bitmap *bmp);
void release_bitmap(al_bitmap *bmp);

void destroy_bitmap(struct al_bitmap *bmp);

#ifdef RGB
#undef RGB
#endif

#define RGB SDL_Color

struct al_bitmap *sdl_create_sub_bitmap(struct al_bitmap *src, int x, int y, int w, int h);
UINT16 bytes_per_pixel(al_bitmap *screen);

#ifndef RAINE_WIN32
#define stricmp strcasecmp
#define strnicmp strncasecmp
#endif

// SDLK_LAST_KEY is around 334 right now, let's take some margin and choose
// 512 values for the array key...
extern unsigned char key[0x300];
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

// defined in control.c, but only for the sdl version
#define get_mouse_mickeys my_get_mouse_mickeys
void my_get_mouse_mickeys(int *mx, int *my);

#define allegro_message printf

#ifdef RAINE_DEBUG
   #define ASSERT(condition)  if (!(condition)) { fprintf(stderr,"assertion failed file %s line %d\n",__FILE__, __LINE__); exit(1); }
#else
   #define ASSERT(condition)
#endif

void setup_gfx_modes();

void sdl_create_overlay( int w, int h);
#define bitmap_color_depth(b) bytes_per_pixel(b)*8

#define create_bitmap_ex sdl_create_bitmap_ex
#define create_bitmap(w, h) sdl_create_bitmap_ex(display_cfg.bpp,w,h)

extern void load_message(char *);

#define load_explicit_progress(a,b) load_progress("emudx",(a)*100/(b))

#ifdef __cplusplus
}
#endif
#endif

