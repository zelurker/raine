#ifndef PARSER_H
#define PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

extern int get_cpu_id();
int parse(char *expr);
void get_regs(int cpu = get_cpu_id());
void set_regs(int cpu = get_cpu_id());

#ifdef __cplusplus
}
#endif
#endif
