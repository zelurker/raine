/******************************************************************************/
/*                                                                            */
/*                      CABAL (C) 1988 TAD CORPORATION                        */
/*                                                                            */
/******************************************************************************/

#include "gameinc.h"
#include "taitosnd.h"
#include "2151intf.h"
#include "smp16bit.h"
#include "sasound.h"		// sample support routines
#include "savegame.h"
#include "timer.h"

static struct ROM_INFO rom_cabalbl[] =
{
  LOAD_16_8( CPU1, "cabal_24.bin", 0x00000, 0x10000, 0x00abbe0c),
  LOAD_16_8( CPU1, "cabal_22.bin", 0x00001, 0x10000, 0x78c4af27),
  LOAD_16_8( CPU1, "cabal_23.bin", 0x20000, 0x10000, 0xd763a47c),
  LOAD_16_8( CPU1, "cabal_21.bin", 0x20001, 0x10000, 0x96d5e8af),
  LOAD( ROM2, "cabal_11.bin", 0x0000, 0x10000, 0xd308a543),
	/* The bootleg versions use a sub-board instead of the mask roms
	   the content is the same as the mask roms */
  LOAD( GFX1, "cabal_20.bin", 0x00000, 0x08000, 0x189033fd), /* characters */
  LOAD_16_8( GFX2, "cabal_15.bin", 0x00000, 0x10000, 0x1023319b),
  LOAD_16_8( GFX2, "cabal_17.bin", 0x00001, 0x10000, 0x3b6d2b09),
  LOAD_16_8( GFX2, "cabal_14.bin", 0x20000, 0x10000, 0x420b0801),
  LOAD_16_8( GFX2, "cabal_16.bin", 0x20001, 0x10000, 0x77bc7a60),
  LOAD_16_8( GFX2, "cabal_12.bin", 0x40000, 0x10000, 0x543fcb37),
  LOAD_16_8( GFX2, "cabal_18.bin", 0x40001, 0x10000, 0x0bc50075),
  LOAD_16_8( GFX2, "cabal_13.bin", 0x60000, 0x10000, 0xd28d921e),
  LOAD_16_8( GFX2, "cabal_19.bin", 0x60001, 0x10000, 0x67e4fe47),
  LOAD_16_8( GFX3, "cabal_04.bin", 0x00000, 0x10000, 0x34d3cac8),
  LOAD_16_8( GFX3, "cabal_05.bin", 0x00001, 0x10000, 0x4e49c28e),
  LOAD_16_8( GFX3, "cabal_03.bin", 0x20000, 0x10000, 0x7065e840),
  LOAD_16_8( GFX3, "cabal_06.bin", 0x20001, 0x10000, 0x6a0e739d),
  LOAD_16_8( GFX3, "cabal_02.bin", 0x40000, 0x10000, 0x0e1ec30e),
  LOAD_16_8( GFX3, "cabal_07.bin", 0x40001, 0x10000, 0x581a50c1),
  LOAD_16_8( GFX3, "cabal_01.bin", 0x60000, 0x10000, 0x55c44764),
  LOAD_16_8( GFX3, "cabal_08.bin", 0x60001, 0x10000, 0x702735c9),
  // There is an ultra minimum z80 code in these 2 roms just to read the samples starting at address 100
  // each word gives an offset for a sample, the sample itself starts by its size (high then low), then the data
  LOAD( SOUND1, "cabal_09.bin", 0x00000, 0x10000, 0x4ffa7fe3), /* Z80 code/adpcm data */
  LOAD( SOUND2, "cabal_10.bin", 0x00000, 0x10000, 0x958789b6), /* Z80 code/adpcm data */
  { NULL, 0, 0, 0, 0, 0 }
};

static gfx_layout text_layout =
{
	8,8,
	RGN_FRAC(1,1),
	2,
	{ 0,4 },
	{ 3, 2, 1, 0, 8+3, 8+2, 8+1, 8+0},
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16 },
	16*8
};

static gfx_layout tile_layout =
{
	16,16,
	RGN_FRAC(1,1),
	4,
	{ 8, 12, 0, 4 },
	{ 3, 2, 1, 0, 16+3, 16+2, 16+1, 16+0,
			32*16+3, 32*16+2, 32*16+1, 32*16+0, 33*16+3, 33*16+2, 33*16+1, 33*16+0 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32,
			8*32, 9*32, 10*32,  11*32,  12*32,  13*32, 14*32,  15*32 },
	64*16
};

