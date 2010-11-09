/******************************************************************************/
/*                                                                            */
/*                    CADASH (C) 1988 TAITO CORPORATION                       */
/*                                                                            */
/******************************************************************************/

#include "gameinc.h"
#include "cadash.h"
#include "tc100scn.h"
#include "tc110pcr.h"
#include "tc002obj.h"
#include "tc220ioc.h"
#include "taitosnd.h"

static struct DIR_INFO cadash_dirs[] =
{
   { "cadash", },
   { NULL, },
};

static struct ROM_INFO cadash_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x000000,  0x00020000,
                  "c21-14",  0x5daf13fb,       "c21-16",  0xcbaa2e75),
  LOAD8_16(  REGION_ROM1,  0x040000,  0x00020000,
                  "c21-13",  0x6b9e0ee9,       "c21-17",  0xbf9a578a),
   {       "c21-02.9", 0x00080000, 0x205883b9, REGION_GFX1, 0x000000, LOAD_SWAP_16, },
   {       "c21-01.1", 0x00080000, 0x1ff6f39c, REGION_GFX2, 0x000000, LOAD_SWAP_16, },
   {       "c21-07.57", 0x00008000, 0xf02292bd, 0, 0, 0, },
   {       "c21-08.38", 0x00010000, 0xdca495a0, 0, 0, 0, },
   {           NULL,          0,          0, 0, 0, 0, },
};

static struct INPUT_INFO cadash_inputs[] =
{
   { KB_DEF_COIN1,        MSG_COIN1,               0x01A00E, 0x01, BIT_ACTIVE_0 },
   { KB_DEF_COIN2,        MSG_COIN2,               0x01A00E, 0x02, BIT_ACTIVE_0 },
   { KB_DEF_TILT,         MSG_TILT,                0x01A00E, 0x20, BIT_ACTIVE_0 },
   { KB_DEF_SERVICE,      MSG_SERVICE,             0x01A00E, 0x10, BIT_ACTIVE_0 },

   { KB_DEF_P1_START,     MSG_P1_START,            0x01A00E, 0x08, BIT_ACTIVE_0 },
   { KB_DEF_P1_UP,        MSG_P1_UP,               0x01A004, 0x80, BIT_ACTIVE_0 },
   { KB_DEF_P1_DOWN,      MSG_P1_DOWN,             0x01A004, 0x40, BIT_ACTIVE_0 },
   { KB_DEF_P1_LEFT,      MSG_P1_LEFT,             0x01A004, 0x20, BIT_ACTIVE_0 },
   { KB_DEF_P1_RIGHT,     MSG_P1_RIGHT,            0x01A004, 0x10, BIT_ACTIVE_0 },
   { KB_DEF_P1_B1,        MSG_P1_B1,               0x01A004, 0x08, BIT_ACTIVE_0 },
   { KB_DEF_P1_B2,        MSG_P1_B2,               0x01A004, 0x04, BIT_ACTIVE_0 },

   { KB_DEF_P2_START,     MSG_P2_START,            0x01A00E, 0x04, BIT_ACTIVE_0 },
   { KB_DEF_P2_UP,        MSG_P2_UP,               0x01A006, 0x80, BIT_ACTIVE_0 },
   { KB_DEF_P2_DOWN,      MSG_P2_DOWN,             0x01A006, 0x40, BIT_ACTIVE_0 },
   { KB_DEF_P2_LEFT,      MSG_P2_LEFT,             0x01A006, 0x20, BIT_ACTIVE_0 },
   { KB_DEF_P2_RIGHT,     MSG_P2_RIGHT,            0x01A006, 0x10, BIT_ACTIVE_0 },
   { KB_DEF_P2_B1,        MSG_P2_B1,               0x01A006, 0x08, BIT_ACTIVE_0 },
   { KB_DEF_P2_B2,        MSG_P2_B2,               0x01A006, 0x04, BIT_ACTIVE_0 },

   { 0,                   NULL,                    0,        0,    0            },
};

