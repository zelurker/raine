/* Differences from the initial version for sdl-1.2 :
 *  - bg_layer is replaced by an external object, desktop which should allow more options for the background
 *  - fg_layer becomes a texture instead of an SDL_Surface
 *  - make the gui part more independant from raine (to be re-usable, and also to be able to test it without compiling everything !)
 *
 * That doesn't make many changes, but they have deep consequences... !
 * Here are the old comments to explain how the gui works :
 * The idea behind this one is to have something as easy to program as
 * possible (1st goal), usable with any peripheral (2nd), and also nice if
 * possible (3rd !) ;-)
 *
 * To achieve this easy programmation, I took some ideas from the guis on psp:
 * a basic "menu" on screen, you can move to any option, pressing a button
 * execute it, and moving right or left changes values in a list of values
 * if there is one. It's nice, easy to use, and easy to program. All this stuff
 * is handled by a very basic struct :
 *
 * typedef struct {
 *                const char *label;
 *                int (*menu_func)(int);
 *                int     *value_int;
 *                int     values_list_size;
 *                int values_list[16];
 *                char* values_list_label[16];
 * } menu_item_t;
 *
 * A menu is just a list of these structs, ended by an entry with label=NULL
 * To have something static, just pass a label and menu_func=value_int=NULL
 * To have a simple command, just pass a function in menu_func.
 * And to have list of values where to choose from, just pass an int and
 * some lists. Something to notice about these lists : contrary to what
 * allegro did, here you don't have to worry about list indexes. For example
 * if you want to choose the audio frequency you would put :
 * { "Audio frequency", NULL, &audio_sample_rate, 3, {11025,22050,44100}, NULL}
 * If you pass a menu_func != NULL with a value_int != NULL, then menu_func
 * is called everytime you change the value of value_int in the list.
 *
 * The end result is something which looks a little like the mame's menu system
 * but with quite some difference :
 * 1) it's nicer : transparency effects, true type support, bitmap handling,
 * flashing background for the selected option...
 * 2) it can be controlled with any device (mouse, keyboard, joystick...).
 * 3) it pauses the game when you call the gui ! ;-)
 *
 * Another advantage is that you don't have to care about coordinates or fonts
 * sizes. It's configured to adapt to anything you pass.
 *
 * The original psp implementation was in pure C. This one is in C++ to add
 * a few features more easily, like bitmap handling, and make the whole thing
 * even easier to use. This file contains the gory details, you shouldn't
 * have to look into it just to use the gui !
 *
 * The objects hierarchy :
 *
 * TMenu is the default container class. It displays menu_item entries one
 * under the other, with a frame (a status bar at the bottom + a title bar at
 * the top).
 *
 * TDialog is the same thing without frame. The goal of TDialog is to be able
 * to display a dialog on top of a menu (or another dialog) which is already on
 * scren.
 *
 * TMenuMultiCol displays a multi column dialog based on a double indexed array
 * of strings (char *cols[][]). It might have been possible to do it using
 * value_list_label from the menu_item_t struct, but at the time I wrote this
 * it looked like the easiest solution. Since it works well, I won't try to
 * rewrite it any time soon.
 *
 * TBitmap_menu is the way I found to have a bitmap in a dialog (as a header
 * here). This is the dialog which showed the most the need for some
 * extensions.
 *
 * Extensions : a pc being more complex than a psp, I had quickly a need for
 * extensions, that is widgets more complex than simple strings in dialogs.
 * But at the same time, I wanted to keep the simplicity of menu_item_t. This
 * is why I created the widget classes (see widget.cpp/widget.h for details) to
 * handle the widgets, as an array in TMenu (child).
 *
 * In menu_item_t an extension is identified by values_list_size < 0 in
 * menu_item_t (which should never happen normally with strings).
 * Depending on the value of this field, different types of widgets are created
 * to fill the child array and handle the menu_item entry.
 * The list of extensions is given and commented in menuitem.h
 */

#include "SDL_gfx/SDL_gfxPrimitives.h"
#include "video/palette.h"
#include "version.h"
#include "games.h"
#include "menu.h"
#include "tfont.h"
#include "widget.h"
#include "tslider.h"
#include "tedit.h"
#include "SDL_gfx/SDL_framerate.h"
#include "SDL_gfx/SDL_rotozoom.h"
#include <SDL_image.h>
#include "blit.h"
#include "dialogs/gui_options.h"
#include "sdl/gui/tlift.h"
#include "control.h"
#include "sdl/gui/tbitmap.h"
#include "sasound.h"
#include "neocd/neocd.h"
#include "newmem.h" // GetMemoryPoolSize
#ifdef RAINE_UNIX
#define NOGDI // define this before including windows.h to avoid al_bitmap !
#define GL_GLEXT_LEGACY // to try not to include glext.h which redefines the GL_GLEXT_VERSION which shouldn't have gone to SDL_opengl.h !
#include <SDL_opengl.h> // super annoying windows.h collides here !
#endif

int return_mandatory = 0, use_transparency = 1;
int keep_vga = 1,gui_level;

int repeat_interval, repeat_delay; // in gui.cpp

SDL_PixelFormat *fg_format;

TDesktop *desktop;

void disp_cursor(SDL_Surface *s,int x,int y, int w, int h) {
    // Problem is I don't see any way to make a simple xor cursor here...
    // so for now the work around is to make it slimer (width forced to 3),
    // since it's blinking it's very usable...
    // xor one later maybe...
    boxColor(rend,x,y,x+2,y+h-1,0xffffff80);
}

menu_item_t menu_options[] =
{
  { _("Return mandatory"), NULL, &return_mandatory, 2, { 0, 1 }, { _("No"), _("Yes") }},
  { _("GUI transparency"), NULL, &use_transparency, 2, { 0, 1 }, { _("No"), _("Yes") }},
  { _("Minimum GUI resolution = VGA"), NULL,&keep_vga, 2, { 0,1 }, { _("No"),_("Yes") }},
};

int fg_color = mymakecol(255,255,255),
    bg_color = makecol_alpha(0x11,0x07,0x78,128),
    fgframe_color = mymakecol(255,255,255),
    bgframe_color = mymakecol(0,0,128),
    cslider_border = mymakecol(0,0,0),
    cslider_bar = mymakecol(0xc0,0xc0,0xc0),
    cslider_lift = mymakecol(0xff,0xff,0xff),
    bg_dialog_bar = mymakecol(0,0,0),
    bg_dialog_bar_gfx = makecol_alpha(255,0,0,0);

int add_menu_options(menu_item_t *menu) {
  menu[0] = menu_options[0];
  menu[1] = menu_options[1];
  menu[2] = menu_options[2];
  return 3;
}

void sort_menu(menu_item_t *menu) {
  int n,ref,x;
  if (!menu)
    return;
  for (n=0; menu[n].label; n++) {
    if (menu[n].value_int && menu[n].values_list_size > 0 && menu[n].values_list_label[0]) {
      for (ref=0; ref<menu[n].values_list_size-1; ref++) {
	for (x=ref+1; x<menu[n].values_list_size; x++) {
	  if (strcmp(menu[n].values_list_label[ref],
	      menu[n].values_list_label[x]) > 0) {
	    char *s = menu[n].values_list_label[ref];
	    int nb = menu[n].values_list[ref];
	    menu[n].values_list_label[ref] = menu[n].values_list_label[x];
	    menu[n].values_list[ref] = menu[n].values_list[x];
	    menu[n].values_list_label[x] = s;
	    menu[n].values_list[x] = nb;
	  }
	}
      }
    }
  }
}

static SDL_Surface screen_info;

TDesktop::TDesktop() {
    SDL_GetRendererOutputSize(rend,&screen_info.w,&screen_info.h);
    sdl_screen = &screen_info;
    SDL_Rect usable;
    SDL_GetDisplayUsableBounds(0, &usable);
    w = usable.w; h = usable.h;
    pic = NULL;
    fg_format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
}

