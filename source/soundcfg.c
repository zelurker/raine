#include "raine.h"
#include "sasound.h"
#include "es5506.h"
#include "games/darius.h"

#ifdef ALLEGRO_SOUND
int max_mixer_volume;
#else
int smallest_sound_buffer;
#endif

void sound_load_cfg() {
   int id;
#ifdef ALLEGRO_SOUND
   _DRIVER_INFO *digi;
   int i;
#endif

   RaineSoundCard = 0;
#ifndef ALLEGRO_SOUND
   // sdl, default to sound enabled (1)
   // id = raine_get_config_id(    "Sound",        "sound_card",  1);
   id = 1; // force auto-detect ALWAYS
   // there is no reason not to autodetect the sound
#else
#ifdef RAINE_WIN32
   id = raine_get_config_id(    "Sound",        "sound_card",  DIGI_DIRECTAMX(0));
#else
   id = raine_get_config_id(    "Sound",        "sound_card",           DIGI_NONE);
#endif
   if (id > 1<<8) { // the id is really a soundcard id, not an index
     install_sound(id, MIDI_NONE, NULL);
     if (system_driver->digi_drivers)
       digi = system_driver->digi_drivers();
     else
       digi = _digi_driver_list;

     for (i=0; digi[i].driver; i++){
       if (id==((DIGI_DRIVER *)digi[i].driver)->id)
	 RaineSoundCard = i+1;
     }
     remove_sound(); // the sound is not installed here.
   } else
#endif
     RaineSoundCard = id;

#ifdef NEO
  audio_sample_rate = 44100; // with cd tracks, it would be crazy to have
  // something else here !
#else
#if defined(RAINE_WIN32) && !defined(ALLEGRO_SOUND)
   /* It's better to default to 44Khz in sdl in win32 because of the buggy sound drivers
      in this os (they oblige to have quite a big sound buffer, which produces a
      noticeable sound delay at low sampling rates */
   audio_sample_rate= raine_get_config_int( "Sound",        "sample_rate",          44100 );
#else
   audio_sample_rate= raine_get_config_int( "Sound",        "sample_rate",          22050 );
#endif
#endif
#if HAS_ES5505
   es5506_voice_filters = raine_get_config_int( "Sound",        "es5506_voice_filters",1 );
#endif
   // enh_stereo = raine_get_config_int( "Sound",        "enh_stereo",0 );
#ifdef ALLEGRO_SOUND
   max_mixer_volume = raine_get_config_int( "Sound",        "max_mixer_volume",0 );
#else
#ifdef RAINE_WIN32
   smallest_sound_buffer = raine_get_config_int( "Sound",        "smallest_sound_buffer",0 );
#endif
#endif
#ifndef NEO
   DariusStereo 	= raine_get_config_int( "Sound",        "DariusStereo",         1); /* 0:monaural 1=stereo */
#endif
}

void sound_save_cfg() {
   // SOUND
   raine_set_config_id( 	"Sound",        "sound_card",           sound_card_id(RaineSoundCard));
   raine_set_config_int(	"Sound",        "sample_rate",          audio_sample_rate);

#ifndef NEO
   raine_set_config_int(	"Sound",        "DariusStereo",         DariusStereo);
#endif
#if HAS_ES5505
   raine_set_config_int(	"Sound",        "es5506_voice_filters", es5506_voice_filters);
#endif
   // raine_set_config_int(	"Sound",        "enh_stereo",         enh_stereo);
#ifdef ALLEGRO_SOUND
   raine_set_config_int(	"Sound",        "max_mixer_volume",         max_mixer_volume);
#else
#ifdef RAINE_WIN32
   raine_set_config_int(	"Sound",        "smallest_sound_buffer",         smallest_sound_buffer);
#endif
#endif
}
