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

#if SDL == 2
static int choose_driver(int sel) {
    char buf[512];
    const char *current = SDL_GetCurrentAudioDriver();
    buf[0] = 0;
    for (int i = 0; i < SDL_GetNumAudioDrivers(); ++i) {
        snprintf(&buf[strlen(buf)],512-strlen(buf),"%s|", SDL_GetAudioDriver(i));
    }
    buf[strlen(buf)-1] = 0; // remove last |
    int ret = MessageBox("Sound driver",_("Choose one of the available drivers:"),buf);
    if (ret > 0 && strcmp(SDL_GetAudioDriver(ret-1),current)) {
	const char *driver = SDL_GetAudioDriver(ret-1);
	snprintf(buf,512,"SDL_AUDIODRIVER=%s",driver);
	putenv(buf);
	saDestroySound(1);
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	ret = SDL_InitSubSystem(SDL_INIT_AUDIO);
	if (ret < 0) {
	    MessageBox("Error",_("Could not even initialize the audio subsystem with this driver\nForget it"),"Ok");
	    snprintf(buf,512,"SDL_AUDIODRIVER=%s",current);
	    putenv(buf);
	    ret = SDL_InitSubSystem(SDL_INIT_AUDIO);
	    if (ret < 0) {
		fatal_error("We lost the audio subsystem, bye!");
	    }
	}
    }
    init_sound_driver(1);
    return 0;
}
#endif

static int driver_id;

menu_item_t sound_menu[] =
{
#if SDL == 2
    { _("Sound driver"), &choose_driver,&driver_id  },
  { _("Sound device"), NULL, &RaineSoundCard, 1, { 0 }, { _("No") } },
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
  { _("Default volumes"), &set_default_volumes },
#endif
  { _("Record to raine_sound.wav"), NULL, &recording, 3, { 0, 1, 2 }, { _("No"), _("Without monitoring"), _("With monitoring") } },
#if HAS_NEO
{ _("Sound commands..."), &do_sound_cmd },
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
	    SDL_AudioSpec spec;
	    spec.freq = 0;
	    SDL_GetAudioDeviceSpec(i,0,&spec);
	    if (spec.freq)
		audio_sample_rate = spec.freq;
	}
    }
    // Must also init the names of the devices, they depend on the driver... !
    sound_menu[1].values_list_size = SDL_GetNumAudioDevices(0)+1;
    sound_menu[1].values_list_label[0] = "None";
    if (menu) {
	menu->update_list_label(1,0,_("None"));
	menu->update_list_size(1,sound_menu[1].values_list_size);
    }
    for (int i = 0; i < SDL_GetNumAudioDevices(0); i++) {
	if (sound_menu[1].values_list_label[i+1])
	    free(sound_menu[1].values_list_label[i+1]);
	sound_menu[1].values_list_label[i+1] = strdup(SDL_GetAudioDeviceName(i, 0));
	sound_menu[1].values_list[i+1] = i+1;
	if (menu) menu->update_list_label(1,i+1,sound_menu[1].values_list_label[i+1]);
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
    init_sound_driver(0);
#endif

  menu = new TSoundDlg("", sound_menu);
  menu->execute();
  delete menu;
  menu = NULL;
  for (int i=1; i<sound_menu[1].values_list_size; i++) {
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
