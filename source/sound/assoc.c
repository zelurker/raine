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
#include "raine.h" // Z80ROM
#include "games.h" // current_game
#include "ingame.h" // print_ingame

static int type,adr;
static char *track[256];
int show_song;

void init_assoc(int kind) {
    if (kind == 1) { // neogeo
	/* Some roms have a version + an author in them, but apparently
	 * there are some variants, so they must be recognized on something
	 * else. So I just check the instruction, it must be ld (ld),adr
	 * ($21) */
	if (!strncmp((char*)&Z80ROM[0x3E],"Sound Driver(ROM)Ver 1.8",24))
	    type = 1; // garou
	else if (!strncmp((char*)&Z80ROM[0x3e],"Ver 3.0 by MAKOTO",17)) {
	    adr =0x1c7; // galaxyfg
	    if (Z80ROM[adr-1] != 0x21)
		adr = 0x1d0;
	    if (Z80ROM[adr-1] != 0x21) {
		type = 0;
		printf("galaxyfg variant not recognized\n");
		return;
	    }
	    type = 2; // galaxyfg
	    adr = ReadWord(&Z80ROM[adr]);
	} else if (!strncmp((char*)&Z80ROM[0x101],"SYSTEM",6))
	    type = 3; // sonicwi2/3
	else if (!strncmp(current_game->main_name,"mslug",5))
	    type = 4; // mslug, except mslug4/5
	else if (!strncmp((char*)&Z80ROM[0x3e],"Ver 2.0a by MAKOTO",18)) {
	    adr = 0x189; // mutnat
	    if (Z80ROM[adr-1] != 0x21)
		adr = 0x173;
	    if (Z80ROM[adr-1] != 0x21) {
		type = 0;
		printf("mutnat variant not recognized\n");
		return;
	    }
	    type = 5; // mutnat : variation of galaxyfg
	    adr = ReadWord(&Z80ROM[adr]);
	}
    } else if (kind == 2)
	type = 10; // gunbird
}

int get_assoc_adr() {
    return adr;
}

int get_assoc_type() {
    return type;
}

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
    type = 0;
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

static void show(int song) {
    print_ingame(600,"Song %xh",song);
}

int handle_sound_cmd(int cmd) {
    switch (type) {
    case 4:
	// all the mslug games support sound modes. The default is music after
	// a reset or command 0. Command 3 sets music mode. Command $11 sets
	// sounds mode, and there are some other unknown commands < 0x20
	// This could get extremely messy if each games had its own mode
	// numbers, but apparently it's not the case for most of them at least.
	if (cmd == 0 || cmd == 3) {
	    mode = music;
	} else if (cmd < 0x20 && cmd != 2) {
	    mode = sound;
	}
	if (mode == sound) return 0;
	if (cdda.playing && (cmd == 1 || cmd == 3 || cmd == 2 || cmd >= 0x20))
	    cdda.playing = 0;
	if (show_song && cmd >= 0x20) show(cmd);
	break;
    case 3: // sonicwi2 / sonicwi3
	if (cdda.playing && (cmd == 3 || cmd < Z80ROM[0x30d]))
	    cdda.playing = 0;
	else if (show_song && cmd >= 0x20 && cmd < Z80ROM[0x30d])
	    show(cmd);
	break;
    case 2: // galaxyfg
    case 5: // mutnat
	if (cmd == 7) mode = music;
	else if (cmd == 0x1c) mode = sound;
	if (mode == sound) {
	    // mode reset after just 1 byte !
	    if (cmd >= 0x20) mode = music;
	    return 0;
	}
	if (cdda.playing && (cmd == 3 || Z80ROM[adr + cmd] == 2))
	    cdda.playing = 0;
	else if (show_song && Z80ROM[adr + cmd] == 2)
	    show(cmd);
	break;
    case 1: // garou
	// Garou has modes + interruptable songs !
	if (cmd == 7) mode = music;
	else if (cmd == 0x1c) mode = sound;
	if (mode == sound) return 0;
	if (cdda.playing && (cmd == 4 ||
		    (cmd >= 0x20 && Z80ROM[0x3038 + cmd - 0x20] == 2)))
	    cdda.playing = 0;
	else if (show_song && cmd >= 20 && Z80ROM[0x3038 + cmd - 0x20] == 2)
	    show(cmd);
	break;
    default:
	if (cdda.playing && cmd < 0x40)  // gunbird
	    cdda.playing = 0;
	else if (show_song && cmd < 0x40)
	    show(cmd);
    }
    if (type < 10 && cmd == 2 && show_song)
	show(cmd);
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

