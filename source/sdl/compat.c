// Compatibility layer with allegro

#include <SDL.h>
#include "raine.h"
#include <sys/stat.h>
#include "raine_cpuid.h"
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
#include "files.h"

void sdl_fatal_error(const char *file, const char *func, int line, char *format, ...) {
    char msg[512];
    va_list ap;
    va_start(ap,format);
    snprintf(msg,512,"function %s file %s line %d\n",func,file,line);
    vsnprintf(&msg[strlen(msg)],512-strlen(msg),format,ap);
    va_end(ap);
    fprintf(stderr,"Error: %s",msg);
    if (current_game->clear) (*current_game->clear)();
    exit(1);
}

static SDL_PixelFormat overlay_format = {
  NULL,
  16,
  2,
  3, 2, 3, 0,
  0, 5, 11, 0,
  0x1f, 0x7e0, 0xf800, 0 };

// Just to be bothersome, they chose the other format for 16bpp !!!
static SDL_PixelFormat hq2x_format = {
  NULL,
  16,
  2,
  3, 2, 3, 0,
  11, 5, 0, 0,
  0xf800, 0x7e0, 0x1f, 0 };

struct al_bitmap *sdl_create_bitmap_ex(int bpp, int w, int h) {
  // Init a fake bitmap to point to a newly created sdl_surface
  int r=0,g=0,b=0,a=0; // masks if necessary...
  SDL_Surface *s;

  if (bpp > 8) {
    const SDL_VideoInfo *inf = SDL_GetVideoInfo();
    SDL_PixelFormat *fmt = inf->vfmt;
    if (sdl_overlay) {
      fmt = &overlay_format;
    } else if (display_cfg.stretch == 3 && display_cfg.video_mode == 2) // hq2x
      fmt = &hq2x_format;

    if (fmt->BitsPerPixel != bpp) {
      /* SDL interprets each pixel as a 32-bit number, so our masks must depend
       * on the endianness (byte order) of the machine
       * (taken from SDL_CreateRGBSurface manual page) */
      switch (bpp) {
      case 24:
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	r = 0xff000000;
	g = 0x00ff0000;
	b = 0x0000ff00;
	a = 0;
#else
	r = 0x000000ff;
	g = 0x0000ff00;
	b = 0x00ff0000;
	a = 0;
#endif
	break;
      case 32:
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	r = 0xff000000;
	g = 0x00ff0000;
	b = 0x0000ff00;
	a = 0x000000ff;
#else
	r = 0x000000ff;
	g = 0x0000ff00;
	b = 0x00ff0000;
	a = 0xff000000;;
#endif
	break;
      case 16:
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	r = 0x0000f800;
	g = 0x000007e0;
	b = 0x0000001f;
	a = 0x00000000;
#else
	r = 0x0000001f;
	g = 0x000007e0;
	b = 0x0000f800;
	a = 0x00000000;;
#endif
	break;
      case 15:
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	r = 0x00007c00;
	g = 0x000003e0;
	b = 0x0000001f;
	a = 0x00000000;
#else
	r = 0x0000001f;
	g = 0x000003e0;
	b = 0x00007c00;
	a = 0x00000000;;
#endif
	break;
      case 8:
        r = g = b = a = 0;
	break;
      }
    } else {
      // easiest case, just copy from the video info
      r = fmt->Rmask;
      g = fmt->Gmask;
      b = fmt->Bmask;
      a = fmt->Amask;
    }
  }

  s = SDL_CreateRGBSurface(SDL_SWSURFACE, // these bitmaps are ALWAYS in ram
			   w,h,bpp,r,g,b,a);
  return surface_to_bmp(s);
}

// Not sure the locking functions are usefull, they are used only in the C
// video functions apparently. I should investigate about that...
void acquire_bitmap(al_bitmap *bmp) {
  SDL_LockSurface(get_surface_from_bmp(bmp));
}

