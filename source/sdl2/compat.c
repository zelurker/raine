// Compatibility layer with allegro

#include <SDL.h>
#include "raine.h"
#include <sys/stat.h>
#include "cpuid.h"
#include "video/tilemod.h" // internal_bpp
#include "blit.h" // ReClipScreen
#include <SDL_ttf.h>
#include "palette.h"
#include "blit_sdl.h"
#include "display_sdl.h"
#include "control.h"
#include "control_internal.h"
#include "blit_x2.h"
#include "video/res.h"
#include "dialogs/messagebox.h"
#include "opengl.h"
#include "loadpng.h"
#include "version.h"
#include <SDL_image.h>

struct BITMAP *sdl_create_bitmap_ex(int bpp, int w, int h) {
  // Init a fake bitmap to point to a newly created sdl_surface
  UINT32 r=0,g=0,b=0,a=0; // masks if necessary...
  SDL_Surface *s;

  if (bpp > 8) {

      switch(bpp) {
      case 8:
	  r = g = b = a = 0;
	  break;
      default:
	  // A considerable change compared to sdl-1.2... !
	  // The format here is just one I chose, 32 bits is almost mandatory in 3d, but for GameBitmap we don't need alpha
	  // so rgbx becomes the 1st choice
	  if (!SDL_PixelFormatEnumToMasks(SDL_PIXELFORMAT_RGBX8888,&bpp,&r,&g,&b,&a))
	  {
	      printf("masks pas ok\n");
	      exit(1);
	  }
      }
  }

  s = SDL_CreateRGBSurface(SDL_SWSURFACE, // these bitmaps are ALWAYS in ram
			   w,h,bpp,r,g,b,a);
  return surface_to_bmp(s);
}

// Not sure the locking functions are usefull, they are used only in the C
// video functions apparently. I should investigate about that...
void acquire_bitmap(BITMAP *bmp) {
  SDL_LockSurface(get_surface_from_bmp(bmp));
}

void release_bitmap(BITMAP *bmp) {
  SDL_UnlockSurface(get_surface_from_bmp(bmp));
}

// Strings...

// Return the filename without the path... / and \\ accepted (lovely dos !)
char *get_filename(char *f) {
  char *s = strrchr(f,'/');
  if (!s)
    s = strrchr(f,'\\');
  if (s)
    return s+1;
  return f;
}

#ifndef RAINE_WIN32
void strlwr(char *s) {
  while (*s) {
    if (*s >= 'A' && *s <= 'Z')
      *s+=32;
    s++;
  }
}
#endif

void put_backslash(char *s) {
  int l = strlen(s)-1;
  s += l;
  if (*s != '/' && *s != '\\')
    strcat(s,SLASH);
}

// Todo...

int exists(char *filename) {
  // This is very fast in linux, but the disk cache of windows for cdrom
  // seems catastrophic ! Extremely slow, no wonder that games never read from
  // cds... !!!
  struct stat buf;
  return !stat(filename,&buf); // stat returns 0 on success...
}

SDL_PixelFormat *color_format;
SDL_Window *win;
SDL_Renderer *rend;
SDL_Surface *sdl_screen;

