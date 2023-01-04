/* This is mostly inspired from my work on neocdpsp, and from the source
 * of the neogeo driver in mame (to help to clarify things at some places!)
 * and got some info from an old version of the ncdz emu (mainly the array
 * to get the short names of the games + the info about loading animations)
 *
 * Notice that the cd audio emulation is based on values in RAM !!!
 * The upload area which was a big mystery in the psp version is now correct
 * at least for the writes,
 *
 */

#include "gameinc.h"
#include "dialogs/messagebox.h"
#include "pd4990a.h"
#include "files.h"
#include "2610intf.h"
#include "neocd.h"
#include "emumain.h" // reset_game_hardware
#include "cdrom.h"
#include "config.h"
#include "savegame.h"
#include "blit.h"
#include "zoom/16x16.h"
#include "cdda.h"
#include "dsw.h"
#include "profile.h"
#include "cache.h"
#include "default.h"
#include "hiscore.h"
#include "history.h"
#include "display.h"
#ifdef RAINE_DEBUG
#include "gui.h"
#endif
#ifndef RAINE_DOS
#include "control_internal.h"
#include "dialogs/fsel.h"
#include "gui.h"
#include "dialogs/neo_debug_dips.h"
#else
#include "alleg/gui/rgui.h"
#endif
#include "loadpng.h"
#include "games/gun.h"
#include "taitosnd.h"
#include "games/neogeo.h"
#include "decode.h"
#include "sound/assoc.h"
#include "bld.h"
#include "alpha.h"
#include "version.h"
#include "compat_sdl.h"
#include "iso.h"

// #define NEOGEO_MCARD_16BITS 1

#ifdef RAINE_DOS
#ifdef ALLEGRO_LITTLE_ENDIAN
#define NATIVE_ENDIAN_VALUE_LE_BE(leval,beval)  (leval)
#else
#define NATIVE_ENDIAN_VALUE_LE_BE(leval,beval)  (beval)
#endif
#else
#define NATIVE_ENDIAN_VALUE_LE_BE(leval,beval)  \
    (SDL_BYTEORDER == SDL_LIL_ENDIAN ?  leval : beval)
#endif

#define BYTE_XOR_BE(a)                  ((a) ^ NATIVE_ENDIAN_VALUE_LE_BE(1,0))
#define BYTE_XOR_LE(a)                  ((a) ^ NATIVE_ENDIAN_VALUE_LE_BE(0,1))

#define DBG_RASTER 1
#define DBG_IRQ    2
#define DBG_SPRITES 4
#define DBG_LEVEL 0

// BOOT_BIOS : boots the bios, but stays in the bios if doing that, you never reach the game
// very likely because of the lack of emulation for the cdrom irq... !
// #define BOOT_BIOS 1

#ifndef RAINE_DEBUG
#define debug
#else

#define MCARD_SIZE_NEOCD 0x2000
#define MCARD_SIZE_NEOGEO 0x800
#define MCARD_SIZE_16BIT  0x20000

void debug(int level, const char *format, ...)
{
    if (level & DBG_LEVEL) {
	va_list ap;
	va_start(ap,format);
	vprintf(format,ap);
	va_end(ap);
    }
}
#endif

#define LongSc(a) (is_neocd() ? ReadLongSc(&RAM[a]) : ReadLongSc(&RAM[((a) & 0xffffff)-0x100000]))

#define NB_LINES 264 // lines on the screen (including borders)
#define START_SCREEN 0x10
/* This is quite surprising, mame team has made some measurements and it appears that the dev documentation
 * is at least incomplete, or wrong. From the web, it appears there should be :
 From http://wiki.neogeodev.org/index.php?title=Display_timing
 8 scanlines vertical sync pulse
 16 scanlines top border
 224 scanlines active display
 16 scanlines bottom border

 I knew there was something wrong because I had found already that neo turf masters needed at least
 START_SCREEN = 36 instead of 24 so that the animation of the clouds on top of the mountains be
 correct, but these numbers produced problmes elsewhere, and I started to wonder if the vbl should not
 start before the end of the screen... I was on the right track, finally there is no real border at all,
 the screen starts 16 lines after the top, but it's not a border, it's inside the vbl, and the vbl starts
 right after the viewable display, at line 16+224 (f0). That makes quite a considerable difference with the
 docs !!!
 And you can't really call the area after line $f0 a border since it's the line where the hbl counter is
 reloaded on vblank !
 */

static struct {
    UINT8 *ram;
    UINT16 unlock;
} saveram;

static int capture_mode = 0,start_line,screen_cleared,load_sdips,fix_mask,
	   fix_banked,aes;
static int capture_block; // block to be shown...
int allowed_speed_hacks = 1,disable_irq1 = 0;
static int one_palette,sprites_mask,nb_sprites;
static int assigned_banks, current_bank;
static UINT32 zbank[4];
static UINT8 bank_68k,fixed_layer_bank_type,*pvc_cart;
int capture_new_pictures;
static al_bitmap *raster_bitmap;
static void draw_neocd();
static void draw_sprites(int start, int end, int start_line, int end_line);
static UINT8 dark_screen; // only neogeo ?
static int fc;
static int size_mcard;
static int neogeo_frame_counter_speed,raster_frame,neogeo_frame_counter,
	   scanline,watchdog_counter,
	   irq3_pending,
	   display_position_interrupt_pending, vblank_interrupt_pending,
	   vbl,hbl;

struct VIDEO_INFO neocd_video =
{
  draw_neocd,
  304,
  224,
  16,
  VIDEO_ROTATE_NORMAL |
    VIDEO_ROTATABLE,
  NULL,
  59.185606 // As reported in the forum, see http://rainemu.swishparty.co.uk/msgboard/yabbse/index.php?topic=1299.msg5496#msg5496
};

int neogeo_bios;

UINT8* get_neogeo_saveram() {
    return saveram.ram;
}

void restore_neocd_config() {
  allowed_speed_hacks = raine_get_config_int("neocd","allowed_speed_hacks",1);
  disable_irq1 = raine_get_config_int("neocd","disable_irq1",0);
  capture_new_pictures = raine_get_config_int("neocd","capture_new_pictures",0);
  neogeo_bios = raine_get_config_int("neogeo","bios",0);
  set_neogeo_bios(neogeo_bios);
}

void save_neocd_config() {
  raine_set_config_int("neocd","allowed_speed_hacks",allowed_speed_hacks);
  raine_set_config_int("neocd","disable_irq1",disable_irq1);
  raine_set_config_int("neocd","capture_new_pictures",capture_new_pictures);
  raine_set_config_int("neogeo","bios",neogeo_bios);
}

#ifndef RAINE_DOS
static void toggle_capture_mode() {
  capture_mode++;
  if (capture_mode == 2)
      one_palette = 1;
  else one_palette = 0;
  if (capture_mode > 2)
      capture_mode = 0;
}

static void prev_sprite_block() {
  switch (capture_mode) {
  case 1: if (capture_block > 0) capture_block--; break;
  case 2: if (current_bank > 0) current_bank--; break;
  }
}

static void next_sprite_block() {
  switch(capture_mode) {
  case 1: capture_block++; break;
  case 2: if (current_bank < assigned_banks) current_bank++; break;
  }
}
#endif // RAINE_DOS

static void draw_sprites_capture(int start, int end, int start_line,
	int end_line);

static FILE *fdata;
#define MAX_BANKS 256
static UINT8 banks[MAX_BANKS];

#ifndef RAINE_DOS
static void do_capture() {
    if (!capture_mode) return;
    int bpp = display_cfg.bpp;
    display_cfg.bpp = 8;
    current_game->video->flags |= VIDEO_NEEDS_8BPP;
    SetupScreenBitmap();
    set_colour_mapper(&col_Map_15bit_xRGBRRRRGGGGBBBB);
    one_palette = 1;
    if (capture_mode < 2)
	assigned_banks = current_bank = -1;
    do {
	ClearPaletteMap();
	ClearPaletteMap(); // 2 ClearPaletteMap, it's not an error
	// it's to clear both palettes in double buffer mode
	char filename[256];
	if (current_bank > -1)
	    snprintf(filename,256,"%ssavedata" SLASH "block%d-%d.map", dir_cfg.exe_path, capture_block,(capture_mode == 2 ? current_bank : current_bank+1));
	else
	    snprintf(filename,256,"%ssavedata" SLASH "block%d.map", dir_cfg.exe_path, capture_block);
	fdata = fopen(filename,"w");
	draw_sprites_capture(0,384,0,224);
	memset(&pal[16],1,sizeof(pal)-16*sizeof(SDL_Color));
	fclose(fdata);
	fdata = NULL;
	// memcpy(&pal[0],&pal[1],16*sizeof(SDL_Color)); // fix the palette for the
	pal[15].r = pal[15].g = pal[15].b = 1;
	/* Color 15 is background, always transparent for sprites.
	 * I set it to almost black (1,1,1), this color is impossible to render
	 * with the native neocd colors so there should be no color collision with
	 * it */
	if (current_bank > 0)
	    snprintf(filename,256,"%sblock%d-%d.png",dir_cfg.screen_dir,capture_block,current_bank);
	else
	    snprintf(filename,256,"%sblock%d.png",dir_cfg.screen_dir,capture_block);
	fdata = fopen(filename,"rb");
	if (fdata && capture_new_pictures) {
	    fclose(fdata);
	    char *s = strstr(filename,".png");
	    int nb = 0;
	    do {
		sprintf(s,"-%d.png",nb);
		fdata = fopen(filename,"rb");
		if (fdata) {
		    fclose(fdata);
		    nb++;
		}
	    } while (fdata); // find an unused name
	}
	save_png(filename,GameViewBitmap,pal);
	if (capture_mode == 2) break;
    } while (current_bank < assigned_banks);
    if (capture_mode < 2) one_palette = 0;
    display_cfg.bpp = bpp;
    current_game->video->flags &= ~VIDEO_NEEDS_8BPP;
    SetupScreenBitmap();
    set_colour_mapper(&col_Map_15bit_xRGBRRRRGGGGBBBB);
}

static struct DEF_INPUT_EMU list_emu[] =
{
 { SDLK_BACKSPACE,  0x00, _("Toggle capture mode"), 0, toggle_capture_mode },
 { SDLK_RIGHT,      0x00, _("Next sprite block"), 0, next_sprite_block     },
 { SDLK_LEFT,       0x00, _("Previous sprite block"), 0, prev_sprite_block     },
 { SDLK_c,          0x00, _("Capture block"), 0, do_capture },
};
#endif
static int frame_neo;

// neocd_path points to a neocd image, neocd_dir is the last path used for
// neocd files (which is not always an image path).
char neocd_path[FILENAME_MAX],neocd_dir[FILENAME_MAX];
char neocd_bios_file[FILENAME_MAX];
static int loading_animation,loading_animation_progress,loading_animation_init,
	   default_anim;
int loading_animation_fix,loading_animation_pal; // used in cdrom.c
static int exit_to_adr;

static struct INPUT_INFO neocd_inputs[] = // 2 players, 4 buttons
{
  { KB_DEF_P1_UP, MSG_P1_UP, 0x01, 0x01, BIT_ACTIVE_0 },
  { KB_DEF_P1_DOWN, MSG_P1_DOWN, 0x01, 0x02, BIT_ACTIVE_0 },
  { KB_DEF_P1_LEFT, MSG_P1_LEFT, 0x01, 0x04, BIT_ACTIVE_0 },
  { KB_DEF_P1_RIGHT, MSG_P1_RIGHT, 0x01, 0x08, BIT_ACTIVE_0 },
  { KB_DEF_P1_B1, "Player1 A", 0x01, 0x10, BIT_ACTIVE_0 },
  { KB_DEF_P1_B2, "Player1 B", 0x01, 0x20, BIT_ACTIVE_0 },
  { KB_DEF_P1_B3, "Player1 C", 0x01, 0x40, BIT_ACTIVE_0 },
  { KB_DEF_P1_B4, "Player1 D", 0x01, 0x80, BIT_ACTIVE_0 },
#ifndef RAINE_DOS
  { KB_DEF_P1_B1B2,MSG_P1_AB, 1, 0x30, BIT_ACTIVE_0 },
  { KB_DEF_P1_B3B4,"Player1 C+D", 1, 0xc0, BIT_ACTIVE_0 },
  { KB_DEF_P1_B2B3,"Player1 B+C", 1, 0x60, BIT_ACTIVE_0 },
  { KB_DEF_P1_B1B2B3,"Player1 A+B+C", 1, 0x70, BIT_ACTIVE_0 },
  { KB_DEF_P1_B2B3B4,"Player1 B+C+D", 1, 0xe0, BIT_ACTIVE_0 },
#endif

  { KB_DEF_P2_UP, MSG_P2_UP, 0x03, 0x01, BIT_ACTIVE_0 },
  { KB_DEF_P2_DOWN, MSG_P2_DOWN, 0x03, 0x02, BIT_ACTIVE_0 },
  { KB_DEF_P2_LEFT, MSG_P2_LEFT, 0x03, 0x04, BIT_ACTIVE_0 },
  { KB_DEF_P2_RIGHT, MSG_P2_RIGHT, 0x03, 0x08, BIT_ACTIVE_0 },
  { KB_DEF_P2_B1, "Player2 A", 0x03, 0x10, BIT_ACTIVE_0 },
  { KB_DEF_P2_B2, "Player2 B", 0x03, 0x20, BIT_ACTIVE_0 },
  { KB_DEF_P2_B3, "Player2 C", 0x03, 0x40, BIT_ACTIVE_0 },
  { KB_DEF_P2_B4, "Player2 D", 0x03, 0x80, BIT_ACTIVE_0 },
#ifndef RAINE_DOS
  { KB_DEF_P2_B1B2,"Player2 A+B", 3, 0x30, BIT_ACTIVE_0 },
  { KB_DEF_P2_B3B4,"Player2 C+D", 3, 0xc0, BIT_ACTIVE_0 },
  { KB_DEF_P2_B2B3,"Player2 B+C", 3, 0x60, BIT_ACTIVE_0 },
  { KB_DEF_P2_B1B2B3,"Player2 A+B+C", 3, 0x70, BIT_ACTIVE_0 },
  { KB_DEF_P2_B2B3B4,"Player2 B+C+D", 3, 0xe0, BIT_ACTIVE_0 },
#endif

  { KB_DEF_P1_START, MSG_P1_START, 0x05, 0x01, BIT_ACTIVE_0 },
  { KB_DEF_COIN1, "Player1 Select", 0x05, 0x02, BIT_ACTIVE_0 },
  { KB_DEF_P2_START, MSG_P2_START, 0x05, 0x04, BIT_ACTIVE_0 },
  { KB_DEF_COIN2, "Player2 Select", 0x05, 0x08, BIT_ACTIVE_0 },
  INP1( UNKNOWN, 5, 0x70), // memcard status
  INP1( UNKNOWN, 5, 0x80), // mvs/aes ?
  // Bit 4 (0x10) is 0 if the memory card is present !!!
  // neogeo doc :
  // bit 5 = mc 2 insertion status (0 = inserted)
  // bit 6 write protect 0 = write enable
  // bit 7 = neogeo mode : 0 = neogeo / 1 = mvs !!!

  { 0, NULL,        0,        0,    0            },
};

static int layer_id_data[2];

UINT8 *neocd_bios;

void setup_neocd_bios() {
  if (!is_neocd()) {
      neocd_bios = load_region[REGION_MAINBIOS];
      return;
  }
  if (neocd_bios)
    return;
  // produces a memwatch warning, but since neocd bios can actually be a load_region for the neogeo bios or this, it can't be freed in clear_neocd, it's freed at the beg of this function
  // so the warning will stay !
  neocd_bios = calloc(0x80000,1); // Be sure to have at least 512k
  // unsigned char rom_fix_usage[4096];
  int ret = 0;
  if (!*neocd_bios_file) {
      if (exists(get_shared("neocd.bin")))
	  strcpy(neocd_bios_file,get_shared("neocd.bin"));
      else if (exists(get_shared("neocd.zip")))
	  strcpy(neocd_bios_file,get_shared("neocd.zip"));
  }
  int tries = 0;
  do {
      if (*neocd_bios_file) {
	  if (!stricmp(&neocd_bios_file[strlen(neocd_bios_file)-3],"zip")) {
	      ret = load_zipped(neocd_bios_file, "neocd.bin", 0x80000, 0, neocd_bios, 1);
	  } else {
	      ret = load_file(neocd_bios_file,neocd_bios,0x80000);
	  }
      }
      if (!ret && !tries) {
	  char *exts[] = { "bin", "zip", NULL };
	  *neocd_bios_file = 0;
	  fsel(dir_cfg.share_path,exts,neocd_bios_file,"Find NeoCD bios");
	  if (!*neocd_bios_file) break;
	  tries++;
      } else
	  break;
  } while (1);

  if (!ret) {
      MessageBox(gettext("Fatal error"), gettext("Find the Neo-Geo CD BIOS (neocd.bin).\nAsk Google if you can't find it!"),gettext("OK"));
      exit(1);
  }

  // Check BIOS validity
  if (ReadWord(&neocd_bios[0x582]) == 0xf94e) {
      // Needs a byteswap !
      ByteSwap(neocd_bios,0x80000);
  }
  if (ReadWord(&neocd_bios[0x0582]) != 0x4eF9)
  {
    ErrorMsg("Fatal Error: Invalid BIOS file.");
    exit(1);;
  }

  /* Search the address of loading animations, for this search for 11c817 which
   * is the address of the value tested in ram. Normally there are 3 matches
   * for the front and neocdz one. The 3rd one is the one we want... */
  int nb = 0;
  int n;
  for (n=0; n<0x10000; n+=2) {
      if (ReadLongSc(&neocd_bios[n]) == 0x11c817) {
	  loading_animation = n;
	  nb++;
	  if (nb==3) break;
      }
  }
  if (nb < 3) {
      ErrorMsg("Couldn't find the loading animations in the bios");
      exit(1);
  }
  loading_animation -= 0xc;
  loading_animation_progress = loading_animation - 4;
  loading_animation += 0xc00000;

  // Ok, now we get the adresses of the previous functions, progress being
  // the 1st, then an unknown one, and then init

  while (ReadWord(&neocd_bios[loading_animation_progress]) != 0x4e75)
      loading_animation_progress -= 2;
  loading_animation_init = loading_animation_progress - 2;
  loading_animation_progress += 2;
  // skip the next one...
  while (ReadWord(&neocd_bios[loading_animation_init]) != 0x4e75)
      loading_animation_init -= 2;
  loading_animation_init -= 2;
  while (ReadWord(&neocd_bios[loading_animation_init]) != 0x4e75)
      loading_animation_init -= 2;
  loading_animation_init += 2;
  loading_animation_init += 0xc00000;
  loading_animation_progress += 0xc00000;
  print_debug("Neocd bios :\nloading_animation : %x\nloading_animation_init : %x\nloading_animation_progress : %x\n",loading_animation,loading_animation_init,loading_animation_progress);

  // Harder : fix and pal area
  /* I am not proud of this, but the easiest way to find the palette
   * is to search for movq #127,d7 / move.l (a0)+,(a1)+ which is what the
   * bios does to initialize the palette, there are 3 matches, the one we
   * want is the 2nd one ! */
  nb = 0;
  for (n=0; n<0x70000; n+=2) {
      if (ReadLongSc(&neocd_bios[n]) == 0x7e7f22d8) {
	  nb++;
	  if (nb == 2) break;
      }
  }
  loading_animation_pal = ReadLongSc(&neocd_bios[n-0xa])-0xc00000;

  // Now search 2b7c 4000 from here, which will be the fix transfer...

  while (ReadWord(&neocd_bios[n]) != 0x2b7c ||
	  ReadLongSc(&neocd_bios[n+2]) != 0x4000)
      n -= 2;
  loading_animation_fix = ReadLongSc(&neocd_bios[n+0xa])-0xc00000;
  print_debug("neocd pal %x fix %x\n",loading_animation_pal,loading_animation_fix);
  // exit_to code
  exit_to_adr = ReadLongSc(&neocd_bios[0x560]);
  exit_to_adr += 0x10;
  exit_to_adr -= 0xc00000;
  for (n=0; n<0x20; n+=2)
      if (ReadWord(&neocd_bios[n+exit_to_adr]) == 0x1b7c &&
	      ReadWord(&neocd_bios[n+exit_to_adr+2]) == 2) {
	  exit_to_adr += n+2;
	  break;
      }
  // Default animation data, it goes to 11c810 so search this...
  for (n=0x5000; n<0x10000; n+=2) {
      if (ReadLongSc(&neocd_bios[n]) == 0x11c810 &&
		  ReadWord(&neocd_bios[n-6]) == 0x23fc)  {
	  default_anim = ReadLongSc(&neocd_bios[n-4]);
	  printf("default anim : %x\n",default_anim);
	  break;
      }
  }

  if (ReadWord(&neocd_bios[exit_to_adr]) != 2) {
      ErrorMsg("Couldn't find exit code in this bios !");
      exit_to_adr = 0;
  }

#if 1
  /*** Patch BIOS CDneocd_bios Check ***/
  // WriteWord(&neocd_bios[0xB040], 0x4E71);
  // WriteWord(&neocd_bios[0xB042], 0x4E71);
  /*** Patch BIOS upload command ***/
  // WriteWord(&neocd_bios[0x546], 0x60fe); // 0xFAC1);
  // WriteWord(&neocd_bios[0x548], 0x4E75);

  /*** Patch BIOS CDDA check ***/
  /* 	*((short*)(neogeo_rom_memory+0x56A)) = 0xFAC3; */
  /* 	*((short*)(neogeo_rom_memory+0x56C)) = 0x4E75; */

  // WriteWord(&neocd_bios[0x56a],0x60fe);
  /*** Full reset, please ***/
#if 0
  // Seems to be called at the beginning of sonicwi2, just after the initial
  // loading animation, but seems totally useless now.
  // Disabled for compatibility with other bioses...
  WriteWord(&neocd_bios[0xA87A], 0x60fe); // 0x4239);
  WriteWord(&neocd_bios[0xA87C], 0x0010);
  WriteWord(&neocd_bios[0xA87E], 0xFDAE);
#endif

  // WriteWord(&neocd_bios[0xd736],0x4e75);

  /*** Trap exceptions ***/
  // WriteWord(&neocd_bios[0xA5B6], 0x4e70); // reset instruction !

#if 0
  WriteWord(&neocd_bios[0xA5B6], 0x4ef9); // reset instruction !
  WriteWord(&neocd_bios[0xA5B8], 0xc0);
  WriteWord(&neocd_bios[0xA5Ba], 0xa822);
#endif
#endif
  set_neocd_exit_to(exit_to_code);
}

static UINT16 result_code,pending_command,*neogeo_vidram,video_modulo,video_pointer;
static UINT8 *neogeo_memorycard;
UINT8 *neogeo_fix_memory,*native_fix,*native_spr,*video_fix_usage,*video_spr_usage,*bios_fix_usage;

static UINT8 temp_fix_usage[0x300],saved_fix;

void save_fix(int vidram) {
  // used by the cdrom before calling the animation functions...
  if (vidram) {
    memcpy(&RAM[0x110804],&neogeo_vidram[0x7000],0x500*2);
    // memset(&neogeo_vidram[0x7000],0,0x500*2);
  }
  memcpy(&RAM[0x115e06],neogeo_fix_memory,0x6000);
  memcpy(temp_fix_usage,video_fix_usage,0x300);
  saved_fix = 1;
}

void restore_fix(int vidram) {
    if (vidram)
	memcpy(&neogeo_vidram[0x7000],&RAM[0x110804],0x500*2);
    memcpy(neogeo_fix_memory,&RAM[0x115e06],0x6000);
    memcpy(video_fix_usage,temp_fix_usage,0x300);
    saved_fix = 0;
}

// Save ram : the neogeo hardware seems to have a non volatile ram, but it
// was not mapped in neocdpsp, so I don't know if it's used or not...
// To be tested...

static UINT8 read_memorycard(UINT32 offset) {
    if (is_neocd()) {
	if ((offset & 1)) {
	    return neogeo_memorycard[(offset & 0x3fff) >> 1];
	}
	return 0xff;
    }
#ifdef NEOGEO_MCARD_16BITS
    return neogeo_memorycard[(offset & (size_mcard-1)) ^ 1];
#endif
    if (offset & 1)
	return neogeo_memorycard[(offset >> 1) & (size_mcard-1)];
    return 0xff;
}

static UINT16 read_memorycardw(UINT32 offset) {
    if (is_neocd())
	return 0xff00 | neogeo_memorycard[(offset & 0x3fff) >> 1];
#ifdef NEOGEO_MCARD_16BITS
    return ReadWord(&neogeo_memorycard[offset & (size_mcard-1)]);
#endif
    return 0xff00 |neogeo_memorycard[(offset >> 1) & (size_mcard-1)];
}

static int memcard_write;

static void write_memcard(UINT32 offset, UINT32 data) {
    if (is_neocd()) {
	if ((offset & 1)) {
	    memcard_write = 1;
	    neogeo_memorycard[(offset & 0x3fff) >> 1] = data;
	}
	return;
    }
#ifdef NEOGEO_MCARD_16BITS
    memcard_write = 1;
    neogeo_memorycard[(offset & (size_mcard-1)) ^ 1] = data;
#else
    if ((offset & 1)) {
	memcard_write = 1;
	neogeo_memorycard[(offset >> 1) & (size_mcard-1) ] = data;
    }
#endif
}

static void write_memcardw(UINT32 offset, UINT32 data) {
    memcard_write = 1;
    if (is_neocd())
	neogeo_memorycard[(offset & 0x3fff) >> 1] = data;
    else
#ifdef NEOGEO_MCARD_16BITS
	WriteWord(&neogeo_memorycard[offset & (size_mcard-1)], data);
#else
	neogeo_memorycard[(offset >> 1) & (size_mcard-1)] = data;
#endif
}

static void set_res_code(UINT32 offset, UINT16 data) {
  result_code = data;
}

static UINT16 read_sound_cmd(UINT32 offset) {
  pending_command = 0;
  return latch;
}

static int z80_enabled,direct_fix,spr_disabled,fix_disabled,video_enabled,
	   pcm_bank,spr_bank,fix_cur,fix_write,spr_min,spr_max;

static void write_sound_command(UINT32 offset, UINT16 data) {
  if (z80_enabled && RaineSoundCard) {
    if (handle_sound_cmd(data))
	return;
    pending_command = 1;
    latch = data;
    cpu_int_nmi(CPU_Z80_0);

// #if !defined(HAS_CZ80) && !defined(MAME_Z80)
    // Very few games seem to need this, but Ironclad is one of them (you loose
    // the z80 just after choosing "start game" if you don't do this !)
    // Also mslug produces bad cd songs without this !!!
    ExitOnEI = 1;
    int ticks = dwElapsedTicks;
    cpu_execute_cycles(CPU_Z80_0, 60000);
    dwElapsedTicks = ticks; // say this never happened
    ExitOnEI = 0;
// #endif
  }
}

static void write_sound_command_word(UINT32 offset, UINT16 data) {
  write_sound_command(offset,data >> 8);
}

static int cpu_readcoin(int addr)
{
  addr &= 0xFFFF;
  if (addr & 0x1) {
    // get calendar status - 2 bits
    /* Notice : lsb bits are not used in the neocd version, it's IN3 in mame
     * Here are the used bits :
     PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 )
     PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_COIN2 )
     PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_SERVICE1 )
     PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_UNKNOWN )  // having this ACTIVE_HIGH causes you to start with 2 credits using USA bios roms
     PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_UNKNOWN ) // having this ACTIVE_HIGH causes you to start with 2 credits using USA bios roms
     PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_SPECIAL )
     */
    int coinflip = pd4990a_testbit_r(0);
    int databit = pd4990a_databit_r(0);
    return input_buffer[6] | (coinflip << 6) | (databit << 7);
  }
  {
    int res = result_code;

    if (RaineSoundCard)
    {
      if (pending_command)
	res &= 0x7f;
    }

    return res;
  }
}

static struct {
  int start, control, pos; // flags for irq1
  int disable,wait_for_vbl_ack; // global / irq2
} irq;

typedef struct {
  char *name;
  int id,width;
} NEOCD_GAME;

static const NEOCD_GAME *game;
static int current_neo_frame, desired_68k_speed, stopped_68k,rolled;
static int do_not_stop,end_screen,start_frame;

static void get_scanline() {
  if (!raster_frame) {
    // scanline isn't available, find it
    // Use start_frame here because current_eno_frame
    // can vary when finding a speed hack
    int cycles = s68000readOdometer() - start_frame;
    // The +0xf0 is to try to make garou happy, it freezes on a certain stage 3 otherwise
    // sadly I can't check it works, lost the savegame and the details of the problem !
    // See the call from read_videoreg, the value of the scanline is expected to go from f0 to 1ff, see details there
    scanline = cycles * NB_LINES / current_neo_frame + 0xf0;
  }
}

static void update_interrupts(void)
{
  int level = 0;

  if (vblank_interrupt_pending) level = vbl;
  if (display_position_interrupt_pending) level = hbl;
  if (irq3_pending) level = 3;

  /* either set or clear the appropriate lines */
  if (level) {
    if (irq.disable) {
	debug(DBG_IRQ,"update_interrupts: irqs disabled\n");
	return;
    }
    if (level == vbl) {

       if (irq.wait_for_vbl_ack) {
	   // For some unknown reason this seems to be
	   // necessary for last blade 2
	   // Without this, the stack is filled !!!
	   debug(DBG_IRQ,"received vbl, still waiting for ack sr=%x\n",s68000_sr);
	   return;
       }
    }

    if (s68000_interrupts & (1<<level)) {
      debug(DBG_IRQ,"irq already pending, ignoring...\n");
    } else {
	debug(DBG_IRQ,"irq %d on line %d sr %x\n",level,scanline,s68000_sr);
	cpu_interrupt(CPU_68K_0,level);
	if (level == vbl)
	    irq.wait_for_vbl_ack = 1;
    }
#if 0
    else
      /* Finally, the hblank neither : in super sidekicks 3, sometimes the
       * programs acks the irq, but not always !!! If we don't disable it here
       * then we make a stack overflow !!! */
      display_position_interrupt_pending = 0;
#endif
  }
  else {
#ifdef RAINE_DEBUG
    if (s68000_interrupts & 7)
      print_debug("should be cleared. %x\n",s68000_interrupts);
#endif
    s68000_interrupts &= ~7;
    irq.wait_for_vbl_ack = 0;
  }
}

static void check_hbl() {
    if (irq.start < 0x180 && irq.start > -1000 && (irq.control & IRQ1CTRL_ENABLE)) {
	// irq.start is a timer, in pixels, 0x180 ticks / line
	// so if irq.start < 0x180 then there is a timer interrupt on this
	// line
	debug(DBG_RASTER,"hbl on %d interrupts %x pc %x sr %x irq.start %d\n",scanline,s68000_interrupts,s68000_pc,s68000_sr,irq.start);
	if (irq.control & IRQ1CTRL_AUTOLOAD_REPEAT) {
	    if (irq.pos == 0xffffffff)
		irq.start = -1000;
	    else {
		irq.start = irq.pos + 1;	/* ridhero gives 0x17d */
	    }
	    debug(DBG_RASTER,"irq1 autorepeat %d (scanline %d)\n",irq.start,scanline);
	} else
	    irq.start = -1000;

	if (!vblank_interrupt_pending)
	    /* The vblank_interrupt_pending test is because garou makes a very
	     * special initialization of the hbl line counter just before
	     * fighting opponent 6. It's seen as a negative number and so hbl
	     * on line 0, before the vbl is finished. So this code just
	     * prevents the hbl to start while the vbl is still active.
	     * A better fix would be to fix this initialization... later...
	     */
	    display_position_interrupt_pending = 1;
    }
}

static void neo_irq1pos_w(int offset, UINT16 data)
{
  /* This is entierly from mame, so thanks to them ! */
  if (offset) {
    irq.pos = (irq.pos & 0xffff0000) | data;
    if (raster_frame) {
      debug(DBG_RASTER,"update irq.pos at line %d = %x\n",scanline,irq.pos);
    } else {
      print_debug("update irq.pos %x control %x\n",irq.pos,irq.control);
    }
    // irq.pos is in ticks of the neogeo pixel clock (6 MHz)
    // 1 tick happens for every pixel, not for every scanline.
    // So 1 scanline is 0x180 ticks, and you have 0x180*0x108 ticks for the
    // whole screen. Since the clock is at 6 MHz, this gives in 1s :
    // 6e6/(0x180*0x108) = 59.19 fps
    // Anyway for the current problem : some games like ridhero put 0x17d in
    // irq.pos to get 1 irq1 for every scanline. Since we use only a scanline
    // counter (irq.start), we must use (irq.pos + 3)/0x180 or we would not
    // get the interrupt repeat...
    if (irq.control & IRQ1CTRL_LOAD_LOW)
    {
      /* This version is from the neocdpsp code.
       * Clearly this is a rough approximation (the irq.pos value is not
       * compared at all to any clock), but it seems to work well enough ! */
      get_scanline();
      irq.start = irq.pos + 3;
      if (irq.pos == 0xffffffff)
	irq.start = -1000;
      else {
	  debug(DBG_RASTER,"irq.start = %d on scanline %d from irq.pos %x\n",irq.start,scanline,irq.pos);
      }
      if (irq.start < 0x180 && irq.start > 0 &&
	      (irq.control & IRQ1CTRL_ENABLE)) {
	  check_hbl();
	  update_interrupts();
      }
    }
  } else {
    irq.pos = (irq.pos & 0x0000ffff) | (data << 16);
    if (raster_frame) {
      debug(DBG_RASTER,"update2 irq.pos at line %d = %x\n",scanline,irq.pos);
    } else {
      print_debug("update2 irq.pos %x\n",irq.pos);
    }
  }
}

static int offx, maxx;

static void update_raster() {
    start_line -= START_SCREEN;
    debug(DBG_RASTER,"draw_sprites between %d and %d\n",start_line,scanline-START_SCREEN);
    draw_sprites(0,381,start_line,scanline-START_SCREEN);
    blit(GameBitmap,raster_bitmap,16,start_line+16,
	    0,start_line,
	    neocd_video.screen_x,
	    scanline-start_line);
    start_line = scanline;
}

static void write_videoreg(UINT32 offset, UINT32 data) {
    // Called LSPC in the neogeo api documentation
  switch((offset >> 1) & 7) {
    case 0x00: // Address register
      video_pointer = data;
      // debug(DBG_IRQ,"set video pointer to %x from %x\n",data,s68000_pc);
      break;
    case 0x01:  // Write data register
      if (raster_frame && scanline > start_line && raster_bitmap &&
	      scanline < end_screen &&
	      neogeo_vidram[video_pointer] != data) {
	  // Must draw the upper part of the screen BEFORE changing the sprites
	  // Disabled the scanline test because ghostlop updates its screen
	  // at line 240 !
	  update_raster();
      }
      neogeo_vidram[video_pointer] = data;

      video_pointer += video_modulo;
      // video_pointer = (video_pointer & 0x8000) | ((video_pointer + video_modulo) & 0x7fff);
      break;
    case 0x02: video_modulo = data; break; // Automatic increment register
    case 0x03: // Mode register
	       neogeo_frame_counter_speed=(data>>8);
	       // printf("counter speed %d\n",neogeo_frame_counter_speed);
	       irq.control = data & 0xff;
	       debug(DBG_IRQ,"irq.control = %x at line %d\n",data,scanline);
	       break;
    case    4: neo_irq1pos_w(0,data); /* timer high register */    break;
    case    5: neo_irq1pos_w(1,data); /* timer low */    break;
    case    6:    /* IRQ acknowledge */
	       if (data & 0x01) irq3_pending = 0;
	       debug(DBG_IRQ,"irq ack %d\n",data);
	       if (data & 0x02) display_position_interrupt_pending = 0;
	       if (data & 0x04) vblank_interrupt_pending = 0;
	       update_interrupts();
	       break;
    case 0x07: break; /* LSPC2 : timer stop switch - indicate wether the hbl
			 interrupt should also count the supplemental lines
			 in pal mode. AFAIK all the games are NTSC, this is
			 not supported.
			 Anyway, the doc says only bit 0 is used :
			 default value = 1
			 bit 0 = 0 timer counter is stopped when in pal mode
			 bit 0 = 1 when in pal, timer counter is stopped 32
			 horizontal lines
 */
  }
}

