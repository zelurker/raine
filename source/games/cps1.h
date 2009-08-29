
#ifdef __cplusplus
extern "C" {
#endif
#include "mame/eeprom.h"

struct SOUND_INFO qsound_sound[2];
struct VIDEO_INFO cps1_video;
struct VIDEO_INFO cps1_video_270;

struct EEPROM_interface pang3_eeprom_interface;

void load_cps1(void);
void clear_cps1(void);
void execute_cps1_frame(void);
void execute_sf2_frame(void);
void draw_cps1(void);
void load_qsound();
void load_cps2();
void execute_qsound_frame();
void execute_cps2_frame();


#ifdef __cplusplus
}
#endif