static struct DSW_DATA dsw_data_cadash_0[] =
{
   DSW_SCREEN( 0x02, 0x00),
   DSW_TEST_MODE( 0x00, 0x04),
   DSW_DEMO_SOUND( 0x08, 0x00),
   { MSG_COINAGE,               0x30, 0x04 },
   { MSG_1COIN_1PLAY,         0x30},
   { MSG_2COIN_1PLAY,         0x20},
   { MSG_3COIN_1PLAY,         0x10},
   { MSG_4COIN_1PLAY,         0x00},
   { "Continue Cost",         0xC0, 0x04 },
   { "No Extra",              0xC0},
   { "Plus 1 Coin",           0x80},
   { "Plus 2 Coins",          0x40},
   { "Plus 3 Coins",          0x00},
   { NULL,                    0,   },
};

static struct DSW_DATA dsw_data_cadash_1[] =
{
   { MSG_DIFFICULTY,          0x03, 0x04 },
   { MSG_NORMAL,              0x03},
   { MSG_EASY,                0x02},
   { MSG_HARD,                0x01},
   { MSG_HARDEST,             0x00},
   { "Start Time",            0x0C, 0x04 },
   { "7 Mins",                0x0C},
   { "8 Mins",                0x08},
   { "6 Mins",                0x04},
   { "5 Mins",                0x00},
   { "Extra Clear Time",      0x30, 0x04 },
   { "None",                  0x30},
   { "+1 Min",                0x20},
   { "-1 Min",                0x10},
   { "-2 Mins",               0x00},
   { "Communication",         0x40, 0x02 },
   { MSG_OFF,                 0x40},
   { MSG_ON,                  0x00},
   { "Status for Comm",       0x80, 0x02 },
   { "Master",                0x80},
   { "Slave",                 0x00},
   { NULL,                    0,   },
};

static struct DSW_INFO cadash_dsw[] =
{
   { 0x01A000, 0xFF, dsw_data_cadash_0 },
   { 0x01A002, 0xFF, dsw_data_cadash_1 },
   { 0,        0,    NULL,      },
};

static struct ROMSW_DATA romsw_data_cadash_0[] =
{
   { "Taito America",          0x02 },
   { "Taito Japan",            0x03 },
   { NULL,                     0    },
};

static struct ROMSW_INFO cadash_romsw[] =
{
   { 0x07FFFF, 0x03, romsw_data_cadash_0 },
   { 0,        0,    NULL },
};

static struct GFX_LAYOUT cadash_gfx_tiles =
{
   8,8,
   RGN_FRAC(1,1),
   4,
   {0,1,2,3},
   {STEP8(0,4)},
   {STEP8(0,8*4)},
   8*8*4
};

static struct GFX_LAYOUT cadash_gfx_object =
{
   16,16,
   RGN_FRAC(1,1),
   4,
   {0,1,2,3},
   {STEP16(0,4)},
   {STEP16(0,16*4)},
   16*16*4
};

static struct GFX_LIST cadash_gfx[] =
{
   { REGION_GFX1, &cadash_gfx_tiles,  },
   { REGION_GFX2, &cadash_gfx_object, },
   { 0,           NULL,               },
};

static struct VIDEO_INFO cadash_video =
{
   draw_cadash,
   320,
   240,
   32,
   VIDEO_ROTATE_NORMAL| VIDEO_ROTATABLE,
   cadash_gfx,
};

GAME( cadash ,
   cadash_dirs,
   cadash_roms,
   cadash_inputs,
   cadash_dsw,
   cadash_romsw,

   load_cadash,
   clear_cadash,
   &cadash_video,
   execute_cadash_frame,
   "cadash",
   "Cadash (World)",
   "カダッシュ American",
   COMPANY_ID_TAITO,
   "C21",
   1989,
   taito_ym2151_sound,
   GAME_BEAT
);

static struct DIR_INFO cadash_french_dirs[] =
{
   { "cadash_french", },
   { "cadashfr", },
   { "cadashf", },
   { ROMOF("cadash"), },
   { CLONEOF("cadash"), },
   { NULL, },
};

