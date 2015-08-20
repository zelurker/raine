#include "sdl/gui/menu.h"
#include "sdl/compat.h"
#include "sdl/gui/tfont.h"
#include "sdl/dialogs/colors.h"
#include "str_opaque.h"
#include "control.h"
#include "control_internal.h"
#include "confile.h"

extern int do_dirs(int sel); // dirs.cpp
static int lang_int;

static int set_lang(int sel) {
    char *locale;
    switch(lang_int) {
    case 1: locale = "fr_FR.UTF-8"; break;
    case 2: locale = "es_ES.UTF-8"; break;
    default: locale = "C"; break;
    }
    setenv("LANG",locale,1);
    setlocale(LC_ALL & ~LC_NUMERIC,locale);
    return 0;
}

static menu_item_t gui_menu[14] =
{
{  _("Language"), &set_lang, &lang_int, 3, {0, 1, 2 }, { "Default (english)", "French","Spanish"} },
};

extern int add_gui_options(menu_item_t *menu);
extern int add_fonts_gui_options(menu_item_t *menu);
TMenu *gui_options;

static menu_item_t color_menu[] =
{
{ _("Colors..."), &do_colors, },
{ _("Opaque HUD"), NULL, &opaque_hud, 2, { 0, 1 }, { _("No"), _("Yes") } },
{ _("Pause when focus lost"), NULL, &pause_on_focus, 2, { 0, 1 }, { _("No"), _("Yes") }}
};

int do_gui_options(int sel) {
  int nb = 1;
  char *lang = getenv("LANG");
  if (strstr(lang,"fr")) lang_int = 1;
  else if (strstr(lang,"es")) lang_int = 2;

  nb += add_fonts_gui_options(&gui_menu[nb]);
  nb += add_gui_options(&gui_menu[nb]);
  nb += add_menu_options(&gui_menu[nb]);
  gui_menu[nb++] = color_menu[0];
  gui_menu[nb++] = color_menu[1];
  gui_menu[nb++] = color_menu[2];
  gui_menu[nb].label = _("Directories...");
  gui_menu[nb].menu_func = &do_dirs;
  gui_options = new TMenu(_("Options"), gui_menu);
  gui_options->execute();
  delete gui_options;

  if (strcmp(lang,getenv("LANG")))
      return 1; // redraw main menu if locale changes
  return 0;
}