void sdl_init() {
  /* Initialize the SDL library */
    static int init;
    if (!init) {
	init = 1;
	if ( SDL_Init(
		    SDL_INIT_TIMER|SDL_INIT_AUDIO| SDL_INIT_VIDEO|SDL_INIT_GAMECONTROLLER
		    ) < 0 ) {
	    fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
	    exit(2);
	}
	if ( TTF_Init() < 0 ) {
	    fprintf(stderr, "Couldn't initialize TTF: %s\n",SDL_GetError());
	    SDL_Quit();
	    return;
	}
	char title[20];
	snprintf(title,20,"%s v%s",EMUNAME,VERSION);
	title[19] = 0;
	win = SDL_CreateWindow(title,
		display_cfg.posx,
		display_cfg.posy,
		(display_cfg.fullscreen ? display_cfg.winx : display_cfg.screen_x),
		(display_cfg.fullscreen ? display_cfg.winy : display_cfg.screen_y),
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | (display_cfg.fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));
	rend = SDL_CreateRenderer(win,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_Surface *sf = IMG_Load("bitmaps/bub2.png");
	if (!sf) {
	    printf("can't load icon\n");
	} else {
	    SDL_SetWindowIcon(win,sf);
	}

	// if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1)< 0 || SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3) < 0)
	//    printf("pb setting opengl version\n");

	inputs_preinit();

	atexit(sdl_done);
    }
}

void sdl_done() {
  // This should also save the sdl specific params... Later !
  print_debug("sdl_done...\n");
  inputs_done();
  TTF_Quit();
  opengl_done();
  SDL_Quit();
}

BITMAP *surface_to_bmp(SDL_Surface *s) {
  int a;
  BITMAP *bmp;

  bmp = malloc( sizeof(BITMAP) + sizeof(char*) * s->h);
  bmp->extra = (void*)s;
  bmp->w = s->w; bmp->h = s->h;
  bmp->x_ofs = bmp->y_ofs = 0;
  for (a=0; a < s->h; a++)
    bmp->line[a] = (UINT8 *)s->pixels+a*s->pitch;
  bmp->dat = s->pixels;
  bmp->id = 0; // not a sub bitmap
  return bmp;
}

void destroy_bitmap(BITMAP *bmp) {
  if (bmp) {
    if (bmp->extra) {
      if (bmp->id == 0) { // real bitmap (not sub bitmap)
	  print_debug("destroy_bitmap: calling sdl_freesurface\n");
	  SDL_FreeSurface((SDL_Surface*)bmp->extra);
      } else if (bmp->id == 2) { // sdl_overlay
	  print_debug("destroy_bitmap: calling sdl_freeoverlay\n");
	  // should be impossible to arrive here normally
	  exit(1);
      }
    }
    free(bmp);
  }
}

UINT16 bytes_per_pixel(BITMAP *screen) {
  if (screen->extra)
    return ((screen->id&2) == 2 ? 2 // overlay
	: ((SDL_Surface*)screen->extra)->format->BytesPerPixel);
  printf("no vtable and no extras for this bitmap !!!\n");
  exit(1);
  return 0;
}

struct BITMAP *sdl_create_sub_bitmap(struct BITMAP *src, int x, int y, int w, int h) {
  /* Apparently a sub bitmap is just a bitmap with x_ofs and y_ofs correctly */
  /* initialised... */

  BITMAP *bmp;
  int bpp;
  int n;

  if (src->id == 2) // SDL_Overlay
    bpp = 2;
  else
    bpp = bytes_per_pixel(src);

  bmp = malloc( sizeof(struct BITMAP) + sizeof(char*) * h);
  bmp->dat = src->dat;
  bmp->w = w; bmp->h = h;
  bmp->x_ofs = x;
  bmp->y_ofs = y;
  for (n=0; n<h; n++)
    bmp->line[n] = src->line[n+y] + bpp*x;
  bmp->extra = src->extra;
  bmp->id = src->id+1; // sub bitmap
  color_format = sdl_game_bitmap->format;
  print_debug("color_format: game bitmap format bpp %d\n",sdl_game_bitmap->format->BitsPerPixel);

  if (!(display_cfg.video_mode == 0) &&
	  (!current_game ||
	   !(current_game->video->flags & VIDEO_NEEDS_8BPP))) {
    /* We start raine in 16bpp for yuv overlays.
     * If a hw yuv overlay can't be created, then adapt to the bpp of the
     * screen to speed up blits */
    if (current_game->video->flags & VIDEO_NEEDS_8BPP)
      display_cfg.bpp = 8;
    else {
      display_cfg.bpp = 32;
    }
    /*
       if (display_cfg.video_mode == 0)
       display_cfg.bpp = 16;
       */
    print_debug("bpp selected %d\n",display_cfg.bpp);
    if (current_colour_mapper) {
      set_colour_mapper(current_colour_mapper);
      ResetPalette();
    }
  }
  return bmp;
}

void save_png_surf_rev(char *name, SDL_Surface *s) {
    BITMAP *b = surface_to_bmp(s);
    int a;
    // Flip the picture, it's upside down in opengl
    for (a=0; a < b->h; a++) {
	b->line[a] = (UINT8 *)s->pixels+(b->h-1-a)*s->pitch;
    }
    save_png(name,b,NULL);
    destroy_bitmap(b);
}

