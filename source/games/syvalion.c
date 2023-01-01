#define DRV_DEF_SOUND taito_ym2610_sound
/******************************************************************************/
/*                                                                            */
/*                    SYVALION (C) 1988 TAITO CORPORATION                     */
/*                                                                            */
/******************************************************************************/

#include "gameinc.h"
#include "tc004vcu.h"
#include "tc220ioc.h"
#include "sasound.h"		// sample support routines
#include "taitosnd.h"
#include "timer.h"
#include "speed_hack.h"

static struct ROM_INFO rom_syvalion[] =
{
  LOAD_16_8( CPU1, "b51-20.bin", 0x00000, 0x20000, 0x440b6418),
  LOAD_16_8( CPU1, "b51-22.bin", 0x00001, 0x20000, 0xe6c61079),
  LOAD_16_8( CPU1, "b51-19.bin", 0x40000, 0x20000, 0x2abd762c),
  LOAD_16_8( CPU1, "b51-21.bin", 0x40001, 0x20000, 0xaa111f30),
  LOAD( ROM2, "b51-23.bin", 0x00000, 0x10000, 0x734662de),
  LOAD_16_8( GFX1, "b51-16.bin", 0x000000, 0x20000, 0xc0fcf7a5),
  LOAD_16_8( GFX1, "b51-12.bin", 0x000001, 0x20000, 0x6b36d358),
  LOAD_16_8( GFX1, "b51-15.bin", 0x040000, 0x20000, 0x30b2ee02),
  LOAD_16_8( GFX1, "b51-11.bin", 0x040001, 0x20000, 0xae9a9ac5),
  LOAD_16_8( GFX1, "b51-08.bin", 0x100000, 0x20000, 0x9f6a535c),
  LOAD_16_8( GFX1, "b51-04.bin", 0x100001, 0x20000, 0x03aea658),
  LOAD_16_8( GFX1, "b51-07.bin", 0x140000, 0x20000, 0x764d4dc8),
  LOAD_16_8( GFX1, "b51-03.bin", 0x140001, 0x20000, 0x8fd9b299),
  LOAD_16_8( GFX1, "b51-14.bin", 0x200000, 0x20000, 0xdea7216e),
  LOAD_16_8( GFX1, "b51-10.bin", 0x200001, 0x20000, 0x6aa97fbc),
  LOAD_16_8( GFX1, "b51-13.bin", 0x240000, 0x20000, 0xdab28958),
  LOAD_16_8( GFX1, "b51-09.bin", 0x240001, 0x20000, 0xcbb4f33d),
  LOAD_16_8( GFX1, "b51-06.bin", 0x300000, 0x20000, 0x81bef4f0),
  LOAD_16_8( GFX1, "b51-02.bin", 0x300001, 0x20000, 0x906ba440),
  LOAD_16_8( GFX1, "b51-05.bin", 0x340000, 0x20000, 0x47976ae9),
  LOAD_16_8( GFX1, "b51-01.bin", 0x340001, 0x20000, 0x8dab004a),
  FILL( GFX1, 0x380000, 0x80000, 0),
  LOAD( SOUND2, "b51-18.bin", 0x00000, 0x80000, 0x8b23ac83), // deltat
  LOAD( SOUND1, "b51-17.bin", 0x00000, 0x80000, 0xd85096aa), // adpcm samples
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_tetristh[] =
{
  LOAD_16_8( CPU1, "c26-12-1.ic36", 0x00000, 0x20000, 0x77e80c82),
  LOAD_16_8( CPU1, "c26-11-1.ic18", 0x00001, 0x20000, 0x069d77d2),
  LOAD( ROM2, "c26-13.ic56", 0x00000, 0x10000, 0xefa89dfa),
  LOAD_16_8( GFX1, "c26-04.ic51", 0x000000, 0x20000, 0x23ddf00f),
  LOAD_16_8( GFX1, "c26-08.ic65", 0x000001, 0x20000, 0x86071824),
  LOAD_16_8( GFX1, "c26-03.ic50", 0x100000, 0x20000, 0x341be9ac),
  LOAD_16_8( GFX1, "c26-07.ic64", 0x100001, 0x20000, 0xc236311f),
  LOAD_16_8( GFX1, "c26-02.ic49", 0x200000, 0x20000, 0x0b0bc88f),
  LOAD_16_8( GFX1, "c26-06.ic63", 0x200001, 0x20000, 0xdeae0394),
  LOAD_16_8( GFX1, "c26-01.ic48", 0x300000, 0x20000, 0x7efc7311),
  LOAD_16_8( GFX1, "c26-05.ic62", 0x300001, 0x20000, 0x12718d97),
  FILL( GFX1, 0x340000, 0xc0000, 0),
  LOAD( SMP2, "b56-09.bin", 0x00000, 0x80000, 0x7fd9ee68),
  LOAD( SMP1, "b56-10.bin", 0x00000, 0x80000, 0xde1bce59),
  // LOAD( USER1, "b56-18.bin", 0x00000, 0x02000, 0xc88f0bbe),
  { NULL, 0, 0, 0, 0, 0 }
};

static gfx_layout tilelayout =
{
	16,16,  /* 16x16 pixels */
	32768,  /* 32768 tiles */
	4,
	{ 0, 1, 2, 3 },
	{ 4, 0, 12, 8, 0x100000*8+4, 0x100000*8, 0x100000*8+12, 0x100000*8+8,
		0x200000*8+4, 0x200000*8, 0x200000*8+12, 0x200000*8+8, 0x300000*8+4, 0x300000*8, 0x300000*8+12, 0x300000*8+8 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16,
		8*16, 9*16, 10*16, 11*16, 12*16, 13*16, 14*16, 15*16 },
	16*16
};

static struct GFX_LIST syvalion_gfx[] =
{
   { REGION_GFX1, &tilelayout,  },
   { 0,           NULL,             },
};

static struct INPUT_INFO input_syvalion[] =
{
   INP1( COIN1, 0x032004, 0x04 ),
   INP1( COIN2, 0x032004, 0x08 ),
   INP0( TILT, 0x032004, 0x20 ),
   INP0( SERVICE, 0x032004, 0x10 ),

