#include "sdl/gui/menu.h"
#include "sdl/compat.h"
#include "sdl/gui/tfont.h"
#include "sdl/dialogs/colors.h"
#include "str_opaque.h"
#include "control.h"
#include "control_internal.h"
#include "confile.h"
#include "games/neogeo.h"

extern int do_dirs(int sel); // dirs.cpp
static menu_item_t gui_menu[14];
extern int add_gui_options(menu_item_t *menu);
extern int add_fonts_gui_options(menu_item_t *menu);
TMenu *gui_options;
static int neogeo_bios;

static menu_item_t color_menu[] =
{
{ "Colors...", &do_colors, },
{ "Opaque HUD", NULL, &opaque_hud, 2, { 0, 1 }, { "No", "Yes" } },
{ "Pause when focus lost", NULL, &pause_on_focus, 2, { 0, 1 }, { "No", "Yes" }}
};

static char *neo_names[] =
{
    "Europe MVS (Ver. 2)",
    "Europe MVS (Ver. 1)",
    "US MVS (Ver. 2?)",
    "US MVS (Ver. 1)",
    "Asia MVS (Ver. 3)",
    "Japan MVS (Ver. 3)",
    "Japan MVS (Ver. 2)",
    "Japan MVS (Ver. 1)",
    "NEO-MVH MV1C",
    "Japan MVS (J3)",
    "Custom Japanese Hotel",
    "Universe Bios (Hack, Ver. 3.0)",
    "Universe Bios (Hack, Ver. 2.3)",
    "Universe Bios (Hack, Ver. 2.2)",
    "Universe Bios (Hack, Ver. 2.1)",
    "Universe Bios (Hack, Ver. 2.0)",
    "Universe Bios (Hack, Ver. 1.3)",
    "Universe Bios (Hack, Ver. 1.2)",
    "Universe Bios (Hack, Ver. 1.1)",
    "Universe Bios (Hack, Ver. 1.0)",
    // hacks
    "Debug MVS (Hack?)",
    "Asia AES",
    "Japan AES",
};

static int select_bios(int sel);

static int choose_bios(int sel) {
    int size = sizeof(neo_names)/sizeof(char*)+1;
    menu_item_t *menu = (menu_item_t*)malloc(size*sizeof(menu_item_t));
    memset(menu,0,size*sizeof(menu_item_t));
    int n;
    size--;
    for (n=0; n<size; n++) {
	menu[n].label = neo_names[n];
	if (check_bios_presence(n)) {
	    menu[n].menu_func = &select_bios;
	}
    }
    TMenu *load = new TMenu("Neogeo bios",menu);
    load->execute();
    delete load;
    free(menu);
    printf("bios %d\n",neogeo_bios);
    return 0;
}

static char label[100];

static int select_bios(int sel) {
    neogeo_bios = sel;
    sprintf(label,"Neo-geo bios : %s",neo_names[neogeo_bios]);
    gui_menu[0].label = (const char*)label;
    set_neogeo_bios(sel);
    printf("select_bios: %d\n",sel);
    return 1;
}

int do_gui_options(int sel) {
  int nb = 0;
  gui_menu[nb].label = "Neo-geo bios";
  gui_menu[nb++].menu_func = &choose_bios;
    select_bios(neogeo_bios);
  nb += add_fonts_gui_options(&gui_menu[nb]);
  nb += add_gui_options(&gui_menu[nb]);
  nb += add_menu_options(&gui_menu[nb]);
  gui_menu[nb++] = color_menu[0];
  gui_menu[nb++] = color_menu[1];
  gui_menu[nb++] = color_menu[2];
  gui_menu[nb].label = "Directories...";
  gui_menu[nb].menu_func = &do_dirs;
  gui_options = new TMenu("Options", gui_menu);
  gui_options->execute();
  delete gui_options;

  return 0;
}

void read_config_neogeo() {
  neogeo_bios = raine_get_config_int("neogeo","bios",0);
  set_neogeo_bios(neogeo_bios);
}

void save_config_neogeo() {
  raine_set_config_int("neogeo","bios",neogeo_bios);
}