static struct ROM_INFO cadash_french_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x000000,  0x00020000,
                  "c21-19",  0x4d70543b,       "c21-21",  0x0e5b9950),
  LOAD8_16(  REGION_ROM1,  0x040000,  0x00020000,
                  "c21-18",  0x8a19e59b,       "c21-20",  0xb96acfd9),
   {       "c21-02.9", 0x00080000, 0x205883b9, REGION_GFX1, 0x000000, LOAD_SWAP_16, },
   {       "c21-01.1", 0x00080000, 0x1ff6f39c, REGION_GFX2, 0x000000, LOAD_SWAP_16, },
   {       "c21-07.57", 0x00008000, 0xf02292bd, 0, 0, 0, },
   {       "c21-08.38", 0x00010000, 0xdca495a0, 0, 0, 0, },
   {           NULL,          0,          0, 0, 0, 0, },
};

GAME( cadash_french ,
   cadash_french_dirs,
   cadash_french_roms,
   cadash_inputs,
   cadash_dsw,
   cadash_romsw,

   load_cadash,
   clear_cadash,
   &cadash_video,
   execute_cadash_frame,
   "cadashf",
   "Cadash (France)",
   "カダッシュ French",
   COMPANY_ID_TAITO,
   "C21",
   1989,
   taito_ym2151_sound,
   GAME_BEAT
);

static struct DIR_INFO cadash_italian_dirs[] =
{
   { "cadash_italian", },
   { "cadashit", },
   { "cadashi", },
   { ROMOF("cadash"), },
   { CLONEOF("cadash"), },
   { NULL, },
};

static struct ROM_INFO cadash_italian_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x000000,  0x00020000,
                "c21-14it",  0xd1d9e613,     "c21-16it",  0x142256ef),
  LOAD8_16(  REGION_ROM1,  0x040000,  0x00020000,
                "c21-13it",  0xc9cf6e30,     "c21-17it",  0x641fc9dd),
   {       "c21-02.9", 0x00080000, 0x205883b9, REGION_GFX1, 0x000000, LOAD_SWAP_16, },
   {       "c21-01.1", 0x00080000, 0x1ff6f39c, REGION_GFX2, 0x000000, LOAD_SWAP_16, },
   {       "c21-07.57", 0x00008000, 0xf02292bd, 0, 0, 0, },
   {       "c21-08.38", 0x00010000, 0xdca495a0, 0, 0, 0, },
   {           NULL,          0,          0, 0, 0, 0, },
};

GAME( cadash_italian ,
   cadash_italian_dirs,
   cadash_italian_roms,
   cadash_inputs,
   cadash_dsw,
   cadash_romsw,

   load_cadash,
   clear_cadash,
   &cadash_video,
   execute_cadash_frame,
   "cadashi",
   "Cadash (Italy)",
   "カダッシュ Italian",
   COMPANY_ID_TAITO,
   "C21",
   1989,
   taito_ym2151_sound,
   GAME_BEAT
);

static struct DIR_INFO cadash_japanese_dirs[] =
{
   { "cadash_japanese", },
   { "cadashj", },
   { ROMOF("cadash"), },
   { CLONEOF("cadash"), },
   { NULL, },
};

static struct ROM_INFO cadash_japanese_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x000000,  0x00020000,
             "c21-04.11",  0xcc22ebe5,  "c21-06.15",  0x26e03304),
  LOAD8_16(  REGION_ROM1,  0x040000,  0x00020000,
             "c21-03.10",  0xc54888ed,  "c21-05.14",  0x834018d2),
   {       "c21-02.9", 0x00080000, 0x205883b9, REGION_GFX1, 0x000000, LOAD_SWAP_16, },
   {       "c21-01.1", 0x00080000, 0x1ff6f39c, REGION_GFX2, 0x000000, LOAD_SWAP_16, },
   {       "c21-07.57", 0x00008000, 0xf02292bd, 0, 0, 0, },
   {       "c21-08.38", 0x00010000, 0xdca495a0, 0, 0, 0, },
   {           NULL,          0,          0, 0, 0, 0, },
};

static struct ROMSW_DATA romsw_data_cadash_japanese_0[] =
{
   { "Taito Japan (notice)",   0x01 },
   { NULL,                     0    },
};

static struct ROMSW_INFO cadash_japanese_romsw[] =
{
   { 0x07FFFF, 0x01, romsw_data_cadash_japanese_0 },
   { 0,        0,    NULL },
};

