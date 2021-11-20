#define NOGDI // define this before including windows.h to avoid BITMAP !
#ifdef ANDROID
#include <GLES/gl.h>
#else
#define GL_GLEXT_LEGACY // to try not to include glext.h which redefines the GL_GLEXT_VERSION which shouldn't have gone to SDL_opengl.h !
#include <SDL_opengl.h>
#endif
#undef WINAPI
#include <time.h>
#include "raine.h"
#include "blit.h"
#include "palette.h"
#include "compat.h"
#include "video/res.h"
#ifdef RAINE_UNIX
#include <unistd.h>
#endif
#include "gui/menu.h"
#ifdef USE_BEZELS
#include "bezel.h"
#endif
#include "winpos.h"
#include "opengl.h"
#include "display_sdl.h"
#include "bld.h"
#include "profile.h"

togl_options ogl;
float max_fps;

int disp_screen_x;
int disp_screen_y;
int desktop_w,desktop_h;

void display_read_config() {
   display_cfg.video_mode = raine_get_config_int( "Display", "video_mode", 3);
   if (display_cfg.video_mode != 0 && display_cfg.video_mode != 3)
       display_cfg.video_mode = 3;
   int x,y;
   SDL_GetWindowPosition(win,&x,&y);
   int x2 = raine_get_config_int("Display", "posx", x);
   int y2 = raine_get_config_int("Display", "posy", y);
   if (x2 != x || y2 != y)
       SDL_SetWindowPosition(win,x2,y2);

   display_cfg.screen_x = raine_get_config_int( "Display", "screen_x", display_cfg.screen_x);
   display_cfg.screen_y = raine_get_config_int( "Display", "screen_y", display_cfg.screen_y);
   video_fps = raine_get_config_int( "Display", "video_fps", 15); // when recording video
   char *s = raine_get_config_string("Display", "max_fps", "60");
   if (*s)
       sscanf(s,"%f",&max_fps);

   display_cfg.user_rotate = raine_get_config_int( "Display", "rotate", 0);
   display_cfg.user_flip = raine_get_config_int( "Display", "flip", 0);
   display_cfg.fix_aspect_ratio = raine_get_config_int("display", "fix_aspect_ratio", 1);
   display_cfg.fullscreen = raine_get_config_int("display", "fullscreen", 0);
   display_cfg.double_buffer = raine_get_config_int("display", "double_buffer", 1);
   use_bld = raine_get_config_int("display","use_bld",1);
   ogl.dbuf = raine_get_config_int("display","ogl_dbuf",1);
   ogl.render = raine_get_config_int("display","ogl_render",1); // 0 = drawpixels, 1 = texture
   ogl.overlay = raine_get_config_int("display","ogl_overlay",1); // overlay interface
   ogl.save = raine_get_config_int("display","ogl_save",1);
   ogl.filter = raine_get_config_int("display","ogl_filter",GL_NEAREST);
   strcpy(ogl.shader, raine_get_config_string("display","ogl_shader","None"));

   display_cfg.keep_ratio = raine_get_config_int("display","keep_ratio",1);
   set_bld1(raine_get_config_int("display","bld1",25));
   set_bld2(raine_get_config_int("display","bld2",50));
}

void set_opengl_filter(int filter) {
    if (filter == 1)
	ogl.filter = GL_LINEAR;
    else
	ogl.filter = GL_NEAREST; // default
}

void display_write_config() {
  if (!display_cfg.fullscreen && !display_cfg.noborder)
      update_window_pos();

   raine_set_config_int("Display", "video_mode", display_cfg.video_mode);
#ifdef RAINE_WIN32
   raine_set_config_int("Display", "video_driver", display_cfg.video_driver);
#endif
   print_debug("display_write_config: screen_x %d screen_y %d\n",display_cfg.screen_x,display_cfg.screen_y);
   raine_set_config_int("Display", "screen_x", display_cfg.screen_x);
   raine_set_config_int("Display", "screen_y", display_cfg.screen_y);
   raine_set_config_int("Display", "winx", display_cfg.winx);
   raine_set_config_int("Display", "winy", display_cfg.winy);
   raine_set_config_int("Display", "video_fps", video_fps);
   char s[20];
   snprintf(s,20,"%g",max_fps);
   raine_set_config_string("Display","max_fps",s);

   raine_set_config_int("Display", "rotate", display_cfg.user_rotate);
   raine_set_config_int("Display", "flip", display_cfg.user_flip);
   raine_set_config_int("display", "fix_aspect_ratio", display_cfg.fix_aspect_ratio);
   raine_set_config_int("display", "ogl_dbuf", ogl.dbuf);
   raine_set_config_int("display", "ogl_render", ogl.render);
   raine_set_config_int("display", "ogl_overlay", ogl.overlay);
   raine_set_config_int("display", "ogl_save", ogl.save);
   raine_set_config_int("display", "ogl_filter", ogl.filter);
   raine_set_config_string("display", "ogl_shader", ogl.shader);
   raine_set_config_int("display", "fullscreen", display_cfg.fullscreen);
   raine_set_config_int("display", "double_buffer", display_cfg.double_buffer);
   raine_set_config_int("display", "use_bld", use_bld);

   raine_set_config_int("Display", "keep_ratio", display_cfg.keep_ratio);
   raine_set_config_int("Display", "bld1", get_bld1());
   raine_set_config_int("Display", "bld2", get_bld2());
}

