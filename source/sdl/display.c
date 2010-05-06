#include "raine.h"
#include "blit.h"
#include "palette.h"
#include "compat.h"
#include "video/res.h"
#ifdef RAINE_UNIX
#include <unistd.h>
#endif
#include "sdl/gui/menu.h"
#include "sdl/dialogs/messagebox.h"
#ifndef NEO
#include "bezel.h"
#endif
#include "winpos.h"

int disp_screen_x, prefered_yuv_format;
int disp_screen_y;
BITMAP *screen;
UINT32 videoflags;
static int desktop_w,desktop_h,desktop_bpp;
static char driver[128];
const SDL_VideoInfo *video_info;
UINT32 screen_flags;

void adjust_gui_resolution() {
  // To be called just before starting the gui, when already with a video mode
  // 1st keep the current video mode parameters for video info...
  video_info = SDL_GetVideoInfo();
  screen_flags = sdl_screen->flags;

  if (sdl_screen->w < 640 || sdl_screen->h < 480) {
    if (!strcmp(driver,"fbcon")) {
      SDL_QuitSubSystem(SDL_INIT_VIDEO);
      SDL_InitSubSystem(SDL_INIT_VIDEO);
    }
    sdl_screen = SDL_SetVideoMode(640,480,display_cfg.bpp,sdl_screen->flags | SDL_ANYFORMAT);
  }
  if (sdl_screen->format->BitsPerPixel < 16 && strcmp(driver,"fbcon")) {
    sdl_screen = SDL_SetVideoMode(sdl_screen->w,sdl_screen->h,16,sdl_screen->flags | SDL_ANYFORMAT);
  }
  if (sdl_screen->flags & SDL_DOUBLEBUF) {
    printf("disabling double buffer\n");
    sdl_screen = SDL_SetVideoMode(sdl_screen->w,sdl_screen->h,sdl_screen->format->BitsPerPixel,(sdl_screen->flags | SDL_ANYFORMAT) & ~SDL_DOUBLEBUF & ~SDL_HWSURFACE);
  }
  if (sdl_screen->flags & SDL_HWSURFACE) {
    sdl_screen = SDL_SetVideoMode(sdl_screen->w,sdl_screen->h,sdl_screen->format->BitsPerPixel,sdl_screen->flags & ~SDL_HWSURFACE);
  }
  SDL_ShowCursor(SDL_ENABLE);
  if (sdl_screen->flags & (SDL_DOUBLEBUF|SDL_HWSURFACE)) {
    printf("gros débile frappe encore\n");
    exit(1);
  }
}

void display_read_config() {
   if(display_cfg.scanlines == 2) display_cfg.screen_y <<= 1;

   char *pos = raine_get_config_string("Display", "position", "");
   if (pos) {
       static char buffer[100];
       snprintf(buffer,100,"SDL_VIDEO_WINDOW_POS=%s",pos);
       buffer[99] = 0;
       putenv(buffer);
       printf("window position recorded !\n");
   }

   display_cfg.video_mode = raine_get_config_int( "Display", "video_mode", 0);
   display_cfg.screen_x = raine_get_config_int( "Display", "screen_x", display_cfg.screen_x);
   display_cfg.screen_y = raine_get_config_int( "Display", "screen_y", display_cfg.screen_y);
   display_cfg.winx = raine_get_config_int( "Display", "winx", 640);
   display_cfg.winy = raine_get_config_int( "Display", "winy", 480);
   display_cfg.scanlines = raine_get_config_int( "Display", "scanlines", 0);
   display_cfg.stretch = raine_get_config_int( "Display", "stretch", 2);
   video_fps = raine_get_config_int( "Display", "video_fps", 15);

   display_cfg.user_rotate = raine_get_config_int( "Display", "rotate", 0);
   display_cfg.user_flip = raine_get_config_int( "Display", "flip", 0);
   display_cfg.no_rotate = raine_get_config_int( "Display", "no_rotate", 0);
   display_cfg.no_flip = raine_get_config_int( "Display", "no_flip", 0);
   display_cfg.auto_mode_change = raine_get_config_int( "Display", "auto_mode_change", 0);
   display_cfg.fix_aspect_ratio = raine_get_config_int("display", "fix_aspect_ratio", 1);
   display_cfg.fullscreen = raine_get_config_int("display", "fullscreen", 0);
   display_cfg.double_buffer = raine_get_config_int("display", "double_buffer", 1);
   prefered_yuv_format = raine_get_config_int("display","prefered_yuv_format",0);

   if(display_cfg.scanlines == 2) display_cfg.screen_y >>= 1;
}

