#include "../gui/menu.h"
#include "raine.h" // display_cfg
#include "video_info.h"
#include "blit.h"
#include "sdl/blit_sdl.h"
#include "control.h"
#include "sdl/control_internal.h"
#include "sdl/display_sdl.h"
#include "sdl/dialogs/messagebox.h"

class TVideo : public TMenu
{
  public:
  TVideo(char *my_title, menu_item_t *mymenu) :
    TMenu(my_title,mymenu)
    {}
  int can_be_displayed(int n) {
    if (n >= 4 && n <= 6) // yuv overlay options
      return ((sdl_overlay != NULL || display_cfg.video_mode <= 1) &&
        display_cfg.video_mode != 2);
    else if (n >=7 && n <= 10) // normal blits
      if (display_cfg.video_mode <= 1) {
	// At first I had these options shown when in autodetect mode
	// (video_mode == 0), but it's unclear for the users since the default
	// is to use yuv overlays these options are indeed rarely used in
	// autodetect mode. So it's probably better to hide them completely
	// and keep them only for "force normal blits"
	return 0;
      }
    return 1;
  }
  void update_options() {
    if (font) {
      delete font;
      font = NULL;
    }
    draw();
  }
};

static TVideo *video_options;

static int my_toggle_fullscreen(int sel) {
  display_cfg.fullscreen ^= 1;
  toggle_fullscreen();
  adjust_gui_resolution();
  video_options->draw();
  if (!(sdl_screen->flags & SDL_DOUBLEBUF) && !emulate_mouse_cursor)
    SDL_ShowCursor(SDL_ENABLE);
  return 0;
}
  
static int update_options(int sel) {
  // Each time we change the renderer type, we redraw the whole dialog
  // to change the options which should be hidden/shown
  video_options->update_options();
  return 0;
}

static int update_scaler(int sel) {
  if (display_cfg.scanlines && display_cfg.stretch) { // scaling options
    MessageBox("Warning","You can't have at the same time a scaler + scanlines,\nChoose 1","Ok");
    display_cfg.stretch = 0; // no scaling if scanlines
    video_options->draw();
  }
  return 0;
}

static menu_item_t video_items[] =
{
{  "Video renderer", &update_options, (int*)&display_cfg.video_mode, 3, {0, 1, 2},
  { "Autodetect : hw YUV overlays / normal blits", "Force YUV overlays (can be slow!)",
    "Force normal blits"} },
{ "Fullscreen", &my_toggle_fullscreen, &display_cfg.fullscreen, 2, {0, 1}, {"No", "Yes"}},
{ "Use double buffer", NULL, &display_cfg.double_buffer, 3, {0, 1, 2}, {"Never", "When possible", "Even with overlays" } },
{ "Video info...", &do_video_info, },
{ "YUV overlays:" },
{ "Prefered YUV format", NULL, &prefered_yuv_format, 2, { 0, 1 }, { "YUY2", "YV12 (mpeg)" } },
{ "Fix aspect ratio to 4:3", NULL, (int*)&display_cfg.fix_aspect_ratio, 2, {0,1}, {"No", "Yes" } },
{ "Normal blits:" },
{ "Change video mode", NULL, (int*)&display_cfg.auto_mode_change, 3, {0, 1, 2},
  { "Never", "To match game resolution (low res most of the time)", "To match 2x game resolution" } },
{ "Scaler", &update_scaler, (int*)&display_cfg.stretch, 4, { 0, 1, 2, 3 }, 
  { "None", "Scale2x/3x", "Pixel double", "hq2x/3x" } },
{ "Scanlines", &update_scaler, (int*)&display_cfg.scanlines, 4, { 0, 1, 2, 3 },
  { "Off", "Halfheight", "Fullheight", "Fullheight + Double width" } },
{ "General options:" },
{ "Limit framerate <= 60fps", NULL, (int*)&display_cfg.limit_speed, 2, {0, 1}, {"No","Yes"} },
{ "Frame skip", NULL, (int*)&display_cfg.frame_skip, 10, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
  {"Auto", "1", "2", "3", "4", "5", "6", "7", "8", "9" } },
{ "Screen rotation", NULL, (int*)&display_cfg.user_rotate, 4, {0, 1, 2, 3 },
  { "None", "90°", "180°", "270°" } },
{ "Flip screen", NULL, (int*)&display_cfg.user_flip, 4, {0, 1, 2, 3 },
  { "None", "Flip X", "Flip Y", "Flip XY" } },
{  NULL },
};

int do_video_options(int sel) {
  video_options = new TVideo("Video options", video_items);
  video_options->execute();
  SetupScreenBitmap();
  if ((sdl_overlay != NULL || display_cfg.video_mode == 1) &&
        display_cfg.video_mode != 2) {
    // explicitely clear scanlines when overlays are enabled
    display_cfg.scanlines = 0;
  }
  if (display_cfg.stretch == 3 && sdl_game_bitmap) {
    DestroyScreenBitmap(); // init hq2x/3x, switch gamebitmap to 16bpp
    // + recall InitLUTs by recreating game_bitmap when enabling hq2x/3x
  }
  return 0;
}