// Same thing, but saved as specific game parameters...
void load_screen_settings(char *section)
{
   if(raine_cfg.save_game_screen_settings){

       // DISPLAY

       display_cfg.frame_skip	= raine_get_config_int( section,        "frame_skip", 0);

       display_cfg.screen_x = raine_get_config_int(	section, "screen_x", display_cfg.screen_x);
       display_cfg.screen_y = raine_get_config_int(	section, "screen_y", display_cfg.screen_y);
       char *s = raine_get_config_string(section,"fps","");
       if (*s) {
	   sscanf(s,"%f",&fps);
       }
   }
}

void save_screen_settings(char *section)
{
   // remove all old settings

   raine_clear_config_section(section);

   if(raine_cfg.save_game_screen_settings){

       // DISPLAY

       raine_set_config_int(	section, "frame_skip", display_cfg.frame_skip);
       raine_set_config_int(	section, "screen_x", display_cfg.screen_x);
       raine_set_config_int(	section, "screen_y", display_cfg.screen_y);
       char buf[40];
       snprintf(buf,40,"%g",fps);
       raine_set_config_string(section,"fps",buf);


   }
}

void init_display() {
}

void ScreenChange(void)
{
}

int resize(int call,int sx,int sy) {
  // Minimum size
  static int last_time;
  if (keep_vga && (sx < 640 || sy < 480)) {
      SDL_SetWindowSize(win,640,480);
      return 0;
  }
  int changed = 0;
  if (sx < 320) { sx = 320; changed = 1; }
  if (sy < 200) { sy = 200; changed = 1; }
  if (sx == display_cfg.screen_x && sy == display_cfg.screen_y)
      return 0;
  display_cfg.screen_x = sx;
  display_cfg.screen_y = sy;

  if (current_game && display_cfg.keep_ratio ) {
    // keep aspect ratio
    VIDEO_INFO *video = (VIDEO_INFO*)current_game->video;
#ifdef USE_BEZELS
    double ratio = get_bezel_ratio();
    if (ratio < 0)
#else
	double
#endif
      ratio = video->screen_x*1.0/video->screen_y;
    if (video->flags & VIDEO_ROTATE_90 || (video->flags & 3)==VIDEO_ROTATE_270)
      ratio = 1/ratio;

    /* Resize to keep ratio but always within the size chosen by the user */
    if (time(NULL) - last_time > 1) {
	/* this timing thing is to try to detect windows managers which block
	 * resize commands, like compiz.
	 * Basically when a window size matches the fullscreen size, it switches
	 * its state to fullscreen. At this point the window can't be resized
	 * anymore, any resize attempt will result in another resize message
	 * sent immediately to restore it to fullscreen, which can create
	 * a stupid loop. So this timing here is to try to detect this kind
	 * of loop. Never understood this behavior anyway... */
	changed = 1;
	if (ratio < 1)
	    display_cfg.screen_x = ratio * display_cfg.screen_y;
	else
	    display_cfg.screen_y = display_cfg.screen_x / ratio;
    } else
	printf("blocking ratio correction\n");
    last_time = time(NULL);

    display_cfg.screen_x &= ~1; // even number
    // odd numbers can crash sdl_createyuvoverlay when libefence is in use !

    if (changed) SDL_SetWindowSize(win,display_cfg.screen_x,display_cfg.screen_y);
#ifdef USE_BEZELS
    bezel_fix_screen_size(&display_cfg.screen_x,&display_cfg.screen_y);
#endif
  }
  if (call) {
      print_debug("calling ScreenChange from resize\n");
      ScreenChange();
  }
  return 1;
}

void set_default_video_mode() {
  display_cfg.screen_x = 640;
  display_cfg.screen_y = 480;
  display_cfg.bpp = 32;
}

void setup_gfx_modes() {
  set_default_video_mode();
}

void clear_bitmap(BITMAP *screen) {
  int len = screen->w*screen->h*bytes_per_pixel(screen);
  SDL_Surface *s = get_surface_from_bmp(screen);
  if (s->pixels) {
    memset(s->pixels,0,len);
  }
}

void clear_raine_screen() {
    // The only call left for this thing is in gui.cpp before starting emulation
    // it doesn't seem necessary anymore with sdl2
}

// Sadly, I have to make this because I can't switch res as soon as I want.
// I have to indicate to the game being loaded the mode it will work in
// but wait for the gui to be redrawn before switching the res for real...
// Note also that the default values are 0 (no change)
int bestw,besth,bestbpp;

// Chooses a resolution with the variables bestw, besth and bestbpp
int switch_res(const VIDEO_INFO *vid)
{
    // Since res.c is removed from sdl2 build for now, there is not much point for this function...
   bestw = besth = bestbpp = 0; // No change !!!
   return 0;
}

void build_mode_list() {
}
