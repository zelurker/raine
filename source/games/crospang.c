/* Cross Pang

  - Similar hardware to the Cookie Bibi (sprite system, tilemaps)

*/

#include "gameinc.h"
#include "sasound.h"		// sample support routines
#include "2151intf.h"
#include "adpcm.h"
#include "3812intf.h"
#include "timer.h"
#include "savegame.h"
#include "emumain.h" // set_reset_function
#include "scroll.h"

static UINT8 *RAM_Z80;
static UINT8 *RAM_MAIN;
static UINT8 *RAM_SPRITE;
static UINT8 *RAM_PF1;
static UINT8 *RAM_PF2;
static UINT8 *RAM_PALETTE;
static UINT8 *RAM_PFCTRL;


static struct ROM_INFO rom_crospang[] =
{
  LOAD8_16(  REGION_ROM1,  0x000000,       0x020000,
            "p2.bin",  0x0947d204, "p1.bin",  0x0bcbbaad),
{ "s1.bin",      0x010000, 0xd61a224c, REGION_ROM2, 0x000000, LOAD_NORMAL },
{ "s2.bin",      0x020000, 0x9f9ecd22, REGION_SMP1, 0x000000, LOAD_NORMAL },
{ "rom6.bin",    0x080000, 0x9c633082, REGION_GFX1, 0x000000, LOAD_NORMAL },
{ "rom5.bin",    0x080000, 0x53a34dc5, REGION_GFX1, 0x080000, LOAD_NORMAL },
{ "rom4.bin",    0x080000, 0x9a91d494, REGION_GFX1, 0x100000, LOAD_NORMAL },
{ "rom3.bin",    0x080000, 0xcc6e1fce, REGION_GFX1, 0x180000, LOAD_NORMAL },
{ "rom1.bin",    0x040000, 0x905042bb, REGION_GFX2, 0x000000, LOAD_NORMAL },
{ "rom2.bin",    0x040000, 0xbc4381e9, REGION_GFX2, 0x040000, LOAD_NORMAL },
{ NULL, 0, 0, 0, 0, 0 }
};

static struct INPUT_INFO input_crospang[] =
{
	INP0( P1_UP, 0x0000, 0x01 ),
	INP0( P1_DOWN, 0x0000, 0x02 ),
	INP0( P1_LEFT, 0x0000, 0x04 ),
	INP0( P1_RIGHT, 0x0000, 0x08 ),
	INP0( P1_B1, 0x0000, 0x10 ),
	INP0( P1_B2, 0x0000, 0x20 ),
	INP0( P1_B3, 0x0000, 0x40 ),
	INP0( P1_START, 0x0000, 0x80 ),

	INP0( P2_UP, 0x0001, 0x01 ),
	INP0( P2_DOWN, 0x0001, 0x02 ),
	INP0( P2_LEFT, 0x0001, 0x04 ),
	INP0( P2_RIGHT, 0x0001, 0x08 ),
	INP0( P2_B1, 0x0001, 0x10 ),
	INP0( P2_B2, 0x0001, 0x20 ),
	INP0( P2_B3, 0x0001, 0x40 ),
	INP0( P2_START, 0x0001, 0x80 ),

	INP0( COIN1, 0x0003, 0x01 ),
	INP0( COIN2, 0x0003, 0x02 ),
INP0( COIN3, 0x0003, 0x04 ),
	END_INPUT
};

static struct DSW_DATA dsw_data_crospang_0[] =
{
	{ MSG_COINAGE,             0x03,    4 },
	{ MSG_3COIN_1PLAY,         0x00},
	{ MSG_2COIN_1PLAY,         0x01},
	{ MSG_1COIN_1PLAY,         0x02},
	{ MSG_1COIN_2PLAY,         0x03},
	{ MSG_DSWA_BIT3,           0x04,    2 },
	{ MSG_OFF,                 0x04},
	{ MSG_ON,                  0x00},
	{ MSG_DSWA_BIT4,           0x08,    2 },
	{ MSG_OFF,                 0x08},
	{ MSG_ON,                  0x00},
	{ MSG_DSWA_BIT5,           0x10,    2 },
	{ MSG_OFF,                 0x10},
	{ MSG_ON,                  0x00},
	{ "Number of Powers",      0x20,    2 },
	{ "1",                     0x00},
	{ "2",                     0x20},
	{ "Extra Balls"     ,      0xc0,    4 },
	{ "1 (too slow)",          0xc0},
	{ "2",                     0x80},
	{ "3",                     0x40},
	{ "4",                     0x00},
	{ NULL,                    0,   },
};

