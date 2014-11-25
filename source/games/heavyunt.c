/******************************************************************************/
/*                                                                            */
/*                HEAVY UNIT (C) 1988 KANEKO (TAITO LICENSE)                  */
/*                                                                            */
/******************************************************************************/

#include "gameinc.h"
#include "tc220ioc.h"
#include "taitosnd.h"
#include "2203intf.h"
#include "decode.h"
#include "sasound.h"		// sample support routines
#include "savegame.h"
#include "blit.h" // clear_game_screen

/*

b73-12.bin - sound cpu
----------------------

nmi is triggered when sound command is written?
int is triggered by ym2203 timers or 60fps?

memory:

0000-7FFF = base rom
8000-BFFF = bank rom

port:

00-00 = bank switch
02-03 = ym2203
04-04 = sound command?

*/



static struct ROM_INFO rom_heavyunt[] =
{
   {       "b73.01", 0x00010000, 0x3a8a4489, 0, 0, 0, },
   {       "b73.02", 0x00010000, 0x025c536c, 0, 0, 0, },
   {       "b73.03", 0x00010000, 0xec6020cf, 0, 0, 0, },
   {       "b73.04", 0x00010000, 0xf7badbb2, 0, 0, 0, },
   {       "b73.05", 0x00010000, 0xb8e829d2, 0, 0, 0, },
   {       "b73.06", 0x00010000, 0xa98e4aea, 0, 0, 0, },
   {       "b73.07", 0x00010000, 0x5cffa42c, 0, 0, 0, },
   {       "b73.08", 0x00080000, 0xf83dd808, 0, 0, 0, },
   {       "b73.09", 0x00080000, 0x537c647f, 0, 0, 0, },
   {       "b73.12", 0x00010000, 0xd1d24fab, 0, 0, 0, },
   {       "b73.13", 0x00020000, 0xe2874601, 0, 0, 0, },
   {       "b73.14", 0x00010000, 0x0dfb51d4, 0, 0, 0, },
   {           NULL,          0,          0, 0, 0, 0, },
};

#if 0
static struct INPUT_INFO input_heavy_unit[] =
{
   INP1( COIN1, 0x020002, 0x10 ),
   INP1( COIN2, 0x020002, 0x20 ),
   INP0( TILT, 0x020002, 0x02 ),
   INP0( SERVICE, 0x020002, 0x01 ),

   INP0( P1_START, 0x020002, 0x04 ),
   INP0( P1_UP, 0x020000, 0x01 ),
   INP0( P1_DOWN, 0x020000, 0x02 ),
   INP0( P1_LEFT, 0x020000, 0x08 ),
   INP0( P1_RIGHT, 0x020000, 0x04 ),
   INP0( P1_B1, 0x020000, 0x10 ),
   INP0( P1_B2, 0x020000, 0x20 ),

   INP0( P2_START, 0x020002, 0x08 ),
   INP0( P2_UP, 0x020001, 0x01 ),
   INP0( P2_DOWN, 0x020001, 0x02 ),
   INP0( P2_LEFT, 0x020001, 0x08 ),
   INP0( P2_RIGHT, 0x020001, 0x04 ),
   INP0( P2_B1, 0x020001, 0x10 ),
   INP0( P2_B2, 0x020001, 0x20 ),

   END_INPUT
};

static struct DSW_DATA dsw_data_heavy_unit_0[] =
{
   { MSG_DSWA_BIT1,           0x01, 0x02 },
   { MSG_OFF,                 0x01},
   { MSG_ON,                  0x00},
   { MSG_DSWA_BIT2,           0x02, 0x02 },
   { MSG_OFF,                 0x02},
   { MSG_ON,                  0x00},
   { MSG_TEST_MODE,           0x04, 0x02 },
   { MSG_OFF,                 0x04},
   { MSG_ON,                  0x00},
   { MSG_DEMO_SOUND,          0x08, 0x02 },
   { MSG_ON,                  0x08},
   { MSG_OFF,                 0x00},
   { MSG_COIN1,               0x30, 0x04 },
   { MSG_1COIN_1PLAY,         0x30},
   { MSG_1COIN_2PLAY,         0x20},
   { MSG_2COIN_1PLAY,         0x10},
   { MSG_3COIN_1PLAY,         0x00},
   { MSG_COIN2,               0xC0, 0x04 },
   { MSG_1COIN_1PLAY,         0xC0},
   { MSG_1COIN_2PLAY,         0x80},
   { MSG_2COIN_1PLAY,         0x40},
   { MSG_3COIN_1PLAY,         0x00},
   { NULL,                    0,   },
};

