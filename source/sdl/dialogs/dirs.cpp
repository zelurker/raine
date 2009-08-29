#include "raine.h"
#include "../gui/menu.h"
#include "../compat.h"
#include "sdl/dialogs/messagebox.h"
#include "sdl/dialogs/fsel.h"

static menu_item_t *menu;

static int do_dir(int sel) {
  char res[256];
  dsel(menu[sel].values_list_label[0],NULL,res,(char*)menu[sel].label);
  strcpy(menu[sel].values_list_label[0], res);
  return 0;
}

int do_dirs(int sel) {
  int nb = 3;
  int bidon = 1;
  int n;
  menu = (menu_item_t*)malloc(sizeof(menu_item_t)*(nb+2));
  memset(menu,0,sizeof(menu_item_t)*(nb+2));
  menu[0].label = "Screenshot dir";
  menu[0].values_list_label[0] = dir_cfg.screen_dir;

#ifndef NEO
  menu[1].label = "Emudx dir";
  menu[1].values_list_label[0] = dir_cfg.emudx_dir;

  menu[2].label = "Artwork dir";
  menu[2].values_list_label[0] = dir_cfg.artwork_dir;
#endif

  for (n=0; n< nb; n++) {
    menu[n].value_int = &bidon;
    menu[n].values_list_size = 1;
    menu[n].values_list[0] = bidon;
    menu[n].menu_func = &do_dir;
  }
  TMenu *mbox = new TMenu("Directories",menu);
  mbox->execute();
  delete mbox;
  free(menu);
  return 0;
}