GAME( cadash_japanese ,
   cadash_japanese_dirs,
   cadash_japanese_roms,
   cadash_inputs,
   cadash_dsw,
   cadash_japanese_romsw,

   load_cadash,
   clear_cadash,
   &cadash_video,
   execute_cadash_frame,
   "cadashj",
   "Cadash (Japan)",
   "カダッシュ",
   COMPANY_ID_TAITO,
   "C21",
   1989,
   taito_ym2151_sound,
   GAME_BEAT
);

static UINT8 *RAM_VIDEO;
static UINT8 *RAM_SCROLL;
static UINT8 *RAM_INPUT;

static UINT8 *RAM_OBJECT;

void load_cadash(void)
{
   RAMSize=0x34000;
   if(!(RAM=AllocateMem(RAMSize))) return;

   /*-----[Sound Setup]-----*/

   Z80ROM=RAM+0x24000;
   if(!load_rom("c21-08.38", Z80ROM, 0x10000)) return;	// Z80 SOUND ROM

   AddTaitoYM2151(0x01AF, 0x0143, 0x10000, NULL, NULL);

   /*-----------------------*/

   memset(RAM+0x00000,0x00,0x20000);

   tc0110pcr_init(RAM+0x1C000, 1);

   set_colour_mapper(&col_map_xxxx_bbbb_gggg_rrrr);
   InitPaletteMap(RAM+0x1C000, 0x100, 0x10, 0x1000);

   RAM_VIDEO  = RAM+0x08000;
   RAM_SCROLL = RAM+0x1B000;
   RAM_OBJECT = RAM+0x18000;
   RAM_INPUT  = RAM+0x1A000;
   GFX_FG0    = RAM+0x20000;

   // 68000 Speed Hack

   if(is_current_game("cadashj"))
   {
   WriteLong68k(&ROM[0x0132A],0x4EF84F00);

   WriteLong68k(&ROM[0x04F00],0x13FC0000);
   WriteLong68k(&ROM[0x04F04],0x00AA0000);

   WriteLong68k(&ROM[0x04F08],0x4EF8124C);
   }
   else
   {
   WriteLong68k(&ROM[0x013AC],0x4EF84F00);

   WriteLong68k(&ROM[0x04F00],0x13FC0000);
   WriteLong68k(&ROM[0x04F04],0x00AA0000);

   WriteLong68k(&ROM[0x04F08],0x4EF812CE);
   }

   // Init tc0220ioc emulation
   // ------------------------

   tc0220ioc.RAM  = RAM_INPUT;
   tc0220ioc.ctrl = 0;		//TC0220_STOPCPU;
   reset_tc0220ioc();

   // Init tc0100scn emulation
   // ------------------------

   tc0100scn[0].layer[0].RAM	=RAM_VIDEO+0x0000;
   tc0100scn[0].layer[0].SCR	=RAM_SCROLL+0;
   tc0100scn[0].layer[0].type	=0;
   tc0100scn[0].layer[0].bmp_x	=32;
   tc0100scn[0].layer[0].bmp_y	=32;
   tc0100scn[0].layer[0].bmp_w	=320;
   tc0100scn[0].layer[0].bmp_h	=240;
   //tc0100scn[0].layer[0].mapper	=&Map_12bit_xBGR;
   tc0100scn[0].layer[0].tile_mask=0x3FFF;
   tc0100scn[0].layer[0].scr_x	=16;
   tc0100scn[0].layer[0].scr_y	=8;

   tc0100scn[0].layer[1].RAM	=RAM_VIDEO+0x8000;
   tc0100scn[0].layer[1].SCR	=RAM_SCROLL+2;
   tc0100scn[0].layer[1].type	=0;
   tc0100scn[0].layer[1].bmp_x	=32;
   tc0100scn[0].layer[1].bmp_y	=32;
   tc0100scn[0].layer[1].bmp_w	=320;
   tc0100scn[0].layer[1].bmp_h	=240;
   //tc0100scn[0].layer[1].mapper	=&Map_12bit_xBGR;
   tc0100scn[0].layer[1].tile_mask=0x3FFF;
   tc0100scn[0].layer[1].scr_x	=16;
   tc0100scn[0].layer[1].scr_y	=8;

   tc0100scn[0].layer[2].RAM	=RAM_VIDEO+0x4000;
   tc0100scn[0].layer[2].GFX	=GFX_FG0;
   tc0100scn[0].layer[2].SCR	=RAM_SCROLL+4;
   tc0100scn[0].layer[2].type	=3;
   tc0100scn[0].layer[2].bmp_x	=32;
   tc0100scn[0].layer[2].bmp_y	=32;
   tc0100scn[0].layer[2].bmp_w	=320;
   tc0100scn[0].layer[2].bmp_h	=240;
   //tc0100scn[0].layer[2].mapper	=&Map_12bit_xBGR;
   tc0100scn[0].layer[2].scr_x	=16;
   tc0100scn[0].layer[2].scr_y	=8;

   tc0100scn[0].RAM     = RAM_VIDEO;
   tc0100scn[0].GFX_FG0 = GFX_FG0;

   init_tc0100scn(0);

   // Init tc0002obj emulation
   // ------------------------

   tc0002obj.RAM	= RAM_OBJECT;
   tc0002obj.MASK	= NULL;
   tc0002obj.bmp_x	= 32;
   tc0002obj.bmp_y	= 32;
   tc0002obj.bmp_w	= 320;
   tc0002obj.bmp_h	= 240;
   //tc0002obj.mapper	= &Map_12bit_xBGR;
   tc0002obj.tile_mask	= 0x0FFF;
   tc0002obj.ofs_x	= 0;
   tc0002obj.ofs_y	= -8;

/*
 *  StarScream Stuff follows
 */

   ByteSwap(ROM,0x80000);
   ByteSwap(RAM,0x20000);

   AddMemFetch(0x000000, 0x07FFFF, ROM+0x000000-0x000000);		// 68000 ROM
   AddMemFetch(-1, -1, NULL);

   AddReadByte(0x000000, 0x07FFFF, NULL, ROM+0x000000);			// 68000 ROM
   AddReadByte(0x100000, 0x107FFF, NULL, RAM+0x000000);			// 68000 RAM
   AddReadByte(0xB00000, 0xB007FF, NULL, RAM_OBJECT);			// OBJECT RAM
   AddReadByte(0xC00000, 0xC0FFFF, NULL, RAM_VIDEO);			// SCREEN RAM
   AddReadByte(0x800000, 0x800FFF, NULL, RAM+0x018800);			// ??? RAM
   AddReadByte(0x900000, 0x90001F, tc0220ioc_rb, NULL);			// INPUT
   AddReadByte(0x000000, 0xFFFFFF, DefBadReadByte, NULL);		// <Bad Reads>
   AddReadByte(-1, -1, NULL, NULL);

   AddReadWord(0x000000, 0x07FFFF, NULL, ROM+0x000000);			// 68000 ROM
   AddReadWord(0x100000, 0x107FFF, NULL, RAM+0x000000);			// 68000 RAM
   AddReadWord(0xB00000, 0xB007FF, NULL, RAM_OBJECT);			// OBJECT RAM
   AddReadWord(0xC00000, 0xC0FFFF, NULL, RAM_VIDEO);			// SCREEN RAM
   AddReadWord(0x800000, 0x800FFF, NULL, RAM+0x018800);			// ??? RAM
   AddReadWord(0x900000, 0x90001F, tc0220ioc_rw, NULL);			// INPUT
   AddReadWord(0x0C0000, 0x0C0003, tc0140syt_read_main_68k, NULL); 	// SOUND
   AddReadWord(0xA00000, 0xA00007, tc0110pcr_rw, NULL);			// COLOUR RAM
   AddReadWord(0x000000, 0xFFFFFF, DefBadReadWord, NULL);		// <Bad Reads>
   AddReadWord(-1, -1,NULL, NULL);

   AddWriteByte(0x100000, 0x107FFF, NULL, RAM+0x000000);		// 68000 RAM
   AddWriteByte(0xB00000, 0xB007FF, NULL, RAM_OBJECT);			// OBJECT RAM
   AddWriteByte(0xC06000, 0xC06FFF, tc0100scn_0_gfx_fg0_wb, NULL);	// FG0 GFX RAM
   AddWriteByte(0xC00000, 0xC0FFFF, NULL, RAM_VIDEO);			// SCREEN RAM
   AddWriteByte(0x800000, 0x800FFF, NULL, RAM+0x018800);		// ??? RAM
   AddWriteByte(0x900000, 0x90001F, tc0220ioc_wb, NULL);		// INPUT
   AddWriteByte(0xAA0000, 0xAA0001, Stop68000, NULL);			// Trap Idle 68000
   AddWriteByte(0x000000, 0xFFFFFF, DefBadWriteByte, NULL);		// <Bad Writes>
   AddWriteByte(-1, -1, NULL, NULL);

   AddWriteWord(0x100000, 0x107FFF, NULL, RAM+0x000000);		// 68000 RAM
   AddWriteWord(0xB00000, 0xB007FF, NULL, RAM_OBJECT);			// OBJECT RAM
   AddWriteWord(0xC06000, 0xC06FFF, tc0100scn_0_gfx_fg0_ww, NULL);	// FG0 GFX RAM
   AddWriteWord(0xC00000, 0xC0FFFF, NULL, RAM_VIDEO);			// SCREEN RAM
   AddWriteWord(0xA00000, 0xA00007, tc0110pcr_ww, NULL);		// COLOUR RAM
   AddWriteWord(0x800000, 0x800FFF, NULL, RAM+0x018800);		// ??? RAM
   AddWriteWord(0x900000, 0x90001F, tc0220ioc_ww, NULL);		// INPUT
   AddWriteWord(0x0C0000, 0x0C0003, tc0140syt_write_main_68k, NULL);	// SOUND
   AddWriteWord(0x080000, 0x080001, NULL, RAM+0x01B010);		// CTRL RAM
   AddWriteWord(0xC20000, 0xC2000F, NULL, RAM_SCROLL);			// SCROLL RAM
   AddWriteWord(0x000000, 0xFFFFFF, DefBadWriteWord, NULL);		// <Bad Writes>
   AddWriteWord(-1, -1, NULL, NULL);

   AddInitMemory();	// Set Starscream mem pointers...
}