   INP0( P1_START, 0x032004, 0x40 ),
   INP1( P1_UP, 0x032080, 0xFF ),
   INP1( P1_DOWN, 0x032081, 0xFF ),
   INP1( P1_LEFT, 0x032082, 0xFF ),
   INP1( P1_RIGHT, 0x032083, 0xFF ),
   INP0( P1_B1, 0x032006, 0x10 ),

   INP0( P2_START, 0x032004, 0x80 ),
   INP1( P2_UP, 0x032084, 0xFF ),
   INP1( P2_DOWN, 0x032085, 0xFF ),
   INP1( P2_LEFT, 0x032086, 0xFF ),
   INP1( P2_RIGHT, 0x032087, 0xFF ),
   INP0( P2_B1, 0x032006, 0x01 ),

   END_INPUT
};

static struct DSW_DATA dsw_data_syvalion_0[] =
{
    DSW_CABINET( 0, 1 ),
    DSW_SCREEN( 2, 0 ),
    DSW_TEST_MODE(0, 4),
    DSW_DEMO_SOUND(8, 0),
   { MSG_DSWA_BIT5,           0x10, 0x02 },
   { MSG_OFF,                 0x10},
   { MSG_ON,                  0x00},
   { MSG_DSWA_BIT6,           0x20, 0x02 },
   { MSG_OFF,                 0x20},
   { MSG_ON,                  0x00},
   { MSG_DSWA_BIT7,           0x40, 0x02 },
   { MSG_OFF,                 0x40},
   { MSG_ON,                  0x00},
   { MSG_DSWA_BIT8,           0x80, 0x02 },
   { MSG_OFF,                 0x80},
   { MSG_ON,                  0x00},
   { NULL,                    0,   },
};

static struct DSW_INFO dsw_syvalion[] =
{
   { 0x032000, 0xFF, dsw_data_syvalion_0 },
   { 0x032002, 0xFF, dsw_data_default_1 },
   { 0,        0,    NULL,      },
};

/* Stephh notes about the region :

  - Region stored at 0x07fffe.w
  - Sets :
      * 'syvalion' : region = 0x0000
  - Coinage relies on the region (code at 0x003640) :
      * 0x0000 (Japan), 0x0001 (US), 0x0003 (US, Romstar license) and
        0x0004 (licensed to PHOENIX ELECTRONICS CO.) use TAITO_COINAGE_JAPAN_OLD
      * 0x0002 (World) uses TAITO_COINAGE_WORLD
  - Notice screen only if region = 0x0000
  - Text is always in Japanese regardless of the region !
  - DSW bit 6 has an unknown effect because of a write to unmapped 0x430000.w
    (see code at 0x002af8). Another write to this address is done at 0x002a96.
    */

static struct ROMSW_DATA romsw_data_syvalion_0[] =
{
   { "Japan with copyright screen",                 0x00 },
   { "Japan, no copyright screen",           0x02 },
   { NULL,                    0    },
};

static struct ROMSW_INFO romsw_syvalion[] =
{
   { 0x07FFFF, 0x00, romsw_data_syvalion_0 },
   { 0,        0,    NULL },
};



static UINT8 *RAM_COLOUR;
static UINT8 *RAM_INPUT;

static void load_syvalion(void)
{
   setup_z80_frame(CPU_Z80_0,CPU_FRAME_MHz(5,60));

   RAMSize=0x48000+0x10000;
   if(!(RAM=AllocateMem(RAMSize))) return;
   AddTaitoYM2610(0x02EE, 0x028D, 0x10000);

   memset(RAM+0x00000,0x00,0x40000);

   RAM_COLOUR= RAM+0x31000+0x800;
   RAM_INPUT = RAM+0x32000;

   InitPaletteMap(RAM_COLOUR, 0x40, 0x10, 0x8000);

   set_colour_mapper(&col_map_xbbb_bbgg_gggr_rrrr);

   // Init tc0004vcu emulation
   // ------------------------

   tc0004vcu.RAM	= RAM+0x10000;
   tc0004vcu.GFX_FG0	= RAM+0x40000;
   tc0004vcu.tile_mask	= 0x3FFF;
   tc0004vcu.bmp_x	= 64;
   tc0004vcu.bmp_y	= 64;
   if (is_current_game("tetristh")) {
       tc0004vcu.bmp_w	= 320;
       tc0004vcu.bmp_h	= 240;
       tc0004vcu.scr_x	= 16-1;
       tc0004vcu.scr_y	= 32-1;
   } else {
       tc0004vcu.bmp_w	= 512;
       tc0004vcu.bmp_h	= 400;
       tc0004vcu.scr_x	= 0;
       tc0004vcu.scr_y	= 48;
   }

   tc0004vcu_init();

   // Init tc0220ioc emulation
   // ------------------------

   tc0220ioc.RAM  = RAM_INPUT;
   tc0220ioc.ctrl = 0;		//TC0220_STOPCPU;
   reset_tc0220ioc();

/*
 *  StarScream Stuff follows
 */


   Add68000Code(0,0,REGION_ROM1);
   if (is_current_game("syvalion")) apply_hack(0x72a,4);
   else if (is_current_game("tetristh")) apply_hack(0x72c,4);

   add_68000_ram(0,0x100000, 0x10FFFF, RAM+0x000000);			// 68000 RAM
   AddReadBW(0x400000, 0x420FFF, NULL, RAM+0x010000);			// SCREEN RAM
   add_68000_ram(0,0x500000, 0x500FFF, RAM+0x031000);			// COLOR RAM
   if (is_current_game("syvalion")) {
       GameMouse=1;
       AddReadByte(0x200000, 0x200003, tc0220ioc_rb_port, NULL);		// INPUT
       AddReadWord(0x200000, 0x200003, tc0220ioc_rw_port, NULL);		// INPUT
       AddWriteByte(0x200000, 0x200003, tc0220ioc_wb_port, NULL);		// INPUT
       AddWriteWord(0x200000, 0x200003, tc0220ioc_ww_port, NULL);		// INPUT
       AddReadByte(0x300000, 0x300003, tc0140syt_read_main_68k, NULL);	// SOUND COMM
       AddWriteByte(0x300000, 0x300003, tc0140syt_write_main_68k, NULL);	// SOUND COMM
   } else {
       AddReadByte(0x200000, 0x200003, tc0140syt_read_main_68k, NULL);	// SOUND COMM
       AddWriteByte(0x200000, 0x200003, tc0140syt_write_main_68k, NULL);	// SOUND COMM
       AddReadByte(0x300000, 0x300003, tc0220ioc_rb_port, NULL);		// INPUT
       AddReadWord(0x300000, 0x300003, tc0220ioc_rw_port, NULL);		// INPUT
       AddWriteByte(0x300000, 0x300003, tc0220ioc_wb_port, NULL);		// INPUT
       AddWriteWord(0x300000, 0x300003, tc0220ioc_ww_port, NULL);		// INPUT
   }

   AddReadWord(0x110000, 0x11FFFF, NULL, RAM+0x000000);			// 68000 RAM [MIRROR]

   AddWriteByte(0x410000, 0x410FFF, tc0004vcu_gfx_fg0_b_wb, NULL);	// FG0 GFX RAM
   AddWriteByte(0x401000, 0x420FFF, NULL, RAM+0x011000);		// SCREEN RAM
   AddWriteByte(0x400000, 0x400FFF, tc0004vcu_gfx_fg0_a_wb, NULL);	// FG0 GFX RAM
   AddWriteByte(0xAA0000, 0xAA0001, Stop68000, NULL);			// Trap Idle 68000

   AddWriteWord(0x410000, 0x410FFF, tc0004vcu_gfx_fg0_b_ww, NULL);	// FG0 GFX RAM
   AddWriteWord(0x401000, 0x420FFF, NULL, RAM+0x011000);		// SCREEN RAM
   AddWriteWord(0x400000, 0x400FFF, tc0004vcu_gfx_fg0_a_ww, NULL);	// FG0 GFX RAM

   finish_conf_68000(0);
}

static void execute_syvalion(void)
{
   static int p1x,p1y;

   /*------[Mouse Hack]-------*/

   //set_mouse_range(0,0,319,239);

   GetMouseMickeys(&p1x,&p1y);

   p1y=0-p1y;

   //p1x=0-(160-mouse_x);
   //p1y=(120-mouse_y);

   //mouse_x=160;
   //mouse_y=120;

   if(*MouseB&1) RAM[0x32006]&=0x10^255;

   if((RAM[0x32080]!=0)&&(p1y<  0x200)) p1y+=0x20;
   if((RAM[0x32081]!=0)&&(p1y> -0x200)) p1y-=0x20;
   if((RAM[0x32082]!=0)&&(p1x> -0x200)) p1x-=0x20;
   if((RAM[0x32083]!=0)&&(p1x<  0x200)) p1x+=0x20;

   RAM[0x3201C] = (p1x>>0)&0xFF;
   RAM[0x3201E] = (p1x>>8)&0xFF;
   RAM[0x32018] = (p1y>>0)&0xFF;
   RAM[0x3201A] = (p1y>>8)&0xFF;

   cpu_execute_cycles(CPU_68K_0, CPU_FRAME_MHz(12,60));	// M68000 12MHz (60fps)
   cpu_interrupt(CPU_68K_0, 2);

   execute_z80_audio_frame();
}

static void DrawSyvalion(void)
{
   ClearPaletteMap();

   if (RefreshBuffers) {
       tc0004vcu.GFX_BG0	= gfx[0];
       tc0004vcu.GFX_BG0_MSK= gfx_solid[0];
   }

   // Init tc0180vcu emulation
   // ------------------------

   // BG0
   // ---

   tc0004vcu_render_bg0();

   // BG1
   // ---

   tc0004vcu_render_bg1();

   // OBJECT
   // ------

   tc0004vcu_render_obj();

   // FG0
   // ---

   tc0004vcu_render_fg0();
}

static struct VIDEO_INFO video_syvalion =
{
   DrawSyvalion,
   512,
   400,
   64,
   VIDEO_ROTATE_NORMAL,
   syvalion_gfx
};
static struct VIDEO_INFO video_tetristh =
{
   DrawSyvalion,
   320,
   240,
   64,
   VIDEO_ROTATE_NORMAL,
   syvalion_gfx
};
GMEI( syvalion, "Syvalion", TAITO, 1988, GAME_SHOOT,
	.romsw = romsw_syvalion,
	.long_name_jpn = "サイバリオン",
);
extern void execute_recordbr(void);
static struct INPUT_INFO input_tetristh[] =
{
   INP1( COIN1, 0x032004, 0x04 ),
   INP1( COIN2, 0x032004, 0x08 ),
   INP0( TILT, 0x032004, 0x20 ),
   INP0( SERVICE, 0x032004, 0x10 ),

   INP0( P1_START, 0x032006, 0x01 ),
   INP0( P2_START, 0x032006, 0x02 ),
   INP0( P1_B1, 0x032006, 0x04 ),
   INP0( P2_B1, 0x032006, 0x08 ),
   INP0( P1_UP, 0x03200E, 0x01 ),
   INP0( P1_DOWN, 0x03200E, 0x02 ),
   INP0( P1_LEFT, 0x03200E, 0x04 ),
   INP0( P1_RIGHT, 0x03200E, 0x08 ),
   INP0( P2_UP, 0x03200E, 0x10 ),
   INP0( P2_DOWN, 0x03200E, 0x20 ),
   INP0( P2_LEFT, 0x03200E, 0x40 ),
   INP0( P2_RIGHT, 0x03200E, 0x80 ),

   INP0( P3_START, 0x032006, 0x04 ),

   INP0( P4_START, 0x032006, 0x08 ),

   END_INPUT
};

CLNEI( tetristh,syvalion,"Tetris (Japan,Taito H-System)",SEGA,1988, GAME_PUZZLE,
	.video = &video_tetristh,
	.exec = &execute_recordbr,
	.input = input_tetristh);

