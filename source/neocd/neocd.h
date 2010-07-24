#ifdef __cplusplus
extern "C" {
#endif

#include "sdl/dialogs/messagebox.h"

// A ReadLong for StarScream (a mix of intel and motorola ways...)
#define ReadLongSc(a) ((ReadWord(a)<<16) | ReadWord(a+2))
#define WriteLongSc(a,b) WriteWord(a,(b)>>16); WriteWord(a+2,((b)&0xffff))

#define IRQ1CTRL_AUTOANIM_STOP		0x08
#define IRQ1CTRL_ENABLE				0x10
#define IRQ1CTRL_LOAD_RELATIVE		0x20
#define IRQ1CTRL_AUTOLOAD_VBLANK	0x40
#define IRQ1CTRL_AUTOLOAD_REPEAT	0x80

extern int neocd_id,allowed_speed_hacks;

extern UINT8 *neogeo_fix_memory,*video_fix_usage,*video_spr_usage;
extern char neocd_path[1024],neocd_dir[1024];
extern char neocd_bios_file[1024];
extern int sfx_volume, music_volume;
extern int capture_new_pictures;

extern UINT8 *neocd_bios;
void setup_neocd_bios();
void set_neocd_exit_to(int code);
void execute_neocd();
void loading_progress_function();
void neocd_function(int vector);
void save_fix(int vidram);
void restore_fix(int vidram);
void postprocess_ipl();
void spr_disable(UINT32 offset, UINT8 data);
void fix_disable(UINT32 offset, UINT8 data);
void video_enable(UINT32 offset, UINT8 data);
void update_game_vectors();
void myStop68000(UINT32 adr, UINT8 data);
void draw_neocd_paused();
void neogeo_read_gamename(void);
void restore_neocd_config();
void save_neocd_config();

extern struct GAME_MAIN game_neocd;

#ifdef __cplusplus
}
#endif
