/******************************************************************************/
/*                                                                            */
/*                 TAITO ?-SYSTEM (C) 1986 TAITO CORPORATION                  */
/*                                                                            */
/*                       Z80+Z80+YM2203 [KIKI KAI KAI]                        */
/*                                                                            */
/******************************************************************************/

#include "gameinc.h"
#include "flstory.h"
#include "tc220ioc.h"
#include "taitosnd.h"
#include "2203intf.h"
#include "decode.h"
#include "sasound.h"		// sample support routines

static struct DIR_INFO fairy_land_story_dirs[] =
{
   { "fairy_land_story", },
   { "flstory", },
   { NULL, },
};

static struct ROM_INFO fairy_land_story_roms[] =
{
   {   "cpu-a45.15", 0x00004000, 0xf03fc969, 0, 0, 0, },
   {   "cpu-a45.16", 0x00004000, 0x311aa82e, 0, 0, 0, },
   {   "cpu-a45.17", 0x00004000, 0xa2b5d17d, 0, 0, 0, },
   {       "snd.22", 0x00002000, 0xd58b201d, 0, 0, 0, },
   {       "snd.23", 0x00002000, 0x25e7fd9d, 0, 0, 0, },
   {   "vid-a45.06", 0x00004000, 0xdc856a75, 0, 0, 0, },
   {   "vid-a45.07", 0x00004000, 0xaa4b0762, 0, 0, 0, },
   {   "vid-a45.08", 0x00004000, 0xd0b028ca, 0, 0, 0, },
   {   "vid-a45.09", 0x00004000, 0x8336be58, 0, 0, 0, },
   {   "vid-a45.18", 0x00004000, 0x6f08f69e, 0, 0, 0, },
   {   "vid-a45.19", 0x00004000, 0x2b572dc9, 0, 0, 0, },
   {   "vid-a45.20", 0x00004000, 0x1b0edf34, 0, 0, 0, },
   {   "vid-a45.21", 0x00004000, 0xfc382bd1, 0, 0, 0, },
   {           NULL,          0,          0, 0, 0, 0, },
};

static struct INPUT_INFO fairy_land_story_inputs[] =
{
   { KB_DEF_COIN1,        MSG_COIN1,               0x020005, 0x01, BIT_ACTIVE_1 },
   { KB_DEF_COIN2,        MSG_COIN2,               0x020005, 0x02, BIT_ACTIVE_1 },
   { KB_DEF_TILT,         MSG_TILT,                0x020000, 0x04, BIT_ACTIVE_0 },
   { KB_DEF_SERVICE,      MSG_SERVICE,             0x020000, 0x01, BIT_ACTIVE_0 },

   { KB_DEF_P1_START,     MSG_P1_START,            0x020000, 0x08, BIT_ACTIVE_0 },
   { KB_DEF_P1_UP,        MSG_P1_UP,               0x020001, 0x01, BIT_ACTIVE_0 },
   { KB_DEF_P1_DOWN,      MSG_P1_DOWN,             0x020001, 0x02, BIT_ACTIVE_0 },
   { KB_DEF_P1_LEFT,      MSG_P1_LEFT,             0x020001, 0x08, BIT_ACTIVE_0 },
   { KB_DEF_P1_RIGHT,     MSG_P1_RIGHT,            0x020001, 0x04, BIT_ACTIVE_0 },
   { KB_DEF_P1_B1,        MSG_P1_B1,               0x020001, 0x10, BIT_ACTIVE_0 },
   { KB_DEF_P1_B2,        MSG_P1_B2,               0x020001, 0x20, BIT_ACTIVE_0 },

   { KB_DEF_P2_START,     MSG_P2_START,            0x020000, 0x10, BIT_ACTIVE_0 },
   { KB_DEF_P2_UP,        MSG_P2_UP,               0x020002, 0x01, BIT_ACTIVE_0 },
   { KB_DEF_P2_DOWN,      MSG_P2_DOWN,             0x020002, 0x02, BIT_ACTIVE_0 },
   { KB_DEF_P2_LEFT,      MSG_P2_LEFT,             0x020002, 0x08, BIT_ACTIVE_0 },
   { KB_DEF_P2_RIGHT,     MSG_P2_RIGHT,            0x020002, 0x04, BIT_ACTIVE_0 },
   { KB_DEF_P2_B1,        MSG_P2_B1,               0x020002, 0x10, BIT_ACTIVE_0 },
   { KB_DEF_P2_B2,        MSG_P2_B2,               0x020002, 0x20, BIT_ACTIVE_0 },

