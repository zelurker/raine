#include "../gui/menu.h"
#include "raine.h" // display_cfg
#include "video_info.h"
#include "blit.h"
#include "sdl/blit_sdl.h"
#include "control.h"
#include "sdl/control_internal.h"
#include "display.h"
#include "sdl/display_sdl.h"
#include "sdl/dialogs/messagebox.h"
#include "sdl/dialogs/fsel.h"

class TVideo : public TMenu
{
  public:
  TVideo(char *my_title, menu_item_t *mymenu) :
    TMenu(my_title,mymenu)
    {}
};

static TVideo *video_options;

static int my_toggle_fullscreen(int sel) {
    // int oldx = display_cfg.screen_x, oldy = display_cfg.screen_y;
  display_cfg.fullscreen ^= 1;
  toggle_fullscreen();
  adjust_gui_resolution();
  video_options->draw();
  if (!(sdl_screen->flags & SDL_DOUBLEBUF) && !emulate_mouse_cursor)
    SDL_ShowCursor(SDL_ENABLE);
  clear_raine_screen();
  video_options->draw();
  return 0; // (oldx < display_cfg.screen_x || oldy < display_cfg.screen_y);
}
  
static int my_toggle_border(int sel) {
    ScreenChange();
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

static menu_item_t overlays_options[] =
{
    { "Prefered YUV format", NULL, &prefered_yuv_format, 2, { 0, 1 }, { "YUY2", "YV12 (mpeg)" } },
    { "Fix aspect ratio to 4:3", NULL, (int*)&display_cfg.fix_aspect_ratio, 2, {0,1}, {"No", "Yes" } },
#ifdef DARWIN
    { "Overlays workarounds", NULL, &overlays_workarounds, 2, { 0, 1}, {"No","Yes"}},
#endif
    {  NULL },
};

static menu_item_t blits_options[] =
{
    { "Change video mode", NULL, (int*)&display_cfg.auto_mode_change, 3, {0, 1, 2},
	{ "Never", "To match game resolution (low res most of the time)", "To match 2x game resolution" } },
    { "Scaler", &update_scaler, (int*)&display_cfg.stretch, 4, { 0, 1, 2, 3 }, 
	{ "None", "Scale2x/3x", "Pixel double", "hq2x/3x" } },
    { "Scanlines", &update_scaler, (int*)&display_cfg.scanlines, 4, { 0, 1, 2, 3 },
	{ "Off", "Halfheight", "Fullheight", "Fullheight + Double width" } },
    {  NULL },
};

static void preinit_ogl_options();

static int choose_shader(int sel) {
    // This part is a little tricky because I want the fileselector when I
    // select this, but I don't want the whole path to be displayed, just
    // the filename selected alone. Tricky, but not long... !
    char *exts[] = { "*.shader", NULL };
    char dir[PATH_MAX];
    strcpy(dir,dir_cfg.shader_dir);
    if (!strcmp(ogl.shader,"None"))
	*ogl.shader = 0;
    else {
	strcpy(dir,ogl.shader);
	char *p = strrchr(dir,SLASH[0]);
	if (p) *p = 0;
    }
    fsel(dir,exts,ogl.shader,"Select shader");
    if (ogl.shader[strlen(ogl.shader)-1] == SLASH[0]) // cancelled ?
	strcpy(ogl.shader,"None");
    preinit_ogl_options();
    return 0;
}

static int bidon;

static menu_item_t ogl_options[] =
{
    { "Rendering", NULL, &ogl.render, 2, { 0, 1 }, { "DrawPixels (no shaders)", "Texture (possible shaders)" }, },
    { "Double buffer", NULL, &ogl.dbuf, 2, { 0, 1 }, {"No","Yes"} },
    { "Save opengl screenshots", NULL, &ogl.save, 2, {0, 1}, {"No", "Yes"} },
    { "Shader", &choose_shader,&bidon,1,{0},{ogl.shader}}, 
    { "OpenGL overlay interface", NULL, &ogl.overlay, 2, { 0, 1 }, {"No","Yes"} },
    {  NULL },
};

static void preinit_ogl_options() {
    char *p = strrchr(ogl.shader,SLASH[0]);
    if (p) ogl_options[3].values_list_label[0] = p+1;
    else
	ogl_options[3].values_list_label[0] = "None";
}

int renderer_options(int sel) {
    TMenu *menu;
    preinit_ogl_options();
    switch(display_cfg.video_mode) {
    case 0: menu = new TDialog("OpenGL Options", ogl_options); break;
    case 1: menu = new TDialog("Overlays Options", overlays_options); break;
    case 2: menu = new TDialog("Blits Options", blits_options); break;
    default:
	    MessageBox("Error","No options for this renderer ? Strange !","OK");
	    return 0;
    }
    menu->set_transparency(0);
    menu->execute();
    delete menu;
    return 0;
}

static menu_item_t video_items[] =
{
#ifdef RAINE_WIN32
{  "SDL video driver", NULL, (int*)&display_cfg.video_driver, 3, {0, 1, 2},
  { "SDL default (windib since 1.2.10)", "windib (good for ogl)","directx (good for hw overlays/blits)"} },
#endif
{  "Video renderer", NULL, (int*)&display_cfg.video_mode, 3, {0, 1, 2},
  { "OpenGL", "YUV overlays","Normal blits"} },
{ "Fullscreen", &my_toggle_fullscreen, &display_cfg.fullscreen, 2, {0, 1}, {"No", "Yes"}},
{ "Borderless", &my_toggle_border, &display_cfg.noborder, 2, {0, 1}, {"No", "Yes"} },
{ "Use double buffer (ignored by opengl)", NULL, &display_cfg.double_buffer, 3, {0, 1, 2}, {"Never", "When possible", "Even with overlays" } },
{ "Video info...", &do_video_info, },
{ "Renderer options", &renderer_options },
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
    int old_stretch = display_cfg.stretch;
#ifdef RAINE_WIN32
    UINT32 old_driver = display_cfg.video_driver;
#endif
    // int oldx = display_cfg.screen_x,oldy = display_cfg.screen_y;
    video_options = new TVideo("Video options", video_items);
    video_options->execute();
#ifdef RAINE_WIN32
    if (old_driver != display_cfg.video_driver) {
	if (sdl_overlay) {
	    SDL_FreeYUVOverlay(sdl_overlay);
	    sdl_overlay = NULL;
	}
	sdl_screen = NULL;
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	setup_video_driver();
	SDL_InitSubSystem(SDL_INIT_VIDEO);
	ScreenChange();
    }
#endif
    SetupScreenBitmap();
    if ((sdl_overlay != NULL || display_cfg.video_mode == 1) &&
	    display_cfg.video_mode != 2) {
	// explicitely clear scanlines when overlays are enabled
	display_cfg.scanlines = 0;
    }
    if (old_stretch != display_cfg.stretch && display_cfg.stretch == 3 &&
	    sdl_game_bitmap) {
	DestroyScreenBitmap(); // init hq2x/3x, switch gamebitmap to 16bpp
	// + recall InitLUTs by recreating game_bitmap when enabling hq2x/3x
    }
    return 0; // (oldx != display_cfg.screen_x || oldy != display_cfg.screen_y);
}