static struct DSW_DATA dsw_data_crospang_1[] =
{
	{ MSG_DSWB_BIT1,           0x01,    2 },
	{ MSG_OFF,                 0x01},
	{ MSG_ON,                  0x00},
	{ MSG_DSWB_BIT2,           0x02,    2 },
	{ MSG_OFF,                 0x02},
	{ MSG_ON,                  0x00},
	{ MSG_DSWB_BIT3,           0x04,    2 },
	{ MSG_OFF,                 0x04},
	{ MSG_ON,                  0x00},
	{ "Min Balls per Row",     0x18,    4 },
	{ "3 (buggy)",             0x18},
	{ "4",                     0x10},
	{ "5",                     0x08},
	{ "6",                     0x00},
	{ MSG_DSWB_BIT6,           0x20, 0x02 },
	{ MSG_OFF,                 0x20},
	{ MSG_ON,                  0x00},
	{ "Service Mode",          0x40, 0x02 },
	{ MSG_OFF,                 0x40},
	{ MSG_ON,                  0x00},
	{ MSG_DSWB_BIT8,           0x80, 0x02 },
	{ MSG_OFF,                 0x80},
	{ MSG_ON,                  0x00},
};

static struct DSW_INFO dsw_crospang[] =
{
  { 0x4, 0x7e, dsw_data_crospang_0 },
  { 0x5, 0xf7, dsw_data_crospang_1 },
  { 0, 0, NULL }
};


static struct YM3812interface ym3812_interface =
{
   1,              // 1 chip
   14318180/4,	/**/
   { 255 },        // Volume (emu only)
   { z80_irq_handler }
};

static struct OKIM6295interface okim6295_interface =
{
	1,			/* 1 chip */
	{ 8000 },		/* 7575Hz playback? */
	{ REGION_SOUND1 },
	{ 240 }
};

static struct SOUND_INFO sound_crospang[] =
{
   { SOUND_YM3812,  &ym3812_interface,    },
   { SOUND_M6295,    &okim6295_interface   },
   { 0,              NULL,                 },
};



static gfx_layout layout_16x16x4a =
{
	16,16,
	RGN_FRAC(1,4),
	4,
	{ RGN_FRAC(0,4),RGN_FRAC(1,4),RGN_FRAC(2,4),RGN_FRAC(3,4) },
	{ 128,129,130,131,132,133,134,135, 0,1,2,3,4,5,6,7 },
	{ 8*0, 8*1, 8*2, 8*3, 8*4, 8*5, 8*6, 8*7, 8*8, 8*9, 8*10, 8*11, 8*12, 8*13, 8*14, 8*15 },
	8*32
};


static gfx_layout layout_16x16x4 =
{
	16,16,
	RGN_FRAC(1,4),
	4,
	{ RGN_FRAC(2,4),RGN_FRAC(0,4),RGN_FRAC(3,4),RGN_FRAC(1,4) },
	{ 0,1,2,3,4,5,6,7, 128,129,130,131,132,133,134,135 },
	{ 8*0, 8*1, 8*2, 8*3, 8*4, 8*5, 8*6, 8*7, 8*8, 8*9, 8*10, 8*11, 8*12, 8*13, 8*14, 8*15 },
	8*32
};


static struct GFX_LIST crospang_gfx[] =
{
   { REGION_GFX1, &layout_16x16x4a, },
   { REGION_GFX2, &layout_16x16x4, },
   { 0,           NULL,           },
};