static gfx_layout sprite_layout =
{
	16,16,
	RGN_FRAC(1,1),
	4,
	{ 8, 12, 0, 4 },
	{ 3, 2, 1, 0, 16+3, 16+2, 16+1, 16+0,
			32+3, 32+2, 32+1, 32+0, 48+3, 48+2, 48+1, 48+0 },
	{ 30*32, 28*32, 26*32, 24*32, 22*32, 20*32, 18*32, 16*32,
			14*32, 12*32, 10*32,  8*32,  6*32,  4*32,  2*32,  0*32 },
	64*16
};

static struct GFX_LIST cabal_gfx[] =
{
    { REGION_GFX1, &text_layout, },
    { REGION_GFX2, &tile_layout, },
    { REGION_GFX3, &sprite_layout, },
    { -1 },
};

static struct INPUT_INFO input_cabalbl[] =
{
  INP0( P1_UP, 0x5419, 0x01),
  INP0( P1_DOWN, 0x5419, 0x02),
  INP0( P1_LEFT, 0x5419, 0x04),
  INP0( P1_RIGHT, 0x5419, 0x08),
  INP0( P2_UP, 0x5419, 0x10),
  INP0( P2_DOWN, 0x5419, 0x20),
  INP0( P2_LEFT, 0x5419, 0x40),
  INP0( P2_RIGHT, 0x5419, 0x80),

  INP0( P1_B1, 0x5420, 0x01),
  INP0( P1_B2, 0x5420, 0x02),
  INP0( P2_B1, 0x5420, 0x04),
  INP0( P2_B2, 0x5420, 0x08),
  INP0( UNKNOWN, 0x5420, 0xf0),
  INP0( UNKNOWN, 0x5421, 0x0f),
  INP0( P2_B3, 0x5421, 0x10),
  INP0( P1_B3, 0x5421, 0x20),
  INP0( P2_START, 0x5421, 0x40),
  INP0( P1_START, 0x5421, 0x80),

  INP1( COIN1, 0x5422, 0x01),
  INP1( COIN2, 0x5422, 0x02),
  END_INPUT
};

static struct DSW_DATA dsw_data_cabal_0[] =
{
   { MSG_COIN_SLOTS,          0x0F, 0x08 },
   { MSG_1COIN_1PLAY,         0x0F},
   { MSG_2COIN_1PLAY,         0x0E},
   { MSG_3COIN_1PLAY,         0x0D},
   { MSG_4COIN_1PLAY,         0x0C},
   { MSG_5COIN_1PLAY,         0x0B},
   { MSG_6COIN_1PLAY,         0x0A},
   { MSG_1COIN_2PLAY,         0x09},
   { MSG_FREE_PLAY,              0x00},
   { _("Coin Mode"),             0x10, 0x02 },
   { _("Type 1"),                0x10},
   { _("Type 2"),                0x00},
   { _("Buttons"),               0x20, 0x02 },
   { MSG_NORMAL,              0x20},
   { MSG_INVERT,              0x00},
   { _("Trackball"),             0x80, 0x02 },
   { _("Small"),                 0x80},
   { _("Large"),                 0x00},
   { NULL,                    0,   },
};

static struct DSW_DATA dsw_data_cabal_1[] =
{
   { MSG_LIVES,               0x03, 0x04 },
   { "3",                     0x03},
   { "2",                     0x02},
   { "5",                     0x01},
   { _("Infinite"),              0x00},
   { MSG_EXTRA_LIFE,          0x0C, 0x04 },
   { _("20k and 50k"),           0x0C},
   { _("30k and 100k"),          0x08},
   { _("50k and 150k"),          0x04},
   { _("70k"),                   0x00},
   { MSG_DIFFICULTY,          0x30, 0x04 },
   { MSG_EASY,                0x30},
   { MSG_NORMAL,              0x20},
   { MSG_HARD,                0x10},
   { MSG_HARDEST,             0x00},
   { MSG_DEMO_SOUND,          0x80, 0x02 },
   { MSG_ON,                  0x80},
   { MSG_OFF,                 0x00},
   { NULL,                    0,   },
};

static struct DSW_INFO dsw_cabalbl[] =
{
   { 0x005410, 0xFF, dsw_data_cabal_0 },
   { 0x005411, 0xFF, dsw_data_cabal_1 },
   { 0,        0,    NULL,      },
};


