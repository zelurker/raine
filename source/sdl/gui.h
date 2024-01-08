#ifndef GUI_H
#define GUI_H

#ifdef __cplusplus
#include "sdl/gui/menu.h"
#include "display_sdl.h"

class TMain_menu : public TMenu
{
  public:
  TMain_menu(char *my_title, menu_item_t *mymenu) :
    TMenu(my_title,mymenu)
    {
#if SDL == 2
	desktop_w = desktop->w; desktop_h = desktop->h;
#endif
    }
  int can_be_displayed(int n);
  char *get_bot_frame_text();
  virtual char* get_top_string();
};

#if SDL == 2
class TRaineDesktop : public TDesktop
{
    private:
	int tgame;
    public:
	TRaineDesktop();
	void preinit();
	virtual void draw(TMenu *from = NULL);
	void end_preinit();
};

// A Raine desktop, which starts the game in the background as soon as it's loaded, but only until it's played
// or interrupted by a call to end_preinit()
#define rdesktop ((TRaineDesktop*)desktop)
#endif

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
int do_preload_ips(int sel);
#ifdef __cplusplus
}
#endif
#endif