static struct DSW_INFO dsw_heavy_unit[] =
{
   { 0x020004, 0xFF, dsw_data_heavy_unit_0 },
   { 0x020005, 0xFF, dsw_data_default_1 },
   { 0,        0,    NULL,      },
};
#endif

#if 0
static struct ROMSW_DATA romsw_data_heavy_unit_0[] =
{
   { "Taito Japan (notice)",     0x01 },
   { "Taito America",            0x02 },
   { "Taito Japan",              0x03 },
   { "Tad Corporation",          0x04 },
   { NULL,                       0    },
};
#endif

static struct YM2203interface ym2203_interface =
{
  1,			// 1 chip
  4000000,		// rate
  { 0x00ff20c0 },	// volume
  { 0 },
  { 0 },
  { 0 },
  { 0 },
  { NULL }
};

static struct SOUND_INFO sound_heavyunt[] =
{
   { SOUND_YM2203,  &ym2203_interface,  },
   { 0,             NULL,               },
};

#if 0
static struct ROMSW_INFO romsw_heavy_unit[] =
{
   { 0x007FFF, 0x01, romsw_data_heavy_unit_0 },
   { 0,        0,    NULL },
};
#endif


static UINT8 *GFX_BG0;
static UINT8 *GFX_BG0_SOLID;
static UINT8 *GFX_BG1;
static UINT8 *GFX_BG1_SOLID;

static UINT8 *RAM2;
static UINT8 *RAM3;
static UINT8 *RAM_INPUT;
static UINT8 *RAM_COLOUR;

/******************************************************************************/
/* MAIN Z80 ROM BANKING                                                       */
/******************************************************************************/

static void heavy_unit_bank_w(UINT16 offset, UINT8 data)
{
   offset &= 15;

   if(offset==0){

     z80_set_bank(1,data & 7);
   }
}

static void init_bank_rom(UINT8 *src, UINT8 *dst)
{
  z80_init_banks_area_ofs(1,src,0x20000,0x8000,0x4000,2);
}

/******************************************************************************/
/* SUB Z80 ROM BANKING                                                        */
/******************************************************************************/

static void heavy_unit_sub_bank_w(UINT16 offset, UINT8 data)
{
   offset &= 15;

   switch(offset){
   case 0x00:

     z80_set_bank(2,data & 3);

   break;
   case 0x02:

      switch_led(0,(data>>2)&1);		// Coin A [Coin Inserted]
      switch_led(1,(data>>3)&1);		// Coin B [Coin Inserted]
      switch_led(2,(data>>0)&1);		// Coin A [Ready for coins]
    //switch_led(3,(data>>1)&1);		// Coin B [Ready for coins]

   break;
   }
}

static void init_bank_rom_1(UINT8 *src, UINT8 *dst)
{
  z80_init_banks_area_ofs(2,src,0x10000,0x8000,0x4000,2);
}

/******************************************************************************/
/* SUB Z80 ROM BANKING                                                        */
/******************************************************************************/

static void heavy_unit_sub2_bank_w(UINT16 offset, UINT8 data)
{
   offset &= 15;

   switch(offset){
   case 0x00:

     z80_set_bank(3,data & 3);

   break;
   case 0x02:

      switch_led(0,(data>>2)&1);		// Coin A [Coin Inserted]
      switch_led(1,(data>>3)&1);		// Coin B [Coin Inserted]
      switch_led(2,(data>>0)&1);		// Coin A [Ready for coins]
    //switch_led(3,(data>>1)&1);		// Coin B [Ready for coins]

   break;
   }
}

static void init_bank_rom_2(UINT8 *src, UINT8 *dst)
{
  z80_init_banks_area_ofs(3,src,0x10000,0x8000,0x4000,2);
}

/******************************************************************************/
/* INPUT + DSW                                                                */
/******************************************************************************/

static UINT32 port_unk;

static UINT32 sound_command;
static UINT32 sound_nmi;

static UINT8 heavy_unit_main_port_rb(UINT16 offset)
{
   UINT8 ret;

   offset &= 0xFF;

   switch(offset){
      default:
         print_debug("main_port_rb(%02x) [%04x]\n",offset, z80pc);
         ret = 0xFF;
      break;
   }

   return ret;
}

