#ifndef TEDIT_H
#define TEDIT_H

#include "widget.h"

#define MAX_HISTORY 30

class TEdit : public TStatic
{
  protected:
    char *field;
    unsigned int maxl,pos,cursor_on,update_count,old_pos,pos_cursor;
    int myx,myy,myh;
    SDL_Surface *mys;
    TFont *font;
    char *history[MAX_HISTORY];
    int used_hist,current_hist,use_hist;
    char prefix_hist[80];
  public:
    TEdit(menu_item_t *my_menu) : TStatic(my_menu)
    {
      maxl = menu->values_list[0];
      use_hist = menu->values_list[1];
      cursor_on = 0;
      used_hist = update_count = 0;
      current_hist = -1;
      history[MAX_HISTORY-1] = NULL;
      pos_cursor = old_pos = 0;
      field = menu->values_list_label[0];
      pos = strlen(field);
    }
    ~TEdit() {
      for (int n=0; n<used_hist; n++) {
	free(history[n]);
      }
      used_hist = 0;
    }
    void update_cursor();
    virtual void disp(SDL_Surface *mys, TFont *myfont, int x, int y, int w, int h,
  int fg, int bg, int xoptions);
    virtual int get_height(TFont *font);
    virtual int get_len();
    virtual int get_len_max_options();
    // virtual int handle_mouse(SDL_Event *event);
    virtual int handle_key(SDL_Event *event);
    virtual void update();
    virtual void add_history();
    virtual void insert(char *s);
    virtual int can_be_selected() { return 1; }
    virtual int valid_chars(int sym, int unicode);
};

class TFloatEdit : public TEdit {
  protected:
      float *the_float, min, max;
  public:
    TFloatEdit(menu_item_t *my_menu);
    ~TFloatEdit();
    virtual int valid_chars(int sym, int unicode);
    virtual int can_exit();
    int get_len_max_options() { return 10; } // minimum field size !
};

#endif