   { 0,                   NULL,                    0,        0,    0            },
};

static struct DSW_DATA dsw_data_fairy_land_story_0[] =
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

static struct DSW_INFO fairy_land_story_dsw[] =
{
   { 0x020003, 0xFF, dsw_data_fairy_land_story_0 },
   { 0x020004, 0xFF, dsw_data_default_1 },
   { 0,        0,    NULL,      },
};

static struct VIDEO_INFO fairy_land_story_video =
{
   DrawFLStory,
   256,
   256,
   32,
   VIDEO_ROTATE_NORMAL | VIDEO_NEEDS_8BPP,
};

static struct YM2203interface ym2203_interface =
{
   1,
   4000000,
   { 0x00ff20c0 },
   {0},
   {0},
   {0},
   {0},
   {NULL}
};

static struct SOUND_INFO fairy_land_story_sound[] =
{
   { SOUND_YM2203,  &ym2203_interface,    },
   { 0,             NULL,                 },
};

GAME( fairy_land_story ,
   fairy_land_story_dirs,
   fairy_land_story_roms,
   fairy_land_story_inputs,
   fairy_land_story_dsw,
   NULL,

   LoadFLStory,
   ClearFLStory,
   &fairy_land_story_video,
   ExecuteFLStoryFrame,
   "flstory",
   "Fairy Land Story",
   NULL,
   COMPANY_ID_TAITO,
   "A45",
   1985,
   fairy_land_story_sound,
   GAME_PLATFORM | GAME_NOT_WORKING
);

static int romset;

static UINT8 *RAM2;
static UINT8 *ROM2;
static UINT8 *RAM_INPUT;
static UINT8 *RAM_COLOUR;

/* static int spr_mask; */
/* static int tile_mask; */

/*

0000-BFFF BASE ROM
C000-C7FF BG0 RAM
D400-D401 SOUND COMM
DC00-DCBF MCU?
DD00-DEFF ?
E000-E7FF WORK RAM

---

0000-3FFF BASE ROM
C000-C7FF WORK RAM
D800-D800 SOUND COMM

Supported romsets:

0 - A45 - Fairyland Story       - 1985 - ?-System

Todo:

- Many things.

*/

/******************************************************************************/
/* K-SYSTEM MCU RAM                                                           */
/******************************************************************************/

static int mcu_enable;

#if 0
// Unused !!!
static void FLStoryMCUWrite(UINT16 offset, UINT8 data)
{
   offset&=0xFF;

   print_debug("MCU Write: %02x:%02x [%04x]\n", offset, data, z80pc);

   if(mcu_enable!=0){

   switch(offset){
      case 0x01:		// [CREDIT COUNTER] Used by MCU in credit handling
         RAM[offset+0xE800] = data;
      break;
      case 0x0B:		// [DSW A WRITE] Used by MCU in credit handling
         RAM[offset+0xE800] = data;
      break;
      case 0x0C:		// [DSW B WRITE] Used by MCU in credit handling
         RAM[offset+0xE800] = data;
      break;
      case 0x18:		// [LIVES WRITE] CURRENT PLAYER (0x00 - 0x03)
         RAM[offset+0xE800] = data;
      break;
      case 0x19:		// [????? WRITE] CURRENT PLAYER
         RAM[offset+0xE800] = data;
      break;
      case 0x1A:		// [????? WRITE] CURRENT PLAYER
         RAM[offset+0xE800] = data;
      break;
      case 0x90:		// [?????]
      case 0x91:		// [?????]
      case 0x92:		// [?????]
      case 0x93:		// [?????]
      case 0x94:		// [?????]
      case 0x95:		// [?????]
      case 0x96:		// [?????]
      case 0x97:		// [?????]
         RAM[offset+0xE800] = data;
      break;
      case 0xE0:		// [????? WRITE] SOMETHING
         RAM[offset+0xE800] = data;
      break;
      case 0xE1:		// [?????]
      case 0xE2:		// [?????]
      case 0xE3:		// [?????]
      case 0xE4:		// [?????]
      case 0xE5:		// [?????]
      case 0xE6:		// [?????]
      case 0xE7:		// [?????]
      case 0xE8:		// [?????]
      case 0xE9:		// [?????]
      case 0xEA:		// [?????]
      case 0xEB:		// [?????]
      case 0xEC:		// [?????]
      case 0xED:		// [?????]
      case 0xEE:		// [?????]
      case 0xEF:		// [?????]
         RAM[offset+0xE800] = data;
      break;
      default:
         RAM[offset+0xE800] = data;
      break;
   }

   }
   else{

   RAM[offset+0xE800] = data;

   }
}

