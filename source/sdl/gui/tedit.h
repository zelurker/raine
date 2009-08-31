#ifndef TEDIT_H
#define TEDIT_H

#include "widget.h"

#define MAX_HISTORY 30

class TEdit : public TStatic
{
  private:
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
      field = menu->values_list_label[0];
      maxl = menu->values_list[0];
      use_hist = menu->values_list[1];
      pos = strlen(field);
      cursor_on = 0;
      used_hist = update_count = 0;
      current_hist = -1;
      history[MAX_HISTORY-1] = NULL;
      pos_cursor = old_pos = 0;
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
    virtual int get_len() {
      return maxl;
    }
    virtual int get_height(TFont *font);
    virtual int get_width(TFont *font);
    // virtual int handle_mouse(SDL_Event *event);
    virtual int handle_key(SDL_Event *event);
    virtual void update();
    virtual void add_history();
    virtual void insert(char *s);
    virtual int can_be_selected() { return 1; }
};

#endif