void TDesktop::draw() {
    SDL_SetRenderDrawColor(rend, 0x0, 0x0, 0x0, 0xFF);
    SDL_RenderClear(rend);
    if (pic) {
	if (work_area.w) {
	    // The SetLogicalSize can't move the origin, so it's not appropriate here
	    // and we have to make our dest rect
	    double ratio1 = work_area.w*1.0/picw;
	    double ratio2 = work_area.h*1.0/pich;
	    double ratio = MIN(ratio1,ratio2);
	    SDL_Rect dest;
	    int w = picw*ratio, h = pich*ratio;
	    dest.x = (work_area.w-w)/2+work_area.x;
	    dest.y = (work_area.h-h)/2+work_area.y;
	    dest.w = w;
	    dest.h = h;
	    SDL_RenderCopy(rend,pic,NULL,&dest);
	    return;
	}
	SDL_RenderSetLogicalSize(rend, picw,pich);
	SDL_RenderCopy(rend,pic,NULL,NULL);
	SDL_RenderSetLogicalSize(rend, 0,0);
	return;
    }
    if (work_area.w)
	SDL_RenderSetViewport(rend,&work_area);

    SDL_SetRenderDrawColor(rend, 0xff, 0xff, 0xff, 0xFF);
    static int count;
    int step;
    int hlimit;
    if (work_area.w)
	hlimit = work_area.h;
    else
	hlimit = sdl_screen->h;
    int w = sdl_screen->w-1, h = hlimit-1;
    float ratio = w*1.0/h;
    for (step=0; step<=10; step++) {
	int x1 = w*step/10+count*ratio;
	int y1 = h*step/10+count;
	SDL_RenderDrawLine(rend,x1,0,w,y1);
	SDL_RenderDrawLine(rend,w,y1,w-x1,h);
	SDL_RenderDrawLine(rend,w-x1,h,0,h-y1);
	SDL_RenderDrawLine(rend,0,h-y1,x1,0);
    }
    count++;
    if (count >= hlimit/10) count = 0;
    SDL_RenderSetViewport(rend,NULL);
}

int TDesktop::set_picture(const char *name) {
    if (pic) {
	SDL_DestroyTexture(pic);
	pic = NULL;
    }
    if (name) {
	pic = IMG_LoadTexture(rend,name);
	UINT32 access;
	int format;
	SDL_QueryTexture(pic,&access,&format,&picw,&pich);
	return pic != NULL;
    }
    return 0;
}

static TMenu *caller;
int (*window_event_hook)(SDL_Event *event);
static char* my_get_shared(char *s) {
    return s;
}

char * (*get_shared_hook)(char *name) = &my_get_shared;

TMenu::TMenu(char *my_title, menu_item_t *my_menu, char *myfont, int myfg, int mybg, int myfg_frame, int mybg_frame,int to_translate) {
    if (!desktop)
	desktop = new TDesktop();
    header = NULL;
    h_child = NULL;
    focus = 0;
  top = 0;
  sel = -1; // compute first selection later (compute_nb_items)
  child = NULL;
  title = my_title;
  menu = my_menu;
  translated = to_translate && menu;

  if (translated) {
      /* Translation problem :
       * when using _() in the menu_item_t definition, it's called before the
       * gettext environment is initialized and so it does nothing. The reason
       * to use it is to mark the text for translation in the .po files. The
       * solution is to translate everything here.
       * But now there is the problem of dialogs which allocate dynamically
       * their entries, even if they are a minority. To avoid to mess things
       * up I use a copy of the menu_item_t passed. The downside is that it
       * makes dynamic dialogs harder to handle, explicit calls are needed to
       * update the strings in the dialogs in this case.
       * The copy of the menu passed has another advantage : it allows to
       * change the translated language without restarting since the original
       * text is preserved... */
      /* Now this thing is really starting to become bothersome, but for now
       * I see no way to get back to the easy dyanmic dialogs handling while
       * keeping the translations ! */
      int nb = 0;
      while (menu->label) {
	  nb++;
	  menu++;
      }
      int sz = sizeof(menu_item_t)*(nb+1);
      menu = (menu_item_t *)malloc(sz);
      memcpy(menu,my_menu,sz);
      my_menu = menu;
      while (menu && menu->label) {
	  if (*menu->label) {
	      menu->label = gettext(menu->label);
	      for (int n=0; n<menu->values_list_size && menu->values_list_label[n]; n++)
		  menu->values_list_label[n] = gettext(menu->values_list_label[n]);
	  }
	  menu++;
      }
      menu = my_menu;
  }

  fg_layer = NULL;
  font = NULL;
  menu_disp = NULL;
  parent = caller;
  cycle_up = cycle_down = 1;

  if (myfg == -1)
    fg = fg_color;
  else
    fg = myfg;
  if (mybg == -1)
    bg = bg_color;
  else
    bg = mybg;
  if (myfg_frame == -1)
    fg_frame = fgframe_color;
  else
    fg_frame = myfg_frame;
  if (mybg_frame == -1) {
    bg_frame = bgframe_color;
  } else
    bg_frame = mybg_frame;
  int a = bg_frame & 0xff, b = (bg_frame >> 8) & 0xff, g = (bg_frame >> 16) & 0xff, r = (bg_frame >> 24);
  // sdl_gfx has an arbitrary mapping for colors, abgr intel !
  bg_frame_gfx = makecol_alpha(a,b,g,r);
  font_name = "Vera.ttf";
  lift = NULL;
  keybuf[0] = 0;
  SDL_GetMouseState(&mousex[0],&mousey[0]);
  mousex[1] = mousex[0]; mousey[1] = mousey[0];
  flip_page = 0;
  phase_repeat = jmoved = 0;
  work_area.h = 0;
  rows = 0;
  work_area.x = 0;
  work_area.w = sdl_screen->w;
  use_transparency = ::use_transparency;
}

void TMenu::update_label(int nb, char *str,int (*menu_func)(int)) {
    menu[nb].label = str;
    if (menu_func) menu[nb].menu_func = menu_func;
}

void TMenu::update_list_label(int nb, int index, char *str) {
    menu[nb].values_list_label[index] = str;
}

TMenu::~TMenu() {
    if (translated)
	free(menu);
  if (child) {
    for (int n=0; n<nb_items; n++)
      delete child[n];
    free(child);
    child = NULL;
  }
  if (h_child) {
      for (int n=0; header[n].label; n++)
	  free(h_child[n]);
      free(h_child);
      h_child = NULL;
  }

  delete font;
  if (menu_disp)
    free(menu_disp);
  if (lift)
    delete lift;
  if (fg_layer)
    SDL_DestroyTexture(fg_layer);
  desktop->set_work_area(NULL);
}

int TMenu::can_be_displayed(int n) {
  // can hide some menu entries at will. By default, show everything
  return 1;
}

int TMenu::can_be_selected(int n) {
  // same thing to say if an item can be selected
  // by default everything which can be displayed can also be selected
  return can_be_displayed(n) && child[n]->can_be_selected();
}

void TMenu::create_child(int n) {
  if (!menu[n].value_int)
    child[n] = new TStatic(&menu[n]);
  else if (menu[n].values_list_size > 0 && menu[n].value_int) {
    child[n] = new TOptions(&menu[n]);
  } else if (menu[n].values_list_size == ITEM_PROGRESS_BAR)
    child[n] = new TProgressBar(&menu[n]);
  else if (menu[n].values_list_size == ITEM_SLIDER)
    child[n] = new TSlider(&menu[n]);
  else if (menu[n].values_list_size == ITEM_EDIT)
    child[n] = new TEdit(&menu[n]);
  else if (menu[n].values_list_size == ITEM_FLOATEDIT)
    child[n] = new TFloatEdit(&menu[n]);
  else if (menu[n].values_list_size == ITEM_HEXEDIT)
    child[n] = new THexEdit(&menu[n]);
  else if (menu[n].values_list_size == ITEM_INTEDIT)
    child[n] = new TIntEdit(&menu[n]);

  else {
    printf("unknown extension %d\n",menu[n].values_list_size);
    exit(1);
  }
}

