#include "../gui/menu.h"
#include "deftypes.h"
#include "neocd/neocd.h"
#include "neocd/cdrom.h"
#include "neocd/cdda.h"
#include "sdl/dialogs/fsel.h"
#include "raine.h"
#include "SDL_image.h"

int exit_to_code = 2;
static int exit_to(int sel) {
  set_neocd_exit_to(exit_to_code);
  return 0;
}

static int do_update_block(int sel) {
  char fdir[1024];
  sprintf(fdir,"%s",dir_cfg.screen_dir);
  char picture[256];
  *picture = 0;
  char *exts[] = { "block*png", NULL };
  fsel(fdir,exts,picture,"Update block");
  if (picture[strlen(picture)-1] != SLASH[0]) {
    char map[256];
    char *name = strrchr(picture,'/');
    SDL_Surface *img = IMG_Load(picture);
    if (img->format->BitsPerPixel != 8) {
      MessageBox("Error","This picture isn't 8bpp");
      SDL_FreeSurface(img);
      return 0;
    }
    int pitch = img->pitch;
    if (!name) return 0;
    sprintf(map,"%ssavedata" SLASH "%s",dir_cfg.exe_path,name);
    strcpy(&map[strlen(map)-3],"map");
    FILE *fmap = fopen(map,"rb");
    if (!fmap) {
      char error[256];
      sprintf(error,"no map %s",map);
      MessageBox("Error",error);
      return 0;
    }
    char last_name[30];
    *last_name = 0;
    FILE *fspr = NULL;
    while (!feof(fmap)) {
      int x,y,tileno,flip;
      char spr[30];
      fscanf(fmap,"%d,%d,%x,%d,%s\n",&x,&y,&tileno,&flip,spr);
      if (!*last_name || strcmp(last_name,spr)) {
	strcpy(last_name,spr);
	char path[256];
	sprintf(path,"%soverride" SLASH "%s",dir_cfg.exe_path,spr);
	if (fspr) fclose(fspr);
	fspr = fopen(path,"rb+");
	if (!fspr) {
	  char error[128];
	  sprintf(error,"could not open %s",path);
	  MessageBox("Error",path);
	  break;
	}
	printf("%s:\n",spr);
      }
      printf("%x\n",tileno);
      fseek(fspr,tileno*256,SEEK_SET);
      UINT8 *pixels = ((UINT8*)img->pixels)+y*pitch+x;
      for (y=0; y<16; y++) {
	fwrite(pixels,1,16,fspr);
	pixels += pitch;
      }
    }
    fclose(fmap);
    if (fspr) fclose(fspr);
    SDL_FreeSurface(img);
  }
  return 0;
}

menu_item_t neocd_menu[] =
{
{ "Exit to", &exit_to, &exit_to_code, 4, {0, 2, 5, 6},
  {"NeoGeo Logo", "CD Interface", "Test mode", "Config mode" } },
  { "Loading animations speed", NULL, &cdrom_speed, 8, { 0, 1, 2, 4, 8, 16, 32, 48 },
    { "Off", "CD 1x (150 Kb/s)", "2x", "4x", "8x", "16x", "32x", "48x" } },
  { "Update neocd sprite block", &do_update_block },
  { "Allow speed hacks", NULL, &allowed_speed_hacks, 2, { 0, 1 }, { "No", "Yes" }},
  { NULL },
};

int do_neocd_options(int sel) {
  TMenu *menu = new TMenu("Neocd options", neocd_menu);
  menu->execute();
  delete menu;
  return 0;
}