static struct SMP16_ROM smp16_romlist_chip_a[16];	// Fill in later

static struct SMP16_ROM smp16_romlist_chip_b[16];	// Fill in later

static struct SMP16buffer_interface smp16_interface =
{
   2,					// 2 chips
   { 8000,
     8000 },				// rate
   { smp16_romlist_chip_a,
     smp16_romlist_chip_b },		// rom list
};

static struct YM2151interface ym2151_interface =
{
   1,			// 1 chip
   3579545, // 4000000,		// 4 MHz
   { YM3012_VOL(255,OSD_PAN_LEFT,255,OSD_PAN_RIGHT) },
   { z80_irq_handler }
};

static struct SOUND_INFO sound_cabalbl[] =
{
   { SOUND_YM2151J, &ym2151_interface,    },
   { SOUND_SMP16,   &smp16_interface,     },
   { 0,             NULL,                 },
};

static int latch2,ack_68k;

static void CabalSoundWrite68k(UINT32 offset, UINT8 data)
{
   switch(offset&0xe){
   case 0:
       latch = data;
       break;
      case 2:
         latch2=data;
      break;
      case 8:
      cpu_int_nmi(CPU_Z80_0);
      cpu_execute_cycles(CPU_Z80_0,50); // to fix coins which "stick" ! (doesn't seem to work with musashi ?!!)
      break;
   }
}

static UINT8 CabalSoundRead68k(UINT32 offset)
{
   int ret;

   switch(offset&15){
      case 0x05:
         ret = ack_68k;
         // ret = RAM[0x5404];
      break;
      default:
         ret = 0xFF;
      break;
   }

   return ret;
}


UINT8 CabalSoundReadZ80(UINT16 offset)
{
   UINT8 ta;

   switch(offset&15){
      case 0x08:
         ta=latch2;
	 // bitswap 7,2,4,5,3,6,1,0
	 // thanks to mame for finding this, I was about myself to make something like that, this thing didn't have any sense at all !
	 ta = (ta&0x8b)|((ta>>4)&4)|((ta>>1)&0x10)|((ta<<1)&0x20)|((ta&4)<<4);
      break;
      case 0x0A:
         ta=latch;
	 // bitswap 7,2,4,5,3,6,1,0
	 ta = (ta&0x8b)|((ta>>4)&4)|((ta>>1)&0x10)|((ta<<1)&0x20)|((ta&4)<<4);
      break;
      case 6:
      ta = RAM[0x5422];
      break;
      default:
         ta=0xFF;
      break;
   }

   print_debug("Z80Read:%04x,%02x [%04x]\n",offset,ta,z80pc);
   return(ta);
}

static void CabalSoundWriteZ80(UINT16 offset, UINT8 data)
{
   switch(offset&15){
      case 0x00:
         data&=0x7F;
         if((data>0)&&(data<16)) SMP16buffer_request(0,data-1);
      break;
      case 0x02:
         data&=0x7F;
         if((data>0)&&(data<16)) SMP16buffer_request(1,data-1);
      break;
      case 0xc: ack_68k = data; print_debug("ack_68k=%x for latch\n",ack_68k); break;
      default:
         print_debug("Z80Write:%04x,%02x [%04x]\n",offset,data,z80pc);
      break;
   }
}

static int gfx_init;