void TMenu::compute_nb_items() {
  if (child) {
    for (int n=0; n<nb_items; n++)
      if (child[n]) delete child[n];
    free(child);
    child = NULL;
  }
  nb_items = 0;
  nb_disp_items = 0;
  if (menu)  {
    while (menu[nb_items].label) {
      if (can_be_displayed(nb_items)) {
	nb_disp_items++;
      }
      nb_items++;
    }
    if (menu_disp)
      free(menu_disp);
    if (nb_disp_items)
	menu_disp = (int*)malloc(sizeof(int)*(nb_disp_items));
    else
	menu_disp = NULL;
    child = (TStatic **)malloc(sizeof(TStatic*)*nb_items);
    int n=0;
    nb_disp_items = 0;
    while (menu[n].label) {
      create_child(n);

      if (can_be_displayed(n)) {
	menu_disp[nb_disp_items++] = n;
	if (sel < 0 && can_be_selected(n))
	  sel = n;
      }
      n++;
    }

    if (header) {
	TStatic **old_child = child;
	menu_item_t *old_menu = menu;
	int nb;
	for (nb=0; header[nb].label; nb++);
	h_child = child = (TStatic **)malloc(sizeof(TStatic*)*nb);
	menu = header;
	n = 0;
	while (menu[n].label) {
	    create_child(n);
	    n++;
	}
	menu = old_menu;
	child = old_child;
	find_new_sel();
    }

    if (sel >= nb_items) sel = 0;
  }
}

int TMenu::can_exit() {
  for (int n=0; n<nb_items; n++) {
    if (!child[n]->can_exit()) {
      if (can_be_selected(n)) {
	sel = n;
	reset_top();
      }
      return 0;
    }
  }
  return 1; // nobody refuses the exit
}

void TMenu::draw_top_frame() {
}

char* TMenu::get_bot_frame_text() {
    return "";
}

int TMenu::get_max_bot_frame_dimensions(int &w, int &h) {
  char *game = get_bot_frame_text();
  if (!*game) {
      w = h = 0;
  } else if (font) font->dimensions(game,&w,&h);
  return strlen(game);
}

void TMenu::draw_bot_frame() {
    char *game = get_bot_frame_text();
    if (*game) {
	int base = work_area.y+work_area.h;
	boxColor(rend,0,base,sdl_screen->w,sdl_screen->h,bg_frame_gfx);
	font->put_string(HMARGIN,base,game,fg_frame,bg_frame);
    }
}

void TMenu::draw_frame(SDL_Rect *r) {
  int w_game, h_game;
  int w_title,h_title;

  // Big remake of this one, because the old one was calling compute_nb_items
  // which was recreating the whole list of children which was creating a pure mess
  // Now it's dangerous and might create problems... let's cross fingers !

  int len_top = strlen(title);
  int len_bot = strlen(get_bot_frame_text());
  if (!font) setup_font(MAX(len_bot,len_top));
  get_max_bot_frame_dimensions(w_game,h_game);
  if (*title)
      font->dimensions(title,&w_title,&h_title);
  else
      w_title = h_title = 0;

  int base = sdl_screen->h-h_game;

  work_area.y = h_title;
  work_area.h = (base - (h_title));

  if (!r || r->y < h_title) {
    draw_top_frame();
  }

  if (!r || r->y+r->h > base) {
    draw_bot_frame();
  }
  desktop->set_work_area(&work_area);
}

void TMenu::compute_width_from_font() {
  width_max = 0;
  int width_max_options = 0,hwidth_max_options = 0,n,w;
  // The problem with variable width fonts is that it's not because a string
  // is longer than another that it will require more space on screen
  // so we must restart the loop again and compute the real width on screen
  // for each string !
  if (header) {
      for (n=0; header[n].label; n++) {
	  w = h_child[n]->get_width(font);
	  if (w > width_max) {
	      width_max = w;
	  }
	  w = h_child[n]->get_width_max_options(font);
	  if (w > hwidth_max_options)
	      hwidth_max_options = w;
      }
  }
  width_max += 2*HMARGIN;
  hxoptions = width_max;
  for (int x=0; x<nb_disp_items; x++) {
      n = menu_disp[x];
    w = child[n]->get_width(font);
    if (w > width_max) {
      width_max = w;
    }
    w = child[n]->get_width_max_options(font);
    if (w > width_max_options)
      width_max_options = w;
  }
  if (width_max > hxoptions) width_max += 2*HMARGIN;
  if (width_max > work_area.w) width_max = work_area.w;
  if (hxoptions + hwidth_max_options + HMARGIN > width_max) {
      width_max = hxoptions + hwidth_max_options + HMARGIN;
  }
  w = get_fglayer_footer_width();
  if (w > width_max)
    width_max = w;
  xoptions = width_max;
  if (width_max_options) {
    width_max += width_max_options;
    width_max += HMARGIN;
  }
  if (width_max > work_area.w) width_max = work_area.w;
}

void TMenu::adjust_len_max_options(unsigned int &len_max_options) {
  // Just in case some inheritance needs to adjust this...
}

void TMenu::setup_font(unsigned int len_frame) {
  int w,h,n;
  compute_nb_items();
  unsigned int len_max_options = 0,len_max = 0;
  UINT32 len;
  int nb=0;
  if (nb_items) {
    for (n=0; n<nb_items; n++) {
      len = child[n]->get_len();
      if (len > len_max) {
	len_max = len;
      }
      len = child[n]->get_len_max_options();
      if (len > len_max_options)
	len_max_options = len;
    }
    if (header) {
	for (nb=0; header[nb].label; nb++) {
	    len = h_child[nb]->get_len();
	    if (len > len_max) len_max = len;
	    len = h_child[nb]->get_len_max_options();
	    if (len > len_max_options) len_max_options = len;
	}
    }
    len = get_fglayer_footer_len();
    if (len > len_max + len_max_options)
      len_max = len;
  }

  adjust_len_max_options(len_max_options);

  if (font) delete font;
  if (len_max + len_max_options + 3 > len_frame) {
    w = sdl_screen->w/(len_max+len_max_options + 3); // ideal font width & height, with some
  } else {
    w = sdl_screen->w/(len_frame); // ideal font width & height, with some
  }
  if (nb_disp_items + nb == 0) return;
  int hheader = 0;
  if (!header) skip_fglayer_header(hheader);
  h = (sdl_screen->h - 40 - hheader)/(nb_disp_items+nb); // margin
  h = h*4/9; // This 5/11 is just the result of tests, so that the main
  // menu fits on the screen without scrollbar when loading bublbobl !
  // Actually it's probably dependant of the font (size w != size h).
  if (h<min_font_size) h = min_font_size;
  if (h < w) w=h; // take the minimum for the ttf...
  else while (h < min_font_size && (w+h)/2 > 2+min_font_size) {
    w = (w+h)/2; // take an average value when w much bigger than h
  }
  if (w < min_font_size) w = min_font_size;
  font = new TFont_ttf(w,font_name);
  if (lift) { // lift place changes with the font
    delete lift;
    lift = NULL;
  }
  compute_width_from_font();
}

int TMenu::compute_fglayer_height() {
  int h;
  int nbh = 0;
  if (header) {
      for (nbh=0; header[nbh].label; nbh++);
  }
  if (nb_disp_items+nbh == 0) return 0;
  rows = work_area.h / font->get_font_height()-1;
  if (rows > nb_disp_items)
    rows = nb_disp_items;
  reset_top();
  if (top+rows > nb_disp_items)
    rows = nb_disp_items - top;
  if (top >= 0) {
    do {
      h = 2*HMARGIN + get_fglayer_footer_height();
      skip_fglayer_header(h);
      if (menu_disp)
	  for (int n=top; n<top+rows; n++)
	      h += child[menu_disp[n]]->get_height(font);
      if (h > work_area.h)
	rows--;
    } while (h > work_area.h);
  } else
    h = 2*HMARGIN+font->get_font_height()*rows; // rough approximation
  return h;
}

