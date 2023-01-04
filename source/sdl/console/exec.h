#ifdef __cplusplus
extern "C" {
#endif

void do_cond(int argc, char **argv);
void do_step(int argc, char **argv);
void do_next(int argc, char **argv);
void do_irq(int argc, char **argv);
void do_list(int argc, char **argv);
void done_breakpoint();
int check_irq(UINT32 adr);
int check_breakpoint();
void do_print_ingame(int args, char **argv);
void do_break(int argc, char **argv);
void restore_breakpoints();
void done_breakpoints();
void do_until(int argc, char **argv);
void do_set_save_slot(int argc, char **argv);
void cons_save_state(int argc, char **argv);
void cons_load_state(int argc, char **argv);
void disp_instruction();

#ifdef __cplusplus
}
#endif
