#ifndef TCONSOLE_H
#define TCONSOLE_H

#if SDL==2
#include "sdl2/gui/menu.h"
#else
#include "menu.h"
#endif
#include "tedit.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
  char *name;
  void (*handler)(int, char **);
  char *help, *long_help;
} commands_t;

UINT32 ansilen(const char *s);
int ansi_pos(char *s,int pos);
char* ansistr(char *s,int n);
void split_command(char *field, char **argv, int *argc, int max,int strip_quotes = 0);

#ifdef __cplusplus
}

#include <stddef.h>
#include <stdarg.h>

class ConsExcept {
  private:
    char str[256];
  public:
    ConsExcept(char *msg) {
      strcpy(str,msg);
    }
    ConsExcept(char *format, ...) {
      va_list ap;
      va_start(ap,format);
      vsprintf(str,format,ap);
      va_end(ap);
    }
    char *what() { return str; }
};

typedef void (*tinteractive)(char *);

extern int key_console;

class TConsole : public TDialog
{
  protected:
    int nb_alloc, nb_used;
    int unicode,visible;
    menu_item_t edit_menu;
    TEdit *edit_child;
    TStatic **last_child;
    unsigned int len_max;
    commands_t *commands;
    char *field,oldfield;
    // results from the last call to run_cmd
    commands_t *cmd;
    int argc; char *argv[50];

    // handler is for internal use only, not supposed to be called from the
    // outside
    virtual int handler(int cause);
  public:
    tinteractive interactive;
    TConsole(char *my_title, char *init_label, int maxlen, int maxlines, commands_t *mycmd, int is_visible = 1);
    ~TConsole();
    // virtual void compute_nb_items();
    virtual void print(const char *format, ...);
    virtual void post_print(); // easier than overloading print
    virtual void setup_fg_layer();
    virtual int get_fglayer_footer_height();
    virtual int get_fglayer_footer_len();
    virtual void display_fglayer_footer(int x,int &y,int w, int xoptions);
    virtual void fglayer_footer_update();
    virtual int fglayer_footer_handle_key(SDL_Event *event);
    virtual void fglayer_footer_call_handler();
    virtual void unknown_command(int argc, char **argv);
    virtual void do_help(int argc, char **argv);
    virtual void draw();
    void set_interactive(void (*my_interactive)(char *)) {
      interactive = my_interactive;
    }
    void end_interactive() {
      interactive = NULL;
    }
    virtual void  execute();
    virtual int run_cmd(char *field,int myinteractive=1);
    void set_visible() { visible = 1; }
    int is_visible() { return visible; }
    void get_parsed_info(int *myargc, char ***myargv,void (**mycmd)(int, char **) );
    void finish_parsed_info();
    int parse_cmd(char *string);
};
#endif
#endif