static UINT16 read_videoreg(UINT32 offset) {
  switch((offset >> 1) & 3) {
    case 0:
    case 1: return neogeo_vidram[video_pointer];
    case 2: return video_modulo;
    case 3:
	    /*
	     * From mame :
	     *
	     The format of this very important location is:  AAAA AAAA A??? BCCC

	     A is the raster line counter. mosyougi relies solely on this to do the
	     raster effects on the title screen; sdodgeb loops waiting for the top
	     bit to be 1; zedblade heavily depends on it to work correctly (it
	     checks the top bit in the IRQ1 handler).
	     B is definitely a PAL/NTSC flag. Evidence:
	     1) trally changes the position of the speed indicator depending on
	     it (0 = lower 1 = higher).
	     2) samsho3 sets a variable to 60 when the bit is 0 and 50 when it's 1.
	     This is obviously the video refresh rate in Hz.
	     3) samsho3 sets another variable to 256 or 307. This could be the total
	     screen height (including vblank), or close to that.
	     Some games (e.g. lstbld2, samsho3) do this (or similar):
	     bclr    #$0, $3c000e.l
	     when the bit is set, so 3c000e (whose function is unknown) has to be
	     related
	     C animation counter lower 3 bits
	     */
	    /* Ok, so to sum up, there are 264 video lines, only 224 displayed
	     * and the counter goes from 0xf8 to 0x1ff with a twist... ! */
	    {
	      get_scanline();
	      int vcounter = scanline + 0x100;
	      if (vcounter >= 0x200) vcounter -= 0x108; // to have the range f8..1ff
	      // All the dev docs said about this was that the highest bit was 1 for the displayed area
	      // which is actually wrong ! This counter goes from f8 to 1ff, and the vbl starts at line f0
	      // ends at line $10, so much more than 8 lines are not visible on screen !
	      debug(DBG_RASTER,"access vcounter %x frame_counter %x from pc=%x scanline=%d final value %x\n",vcounter,neogeo_frame_counter,s68000_pc,scanline,(vcounter << 7) | (neogeo_frame_counter & 7));

	      return (vcounter << 7) | ((neogeo_frame_counter) & 7);
	    }
  }
  return 0xffff;
}

/*************************************
 * From mame :
 *
 *  Watchdog
 *
 *
 *    - The watchdog timer will reset the system after ~0.13 seconds
 *     On an MV-1F MVS system, the following code was used to test:
 *        000100  203C 0001 4F51             MOVE.L   #0x14F51,D0
 *        000106  13C0 0030 0001             MOVE.B   D0,0x300001
 *        00010C  5380                       SUBQ.L   #1,D0
 *        00010E  64FC                       BCC.S    *-0x2 [0x10C]
 *        000110  13C0 0030 0001             MOVE.B   D0,0x300001
 *        000116  60F8                       BRA.S    *-0x6 [0x110]
 *     This code loops long enough to sometimes cause a reset, sometimes not.
 *     The move takes 16 cycles, subq 8, bcc 10 if taken and 8 if not taken, so:
 *     (0x14F51 * 18 + 14) cycles / 12000000 cycles per second = 0.128762 seconds
 *     Newer games force a reset using the following code (this from kof99):
 *        009CDA  203C 0003 0D40             MOVE.L   #0x30D40,D0
 *        009CE0  5380                       SUBQ.L   #1,D0
 *        009CE2  64FC                       BCC.S    *-0x2 [0x9CE0]
 *     Note however that there is a valid code path after this loop.
 *
 *     The watchdog is used as a form of protecetion on a number of games,
 *     previously this was implemented as a specific hack which locked a single
 *     address of SRAM.
 *
 *     What actually happens is if the game doesn't find valid data in the
 *     backup ram it will initialize it, then sit in a loop.  The watchdog
 *     should then reset the system while it is in this loop.  If the watchdog
 *     fails to reset the system the code will continue and set a value in
 *     backup ram to indiate that the protection check has failed.
 *
 *************************************/

static void watchdog_w(UINT32 offset, UINT16 data)
{
    /* Ok, maybe there is a watchdog after all, it takes 0.13s from mame to
     * reset the hardware, which is 8 frames. With 8, mslug resets at the
     * beginning of stage 3, 9 seems to be ok. I'll keep this value for now */
  watchdog_counter = 9;
}

#define NEO_VECTORS 0x80

static UINT8 game_vectors[NEO_VECTORS], game_vectors_set;

static void    neogeo_select_bios_vectors (int bit) {
    if (is_neocd()) {
	if (bit) {
	    print_debug("set game vectors\n");
	    if (game_vectors_set) {
		print_debug("already set\n");
	    } else {
		memcpy(RAM,game_vectors,NEO_VECTORS);
		game_vectors_set = 1;
	    }
	} else {
	    if (game_vectors_set) {
		memcpy(game_vectors,RAM,NEO_VECTORS);
		game_vectors_set = 0;
	    }
	    print_debug("set bios vectors\n");
	    memcpy(RAM, neocd_bios, NEO_VECTORS);
	}
	return;
    }
    // In neogeo it's easier for once, game_vectors are set once only
    if (bit) {
	if (!game_vectors_set) {
	    print_debug("set game vectors\n");
	    memcpy(ROM,game_vectors,NEO_VECTORS);
	    game_vectors_set = 1;
	}
    } else {
	if (game_vectors_set) {
	    print_debug("set bios vectors\n");
	    memcpy(ROM, neocd_bios, NEO_VECTORS);
	    game_vectors_set = 0;
	}
    }
}

void update_game_vectors() {
  memcpy(game_vectors,RAM,NEO_VECTORS);
  game_vectors_set = 1;
  print_debug("game vectors updated\n");
}

static int fixed_layer_source;

static void neogeo_set_fixed_layer_source(UINT8 data)
{
  // This is used to select the gfx source (cartridge or bios)
  // so maybe it's not used in the neocd version ?
  fixed_layer_source = data;
}

static int palbank;

static void neogeo_set_palette_bank(int bit) {
  if (palbank != bit) {
    palbank = bit;
    if (is_neocd())
	RAM_PAL = RAM + 0x230000 + 0x2000*palbank;
    else
	RAM_PAL = RAM + 0x040000 + 0x2000*palbank;
    set_68000_io(0,0x400000, 0x401fff, NULL, RAM_PAL);
    print_debug("palbank %d\n",bit);
  }
}

static int last_cdda_cmd, last_cdda_track;

static UINT16 audio_cpu_bank_select(UINT16 offset) {
    UINT8 *rom = load_region[REGION_CPU2];
    if (!rom) return 0xffff; // neogeo only
    UINT16 region = (offset & 0xf) - 8;
    if (region > 3) {
	print_debug("audio_cpu_bank_select: bad region %d from offset %x\n",region,offset);
	return 0xffff;
    }
    UINT8 bank = offset >> 8;
    int address_mask = get_region_size(REGION_CPU2) - 1;
    UINT32 adr = ((bank << (11 + region)) & address_mask);
    if (zbank[region] != adr) {
	zbank[region] = adr;
	print_debug("audio_cpu_bank_select: adr %x region %d bank %d\n",adr,region,bank);
	// z80_set_read_db(0,(3-region),&rom[adr]);
	/* Sadly, at least some of these banks are code banks, a memcpy seems
	 * mandatory. A good game to test this : sengoku2, it jumps to a bank
	 * during its initialisation, before receiving anything from the 68k */
	switch (3 - region) {
	case 0: memcpy(Z80ROM+0x8000,&rom[adr],0x4000); break;
	case 1: memcpy(Z80ROM+0xc000,&rom[adr],0x2000); break;
	case 2: memcpy(Z80ROM+0xe000,&rom[adr],0x1000); break;
	case 3: memcpy(Z80ROM+0xf000,&rom[adr],0x0800); break;
	}
    }
    return 0;
}

static void z80_set_audio_bank(UINT16 region, UINT16 bank) {
    audio_cpu_bank_select((bank << 8)|(region+8));
}

static void set_68k_bank(UINT32 offset, UINT16 data) {
    if (data != bank_68k) {
	bank_68k = data;
	int n = ((data & 7) + 1)*0x100000;
	if (n + 0x100000 > get_region_size(REGION_CPU1)) {
	    n = 0x100000;
	    print_debug("set_68k_bank: received data %x too high\n",data);
	}
	print_debug("set_68k_bank set bank %d\n",n);
	UINT8 *adr = load_region[REGION_CPU1]+n;
	set_68000_io(0,0x200000,0x2fffff, NULL, adr);
    }
}

static void restore_bank() {
  int new_bank = palbank;
  palbank = -1;
  neogeo_set_palette_bank(new_bank);
  print_debug("palette bank restored %d\n",new_bank);
  // now also restore the loading progress status...
  if (is_neocd()) {
      neocd_lp.sectors_to_load = 0;
      if (neocd_lp.bytes_loaded) {
	  // saved in the middle of loading a file, we'd better reload it !
	  neocd_lp.file_to_load--;
	  neocd_lp.bytes_loaded = 0;
      }
      if (neocd_lp.function)
	  current_game->exec = &loading_progress_function;
      else
	  current_game->exec = &execute_neocd;
      // These last 2 should have been saved but I guess I can just reset them...
      last_cdda_cmd = 0;
      last_cdda_track = 0;
      restore_override(0);
  } else {
      UINT32 znew[4];
      int n;
      memcpy(znew,zbank,sizeof(znew));
      memset(zbank,0xff,sizeof(zbank));
      for (n=0; n<4; n++) {
	  UINT32 bank = znew[n]>>11;
	  UINT32 region = znew[n]-(bank << 11);
	  audio_cpu_bank_select((bank << 8)|(region+8));
      }
      game_vectors_set = 1-game_vectors_set;
      neogeo_select_bios_vectors(1-game_vectors_set);
      if (get_region_size(REGION_CPU1) > 0x100000 && !pvc_cart) {
	  int old = bank_68k;
	  bank_68k = 255;
	  set_68k_bank(0,old);
      }
  }
  print_debug("end restore bank\n");
}

static void system_control_w(UINT32 offset, UINT16 data)
{
  offset >>=1;
  UINT8 bit = (offset >> 3) & 0x01;

  switch (offset & 0x07)
  {
    default:
    case 0x00: dark_screen = bit; break;
    case 0x01: neogeo_select_bios_vectors(bit); break;
    case 0x05: neogeo_set_fixed_layer_source(bit); break;
    case 0x06: saveram.unlock = bit; break;
    case 0x07: neogeo_set_palette_bank(bit); break;

    case 0x02: /* unknown - HC32 middle pin 1 */ // mc 1 write enable
    case 0x03: /* unknown - uPD4990 pin ? */     // mc 2 write enable
    case 0x04: /* unknown - HC32 middle pin 10 */ // mc register select/normal
	       // writes 0 here when the memory card has been detected
	       // print_debug("PC: %x  Unmapped system control write.  Offset: %x  bank: %x data %x return %x ret2 %x\n", s68000_pc, offset & 0x07, bit,data,LongSc(s68000context.areg[7]),LongSc(s68000context.areg[7]+4));
	       break;
  }
}

static char config_game_name[150];

static struct YM2610interface ym2610_interface =
{
  1,
  8000000,
  { (180|(OSD_PAN_CENTER << 8)) },
  { 0 },
  { 0 },
  { 0 },
  { 0 },
  { z80_irq_handler },	/* irq */
  { REGION_YMSND_DELTAT },	/* delta_t */
  { REGION_SMP1 },	/* adpcm */
  { YM3012_VOL(255,OSD_PAN_LEFT,255,OSD_PAN_RIGHT) },
};

struct SOUND_INFO sound_neocd[] =
{
  { SOUND_YM2610,  &ym2610_interface,  },
  { 0,             NULL,               },
};

static void restore_memcard() {
  char path[1024];
  if (!neogeo_memorycard) return;

  snprintf(path,1024,"%ssavedata" SLASH "%s%s.bin", dir_cfg.exe_path, current_game->main_name,(is_neocd() ? "" : "-neogeo")); // 1st try game name in savedata
  FILE *f = fopen(path,"rb");
  memcard_write = 0;
  if (!f) {
    snprintf(path,1024,"%s%smemcard.bin",neocd_dir,SLASH); // otherwise try this
    f = fopen(path,"rb");
  }
  if (f) {
    print_debug("memcard read from %s\n",path);
    fread(neogeo_memorycard,size_mcard,1,f);
    fclose(f);
  } else if (is_neocd()) {
      // Format the memory card : some games like neocd magdrop2 hang instead
      // of proposing to format the card, and it doesn't seem so hard to do...
      // I restrict this to neocd, neogeo has 2 possible card types both of
      // different sizes, so it's unlikely to be compatible !
      UINT8 *m = neogeo_memorycard;
      m[1] = 5; m[0xa] = 0x40; m[0xc] = 2; m[0xd] = m[0xe] = 0x1a;
      int n;
      for (n=0x10; n<0x20; n++)
	  m[n] = 0x20;
      // 3rd line : bizarre signature...
      strcpy((char*)&m[0x20],"N@EDOH-LGPETOX\x80\\");
      m[0x30] = 2;
      int a = 0, val = 0x80; n = 0x40;
      do {
	  if (n == 0x240) {
	      for (n=0x240; n<=0x24c; n++)
		  m[n] = m[n+0x80] = 2;
	      n = 0x340;
	      a = 6;
	  }
	  m[n] = (n < 0x340 ? 0xff : a);
	  m[n+1] = val;
	  m[n+2] = a;
	  val += 4;
	  m[n+3] = val;
	  val += 4;
	  if (val > 0xff) {
	      val -= 0x100;
	      a++;
	  }
	  n += 4;
      } while (n < 0x2000);
  }

  if (saveram.ram) {
      snprintf(path,1024,"%ssavedata" SLASH "neogeo.saveram", dir_cfg.exe_path);
      f = fopen(path,"rb");
      if (f) {
	  fread(saveram.ram, 0x10000, 1, f);
	  if (saveram.ram[0x10] == 66)
	      ByteSwap(saveram.ram,0x10000);
	  fclose(f);
      }
  }
}

static void save_memcard() {
    char path[1024];
    if (memcard_write) {
	snprintf(path,1024,"%ssavedata" SLASH "%s%s.bin", dir_cfg.exe_path, current_game->main_name,(is_neocd() ? "" : "-neogeo")); // 1st try game name in savedata
	FILE *f = fopen(path,"wb");
	if (f) {
	    fwrite(neogeo_memorycard,size_mcard,1,f);
	    fclose(f);
	}
	memcard_write = 0;
    }
    if (!is_neocd() && saveram.ram) {
	if (saveram.unlock) {
	    MessageBox(gettext("Warning"), gettext("Can't save the backup RAM, it's unlocked"),gettext("OK"));
	    return;
	}

	char str[16];
	memcpy(str,saveram.ram+0x10,16);
	ByteSwap((UINT8*)str,16);
	str[15] = 0;
	if (!strcmp(str,"BACKUP RAM OK !")) {
	    // Quick check the backup ram was really used
	    // the bios is supposed to write this after testing it
	    // see http://wiki.neogeodev.org/index.php?title=Backup_RAM
	    snprintf(path,1024,"%ssavedata" SLASH "neogeo.saveram", dir_cfg.exe_path);
	    FILE *f = fopen(path,"wb");
	    if (f) {
		ByteSwap(saveram.ram,0x10000);
		fwrite(saveram.ram,0x10000,1,f);
		ByteSwap(saveram.ram,0x10000);
		fclose(f);
	    }
	}
    }
}

int neocd_id;
/* The spr_disabled...video_enabled are usefull to clean up loading sequences
 * they are really used by the console, verified on a youtube video for aof3
 * without them, there are some flashing effects */

// There seems to be a majority of games using 304x224, so the default value
// for the width is 304 (when left blank).
const NEOCD_GAME games[] =
{
  { "nam1975",    0x0001 },
  { "bstars",     0x0002, 320 },
  { "tpgolf",     0x0003, 304 },
  { "mahretsu",   0x0004, },
  { "maglord",    0x0005, 320 },
  { "ridhero",    0x0006 },
  { "alpham2",    0x0007 },
  { "ncombat",    0x0009 },
  { "cyberlip",   0x0010 },
  { "superspy",   0x0011 },
  { "mutnat",     0x0014 },
  { "sengoku",    0x0017, 320 },
  { "burningf",   0x0018 },
  { "lbowling",   0x0019 },
  { "gpilots",    0x0020 },
  { "joyjoy",     0x0021 },
  { "bjourney",   0x0022, 320 },
  { "lresort",    0x0024 },
  { "2020bb",     0x0030 },
  { "socbrawl",   0x0031 },
  { "roboarmy",   0x0032 },
  { "fatfury1",    0x0033 },
  { "fbfrenzy",   0x0034 },
  { "crswords",   0x0037 },
  { "rallych",    0x0038 },
  { "kotm2",      0x0039 },
  { "sengoku2",   0x0040 },
  { "bstars2",    0x0041 },
  { "3countb",    0x0043, 320 },
  { "aof",        0x0044, 304 },
  { "samsho",     0x0045, 320 },
  { "tophuntr",   0x0046, 320 },
  { "fatfury2",   0x0047, 320 },
  { "janshin",    0x0048 },
  { "androdun",   0x0049 },
  { "ncommand",   0x0050 },
  { "viewpoin",   0x0051 },
  { "ssideki",    0x0052 },
  { "wh1",        0x0053, 320 },
  { "crsword2",   0x0054 },
  { "kof94",      0x0055, 304 },
  { "aof2",       0x0056, 304 },
  { "wh2",        0x0057 },
  { "fatfursp",   0x0058, 320 },
  { "savagere",   0x0059 },
  { "ssideki2",   0x0061, 320 },
  { "samsho2",    0x0063, 320 },
  { "wh2j",       0x0064 },
  { "wjammers",   0x0065 },
  { "karnovr",    0x0066 },
  { "pspikes2",   0x0068, 320 },
  { "aodk",       0x0074 },
  { "sonicwi2",   0x0075, 320 },
  { "galaxyfg",   0x0078 },
  { "strhoop",    0x0079 },
  { "quizkof",    0x0080, 304 },
  { "ssideki3",   0x0081, 320 },
  { "doubledr",   0x0082, 320 },
  { "pbobblen",   0x0083, 320 },
  { "kof95",      0x0084, 304 },
  { "ssrpg",      0x0085 },
  { "samsho3",    0x0087 },
  { "stakwin",    0x0088, 320 },
  { "pulstar",    0x0089, 320 },
  { "whp",        0x0090, 320 },
  { "kabukikl",   0x0092, 320 },
  { "gowcaizr",   0x0094 },
  { "rbff1",      0x0095, 320 },
  { "aof3",       0x0096, 304 },
  { "sonicwi3",   0x0097, 320 },
  { "fromanc2",   0x0098 },
  { "turfmast",   0x0200 }, // if speed hack -> arcade game on title screen -> black screen !
  { "mslug",      0x0201,304 },
  { "puzzledp",   0x0202 },
  { "mosyougi",   0x0203 },
  { "adkworld",   0x0204 },
  { "ngcdsp",     0x0205 },
  { "neomrdo",    0x0207 },
  { "zintrick",   0x0211 },
  { "overtop",    0x0212 },
  { "neodrift",   0x0213, 304 },
  { "kof96",      0x0214, 304 },
  { "ninjamas",   0x0217, 320 },
  { "ragnagrd",   0x0218, 320 },
  { "pgoal",      0x0219 },
  { "ironclad",   0x0220, 304 },
  { "magdrop2",   0x0221 },
  { "samsho4",    0x0222, 320 },
  { "rbffspec",   0x0223, 320 },
  { "twinspri",   0x0224 },
  { "kof96ngc",   0x0229 },
  { "breakers",   0x0230, 320 },
  { "kof97",      0x0232, 304 },
  { "lastblad",   0x0234, 320 },
  { "rbff2",      0x0240, 320 },
  { "mslug2",     0x0241 },
  { "kof98",      0x0242, 304 },
  { "lastbld2",   0x0243, 320 },
  { "kof99",      0x0251, 304 },
  { "fatfury3",   0x069c, 320 },
  { "neogeocd",   0x0000 },
  { NULL, 0 }
};

// isprint is broken in windows, they allow non printable characters !!!
#define ischar(x) ((x)>=32) //  && (x)<=127)
static char *old_name;

void neogeo_read_gamename(void)
{
  unsigned char	*Ptr;
  int	temp;

  int region_code = 2; // always take region europe for the game name
  if (is_neocd()) {
      Ptr = RAM + ReadLongSc(&RAM[0x116]+4*region_code);
/*  else
      Ptr = ROM + ReadLongSc(&ROM[0x116]+4*region_code); */
      memcpy(config_game_name,Ptr,80);
      ByteSwap((UINT8*)config_game_name,80);

      for(temp=0;temp<80;temp++) {
	  if (!ischar(config_game_name[temp])) {
	      config_game_name[temp]=0;
	      break;
	  }
      }
      while (config_game_name[temp-1] == ' ')
	  temp--;
      config_game_name[temp] = 0;
      temp = 0;
      while (config_game_name[temp] == ' ')
	  temp++;
      if (temp)
	  memcpy(config_game_name,&config_game_name[temp],strlen(config_game_name)-temp+1);
      print_debug("game name : %s\n",config_game_name);
  }

  if (is_neocd())
      neocd_id = ReadWord(&RAM[0x108]);
  else
      neocd_id = ReadWord(&ROM[0x108]);
  // get the short name based on the id. This info is from neocdz...
  game = &games[0];
  while (game->name && game->id != neocd_id)
    game++;

  if (neocd_id == 0x48 || neocd_id == 0x0221 || neocd_id == 0x96 ||
	  neocd_id == 0x200 || // neo turf master
	  neocd_id == 0x92 ) { // kabukikl tst/bmi
    desired_68k_speed = current_neo_frame; // no speed hack for mahjong quest
    // nor for magical drop 2 (it sets manually the vbl bit for the controls
    // on the main menu to work, which makes the speed hack much more complex!
    // And aof3 has a problem on the background on the demo with the water
    // fall if speed hacks are enabled. Didn't investigate why (neocd & neogeo)
  }

  if (!is_neocd())
      return; // it's over for neogeo !

  load_game_config(); // init default region and so on (neocd)
  if (game->id == neocd_id)
      current_game->main_name = game->name;
  else {
      print_debug("warning could not find short name for this game\n");
      current_game->main_name = "neocd"; // resets name in case we don't find
  }
  // Retrieve the long name from the neogeo list instead of the raw name
  // because it's used to find debug dips...
  int n;
  for (n=0; n<game_count; n++)
      if (!strcmp(game_list[n]->main_name, current_game->main_name)) {
	  strcpy(config_game_name,game_list[n]->long_name);
	  if (strlen(config_game_name) > 50) {
	      char *s = strstr(config_game_name," /");
	      if (s) *s = 0;
	      s = strstr(config_game_name," -");
	      if (s) *s = 0;
	  }
	  break;
      }
  current_game->long_name = (char*)config_game_name;
  print_debug("main_name %s\n",current_game->main_name);
  if (old_name != current_game->main_name) {
      // reload config only when name changes
      // to preserve game config like region !
      load_game_config();
      old_name = current_game->main_name;
  }
  region_code = GetLanguageSwitch();
  SetLanguageSwitch(region_code); // update input_buffer[10] with it
  if (region_code > 2) {
      RAM[0x10fec5^1] = region_code - 2;
      RAM[0x10fd83^1] = 2;
  } else
      RAM[0x10fd83^1] = region_code;
  if (memcard_write) {
      print_debug("save_memcard\n");
      save_memcard(); // called after a reset
  } else {
      print_debug("restore_memcard\n");
      restore_memcard(); // called after loading
  }
  /* update window title with game name */
  char neocd_wm_title[160];
  sprintf(neocd_wm_title,"Raine - %s",config_game_name);
  SDL_WM_SetCaption(neocd_wm_title,neocd_wm_title);
}

static struct ROMSW_DATA romsw_data_neocd[] =
{
  { "Japan",           0x00 },
  { "USA",             0x01 },
  { "Europe",          0x02 },
  // Extra regions : normally there is the possibility to encode up to 8
  // regions with the neocdz bios, but they are read by almost all the games
  // from the neo soft dips. The only game which is an exception afaik is
  // kof95.
  { "Portuguese (kof95, others ?)", 3 },
  { NULL,                    0    },
};

static struct ROMSW_INFO neocd_romsw[] =
{
    /* Normally the region dip is in ff011c with a special encoding.
     * Except that since raine doesn't boot the bios it has to place the
     * value directly in ram. ff011c goes to input_buffer[10] 1st... */
  { /* 6 */ 0xff011c , 0x2, romsw_data_neocd },
  // { 0xc00401, 0x2, romsw_data_neocd },
  { 0,        0,    NULL },
};

/* Draw entire Character Foreground */
void video_draw_fix(void)
{
  UINT16 x, y;
  UINT16 code, colour;
  UINT16 *fixarea=&neogeo_vidram[0x7002];
  UINT8 *map;
  UINT8 *fix_usage = video_fix_usage;;
  int garouoffsets[32];

  if (!is_neocd()) {
      if (!aes && (fixed_layer_source == 0 || !load_region[REGION_FIXED])) {
	  neogeo_fix_memory = load_region[REGION_FIXEDBIOS];
	  fix_usage = bios_fix_usage;
	  // banked = 0;
      } else {
	  neogeo_fix_memory = load_region[REGION_FIXED];
	  if (fix_banked && fixed_layer_bank_type == 1) {
	      int garoubank = 0;
	      int k = 0;
	      int y = 0;
	      while (y < 32)
	      {
		  if (neogeo_vidram[0x7500 + k] == 0x0200 && (neogeo_vidram[0x7580 + k] & 0xff00) == 0xff00)
		  {
		      garoubank = neogeo_vidram[0x7580 + k] & 3;
		      garouoffsets[y++] = garoubank;
		  }
		  garouoffsets[y++] = garoubank;
		  k += 2;
	      }
	  }

      }
  }

  for (y=0; y < 28; y++)
  {
    for (x = 0; x < 40; x++)
    {
      code = fixarea[x << 5];

      colour = (code&0xf000)>>12;
      code  &= 0xfff;

      // Since some part of the fix area is in the bios, it would be
      // a mess to convert it to the unpacked version, so I'll keep it packed
      // for now...
      if (fix_banked && fixed_layer_source) {
	  switch(fixed_layer_bank_type) {
	  case 1:
	      code += 0x1000 * (garouoffsets[(y ) & 31] ^ 3);
	      break;
	  case 2:
	      code += 0x1000 * (((neogeo_vidram[0x7500 + ((y + 1) & 31) + 32 * (x / 6)] >> (5 - (x % 6)) * 2) & 3) ^ 3);
	      break;
	  }
      }
      if(fix_usage[code]) {
	// printf("%d,%d,%x,%x\n",x,y,fixarea[x << 5],0x7002+(x<<5));
	MAP_PALETTE_MAPPED_NEW(colour,16,map);
	Draw8x8_Trans_Packed_Mapped_Rot(&neogeo_fix_memory[code<<5],(x<<3)+offx,(y<<3)+16,map);
      }
    }
    fixarea++;
  }
}

static int mousex, mousey;

typedef struct {
    int x,y,sprite,attr,tileno,rzx,zy;
    char name[14];
} tsprite;

static void draw_sprites_capture(int start, int end, int start_line, int end_line) {
    // Version which draws only 1 specific block of sprites !
    int         sx =0,sy =0,oy =0,rows =0,zx = 1, rzy = 1;
    int         offs,count,y;
    int         tileatr,y_control,zoom_control;
    UINT16 tileno;
    char         fullmode=0;
    int         rzx=16,zy=0;
    UINT8 *map;
    int nb_block = 0,new_block;
    int bank = -1;
    int mx,my;
    int fixed_palette = 0;
    tsprite *sprites = NULL;
    int nb_sprites = 0,alloc_sprites = 0;

    GetMouseMickeys(&mx,&my);
    mousex += mx; if (mousex > 320-16) mousex = 320-16;
    if (mousex < -8) mousex = -8;
    mousey += my;
    if (mousey > 224-8) mousey = 224-8;
    if (mousey < -16) mousey = -16;
    // display gun at the end to see something !!!


    for (count=start; count<end;count++) {

	zoom_control = neogeo_vidram[0x8000 + count];
	y_control = neogeo_vidram[0x8200 + count];

	// If this bit is set this new column is placed next to last one
	if (y_control & 0x40) {
	    new_block = 0;
	    sx += (rzx);

	    // Get new zoom for this column
	    zx = (zoom_control >> 8)&0x0F;

	    sy = oy;
	} else {   // nope it is a new block
	    new_block = 1;
	    // Sprite scaling
	    sx = (neogeo_vidram[0x8400 + count]) >> 7;
	    sy = 0x1F0 - (y_control >> 7);
	    rows = y_control & 0x3f;
	    zx = (zoom_control >> 8)&0x0F;

	    rzy = (zoom_control & 0xff)+1;


	    // Number of tiles in this strip
	    if (rows == 0x20)
		fullmode = 1;
	    else if (rows >= 0x21)
		fullmode = 2;   // most games use 0x21, but
	    else
		fullmode = 0;   // Alpha Mission II uses 0x3f

	    if (sy > 0x100) sy -= 0x200;

	    if (fullmode == 2 || (fullmode == 1 && rzy == 0x100))
	    {
		while (sy < -16) sy += 2 * rzy;
	    }
	    oy = sy;

	    if(rows==0x21) rows=0x20;
	    else if(rzy!=0x100 && rows!=0) {
		rows=((rows*16*256)/rzy + 15)/16;
	    }

	    if(rows>0x20) rows=0x20;
	}

	rzx = zx+1;
	// skip if falls completely outside the screen
	if (sx >= 0x140 && sx <= 0x1f0) {
	    // printf("%d,%d,%d continue sur sx count %x\n",sx,sy,rzx,count);
	    continue;
	}

	if ( sx >= 0x1F0 )
	    sx -= 0x200;

	// No point doing anything if tile strip is 0
	if ((rows==0)|| sx < -offx || (sx>= maxx)) {
	    continue;
	}
	if (new_block && capture_mode==1) {
	    // check if sprite block is enabled only here, because there are lots
	    // of bad blocks in sprite ram usually, so we just skip them first
	    if (nb_block < capture_block) {
		// look for next start of block
		do {
		    count++;
		} while ((neogeo_vidram[0x8200 + count] & 0x40) && count < end);
		count--; // continue will increase count again...
		nb_block++;
		continue;
	    }
	    nb_block++;
	    if (nb_block > capture_block+1)
		break;
	}

	offs = count<<6;

	// TODO : eventually find the precise correspondance between rzy and zy, this
	// here is just a guess...
	zy = (rzy >> 4);

	// rows holds the number of tiles in each vertical multisprite block
	for (y=0; y < rows ;y++) {
	    tileno = neogeo_vidram[offs];
	    tileatr = neogeo_vidram[offs+1];
	    offs += 2;
	    if (y)
		// This is much more accurate for the zoomed bgs in aof/aof2
		sy = oy + (((rzy+1)*y)>>4);

	    if (!(irq.control & IRQ1CTRL_AUTOANIM_STOP)) {
		if (tileatr&0x8) {
		    tileno = (tileno&~7)|(neogeo_frame_counter&7);
		} else if (tileatr&0x4) {
		    // printf("animation tileno 4\n");
		    tileno = (tileno&~3)|(neogeo_frame_counter&3);
		}
	    }

	    //         tileno &= 0x7FFF;
	    if (tileno>0x7FFF) {
		// printf("%d,%d continue sur tileno %x count %x\n",sx,sy,tileno,count);
		continue;
	    }

	    if (fullmode == 2 || (fullmode == 1 && rzy == 0xff))
	    {
		if (sy >= 248) {
		    sy -= 2 * (rzy + 1);
		}
	    }
	    else if (fullmode == 1)
	    {
		if (y >= 0x10) sy -= 2 * (rzy + 1);
	    }
	    else if (sy > 0x110) sy -= 0x200;

	    if (((tileatr>>8))&&(sy<end_line && sy+zy>=start_line) && video_spr_usage[tileno])
	    {
		if (one_palette) {
		    if (bank == -1) {
			if (current_bank < 0) {
			    bank = tileatr >> 8;
			    current_bank = assigned_banks = 0;
			    banks[current_bank] = bank;
			} else {
			    if (capture_mode == 1) {
				current_bank++;
				if (current_bank > assigned_banks) {
				    fatal_error("banks error: %d > %d",current_bank,assigned_banks);
				}
			    }
			    bank = banks[current_bank];
			    if (bank != (tileatr >> 8))
				continue;
			}
		    } else if (bank != (tileatr >> 8)){ // 2nd palette of this sprite bank
			// store the new bank for next call (if we don't have it already)
			int new = tileatr >> 8;
			int n;
			int found = 0;
			for (n=0; n<=assigned_banks; n++) {
			    if (banks[n] == new) {
				found = 1;
				break;
			    }
			}
			if (found)
			    continue;
			assigned_banks++;
			if (assigned_banks == MAX_BANKS) {
			    fatal_error("color banks overflow");
			}
			banks[assigned_banks] = tileatr >> 8;
			continue;
		    }
		    char *name;
		    int nb=-1;
		    get_cache_origin(SPR_TYPE,tileno<<8,&name,&nb);
		    if (fdata && nb > -1 && sx+offx+rzx >= 16 && sx+offx < 320+16) {
			if (nb_sprites == alloc_sprites) {
			    alloc_sprites += 20;
			    sprites = realloc(sprites,alloc_sprites*sizeof(tsprite));
			}
			tsprite *spr = &sprites[nb_sprites];
			spr->x = sx+offx-16;
			spr->y = sy;
			spr->sprite = nb/256;
			spr->tileno = tileno;
			spr->rzx = rzx;
			spr->zy = zy;
			spr->attr = tileatr & 3;
			strcpy(spr->name,name);
			nb_sprites++;

			put_override(SPR_TYPE,name,0);
		    }
		}
		MAP_PALETTE_MAPPED_NEW(
			(tileatr >> 8),
			16,
			map);
		if (one_palette) {
		    // in 8bpp, there is 1 reserved color (white)
		    // as a result, the color n is mapped to n+1, and we want a direct
		    // mapping here...
		    if (bitmap_color_depth(GameBitmap) == 8 && !fixed_palette) {
			fixed_palette = 1;
			printf("palette fix\n");
			PALETTE pal2;
			int n;
			for (n=0; n<16; n++) {
			    pal2[n] = pal[map[n]];
			    printf("pal %d = pal %d rgb %02x %02x %02x\n",n,map[n],
				    pal2[n].r,pal2[n].g,pal2[n].b);
			    map[n] = n;
			}
			memcpy(pal,pal2,sizeof(SDL_Color)*16);
			// clear screen with transparent color : color 0.
			clear_game_screen(0);
		    }
		}
		if (sx+offx < 0) {
		    fatal_error("bye: %d,%d rzx %d offx %d",sx+offx,sy+16,rzx,offx);
		}
		// printf("sprite %d,%d,%x\n",sx,sy,tileno);
		if (sx >= mousex && sx < mousex+16 && sy>= mousey && sy < mousey+16)
		    print_ingame(1,"%d,%d,%x",sx,sy,tileno);

		if (!fdata) {
		    if (video_spr_usage[tileno] == 2) // all solid
			Draw16x16_Mapped_ZoomXY_flip_Rot(&GFX[tileno<<8],sx+offx,sy+16,map,rzx,zy,tileatr & 3);
		    else
			Draw16x16_Trans_Mapped_ZoomXY_flip_Rot(&GFX[tileno<<8],sx+offx,sy+16,map,rzx,zy,tileatr & 3);
		}
	    }
	}  // for y
    }  // for count
    if (nb_block <= capture_block && capture_block > 0) {
	capture_block = 0;
	draw_sprites_capture(start,end,start_line,end_line);
	return;
    }
    if (!raine_cfg.req_pause_game)
	print_ingame(1,"block %d",capture_block);
    if (!one_palette)
	disp_gun(0,mousex+offx+8,mousey+16+8);
    if (fdata && sprites) {
	int nb = nb_sprites-1;
	int nb2 = nb-1;
	while (nb > 0) {
	    tsprite *spr = &sprites[nb], *spr2 = &sprites[nb2];
	    if (abs(spr->x - spr2->x) < 16 && abs(spr->y - spr2->y) < 16) {
		// the 2 sprites overlap
		// in this case the last of the list is on top, so I remove nb2
		memmove(&sprites[nb2],&sprites[nb2+1],
			(nb_sprites-nb2)*sizeof(tsprite));
		nb--;
		nb_sprites--;
	    }
	    nb2--;
	    if (nb2 < 0) {
		nb--;
		nb2 = nb-1;
	    }
	}
	nb = nb_sprites-1;
	while (nb >= 0) {
	    tsprite *spr = &sprites[nb];
	    fprintf(fdata,"%d,%d,%x,%d,%s\n",spr->x,spr->y,spr->sprite,spr->attr,
		    spr->name);
	    if (video_spr_usage[tileno] == 2) // all solid
		Draw16x16_Mapped_ZoomXY_flip_Rot(&GFX[spr->tileno<<8],spr->x+16,
			spr->y+16,map,spr->rzx,spr->zy,spr->attr);
	    else
		Draw16x16_Trans_Mapped_ZoomXY_flip_Rot(&GFX[spr->tileno<<8],spr->x+16,
			spr->y+16,map,spr->rzx,spr->zy,spr->attr);
	    nb--;
	}
	free(sprites);
    }
}

