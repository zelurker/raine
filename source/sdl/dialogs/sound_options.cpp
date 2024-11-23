#include "../gui/menu.h"
#include "sasound.h"
#if HAS_ES5506
#include "es5506.h"
#endif
#include "neocd/neocd.h"
#include "neocd/cdda.h"
#include "games.h"
#include "dialogs/sound_commands.h"
#include "dialogs/messagebox.h"
#include "sound/assoc.h"
#include "compat.h"
#include "gui.h"

static TMenu *menu;

#if HAS_NEO
static int set_default_volumes(int sel) {
  sfx_volume = DEFAULT_SFX_VOLUME;
  music_volume = DEFAULT_MUSIC_VOLUME;
  menu->draw();
  mute_music = mute_sfx = 0;
  return 0;
}
#endif

static void init_sound_driver(int changed);
static int driver_id,scard_id;

#if SDL == 2
static int choose_driver(int sel) {
    char buf[512];
    const char *current = SDL_GetCurrentAudioDriver();
    buf[0] = 0;
    for (int i = 0; i < SDL_GetNumAudioDrivers(); ++i) {
        snprintf(&buf[strlen(buf)],512-strlen(buf),"%s|", SDL_GetAudioDriver(i));
    }
    buf[strlen(buf)-1] = 0; // remove last |
    int ret = raine_mbox("Sound driver",_("Choose one of the available drivers:"),buf);
    if (ret > 0 && strcmp(SDL_GetAudioDriver(ret-1),current)) {
	const char *driver = SDL_GetAudioDriver(ret-1);
	snprintf(buf,512,"SDL_AUDIODRIVER=%s",driver);
	putenv(buf);
	saDestroySound(1);
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	ret = SDL_InitSubSystem(SDL_INIT_AUDIO);
	if (ret < 0) {
	    raine_mbox("Error",_("Could not even initialize the audio subsystem with this driver\nForget it"),"Ok");
	    snprintf(buf,512,"SDL_AUDIODRIVER=%s",current);
	    putenv(buf);
	    ret = SDL_InitSubSystem(SDL_INIT_AUDIO);
	    if (ret < 0) {
		fatal_error("We lost the audio subsystem, bye!");
	    }
	}
	const char *driver2 = SDL_GetCurrentAudioDriver();
	if (strcmp(driver,driver2))
	    raine_mbox("Error","couldn't change the sound driver !\nTry before starting any game","ok");
    }
    init_sound_driver(1);
    return 0;
}

extern "C" SDL_AudioSpec gotspec; // streams.c

static int choose_card(int sel) {
    if (scard_id > 1)
	RaineSoundCard = (char*)SDL_GetAudioDeviceName(scard_id-2,0);
    else if (scard_id == 0)
	RaineSoundCard = "None";
    else if (scard_id == 1) // Autodetect
	RaineSoundCard = "Autodetect";
    detect_soundcard();
    if (gotspec.freq) {
	audio_sample_rate = gotspec.freq;
	if (menu)
	    menu->draw();
    }
    return 0;
}
#endif

menu_item_t sound_menu[] =
{
#if SDL == 2
    { _("Sound driver"), &choose_driver,&driver_id  },
  { _("Sound device"), &choose_card, &scard_id, 1, { 0 }, { _("No") } },
#endif
  { _("Sample rate"), NULL, &audio_sample_rate, 4, { 11025, 22050, 44100, 48000 }},
#if HAS_ES5506
  { _("ES5506 voice filters"), NULL, &es5506_voice_filters, 2, { 0, 1 }, { _("No"), _("Yes") } },
#endif
#if HAS_NEO
{ _("Always stop Neo-Geo CD music while loading"), NULL, &auto_stop_cdda, 2, { 0, 1 }, { _("No"), _("Yes") } },
  { _("Sound effects volume"), NULL, &sfx_volume, 3, { 0, 200, 10 }, },
  { _("Music volume"), NULL, &music_volume, 3, { 0, 200, 10 }, },
  { _("Raw audio tracks format"), NULL, &neocd_cdda_format, 2, { AUDIO_S16LSB, AUDIO_S16MSB }, { _("Less significant byte"),_("Most significant byte") }},
  { _("Mute sound effects"), NULL, &mute_sfx, 2, { 0, 1 }, { _("No"), _("Yes") } },
  { _("Mute music"), NULL, &mute_music, 2, { 0, 1 }, { _("No"), _("Yes") } },
  { _("Mute sfa3 announcer"), NULL, &mute_sfa3_speaker, 2, {0, 1}, { _("No"), _("Yes") } },
  { _("Default volumes"), &set_default_volumes },
#endif
  { _("Record to raine_sound.wav"), NULL, &recording, 3, { 0, 1, 2 }, { _("No"), _("Without monitoring"), _("With monitoring") } },
#if HAS_NEO
{ _("Sound associations..."), &do_sound_cmd },
#endif
  { NULL },
};