static void heavy_unit_main_port_wb(UINT16 offset, UINT8 data)
{
   offset &= 0xFF;

   switch(offset){
      case 0x01:
	z80_set_bank(1,data & 7);
      break;
      default:
         print_debug("main_port_wb(%02x,%02x) [%04x]\n", offset, data, z80pc);
      break;
   }
}


static UINT8 heavy_unit_sub_port_rb(UINT16 offset)
{
   UINT8 ret;

   offset &= 0xFF;

   switch(offset){
      case 0x0C:
         port_unk ++;
         ret = port_unk;
      break;
      default:
         print_debug("sub_port_rb(%02x) [%04x]\n",offset, z80pc);
         ret = 0xFF;
      break;
   }

   return ret;
}

static void heavy_unit_sub_port_wb(UINT16 offset, UINT8 data)
{
   offset &= 0xFF;

   switch(offset){
      case 0x00:
	z80_set_bank(2,data & 3);
      break;
      case 0x02:
         sound_command = data;
         sound_nmi = 1;
      break;
      default:
         print_debug("sub_port_wb(%02x,%02x) [%04x]\n", offset, data, z80pc);
      break;
   }
}


static UINT8 heavyunt_sound_port_rb(UINT16 offset)
{
   UINT8 ret;

   offset &= 0xFF;

   switch(offset){
      case 0x02:
      case 0x03:
         ret = YM2203AReadZ80(offset & 1);
      break;
      case 0x04:
         ret = sound_command;
      break;
      default:
         print_debug("sound_port_rb(%02x) [%04x]\n", offset, z80pc);
         ret = 0xFF;
      break;
   }

   return ret;
}

static void heavyunt_sound_port_wb(UINT16 offset, UINT8 data)
{
   offset &= 0xFF;

   switch(offset){
      case 0x00:
	z80_set_bank(3,data & 3);
      break;
      case 0x02:
      case 0x03:
         YM2203AWriteZ80(offset & 1, data);
      break;
      default:
         print_debug("sound_port_wb(%02x,%02x) [%04x]\n", offset, data, z80pc);
      break;
   }
}

/******************************************************************************/
/* LOAD SAVE DATA                                                             */
/******************************************************************************/

static void DrawNibble0(UINT8 *out, int plane, UINT8 c)
{
   int count, t;

   count = 4;
      do {
         t = c & 1;
         *out = t << plane;
         out++;
         c >>= 1;
      } while(--count);
}

static void DrawNibble(UINT8 *out, int plane, UINT8 c)
{
   int count, t;

   count = 4;
      do {
         t = c & 1;
         *out |= t << plane;
         out++;
         c >>= 1;
      } while(--count);
}

