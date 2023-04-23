#ifndef CONSOLE_H
#define CONSOLE_H

#ifdef __cplusplus

#include "gui/tconsole.h"
#include "parser.h"

class TRaineConsole : public TConsole
{
  char last_cmd[80];
  int pointer_on, pointer_n, pointer_x,pointer_top,pointer_rows,pointer_end;
  int dump_cols;
  public:
    TRaineConsole(char *my_title, char *init_label, int maxlen, int maxlines, commands_t *mycmd,int is_visible = 1) : TConsole(my_title,init_label,maxlen,maxlines,mycmd,is_visible)
    {
	load_history();
      pointer_on = 0;
      dump_cols = ((maxlen-7)/4)&~7;
      use_transparency = 0;
    }
    ~TRaineConsole() {
	save_history();
    }
    virtual void  execute();
    void load_history();
    void save_history();
    virtual void handle_mouse(SDL_Event *event);
    virtual void unknown_command(int argc, char **argv);
    virtual int run_cmd(char *field, int interactive=1);
    virtual void post_print() {
      pointer_on = 0; // erase mouse cursor after a print
    }
    int get_dump_cols() { return dump_cols; }
};

extern "C" {
extern TRaineConsole *cons;
int get_cpu_id();
void set_cpu_id(int cpu);
char *get_cpu_name_from_cpu_id(int cpu_id);
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

int do_console(int sel);
void preinit_console();
void done_console();
void run_console_command(char *command);
void do_regs(int argc, char **argv);
void do_lua(int argc, char **argv);
#ifdef __cplusplus
UINT8 *get_ptr(UINT32 addr, UINT32 *the_block = NULL);

}
#endif
#endif