/* this doesn't take into account priority, but afaik these games don't really care */
static int spr_xoffset = -4;
static int spr_yoffset = 0;

UINT8 *map;

#define BORDER 16

static void draw_solid_layer(int scrollx,int scrolly, int palbase, UINT8 *PFRAM)
{
	int x,y;
	UINT16 tileno;
	UINT8  colour;
	int x16,y16,zzz,zzzz;
	int zz;


	MAKE_SCROLL_512x512_2_16(scrollx,scrolly);
	START_SCROLL_512x512_2_16(BORDER,BORDER,320,240);
	{

	  tileno = ReadWord(&PFRAM[zz]);
	  colour = (tileno>>12) + palbase;
	  tileno &= 0x0fff;

	  MAP_PALETTE_MAPPED_NEW(
				 colour,
				 16,
				 map
				 );

	  Draw16x16_Mapped_Rot(&gfx2[tileno<<8], x, y, map);
	}
	END_SCROLL_512x512_2_16();
}

static void crospang_draw_16x16_layer(int scrollx,int scrolly, int palbase, UINT8 *PFRAM)
{
	int x,y;
	UINT16 tileno;
	UINT8  colour;
	int x16,y16,zzz,zzzz;
	int zz;


	MAKE_SCROLL_512x512_2_16(scrollx,scrolly);
	START_SCROLL_512x512_2_16(BORDER,BORDER,320,240);
	{

	  tileno = ReadWord(&PFRAM[zz]);
	  colour = (tileno>>12) + palbase;
	  tileno &= 0x0fff;

	  if( gfx2_solid[tileno] )
	    {
	      MAP_PALETTE_MAPPED_NEW(
				     colour,
				     16,
				     map
				     );

	      if((gfx2_solid[tileno]==1))
		{
		  Draw16x16_Trans_Mapped_Rot(&gfx2[tileno<<8], x, y, map);
		}
	      else
		{
		  Draw16x16_Mapped_Rot(&gfx2[tileno<<8], x, y, map);
		}
	    }
	}
	END_SCROLL_512x512_2_16();
}

