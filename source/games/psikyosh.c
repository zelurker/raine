#include "gameinc.h"
#include "sh2.h"
#include "mame/handlers.h"
#include "sasound.h"
#include "ymf278b.h"
#include "mame/eeprom.h"
#include "savegame.h"
#include "zoom/16x16.h"
#include "alpha.h"
#include "blit.h"
#include "priorities.h"
#include "pdraw.h"

#define MASTER_CLOCK 57272700   // main oscillator frequency

static struct EEPROM_interface eeprom_interface_93C56 =
{
        8,              // address bits 8
        8,              // data bits    8
        "*110x",        // read         110x aaaaaaaa
        "*101x",        // write        101x aaaaaaaa dddddddd
        "*111x",        // erase        111x aaaaaaaa
        "*10000xxxxxxx",// lock         100x 00xxxx
        "*10011xxxxxxx",// unlock       100x 11xxxx
//  "*10001xxxx",   // write all    1 00 01xxxx dddddddddddddddd
//  "*10010xxxx"    // erase all    1 00 10xxxx
};

static UINT8 factory_eeprom[16]  = { 0x00,0x02,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x00,0x00 };
static al_bitmap *zoom_bitmap;
static UINT8 alphatable[256];
static int layer_id_data[4];
static char *layer_id_name[4] =
{
   "BG0", "BG1", "BG2", "sprites",
};

static u8 *ram_video, *ram_zoom, *bank,
    *ram_pal, *ram_spr,*ram_bg;

static struct ROM_INFO rom_gunbird2[] =
{
  LOAD_32_SWAP_16( CPU1, "2_prog_l.u16", 0x000002, 0x080000, 0x76f934f0),
  LOAD_32_SWAP_16( CPU1, "1_prog_h.u17", 0x000000, 0x080000, 0x7328d8bf),
  LOAD_SW16( CPU1, "3_pdata.u1", 0x100000, 0x080000, 0xa5b697e6),
  LOAD16_32( GFX1, "0l.u3", 0x0000000, 0x800000, 0x5c826bc8),
  LOAD16_32( GFX1, "0h.u10", 0x0000002, 0x800000, 0x3df0cb6c),
  LOAD16_32( GFX1, "1l.u4", 0x1000000, 0x800000, 0x1558358d),
  LOAD16_32( GFX1, "1h.u11", 0x1000002, 0x800000, 0x4ee0103b),
  LOAD16_32( GFX1, "2l.u5", 0x2000000, 0x800000, 0xe1c7a7b8),
  LOAD16_32( GFX1, "2h.u12", 0x2000002, 0x800000, 0xbc8a41df),
  LOAD16_32( GFX1, "3l.u6", 0x3000000, 0x400000, 0x0229d37f),
  LOAD16_32( GFX1, "3h.u13", 0x3000002, 0x400000, 0xf41bbf2b),
  LOAD( SMP1, "sound.u9", 0x000000, 0x400000, 0xf19796ab),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct DSW_DATA dsw_data_gunbird2_0[] =
{
    { "Region",2,3 },
  { "Japan", 0x0 },
  { "International Ver A.", 0x1 },
  { "International Ver B.", 0x2 },
  { NULL, 0}
};

static struct DSW_INFO dsw_gunbird2[] =
{
  { 0x7, 0x2, dsw_data_gunbird2_0 },
  { 0, 0, NULL }
};

static struct INPUT_INFO input_gunbird2[] =
{
  INP0( COIN1, 0x03, 0x01),
  INP0( COIN2, 0x03, 0x02),
  INP0( UNKNOWN, 0x03, 0x04),
  INP0( UNKNOWN, 0x03, 0x08),
  // there is a unique dip for service on 0x10, toggleable
  INP0( UNKNOWN, 0x03, 0x18),
  INP0( TEST, 0x03, 0x20),
  INP0( UNKNOWN, 0x03, 0x40), // debug stuff, resets eeprom
  INP0( UNKNOWN, 0x03, 0x80),

  INP0( UNKNOWN, 0x02, 0x01),
  INP0( UNKNOWN, 0x02, 0x02),
  INP0( UNKNOWN, 0x02, 0x04),
  INP0( UNKNOWN, 0x02, 0x08),
  INP0( UNKNOWN, 0x02, 0x10),
  INP0( UNKNOWN, 0x02, 0x20),
  INP0( UNKNOWN, 0x02, 0x40),
  INP0( UNKNOWN, 0x02, 0x80),

  INP0( P2_START, 0x01, 0x1),
  INP0( P2_B3, 0x01, 0x2),
  INP0( P2_B2, 0x01, 0x4),
  INP0( P2_B1, 0x01, 0x8),
  INP0( P2_LEFT, 0x01, 0x10),
  INP0( P2_RIGHT, 0x01, 0x20),
  INP0( P2_DOWN, 0x01, 0x40),
  INP0( P2_UP, 0x01, 0x80),