void clear_cadash(void)
{
   RemoveTaitoYM2151();

   #ifdef RAINE_DEBUG
      save_debug("RAM.bin",RAM,0x024000,1);
   #endif
}

void execute_cadash_frame(void)
{
   cpu_execute_cycles(CPU_68K_0, CPU_FRAME_MHz(12,60));	// M68000 12MHz (60fps)
   cpu_interrupt(CPU_68K_0, 5);
   cpu_interrupt(CPU_68K_0, 4);

   Taito2151_FrameRI();					// Z80 and YM2151
}

void draw_cadash(void)
{
  if (!tc0002obj.MASK) {
    tc0100scn[0].layer[0].MASK	=gfx_solid[0];
    tc0100scn[0].layer[1].MASK	=gfx_solid[0];
    tc0002obj.MASK	= gfx_solid[1];
    tc0002obj.GFX	= gfx[1];
    tc0100scn[0].layer[0].GFX	=gfx[0];
    tc0100scn[0].layer[1].GFX	=gfx[0];
  }

  ClearPaletteMap();
   // Init tc0100scn emulation
   // ------------------------

   tc0100scn_layer_count = 0;
   tc0100scn[0].ctrl = ReadWord(RAM_SCROLL+12);

   // Init tc0002obj emulation
   // ------------------------

   tc0002obj.ctrl = ReadWord(&RAM[0x1B010]);

   // BG0
   // ---

   render_tc0100scn_layer_mapped(0,0,0);

   // BG1+OBJECT
   // ----------

   if((tc0002obj.ctrl & 0x2000)==0){
      render_tc0100scn_layer_mapped(0,1,1);
      render_tc0002obj_mapped();
   }
   else{
      render_tc0002obj_mapped();
      render_tc0100scn_layer_mapped(0,1,1);
   }

   // FG0
   // ---

   render_tc0100scn_layer_mapped(0,2,1);
}

