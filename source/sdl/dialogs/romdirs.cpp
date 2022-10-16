#include "raine.h"
#include "../gui/menu.h"
#include "../compat.h"
#include "dialogs/messagebox.h"
#include "dialogs/fsel.h"
#include "romdir.h"
#include "games.h"

static int end;

static int do_dir(int sel) {
  sel--;
  int ret = MessageBox(dir_cfg.rom_dir[sel],_("Directory options"),_("Change directory|Remove directory"));
  if (ret == 1) {
    char res[256];
    dsel(dir_cfg.rom_dir[sel],NULL,res,"ROM directories");
    free(dir_cfg.rom_dir[sel]);
    dir_cfg.rom_dir[sel] = strdup(res);
    update_cache(sel);
  } else if (ret == 2) // remove
    remove_rom_dir(sel);
  end = 0;
  return 1;
}

static int add_dir(int sel) {
  char res[256];
  if (dir_cfg.rom_dir[0])
    strcpy(res,dir_cfg.rom_dir[0]);
  else
    res[0] = 0;
  dsel(res,NULL,res,"ROM directories");
  add_rom_dir(res);
  end = 0;
  return 1;
}

int do_romdir(int sel) {
  int nb;
  menu_item_t *menu;
  do {
    end = 1;
    for (nb=0; dir_cfg.rom_dir[nb]; nb++);
    menu = (menu_item_t*)malloc(sizeof(menu_item_t)*(nb+2));
    memset(menu,0,sizeof(menu_item_t)*(nb+2));
    for (int n=0; n<nb; n++) {
      menu[n+1].label = dir_cfg.rom_dir[n];
      menu[n+1].menu_func = &do_dir;
    }
    menu[0].label = _("Add ROM directory...");
    menu[0].menu_func = &add_dir;

    TMenu *mbox = new TMenu(_("ROM directories"),menu);
    mbox->execute();
    delete mbox;
    free(menu);
  } while (!end);
  return 0;
}
