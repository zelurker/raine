#ifdef __cplusplus
extern "C" {
#endif

#include "deftypes.h"

#define MAX_ASSOC 1024

extern int disable_assoc,last_song,use_music,qsound_last_song;
extern int mute_sfa3_speaker;

void init_assoc(int kind);
void clear_assoc();
int get_asso_loop(int cmd);
void set_asso_loop(int cmd, int lp);
int get_assoc_adr();
char* get_assoc(int cmd);
void del_assoc(int cmd);
void assoc(int cmd, char *t);
void save_assoc(char *section);
void load_assoc(char *section);
int handle_sound_cmd(int cmd);
int handle_cps2_cmd(UINT8 *shared, int offset, int cmd);
int get_assoc_type();
int is_qsound_song(int cmd);

#ifdef __cplusplus
}
#endif
