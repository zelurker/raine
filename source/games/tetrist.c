/******************************************************************************/
/*                                                                            */
/*                 TAITO TETRIS (C) 1989 TAITO CORPORATION                    */
/*                                                                            */
/******************************************************************************/

#include "gameinc.h"
#include "tetrist.h"
#include "tc220ioc.h"
#include "sasound.h"		// sample support routines
#include "taitosnd.h"
#include "blit.h"

static struct DIR_INFO taito_sega_tetris_dirs[] =
{
   { "taito_tetris", },
   { "taito_sega_tetris", },
   { "tetrist", },
   { "tetris", },
   { NULL, },
};

static struct ROM_INFO taito_sega_tetris_roms[] =
{
   {   "c12-03.bin", 0x00020000, 0x38f1ed41, REGION_ROM1, 0x000000, LOAD_8_16, },
   {   "c12-02.bin", 0x00020000, 0xed9530bc, REGION_ROM1, 0x000001, LOAD_8_16, },
   {   "c12-05.bin", 0x00020000, 0x128e9927, REGION_ROM1, 0x040000, LOAD_8_16, },
   {   "c12-04.bin", 0x00020000, 0x5da7a319, REGION_ROM1, 0x040001, LOAD_8_16, },
   {   "c12-06.bin", 0x00010000, 0xf2814b38, 0, 0, 0, },
   {           NULL,          0,          0, 0, 0, 0, },
};

static struct INPUT_INFO taito_sega_tetris_inputs[] =
{
   { KB_DEF_COIN1,        MSG_COIN1,               0x05800E, 0x04, BIT_ACTIVE_0 },
   { KB_DEF_COIN2,        MSG_COIN2,               0x05800E, 0x08, BIT_ACTIVE_0 },
   { KB_DEF_TILT,         MSG_TILT,                0x05800E, 0x01, BIT_ACTIVE_0 },
   { KB_DEF_SERVICE,      MSG_SERVICE,             0x05800E, 0x02, BIT_ACTIVE_0 },

   { KB_DEF_P1_START,     MSG_P1_START,            0x05800E, 0x40, BIT_ACTIVE_0 },
   { KB_DEF_P1_UP,        MSG_P1_UP,               0x058004, 0x01, BIT_ACTIVE_0 },
   { KB_DEF_P1_DOWN,      MSG_P1_DOWN,             0x058004, 0x02, BIT_ACTIVE_0 },
   { KB_DEF_P1_LEFT,      MSG_P1_LEFT,             0x058004, 0x04, BIT_ACTIVE_0 },
   { KB_DEF_P1_RIGHT,     MSG_P1_RIGHT,            0x058004, 0x08, BIT_ACTIVE_0 },
   { KB_DEF_P1_B1,        MSG_P1_B1,               0x058004, 0x10, BIT_ACTIVE_0 },
   { KB_DEF_P1_B2,        MSG_P1_B2,               0x058004, 0x20, BIT_ACTIVE_0 },

   { KB_DEF_P2_START,     MSG_P2_START,            0x05800E, 0x80, BIT_ACTIVE_0 },
   { KB_DEF_P2_UP,        MSG_P2_UP,               0x058006, 0x01, BIT_ACTIVE_0 },
   { KB_DEF_P2_DOWN,      MSG_P2_DOWN,             0x058006, 0x02, BIT_ACTIVE_0 },
   { KB_DEF_P2_LEFT,      MSG_P2_LEFT,             0x058006, 0x04, BIT_ACTIVE_0 },
   { KB_DEF_P2_RIGHT,     MSG_P2_RIGHT,            0x058006, 0x08, BIT_ACTIVE_0 },
   { KB_DEF_P2_B1,        MSG_P2_B1,               0x058006, 0x10, BIT_ACTIVE_0 },
   { KB_DEF_P2_B2,        MSG_P2_B2,               0x058006, 0x20, BIT_ACTIVE_0 },

   { 0,                   NULL,                    0,        0,    0            },
};

