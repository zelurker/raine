#include "gui/menu.h"
#include "compat.h"
#include "gui/tfont.h"
#include "dialogs/colors.h"
#include "str_opaque.h"
#include "control.h"
#include "control_internal.h"
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include "sdl/dialogs/messagebox.h"
#include "sdl/dialogs/fsel.h"
#include "leds.h"

extern int do_dirs(int sel); // dirs.cpp
static int lang_int;
extern "C" void init_lang(); // raine.c

static int set_lang(int sel) {
    char *locale;
    switch(lang_int) {
    case 1: locale = "fr"; break;
    case 2: locale = "es"; break;
    case 3: locale = "it"; break;
    case 4: locale = "pt"; break;
    default: locale = "C"; break;
    }
    strcpy(language,locale);
    init_lang();
    return 0;
}

#define MAX_GUI 19

static menu_item_t gui_menu[MAX_GUI] =
{
{  _("Language"), &set_lang, &lang_int, 5, {0, 1, 2, 3, 4 }, { _("Default (English)"), _("French"),_("Spanish"),_("Italian"), _("Brazilian Portuguese")} },
};

extern int add_gui_options(menu_item_t *menu);
extern int add_fonts_gui_options(menu_item_t *menu);
TMenu *gui_options;

static menu_item_t color_menu[] =
{
{ _("Colors..."), &do_colors, },
{ _("No HUD messages"), NULL, &silent_hud, 2, {0, 1}, { _("No"), _("Yes") } },
{ _("Opaque HUD"), NULL, &opaque_hud, 2, { 0, 1 }, { _("No"), _("Yes") } },
{ _("Pause when focus lost"), NULL, &pause_on_focus, 2, { 0, 1 }, { _("No"), _("Yes") }}
};

#ifdef RAINE_UNIX
static menu_item_t *menu;
static int my_led,end; // end is a trick to force a redraw of the "leds assignments" dialog when 1 changes

static int my_init_led(int sel) {
    init_led(my_led,menu[sel].label);
    end = 0;
    return 1;
}

static int do_led_paths(int sel) {
    my_led = sel;
    DIR *dir = opendir("/sys/class/leds");
    if (!dir)
	return 0;
    int nb_menu = 10;
    menu = (menu_item_t *)malloc(sizeof(menu_item_t)*(nb_menu+1));
    memset(menu,0,sizeof(menu_item_t)*(nb_menu+1));
    int nb_files = 0;
    struct dirent *dent;
    while ((dent = readdir(dir))) {
	if (!strcmp(dent->d_name,".") || !strcmp(dent->d_name,".."))
	    continue;
	char tmp[FILENAME_MAX];
	snprintf(tmp,FILENAME_MAX,"/sys/class/leds/%s/brightness",dent->d_name);
	if (access(tmp,W_OK))
	    continue;

	menu[nb_files].label = strdup(dent->d_name);
	menu[nb_files++].menu_func = &my_init_led;

	if (nb_files == nb_menu) {
	    nb_menu += 10;
	    menu = (menu_item_t *)realloc(menu,sizeof(menu_item_t)*(nb_menu+1));
	    memset(&menu[nb_files],0,sizeof(menu_item_t)*11);
	    if (!menu) {
		fatal_error("Failed to reallocate files buffer (%d entries)",nb_menu);
	    }
	}
    }
    if (nb_files == 0) {
	MessageBox("Error","No write permission to any brightness file\nin /sys/class/leds","ok");
	return 0;
    }
    qsort(&menu[0],nb_files,sizeof(menu_item_t),&qsort_menu);

    TMenu *dlg = new TMenu("LEDs",menu);
    dlg->execute();
    delete dlg;

    for (int n=0; n<nb_files; n++)
	free(menu[n].label);
    free(menu);

    end = 0;
    return 1;
}

int do_leds(int sel) {
    int nb_menu = MAX_LEDS + 1;
    do {
	end = 1;
	menu_item_t *menu = (menu_item_t *)malloc(sizeof(menu_item_t)*(nb_menu+1));
	memset(menu,0,sizeof(menu_item_t)*(nb_menu+1));
	for (int n=0; n<MAX_LEDS; n++) {
	    char tmp[80];
	    char *s = get_led_name(n);
	    if (!s)
		snprintf(tmp,80,"led%d none",n);
	    else {
		snprintf(tmp,80,"led%d %s",n,s);
		free(s);
	    }
	    menu[n].label = strdup(tmp);
	    menu[n].menu_func = &do_led_paths;
	}

	TMenu *dlg = new TMenu(_("LEDs assignments"),menu);
	dlg->execute();
	delete dlg;

	for (int n=0; n<MAX_LEDS; n++)
	    free(menu[n].label);
	free(menu);
    } while (!end);
    return 0;
}
#endif

int do_gui_options(int sel) {
  int nb = 1;
  if (strstr(language,"fr")) lang_int = 1;
  else if (strstr(language,"es")) lang_int = 2;
  else if (strstr(language,"it")) lang_int = 3;
  else if (strstr(language,"pt")) lang_int = 4;
  else {
      lang_int = 0;
      printf("Unknown lang %s\n",language);
  }
  char old_lang[6];
  strcpy(old_lang,language);

  nb += add_fonts_gui_options(&gui_menu[nb]);
  nb += add_gui_options(&gui_menu[nb]);
  nb += add_menu_options(&gui_menu[nb]);
  gui_menu[nb++] = color_menu[0];
  gui_menu[nb++] = color_menu[1];
  gui_menu[nb++] = color_menu[2];
  gui_menu[nb++] = color_menu[3];
  gui_menu[nb].label = _("Directories...");
  gui_menu[nb].menu_func = &do_dirs;
#ifdef RAINE_UNIX
  gui_menu[++nb].label = _("LEDs...");
  gui_menu[nb].menu_func = &do_leds;
#endif
  if (nb >= MAX_GUI-1) { // -1 because we need a NULL at the end
      fatal_error("Too many GUI options!");
  }
  gui_options = new TMenu("", gui_menu);
  gui_options->execute();
  delete gui_options;

  if (strcmp(language,old_lang)) {
      return 1; // redraw main menu if locale changes
  }
  return 0;
}

