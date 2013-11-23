#include <string.h>
#include "../gui/menu.h"
#include "sasound.h"
#include "games.h"
#include "z80/mz80help.h"
#include "timer.h"
#include "cpumain.h"
#include "raine.h"
#include "sdl/dialogs/messagebox.h"
#include "profile.h"
#include "files.h"
#include "sound/assoc.h"
#include "sdl/dialogs/fsel.h"

static TMenu *menu;
static int command;

static int test_command(int sel) {
    latch = command;
    cpu_int_nmi(audio_cpu);
    return 0;
}

static void send(int cmd) {
    command = cmd;
    test_command(0);
    execute_z80_audio_frame();
}

static int stop(int sel) {
    int old = command;
    if (supports_sound_assoc == 2) { // gunbird
	send(1);
    } else { // default : neogeo
	send(3); // stop / music mode for mslug
	if (supports_sound_assoc == 3) { // except mslug/2/3/x/4
	    send(7); // music mode for 3countb and most neogeo games
	}
    }
    command = old;
    return 0;
}

class TSoundCmd : public TMenu {
  public:
    TSoundCmd(char *title,menu_item_t *menu) :
      TMenu(title,menu)
      {}
    void update_fg_layer(int nb_to_update) {
	execute_z80_audio_frame();
	execute_z80_audio_frame();
	TMenu::update_fg_layer(nb_to_update);
    }
};

// This should be placed somewhere more useful... !
static char *strrstr(char *path, char *sub) {
    if (!path || !*path) return NULL;
    char *s=path,*old;
    int found = 0;
    do {
	old = s;
	s = strstr(old+1,SLASH);
	if (s) found = 1;
    } while (s);
    if (found) return old;
    return NULL;
}

static int associate(int sel) {
    static char path[FILENAME_MAX+1];
    char res[FILENAME_MAX+1];
    char *exts[] = { ".mp3",
	".ogg", ".flac", ".wav",
	// extensions supported by modplug, from the doc...
	".mod", ".s3m", ".xm", ".it", ".669", ".amf", ".ams", ".dbm", ".dmf",
	".dsn", ".far", ".mdl", ".med", ".mtm", ".okt", ".ptm", ".stm", ".ult",
	".umx", ".mt2", ".psm",
	// And compressed versions...
	".mdz",".s3z", ".xmz",".itz",
	".mdr",".s3r", ".xmr",".itr",
	".mdgz",".s3gz", ".xmgz",".itgz",
	NULL };
    char *s = get_assoc(command);
    stop(0);
    if (s && *s) {
	strcpy(path,s);
	strcpy(res,s);
	s = strrstr(path,SLASH);
	if (s) *s = 0;
    } else
	*res = 0;
    fsel(path,exts,res,"Choose audio track");
    if (res[strlen(res)-1] == SLASH[0])
	assoc(command,"");
    else {
	assoc(command,res);
	strcpy(path,res);
	s = strrstr(path,SLASH);
	if (s) *s = 0;
    }
    return 0;
}

static menu_item_t *asso = NULL;
static int redraw_assoc;

int menu_asso(int sel) {
    int cmd = asso[sel].values_list[0];
    int old;
    int nb = MessageBox("Question","What do you want to do ?",
	    "Change track|Remove association|Silence");
    switch (nb) {
    case 1:
	old = command;
	command = cmd;
	associate(0);
	command = old;
	break;
    case 2:
	del_assoc(cmd);
	break;
    case 3:
	assoc(cmd,"");
	break;
    }
    redraw_assoc = 1;
    return 1;
}

static int associations(int sel) {
    do {
	redraw_assoc = 0;
	int nb = 0,bidon;
	for (int cmd=2; cmd<256; cmd++) {
	    char *s = get_assoc(cmd);
	    if (s) {
		asso = (menu_item_t*)realloc(asso,sizeof(menu_item_t)*(nb+1));
		char key[4];
		sprintf(key,"%xh",cmd);
		asso[nb].label = strdup(key);
		asso[nb].menu_func = &menu_asso;
		asso[nb].value_int = &bidon;
		asso[nb].values_list_size = 1;
		asso[nb].values_list[0] = cmd;
		if (*s) {
		    char *s2 = strrstr(s,SLASH);
		    asso[nb++].values_list_label[0] = strdup(s2 ? s2+1 : s);
		} else
		    asso[nb++].values_list_label[0] = strdup("Silence");
	    }
	}
	asso = (menu_item_t*)realloc(asso,sizeof(menu_item_t)*(nb+1));
	asso[nb].label = NULL;
	if (!nb) {
	    MessageBox("Error","No associations yet","OK");
	    return 0;
	}
	TMenu *dlg = new TMenu("Sound associations",asso);
	dlg->execute();
	delete dlg;
	for (int n=0; n<nb; n++) {
	    free((void*)asso[n].label);
	    free(asso[n].values_list_label[0]);
	}
	free(asso);
    } while (redraw_assoc);
    return 0;
}

static menu_item_t sound_menu[] =
{
    { "Music", NULL, &command, 3, {32, 57, 1} },
    { "Test", &test_command },
    { "Stop", &stop },
    { "Associate...", &associate },
    { "Associations", &associations },
  { NULL },
};

int do_sound_cmd(int sel) {
    if (!GameSound || !RaineSoundCard)
	return 0;
    int ticks = dwElapsedTicks;
    int n = 0;
#ifdef RAINE_DEBUG
    sound_menu[0].values_list_size = 0;
    sound_menu[0].values_list_size++;
    sound_menu[0].values_list[n++] = 0;
    sound_menu[0].values_list_size++;
    sound_menu[0].values_list[n++] = 1;
    sound_menu[0].values_list_size++;
    sound_menu[0].values_list[n++] = 3;
    sound_menu[0].values_list_size++;
    sound_menu[0].values_list[n++] = 4;
    sound_menu[0].values_list_size++;
    sound_menu[0].values_list[n++] = 7;
    sound_menu[0].values_list_size++;
    sound_menu[0].values_list[n++] = 0x11;
#endif
    if (supports_sound_assoc == 1) { // neogeo has music 2 too !
	sound_menu[0].values_list_size++;
	sound_menu[0].values_list[n++] = 2;
    }
    for (int x=32; x<=0x3f; x++) {
	char hex[4];
	sprintf(hex,"%x",x);
	sound_menu[0].values_list[n] = x;
	// It's bothersome to allocate strings just to be able to display in
	// hex, but here I really don't see how to add the option to have an
	// hex value in menuitem_t... !
	sound_menu[0].values_list_label[n++] = strdup(hex);
	sound_menu[0].values_list_size++;
    }
    command = sound_menu[0].values_list[0];

    reset_ingame_timer(); // For sound we'd better init this here...
    start_cpu_main();
    saInitSoundCard( RaineSoundCard, audio_sample_rate );
    if (is_neocd())
	sa_unpause_sound();
    execute_z80_audio_frame(); // start the z80 if it was not already done !
    stop(0);
    menu = new TSoundCmd("Sound commands", sound_menu);
    menu->execute();
    delete menu;
    stop(0);
    if (is_neocd())
	sa_pause_sound();
    else
	saDestroySound(0);
    stop_cpu_main();
    stop_ingame_timer();

    for (int x=0; x<sound_menu[0].values_list_size; x++) {
	if (sound_menu[0].values_list_label[x])
	    free(sound_menu[0].values_list_label[x]);
    }
    dwElapsedTicks = ticks;
    return 0;
}