static void load_heavyunt(void)
{
  int ta,tb,tc;
   UINT8 *TMP;

   RAMSize=0x10000+0x10000+0x10+0x10000+0x10000+0x400;

   if(!(RAM=AllocateMem(RAMSize))) return;

   if(!(TMP=AllocateMem(0x080000))) return;

   RAM2       = RAM+0x10000;
   RAM_INPUT  = RAM+0x10000+0x10000;
   Z80ROM     = RAM+0x10000+0x10000+0x10;
   RAM3       = RAM+0x10000+0x10000+0x10+0x10000;
   RAM_COLOUR = RAM+0x10000+0x10000+0x10+0x10000+0x10000;

   if(!load_rom("b73.13",TMP,0x20000)) return;	// Z80 MAIN ROM

   // Skip Idle Z80

   TMP[0x1B2C]=0xD3;  // OUTA (AAh)
   TMP[0x1B2D]=0xAA;  //

   SetStopZ80BMode2(0x1B22);

   init_bank_rom(TMP,ROM);

   memset(RAM, 0x00, 0x10000);
   memcpy(RAM, TMP,  0x0C000);

   AddZ80BROMBase(RAM, 0x0038, 0x0066);

   AddZ80BReadByte(0x0000, 0xBFFF, NULL,			NULL);		// Z80 ROM + BANK
   AddZ80BReadByte(0xC000, 0xDFFF, NULL,			RAM +0xC000);	// RAM
   AddZ80BReadByte(0xE000, 0xFFFF, NULL,			RAM2+0xE000);	// COMMON RAM
   AddZ80BReadByte(0x0000, 0xFFFF, DefBadReadZ80,		NULL);		// <bad reads>
   AddZ80BReadByte(-1, -1, NULL, NULL);

   AddZ80BWriteByte(0xC000, 0xDFFF, NULL,			RAM +0xC000);	// RAM
   AddZ80BWriteByte(0xE000, 0xFFFF, NULL,			RAM2+0xE000);	// COMMON RAM
   AddZ80BWriteByte(0x0000, 0xFFFF, DefBadWriteZ80,		NULL);		// <bad writes>
   AddZ80BWriteByte(-1, -1, NULL, NULL);

   AddZ80BReadPort(0x00, 0xFF, heavy_unit_main_port_rb,		NULL);		// PORT
   AddZ80BReadPort(0x00, 0xFF, DefBadReadZ80,			NULL);		// <bad reads>
   AddZ80BReadPort(-1, -1, NULL, NULL);

   AddZ80BWritePort(0xAA, 0xAA, StopZ80BMode2,			NULL);		// Trap Idle Z80
   AddZ80BWritePort(0x00, 0xFF, heavy_unit_main_port_wb,	NULL);		// PORT
   AddZ80BWritePort(0x00, 0xFF, DefBadWriteZ80,			NULL);		// <bad reads>
   AddZ80BWritePort(-1, -1, NULL, NULL);

   AddZ80BInit();

   if(!load_rom("b73.14",TMP+0x00000,0x10000)) return;	// Z80 SUB ROM
/*
   // Skip Idle Z80

   TMP[0x0089]=0xD3;  // OUTA (AAh)
   TMP[0x008A]=0xAA;  //

   SetStopZ80CMode2(0x0083);
*/
   init_bank_rom_1(TMP,NULL);

   memset(RAM2, 0x00, 0x10000);
   memcpy(RAM2, TMP, 0x0C000);

   AddZ80CROMBase(RAM2, 0x0038, 0x0066);

   AddZ80CReadByte(0x0000, 0xBFFF, NULL,			NULL);		// Z80 ROM + BANK
   AddZ80CReadByte(0xC000, 0xFFFF, NULL,			RAM2+0xC000);	// RAM
   AddZ80CReadByte(0x0000, 0xFFFF, DefBadReadZ80,		NULL);		// <bad reads>
   AddZ80CReadByte(-1, -1, NULL, NULL);

   AddZ80CWriteByte(0xC000, 0xFFFF, NULL,			RAM2+0xC000);	// RAM
   AddZ80CWriteByte(0x0000, 0xFFFF, DefBadWriteZ80,		NULL);		// <bad writes>
   AddZ80CWriteByte(-1, -1, NULL, NULL);

   AddZ80CReadPort(0x00, 0xFF, heavy_unit_sub_port_rb,		NULL);		// PORT
   AddZ80CReadPort(0x00, 0xFF, DefBadReadZ80,			NULL);		// <bad reads>
   AddZ80CReadPort(-1, -1, NULL, NULL);

   AddZ80CWritePort(0xAA, 0xAA, StopZ80CMode2,			NULL);		// Trap Idle Z80
   AddZ80CWritePort(0x00, 0xFF, heavy_unit_sub_port_wb,		NULL);		// PORT
   AddZ80CWritePort(0x00, 0xFF, DefBadWriteZ80,			NULL);		// <bad reads>
   AddZ80CWritePort(-1, -1, NULL, NULL);

   AddZ80CInit();

   if(!load_rom("b73.12",TMP,0x10000)) return;			// Z80 SOUND ROM

   // Skip Idle Z80

   TMP[0x00CB]=0xD3;  // OUTA (AAh)
   TMP[0x00CC]=0xAA;  //

   SetStopZ80DMode2(0x00CB);

   init_bank_rom_2(TMP,NULL);

   memset(RAM3, 0x00, 0x10000);
   memcpy(RAM3, TMP, 0x0C000);

   AddZ80DROMBase(RAM3, 0x0038, 0x0066);

   AddZ80DReadByte(0x0000, 0xBFFF, NULL,			NULL);		// Z80 ROM + BANK
   AddZ80DReadByte(0xC000, 0xFFFF, NULL,			RAM3+0xC000);	// RAM
   AddZ80DReadByte(0x0000, 0xFFFF, DefBadReadZ80,		NULL);		// <bad reads>
   AddZ80DReadByte(-1, -1, NULL, NULL);

   AddZ80DWriteByte(0xC000, 0xFFFF, NULL,			RAM3+0xC000);	// RAM
   AddZ80DWriteByte(0x0000, 0xFFFF, DefBadWriteZ80,		NULL);		// <bad writes>
   AddZ80DWriteByte(-1, -1, NULL, NULL);

   AddZ80DReadPort(0x00, 0xFF, heavyunt_sound_port_rb,	NULL);		// PORT
   AddZ80DReadPort(0x00, 0xFF, DefBadReadZ80,			NULL);		// <bad reads>
   AddZ80DReadPort(-1, -1, NULL, NULL);

   AddZ80DWritePort(0xAA, 0xAA, StopZ80DMode2,			NULL);		// Trap Idle Z80
   AddZ80DWritePort(0x00, 0xFF, heavyunt_sound_port_wb,	NULL);		// PORT
   AddZ80DWritePort(0x00, 0xFF, DefBadWriteZ80,			NULL);		// <bad reads>
   AddZ80DWritePort(-1, -1, NULL, NULL);

   AddZ80DInit();

   if(!(GFX=AllocateMem(0x100000+0x100000))) return;

   GFX_BG0 = GFX+0x000000;
   GFX_BG1 = GFX+0x100000;

   tb=0;
   if(!load_rom("b73.06", TMP+0x00000, 0x10000)) return;
   if(!load_rom("b73.08", TMP+0x10000, 0x10000)) return;
   memset(TMP+0x20000,0x00,0x10000);
   memset(TMP+0x30000,0x00,0x10000);
   if(!load_rom("b73.07", TMP+0x40000, 0x10000)) return;
   if(!load_rom("b73.09", TMP+0x50000, 0x10000)) return;
   memset(TMP+0x60000,0x00,0x10000);
   memset(TMP+0x70000,0x00,0x10000);
   for(ta=0;ta<0x40000;ta+=2,tb+=8){
      DrawNibble0(GFX_BG1+tb+0, 0, (UINT8) (TMP[ta+0x00000]&15) );
      DrawNibble (GFX_BG1+tb+0, 1, (UINT8) (TMP[ta+0x00000]>>4) );
      DrawNibble (GFX_BG1+tb+0, 2, (UINT8) (TMP[ta+0x40000]&15) );
      DrawNibble (GFX_BG1+tb+0, 3, (UINT8) (TMP[ta+0x40000]>>4) );
      DrawNibble0(GFX_BG1+tb+4, 0, (UINT8) (TMP[ta+0x00001]&15) );
      DrawNibble (GFX_BG1+tb+4, 1, (UINT8) (TMP[ta+0x00001]>>4) );
      DrawNibble (GFX_BG1+tb+4, 2, (UINT8) (TMP[ta+0x40001]&15) );
      DrawNibble (GFX_BG1+tb+4, 3, (UINT8) (TMP[ta+0x40001]>>4) );
   }

/*    tb=0; */
/*    if(!load_rom("b12-16", TMP+0x00000, 0x10000)) return; */
/*    if(!load_rom("b12-18", TMP+0x10000, 0x10000)) return; */
/*    if(!load_rom("b12-20", TMP+0x20000, 0x10000)) return; */
/*    memset(TMP+0x30000,0x00,0x10000); */
/*    if(!load_rom("b12-15", TMP+0x40000, 0x10000)) return; */
/*    if(!load_rom("b12-17", TMP+0x50000, 0x10000)) return; */
/*    if(!load_rom("b12-19", TMP+0x60000, 0x10000)) return; */
/*    memset(TMP+0x70000,0x00,0x10000); */
/*    for(ta=0;ta<0x40000;ta+=2,tb+=8){ */
/*       DrawNibble0(GFX_BG0+tb+0, 0, (UINT8) (TMP[ta+0x00000]&15) ); */
/*       DrawNibble (GFX_BG0+tb+0, 1, (UINT8) (TMP[ta+0x00000]>>4) ); */
/*       DrawNibble (GFX_BG0+tb+0, 2, (UINT8) (TMP[ta+0x40000]&15) ); */
/*       DrawNibble (GFX_BG0+tb+0, 3, (UINT8) (TMP[ta+0x40000]>>4) ); */
/*       DrawNibble0(GFX_BG0+tb+4, 0, (UINT8) (TMP[ta+0x00001]&15) ); */
/*       DrawNibble (GFX_BG0+tb+4, 1, (UINT8) (TMP[ta+0x00001]>>4) ); */
/*       DrawNibble (GFX_BG0+tb+4, 2, (UINT8) (TMP[ta+0x40001]&15) ); */
/*       DrawNibble (GFX_BG0+tb+4, 3, (UINT8) (TMP[ta+0x40001]>>4) ); */
/*    } */

   if(!load_rom("b73.04", TMP+0x00000, 0x400)) return;
   if(!load_rom("b73.03", TMP+0x00400, 0x400)) return;
   if(!load_rom("b73.05", TMP+0x00800, 0x400)) return;

   for(ta=0;ta<0x200;ta++){

      tb  = ta & 0x1F0;
      tb |= 15 - (ta & 15);

      tc  = (TMP[tb+0x000]&15)<<8;
      tc |= (TMP[tb+0x400]&15)<<4;
      tc |= (TMP[tb+0x800]&15)<<0;

      WriteWord(&RAM_COLOUR[ta<<1], tc);

   }
   FreeMem(TMP);
   GFX_BG0_SOLID = make_solid_mask_8x8(GFX_BG0, 0x4000);
   GFX_BG1_SOLID = make_solid_mask_8x8(GFX_BG1, 0x4000);

   InitPaletteMap(RAM_COLOUR, 0x20, 0x10, 0x1000);
}