static inline void alpha_sprite(UINT32 code, int x,int y,UINT8 *map,int zx,int zy,int flip) {
    int alpha = get_spr_alpha(code);
    if (!alpha) {
	if (video_spr_usage[code] == 2) // all solid
	    return Draw16x16_Mapped_ZoomXY_flip_Rot(&GFX[code<<8],x,y,map,zx,zy,flip);
	return Draw16x16_Trans_Mapped_ZoomXY_flip_Rot(&GFX[code<<8],x,y,map,zx,zy,flip);
    }
    set_alpha(alpha);
    if (video_spr_usage[code] == 2) // all solid
	Draw16x16_Mapped_ZoomXY_Alpha_flip_Rot(&GFX[code<<8],x,y,map,zx,zy,flip);
    else
	Draw16x16_Trans_Mapped_ZoomXY_Alpha_flip_Rot(&GFX[code<<8],x,y,map,zx,zy,flip);
}

static void draw_sprites(int start, int end, int start_line, int end_line) {
    if (!check_layer_enabled(layer_id_data[1])) return;
    if (end_line > 223) end_line = 223;
    if (capture_mode) return draw_sprites_capture(start,end,start_line,end_line);
    int         sx =0,sy =0,oy =0,rows =0,zx = 1, rzy = 1;
    int         offs,count,y;
    int         tileatr,y_control,zoom_control;
    UINT32 tileno;
    char         fullmode=0;
    int         rzx=16,zy=0;
    UINT8 *map;

    for (count=start; count<end;count++) {

	zoom_control = neogeo_vidram[0x8000 + count];
	y_control = neogeo_vidram[0x8200 + count];

	// If this bit is set this new column is placed next to last one
	if (y_control & 0x40) {
	    if (rows == 0) continue; // chain on an erased 3d sprite
	    sx += (rzx);

	    // Get new zoom for this column
	    zx = (zoom_control >> 8)&0x0F;

	    sy = oy;
	    debug(DBG_SPRITES,"init sy = oy %d\n",sy);
	} else {   // nope it is a new block
	    // Sprite scaling
	    sx = (neogeo_vidram[0x8400 + count]) >> 7;
	    sy = 0x1F0 - (y_control >> 7);
	    debug(DBG_SPRITES,"init sy = 0x1f0 - (y_control >> 7) %d\n",sy);
	    rows = y_control & 0x3f;
	    if (rows == 0) continue;
	    zx = (zoom_control >> 8)&0x0F;

	    rzy = zoom_control & 0xff;

	    // Number of tiles in this strip
	    if (rows == 0x20)
		fullmode = 1;
	    else if (rows >= 0x21)
		// The neogeo api doc says it should be 0x21 (specific setting)
		fullmode = 2;   // most games use 0x21, but
	    else
		fullmode = 0;   // Alpha Mission II uses 0x3f

	    /* Super ugly hack : this 1 line fix for sy below is required for games
	     * like neo drift out, but if you enable it without fullmode==2, then
	     * the upper part of the playground is not drawn correctly in super
	     * sidekicks 2 & 3 ! Absolutely no idea why it works like that for now!
	     * So this fix reads something like "if the code is stupid enough to set
	     * fullmode to 2 (which should not happen normally !), then fix its
	     * coordinates this way.
	     * Not generic at all, frustrating, but it works (afaik). */
	    /* If fullmode == 1 for this hack, then the bg in "how to play"
	     * for art of fighting 3 becomes visible so the old test
	     * fullmode == 2 stays when raster frames are enabled, which is
	     * the case for ssideki2 and 3 */
	    if (sy > 0x100 && ((fullmode && !raster_frame) ||
			(fullmode == 2 && raster_frame))) {
		sy -= 0x200;
		debug(DBG_SPRITES,"corr sy sur fullmode -0x200 -> %d\n",sy);
	    }

	    if (fullmode == 2 || (fullmode == 1 && rzy == 0xff))
	    {
		debug(DBG_SPRITES,"loop corr sy now %d\n",sy);
		while (sy < -16) sy += 2 * (rzy + 1);
		debug(DBG_SPRITES,"end loop sy %d\n",sy);
	    }
	    oy = sy;

	    if(rows==0x21) rows=0x20;
	    else if(rzy!=0xff && rows!=0) {
		rows=((rows*16*256)/(rzy+1) + 15)/16;
		// Limit for kabuki end credits !
		if (!fullmode && rows > 16) rows = 16;
	    }

	    if(rows>0x20) rows=0x20;
	}

	rzx = zx+1;

	if ( sx >= 0x1F0 )
	    sx -= 0x200;

	// No point doing anything if tile strip is 0
	if (sx < -offx || (sx>= maxx)) {
	    continue;
	}

	offs = count<<6;

	// TODO : eventually find the precise correspondance between rzy ane zy, this
	// here is just a guess...
	if (rzy)
	    zy = (rzy >> 4) + 1;
	else
	    zy = 0;

	debug(DBG_SPRITES,"before row loop sy %d oy %d\n",sy,oy);
	// rows holds the number of tiles in each vertical multisprite block
	for (y=0; y < rows ;y++) {
	    // 100% specific to neocd : maximum possible sprites $80000
	    // super sidekicks 2 draws the playground with bit $8000 set
	    // the only way to see the playground is to use and $7fff
	    // Plus rasters must be enabled
	    tileatr = neogeo_vidram[offs+1];
	    /* So here is the tileatr format :
	     * bits 0-3 : real attribute bits
	     * bits 4-6 : high part of tile number in neogeo !
	     * bits 8-15: color bank */
	    if (is_neocd())
		tileno = neogeo_vidram[offs] & sprites_mask;
	    else {
		tileno = neogeo_vidram[offs] | ((tileatr << 12) & 0x70000);
		/* A sprite_mask is mandatory here, we must just keep the lower
		 * bits, a division would be incorrect */
		tileno &= sprites_mask;
	    }
	    offs += 2;
	    if (y)
		// This is much more accurate for the zoomed bgs in aof/aof2
		sy = oy + (((rzy+1)*y)>>4);

	    if (!(irq.control & IRQ1CTRL_AUTOANIM_STOP)) {
		if (tileatr&0x8) {
		    tileno = (tileno&~7)|(neogeo_frame_counter&7);
		} else if (tileatr&0x4) {
		    tileno = (tileno&~3)|(neogeo_frame_counter&3);
		}
	    }

	    if (fullmode == 2 || (fullmode == 1 && rzy == 0xff))
	    {
		if (sy >= 248) {
		    debug(DBG_SPRITES,"sy corr 1 old %d new %d\n",sy,sy-2*(rzy+1));
		    sy -= 2 * (rzy + 1);
		}
	    }
	    else if (fullmode == 1)
	    {
		if (y >= 0x10) sy -= 2 * (rzy + 1);
	    }
	    else if (sy > 0x110) sy -= 0x200;

	    if ((sy<=end_line && sy+zy>=start_line) && video_spr_usage[tileno])
	    {
		debug(DBG_SPRITES,"%d,%d,%x zoom %d,%d offs %x\n",sx,sy,tileno,rzx,zy,offs);
		MAP_PALETTE_MAPPED_NEW(
			(tileatr >> 8),
			16,
			map);
		alpha_sprite(tileno,sx+offx,sy+16,map,rzx,zy,tileatr & 3);
	    }
	}  // for y
    }  // for count
    debug(DBG_SPRITES,"\n");
}

static void clear_screen() {
    UINT8 *map;
    if (screen_cleared)
	return;
    screen_cleared = 1;
    /* Not totally sure the palette can be cleared here and not every
     * time the sprites are drawn during a raster interrupt.
     * The doc says the palette should be changed only during vbl to
     * avoid noise... we'll try, I didn't find any game so far changing
     * the palette during an hbl */
    ClearPaletteMap();
    /* Confirmed by neogeo doc : palette 255 for bg */
    if (neocd_lp.function) {
	// Loading animations use only 256 colors, so it's impossible that
	// the screen is cleared with the very last color here...
	// We'll assume color 0 forced here, never saw a loading animation
	// with a background which was not black...
	clear_game_screen(0);
	return;
    }
    MAP_PALETTE_MAPPED_NEW(
	    0xff,
	    16,
	    map);
    switch(display_cfg.bpp) {
    case 8: clear_game_screen(map[15]); break;
    case 15:
    case 16: clear_game_screen(ReadWord(&map[15*2])); break;
    case 32: clear_game_screen(ReadLong(&map[15*4])); break;
    }
}

static void draw_neocd() {
  // Apparently there are only sprites to be drawn, zoomable and chainable
  // + an 8x8 text layer (fix) over them

  clear_screen();
  // The computation of neogeo_frame_counter MUST happen before drawing the
  // sprites, it's because it happens for the real hardware during the vbl
  // and some games like blazstar read it to synchronize animations and
  // expect a very precise value here !
  if (!(irq.control & IRQ1CTRL_AUTOANIM_STOP))
  {
      // printf("draw_neocd: frame %d cpu_frame %d raster %d\n",neogeo_frame_counter,cpu_frame_count,raster_frame);
    if (fc == 0) {
      neogeo_frame_counter++;
      fc=neogeo_frame_counter_speed;
    } else
	fc--;
  }
  if (!video_enabled) {
      return;
  }

  int start = 0, end = 0x300 >> 1;
  if (!spr_disabled && start_line < 224) {
    if (neocd_id == 0x0085 && !capture_mode) {
      // pseudo priority code specific to ssrpg (samurai spirits rpg)
      // it draws the sprites at the begining of the list at the end to have them
      // on top. This code is taken from the japenese source of ncd 0.5 !
      if ((neogeo_vidram[(0x8200 + 2)] & 0x40) == 0 &&
	  (neogeo_vidram[(0x8200 + 3)] & 0x40) != 0) {
	// if a block starts at count = 2, then starts to draw the sprites after
	// this block. This doesn't make any sense, but it seems to work fine
	// for ssrpg... really weird. There shouldn't be any specific code for a
	// game since it's a console... !!!
	start = 6 >> 1;
	while ((neogeo_vidram[0x8200 + start] & 0x40) != 0)
	  start++;
	if (start == 6 >> 1) start = 0;
      }

      do {
	draw_sprites(start,end,start_line,224);
	end = start;
	start = 0;
      } while (end != 0);
    } else
      draw_sprites(0, 381,start_line,224);
  }
  if (raster_frame && start_line > 0) {
      blit(raster_bitmap,GameBitmap,0,0,16,16,neocd_video.screen_x,start_line);
      debug(DBG_RASTER,"draw_neocd: sprites disabled on raster frame blit until line %d\n",start_line);
  }

  if (check_layer_enabled(layer_id_data[0]) && !fix_disabled)
    video_draw_fix();
}

void draw_neocd_paused() {
    clear_screen();
    draw_sprites_capture(0,384,0,224);
    if (check_layer_enabled(layer_id_data[0]) && !fix_disabled)
	video_draw_fix();
}

int exit_to_code = 2;

void set_neocd_exit_to(int code) {
    if (neocd_bios) {
	// Apparently the jump table at the beginning of the rom is stable
	if (exit_to_adr) {
	    neocd_bios[exit_to_adr] = code;
	}
    }
}

static void apply_hack(int pc,char *comment) {
    UINT8 *RAM = get_userdata(CPU_68K_0,pc);
    WriteWord(&RAM[pc],0x4239);
    WriteWord(&RAM[pc+2],0xaa);
    WriteWord(&RAM[pc+4],0);
    current_neo_frame = desired_68k_speed;
    print_ingame(120,gettext("Applied speed hack at %x:%s"),pc,comment);
    print_debug("Applied speed hack at %x:%s\n",pc,comment);
}

static int frame_count;
static int upload_type,mx,my;

static void neogeo_hreset(void)
{
    if (is_neocd() && strcmp(current_game->main_name,"neocd")) // different
	save_game_config(); // to save the region when it has just changed !
    // Setting these 2 to 255 emulates standard inputs, it's not really required, but it makes a cleaner test mode for inputs
    mx = my = 255;
  frame_count = 0;
  fix_cur = -1;
  fix_write = 0;
  spr_min = 0x800000;
  spr_max = 0;
  fix_mask = get_region_size(REGION_FIXED)-1;
  fix_banked = fix_mask > 0x1ffff;
  if (saved_fix)
    restore_fix(0);
  frame_neo = CPU_FRAME_MHz(12,60);

  current_neo_frame = frame_neo;;
  old_name = current_game->main_name;
  direct_fix = -1;
  fix_disabled = 0;
  spr_disabled = 0;
  video_enabled = 1;
  neogeo_set_palette_bank(0);
  memset(&irq,0,sizeof(irq));
  pd4990a_init();
  pending_command = latch = 0;
  last_cdda_cmd = 0;
  last_cdda_track = 0;
  /* Clear last bank of palette so that the bg color of screen is black
   * when you start the game (last color of last bank) */
  memset(RAM_PAL+0xff*0x20,0,0x20);

  video_modulo = video_pointer = 0;
  int offs;
  for (offs=0; offs<8*2; offs += 2)
      system_control_w( offs, 0x00ff);

  if (is_neocd()) {
      RAM[0x115a06 ^ 1] = 0; // clear "load files" buffer
      z80_enabled = 0;

#ifndef BOOT_BIOS
      neogeo_cdrom_load_title();
#endif
      WriteLongSc(&RAM[0x11c810], default_anim); // default anime data for load progress
      // First time init
#if USE_MUSASHI == 2
#ifdef BOOT_BIOS
      REG_PC = ReadLongSc(&neocd_bios[4]); // 0xc00582; // 0xc0a822;
#else
      REG_PC = 0xc00582; // 0xc0a822;
#endif
      m68ki_set_sr(0x2700);
      REG_A[7] = 0x10F300;
      REG_USP = 0x10F400;
      s68000_interrupts = 0;
#else // USE_MUSASHI
#ifdef BOOT_BIOS
      s68000context.pc = ReadLongSc(&neocd_bios[4]); // 0xc00582; // 0xc0a822;
#else
      s68000context.pc = 0xc00582; // 0xc0a822;
#endif
      s68000context.sr = 0x2700;
      s68000context.areg[7] = 0x10F300;
      s68000context.asp = 0x10F400;
      s68000context.interrupts[0] = 0;
#endif
#ifndef BOOT_BIOS
      if (!neogeo_cdrom_process_ipl(NULL)) {
	  ErrorMsg("Error: Error while processing IPL.TXT.\n");
	  ClearDefault();
	  return;
      }
#endif
  } else {
      z80_enabled = 1; // always enabled in neogeo
      irq3_pending = 1;
#if USE_MUSASHI == 2
      REG_A[7] = ReadLongSc(&ROM[0]); // required for at least fatfury3 !
      REG_PC = ReadLongSc(&ROM[4]); // required for at least fatfury3 !
#else
      s68000context.areg[7] = ReadLongSc(&ROM[0]); // required for at least fatfury3 !
      s68000context.pc = ReadLongSc(&ROM[4]); // required for at least fatfury3 !
#endif
      aes = (neogeo_bios == 22 || neogeo_bios == 23);
      if (aes)
	  input_buffer[5] &= 0x7f;
      else
	  input_buffer[5] |= 0x80;
  }
  s68000GetContext(&M68000_context[0]);
  watchdog_counter = 9;
  display_position_interrupt_pending = 0;
  vblank_interrupt_pending = 0;
  if (is_current_game("s1945p") && allowed_speed_hacks) {
      /* This one is special, it tests twice its vbl with a jsr in the middle
       * so applying automatically a speed hack is impossible here, I disable
       * the 1st test with a nop and replace the 2nd with a speed hack, and
       * it's placed here so that it replaces current_neo_frame from the
       * start */
      WriteWord(&ROM[0x514c],0x4e71);
      apply_hack(0x5154,"s1945p");
      WriteWord(&ROM[0x515a],0x4e71);
  }
}

void postprocess_ipl() {
  // called at the end of process_ipl, to setup stuff before emulation really
  // starts. This has to be in a separate function because process_ipl can
  // now be called many times before really finishing to process ipl.txt.

  if (cdrom_speed) {
	  // Force clearing of the screen to be sure to erase the interface when
	  // changing the game resolution
	  int fps = raine_cfg.show_fps_mode;
	  raine_cfg.show_fps_mode = 0;
	  clear_ingame_message_list();
	  clear_screen(0);
	  DrawNormal();
	  raine_cfg.show_fps_mode = fps;
  }
  if (game->width == 320) {
    neocd_video.screen_x = 320;
    offx = 16;
    maxx = 320;
  } else {
    neocd_video.screen_x = 304;
    offx = 16-8;
    maxx = 320-8;
  }
  if (cdrom_speed) {
    /* If loading animations are enabled, then the game name is known only after
     * having started the emulation, so we must reset a few parameters at this
     * time */
      print_debug("neocd reset params\n");
    if (neocd_video.screen_x+2*neocd_video.border_size != GameBitmap->w)
	ScreenChange();
    hs_close();
    hs_open();
    hs_init();
    hist_open("history.dat",current_game->main_name);
    if (!history)
	hist_open("history.dat",parent_name());
    hist_open("command.dat",current_game->main_name);
    read_bld();
  }
  if (cdrom_speed)
    reset_ingame_timer();
  char path[FILENAME_MAX];
  snprintf(path,FILENAME_MAX,"%ssavedata" SLASH "%s.sdips", dir_cfg.exe_path, current_game->main_name);
  load_sdips = exists(path);
#ifndef RAINE_DOS
  init_debug_dips();
#endif
}

/* Upload area : this area is NOT in the neogeo cartridge systems
 * it allows the 68k to access memory areas directly such as the z80 memory
 * the sprites memory and the fix memory to initialize them from the cd for
 * example. */

static UINT8 upload_param[0x10],dma_mode[9*2];

static void do_fix_conv() {
    if (fix_cur > -1) {
	// We have something written in the fix area...
	fix_conv(native_fix,
	       	neogeo_fix_memory + fix_cur, 32,
	       	video_fix_usage + (fix_cur>>5));
	file_cache("upload",fix_cur,32,FIX_TYPE); // for the savegames
	fix_cur = -1;
	fix_write = 0;
    }
}

static void do_spr_conv() {
    if (spr_max > 0) {
	UINT8 *tmp;
	int len = (spr_max-spr_min)/2+2;
	print_debug("do_spr_conv %x,%x len %x\n",spr_min,spr_max,len);
	if (!(tmp = AllocateMem(len))) return;
	int n;
	for (n=spr_min; n<=spr_max; n+=4)
	    WriteWord68k(&tmp[(n-spr_min)/2],ReadWord(&GFX[n]));
	spr_conv(tmp,
		&GFX[spr_min],
		len,
		video_spr_usage+(spr_min>>8));
	file_cache("upload",spr_min,len*2,SPR_TYPE); // for the savegames
	FreeMem(tmp);
	spr_max = 0;
	spr_min = 0x800000;
    }
}

static void upload_cmd_w(UINT32 offset, UINT8 data);

void write_reg_b(UINT32 offset, UINT8 data) {
    switch(offset) {
    case 0xff0061: upload_cmd_w(offset,data); break;
    case 0xff0105: upload_type = data; break;
    case 0xff0111: spr_disabled = data; break;
    case 0xff0115: fix_disabled = data; break;
    case 0xff0119: video_enabled = data; break;
    case 0xff0141: do_spr_conv(); break;
    case 0xff0149: do_fix_conv(); break;
    case 0xff016f: irq.disable = data; break;
    case 0xff0183:
		   if (!data) {
		       print_debug("z80_enable: reset z80\n");
		       z80_enabled = 0;
		   } else {
		       print_debug("z80_enable: received %x\n",data);
		       z80_enabled = 1;
		       /* Reset the z80 when enabled in case its program
			* was completely changed */
		       cpu_reset(CPU_Z80_0);
		       reset_timers();
		   }
		   break;
    case 0xff01a1: spr_bank = data; break;
    case 0xff01a3: pcm_bank = data; break;
    default:
		   print_debug("WB %x,%x\n",offset,data);
    }
}

static int get_upload_type() {
  // return a zone type suitable for the upload area from the upload type
  // This upload type is used for reading bytes from the z80 (instead of
  // whole blocks)
  int zone = 255;
  switch(upload_type) {
    case 0: zone = SPR_TYPE; break;
    case 1: zone = PCM_TYPE; break;
    case 4: zone = Z80_TYPE; break;
    case 5: zone = FIX_TYPE; break;
    default: print_debug("unknown upload type : %d\n",upload_type);
  }
  return zone;
}

static int read_upload(int offset) {
  /* The read is confirmed at least during kof96 demo : it reads the main ram
   * (offset < 0x200000, zone 0) by the upload area instead of accessing
   * directly... so at least it shows this thing is really used after all ! */

    int zone = get_upload_type();
  // print_debug("read_upload: offset %x offset2 %x offset_dst %x zone %x bank %x size %x pc:%x\n",offset,offset2,offset_dst,zone,bank,size,s68000_pc);
  // int bank = m68k_read_memory_8(0x10FEDB);
  offset &= 0xfffff;

  switch (zone & 0xf) {
    case 0x00:  // /* 68000 */          return neogeo_prg_memory[offset^1];
      // return subcpu_memspace[(offset>>1)^1];
      if (offset < 0x200000)
	return RAM[offset^1];
      print_debug("read overflow\n");
      return 0xffff;

    case FIX_TYPE:
      offset >>=1;
      if (offset < 0x20000) {
	  int n = offset / 32 * 32;
	  if (fix_cur > -1 && fix_cur != n && fix_write) // changed char
	      do_fix_conv();
	  if (fix_cur != n) {
	      fix_cur = n;
	      fix_inv_conv(neogeo_fix_memory + fix_cur,
		      native_fix,
		      32);
	  }
	  return native_fix[offset - n] | 0xff00;
      }
      return 0xffff;
    case SPR_TYPE:
      offset += (spr_bank<<20);
      if (offset < 0x800000) return ReadWord(&GFX[offset]);
      return 0xffff;

    case Z80_TYPE:
      if (offset < 0x20000) {
	return Z80ROM[offset >> 1];
      }
      return 0xff;
    case PCM_TYPE:
      offset = (offset>>1) + (pcm_bank<<19);
      if (offset > 0x100000) {
	print_debug("read_upload: pcm overflow\n");
	return 0xffff;
      }
      return PCMROM[offset] | 0xff00;
    default:
      //sprintf(mem_str,"read_upload unimplemented zone %x\n",zone);
      //debug_log(mem_str);
      print_debug("read_upload unmapped zone %x bank %x upload_type %x\n",zone,RAM[0x10FEDB ^ 1],upload_type);
      return -1;
  }
}

static void write_upload(int offset, int data);
static void write_upload_word(UINT32 offset, UINT16 data) {
    int zone = get_upload_type();
    if (zone == Z80_TYPE) {
      // The z80 seems to be the only interesting area for bytes accesses
      // like this...
      offset &= 0x1ffff;
      offset >>= 1;
      Z80ROM[offset] = data;
      return;
    } else if (zone == SPR_TYPE) { // used by the custom bios 07Z !
	offset = ((offset & 0xfffff) + (spr_bank << 20))*2;
	/* Nice little hack here : the sprites in raine are unpacked, which
	 * means they take twice the space of the original sprites. So if we
	 * wrote directly to the offset given it would overwrite sprites
	 * placed before what the game wants to change.
	 * Actually this effect doesn't show a lot in real life because most
	 * games using this are using it from offset 0. I found only samsho3
	 * using this starting at an offset > 0.
	 * Anyway the idea is to double the offset here, so it will write
	 * directly to the sprites to change. Of course it means it will write
	 * a word ever 4 bytes, so we need to re-convert the buffer to a linear
	 * one in do_spr_conv */
	if (offset < 0x800000) {
	    if (offset < spr_min) spr_min = offset;
	    if (offset > spr_max) spr_max = offset;
	    WriteWord(&GFX[offset],data);
	}
	return;
    } else if (zone == PCM_TYPE) {
	offset = ((offset&0xfffff)>>1) + (pcm_bank<<19);
	if (offset < 0x100000) {
	    PCMROM[offset] = data;
	    file_cache("upload",offset,1,PCM_TYPE);
	} else {
	    print_debug("overflow pcm write %x,%x\n",offset,data);
	}
	return;
    } else if (zone == FIX_TYPE) {
	write_upload(offset,data);
	return;
    } else {
      print_debug("direct write to zone %d offset/2 = %x???\n",zone,offset/2);
    }
}

UINT8 *get_target(UINT32 target,int *size) {
    UINT8 *dst;
    if (target < 0x200000)
	dst = &RAM[target];
    else if (target >= 0x400000 && target < 0x402000) {
	target -= 0x400000;
	dst = &RAM_PAL[target];
	if (target + *size*2 > 0x2000)
	    *size = (0x2000-target)/2;
    } else if (target >= 0xe00000) {
	if (upload_type == 0) { // spr
	    /* Huge hack !!! */
	    dst = NULL;
	    print_debug("target in sprites\n");
	} else if (upload_type == 1) { // pcm
	    target = ((target & 0xfffff)>>1) + (pcm_bank<<19);
	    dst = &PCMROM[target];
	    if (target + *size*2 > 0x100000) {
		*size = (0x100000-target)/2;
		print_debug("adjust pcm target size %x\n",*size);
	    }
	} else if (upload_type == 5) { // fix
	    target = (target & 0x3ffff) >> 1;
	    dst = &neogeo_fix_memory[target];
	    if (*size + target > 0x20000) { // in bytes for fix...
		*size = (0x20000 - target);
		print_debug("adjust fix target size %x\n",*size);
	    }
	} else {
	    fatal_error("can't find target %x upload_type %x",target,upload_type);
	}
    } else {
	fatal_error("don't know how to handle target %x",target);
    }
    return dst;
}

static void do_dma(char *name,UINT8 *dest, int max) {
    UINT16 dma = ReadWord(&dma_mode[0]);
    UINT16 upload_fill = ReadWord(&upload_param[8]);
    int upload_src = ReadLongSc(&upload_param[0]);
    int upload_len = ReadLongSc(&upload_param[12]);
    UINT32 target = ReadLongSc(&upload_param[4]);
    int n;
    int fix = (dest == neogeo_fix_memory); // special case for fix !
    if (dma == 0xfe6d || dma == 0xfe3d || dma == 0xe2dd) { // transfer in words
	print_debug("dma transfer from %s (%x) to %x len %x in words (dma %x) from %x type:%x\n",name,upload_src,target,upload_len,dma,s68000_pc,upload_type);
	UINT8 *src = dest;
	if (upload_src < 0x200000) {
	    src += upload_src;
	    if (upload_src + upload_len*2 > max) upload_len = (max-upload_src)/2;
	} else if (upload_len > max/2) upload_len = max/2;
	UINT8 *dst = get_target(target,&upload_len);
	if (dma == 0xe2dd) {
	    // e2dd seems designed specifically for fix...
	    // used by kof99 for all the loading screens
	    if ((target & 0xe00000) != 0xe00000) {
		// Normaly this should go through upload_write,
		// I absolutely need something in the upload area here
		fatal_error("dma e2dd, target %x upload_type %x",target,upload_type);
	    }
	    target -= 0xe00000;
	    for (n=0; upload_len > 0; n+=2, upload_len--) {
		write_upload_word(target + n*2,src[n^1]);
		write_upload_word(target + n*2+2,src[(n+1)^1]);
	    }
	    return;
	}
	if (dst) {
	    for (n=0; upload_len > 0; n+=2, upload_len--) {
		WriteWord(&dst[n],ReadWord(&src[n]));
	    }
	} else { // sprites -> write to upload area
	    for (n=0; upload_len > 0; n+=2, upload_len--) {
		write_upload_word(target + n,ReadWord(&src[n]));
	    }
	}
    } else if (dma == 0xf2dd) { // transfer again but words inverted (useless !)
	// for each word abcd in source, generate abcd and then cdab !
	UINT32 target = ReadLongSc(&upload_param[4]);
	print_debug("dma transfer from %s (%x) to %x len %x in words with inversion from %x\n",name,upload_src,target,upload_len,s68000_pc);
	if (upload_len > max/4) upload_len = max/4;
	UINT8 *src = dest;
	UINT8 *dst = get_target(target,&upload_len);
	// Nothing specific for fix here, it would be reading from fix anyway...
	for (n=0; upload_len > 0; n+=2, upload_len--) {
	    WriteWord(&dst[n*2],ReadWord68k(&src[n]));
	    WriteWord(&dst[n*2+2],ReadWord(&src[n]));
	}
    } else if (dma == 0xfef5) {
	// fill with address WriteLongSc / all adrs
	print_debug("%s fill with adr dma %x src %x len %x from %x\n",name,dma,upload_src,upload_len,s68000_pc);
	if (upload_len > max/4) upload_len = max/4;
	if (upload_src < 0x200000)
	    for (n=upload_src; upload_len > 0; n+=4, upload_len--) {
		WriteLongSc(&dest[n],n);
	    }
	else
	    if (fix) {
		for (n=0; upload_len > 0; n+=4, upload_len--) {
		    write_upload_word(n*2,(upload_src + n) >> 24);
		    write_upload_word(n*2+2,(upload_src + n) >> 16);
		    write_upload_word(n*2+4,(upload_src + n) >> 8);
		    write_upload_word(n*2+6,(upload_src + n));
		}
	    } else {
		for (n=0; upload_len > 0; n+=4, upload_len--) {
		    WriteLongSc(&dest[n],upload_src + n);
		}
	    }
    } else if (dma == 0xcffd) {
	// fill with address WriteLong68k / adr * 2
	print_debug("%s fill with adr dma %x len %x from %x\n",name,dma,upload_len,s68000_pc);
	if (upload_len > max/4) upload_len = max/4;
	if (fix) {
	    for (n=0; upload_len > 0; n+=4, upload_len--) {
		write_upload_word(n*2,(upload_src + n*2)>>24);
		write_upload_word(n*2+2,(upload_src + n*2)>>16);
		write_upload_word(n*2+4,(upload_src + n*2)>>8);
		write_upload_word(n*2+6,(upload_src + n*2));
	    }
	} else {
	    for (n=0; upload_len > 0; n+=4, upload_len--) {
		WriteLong68k(&dest[n],upload_src + n*2);
	    }
	}
    } else {
	print_debug("%s fill with %x dma %x src %x len %x from %x\n",name,upload_fill,dma,upload_src,upload_len,s68000_pc);
	if (upload_len > max/2) upload_len = max/2;
	for (n=(upload_src < 0x200000 ? upload_src : 0); upload_len > 0; n+=2, upload_len--) {
	    if (fix) {
		write_upload_word(n*2,upload_fill);
		write_upload_word(n*2+2,upload_fill);
	    } else
		WriteWord(&dest[n],upload_fill);
	}
    }
}

static void upload_cmd_w(UINT32 offset, UINT8 data) {
  if (data == 0x40) {
    int upload_src = ReadLongSc(&upload_param[0]);
    int upload_len = ReadLongSc(&upload_param[12]);
    UINT16 dma = ReadWord(&dma_mode[0]);
    if (upload_len && upload_src) {
	if (dma == 0xffdd || dma == 0xffcd || dma == 0xcffd || dma == 0xfef5 ||
		dma == 0xfe6d || dma == 0xf2dd || dma == 0xfe3d ||
	       	dma == 0xe2dd) {
	    // ffdd is fill with data word
	    // ffcd would be the same ??? not confirmed, see code at c08eca
	    // for example, it looks very much the same !!!
	    // cffd is fill with address apparently !
	    if (upload_src == 0x400000) {
		do_dma("palette", RAM_PAL, 0x2000);
	    } else  if (upload_src < 0x200000) {
		do_dma("ram",RAM,0x200000);
	    } else if (upload_src == 0x800000) // memory card !
		do_dma("memory card", neogeo_memorycard,8192);
	    else if (upload_src >= 0xc00000 && upload_src <= 0xc80000) // move
		do_dma("bios",&neocd_bios[upload_src - 0xc00000],
			0xc80000-upload_src);
	    else if (upload_type == 1) { // pcm fill...
		do_dma("pcm",PCMROM,0x100000);
	    } else if (upload_type == 4) // z80 fill !
		do_dma("z80", Z80ROM, 0x10000);
	    else if (upload_type == 5) { // fix
		do_dma("fix",neogeo_fix_memory,0x20000);
	    } else if (upload_type == 0) // spr
		do_dma("spr",&GFX[spr_bank<<20],0x800000-(spr_bank<<20));
	    else {
		print_debug("unknown fill %x upload_type %x dma %x from %x\n",upload_src,upload_type,dma,s68000_pc);
	    }
	} else {
	    print_debug("upload: unknown dma %x from %x\n",dma,s68000_pc);
	}
    }
  }
}

static void write_upload(int offset, int data) {
  // int zone = RAM[0x10FEDA ^ 1];
  // int size = ReadLongSc(&RAM[0x10FEFC]);
  if (upload_type == 0) { // spr
      // the sprites seem to be the only one where the bios dares to write
      // bytes directly (the bank in the highest bytes of the addresses
      // actually !).
      offset &= 0xfffff;
      offset += (spr_bank<<20);
      offset *= 2;
      if (offset < 0x800000) {
	  UINT8 *dest = GFX + (offset ^1);
	  if (offset < spr_min) spr_min = offset;
	  if (offset > spr_max) spr_max = offset;
	  *dest = data;
      } else
	  printf("sprite wb overflow %x\n",offset);
  } else if (upload_type == 5) { // fix
      offset &= 0x3ffff;
      offset >>=1;
      if (offset < 0x20000) {
	  int n = offset / 32 * 32;
	  if (fix_cur > -1 && fix_cur != n && fix_write) // changed char
	      do_fix_conv();
	  if (fix_cur != n) {
	      fix_cur = n;
	      fix_write = 1;
	      fix_inv_conv(neogeo_fix_memory + fix_cur,
		      native_fix,
		      32);
	  }
	  native_fix[offset - n] = data;
      }
  } else
      write_upload_word(offset,data);
}

static void load_files(UINT32 offset, UINT16 data) {
  offset = ReadLongSc(&RAM[0x10f6a0]);
  print_debug("load_files command %x from %x offset %x\n",data,s68000_pc,offset);

  if (data == 0x550) {
    if (RAM[0x115a06 ^ 1]>32 && RAM[0x115a06 ^ 1] < 127) {
      neogeo_cdrom_load_files(&RAM[0x115a06]);
    } else {
      print_debug("load_files: name %x 10f6b5 %x sector h %x %x %x\n",RAM[0x115a06^1],RAM[0x10f6b5^1],RAM[0x76C8^1],RAM[0x76C9^1],RAM[0x76Ca^1]);
    }
    // irq.disable : during test mode when testing the cd, the bios disables
    // irqs, issues cd commands, and waits for irqs to come back. So I guess
    // the cd commands restore the irqs when they complete, maybe a specific
    // command does the job, but for now it's a guess...
    // I put it here and not in finish_load_files because it can happen even
    // without a filename buffer in the bios...
    irq.disable = 0;
  } else {
    int nb_sec = ReadLongSc(&RAM[0x10f688]);
    print_debug("load_files: unknown command, name %x 10f6b5 %x sector %x %x %x nb_sec %x\n",RAM[0x115a06^1],RAM[0x10f6b5^1],RAM[0x10f6C8^1],RAM[0x10f6C9^1],RAM[0x10f6Ca^1],nb_sec);
  }
}

static void test_end_loading(UINT32 offset,UINT16 data) {
  // This is a weird test for the end of the loading of files made by the bios
  // really this should be done differently, but I don't know most of the
  // commands sent to the cd, and it's too long to try to understand them...
  // To test this : metal slug / start game / choose misson 1, black screen.
  RAM[offset^1] = data; // normal write
  RAM[offset] = data; // duplicate
}