void TMenu::setup_fg_layer() {
  int w,h;

  work_area.x = 0;
  work_area.w = sdl_screen->w;
  w = width_max;
  h = compute_fglayer_height();
  fgdst.x = (work_area.w-w)/2;
  fgdst.y = (work_area.h-h)/2+work_area.y;
  if (fgdst.y < 0) {
    fgdst.y = 0;
  }
  int had_lift = 0;
  if (lift) {
      delete lift;
      had_lift = 1;
      lift = NULL;
  }
  if (fg_layer)
    SDL_DestroyTexture(fg_layer);
  if (nb_disp_items > rows) {
      if (!had_lift)
	  width_max += 10;
    w = width_max;
    int y = 0;
    skip_fglayer_header(y);
    lift = new TLift(width_max-20,y,h-y-get_fglayer_footer_height()-HMARGIN,
	    &top,&nb_disp_items,&rows,&update_count,cslider_border,cslider_bar,cslider_lift);
  }
  fg_layer = SDL_CreateTexture(rend,SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,w,h);
  if (!fg_layer) {
      printf("fg_layer creation problem (%d,%d) : %s\n",w,h,SDL_GetError());
      exit(1);
  }
  if (use_transparency)
      SDL_SetTextureBlendMode(fg_layer, SDL_BLENDMODE_BLEND);
  if (lift)
    lift->set_surface(fg_layer);
  bgsdl = gfx_to_sdlcolor(fg_format,bg);
  if (!fg_layer) {
    printf("fg layer creation failed: %s\n",SDL_GetError());
    exit(1);
  }
  // SDL_SetAlpha(fg_layer,SDL_SRCALPHA | SDL_RLEACCEL,(fw > 10 ? 128 : 128+48));
  fgdst.w = w; // Here is a trick to keep w & h since they can't be directly accessed for a texture !
  fgdst.h = h;
}

int TMenu::get_list_index(int n) {
  return child[n]->get_list_index();
}

int TMenu::compute_selcolor() {
  int selcolor;
  selcolor = (update_count<<1) & 0xff;
  if (selcolor > 0x80)
      selcolor = 0x100-selcolor;
  selcolor = mymakecol(0x7f+selcolor,0x7f+selcolor,0x7f+selcolor);
  return selcolor;
}

void TMenu::disp_menu(int n,int y,int w,int h) {
  /* This is only a simplification function :
   * it's passed y,s,w,h from update_fg_layer so that it can concentrate
   * only on what to display, and forget about the layers (just update
   * fg_layer with what was passed)
   * This is needed because if the entry has list of values attached to it
   * then there is more to display than just the label passed in s, and this
   * function also handles the flashing background on the current selection */

  int fw = HMARGIN;
  int selcolor;
  int myfg = get_fgcolor(n);
  if ((!focus && n == sel)) {
    selcolor = compute_selcolor();
    child[n]->disp(NULL,font,fw,y,w,h,bg,selcolor,xoptions);
  } else {
    child[n]->disp(NULL,font,fw,y,w,h,myfg,0,xoptions);
  }
}

void TMenu::disp_header(int n,int y,int w,int h) {
  // Equivalent to disp_menu but for the header
  // Even if the code is almost the same it would be awkwared to share it
  // because we must know what is currently drawn to know if the selection must
  // be drawn or not
  int fw = HMARGIN;
  int selcolor;
  int myfg = fg; // for now no need to change the header colors...
  if ((focus && n == hsel)) {
    selcolor = compute_selcolor();
    h_child[n]->disp(NULL,font,fw,y,w,h,bg,selcolor,hxoptions);
  } else {
    h_child[n]->disp(NULL,font,fw,y,w,h,myfg,0,hxoptions);
  }
}

void TMenu::display_fglayer_header(int &y) {
  // a border, usefull when dialogs are opaque
  rectangleColor(rend,0,0,fgdst.w-1,fgdst.h-1,fg);
  if (!font) return;
  if (header) {
      int w = width_max - HMARGIN;

      for (int n=0; header[n].label; n++) {
	  int h = h_child[n]->get_height(font);
	  disp_header(n,y,w,h);
	  y += h;
      }
  }
}

void TMenu::blit_area(int x,int y,int w,int h) {
    /* Blit only 1 entry, in this case, update bg, then fg, then screen */
    SDL_Rect from, to;
    from.x = x; from.y = y; from.h = h;
    from.w = w;
    to.x = fgdst.x+x;
    to.y = fgdst.y+y;
    to.w = from.w;
    to.h = from.h;
    SDL_Rect tmp = to;
    if (use_transparency)
	desktop->draw();
    SDL_RenderCopy(rend,fg_layer,&from,&tmp);
    // do_update(&to);
}

void TMenu::update_header_entry(int nb) {
    if (header) {
	int ret = SDL_SetRenderTarget(rend,fg_layer);
	if (ret < 0) {
	    printf("update_header_entry: RenderTarget failed ?\n");
	    exit(1);
	}
	int y = HMARGIN;
	int w = width_max - HMARGIN;
	for (int n=0; header[n].label; n++) {
	  int h = h_child[n]->get_height(font);
	  if (n == nb) {
	      // if there is only 1 indice to update, clear the bg for it
	      boxColor(rend,HMARGIN, y, w, h, bgsdl);
	      disp_header(n,y,w,h);
	      // blit_area(HMARGIN,y,w,h);
	      break;
	  }
	  y += h;
	}
	SDL_SetRenderTarget(rend,NULL);
    }
}

void TMenu::skip_fglayer_header(int &y) {
  // No default header
  if (header) {
      for (int n=0; header[n].label; n++) {
	  int h = h_child[n]->get_height(font);
	  y += h;
      }
  }
}

int TMenu::get_fglayer_footer_height() {
  return 0;
}

int TMenu::get_fglayer_footer_len() {
  return 0;
}

int TMenu::get_fglayer_footer_width() {
    if (font) {
	int w,h;
	font->dimensions("W",&w,&h);
	return w * get_fglayer_footer_len();
    }
  return 0;
}

void TMenu::display_fglayer_footer(int x,int &y, int w, int xoptions) {
}

void TMenu::fglayer_footer_update() {}

int TMenu::fglayer_footer_handle_key(SDL_Event *event) {
  return 0;
}

void TMenu::fglayer_footer_call_handler() {}

void TMenu::update_fg_layer(int nb_to_update) {
  int n,max;
  int fw = HMARGIN;
  /* Basic scrolling setup... */
  if(top > nb_disp_items-rows) top=nb_disp_items-rows;
  if(top < 0)		  top = 0;
  if(sel >= nb_items)	  sel = 0;

  int y = HMARGIN;
  int ret = SDL_SetRenderTarget(rend,fg_layer);
  if (ret < 0) {
      printf("SDL_SetRenderTarget %d : %s\n",ret,SDL_GetError());
      exit(1);
  }
  if (nb_to_update < 0) {
    setcolor(bgsdl);
    SDL_RenderClear(rend);
    display_fglayer_header(y);
  } else {
    skip_fglayer_header(y);
  }

  max = top+rows;
  if (nb_disp_items < max)
    max = nb_disp_items;

  int w = width_max - HMARGIN;

  if (lift)
    w -= 20;
  else
    w -= HMARGIN; // the lift should probably use HMARGIN width

  for (n=top; n<max; n++) {
    int index = menu_disp[n];
    int h;
    h = child[index]->get_height(font);

    if (nb_to_update < 0 || nb_to_update == index) {
	setcolor(bgsdl);
	SDL_Rect r = { HMARGIN, y, w, h };
	// Clear the background in case we draw an option which had just its length reduced
	SDL_RenderFillRect(rend,&r);
	disp_menu(index,y,w,h);
      // if (nb_to_update == index) {
	//  blit_area(fw,y,w,h);
      // }
    }
    y += h;
  }

  if (nb_to_update == -1) {
    if (lift) {
      lift->draw();
    }
    display_fglayer_footer(fw,y,w,xoptions);
    // do_update(&fgdst);
  }
  SDL_SetRenderTarget(rend,NULL);
}

void TMenu::do_update(SDL_Rect *region) {
  SDL_RenderPresent(rend);
}