static void load_cabalbl(void)
{
    setup_z80_frame(CPU_Z80_0,3759545/60);
   int ta,tb,tc;
   latch = latch2 = ack_68k = 0xff;
   gfx_init = 0;
   RAMSize=0x30000;
   if(!(RAM = AllocateMem(RAMSize))) return;

   // Apply Speed Patch
   // -----------------

   Z80ROM[0x0120]=0xD3;	// OUTA (AAh)
   Z80ROM[0x0121]=0xAA;	//

   SetStopZ80Mode2(0x0110);

   // Setup Z80 memory map
   // --------------------

   AddZ80AROMBase(Z80ROM, 0x0038, 0x0066);

   AddZ80AReadByte(0x0000, 0x27FF, NULL,                  	Z80ROM+0x0000);	// Z80 ROM/RAM
   AddZ80AReadByte(0x400E, 0x400F, YM2151ReadZ80,         	NULL);		// YM2151 I/O
   AddZ80AReadByte(0x4000, 0x400D, CabalSoundReadZ80,     	NULL);		// 68000 + OTHER I/O
   AddZ80AReadByte(0x8000, 0xFFFF, NULL,			Z80ROM+0x8000);	// MORE ROM
   AddZ80AReadByte(0x0000, 0xFFFF, DefBadReadZ80,		NULL);		// <bad reads>

   AddZ80AWriteByte(0x2000, 0x27FF, NULL,                   	Z80ROM+0x2000);	// Z80 RAM
   AddZ80AWriteByte(0x400E, 0x400F, YM2151WriteZ80,         	NULL);		// YM2151 I/O
   AddZ80AWriteByte(0x4000, 0x400D, CabalSoundWriteZ80,     	NULL);		// 68000 + OTHER I/O
   AddZ80AWriteByte(0x0000, 0xFFFF, DefBadWriteZ80,		NULL);		// <bad writes>

   AddZ80AReadPort(0x00, 0xFF, DefBadReadZ80,           NULL);

   AddZ80AWritePort(0xAA, 0xAA, StopZ80Mode2,           NULL);
   AddZ80AWritePort(0x00, 0xFF, DefBadWriteZ80,         NULL);
   AddZ80AInit();

   tc = 0x102;
   ta=0;
   do {
       tb = ReadWord(&PCMROM[tc]);
       smp16_romlist_chip_a[ta].data = PCMROM+(tb+2);
       smp16_romlist_chip_a[ta].size = ReadWord68k(&PCMROM[tb]);
       smp16_romlist_chip_a[ta].type = 0;
       ta++,tc+=2;
   } while (tb != ReadWord(&PCMROM[tc]));
   smp16_romlist_chip_a[ta].data = NULL;

   UINT8 *PCMROM2 = load_region[REGION_SOUND2];
   tc = 0x102;
   ta=0;
   do {
       tb = ReadWord(&PCMROM2[tc]);
       smp16_romlist_chip_b[ta].data = PCMROM2+(tb+2);
       smp16_romlist_chip_b[ta].size = ReadWord68k(&PCMROM2[tb]);
       smp16_romlist_chip_b[ta].type = 0;
       ta++,tc+=2;
   } while (tb != ReadWord(&PCMROM2[tc]));
   smp16_romlist_chip_b[ta].data = NULL;

   /*-----------------------*/

   // memset(RAM+0x00000,0x00,0x20000);
   // memset(RAM+0x05400,0xFF,0x00100);

   set_colour_mapper(&col_map_xxxx_bbbb_gggg_rrrr_rev);
   InitPaletteMap(RAM+0x4C00, 0x40, 0x10, 0x1000);

   // 68000 Speed hack
   // ----------------

   WriteWord68k(&ROM[0x04FE],0x4EF9);
   WriteLong68k(&ROM[0x0500],0x00000300);

   WriteWord68k(&ROM[0x0300],0x5279);
   WriteLong68k(&ROM[0x0302],0x0004003E);

   WriteLong68k(&ROM[0x0306],0x13FC0000);	// move.b #$00,$AA0000
   WriteLong68k(&ROM[0x030A],0x00AA0000);	//

   WriteWord68k(&ROM[0x030E],0x6100-16);

/*
 *  StarScream Stuff follows
 */

   Add68000Code(0,0,REGION_CPU1);
   add_68000_ram(0,0x040000, 0x043FFF, RAM+0x000000);			// 68000 RAM
   // Not sure wether it's used or not, just saw a jsr to ram in the rom so...
   // but it might be related to something unused like a debug dip.
   // Anyway the memfetch doesn't do any harm
   AddMemFetch(0x40000, 0x43fff, RAM - 0x40000);
   add_68000_ram(0, 0x060000, 0x0607FF, RAM+0x004000);			// FG0 RAM
   AddReadByte(0x0E8000, 0x0E800F, CabalSoundRead68k, NULL);		// COIN RAM

   AddReadWord(0x0A0000, 0x0A001F, NULL, RAM+0x005410);			// INPUT RAM

   AddWriteByte(0x0E8000, 0x0E800F, CabalSoundWrite68k, NULL);		// SOUND COMM
   AddWriteByte(0xAA0000, 0xAA0001, Stop68000, NULL);			// Trap Idle 68000

   AddWriteWord(0x080000, 0x0803FF, NULL, RAM+0x004800);		// BG0 RAM
   AddWriteWord(0x0E0000, 0x0E07FF, NULL, RAM+0x004C00);		// COLOR RAM
   finish_conf_68000(0);
}