static struct DSW_DATA dsw_data_taito_sega_tetris_0[] =
{
   { MSG_DSWA_BIT1,           0x01, 0x02 },
   { MSG_OFF,                 0x01, 0x00 },
   { MSG_ON,                  0x00, 0x00 },
   { MSG_DSWA_BIT2,           0x02, 0x02 },
   { MSG_OFF,                 0x02, 0x00 },
   { MSG_ON,                  0x00, 0x00 },
   { MSG_TEST_MODE,           0x04, 0x02 },
   { MSG_OFF,                 0x04, 0x00 },
   { MSG_ON,                  0x00, 0x00 },
   { MSG_DEMO_SOUND,          0x08, 0x02 },
   { MSG_ON,                  0x08, 0x00 },
   { MSG_OFF,                 0x00, 0x00 },
   { MSG_DSWA_BIT5,           0x10, 0x02 },
   { MSG_OFF,                 0x10, 0x00 },
   { MSG_ON,                  0x00, 0x00 },
   { MSG_DSWA_BIT6,           0x20, 0x02 },
   { MSG_OFF,                 0x20, 0x00 },
   { MSG_ON,                  0x00, 0x00 },
   { MSG_DSWA_BIT7,           0x40, 0x02 },
   { MSG_OFF,                 0x40, 0x00 },
   { MSG_ON,                  0x00, 0x00 },
   { MSG_DSWA_BIT8,           0x80, 0x02 },
   { MSG_OFF,                 0x80, 0x00 },
   { MSG_ON,                  0x00, 0x00 },
   { NULL,                    0,    0,   },
};

static struct DSW_INFO taito_sega_tetris_dsw[] =
{
   { 0x058000, 0xFF, dsw_data_taito_sega_tetris_0 },
   { 0x058002, 0xFF, dsw_data_default_1 },
   { 0,        0,    NULL,      },
};

static struct VIDEO_INFO taito_tetris_video =
{
   DrawTaitoTetris,
   320,
   224,
   32,
   VIDEO_ROTATE_NORMAL|VIDEO_NEEDS_8BPP
};

GAME( taito_sega_tetris ,
   taito_sega_tetris_dirs,
   taito_sega_tetris_roms,
   taito_sega_tetris_inputs,
   taito_sega_tetris_dsw,
   NULL,

   LoadTaitoTetris,
   ClearTaitoTetris,
   &taito_tetris_video,
   ExecuteTaitoTetrisFrame,
   "tetrist",
   "Taito Tetris",
   "ƒeƒgƒŠƒX",
   COMPANY_ID_TAITO,
   "C12",
   1989,
   taito_ym2610_sound,
   GAME_PUZZLE
);

static UINT8 *RAM_INPUT;