void display_write_config() {
   if(display_cfg.scanlines == 2) display_cfg.screen_y <<= 1;

  if (!display_cfg.fullscreen)
      update_window_pos();

   raine_set_config_int("Display", "video_mode", display_cfg.video_mode);
   raine_set_config_int("Display", "screen_x", display_cfg.screen_x);
   raine_set_config_int("Display", "screen_y", display_cfg.screen_y);
   raine_set_config_int("Display", "winx", display_cfg.winx);
   raine_set_config_int("Display", "winy", display_cfg.winy);
   raine_set_config_int("Display", "scanlines", display_cfg.scanlines);
   raine_set_config_int("Display", "stretch", display_cfg.stretch);
   raine_set_config_int("Display", "video_fps", video_fps);

   raine_set_config_int("Display", "rotate", display_cfg.user_rotate);
   raine_set_config_int("Display", "flip", display_cfg.user_flip);
   raine_set_config_int("Display", "no_rotate", display_cfg.no_rotate);
   raine_set_config_int("Display", "no_flip", display_cfg.no_flip);
   raine_set_config_int("Display", "auto_mode_change", display_cfg.auto_mode_change);
   raine_set_config_int("display", "fix_aspect_ratio", display_cfg.fix_aspect_ratio);
   raine_set_config_int("display", "prefered_yuv_format", prefered_yuv_format);
   raine_set_config_int("display", "fullscreen", display_cfg.fullscreen);
   raine_set_config_int("display", "double_buffer", display_cfg.double_buffer);

   if(display_cfg.scanlines == 2) display_cfg.screen_y >>= 1;
}

// Same thing, but saved as specific game parameters...
void load_screen_settings(char *section)
{
   if(raine_cfg.save_game_screen_settings){

   // DISPLAY

   if(display_cfg.scanlines == 2) display_cfg.screen_y <<= 1;

   display_cfg.stretch = raine_get_config_int( section, "stretch", display_cfg.stretch);
   display_cfg.screen_x = raine_get_config_int(	section, "screen_x", display_cfg.screen_x);
   display_cfg.screen_y = raine_get_config_int(	section, "screen_y", display_cfg.screen_y);
   display_cfg.scanlines = raine_get_config_int(	section, "scanlines", display_cfg.scanlines);

   if(display_cfg.scanlines == 2) display_cfg.screen_y >>= 1;

   }
}

void save_screen_settings(char *section)
{
   // remove all old settings

   raine_clear_config_section(section);

   if(raine_cfg.save_game_screen_settings){

   // DISPLAY

   if(display_cfg.scanlines == 2) display_cfg.screen_y <<= 1;

   raine_set_config_int(	section, "screen_x", display_cfg.screen_x);
   raine_set_config_int(	section, "stretch", display_cfg.stretch);
   raine_set_config_int(	section, "screen_y", display_cfg.screen_y);
   raine_set_config_int(	section, "scanlines", display_cfg.scanlines);

   if(display_cfg.scanlines == 2) display_cfg.screen_y >>= 1;

   }
}

static void get_desktop_mode() {
  const SDL_VideoInfo *inf = SDL_GetVideoInfo();
  desktop_w = inf->current_w;
  desktop_h = inf->current_h;
  desktop_bpp = inf->vfmt->BitsPerPixel;
  print_debug("desktop : %d %d %d\n",desktop_w,desktop_h,desktop_bpp);
  if ( SDL_VideoDriverName(driver, sizeof(driver)) ) {
    print_debug("Video driver: %s\n", driver);
  } else
    driver[0] = 0;
}

