#ifdef __cplusplus
extern "C" {
#endif

#include "gui/menuitem.h"

void init_scripts();
void done_scripts();
void add_scripts(menu_item_t *menu);
void update_scripts();
void do_script(int argc, char **argv);
void do_start_script(int argc, char **argv);
char *get_script_comment(int n);
void stop_scripts();
int get_running_script_info(int *nb, int *line, char **sect);
char* get_script_title(int n);
void stop_script(int n);
int is_script_parsing();
void get_script_parsed(int n, int line, int *myargc, char ***myargv,void (**mycmd)(int, char **) );
void script_set_parsed(int n, int line, int argc, char **argv, void (*cmd)(int, char **) );

extern int nb_scripts;

#ifdef __cplusplus
}
#endif