  INP0( P1_START, 0x00, 0x1),
  INP0( P1_B3, 0x00, 0x2),
  INP0( P1_B2, 0x00, 0x4),
  INP0( P1_B1, 0x00, 0x8),
  INP0( P1_LEFT, 0x00, 0x10),
  INP0( P1_RIGHT, 0x00, 0x20),
  INP0( P1_DOWN, 0x00, 0x40),
  INP0( P1_UP, 0x00, 0x80),
  END_INPUT
};

static void irq_handler(int irq) {
    if (irq)
	cpu_interrupt(CPU_SH2_0,12);
    printf("irq_handler %d\n",irq);
}

static struct YMF278B_interface ymf278b_interface =
{
	1,
	{ MASTER_CLOCK/2 },       /* 33.8688 MHz */
	{ REGION_SMP1 },
	{ YM3012_VOL(255, MIXER_PAN_CENTER, 255, MIXER_PAN_CENTER) },
	{ irq_handler }
};

static struct SOUND_INFO sound_gunbird2[] =
{
   { SOUND_YMF278B,  &ymf278b_interface,  },
   { 0,             NULL,               },
};

static u8 FASTCALL read_romb(u32 addr) {
    if (addr <= 0xfffff)
	return ROM[addr];
    return 0xff;
}

FASTCALL static u16 read_romw(u32 addr) {
    if (addr <= 0xffffe)
	return ReadWord68k(&ROM[addr]);
    return 0xffff;
}

static u32 FASTCALL read_roml(u32 addr) {
    if (addr <= 0xffffc) {
	return ReadLong68k(&ROM[addr]);
    }
    return 0xffffffff;
}

static u8 FASTCALL read_bankb(u32 offset) {
    offset &= 0xffffff;
    if (offset <= 0x7ffff)
	return bank[offset];
    return 0xff;
}

static u16 FASTCALL read_bankw(u32 offset) {
    offset &= 0xffffff;
    if (offset <= 0x7ffff)
	return ReadWord68k(&bank[offset]);
    return 0xffff;
}

static u32 FASTCALL read_bankl(u32 offset) {
    offset &= 0xffffff;
    if (offset <= 0x7ffff)
	return ReadLong68k(&bank[offset]);
    return 0xffffffff;
}


static u8 FASTCALL read_ramb(u32 offset) {
    offset &= 0xffffff;
    if (offset <= 0xfffff)
        return RAM[offset];
    return 0xff;
}

static u16 FASTCALL read_ramw(u32 offset) {
    offset &= 0xffffff;
    if (offset <= 0xfffff)
        return ReadWord68k(&RAM[offset]);
    return 0xffff;
}

static u32 FASTCALL read_raml(u32 offset) {
    offset &= 0xffffff;
    if (offset <= 0xfffff) {
	int ret = ReadLong68k(&RAM[offset]);
	if (offset == 0x40030) {
	    // This is equivalent to a speed hack here for gunbird2 but without modifying the rom
	    if (!ret) {
		M_SH2.Cycle_IO = 3;
	    }
	}
        return ret;
    }
    return 0xffffffff;
}

static void FASTCALL write_ramb(u32 offset,u8 data) {
    offset &= 0xffffff;
    if (offset <= 0xfffff)
        RAM[offset] = data;
}

static void FASTCALL write_ramw(u32 offset,u16 data) {
    offset &= 0xffffff;
    if (offset <= 0xfffff)
        WriteWord68k(&RAM[offset],data);
}

static void FASTCALL write_raml(u32 offset,u32 data) {
    offset &= 0xffffff;
    if (offset <= 0xfffff)
        WriteLong68k(&RAM[offset],data);
}

static u8 FASTCALL read_videob(u32 offset) {
    offset &= 0xffffff;
    if (offset <= 0xffff)
        return ram_spr[offset]; // bg & sprites are contiguous
    else if (offset >= 0x40000 && offset <= 0x44fff)
	return ram_pal[offset & 0xffff];
    else if (offset >= 0x50000 && offset <= 0x501ff)
	return ram_zoom[offset & 0x1ff];
    else if (offset >= 0x5ffdc && offset <= 0x5ffdf)
	return 0; // irq related
    else if (offset >= 0x5ffe0 && offset <= 0x5ffff)
	return ram_video[offset & 0x1f];

    return 0xff;
}

static u16 FASTCALL read_videow(u32 offset) {
    offset &= 0xffffff;
    if (offset <= 0xffff)
        return ReadWord68k(&ram_spr[offset]); // bg + sprites
    else if (offset >= 0x40000 && offset <= 0x44fff)
	return ReadWord68k(&ram_pal[offset & 0xffff]);
    else if (offset >= 0x50000 && offset <= 0x501ff)
	return ReadWord68k(&ram_zoom[offset & 0x1ff]);
    else if (offset >= 0x5ffdc && offset <= 0x5ffdf)
	return 0; // irq related
    else if (offset >= 0x5ffe0 && offset <= 0x5ffff)
	return ReadWord68k(&ram_video[offset & 0x1f]);

    return 0xffff;
}

static u32 FASTCALL read_videol(u32 offset) {
    offset &= 0xffffff;
    if (offset <= 0xffff)
        return ReadLong68k(&ram_spr[offset]); // bg + sprites
    else if (offset >= 0x40000 && offset <= 0x44fff)
	return ReadLong68k(&ram_pal[offset & 0xffff]);
    else if (offset >= 0x50000 && offset <= 0x501ff)
	return ReadLong68k(&ram_zoom[offset & 0x1ff]);
    else if (offset >= 0x5ffdc && offset <= 0x5ffdf)
	return 0; // irq related
    else if (offset >= 0x5ffe0 && offset <= 0x5ffff)
	return ReadLong68k(&ram_video[offset & 0x1f]);

    return 0xffffffff;
}

static void FASTCALL write_videob(u32 offset,u8 data) {
    offset &= 0xffffff;
    if (offset <= 0xffff)
        ram_spr[offset] = data; // bg + sprites
    else if (offset >= 0x40000 && offset <= 0x44fff)
	ram_pal[offset & 0xffff] = data;
    else if (offset >= 0x50000 && offset <= 0x501ff)
	ram_zoom[offset & 0x1ff] = data;
//    else if (offset >= 0x405ffdc && offset <= 0x405ffdf)
	// irq related
    else if (offset >= 0x5ffe0 && offset <= 0x5ffff)
	ram_video[offset & 0x1f] = data;
}

static void FASTCALL write_videow(u32 offset,u16 data) {
    offset &= 0xffffff;
    if (offset <= 0xffff) {
	// printf("ram_sprw %x = %x\n",offset,data);
        WriteWord68k(&ram_spr[offset],data); // bg + sprites
    } else if (offset >= 0x40000 && offset <= 0x44fff) {
	WriteWord68k(&ram_pal[offset & 0xffff],data);
	bank_status[(offset >> 6) & 0xff] = 0;
    } else if (offset >= 0x50000 && offset <= 0x501ff)
	WriteWord68k(&ram_zoom[offset & 0x1ff],data);
    // else if (offset >= 0x405ffdc && offset <= 0x405ffdf)
	// irq related
    else if (offset >= 0x5ffe0 && offset <= 0x5ffff)
	WriteWord68k(&ram_video[offset & 0x1f],data);
}

static void FASTCALL write_videol(u32 offset,u32 data) {
    offset &= 0xffffff;
    if (offset <= 0xffff) {
	// printf("ram_sprl %x = %x\n",offset,data);
        WriteLong68k(&ram_spr[offset],data); // bg + sprites
    } else if (offset >= 0x40000 && offset <= 0x44fff) {
	bank_status[(offset >> 6) & 0xff] = 0;
	WriteLong68k(&ram_pal[offset & 0xffff],data);
    } else if (offset >= 0x50000 && offset <= 0x501ff)
	WriteLong68k(&ram_zoom[offset & 0x1ff],data);
    // else if (offset >= 0x405ffdc && offset <= 0x405ffdf)
	// irq related
    else if (offset >= 0x5ffe0 && offset <= 0x5ffff)
	WriteLong68k(&ram_video[offset & 0x1f],data);
}

static u8 FASTCALL read_inputs_soundb(u32 offset) {
    offset &= 0xffffff;
    if (offset == 4) // special case here, 2 lowest bits = region, 0x10 = eeprom bit
	return (input_buffer[7] & 3) | ((EEPROM_read_bit() & 0x01) << 4);
    else if (offset < 7) {
	return input_buffer[offset];
    } else if (offset == 0x100000) {
	int status = YMF278B_status_port_0_r(0);
	return status;
    } else {
	printf("read unknown port %x\n",offset);
	return 0xff;
    }
}

static void FASTCALL write_sound(u32 offset,u8 data) {
    offset &= 0x7ffffff;
    // printf("write_sound %x,%x\n",offset,data);
    if (offset >= 0x3100000 && offset <= 0x3100007)
	ymf278b_0_w(offset,data);
    else if (offset == 0x3000004) {
	// printf("eeprom write\n");
	EEPROM_write_bit((data & 0x20) ? 1 : 0);
	EEPROM_set_cs_line((data & 0x80) ? CLEAR_LINE : ASSERT_LINE);
	EEPROM_set_clock_line((data & 0x40) ? ASSERT_LINE : CLEAR_LINE);
    }
}

#define BG_TYPE(n) (ram_video[6*4+n] & 0x7f )
#define BG_NORMAL      0x0a
#define BG_NORMAL_ALT  0x0b /* Same as above but with different location for scroll/priority reg */
#define DISPLAY_DISABLE (((ram_video[2*4+3] & 0xf) == 0x6) ? 1:0)
// For the << (4*n) it seems simpler to read the long and work on it, to avoid to work with half bytes !
#define BG_LARGE(n) (((ReadLong68k(&ram_video[7*4]) << (4*n)) & 0x00001000 ) ? 1:0)
#define BG_DEPTH_8BPP(n) (((ReadLong68k(&ram_video[7*4]) << (4*n)) & 0x00004000 ) ? 1:0)
#define BG_LAYER_ENABLE(n) (((ReadLong68k(&ram_video[7*4]) << (4*n)) & 0x00008000 ) ? 1:0)

static void load_gunbird2() {
    init_pbitmap();
    EEPROM_init(&eeprom_interface_93C56);
    default_eeprom = factory_eeprom;
    default_eeprom_size = sizeof(factory_eeprom);
    load_eeprom();
    SH2_Init(&M_SH2,0);
    bank = &ROM[0x100000];
    RAMSize = 0x100000 + // main ram 0x6000000
	0x20 + // video registers
	0x200 + // zoom ram 0x4050000
	0x5000 + // palette 0x4040000
	0x4000 + // sprite rama   0x4000000
	0xc000; // bg ram 0x4004000
    // ROM & RAM are supposed to be dword aligned, that is aligned on 4 bytes boundaries then, but it's been like that with malloc for ages, it's even 64 bits aligned for x64... !
    if (!(RAM = AllocateMem(RAMSize))) return;
    ram_video = &RAM[0x100000];
    ram_zoom = &ram_video[0x20];
    ram_pal = &ram_zoom[0x200];
    ram_spr = &ram_pal[0x5000];
    ram_bg = &ram_spr[0x4000];
    if (!(zoom_bitmap = create_bitmap_ex(8,256,256))) return;

    set_colour_mapper(&col_map_24bit_rgb);
    // the text layer has 16 colours, the other layers seem to have 256 colors...
    InitPaletteMap(ram_pal, 0x100, 0x100, 0x10000);
    init_16x16_zoom_64(); // Should have zoom...

    SH2_Add_Fetch(&M_SH2,0, 0x17ffff, (UINT16*)ROM);
    SH2_Add_Fetch(&M_SH2,0x6000000, 0x60fffff, (UINT16*)RAM); // Yeah it's used

    // cache : it's probably useless here, but with gens emulator it must be done...
    SH2_Add_Fetch(&M_SH2,0x20000000, 0x200fffff, (UINT16*)ROM);
    SH2_Add_Fetch(&M_SH2,0x26000000, 0x260fffff, (UINT16*)RAM); // just in case, not verified
								//
    SH2_Add_Fetch(&M_SH2, 0x00000000, 0x00000000, (UINT16 *) -1);

    // All the memory maps addresses except Fetch are >>24, so only the high byte here, a big potential source of errors !
    SH2_Add_ReadB(&M_SH2,0, 0, read_romb);
    SH2_Add_ReadW(&M_SH2,0, 0, read_romw);
    SH2_Add_ReadL(&M_SH2,0, 0, read_roml);

    SH2_Add_ReadB(&M_SH2,5, 5, read_bankb);
    SH2_Add_ReadW(&M_SH2,5, 5, read_bankw);
    SH2_Add_ReadL(&M_SH2,5, 5, read_bankl);

    SH2_Add_ReadB(&M_SH2,6, 6, read_ramb);
    SH2_Add_ReadW(&M_SH2,6, 6, read_ramw);
    SH2_Add_ReadL(&M_SH2,6, 6, read_raml);
    SH2_Add_WriteB(&M_SH2,6, 6, write_ramb);
    SH2_Add_WriteW(&M_SH2,6, 6, write_ramw);
    SH2_Add_WriteL(&M_SH2,6, 6, write_raml);

    SH2_Add_ReadB(&M_SH2,4, 4, read_videob);
    SH2_Add_ReadW(&M_SH2,4, 4, read_videow);
    SH2_Add_ReadL(&M_SH2,4, 4, read_videol);
    SH2_Add_WriteB(&M_SH2,4, 4, write_videob);
    SH2_Add_WriteW(&M_SH2,4, 4, write_videow);
    SH2_Add_WriteL(&M_SH2,4, 4, write_videol);

    SH2_Add_ReadB(&M_SH2,3, 3, &read_inputs_soundb);
    SH2_Add_WriteB(&M_SH2,3, 3, &write_sound);

    SH2_Map_Cache_Trough(&M_SH2);
    // alphatable initialized from color index...
    for (int i=0; i<0xc0; i++)
	alphatable[i] = 0xff;
    for (int i=0; i<0x40; i++) {
	int alpha = ((0x3f-i)*0xff)/0x3f;
	alphatable[i+0xc0] = alpha;
    }
    for (int i=0; i<4; i++)
	layer_id_data[i] = add_layer_info(layer_id_name[i]);
}

static void execute_gunbird2() {
    cpu_execute_cycles(CPU_SH2_0,MASTER_CLOCK/2/60);
    cpu_interrupt(CPU_SH2_0,4); // vbl
}

/* zoomx/y are pixel slopes in 6.10 floating point, not scale. 0x400 is 1:1 */
/* high/wide are number of tiles wide/high up to max size of zoom_bitmap in either direction */
/* code is index of first tile and incremented across rows then down columns (adjusting for flip obviously) */
/* sx and sy is top-left of entire sprite regardless of flip */
/* Note that Level 5-4 of sbomberb boss is perfect! (Alpha blended zoomed) as well as S1945II logo */
/* pixel is only plotted if z is >= priority_buffer[y][x] */
static void psikyosh_drawgfxzoom(
		int region,
		UINT32 code,u8 *map,int flipx,int flipy,int offsx,int offsy,
		int alpha,
		int zoomx, int zoomy, int wide, int high,int pri )
{
    // A version without the z parameter, that is without inter sprites priorities
    // I am annoyed by the idea of a 16 bits priority bitmap just to be able to store the sequence number of each sprite in it, I'd like to avoid it but I need to see more examples of this thing
    // in action. For now gunbird2 doesn't seem to use inter sprites priorities so I'll start with a version without this stuff.
    int code_offset = 0;
    int xtile, ytile,xpixel, ypixel;

    if (!zoomx || !zoomy) return;
    // printf("region %d code %x zoomx %x zoomy %x\n",region,code,zoomx,zoomy);


    /* Temporary fallback for non-zoomed, needs z-buffer. Note that this is probably a lot slower than drawgfx.c, especially if there was seperate code for flipped cases */
    if (zoomx == 0x400 && zoomy == 0x400)
    {
	int xstart, ystart, xend, yend, xinc, yinc;

	if (flipx)	{ xstart = wide-1; xend = -1;   xinc = -1; }
	else		{ xstart = 0;      xend = wide; xinc = +1; }

	if (flipy)	{ ystart = high-1; yend = -1;   yinc = -1; }
	else		{ ystart = 0;      yend = high; yinc = +1; }

	/* Start drawing */
	for (ytile = ystart; ytile != yend; ytile += yinc )
	{
	    for (xtile = xstart; xtile != xend; xtile += xinc )
	    {
		const u32 code_base = (code + code_offset++) % max_sprites[region];

		/* start coordinates */
		int sx = offsx + xtile*16 + 16;
		int sy = offsy + ytile*16 + 16;

		if (!gfx_solid[region][code_base] ||
			sx <= 0 || sx >= current_game->video->screen_x + current_game->video->border_size ||
			sy <= 0 || sy >= current_game->video->screen_y + current_game->video->border_size) // all transp
		    continue;

		/* case 1: TRANSPARENCY_PEN */
		if (alpha == 0xff)
		{

		    if(gfx_solid[region][code_base]==1)                    // Some pixels; trans
			pdraw16x16_Trans_Mapped_back_flip_Rot(&gfx[region][code_base<<8],sx,sy,map,flipx | (flipy<<1),pri);
		    else
			pdraw16x16_Mapped_back_flip_Rot(&gfx[region][code_base<<8],sx,sy,map,flipx | (flipy<<1),pri);
		}

		/* case 6: alpha-blended */
		else if (alpha >= 0)
		{
		    {

			set_alpha(alpha);
			if(gfx_solid[region][code_base]==1)                    // Some pixels; trans
			    pdraw16x16_Trans_Mapped_back_Alpha_flip_Rot(&gfx[region][code_base<<8],sx,sy,map,flipx | (flipy<<1),pri);
			else
			    pdraw16x16_Mapped_back_Alpha_flip_Rot(&gfx[region][code<<8],sx,sy,map,flipx | (flipy<<1),pri);
		    }
		}

		/* pjp 31/5/02 */
		/* case 7: TRANSPARENCY_ALPHARANGE */
		else
		{
		    // printf("sprite alphatable not supported (not zoomed)\n");
		    // temp workaround : just ignore the alphatable for now
		    if(gfx_solid[region][code_base]==1)                    // Some pixels; trans
			pdraw16x16_Trans_Mapped_back_flip_Rot(&gfx[region][code_base<<8],sx,sy,map,flipx | (flipy<<1),pri);
		    else
			pdraw16x16_Mapped_back_flip_Rot(&gfx[region][code_base<<8],sx,sy,map,flipx | (flipy<<1),pri);
		}

	    }
	}
    }
    else /* Zoomed */
    {
	// The very 1st screen of gunbird2 is already asking for a zzom of 128 !!!
	// so the idea from mame to use an intermediate bitmap and zoom from this bitmap is really excellent here
	// another approach would be to use real opengl zooming functions, but I'd like to keep this for later...
	for(ytile=0; ytile<high; ytile++)
	{
	    for(xtile=0; xtile<wide; xtile++)
	    {
		const UINT8 *source = &gfx[region][((code + code_offset++) % max_sprites[region])<<8];
		for( ypixel=0; ypixel<16; ypixel++ )
		{
		    UINT8 *dest = zoom_bitmap->line[ypixel + ytile*16];

		    for( xpixel=0; xpixel<16; xpixel++ )
		    {
			dest[xpixel + xtile*16] = *source++;
		    }
		}
	    }
	}
	const VIDEO_INFO *vid = current_game->video;
	int rotate = vid->flags ^ display_cfg.user_rotate;
	int disp_x_16 = vid->screen_x + 2*vid->border_size - 16;

	/* Start drawing */
	int sprite_screen_height = ((high*16*(0x400*0x400))/zoomy + 0x200)>>10; /* Round up to nearest pixel */
	int sprite_screen_width = ((wide*16*(0x400*0x400))/zoomx + 0x200)>>10;

	if (sprite_screen_width && sprite_screen_height)
	{
	    /* start coordinates */
	    int sx = offsx + 16;
	    int sy = offsy + 16;

	    /* end coordinates */
	    int ex = sx + sprite_screen_width;
	    int ey = sy + sprite_screen_height;
	    ex = MIN(ex, current_game->video->screen_x + current_game->video->border_size);
	    ey = MIN(ey, current_game->video->screen_y + current_game->video->border_size);
	    if (sx < 0) sx = 0;
	    if (sy < 0) sy = 0;

	    int x_index_base;
	    int y_index;

	    int dx, dy;

	    if( flipx )	{ x_index_base = (sprite_screen_width-1)*zoomx; dx = -zoomx; }
	    else		{ x_index_base = 0; dx = zoomx; }

	    if( flipy )	{ y_index = (sprite_screen_height-1)*zoomy; dy = -zoomy; }
	    else		{ y_index = 0; dy = zoomy; }

	    if( ex>sx )
	    { /* skip if inner loop doesn't draw anything */
		int y;

		/* case 1: TRANSPARENCY_PEN */
		/* Note: adjusted to >>10 and draws from zoom_bitmap not gfx */
		if (alpha == 0xff)
		{
		    if (rotate == 0) {
			for( y=sy; y<ey; y++ )
			{
			    UINT8 *source = zoom_bitmap->line[y_index>>10];
			    UINT32 *dest = (u32*)GameBitmap->line[y];

			    int x, x_index = x_index_base;
			    for( x=sx; x<ex; x++ )
			    {
				int c = source[x_index>>10];
				if( c != 0 ) dest[x] = map[c];
				x_index += dx;
			    }

			    y_index += dy;
			}
		    } else if (rotate == 3) { // 270
			for( y=sy; y<ey; y++ )
			{
			    UINT8 *source = zoom_bitmap->line[y_index>>10];

			    int x, x_index = x_index_base;
			    for( x=sx; x<ex; x++ )
			    {
				UINT32 *dest = (u32*)GameBitmap->line[disp_x_16-x];
				int c = source[x_index>>10];
				if( c != 0 ) dest[y] = map[c];
				x_index += dx;
			    }

			    y_index += dy;
			}
		    }
		}

		/* case 6: alpha-blended */
		else if (alpha >= 0)
		{
		    set_alpha(alpha);
		    if (rotate == 0) {
			for( y=sy; y<ey; y++ )
			{
			    UINT8 *source = zoom_bitmap->line[y_index>>10];
			    UINT32 *dest = (u32*)GameBitmap->line[y];

			    int x, x_index = x_index_base;
			    for( x=sx; x<ex; x++ )
			    {
				int c = source[x_index>>10];
				if( c != 0 ) blend_32(&dest[x], map[c]);
				x_index += dx;
			    }

			    y_index += dy;
			}
		    } else if (rotate == 3) { // 270
			for( y=sy; y<ey; y++ )
			{
			    UINT8 *source = zoom_bitmap->line[y_index>>10];

			    int x, x_index = x_index_base;
			    for( x=sx; x<ex; x++ )
			    {
				UINT32 *dest = (u32*)GameBitmap->line[disp_x_16 - x];
				int c = source[x_index>>10];
				if( c != 0 ) blend_32(&dest[y], map[c]);
				x_index += dx;
			    }

			    y_index += dy;
			}
		    }
		}

		/* case 7: TRANSPARENCY_ALPHARANGE */
		else
		{
		    if (rotate == 0) {
			for( y=sy; y<ey; y++ )
			{
			    UINT8 *source = zoom_bitmap->line[y_index>>10];
			    UINT32 *dest = (u32*)GameBitmap->line[y];

			    int x, x_index = x_index_base;
			    for( x=sx; x<ex; x++ )
			    {
				int c = source[x_index>>10];
				if( c != 0 )
				{
				    if( alphatable[c] == 0xff )
					dest[x] = map[c];
				    else {
					set_alpha(alphatable[c]);
					blend_32(&dest[x], map[c]);
				    }
				}
				x_index += dx;
			    }

			    y_index += dy;
			}
		    } else {
			printf("spr trans map not supported\n");
		    }
		}
	    }
	}
    }
}

#define SPRITE_PRI(n) (((ReadLong68k(&ram_video[2*4]) << (4*n)) & 0xf0000000 ) >> 28)

#undef BYTE_XOR_BE
#undef BYTE4_XOR_BE
#define BYTE_XOR_BE(x) x
#define BYTE4_XOR_BE(x) x

static void draw_sprites( )
{
	/*- Sprite Format 0x0000 - 0x37ff -**

    0 ---- --yy yyyy yyyy | ---- --xx xxxx xxxx  1  F--- hhhh ZZZZ ZZZZ | fPPP wwww zzzz zzzz
    2 pppp pppp -aaa -nnn | nnnn nnnn nnnn nnnn  3  ---- ---- ---- ---- | ---- ---- ---- ----

    y = ypos
    x = xpos

    h = height
    w = width

    F = flip (y)
    f = flip (x)

    Z = zoom (y)
    z = zoom (x)

    n = tile number

    p = palette

    a = alpha blending, selects which of the 8 alpha values in vid_regs[0-1] to use

    P = priority
    Points to a 4-bit entry in vid_regs[2] which provides a priority comparable with the bg layer's priorities.
    However, sprite-sprite priority needs to be preserved.
    daraku and soldivid only use the lsb

    **- End Sprite Format -*/

	int spr;
	UINT32 *src = (u32*)ram_spr; /* Use buffered spriteram */
	UINT16 *list = (UINT16 *)src + 0x3800/2;
	UINT16 listlen=0x800/2, listcntr=0;
	UINT16 *zoom_table = (UINT16 *)ram_zoom;
	UINT8  *alpha_table = ram_video;

	while( listcntr < listlen )
	{
		UINT32 listdat, sprnum, xpos, ypos, high, wide, flpx, flpy, zoomx, zoomy, tnum, colr, dpth;
		UINT32 pri, alphamap;
		int alpha;

		listdat = ReadWord68k(&list[BYTE_XOR_BE(listcntr)]);
		sprnum = (listdat & 0x03ff) * 4;

		pri  = (ReadLong68k(&src[sprnum+1]) & 0x00003000) >> 12; // & 0x00007000/0x00003000 ?
		pri = SPRITE_PRI(pri);

		{
		    u8 *map;
			ypos = (ReadLong68k(&src[sprnum+0]) & 0x03ff0000) >> 16;
			xpos = (ReadLong68k(&src[sprnum+0]) & 0x000003ff) >> 00;

			if(ypos & 0x200) ypos -= 0x400;
			if(xpos & 0x200) xpos -= 0x400;

			high  = ((ReadLong68k(&src[sprnum+1]) & 0x0f000000) >> 24) + 1;
			wide  = ((ReadLong68k(&src[sprnum+1]) & 0x00000f00) >> 8) + 1;
			dpth  = (ReadLong68k(&src[sprnum+2]) & 0x00800000) >> 23;
			spr = (dpth ? 1 : 0);
			tnum  = (ReadLong68k(&src[sprnum+2]) & 0x0007ffff);
			if (high == 1 && wide == 1 && !gfx_solid[spr][tnum % max_sprites[spr]]) {
			    listcntr++;
			    if (listdat & 0x4000) break;
			    continue;
			}

			flpy  = (ReadLong68k(&src[sprnum+1]) & 0x80000000) >> 31;
			flpx  = (ReadLong68k(&src[sprnum+1]) & 0x00008000) >> 15;

			zoomy = (ReadLong68k(&src[sprnum+1]) & 0x00ff0000) >> 16;
			zoomx = (ReadLong68k(&src[sprnum+1]) & 0x000000ff);

			colr  = (ReadLong68k(&src[sprnum+2]) & 0xff000000) >> 24;

			alpha = (ReadLong68k(&src[sprnum+2]) & 0x00700000) >> 20;

			alphamap = (alpha_table[BYTE4_XOR_BE(alpha)] & 0x80)? 1:0;
			alpha = alpha_table[BYTE4_XOR_BE(alpha)] & 0x3f;

			if (dpth) {
			    MAP_PALETTE_MULTI_MAPPED_NEW(
				    colr,
				    256,
				    map
				    );
			} else {
			    MAP_PALETTE_MAPPED_NEW(
				    colr,
				    16, // ?
				    map
				    );
			}

			if(alphamap) { /* alpha values are per-pen */
				alpha = -1;
			} else {
				alpha = ((0x3f-alpha)*0xff)/0x3f; /* 0x3f-0x00 maps to 0x00-0xff */
			}

			/* start drawing */
			if( ReadWord68k(&zoom_table[BYTE_XOR_BE(zoomy)]) && ReadWord68k(&zoom_table[BYTE_XOR_BE(zoomx)]) ) /* Avoid division-by-zero when table contains 0 (Uninitialised/Bug) */
			{
				psikyosh_drawgfxzoom(spr,tnum,map,flpx,flpy,xpos,ypos,alpha,
					(UINT32)ReadWord68k(&zoom_table[BYTE_XOR_BE(zoomx)]), (UINT32)ReadWord68k(&zoom_table[BYTE_XOR_BE(zoomy)]), wide, high,pri); // , listcntr);

#if 0
#ifdef MAME_DEBUG
				if (input_code_pressed(KEYCODE_Z))	/* Display some info on each sprite */
				{
					char buf[10];
					int x, y;

					sprintf(buf, "%X",xdim/16); /* Display Zoom in 16.16 */
					if (machine->gamedrv->flags & ORIENTATION_SWAP_XY) {
						x = ypos;
						y = video_screen_get_visible_area(machine->primary_screen)->max_x - xpos; /* ORIENTATION_FLIP_Y */
					}
					else {
						x = xpos;
						y = ypos;
					}
					ui_draw_text(buf, x, y);
				}
#endif
#endif
			}
			/* end drawing */
		}
		listcntr++;
		if (listdat & 0x4000) break;
	}
}

static void drawgfx_alphatable(int region,
		UINT32 code, UINT32 color, INT32 destx, INT32 desty,
		int fixedalpha, int pri)
{
    code %= max_sprites[region];
    destx += 16; desty += 16;
    if (!gfx_solid[region][code] ||
	    destx <= 0 || destx >= current_game->video->screen_x + current_game->video->border_size ||
	    desty <= 0 || desty >= current_game->video->screen_y + current_game->video->border_size) // all transp
	return;
    u8 *map;
    if (region == 1) { // 8bpp
	MAP_PALETTE_MULTI_MAPPED_NEW(
		color,
		256,
		map
		);
    } else {
	MAP_PALETTE_MAPPED_NEW(
		color,
		16, // ?
		map
		);
    }
    /* if we have a fixed alpha, call the standard drawgfx_alpha */
    if (fixedalpha == 255) { // all opaque
	if(gfx_solid[region][code]==1)                    // Some pixels; trans
	    pdraw16x16_Test_Trans_Mapped_Rot(&gfx[region][code<<8],destx,desty,map,pri);
	else
	    pdraw16x16_Test_Mapped_Rot(&gfx[region][code<<8],destx,desty,map,pri);
    } else if (fixedalpha >= 0)
    {
	set_alpha(fixedalpha);
	if(gfx_solid[region][code]==1)                    // Some pixels; trans
	    pdraw16x16_Test_Trans_Mapped_Alpha_Rot(&gfx[region][code<<8],destx,desty,map,pri);
	else
	    pdraw16x16_Test_Mapped_Alpha_Rot(&gfx[region][code<<8],destx,desty,map,pri);
    } else {

	// No alpha table here for now, just display the sprite normally
	if(gfx_solid[region][code]==1)                    // Some pixels; trans
	    pdraw16x16_Test_Trans_Mapped_Rot(&gfx[region][code<<8],destx,desty,map,pri);
	else
	    pdraw16x16_Test_Mapped_Rot(&gfx[region][code<<8],destx,desty,map,pri);
    }
}

/* 'Normal' layers, no line/columnscroll. No per-line effects */
static void draw_bglayer(int layer, int pri )
{
	int offs=0, sx, sy, gfx;
	int scrollx, scrolly, bank, alpha, alphamap, size, width;

	if ( BG_TYPE(layer) == BG_NORMAL_ALT )
	{
		bank    = ram_bg[0x1ff0 + layer*4+3];
		alpha   = ram_bg[0x1ff0 + layer*4+2] & 0x3f;
		alphamap= (ram_bg[0x1ff0 + layer*4+2] & 0x80) >> 7;
		scrollx = (ReadWord68k(&ram_bg[0x1bf0 + layer*4+2]) & 0x01ff);
		scrolly = (ReadWord68k(&ram_bg[0x1bf0 + layer*4]) & 0x03ff);
	}
	else /* BG_NORMAL */
	{
		bank    = ram_bg[0x17f0 + layer*4+3];
		alpha   = ram_bg[0x17f0 + layer*4+2] & 0x3f;
		alphamap= (ram_bg[0x17f0 + layer*4+2] & 0x80) >> 7;
		scrollx = (ReadWord68k(&ram_bg[0x13f0 + layer*4+2]) & 0x000001ff);
		scrolly = (ReadWord68k(&ram_bg[0x13f0 + layer*4]) & 0x03ff);
	}

	gfx = BG_DEPTH_8BPP(layer) ? 1 : 0;
	size = BG_LARGE(layer) ? 32 : 16;
	width = BG_LARGE(layer) ? 0x200 : 0x100;

	if(alphamap) { /* alpha values are per-pen */
		alpha = -1;
	} else {
		alpha = ((0x3f-alpha)*0xff)/0x3f; /* 0x3f-0x00 maps to 0x00-0xff */
	}

	if((bank>=0x0c) && (bank<=0x1f)) /* shouldn't happen, 20 banks of 0x800 bytes */
	{
		for (sy=0; sy<size; sy++)
		{
			for (sx=0; sx<32; sx++)
			{
				int tileno, colour;

				tileno = ReadLong68k(&ram_bg[(bank*0x800) + offs*4 - 0x4000]); /* seems to take into account spriteram, hence -0x4000 */
				colour = (tileno & 0xff000000) >> 24;
				tileno &= 0x0007ffff;

				drawgfx_alphatable(gfx,tileno,colour,(16*sx+scrollx)&0x1ff,((16*sy+scrolly)&(width-1)),alpha,pri); /* normal */
				if(scrollx)
					drawgfx_alphatable(gfx,tileno,colour,((16*sx+scrollx)&0x1ff)-0x200,((16*sy+scrolly)&(width-1)),alpha,pri); /* wrap x */
				if(scrolly)
					drawgfx_alphatable(gfx,tileno,colour,(16*sx+scrollx)&0x1ff,((16*sy+scrolly)&(width-1))-width,alpha,pri); /* wrap y */
				if(scrollx && scrolly)
					drawgfx_alphatable(gfx,tileno,colour,((16*sx+scrollx)&0x1ff)-0x200,((16*sy+scrolly)&(width-1))-width,alpha,pri); /* wrap xy */

				offs++;
			}
		}
	}
}

static void draw_bg() {
    int i;
    for(i=0; i<3; i++)
    {
	if ( !BG_LAYER_ENABLE(i) || !check_layer_enabled(layer_id_data[i]) ) {
	    continue;
	}

	switch(BG_TYPE(i))
	{
	case BG_NORMAL:
	    draw_bglayer( i,ram_bg[0x17f0 + i*4]);
	    break;
	case BG_NORMAL_ALT:
	    draw_bglayer( i,ram_bg[0x1ff0 + i*4]);
	    break;
	default:
	    printf("layer %d bg type %x\n",i,BG_TYPE(i));
	}
    }
}

static void draw_gunbird2() {
    if (RefreshBuffers) {
	printf("gfx1 %p gfx2 %p\n",load_region[REGION_GFX1],load_region[REGION_GFX2]);
    }
    clear_game_screen(0);
    clear_bitmap(pbitmap);
    // ClearPaletteMap();
    if (check_layer_enabled(layer_id_data[3]))
	draw_sprites();
    draw_bg();
}

static gfx_layout layout_16x16x4 =
{
	16,16,
	RGN_FRAC(1,1),
	4,
	{STEP4(0,1)},
	{STEP16(0,4)},
	{STEP16(0,16*4)},
	16*16*4
};

static gfx_layout layout_16x16x8 =
{
	16,16,
	RGN_FRAC(1,1),
	8,
	{STEP8(0,1)},
	{STEP16(0,8)},
	{STEP16(0,16*8)},
	16*16*8
};

static struct GFX_LIST psikyosh_gfx[] =
{
	{ REGION_GFX1, &layout_16x16x4 }, // 0x000, 0x100 ) // 4bpp tiles
	{ REGION_GFX1, &layout_16x16x8,}, // 0x000, 0x100 ) // 8bpp tiles
   { 0,           NULL,             },
};

static struct VIDEO_INFO video_gunbird2 =
{
   draw_gunbird2,
   320,
   224,
   16,
   VIDEO_ROTATE_270 | VIDEO_ROTATABLE,
   psikyosh_gfx
};

static void clear_gunbird2() {
    destroy_bitmap(zoom_bitmap);
    zoom_bitmap = NULL;
}

GMEI( gunbird2,"Gunbird 2",PSIKYO,1998, GAME_SHOOT,
	.dsw = dsw_gunbird2, .clear = clear_gunbird2);
