#include "gameinc.h"
#include "sh2.h"
#include "mame/handlers.h"
#include "sasound.h"
#include "ymf278b.h"
#include "mame/eeprom.h"
#include "savegame.h"

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

static u8 *ram_video, *ram_zoom, *bank,
    *ram_pal, *ram_spr;

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

static struct ROMSW_DATA romsw_data_gunbird2_1[] =
{
  { "Japan", 0x0 },
  { "International Ver A.", 0x1 },
  { "International Ver B.", 0x2 },
  { NULL, 0}
};

static struct ROMSW_INFO romsw_gunbird2[] =
{
  { 0x4, 0x2, romsw_data_gunbird2_1 },
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
  INP0( SERVICE, 0x03, 0x20),
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
	cpu_interrupt(CPU_SH2,12);
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

#define dsw_gunbird2 NULL

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
        return ReadLong68k(&RAM[offset]);
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
        return ram_spr[offset];
    else if (offset >= 0x40000 && offset <= 0x44fff)
	return ram_pal[offset & 0xffff];
    else if (offset >= 0x4050000 && offset <= 0x40501ff)
	return ram_zoom[offset & 0x1ff];
    else if (offset >= 0x405ffdc && offset <= 0x405ffdf)
	return 0; // irq related
    else if (offset >= 0x405ffe0 && offset <= 0x405ffff)
	return ram_video[offset & 0x1f];

    return 0xff;
}

static u16 FASTCALL read_videow(u32 offset) {
    offset &= 0xffffff;
    if (offset <= 0xffff)
        return ReadWord68k(&ram_spr[offset]);
    else if (offset >= 0x40000 && offset <= 0x44fff)
	return ReadWord68k(&ram_pal[offset & 0xffff]);
    else if (offset >= 0x4050000 && offset <= 0x40501ff)
	return ReadWord68k(&ram_zoom[offset & 0x1ff]);
    else if (offset >= 0x405ffdc && offset <= 0x405ffdf)
	return 0; // irq related
    else if (offset >= 0x405ffe0 && offset <= 0x405ffff)
	return ReadWord68k(&ram_video[offset & 0x1f]);

    return 0xffff;
}

static u32 FASTCALL read_videol(u32 offset) {
    offset &= 0xffffff;
    if (offset <= 0xffff)
        return ReadLong68k(&ram_spr[offset]);
    else if (offset >= 0x40000 && offset <= 0x44fff)
	return ReadLong68k(&ram_pal[offset & 0xffff]);
    else if (offset >= 0x4050000 && offset <= 0x40501ff)
	return ReadLong68k(&ram_zoom[offset & 0x1ff]);
    else if (offset >= 0x405ffdc && offset <= 0x405ffdf)
	return 0; // irq related
    else if (offset >= 0x405ffe0 && offset <= 0x405ffff)
	return ReadLong68k(&ram_video[offset & 0x1f]);

    return 0xffffffff;
}

static void FASTCALL write_videob(u32 offset,u8 data) {
    offset &= 0xffffff;
    if (offset <= 0xffff)
        ram_spr[offset] = data;
    else if (offset >= 0x40000 && offset <= 0x44fff)
	ram_pal[offset & 0xffff] = data;
    else if (offset >= 0x4050000 && offset <= 0x40501ff)
	ram_zoom[offset & 0x1ff] = data;
//    else if (offset >= 0x405ffdc && offset <= 0x405ffdf)
	// irq related
    else if (offset >= 0x405ffe0 && offset <= 0x405ffff)
	ram_video[offset & 0x1f] = data;
}

