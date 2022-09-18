#ifdef RAINE_UNIX // for linux only !
#ifndef LEDS_H
#define LEDS_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_LEDS 4 // so far up to 4

void done_led(int n);
void done_leds();
char *get_led_name(int n);
void init_led(int n, char *path);
// switch_led prototype is in control.h
void read_leds_config();
void write_leds_config();

#ifdef __cplusplus
}
#endif
#endif
#endif