static void execute_heavyunt(void)
{
   cpu_execute_cycles(CPU_Z80_1, CPU_FRAME_MHz(8,60));	// MAIN Z80 8MHz (60fps)
      print_debug("Z80PC_MAIN:%04x\n",z80pc);
   cpu_interrupt(CPU_Z80_1, 0x38);

   cpu_execute_cycles(CPU_Z80_2, CPU_FRAME_MHz(8,60));	// SUB Z80 8MHz (60fps)
      print_debug("Z80PC_SUB:%04x\n",z80pc);
   cpu_interrupt(CPU_Z80_2, 0x38);

   if(sound_nmi){
      sound_nmi = 0;
      cpu_int_nmi(CPU_Z80_3);
   }

   cpu_execute_cycles(CPU_Z80_3, CPU_FRAME_MHz(8,60));	// SUB Z80 8MHz (60fps)
      print_debug("Z80PC_SOUND:%04x\n",z80pc);
   cpu_interrupt(CPU_Z80_3, 0x38);
}

static void draw_heavy_unit(void)
{
  int x,y,ta;
  int sx,sy,offs,goffs,gfx_offs,gfx_num,gfx_attr,height,xc,yc;
  UINT8 *map,*RAM_BG;
   ClearPaletteMap();

   clear_game_screen(0);
   // TC0010VCU 0

   RAM_BG = RAM+0xC000;

   sx = 0;
   for (offs=0;offs<0x600;offs+=4)
   {
      if (*(UINT32 *)(&RAM_BG[offs + 0]) == 0) continue;

      gfx_num = RAM_BG[offs + 1];
      gfx_attr = RAM_BG[offs + 3];

      if ((gfx_num & 0x80) == 0){     /* 16x16 sprites */

         gfx_offs = gfx_num & 0x7F;
         gfx_offs <<= 3;
         gfx_offs += 0x0600;
         height = 2;

         sx  = RAM_BG[offs + 2];
         sx |= (gfx_attr & 0x40) << 2;

      }
      else{                           /* 16x256 columns */

         gfx_offs = gfx_num & 0x3F;
         gfx_offs <<= 7;
         gfx_offs += 0x0A00;
         height = 32;

         if(gfx_num & 0x40)
            sx += 16;
         else
            sx  = RAM_BG[offs + 2];
            sx |= (gfx_attr & 0x40) << 2;
      }

      sy = 256 - height*8 - (RAM_BG[offs + 0]);

      for (xc=0;xc<2;xc++)
      {
         goffs = gfx_offs;

         for (yc=0;yc<height;yc++,goffs+=2)
         {
            ta = ReadWord(&RAM_BG[goffs]);
            if(GFX_BG0_SOLID[ta&0x3FFF]){

            MAP_PALETTE_MAPPED(
               Map_12bit_xRGB,
               (gfx_attr & 0x0F) | ((ta & 0xC000) >> 14),
               16,
               map
            );

            ta &= 0x3FFF;

            x = (sx + (xc << 3)) & 0xff;
            y = ((sy + (yc << 3)) & 0xff) -16;

            if ((x > -8) && (y > -8) && (x < 256) && (y < 224))
            {
               if(GFX_BG0_SOLID[ta] == 1)
                  Draw8x8_Trans_Mapped_8(&GFX_BG0[ta<<6], x+32, y+32, map);
               else
                  Draw8x8_Mapped_8(&GFX_BG0[ta<<6], x+32, y+32, map);
            }
            }
         }

         gfx_offs += height<<1;
      }
   }

   // TC0010VCU 1

   RAM_BG = RAM3+0x8000;

   sx = 0;
   for (offs=0;offs<0x600;offs+=4)
   {
      if (*(UINT32 *)(&RAM_BG[offs + 0]) == 0) continue;

      gfx_num = RAM_BG[offs + 1];
      gfx_attr = RAM_BG[offs + 3];

      if ((gfx_num & 0x80) == 0){     /* 16x16 sprites */

         gfx_offs = gfx_num & 0x7F;
         gfx_offs <<= 3;
         gfx_offs += 0x0600;
         height = 2;

         sx  = RAM_BG[offs + 2];
         sx |= (gfx_attr & 0x40) << 2;

      }
      else{                           /* 16x256 columns */

         gfx_offs = gfx_num & 0x3F;
         gfx_offs <<= 7;
         gfx_offs += 0x0A00;
         height = 32;

         if(gfx_num & 0x40)
            sx += 16;
         else
            sx  = RAM_BG[offs + 2];
            sx |= (gfx_attr & 0x40) << 2;
      }

      sy = 256 - height*8 - (RAM_BG[offs + 0]);

      for (xc=0;xc<2;xc++)
      {
         goffs = gfx_offs;

         for (yc=0;yc<height;yc++,goffs+=2)
         {
            ta = ReadWord(&RAM_BG[goffs]);
            if(GFX_BG1_SOLID[ta&0x3FFF]){

            MAP_PALETTE_MAPPED(
               Map_12bit_xRGB,
               (gfx_attr & 0x0F) | ((ta & 0xC000) >> 14) | 0x10,
               16,
               map
            );

            ta &= 0x3FFF;

            x = (sx + (xc << 3)) & 0xff;
            y = ((sy + (yc << 3)) & 0xff) -16;

            if ((x > -8) && (y > -8) && (x < 256) && (y < 224))
            {
               if(GFX_BG1_SOLID[ta] == 1)
                  Draw8x8_Trans_Mapped_8(&GFX_BG1[ta<<6], x+32, y+32, map);
               else
                  Draw8x8_Mapped_8(&GFX_BG1[ta<<6], x+32, y+32, map);
            }
            }
         }

         gfx_offs += height<<1;
      }
   }
}