static void cdda_cmd(UINT32 offset, UINT8 data) {
  int track = RAM[0x10F6F9];
  RAM[0x10f6f6 ^ 1] = data;
  // printf("do_cdda %d,%d\n",data,track);
  if (data == 255 && auto_stop_cdda) {
    // apparently the program sends the 255 command just before loading
    // anything from cd (and also at the start of the game).
    // Default is to ignore this command, but if we want to pause the music
    // as the original console, then we must translate this to pause (2)
    data = 2;
  }
  if (data <= 7) {
    if (data != last_cdda_cmd || last_cdda_track != track) {
      print_debug("data : %d %d pc:%x\n",RAM[0x10f6f7],RAM[0x10F6F9],s68000_pc);
      last_cdda_cmd = data;
      last_cdda_track = track;
      do_cdda(data,RAM[0x10f6f8 ^ 1]);
    }
  }
}

void myStop68000(UINT32 adr, UINT8 data) {
    if (!do_not_stop) {
	Stop68000(0,0);
	stopped_68k = 1;
	print_debug("mystop68k: normal stop\n");
    } else {
	print_debug("mystop68k: do_not_stop\n");
	do_not_stop = 0;
    }
    rolled = 0;
#if 0
    // Finally it's not necessary to clear the bit
    // for mslug2 here, I keep the code for reference for now
    if (adr > 0xaa0000) {
	int mask = ~(1<<data);
	adr -= 0xaa0000;
	if (adr & 0x8000)
	    RAM[s68000context.areg[5]-(adr & 0x7fff)] &= mask;
	else
	    RAM[s68000context.areg[5]+(adr & 0x7fff)] &= mask;
	bclr ^= 1;
	printf("bclr %d sr %x\n",bclr,s68000_sr);
    }
#endif
}

static UINT16 my_read_ff011c(UINT32 offset) {
    // The bios always reads the region as a word, so it's handled in read_reg
    // the problem is with the GetLanguageSwitch function, which reads a byte, and doesn't know what to do with the returned word
    // so we must define a byte handler here for it
    return input_buffer[10];
}

static UINT16 read_reg(UINT32 offset) {
  if (offset == 0xff011c)  {
    // only bit 4 of upper byte is tested - setting it to 1 makes some games
    // to freeze after a few frames (tested on futsal)
    // return 0xefff;
    // lowest 2 bits are region, region 3 portugal might not be supported
    int region_code = input_buffer[10];
    // region_code is never read from here in raine because the bios is not
    // booted. I keep the code for reference only.
    return 0xff | (region_code << 8);
  }
  print_debug("RW %x -> ffff [pc=%x]\n",offset,s68000_pc);
  return 0xffff;
}

static void write_pal(UINT32 offset, UINT16 data) {
  /* There are REALLY mirrors of the palette, used by kof96ng at least,
   * see demo / story */
  offset &= 0x1fff;
  WriteWord(&RAM_PAL[offset],data);
/*  get_scanline();
  print_debug("write_pal %x,%x scanline %d\n",offset,data,scanline); */
}

static void kof99_bankswitch_w(UINT32 offset, UINT16 data) {
	int bankaddress;
	static const int bankoffset[64] =
	{
		0x000000, 0x100000, 0x200000, 0x300000,
		0x3cc000, 0x4cc000, 0x3f2000, 0x4f2000,
		0x407800, 0x507800, 0x40d000, 0x50d000,
		0x417800, 0x517800, 0x420800, 0x520800,
		0x424800, 0x524800, 0x429000, 0x529000,
		0x42e800, 0x52e800, 0x431800, 0x531800,
		0x54d000, 0x551000, 0x567000, 0x592800,
		0x588800, 0x581800, 0x599800, 0x594800,
		0x598000,   /* rest not used? */
	};

	/* unscramble bank number */
	data =
		(((data>>14)&1)<<0)+
		(((data>> 6)&1)<<1)+
		(((data>> 8)&1)<<2)+
		(((data>>10)&1)<<3)+
		(((data>>12)&1)<<4)+
		(((data>> 5)&1)<<5);

	bankaddress = 0x100000 + bankoffset[data];

	set_68000_io(0,0x200000,0x2fffff, NULL, &ROM[bankaddress]);
}

static void kof2000_bankswitch_w(UINT32 offset, UINT16 data) {
    /* thanks to Razoola and Mr K for the info */
    int bankaddress;
    static const int bankoffset[64] =
    {
	0x000000, 0x100000, 0x200000, 0x300000, // 00
	0x3f7800, 0x4f7800, 0x3ff800, 0x4ff800, // 04
	0x407800, 0x507800, 0x40f800, 0x50f800, // 08
	0x416800, 0x516800, 0x41d800, 0x51d800, // 12
	0x424000, 0x524000, 0x523800, 0x623800, // 16
	0x526000, 0x626000, 0x528000, 0x628000, // 20
	0x52a000, 0x62a000, 0x52b800, 0x62b800, // 24
	0x52d000, 0x62d000, 0x52e800, 0x62e800, // 28
	0x618000, 0x619000, 0x61a000, 0x61a800, // 32
    };

    /* unscramble bank number */
    data =
	(((data>>15)&1)<<0)+
	(((data>>14)&1)<<1)+
	(((data>> 7)&1)<<2)+
	(((data>> 3)&1)<<3)+
	(((data>>10)&1)<<4)+
	(((data>> 5)&1)<<5);

    bankaddress = 0x100000 + bankoffset[data];

    set_68000_io(0,0x200000,0x2fffff, NULL, &ROM[bankaddress]);
}

static void garou_bankswitch_w(UINT32 offset, UINT16 data) {
    /* thanks to Razoola and Mr K for the info */
    int bankaddress;
    static const int bankoffset[64] =
    {
	0x000000, 0x100000, 0x200000, 0x300000, // 00
	0x280000, 0x380000, 0x2d0000, 0x3d0000, // 04
	0x2f0000, 0x3f0000, 0x400000, 0x500000, // 08
	0x420000, 0x520000, 0x440000, 0x540000, // 12
	0x498000, 0x598000, 0x4a0000, 0x5a0000, // 16
	0x4a8000, 0x5a8000, 0x4b0000, 0x5b0000, // 20
	0x4b8000, 0x5b8000, 0x4c0000, 0x5c0000, // 24
	0x4c8000, 0x5c8000, 0x4d0000, 0x5d0000, // 28
	0x458000, 0x558000, 0x460000, 0x560000, // 32
	0x468000, 0x568000, 0x470000, 0x570000, // 36
	0x478000, 0x578000, 0x480000, 0x580000, // 40
	0x488000, 0x588000, 0x490000, 0x590000, // 44
	0x5d0000, 0x5d8000, 0x5e0000, 0x5e8000, // 48
	0x5f0000, 0x5f8000, 0x600000, /* rest not used? */
    };

    /* unscramble bank number */
    data =
	(((data>> 5)&1)<<0)+
	(((data>> 9)&1)<<1)+
	(((data>> 7)&1)<<2)+
	(((data>> 6)&1)<<3)+
	(((data>>14)&1)<<4)+
	(((data>>12)&1)<<5);

    bankaddress = 0x100000 + bankoffset[data];

    set_68000_io(0,0x200000,0x2fffff, NULL, &ROM[bankaddress]);
}

static void garouh_bankswitch_w(UINT32 offset, UINT16 data) {
    /* thanks to Razoola and Mr K for the info */
    int bankaddress;
    static const int bankoffset[64] =
    {
	0x000000, 0x100000, 0x200000, 0x300000, // 00
	0x280000, 0x380000, 0x2d0000, 0x3d0000, // 04
	0x2c8000, 0x3c8000, 0x400000, 0x500000, // 08
	0x420000, 0x520000, 0x440000, 0x540000, // 12
	0x598000, 0x698000, 0x5a0000, 0x6a0000, // 16
	0x5a8000, 0x6a8000, 0x5b0000, 0x6b0000, // 20
	0x5b8000, 0x6b8000, 0x5c0000, 0x6c0000, // 24
	0x5c8000, 0x6c8000, 0x5d0000, 0x6d0000, // 28
	0x458000, 0x558000, 0x460000, 0x560000, // 32
	0x468000, 0x568000, 0x470000, 0x570000, // 36
	0x478000, 0x578000, 0x480000, 0x580000, // 40
	0x488000, 0x588000, 0x490000, 0x590000, // 44
	0x5d8000, 0x6d8000, 0x5e0000, 0x6e0000, // 48
	0x5e8000, 0x6e8000, 0x6e8000, 0x000000, // 52
	0x000000, 0x000000, 0x000000, 0x000000, // 56
	0x000000, 0x000000, 0x000000, 0x000000, // 60
    };

    /* unscramble bank number */
    data =
	(((data>> 4)&1)<<0)+
	(((data>> 8)&1)<<1)+
	(((data>>14)&1)<<2)+
	(((data>> 2)&1)<<3)+
	(((data>>11)&1)<<4)+
	(((data>>13)&1)<<5);

    bankaddress = 0x100000 + bankoffset[data];

    set_68000_io(0,0x200000,0x2fffff, NULL, &ROM[bankaddress]);
}

static void mslug3_bankswitch_w(UINT32 offset, UINT16 data) {
    /* thanks to Razoola and Mr K for the info */
    int bankaddress;
    static const int bankoffset[64] =
    {
	0x000000, 0x020000, 0x040000, 0x060000, // 00
	0x070000, 0x090000, 0x0b0000, 0x0d0000, // 04
	0x0e0000, 0x0f0000, 0x120000, 0x130000, // 08
	0x140000, 0x150000, 0x180000, 0x190000, // 12
	0x1a0000, 0x1b0000, 0x1e0000, 0x1f0000, // 16
	0x200000, 0x210000, 0x240000, 0x250000, // 20
	0x260000, 0x270000, 0x2a0000, 0x2b0000, // 24
	0x2c0000, 0x2d0000, 0x300000, 0x310000, // 28
	0x320000, 0x330000, 0x360000, 0x370000, // 32
	0x380000, 0x390000, 0x3c0000, 0x3d0000, // 36
	0x400000, 0x410000, 0x440000, 0x450000, // 40
	0x460000, 0x470000, 0x4a0000, 0x4b0000, // 44
	0x4c0000, /* rest not used? */
    };

    /* unscramble bank number */
    data =
	(((data>>14)&1)<<0)+
	(((data>>12)&1)<<1)+
	(((data>>15)&1)<<2)+
	(((data>> 6)&1)<<3)+
	(((data>> 3)&1)<<4)+
	(((data>> 9)&1)<<5);

    bankaddress = 0x100000 + bankoffset[data];

    set_68000_io(0,0x200000,0x2fffff, NULL, &ROM[bankaddress]);
}

static int controller,init1;

static void io_control_w(UINT32 offset, UINT32 data) {
    offset &= 0x7f;
    switch (offset/2)
    {
    case 0x00: controller = data & 0x00ff;
	       /* Used by the trackball to select the axis (X/Y).
		* Changed more than once / frame, so we must change the input
		* here */
	       if (GameMouse == 1) {
		   if (controller == 0) input_buffer[1] = mx;
		   else if (controller == 1) input_buffer[1] = my;
		   else input_buffer[1] = init1;
		   // else input_buffer[1] = mx;
	       } else if (GameMouse == 2) {
		   /* Choosing joystick or paddle input is done in the soft
		    * dips. The best way to handle this would be to test the
		    * value of the soft dip in ram and return the right input
		    * here, this way of doing things here is a hack... ! */
		   if (controller & 0x1) {
		       input_buffer[1] = input_buffer[2];
		       input_buffer[3] = input_buffer[4];
		   } else {
		       input_buffer[1] = mx;
		       input_buffer[3] = my;
		   }
	       }
	       break;
    /* case 0x18: if (m_is_mvs) set_output_latch(data & 0x00ff); break;
    case 0x20: if (m_is_mvs) set_output_data(data & 0x00ff); break; */
    case 0x28: pd4990a_control_16_w(0, data); break;
	       //  case 0x30: break; // coin counters
	       //  case 0x31: break; // coin counters
	       //  case 0x32: break; // coin lockout
	       //  case 0x33: break; // coui lockout

    default:
//	       logerror("PC: %x  Unmapped I/O control write.  Offset: %x  Data: %x\n", space.device().safe_pc(), offset, data);
	       break;
    }
}

static void save_ram_wb(UINT32 offset, UINT16 data) {
    if (saveram.unlock) {
	saveram.ram[(offset & 0xffff) ^ 1] = data;
    }
}

static void save_ram_ww(UINT32 offset, UINT16 data) {
    if (saveram.unlock) {
	WriteWord(&saveram.ram[offset & 0xffff], data);
    }
}

static void write_port(UINT16 offset, UINT8 data) {
    switch(offset & 0xff) {
    case 4: YM2610_control_port_0_A_w(offset,data); break;
    case 5: YM2610_data_port_0_A_w(offset,data); break;
    case 6: YM2610_control_port_0_B_w(offset,data); break;
    case 7: YM2610_data_port_0_B_w(offset,data); break;
    /* Port 8 : NMI enable / acknowledge? (the data written doesn't matter)
     * Metal Slug Passes this 35, then 0 in sequence. After a
     * mission begins it switches to 1 */
    case 0xc: set_res_code(offset,data); break;
    default: DefBadWritePortZ80(offset,data);
    }
}

static UINT16 read_port(UINT16 offset) {
    switch(offset & 0xff) {
    case 0: return read_sound_cmd(offset);
    case 4: return YM2610_status_port_0_A_r(offset);
    case 5: return YM2610_read_port_0_r(offset);
    case 6: return YM2610_status_port_0_B_r(offset);
    default:
	    if ((offset & 0xf) >= 8 && (offset & 0xf) <= 0xb)
		return audio_cpu_bank_select(offset); // neogeo only
	    return DefBadReadPort(offset);
    }
}

static int fatfury2_prot_data;

static UINT16 fatfury2_prot_rw(UINT32 offset) {
    UINT16 res = fatfury2_prot_data >> 24;
    switch(offset & 0xfffff) {
    case 0x55550:
    case 0xffff0:
    case 0x00000:
    case 0xff000:
    case 0x36000:
    case 0x36008: return res;
    case 0x36004:
    case 0x3600c: return ((res & 0xf0) >> 4) | ((res & 0x0f) << 4);
    default:
		  printf("unknown fatufury2_prot_rw from %x : %x\n",s68000_pc,offset);
    }
    return 0;
}

static void fatfury2_prot_ww(UINT32 offset, UINT16 data) {
    switch(offset & 0xfffff) {
    case 0x11112: fatfury2_prot_data = 0xff000000; break;
    case 0x33332: fatfury2_prot_data = 0x0000ffff; break;
    case 0x44442: fatfury2_prot_data = 0x00ff0000; break;
    case 0x55552: fatfury2_prot_data = 0xff00ff00; break;
    case 0x56782: fatfury2_prot_data = 0xf05a3601; break;
    case 0x42812: fatfury2_prot_data = 0x81422418; break;
    case 0x55550:
    case 0xffff0:
    case 0xff000:
    case 0x36000:
    case 0x36004:
    case 0x36008:
    case 0x3600c: fatfury2_prot_data <<= 8; break;
    default:
		  printf("unknown protwrite from %x: %x,%x\n",s68000_pc,offset,data);
    }
}

static UINT16 read_bios_w(UINT32 offset) {
    int len = get_region_size(REGION_MAINBIOS);
    return ReadWord(load_region[REGION_MAINBIOS]+(offset & (len-1)));
}

static UINT16 read_bios_b(UINT32 offset) {
    int len = get_region_size(REGION_MAINBIOS);
    offset ^= 1;
    return ReadByte(load_region[REGION_MAINBIOS]+(offset & (len-1)));
}

/***************************************************************************

  From mame, the wizards of decryption :
NeoGeo 'C' ROM encryption

Starting with KOF99, all NeoGeo games have encrypted graphics. Additionally
to that, the data for the front text layer, which was previously stored in
a separate ROM, is stored at the end of the tile data.

The encryption is one of the nastiest implementation of a XOR scheme ever
seen, involving 9 seemingly uncorrelated 256-byte tables. All known games use
the same tables except KOF2000 and MS4 which use a different set.

The 32 data bits of every longword are decrypted in a single step (one byte at
a time), but the values to use for the xor are determined in a convoluted way.
It's actually so convoluted that it's too difficult to describe - please refer
to the source below.
Suffice to say that bytes are handled in couples (0&3 and 1&2), and the two xor
values are taken from three tables, the indexes inside the tables depending on
bits 0-7 and 8-15 of the address, in one case further xored through the table
used in step 5) below. Additionally, the bytes in a couple can be swapped,
depending either on bit 8 of the address, or on bit 16 xored with the table
used in step 4) below.

The 24 address bits are encrypted in five steps. Each step xors 8 bits with a
value taken from a different table; the index inside the table depends on 8
other bits.
0) xor bits  0-7  with a fixed value that changes from game to game
1) xor bits  8-15 depending on bits 16-23
2) xor bits  8-15 depending on bits  0-7
3) xor bits 16-23 depending on bits  0-7
4) xor bits 16-23 depending on bits  8-15
5) xor bits  0-7  depending on bits  8-15

Each step acts on the current value, so e.g. step 4) uses bits 8-15 as modified
by step 2).

[Note: the table used in step 1) is currently incomplete due to lack of data to
analyze]


There are two major weaknesses in this encryption algorithm, that exposed it to
a known plaintext attack.

The first weakness is that the data xor depends on the address inside the
encrypted ROM instead that on the decrypted address; together with the high
concentration of 0x00 and 0xFF in the decrypted data (more than 60% of the
total), this exposed easily recognizable patterns in the encrypted data, which
could be exploited with some simple statistical checks. The deviousness of the
xor scheme was the major difficulty.

The second weakness is that the address scrambling works on 32-bit words. Since
there are a large number of 32-bit values that appear only once in the whole
encrypted ROM space, this means that once the xor layer was broken, a large
table of encrypted-decrypted address correspondencies could be built and
analyzed, quickly leading to the algorithm.

***************************************************************************/

static const UINT8 kof99_type0_t03[256] =
{
	0xfb, 0x86, 0x9d, 0xf1, 0xbf, 0x80, 0xd5, 0x43, 0xab, 0xb3, 0x9f, 0x6a, 0x33, 0xd9, 0xdb, 0xb6,
	0x66, 0x08, 0x69, 0x88, 0xcc, 0xb7, 0xde, 0x49, 0x97, 0x64, 0x1f, 0xa6, 0xc0, 0x2f, 0x52, 0x42,
	0x44, 0x5a, 0xf2, 0x28, 0x98, 0x87, 0x96, 0x8a, 0x83, 0x0b, 0x03, 0x61, 0x71, 0x99, 0x6b, 0xb5,
	0x1a, 0x8e, 0xfe, 0x04, 0xe1, 0xf7, 0x7d, 0xdd, 0xed, 0xca, 0x37, 0xfc, 0xef, 0x39, 0x72, 0xda,
	0xb8, 0xbe, 0xee, 0x7f, 0xe5, 0x31, 0x78, 0xf3, 0x91, 0x9a, 0xd2, 0x11, 0x19, 0xb9, 0x09, 0x4c,
	0xfd, 0x6d, 0x2a, 0x4d, 0x65, 0xa1, 0x89, 0xc7, 0x75, 0x50, 0x21, 0xfa, 0x16, 0x00, 0xe9, 0x12,
	0x74, 0x2b, 0x1e, 0x4f, 0x14, 0x01, 0x70, 0x3a, 0x4e, 0x3f, 0xf5, 0xf4, 0x1d, 0x3d, 0x15, 0x27,
	0xa7, 0xff, 0x45, 0xe0, 0x6e, 0xf9, 0x54, 0xc8, 0x48, 0xad, 0xa5, 0x0a, 0xf6, 0x2d, 0x2c, 0xe2,
	0x68, 0x67, 0xd6, 0x85, 0xb4, 0xc3, 0x34, 0xbc, 0x62, 0xd3, 0x5f, 0x84, 0x06, 0x5b, 0x0d, 0x95,
	0xea, 0x5e, 0x9e, 0xd4, 0xeb, 0x90, 0x7a, 0x05, 0x81, 0x57, 0xe8, 0x60, 0x2e, 0x20, 0x25, 0x7c,
	0x46, 0x0c, 0x93, 0xcb, 0xbd, 0x17, 0x7e, 0xec, 0x79, 0xb2, 0xc2, 0x22, 0x41, 0xb1, 0x10, 0xac,
	0xa8, 0xbb, 0x9b, 0x82, 0x4b, 0x9c, 0x8b, 0x07, 0x47, 0x35, 0x24, 0x56, 0x8d, 0xaf, 0xe6, 0x26,
	0x40, 0x38, 0xc4, 0x5d, 0x1b, 0xc5, 0xd1, 0x0f, 0x6c, 0x7b, 0xb0, 0xe3, 0xa3, 0x23, 0x6f, 0x58,
	0xc1, 0xba, 0xcf, 0xd7, 0xa2, 0xe7, 0xd0, 0x63, 0x5c, 0xf8, 0x73, 0xa0, 0x13, 0xdc, 0x29, 0xcd,
	0xc9, 0x76, 0xae, 0x8f, 0xe4, 0x59, 0x30, 0xaa, 0x94, 0x1c, 0x3c, 0x0e, 0x55, 0x92, 0x77, 0x32,
	0xc6, 0xce, 0x18, 0x36, 0xdf, 0xa9, 0x8c, 0xd8, 0xa4, 0xf0, 0x3b, 0x51, 0x4a, 0x02, 0x3e, 0x53,
};

static const UINT8 kof99_type0_t12[256] =
{
	0x1f, 0xac, 0x4d, 0xcd, 0xca, 0x70, 0x02, 0x6b, 0x18, 0x40, 0x62, 0xb2, 0x3f, 0x9b, 0x5b, 0xef,
	0x69, 0x68, 0x71, 0x3b, 0xcb, 0xd4, 0x30, 0xbc, 0x47, 0x72, 0x74, 0x5e, 0x84, 0x4c, 0x1b, 0xdb,
	0x6a, 0x35, 0x1d, 0xf5, 0xa1, 0xb3, 0x87, 0x5d, 0x57, 0x28, 0x2f, 0xc4, 0xfd, 0x24, 0x26, 0x36,
	0xad, 0xbe, 0x61, 0x63, 0x73, 0xaa, 0x82, 0xee, 0x29, 0xd0, 0xdf, 0x8c, 0x15, 0xb5, 0x96, 0xf3,
	0xdd, 0x7e, 0x3a, 0x37, 0x58, 0x7f, 0x0c, 0xfc, 0x0b, 0x07, 0xe8, 0xf7, 0xf4, 0x14, 0xb8, 0x81,
	0xb6, 0xd7, 0x1e, 0xc8, 0x85, 0xe6, 0x9d, 0x33, 0x60, 0xc5, 0x95, 0xd5, 0x55, 0x00, 0xa3, 0xb7,
	0x7d, 0x50, 0x0d, 0xd2, 0xc1, 0x12, 0xe5, 0xed, 0xd8, 0xa4, 0x9c, 0x8f, 0x2a, 0x4f, 0xa8, 0x01,
	0x52, 0x83, 0x65, 0xea, 0x9a, 0x6c, 0x44, 0x4a, 0xe2, 0xa5, 0x2b, 0x46, 0xe1, 0x34, 0x25, 0xf8,
	0xc3, 0xda, 0xc7, 0x6e, 0x48, 0x38, 0x7c, 0x78, 0x06, 0x53, 0x64, 0x16, 0x98, 0x3c, 0x91, 0x42,
	0x39, 0xcc, 0xb0, 0xf1, 0xeb, 0x13, 0xbb, 0x05, 0x32, 0x86, 0x0e, 0xa2, 0x0a, 0x9e, 0xfa, 0x66,
	0x54, 0x8e, 0xd3, 0xe7, 0x19, 0x20, 0x77, 0xec, 0xff, 0xbd, 0x6d, 0x43, 0x23, 0x03, 0xab, 0x75,
	0x3d, 0xcf, 0xd1, 0xde, 0x92, 0x31, 0xa7, 0x45, 0x4b, 0xc2, 0x97, 0xf9, 0x7a, 0x88, 0xd9, 0x1c,
	0xe9, 0xe4, 0x10, 0xc9, 0x22, 0x2d, 0x90, 0x76, 0x17, 0x79, 0x04, 0x51, 0x1a, 0x5a, 0x5f, 0x2c,
	0x21, 0x6f, 0x3e, 0xe0, 0xf0, 0xbf, 0xd6, 0x94, 0x0f, 0x80, 0x11, 0xa0, 0x5c, 0xa9, 0x49, 0x2e,
	0xce, 0xaf, 0xa6, 0x9f, 0x7b, 0x99, 0xb9, 0xb4, 0xe3, 0xfb, 0xf6, 0x27, 0xf2, 0x93, 0xfe, 0x08,
	0x67, 0xae, 0x09, 0x89, 0xdc, 0x4e, 0xc6, 0xc0, 0x8a, 0xb1, 0x59, 0x8b, 0x41, 0x56, 0x8d, 0xba,
};

static const UINT8 kof99_type1_t03[256] =
{
	0xa9, 0x17, 0xaf, 0x0d, 0x34, 0x6e, 0x53, 0xb6, 0x7f, 0x58, 0xe9, 0x14, 0x5f, 0x55, 0xdb, 0xd4,
	0x42, 0x80, 0x99, 0x59, 0xa8, 0x3a, 0x57, 0x5d, 0xd5, 0x6f, 0x4c, 0x68, 0x35, 0x46, 0xa6, 0xe7,
	0x7b, 0x71, 0xe0, 0x93, 0xa2, 0x1f, 0x64, 0x21, 0xe3, 0xb1, 0x98, 0x26, 0xab, 0xad, 0xee, 0xe5,
	0xbb, 0xd9, 0x1e, 0x2e, 0x95, 0x36, 0xef, 0x23, 0x79, 0x45, 0x04, 0xed, 0x13, 0x1d, 0xf4, 0x85,
	0x96, 0xec, 0xc2, 0x32, 0xaa, 0x7c, 0x15, 0xd8, 0xda, 0x92, 0x90, 0x9d, 0xb7, 0x56, 0x6a, 0x66,
	0x41, 0xfc, 0x00, 0xf6, 0x50, 0x24, 0xcf, 0xfb, 0x11, 0xfe, 0x82, 0x48, 0x9b, 0x27, 0x1b, 0x67,
	0x4e, 0x84, 0x69, 0x97, 0x6d, 0x8c, 0xd2, 0xba, 0x74, 0xf9, 0x8f, 0xa5, 0x54, 0x5c, 0xcd, 0x73,
	0x07, 0xd1, 0x01, 0x09, 0xf1, 0x19, 0x3b, 0x5e, 0x87, 0x30, 0x76, 0xcc, 0xc0, 0x5a, 0xa7, 0x49,
	0x22, 0xfa, 0x16, 0x02, 0xdf, 0xa4, 0xff, 0xb3, 0x75, 0x33, 0xbd, 0x88, 0x2f, 0xcb, 0x2a, 0x44,
	0xb8, 0xbf, 0x1c, 0x0f, 0x81, 0x10, 0x43, 0xb4, 0xc8, 0x7e, 0x9a, 0x25, 0xea, 0x83, 0x4b, 0x38,
	0x7a, 0xd7, 0x3d, 0x1a, 0x4f, 0x62, 0x51, 0xc9, 0x47, 0x0e, 0xce, 0x3f, 0xc7, 0x4d, 0x2c, 0xa1,
	0x86, 0xb9, 0xc5, 0xca, 0xdd, 0x6b, 0x70, 0x6c, 0x91, 0x9c, 0xbe, 0x0a, 0x9f, 0xf5, 0x94, 0xbc,
	0x18, 0x2b, 0x60, 0x20, 0x29, 0xf7, 0xf2, 0x28, 0xc4, 0xa0, 0x0b, 0x65, 0xde, 0x8d, 0x78, 0x12,
	0x3e, 0xd0, 0x77, 0x08, 0x8b, 0xae, 0x05, 0x31, 0x3c, 0xd6, 0xa3, 0x89, 0x06, 0xdc, 0x52, 0x72,
	0xb0, 0xb5, 0x37, 0xd3, 0xc3, 0x8a, 0xc6, 0xf0, 0xc1, 0x61, 0xfd, 0x4a, 0x5b, 0x7d, 0x9e, 0xf3,
	0x63, 0x40, 0x2d, 0xe8, 0xb2, 0xe6, 0x39, 0x03, 0xeb, 0x8e, 0xe1, 0x0c, 0xe4, 0xe2, 0xf8, 0xac,
};

static const UINT8 kof99_type1_t12[256] =
{
	0xea, 0xe6, 0x5e, 0xa7, 0x8e, 0xac, 0x34, 0x03, 0x30, 0x97, 0x52, 0x53, 0x76, 0xf2, 0x62, 0x0b,
	0x0a, 0xfc, 0x94, 0xb8, 0x67, 0x36, 0x11, 0xbc, 0xae, 0xca, 0xfa, 0x15, 0x04, 0x2b, 0x17, 0xc4,
	0x3e, 0x5b, 0x59, 0x01, 0x57, 0xe2, 0xba, 0xb7, 0xd1, 0x3f, 0xf0, 0x6a, 0x9c, 0x2a, 0xcb, 0xa9,
	0xe3, 0x2c, 0xc0, 0x0f, 0x46, 0x91, 0x8a, 0xd0, 0x98, 0xc5, 0xa6, 0x1b, 0x96, 0x29, 0x12, 0x09,
	0x63, 0xed, 0xe0, 0xa2, 0x86, 0x77, 0xbe, 0xe5, 0x65, 0xdb, 0xbd, 0x50, 0xb3, 0x9d, 0x1a, 0x4e,
	0x79, 0x0c, 0x00, 0x43, 0xdf, 0x3d, 0x54, 0x33, 0x8f, 0x89, 0xa8, 0x7b, 0xf9, 0xd5, 0x27, 0x82,
	0xbb, 0xc2, 0x8c, 0x47, 0x88, 0x6b, 0xb4, 0xc3, 0xf8, 0xaa, 0x06, 0x1e, 0x83, 0x7d, 0x05, 0x78,
	0x85, 0xf6, 0x6e, 0x2e, 0xec, 0x5a, 0x31, 0x45, 0x38, 0x14, 0x16, 0x8b, 0x02, 0xe4, 0x4f, 0xb0,
	0xbf, 0xab, 0xa4, 0x9e, 0x48, 0x60, 0x19, 0x35, 0x08, 0xde, 0xdd, 0x66, 0x90, 0x51, 0xcc, 0xa3,
	0xaf, 0x70, 0x9b, 0x75, 0x95, 0x49, 0x6c, 0x64, 0x72, 0x7e, 0x44, 0xa0, 0x73, 0x25, 0x68, 0x55,
	0x1f, 0x40, 0x7a, 0x74, 0x0e, 0x8d, 0xdc, 0x1c, 0x71, 0xc8, 0xcf, 0xd7, 0xe8, 0xce, 0xeb, 0x32,
	0x3a, 0xee, 0x07, 0x61, 0x4d, 0xfe, 0x5c, 0x7c, 0x56, 0x2f, 0x2d, 0x5f, 0x6f, 0x9f, 0x81, 0x22,
	0x58, 0x4b, 0xad, 0xda, 0xb9, 0x10, 0x18, 0x23, 0xe1, 0xf3, 0x6d, 0xe7, 0xe9, 0x28, 0xd6, 0xd8,
	0xf4, 0x4c, 0x39, 0x21, 0xb2, 0x84, 0xc1, 0x24, 0x26, 0xf1, 0x93, 0x37, 0xc6, 0x4a, 0xcd, 0x20,
	0xc9, 0xd9, 0xc7, 0xb1, 0xff, 0x99, 0xd4, 0x5d, 0xb5, 0xa1, 0x87, 0x0d, 0x69, 0x92, 0x13, 0x80,
	0xd2, 0xd3, 0xfd, 0x1d, 0xf5, 0x3b, 0xa5, 0x7f, 0xef, 0x9a, 0xb6, 0x42, 0xfb, 0x3c, 0xf7, 0x41,
};

static const UINT8 kof2000_type0_t03[256] =
{
	0x10, 0x61, 0xf1, 0x78, 0x85, 0x52, 0x68, 0xe3, 0x12, 0x0d, 0xfa, 0xf0, 0xc9, 0x36, 0x5e, 0x3d,
	0xf9, 0xa6, 0x01, 0x2e, 0xc7, 0x84, 0xea, 0x2b, 0x6d, 0x14, 0x38, 0x4f, 0x55, 0x1c, 0x9d, 0xa7,
	0x7a, 0xc6, 0xf8, 0x9a, 0xe6, 0x42, 0xb5, 0xed, 0x7d, 0x3a, 0xb1, 0x05, 0x43, 0x4a, 0x22, 0xfd,
	0xac, 0xa4, 0x31, 0xc3, 0x32, 0x76, 0x95, 0x9e, 0x7e, 0x88, 0x8e, 0xa2, 0x97, 0x18, 0xbe, 0x2a,
	0xf5, 0xd6, 0xca, 0xcc, 0x72, 0x3b, 0x87, 0x6c, 0xde, 0x75, 0xd7, 0x21, 0xcb, 0x0b, 0xdd, 0xe7,
	0xe1, 0x65, 0xaa, 0xb9, 0x44, 0xfb, 0x66, 0x15, 0x1a, 0x3c, 0x98, 0xcf, 0x8a, 0xdf, 0x37, 0xa5,
	0x2f, 0x67, 0xd2, 0x83, 0xb6, 0x6b, 0xfc, 0xe0, 0xb4, 0x7c, 0x08, 0xdc, 0x93, 0x30, 0xab, 0xe4,
	0x19, 0xc2, 0x8b, 0xeb, 0xa0, 0x0a, 0xc8, 0x03, 0xc0, 0x4b, 0x64, 0x71, 0x86, 0x9c, 0x9b, 0x16,
	0x79, 0xff, 0x70, 0x09, 0x8c, 0xd0, 0xf6, 0x53, 0x07, 0x73, 0xd4, 0x89, 0xb3, 0x00, 0xe9, 0xfe,
	0xec, 0x8f, 0xbc, 0xb2, 0x1e, 0x5d, 0x11, 0x35, 0xa9, 0x06, 0x59, 0x9f, 0xc1, 0xd3, 0x7b, 0xf2,
	0xc5, 0x77, 0x4e, 0x39, 0x20, 0xd5, 0x6a, 0x82, 0xda, 0x45, 0xf3, 0x33, 0x81, 0x23, 0xba, 0xe2,
	0x1d, 0x5f, 0x5c, 0x51, 0x49, 0xae, 0x8d, 0xc4, 0xa8, 0xf7, 0x1f, 0x0f, 0x34, 0x28, 0xa1, 0xd9,
	0x27, 0xd8, 0x4c, 0x2c, 0xbf, 0x91, 0x3e, 0x69, 0x57, 0x41, 0x25, 0x0c, 0x5a, 0x90, 0x92, 0xb0,
	0x63, 0x6f, 0x40, 0xaf, 0x74, 0xb8, 0x2d, 0x80, 0xbb, 0x46, 0x94, 0xe5, 0x29, 0xee, 0xb7, 0x1b,
	0x96, 0xad, 0x13, 0x0e, 0x58, 0x99, 0x60, 0x4d, 0x17, 0x26, 0xce, 0xe8, 0xdb, 0xef, 0x24, 0xa3,
	0x6e, 0x7f, 0x54, 0x3f, 0x02, 0xd1, 0x5b, 0x50, 0x56, 0x48, 0xf4, 0xbd, 0x62, 0x47, 0x04, 0xcd,
};

