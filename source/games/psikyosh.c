#define DRV_DEF_SOUND sound_gunbird2
#define DRV_DEF_EXEC execute_gunbird2
#define DRV_DEF_INPUT input_gunbird2
#define DRV_DEF_CLEAR clear_gunbird2
#define DRV_DEF_LOAD load_gunbird2
#define DRV_DEF_VIDEO &video_gunbird2
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

static al_bitmap *zoom_bitmap;
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
  { "eeprom-gunbird2.bin", 0x0100, 0x7ac38846, REGION_EEPROM, 0x0000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_s1945iii[] =
{
  { "2_progl.u16", 0x080000, 0x5d5d385f, REGION_CPU1, 0x000002, LOAD32_SWAP_16 },
  { "1_progh.u17", 0x080000, 0x1b8a5a18, REGION_CPU1, 0x000000, LOAD32_SWAP_16 },
  { "3_data.u1", 0x080000, 0x8ff5f7d3, REGION_CPU1, 0x100000, LOAD_SWAP_16 },
  { "0l.u3", 0x800000, 0x70a0d52c, REGION_GFX1, 0x0000000, LOAD_16_32 },
  { "0h.u10", 0x800000, 0x4dcd22b4, REGION_GFX1, 0x0000002, LOAD_16_32 },
  { "1l.u4", 0x800000, 0xde1042ff, REGION_GFX1, 0x1000000, LOAD_16_32 },
  { "1h.u11", 0x800000, 0xb51a4430, REGION_GFX1, 0x1000002, LOAD_16_32 },
  { "2l.u5", 0x800000, 0x23b02dca, REGION_GFX1, 0x2000000, LOAD_16_32 },
  { "2h.u12", 0x800000, 0x9933ab04, REGION_GFX1, 0x2000002, LOAD_16_32 },
  { "3l.u6", 0x400000, 0xf693438c, REGION_GFX1, 0x3000000, LOAD_16_32 },
  { "3h.u13", 0x400000, 0x2d0c334f, REGION_GFX1, 0x3000002, LOAD_16_32 },
  { "sound.u9", 0x400000, 0xc5374beb, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "eeprom-s1945iii.bin", 0x0100, 0xb39f3604, REGION_EEPROM, 0x0000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_s1945ii[] =
{
  LOAD_32_SWAP_16( CPU1, "2_prog_l.u18", 0x000002, 0x080000, 0x20a911b8),
  LOAD_32_SWAP_16( CPU1, "1_prog_h.u17", 0x000000, 0x080000, 0x4c0fe85e),
  LOAD16_32( GFX1, "0l.u4", 0x0000000, 0x400000, 0xbfacf98d),
  LOAD16_32( GFX1, "0h.u13", 0x0000002, 0x400000, 0x1266f67c),
  LOAD16_32( GFX1, "1l.u3", 0x0800000, 0x400000, 0x2d3332c9),
  LOAD16_32( GFX1, "1h.u12", 0x0800002, 0x400000, 0x27b32c3e),
  LOAD16_32( GFX1, "2l.u2", 0x1000000, 0x400000, 0x91ba6d23),
  LOAD16_32( GFX1, "2h.u20", 0x1000002, 0x400000, 0xfabf4334),
  LOAD16_32( GFX1, "3l.u1", 0x1800000, 0x400000, 0xa6c3704e),
  LOAD16_32( GFX1, "3h.u19", 0x1800002, 0x400000, 0x4cd3ca70),
  LOAD( SOUND1, "sound.u32", 0x000000, 0x400000, 0xba680ca7),
  LOAD( EEPROM, "eeprom-s1945ii.bin", 0x0000, 0x0100, 0x7ac38846),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_tgm2[] =
{
  LOAD_32_SWAP_16( CPU1, "2.u21", 0x000000, 0x080000, 0xb19f6c31),
  LOAD_32_SWAP_16( CPU1, "1.u22", 0x000002, 0x080000, 0xc521bf24),
	// Lower positions not populated
  LOAD16_32( GFX1, "81ts_3l.u6", 0x0c00000, 0x200000, 0xd77cff9c),
  LOAD16_32( GFX1, "82ts_3h.u14", 0x0c00002, 0x200000, 0xf012b583),
  LOAD16_32( GFX1, "83ts_4l.u7", 0x1000000, 0x200000, 0x078cafc3),
  LOAD16_32( GFX1, "84ts_4h.u15", 0x1000002, 0x200000, 0x1f91446b),
  LOAD16_32( GFX1, "85ts_5l.u8", 0x1400000, 0x200000, 0x40fbd259),
  LOAD16_32( GFX1, "86ts_5h.u16", 0x1400002, 0x200000, 0x186c935f),
  LOAD16_32( GFX1, "87ts_6l.u1", 0x1800000, 0x200000, 0xc17dc48a),
  LOAD16_32( GFX1, "88ts_6h.u2", 0x1800002, 0x200000, 0xe4dba5da),
  LOAD16_32( GFX1, "89ts_7l.u19", 0x1c00000, 0x200000, 0xdab1b2c5),
  LOAD16_32( GFX1, "90ts_7h.u20", 0x1c00002, 0x200000, 0xaae696b3),
  LOAD16_32( GFX1, "91ts_8l.u28", 0x2000000, 0x200000, 0xe953ace1),
  LOAD16_32( GFX1, "92ts_8h.u29", 0x2000002, 0x200000, 0x9da3b976),
  LOAD16_32( GFX1, "93ts_9l.u41", 0x2400000, 0x200000, 0x233087fe),
  LOAD16_32( GFX1, "94ts_9h.u42", 0x2400002, 0x200000, 0x9da831c7),
  LOAD16_32( GFX1, "95ts_10l.u58", 0x2800000, 0x200000, 0x303a5240),
  LOAD16_32( GFX1, "96ts_10h.u59", 0x2800002, 0x200000, 0x2240ebf6),
  LOAD( SOUND1, "97ts_snd.u52", 0x000000, 0x400000, 0x9155eca6),
  FILL( SOUND1, 0x400000, 0x400000, 0 ),
	// might need byteswapping to reprogram actual chip
  LOAD( EEPROM, "tgm2.default.nv", 0x000, 0x100, 0x50e2348c),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_tgm2p[] = // clone of tgm2
{
  { "2b.u21", 0x080000, 0x38bc626c, REGION_CPU1, 0x000000, LOAD32_SWAP_16 },
  { "1b.u22", 0x080000, 0x7599fb19, REGION_CPU1, 0x000002, LOAD32_SWAP_16 },
	// Lower positions not populated
  { "tgm2p.default.nv", 0x100, 0xb2328b40, REGION_EEPROM, 0x000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_soldivid[] =
{
  LOAD_32_SWAP_16( CPU1, "2-prog_l.u18", 0x000002, 0x080000, 0xcf179b04),
  LOAD_32_SWAP_16( CPU1, "1-prog_h.u17", 0x000000, 0x080000, 0xf467d1c4),
	/* This Space Empty! */
  LOAD_32_SWAP_16( GFX1, "4l.u10", 0x2000000, 0x400000, 0x9eb9f269),
  LOAD_32_SWAP_16( GFX1, "4h.u31", 0x2000002, 0x400000, 0x7c76cfe7),
  LOAD_32_SWAP_16( GFX1, "5l.u9", 0x2800000, 0x400000, 0xc59c6858),
  LOAD_32_SWAP_16( GFX1, "5h.u30", 0x2800002, 0x400000, 0x73bc66d0),
  LOAD_32_SWAP_16( GFX1, "6l.u8", 0x3000000, 0x400000, 0xf01b816e),
  LOAD_32_SWAP_16( GFX1, "6h.u37", 0x3000002, 0x400000, 0xfdd57361),
  LOAD( SOUND1, "sound.bin", 0x000000, 0x400000, 0xe98f8d45),
  { NULL, 0, 0, 0, 0, 0 }
};

#define dsw_tgm2 NULL // just ignored in tgm2

static struct ROM_INFO rom_dragnblz[] =
{
  LOAD_32_SWAP_16( CPU1, "2prog_h.u21", 0x000000, 0x080000, 0xfc5eade8),
  LOAD_32_SWAP_16( CPU1, "1prog_l.u22", 0x000002, 0x080000, 0x95d6fd02),
  LOAD16_32( GFX1, "1l.u4", 0x0400000, 0x200000, 0xc2eb565c),
  LOAD16_32( GFX1, "1h.u12", 0x0400002, 0x200000, 0x23cb46b7),
  LOAD16_32( GFX1, "2l.u5", 0x0800000, 0x200000, 0xbc256aea),
  LOAD16_32( GFX1, "2h.u13", 0x0800002, 0x200000, 0xb75f59ec),
  LOAD16_32( GFX1, "3l.u6", 0x0c00000, 0x200000, 0x4284f008),
  LOAD16_32( GFX1, "3h.u14", 0x0c00002, 0x200000, 0xabe5cbbf),
  LOAD16_32( GFX1, "4l.u7", 0x1000000, 0x200000, 0xc9fcf2e5),
  LOAD16_32( GFX1, "4h.u15", 0x1000002, 0x200000, 0x0ab0a12a),
  LOAD16_32( GFX1, "5l.u8", 0x1400000, 0x200000, 0x68d03ccf),
  LOAD16_32( GFX1, "5h.u16", 0x1400002, 0x200000, 0x5450fbca),
  LOAD16_32( GFX1, "6l.u1", 0x1800000, 0x200000, 0x8b52c90b),
  LOAD16_32( GFX1, "6h.u2", 0x1800002, 0x200000, 0x7362f929),
  LOAD16_32( GFX1, "7l.u19", 0x1c00000, 0x200000, 0xb4f4d86e),
  LOAD16_32( GFX1, "7h.u20", 0x1c00002, 0x200000, 0x44b7b9cc),
  LOAD16_32( GFX1, "8l.u28", 0x2000000, 0x200000, 0xcd079f89),
  LOAD16_32( GFX1, "8h.u29", 0x2000002, 0x200000, 0x3edb508a),
  LOAD16_32( GFX1, "9l.u41", 0x2400000, 0x200000, 0x0b53cd78),
  LOAD16_32( GFX1, "9h.u42", 0x2400002, 0x200000, 0xbc61998a),
  LOAD16_32( GFX1, "10l.u58", 0x2800000, 0x200000, 0xa3f5c7f8),
  LOAD16_32( GFX1, "10h.u59", 0x2800002, 0x200000, 0x30e304c4),
  LOAD( SOUND1, "snd0.u52", 0x000000, 0x200000, 0x7fd1b225),
  FILL( SOUND1, 0x200000, 0x600000, 0 ),
  LOAD_SW16( EEPROM, "eeprom-dragnblz.u44", 0x0000, 0x0100, 0x46e85da9),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_daraku[] =
{
  LOAD_32_SWAP_16( CPU1, "4_prog_l.u18", 0x000002, 0x080000, 0x660b4609),
  LOAD_32_SWAP_16( CPU1, "3_prog_h.u17", 0x000000, 0x080000, 0x7a9cf601),
  LOAD_SW16( CPU1, "prog.u16", 0x100000, 0x100000, 0x3742e990),
  LOAD16_32( GFX1, "0l.u4", 0x0000000, 0x400000, 0x565d8427),
  LOAD16_32( GFX1, "0h.u13", 0x0000002, 0x400000, 0x9a602630),
  LOAD16_32( GFX1, "1l.u3", 0x0800000, 0x400000, 0xac5ce8e1),
  LOAD16_32( GFX1, "1h.u12", 0x0800002, 0x400000, 0xb0a59f7b),
  LOAD16_32( GFX1, "2l.u2", 0x1000000, 0x400000, 0x2daa03b2),
  LOAD16_32( GFX1, "2h.u20", 0x1000002, 0x400000, 0xe98e185a),
  LOAD16_32( GFX1, "3l.u1", 0x1800000, 0x400000, 0x1d372aa1),
  LOAD16_32( GFX1, "3h.u19", 0x1800002, 0x400000, 0x597f3f15),
  LOAD16_32( GFX1, "4l.u10", 0x2000000, 0x400000, 0xe3d58cd8),
  LOAD16_32( GFX1, "4h.u31", 0x2000002, 0x400000, 0xaebc9cd0),
  LOAD16_32( GFX1, "5l.u9", 0x2800000, 0x400000, 0xeab5a50b),
  LOAD16_32( GFX1, "5h.u30", 0x2800002, 0x400000, 0xf157474f),
  LOAD16_32( GFX1, "6l.u8", 0x3000000, 0x200000, 0x9f008d1b),
  LOAD16_32( GFX1, "6h.u37", 0x3000002, 0x200000, 0xacd2d0e3),
  LOAD( SOUND1, "sound.u32", 0x000000, 0x400000, 0xef2c781d),
  LOAD( EEPROM, "eeprom-daraku.bin", 0x0000, 0x0100, 0xa9715297),
  { NULL, 0, 0, 0, 0, 0 }
};

/*
Starting with Gunbarich and including Mahjong G-Taste, Psikyo started to "recycle" left over Dragon Blaze PCBs.
  Psikyo would replace some of the Dragon Blaze roms with the new game roms leaving many of the surface mounted
  roms intact.  The new games don't use or access the left over roms, but the PCB needs the roms filled to function.

  The hidden rom tests in Gunbarich and Mahjong G-Teste shows the games only uses the new game roms.
*/
static struct ROM_INFO rom_gnbarich[] =
{
  LOAD_32_SWAP_16( CPU1, "2-prog_l.u21", 0x000000, 0x080000, 0xc136cd9c),
  LOAD_32_SWAP_16( CPU1, "1-prog_h.u22", 0x000002, 0x080000, 0x6588fc96),
//  ROM_LOAD32_WORD( "1l.u4",  0x0400000, 0x200000, CRC(c2eb565c) SHA1(07e41b36cc03a87f28d091754fdb0d1a7316a532) ) /* From Dragon Blaze */
//  ROM_LOAD32_WORD( "1h.u12", 0x0400002, 0x200000, CRC(23cb46b7) SHA1(005b7cc40eea103688a64a72c219c7535970dbfb) ) /* From Dragon Blaze */
//  ROM_LOAD32_WORD( "2l.u5",  0x0800000, 0x200000, CRC(bc256aea) SHA1(1f1d678e8a63513a95f296b8a07d2ea485d1e53f) ) /* From Dragon Blaze */
//  ROM_LOAD32_WORD( "2h.u13", 0x0800002, 0x200000, CRC(b75f59ec) SHA1(a6cde94bc972e46e54c962fde49fc2174b312882) ) /* From Dragon Blaze */
//  ROM_LOAD32_WORD( "3l.u6",  0x0c00000, 0x200000, CRC(4284f008) SHA1(610b13304043411b3088fd4299b3cb0a4d8b0cc2) ) /* From Dragon Blaze */
//  ROM_LOAD32_WORD( "3h.u14", 0x0c00002, 0x200000, CRC(abe5cbbf) SHA1(c2fb1d8ea8772572c08b36496cf9fc5b91cf848b) ) /* From Dragon Blaze */
//  ROM_LOAD32_WORD( "4l.u7",  0x1000000, 0x200000, CRC(c9fcf2e5) SHA1(7cecdf3406da11289b54aaf58d12883ddfdc5e6b) ) /* From Dragon Blaze */
//  ROM_LOAD32_WORD( "4h.u15", 0x1000002, 0x200000, CRC(0ab0a12a) SHA1(1b29b6dc79e69edb56634517365d0ee8e6ea78ae) ) /* From Dragon Blaze */
//  ROM_LOAD32_WORD( "5l.u8",  0x1400000, 0x200000, CRC(68d03ccf) SHA1(d2bf6da5fa6e346b05872ed9616ffe51c3768f50) ) /* From Dragon Blaze */
//  ROM_LOAD32_WORD( "5h.u16", 0x1400002, 0x200000, CRC(5450fbca) SHA1(7a804263549cea951782a67855e69cb8cb417e98) ) /* From Dragon Blaze */
  LOAD16_32( GFX1, "6l.u1", 0x1800000, 0x200000, 0x0432e1a8),
  LOAD16_32( GFX1, "6h.u2", 0x1800002, 0x200000, 0xf90fa3ea),
  LOAD16_32( GFX1, "7l.u19", 0x1c00000, 0x200000, 0x36bf9a58),
  LOAD16_32( GFX1, "7h.u20", 0x1c00002, 0x200000, 0x4b3eafd8),
  LOAD16_32( GFX1, "8l.u28", 0x2000000, 0x200000, 0x026754da),
  LOAD16_32( GFX1, "8h.u29", 0x2000002, 0x200000, 0x8cd7aaa0),
  LOAD16_32( GFX1, "9l.u41", 0x2400000, 0x200000, 0x02c066fe),
//  ROM_LOAD32_WORD( "10l.u58",0x2800000, 0x200000, CRC(a3f5c7f8) SHA1(d17478ca3e7ef46270f350ffa35d43acb05b1185) ) /* From Dragon Blaze */
//  ROM_LOAD32_WORD( "10h.u59",0x2800002, 0x200000, CRC(30e304c4) SHA1(1d866276bfe7f7524306a880d225aaf11ac2e5dd) ) /* From Dragon Blaze */
  LOAD16_32( GFX1, "9h.u42", 0x2400002, 0x200000, 0x5433385a),
  LOAD( SOUND1, "snd0.u52", 0x000000, 0x200000, 0x7b10436b),
  LOAD( EEPROM, "eeprom-gnbarich.bin", 0x0000, 0x0100, 0x0f5bf42f),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct DSW_DATA dsw_data_daraku_0[] =
{
    { "Region",1,2 },
  { "Japan", 0x0 },
  { "World", 0x1 },
  { NULL, 0}
};

static struct DSW_INFO dsw_daraku[] =
{
  { 0x7, 0x1, dsw_data_daraku_0 },
  { 0, 0, NULL }
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

static struct DSW_DATA dsw_data_s1945iii_0[] =
{
    { "Region",2,3 },
  { "Japan", 0x0 },
  { "International Ver A.", 0x2 },
  { "International Ver B.", 0x1 },
  { NULL, 0}
};

static struct DSW_INFO dsw_s1945iii[] =
{
  { 0x7, 0x1, dsw_data_s1945iii_0 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_soldivid_0[] =
{
    { "Region",1,2 },
  { "Japan", 0x0 },
  { "World", 0x1 },
  { NULL, 0}
};

static struct DSW_INFO dsw_soldivid[] =
{
  { 0x7, 0x1, dsw_data_soldivid_0 },
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

static struct INPUT_INFO input_dragnblz[] =
{
    INCL_INP( gunbird2 ),
    INP1( UNKNOWN, 0x03, 0x40), // active HIGH here
    END_INPUT
};

static struct INPUT_INFO input_daraku[] =
{
    INCL_INP( gunbird2 ),
    INP0( UNKNOWN, 0x01, 0x2),
    INP0( UNKNOWN, 0x00, 0x2),
    INP0( P1_B4, 0x02, 0x04),
    INP0( P1_B3, 0x02, 0x08),
    INP0( P2_B4, 0x02, 0x40),
    INP0( P2_B3, 0x02, 0x80),
    END_INPUT
};

static struct INPUT_INFO input_s1945ii[] =
{
    INCL_INP( gunbird2 ),
    INP0( UNKNOWN, 0x00, 0x2), // no button 3
    INP0( UNKNOWN, 0x01, 0x2),
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
	{ MASTER_CLOCK/2 },
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
    addr &= 0xfffff;
    if (addr <= 0xfffff)
	return ROM[addr];
    return 0xff;
}

FASTCALL static u16 read_romw(u32 addr) {
    addr &= 0xfffff;
    if (addr <= 0xffffe)
	return ReadWord68k(&ROM[addr]);
    return 0xffff;
}

static u32 FASTCALL read_roml(u32 addr) {
    addr &= 0xfffff;
    if (addr <= 0xffffc) {
	return ReadLong68k(&ROM[addr]);
    }
    printf("*** read_roml %x\n",addr);
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

static u32 offset_vbl;

static u32 FASTCALL read_raml_hack(u32 offset) {
    offset &= 0xffffff;
    if (offset <= 0xfffff) {
	int ret = ReadLong68k(&RAM[offset]);
	if (offset == offset_vbl) {
	    // This is equivalent to a speed hack here for gunbird2 but without modifying the rom
	    if (!ret) {
		SH2_ReleaseTimeSlice(&M_SH2);
	    }
	}
        return ret;
    }
    return 0xffffffff;
}

static u32 FASTCALL read_raml(u32 offset) {
    offset &= 0xffffff;
    if (offset <= 0xfffff) {
	int ret = ReadLong68k(&RAM[offset]);
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
    printf("unknwon read video region byte %x\n",offset);

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

    printf("unknwon read video region word %x\n",offset);
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

    printf("unknwon read video region long %x\n",offset);
    return 0xffffffff;
}

static int colchange;

static void FASTCALL write_videob(u32 offset,u8 data) {
    offset &= 0xffffff;
    if (offset <= 0xffff)
        ram_spr[offset] = data; // bg + sprites
    else if (offset >= 0x40000 && offset <= 0x44fff) {
	ram_pal[offset & 0xffff] = data;
	if (display_cfg.bpp == 8)
	    colchange = 1;
	else
	    bank_status[(offset >> 6) & 0xff] = 0;
    }
    else if (offset >= 0x50000 && offset <= 0x501ff)
	ram_zoom[offset & 0x1ff] = data;
//    else if (offset >= 0x405ffdc && offset <= 0x405ffdf)
	// irq related
    else if (offset >= 0x5ffe0 && offset <= 0x5ffff)
	ram_video[offset & 0x1f] = data;
    else if (offset >= 0x5ffdc && offset <= 0x5ffdf) {
	// irq ack
    } else
	printf("write byte video unknown %x\n",offset);
}

static void FASTCALL write_videow(u32 offset,u16 data) {
    offset &= 0xffffff;
    if (offset <= 0xffff) {
	// printf("ram_sprw %x = %x\n",offset,data);
        WriteWord68k(&ram_spr[offset],data); // bg + sprites
    } else if (offset >= 0x40000 && offset <= 0x44fff) {
	WriteWord68k(&ram_pal[offset & 0xffff],data);
	if (display_cfg.bpp == 8)
	    colchange = 1;
	else
	    bank_status[(offset >> 6) & 0xff] = 0;
    } else if (offset >= 0x50000 && offset <= 0x501ff)
	WriteWord68k(&ram_zoom[offset & 0x1ff],data);
    // else if (offset >= 0x405ffdc && offset <= 0x405ffdf)
	// irq related
    else if (offset >= 0x5ffe0 && offset <= 0x5ffff)
	WriteWord68k(&ram_video[offset & 0x1f],data);
    else
	printf("write word video unknown %x\n",offset);
}

static void FASTCALL write_videol(u32 offset,u32 data) {
    offset &= 0xffffff;
    if (offset <= 0xffff) {
	// printf("ram_sprl %x = %x\n",offset,data);
        WriteLong68k(&ram_spr[offset],data); // bg + sprites
    } else if (offset >= 0x40000 && offset <= 0x44fff) {
	if (display_cfg.bpp == 8)
	    colchange = 1;
	else
	    bank_status[(offset >> 6) & 0xff] = 0;
	WriteLong68k(&ram_pal[offset & 0xffff],data);
    } else if (offset >= 0x50000 && offset <= 0x501ff)
	WriteLong68k(&ram_zoom[offset & 0x1ff],data);
    // else if (offset >= 0x405ffdc && offset <= 0x405ffdf)
	// irq related
    else if (offset >= 0x5ffe0 && offset <= 0x5ffff)
	WriteLong68k(&ram_video[offset & 0x1f],data);
    else
	printf("write long video unknown %x\n",offset);
}

static u8 FASTCALL read_inputs_soundb(u32 offset) {
    offset &= 0xffffff;
    if (offset == 4) // special case here, 2 lowest bits = region, 0x10 = eeprom bit
	return (input_buffer[7] & 3) | ((EEPROM_read_bit() & 0x01) << 4);
    else if (offset < 7) {
	// printf("read input %x = %x\n",offset,input_buffer[offset]);
	return input_buffer[offset];
    } else if (offset >= 0x100000 && offset <= 0x100003) {
	// printf("read_sound %x from %x\n",offset,M_SH2.PC - M_SH2.Base_PC);
	return ymf278b_0_r(offset & 3);
    } else {
	printf("read unknown port %x\n",offset);
	return 0xff;
    }
}

static u8 FASTCALL read_inputs_soundb_ps3v1(u32 offset) {
    offset &= 0xffffff;
    if (offset == 0x800004) { // special case here, 2 lowest bits = region, 0x10 = eeprom bit
	return (input_buffer[7] & 3) | ((EEPROM_read_bit() & 0x01) << 4);
    } else if (offset >= 0x800000 && offset < 0x800007) {
	// printf("read input %x = %x\n",offset,input_buffer[offset]);
	return input_buffer[offset & 7];
    } else if (offset <= 0x3) {
	// printf("read_sound %x\n",offset);
	return ymf278b_0_r(offset & 3);
    } else {
	printf("read unknown port %x\n",offset);
	return 0xff;
    }
}

static void FASTCALL write_sound(u32 offset,u8 data) {
    offset &= 0x7ffffff;
    // printf("write_sound %x,%x\n",offset,data);
    if (offset >= 0x3100000 && offset <= 0x3100007) {
	// printf("sound %x,%x from %x\n",offset,data,M_SH2.PC - M_SH2.Base_PC);
	ymf278b_0_w(offset,data);
    } else if (offset == 0x3000004) {
	// printf("eeprom write\n");
	EEPROM_write_bit((data & 0x20) ? 1 : 0);
	EEPROM_set_cs_line((data & 0x80) ? CLEAR_LINE : ASSERT_LINE);
	EEPROM_set_clock_line((data & 0x40) ? ASSERT_LINE : CLEAR_LINE);
    } else
	printf("write sound area unknown %x,%x\n",offset,data);
}

static void FASTCALL write_sound_ps3v1(u32 offset,u8 data) {
    offset &= 0x7ffffff;
    // printf("write_sound %x,%x\n",offset,data);
    if (offset >= 0x5000000 && offset <= 0x5000007)
	ymf278b_0_w(offset,data);
    else if (offset == 0x5800004) {
	// printf("eeprom write\n");
	EEPROM_write_bit((data & 0x20) ? 1 : 0);
	EEPROM_set_cs_line((data & 0x80) ? CLEAR_LINE : ASSERT_LINE);
	EEPROM_set_clock_line((data & 0x40) ? ASSERT_LINE : CLEAR_LINE);
    } else
	printf("write_sound_ps3v1 unknown %x,%x\n",offset,data);
}

#define BG_TYPE(n) (ram_video[6*4+n] & 0x7f )
#define BG_LINE(n) (ram_video[6*4+n] & 0x80 )
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
    if (load_region[REGION_EEPROM]) {
	default_eeprom = load_region[REGION_EEPROM];
	default_eeprom_size = get_region_size(REGION_EEPROM);
    }
    load_eeprom();
    SH2_Init(&M_SH2,0);
    if (get_region_size(REGION_CPU1) > 0x100000)
	bank = &ROM[0x100000];
    else
	bank = NULL;
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
    InitPaletteMap(ram_pal, 0x100, 0x100, 0x8000); // colors converted to 15bpp when in 8bpp, so map size = 0x8000 (15 bits).
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

    if (bank) {
	SH2_Add_ReadB(&M_SH2,5, 5, read_bankb);
	SH2_Add_ReadW(&M_SH2,5, 5, read_bankw);
	SH2_Add_ReadL(&M_SH2,5, 5, read_bankl);
    }

    SH2_Add_ReadB(&M_SH2,6, 6, read_ramb);
    SH2_Add_ReadW(&M_SH2,6, 6, read_ramw);
    if (is_current_game("gunbird2")) {
	offset_vbl = 0x40030;
	SH2_Add_ReadL(&M_SH2,6, 6, read_raml_hack);
    } else if (is_current_game("s1945iii") || is_current_game("tgm2") || is_current_game("tgm2p")) {
	offset_vbl = 0x60030;
	SH2_Add_ReadL(&M_SH2,6, 6, read_raml_hack);
    } else if (is_current_game("s1945ii")) {
	offset_vbl = 0x30;
	SH2_Add_ReadL(&M_SH2,6,6,read_raml_hack);
    } else if (is_current_game("soldivid")) {
	offset_vbl = 0x18;
	SH2_Add_ReadL(&M_SH2,6, 6, read_raml_hack);
    } else if (is_current_game("daraku")) {
	offset_vbl = 0x1c;
	SH2_Add_ReadL(&M_SH2,6, 6, read_raml_hack);
    } else if (is_current_game("gnbarich") || is_current_game("dragnblz")) {
	offset_vbl = 0x60028;
	SH2_Add_ReadL(&M_SH2,6, 6, read_raml_hack);
    } else
	SH2_Add_ReadL(&M_SH2,6, 6, read_raml);
    SH2_Add_WriteB(&M_SH2,6, 6, write_ramb);
    SH2_Add_WriteW(&M_SH2,6, 6, write_ramw);
    SH2_Add_WriteL(&M_SH2,6, 6, write_raml);
    if (is_current_game("soldivid") || is_current_game("s1945ii") || is_current_game("daraku")) { // ps3v1
	SH2_Add_ReadB(&M_SH2,3, 3, read_videob);
	SH2_Add_ReadW(&M_SH2,3, 3, read_videow);
	SH2_Add_ReadL(&M_SH2,3, 3, read_videol);
	SH2_Add_WriteB(&M_SH2,3, 3, write_videob);
	SH2_Add_WriteW(&M_SH2,3, 3, write_videow);
	SH2_Add_WriteL(&M_SH2,3, 3, write_videol);

	SH2_Add_ReadB(&M_SH2,5, 5, &read_inputs_soundb_ps3v1);
	SH2_Add_WriteB(&M_SH2,5, 5, &write_sound_ps3v1);
    } else {
	SH2_Add_ReadB(&M_SH2,4, 4, read_videob);
	SH2_Add_ReadW(&M_SH2,4, 4, read_videow);
	SH2_Add_ReadL(&M_SH2,4, 4, read_videol);
	SH2_Add_WriteB(&M_SH2,4, 4, write_videob);
	SH2_Add_WriteW(&M_SH2,4, 4, write_videow);
	SH2_Add_WriteL(&M_SH2,4, 4, write_videol);

	SH2_Add_ReadB(&M_SH2,3, 3, &read_inputs_soundb);
	SH2_Add_WriteB(&M_SH2,3, 3, &write_sound);
    }

    SH2_Map_Cache_Trough(&M_SH2);
    // alphatable initialized from color index...
    for (int i=0; i<0xc0; i++)
	alphatable[i] = 0xff;
    for (int i=0; i<0x40; i++) {
	int alpha = (0x3f-i)<<2;
	alphatable[i+0xc0] = alpha;
    }
    for (int i=0; i<4; i++)
	layer_id_data[i] = add_layer_info(layer_id_name[i]);
}

static void execute_gunbird2() {
    cpu_execute_cycles(CPU_SH2_0,MASTER_CLOCK/2/60);
    cpu_interrupt(CPU_SH2_0,4); // vbl
}

/* I would have loved to find a way to avoid the alpha test inside the 2 loops, but I can't pass the action there as a parameter because macro parameters are always expanded so I don't see any short
 * way to do it. It would require to make a super big block duplicating most of it based on the alpha value which would miss the point of making a macro here... so let's try that here and see if it's
 * that bad... */

#define draw_sprite_map( bpp )                                                \
    if (rotate == 0) {                                                        \
        for( y=sy; y<ey; y++ )                                                \
        {                                                                     \
            UINT8 *source = zoom_bitmap->line[y_index>>10];                   \
            UINT##bpp *dest = (u##bpp*)GameBitmap->line[y];                   \
            u8 *pline = pbitmap->line[y];                                     \
                                                                              \
            int x, x_index = x_index_base;                                    \
            for( x=sx; x<ex; x++ )                                            \
            {                                                                 \
                if (x >= 0 && y >= 0) {                                       \
                    int c = source[x_index>>10];                              \
                    if( c != 0 ) {                                            \
                        if ( alpha == 255 || bpp == 8) {                      \
                            dest[x] = ((UINT##bpp *)map)[c];                  \
                        } else if (alpha >= 0) {                              \
                            blend_##bpp(&dest[x], ((UINT##bpp *) map)[c]);    \
                        } else { /* alpha < 0 */                              \
                            if( alphatable[c] == 0xff )                       \
                                dest[x] = ((UINT##bpp *)map)[c];              \
                            else {                                            \
                                set_alpha(alphatable[c]);                     \
                                blend_##bpp(&dest[x], ((UINT##bpp *)map)[c]); \
                            }                                                 \
                        }                                                     \
                        pline[x] = pri;                                       \
                    }                                                         \
                }                                                             \
                x_index += dx;                                                \
            }                                                                 \
                                                                              \
            y_index += dy;                                                    \
        }                                                                     \
    } else if (rotate == 3) { /* 270 */                                       \
        for( y=sy; y<ey; y++ )                                                \
        {                                                                     \
            UINT8 *source = zoom_bitmap->line[y_index>>10];                   \
                                                                              \
            int x, x_index = x_index_base;                                    \
            for( x=sx; x<ex; x++ )                                            \
            {                                                                 \
                if (x >= 0 && y >= 0) {                                       \
                    UINT##bpp *dest = (u##bpp*)GameBitmap->line[disp_x_16-x]; \
                    u8 *pline = pbitmap->line[disp_x_16-x];                   \
                    int c = source[x_index>>10];                              \
                    if( c != 0 ) {                                            \
                        if (alpha == 255 || bpp == 8) {                                   \
                            dest[y] = ((UINT##bpp*)map)[c];                   \
                        } else if (alpha >= 0) {                              \
                            blend_##bpp(&dest[y], ((UINT##bpp *)map)[c]);     \
                        } else { /* alpha < 0 */                              \
                            if( alphatable[c] == 0xff )                       \
                                dest[y] = ((UINT##bpp *)map)[c];              \
                            else {                                            \
                                set_alpha(alphatable[c]);                     \
                                blend_##bpp(&dest[y], ((UINT##bpp *)map)[c]); \
                            }                                                 \
                        }                                                     \
                        pline[y] = pri;                                       \
                    }                                                         \
                }                                                             \
                x_index += dx;                                                \
            }                                                                 \
                                                                              \
            y_index += dy;                                                    \
        }                                                                     \
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
		    // The alphamap is super slow, + the sprites can't be rendered this way since they are drawn 1st... !!!
#if 1
		    if(gfx_solid[region][code_base]==1)                    // Some pixels; trans
			pdraw16x16_Trans_Mapped_back_flip_Rot(&gfx[region][code_base<<8],sx,sy,map,flipx | (flipy<<1),pri);
		    else
			pdraw16x16_Mapped_back_flip_Rot(&gfx[region][code_base<<8],sx,sy,map,flipx | (flipy<<1),pri);
#else
		    if(gfx_solid[region][code_base]==1)                    // Some pixels; trans
			pdraw16x16_Trans_Mapped_back_AlphaMap_flip_Rot(&gfx[region][code_base<<8],sx,sy,map,flipx | (flipy<<1),pri);
		    else
			pdraw16x16_Mapped_back_AlphaMap_flip_Rot(&gfx[region][code<<8],sx,sy,map,flipx | (flipy<<1),pri);
#endif
		}

	    }
	}
    }
    else /* Zoomed */
    {
	// The very 1st screen of gunbird2 is already asking for a zzom of 128 !!!
	// so the idea from mame to use an intermediate bitmap and zoom from this bitmap is really excellent here
	// another approach would be to use real opengl zooming functions, but I'd like to keep this for later...
	const VIDEO_INFO *vid = current_game->video;
	int rotate = (vid->flags ^ display_cfg.user_rotate) & 3;
	int disp_x_16 = vid->screen_x + 2*vid->border_size - 16;
	for(ytile=0; ytile<high; ytile++)
	{
	    for(xtile=0; xtile<wide; xtile++)
	    {
		const UINT8 *source = &gfx[region][((code + code_offset++) % max_sprites[region])<<8];
		for( ypixel=0; ypixel<16; ypixel++ )
		{
		    UINT8 *dest = zoom_bitmap->line[ypixel + ytile*16];

		    if (rotate == 3) {
			for( xpixel=0; xpixel<16; xpixel++ )
			{
			    dest[xpixel + xtile*16] = source[(15-xpixel)*16+ypixel];
			}
		    } else {
			for( xpixel=0; xpixel<16; xpixel++ )
			{
			    // untested so far, but should be ok
			    dest[xpixel + xtile*16] = *source++;
			}
		    }
		}
	    }
	}

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

	    int x_index_base;
	    int y_index;

	    int dx, dy;

	    if( flipx )	{ x_index_base = (sprite_screen_width-1)*zoomx; dx = -zoomx; }
	    else		{ x_index_base = 0; dx = zoomx; }

	    if( flipy )	{ y_index = (sprite_screen_height-1)*zoomy; dy = -zoomy; }
	    else		{ y_index = 0; dy = zoomy; }
	    if (sx < vid->border_size) {
		int osx = sx;
		sx = vid->border_size;
		x_index_base += dx * (sx-osx);
	    }
	    if (sy < vid->border_size) {
		int osy = sy;
		sy = vid->border_size;
		y_index += dy * (sy-osy);
	    }

	    if( ex>sx )
	    { /* skip if inner loop doesn't draw anything */
		int y;

		/* case 1: TRANSPARENCY_PEN */
		/* Note: adjusted to >>10 and draws from zoom_bitmap not gfx */
		if (alpha >= 0 && alpha < 255)
		    set_alpha(alpha);
		switch(display_cfg.bpp) {
		case 8:
		    draw_sprite_map( 8 );
		    break;
		case 15:
		case 16:
		    draw_sprite_map( 16 );
		    break;
		case 32:
		    draw_sprite_map( 32 );
		    break;
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

	if(gfx_solid[region][code]==1)                    // Some pixels; trans
	    pdraw16x16_Test_Trans_Mapped_AlphaMap_Rot(&gfx[region][code<<8],destx,desty,map,pri);
	else
	    pdraw16x16_Test_Mapped_AlphaMap_Rot(&gfx[region][code<<8],destx,desty,map,pri);
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
    clear_game_screen(0);
    clear_bitmap(pbitmap);
    if (colchange) {
	colchange = 0;
	// It's useful only in 8bpp, because any color change can change the balance of colors
	ClearPaletteMap();
    }
    // The layers have priority over the sprites, so the idea is to draw 1st the sprites, initializing the priority bitmap, then draw the layers still visible on top
    // it allows to use an 8bpp priority bitmap only
    // the only problem is when drawing and transparent sprite on top of that, since sprites are drawn 1st it just doesn't work !
    // For now an alt drawing function could do it at least for tgm2 which obviously doesn't use priorities, but which use transparent sprites !
    // Yeah except it's annoying to have such slow alpha blending functions so for now I just disable them, I should redo all these alpha blending functions differently...
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

static struct VIDEO_INFO video_horiz =
{
   draw_gunbird2,
   320,
   240,
   16,
   VIDEO_ROTATE_NORMAL | VIDEO_ROTATABLE,
   psikyosh_gfx
};

static struct VIDEO_INFO video_horiz224 =
{
   draw_gunbird2,
   320,
   224,
   16,
   VIDEO_ROTATE_NORMAL | VIDEO_ROTATABLE,
   psikyosh_gfx
};
static void clear_gunbird2() {
    destroy_bitmap(zoom_bitmap);
    zoom_bitmap = NULL;
}

GMEI( gunbird2,"Gunbird 2",PSIKYO,1998, GAME_SHOOT,
	.dsw = dsw_gunbird2);
GMEI( s1945iii,"Strikers 1945 III (World) / Strikers 1999 (Japan)",PSIKYO,1999, GAME_SHOOT,
	.dsw = dsw_s1945iii);
#define dsw_dragnblz dsw_s1945iii
GMEI( dragnblz,"Dragon Blaze",PSIKYO,2000, GAME_SHOOT|GAME_PARTIALLY_WORKING, // graphical glitches
	.input = input_dragnblz);
GMEI( tgm2,"Tetris the Absolute The Grand Master 2",ARIKA,2000, GAME_MISC,
	.video = &video_horiz);
CLNEI( tgm2p,tgm2,"Tetris the Absolute The Grand Master 2 Plus",ARIKA,2000, GAME_MISC,
	.video = &video_horiz);
GMEI( soldivid,"Sol Divide - The Sword Of Darkness",PSIKYO,1997, GAME_MISC,
	.video = &video_horiz224);
#define dsw_s1945ii dsw_soldivid
GMEI( s1945ii,"Strikers 1945 II",PSIKYO,1997, GAME_SHOOT,
	.input = input_s1945ii);
GMEI( daraku,"Daraku Tenshi - The Fallen Angels",PSIKYO,1998, GAME_BEAT | GAME_NOT_WORKING, // use special layers, type c & d for text layers, & line & row scroll
	.video = &video_horiz, .input = input_daraku);
#define dsw_gnbarich dsw_s1945iii
GMEI( gnbarich,"Gunbarich",PSIKYO,2001, GAME_BREAKOUT );