static SDL_Surface *new_set_gfx_mode() {
  SDL_Surface *s;
  int bpp;
  if (display_cfg.fullscreen && !display_cfg.auto_mode_change) {
    if (!desktop_w) get_desktop_mode();
    display_cfg.screen_x = desktop_w;
    display_cfg.screen_y = desktop_h;
  }
  print_debug("entering new_set_gfx_mode: %d %d fs %d\n",display_cfg.screen_x,display_cfg.screen_y,display_cfg.fullscreen);

  /* In the mess of video flags :
   * SDL_ANYFORMAT is mandatory : without it, if the video mode can't be set
   * with the required bpp, then there will be a slow color conversion for each
   * blit and you definetely don't want that !!!
   *
   * SDL_DOUBLEBUF : doesn't seem to work anywhere in linux ! */

  // videoflags = SDL_HWSURFACE | SDL_HWPALETTE | SDL_FULLSCREEN | SDL_ANYFORMAT;
	// with libefence, I get a crash on createyuvoverlay if I pass the resizable
	// flag here. It might be related to the 32 bit compatibility layer on my
	// amd64 though. It seems harmless without libefence.
  videoflags = SDL_SWSURFACE| SDL_RESIZABLE| SDL_ASYNCBLIT|SDL_ANYFORMAT | SDL_HWPALETTE;
  if (display_cfg.double_buffer)
    videoflags |= SDL_DOUBLEBUF;
  if (display_cfg.fullscreen)
    videoflags |= SDL_FULLSCREEN;
  if (current_game && current_game->video_info->flags & VIDEO_NEEDS_8BPP) {
    bpp = 8;
    /* Actually we could leave the SDL_ANYFORMAT alone and let the
     * SDL_BlitSurface function do the color conversion... The only problem
     * is that with this new version, scale2x is used automatically (or 3x)
     * and when it's used, SDL_BlitSurface isn't used anymore, so there
     * can't be any color conversion !
     * I could do the color conversion manually in this case by allocating
     * a temporary 16bpp surface and blit to it, but it would be boresome,
     * and unefficient. The best solution is clearly to have every driver
     * to be compatible with 16bpp. Until now, this dirty work around with
     * SDL_ANYFORMAT will do.
     * What it does : if the physical screen is in 16 bpp or more, then
     * SDL_SetVideoMode returns a logical screen of 8bpp. Then SDL does
     * the color conversion tranparently. It's much easier to use... */
    videoflags &= (~SDL_ANYFORMAT); // we absolutely require 8bpp here !
  } else {
    // bpp = 16 is the default : better looking gui, and hq2x works at 16bpp min
    bpp = 16;
#ifdef DARWIN
      if (display_cfg.fullscreen)
	  bpp = desktop_bpp;
#endif
  }

  if (!desktop_w) {
    get_desktop_mode();
  }
  if (!strcmp(driver,"fbcon")) {
    // most fbcon drivers hate to change bpp.
    // It triggers a crash at least in matroxfb and nvidiafb, so we'd better avoid it
    display_cfg.bpp = bpp = desktop_bpp;
    print_debug("forcing bpp = %d on driver fbcon\n",desktop_bpp);
  }
  if ((videoflags & SDL_FULLSCREEN) == 0) {
    // Forbid windows larger than the screen
    if (display_cfg.screen_x > desktop_w)
      display_cfg.screen_x = desktop_w;
    if (display_cfg.screen_y > desktop_h)
      display_cfg.screen_y = desktop_h;
  }
  {
    /* In fullscreen modes, SDL accepts any resolution, and automatically
     * adds a black border to match the real screen size.
     * I don't want this, because I want to be able to select some scaler
     * to make this black border disappear, so I have to look directly in the
     * list of video modes... */
    int flags = videoflags;
    if (sdl_screen) flags = sdl_screen->flags;
    SDL_Rect **modes = SDL_ListModes(NULL,flags);
    if (modes && modes != (SDL_Rect **)-1) {
      int diffx = 10000, diffy = 10000, selected = 0;
      int i;
      for (i = 0; modes[i]; i++) {
	if (modes[i]->w >= display_cfg.screen_x && modes[i]->h >= display_cfg.screen_y &&
	    (modes[i]->w - display_cfg.screen_x < diffx || modes[i]->h - display_cfg.screen_y < diffy)) {
	  diffx = modes[i]->w - display_cfg.screen_x;
	  diffy = modes[i]->h - display_cfg.screen_y;
	  selected = i;
	}
      }
      if (diffx || diffy) {
	display_cfg.screen_x = modes[selected]->w;
	display_cfg.screen_y = modes[selected]->h;
      }
    }
  }
  print_debug("SDL_SetVideMode %d %d %d %x\n",display_cfg.screen_x, display_cfg.screen_y,
	  bpp, videoflags);
  /*
  SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
  SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
  SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
  SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
  videoflags |= SDL_OPENGL;
  */

  if (!sdl_screen || display_cfg.screen_x != sdl_screen->w ||
    display_cfg.screen_y != sdl_screen->h ||
    display_cfg.bpp != sdl_screen->format->BitsPerPixel ||
    sdl_screen->flags != videoflags) {
    if (!strcmp(driver,"fbcon")) {
      SDL_QuitSubSystem(SDL_INIT_VIDEO);
      SDL_InitSubSystem(SDL_INIT_VIDEO);
    }
    int overlay_w = 0, overlay_h = 0;
    if (sdl_overlay) {
      // There seems to be a bug in my mac os x version which makes fullscreen
      // modes with overlays all black. I can work around them with this.
      // So far there is still a crash when switching from fullscreen to
      // windowed mode and then calling the menu, the bug is in the sdl library
      // itself when calling SDL_SetVideoMode...
      overlay_w = sdl_overlay->w;
      overlay_h = sdl_overlay->h;
      print_debug("destroying sdl overlay...\n");
      SDL_FreeYUVOverlay(sdl_overlay);
      sdl_overlay = NULL;
    }
    if ( (s = SDL_SetVideoMode(display_cfg.screen_x, display_cfg.screen_y,
	    bpp, videoflags)) == NULL ) {
      SDL_Rect **modes = SDL_ListModes(NULL,videoflags);
      if (modes && modes != (SDL_Rect **)-1) {
	int diffx = 10000, diffy = 10000, selected;
	int i;
	for (i = 0; modes[i]; i++) {
	  if (
	      (abs(modes[i]->w - display_cfg.screen_x) < diffx || abs(modes[i]->h - display_cfg.screen_y) < diffy)) {
	    diffx = abs(modes[i]->w - display_cfg.screen_x);
	    diffy = abs(modes[i]->h - display_cfg.screen_y);
	    selected = i;
	  }
	}
	display_cfg.screen_x = modes[selected]->w;
	display_cfg.screen_y = modes[selected]->h;
	print_debug("found res %d %d\n",display_cfg.screen_x,display_cfg.screen_y);
      }
      if ( (s = SDL_SetVideoMode(display_cfg.screen_x, display_cfg.screen_y,
	      bpp, videoflags)) == NULL ) {
	fprintf(stderr,"could not setup %dx%d %d bpp\n",display_cfg.screen_x,display_cfg.screen_y,display_cfg.bpp);
	exit(1);
      }
    }
    sdl_screen = s;
    if (overlay_w) {
      print_debug("recreating sdl_overlay %d %d\n",overlay_w,overlay_h);
      sdl_create_overlay(overlay_w,overlay_h);
    }
  } else { // no need to change the mode
    s = sdl_screen;
  }
  display_cfg.screen_x = s->w; display_cfg.screen_y = s->h;
  disp_screen_y = display_cfg.screen_y;
  disp_screen_x = display_cfg.screen_x;
  display_cfg.bpp = s->format->BitsPerPixel;
  print_debug("mode %dx%d %dbpp fullscreen %d flags asked %x got %x video memory %x\n",display_cfg.screen_x, display_cfg.screen_y,display_cfg.bpp,s->flags & SDL_FULLSCREEN,videoflags,s->flags,s->flags&SDL_HWSURFACE);

/*  if (!color_format)
    color_format = sdl_screen->format; */
  if ((s->flags & SDL_DOUBLEBUF) || emulate_mouse_cursor) {
    SDL_ShowCursor(SDL_DISABLE);
  }
  return s;
}