static void crospang_drawsprites(void)
{
	int offs;
	static int frame=0;

	frame++;

	for (offs = 0;offs<0x800;offs+=8)
	{
		int x,y,sprite,colour,multi,/* fx, */ fy,inc,flash,mult,flipyx,xdraw;

//		print_debug("sprite %04x\n",offs);

		sprite = (RAM_SPRITE[offs+2]|(RAM_SPRITE[offs+3]<<8));

		if (!sprite) continue; // careful.. not all games have sprite 0 as blank, these AFAIK do however

		y =      ReadWord(&RAM_SPRITE[offs]);

//		sprite &=0x7fff;

		flash=y&0x1000;
		if ((flash) && (frame & 1) ) continue;

		x =      ReadWord(&RAM_SPRITE[offs+4]);
		colour = (x >>9) & 0xf;

		// fx = y & 0x2000;
		fy = y & 0x4000;
		flipyx = (y&0x6000)>>13;

		multi = (1 << ((y & 0x0600) >> 9)) - 1;	// 1x, 2x, 4x, 8x height

		x = x & 0x01ff;
		y = y & 0x01ff;
		if (x >= 320) x -= 512;
		if (y >= 256) y -= 512;
		y = 240 - y;
		x = 304 - x;

		xdraw = x+BORDER+spr_xoffset; // x-1 for bcstory .. realign other layers?

		if((xdraw<=BORDER-16)||(xdraw>=320+BORDER)) continue;

	//  sprite &= ~multi; // Todo:  I bet TumblePop bootleg doesn't do this either
		if (fy)
			inc = -1;
		else
		{
			sprite += multi;
			inc = 1;
		}

//		if (flipscreen)
//		{
//			y=240-y;
//			x=304-x;
//			if (fx) fx=0; else fx=1;
//			if (fy) fy=0; else fy=1;
//			mult=16;
//		}
//		else
		mult=-16;

		while (multi >= 0)
		{
			int ydraw = (y+mult*multi)+BORDER-7+spr_yoffset;
			int spriteno = sprite - multi * inc;
/*
			drawgfx(bitmap,Machine->gfx[3],
					spriteno,
					colour,
					fx,fy,
					xdraw,ydraw,
					cliprect,TRANSPARENCY_PEN,0);
*/


			if ((ydraw>BORDER-16)&&(ydraw<240+BORDER))
			{
//				print_debug("sprite draw %04x %04x %04x\n",xdraw,ydraw,spriteno);

				if(gfx1_solid[spriteno])
				{            // No pixels; skip

					MAP_PALETTE_MAPPED_NEW(
						colour,
						16,
						map
					);

					if(gfx1_solid[spriteno]==1)
					{         // Some pixels; trans
						switch(flipyx)
						{
							case 0x00: Draw16x16_Trans_Mapped_Rot       (&gfx1[spriteno<<8], xdraw, ydraw, map); break;
							case 0x01: Draw16x16_Trans_Mapped_FlipY_Rot (&gfx1[spriteno<<8], xdraw, ydraw, map); break;
							case 0x02: Draw16x16_Trans_Mapped_FlipX_Rot (&gfx1[spriteno<<8], xdraw, ydraw, map); break;
							case 0x03: Draw16x16_Trans_Mapped_FlipXY_Rot(&gfx1[spriteno<<8], xdraw, ydraw, map); break;
						}
					}
					else
					{                  // all pixels; solid
						switch(flipyx)
						{
							case 0x00: Draw16x16_Mapped_Rot       (&gfx1[spriteno<<8], xdraw, ydraw, map); break;
							case 0x01: Draw16x16_Mapped_FlipY_Rot (&gfx1[spriteno<<8], xdraw, ydraw, map); break;
							case 0x02: Draw16x16_Mapped_FlipX_Rot (&gfx1[spriteno<<8], xdraw, ydraw, map); break;
							case 0x03: Draw16x16_Mapped_FlipXY_Rot(&gfx1[spriteno<<8], xdraw, ydraw, map); break;
						}
					}
				}
			}

			multi--;
		}

	}
}


static void draw_crospang(void)
{
	int scrollx,scrolly;

	ClearPaletteMap(); // some colours are black if you forget this ;p

	scrollx = (RAM_PFCTRL[8]|(RAM_PFCTRL[9]<<8));
	scrolly = (RAM_PFCTRL[6]|(RAM_PFCTRL[7]<<8));
	scrolly += 8;
	draw_solid_layer(scrollx,scrolly,0x20,RAM_PF2);

	scrollx = (RAM_PFCTRL[4]|(RAM_PFCTRL[5]<<8));
	scrolly = (RAM_PFCTRL[2]|(RAM_PFCTRL[3]<<8));
	scrollx += 4;
	scrolly += 8;
	crospang_draw_16x16_layer  (scrollx,scrolly,0x10,RAM_PF1);

	crospang_drawsprites();
}



#define FRAME_Z80 CPU_FRAME_MHz(4,60)

static void execute_crospang(void)
{
  int frame = FRAME_Z80,diff;
  while (frame > 0) {
    diff = execute_one_z80_audio_frame(frame);
    cpu_execute_cycles(CPU_68K_0, diff*2); // Main 68000
    frame -= diff;
  }
  cpu_interrupt(CPU_68K_0, 6);
}

UINT8 input_crospang_8r(UINT32 offset)
{
	offset &=0x07;
	offset ^=1;

	return input_buffer[offset];
}


UINT16 input_crospang_16r(UINT32 offset)
{
	offset &=0x0f;

	return input_crospang_8r(offset+1)|(input_crospang_8r(offset)<<8);
}

UINT16 sound_crospang_latch;

UINT8 crosspang_soundlatch_r (UINT32 offset)
{
	return sound_crospang_latch;
}