#if SDL == 2
static void init_sound_driver(int changed) {
    sound_menu[0].values_list_size = SDL_GetNumAudioDrivers();
    const char *driver = SDL_GetCurrentAudioDriver();
    for (int i = 0; i < SDL_GetNumAudioDrivers(); ++i) {
        sound_menu[0].values_list_label[i] = (char*)SDL_GetAudioDriver(i);
        sound_menu[0].values_list[i] = i;
	if (!strcmp(driver,SDL_GetAudioDriver(i)) && changed) {
	    // If the driver was just changed and it has a default audio frequency, then change audio_sample_rate
	    driver_id = i;
	}
    }
    // Must also init the names of the devices, they depend on the driver... !
    detect_soundcard();
    scard_id = 1; // Choose Autodetect if soundcard not found
    for (int n=0; n<devs_audio; n++) {
	const char *name2 = SDL_GetAudioDeviceName(n,0);
	int l = strlen(name2);
	while (name2[l-1] == ' ' && l>0)
	    l--; // skip trailing spaces, because they are removed when reading a string value from the config file !
	if (!strncmp(name2,RaineSoundCard,l)) {
	    scard_id = n+2;
	}
    }

    sound_menu[1].values_list_size = devs_audio+2;
    sound_menu[1].values_list_label[0] = "None";
    if (menu)
	menu->update_list_label(1,0,_("None"));
    sound_menu[1].values_list_label[1] = "Autodetect";
    sound_menu[1].values_list[0] = 0;
    sound_menu[1].values_list[1] = 1;
    if (menu)
	menu->update_list_label(1,1,_("Autodetect"));
    for (int i = 0; i < devs_audio; i++) {
	if (sound_menu[1].values_list_label[i+2])
	    free(sound_menu[1].values_list_label[i+2]);
	sound_menu[1].values_list_label[i+2] = strdup(SDL_GetAudioDeviceName(i, 0));
	sound_menu[1].values_list[i+2] = i+2;
	if (menu) menu->update_list_label(1,i+2,sound_menu[1].values_list_label[i+2]);
    }
    if (menu)
	menu->draw();
}
#endif

class TSoundDlg : public TMenu {
  public:
    TSoundDlg(char *title,menu_item_t *menu) :
      TMenu(title,menu)
      {}

    int can_be_selected(int sel) {
#if HAS_NEO
	if (sound_menu[sel].menu_func == &do_sound_cmd)
	    return get_assoc_type() != 0;
#endif
      return TMenu::can_be_selected(sel);
    }
};

int do_sound_options(int sel) {
    int old = recording;
    if (current_game) {
#if SDL == 2
	rdesktop->end_preinit();
#endif
	saDestroySound(0);
    }
#if SDL == 2
    init_sound_driver(1); // 1st call must tell it's changed or we don't get any initialization !
#endif

  menu = new TSoundDlg("", sound_menu);
  menu->execute();
  delete menu;
  menu = NULL;
  int rate = audio_sample_rate;
  choose_card(1);
  audio_sample_rate = rate; // otherwise it's overwritten by choose_card and then detect_soundcard
  for (int i=2; i<sound_menu[1].values_list_size; i++) {
      free(sound_menu[1].values_list_label[i]);
      sound_menu[1].values_list_label[i] = NULL;
  }
  if (recording == 2)
    monitoring = 1;
  else
    monitoring = 0;
  if (old && !recording)
      end_recording();

  return 0;
}