static const UINT8 kof2000_type0_t12[256] =
{
	0xf4, 0x28, 0xb4, 0x8f, 0xfa, 0xeb, 0x8e, 0x54, 0x2b, 0x49, 0xd1, 0x76, 0x71, 0x47, 0x8b, 0x57,
	0x92, 0x85, 0x7c, 0xb8, 0x5c, 0x22, 0xf9, 0x26, 0xbc, 0x5b, 0x6d, 0x67, 0xae, 0x5f, 0x6f, 0xf5,
	0x9f, 0x48, 0x66, 0x40, 0x0d, 0x11, 0x4e, 0xb2, 0x6b, 0x35, 0x15, 0x0f, 0x18, 0x25, 0x1d, 0xba,
	0xd3, 0x69, 0x79, 0xec, 0xa8, 0x8c, 0xc9, 0x7f, 0x4b, 0xdb, 0x51, 0xaf, 0xca, 0xe2, 0xb3, 0x81,
	0x12, 0x5e, 0x7e, 0x38, 0xc8, 0x95, 0x01, 0xff, 0xfd, 0xfb, 0xf2, 0x74, 0x62, 0x14, 0xa5, 0x98,
	0xa6, 0xda, 0x80, 0x53, 0xe8, 0x56, 0xac, 0x1b, 0x52, 0xd0, 0xf1, 0x45, 0x42, 0xb6, 0x1a, 0x4a,
	0x3a, 0x99, 0xfc, 0xd2, 0x9c, 0xcf, 0x31, 0x2d, 0xdd, 0x86, 0x2f, 0x29, 0xe1, 0x03, 0x19, 0xa2,
	0x41, 0x33, 0x83, 0x90, 0xc1, 0xbf, 0x0b, 0x08, 0x3d, 0xd8, 0x8d, 0x6c, 0x39, 0xa0, 0xe3, 0x55,
	0x02, 0x50, 0x46, 0xe6, 0xc3, 0x82, 0x36, 0x13, 0x75, 0xab, 0x27, 0xd7, 0x1f, 0x0a, 0xd4, 0x89,
	0x59, 0x4f, 0xc0, 0x5d, 0xc6, 0xf7, 0x88, 0xbd, 0x3c, 0x00, 0xef, 0xcd, 0x05, 0x1c, 0xaa, 0x9b,
	0xed, 0x7a, 0x61, 0x17, 0x93, 0xfe, 0x23, 0xb9, 0xf3, 0x68, 0x78, 0xf6, 0x5a, 0x7b, 0xe0, 0xe4,
	0xa3, 0xee, 0x16, 0x72, 0xc7, 0x3b, 0x8a, 0x37, 0x2a, 0x70, 0xa9, 0x2c, 0x21, 0xf8, 0x24, 0x09,
	0xce, 0x20, 0x9e, 0x06, 0x87, 0xc5, 0x04, 0x64, 0x43, 0x7d, 0x4d, 0x10, 0xd6, 0xa4, 0x94, 0x4c,
	0x60, 0xde, 0xdf, 0x58, 0xb1, 0x44, 0x3f, 0xb0, 0xd9, 0xe5, 0xcb, 0xbb, 0xbe, 0xea, 0x07, 0x34,
	0x73, 0x6a, 0x77, 0xf0, 0x9d, 0x0c, 0x2e, 0x0e, 0x91, 0x9a, 0xcc, 0xc2, 0xb7, 0x63, 0x97, 0xd5,
	0xdc, 0xc4, 0x32, 0xe7, 0x84, 0x3e, 0x30, 0xa1, 0x1e, 0xb5, 0x6e, 0x65, 0xe9, 0xad, 0xa7, 0x96,
};

static const UINT8 kof2000_type1_t03[256] =
{
	0x9a, 0x2f, 0xcc, 0x4e, 0x40, 0x69, 0xac, 0xca, 0xa5, 0x7b, 0x0a, 0x61, 0x91, 0x0d, 0x55, 0x74,
	0xcd, 0x8b, 0x0b, 0x80, 0x09, 0x5e, 0x38, 0xc7, 0xda, 0xbf, 0xf5, 0x37, 0x23, 0x31, 0x33, 0xe9,
	0xae, 0x87, 0xe5, 0xfa, 0x6e, 0x5c, 0xad, 0xf4, 0x76, 0x62, 0x9f, 0x2e, 0x01, 0xe2, 0xf6, 0x47,
	0x8c, 0x7c, 0xaa, 0x98, 0xb5, 0x92, 0x51, 0xec, 0x5f, 0x07, 0x5d, 0x6f, 0x16, 0xa1, 0x1d, 0xa9,
	0x48, 0x45, 0xf0, 0x6a, 0x9c, 0x1e, 0x11, 0xa0, 0x06, 0x46, 0xd5, 0xf1, 0x73, 0xed, 0x94, 0xf7,
	0xc3, 0x57, 0x1b, 0xe0, 0x97, 0xb1, 0xa4, 0xa7, 0x24, 0xe7, 0x2b, 0x05, 0x5b, 0x34, 0x0c, 0xb8,
	0x0f, 0x9b, 0xc8, 0x4d, 0x5a, 0xa6, 0x86, 0x3e, 0x14, 0x29, 0x84, 0x58, 0x90, 0xdb, 0x2d, 0x54,
	0x9d, 0x82, 0xd4, 0x7d, 0xc6, 0x67, 0x41, 0x89, 0xc1, 0x13, 0xb0, 0x9e, 0x81, 0x6d, 0xa8, 0x59,
	0xbd, 0x39, 0x8e, 0xe6, 0x25, 0x8f, 0xd9, 0xa2, 0xe4, 0x53, 0xc5, 0x72, 0x7e, 0x36, 0x4a, 0x4f,
	0x52, 0xc2, 0x22, 0x2a, 0xce, 0x3c, 0x21, 0x2c, 0x00, 0xd7, 0x75, 0x8a, 0x27, 0xee, 0x43, 0xfe,
	0xcb, 0x6b, 0xb9, 0xa3, 0x78, 0xb7, 0x85, 0x02, 0x20, 0xd0, 0x83, 0xc4, 0x12, 0xf9, 0xfd, 0xd8,
	0x79, 0x64, 0x3a, 0x49, 0x03, 0xb4, 0xc0, 0xf2, 0xdf, 0x15, 0x93, 0x08, 0x35, 0xff, 0x70, 0xdd,
	0x28, 0x6c, 0x0e, 0x04, 0xde, 0x7a, 0x65, 0xd2, 0xab, 0x42, 0x95, 0xe1, 0x3f, 0x3b, 0x7f, 0x66,
	0xd1, 0x8d, 0xe3, 0xbb, 0x1c, 0xfc, 0x77, 0x1a, 0x88, 0x18, 0x19, 0x68, 0x1f, 0x56, 0xd6, 0xe8,
	0xb6, 0xbc, 0xd3, 0xea, 0x3d, 0x26, 0xb3, 0xc9, 0x44, 0xdc, 0xf3, 0x32, 0x30, 0xef, 0x96, 0x4c,
	0xaf, 0x17, 0xf8, 0xfb, 0x60, 0x50, 0xeb, 0x4b, 0x99, 0x63, 0xba, 0xb2, 0x71, 0xcf, 0x10, 0xbe,
};

static const UINT8 kof2000_type1_t12[256] =
{
	0xda, 0xa7, 0xd6, 0x6e, 0x2f, 0x5e, 0xf0, 0x3f, 0xa4, 0xce, 0xd3, 0xfd, 0x46, 0x2a, 0xac, 0xc9,
	0xbe, 0xeb, 0x9f, 0xd5, 0x3c, 0x61, 0x96, 0x11, 0xd0, 0x38, 0xca, 0x06, 0xed, 0x1b, 0x65, 0xe7,
	0x23, 0xdd, 0xd9, 0x05, 0xbf, 0x5b, 0x5d, 0xa5, 0x95, 0x00, 0xec, 0xf1, 0x01, 0xa9, 0xa6, 0xfc,
	0xbb, 0x54, 0xe3, 0x2e, 0x92, 0x58, 0x0a, 0x7b, 0xb6, 0xcc, 0xb1, 0x5f, 0x14, 0x35, 0x72, 0xff,
	0xe6, 0x52, 0xd7, 0x8c, 0xf3, 0x43, 0xaf, 0x9c, 0xc0, 0x4f, 0x0c, 0x42, 0x8e, 0xef, 0x80, 0xcd,
	0x1d, 0x7e, 0x88, 0x3b, 0x98, 0xa1, 0xad, 0xe4, 0x9d, 0x8d, 0x2b, 0x56, 0xb5, 0x50, 0xdf, 0x66,
	0x6d, 0xd4, 0x60, 0x09, 0xe1, 0xee, 0x4a, 0x47, 0xf9, 0xfe, 0x73, 0x07, 0x89, 0xa8, 0x39, 0xea,
	0x82, 0x9e, 0xcf, 0x26, 0xb2, 0x4e, 0xc3, 0x59, 0xf2, 0x3d, 0x9a, 0xb0, 0x69, 0xf7, 0xbc, 0x34,
	0xe5, 0x36, 0x22, 0xfb, 0x57, 0x71, 0x99, 0x6c, 0x83, 0x30, 0x55, 0xc2, 0xbd, 0xf4, 0x77, 0xe9,
	0x76, 0x97, 0xa0, 0xe0, 0xb9, 0x86, 0x6b, 0xa3, 0x84, 0x67, 0x1a, 0x70, 0x02, 0x5a, 0x41, 0x5c,
	0x25, 0x81, 0xaa, 0x28, 0x78, 0x4b, 0xc6, 0x64, 0x53, 0x16, 0x4d, 0x8b, 0x20, 0x93, 0xae, 0x0f,
	0x94, 0x2c, 0x3a, 0xc7, 0x62, 0xe8, 0xc4, 0xdb, 0x04, 0xc5, 0xfa, 0x29, 0x48, 0xd1, 0x08, 0x24,
	0x0d, 0xe2, 0xd8, 0x10, 0xb4, 0x91, 0x8a, 0x13, 0x0e, 0xdc, 0xd2, 0x79, 0xb8, 0xf8, 0xba, 0x2d,
	0xcb, 0xf5, 0x7d, 0x37, 0x51, 0x40, 0x31, 0xa2, 0x0b, 0x18, 0x63, 0x7f, 0xb3, 0xab, 0x9b, 0x87,
	0xf6, 0x90, 0xde, 0xc8, 0x27, 0x45, 0x7c, 0x1c, 0x85, 0x68, 0x33, 0x19, 0x03, 0x75, 0x15, 0x7a,
	0x1f, 0x49, 0x8f, 0x4c, 0xc1, 0x44, 0x17, 0x12, 0x6f, 0x32, 0xb7, 0x3e, 0x74, 0x1e, 0x21, 0x6a,
};

static const UINT8 kof2000_address_8_15_xor1[256] =
{
	0xfc, 0x9b, 0x1c, 0x35, 0x72, 0x53, 0xd6, 0x7d, 0x84, 0xa4, 0xc5, 0x93, 0x7b, 0xe7, 0x47, 0xd5,
	0x24, 0xa2, 0xfa, 0x19, 0x0c, 0xb1, 0x8c, 0xb9, 0x9d, 0xd8, 0x59, 0x4f, 0x3c, 0xb2, 0x78, 0x4a,
	0x2a, 0x96, 0x9a, 0xf1, 0x1f, 0x22, 0xa8, 0x5b, 0x67, 0xa3, 0x0f, 0x00, 0xfb, 0xdf, 0xeb, 0x0a,
	0x57, 0xb8, 0x25, 0xd7, 0xf0, 0x6b, 0x0b, 0x31, 0x95, 0x23, 0x2d, 0x5c, 0x27, 0xc7, 0xf4, 0x55,
	0x1a, 0xf7, 0x74, 0xbe, 0xd3, 0xac, 0x3d, 0xc1, 0x7f, 0xbd, 0x28, 0x01, 0x10, 0xe5, 0x09, 0x37,
	0x1e, 0x58, 0xaf, 0x17, 0xf2, 0x16, 0x30, 0x92, 0x36, 0x68, 0xe6, 0xd4, 0xea, 0xb7, 0x75, 0x54,
	0x77, 0x41, 0xb4, 0x8d, 0xe0, 0xf3, 0x51, 0x03, 0xa9, 0xe8, 0x66, 0xab, 0x29, 0xa5, 0xed, 0xcb,
	0xd1, 0xaa, 0xf5, 0xdb, 0x4c, 0x42, 0x97, 0x8a, 0xae, 0xc9, 0x6e, 0x04, 0x33, 0x85, 0xdd, 0x2b,
	0x6f, 0xef, 0x12, 0x21, 0x7a, 0xa1, 0x5a, 0x91, 0xc8, 0xcc, 0xc0, 0xa7, 0x60, 0x3e, 0x56, 0x2f,
	0xe4, 0x71, 0x99, 0xc2, 0xa0, 0x45, 0x80, 0x65, 0xbb, 0x87, 0x69, 0x81, 0x73, 0xca, 0xf6, 0x46,
	0x43, 0xda, 0x26, 0x7e, 0x8f, 0xe1, 0x8b, 0xfd, 0x50, 0x79, 0xba, 0xc6, 0x63, 0x4b, 0xb3, 0x8e,
	0x34, 0xe2, 0x48, 0x14, 0xcd, 0xe3, 0xc4, 0x05, 0x13, 0x40, 0x06, 0x6c, 0x88, 0xb0, 0xe9, 0x1b,
	0x4d, 0xf8, 0x76, 0x02, 0x44, 0x94, 0xcf, 0x32, 0xfe, 0xce, 0x3b, 0x5d, 0x2c, 0x89, 0x5f, 0xdc,
	0xd2, 0x9c, 0x6a, 0xec, 0x18, 0x6d, 0x0e, 0x86, 0xff, 0x5e, 0x9e, 0xee, 0x11, 0xd0, 0x49, 0x52,
	0x4e, 0x61, 0x90, 0x0d, 0xc3, 0x39, 0x15, 0x83, 0xb5, 0x62, 0x3f, 0x70, 0x7c, 0xad, 0x20, 0xbf,
	0x2e, 0x08, 0x1d, 0xf9, 0xb6, 0xa6, 0x64, 0x07, 0x82, 0x38, 0x98, 0x3a, 0x9f, 0xde, 0xbc, 0xd9,
};

static const UINT8 kof2000_address_8_15_xor2[256] =
{
	0x00, 0xbe, 0x06, 0x5a, 0xfa, 0x42, 0x15, 0xf2, 0x3f, 0x0a, 0x84, 0x93, 0x4e, 0x78, 0x3b, 0x89,
	0x32, 0x98, 0xa2, 0x87, 0x73, 0xdd, 0x26, 0xe5, 0x05, 0x71, 0x08, 0x6e, 0x9b, 0xe0, 0xdf, 0x9e,
	0xfc, 0x83, 0x81, 0xef, 0xb2, 0xc0, 0xc3, 0xbf, 0xa7, 0x6d, 0x1b, 0x95, 0xed, 0xb9, 0x3e, 0x13,
	0xb0, 0x47, 0x9c, 0x7a, 0x24, 0x41, 0x68, 0xd0, 0x36, 0x0b, 0xb5, 0xc2, 0x67, 0xf7, 0x54, 0x92,
	0x1e, 0x44, 0x86, 0x2b, 0x94, 0xcc, 0xba, 0x23, 0x0d, 0xca, 0x6b, 0x4c, 0x2a, 0x9a, 0x2d, 0x8b,
	0xe3, 0x52, 0x29, 0xf0, 0x21, 0xbd, 0xbb, 0x1f, 0xa3, 0xab, 0xf8, 0x46, 0xb7, 0x45, 0x82, 0x5e,
	0xdb, 0x07, 0x5d, 0xe9, 0x9d, 0x1a, 0x48, 0xce, 0x91, 0x12, 0xd4, 0xee, 0xa9, 0x39, 0xf1, 0x18,
	0x2c, 0x22, 0x8a, 0x7e, 0x34, 0x4a, 0x8c, 0xc1, 0x14, 0xf3, 0x20, 0x35, 0xd9, 0x96, 0x33, 0x77,
	0x9f, 0x76, 0x7c, 0x90, 0xc6, 0xd5, 0xa1, 0x5b, 0xac, 0x75, 0xc7, 0x0c, 0xb3, 0x17, 0xd6, 0x99,
	0x56, 0xa6, 0x3d, 0x1d, 0xb1, 0x2e, 0xd8, 0xbc, 0x2f, 0xde, 0x60, 0x55, 0x6c, 0x40, 0xcd, 0x43,
	0xff, 0xad, 0x38, 0x79, 0x51, 0xc8, 0x0e, 0x5f, 0xc4, 0x66, 0xcb, 0xa8, 0x7d, 0xa4, 0x3a, 0xea,
	0x27, 0x7b, 0x70, 0x8e, 0x5c, 0x19, 0x0f, 0x80, 0x6f, 0x8f, 0x10, 0xf9, 0x49, 0x85, 0x69, 0x7f,
	0xeb, 0x1c, 0x01, 0x65, 0x37, 0xa5, 0x28, 0xe4, 0x6a, 0x03, 0x04, 0xd1, 0x31, 0x11, 0x30, 0xfb,
	0x88, 0x97, 0xd3, 0xf6, 0xc5, 0x4d, 0xf5, 0x3c, 0xe8, 0x61, 0xdc, 0xd2, 0xb4, 0xb8, 0xa0, 0xae,
	0x16, 0x25, 0x02, 0x09, 0xfe, 0xcf, 0x53, 0x63, 0xaf, 0x59, 0xf4, 0xe1, 0xec, 0xd7, 0xe7, 0x50,
	0xe2, 0xc9, 0xaa, 0x4b, 0x8d, 0x4f, 0xe6, 0x64, 0xda, 0x74, 0xb6, 0x72, 0x57, 0x62, 0xfd, 0x58,
};

static const UINT8 kof2000_address_16_23_xor1[256] =
{
	0x45, 0x9f, 0x6e, 0x2f, 0x28, 0xbc, 0x5e, 0x6d, 0xda, 0xb5, 0x0d, 0xb8, 0xc0, 0x8e, 0xa2, 0x32,
	0xee, 0xcd, 0x8d, 0x48, 0x8c, 0x27, 0x14, 0xeb, 0x65, 0xd7, 0xf2, 0x93, 0x99, 0x90, 0x91, 0xfc,
	0x5f, 0xcb, 0xfa, 0x75, 0x3f, 0x26, 0xde, 0x72, 0x33, 0x39, 0xc7, 0x1f, 0x88, 0x79, 0x73, 0xab,
	0x4e, 0x36, 0x5d, 0x44, 0xd2, 0x41, 0xa0, 0x7e, 0xa7, 0x8b, 0xa6, 0xbf, 0x03, 0xd8, 0x86, 0xdc,
	0x2c, 0xaa, 0x70, 0x3d, 0x46, 0x07, 0x80, 0x58, 0x0b, 0x2b, 0xe2, 0xf0, 0xb1, 0xfe, 0x42, 0xf3,
	0xe9, 0xa3, 0x85, 0x78, 0xc3, 0xd0, 0x5a, 0xdb, 0x1a, 0xfb, 0x9d, 0x8a, 0xa5, 0x12, 0x0e, 0x54,
	0x8f, 0xc5, 0x6c, 0xae, 0x25, 0x5b, 0x4b, 0x17, 0x02, 0x9c, 0x4a, 0x24, 0x40, 0xe5, 0x9e, 0x22,
	0xc6, 0x49, 0x62, 0xb6, 0x6b, 0xbb, 0xa8, 0xcc, 0xe8, 0x81, 0x50, 0x47, 0xc8, 0xbe, 0x5c, 0xa4,
	0xd6, 0x94, 0x4f, 0x7b, 0x9a, 0xcf, 0xe4, 0x59, 0x7a, 0xa1, 0xea, 0x31, 0x37, 0x13, 0x2d, 0xaf,
	0x21, 0x69, 0x19, 0x1d, 0x6f, 0x16, 0x98, 0x1e, 0x08, 0xe3, 0xb2, 0x4d, 0x9b, 0x7f, 0xa9, 0x77,
	0xed, 0xbd, 0xd4, 0xd9, 0x34, 0xd3, 0xca, 0x09, 0x18, 0x60, 0xc9, 0x6a, 0x01, 0xf4, 0xf6, 0x64,
	0xb4, 0x3a, 0x15, 0xac, 0x89, 0x52, 0x68, 0x71, 0xe7, 0x82, 0xc1, 0x0c, 0x92, 0xf7, 0x30, 0xe6,
	0x1c, 0x3e, 0x0f, 0x0a, 0x67, 0x35, 0xba, 0x61, 0xdd, 0x29, 0xc2, 0xf8, 0x97, 0x95, 0xb7, 0x3b,
	0xe0, 0xce, 0xf9, 0xd5, 0x06, 0x76, 0xb3, 0x05, 0x4c, 0x04, 0x84, 0x3c, 0x87, 0x23, 0x63, 0x7c,
	0x53, 0x56, 0xe1, 0x7d, 0x96, 0x1b, 0xd1, 0xec, 0x2a, 0x66, 0xf1, 0x11, 0x10, 0xff, 0x43, 0x2e,
	0xdf, 0x83, 0x74, 0xf5, 0x38, 0x20, 0xfd, 0xad, 0xc4, 0xb9, 0x55, 0x51, 0xb0, 0xef, 0x00, 0x57,
};

static const UINT8 kof2000_address_16_23_xor2[256] =
{
	0x00, 0xb8, 0xf0, 0x34, 0xca, 0x21, 0x3c, 0xf9, 0x01, 0x8e, 0x75, 0x70, 0xec, 0x13, 0x27, 0x96,
	0xf4, 0x5b, 0x88, 0x1f, 0xeb, 0x4a, 0x7d, 0x9d, 0xbe, 0x02, 0x14, 0xaf, 0xa2, 0x06, 0xc6, 0xdb,
	0x35, 0x6b, 0x74, 0x45, 0x7b, 0x29, 0xd2, 0xfe, 0xb6, 0x15, 0xd0, 0x8a, 0xa9, 0x2d, 0x19, 0xf6,
	0x5e, 0x5a, 0x90, 0xe9, 0x11, 0x33, 0xc2, 0x47, 0x37, 0x4c, 0x4f, 0x59, 0xc3, 0x04, 0x57, 0x1d,
	0xf2, 0x63, 0x6d, 0x6e, 0x31, 0x95, 0xcb, 0x3e, 0x67, 0xb2, 0xe3, 0x98, 0xed, 0x8d, 0xe6, 0xfb,
	0xf8, 0xba, 0x5d, 0xd4, 0x2a, 0xf5, 0x3b, 0x82, 0x05, 0x16, 0x44, 0xef, 0x4d, 0xe7, 0x93, 0xda,
	0x9f, 0xbb, 0x61, 0xc9, 0x53, 0xbd, 0x76, 0x78, 0x52, 0x36, 0x0c, 0x66, 0xc1, 0x10, 0xdd, 0x7a,
	0x84, 0x69, 0xcd, 0xfd, 0x58, 0x0d, 0x6c, 0x89, 0x68, 0xad, 0x3a, 0xb0, 0x4b, 0x46, 0xc5, 0x03,
	0xb4, 0xf7, 0x30, 0x8c, 0x4e, 0x60, 0x73, 0xa1, 0x8b, 0xb1, 0x62, 0xcc, 0xd1, 0x08, 0xfc, 0x77,
	0x7e, 0xcf, 0x56, 0x51, 0x07, 0xa6, 0x80, 0x92, 0xdc, 0x0b, 0xa4, 0xc7, 0xe8, 0xe1, 0xb5, 0x71,
	0xea, 0xb3, 0x2f, 0x94, 0x18, 0xe2, 0x3d, 0x49, 0x65, 0xaa, 0xf1, 0x91, 0xc8, 0x99, 0x55, 0x79,
	0x86, 0xa7, 0x26, 0xa0, 0xac, 0x5f, 0xce, 0x6a, 0x5c, 0xf3, 0x87, 0x8f, 0x12, 0x1c, 0xd8, 0xe4,
	0x9b, 0x64, 0x2e, 0x1e, 0xd7, 0xc0, 0x17, 0xbc, 0xa3, 0xa8, 0x9a, 0x0e, 0x25, 0x40, 0x41, 0x50,
	0xb9, 0xbf, 0x28, 0xdf, 0x32, 0x54, 0x9e, 0x48, 0xd5, 0x2b, 0x42, 0xfa, 0x9c, 0x7f, 0xd3, 0x85,
	0x43, 0xde, 0x81, 0x0f, 0x24, 0xc4, 0x38, 0xae, 0x83, 0x1b, 0x6f, 0x7c, 0xe5, 0xff, 0x1a, 0xd9,
	0x3f, 0xb7, 0x22, 0x97, 0x09, 0xe0, 0xa5, 0x20, 0x23, 0x2c, 0x72, 0xd6, 0x39, 0xab, 0x0a, 0xee,
};

static const UINT8 kof2000_address_0_7_xor[256] =
{
	0x26, 0x48, 0x06, 0x9b, 0x21, 0xa9, 0x1b, 0x76, 0xc9, 0xf8, 0xb4, 0x67, 0xe4, 0xff, 0x99, 0xf7,
	0x15, 0x9e, 0x62, 0x00, 0x72, 0x4d, 0xa0, 0x4f, 0x02, 0xf1, 0xea, 0xef, 0x0b, 0xf3, 0xeb, 0xa6,
	0x93, 0x78, 0x6f, 0x7c, 0xda, 0xd4, 0x7b, 0x05, 0xe9, 0xc6, 0xd6, 0xdb, 0x50, 0xce, 0xd2, 0x01,
	0xb5, 0xe8, 0xe0, 0x2a, 0x08, 0x1a, 0xb8, 0xe3, 0xf9, 0xb1, 0xf4, 0x8b, 0x39, 0x2d, 0x85, 0x9c,
	0x55, 0x73, 0x63, 0x40, 0x38, 0x96, 0xdc, 0xa3, 0xa2, 0xa1, 0x25, 0x66, 0x6d, 0x56, 0x8e, 0x10,
	0x0f, 0x31, 0x1c, 0xf5, 0x28, 0x77, 0x0a, 0xd1, 0x75, 0x34, 0xa4, 0xfe, 0x7d, 0x07, 0x51, 0x79,
	0x41, 0x90, 0x22, 0x35, 0x12, 0xbb, 0xc4, 0xca, 0xb2, 0x1f, 0xcb, 0xc8, 0xac, 0xdd, 0xd0, 0x0d,
	0xfc, 0xc5, 0x9d, 0x14, 0xbc, 0x83, 0xd9, 0x58, 0xc2, 0x30, 0x9a, 0x6a, 0xc0, 0x0c, 0xad, 0xf6,
	0x5d, 0x74, 0x7f, 0x2f, 0xbd, 0x1d, 0x47, 0xd5, 0xe6, 0x89, 0xcf, 0xb7, 0xd3, 0x59, 0x36, 0x98,
	0xf0, 0xfb, 0x3c, 0xf2, 0x3f, 0xa7, 0x18, 0x82, 0x42, 0x5c, 0xab, 0xba, 0xde, 0x52, 0x09, 0x91,
	0xaa, 0x61, 0xec, 0xd7, 0x95, 0x23, 0xcd, 0x80, 0xa5, 0x68, 0x60, 0x27, 0x71, 0xe1, 0x2c, 0x2e,
	0x8d, 0x2b, 0x57, 0x65, 0xbf, 0xc1, 0x19, 0xc7, 0x49, 0x64, 0x88, 0x4a, 0xcc, 0x20, 0x4e, 0xd8,
	0x3b, 0x4c, 0x13, 0x5f, 0x9f, 0xbe, 0x5e, 0x6e, 0xfd, 0xe2, 0xfa, 0x54, 0x37, 0x0e, 0x16, 0x7a,
	0x6c, 0x33, 0xb3, 0x70, 0x84, 0x7e, 0xc3, 0x04, 0xb0, 0xae, 0xb9, 0x81, 0x03, 0x29, 0xdf, 0x46,
	0xe5, 0x69, 0xe7, 0x24, 0x92, 0x5a, 0x4b, 0x5b, 0x94, 0x11, 0x3a, 0x3d, 0x87, 0xed, 0x97, 0xb6,
	0x32, 0x3e, 0x45, 0xaf, 0x1e, 0x43, 0x44, 0x8c, 0x53, 0x86, 0x6b, 0xee, 0xa8, 0x8a, 0x8f, 0x17,
};

/* underlined values are wrong (not enough evidence, FF fill in kof99 and garou) */
/* they correspond to tiles 7d000-7efff */
static const UINT8 kof99_address_8_15_xor1[256] =
{
	0x00, 0xb1, 0x1e, 0xc5, 0x3d, 0x40, 0x45, 0x5e, 0xf2, 0xf8, 0x04, 0x63, 0x36, 0x87, 0x88, 0xbf,
	0xab, 0xcc, 0x78, 0x08, 0xdd, 0x20, 0xd4, 0x35, 0x09, 0x8e, 0x44, 0xae, 0x33, 0xa9, 0x9e, 0xcd,
	0xb3, 0xe5, 0xad, 0x41, 0xda, 0xbe, 0xf4, 0x16, 0x57, 0x2e, 0x53, 0x67, 0xaf, 0xdb, 0x8a, 0xd8,
	0x34, 0x17, 0x3c, 0x01, 0x55, 0x73, 0xcf, 0xe3, 0xe8, 0xc7, 0x0d, 0xe9, 0xa3, 0x13, 0x0c, 0xf6,
	0x90, 0x4e, 0xfb, 0x97, 0x6d, 0x5f, 0xa8, 0x71, 0x11, 0xfc, 0xd1, 0x95, 0x81, 0xba, 0x8c, 0x1b,
	0x39, 0xfe, 0xa2, 0x15, 0xa6, 0x52, 0x4d, 0x5b, 0x59, 0xa5, 0xe0, 0x96, 0xd9, 0x8f, 0x7b, 0xed,
	0x29, 0xd3, 0x1f, 0x0e, 0xec, 0x23, 0x0f, 0xb8, 0x6c, 0x6f, 0x7d, 0x18, 0x46, 0xd6, 0xe4, 0xb5,
	0x9a, 0x79, 0x02, 0xf5, 0x03, 0xc0, 0x60, 0x66, 0x5c, 0x2f, 0x76, 0x85, 0x9d, 0x54, 0x1a, 0x6a,
	0x28, 0xce, 0x7f, 0x7c, 0x91, 0x99, 0x4c, 0x83, 0x3e, 0xb4, 0x1d, 0x05, 0xc1, 0xc3, 0xd7, 0x47,
	0xde, 0xbc, 0x62, 0x6e, 0x86, 0x14, 0x80, 0x77, 0xeb, 0xf3, 0x07, 0x31, 0x56, 0xd2, 0xc2, 0xc6,
	0x6b, 0xdc, 0xfd, 0x22, 0x92, 0xf0, 0x06, 0x51, 0x2d, 0x38, 0xe6, 0xa0, 0x25, 0xdf, 0xd5, 0x2c,
	0x1c, 0x94, 0x12, 0x9c, 0xb0, 0x9b, 0xc4, 0x0b, 0xc8, 0xd0, 0xf7, 0x30, 0xcb, 0x27, 0xfa, 0x7a,
	0x10, 0x61, 0xaa, 0xa4, 0x70, 0xb7, 0x2a, 0x5a, 0xc9, 0xf1, 0x0a, 0x49, 0x65, 0xee, 0x69, 0x4b,
	0x3a, 0x8d, 0x32, 0x5d, 0x68, 0xb9, 0x9f, 0x75, 0x19, 0x3f, 0xac, 0x37, 0x4f, 0xe7, 0x93, 0x89,
	0x7e, 0x4a, 0x3b, 0xea, 0x74, 0x72, 0x43, 0xbd, 0x24, 0xef, 0xb6, 0xff, 0x64, 0x58, 0x84, 0x8b,
	0xa7, 0xbb, 0xb2, 0xe1, 0x26, 0x2b, 0x50, 0xca, 0x21, 0xf9, 0x98, 0xa1, 0xe2, 0x42, 0x82, 0x48,
//                                                              ^^^^  ^^^^  ^^^^  ^^^^
};

static const UINT8 kof99_address_8_15_xor2[256] =
{
	0x9b, 0x9d, 0xc1, 0x3d, 0xa9, 0xb8, 0xf4, 0x6f, 0xf6, 0x25, 0xc7, 0x47, 0xd5, 0x97, 0xdf, 0x6b,
	0xeb, 0x90, 0xa4, 0xb2, 0x5d, 0xf5, 0x66, 0xb0, 0xb9, 0x8b, 0x93, 0x64, 0xec, 0x7b, 0x65, 0x8c,
	0xf1, 0x43, 0x42, 0x6e, 0x45, 0x9f, 0xb3, 0x35, 0x06, 0x71, 0x96, 0xdb, 0xa0, 0xfb, 0x0b, 0x3a,
	0x1f, 0xf8, 0x8e, 0x69, 0xcd, 0x26, 0xab, 0x86, 0xa2, 0x0c, 0xbd, 0x63, 0xa5, 0x7a, 0xe7, 0x6a,
	0x5f, 0x18, 0x9e, 0xbf, 0xad, 0x55, 0xb1, 0x1c, 0x5c, 0x03, 0x30, 0xc6, 0x37, 0x20, 0xe3, 0xc9,
	0x52, 0xe8, 0xee, 0x4f, 0x01, 0x70, 0xc4, 0x77, 0x29, 0x2a, 0xba, 0x53, 0x12, 0x04, 0x7d, 0xaf,
	0x33, 0x8f, 0xa8, 0x4d, 0xaa, 0x5b, 0xb4, 0x0f, 0x92, 0xbb, 0xed, 0xe1, 0x2f, 0x50, 0x6c, 0xd2,
	0x2c, 0x95, 0xd9, 0xf9, 0x98, 0xc3, 0x76, 0x4c, 0xf2, 0xe4, 0xe5, 0x2b, 0xef, 0x9c, 0x49, 0xb6,
	0x31, 0x3b, 0xbc, 0xa1, 0xca, 0xde, 0x62, 0x74, 0xea, 0x81, 0x00, 0xdd, 0xa6, 0x46, 0x88, 0x3f,
	0x39, 0xd6, 0x23, 0x54, 0x24, 0x4a, 0xd8, 0xdc, 0xd7, 0xd1, 0xcc, 0xbe, 0x57, 0x7c, 0xda, 0x44,
	0x61, 0xce, 0xd3, 0xd4, 0xe9, 0x28, 0x80, 0xe0, 0x56, 0x8a, 0x09, 0x05, 0x9a, 0x89, 0x1b, 0xf7,
	0xf3, 0x99, 0x6d, 0x5e, 0x48, 0x91, 0xc0, 0xd0, 0xc5, 0x79, 0x78, 0x41, 0x59, 0x21, 0x2e, 0xff,
	0xc2, 0x4b, 0x38, 0x83, 0x32, 0xe6, 0xe2, 0x7f, 0x1e, 0x17, 0x58, 0x1d, 0x1a, 0xfa, 0x85, 0x82,
	0x94, 0xc8, 0x72, 0x7e, 0xb7, 0xac, 0x0e, 0xfc, 0xfd, 0x16, 0x27, 0x75, 0x8d, 0xcb, 0x08, 0xfe,
	0x0a, 0x02, 0x0d, 0x36, 0x11, 0x22, 0x84, 0x40, 0x34, 0x3e, 0x2d, 0x68, 0x5a, 0xa7, 0x67, 0xae,
	0x87, 0x07, 0x10, 0x60, 0x14, 0x73, 0x3c, 0x51, 0x19, 0xa3, 0xb5, 0xcf, 0x13, 0xf0, 0x15, 0x4e,
};