void TMenu::draw() {

  desktop->draw();
  draw_frame();

  if (!fg_layer)
      setup_fg_layer();
  update_fg_layer();
  SDL_RenderCopy(rend,fg_layer,NULL,&fgdst);
  do_update(NULL);
}

void TMenu::redraw_fg_layer() {
  // this layer has become tricky to update finally !!!
  if (!fg_layer) {
      return draw();
  }
  update_fg_layer(-1);
  if (use_transparency)
    desktop->draw();
  else
    printf("skip update bg\n");
  SDL_RenderCopy(rend,fg_layer,NULL,&fgdst);
  do_update(&fgdst);
}

static int axis_x,axis_y;

void TMenu::reset_top() {
    if (!rows) return;
    if (focus) {
	if (hsel != -1)
	    for (hsel=0; !h_child[hsel]->can_be_selected(); hsel++);
	// Don't return here, reset top for the main part too !
    }
    int seldisp = get_seldisp();
    if (seldisp > top+rows-1)
	top = seldisp-rows/2;
    else if (seldisp < top && seldisp >= 0) {
	top = seldisp;
	// In case there are non selectable lines upper, there will be no way
	// to display them again, so it's better to scroll up more in this case
	while (top > 0 && !can_be_selected(top-1))
	    top--;
    }
}

void TMenu::next_sel() {
    if (focus) {
	// in the header
	if (hsel < 0) return;
	int old = hsel;
	do {
	    hsel++;
	    if (!header[hsel].label) {
		hsel--;
		if (nb_disp_items > 0) {
		    toggle_header();
		    sel = menu_disp[0];
		}
		return;
	    }
	} while (!h_child[hsel]->can_be_selected() && hsel != old);
	return;
    }
  int old_sel = sel;
  if (sel < 0) {
      if (header) {
	  while (header[hsel].label)
	      hsel++;
	  hsel--;
      } else if (top+rows < nb_disp_items)
	  top ++;
      return;
  }
  do {
    sel++;
    if (sel > menu_disp[nb_disp_items-1]) {
      sel = menu_disp[0];
    }
  } while (sel < nb_items && !can_be_selected(sel));
  if (sel == 0 && !can_be_selected(sel)) {
    sel = old_sel; // failure to change selection
    if (top+rows < nb_disp_items) top++;
  } else if (old_sel == sel && top+rows < nb_disp_items)
      top++;
  else if (cycle_down) {
    reset_top();
  }
}

void TMenu::prev_sel() {
    if (focus) {
	// in the header
	if (hsel == -1) return;
	if (hsel == 0 && nb_disp_items) {
	    // Headers must feel like the normal dialog, if reaching the top
	    // we go directly to the end of the normal list
	    sel = menu_disp[nb_disp_items-1];
	    toggle_header();
	    reset_top();
	    return;
	}
	int nb;
	for (nb=0; header[nb].label; nb++);
	int old = hsel;
	do {
	    hsel--;
	    if (hsel < 0) hsel = nb-1;
	} while (!h_child[hsel]->can_be_selected() && hsel != old);
	return;
    }
  int old_sel = sel;
  if (sel < 0) {
      if (header) {
	  sel = 0;
	  toggle_header();
	  while (header[hsel].label)
	      hsel++;
	  hsel--;
	  return;
      }
      if (top > 0)
	  top--;
      return;
  }
  do {
    sel--;
    if (sel < 0) {
	if (header) {
	    sel = 0;
	    toggle_header();
	    while (header[hsel].label)
		hsel++;
	    hsel--;
	    return;
	}
      sel = menu_disp[nb_disp_items - 1];
    }
  } while (sel >= 0 && !can_be_selected(sel));
  if (!can_be_selected(sel))  {
    sel = old_sel; // failure to change selection
    if (top > 0) top--;
  } else if (old_sel == sel && top > 0)
      top--;
  else if (cycle_up) {
    int seldisp = get_seldisp();
    if (seldisp > top+rows-1)
      top = seldisp-rows+1;
    else if (seldisp < top)
      top = seldisp;
  }
}

void TMenu::find_new_sel() {
  // Find a new selection in the visible entries when top has just changed
  if (header) {
      for (hsel=0; header[hsel].label && !h_child[hsel]->can_be_selected();
	      hsel++);
      if (!header[hsel].label)
	  hsel = -1;
  }
  if (top > nb_disp_items || top + rows > nb_disp_items)
      top = 0;
  if (top + rows > nb_disp_items)
      rows = nb_disp_items;

  if (menu_disp)
      for (int n=top; n<top+rows; n++) {
	  int index = menu_disp[n];
	  if (can_be_selected(index)) {
	      sel = index;
	      break;
	  }
      }
  if (sel == -1 && header && !focus) {
      focus = 1;
  }
}

void TMenu::find_new_sel_from_end() {
  // Find a new selection in the visible entries when top has just changed
  for (int n=top+rows-1; n>=top; n--) {
    int index = menu_disp[n];
    if (can_be_selected(index)) {
      sel = index;
      break;
    }
  }
}

void TMenu::next_page() {
  if (top+rows < nb_disp_items) {
    top += rows;
    if (top + rows > nb_disp_items)
      top = nb_disp_items - rows;
    find_new_sel();
  } else if (sel < menu_disp[nb_disp_items-1])
    find_new_sel_from_end();
}

void TMenu::prev_page() {
  if (top > 0) {
    top -= rows;
    if (top < 0)
      top = 0;
    find_new_sel();
  } else if (sel > menu_disp[top])
    find_new_sel();
}

void TMenu::goto_top() {
  if (top > 0) {
    top = 0;
    find_new_sel();
  } else if (sel > menu_disp[0])
    find_new_sel();
}

void TMenu::goto_end() {
  if (top < nb_disp_items - rows) {
    top = nb_disp_items - rows;
    find_new_sel();
  } else if (sel < menu_disp[nb_disp_items-1])
    find_new_sel_from_end();
}

int TMenu::get_seldisp() {
  int n;
  if (sel == -1) return -1;
  for (n=0; n<nb_disp_items; n++) {
    if (menu_disp[n] == sel) break;
  }
  return n;
}

static int mystrcasestr(const char *s1,const char *s2) {
    /* Since super mingw doesn't know strcasestr, here is my version */
    /* returns an int, it's just a boolean */
    char lower1[256],lower2[256];
    int n;
    for (n=0; n<255 && s1[n]; n++)
	lower1[n] = tolower(s1[n]);
    lower1[n] = 0;
    for (n=0; n<255 && s2[n]; n++)
	lower2[n] = tolower(s2[n]);
    lower2[n] = 0;
    return strstr(lower1,lower2) != NULL;
}

void TMenu::set_header(menu_item_t *myheader) {
    header = myheader;
    // Translate the header...
    while (header && header->label) {
	header->label = gettext(header->label);
	for (int n=0; n<header->values_list_size && header->values_list_label[n]; n++)
	    header->values_list_label[n] = gettext(header->values_list_label[n]);
	header++;
    }
    header = myheader;
}

void TMenu::toggle_header() {
    if (focus && sel == -1) return;
    if (!focus && (hsel == -1 || !header)) return;
    focus ^= 1;
    update_fg_layer(sel);
    if (hsel >= 0) update_header_entry(hsel);
}

static const char *skip_esc(const char *s) {
    while (*s == 27 && s[1]=='[') {
	s+=2;
	while (*s >= '0' && *s <= '9') // color
	    s++;
	s++; // mode;
    }
    return s;
}

