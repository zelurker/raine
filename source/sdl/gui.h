#ifndef GUI_H
#define GUI_H

#ifdef __cplusplus
#include "sdl/gui/menu.h"

class TMain_menu : public TMenu
{
  public:
  TMain_menu(char *my_title, menu_item_t *mymenu) :
    TMenu(my_title,mymenu)
    {}
  int can_be_displayed(int n);
  char *get_bot_frame_text();
  void draw_top_frame();
  char* get_top_string();
};

extern "C" {
#endif

extern int goto_debuger;

void read_gui_config();
void write_gui_config();
void setup_font();
void load_progress(char *rom,int count);
void curl_progress_f(int count);
void setup_curl_dlg(char *name);
void load_message(char *msg);
void StartGUI(void);
#ifdef __cplusplus
}
#endif
#endif

