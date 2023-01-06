#ifndef MENU_CPP
#define MENU_CPP

#include "compat_sdl.h"
#include "SDL_gfx/SDL_framerate.h"
#include "menuitem.h" // C only

#ifdef __cplusplus
#include "tfont.h"
#include "tlift.h"
#include "widget.h"
#include "translate.h"

#ifdef MEMWATCH
#include "memwatch.h"
#endif

extern SDL_PixelFormat *fg_format;
extern char *jap_font; // font to use if an utf8 extended glyph is detected, default is "", no font

void disp_cursor(SDL_Surface *s,int x,int y, int w, int h);

/* If values_list_size == 3, and there are no labels for this list, then it's
 * a special interval list. Format { min, max, step }. It's an easy way to
 * allow big ranges of values */

// size of the key buffer to choose a menu entry by its label
#define MAX_KEYBUF 20

class TDesktop {
    protected:
	SDL_Texture *pic;
	int picw,pich;
	SDL_Rect work_area;
    public:
	int w,h;
	TDesktop();
	// Set a picture as the background, returns 1 if the picture was correctly loaded
	int set_picture(const char *name);
	virtual void draw();
	void set_work_area(SDL_Rect *r) {
	    if (r)
		work_area = *r;
	    else {
		work_area = {0,0,0,0};
	    }
	}
};

// This desktop replaces the bg_layer there was for sdl-1.2
// the idea is to allow something more complex than a rectill or the blit of a bitmap...
extern TDesktop *desktop;

extern int repeat_interval, repeat_delay;
extern int return_mandatory, use_transparency,bg_anim;
// All windows event are passed to this hook, returns 1 only if
extern void (*event_hook)(SDL_Event *event); // Actually it's now passed any event to the gui and can modify it before the gui processes it...
extern char * (*get_shared_hook)(char *name); // to get a shared directory
extern void (*gui_end_hook)(); // called at the end of execute, to setup game screen eventually
extern void (*gui_start_hook)();

 // Useful macro to easily assign a color from a variable...
#define setcolor(c) SDL_SetRenderDrawColor(rend,(c)>>24,((c)>>16)&0xff,((c)>>8)&0xff,(c)&0xff)