static const UINT8 kof99_address_16_23_xor1[256] =
{
	0x00, 0x5f, 0x03, 0x52, 0xce, 0xe3, 0x7d, 0x8f, 0x6b, 0xf8, 0x20, 0xde, 0x7b, 0x7e, 0x39, 0xbe,
	0xf5, 0x94, 0x18, 0x78, 0x80, 0xc9, 0x7f, 0x7a, 0x3e, 0x63, 0xf2, 0xe0, 0x4e, 0xf7, 0x87, 0x27,
	0x69, 0x6c, 0xa4, 0x1d, 0x85, 0x5b, 0xe6, 0x44, 0x25, 0x0c, 0x98, 0xc7, 0x01, 0x02, 0xa3, 0x26,
	0x09, 0x38, 0xdb, 0xc3, 0x1e, 0xcf, 0x23, 0x45, 0x68, 0x76, 0xd6, 0x22, 0x5d, 0x5a, 0xae, 0x16,
	0x9f, 0xa2, 0xb5, 0xcd, 0x81, 0xea, 0x5e, 0xb8, 0xb9, 0x9d, 0x9c, 0x1a, 0x0f, 0xff, 0xe1, 0xe7,
	0x74, 0xaa, 0xd4, 0xaf, 0xfc, 0xc6, 0x33, 0x29, 0x5c, 0xab, 0x95, 0xf0, 0x19, 0x47, 0x59, 0x67,
	0xf3, 0x96, 0x60, 0x1f, 0x62, 0x92, 0xbd, 0x89, 0xee, 0x28, 0x13, 0x06, 0xfe, 0xfa, 0x32, 0x6d,
	0x57, 0x3c, 0x54, 0x50, 0x2c, 0x58, 0x49, 0xfb, 0x17, 0xcc, 0xef, 0xb2, 0xb4, 0xf9, 0x07, 0x70,
	0xc5, 0xa9, 0xdf, 0xd5, 0x3b, 0x86, 0x2b, 0x0d, 0x6e, 0x4d, 0x0a, 0x90, 0x43, 0x31, 0xc1, 0xf6,
	0x88, 0x0b, 0xda, 0x53, 0x14, 0xdc, 0x75, 0x8e, 0xb0, 0xeb, 0x99, 0x46, 0xa1, 0x15, 0x71, 0xc8,
	0xe9, 0x3f, 0x4a, 0xd9, 0x73, 0xe5, 0x7c, 0x30, 0x77, 0xd3, 0xb3, 0x4b, 0x37, 0x72, 0xc2, 0x04,
	0x97, 0x08, 0x36, 0xb1, 0x3a, 0x61, 0xec, 0xe2, 0x1c, 0x9a, 0x8b, 0xd1, 0x1b, 0x2e, 0x9e, 0x8a,
	0xd8, 0x41, 0xe4, 0xc4, 0x40, 0x2f, 0xad, 0xc0, 0xb6, 0x84, 0x51, 0x66, 0xbb, 0x12, 0xe8, 0xdd,
	0xcb, 0xbc, 0x6f, 0xd0, 0x11, 0x83, 0x56, 0x4c, 0xca, 0xbf, 0x05, 0x10, 0xd7, 0xba, 0xfd, 0xed,
	0x8c, 0x0e, 0x4f, 0x3d, 0x35, 0x91, 0xb7, 0xac, 0x34, 0x64, 0x2a, 0xf1, 0x79, 0x6a, 0x9b, 0x2d,
	0x65, 0xf4, 0x42, 0xa0, 0x8d, 0xa7, 0x48, 0x55, 0x21, 0x93, 0x24, 0xd2, 0xa6, 0xa5, 0xa8, 0x82,
};

static const UINT8 kof99_address_16_23_xor2[256] =
{
	0x29, 0x97, 0x1a, 0x2c, 0x0b, 0x94, 0x3e, 0x75, 0x01, 0x0d, 0x1b, 0xe1, 0x4d, 0x38, 0x39, 0x8f,
	0xe7, 0xd0, 0x60, 0x90, 0xb2, 0x0f, 0xbb, 0x70, 0x1f, 0xe6, 0x5b, 0x87, 0xb4, 0x43, 0xfd, 0xf5,
	0xf6, 0xf9, 0xad, 0xc0, 0x98, 0x17, 0x9f, 0x91, 0x15, 0x51, 0x55, 0x64, 0x6c, 0x18, 0x61, 0x0e,
	0xd9, 0x93, 0xab, 0xd6, 0x24, 0x2f, 0x6a, 0x3a, 0x22, 0xb1, 0x4f, 0xaa, 0x23, 0x48, 0xed, 0xb9,
	0x88, 0x8b, 0xa3, 0x6b, 0x26, 0x4c, 0xe8, 0x2d, 0x1c, 0x99, 0xbd, 0x5c, 0x58, 0x08, 0x50, 0xf2,
	0x2a, 0x62, 0xc1, 0x72, 0x66, 0x04, 0x10, 0x37, 0x6e, 0xfc, 0x44, 0xa9, 0xdf, 0xd4, 0x20, 0xdd,
	0xee, 0x41, 0xdb, 0x73, 0xde, 0x54, 0xec, 0xc9, 0xf3, 0x4b, 0x2e, 0xae, 0x5a, 0x4a, 0x5e, 0x47,
	0x07, 0x2b, 0x76, 0xa4, 0xe3, 0x28, 0xfe, 0xb0, 0xf0, 0x02, 0x06, 0xd1, 0xaf, 0x42, 0xc2, 0xa5,
	0xe0, 0x67, 0xbf, 0x16, 0x8e, 0x35, 0xce, 0x8a, 0xe5, 0x3d, 0x7b, 0x96, 0xd7, 0x79, 0x52, 0x1e,
	0xa1, 0xfb, 0x9b, 0xbe, 0x21, 0x9c, 0xe9, 0x56, 0x14, 0x7f, 0xa0, 0xe4, 0xc3, 0xc4, 0x46, 0xea,
	0xf7, 0xd2, 0x1d, 0x31, 0x0a, 0x5f, 0xeb, 0xa2, 0x68, 0x8d, 0xb5, 0xc5, 0x74, 0x0c, 0xdc, 0x82,
	0x80, 0x09, 0x19, 0x95, 0x71, 0x9a, 0x11, 0x57, 0x77, 0x4e, 0xc6, 0xff, 0x12, 0x03, 0xa7, 0xc7,
	0xf4, 0xc8, 0xb6, 0x7a, 0x59, 0x36, 0x3c, 0x53, 0xe2, 0x69, 0x8c, 0x25, 0x05, 0x45, 0x63, 0xf8,
	0x34, 0x89, 0x33, 0x3f, 0x85, 0x27, 0xbc, 0x65, 0xfa, 0xa8, 0x6d, 0x84, 0x5d, 0xba, 0x40, 0x32,
	0x30, 0xef, 0x83, 0x13, 0xa6, 0x78, 0xcc, 0x81, 0x9e, 0xda, 0xca, 0xd3, 0x7e, 0x9d, 0x6f, 0xcd,
	0xb7, 0xb3, 0xd8, 0xcf, 0x3b, 0x00, 0x92, 0xb8, 0x86, 0xac, 0x49, 0x7c, 0xf1, 0xd5, 0xcb, 0x7d,
};

static const UINT8 kof99_address_0_7_xor[256] =
{
	0x74, 0xad, 0x5d, 0x1d, 0x9e, 0xc3, 0xfa, 0x4e, 0xf7, 0xdb, 0xca, 0xa2, 0x64, 0x36, 0x56, 0x0c,
	0x4f, 0xcf, 0x43, 0x66, 0x1e, 0x91, 0xe3, 0xa5, 0x58, 0xc2, 0xc1, 0xd4, 0xb9, 0xdd, 0x76, 0x16,
	0xce, 0x61, 0x75, 0x01, 0x2b, 0x22, 0x38, 0x55, 0x50, 0xef, 0x6c, 0x99, 0x05, 0xe9, 0xe8, 0xe0,
	0x2d, 0xa4, 0x4b, 0x4a, 0x42, 0xae, 0xba, 0x8c, 0x6f, 0x93, 0x14, 0xbd, 0x71, 0x21, 0xb0, 0x02,
	0x15, 0xc4, 0xe6, 0x60, 0xd7, 0x44, 0xfd, 0x85, 0x7e, 0x78, 0x8f, 0x00, 0x81, 0xf1, 0xa7, 0x3b,
	0xa0, 0x10, 0xf4, 0x9f, 0x39, 0x88, 0x35, 0x62, 0xcb, 0x19, 0x31, 0x11, 0x51, 0xfb, 0x2a, 0x20,
	0x45, 0xd3, 0x7d, 0x92, 0x1b, 0xf2, 0x09, 0x0d, 0x97, 0xa9, 0xb5, 0x3c, 0xee, 0x5c, 0xaf, 0x7b,
	0xd2, 0x3a, 0x49, 0x8e, 0xb6, 0xcd, 0xd9, 0xde, 0x8a, 0x29, 0x6e, 0xd8, 0x0b, 0xe1, 0x69, 0x87,
	0x1a, 0x96, 0x18, 0xcc, 0xdf, 0xe7, 0xc5, 0xc7, 0xf8, 0x52, 0xc9, 0xf0, 0xb7, 0xe5, 0x33, 0xda,
	0x67, 0x9d, 0xa3, 0x03, 0x0e, 0x72, 0x26, 0x79, 0xe2, 0xb8, 0xfc, 0xaa, 0xfe, 0xb4, 0x86, 0xc8,
	0xd1, 0xbc, 0x12, 0x08, 0x77, 0xeb, 0x40, 0x8d, 0x04, 0x25, 0x4d, 0x5a, 0x6a, 0x7a, 0x2e, 0x41,
	0x65, 0x1c, 0x13, 0x94, 0xb2, 0x63, 0x28, 0x59, 0x5e, 0x9a, 0x30, 0x07, 0xc6, 0xbf, 0x17, 0xf5,
	0x0f, 0x89, 0xf3, 0x1f, 0xea, 0x6d, 0xb3, 0xc0, 0x70, 0x47, 0xf9, 0x53, 0xf6, 0xd6, 0x54, 0xed,
	0x6b, 0x4c, 0xe4, 0x8b, 0x83, 0x24, 0x90, 0xb1, 0x7c, 0xbb, 0x73, 0xab, 0xd5, 0x2f, 0x5f, 0xec,
	0x9c, 0x2c, 0xa8, 0x34, 0x46, 0x37, 0x27, 0xa1, 0x0a, 0x06, 0x80, 0x68, 0x82, 0x32, 0x84, 0xff,
	0x48, 0xac, 0x7f, 0x3f, 0x95, 0xdc, 0x98, 0x9b, 0xbe, 0x23, 0x57, 0x3e, 0x5b, 0xd0, 0x3d, 0xa6,
};

static const UINT8 m1_address_8_15_xor[256] =
{
		0x0a, 0x72, 0xb7, 0xaf, 0x67, 0xde, 0x1d, 0xb1, 0x78, 0xc4, 0x4f, 0xb5, 0x4b, 0x18, 0x76, 0xdd,
		0x11, 0xe2, 0x36, 0xa1, 0x82, 0x03, 0x98, 0xa0, 0x10, 0x5f, 0x3f, 0xd6, 0x1f, 0x90, 0x6a, 0x0b,
		0x70, 0xe0, 0x64, 0xcb, 0x9f, 0x38, 0x8b, 0x53, 0x04, 0xca, 0xf8, 0xd0, 0x07, 0x68, 0x56, 0x32,
		0xae, 0x1c, 0x2e, 0x48, 0x63, 0x92, 0x9a, 0x9c, 0x44, 0x85, 0x41, 0x40, 0x09, 0xc0, 0xc8, 0xbf,
		0xea, 0xbb, 0xf7, 0x2d, 0x99, 0x21, 0xf6, 0xba, 0x15, 0xce, 0xab, 0xb0, 0x2a, 0x60, 0xbc, 0xf1,
		0xf0, 0x9e, 0xd5, 0x97, 0xd8, 0x4e, 0x14, 0x9d, 0x42, 0x4d, 0x2c, 0x5c, 0x2b, 0xa6, 0xe1, 0xa7,
		0xef, 0x25, 0x33, 0x7a, 0xeb, 0xe7, 0x1b, 0x6d, 0x4c, 0x52, 0x26, 0x62, 0xb6, 0x35, 0xbe, 0x80,
		0x01, 0xbd, 0xfd, 0x37, 0xf9, 0x47, 0x55, 0x71, 0xb4, 0xf2, 0xff, 0x27, 0xfa, 0x23, 0xc9, 0x83,
		0x17, 0x39, 0x13, 0x0d, 0xc7, 0x86, 0x16, 0xec, 0x49, 0x6f, 0xfe, 0x34, 0x05, 0x8f, 0x00, 0xe6,
		0xa4, 0xda, 0x7b, 0xc1, 0xf3, 0xf4, 0xd9, 0x75, 0x28, 0x66, 0x87, 0xa8, 0x45, 0x6c, 0x20, 0xe9,
		0x77, 0x93, 0x7e, 0x3c, 0x1e, 0x74, 0xf5, 0x8c, 0x3e, 0x94, 0xd4, 0xc2, 0x5a, 0x06, 0x0e, 0xe8,
		0x3d, 0xa9, 0xb2, 0xe3, 0xe4, 0x22, 0xcf, 0x24, 0x8e, 0x6b, 0x8a, 0x8d, 0x84, 0x4a, 0xd2, 0x91,
		0x88, 0x79, 0x57, 0xa5, 0x0f, 0xcd, 0xb9, 0xac, 0x3b, 0xaa, 0xb3, 0xd1, 0xee, 0x31, 0x81, 0x7c,
		0xd7, 0x89, 0xd3, 0x96, 0x43, 0xc5, 0xc6, 0xc3, 0x69, 0x7f, 0x46, 0xdf, 0x30, 0x5b, 0x6e, 0xe5,
		0x08, 0x95, 0x9b, 0xfb, 0xb8, 0x58, 0x0c, 0x61, 0x50, 0x5d, 0x3a, 0xa2, 0x29, 0x12, 0xfc, 0x51,
		0x7d, 0x1a, 0x02, 0x65, 0x54, 0x5e, 0x19, 0xcc, 0xdc, 0xdb, 0x73, 0xed, 0xad, 0x59, 0x2f, 0xa3,
};

static const UINT8 m1_address_0_7_xor[256] =
{
		0xf4, 0xbc, 0x02, 0xf7, 0x2c, 0x3d, 0xe8, 0xd9, 0x50, 0x62, 0xec, 0xbd, 0x53, 0x73, 0x79, 0x61,
		0x00, 0x34, 0xcf, 0xa2, 0x63, 0x28, 0x90, 0xaf, 0x44, 0x3b, 0xc5, 0x8d, 0x3a, 0x46, 0x07, 0x70,
		0x66, 0xbe, 0xd8, 0x8b, 0xe9, 0xa0, 0x4b, 0x98, 0xdc, 0xdf, 0xe2, 0x16, 0x74, 0xf1, 0x37, 0xf5,
		0xb7, 0x21, 0x81, 0x01, 0x1c, 0x1b, 0x94, 0x36, 0x09, 0xa1, 0x4a, 0x91, 0x30, 0x92, 0x9b, 0x9a,
		0x29, 0xb1, 0x38, 0x4d, 0x55, 0xf2, 0x56, 0x18, 0x24, 0x47, 0x9d, 0x3f, 0x80, 0x1f, 0x22, 0xa4,
		0x11, 0x54, 0x84, 0x0d, 0x25, 0x48, 0xee, 0xc6, 0x59, 0x15, 0x03, 0x7a, 0xfd, 0x6c, 0xc3, 0x33,
		0x5b, 0xc4, 0x7b, 0x5a, 0x05, 0x7f, 0xa6, 0x40, 0xa9, 0x5d, 0x41, 0x8a, 0x96, 0x52, 0xd3, 0xf0,
		0xab, 0x72, 0x10, 0x88, 0x6f, 0x95, 0x7c, 0xa8, 0xcd, 0x9c, 0x5f, 0x32, 0xae, 0x85, 0x39, 0xac,
		0xe5, 0xd7, 0xfb, 0xd4, 0x08, 0x23, 0x19, 0x65, 0x6b, 0xa7, 0x93, 0xbb, 0x2b, 0xbf, 0xb8, 0x35,
		0xd0, 0x06, 0x26, 0x68, 0x3e, 0xdd, 0xb9, 0x69, 0x2a, 0xb2, 0xde, 0x87, 0x45, 0x58, 0xff, 0x3c,
		0x9e, 0x7d, 0xda, 0xed, 0x49, 0x8c, 0x14, 0x8e, 0x75, 0x2f, 0xe0, 0x6e, 0x78, 0x6d, 0x20, 0xd2,
		0xfa, 0x2d, 0x51, 0xcc, 0xc7, 0xe7, 0x1d, 0x27, 0x97, 0xfc, 0x31, 0xdb, 0xf8, 0x42, 0xe3, 0x99,
		0x5e, 0x83, 0x0e, 0xb4, 0x2e, 0xf6, 0xc0, 0x0c, 0x4c, 0x57, 0xb6, 0x64, 0x0a, 0x17, 0xa3, 0xc1,
		0x77, 0x12, 0xfe, 0xe6, 0x8f, 0x13, 0x71, 0xe4, 0xf9, 0xad, 0x9f, 0xce, 0xd5, 0x89, 0x7e, 0x0f,
		0xc2, 0x86, 0xf3, 0x67, 0xba, 0x60, 0x43, 0xc9, 0x04, 0xb3, 0xb0, 0x1e, 0xb5, 0xc8, 0xeb, 0xa5,
		0x76, 0xea, 0x5c, 0x82, 0x1a, 0x4f, 0xaa, 0xca, 0xe1, 0x0b, 0x4e, 0xcb, 0x6a, 0xef, 0xd1, 0xd6,
};

const UINT8 *type0_t03;
const UINT8 *type0_t12;
const UINT8 *type1_t03;
const UINT8 *type1_t12;
const UINT8 *address_8_15_xor1;
const UINT8 *address_8_15_xor2;
const UINT8 *address_16_23_xor1;
const UINT8 *address_16_23_xor2;
const UINT8 *address_0_7_xor;

static void neogeo_bootleg_sx_decrypt(int value )
{
	int sx_size = get_region_size(REGION_FIXED);
	UINT8 *rom = load_region[REGION_FIXED];
	int i;

	if (value == 1)
	{
		UINT8 *buf = AllocateMem(sx_size);
		memcpy( buf, rom, sx_size );

		for( i = 0; i < sx_size; i += 0x10 )
		{
			memcpy( &rom[ i ], &buf[ i + 8 ], 8 );
			memcpy( &rom[ i + 8 ], &buf[ i ], 8 );
		}
		FreeMem(buf);
	}
	else if (value == 2)
	{
		for( i = 0; i < sx_size; i++ )
			rom[ i ] = BITSWAP8( rom[ i ], 7, 6, 0, 4, 3, 2, 1, 5 );
	}
}

static void neogeo_bootleg_cx_decrypt()
{
	int i;
	int cx_size = get_region_size(REGION_SPRITES);
	UINT8 *rom = load_region[ REGION_SPRITES ];
	UINT8 *buf = AllocateMem(cx_size);

	memcpy( buf, rom, cx_size );

	for( i = 0; i < cx_size / 0x40; i++ ){
		memcpy( &rom[ i * 0x40 ], &buf[ (i ^ 1) * 0x40 ], 0x40 );
	}

	FreeMem(buf);
}

static void decrypt(UINT8 *r0, UINT8 *r1,
					UINT8 c0,  UINT8 c1,
					const UINT8 *table0hi,
					const UINT8 *table0lo,
					const UINT8 *table1,
					int base,
					int invert)
{
	UINT8 tmp,xor0,xor1;

	tmp = table1[(base & 0xff) ^ address_0_7_xor[(base >> 8) & 0xff]];
	xor0 = (table0hi[(base >> 8) & 0xff] & 0xfe) | (tmp & 0x01);
	xor1 = (tmp & 0xfe) | (table0lo[(base >> 8) & 0xff] & 0x01);

	if (invert)
	{
		*r0 = c1 ^ xor0;
		*r1 = c0 ^ xor1;
	}
	else
	{
		*r0 = c0 ^ xor0;
		*r1 = c1 ^ xor1;
	}
}

static void neogeo_gfx_decrypt(int extra_xor)
{
	int rom_size;
	UINT8 *buf;
	UINT8 *rom;
	int rpos;

	load_message(_("decrypting sprites (kof99 type)"));
	rom_size = get_region_size(REGION_SPRITES);

	buf = AllocateMem(rom_size);

	rom = load_region[REGION_SPRITES];

	// Data xor
	for (rpos = 0;rpos < rom_size/4;rpos++)
	{
		decrypt(buf+4*rpos+0, buf+4*rpos+3, rom[4*rpos+0], rom[4*rpos+3], type0_t03, type0_t12, type1_t03, rpos, (rpos>>8) & 1);
		decrypt(buf+4*rpos+1, buf+4*rpos+2, rom[4*rpos+1], rom[4*rpos+2], type0_t12, type0_t03, type1_t12, rpos, ((rpos>>16) ^ address_16_23_xor2[(rpos>>8) & 0xff]) & 1);
	}

	// Address xor
	for (rpos = 0;rpos < rom_size/4;rpos++)
	{
		int baser;

		baser = rpos;

		baser ^= extra_xor;

		baser ^= address_8_15_xor1[(baser >> 16) & 0xff] << 8;
		baser ^= address_8_15_xor2[baser & 0xff] << 8;
		baser ^= address_16_23_xor1[baser & 0xff] << 16;
		baser ^= address_16_23_xor2[(baser >> 8) & 0xff] << 16;
		baser ^= address_0_7_xor[(baser >> 8) & 0xff];


		if (rom_size == 0x3000000) /* special handling for preisle2 */
		{
			if (rpos < 0x2000000/4)
				baser &= (0x2000000/4)-1;
			else
				baser = 0x2000000/4 + (baser & ((0x1000000/4)-1));
		}
		else if (rom_size == 0x6000000) /* special handling for kf2k3pcb */
		{
			if (rpos < 0x4000000/4)
				baser &= (0x4000000/4)-1;
			else
				baser = 0x4000000/4 + (baser & ((0x1000000/4)-1));
		}
		else /* Clamp to the real rom size */
			baser &= (rom_size/4)-1;

		rom[4*rpos+0] = buf[4*baser+0];
		rom[4*rpos+1] = buf[4*baser+1];
		rom[4*rpos+2] = buf[4*baser+2];
		rom[4*rpos+3] = buf[4*baser+3];
	}

	FreeMem( buf);
}

/* the S data comes from the end of the C data */
static void neogeo_sfix_decrypt()
{
	int i;
	int rom_size = get_region_size(REGION_SPRITES);
	int tx_size = get_region_size(REGION_FIXED);
	UINT8 *src = load_region[REGION_SPRITES]+rom_size-tx_size;
	UINT8 *dst = load_region[REGION_FIXED];
	load_message(_("recreating fixed layer"));

	for (i = 0;i < tx_size;i++)
		dst[i] = src[(i & ~0x1f) + ((i & 7) << 2) + ((~i & 8) >> 2) + ((i & 0x10) >> 4)];
}

static void kof2000_neogeo_gfx_decrypt(int extra_xor)
{
	type0_t03 =          kof2000_type0_t03;
	type0_t12 =          kof2000_type0_t12;
	type1_t03 =          kof2000_type1_t03;
	type1_t12 =          kof2000_type1_t12;
	address_8_15_xor1 =  kof2000_address_8_15_xor1;
	address_8_15_xor2 =  kof2000_address_8_15_xor2;
	address_16_23_xor1 = kof2000_address_16_23_xor1;
	address_16_23_xor2 = kof2000_address_16_23_xor2;
	address_0_7_xor =    kof2000_address_0_7_xor;
	neogeo_gfx_decrypt(extra_xor);
	neogeo_sfix_decrypt();
}

static void kof99_neogeo_gfx_decrypt(int extra_xor)
{
	type0_t03 =          kof99_type0_t03;
	type0_t12 =          kof99_type0_t12;
	type1_t03 =          kof99_type1_t03;
	type1_t12 =          kof99_type1_t12;
	address_8_15_xor1 =  kof99_address_8_15_xor1;
	address_8_15_xor2 =  kof99_address_8_15_xor2;
	address_16_23_xor1 = kof99_address_16_23_xor1;
	address_16_23_xor2 = kof99_address_16_23_xor2;
	address_0_7_xor =    kof99_address_0_7_xor;
	neogeo_gfx_decrypt(extra_xor);
	neogeo_sfix_decrypt();
}

/* Kof98 uses an early encryption, quite different from the others */
static void kof98_decrypt_68k()
{
        UINT8 *src = ROM;
        UINT8 *dst = AllocateMem( 0x200000);
        int i, j, k;
        static const UINT32 sec[]={0x000000,0x100000,0x000004,0x100004,0x10000a,
0x00000a,0x10000e,0x00000e};
        static const UINT32 pos[]={0x000,0x004,0x00a,0x00e};

        memcpy( dst, src, 0x200000);
        for( i=0x800; i<0x100000; i+=0x200 )
        {
                for( j=0; j<0x100; j+=0x10 )
                {
                        for( k=0; k<16; k+=2)
                        {
                                memcpy( &src[i+j+k],       &dst[ i+j+sec[k/2]+0x100 ], 2 );
                                memcpy( &src[i+j+k+0x100], &dst[ i+j+sec[k/2] ],
					2 );
                        }
                        if( i >= 0x080000 && i < 0x0c0000)
                        {
                                for( k=0; k<4; k++ )
                                {
                                        memcpy( &src[i+j+pos[k]],       &dst[i+j+pos[k]],       2 );
                                        memcpy( &src[i+j+pos[k]+0x100], &dst[i+j+pos[k]+0x100], 2 );
                                }
                        }
                        else if( i >= 0x0c0000 )
                        {
                                for( k=0; k<4; k++ )
                                {
                                        memcpy( &src[i+j+pos[k]],       &dst[i+j+pos[k]+0x100], 2 );
                                        memcpy( &src[i+j+pos[k]+0x100], &dst[i+j+pos[k]],       2 );
                                }
                        }
                }
                memcpy( &src[i+0x000000], &dst[i+0x000000], 2 );
                memcpy( &src[i+0x000002], &dst[i+0x100000], 2 );
                memcpy( &src[i+0x000100], &dst[i+0x000100], 2 );
                memcpy( &src[i+0x000102], &dst[i+0x100100], 2 );
        }
        memmove( &src[0x100000], &src[0x200000], 0x400000 );

        FreeMem( dst);
}

static void mslug3_decrypt_68k() {
    UINT16 *rom;
    int i,j;

    /* thanks to Razoola and Mr K for the info */
    rom = (UINT16 *)&ROM[0x100000];
    /* swap data lines on the whole ROMs */
    for (i = 0;i < 0x800000/2;i++)
    {
	rom[i] = BITSWAP16(rom[i],4,11,14,3,1,13,0,7,2,8,12,15,10,9,5,6);
    }

    /* swap address lines & relocate fixed part */
    rom = (UINT16 *)ROM;
    for (i = 0;i < 0x0c0000/2;i++)
    {
	rom[i] = rom[0x5d0000/2 + BITSWAP24(i,23,22,21,20,19,18,15,2,1,13,3,0,9,6,16,4,11,5,7,12,17,14,10,8)];
    }

    /* swap address lines for the banked part */
    rom = (UINT16 *)&ROM[0x100000];
    for (i = 0;i < 0x800000/2;i+=0x10000/2)
    {
	UINT16 buffer[0x10000/2];
	memcpy(buffer,&rom[i],0x10000);
	for (j = 0;j < 0x10000/2;j++)
	{
	    rom[i+j] = buffer[BITSWAP24(j,23,22,21,20,19,18,17,16,15,2,11,0,14,6,4,13,8,9,3,10,7,5,12,1)];
	}
    }
}

static void kof99_decrypt_68k()
{
	UINT16 *rom;
	int i,j;

	rom = (UINT16*)&ROM[0x100000];
	/* swap data lines on the whole ROMs */
	for (i = 0;i < 0x800000/2;i++)
	{
		rom[i] = BITSWAP16(rom[i],13,7,3,0,9,4,5,6,1,12,8,14,10,11,2,15);
	}

	/* swap address lines for the banked part */
	for (i = 0;i < 0x600000/2;i+=0x800/2)
	{
		UINT16 buffer[0x800/2];
		memcpy(buffer,&rom[i],0x800);
		for (j = 0;j < 0x800/2;j++)
		{
			rom[i+j] = buffer[BITSWAP24(j,23,22,21,20,19,18,17,16,15,14,13,12,11,10,6,2,4,9,8,3,1,7,0,5)];
		}
	}

	/* swap address lines & relocate fixed part */
	rom = (UINT16*)ROM;
	for (i = 0;i < 0x0c0000/2;i++)
	{
		rom[i] = rom[0x700000/2 + BITSWAP24(i,23,22,21,20,19,18,11,6,14,17,16,5,8,10,12,0,4,3,2,7,9,15,13,1)];
	}
}

static void kof2000_decrypt_68k()
{
    UINT16 *rom;
    int i,j;

    /* thanks to Razoola and Mr K for the info */
    rom = (UINT16 *)&ROM[0x100000];
    /* swap data lines on the whole ROMs */
    for (i = 0;i < 0x800000/2;i++)
    {
	rom[i] = BITSWAP16(rom[i],12,8,11,3,15,14,7,0,10,13,6,5,9,2,1,4);
    }

    /* swap address lines for the banked part */
    for (i = 0;i < 0x63a000/2;i+=0x800/2)
    {
	UINT16 buffer[0x800/2];
	memcpy(buffer,&rom[i],0x800);
	for (j = 0;j < 0x800/2;j++)
	{
	    rom[i+j] = buffer[BITSWAP24(j,23,22,21,20,19,18,17,16,15,14,13,12,11,10,4,1,3,8,6,2,7,0,9,5)];
	}
    }

    /* swap address lines & relocate fixed part */
    rom = (UINT16 *)ROM;
    for (i = 0;i < 0x0c0000/2;i++)
    {
	rom[i] = rom[0x73a000/2 + BITSWAP24(i,23,22,21,20,19,18,8,4,15,13,3,14,16,2,6,17,7,12,10,0,5,11,1,9)];
    }
}

static UINT16 generate_cs16(UINT8 *rom, int size)
{
	UINT16 cs16;
	int i;
	cs16 = 0x0000;
	for (i=0;i<size;i++ )
	{
		cs16 += rom[i];
	}
	return cs16&0xFFFF;
}

static int m1_address_scramble(int address, UINT16 key)
{
	int block;
	int aux;

	const int p1[8][16] = {
		{15,14,10,7,1,2,3,8,0,12,11,13,6,9,5,4},
		{7,1,8,11,15,9,2,3,5,13,4,14,10,0,6,12},
		{8,6,14,3,10,7,15,1,4,0,2,5,13,11,12,9},
		{2,8,15,9,3,4,11,7,13,6,0,10,1,12,14,5},
		{1,13,6,15,14,3,8,10,9,4,7,12,5,2,0,11},
		{11,15,3,4,7,0,9,2,6,14,12,1,8,5,10,13},
		{10,5,13,8,6,15,1,14,11,9,3,0,12,7,4,2},
		{9,3,7,0,2,12,4,11,14,10,5,8,15,13,1,6},
	};

	block = (address>>16)&7;
	aux = address&0xffff;

		aux ^= BITSWAP16(key,12,0,2,4,8,15,7,13,10,1,3,6,11,9,14,5);
	aux = BITSWAP16(aux,
		p1[block][15],p1[block][14],p1[block][13],p1[block][12],
		p1[block][11],p1[block][10],p1[block][9],p1[block][8],
		p1[block][7],p1[block][6],p1[block][5],p1[block][4],
		p1[block][3],p1[block][2],p1[block][1],p1[block][0]);
		aux ^= m1_address_0_7_xor[(aux>>8)&0xff];
	aux ^= m1_address_8_15_xor[aux&0xff]<<8;
	aux = BITSWAP16(aux, 7,15,14,6,5,13,12,4,11,3,10,2,9,1,8,0);

	return (block<<16)|aux;
}

static void neogeo_cmc50_m1_decrypt() {
    UINT8* rom = REG(AUDIOCRYPT);
    size_t rom_size = 0x80000;

    UINT8* buffer = AllocateMem( rom_size);

    UINT32 i;

    UINT16 key=generate_cs16(rom,0x10000);

    //printf("key %04x\n",key);

    for (i=0; i<rom_size; i++)
    {
	buffer[i] = rom[m1_address_scramble(i,key)];
    }

    memcpy(rom,buffer,rom_size);

    load_region[REGION_ROM2] = rom;
    set_region_size(REGION_ROM2,0x80000);
    FreeMem(buffer);
}

static void neo_pcm2_snk_1999(int value)
{   /* thanks to Elsemi for the NEO-PCM2 info */
    UINT16 *rom = (UINT16 *)REG(SMP1);
    int size = get_region_size(REGION_SMP1);
    int i, j;

    if( rom != NULL )
    {   /* swap address lines on the whole ROMs */
	UINT16 *buffer = (UINT16*)AllocateMem(value);

	for( i = 0; i < size / 2; i += ( value / 2 ) )
	{
	    memcpy( buffer, &rom[ i ], value );
	    for( j = 0; j < (value / 2); j++ )
	    {
		rom[ i + j ] = buffer[ j ^ (value/4) ];
	    }
	}
	FreeMem( buffer);
    }
}

static void garou_decrypt_68k() {
    UINT16 *rom;
    int i,j;

    /* thanks to Razoola and Mr K for the info */
    rom = (UINT16 *)&ROM[0x100000];
    /* swap data lines on the whole ROMs */
    for (i = 0;i < 0x800000/2;i++)
    {
	rom[i] = BITSWAP16(rom[i],13,12,14,10,8,2,3,1,5,9,11,4,15,0,6,7);
    }

    /* swap address lines & relocate fixed part */
    rom = (UINT16 *)ROM;
    for (i = 0;i < 0x0c0000/2;i++)
    {
	rom[i] = rom[0x710000/2 + BITSWAP24(i,23,22,21,20,19,18,4,5,16,14,7,9,6,13,17,15,3,1,2,12,11,8,10,0)];
    }

    /* swap address lines for the banked part */
    rom = (UINT16 *)&ROM[0x100000];
    for (i = 0;i < 0x800000/2;i+=0x8000/2)
    {
	UINT16 buffer[0x8000/2];
	memcpy(buffer,&rom[i],0x8000);
	for (j = 0;j < 0x8000/2;j++)
	{
	    rom[i+j] = buffer[BITSWAP24(j,23,22,21,20,19,18,17,16,15,14,9,4,8,3,13,6,2,7,0,12,1,11,10,5)];
	}
    }
}

static void garouh_decrypt_68k() {
    UINT16 *rom;
    int i,j;

    /* thanks to Razoola and Mr K for the info */
    rom = (UINT16 *)&ROM[0x100000];
    /* swap data lines on the whole ROMs */
    for (i = 0;i < 0x800000/2;i++)
    {
	rom[i] = BITSWAP16(rom[i],14,5,1,11,7,4,10,15,3,12,8,13,0,2,9,6);
    }

    /* swap address lines & relocate fixed part */
    rom = (UINT16 *)ROM;
    for (i = 0;i < 0x0c0000/2;i++)
    {
	rom[i] = rom[0x7f8000/2 + BITSWAP24(i,23,22,21,20,19,18,5,16,11,2,6,7,17,3,12,8,14,4,0,9,1,10,15,13)];
    }

    /* swap address lines for the banked part */
    rom = (UINT16 *)&ROM[0x100000];
    for (i = 0;i < 0x800000/2;i+=0x8000/2)
    {
	UINT16 buffer[0x8000/2];
	memcpy(buffer,&rom[i],0x8000);
	for (j = 0;j < 0x8000/2;j++)
	{
	    rom[i+j] = buffer[BITSWAP24(j,23,22,21,20,19,18,17,16,15,14,12,8,1,7,11,3,13,10,6,9,5,4,0,2)];
	}
    }
}

static void kof2002_decrypt_68k() {
    int i;
    static const int sec[]={0x100000,0x280000,0x300000,0x180000,0x000000,0x380000,0x200000,0x080000};
    UINT8 *src = &ROM[0x100000];
    UINT8 *dst = AllocateMem(0x400000);
    memcpy( dst, src, 0x400000 );
    for( i=0; i<8; ++i )
    {
	memcpy( src+i*0x80000, dst+sec[i], 0x80000 );
    }
    FreeMem( dst);
}

