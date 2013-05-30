#include "gameinc.h"
#include "arpro.h"
#include "hiscore.h"
#include "profile.h"
#include "ay8910.h"
#include "timer.h"
#include "sasound.h"
#include "savegame.h"
#include "priorities.h"
#include "emumain.h"
#include "blit.h"
#include "config.h"
#if HAS_CONSOLE
#include "sdl/console/console.h"
#endif
#include "cache.h"

// I just need these for the general warning messages
int load_error;
char *load_debug;
UINT8 *GFX,*Z80ROM,*ROM,*PCMROM,*neogeo_pcm_memory;

void LoadDefault(void)
{
#ifdef RDTSC_PROFILE
  cycles_per_frame = 0; // recalibrate for non 60fps games
#endif
  fps = 59.185606; // As reported in the forum, see
  default_fps = 0.0;
  // http://rainemu.swishparty.co.uk/msgboard/yabbse/index.php?topic=1299.msg5496#msg5496
  ay8910_amplify = 1;

  ExitOnEI = 0;
  MouseB = &mouse_b;
  setup_z80_frame(CPU_Z80_0,CPU_FRAME_MHz(4,60));
  z80_offdata = 0; // code location = data location
  reset_timers();
  reset_z80_banks();

  ResetMemoryPool();				// Init game memory pool

   reset_arpro();				// Init action replay
   Clear68000List();				// Clear M68000 memory lists
   StarScreamEngine=0;				// No M68000 by default

   ClearZ80List();				// Clear Z80 memory lists
   MZ80Engine=0;				// No Z80 by default

#ifdef HAVE_6502
   ClearM6502List();				// Clear M6502 memory lists
   M6502Engine=0;				// No M6502 by default
#endif

   GameSound=0; 				// No Sound emulation by default
   GameMouse=0; 				// No Mouse driver by default

   raine_cfg.req_pause_game = 0;
   raine_cfg.req_save_screen = 0;

   dir_cfg.last_screenshot_num = 0;

   SaveSlot=0;					// Reset save slot to 0
   SaveDataCount=0;				// No save data
   SaveCallbackCount=0; 			// No save callbacks
   savecbptr_count = 0;
   reset_dyn_callbacks();

   RAMSize=0x80000;				// Size of RAM memory allocated (should be 0 here really)

   clear_eeprom_list(); 			// No EEPROMs mapped

   if(raine_cfg.show_fps_mode==2) raine_cfg.show_fps_mode=0;			// Don't like it running from time=0

   PCMROM=NULL; 				// Might as well clear these too
   Z80ROM=NULL;
   ROM=RAM=GFX=NULL;

   tile_list_count = 0;
   max_tile_sprites = 0;
   set_pulse_time(15);

   tile_start = 0; // tile queue reset

   set_white_pen(255);

   current_cmap_func = NULL;
   current_colour_mapper = NULL;

   reset_layer_switches();
   set_reset_function(NULL);
   load_game_config();
}

void ClearDefault(void)
{
  use_scale2x = 0;
  save_game_config();
  hs_close();
  if (current_game->clear)
    current_game->clear();

  reset_savegames();
   saDestroySound(1);				// Free all sound resources
   reset_arpro();				// Free action replay

   destroy_palette_map();			// Free colour mapping resources

   if(raine_cfg.show_fps_mode==2) raine_cfg.show_fps_mode=0;			// Don't like it running from time=0

   FreeMemoryPool();				// Free anything left in the memory pool

   if (pbitmap) {
     destroy_bitmap(pbitmap);
     pbitmap = NULL;
   }

   current_game->main_name = "neocd";
   current_game = NULL;
   DestroyScreenBitmap();
   clear_file_cache();
#if HAS_CONSOLE
   done_console();
#endif
}

