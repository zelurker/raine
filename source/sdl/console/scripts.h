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

extern int nb_scripts;

#ifdef __cplusplus
}
#endif