void LoadTaitoTetris(void)
{
   RAMSize=0x60000+0x10000;

   if(!(RAM=AllocateMem(RAMSize))) return;

   /*-----[Sound Setup]-----*/

   Z80ROM=RAM+0x60000;
   if(!load_rom("c12-06.bin", Z80ROM, 0x10000)) return;	// Z80 SOUND ROM

   if(!(PCMROM=AllocateMem(0x100000))) return;
   memset(PCMROM,0x80,0x100000);
   YM2610SetBuffers(PCMROM, PCMROM+0x80000, 0x80000, 0x80000);

   AddTaitoYM2610(0x01BE, 0x016A, 0x10000);

   /*-----------------------*/

   memset(RAM+0x00000,0x00,0x58000);

   RAM_INPUT = RAM+0x58000;

   WriteLong68k(&ROM[0x072E],0x13FC0000);	// move.b #$00,$AA0000
   WriteLong68k(&ROM[0x0732],0x00AA0000);	// Speed Hack
   WriteWord68k(&ROM[0x0736],0x6100-10);	// <Loop>


   // Init tc0220ioc emulation
   // ------------------------

   tc0220ioc.RAM  = RAM_INPUT;
   tc0220ioc.ctrl = 0;		//TC0220_STOPCPU;
   reset_tc0220ioc();

/*
 *  StarScream Stuff follows
 */

   ByteSwap(ROM,0x80000);
   ByteSwap(RAM,0x60000);

   AddMemFetch(0x000000, 0x07FFFF, ROM+0x000000-0x000000);	// 68000 ROM
   AddMemFetch(-1, -1, NULL);

   AddReadByte(0x000000, 0x07FFFF, NULL, ROM+0x000000);			// 68000 ROM
   AddReadByte(0x800000, 0x807FFF, NULL, RAM+0x000000);			// 68000 RAM
   AddReadByte(0x400000, 0x408FFF, NULL, RAM+0x008000);			// ??? RAM
   AddReadByte(0x410000, 0x413FFF, NULL, RAM+0x013000);			// ??? RAM
   AddReadByte(0xA00000, 0xA01FFF, NULL, RAM+0x011000);			// COLOR RAM
   AddReadByte(0x440000, 0x47FFFF, NULL, RAM+0x018000);			// PIXEL RAM
   AddReadByte(0x600000, 0x60000F, tc0220ioc_rb, NULL);			// INPUT
   AddReadByte(0x200000, 0x200003, tc0140syt_read_main_68k, NULL);	// SOUND COMM
   AddReadByte(0x000000, 0xFFFFFF, DefBadReadByte, NULL);		// <Bad Reads>
   AddReadByte(-1, -1, NULL, NULL);

   AddReadWord(0x000000, 0x07FFFF, NULL, ROM+0x000000);			// 68000 ROM
   AddReadWord(0x800000, 0x807FFF, NULL, RAM+0x000000);			// 68000 RAM
   AddReadWord(0x400000, 0x408FFF, NULL, RAM+0x008000);			// ??? RAM
   AddReadWord(0x410000, 0x413FFF, NULL, RAM+0x013000);			// SCREEN RAM
   AddReadWord(0xA00000, 0xA01FFF, NULL, RAM+0x011000);			// COLOR RAM
   AddReadWord(0x440000, 0x47FFFF, NULL, RAM+0x018000);			// PIXEL RAM
   AddReadWord(0x600000, 0x60000F, tc0220ioc_rw, NULL);			// INPUT
   AddReadWord(0x000000, 0xFFFFFF, DefBadReadWord, NULL);		// <Bad Reads>
   AddReadWord(-1, -1,NULL, NULL);

   AddWriteByte(0x800000, 0x807FFF, NULL, RAM+0x000000);		// 68000 RAM
   AddWriteByte(0x440000, 0x47FFFF, NULL, RAM+0x018000);		// PIXEL RAM
   AddWriteByte(0x400000, 0x408FFF, NULL, RAM+0x008000);		// ??? RAM
   AddWriteByte(0x410000, 0x413FFF, NULL, RAM+0x013000);		// SCREEN RAM
   AddWriteByte(0xA00000, 0xA01FFF, NULL, RAM+0x011000);		// COLOR RAM
   AddWriteByte(0x418000, 0x41801F, NULL, RAM+0x058100);		// SCROLL RAM
   AddWriteByte(0x200000, 0x200003, tc0140syt_write_main_68k, NULL);	// SOUND COMM
   AddWriteByte(0x600000, 0x60000F, tc0220ioc_wb, NULL);		// INPUT
   AddWriteByte(0xAA0000, 0xAA0001, Stop68000, NULL);			// Trap Idle 68000
   AddWriteByte(0x000000, 0xFFFFFF, DefBadWriteByte, NULL);		// <Bad Writes>
   AddWriteByte(-1, -1, NULL, NULL);

   AddWriteWord(0x800000, 0x807FFF, NULL, RAM+0x000000);		// 68000 RAM
   AddWriteWord(0x440000, 0x47FFFF, NULL, RAM+0x018000);		// PIXEL RAM
   AddWriteWord(0x400000, 0x408FFF, NULL, RAM+0x008000);		// ??? RAM
   AddWriteWord(0x410000, 0x413FFF, NULL, RAM+0x013000);		// ??? RAM
   AddWriteWord(0xA00000, 0xA01FFF, NULL, RAM+0x011000);		// COLOR RAM
   AddWriteWord(0x418000, 0x41801F, NULL, RAM+0x058100);		// SCROLL RAM
   AddWriteWord(0x600000, 0x60000F, tc0220ioc_ww, NULL);		// INPUT
   AddWriteWord(0x000000, 0xFFFFFF, DefBadWriteWord, NULL);		// <Bad Writes>
   AddWriteWord(-1, -1, NULL, NULL);

   AddInitMemory();	// Set Starscream mem pointers... 
}

void ClearTaitoTetris(void)
{
   RemoveTaitoYM2610();
}

void ExecuteTaitoTetrisFrame(void)
{
   cpu_execute_cycles(CPU_68K_0, CPU_FRAME_MHz(12,60));	// M68000 12MHz (60fps)
   cpu_interrupt(CPU_68K_0, 2);
   cpu_interrupt(CPU_68K_0, 4);

   Taito2610_Frame();				// Z80 and YM2610
}

void DrawTaitoTetris(void)
{
   int x,y,tc,yy;
   int zz;
   UINT8 *BIT;

   for(tc=0;tc<256;tc++){
      yy=ReadWord(&RAM[0x11800+tc+tc]);
#ifdef SDL
      pal[tc].r=(yy&0xF000)>>8;
      pal[tc].g=(yy&0x0F00)>>4;
      pal[tc].b=(yy&0x00F0)>>0;
#else
      pal[tc].r=(yy&0xF000)>>10;
      pal[tc].g=(yy&0x0F00)>>6;
      pal[tc].b=(yy&0x00F0)>>2;
#endif
   }

   if((RAM[0x05810F]&0x40)!=0){
      zz=0x1A000;
   }
   else{
      zz=0x3A000;
   }

   for(y=0;y<224;y++){
   BIT=GameViewBitmap->line[y];
   for(x=0;x<320;x+=2){
      WriteWord68k(&BIT[x],ReadWord(&RAM[zz]));
      zz+=2;
   }
   zz+=(512-320);
   }
}