void release_bitmap(al_bitmap *bmp) {
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
SDL_Surface *sdl_screen;
SDL_Overlay *sdl_overlay;

void sdl_init() {
  /* Initialize the SDL library */
    static int init;
    if (!init) {
	init = 1;
	if ( SDL_Init(
		    SDL_INIT_TIMER|SDL_INIT_AUDIO| SDL_INIT_VIDEO|SDL_INIT_JOYSTICK|SDL_INIT_CDROM
		    ) < 0 ) {
	    fatal_error( "Couldn't initialize SDL: %s",SDL_GetError());
	}
	if ( TTF_Init() < 0 ) {
	    fprintf(stderr, "Couldn't initialize TTF: %s\n",SDL_GetError());
	    SDL_Quit();
	    return;
	}
	char title[20];
	snprintf(title,20,"%s v%s",EMUNAME,VERSION);
	title[19] = 0;
	SDL_WM_SetCaption(title,title);

	inputs_preinit();

	atexit(sdl_done);
    }
}

static void save_game_stats() {
    FILE *f = fopen(get_shared("savedata" SLASH "stats"),"w");
    if (!f) {
	printf("save_game_stats: can't create savedata/stats ?!! path:%s\n",get_shared("savedata" SLASH "stats"));
	return;
    }
    for (int n=0; n<game_count; n++) {
	if (game_list[n]->time_played || game_list[n]->last_played)
#ifdef __x86_64__
	    fprintf(f,"%s=%d,%d,%d\n",game_list[n]->main_name,game_list[n]->nb_loaded,game_list[n]->time_played,game_list[n]->last_played);
#else
	    fprintf(f,"%s=%d,%d,%ld\n",game_list[n]->main_name,game_list[n]->nb_loaded,game_list[n]->time_played,game_list[n]->last_played);
#endif
    }
    fclose(f);
}

void sdl_done() {
  // This should also save the sdl specific params... Later !
  print_debug("sdl_done...\n");
  inputs_done();
  TTF_Quit();
  opengl_done();
  save_game_stats();
  SDL_Quit();
}

al_bitmap *surface_to_bmp(SDL_Surface *s) {
  int a;
  al_bitmap *bmp;

  bmp = malloc( sizeof(al_bitmap) + sizeof(char*) * s->h);
  bmp->extra = (void*)s;
  bmp->w = s->w; bmp->h = s->h;
  bmp->x_ofs = bmp->y_ofs = 0;
  for (a=0; a < s->h; a++)
    bmp->line[a] = (UINT8 *)s->pixels+a*s->pitch;
  bmp->dat = s->pixels;
  bmp->id = 0; // not a sub bitmap
  return bmp;
}

void sdl_create_overlay( int w, int h) {
  if (sdl_screen->flags & SDL_OPENGL) {
      print_debug("no overlays with opengl\n");
      return;
  }
  // Init a fake bitmap to point to a newly created sdl_surface
  if (sdl_overlay) {
    SDL_FreeYUVOverlay(sdl_overlay);
    sdl_overlay = NULL;
  }

  if (!(raine_cpu_capabilities & CPU_MMX)) {
    print_debug("no MMX support -> no overlay\n");
    return;
  }
  if (sdl_screen->format->BitsPerPixel == 8) {
      print_debug("no overlay in 8bpp\n");
    return;
  }

  if (!current_game || !(current_game->video->flags & VIDEO_NEEDS_8BPP)) {
    sdl_overlay = SDL_CreateYUVOverlay( // these bitmaps are ALWAYS in ram
	w,h,(prefered_yuv_format == 0 ? SDL_YUY2_OVERLAY : SDL_YV12_OVERLAY),
	sdl_screen);
    if (sdl_overlay && !sdl_overlay->hw_overlay) {
      // Try YUY2 1st, because my rgb -> yuv function seems more efficient
      // with this one. Then try yv12 because it's the most common yuv format
      // especially with mpeg cards like the wintv pvr 350
      SDL_FreeYUVOverlay(sdl_overlay);
      sdl_overlay = SDL_CreateYUVOverlay( // these bitmaps are ALWAYS in ram
       w,h,(prefered_yuv_format == 0 ? SDL_YV12_OVERLAY : SDL_YUY2_OVERLAY),
       sdl_screen);
    }
  }
  if (sdl_overlay && !sdl_overlay->hw_overlay) {
    print_debug("no hardware support for YUY2 overlay\n");
    static int warned_hw;
    if (!warned_hw) {
	warned_hw = 1;
	raine_mbox(gettext("Warning"),gettext("No hardware support for overlays"),gettext("ok"));
    }
     if ( display_cfg.video_mode != 1) {
	 SDL_FreeYUVOverlay(sdl_overlay);
	 sdl_overlay = NULL;
     }
    // return;
  } else if (sdl_overlay)
    print_debug("using YUV overlay\n");

/*  printf("w %d h %d bpp %d\n",w,h,color_format->BitsPerPixel);
  printf("masks r %x g %x b %x a %x\nshifts : %d,%d,%d,%d\n",
	 fmt->Rmask,fmt->Gmask,fmt->Bmask,fmt->Amask,
	 fmt->Rshift,fmt->Gshift,fmt->Bshift,fmt->Ashift); */

  // force the right color format
/*  if (sdl_overlay && sdl_screen->format->BitsPerPixel == 16) {
    if (current_game) {
      SDL_PixelFormat *fmt = sdl_screen->format;
      fmt->Rmask = 0x1f; fmt->Gmask = 0x7e0; fmt->Bmask = 0xf800;
      fmt->Rshift = 0;   fmt->Gshift= 5;     fmt->Bshift= 11;
    }
  } */

/*   if ( SDL_MUSTLOCK(screen) ) { */
/*     fprintf(stderr,"Locing problem !\n"); */
/*     SDL_Quit(-1); */
/*   } */

  ReClipScreen();
  if (GameBitmap) {
    SDL_Surface *sdl_game_bitmap = get_surface_from_bmp(GameBitmap);
    print_debug("fmt avant %x %x %x, shifts %x %x %x\n",
	sdl_game_bitmap->format->Rloss,sdl_game_bitmap->format->Gloss,sdl_game_bitmap->format->Bloss,
	sdl_game_bitmap->format->Rshift,sdl_game_bitmap->format->Gshift,sdl_game_bitmap->format->Bshift);
  }
}

void destroy_bitmap(al_bitmap *bmp) {
  if (bmp) {
    if (bmp->extra) {
      if (bmp->id == 0) { // real bitmap (not sub bitmap)
	  print_debug("destroy_bitmap: calling sdl_freesurface\n");
	  SDL_FreeSurface((SDL_Surface*)bmp->extra);
      } else if (bmp->id == 2) { // sdl_overlay
	  print_debug("destroy_bitmap: calling sdl_freeoverlay\n");
	  SDL_FreeYUVOverlay((SDL_Overlay*)bmp->extra);
      }
    }
    free(bmp);
  }
}

UINT16 bytes_per_pixel(al_bitmap *screen) {
  if (screen->extra)
    return ((screen->id&2) == 2 ? 2 // overlay
	: ((SDL_Surface*)screen->extra)->format->BytesPerPixel);
  fatal_error("no vtable and no extras for this bitmap !!!");
  return 0;
}

blit_x2_func *my_blit_x2_y1,*my_blit_x2_y2;

struct al_bitmap *sdl_create_sub_bitmap(struct al_bitmap *src, int x, int y, int w, int h) {
  /* Apparently a sub bitmap is just a bitmap with x_ofs and y_ofs correctly */
  /* initialised... */

  al_bitmap *bmp;
  int bpp;
  int n;

  if (src->id == 2) // SDL_Overlay
    bpp = 2;
  else
    bpp = bytes_per_pixel(src);

  bmp = malloc( sizeof(struct al_bitmap) + sizeof(char*) * h);
  bmp->dat = src->dat;
  bmp->w = w; bmp->h = h;
  bmp->x_ofs = x;
  bmp->y_ofs = y;
  for (n=0; n<h; n++)
    bmp->line[n] = src->line[n+y] + bpp*x;
  bmp->extra = src->extra;
  bmp->id = src->id+1; // sub bitmap
  if (display_cfg.video_mode == 1) {
    sdl_create_overlay(w,h);
  }
  if (sdl_overlay) {
    color_format = &overlay_format;
    print_debug("color_format: overlay_format\n");
  } else {
    color_format = sdl_game_bitmap->format;
    print_debug("color_format: game bitmap format bpp %d\n",sdl_game_bitmap->format->BitsPerPixel);
  }

  if (!sdl_overlay && !(sdl_screen->flags & SDL_OPENGL) &&
	  (!current_game ||
	   !(current_game->video->flags & VIDEO_NEEDS_8BPP))) {
    /* We start raine in 16bpp for yuv overlays.
     * If a hw yuv overlay can't be created, then adapt to the bpp of the
     * screen to speed up blits */
    if (current_game->video->flags & VIDEO_NEEDS_8BPP)
      display_cfg.bpp = 8;
    else {
      display_cfg.bpp = sdl_screen->format->BitsPerPixel;
      if (display_cfg.stretch == 3) // hq2x is in 16bpp source only
	display_cfg.bpp = 16;
    }
    /*
       if (sdl_screen->flags & SDL_OPENGL)
       display_cfg.bpp = 16;
       */
    print_debug("bpp selected %d\n",display_cfg.bpp);
    if (current_colour_mapper) {
      set_colour_mapper(current_colour_mapper);
      ResetPalette();
    }
  } else if ((sdl_overlay /* || sdl_screen->flags & SDL_OPENGL */) && bpp != 2) {
    // Overlays are for 16bpp only (the mmx code knows only how to handle
    // 16bpp)
    display_cfg.bpp = 16;
    if (current_colour_mapper) {
      set_colour_mapper(current_colour_mapper);
      ResetPalette();
    }
  }
  if (sdl_overlay) {
    print_debug("color_format: overlay_format\n");
    color_format = &overlay_format;
    *get_surface_from_bmp(GameBitmap)->format = overlay_format;
  }
  InitLUTs();
  switch(display_cfg.bpp) {
    case 8: my_blit_x2_y1 = raine_blit_x2_y1;
      my_blit_x2_y2 = raine_blit_x2_y2;
      break;
    case 16: my_blit_x2_y1 = raine_blit_x2_y1_16;
      my_blit_x2_y2 = raine_blit_x2_y2_16;
      break;
    case 32: my_blit_x2_y1 = raine_blit_x2_y1_32;
      my_blit_x2_y2 = raine_blit_x2_y2_32;
      break;
  }
  return bmp;
}

void save_png_surf_rev(char *name, SDL_Surface *s) {
    al_bitmap *b = surface_to_bmp(s);
    int a;
    // Flip the picture, it's upside down in opengl
    for (a=0; a < b->h; a++) {
	b->line[a] = (UINT8 *)s->pixels+(b->h-1-a)*s->pitch;
    }
    save_png(name,b,NULL);
    destroy_bitmap(b);
}

