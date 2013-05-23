#include "../gui/menu.h"
#include "video_info.h"
#include "sdl/compat.h"
#include "blit.h"
#include "sdl/blit_sdl.h"
#include "sdl/display_sdl.h"
#include "raine.h"

static menu_item_t video_info_menu[20];

static int video_mode = 0,status[11];
static char video_mode_label[20],info[11][60];

static void print_tf_state(int n, char *label, int test) {
  video_info_menu[n].label = label;
  video_info_menu[n].value_int = &status[n];
  status[n] = test;
  if (test != 0 && test != 1) {
    printf("received test %d for %s\n",test,label);
    exit(1);
  }
  video_info_menu[n].values_list_size = 2;
  video_info_menu[n].values_list[1] = 1;
  video_info_menu[n].values_list_label[0] = "No";
  video_info_menu[n].values_list_label[1] = "Yes";
}

static void print_menu_string(int n, char *label, char *s) {
  video_info_menu[n].label = label;
  video_info_menu[n].value_int = &video_mode;
  video_info_menu[n].values_list_size = 1;
  video_info_menu[n].values_list_label[0] = info[n];
  strncpy(info[n],s,60);
  info[n][59] = 0;
}

class TVideo_menu : public TMenu
{
  public:
  TVideo_menu(char *my_title);
  virtual int can_be_selected(int n) {
    return 0;
  }
};

TVideo_menu::TVideo_menu(char *my_title) : TMenu(my_title,NULL) {
  char driver[128];

  memset(video_info_menu,0,sizeof(video_info_menu));
  sprintf(video_mode_label,"%dx%d, %d bpp",video_info->current_w, video_info->current_h, video_info->vfmt->BitsPerPixel);
  print_menu_string(0,"Current display",video_mode_label);
  menu = video_info_menu;
  if (display_cfg.video_mode == 0) {
      if (!ogl.info) {
	  print_menu_string(1,"No opengl info yet, run something first","");
	  video_info_menu[2].label = NULL;
	  return;
      }
      print_menu_string(1,"OpenGL vendor",ogl.vendor);
      print_menu_string(2,"Renderer",ogl.renderer);
      print_menu_string(3,"version",ogl.version);
      print_tf_state(4,"OpenGL double buffer", ogl.infos.dbuf);
      print_tf_state(5,"fsaa (multisampling)", ogl.infos.fsaa_buffers);
      print_tf_state(6,"Acceleration", ogl.infos.accel);
      print_tf_state(7,"OpenGL VBL (force sync)", ogl.infos.vbl);
      video_info_menu[8].label = NULL;
      return;
  }
  if ( SDL_VideoDriverName(driver, sizeof(driver)) ) {
    print_menu_string(1,"Video driver name",driver);
  }

  print_tf_state(2,"hardware surface available", video_info->hw_available);
  print_tf_state(3,"window manager available", video_info->wm_available);
  print_tf_state(4,"accelerated hardware->hardware blits", video_info->blit_hw);
  print_tf_state(5,"accelerated hardware->hardware colorkey blits", video_info->blit_hw_CC);
  print_tf_state(6,"accelerated hardware->hardware alpha blits", video_info->blit_hw_A);
  print_tf_state(7,"accelerated software->hardware blits", video_info->blit_sw);
  print_tf_state(8,"accelerated software->hardware colorkey blits", video_info->blit_sw_CC);
  print_tf_state(9,"accelerated software->hardware alpha blits", video_info->blit_sw_A);
  print_tf_state(10,"accelerated color fills", video_info->blit_fill);
  print_tf_state(11,"Double buffer enabled", (screen_flags & SDL_DOUBLEBUF ? 1 : 0));
  if (sdl_overlay && sdl_overlay->hw_overlay) {
    if (sdl_overlay->format == SDL_YV12_OVERLAY)
      video_info_menu[12].label = "Currently using hardware accelerated YV12 overlay";
    else if (sdl_overlay->format == SDL_YUY2_OVERLAY)
      video_info_menu[12].label = "Currently using hardware accelerated YUY2 overlay";
  } else if (use_scale2x == 1)
    video_info_menu[12].label = "Currently using scale2x";
  else if (use_scale2x == 2)
    video_info_menu[12].label = "Currently using scale3x";
  else
    video_info_menu[12].label = "Using unscaled blits";

}

int do_video_info(int sel) {
  TVideo_menu *video_menu = new TVideo_menu("Video info");
  video_menu->execute();
  delete video_menu;
  return 0;
}