static UINT8 FLStoryMCURead(UINT16 offset)
{
   offset&=0xFF;

   print_debug("MCU Read: %02x:?? [%04x]\n", offset, z80pc);

   if(mcu_enable!=0){

   switch(offset){
      case 0x01:		// [CREDIT COUNTER]
         return RAM[offset+0xE800];
      break;
      case 0x02:		// [P1 INPUT]
         return RAM_INPUT[1];
      break;
      case 0x03:		// [P2 INPUT]
         return RAM_INPUT[2];
      break;
      case 0x04:		// [COIN INPUT]
         return RAM_INPUT[5];
      break;
      case 0x05:		// [I/O OK RESPONSE] 0x01 = ERROR
         return 0x00;
      break;
      case 0x06:
         return 0xff;
      break;
      case 0x07:
         return 0x03;
      break;
      case 0x0a:
         if(key[KEY_H]) return 0x03;
         else return 0x00;
         //return RAM[offset+0xE800];	//0x01;
      break;
      case 0x0d:
         return RAM[offset+0xE800];	//0x01;
      break;
      case 0x18:		// [LIVES] CURRENT PLAYER (0x00 - 0x03) (OR CURRENT PLAYER? 0/1 = P1/P2???)
         return RAM[offset+0xE800];
      break;
      case 0x19:		// [?????] CURRENT PLAYER
         return RAM[offset+0xE800];
      break;
      case 0x1A:		// [?????] CURRENT PLAYER
         return RAM[offset+0xE800];
      break;
      case 0x1B:		// [INPUT] CURRENT PLAYER (P1/P2)
         return RAM_INPUT[1];
      break;
      case 0x90:		// [?????]
      case 0x91:		// [?????]
      case 0x92:		// [?????]
      case 0x93:		// [?????]
      case 0x94:		// [?????]
      case 0x95:		// [?????]
      case 0x96:		// [?????]
      case 0x97:		// [?????]
         return RAM[offset+0xE800];
      break;
      case 0xD0:
         return 0xFF;
      break;
      case 0xE0:		// [?????] 8x = READY  0x = WAITING
         return RAM[offset+0xE800] | 0x80;
      break;
      case 0xE1:		// [?????]
      case 0xE2:		// [?????]
      case 0xE3:		// [?????]
      case 0xE4:		// [?????]
      case 0xE5:		// [?????]
      case 0xE6:		// [?????]
      case 0xE7:		// [?????]
      case 0xE8:		// [?????]
      case 0xE9:		// [?????]
      case 0xEA:		// [?????]
      case 0xEB:		// [?????]
      case 0xEC:		// [?????]
      case 0xED:		// [?????]
      case 0xEE:		// [?????]
      case 0xEF:		// [?????]
         return RAM[offset+0xE800];
      break;
      case 0xF1:
         return 0xb3;
      break;
      case 0xF0:
         return 0xff;
      break;
      default:
         return 0x00;
      break;
   }

   }
   else{

   return RAM[offset+0xE800];

   }
}

static void FLStoryF008Write(UINT16 offset, UINT8 data)
{
   print_debug("??? Write: %04x:%02x [%04x]\n", offset, data, z80pc);

   // Just a hack for the MCU

   if(data==0x1E) mcu_enable = 1;
   else mcu_enable = 0;
}

static void FLStoryF018Write(UINT16 offset, UINT8 data)
{
}

/******************************************************************************/
/* K-SYSTEM INPUT RAM                                                         */
/******************************************************************************/

