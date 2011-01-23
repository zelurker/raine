#include "sdl/gui/menu.h"
#include "sdl/compat.h"
#include "sdl/gui/tfont.h"
#include "sdl/dialogs/colors.h"
#include "str_opaque.h"

extern int do_dirs(int sel); // dirs.cpp
static menu_item_t gui_menu[13];
extern int add_gui_options(menu_item_t *menu);
extern int add_fonts_gui_options(menu_item_t *menu);
TMenu *gui_options;
static menu_item_t color_menu[] =
{
{ "Colors...", &do_colors, },
{ "Opaque HUD", NULL, &opaque_hud, 2, { 0, 1 }, { "No", "Yes" } }
};

int do_gui_options(int sel) {
  int nb = 0;
  nb += add_fonts_gui_options(&gui_menu[nb]);
  nb += add_gui_options(&gui_menu[nb]);
  nb += add_menu_options(&gui_menu[nb]);
  gui_menu[nb++] = color_menu[0];
  gui_menu[nb++] = color_menu[1];
  gui_menu[nb].label = "Directories...";
  gui_menu[nb].menu_func = &do_dirs;
  gui_options = new TMenu("GUI options", gui_menu);
  gui_options->execute();
  delete gui_options;

  return 0;
}