/*

CTRL RAM (OBJECT RAM)

0x0600 bytes

 0 |xxxxxxxx| Sprite Y (low)
 1 |x.......| 16x16 sprite / 16x256 column select
 1 |.x......| X Chain (16x256 column)
 1 |..xxxxxx| Sprite num (16x256 column)
 1 |.xxxxxxx| Sprite num (16x16 sprite)
 2 |xxxxxxxx| Sprite X (low)
 3 |.x......| Sprite X (high)
 3 |....xx..| Colour Bank (high)
 3 |......xx| Colour Bank (low mirror)

DATA RAM (SCREEN RAM)

0x0400 bytes (16x16 sprite)
0x1600 bytes (16x256 column)

 0 |xx......| Colour Bank (low)
 0 |..xxxxxx| Tile (high)
 1 |xxxxxxxx| Tile (low)

*/
static struct VIDEO_INFO video_heavyunt =
{
   draw_heavy_unit,
   256,
   224,
   32,
   VIDEO_ROTATE_NORMAL,
};
static struct DIR_INFO dir_heavyunt[] =
{
   { "heavy_unit", },
   { "heavyunt", },
   { NULL, },
};
GAME( heavyunt, "Heavy Unit", KANEKO, 1988, GAME_NOT_WORKING,
	.input = NULL, //heavy_unit,
	.dsw = NULL, //heavy_unit,
	.romsw = NULL, //heavy_unit,
	.video = &video_heavyunt,
	.exec = execute_heavyunt,
	.board = "B73",
	.sound = sound_heavyunt,
);