static void FLStoryInputWrite(UINT16 offset, UINT8 data)
{
   print_debug("Input Write: %04x:%02x [%04x]\n", offset, data, z80pc);
}

static UINT8 FLStoryInputRead(UINT16 offset)
{
   return RAM_INPUT[0];
}

/******************************************************************************/
/* K-SYSTEM YM2203 AND DSW ACCESS                                             */
/******************************************************************************/

static UINT8 kiki_ym2203_reg;

// still unused !
static UINT8 FLStoryYM2203Read(UINT16 offset)
{
   //print_debug("YM2203 RB:%04x/?? [%04x]\n", offset, z80pc);

   if((offset&1)==0){
      return 0; //YM2203_status_port_0_r(0);
   }
   else{
      switch(kiki_ym2203_reg){
         case 0x0e: return get_dsw(0);
         case 0x0f: return get_dsw(1);
         default:   return 0; //return YM2203_read_port_0_r(0);
      }
   }
}

static void FLStoryYM2203Write(UINT16 offset, UINT8 data)
{
   //print_debug("YM2203 WB:%04x/%02x [%04x]\n", offset, data, z80pc);

   if((offset&1)==0){
      kiki_ym2203_reg = data;
   }
   else{
      YM2203_control_port_0_w(0,kiki_ym2203_reg);
      YM2203_write_port_0_w(1,data);
   }
}
// end of unused functions !!!
#endif

/******************************************************************************/

static UINT8 s_byte;
static UINT8 s_stat;

static void FLStorySoundWriteMain(UINT16 offset, UINT8 data)
{
   if((offset&1)==0){
      print_debug("Main Sends: %02x\n",data);
      s_byte  = data;
      s_stat |= 0x02;
   }
   else{
      print_debug("(ERROR) Main Sends Status: %02x\n",data);
   }
}

static UINT8 FLStorySoundReadMain(UINT16 offset)
{
   if((offset&1)==0){
      print_debug("Main Reads: %02x\n",s_byte);
      s_stat &= ~0x02;
      return s_byte;
   }
   else{
      print_debug("Main Reads Status: %02x\n",s_stat);
      return s_stat;
   }
}

static void FLStorySoundWriteSub(UINT16 offset, UINT8 data)
{
   print_debug("Sub Sends: %02x\n",data);
   s_byte  = data;
   s_stat |= 0x02;
}

static UINT8 FLStorySoundReadSub(UINT16 offset)
{
   print_debug("Sub Reads: %02x\n",s_byte);
   s_stat &= ~0x02;
   return s_byte;
}


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