void init_display() {
  new_set_gfx_mode();
  if (screen) free(screen);
  screen = surface_to_bmp(sdl_screen);
}

void ScreenChange(void)
{
  //show_mouse(NULL);

  SDL_Surface *s;
  if (!display_cfg.fullscreen)
      update_window_pos();

   if((s = new_set_gfx_mode()) == NULL){	// Didn't work:
      memcpy(&display_cfg, &prev_display_cfg, sizeof(DISPLAY_CFG));
      s = new_set_gfx_mode();	// Revert to old mode
   }
   else{
     SetupScreenBitmap();
      if (prev_display_cfg.bpp != display_cfg.bpp) {
	if (current_colour_mapper){
	  set_colour_mapper(current_colour_mapper);
	  ResetPalette();
	}
      }
   }
   if (screen) free(screen);
   screen = surface_to_bmp(s);
   //show_mouse(screen);
}

void resize() {
  // Minimum size
  if (display_cfg.screen_x < 320)
    display_cfg.screen_x = 320;
  if (display_cfg.screen_y < 240)
    display_cfg.screen_y = 240;


  if (current_game ) {
    // keep aspect ratio
    VIDEO_INFO *video = (VIDEO_INFO*)current_game->video_info;
#ifndef NEO
    double ratio = get_bezel_ratio();
    if (ratio < 0)
#else
    double
#endif
      ratio = video->screen_x*1.0/video->screen_y;
    int vx,vy;
    if (video->flags & VIDEO_ROTATE_90 || (video->flags & 3)==VIDEO_ROTATE_270){
      ratio = 1/ratio;
      vx = video->screen_y;
      vy = video->screen_x;
    } else {
      vx = video->screen_x;
      vy = video->screen_y;
    }

    /* Resize to keep ratio but always within the size chosen by the user */
    if (ratio < 1)
      display_cfg.screen_x = ratio * display_cfg.screen_y;
    else
      display_cfg.screen_y = display_cfg.screen_x / ratio;

    if (!sdl_overlay || display_cfg.video_mode == 2) { // normal blits
      if (display_cfg.scanlines >= 2) {
	if (display_cfg.screen_y < vy*2)
	  display_cfg.screen_y = vy*2;
	if (display_cfg.scanlines == 3 && display_cfg.screen_x < vx*2)
	  display_cfg.screen_x = vx*2;
      } 
    }
    display_cfg.screen_x &= ~1; // even number
    // odd numbers can crash sdl_createyuvoverlay when libefence is in use !

#ifndef NEO
    bezel_fix_screen_size(&display_cfg.screen_x,&display_cfg.screen_y);
#endif
  }
  ScreenChange();
}