static void FASTCALL write_videow(u32 offset,u16 data) {
    offset &= 0xffffff;
    if (offset <= 0xffff)
        WriteWord68k(&ram_spr[offset],data);
    else if (offset >= 0x40000 && offset <= 0x44fff)
	WriteWord68k(&ram_pal[offset & 0xffff],data);
    else if (offset >= 0x4050000 && offset <= 0x40501ff)
	WriteWord68k(&ram_zoom[offset & 0x1ff],data);
    // else if (offset >= 0x405ffdc && offset <= 0x405ffdf)
	// irq related
    else if (offset >= 0x405ffe0 && offset <= 0x405ffff)
	WriteWord68k(&ram_video[offset & 0x1f],data);
}

static void FASTCALL write_videol(u32 offset,u32 data) {
    offset &= 0xffffff;
    if (offset <= 0xffff)
        WriteLong68k(&ram_spr[offset],data);
    else if (offset >= 0x40000 && offset <= 0x44fff)
	WriteLong68k(&ram_pal[offset & 0xffff],data);
    else if (offset >= 0x4050000 && offset <= 0x40501ff)
	WriteLong68k(&ram_zoom[offset & 0x1ff],data);
    // else if (offset >= 0x405ffdc && offset <= 0x405ffdf)
	// irq related
    else if (offset >= 0x405ffe0 && offset <= 0x405ffff)
	WriteLong68k(&ram_video[offset & 0x1f],data);
}

FASTCALL static u8 read_inputs_soundb(u32 offset) {
    offset &= 0xffffff;
    if (offset == 4) // special case here, 2 lowest bits = region, 0x10 = eeprom bit
	return (input_buffer[7] & 3) | ((EEPROM_read_bit() & 0x01) << 4);
    else if (offset < 7)
	return input_buffer[offset];
    else if (offset == 0x100000) {
	int status = YMF278B_status_port_0_r(0);
	printf("sound status %x\n",status);
	return status;
    } else
	return 0xff;
}

FASTCALL static void write_sound(u32 offset,u8 data) {
    offset &= 0x7ffffff;
    printf("write_sound %x,%x\n",offset,data);
    if (offset >= 0x3100000 && offset <= 0x3100007)
	ymf278b_0_w(offset,data);
    else if (offset == 0x3000004) {
	printf("eeprom write\n");
	EEPROM_write_bit((data & 0x20) ? 1 : 0);
	EEPROM_set_cs_line((data & 0x80) ? CLEAR_LINE : ASSERT_LINE);
	EEPROM_set_clock_line((data & 0x40) ? ASSERT_LINE : CLEAR_LINE);
    }
}

static void load_gunbird2() {
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
	0xc000 + // bg rama   0x4004000
	0x4000; // sprite ram 0x4000000
    if (!(RAM = AllocateMem(RAMSize))) return;
    ram_video = &RAM[0x100000];
    ram_zoom = &ram_video[0x20];
    ram_pal = &ram_zoom[0x200];
    ram_spr = &ram_pal[0x5000];

    SH2_Add_Fetch(&M_SH2,0, 0x17ffff, (UINT16*)ROM);
    SH2_Add_Fetch(&M_SH2,0x6000000, 0x60fffff, (UINT16*)RAM); // Yeah it's used

    // cache : it's probably useless here, but with gens emulator it must be done...
    SH2_Add_Fetch(&M_SH2,0x20000000, 0x200fffff, (UINT16*)ROM);
    SH2_Add_Fetch(&M_SH2,0x26000000, 0x260fffff, (UINT16*)RAM); // just in case, not verified
								//
    SH2_Add_Fetch(&M_SH2, 0x00000000, 0x00000000, (UINT16 *) -1);

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
}

static void execute_gunbird2() {
    cpu_execute_cycles(CPU_SH2,MASTER_CLOCK/2/60);
    cpu_interrupt(CPU_SH2,4); // vbl
}

static void draw_gunbird2() {
    if (RefreshBuffers) {
	printf("gfx1 %p gfx2 %p\n",load_region[REGION_GFX1],load_region[REGION_GFX2]);
    }
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

GMEI( gunbird2,"Gunbird 2",PSIKYO,1998, GAME_SHOOT,
	.romsw = romsw_gunbird2);