void LoadFLStory(void)
{
   int ta,tb;
   UINT8 *TMP;

   romset=0;
   mcu_enable=0;

   RAMSize=0x10000+0x10000+0x10+0x200;

   if(!(ROM=AllocateMem(0x20000+0x8000))) return;

   ROM2 = ROM+0x20000;

   if(!(RAM=AllocateMem(RAMSize))) return;

   RAM2       = RAM+0x10000;
   RAM_INPUT  = RAM+0x10000+0x10000;
   RAM_COLOUR = RAM+0x10000+0x10000+0x10;

   if(!load_rom("cpu-a45.15",ROM+0x00000,0x04000)) return;	// Z80 MAIN ROM
   if(!load_rom("cpu-a45.16",ROM+0x04000,0x04000)) return;	// Z80 MAIN ROM
   if(!load_rom("cpu-a45.17",ROM+0x08000,0x04000)) return;	// Z80 MAIN ROM

   // BAD HW

   ROM[0xBF59]=0xC9;

   // BAD ROM

   ROM[0xBFFA]=0xC9;

   // BAD SOUND PCB

   ROM[0x021A]=0x00;
   ROM[0x021B]=0x00;
   ROM[0x021C]=0x00;

   // BAD IO

   ROM[0x0225]=0x00;
   ROM[0x0226]=0x00;
   ROM[0x0227]=0x00;

   // Skip Idle Z80
   // -------------
/*
   ROM[0x485a]=0xC9;  // RET

   ROM[0x04FF]=0xC3;  // JP 005D
   ROM[0x0500]=0x5D;  //
   ROM[0x0501]=0x00;  //

   ROM[0x005D]=0x3A;  //
   ROM[0x005E]=0x90;  //
   ROM[0x005F]=0xDA;  //
   ROM[0x0060]=0xD3;  // OUTA (AAh)
   ROM[0x0061]=0xAA;  //

   SetStopZ80BMode2(0x0502);
*/
   memset(RAM+0x00000, 0x00, 0x10000);
   memcpy(RAM, ROM, 0x8000+0x4000);

   AddZ80BROMBase(RAM, 0x0038, 0x0066);

   AddZ80BReadByte(0x0000, 0xBFFF, NULL,			NULL);	// Z80 ROM
   AddZ80BReadByte(0xC000, 0xC7FF, NULL,			NULL);	// BG0 RAM
   AddZ80BReadByte(0xD400, 0xD401, FLStorySoundReadMain,	NULL);	// SOUND COMM
   //AddZ80BReadByte(0xDC00, 0xDCBF, NULL,			NULL);	// ?
   AddZ80BReadByte(0xDD00, 0xDEFF, NULL,			NULL);	// ?
   AddZ80BReadByte(0xE000, 0xE7FF, NULL,			NULL);	// WORK RAM
   AddZ80BReadByte(0x0000, 0xFFFF, DefBadReadZ80,		NULL);	// <bad reads>
   AddZ80BReadByte(-1, -1, NULL, NULL);

   AddZ80BWriteByte(0xC000, 0xC7FF, NULL,			NULL);	// BG0 RAM
   AddZ80BWriteByte(0xD400, 0xD401, FLStorySoundWriteMain,	NULL);	// SOUND COMM
   AddZ80BWriteByte(0xDC00, 0xDCBF, NULL,			NULL);	// ?
   AddZ80BWriteByte(0xDD00, 0xDEFF, NULL,			NULL);	// ?
   AddZ80BWriteByte(0xE000, 0xE7FF, NULL,			NULL);	// WORK RAM
   AddZ80BWriteByte(0x0000, 0xFFFF, DefBadWriteZ80,		NULL);	// <bad writes>
   AddZ80BWriteByte(-1, -1, NULL, NULL);

   AddZ80BReadPort(0x00, 0xFF, DefBadReadZ80,			NULL);	// <bad reads>
   AddZ80BReadPort(-1, -1, NULL, NULL);

   AddZ80BWritePort(0xAA, 0xAA, StopZ80BMode2,			NULL);	// Trap Idle Z80
   AddZ80BWritePort(0x00, 0xFF, DefBadWriteZ80,			NULL);	// <bad reads>
   AddZ80BWritePort(-1, -1, NULL, NULL);

   AddZ80BInit();

   if(!load_rom("snd.22",ROM2+0x0000,0x2000)) return;		// Z80 SUB ROM
   if(!load_rom("snd.23",ROM2+0x2000,0x2000)) return;		// Z80 SUB ROM

   // Skip Idle Z80
   // -------------

   ROM2[0x0167]=0xD3;  // OUTA (AAh)
   ROM2[0x0168]=0xAA;  //

   SetStopZ80CMode2(0x0160);

   memset(RAM2+0x00000, 0x00, 0x10000);
   memcpy(RAM2, ROM2, 0x4000);

   AddZ80CROMBase(RAM2, 0x0038, 0x0066);

   AddZ80CReadByte(0x0000, 0x3FFF, NULL,			NULL);	// Z80 ROM
   AddZ80CReadByte(0xC000, 0xC7FF, NULL,			NULL);	// WORK RAM
   AddZ80CReadByte(0xD800, 0xD800, FLStorySoundReadSub,		NULL);	// SOUND COMM
   AddZ80CReadByte(0x0000, 0xFFFF, DefBadReadZ80,		NULL);	// <bad reads>
   AddZ80CReadByte(-1, -1, NULL, NULL);

   AddZ80CWriteByte(0xC000, 0xC7FF, NULL,			NULL);	// WORK RAM
   AddZ80CWriteByte(0xD800, 0xD800, FLStorySoundWriteSub,	NULL);	// SOUND COMM
   AddZ80CWriteByte(0x0000, 0xFFFF, DefBadWriteZ80,		NULL);	// <bad writes>
   AddZ80CWriteByte(-1, -1, NULL, NULL);

   AddZ80CReadPort(0x00, 0xFF, DefBadReadZ80,			NULL);	// <bad reads>
   AddZ80CReadPort(-1, -1, NULL, NULL);

   AddZ80CWritePort(0xAA, 0xAA, StopZ80CMode2,			NULL);	// Trap Idle Z80
   AddZ80CWritePort(0x00, 0xFF, DefBadWriteZ80,			NULL);	// <bad reads>
   AddZ80CWritePort(-1, -1, NULL, NULL);

   AddZ80CInit();

   /*-----------------------*/

   if(!(GFX=AllocateMem(0x040000))) return;
   if(!(TMP=AllocateMem(0x020000))) return;

   tb=0;
   if(!load_rom("vid-a45.06", TMP+0x00000, 0x04000)) return;
   if(!load_rom("vid-a45.08", TMP+0x04000, 0x04000)) return;
   if(!load_rom("vid-a45.20", TMP+0x08000, 0x04000)) return;
   if(!load_rom("vid-a45.18", TMP+0x0C000, 0x04000)) return;
   if(!load_rom("vid-a45.07", TMP+0x10000, 0x04000)) return;
   if(!load_rom("vid-a45.09", TMP+0x14000, 0x04000)) return;
   if(!load_rom("vid-a45.21", TMP+0x18000, 0x04000)) return;
   if(!load_rom("vid-a45.19", TMP+0x1C000, 0x04000)) return;
   for(ta=0;ta<0x10000;ta+=2,tb+=8){
      DrawNibble0(&GFX[tb+0],0, (UINT8) (TMP[ta+0x00000]&15) );
      DrawNibble (&GFX[tb+0],1, (UINT8) (TMP[ta+0x00000]>>4) );
      DrawNibble (&GFX[tb+0],2, (UINT8) (TMP[ta+0x10000]&15) );
      DrawNibble (&GFX[tb+0],3, (UINT8) (TMP[ta+0x10000]>>4) );
      DrawNibble0(&GFX[tb+4],0, (UINT8) (TMP[ta+0x00001]&15) );
      DrawNibble (&GFX[tb+4],1, (UINT8) (TMP[ta+0x00001]>>4) );
      DrawNibble (&GFX[tb+4],2, (UINT8) (TMP[ta+0x10001]&15) );
      DrawNibble (&GFX[tb+4],3, (UINT8) (TMP[ta+0x10001]>>4) );
   }

   FreeMem(TMP);
/*
   Rotate8x8(GFX,0x1000);
   Flip8x8_Y(GFX,0x1000);
*/
}

