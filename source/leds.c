// Leds usage for linux based on the kernel interface in /sys/class/leds

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leds.h"
#include "confile.h"
#ifdef MEMWATCH
// to avoid "wild free" messages in the end...
#include "memwatch.h"
#endif

static char *led[MAX_LEDS];
static int orig_status[MAX_LEDS], status[MAX_LEDS];

void read_leds_config() {
    for (int n=0; n<MAX_LEDS; n++) {
	char tmp[80];
	sprintf(tmp,"led%d",n);
	char *s = raine_get_config_string("leds",tmp,NULL);
	if (s)
	    init_led(n,s);
    }
}

void write_leds_config() {
    for (int n=0; n<MAX_LEDS; n++) {
	if (led[n]) {
	    char tmp[80];
	    sprintf(tmp,"led%d",n);
	    raine_set_config_string("leds",tmp,led[n]);
	}
    }
}

void done_led(int n) {
    if (led[n]) {
	FILE *f = fopen(led[n],"w");
	if (f) { // Just in case we lost write access during runtime !
	    fprintf(f,"%d",orig_status[n]);
	    fclose(f);
	}
	free(led[n]);
	led[n] = NULL;
    }
}

char *get_led_name(int n) {
    // returns allocated string, free it !
    if (!led[n]) return NULL;
    char *tmp;
    tmp = strdup(led[n]+16);
    char *slash = strchr(tmp,'/');
    *slash = 0;
    return tmp;
}

void init_led(int n, char *path) {
    if (n >= MAX_LEDS)
	return;
    char tmp[FILENAME_MAX];
    if (path[0] == '/') {
	if (strncmp(path,"/sys/class/leds/",16))
	    return; // sanity check, all the paths here should start by that
	strncpy(tmp,path,FILENAME_MAX);
    } else {
	snprintf(tmp,FILENAME_MAX,"/sys/class/leds/%s/brightness",path);
    }
    if (access(tmp,W_OK))
	return; // no write access -> no init
    char buf;
    FILE *f = fopen(tmp,"r");
    if (!f) return;
    if (led[n]) done_led(n);
    fread(&buf,1,1,f);
    if (buf == '0') orig_status[n] = 0;
    else orig_status[n] = 1;
    fclose(f);
    f = fopen(tmp,"w");
    if (!f) return;
    fprintf(f,"0");
    fclose(f);
    led[n] = strdup(tmp);
}

void done_leds() {
    for (int n=0; n<MAX_LEDS; n++) {
	done_led(n);
    }
}

void switch_led(int a, int b) {
    if (a < MAX_LEDS && led[a] && status[a] != b) {
	FILE *f = fopen(led[a],"w");
	if (f) {
	    if (b)
		fprintf(f,"1");
	    else
		fprintf(f,"0");
	    fclose(f);
	    status[a] = b;
	}
    }
}