static void mslug5_decrypt_68k() {
    static const UINT8 xor1[ 0x20 ] = { 0xc2, 0x4b, 0x74, 0xfd, 0x0b, 0x34, 0xeb, 0xd7, 0x10, 0x6d, 0xf9, 0xce, 0x5d, 0xd5, 0x61, 0x29, 0xf5, 0xbe, 0x0d, 0x82, 0x72, 0x45, 0x0f, 0x24, 0xb3, 0x34, 0x1b, 0x99, 0xea, 0x09, 0xf3, 0x03 };
    static const UINT8 xor2[ 0x20 ] = { 0x36, 0x09, 0xb0, 0x64, 0x95, 0x0f, 0x90, 0x42, 0x6e, 0x0f, 0x30, 0xf6, 0xe5, 0x08, 0x30, 0x64, 0x08, 0x04, 0x00, 0x2f, 0x72, 0x09, 0xa0, 0x13, 0xc9, 0x0b, 0xa0, 0x3e, 0xc2, 0x00, 0x40, 0x2b };
    int i;
    int ofst;
    int rom_size = 0x800000;
    UINT8 *rom = ROM;
    UINT8 *buf = AllocateMem( rom_size );

    for( i = 0; i < 0x100000; i++ )
    {
	rom[ i ] ^= xor1[ (BYTE_XOR_LE(i) % 0x20) ];
    }
    for( i = 0x100000; i < 0x800000; i++ )
    {
	rom[ i ] ^= xor2[ (BYTE_XOR_LE(i) % 0x20) ];
    }

    for( i = 0x100000; i < 0x0800000; i += 4 )
    {
	UINT16 rom16;
	rom16 = rom[BYTE_XOR_LE(i+1)] | rom[BYTE_XOR_LE(i+2)]<<8;
	rom16 = BITSWAP16( rom16, 15, 14, 13, 12, 10, 11, 8, 9, 6, 7, 4, 5, 3, 2, 1, 0 );
	rom[BYTE_XOR_LE(i+1)] = rom16&0xff;
	rom[BYTE_XOR_LE(i+2)] = rom16>>8;
    }
    memcpy( buf, rom, rom_size );
    for( i = 0; i < 0x0100000 / 0x10000; i++ )
    {
	ofst = (i & 0xf0) + BITSWAP8( (i & 0x0f), 7, 6, 5, 4, 1, 0, 3, 2 );
	memcpy( &rom[ i * 0x10000 ], &buf[ ofst * 0x10000 ], 0x10000 );
    }
    for( i = 0x100000; i < 0x800000; i += 0x100 )
    {
	ofst = (i & 0xf000ff) + ((i & 0x000f00) ^ 0x00700) + (BITSWAP8( ((i & 0x0ff000) >> 12), 5, 4, 7, 6, 1, 0, 3, 2 ) << 12);
	memcpy( &rom[ i ], &buf[ ofst ], 0x100 );
    }
    memcpy( buf, rom, rom_size );
    memcpy( &rom[ 0x100000 ], &buf[ 0x700000 ], 0x100000 );
    memcpy( &rom[ 0x200000 ], &buf[ 0x100000 ], 0x600000 );
    FreeMem( buf );
}

static void matrim_decrypt_68k() {
    int i;
    static const int sec[]={0x100000,0x280000,0x300000,0x180000,0x000000,0x380000,0x200000,0x080000};
    UINT8 *src = &ROM[0x100000];
    UINT8 *dst = AllocateMem(0x400000);
    memcpy( dst, src, 0x400000);
    for( i=0; i<8; ++i )
    {
	memcpy( src+i*0x80000, dst+sec[i], 0x80000 );
    }
    FreeMem(dst);
}

static void samsho5_decrypt_68k() {
    int i;
    static const int sec[]={0x000000,0x080000,0x700000,0x680000,0x500000,0x180000,0x200000,0x480000,0x300000,0x780000,0x600000,0x280000,0x100000,0x580000,0x400000,0x380000};
    UINT8 *src = ROM;
    UINT8 *dst = AllocateMem( 0x800000);

    memcpy( dst, src, 0x800000 );
    for( i=0; i<16; ++i )
    {
	memcpy( src+i*0x80000, dst+sec[i], 0x80000 );
    }
    FreeMem( dst);
}

static void neo_pcm2_swap(int value) {
    static const UINT32 addrs[7][2]={
	{0x000000,0xa5000},
	{0xffce20,0x01000},
	{0xfe2cf6,0x4e001},
	{0xffac28,0xc2000},
	{0xfeb2c0,0x0a000},
	{0xff14ea,0xa7001},
	{0xffb440,0x02000}};
    static const UINT8 xordata[7][8]={
	{0xf9,0xe0,0x5d,0xf3,0xea,0x92,0xbe,0xef},
	{0xc4,0x83,0xa8,0x5f,0x21,0x27,0x64,0xaf},
	{0xc3,0xfd,0x81,0xac,0x6d,0xe7,0xbf,0x9e},
	{0xc3,0xfd,0x81,0xac,0x6d,0xe7,0xbf,0x9e},
	{0xcb,0x29,0x7d,0x43,0xd2,0x3a,0xc2,0xb4},
	{0x4b,0xa4,0x63,0x46,0xf0,0x91,0xea,0x62},
	{0x4b,0xa4,0x63,0x46,0xf0,0x91,0xea,0x62}};
    UINT8 *src = REG(SMP1);
    UINT8 *buf = AllocateMem(0x1000000);
    int i, j, d;

    memcpy(buf,src,0x1000000);
    for (i=0;i<0x1000000;i++)
    {
	j=BITSWAP24(i,23,22,21,20,19,18,17,0,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,16);
	j=j^addrs[value][1];
	d=((i+addrs[value][0])&0xffffff);
	src[j]=buf[d]^xordata[value][j&0x7];
    }
    FreeMem( buf);
}

static void kof98_prot_w(UINT32 offset, UINT16 data) {
    switch(data) {
    case 0x90:
	print_debug("kof98_prot_w %x 100:%x,%x\n",data,ReadWord(&ROM[0x100]),
		ReadWord(&ROM[0x102]));
	WriteWord(&ROM[0x100],0xc2);
	WriteWord(&ROM[0x102],0xfd);
	break;
    case 0xf0:
	print_debug("kof98_prot_w %x 100:%x,%x\n",data,ReadWord(&ROM[0x100]),
		ReadWord(&ROM[0x102]));
	WriteWord(&ROM[0x100],0x4e45);
	WriteWord(&ROM[0x102],0x4f2d);
	break;
    default:
	print_debug("kof98_prot_w %x\n",data);
    }
}

static UINT16 prot,mslugx_command,mslugx_counter,neogeo_rng;

static UINT16 read_rng(UINT32 offset) {
    	UINT16 old = neogeo_rng;

	UINT16 newbit = ((neogeo_rng >> 2) ^
						(neogeo_rng >> 3) ^
						(neogeo_rng >> 5) ^
						(neogeo_rng >> 6) ^
						(neogeo_rng >> 7) ^
						(neogeo_rng >>11) ^
						(neogeo_rng >>12) ^
						(neogeo_rng >>15)) & 1;

	neogeo_rng = (neogeo_rng << 1) | newbit;

	return old;
}

static void mslugx_prot_w(UINT32 offset, UINT16 data) {
    switch (offset & 0xf) {
    case 0: mslugx_command = 0; break;
    case 2:
    case 4: mslugx_command |= data; break;
    case 6: break; // finished ?
    case 0xa: mslugx_command = mslugx_counter = 0; break; // init ?
    default: print_debug("unknown mslugx_prot_w %x,%x\n",offset,data);
    }
}

static UINT16 mslugx_prot_r(UINT32 offset) {
    UINT16 res;
    switch(mslugx_command) {
    case 1: res = (ROM[(0xdedd2 + ((mslugx_counter >> 3) & 0xfff)) ^ 1] >> (~mslugx_counter & 0x07)) & 1;
	    mslugx_counter++;
	    break;
    case 0xfff:
	    {
		UINT16 select = ReadWord(&RAM[0xf00a]) - 1;
		res = (ROM[(0xdedd2 + ((select >> 3) & 0x0fff)) ^ 1] >> (~select & 0x07)) & 1;
	    }
	    break;
    default:
	    print_debug("unknown mslugx_prot_r pc:%x offset %x\n",s68000_pc,offset);
    }
    return res;
}

static void pvc_write_unpack_color() {
    UINT16 pen = ReadWord(pvc_cart+0xff0*2);

    UINT8 b = ((pen & 0x000f) << 1) | ((pen & 0x1000) >> 12);
    UINT8 g = ((pen & 0x00f0) >> 3) | ((pen & 0x2000) >> 13);
    UINT8 r = ((pen & 0x0f00) >> 7) | ((pen & 0x4000) >> 14);
    UINT8 s = (pen & 0x8000) >> 15;

    WriteWord(&pvc_cart[0xff1*2], (g << 8) | b);
    WriteWord(&pvc_cart[0xff2*2], (s << 8) | r);
}

static void pvc_write_pack_color() {
    UINT16 gb = ReadWord(&pvc_cart[0xff4*2]);
    UINT16 sr = ReadWord(&pvc_cart[0xff5*2]);

    WriteWord(&pvc_cart[0xff6*2], ((gb & 0x001e) >> 1) |
	    ((gb & 0x1e00) >> 5) |
	    ((sr & 0x001e) << 7) |
	    ((gb & 0x0001) << 12) |
	    ((gb & 0x0100) << 5) |
	    ((sr & 0x0001) << 14) |
	    ((sr & 0x0100) << 7));
}

static void pvc_write_bankswitch() {
    UINT32 bankaddress;

    bankaddress = ((ReadWord(&pvc_cart[0xff8*2]) >> 8)|
	    (ReadWord(&pvc_cart[0xff9*2]) << 8));
    WriteWord(&pvc_cart[0xff8*2], (ReadWord(&pvc_cart[0xff8*2]) & 0xfe00) | 0x00a0);
    WriteWord(&pvc_cart[0xff9*2], ReadWord(&pvc_cart[0xff9*2]) & 0x7fff);
    set_68000_io(0,0x200000,0x2fffff, NULL, &ROM[bankaddress+0x100000]);
}

static void pvc_prot_w(UINT32 offset, UINT16 data) {
    offset &= 0x1fff;
    WriteWord(&pvc_cart[offset],data);
    offset /= 2;
    if (offset == 0xff0)
	pvc_write_unpack_color();
    else if (offset >= 0xff4 && offset <= 0xff5)
	pvc_write_pack_color();
    else if (offset >= 0xff8)
	pvc_write_bankswitch();
}

static void pvc_prot_wb(UINT32 offset, UINT8 data) {
    offset &= 0x1fff;
    pvc_cart[offset ^ 1] = data;
    offset /= 2;
    if (offset == 0xff0)
	pvc_write_unpack_color();
    else if (offset >= 0xff4 && offset <= 0xff5)
	pvc_write_pack_color();
    else if (offset >= 0xff8)
	pvc_write_bankswitch();
}

static int init_pvc() {
    if (!(pvc_cart = AllocateMem(0x2000))) return 0;
    AddSaveData_ext("pvc",pvc_cart,0x2000);
    AddReadBW(0x2fe000,0x2fffff,NULL, pvc_cart);
    AddWriteWord(0x2fe000,0x2fffff,pvc_prot_w, NULL);
    AddWriteByte(0x2fe000,0x2fffff,pvc_prot_wb, NULL);
    return 1;
}

static void svc_px_decrypt() {
    const UINT8 xor1[ 0x20 ] = { 0x3b, 0x6a, 0xf7, 0xb7, 0xe8, 0xa9, 0x20, 0x99, 0x9f, 0x39, 0x34, 0x0c, 0xc3, 0x9a, 0xa5, 0xc8, 0xb8, 0x18, 0xce, 0x56, 0x94, 0x44, 0xe3, 0x7a, 0xf7, 0xdd, 0x42, 0xf0, 0x18, 0x60, 0x92, 0x9f };
    const UINT8 xor2[ 0x20 ] = { 0x69, 0x0b, 0x60, 0xd6, 0x4f, 0x01, 0x40, 0x1a, 0x9f, 0x0b, 0xf0, 0x75, 0x58, 0x0e, 0x60, 0xb4, 0x14, 0x04, 0x20, 0xe4, 0xb9, 0x0d, 0x10, 0x89, 0xeb, 0x07, 0x30, 0x90, 0x50, 0x0e, 0x20, 0x26 };
    int i;
    int ofst;
    int rom_size = 0x800000;
    UINT8 *rom = ROM;
    UINT8 *buf = AllocateMem( rom_size );

    for( i = 0; i < 0x100000; i++ )
    {
	rom[ i ] ^= xor1[ (BYTE_XOR_LE(i) % 0x20) ];
    }
    for( i = 0x100000; i < 0x800000; i++ )
    {
	rom[ i ] ^= xor2[ (BYTE_XOR_LE(i) % 0x20) ];
    }

    for( i = 0x100000; i < 0x0800000; i += 4 )
    {
	UINT16 rom16;
	rom16 = rom[BYTE_XOR_LE(i+1)] | rom[BYTE_XOR_LE(i+2)]<<8;
	rom16 = BITSWAP16( rom16, 15, 14, 13, 12, 10, 11, 8, 9, 6, 7, 4, 5, 3, 2, 1, 0 );
	rom[BYTE_XOR_LE(i+1)] = rom16&0xff;
	rom[BYTE_XOR_LE(i+2)] = rom16>>8;
    }
    memcpy( buf, rom, rom_size );
    for( i = 0; i < 0x0100000 / 0x10000; i++ )
    {
	ofst = (i & 0xf0) + BITSWAP8( (i & 0x0f), 7, 6, 5, 4, 2, 3, 0, 1 );
	memcpy( &rom[ i * 0x10000 ], &buf[ ofst * 0x10000 ], 0x10000 );
    }
    for( i = 0x100000; i < 0x800000; i += 0x100 )
    {
	ofst = (i & 0xf000ff) + ((i & 0x000f00) ^ 0x00a00) + (BITSWAP8( ((i & 0x0ff000) >> 12), 4, 5, 6, 7, 1, 0, 3, 2 ) << 12);
	memcpy( &rom[ i ], &buf[ ofst ], 0x100 );
    }
    memcpy( buf, rom, rom_size );
    memcpy( &rom[ 0x100000 ], &buf[ 0x700000 ], 0x100000 );
    memcpy( &rom[ 0x200000 ], &buf[ 0x100000 ], 0x600000 );
    FreeMem( buf );
}

static void kof2003_decrypt_68k() {
    const UINT8 xor1[0x20] = { 0x3b, 0x6a, 0xf7, 0xb7, 0xe8, 0xa9, 0x20, 0x99, 0x9f, 0x39, 0x34, 0x0c, 0xc3, 0x9a, 0xa5, 0xc8, 0xb8, 0x18, 0xce, 0x56, 0x94, 0x44, 0xe3, 0x7a, 0xf7, 0xdd, 0x42, 0xf0, 0x18, 0x60, 0x92, 0x9f };
    const UINT8 xor2[0x20] = { 0x2f, 0x02, 0x60, 0xbb, 0x77, 0x01, 0x30, 0x08, 0xd8, 0x01, 0xa0, 0xdf, 0x37, 0x0a, 0xf0, 0x65, 0x28, 0x03, 0xd0, 0x23, 0xd3, 0x03, 0x70, 0x42, 0xbb, 0x06, 0xf0, 0x28, 0xba, 0x0f, 0xf0, 0x7a };
    int i;
    int ofst;
    int rom_size = 0x900000;
    UINT8 *rom = ROM;
    UINT8 *buf = AllocateMem( rom_size );

    for (i = 0; i < 0x100000; i++)
    {
	rom[ 0x800000 + i ] ^= rom[ 0x100002 | i ];
    }
    for( i = 0; i < 0x100000; i++)
    {
	rom[ i ] ^= xor1[ (BYTE_XOR_LE(i) % 0x20) ];
    }
    for( i = 0x100000; i < 0x800000; i++)
    {
	rom[ i ] ^= xor2[ (BYTE_XOR_LE(i) % 0x20) ];
    }
    for( i = 0x100000; i < 0x800000; i += 4)
    {
	UINT16 rom16;
	rom16 = rom[BYTE_XOR_LE(i+1)] | rom[BYTE_XOR_LE(i+2)]<<8;
	rom16 = BITSWAP16( rom16, 15, 14, 13, 12, 5, 4, 7, 6, 9, 8, 11, 10, 3, 2, 1, 0 );
	rom[BYTE_XOR_LE(i+1)] = rom16&0xff;
	rom[BYTE_XOR_LE(i+2)] = rom16>>8;
    }
    for( i = 0; i < 0x0100000 / 0x10000; i++ )
    {
	ofst = (i & 0xf0) + BITSWAP8((i & 0x0f), 7, 6, 5, 4, 0, 1, 2, 3);
	memcpy( &buf[ i * 0x10000 ], &rom[ ofst * 0x10000 ], 0x10000 );
    }
    for( i = 0x100000; i < 0x900000; i += 0x100)
    {
	ofst = (i & 0xf000ff) + ((i & 0x000f00) ^ 0x00800) + (BITSWAP8( ((i & 0x0ff000) >> 12), 4, 5, 6, 7, 1, 0, 3, 2 ) << 12);
	memcpy( &buf[ i ], &rom[ ofst ], 0x100 );
    }
    memcpy (&rom[0x000000], &buf[0x000000], 0x100000);
    memcpy (&rom[0x100000], &buf[0x800000], 0x100000);
    memcpy (&rom[0x200000], &buf[0x100000], 0x700000);
    FreeMem( buf );
}

static void kof2003h_decrypt_68k() {
    const UINT8 xor1[0x20] = { 0xc2, 0x4b, 0x74, 0xfd, 0x0b, 0x34, 0xeb, 0xd7, 0x10, 0x6d, 0xf9, 0xce, 0x5d, 0xd5, 0x61, 0x29, 0xf5, 0xbe, 0x0d, 0x82, 0x72, 0x45, 0x0f, 0x24, 0xb3, 0x34, 0x1b, 0x99, 0xea, 0x09, 0xf3, 0x03 };
    const UINT8 xor2[0x20] = { 0x2b, 0x09, 0xd0, 0x7f, 0x51, 0x0b, 0x10, 0x4c, 0x5b, 0x07, 0x70, 0x9d, 0x3e, 0x0b, 0xb0, 0xb6, 0x54, 0x09, 0xe0, 0xcc, 0x3d, 0x0d, 0x80, 0x99, 0x87, 0x03, 0x90, 0x82, 0xfe, 0x04, 0x20, 0x18 };
    int i;
    int ofst;
    int rom_size = 0x900000;
    UINT8 *rom = ROM;
    UINT8 *buf = AllocateMem( rom_size );

    for (i = 0; i < 0x100000; i++)
    {
	rom[ 0x800000 + i ] ^= rom[ 0x100002 | i ];
    }
    for( i = 0; i < 0x100000; i++)
    {
	rom[ i ] ^= xor1[ (BYTE_XOR_LE(i) % 0x20) ];
    }
    for( i = 0x100000; i < 0x800000; i++)
    {
	rom[ i ] ^= xor2[ (BYTE_XOR_LE(i) % 0x20) ];
    }
    for( i = 0x100000; i < 0x800000; i += 4)
    {
	UINT16 rom16;
	rom16 = rom[BYTE_XOR_LE(i+1)] | rom[BYTE_XOR_LE(i+2)]<<8;
	rom16 = BITSWAP16( rom16, 15, 14, 13, 12, 10, 11, 8, 9, 6, 7, 4, 5, 3, 2, 1, 0 );
	rom[BYTE_XOR_LE(i+1)] = rom16&0xff;
	rom[BYTE_XOR_LE(i+2)] = rom16>>8;
    }
    for( i = 0; i < 0x0100000 / 0x10000; i++ )
    {
	ofst = (i & 0xf0) + BITSWAP8((i & 0x0f), 7, 6, 5, 4, 1, 0, 3, 2);
	memcpy( &buf[ i * 0x10000 ], &rom[ ofst * 0x10000 ], 0x10000 );
    }
    for( i = 0x100000; i < 0x900000; i += 0x100)
    {
	ofst = (i & 0xf000ff) + ((i & 0x000f00) ^ 0x00400) + (BITSWAP8( ((i & 0x0ff000) >> 12), 6, 7, 4, 5, 0, 1, 2, 3 ) << 12);
	memcpy( &buf[ i ], &rom[ ofst ], 0x100 );
    }
    memcpy (&rom[0x000000], &buf[0x000000], 0x100000);
    memcpy (&rom[0x100000], &buf[0x800000], 0x100000);
    memcpy (&rom[0x200000], &buf[0x100000], 0x700000);
    FreeMem( buf );
}

static void samsh5sp_decrypt_68k() {
    	int i;
	const int sec[]={0x000000,0x080000,0x500000,0x480000,0x600000,0x580000,0x700000,0x280000,0x100000,0x680000,0x400000,0x780000,0x200000,0x380000,0x300000,0x180000};
	UINT8 *src = ROM;
	UINT8 *dst = AllocateMem( 0x800000);

	memcpy( dst, src, 0x800000 );
	for( i=0; i<16; ++i )
	{
		memcpy( src+i*0x80000, dst+sec[i], 0x80000 );
	}
	FreeMem( dst);
}

static UINT16 mirror_ram_rw(UINT32 offset) {
    return ReadWord(&RAM[offset & 0xffff]);
}

static UINT8 mirror_ram_rb(UINT32 offset) {
    return RAM[(offset & 0xffff) ^ 1];
}

static void mirror_ram_ww(UINT32 offset, UINT16 data) {
    WriteWord(&RAM[offset & 0xffff],data);
}

static void mirror_ram_wb(UINT32 offset, UINT8 data) {
    RAM[(offset & 0xffff) ^ 1] = data;
}

static void lans2004_decrypt_68k() {
    /* Descrambling P ROMs - Thanks to Razoola for the info */
    int i;
    UINT8 *src = ROM;
    UINT16 *rom = (UINT16*)ROM;
    UINT8 *dst = AllocateMem( 0x600000);

    {
	static const int sec[] = { 0x3, 0x8, 0x7, 0xC, 0x1, 0xA, 0x6, 0xD };

	for (i = 0; i < 8; i++)
	    memcpy (dst + i * 0x20000, src + sec[i] * 0x20000, 0x20000);

	memcpy (dst + 0x0BBB00, src + 0x045B00, 0x001710);
	memcpy (dst + 0x02FFF0, src + 0x1A92BE, 0x000010);
	memcpy (dst + 0x100000, src + 0x200000, 0x400000);
	memcpy (src, dst, 0x600000);
	FreeMem( dst);
    }

    for (i = 0xBBB00/2; i < 0xBE000/2; i++) {
	if ((((rom[i]&0xFFBF)==0x4EB9) || ((rom[i]&0xFFBF)==0x43B9)) && (rom[i+1]==0x0000)) {
	    rom[i + 1] = 0x000B;
	    rom[i + 2] += 0x6000;
	}
    }

    /* Patched by protection chip (Altera) ? */
    rom[0x2D15C/2] = 0x000B;
    rom[0x2D15E/2] = 0xBB00;
    rom[0x2D1E4/2] = 0x6002;
    rom[0x2EA7E/2] = 0x6002;
    rom[0xBBCD0/2] = 0x6002;
    rom[0xBBDF2/2] = 0x6002;
    rom[0xBBE42/2] = 0x6002;
}

static void lans2004_vx_decrypt()
{
	int i;
	UINT8 *rom = REG(SMP1);
	for (i = 0; i < 0xA00000; i++)
		rom[i] = BITSWAP8(rom[i], 0, 1, 5, 4, 3, 2, 6, 7);
}

static UINT16 neogeo_unmapped_r(UINT32 offset) {
    int pc = s68000_pc & 0xffffff;
    // apparently used at least by magical drop 2, just after entering a name
    // exactly when the game asks if you want to save !
    // So the mame guys say that this hardware returns for unmapped memory
    // what is on the data bus at this present moment, which is usually the
    // next instruction
    return ReadWord(&ROM[pc]);
}