void TMenu::handle_key(SDL_Event *event) {
  int sym,ret;
  if ((!focus && sel >= 0 && (ret = child[sel]->handle_key(event))) ||
	  (focus && hsel >= 0 && (ret = h_child[hsel]->handle_key(event)))) {
    if (ret > 0) call_handler();
    return;
  }
  ret = fglayer_footer_handle_key(event);
  if (ret > 0) fglayer_footer_call_handler();
  if (ret) return;

  switch (event->type) {
    case SDL_KEYDOWN:
      sym = event->key.keysym.sym;
      switch(sym) {
	case SDLK_UP:
	  prev_sel();
	  keybuf[0] = 0;
	  break;
	case SDLK_DOWN:
	  next_sel();
	  keybuf[0] = 0;
	  break;
	case SDLK_PAGEDOWN:
	  next_page();
	  keybuf[0] = 0;
	  break;
	case SDLK_PAGEUP:
	  prev_page();
	  keybuf[0] = 0;
	  break;
	case SDLK_HOME:
	  goto_top();
	  keybuf[0] = 0;
	  break;
	case SDLK_END:
	  goto_end();
	  keybuf[0] = 0;
	  break;
	case SDLK_RIGHT:
	  next_list_item();
	  break;
	case SDLK_LEFT:
	  prev_list_item();
	  break;
	case SDLK_RETURN:
	  exec_menu_item();
	  break;
	case SDLK_ESCAPE:
	  exit_menu = can_exit();
	  break;
	case SDLK_TAB:
	  toggle_header();
	  break;
      }
      break;
    case SDL_TEXTINPUT:
      int index = strlen(keybuf)-1;
      if (index < 0) index = 0;
      if (index < MAX_KEYBUF - 1) {
	  int n,seldisp;
	  strncat(keybuf,event->text.text,MAX_KEYBUF-1);
	  keybuf[MAX_KEYBUF-1] = 0;
	  if (keybuf[0] >= '0' && keybuf[0] <= '9') {
	      // Just give up on numbers, too many stupid matches
	      keybuf[0] = 0;
	      break;
	  }
	  index = strlen(keybuf);
	  if (sel >= 0 && child[sel]->handle_key_buff(keybuf)) {
	      update_fg_layer();
	      call_handler();
	  } else {
	      // look for the new selection then, starting at sel
	      // 1 : find sel in the menu_disp array
	      n = get_seldisp();
	      int found = 0;
	      for (; n<nb_disp_items; n++) {
		  const char *s = skip_esc(menu[menu_disp[n]].label);
		  if (can_be_selected(menu_disp[n]) &&
			  !strncasecmp(s,keybuf,index)) {
		      found = 1;
		      break;
		  }
	      }
	      if (!found) { // not found -> search from 0
		  for (n=0; n<=get_seldisp(); n++) {
		      const char *s = skip_esc(menu[menu_disp[n]].label);
		      if (can_be_selected(menu_disp[n]) &&
			      !strncasecmp(s,keybuf,index)) {
			  found = 1;
			  break;
		      }
		  }
	      }

	      if (!found) { // not found -> search substring
		  for (n=get_seldisp(); n<nb_disp_items; n++) {
		      const char *s = skip_esc(menu[menu_disp[n]].label);
		      if (can_be_selected(menu_disp[n]) &&
			      mystrcasestr(s,keybuf)) {
			  found = 1;
			  break;
		      }
		  }
	      }
	      if (!found) { // and from 0
		  for (n=0; n<nb_disp_items; n++) {
		      const char *s = skip_esc(menu[menu_disp[n]].label);
		      if (can_be_selected(menu_disp[n]) &&
			      mystrcasestr(s,keybuf)) {
			  found = 1;
			  break;
		      }
		  }
	      }

	      if (!found) {
		  // Let's say that the new key is the start of a new selection
		  // then...
		  if ((size_t)index == strlen(keybuf)) { // if it's already the 1st one, forget it
		      keybuf[0] = 0;
		      break;
		  }
		  // Otherwise just clear keybuf and process this event again
		  keybuf[0] = 0;
		  handle_key(event);
		  break;
	      }
	      sel = menu_disp[n];
	      while (sel < menu_disp[top]) {
		  top--;
	      }
	      while (top+rows < nb_disp_items && sel >= menu_disp[top+rows]) {
		  top++;
	      }
	      if (!return_mandatory) {
		  // check if another entry matches keybuf
		  seldisp = n;
		  for (n=seldisp+1; n<nb_disp_items; n++) {
		      const char *s = skip_esc(menu[menu_disp[n]].label);
		      if (can_be_selected(menu_disp[n]) &&
			      !strnicmp(s,keybuf,index)) {
			  break;
		      }
		  }
		  if (n == nb_disp_items) {
		      for (n=0; n<seldisp; n++) {
			  const char *s = skip_esc(menu[menu_disp[n]].label);
			  if (can_be_selected(menu_disp[n]) &&
				  !strnicmp(s,keybuf,index)) {
			      break;
			  }
		      }
		      if (n == seldisp) { // no other entry
			  exec_menu_item();
		      }
		  }
	      }
	  }
      }
      break;
  }
}

void TMenu::redraw(SDL_Rect *r) {
  desktop->draw();
  draw_frame(r);
  if (!r) {
      SDL_RenderCopy(rend,fg_layer,NULL,&fgdst);
      do_update(NULL);
      return;
  }
  SDL_Rect oldr = *r;
  if (r->x < fgdst.x+fgdst.w && r->y < fgdst.y+fgdst.h &&
      r->x+r->w >= fgdst.x && r->y+r->h >= fgdst.y) {
    SDL_Rect from;
    from.x = r->x - fgdst.x;
    from.y = r->y - fgdst.y;
    from.w = r->w;
    from.h = r->h;
    if (from.x < 0) {
      r->x -= from.x;
      from.w += from.x;
      r->w = from.w;
      from.x = 0;
    }
    if (from.y < 0) {
      r->y -= from.y;
      from.h += from.y;
      r->h = from.h;
      from.y = 0;
    }
    SDL_RenderCopy(rend,fg_layer,&from,r);
    do_update(NULL);
    *r = oldr; // preserve region
  }
}

void TMenu::handle_button(SDL_Event *event, int index) {
    if (event->type == SDL_MOUSEBUTTONUP ) {
	switch(event->button.button) {
	case 1: if ((!focus && index == sel) || (focus && hsel >= 0))
		    exec_menu_item();
	       	break;
		// Apparently in sdl button 2 is center button, and 3 is right
		// (hope it's standard in every os !)
	case 2: if ((!focus && index == sel) || (focus && index == hsel))
		    prev_list_item();
	       	break;
	case 3: exit_menu = can_exit(); break;
	case 4: prev_page(); break;
	case 5: next_page(); break;
	}
    }
}

void TMenu::handle_mouse(SDL_Event *event) {
  int mx,my;
  if (event->type == SDL_MOUSEMOTION) {
    mx = event->motion.x;
    my = event->motion.y;
  } else {
    mx = event->button.x;
    my = event->button.y;
  }
  // fix event coordinates for the children...
  if (event->type == SDL_MOUSEMOTION)  {
    event->motion.x -= fgdst.x;
    event->motion.y -= fgdst.y;
  } else {
    event->button.x -= fgdst.x;
    event->button.y -= fgdst.y;
  }
  if (lift && mx > fgdst.x+fgdst.w-lift->get_width()) {
    lift->handle_mouse(event);
    return;
  }
  switch (event->type) {
    case SDL_MOUSEMOTION:
    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEBUTTONDOWN:
      if (my >= fgdst.y && my < fgdst.y+fgdst.h && mx >= fgdst.x && mx < fgdst.x+fgdst.w) {
	int ystart = fgdst.y + HMARGIN;
	int index,h=0;
	int n;
	if (header) {
	    for (n=0; header[n].label; n++) {
		h += h_child[n]->get_height(font);
		if (ystart + h > my) break;
	    }
	    if (header[n].label && h_child[n]->can_be_selected()) {
		if (!focus) {
		    toggle_header();
		}
		hsel = n;
		if (h_child[hsel]->handle_mouse(event)) {
		    call_handler();
		    return;
		}
	    } else if (!header[n].label && focus) {
		toggle_header();
	    }
	} else
	    skip_fglayer_header(ystart);
	for (index = 0; index < rows; index++) {
          h += child[index+top]->get_height(font);
	  if (ystart + h > my)
	    break;
	}
	if (index >= 0 && top+index < nb_disp_items)
	  index = menu_disp[top + index];
	else
	  index = -1;
	if (index >= 0 && index != sel && can_be_selected(index))
	  sel = index;
	if (sel >= 0 && child[sel]->handle_mouse(event)) {
	  call_handler();
	  return;
	}
	if (event->type != SDL_MOUSEMOTION && ((!focus && index >= 0) ||
		    (focus && hsel == n)))
	    handle_button(event,index);
      }
      break;
  }
}