void set_default_video_mode() {
  display_cfg.screen_x = 640;
  display_cfg.screen_y = 480;
  display_cfg.bpp = 16;
}

void setup_gfx_modes() {
  set_default_video_mode();
}

int lock_surface(SDL_Surface *s) {
  while (SDL_MUSTLOCK(s)) {
    if (SDL_LockSurface(s) < 0) {
      /* When locking fails in windows, the surface really becomes totally
      unavailable and we must abort the blits. It makes the testing of the
      return value quite burdensome but there is no other way... */
      return -1;
    }
    if (s == sdl_screen) {
      int a;
      for (a=0; a < s->h; a++)
	screen->line[a] = (UINT8 *)s->pixels+a*s->pitch;
    }
    return 1;
  }
  if (((UINT8*)sdl_screen->pixels) - screen->line[0]) {
    // These 2 must remain identical, but might change after some sdl
    // operations !!!
    print_debug("*** SCREEN CHANGE ***\n");
    if (screen) free(screen);
    screen = surface_to_bmp(sdl_screen);
  }

  return 0;
}

void clear_bitmap(BITMAP *screen) {
  int len = screen->w*screen->h*bytes_per_pixel(screen);
  SDL_Surface *s = get_surface_from_bmp(screen);
  int locked = lock_surface(s);
  if (locked > -1) {
    memset(s->pixels,0,len);
    if (locked)
      SDL_UnlockSurface(s);
  }
}

void clear_raine_screen() {
  clear_bitmap(screen);
  if (sdl_screen->flags & SDL_DOUBLEBUF) {
    SDL_Flip(sdl_screen);
    clear_bitmap(screen);
  } else {
    SDL_UpdateRect(sdl_screen,0,0,0,0);
  }
}

// Sadly, I have to make this because I can't switch res as soon as I want.
// I have to indicate to the game being loaded the mode it will work in
// but wait for the gui to be redrawn before switching the res for real...
// Note also that the default values are 0 (no change)
int bestw,besth,bestbpp;

// Chooses a resolution with the variables bestw, besth and bestbpp
int switch_res(const VIDEO_INFO *vid)
{
   int w,h;

   get_best_resolution(vid,&w,&h);
   if (display_cfg.auto_mode_change == 2) {
     w *= 2;
     h *= 2;
   }
   print_debug("trying to find a resolution close to %dx%d\n",w,h);
   if (vid->flags & VIDEO_NEEDS_16BPP) {
     if (display_cfg.bpp < 15)
       bestbpp = 16;
     else bestbpp = 0;
   } else if (vid->flags & VIDEO_NEEDS_8BPP)
     bestbpp = 8;
   else
     bestbpp = display_cfg.bpp;

   if (!(sdl_screen->flags&SDL_FULLSCREEN)) {
     if (w > desktop_w || h > desktop_h) {
       if (display_cfg.auto_mode_change == 2) {
	 w/=2;
	 h/=2;
       }
     }

     bestw = w; besth = h; // EVERY mode is available for a window
   } else {
     SDL_Rect **modes = SDL_ListModes(NULL,videoflags);
     if (modes && modes != (SDL_Rect **)-1) {
       if (modes[0]->w < w || modes[0]->h < h) {
	 if (display_cfg.auto_mode_change == 2) {
	   w/=2;
	   h/=2;
	 }
       }
       if (modes[0]->w < w || modes[0]->h < h) {
	 bestw = display_cfg.screen_x; besth = display_cfg.screen_y;
       } else {
	 bestw = w; besth = h;
       }
     }
   }

   if (display_cfg.screen_x != bestw || display_cfg.screen_y != besth
       || display_cfg.bpp != (UINT32)bestbpp)
     return 1;
   bestw = besth = bestbpp = 0; // No change !!!
   return 0;
}

void build_mode_list() {
}