static void execute_cabalbl(void)
{
    execute_z80_audio_frame();

    cpu_execute_cycles(CPU_68K_0, CPU_FRAME_MHz(10,60));	// M68000 12MHz (60fps)
   cpu_interrupt(CPU_68K_0, 1);
}

static void draw_cabal(void)
{
   int x,y,x16,y16;
   int zz,zzzz,ta;
   UINT8 *MAP;

   ClearPaletteMap();
   UINT8 *bg0 = load_region[REGION_GFX2], *spr = load_region[REGION_GFX3];
   if (!gfx_init) {
       gfx_init = 1;
       int bg0_size = get_region_size(REGION_GFX2);
       for (int n=0; n<bg0_size; n++) {
	   bg0[n] ^= 0xf;
       }
       int spr_size = get_region_size(REGION_GFX3);
       for (int n=0; n<spr_size; n++) {
	   spr[n] ^= 0xf;
       }
       int gfx_size = get_region_size(REGION_GFX1);
       for (int n=0; n<gfx_size; n++) {
	   GFX[n] ^= 0x3;
       }
   }

   x16=16;
   y16=16;
   zzzz=0x20;

   zzzz=zzzz&0x3FF;

   for(y=(16+y16);y<(224+32);y+=16){
       zz=zzzz;
       for(x=(16+x16);x<(256+32);x+=16){

	   MAP_PALETTE_MAPPED_NEW(
		   (ReadWord(&RAM[0x4800+zz])>>12)|0x20,  // 0x20 o 0x30 è lo stesso perchè i colori sono uguali
		   16,        MAP
		   );

	   Draw16x16_Mapped_Rot(&bg0[(ReadWord(&RAM[0x4800+zz])&0xFFF)<<8],x,y,MAP);

	   zz+=2;
	   if((zz&0x1F)==0){zz-=0x20;}
       }
       zzzz+=0x20;
       zzzz=zzzz&0x3FF;
   }

   for(zz=0x3FF8;zz>=0x3800;zz-=8){

       if((RAM[zz+1]&0x01)==1){

	   y=16+RAM[zz+0];
	   x=(32+ReadWord(&RAM[zz+4]))&0x1FF;

	   if((x>16)&&(y>16)&&(x<256+32)&&(y<224+32)){
	       ta=ReadWord(&RAM[zz+2])&0xFFF;
	       if(ta!=0){

		   MAP_PALETTE_MAPPED_NEW(
			   (RAM[zz+5]>>3)+0x10,
			   16,        MAP
			   );

		   switch(RAM[zz+5]&0x04){
		   case 0x00: Draw16x16_Trans_Mapped_Rot(&spr[(ta<<8)],x,y,MAP);  break;
		   case 0x04: Draw16x16_Trans_Mapped_FlipY_Rot(&spr[(ta<<8)],x,y,MAP); break;
		   }

	       }

	   }

       }

   }

   set_colour_mapper(&col_map_12bit_xbgr_rev_2bpp);
   zz=0x80;
   for(y=32;y<224+32;y+=8){
       for(x=32;x<256+32;x+=8,zz+=2){

	   if((ta=ReadWord(&RAM[0x4000+zz])&0x03FF)!=0){

	       int col = ReadWord(&RAM[0x4000+zz])>>10;
	       MAP_PALETTE_MAPPED_2BPP_NEW(
		       col,
		       4,        MAP
		       );
	       Draw8x8_Trans_Mapped_Rot(&GFX[ta<<6],x,y,MAP);
	   }

       }
   }
   set_colour_mapper(&col_map_xxxx_bbbb_gggg_rrrr_rev);
}


/*

1 |xxxxxxxx| Y Pos
5 |xxxxxxxx| X Pos

*/
static struct VIDEO_INFO video_cabalbl =
{
   draw_cabal,
   256,
   224,
   32,
   VIDEO_ROTATE_NORMAL| VIDEO_ROTATABLE,
   cabal_gfx
};
static struct DIR_INFO dir_cabalbl[] =
{
   { "cabal", },
   { "cabalbl", },
   { NULL, },
};
GME( cabalbl, "Cabal (bootleg)", TAD, 1988, GAME_SHOOT,
	.long_name_jpn = "ƒJƒo[ƒ‹",
);

