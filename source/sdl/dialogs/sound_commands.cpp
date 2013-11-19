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
	send(1); // stop sound
	send(3); // start
	send(0); // ??? - required to play the songs
	if (!strstr(current_game->main_name,"mslug")) {
	    send(4); // 3countb : ???
	    send(0); // ???
	    send(7); // ???
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

static menu_item_t sound_menu[] =
{
    { "Music", NULL, &command, 3, {32, 57, 1} },
    { "Test", &test_command },
    { "Stop", &stop },
  { NULL },
};

int do_sound_cmd(int sel) {
    if (!GameSound || !RaineSoundCard)
	return 0;
    int ticks = dwElapsedTicks;
    sound_menu[0].values_list_size = 57-32+1;
    int n = 0;
    if (supports_sound_assoc == 1) { // neogeo has music 2 too !
	sound_menu[0].values_list_size++;
	sound_menu[0].values_list[n++] = 2;
    }
    for (int x=32; x<=57; x++) {
	char hex[4];
	sprintf(hex,"%x",x);
	sound_menu[0].values_list[n] = x;
	// It's bothersome to allocate strings just to be able to display in
	// hex, but here I really don't see how to add the option to have an
	// hex value in menuitem_t... !
	sound_menu[0].values_list_label[n++] = strdup(hex);
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