static void sound_crospang16_w ( UINT32 offset, UINT16 data )
{
	sound_crospang_latch = data;

	// cpu_execute_cycles(CPU_Z80_0, 6000);
//	printf("sound16_w data %04x\n",data);

}

static UINT16 my_speed_hack(UINT32 offset) {
  /* I am not 100% certain that this is the place tested for the vbl, because this
     rom executes an insane amount of code while waiting for the vbl, but this is
     the 1st thing tested, and it seems to work */
  UINT16 ret = ReadWord(&RAM_MAIN[0x53fe]);
  if (ret == 0) {
    Stop68000(0,0);
  }
  return ret;
}

static void load_crospang(void)
{
	/* In RAINE we allocate one big block of RAM to contain all emulated RAM then set some pointers to it

		Z80:
		0x10000 bytes z80

		68000:
		0x10000 bytes - MAIN RAM
		0x00800 bytes - SPRITERAM
		0x00800 bytes - FG TILEMAP
		0x00800 bytes - BG TILEMAP
		0x00800 bytes - PALETTE (actually 0x600)
		0x00010 bytes - VIDEO REGS
	*/

	/* Calculate how much RAM we need to allocate */
	RAMSize=0x10000
	       +0x10000
	       +0x00800
	       +0x00800
	       +0x00800
	       +0x00800
	       +0x00010;

	/* Allocate the RAM */
	if(!(RAM=AllocateMem(RAMSize))) return;

	/* Set Up the Pointers */
	RAM_Z80       = &RAM[0x00000];
	RAM_MAIN      = &RAM[0x00000+0x10000];
	RAM_SPRITE    = &RAM[0x00000+0x10000+0x10000];
	RAM_PF1       = &RAM[0x00000+0x10000+0x10000+0x00800];
	RAM_PF2       = &RAM[0x00000+0x10000+0x10000+0x00800+0x00800];
	RAM_PALETTE   = &RAM[0x00000+0x10000+0x10000+0x00800+0x00800+0x00800];
	RAM_PFCTRL    = &RAM[0x00000+0x10000+0x10000+0x00800+0x00800+0x00800+0x00800];


   // Setup Z80 memory map
   // --------------------

	AddZ80AROMBase(Z80ROM, 0x0038, 0x0066);

	AddZ80AReadByte (0x0000, 0xbfff, NULL,                     load_region[REGION_ROM2] ); // Z80 ROM
	AddZ80AReadByte (0xc000, 0xc7ff, NULL,                     RAM_Z80                  );

	AddZ80AWriteByte(0xc000, 0xc7ff, NULL,                     RAM_Z80                  ); // Z80 RAM

	AddZ80AReadPort (0x00, 0x00, YM3812ReadZ80,                  NULL);
	AddZ80AReadPort (0x02, 0x02, OKIM6295_status_0_r,            NULL);
	AddZ80AReadPort (0x06, 0x06, crosspang_soundlatch_r,         NULL);

	AddZ80AWritePort(0x00, 0x01, YM3812WriteZ80,                 NULL);
	AddZ80AWritePort(0x02, 0x02, OKIM6295_data_0_w,              NULL);


	AddZ80AReadByte (0x0000, 0xFFFF, DefBadReadZ80,              NULL);
	AddZ80AWriteByte(0x0000, 0xFFFF, DefBadWriteZ80,             NULL);
	AddZ80AReadPort (0x00,   0xFF,   DefBadReadZ80,              NULL);
	AddZ80AWritePort(0x00,   0xFF,   DefBadWriteZ80,             NULL);
	AddZ80AReadByte (  -1,   -1,     NULL,                       NULL);
	AddZ80AWriteByte(  -1,   -1,     NULL,                       NULL);
	AddZ80AReadPort (  -1,   -1,     NULL,                       NULL);
	AddZ80AWritePort(  -1,   -1,     NULL,                       NULL);

	AddZ80AInit();

	setup_z80_frame(CPU_Z80_0,FRAME_Z80); // for the Z80 we need to set up the speed here..


	/* Setup Starscream 68000 core */

	/* ROM is a pointer to the ROM1 region.. automatically set up? */

	/* byteswap the ROM area.. */
	ByteSwap(ROM,get_region_size(REGION_CPU1));

	/*
	   AddMemFetch = For Running Program Code

	   AddReadByte = Add Read Handler for Bytes
	   AddReadWord = Add Read Handler for Words
	   AddReadBW   = Add Read Handler for Bytes+Words

	   AddWriteByte = Add Write Handler for Bytes
	   AddWriteWord = Add Write Handler for Words
	   AddWriteBW   = Add Write Handler for Bytes+Words

	   AddRWBW     = Add Read+Write Handler for Bytes+Words

	*/

	AddMemFetch (0x000000, 0x03ffff,                        ROM                      );
	AddMemFetch (0x320000, 0x32ffff,                        RAM_MAIN-0x320000        ); // note: you need to subtract the address?!

	/* ROM for normal READ Operations -- we don't want to allow writing to ROM! */
	AddReadBW   (0x000000, 0x03ffff,     NULL,              ROM             );   // 68000 ROM
	/* Set Up the RAM, we allow all types of READ/WRITE operations */
	AddRWBW     (0x100000, 0x10000f,     NULL,              RAM_PFCTRL      );   // scroll regs etc.

	AddRWBW     (0x120000, 0x120fff,     NULL,              RAM_PF1         );   // PLAYFIELD 1 DATA
	AddRWBW     (0x122000, 0x122fff,     NULL,              RAM_PF2         );   // PLAYFIELD 2 DATA
	AddRWBW     (0x200000, 0x2007ff,     NULL,              RAM_PALETTE     );   // PALETTE RAM
	AddRWBW     (0x210000, 0x2107ff,     NULL,              RAM_SPRITE      );   // SPRITE RAM

	AddWriteWord(0x270000, 0x270001,     sound_crospang16_w,NULL            );   // sound

	AddReadByte (0x280000, 0x28000f,     input_crospang_8r, NULL           );   // Inputs
	AddReadWord (0x280000, 0x28000f,     input_crospang_16r,NULL           );   // Inputs
	AddReadWord(0x3253fe,0x3253ff,my_speed_hack, NULL);
	AddRWBW     (0x320000, 0x32ffff,     NULL,              RAM_MAIN        );   // MAIN RAM



	/* Handler for the Inputs */

	/* Make sure ALL other READS and WRITES (both BYTE and WORD) fall through to the
	   appropriate Bad Read / Write functions */

	AddReadByte (0x000000, 0xffffff,     DefBadReadByte,    NULL            );   // Unhandled Byte Reads
	AddReadWord (0x000000, 0xffffff,     DefBadReadWord,    NULL            );   // Unhandled Word Reads
	AddWriteByte(0x000000, 0xffffff,     DefBadWriteByte,   NULL            );   // Unhandled Byte Writes
	AddWriteWord(0x000000, 0xffffff,     DefBadWriteWord,   NULL            );   // Unhandled Word Writes

	/* terminate memory list */
	AddMemFetch (-1, -1, NULL);
	AddRWBW     (-1,       -1,           NULL,              NULL            );
	/* Initialize Starscream based on everything we just set up */
	AddInitMemory();     // Set Starscream mem pointers...

	/* Note: In RAINE the palette is handled like this */
	InitPaletteMap(RAM_PALETTE, 0x60, 0x10, 0x8000);
	set_colour_mapper(&col_map_xrrr_rrgg_gggb_bbbb);  /* rrr_rrgg_gggb_bbbb !! */


}

static struct VIDEO_INFO video_crospang =
{
   draw_crospang,
   320,
   240,
   BORDER,
   VIDEO_ROTATE_NORMAL |
   VIDEO_ROTATABLE,
   crospang_gfx,
};
static struct DIR_INFO dir_crospang[] =
{
   { "cross_pang", },
   { "crospang", },
   { NULL, },
};
GME( crospang, "Cross Pang", TOAPLAN, 1998, GAME_MISC);