void load_neocd() {
    if (is_current_game("ghostlop"))
	end_screen = 240+START_SCREEN; // don't know why...
    else
	end_screen = 224+START_SCREEN;
    if (is_neocd())
	size_mcard = 0x2000;
    else {
#ifdef NEOGEO_MCARD_16BITS
	size_mcard = 0x20000;
#else
	size_mcard = 0x800;
#endif
    }
    if (!(neogeo_memorycard = AllocateMem(size_mcard))) return;
    memset(neogeo_memorycard,0,size_mcard);
    raster_frame = 0;
    do_not_stop = 0;
#ifndef RAINE_DOS
    register_driver_emu_keys(list_emu,sizeof(list_emu)/sizeof(list_emu[0]));
#endif
    layer_id_data[0] = add_layer_info(gettext("Fixed layer"));
    layer_id_data[1] = add_layer_info(gettext("Sprites layer"));
    neocd_video.screen_x = 304;
    offx = 16-8;
    maxx = 320-8;
    desired_68k_speed = CPU_FRAME_MHz(32,60);
    upload_type = 0xff;
    memcard_write = 0;
    setup_z80_frame(CPU_Z80_0,CPU_FRAME_MHz(4,60));
    setup_neocd_bios();
    set_colour_mapper(&col_Map_15bit_xRGBRRRRGGGGBBBB);
    fixed_layer_bank_type = 0;
    if (is_neocd()) {
	fps = 59.185606; // As reported in the forum, see http://rainemu.swishparty.co.uk/msgboard/yabbse/index.php?topic=1299.msg5496#msg5496
	// the fps info is kept here for neocd because it doesn't use the
	// standard load_game_proc so it ignores the fps field from video_info
	print_debug("loading neocd game\n");
	use_music = 1;
	current_game->long_name = _("No game loaded yet");
	current_game->main_name = "neocd";
	RAMSize = 0x200000 + // main ram
	    0x010000 + // z80 ram
	    0x020000 + // video ram
	    0x2000*2; // palette (2 banks)
	vbl = 2;
	hbl = 1;
	init_cdda();
	init_load_type();
	clear_file_cache();
	if (get_cd_load_type() < 0) {
	    load_error = LOAD_FATAL_ERROR;
	    ClearDefault();
	    current_game = NULL;
	    return; // error in init_load_type then...
	}
	if(!(RAM=AllocateMem(RAMSize))) return;
	memset(RAM,0,RAMSize); // be sure to start with a clean ram...
	// Starting with a clean ram here is especially important if we loaded
	// a neogeo game just before, for example for the default params of the
	// uploads.
	// This should be reviewed one of these days, it doesn't pass the test
	// mode tests for now !
	if(!(GFX=AllocateMem(0x800000))) return; // sprites data, not ram (unpacked)
	if(!(neogeo_fix_memory=AllocateMem(0x20000))) return;
	if(!(native_fix=AllocateMem(32))) return;
	if(!(video_fix_usage=AllocateMem(4096))) return; // 0x20000/32 (packed)
	nb_sprites = 0x8000;
	if(!(video_spr_usage=AllocateMem(nb_sprites))) return;
	if(!(PCMROM=AllocateMem(0x100000))) return;
	tile_list_count = 2;
	Z80ROM = &RAM[0x200000];
	neogeo_vidram = (UINT16*)(RAM + 0x210000);
	RAM_PAL = RAM + 0x230000;
	AddZ80AROMBase(Z80ROM, 0x0038, 0x0066);
	AddZ80ARW(0x0000, 0xffff, NULL, Z80ROM);
	memset(video_fix_usage,0,4096);
	memset(video_spr_usage,0,0x8000);
	sprites_mask = 0x7fff;
    } else {
	print_debug("loading neogeo game %s\n",current_game->main_name);
	Z80Has16bitsPorts = 1;
	RAMSize = 0x10000 + // main ram
	    0x10000 + // z80 ram
	    // The whole rom for the z80 because some games jump in ram !!!
	    // See legendos for an example !
	    0x020000 + // video ram
	    0x2000*2; // palette (2 banks)
	vbl = 1;
	hbl = 2;
	if(!(saveram.ram=AllocateMem(0x10000))) return; // not to be saved with the ram
	saveram.unlock = 0;
	restore_memcard();
	if(!(RAM=AllocateMem(RAMSize))) return;
	if (load_region[REGION_FIXED]) {
	    if(!(video_fix_usage=AllocateMem(get_region_size(REGION_FIXED)/32))) return; // 0x20000/32 (packed)
	} else
	    if(!(video_fix_usage=AllocateMem(0x20000/32))) return; // 0x20000/32 (packed)
	if(!(bios_fix_usage=AllocateMem(4096))) return; // 0x20000/32 (packed)
	if (!load_region[REGION_SPRITES]) {
	    load_region[REGION_SPRITES] = AllocateMem(0x100000);
	    memset(load_region[REGION_SPRITES],0xff,0x100000);
	    set_region_size(REGION_SPRITES,0x100000);
	}
	if (!load_region[REGION_SMP1]) {
	    if(!(PCMROM=AllocateMem(0x10000))) return;
	    memset(PCMROM,0xff,0x10000);
	    YM2610SetBuffers(PCMROM, PCMROM, 0x10000, 0x10000);
	}
	int size = get_region_size(REGION_SPRITES)*2;
	nb_sprites = get_region_size(REGION_SPRITES)/0x80; // packed 16x16
	sprites_mask = get_mask(nb_sprites);
	if ((sprites_mask + 1)*256 > size) size = (sprites_mask+1)*256;
	int size_fixed = (load_region[REGION_FIXED] ? get_region_size(REGION_FIXED) : 4096*32); // packed 8x8 x 4096
	if (size < size_fixed) size = size_fixed;
	nb_sprites = size/0x100;
	if (is_current_game("kof97oro")) {
	    // 1) order of the bytes altered !
	    load_message(_("kof97oro decrypt rom..."));
	    UINT16 *tmp = AllocateMem(0x500000);
	    int i;
	    for (i=0; i<0x500000/2; i++)
		tmp[i] = ReadWord(&ROM[(i ^ 0x7ffef)*2]);
	    memcpy(ROM,tmp,0x500000);
	    FreeMem(tmp);
	    load_message(_("kof97oro decrypt fix..."));
	    neogeo_bootleg_sx_decrypt(1);
	    load_message(_("kof97oro decrypt sprites..."));
	    neogeo_bootleg_cx_decrypt();
	} else if (is_current_game("zupapa")) {
	    fixed_layer_bank_type = 1;
	    kof99_neogeo_gfx_decrypt(0xbd);
	} else if (is_current_game("bangbead")) {
	    kof99_neogeo_gfx_decrypt(0xf8);
	} else if (is_current_game("nitd")) {
	    kof99_neogeo_gfx_decrypt(0xff);
	} else if (is_current_game("sengoku3")) {
	    kof99_neogeo_gfx_decrypt(0xfe);
	} else if (is_current_game("mslug3") || is_current_game("mslug3h")) {
	    fixed_layer_bank_type = 1;
	    kof99_neogeo_gfx_decrypt(0xad);
	} else if (is_current_game("preisle2")) {
	    fixed_layer_bank_type = 1;
	    kof99_neogeo_gfx_decrypt(0x9f);
	} else if (is_current_game("irrmaze"))
	    GameMouse = 1;
	else if (is_current_game("popbounc"))
	    GameMouse = 2; // mouse type 2
	else if (is_current_game("kof98"))
	    kof98_decrypt_68k();
	else if (is_current_game("kof99") || is_current_game("kof99h")) {
	    fixed_layer_bank_type = 1;
	    kof99_neogeo_gfx_decrypt(0x00);
	} else if (is_current_game("kof2000")) {
	    fixed_layer_bank_type = 2;
	    kof2000_neogeo_gfx_decrypt(0x00);
	    neogeo_cmc50_m1_decrypt();
	} else if (is_current_game("kof2000ps2")) {
	    fixed_layer_bank_type = 2;
	    neogeo_sfix_decrypt();
	    neogeo_cmc50_m1_decrypt();
	} else if (is_current_game("kof2002")) {
	    neogeo_cmc50_m1_decrypt();
	    kof2000_neogeo_gfx_decrypt(0xec);
	} else if (is_current_game("matrim")) {
	    fixed_layer_bank_type = 2;
	    neogeo_cmc50_m1_decrypt();
	    kof2000_neogeo_gfx_decrypt(0x6a);
	} else if (is_current_game("pnyaa")) {
	    neo_pcm2_snk_1999(4);
	    neogeo_cmc50_m1_decrypt();
	    kof2000_neogeo_gfx_decrypt(0x2e);
	} else if (is_current_game("mslug5") || is_current_game("mslug5h")) {
	    neo_pcm2_swap(2);
	    neogeo_cmc50_m1_decrypt();
	    kof2000_neogeo_gfx_decrypt(0x19);
	} else if (is_current_game("svc")) {
	    neo_pcm2_swap(3);
	    fixed_layer_bank_type = 2;
	    neogeo_cmc50_m1_decrypt();
	    kof2000_neogeo_gfx_decrypt(0x57);
	} else if (is_current_game("samsho5") || is_current_game("samsho5h")) {
	    neo_pcm2_swap(4);
	    neogeo_cmc50_m1_decrypt();
	    kof2000_neogeo_gfx_decrypt(0x0f);
	} else if (is_current_game("kof2003") || is_current_game("kof2003h")) {
	    neo_pcm2_swap(5);
	    fixed_layer_bank_type = 2;
	    neogeo_cmc50_m1_decrypt();
	    kof2000_neogeo_gfx_decrypt(0x9d);
	} else if (is_current_game("samsh5sp") || is_current_game("samsh5sph")) {
	    neo_pcm2_swap(6);
	    neogeo_cmc50_m1_decrypt();
	    kof2000_neogeo_gfx_decrypt(0x0d);
	} else if (is_current_game("kof2001") || is_current_game("kof2001h")) {
	    fixed_layer_bank_type = 1;
	    kof2000_neogeo_gfx_decrypt(0x1e);
	    neogeo_cmc50_m1_decrypt();
	} else if (is_current_game("mslug4") || is_current_game("mslug4h")) {
	    fixed_layer_bank_type = 1;
	    neogeo_cmc50_m1_decrypt();
	    kof2000_neogeo_gfx_decrypt(0x31);
	    neo_pcm2_snk_1999(8);
	} else if (is_current_game("rotd")) {
	    neogeo_cmc50_m1_decrypt();
	    kof2000_neogeo_gfx_decrypt(0x3f);
	    neo_pcm2_snk_1999(16);
	} else if (is_current_game("ganryu")) {
	    fixed_layer_bank_type = 1;
	    kof99_neogeo_gfx_decrypt(0x07);
	} else if (is_current_game("garou") || is_current_game("garouh")) {
	    fixed_layer_bank_type = 1;
	    kof99_neogeo_gfx_decrypt(0x06);
	} else if (is_current_game("s1945p")) {
	    fixed_layer_bank_type = 1;
	    kof99_neogeo_gfx_decrypt(0x05);
	} else if (is_current_game("lans2004")) {
	    neogeo_bootleg_sx_decrypt(1);
	    neogeo_bootleg_cx_decrypt();
	}

	UINT8 *tmp = AllocateMem(size);
	if (load_region[REGION_FIXED]) {
	    memcpy(tmp,load_region[REGION_FIXED],size_fixed);
	    fix_conv(tmp, load_region[REGION_FIXED], size_fixed, video_fix_usage);
	    neogeo_fix_memory = load_region[REGION_FIXED];
	} else
	    neogeo_fix_memory = NULL;
	memcpy(tmp,load_region[REGION_FIXEDBIOS],0x20000);
	fix_conv(tmp, load_region[REGION_FIXEDBIOS], 0x20000, bios_fix_usage);

	if(!(video_spr_usage=AllocateMem(nb_sprites))) return;
	load_message(_("Sprites conversion..."));
	spr_conv(load_region[REGION_SPRITES],tmp,get_region_size(REGION_SPRITES),video_spr_usage);
	GFX = tmp;
	FreeMem(load_region[REGION_SPRITES]);
	set_region_size(REGION_SPRITES,size);
	load_region[REGION_SPRITES] = GFX;
	if (size > nb_sprites*256) {
	    memset(&GFX[nb_sprites*256],0,size-nb_sprites*256);
	}
	tile_list_count = 3;

	Z80ROM = RAM + 0x10000;
	memcpy(Z80ROM, load_region[REGION_CPU2], 0x10000);
	neogeo_vidram = (UINT16*)(RAM + 0x20000);
	RAM_PAL = RAM + 0x40000;
	AddZ80AROMBase(Z80ROM, 0x0038, 0x0066);
	/* If these were data banks, then it would be the map to use...
	AddZ80ARead(0, 0x7fff, NULL, Z80ROM);
	AddZ80ARead(0x8000, 0xbfff, NULL, NULL); // data bank 3
	AddZ80ARead(0xc000, 0xdfff, NULL, NULL); // data bank 2
	AddZ80ARead(0xe000, 0xefff, NULL, NULL); // data bank 1
	AddZ80ARead(0xf000, 0xf7ff, NULL, NULL); // data bank 0 */
	AddZ80ARead(0, 0xf7ff, NULL, Z80ROM);
	// legendos jumps to fffd !!! (from c5c after writing result code)
	AddZ80ARW(0xf800, 0xffff, NULL, Z80ROM + 0xf800);

	z80_set_audio_bank(0,0x1e);
	z80_set_audio_bank(1,0x0e);
	z80_set_audio_bank(2,0x06);
	z80_set_audio_bank(3,0x02);
	if (!ROM) {
	    printf("no rom, using bios\n");
	    ROM = load_region[REGION_MAINBIOS];
	} else
	    ByteSwap(ROM,get_region_size(REGION_ROM1));
	if (is_current_game("ridhero"))
	    // A strange protection for aes only, it's probably easier to patch
	    WriteWord(&ROM[0xabba],0x6000);
	else if (is_current_game("ridheroh"))
	    WriteWord(&ROM[0xabf0],0x6000);
#ifndef RAINE_DOS
	init_debug_dips();
#endif
    }

    // manual init of the layers (for the sprites viewer)
    tile_list[0].width = tile_list[0].height = 8;
    tile_list[0].count = 4096;
    tile_list[0].type = TILE_TYPE_8x8; tile_list[0].rotate = 0;
    tile_list[0].data = neogeo_fix_memory;
    tile_list[0].mask = video_fix_usage;

    tile_list[1].width = tile_list[1].height = 16;
    // The type doesn't make any sense, it's some old code written too fast and
    // never fixed until now !
    tile_list[1].type = TILE_TYPE_16x16; tile_list[1].rotate = 0;
    tile_list[1].count = nb_sprites;
    tile_list[1].mask = video_spr_usage;
    tile_list[1].data = GFX;
    if (!is_neocd()) {
	tile_list[2] = tile_list[0];
	tile_list[2].mask = bios_fix_usage;
	tile_list[2].data = load_region[REGION_FIXEDBIOS];
    }

    memset(neogeo_vidram,0,0x20000);

    InitPaletteMap(RAM_PAL,0x100,0x10,0x8000);

    AddZ80AWritePort(0, 0xffff, write_port, NULL);

    AddZ80AReadPort(0, 0xffff, read_port, NULL);
    AddZ80ARead(0,0xffff, DefBadReadZ80, NULL);
    AddZ80AWrite(0,0xffff, DefBadWriteZ80, NULL);
    AddZ80AReadPort(0,0xffff, DefBadReadPort, NULL);
    AddZ80AWritePort(0,0xffff, DefBadWritePort, NULL);
    AddZ80AInit();

    if (is_neocd()) {
	AddWriteByte(0x10f6f6, 0x10f6f6, cdda_cmd, NULL);
	AddWriteByte(0x10F651, 0x10F651, test_end_loading, NULL);
	AddMemFetch(0, 0x200000, RAM);
	AddMemFetch(0xc00000, 0xc7ffff, neocd_bios - 0xc00000);
	AddRWBW(0, 0x200000, NULL, RAM);
	AddReadBW(0xc00000, 0xc7ffff, NULL,neocd_bios);
    } else {
	if (get_region_size(REGION_CPU1) > 0x100000) {
	    AddMemFetch(0, 0xfffff, ROM);
	    AddReadBW(0,0xfffff, NULL, ROM);
	    neogeo_rng = 0x2345;
	    prot = 0x9a37;
	    AddSaveData_ext("neogeo_rng",(UINT8*)&neogeo_rng, sizeof(neogeo_rng));
	    if (is_current_game("kof99") || is_current_game("kof99h")) {
		// The 68k decode must be done after the byteswap, lots of
		// bitswap to decrypt this...
		kof99_decrypt_68k();
		AddReadWord(0x2fe446,0x2fe447, NULL, (UINT8*)&prot);
		AddReadWord(0x2ffff8,0x2ffffb, read_rng, NULL);
	    } else if (is_current_game("lans2004")) {
		lans2004_decrypt_68k();
		lans2004_vx_decrypt();
	    } else if (is_current_game("kof2000")) {
		kof2000_decrypt_68k();
		AddWriteWord(0x2fffec, 0x2fffed, kof2000_bankswitch_w, NULL);
		AddReadWord(0x2fe446,0x2fe447, NULL, (UINT8*)&prot);
		AddReadWord(0x2fffd8,0x2fffdb, read_rng, NULL);
	    } else if (is_current_game("kof2002")) {
		kof2002_decrypt_68k();
		neo_pcm2_swap(0);
	    } else if (is_current_game("mslug5") || is_current_game("mslug5h")) {
		mslug5_decrypt_68k();
		if (!init_pvc()) return;
	    } else if (is_current_game("svc")) {
		svc_px_decrypt();
		if (!init_pvc()) return;
	    } else if (is_current_game("samsho5") || is_current_game("samsho5h")) {
		samsho5_decrypt_68k();
	    } else if (is_current_game("kof2003")) {
		kof2003_decrypt_68k();
		if (!init_pvc()) return;
	    } else if (is_current_game("samsh5sp") || is_current_game("samsh5sph")) {
		samsh5sp_decrypt_68k();
	    } else if (is_current_game("kof2003h")) {
		kof2003h_decrypt_68k();
		if (!init_pvc()) return;
	    } else if (is_current_game("matrim")) {
		matrim_decrypt_68k();
		neo_pcm2_swap(1);
	    } else if (is_current_game("mslugx")) {
		AddSaveData(SAVE_USER_2, (UINT8*)&mslugx_command, sizeof(mslugx_command));
		AddSaveData(SAVE_USER_3, (UINT8*)&mslugx_counter, sizeof(mslugx_counter));
		AddReadWord(0x2fffe0, 0x2fffef, mslugx_prot_r, NULL);
		AddWriteWord(0x2fffe0, 0x2fffef, mslugx_prot_w, NULL);
	    } else if (is_current_game("mslug3")) {
		mslug3_decrypt_68k();
		AddWriteWord(0x2fffe4, 0x2fffe5, mslug3_bankswitch_w, NULL);
		AddReadWord(0x2fe446,0x2fe447, NULL, (UINT8*)&prot);
	    } else if (is_current_game("garou") || is_current_game("garouh")) {
		if (is_current_game("garou"))
		    garou_decrypt_68k();
		else
		    garouh_decrypt_68k();
		AddReadWord(0x2fe446,0x2fe447, NULL, (UINT8*)&prot);
		AddReadWord(0x2fffcc,0x2fffcd, read_rng, NULL);
		AddReadWord(0x2ffff0,0x2ffff1, read_rng, NULL);
	    }

	    AddMemFetch(0x200000, 0x2fffff, ROM+0x100000-0x200000);
	    AddReadBW(0x200000, 0x2fffff, NULL, ROM+0x100000);
	    bank_68k = 0;
	} else {
	    int size = get_region_size(REGION_ROM1);
	    if (!size) size = get_region_size(REGION_MAINBIOS);
	    AddMemFetch(0, size-1, ROM);
	    AddReadBW(0,size-1, NULL, ROM);
	    // AddMemFetch(0x200000, 0x200000+size-1, ROM-0x200000);
	    // AddReadBW(0x200000,0x200000+size-1, NULL, ROM);
	}
	AddRWBW(0x100000, 0x10ffff, NULL, RAM);
	// At least trally executes some code in ram when you start a game
	AddMemFetch(0x100000, 0x10ffff, RAM-0x100000);
	Add68000Code(0, 0xc00000, REGION_MAINBIOS);
	// The mirror bios seems to be tested by kof97, but it doesn't
	// seem to do anything with the result anyway.
	// I keep the code, just in case...
	AddReadByte(0xc20000,0xcfffff, read_bios_b, NULL);
	AddReadWord(0xc20000,0xcfffff, read_bios_w, NULL);
	memcpy(game_vectors, ROM, 0x80);
	// memcpy(ROM,neocd_bios,0x80);
	game_vectors_set = 1; // For now we are on rom
    }

    AddReadBW(0x300000, 0x300001, NULL, &input_buffer[0]);
    AddReadByte(0x300080, 0x300081, NULL, &input_buffer[8]);
    AddWriteByte(0x300001, 0x300001, watchdog_w, NULL);
    AddReadByte(0x320000, 0x320001, cpu_readcoin, NULL);
    AddReadBW(0x340000, 0x340000, NULL, &input_buffer[2]);
    AddReadBW(0x380000, 0x380000, NULL, &input_buffer[4]);
    AddWriteBW(0x380000, 0x39ffff, io_control_w, NULL);

    AddReadByte(0x800000, 0x82ffff, read_memorycard, NULL);
    AddReadWord(0x800000, 0x82ffff, read_memorycardw, NULL);
    AddWriteByte(0x800000, 0x82ffff, write_memcard, NULL);
    AddWriteWord(0x800000, 0x82ffff, write_memcardw, NULL);

    // No byte access supported to the LSPC (neogeo doc)
    // These 2 are mirrored, used by at least puzzledp
    AddReadWord(0x3c0000, 0x3dffff, read_videoreg, NULL);
    AddWriteWord(0x3c0000, 0x3dffff, write_videoreg, NULL);

    AddWriteByte(0x320000, 0x320001, write_sound_command, NULL);
    AddWriteWord(0x320000, 0x320000, write_sound_command_word, NULL);

    AddWriteBW(0x3a0000, 0x3a001f, system_control_w, NULL);
    /* Notes about the palette from neogeo doc :
     * should be accessed only during vbl to avoid noise on screen, by words
     * only. Well byte access can be allowed here, it can't harm */
    AddRWBW(0x400000, 0x401fff, NULL, RAM_PAL);
    AddWriteWord(0x402000, 0x4fffff, write_pal, NULL); // palette mirror !
    AddSaveData(SAVE_USER_0, (UINT8*)&palbank, sizeof(palbank));
    if (is_neocd()) {
	prepare_cdda_save(SAVE_USER_1);
	AddSaveData(SAVE_USER_2, (UINT8 *)&cdda, sizeof(cdda));
	AddSaveData(SAVE_USER_5, (UINT8*)&neocd_lp, sizeof(neocd_lp));
	AddSaveData(SAVE_USER_8, (UINT8*)&direct_fix,sizeof(direct_fix));
	prepare_cache_save();
	AddSaveData(SAVE_USER_3, (UINT8*)&z80_enabled,sizeof(int));
    } else {
	AddSaveData(SAVE_USER_1,(UINT8*)zbank,sizeof(zbank));
	AddSaveData(SAVE_USER_5,(UINT8*)&fixed_layer_source,sizeof(fixed_layer_source));
	// Place this at end of map to minmize overhead
	if (is_current_game("fatfury2")) {
	    AddReadWord(0x200000, 0x2fffff, fatfury2_prot_rw, NULL);
	    AddWriteWord(0x200000, 0x2fffff, fatfury2_prot_ww, NULL);
	    AddSaveData_ext("fatfury2_prot_data", (UINT8*)&fatfury2_prot_data, sizeof(fatfury2_prot_data));
	} else if (is_current_game("kof98"))
	    AddWriteBW(0x20aaaa, 0x20aaab, kof98_prot_w, NULL);
	else if (is_current_game("kof99") || is_current_game("kof99h")) {
	    AddWriteWord(0x2ffff0, 0x2ffff1, kof99_bankswitch_w, NULL);
	} else if (is_current_game("garou"))
	    AddWriteWord(0x2fffc0, 0x2fffc1, garou_bankswitch_w, NULL);
	else if (is_current_game("garouh"))
	    AddWriteWord(0x2fffc0, 0x2fffc1, garouh_bankswitch_w, NULL);
    }
    // I should probably put all these variables in a struct to be cleaner...
    AddSaveData(SAVE_USER_4, (UINT8*)&irq, sizeof(irq));
    AddSaveData(SAVE_USER_6, (UINT8*)&video_modulo,sizeof(video_modulo));
    AddSaveData(SAVE_USER_7, (UINT8*)&video_pointer,sizeof(video_pointer));
    AddSaveData(SAVE_USER_9, (UINT8*)&spr_disabled,sizeof(spr_disabled));
    AddSaveData(SAVE_USER_10, (UINT8*)&fix_disabled,sizeof(fix_disabled));
    AddSaveData(SAVE_USER_11, (UINT8*)&video_enabled,sizeof(video_enabled));
    AddSaveData(SAVE_USER_12, &game_vectors_set,sizeof(game_vectors_set));
    AddSaveData(SAVE_USER_13, (UINT8*)&fc,sizeof(fc));
    AddSaveData(SAVE_USER_14, (UINT8*)&neogeo_frame_counter_speed,sizeof(neogeo_frame_counter_speed));
    AddSaveData(SAVE_USER_15, (UINT8*)&pcm_bank,sizeof(pcm_bank));
    AddSaveData(SAVE_USER_16, (UINT8*)&spr_bank,sizeof(spr_bank));
    AddLoadCallback(restore_bank);
    if (!is_neocd()) {
	// is the save ram usefull ?!??? probably not with neocd...
	AddWriteByte(0xd00000, 0xdfffff, save_ram_wb, NULL);
	AddWriteWord(0xd00000, 0xdfffff, save_ram_ww, NULL);
	AddReadBW(0xd00000, 0xd0ffff, NULL, (UINT8*)saveram.ram);
	// AddReadByte(0xd00000,  0xd0ffff, read_saveram_byte,NULL);
	// AddReadWord(0xd00000,  0xd0ffff, read_saveram_word,NULL);
	if (get_region_size(REGION_CPU1) > 0x100000 && !pvc_cart) {
	    AddWriteBW(0x2ffff0, 0x2fffff, set_68k_bank, NULL);
	    AddSaveData_ext("68k_bank",&bank_68k,sizeof(bank_68k));
	}
	// Mirror ram for some games
	// The only game so far I saw using this is magdrop2, but only if speed
	// hacks are forbidden ! Which is really really weird, but anyway
	// without this mirror and without speed hacks, it crashes during the
	// 1st attract mode, before the game demo.
	// I put it completely at the end of the memory map because it seems
	// to be bogus, and so it shouldn't slow other games
	AddReadWord(0x110000, 0x1fffff, mirror_ram_rw, NULL);
	AddReadByte(0x110000, 0x1fffff, mirror_ram_rb, NULL);
	AddWriteWord(0x110000, 0x1fffff, mirror_ram_ww, NULL);
	AddWriteByte(0x110000, 0x1fffff, mirror_ram_wb, NULL);

	AddReadBW(0xe00000,0xffffff, neogeo_unmapped_r, NULL);
	init_assoc(1);
	neogeo_read_gamename();
    } else {
	AddReadBW(0xe00000,0xefffff, read_upload, NULL);
	AddWriteByte(0xe00000,0xefffff, write_upload, NULL);
	AddWriteWord(0xe00000,0xefffff, write_upload_word, NULL);

	// cdrom : there are probably some more adresses of interest in this area
	// but I found only this one so far (still missing the ones used to control
	// the cd audio from the bios when exiting from a game).
	AddReadByte(0xff011c,0xff011c,my_read_ff011c,NULL);
	AddReadBW(0xff0000, 0xffffff, read_reg, NULL);
	AddWriteByte(0xff011c, 0xff011c, NULL, &input_buffer[10-1]); // 10 !
	AddWriteWord(0xff0002, 0xff0003, load_files, NULL);
	AddWriteWord(0xff0064,0xff0071, NULL, upload_param);
	AddWriteWord(0xff007e, 0xff008f, NULL, dma_mode);
	AddSaveData_ext("upload_param",(UINT8*)&upload_param,sizeof(upload_param));
	AddSaveData_ext("upload_type",(UINT8*)&upload_type,sizeof(upload_type));
	AddSaveData_ext("dma_mode",(UINT8*)&dma_mode,sizeof(dma_mode));
	AddWriteByte(0xff0000, 0xff01a3, write_reg_b, NULL);
	// ff011c seems to be some kind of status, only bit 12 is tested but I
	// couldn't find what for, it doesn't seem to make any difference...
	// The ff0100 area seems to be related to the uploads, but there are many
	// adresses... there might be some kind of locking system, but no dma
	// apprently, it seems easier to emulate this from the ram area instead of
	// using these registers directly
    }
    AddWriteByte(0xAA0000, 0xAAffff, myStop68000, NULL);			// Trap Idle 68000
    finish_conf_68000(0);
    // There doesn't seem to be any irq3 in the neocd, irqs are very different
    // here

    init_16x16_zoom();
    set_reset_function(neogeo_hreset);
    result_code = 0;
    irq.control = 0;
}

void neocd_function(int vector) {
  // This one is called by the cdrom emulation for the loading animations
  int adr;
  if (vector < 0x200000) {
    adr = ReadLongSc(&RAM[vector]);
    if (adr > 0xe00000 || (adr & 1)) {
      adr = 0;
    }
  } else
    adr = vector;
  if (adr == 0) {
    switch (vector) {
      case 0x11c808: // setup loading screen
	adr = loading_animation_init; // 0xc0c760;
	print_debug("neocd_function default %x\n",adr);
	break;
      case 0x11c80c:
	adr = loading_animation_progress; // 0xc0c814; // load screen progress
	print_debug("neocd_function default %x\n",adr);
	break;
      default:
	fatal_error("unknown vector %x",vector);
    }
  }

  // The function executed here is something which is supposed to be called
  // by a jsr. But since we don't load the files sector/sector, it's unlikely
  // to be able to emulate this directly (maybe later).
  // Anyway for now we must setup a temporary environment to execute this
  // function...
  int pc = cpu_get_pc(CPU_68K_0);
  char buff[6];
  print_debug("neocd_function: initial pc %x, sr %x a7 %x raster_frame %d\n",pc,s68000_sr,s68000_areg[7],raster_frame);
  if (pc < 0x200000) {
    memcpy(buff,&RAM[pc],6);
    WriteWord(&RAM[pc],0x4239); // stop 68000 here
    WriteWord(&RAM[pc+2],0xaa);
    WriteWord(&RAM[pc+4],0);
  } else if (pc > 0xc00000) {
    pc -= 0xc00000;
    memcpy(buff,&neocd_bios[pc],6);
    WriteWord(&neocd_bios[pc],0x4239); // stop 68000 here
    WriteWord(&neocd_bios[pc+2],0xaa);
    WriteWord(&neocd_bios[pc+4],0);
    pc += 0xc00000;
  }
#if USE_MUSASHI < 2
  s68000GetContext(&M68000_context[0]);
  s68000context.pc = adr;
#else
  m68ki_cpu_core tmp;
  m68k_get_context(&tmp);
  REG_PC = adr;
#endif
  s68000_areg[5] = 0x108000;
  if (!s68000_areg[7]) {
      print_debug("neocd_function: setup stack ?\n");
      s68000_areg[7] = 0x10F300;
  }
  s68000_areg[7] -= 4*8*2; // ???
  int old_adr = s68000_areg[7];
  int old_val = ReadLongSc(&RAM[old_adr]);
  WriteLongSc(&RAM[s68000_areg[7]],pc);
  cpu_execute_cycles(CPU_68K_0, CPU_FRAME_MHz(32,60));
  WriteLongSc(&RAM[old_adr],old_val);
  if (s68000_pc != pc+6) {
    print_debug("*** got pc = %x instead of %x after frame 11c810:%x vector was %x\n",s68000_pc,pc+6,ReadLongSc(&RAM[0x11c810]),adr);
    /* Last blade 2, just before the 1st fight : it tries to access some data
     * for the animation just after it loaded a prg over it. Well the prg
     * seems to have bad data, so it was probably supposed to read the data
     * just before the prg overwrote it. Well I really don't see how it's
     * possible for now. The easiest solution is to ignore the error, it's
     * harmless anyway. */
    // exit(1);
  }
  if (pc < 0x200000) {
    memcpy(&RAM[pc],buff,6);
  } else if (pc > 0xc00000) {
    memcpy(&neocd_bios[pc- 0xc00000],buff,6);
  }
#if USE_MUSASHI < 2
  s68000SetContext(&M68000_context[0]);
#else
  m68k_set_context(&tmp);
#endif
}

void loading_progress_function() {
  static int frames,init_loaded;
  screen_cleared = 0;
  if (!cdrom_speed) // prevent the cdrom_speed to disappear in the middle of
    // loading !
    cdrom_speed = neocd_lp.initial_cdrom_speed;
  if (neocd_lp.file_to_load == 0) { // init
    write_reg_b(0xff0111,1); // spr_disable
    write_reg_b(0xff0115,0); // fix_disable
    write_reg_b(0xff0119,1); // video_enable
    print_debug("loading_progress_function: init, calling neocd_function\n");
    neocd_function(0x11c808);
  }

  if (neocd_lp.bytes_loaded) {
    cdrom_load_neocd();
  }
  if (neocd_lp.bytes_loaded && neocd_lp.sectors_to_load == 0) {
    // this can happen if cdrom_speed is changed in the middle of loading...
    printf("problem sectors_to_load = 0 and bytes_loaded = %d \n",neocd_lp.bytes_loaded);
    neocd_lp.sectors_to_load = 1;
  }
  if (neocd_lp.sectors_to_load <= 0) {
    switch(neocd_lp.function) {
      case 1:
	if (!neogeo_cdrom_process_ipl(&neocd_lp)) {
	  ErrorMsg("Error: Error while processing IPL.TXT.\n");
	  current_game->exec = &execute_neocd;
	  ClearDefault();
	  return;
	}
	break;
      case 2:
	neogeo_cdrom_test_files(&RAM[0x115a06],&neocd_lp);
	break;
      default:
	printf("function code unknown for loading progress %d\n",
	    neocd_lp.function);
	exit(1);
    }
    frames = 1;
    init_loaded = neocd_lp.loaded_sectors;
  }

  if (neocd_lp.sectors_to_load > 0) {
    int nb = (cdrom_speed*(150000)/60)*frames/2048; // sectors loaded at this frame
    frames++;
    if (nb >= neocd_lp.sectors_to_load) {
      nb = neocd_lp.sectors_to_load;
      neocd_lp.sectors_to_load = 0;
    }
    neocd_lp.loaded_sectors = init_loaded + nb;
    // printf("loaded sectors %d/%d section %d\n",neocd_lp.loaded_sectors,neocd_lp.total_sectors,neocd_lp.sectors_to_load);
    UINT32 progress= ((neocd_lp.loaded_sectors * 0x8000) / neocd_lp.total_sectors) << 8;
    WriteLongSc(&RAM[0x10f690],progress);
    if (progress >= 0x800000)
      WriteLongSc(&RAM[0x10f690],0x800000);
    print_debug("loading_progress_function: call2 neocd_function\n");
    neocd_function(0x11c80c);

    RAM[0x10f793^1] = 0;
    print_debug("loading_progress_function: call3 neocd_function\n");
    neocd_function(loading_animation); // 0xc0c8b2);
    if (z80_enabled && RaineSoundCard) {
      execute_z80_audio_frame();
    }
  }
}

void execute_neocd() {
    start_frame = s68000readOdometer();
    if (GameMouse == 1) {
	init1 = input_buffer[1];
	// if (controller & 0x10) input_buffer[1] = mx;
	if (controller == 1) input_buffer[1] = my;
	int dx,dy;
	GetMouseMickeys(&dx,&dy);
	mx -= dx; my -= dy;
    } else if (GameMouse == 2) {
	int dx,dy;
	GetMouseMickeys(&dx,&dy);
	mx += dx;
	if (!(input_buffer[2] & 4)) mx--;
	if (!(input_buffer[2] & 8)) mx++;
	// Here, my is mouse x for player 2
	if (!(input_buffer[4] & 4)) my--;
	if (!(input_buffer[4] & 8)) my++;
    }

    if (!is_neocd()) {
	/* watchdog is unreliable in neocd because of the loading functions
	 * mainly, and seems unused anyway.
	 * In neogeo it's used by games like kof97 : when it initialises its
	 * saveram it tries to reset using the watchdog. If that fails, it
	 * writes 1 to $100 in the saveram and considers the board is a copy
	 * and you get a protection message, which stays even if you reboot
	 * the game after that. */
	watchdog_counter--;
	if (watchdog_counter == 0) reset_game_hardware();
    }

  /* This code is still more or less experimental
   * the idea is to detect when the hblank interrupt is needed (raster_frame)
   * and to change the handling accordingly to save cycles.
   * Not sure this thing is 100% correct */
  // 7db0(a5) test� par futsal ???
  // WriteWord(&RAM[0x10fe80],0xffff);

  // lab_0432 = cd_test ???
  // printf("765 %x 7656 %x\n",RAM[0x10f765^1],RAM[0x10f656^1]);
  // printf("cd loaded %x 76b9 %x\n",RAM[0x10fec4^1],RAM[0x76b9]);
  // RAM[0x10fd97^1] = 15;

    int pc;

  screen_cleared = 0;
  start_line = START_SCREEN;
  if (raster_bitmap && bitmap_color_depth(raster_bitmap) !=
	  bitmap_color_depth(GameBitmap)) {
      destroy_bitmap(raster_bitmap);
      raster_bitmap = NULL;
  }

  if (!raster_bitmap)
      raster_bitmap = create_bitmap_ex(bitmap_color_depth(GameBitmap),
	      320,224);
  if ((irq.control & (IRQ1CTRL_ENABLE)) && !disable_irq1) {
      debug(DBG_RASTER,"raster frame irq.start %d in lines %d\n",irq.start,irq.start/0x180);


      raster_frame = 1;
      clear_screen();
      rolled = 0;
      /* This code is not perfect yet. For now :
       * ridhero works correctly.
       * ssideki2/ssideki3 have a bad display at the bottom of the screen as if
       * the hbl update starts to high and finishes too high. But if you try
       * to have the hbl starting only on visible screen then ridhero display
       * becoms unstable.
       * It's quite a headache... !
       * For now I leave it like that, ssideki2 and 3 are not perfect, but
       * ridhero is. There should be a way to have all the games happy, should
       * spend more time on this one day... ! */
      stopped_68k = 0;
      for (scanline = 0; scanline < NB_LINES; scanline++) {
	  if (scanline == 0xf0) {
	      vblank_interrupt_pending = 1;	   /* vertical blank */
	      if (irq.control & IRQ1CTRL_AUTOLOAD_VBLANK) {
		  if (irq.pos == 0xffffffff)
		      irq.start = -1000;
		  else {
		      irq.start = irq.pos;	/* ridhero gives 0x17d */
		  }
		  if (irq.start > -1000) {
		      debug(DBG_RASTER,"irq.start %d on vblank (irq.pos %x)\n",irq.start,irq.pos);
		  }
	      }
	  }
	  // Must have >= 0 and not > 0 here, or the playground just disappears
	  // in supersidekick...
	  if (irq.start >= 0 && (irq.control & IRQ1CTRL_ENABLE)) {
	      irq.start -= 0x180;
	      check_hbl();
	  }

	  if (display_position_interrupt_pending || vblank_interrupt_pending) {
	      if (stopped_68k) {
		  /* DO NOT EVER allow a speed hack in the middle of a raster
		   * frame. That is, if the speed hack is installed and hit,
		   * then it must be executed, but in this case we must make
		   * sure to loop on it for each scanline, so that any irq can
		   * be executed */
		  stopped_68k = 0;
		  s68000_pc -= 6;
		  rolled = 1;
	      }
	      update_interrupts();
	  }
	  if (!stopped_68k) {
	      /* Boost cpu frequency, that's 15 Mhz instead of 12.
	       * Well that's just for the raster frames.
	       * Without this boost, the display becomes "unstable" on some
	       * games like neo turf masters (on the green for example).
	       * It's probably a bug in the timing of instructions in
	       * starscream, this should be 12 MHz here. */
	      cpu_execute_cycles(CPU_68K_0,250000/NB_LINES);
	      if (stopped_68k && current_game->exec == &loading_progress_function)
		  /* If we are starting to load files
		   * in the middle of a raster frame
		   * exit immediately !!! */
		  break;
	  }
#ifndef RAINE_DOS
	  if (goto_debuger) {
	      break;
	  }
#endif
      }
      if (rolled && !stopped_68k) {
	  do_not_stop = 1; // the vbl will end in the next frame
	  // so we must not stop when it will end !!!
	  // Case in top golf for example
      }
  } else { // normal frame (no raster)
      // the 68k frame does not need to be sliced any longer, we
      // execute cycles on the z80 upon receiving a command !
      raster_frame = 0;
      /* I didn't think I needed to be precise on the vbl position for a normal frame, but apparently garou checks the
       * screen counter until it finds a number between 0 and 2, which is possible only if the vbl is not started at line 0 !
       * It seems requires by mslug2 too */
      int first_part = current_neo_frame * 0xf0 / NB_LINES; // f0 = start of vbl
      int rest = current_neo_frame - first_part;
      if (!stopped_68k)
	  cpu_execute_cycles(CPU_68K_0, first_part);
      if (stopped_68k && current_game->exec == &loading_progress_function) {
	  // It means we started loading things, best to exit this frame now !
	  return;
      }
      stopped_68k = 0;
      cpu_execute_cycles(CPU_68K_0, rest);
      if (allowed_speed_hacks) {
	  /* Speed hacks are searched ONLY in the normal frame because we
	   * could find places waiting for an hbl and not a vbl if using also
	   * raster frames */
	  static int not_stopped_frames;
	  if (stopped_68k) {
	      not_stopped_frames--;
	      if (not_stopped_frames < 0 && current_neo_frame < desired_68k_speed)
		  // This one is for mslug2, after a game over the game slows
		  // down again but the speed hacks are still enabled.
		  // It's the only way I found which works so far... !
		  current_neo_frame = desired_68k_speed;
	  }
	  if (!stopped_68k && frame_count++ > 60 && current_neo_frame < desired_68k_speed) {
	      pc = s68000_pc & 0xffffff;
	      UINT8 *RAM = get_userdata(CPU_68K_0,pc);

	      if (pc < 0x200000) {
		  // printf("testing speed hack... pc=%x pc: %x pc-6:%x\n",pc,ReadWord(&RAM[pc]),ReadWord(&RAM[pc-6]));
		  not_stopped_frames = 0;
#if 1
		  // The 4a2d / 67fa is used by all the kof games, the trick with this one is that the speed hack works
		  // but after that there is a jump table in ram which gets totally corruped and sends the pc out of bounds
		  // not sure if it's the jump table itself or some crc error which creates the crash, but it's shared by all
		  // the kof games, so the best is to comment this out
		  if ((ReadWord(&RAM[pc]) == 0xb06e || /* ReadWord(&RAM[pc]) == 0x4a2d || */ ReadWord(&RAM[pc]) == 0x4a28) &&
			  (ReadWord(&RAM[pc+4]) == 0x67fa ||
			   ReadWord(&RAM[pc+4]) == 0x6bfa)) {
		      apply_hack(pc,"beq/bmi");
		  } else
#endif
		      if (ReadWord(&RAM[pc]) == 0x4a39 &&
			  ReadWord(&RAM[pc+6]) == 0x6bf8) { // tst.b/bmi
		      apply_hack(pc,"tst/bmi");
		      WriteWord(&RAM[pc+6],0x4e71); // nop
		  } else if (ReadWord(&RAM[pc]) == 0x6bf8 &&
			  ReadWord(&RAM[pc-6]) == 0x4a39) {
		      apply_hack(pc-6,"tst/bmi 2");
		      WriteWord(&RAM[pc],0x4e71);
		  } else if (ReadWord(&RAM[pc]) == 0x0839 &&
			  ReadWord(&RAM[pc+8]) == 0x66f2) {
		      apply_hack(pc,"bne");
		      WriteWord(&RAM[pc+6],0x4e71); // nop
		      WriteWord(&RAM[pc+8],0x4e71); // nop
		  } else if ((ReadWord(&RAM[pc]) == 0x67f8 || ReadWord(&RAM[pc]) == 0x66f8) &&
			  (ReadWord(&RAM[pc-6]) == 0x4a79 ||
			   ReadWord(&RAM[pc-6]) == 0x4a39)) { // TST / BEQ/BNE
		      apply_hack(pc-6,"tst/beq/bne");
		      WriteWord(&RAM[pc],0x4e71); // nop
		  } else if (ReadWord(&RAM[pc]) == 0x8ad &&
			  ReadWord(&RAM[pc+6]) == 0x67f8) {
		      apply_hack(pc,"mslug2 special");
		      WriteWord(&RAM[pc+6],0x4e71);
		  }
		  else if (ReadWord(&RAM[pc]) == 0x6400) {
		      int ofs = pc+ReadWord(&RAM[pc+2])+2;
		      if (ReadWord(&RAM[ofs]) == 0x8ad &&
			      ReadWord(&RAM[ofs+6]) == 0x6700) {
			  apply_hack(ofs,"mslugx special");
			  WriteWord(&RAM[ofs+6],0x4e71);
			  WriteWord(&RAM[ofs+8],0x4e71);
		      }
		  } else if ((ReadWord(&RAM[pc]) == 0xc79) &&
			  ReadWord(&RAM[pc+8]) == 0x6600 &&
			  ReadWord(&RAM[pc+10]) == 8 &&
			  ReadWord(&RAM[pc+16]) == 0x60ee) {
		      /* This one is quite crazy, for neo drift out... */
		      apply_hack(pc,"neodriftout special");
		      WriteWord(&RAM[pc+6],0x4e71); // nop
		      WriteWord(&RAM[pc+8],0x6000); // bra
		  } else if (ReadWord(&RAM[pc]) == 0x67f2 &&
			  ReadWord(&RAM[pc-6]) == 0x4a39 && // tst.b
			  ReadWord(&RAM[pc-12]) == 0x5279) { // addq.w
		      // neo turf masters
		      apply_hack(pc-6,"neoturf master");
		      WriteWord(&RAM[pc],0x4e71); // nop
		  }
	      }
	  } else if (current_neo_frame > frame_neo && frame_count > 60) {
	      // speed hack missed again for some reason (savegames can do that)
	      if (not_stopped_frames++ >= 10)
		  current_neo_frame = frame_neo; // search again !
	  }
      } // allowed_speed_hacks
  }
  start_line -= START_SCREEN;
  if (z80_enabled /* && !irq.disable */ && RaineSoundCard) {
      /* Normally irq.disable is an optimization heree,
       * except that the 007Z bios tests the z80 communication
       * while irqs are disabled, so it's better to allow this here */
      execute_z80_audio_frame();
  }
  if (!raster_frame) {
      // Puting this before the if (allowed_speed_hacks) breaks pbobblen title screen !
      // probably some incompatibility with the speed hack ?
      // anyway keeping it here fixes it
      vblank_interrupt_pending = 1;	   /* vertical blank, after speed hacks */
      update_interrupts();
  }
  /* Add a timer tick to the pd4990a */
  pd4990a_addretrace();
  if (is_neocd()) {
      if (s68000_pc == 0xc0e602) { // start button
	  // For start, irqs are disabled, maybe it expects them to come back
	  // from the cd ?
	  Stop68000(0,0);
	  reset_game_hardware();
      } else if (load_sdips && ReadLong(&RAM[0x10fd84])) {
	  /* The soft dips must be initialised only after the bios has moved
	   * them to ram, which happens only after the 1st cpu frame once the
	   * game has been loaded, so this is the only place to do it ! */
	  load_sdips = 0;
	  char path[FILENAME_MAX];
	  snprintf(path,FILENAME_MAX,"%ssavedata" SLASH "%s.sdips", dir_cfg.exe_path, current_game->main_name);
	  load_file(path,&RAM[0x10fd84],16);
      }
  }
}

void clear_neocd() {
  save_memcard();
  FreeMem(neogeo_memorycard);
  neogeo_memorycard = NULL;
  saved_fix = 0;
  neocd_lp.function = 0; // required ONLY when changing region just before
  // loading another game !!!
  if (pvc_cart) {
      FreeMem(pvc_cart);
      pvc_cart = NULL;
  }
  if (is_neocd()) {
      save_debug("neocd.bin",neocd_bios,0x80000,1);
      save_debug("ram.bin",RAM,0x200000,1);
      save_debug("z80",Z80ROM,0x10000,0);
      init_cdda();
      char *old = current_game->main_name;
      current_game->main_name = "neocd"; // Save romsw to neocd as default
      save_game_config();
      current_game->main_name = old;
#ifdef RAINE_DEBUG
      if (debug_mode)
	  ByteSwap(neocd_bios,0x80000); // restore the bios for the next game
#endif
      free_tracks();
      clear_file_cache();
      free_iso_dir();
      free(neocd_bios);
      neocd_bios = NULL;
  } else {
      // Clear neogeo bios
      neocd_bios = NULL;
  }
  if (raster_bitmap) {
      destroy_bitmap(raster_bitmap);
      raster_bitmap = NULL;
  }
  if (saveram.ram) {
      FreeMem(saveram.ram);
      saveram.ram = NULL;
  }
}

struct GAME_MAIN game_neocd =
{
  __FILE__, /* source_file */ \
    NULL, // dirs
  NULL, // roms
  neocd_inputs,
  NULL, // dsw
  neocd_romsw,

  load_neocd,
  clear_neocd,
  &neocd_video,
  execute_neocd,
  "neocd",
  "neocd",
  "",
  COMPANY_ID_SNK,
  1998,
  sound_neocd,
  GAME_SHOOT
};


