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
#include "savegame.h"

// active : same role as cdda.playing, except that neocd and neogeo share the
// same code, so if we use cdda.playing, neocd tracks are stopped very very
// quickly !!!
static int type,adr,active;
static char *track[256],loop[256];
int show_song;
enum {
    MUSIC=0,
    SOUND,
    ONE_SOUND
};
static int mode;

void init_assoc(int kind) {
    if (kind == 1) { // neogeo
	/* Some roms have a version + an author in them, but apparently
	 * there are some variants, so they must be recognized on something
	 * else. So I just check the instruction, it must be ld (ld),adr
	 * ($21) */
	if (!strncmp((char*)&Z80ROM[0x3e],"Ver 3.0 by MAKOTO",17)) {
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
	    type = 2; // mutnat : variation of galaxyfg
	    adr = ReadWord(&Z80ROM[adr]);
	} else if (!strncmp((char*)&Z80ROM[0x3e],"Sound Driver Ver 0.1 ",21)) {
	    adr = 0x14f;
	    if (Z80ROM[adr-1] != 0x21) {
		type = 0;
		printf("kof96 variant not recognized\n");
		return;
	    }
	    type = 1; // kof96 = like garou
	    adr = ReadWord(&Z80ROM[adr]);
	} else if (!strncmp((char*)&Z80ROM[0x3e],"Sound Driver Ver 1.1 ",21)) {
	    adr = 0x17d;
	    if (Z80ROM[adr-1] != 0x21) {
		type = 0;
		printf("kof97 variant not recognized\n");
		return;
	    }
	    type = 1; // kof97 = like garou
	    adr = ReadWord(&Z80ROM[adr]);
	} else if (!strncmp((char*)&Z80ROM[0x3E],"Sound Driver(ROM)Ver 1.7",24) ||
		!strncmp((char*)&Z80ROM[0x3E],"Sound Driver(ROM)Ver 1.8",24)) {
	    adr = 0x184;
	    if (Z80ROM[adr-1] != 0x21) {
		type = 0;
		printf("kof98/garou variant not recognized\n");
		return;
	    }
	    type = 1; // kof98 / garou
	    adr = ReadWord(&Z80ROM[adr]);
	}
    } else if (kind == 2)
	type = 10; // gunbird
    if (type == 1) mode = MUSIC;
    if (type) {
	prepare_cdda_save(ASCII_ID('T','R','C','K'));
	AddSaveData(ASCII_ID('T','R','C','A'),(UINT8*)&active,sizeof(active));
    }
}

int get_asso_loop(int cmd) {
    return loop[cmd];
}

void set_asso_loop(int cmd, int lp) {
    loop[cmd] = lp;
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
	    char name[20];
	    sprintf(name,"loop%d",cmd);
	    raine_set_config_int(section,name,get_asso_loop(cmd));
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
	if (s) {
	    assoc(cmd,s);
	    char name[20];
	    sprintf(name,"loop%d",cmd);
	    int loop = raine_get_config_int(section,name,0);
	    set_asso_loop(cmd,loop);
	}
    }
}

static void show(int song) {
    print_ingame(600,"Song %xh",song);
}

static void mute_song() {
    // It's quite a hassle to change the 2 variables together, but on neocd
    // active != cdda.playing since they are started by the game itself !
    active = 0;
    cdda.playing = 0;
}

int handle_sound_cmd(int cmd) {
    switch (type) {
    case 4:
	// all the mslug games support sound modes. The default is MUSIC after
	// a reset or command 0. Command 3 sets MUSIC mode. Command $11 sets
	// sounds mode, and there are some other unknown commands < 0x20
	// This could get extremely messy if each games had its own mode
	// numbers, but apparently it's not the case for most of them at least.
	if (cmd == 0 || cmd == 3) {
	    mode = MUSIC;
	} else if (cmd < 0x20 && cmd != 2) {
	    mode = SOUND;
	}
	if (mode == SOUND) return 0;
	if (active && (cmd == 1 || cmd == 3 || cmd == 2 || cmd >= 0x20))
	    mute_song();
	if (show_song && cmd >= 0x20) show(cmd);
	break;
    case 3: // sonicwi2 / sonicwi3
	if (active && (cmd == 3 || (cmd >= 0x20 && cmd < Z80ROM[0x30d])))
	    mute_song();
	else if (show_song && cmd >= 0x20 && cmd < Z80ROM[0x30d])
	    show(cmd);
	break;
    case 2: // galaxyfg
	if (cmd == 7) mode = MUSIC;
	else if (cmd == 0x1c) mode = SOUND;
	if (mode == SOUND) {
	    // mode reset after just 1 byte !
	    if (cmd >= 0x20) mode = MUSIC;
	    return 0;
	}
	if (active && (cmd == 3 || Z80ROM[adr + cmd] == 2))
	    mute_song();
	else if (show_song && Z80ROM[adr + cmd] == 2)
	    show(cmd);
	break;
    case 1: // garou
	// Garou has modes + interruptable songs !
	if (cmd >= 6 && cmd <= 9) mode = MUSIC;
	else if (cmd >= 0x15 && cmd < 0x1f && mode != ONE_SOUND) {
	    mode = ONE_SOUND;
	    return 0;
	}
	else if (cmd < 0x20 && cmd != 2 && cmd != 3 && cmd != 1 &&
	       mode != ONE_SOUND)
	    mode = SOUND;
	if (mode == SOUND) return 0;
	if (mode == ONE_SOUND) {
	    // Eats the next byte as sound, then switch to music
	    mode = MUSIC;
	    return 0;
	}
	if (active && (cmd == 4 ||
		    (cmd >= 0x20 && Z80ROM[adr + cmd - 0x20] == 2)))
	    mute_song();
	else if (show_song && cmd >= 20 && Z80ROM[adr + cmd - 0x20] == 2)
	    show(cmd);
	break;
    default:
	if (active && cmd < 0x40)  // gunbird
	    mute_song();
	else if (show_song && cmd < 0x40)
	    show(cmd);
    }
    if (type < 10 && cmd == 2 && show_song)
	show(cmd);
    if (cmd > 1 && track[cmd]) {
	// An association to an empty track allows to just forbid playing this
	// MUSIC
	if (*track[cmd] && exists(track[cmd])) {
	    cdda.track = cmd; // for restoration
	    cdda.skip_silence = 1;
	    load_sample(track[cmd]);
	    cdda.loop = loop[cmd];
	    active = 1;
	    cdda.playing = 1;
	}
	return 1;
    }
    return 0;
}