class TMenu {
  protected:
    TMenu *parent; // implicitely initiealised with caller
    TStatic **child,**h_child;
    const char *title;
    FPSmanager fpsm;
    /* A header would just be a section of the dialog always visible, no
     * scrolling */
    menu_item_t *menu,*header;
    int focus; // where the focus is : 0 normal dialog, 1 header
    // Index of the current selection or -1 if none, top = 1st index to be
    // displayed (in the menu_disp list now)
    int sel,top,hsel;
    int translated;
    int fg,bg, // colors for the fg layer
      bgsdl, // still bg color of the fg layer but in sdl format
      fg_frame,bg_frame, // colors for the frames (top & bottom)
      bg_frame_gfx, // sdl_gfx specific mapping !
      last_update,update_count;
    SDL_Rect work_area, // Area between the 2 frames
      fgdst; // area used by the fg layer (menu) on screen
    int nb_items, // number of items in the menu (total)
      nb_disp_items, // number of items which can be displayed
      rows, // number of rows which can be displayed on screen
      width_max,exit_menu;
    int *menu_disp; // indexes of entries which can be displayed
    /* This array of indexes makes all the scrolling stuff much easier */
    int xoptions,hxoptions; // start x for the options (area on the right)
    int phase_repeat, timer,jmoved; // automatic repeat for joystick
    TFont *font;
    char *font_name;
    SDL_Texture *fg_layer;
    TLift *lift; // in case a lift is needed !
    char keybuf[MAX_KEYBUF];
    int mousex[2],mousey[2],flip_page; // must handle the mouse when double buffering
    int use_transparency;
    int cycle_up, cycle_down; // scrolling loops when reaching top/bottom (default yes)
  public:
    // There is a lot of methods, but most of them are very small and are
    // here to easily create objects which inherit from this one
    TMenu(char *my_title, menu_item_t *mymenu,char *myfont = NULL,
      int myfg=-1, int mybg=-1,int myfg_frame=-1,int mybg_frame=-1,
      int to_translate=1);
    void free_hchild();
    void set_header(menu_item_t *myheader);
    void toggle_header();
    virtual ~TMenu();
    void set_transparency(int transp) { use_transparency = transp; }
    virtual void draw_top_frame();
    virtual char* get_bot_frame_text();
    virtual int get_max_bot_frame_dimensions(int &w, int &h);
    virtual void draw_bot_frame();
    virtual void draw_frame(SDL_Rect *r = NULL); // draw top & bottom frame, and sets the
    // work_area according to the size of the top & bottom frames.
    virtual void draw();
    virtual void adjust_len_max_options(unsigned int &len_max_options);
    virtual void setup_font(unsigned int len_frame);
    virtual void compute_width_from_font();
    virtual void setup_fg_layer();
    virtual void display_fglayer_header(int &y);
    virtual void skip_fglayer_header(int &y);
    virtual int get_fglayer_footer_height();
    virtual int get_fglayer_footer_width();
    virtual int get_fglayer_footer_len();
    virtual void display_fglayer_footer(int x,int &y,int w, int xoptions);
    virtual void fglayer_footer_update();
    virtual int fglayer_footer_handle_key(SDL_Event *event);
    virtual void fglayer_footer_call_handler();
    virtual void update_fg_layer(int nb_to_update = -1);
    virtual void redraw_fg_layer();
    virtual void redraw(SDL_Rect *r);
    virtual void handle_key(SDL_Event *event);
    virtual void next_list_item();
    virtual void prev_list_item();
    virtual void call_handler();
    virtual void handle_button(SDL_Event *event, int index);
    void update_label(int nb, char *str,int (*menu_func)(int) = NULL);
    void update_list_label(int nb, int index, char *str);
    void update_list_size(int nb, int size) {
	menu[nb].values_list_size = size;
    }
    virtual void handle_mouse(SDL_Event *event);
    virtual void produce_joystick_event();
    virtual void handle_joystick(SDL_Event *event);
    virtual void exec_menu_item();
    virtual void  execute();
    virtual void do_update(SDL_Rect *region);
    virtual void create_child(int n);
    virtual void compute_nb_items();
    virtual int compute_fglayer_height();
    // menu entries tests
    virtual int can_be_displayed(int n);
    virtual int can_be_selected(int n);
    int get_seldisp();
    virtual void reset_top();
    virtual void next_sel();
    virtual void prev_sel();
    virtual void next_page();
    virtual void prev_page();
    virtual void goto_top();
    virtual void goto_end();
    virtual void find_new_sel();
    virtual void find_new_sel_from_end();
    virtual int compute_selcolor();
    virtual int get_fgcolor(int n) {
      return fg;
    }
    virtual void disp_menu(int n,int y,int w,int h);
    virtual void update_header_entry(int nb);
    virtual void disp_header(int n,int y,int w,int h);
    int get_list_index(int n);
    void set_title(char *my_title) {
      title = my_title;
    }
    virtual int can_exit();
    virtual void set_sel(int mysel) { sel = mysel; reset_top(); }
    virtual char *get_top_string();
};

class TBitmap_menu : public TMenu {
  protected:
    SDL_Surface *bmp;
  public:
    TBitmap_menu(char *my_title, menu_item_t *mymenu, char *bitmap_path);
    ~TBitmap_menu();
    virtual void setup_font(unsigned int len_frame);
    virtual void display_fglayer_header(int &y);
    virtual void skip_fglayer_header(int &y);
};

class TDialog : public TMenu {
  protected:
    int htitle;
  public:
    TDialog(char *my_title, menu_item_t *mymenu);
    virtual void compute_width_from_font();
    void clear_screen() {}
    virtual void display_fglayer_header(int &y);
    virtual void skip_fglayer_header(int &y);
    virtual void draw_frame(SDL_Rect *r);
};

class TMenuMultiCol : public TMenu {
  protected:
    int *colpos;
    int nb_cols;
    char **cols;
  public:
    TMenuMultiCol(char *my_title, menu_item_t *mymenu, int nbcol, char **mycols);
    ~TMenuMultiCol();
    virtual void adjust_len_max_options(unsigned int &len_max_options);
    virtual void compute_width_from_font();
    virtual void disp_menu(int n,int y,int w,int h);
};

// Version with a check box at the end of every line
class TMenuPostCb : public TMenu {
    protected:
	int *cb,wcb,pos_cb;
	char *legend;
    public:
	TMenuPostCb(char *my_title, menu_item_t *mymenu, int *mycb,char *leg) :
    TMenu(my_title,mymenu)
    {
	cb = mycb;
	wcb = 0;
	legend = leg;
    }
    virtual void adjust_len_max_options(unsigned int &len_max_options);
    virtual void compute_width_from_font();
    virtual void disp_menu(int n,int y,int w,int h);
    virtual int can_be_selected(int n);
    virtual void handle_key(SDL_Event *event);
    virtual void handle_button(SDL_Event *event, int index);
};

#endif

#endif