void TMenu::produce_joystick_event() {
  // Produce a joystick event based on axis_x & axis_y
  if (sel >=0 && child[sel]->produce_joystick_event(axis_x,axis_y)) {
    call_handler();
    return;
  }
  if (axis_x == -1) {
    if (menu[sel].value_int)
      prev_list_item();
    else {
      prev_page();
    }
  } else if (axis_x == 1) {
    if (menu[sel].value_int)
      next_list_item();
    else
      next_page();
  }
  if (axis_y == -1) {
    prev_sel();
  } else if (axis_y == 1) {
    next_sel();
  }
}

void TMenu::handle_joystick(SDL_Event *event) {
  int hat;
  switch (event->type) {
    case SDL_JOYHATMOTION:
      /* Emulate joystick movement with a hat - what linux does automatically
       * but it's for windows... */
      hat = event->jhat.value;
      axis_x = 0;
      axis_y = 0;
      jmoved = 0;

      if (hat == SDL_HAT_UP)
	axis_y = -1;
      if (hat == SDL_HAT_DOWN)
	axis_y = 1;
      if (hat == SDL_HAT_LEFT)
	axis_x = -1;
      if (hat == SDL_HAT_RIGHT)
	axis_x = 1;
      if (hat) {
	jmoved = 1;
	timer = update_count;
	produce_joystick_event();
      } else
	phase_repeat = 0;
      break;
  case SDL_JOYAXISMOTION:
    switch(event->jaxis.axis) {
      case 0: // x axis normally
        if (event->jaxis.value < -16000) {
	  if (axis_x > -1) {
	    axis_x = -1;
	    jmoved = 1;
	    timer = update_count;
	    produce_joystick_event();
	  }
	} else if (event->jaxis.value > 16000) {
	  if (axis_x < 1) {
	    axis_x = 1;
	    jmoved = 1;
	    timer = update_count;
	    produce_joystick_event();
	  }
	} else if ((axis_x == -1 && event->jaxis.value > -16000) ||
	           (axis_x == 1 && event->jaxis.value < 16000)) {
	  axis_x = 0;
	  jmoved = 0;
	  phase_repeat = 0;
	}
	break;
      case 1: // y axis
        if (event->jaxis.value < -16000) {
	  if (axis_y > -1) {
	    axis_y = -1;
	    jmoved = 1;
	    timer = update_count;
	    produce_joystick_event();
	  }
	} else if (event->jaxis.value > 16000) {
	  if (axis_y < 1) {
	    axis_y = 1;
	    jmoved = 1;
	    timer = update_count;
	    produce_joystick_event();
	  }
	} else if ((axis_y == -1 && event->jaxis.value > -16000) ||
	           (axis_y == 1 && event->jaxis.value < 16000)) {
	  axis_y = 0;
	  jmoved = 0;
	  phase_repeat = 0;
	}
	break;
    } // end processing axis
    break;
  case SDL_JOYBUTTONUP:
    switch (event->jbutton.button) {
      case 0: exec_menu_item(); break;
      case 1: exit_menu = can_exit(); break;
    }
    break;
  } // end processing event type
}

void TMenu::call_handler() {
    int exit = -10;
    if (focus) {
	if (header[hsel].menu_func)
	    exit = (*header[hsel].menu_func)(hsel);
    } else {
	if (menu[sel].menu_func)
	    exit = (*menu[sel].menu_func)(sel);
    }
    if (exit != -10) exit_menu = exit; // hack but it should work always
    if (exit_menu) exit_menu = can_exit();
    // No need to force a redraw here, it's handled by execute...
}

void TMenu::next_list_item() {
  if (!focus && sel >= 0) {
    child[sel]->next_list_item();
    call_handler();
  } else if (focus && hsel >= 0) {
    h_child[hsel]->next_list_item();
    call_handler();
  }
}

void TMenu::prev_list_item() {
  if (!focus && sel >= 0) {
    child[sel]->prev_list_item();
    call_handler();
  } else if (focus && hsel >= 0) {
    h_child[hsel]->prev_list_item();
    call_handler();
  }
}

void TMenu::exec_menu_item() {
  /* Warning : try to never override this function or call the parent
   * because if call_handler is not called then the caller variable is not
   * changed, producing display bugs */
    keybuf[0] = 0; // reset key buffer
    if (focus) {
	if (hsel < 0) return;
	if (header[hsel].value_int)
	    next_list_item();
	else if (header[hsel].menu_func) {
	    call_handler();
	}
	return;
    }
  if (sel < 0) return;
  if (menu[sel].value_int)
    next_list_item();
  else /* if (menu[sel].menu_func)*/ {
      // call_handler must be called in all cases here if overloaded like for
      // the game selection dialog !
    call_handler();
  }
}

void TMenu::execute() {

    update_count = 0;
    gui_level++;
  SDL_Event event;
  SDL_StartTextInput();
  exit_menu = 0;

  if (font) {
    delete font;
    font = NULL;
  }
  SDL_initFramerate(&fpsm);
  fpsm.cpu_frame_count = NULL;
  SDL_setFramerate(&fpsm,30);
  if (caller != this) {
    parent = caller; // update parent for persistant dialogs
  }

  draw();
  caller = this; // init after calling draw

  while (!exit_menu) {
    int oldsel = sel;
    if (focus) oldsel = hsel;
    int oldtop = top;
    while (SDL_PollEvent(&event)) {
      switch(event.type) {
      case SDL_KEYDOWN:
      case SDL_KEYUP:
      case SDL_TEXTINPUT:
	handle_key(&event);
	break;
      case SDL_MOUSEMOTION:
      case SDL_MOUSEBUTTONUP:
      case SDL_MOUSEBUTTONDOWN:
        handle_mouse(&event);
	break;
      case SDL_JOYAXISMOTION:
      case SDL_JOYBALLMOTION:
      case SDL_JOYHATMOTION:
      case SDL_JOYBUTTONDOWN:
      case SDL_JOYBUTTONUP:
        handle_joystick(&event);
	break;
      case SDL_WINDOWEVENT:
	if (!*window_event_hook || (*window_event_hook)(&event)) {
	    if (event.window.event == SDL_WINDOWEVENT_RESIZED || event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
		if (keep_vga && (event.window.data1 < 640 || event.window.data2 < 480)) {
		    SDL_SetWindowSize(win,640,480);
		    break;
		}

		if (font) {
		    delete font;
		    font = NULL;
		}
		SDL_DestroyTexture(fg_layer);
		fg_layer = NULL;
		sdl_screen->w = event.window.data1;
		sdl_screen->h = event.window.data2;
		draw();
	    }
	}
	break;
      case SDL_QUIT:
	exit(0);
      }
      if (exit_menu) break;
    }
    SDL_setFramerate(&fpsm,30);
    if (exit_menu) break;
    if (lift) {
      lift->update();
    }
    if (!focus && sel >= 0)
      child[sel]->update();
    else if (focus && hsel >= 0)
	h_child[hsel]->update();
    fglayer_footer_update();
    if (header)
	for (int n=0; header[n].label; n++)
	    h_child[n]->update();

    if (jmoved) { // joystick moved, handle auto-repeat...
      if (!phase_repeat) {
        if ((update_count - timer)*fpsm.rateticks >= repeat_delay) {
	  phase_repeat = 1;
	  produce_joystick_event();
	  timer = update_count;
        }
      } else if ((update_count - timer)*fpsm.rateticks >= repeat_interval) {
	produce_joystick_event();
	timer = update_count;
      }
    }
    update_count++;
    // if we want to handle animations on the bg layer 1 day...
    if (top != oldtop)
      redraw_fg_layer();
    else if (!focus && (sel != oldsel)) {
      update_fg_layer(-1);
      redraw(NULL);
    } else if (focus && hsel != oldsel) {
      update_header_entry(oldsel);
      if (hsel >= 0 && h_child[hsel]->can_draw_selection())
	update_header_entry(hsel);
      redraw(NULL);
    } else if (!focus && nb_disp_items && (sel >= top && sel <= menu_disp[top+rows-1])) {
      if (child[sel]->can_draw_selection()) {
       	update_fg_layer(sel);
	redraw(NULL);
      }
    } else if (focus && hsel >= 0) {
	if (h_child[hsel]->can_draw_selection()) {
	    update_header_entry(hsel);
	    redraw(NULL);
	}
    }
    SDL_framerateDelay(&fpsm);
  }
  if (parent) {
    // restore the screen
    parent->draw();
    caller = parent;
  } else {
      desktop->draw();
    caller = NULL;
  }
  gui_level--;
  parent = NULL; // to be on the safe side
  if (gui_level == 0) SDL_StopTextInput();
}

