#include "gui/menu.h"
#include "arpro.h"
#include "compat.h"
#include "SDL_gfx/SDL_gfxPrimitives.h"
#include "dialogs/cheats.h"
#ifdef HAS_CONSOLE
#include "console/scripts.h"
#endif
#include "dialogs/messagebox.h"

#if SDL==2
#define boxColor(sf,x,y,w,h,col) boxColor(rend,x,y,w,h,col)
#define bg_frame bg_frame_gfx
#endif

class TCheatDlg : public TMenu
{
  public:
    TCheatDlg(char *title, menu_item_t *menu) : TMenu(title,menu)
    {}
#if SDL == 1
    void disp_menu(int n,int y,int w,int h) {
      TMenu::disp_menu(n,y,w,h);
      draw_bot_frame();
    }
#endif
    char *get_cheat_info() {
      static char cheat_info[512];
      if (sel < CheatCount) {
	snprintf(cheat_info,512,"Info: %s",CheatList[sel].info);
      } else {
	snprintf(cheat_info,512,"%s",get_script_comment(sel));
      }
      cheat_info[511] = 0;
      return cheat_info;
    }
    void handle_mouse(SDL_Event *event) {
	TMenu::handle_mouse(event);
	if (event->type == SDL_MOUSEBUTTONDOWN) {
	    char *cheat_info = get_cheat_info();
	    int base = work_area.y+work_area.h;
	    int w=0,h=0;
	    if (font) font->dimensions(cheat_info,&w,&h);
	    if (w > sdl_screen->w || h > sdl_screen->h-base) {
		if (event->button.y > base)
		    raine_mbox("info",cheat_info,"OK");
	    }
	}
    }
    virtual void draw_bot_frame() {
      int base = work_area.y+work_area.h;
      char *cheat_info = get_cheat_info();
      int w=0,h=0;
      if (font) font->dimensions(cheat_info,&w,&h);
      char *b;
      // The display here is on 1 line, so filter out the \n
      while ((b = strstr(cheat_info,"\n")))
	  *b = ' ';
      if (w > sdl_screen->w || h > sdl_screen->h-base) {
	  // Make the bottom frame to "flash" when text is too long
	  static int frame,inc;
	  if (!inc) inc = 1;

#if SDL==2
	  // Color mapping seems to be abgr with sdl2, rgba with sdl1 !
	  int r = bg_frame & 255, g = (bg_frame >> 8) & 255, b = (bg_frame >> 16) & 255;
#else
	  int r = bg_frame >> 24, g = (bg_frame >> 16) & 255, b = (bg_frame >> 8) & 255;
#endif
	  r = (30-frame)*r/30;
	  g = (30-frame)*g/30;
	  b = (30-frame)*b/30;
	  frame += inc;
	  if (frame <= 0) {
	      frame = 0;
	      inc = 1;
	  } else if (frame >= 30) {
	      frame = 30;
	      inc = -1;
	  }
	  boxColor(sdl_screen,0,base,sdl_screen->w,sdl_screen->h,mymakecol(r,g,b));
      } else
	  boxColor(sdl_screen,0,base,sdl_screen->w,sdl_screen->h,bg_frame);
      font->put_string(HMARGIN,base,cheat_info,fg_frame,0);
#if SDL < 2
      SDL_Rect area;
      area.x = 0; area.y = base; area.w = sdl_screen->w; area.h = sdl_screen->h-base;
      do_update(&area);
#endif
    }
    int get_max_bot_frame_dimensions(int &w, int &h) {
      unsigned int max = 0, maxnb = -1;
      for (int n=0; n<CheatCount; n++) {
	if (strlen(CheatList[n].info) > max) {
	  max = strlen(CheatList[n].info);
	  maxnb = n;
	}
      }
      char cheat_info[266];
#ifdef HAS_CONSOLE
      if (CheatCount < 0 && nb_scripts)
	sprintf(cheat_info,"Script");
      else
#endif
	sprintf(cheat_info,"Info: %s",(max > 0 ? CheatList[maxnb].info : ""));
      if (font) font->dimensions(cheat_info,&w,&h);
      return strlen(cheat_info);
    }
};

int adjust_cheat(int sel) {
  int val = CheatList[sel].edit_value;
  if (CheatList[sel].type & CHEAT_EDIT_BASE)
    val--;
  if (CheatList[sel].type & CHEAT_EDIT_BCD)
    val = ((val/10)<<4) + (val%10);
  CheatList[sel].edit_value = val;
  return 0;
}

int do_cheats(int sel) {
  int size = sizeof(menu_item_t)*(CheatCount+
#ifdef HAS_CONSOLE
	nb_scripts+1
#else
	1
#endif
);
  menu_item_t *menu = (menu_item_t*)malloc(size);
  memset(menu,0,size);
  if (!CheatCount
#ifdef HAS_CONSOLE
		  && !nb_scripts
#endif
     ) return 0;

  for (int n=0; n<CheatCount; n++) {
    menu[n].label = CheatList[n].name;
    if (CheatList[n].type & CHEAT_EDITABLE) {
      char text[10];
      int val;
      get_cheat_hex_value(n,text);
      sscanf(text,"%x",&val);
      CheatList[n].old_value = val;
      menu[n].value_int = (int*)&CheatList[n].edit_value;
      if (CheatList[n].type & CHEAT_EDIT_BCD)
	val = ((val >> 4)*10)+(val & 0xf);
      if (CheatList[n].type & CHEAT_EDIT_BASE)
	val++;
      CheatList[n].edit_value = val;
      menu[n].values_list_size = 3;
      menu[n].values_list[0] = CheatList[n].mask; // inf
      menu[n].values_list[1] = CheatList[n].data; // supp
      menu[n].values_list[2] = 1;
    } else {
      menu[n].value_int = (int*)&CheatList[n].active;
      menu[n].values_list_size = 2;
      menu[n].values_list[0] = 0;
      menu[n].values_list[1] = 1;
      menu[n].values_list_label[0] = _("Off");
      menu[n].values_list_label[1] = _("On");
    }
  }
#ifdef HAS_CONSOLE
  add_scripts(&menu[CheatCount]);
#endif
  TCheatDlg *cheats = new TCheatDlg("",menu);
  cheats->execute();
  delete cheats;
  for (int n=0; n<CheatCount; n++) {
    if (CheatList[n].type & CHEAT_EDITABLE) {
      adjust_cheat(n);
      if (CheatList[n].edit_value != CheatList[n].old_value) {
	CheatList[n].active = 1;
      } else
        CheatList[n].active = 0;
    }
  }
  free(menu);
  return 0;
}