void ClearFLStory(void)
{
#ifdef RAINE_DEBUG
      save_debug("RAM.BIN", RAM, RAMSize, 0);
      save_debug("GFX.BIN", GFX, 0x040000, 0);
#endif
}


void ExecuteFLStoryFrame(void)
{
/*
   static int coin_toggle_a,coin_toggle_b;

   // ----- MCU hacks -----

   if((RAM_INPUT[5]&0x01)!=0){
      if(coin_toggle_a==0){
         coin_toggle_a=1;
         RAM[0xE801]++;
      }
   }
   else{
      coin_toggle_a=0;
   }

   if((RAM_INPUT[5]&0x02)!=0){
      if(coin_toggle_b==0){
         coin_toggle_b=1;
         RAM[0xE801]++;
      }
   }
   else{
      coin_toggle_b=0;
   }

   // ---------------------
*/
   cpu_execute_cycles(CPU_Z80_1, CPU_FRAME_MHz(6,60));	// Main Z80 6MHz (60fps)
      print_debug("Z80PC_MAIN:%04x\n",z80pc);
   cpu_interrupt(CPU_Z80_1, 0x38);

   cpu_execute_cycles(CPU_Z80_2, CPU_FRAME_MHz(4,60));	// Sub Z80 4MHz (60fps)
      print_debug("Z80PC_SUB:%04x\n",z80pc);
   cpu_interrupt(CPU_Z80_2, 0x38);
   //if((s_stat&0x02)!=0) cpu_int_nmi(CPU_Z80_2);
}