// TBitmap_menu : a menu with a bitmap on top of it
// Is it better to handle bitmaps with a child of TMenu or with a type
// of menu_item ???
// For now it seems that it's easier to do it with a child, maybe I'll have
// to change my mind later

TBitmap_menu::TBitmap_menu(char *my_title, menu_item_t *mymenu, char *bitmap_path) :
  TMenu(my_title,mymenu)
{
  bmp = IMG_Load(bitmap_path);
  if (!bmp) {
    printf("TBitmap_menu: couldn't load %s\n",bitmap_path);
  }
}

TBitmap_menu::~TBitmap_menu() {
  if (bmp) {
    SDL_FreeSurface(bmp);
  }
}

void TBitmap_menu::setup_font(unsigned int len_frame) {
  TMenu::setup_font(len_frame);
  if (bmp) {
      if (bmp->w+2*HMARGIN > width_max)
	  width_max = bmp->w+2*HMARGIN;
      int h2;
      h2 = (sdl_screen->h-bmp->h-2)/(nb_items + 4 + 6); // margin
      if (h2 < font->get_font_height() && h2 < font->get_font_width()) {
	  delete font;
	  font = new TFont_ttf(h2);
	  compute_width_from_font();
      }
  }
}

void TBitmap_menu::display_fglayer_header(int &y) {
  if (bmp) {
    SDL_Rect dest;
    dest.x = (fgdst.w - bmp->w)/2;
    dest.y = y;

    SDL_Texture *tex = SDL_CreateTextureFromSurface(rend,bmp);
    SDL_RenderCopy(rend,tex,NULL,&dest);
    SDL_DestroyTexture(tex);
    y += bmp->h+2;
  }
}

void TBitmap_menu::skip_fglayer_header(int &y) {
  if (bmp) {
    y += bmp->h+2;
  }
}

// TDialog : a menu without frames (top & bottom) but with a title bar

TDialog::TDialog(char *my_title, menu_item_t *mymenu) :
  TMenu(my_title,mymenu)
{ htitle = 0;
}

void TDialog::compute_width_from_font() {
    TMenu::compute_width_from_font();
    if (*title) {
	int w;
	font->dimensions(title,&w,&htitle);
	w+=2; htitle+=2; // with border
	if (w > width_max)
	    width_max = w;
    }
}

void TDialog::display_fglayer_header(int &y) {
  if (*title) {
    boxColor(rend,1,1,fgdst.w-2,htitle-1,bg_dialog_bar_gfx);
    font->surf_string_tr(NULL,1,1,title,fg);
    rectangleColor(rend,0,0,fgdst.w-1,htitle,fg);
    y += htitle + 2;
  }
  TMenu::display_fglayer_header(y);
}

void TDialog::skip_fglayer_header(int &y) {
  if (*title) {
    y += htitle + 2;
  }
}

void TDialog::draw_frame(SDL_Rect *r) {
  if (!font) setup_font(0);
  work_area.x = 0;
  work_area.y = 0;
  work_area.w = sdl_screen->w;
  work_area.h = sdl_screen->h-40;
  desktop->set_work_area(&work_area);
}

// TMenuMultiCol : a multi colomn version of TMenu

TMenuMultiCol::TMenuMultiCol(char *my_title, menu_item_t *mymenu, int nbcol, char **mycols) : TMenu(my_title,mymenu) {
  nb_cols = nbcol;
  cols = mycols;
  colpos = (int*)malloc(sizeof(int)*nb_cols);
}

TMenuMultiCol::~TMenuMultiCol() {
  if (colpos) free(colpos);
}

void TMenuMultiCol::adjust_len_max_options(unsigned int &len_max_options) {
  len_max_options += nb_cols;
  for (int c=0; c< nb_cols; c++) {
    int len_max = 0;
    for (int n=0; n<nb_disp_items; n++) {
      int index = menu_disp[n];
      int len = strlen(cols[index*nb_cols+c]);
      if (len > len_max) {
	len_max = len;
      }
    }
    len_max_options += len_max;
  }
}

void TMenuMultiCol::compute_width_from_font() {
  TMenu::compute_width_from_font();
  for (int c=0; c<nb_cols; c++) {
    colpos[c] = width_max;
    int wmax = 0;
    for (int n=0; n<nb_disp_items; n++) {
      int index = menu_disp[n],w,h;
      font->dimensions(cols[index*nb_cols+c],&w,&h);
      if (w > wmax) {
	wmax = w;
      }
    }
    width_max += wmax+HMARGIN;
  }
  width_max += HMARGIN;
  if (width_max > work_area.w) width_max = work_area.w;
}

void TMenuMultiCol::disp_menu(int n,int y,int w,int h) {
  // Just need to add the columns after the normal line
  TMenu::disp_menu(n,y,w,h);
  for (int c=0; c<nb_cols; c++) {
    if (n == sel) {
      int selcolor = compute_selcolor();
      font->surf_string(NULL,colpos[c],y,cols[n*nb_cols+c],bg,selcolor,w-colpos[c]);
    } else
      font->surf_string_tr(NULL,colpos[c],y,cols[n*nb_cols+c],fg,w-colpos[c]);
  }
}

// TMenuPostCb

void TMenuPostCb::adjust_len_max_options(unsigned int &len_max_options) {
    if (*legend)
	len_max_options += strlen(legend);
    else
	len_max_options++;
}

void TMenuPostCb::compute_width_from_font() {
  TMenu::compute_width_from_font();
  int h;
  pos_cb = width_max;
  font->dimensions("X",&wcb,&h);
  if (*legend) {
      int w;
      font->dimensions(legend,&w,&h);
      width_max += w+HMARGIN*2;
  } else
      width_max += wcb+HMARGIN*2;
}

int TMenuPostCb::can_be_selected(int n) {
    if (n == 0)
	return 0;
    return 1;
}

void TMenuPostCb::disp_menu(int n,int y,int w,int h) {
  // Just need to add the columns after the normal line
  TMenu::disp_menu(n,y,w,h);
  if (n == 0) {
      font->surf_string_tr(NULL,pos_cb,y,legend,fg);
  } else {
      int x = pos_cb + HMARGIN;
      rectangleColor(rend,x,y,x+wcb,y+h-1,fg);
      if (cb[n]) {
	  lineColor(rend,x,y,x+wcb,y+h-1,mymakecol(0,255,0));
	  lineColor(rend,x+wcb,y,x,y+h-1,mymakecol(0,255,0));
      }
  }
}

void TMenuPostCb::handle_key(SDL_Event *event) {
  if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_SPACE)
      cb[sel] ^= 1;
  else
      TMenu::handle_key(event);
}

void TMenuPostCb::handle_button(SDL_Event *event, int index) {
    if (event->type == SDL_MOUSEBUTTONUP ) {
	switch(event->button.button) {
	case 1:
	    if (event->button.x >= pos_cb + HMARGIN &&
		    event->button.x < pos_cb + HMARGIN + wcb) {
		cb[index] ^= 1;
		return;
	    }
	default:
	    TMenu::handle_button(event,index);
	}
    }
}

