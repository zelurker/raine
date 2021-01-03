/******************************************************************************/
/*                                                                            */
/*                  OPERATION WOLF (C) 1987 TAITO CORPORATION                 */
/*                                                                            */
/******************************************************************************/
/* I don't know who did that, but the operation wolf in this driver still uses
 * the z80 rom from the bootleg version to emulate the c-chip, so it's not the
 * real thing... although it uses the right 68k roms and even the sound rom. */

#include "gameinc.h"
#include "tc100scn.h"
#include "tc002obj.h"
#include "sasound.h"		// sample support routines
#include "taitosnd.h"
#include "msm5205.h"
#include "2151intf.h"
#include "gun.h"
#include "emumain.h"
#include "timer.h"

static struct ROM_INFO rom_opwolf[] =
{
  LOAD_16_8( CPU1, "b20-05-02.40", 0x00000, 0x10000, 0x3ffbfe3a),
  LOAD_16_8( CPU1, "b20-03-02.30", 0x00001, 0x10000, 0xfdabd8a5),
  LOAD_16_8( CPU1, "b20-04.39", 0x20000, 0x10000, 0x216b4838),
  LOAD_16_8( CPU1, "b20-20.29", 0x20001, 0x10000, 0xd244431a),
  LOAD( ROM2, "b20-07.10", 0x00000, 0x10000, 0x45c7ace3),
  LOAD( CPU3, "opwlfb.09", 0x00000, 0x08000, 0xab27a3dd),
  FILL(0x8000,0x8000,0,CPU3),
  LOAD( GFX1, "b20-13.13", 0x00000, 0x80000, 0xf6acdab1), /* SCR tiles (8 x 8) */
  LOAD( GFX2, "b20-14.72", 0x00000, 0x80000, 0x89f889e5), /* Sprites (16 x 16) */
  LOAD( SOUND1, "b20-08.21", 0x00000, 0x80000, 0xf3e19c64),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct INPUT_INFO input_opwolf[] =
{
   INP1( COIN1, 0x05C580, 0x04 ),
   INP1( COIN2, 0x05C581, 0x04 ),
   INP0( TILT, 0x02B10A, 0x08 ),
   INP0( SERVICE, 0x02B10A, 0x04 ),

   INP0( P1_START, 0x02B10A, 0x10 ),
   INP1( P1_UP, 0x02B408, 0x01 ),
   INP1( P1_DOWN, 0x02B408, 0x02 ),
   INP1( P1_LEFT, 0x02B408, 0x04 ),
   INP1( P1_RIGHT, 0x02B408, 0x08 ),
   INP0( P1_B1, 0x02B10A, 0x01 ),
   INP0( P1_B2, 0x02B10A, 0x02 ),

   END_INPUT
};

static struct DSW_DATA dsw_data_operation_wolf_0[] =
{
   { MSG_DSWA_BIT1,           0x01, 0x02 },
   { MSG_OFF,                 0x01},
   { MSG_ON,                  0x00},
   DSW_CONTINUE_PLAY( 0x00, 0x02),
   DSW_TEST_MODE( 0x00, 0x04),
   { _("Attract Mode"),          0x08, 0x02 },
   { _("Sound"),                 0x08},
   { _("Silence"),               0x00},
   { _("Coin Slot A"),           0x30, 0x04 },
   { MSG_1COIN_1PLAY,         0x30},
   { MSG_1COIN_2PLAY,         0x20},
   { MSG_2COIN_1PLAY,         0x10},
   { MSG_2COIN_3PLAY,         0x00},
   { _("Coin Slot B"),           0xC0, 0x04 },
   { MSG_1COIN_1PLAY,         0xC0},
   { MSG_1COIN_2PLAY,         0x80},
   { MSG_2COIN_1PLAY,         0x40},
   { MSG_2COIN_3PLAY,         0x00},
   { NULL,                    0,   },
};

static struct DSW_DATA dsw_data_operation_wolf_1[] =
{
   { MSG_DIFFICULTY,          0x03, 0x04 },
   { MSG_NORMAL,              0x03},
   { MSG_EASY,                0x02},
   { MSG_HARD,                0x01},
   { MSG_HARDEST,             0x00},
   { _("Start Ammo"),            0x0C, 0x04 },
   { _("6 Clips"),               0x0C},
   { _("7 Clips"),               0x08},
   { _("5 Clips"),               0x04},
   { _("4 Clips"),               0x00},
   { MSG_DSWB_BIT6,           0x10, 0x02 },
   { MSG_OFF,                 0x10},
   { MSG_ON,                  0x00},
   { MSG_DSWB_BIT7,           0x20, 0x02 },
   { MSG_OFF,                 0x20},
   { MSG_ON,                  0x00},
   { _("Cont Discount"),         0x40, 0x02 },
   { MSG_OFF,                 0x40},
   { MSG_ON,                  0x00},
   { _("Language"),              0x80, 0x02 },
   { _("Japanese"),              0x80},
   { _("English"),               0x00},
   { NULL,                    0,   },
};

static struct DSW_INFO dsw_opwolf[] =
{
   { 0x02B000, 0xBF, dsw_data_operation_wolf_0 },
   { 0x02B002, 0xFF, dsw_data_operation_wolf_1 },
   { 0,        0,    NULL,      },
};

static struct ROMSW_DATA romsw_data_operation_wolf_0[] =
{
   { "Taito Japan (Notice)",  0x01 },
   { "Taito America",         0x02 },
   { "Taito Japan",           0x03 },
   { NULL,                    0    },
};

static struct ROMSW_INFO romsw_opwolf[] =
{
   { 0x03FFFF, 0x02, romsw_data_operation_wolf_0 },
   { 0,        0,    NULL },
};


static struct YM2151interface ym2151_interface =
{
  1,			// 1 chip
  4000000,		// 4 MHz
  { YM3012_VOL(160,OSD_PAN_LEFT,160,OSD_PAN_RIGHT) },
  { z80_irq_handler },
  { NULL }
};

static struct msm5205_adpcm_list opwolf_adpcm1[MAX_MSM_ADPCM],opwolf_adpcm2[MAX_MSM_ADPCM];

static struct MSM5205buffer_interface msm5205_interface =
{
   2,
   { 8000, 8000 },
   { 223,  223  },
   { opwolf_adpcm1, opwolf_adpcm2},
   { MAX_MSM_ADPCM, MAX_MSM_ADPCM },
   { NULL, NULL },
   { 0,    0    },
   { REGION_SOUND1, REGION_SOUND1 },
   MSM5205_MONO,
};

static struct SOUND_INFO sound_opwolf[] =
{
   { SOUND_YM2151J, &ym2151_interface,  },
   { SOUND_MSM5205_BUFF, &msm5205_interface, },
   { 0,             NULL,               },
};

static struct ROM_INFO rom_opwolfb[] =
{
  LOAD_16_8( CPU1, "opwlfb.12", 0x00000, 0x10000, 0xd87e4405),
  LOAD_16_8( CPU1, "opwlfb.10", 0x00001, 0x10000, 0x9ab6f75c),
  LOAD_16_8( CPU1, "opwlfb.13", 0x20000, 0x10000, 0x61230c6e),
  LOAD_16_8( CPU1, "opwlfb.11", 0x20001, 0x10000, 0x342e318d),
  LOAD( ROM2, "opwlfb.30", 0x00000, 0x08000, 0x0669b94c),
  LOAD( CPU3, "opwlfb.09", 0x00000, 0x08000, 0xab27a3dd),
  FILL(0x8000,0x8000,0,CPU3),
  LOAD_16_8( GFX1, "opwlfb.08", 0x00000, 0x10000, 0x134d294e), /* SCR tiles (8 x 8) */
  LOAD_16_8( GFX1, "opwlfb.06", 0x20000, 0x10000, 0x317d0e66), /* SCR tiles (8 x 8) */
  LOAD_16_8( GFX1, "opwlfb.07", 0x40000, 0x10000, 0xe1c4095e), /* SCR tiles (8 x 8) */
  LOAD_16_8( GFX1, "opwlfb.05", 0x60000, 0x10000, 0xfd9e72c8), /* SCR tiles (8 x 8) */
  LOAD_16_8( GFX1, "opwlfb.04", 0x00001, 0x10000, 0xde0ca98d), /* SCR tiles (8 x 8) */
  LOAD_16_8( GFX1, "opwlfb.02", 0x20001, 0x10000, 0x6231fdd0), /* SCR tiles (8 x 8) */
  LOAD_16_8( GFX1, "opwlfb.03", 0x40001, 0x10000, 0xccf8ba80), /* SCR tiles (8 x 8) */
  LOAD_16_8( GFX1, "opwlfb.01", 0x60001, 0x10000, 0x0a65f256), /* SCR tiles (8 x 8) */
  LOAD_16_8( GFX2, "opwlfb.14", 0x00000, 0x10000, 0x663786eb), /* Sprites (16 x 16) */
  LOAD_16_8( GFX2, "opwlfb.15", 0x20000, 0x10000, 0x315b8aa9), /* Sprites (16 x 16) */
  LOAD_16_8( GFX2, "opwlfb.16", 0x40000, 0x10000, 0xe01099e3), /* Sprites (16 x 16) */
  LOAD_16_8( GFX2, "opwlfb.17", 0x60000, 0x10000, 0x56fbe61d), /* Sprites (16 x 16) */
  LOAD_16_8( GFX2, "opwlfb.18", 0x00001, 0x10000, 0xde9ab08e), /* Sprites (16 x 16) */
  LOAD_16_8( GFX2, "opwlfb.19", 0x20001, 0x10000, 0x645cf85e), /* Sprites (16 x 16) */
  LOAD_16_8( GFX2, "opwlfb.20", 0x40001, 0x10000, 0xd80b9cc6), /* Sprites (16 x 16) */
  LOAD_16_8( GFX2, "opwlfb.21", 0x60001, 0x10000, 0x97d25157), /* Sprites (16 x 16) */
  LOAD_16_8( SOUND1, "opwlfb.29", 0x00000, 0x10000, 0x05a9eac0),
  LOAD_16_8( SOUND1, "opwlfb.28", 0x20000, 0x10000, 0x281b2175),
  LOAD_16_8( SOUND1, "opwlfb.27", 0x40000, 0x10000, 0x441211a6),
  LOAD_16_8( SOUND1, "opwlfb.26", 0x60000, 0x10000, 0x86d1d42d),
  LOAD_16_8( SOUND1, "opwlfb.25", 0x00001, 0x10000, 0x85b87f58),
  LOAD_16_8( SOUND1, "opwlfb.24", 0x20001, 0x10000, 0x8efc5d4d),
  LOAD_16_8( SOUND1, "opwlfb.23", 0x40001, 0x10000, 0xa874c703),
  LOAD_16_8( SOUND1, "opwlfb.22", 0x60001, 0x10000, 0x9228481f),
  { NULL, 0, 0, 0, 0, 0 }
};

static gfx_layout charlayout =
{
	8,8,    /* 8*8 characters */
	RGN_FRAC(1,1),
	4,  /* 4 bits per pixel */
	{ 0, 1, 2, 3 },
	{ 2*4, 3*4, 0*4, 1*4, 6*4, 7*4, 4*4, 5*4 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	32*8    /* every sprite takes 32 consecutive bytes */
};

static gfx_layout tilelayout =
{
	16,16,  /* 16*16 sprites */
	RGN_FRAC(1,1),
	4,  /* 4 bits per pixel */
	{ 0, 1, 2, 3 },
	{ 2*4, 3*4, 0*4, 1*4, 6*4, 7*4, 4*4, 5*4, 10*4, 11*4, 8*4, 9*4, 14*4, 15*4, 12*4, 13*4 },
	{ 0*64, 1*64, 2*64, 3*64, 4*64, 5*64, 6*64, 7*64, 8*64, 9*64, 10*64, 11*64, 12*64, 13*64, 14*64, 15*64 },
	128*8   /* every sprite takes 128 consecutive bytes */
};

static gfx_layout charlayout_b =
{
	8,8,    /* 8*8 characters */
	RGN_FRAC(1,1),
	4,  /* 4 bits per pixel */
	{ 0, 1, 2, 3 },
	{ 0*4, 1*4, 2*4, 3*4, 4*4, 5*4, 6*4, 7*4 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	32*8    /* every sprite takes 32 consecutive bytes */
};

static gfx_layout tilelayout_b =
{
	16,16,  /* 16*16 sprites */
	RGN_FRAC(1,1),
	4,  /* 4 bits per pixel */
	{ 0, 1, 2, 3 },
	{ 0*4, 1*4, 2*4, 3*4, 4*4, 5*4, 6*4, 7*4, 8*4, 9*4, 10*4, 11*4, 12*4, 13*4, 14*4, 15*4 },
	{ 0*64, 1*64, 2*64, 3*64, 4*64, 5*64, 6*64, 7*64, 8*64, 9*64, 10*64, 11*64, 12*64, 13*64, 14*64, 15*64 },
	128*8   /* every sprite takes 128 consecutive bytes */
};

static GFX_LIST gfx_opwolf[] =
{
    { REGION_GFX2, &tilelayout, },
    { REGION_GFX1, &charlayout, },
    { 0, NULL }
};

static GFX_LIST gfx_opwolfb[] =
{
    { REGION_GFX2, &tilelayout_b, },
    { REGION_GFX1, &charlayout_b, },
    { 0, NULL }
};

static UINT8 *RAM_VIDEO;
static UINT8 *RAM_SCROLL;
static UINT8 *GFX_BG0_SOLID;

static UINT8 *RAM_OBJECT;
static UINT8 *GFX_SPR;
static UINT8 *GFX_SPR_SOLID;

static UINT8 *Z80ROM2;

static int romset;	// 0=ORIGINAL; 1=BOOTLEG

static int x1,myy1,x11,myy11;

static int CChipReadB(UINT32 address)
{
   int i;

   i=address&0x0FFE;

   return(Z80ROM2[0xC000+(i>>1)]);
}

static int CChipReadW(UINT32 address)
{
   return(CChipReadB(address+1));
}

static void CChipWriteB(UINT32 address, int data)
{
   int i;

   i=address&0x0FFE;

   Z80ROM2[0xC000+(i>>1)]=data;
}

static void CChipWriteW(UINT32 address, int data)
{
   CChipWriteB(address+1,data&0xFF);
}

static void clear_ram() {
  // Clears the z80 c-chip ram, or reseting the game will hang
  memset(Z80ROM2+0x8000,0,0x8000);
}

static int init_gfx;

static void finish_conf_gfx() {
    init_gfx = 1;
    GFX_SPR	= gfx[0];
    GFX = gfx[1];
    GFX_BG0_SOLID = gfx_solid[1];
    GFX_SPR_SOLID = gfx_solid[0];
}

/*-------[ADPCM PORT]-------*/

static int ad[8];

static void OpWolfWriteADPCMA(UINT16 offset, UINT8 data)
{
   offset&=7;

   ad[offset]=data;

	if (offset == 0x04) //trigger ?
	{
	    int start=ad[0] + 256*ad[1];
	    int end = ad[2] + 256*ad[3];
	    start *= 16;
	    end   *= 16;

	    live_msm_decode(0,start,end-start);
		//logerror("TRIGGER MSM1\n");
	}
}

/*------[ADPCM PORT 2]------*/

static int ad2[8];

static void OpWolfWriteADPCMB(UINT16 offset, UINT8 data)
{
   offset&=7;

   ad2[offset]=data;
   if (offset == 0x04) //trigger ?
	{
	    int start=ad2[0] + 256*ad2[1];
	    int end = ad2[2] + 256*ad2[3];
	    start *= 16;
	    end   *= 16;

	    live_msm_decode(1,start,end-start);
		//logerror("TRIGGER MSM1\n");
	}
}

static void load_opwolf(void)
{
   if (is_current_game("opwolf"))
       romset = 0;
   else if (is_current_game("opwolfb"))
       romset = 1;

   RAMSize=0x60000;
   init_gfx = 0;

   set_reset_function(clear_ram);

   if(!(RAM=AllocateMem(0x80000))) return;

   GFX_SPR	= gfx[0];
   GFX = gfx[1];
   GFX_BG0_SOLID = gfx_solid[1];
   GFX_SPR_SOLID = gfx_solid[0];

   Z80ROM2=RAM+0x50000;			// C-CHIP ROM/RAM
   // coin input is mapeed directly in 5c580 -> in the middle of this z80rom2 area !
   memcpy(Z80ROM2,load_region[REGION_CPU3],0x8000);
   AddTaitoYM2151(0x028A, 0x0219, 0x10000, (void *) OpWolfWriteADPCMA, (void *) OpWolfWriteADPCMB);
   setup_z80_frame(CPU_Z80_0,CPU_FRAME_MHz(4,60));

   /*--------[C-Chip Z80]---------*/

   Z80ROM2[0x19B]=0xC3;
   Z80ROM2[0x19C]=0x80;
   Z80ROM2[0x19D]=0x00;

   Z80ROM2[0x080]=0x21;
   Z80ROM2[0x081]=0x1B;
   Z80ROM2[0x082]=0xC0;

   Z80ROM2[0x083]=0xD3;  // OUTA (AAh)
   Z80ROM2[0x084]=0xAA;  //

   Z80ROM2[0x0570]=0x18;
   Z80ROM2[0x059E]=0x18;

   SetStopZ80BMode2(0x019E);

   AddZ80BROMBase(Z80ROM2, 0x0038, 0x0066);

   AddZ80BReadByte(0x0000, 0xFFFF, NULL,                        Z80ROM2+0x0000); // Z80 ROM/RAM

   AddZ80BWriteByte(0x0000, 0xFFFF, NULL,                       Z80ROM2+0x0000); // Z80 RAM

   AddZ80BReadPort(0x00, 0xFF, DefBadReadPort,           NULL);
   AddZ80BReadPort(  -1,   -1, NULL,                    NULL);

   AddZ80BWritePort(0xAA, 0xAA, StopZ80BMode2,          NULL);
   AddZ80BWritePort(0x00, 0xFF, DefBadWritePort,         NULL);
   AddZ80BWritePort(  -1,   -1, NULL,                   NULL);

   AddZ80BInit();

   ROM[0x0988]=0x60;	// FIX C-CHIP USER ERROR

   // ROM[0x0C32]=0x60;	// COIN ERROR

   WriteWord68k(&ROM[0xBB72],0x4E71);		// nop

   // 68000 Speed Hack
   // ----------------

   WriteLong68k(&ROM[0x04F4],0x13FC0000);	// move.b #$00,$AA0000
   WriteLong68k(&ROM[0x04F8],0x00AA0000);
   WriteWord68k(&ROM[0x04FC],0x6100-16);	// <loop>

   memset(RAM+0x00000,0x00,0x40000);

   RAM_VIDEO  = RAM+0x08000;
   RAM_SCROLL = RAM+0x2B200;
   RAM_OBJECT = RAM+0x28000;

   set_colour_mapper(&col_map_xxxx_rrrr_gggg_bbbb);
   InitPaletteMap(RAM+0x2A000, 0x100, 0x10, 0x1000);

/*
 *  StarScream Main 68000 Setup
 */

   ByteSwap(ROM,0x40000);

   add_68000_rom(0,0,0x3ffff,ROM);
   add_68000_ram(0,0x100000, 0x107fff, RAM);
   add_68000_ram(0,0xc00000, 0xc1ffff, RAM_VIDEO);

   AddReadByte(0xD00000, 0xD01FFF, NULL, RAM_OBJECT);			// OBJECT RAM
   AddReadByte(0x0FF000, 0x0FFFFF, CChipReadB, NULL);			// C-CHIP
   AddReadByte(0x380000, 0x38000F, NULL, RAM+0x02B000);			// I/O RAM
   AddReadByte(0x0F0000, 0x0F000F, NULL, RAM+0x02B100);			// I/O RAM
   AddReadByte(0x3E0000, 0x3E0003, tc0140syt_read_main_68k, NULL);	// SOUND

   AddReadWord(0x200000, 0x2007FF, NULL, RAM+0x02A000);			// COLOR RAM
   AddReadWord(0x0FF000, 0x0FFFFF, CChipReadW, NULL);			// C-CHIP
   AddReadWord(0x380000, 0x38000F, NULL, RAM+0x02B000);			// I/O RAM
   AddReadWord(0x3A0000, 0x3A000F, NULL, RAM+0x02B400);			// GUN
   AddReadWord(0x0F0000, 0x0F000F, NULL, RAM+0x02B100);			// I/O RAM

   AddWriteByte(0xD00000, 0xD01FFF, NULL, RAM_OBJECT);			// OBJECT RAM
   AddWriteByte(0x0FF000, 0x0FFFFF, CChipWriteB, NULL);			// C-CHIP
   AddWriteByte(0x3E0000, 0x3E0003, tc0140syt_write_main_68k, NULL);	// SOUND
   AddWriteByte(0xAA0000, 0xAA0001, Stop68000, NULL);			// Trap Idle 68000

   AddWriteWord(0xD00000, 0xD01FFF, NULL, RAM_OBJECT);			// OBJECT RAM
   AddWriteWord(0x200000, 0x2007FF, NULL, RAM+0x02A000);		// COLOR RAM
   AddWriteWord(0xC20000, 0xC20003, NULL, RAM_SCROLL+6);		// SCROLL Y
   AddWriteWord(0xC40000, 0xC40003, NULL, RAM_SCROLL+0);		// SCROLL X
   AddWriteWord(0xC50000, 0xC50001, NULL, RAM_SCROLL+14);		// CTRL
   AddWriteWord(0x0FF000, 0x0FFFFF, CChipWriteW, NULL);			// C-CHIP

   finish_conf_68000(0);

   GameMouse=1;
}

static void execute_opwolf(void)
{
   /*------[Mouse Hack]-------*/

   set_mouse_range(0,0,319,239);

   x11=mouse_x;
   myy11=mouse_y;

   x1=x11+16;
   if(romset==1){
      myy1=myy11-8;
   }
   else{
      myy1=myy11-16;
   }

   if(x1<0)   x1=0;
   if(x1>319) x1=319;
   if(myy1<0)   myy1=0;
   if(myy1>239) myy1=239;

   if(mouse_b&1) RAM[0x2B10A]&= ~0x01;
   if(mouse_b&2) RAM[0x2B10A]&= ~0x02;

   /*-----[Keyboard Hack]-----*/

   if(((RAM[0x2B408]&0x01)!=0)&&(myy1>0))   myy1--;
   if(((RAM[0x2B408]&0x02)!=0)&&(myy1<239)) myy1++;
   if(((RAM[0x2B408]&0x04)!=0)&&(x1>0))   x1--;
   if(((RAM[0x2B408]&0x08)!=0)&&(x1<319)) x1++;

   /*----[Write Positions]----*/

   RAM[0x2B400]=x1;
   RAM[0x2B402]=myy1;

   cpu_execute_cycles(CPU_Z80_1, 400000);		// C-CHIP Z80 4Mhz?
   cpu_interrupt(CPU_Z80_1, 0x38);

   cpu_execute_cycles(CPU_68K_0, CPU_FRAME_MHz(12,60));	// M68000 12MHz (60fps)		// MAIN 68000 12Mhz?
   cpu_interrupt(CPU_68K_0, 5);

   Taito2151_Frame();	// Z80 and YM2151
}

static void DrawOperationWolf(void)
{
   int x,y;
   if (!init_gfx)
     finish_conf_gfx();

   ClearPaletteMap();

   // Init tc0100scn emulation
   // ------------------------

   tc0100scn_layer_count = 0;
   tc0100scn[0].ctrl = ReadWord(RAM_SCROLL+12);

   if(RefreshBuffers){

   tc0100scn[0].layer[0].RAM	=RAM_VIDEO+0x0000;
   tc0100scn[0].layer[0].GFX	=GFX;
   tc0100scn[0].layer[0].MASK	=GFX_BG0_SOLID;
   tc0100scn[0].layer[0].SCR	=RAM_SCROLL+0;
   tc0100scn[0].layer[0].type	=0;
   tc0100scn[0].layer[0].bmp_x	=32;
   tc0100scn[0].layer[0].bmp_y	=32;
   tc0100scn[0].layer[0].bmp_w	=320;
   tc0100scn[0].layer[0].bmp_h	=240;
   tc0100scn[0].layer[0].tile_mask=0x3FFF;
   tc0100scn[0].layer[0].scr_x	=16;
   tc0100scn[0].layer[0].scr_y	=8;

   tc0100scn[0].layer[1].RAM	=RAM_VIDEO+0x8000;
   tc0100scn[0].layer[1].GFX	=GFX;
   tc0100scn[0].layer[1].MASK	=GFX_BG0_SOLID;
   tc0100scn[0].layer[1].SCR	=RAM_SCROLL+2;
   tc0100scn[0].layer[1].type	=0;
   tc0100scn[0].layer[1].bmp_x	=32;
   tc0100scn[0].layer[1].bmp_y	=32;
   tc0100scn[0].layer[1].bmp_w	=320;
   tc0100scn[0].layer[1].bmp_h	=240;
   tc0100scn[0].layer[1].tile_mask=0x3FFF;
   tc0100scn[0].layer[1].scr_x	=16;
   tc0100scn[0].layer[1].scr_y	=8;

   tc0100scn[0].RAM     = RAM_VIDEO;
   tc0100scn[0].GFX_FG0 = NULL;
   init_tc0100scn(0);

   }

   // Init tc0002obj emulation
   // ------------------------

   //tc0002obj.ctrl	= ReadWord(&RAM[0x1B010]);
   tc0002obj.ctrl	= 0x2000;

   if(RefreshBuffers){

   tc0002obj.RAM	= RAM_OBJECT;
   tc0002obj.GFX	= GFX_SPR;
   tc0002obj.MASK	= GFX_SPR_SOLID;
   tc0002obj.bmp_x	= 32;
   tc0002obj.bmp_y	= 32;
   tc0002obj.bmp_w	= 320;
   tc0002obj.bmp_h	= 240;
   tc0002obj.tile_mask	= 0x0FFF;
   tc0002obj.ofs_x	= 0;
   tc0002obj.ofs_y	= -8;

   }

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

   // CURSOR
   // ------

   x=(32+x11)-7;
   y=(32+myy11)-7;
   if((x>16)&&(y>16)&&(x<320+32)&&(y<224+32)){
     disp_gun(0,x,y);
   }
}

static struct VIDEO_INFO video_opwolf =
{
   DrawOperationWolf,
   320,
   240,
   32,
   VIDEO_ROTATE_NORMAL| VIDEO_ROTATABLE,
   gfx_opwolf
};

static struct VIDEO_INFO video_opwolfb =
{
   DrawOperationWolf,
   320,
   240,
   32,
   VIDEO_ROTATE_NORMAL| VIDEO_ROTATABLE,
   gfx_opwolfb
};
static struct DIR_INFO dir_opwolf[] =
{
   { "operation_wolf", },
   { "opwolf", },
   { "opwolfb", },
   { "opwolfu", },
   { NULL, },
};
GME( opwolf, "Operation Wolf", TAITO, 1987, GAME_SHOOT,
	.romsw = romsw_opwolf,
	.long_name_jpn = "オペレーションウルフ",
	.board = "B20",
);
static struct DIR_INFO dir_opwolfb[] =
{
   { "operation_wolf_bootleg", },
   { "opwolfbl", },
   { "opwolfb", },
   { ROMOF("opwolf"), },
   { CLONEOF("opwolf"), },
   { NULL, },
};
CLNE( opwolfb, opwolf, "Operation Bear", BOOTLEG, 1987, GAME_SHOOT,
	.long_name_jpn = "オペレーションウルフ (bootleg)",
	.video = &video_opwolfb
);

