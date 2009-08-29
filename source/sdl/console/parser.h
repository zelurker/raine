#ifndef PARSER_H
#define PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

int parse(char *expr);
void get_regs(int cpu);
void set_regs(int cpu);

#ifdef __cplusplus
}
#endif
#endif
