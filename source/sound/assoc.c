#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "sound/assoc.h"
#include "confile.h"
#include "neocd/cdda.h"
#include "sasound.h"
#ifdef SDL
#include "sdl/compat.h" // exists
#endif

int supports_sound_assoc;
static char *track[256];

char* get_assoc(int cmd) {
    return track[cmd];
}

void assoc(int cmd, char *t) {
    if (!track[cmd])
	if (!(track[cmd] = malloc(FILENAME_MAX+1))) return;
    strncpy(track[cmd],t,FILENAME_MAX);
    track[cmd][FILENAME_MAX] = 0;
}

void del_assoc(int cmd) {
    if (track[cmd]) {
	free(track[cmd]);
	track[cmd] = NULL;
    }
}

void save_assoc(char *section) {
    int cmd;
    for (cmd=1; cmd<256; cmd++)
	if (track[cmd]) {
	    char key[5];
	    sprintf(key,"%d",cmd);
	    raine_set_config_string(section,key,track[cmd]);
	    del_assoc(cmd);
	}
    cdda.playing = 0; // just to be sure
}

void load_assoc(char *section) {
    int cmd;
    for (cmd=1; cmd<256; cmd++) {
	char key[5];
	sprintf(key,"%d",cmd);
	char *s = raine_get_config_string(section,key,NULL);
	if (s)
	    assoc(cmd,s);
    }
}

enum {
    music=0,
    sound
};
static int mode;

int handle_sound_cmd(int cmd) {
    if (supports_sound_assoc == 1) {
	// all the mslug games support sound modes. The default is music after
	// a reset or command 0. Command 3 sets music mode. Command $11 sets
	// sounds mode, and there are some other unknown commands < 0x20
	// This could get extremely messy if each games had its own mode
	// numbers, but apparently it's not the case for most of them at least.
	if (cmd == 0 || cmd == 3) {
	    mode = music;
	}
	if (cmd == 0x11) {
	    mode = sound;
	}
	if (cdda.playing && (cmd == 1 || cmd == 3))
	    cdda.playing = 0;
	if (mode == sound) return 0;
    } else if (supports_sound_assoc == 3) { // other neogeo games
	if (cmd == 3) cdda.playing = 0;
    }
    if (cdda.playing &&
	    ((supports_sound_assoc == 2 && cmd <= 0x40) || // gunbird
	     (supports_sound_assoc != 2 && (cmd >= 0x20 && cmd <= 0x40)))) {
	cdda.playing = 0;
    }
    if (cmd > 1 && track[cmd]) {
	// An association to an empty track allows to just forbid playing this
	// music
	if (*track[cmd] && exists(track[cmd])) {
	    load_sample(track[cmd]);
	    cdda.playing = 1;
	}
	return 1;
    }
    return 0;
}