void DrawFLStory(void)
{
   int x,y,code,ta;
   int zz,zzz,zzzz,x16,y16;
/*
   if(RefreshBuffers){

   for(zz=0;zz<0x200;zz+=2){
   ta=ReadWord(&RAM_COLOUR[zz]);
   pal[zz>>1].r=(ta&0x0F00)>>6;
   pal[zz>>1].g=(ta&0x00F0)>>2;
   pal[zz>>1].b=(ta&0x000F)<<2;
   }

   }

   // BG0
   // ---

   MAKE_SCROLL_256x256_2_8(
      16+(ReadWord(&RAM[0xD500])),
      16-(ReadWord(&RAM[0xD502]))
   );

   START_SCROLL_256x256_2_8_FLIPY(32,32,224,240);

      code = ReadWord(&RAM[0xCD00+zz])&0x1FFF;

      Draw8x8(&GFX[code<<6], x, y, (ReadWord(&RAM[0xCD00+zz])>>9)&0x70);

   END_SCROLL_256x256_2_8();

   //for(zz=0xD548;zz<0xD658;zz+=4){
   for(zz=0xD500;zz<0xD7FF;zz+=4){
      x = RAM[zz+0];
      ofs = RAM[zz+1];
      if((x!=0)||(ofs!=0)){
         rx = 8;
         ry = 2;

         ofs = ((ofs<<7) | (ofs>>1)) & 0x0FF0;

         CHAIN = RAM+0xC000+ofs;

         ta=0;

         while((ReadWord(&CHAIN[ta])==0)&&(ta<16)){
            rx--;
            ta+=2;
         }

         for(ryy=0;ryy<ry;ryy++){

         CHAIN_2 = CHAIN + 0x0E;

            y = (RAM[zz+2]+(ryy<<3)+16)&0x1FF;
            x = (RAM[zz+0]+16)&0x1FF;

            for(rxx=0;rxx<rx;rxx++){

               if((x>24)&&(y>24)&&(x<224+32)&&(y<240+32)){

                  code = ReadWord(&CHAIN_2[0])&0x1FFF;

                  Draw8x8_Trans(&GFX[code<<6], x, y, (ReadWord(&CHAIN_2[0])>>9)&0x70);

               }
               CHAIN_2 -= 2;
               x+=8;
            }
            CHAIN = CHAIN + 0x40;
         }
      }
   }
*/

   for(zz=0;zz<0x200;zz+=2){
   ta=ReadWord(&RAM[zz+0xDD00]);
#ifdef SDL
   pal[zz>>1].r=(ta&0x0F00)>>4;
   pal[zz>>1].g=(ta&0x00F0);
   pal[zz>>1].b=(ta&0x000F)<<4;
#else
   pal[zz>>1].r=(ta&0x0F00)>>6;
   pal[zz>>1].g=(ta&0x00F0)>>2;
   pal[zz>>1].b=(ta&0x000F)<<2;
#endif
   }

   // BG1/FG0
   // -------

   MAKE_SCROLL_256x256_2_8(
      0,
      0
   );

   START_SCROLL_256x256_2_8_R180(32,32,256,256);

      ta = ReadWord(&RAM[0xC000+zz]);

      code = (ta&0xFF) | ((ta>>6)&0x0300);

      switch(ta&0x0800){
      case 0x0800: Draw8x8(&GFX[code<<6], x, y, (ta>>6)&0x00);       break;
      case 0x0000: Draw8x8_FlipY(&GFX[code<<6], x, y, (ta>>6)&0x00); break;
      }

   END_SCROLL_256x256_2_8();

#ifdef SDL
   pal[0].b=255;
#else
   pal[0].b=63;
#endif
}

/*

C000-C7FF - BG0
---------------

- 64 bytes/line
- 256x256 size

-----+--------+---------------
Byte | Bit(s) | Use
-----+76543210+---------------
  0  |xx......| Tile (8-9)?
  0  |....x...| Flip Y Axis
  1  |xxxxxxxx| Tile (0-7)
-----+--------+---------------


D500-D7FF - CTRL DATA
---------------------

- 4 bytes/sprite
- D500 BG0 scroll (sprite?)

-----+--------+---------------
Byte | Bit(s) | Use
-----+76543210+---------------
  0  |xxxxxxxx| Sprite X (low)
  1  |xxx.....| Offset (low)
  1  |...xxxxx| Offset (high)
  2  |xxxxxxxx| Sprite Y (low)
-----+--------+---------------


*/

