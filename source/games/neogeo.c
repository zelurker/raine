#define DRV_DEF_LOAD load_neocd
#define DRV_DEF_EXEC execute_neocd
#define DRV_DEF_VIDEO &neocd_video
#define DRV_DEF_SOUND sound_neocd
#define DRV_DEF_CLEAR clear_neocd
#define DRV_DEF_INPUT input_neogeo
#define DRV_DEF_DSW dsw_neogeo

#include "raine.h"
#include "games.h"
#include "68000/starhelp.h"
#include "cpumain.h"
#include "control.h"
#include "neocd/neocd.h"
#include "neogeo.h"
#include "emumain.h" // reset_game_hardware

extern struct SOUND_INFO sound_neocd[];

static struct ROM_INFO rom_bios[] = // struct used to select bios
{
  { "sp-s2.sp1", 0x020000, 0x9036d879, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "sp-s.sp1", 0x020000, 0xc7f2fa45, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "sp-u2.sp1", 0x020000, 0xe72943de, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "sp-e.sp1", 0x020000, 0x2723a5b5, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "asia-s3.rom", 0x020000, 0x91b64be3, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "vs-bios.rom", 0x020000, 0xf0e8f27d, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "sp-j2.sp1", 0x020000, 0xacede59c, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "sp1.jipan.1024", 0x020000, 0x9fb0abe4, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "sp-45.sp1", 0x080000, 0x03cc9f6a, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "japan-j3.bin", 0x020000, 0xdff6d41f, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "sp-1v1_3db8c.bin", 0x020000, 0x162f0ebe, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "uni-bios_3_0.rom", 0x020000, 0xa97c89a9, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "uni-bios_2_3.rom", 0x020000, 0x27664eb5, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "uni-bios_2_2.rom", 0x020000, 0x2d50996a, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "uni-bios_2_1.rom", 0x020000, 0x8dabf76b, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "uni-bios_2_0.rom", 0x020000, 0x0c12c2ad, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "uni-bios_1_3.rom", 0x020000, 0xb24b44a0, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "uni-bios_1_2.rom", 0x020000, 0x4fa698e9, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "uni-bios_1_1.rom", 0x020000, 0x5dda0d84, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "uni-bios_1_0.rom", 0x020000, 0x0ce453a0, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "neodebug.rom", 0x020000, 0x698ebb7d, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "neo-epo.sp1", 0x020000, 0xd27a71f1, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "neo-po.sp1", 0x020000, 0x16d0c132, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

struct ROM_INFO rom_neogeo[] =
{
  { "sp-s2.sp1", 0x020000, 0x9036d879, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "sm1.sm1", 0x20000, 0x94416d67, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "000-lo.lo", 0x20000, 0x5a86cff2, REGION_ZOOMY, 0x00000, LOAD_NORMAL },
  { "sfix.sfix", 0x20000, 0xc2ea0cfd, REGION_FIXEDBIOS, 0x000000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

void set_neogeo_bios(int sel) {
    rom_neogeo[0] = rom_bios[sel];
    if (current_game && !is_neocd() && current_game->load_game == &load_neocd)
	reset_game_hardware();
}

static struct DSW_DATA dsw_data_neogeo_0[] =
{
// "SW:1"
  DSW_TEST_MODE( 0x0000, 0x0001 ),
// "SW:2"
  { "Coin Chutes?", 0x0002, 2 },
  { "1?", 0x0000, 0x00 },
  { "2?", 0x0002, 0x00 },
// "SW:3"
  { "Autofire (in some games)", 0x0004, 2 },
  { MSG_OFF, 0x0004, 0x00 },
  { MSG_ON, 0x0000, 0x00 },
// "SW:4,5"
  { "COMM Setting (Cabinet No.)", 0x0018, 4 },
  { "1", 0x0018, 0x00 },
  { "2", 0x0010, 0x00 },
  { "3", 0x0008, 0x00 },
  { "4", 0x0000, 0x00 },
// "SW:6"
  { "COMM Setting (Link Enable)", 0x0020, 2 },
  { MSG_OFF, 0x0020, 0x00 },
  { MSG_ON, 0x0000, 0x00 },
// "SW:7"
  { MSG_FREE_PLAY, 0x0040, 2 },
  { MSG_OFF, 0x0040, 0x00 },
  { MSG_ON, 0x0000, 0x00 },
// "SW:8"
  { "Freeze", 0x0080, 2 },
  { MSG_OFF, 0x0080, 0x00 },
  { MSG_ON, 0x0000, 0x00 },
  { NULL, 0, 0}
};

static struct DSW_INFO dsw_neogeo[] =
{
  { 0x0, 0xff, dsw_data_neogeo_0 },
  { 0, 0, NULL }
};

/* Inputs : the start is exactly like neocd, then next game/prev game replaces
 * SELECT, and then input_buffer[6] is used for coins ! */
static struct INPUT_INFO input_neogeo[] = // 2 players, 4 buttons
{
    INP0( P1_UP, 1, 1 ),
    INP0( P1_DOWN, 1, 2 ),
    INP0( P1_LEFT, 1, 4 ),
    INP0( P1_RIGHT, 1, 8 ),
  { KB_DEF_P1_B1, "Player1 A", 0x01, 0x10, BIT_ACTIVE_0 },
  { KB_DEF_P1_B2, "Player1 B", 0x01, 0x20, BIT_ACTIVE_0 },
  { KB_DEF_P1_B3, "Player1 C", 0x01, 0x40, BIT_ACTIVE_0 },
  { KB_DEF_P1_B4, "Player1 D", 0x01, 0x80, BIT_ACTIVE_0 },
  { KB_DEF_P1_B1B2,"Player1 A+B", 1, 0x30, BIT_ACTIVE_0 },
  { KB_DEF_P1_B3B4,"Player1 C+D", 1, 0xc0, BIT_ACTIVE_0 },
  { KB_DEF_P1_B2B3,"Player1 B+C", 1, 0x60, BIT_ACTIVE_0 },
  { KB_DEF_P1_B1B2B3,"Player1 A+B+C", 1, 0x70, BIT_ACTIVE_0 },
  { KB_DEF_P1_B2B3B4,"Player1 B+C+D", 1, 0xe0, BIT_ACTIVE_0 },

  { KB_DEF_P2_UP, MSG_P2_UP, 0x03, 0x01, BIT_ACTIVE_0 },
  { KB_DEF_P2_DOWN, MSG_P2_DOWN, 0x03, 0x02, BIT_ACTIVE_0 },
  { KB_DEF_P2_LEFT, MSG_P2_LEFT, 0x03, 0x04, BIT_ACTIVE_0 },
  { KB_DEF_P2_RIGHT, MSG_P2_RIGHT, 0x03, 0x08, BIT_ACTIVE_0 },
  { KB_DEF_P2_B1, "Player2 A", 0x03, 0x10, BIT_ACTIVE_0 },
  { KB_DEF_P2_B2, "Player2 B", 0x03, 0x20, BIT_ACTIVE_0 },
  { KB_DEF_P2_B3, "Player2 C", 0x03, 0x40, BIT_ACTIVE_0 },
  { KB_DEF_P2_B4, "Player2 D", 0x03, 0x80, BIT_ACTIVE_0 },
  { KB_DEF_P2_B1B2,"Player2 A+B", 3, 0x30, BIT_ACTIVE_0 },
  { KB_DEF_P2_B3B4,"Player2 C+D", 3, 0xc0, BIT_ACTIVE_0 },
  { KB_DEF_P2_B2B3,"Player2 B+C", 3, 0x60, BIT_ACTIVE_0 },
  { KB_DEF_P2_B1B2B3,"Player2 A+B+C", 3, 0x70, BIT_ACTIVE_0 },
  { KB_DEF_P2_B2B3B4,"Player2 B+C+D", 3, 0xe0, BIT_ACTIVE_0 },

  INP0( P1_START, 5, 1 ),
  { KB_DEF_NEXT_GAME, "Next Game", 0x05, 0x02, BIT_ACTIVE_0 },
  INP0( P2_START, 5, 4 ),
  { KB_DEF_PREV_GAME, "Prev Game", 0x05, 0x08, BIT_ACTIVE_0 },
  // Bit 4 (0x10) is 0 if the memory card is present !!!
  // neogeo doc :
  // bit 5 = mc 2 insertion status (0 = inserted)
  // bit 6 write protect 0 = write enable
  // bit 7 = neogeo mode : 0 = neogeo / 1 = mvs !!!

  INP0( COIN1, 6, 1 ),
  INP0( COIN2, 6, 2 ),
  INP0( SERVICE, 6, 4 ),
  /* having this ACTIVE_HIGH causes you to start with 2 credits using USA bios roms; if ACTIVE_HIGH + IN4 bit 6 ACTIVE_HIGH = AES 'mode' */
  INP0( UNKNOWN, 6, 8 ),
  INP0( UNKNOWN, 6, 0x10 ), // same as previous
  /* what is this? When ACTIVE_HIGH + IN4 bit 6 ACTIVE_LOW MVS-4 slot is detected */
  { KB_DEF_SPECIAL, MSG_UNKNOWN, 6, 0x20, BIT_ACTIVE_0 },
  { KB_DEF_SPECIAL, MSG_UNKNOWN, 6, 0xc0, BIT_ACTIVE_1 }, // calendar

  INP1( UNKNOWN, 8, 0x3f ),
  /* what is this? If ACTIVE_LOW, MVS-6 slot detected, when ACTIVE_HIGH MVS-1 slot (AES) detected */
  { KB_DEF_SPECIAL, MSG_UNKNOWN, 9, 0x40, BIT_ACTIVE_1 },
  INP0( TEST, 8, 0x80 ), // enter bios

  { 0, NULL,        0,        0,    0            },
};

GMEI( neogeo, "Neo-geo bios", SNK, 1990, GAME_MISC);

int check_bios_presence(int sel) {
    return load_rom_dir(dir_neogeo,rom_bios[sel].name,NULL, rom_bios[sel].size,
		rom_bios[sel].crc32,0);
}

static struct ROM_INFO rom_nam1975[] = /* MVS AND AES VERSION clone of neogeo */
{
  { "001-p1.p1", 0x080000, 0xcc9fc951, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "001-s1.s1", 0x20000, 0x7988ba51, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "001-m1.m1", 0x40000, 0xba874463, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "001-m1.m1", 0x40000, 0xba874463, REGION_ROM2, 0x10000, LOAD_NORMAL },
	// AES has different label, data is the same: 001-v1.v1
  { "001-v11.v11", 0x080000, 0xa7c3d5e5, REGION_SMP1, 0x000000, LOAD_NORMAL },
	// AES has different label, data is the same: 001-v2.v21
  { "001-v21.v21", 0x080000, 0x55e670b3, REGION_YMSND_DELTAT, 0x000000, LOAD_NORMAL },
  { "001-v22.v22", 0x080000, 0xab0d8368, REGION_YMSND_DELTAT, 0x080000, LOAD_NORMAL },
  { "001-v23.v23", 0x080000, 0xdf468e28, REGION_YMSND_DELTAT, 0x100000, LOAD_NORMAL },
  { "001-c1.c1", 0x80000, 0x32ea98e1, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "001-c2.c2", 0x80000, 0xcbc4064c, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "001-c3.c3", 0x80000, 0x0151054c, REGION_SPRITES, 0x100000, LOAD_8_16 },
  { "001-c4.c4", 0x80000, 0x0a32570d, REGION_SPRITES, 0x100001, LOAD_8_16 },
  { "001-c5.c5", 0x80000, 0x90b74cc2, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "001-c6.c6", 0x80000, 0xe62bed58, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( nam1975, neogeo, "NAM-1975 (NGM-001)(NGH-001)", SNK, 1990, GAME_SHOOT);

static struct ROM_INFO rom_bstars[] = /* MVS VERSION clone of neogeo */
{
  { "002-pg.p1", 0x080000, 0xc100b5f5, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "002-s1.s1", 0x20000, 0x1a7fd0c6, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "002-m1.m1", 0x40000, 0x4ecaa4ee, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "002-v11.v11", 0x080000, 0xb7b925bd, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "002-v12.v12", 0x080000, 0x329f26fc, REGION_SMP1, 0x080000, LOAD_NORMAL },
  { "002-v13.v13", 0x080000, 0x0c39f3c8, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "002-v14.v14", 0x080000, 0xc7e11c38, REGION_SMP1, 0x180000, LOAD_NORMAL },
  { "002-v21.v21", 0x080000, 0x04a733d1, REGION_YMSND_DELTAT, 0x000000, LOAD_NORMAL },
  { "002-c1.c1", 0x080000, 0xaaff2a45, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "002-c2.c2", 0x080000, 0x3ba0f7e4, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "002-c3.c3", 0x080000, 0x96f0fdfa, REGION_SPRITES, 0x100000, LOAD_8_16 },
  { "002-c4.c4", 0x080000, 0x5fd87f2f, REGION_SPRITES, 0x100001, LOAD_8_16 },
  { "002-c5.c5", 0x080000, 0x807ed83b, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "002-c6.c6", 0x080000, 0x5a3cad41, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI(bstars, neogeo, "Baseball Stars Professional (NGM-002)", SNK, 1990, GAME_SPORTS);

static struct ROM_INFO rom_bstarsh[] = /* AES VERSION clone of bstars */
{
  { "002-p1.p1", 0x080000, 0x3bc7790e, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( bstarsh, bstars, "Baseball Stars Professional (NGH-002)", SNK, 1990, GAME_SPORTS);

static struct ROM_INFO rom_tpgolf[] = /* MVS AND AES VERSION */
{
  { "003-p1.p1", 0x080000, 0xf75549ba, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "003-p2.p2", 0x080000, 0xb7809a8f, REGION_CPU1, 0x080000, LOAD_SWAP_16 },
  { "003-s1.s1", 0x20000, 0x7b3eb9b1, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "003-m1.m1", 0x20000, 0x4cc545e6, REGION_ROM2, 0x00000, LOAD_NORMAL },
	// AES has different label, data is the same (also found on MVS): 003-v1.v11
  { "003-v11.v11", 0x080000, 0xff97f1cb, REGION_SMP1, 0x000000, LOAD_NORMAL },
	// AES has different label, data is the same (also found on MVS): 003-v2.v21
  { "003-v21.v21", 0x080000, 0xd34960c6, REGION_YMSND_DELTAT, 0x000000, LOAD_NORMAL },
  { "003-v22.v22", 0x080000, 0x9a5f58d4, REGION_YMSND_DELTAT, 0x080000, LOAD_NORMAL },
  { "003-v23.v23", 0x080000, 0x30f53e54, REGION_YMSND_DELTAT, 0x100000, LOAD_NORMAL },
  { "003-v24.v24", 0x080000, 0x5ba0f501, REGION_YMSND_DELTAT, 0x180000, LOAD_NORMAL },
  { "003-c1.c1", 0x80000, 0x0315fbaf, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "003-c2.c2", 0x80000, 0xb4c15d59, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "003-c3.c3", 0x80000, 0x8ce3e8da, REGION_SPRITES, 0x100000, LOAD_8_16 },
  { "003-c4.c4", 0x80000, 0x29725969, REGION_SPRITES, 0x100001, LOAD_8_16 },
  { "003-c5.c5", 0x80000, 0x9a7146da, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "003-c6.c6", 0x80000, 0x1e63411a, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { "003-c7.c7", 0x80000, 0x2886710c, REGION_SPRITES, 0x300000, LOAD_8_16 },
  { "003-c8.c8", 0x80000, 0x422af22d, REGION_SPRITES, 0x300001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( tpgolf, neogeo, "Top Player's Golf (NGM-003)(NGH-003)", SNK, 1990, GAME_SPORTS);

static struct ROM_INFO rom_mahretsu[] = /* MVS AND AES VERSION */
{
  { "004-p1.p1", 0x080000, 0xfc6f53db, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "004-s1.s1", 0x20000, 0x2bd05a06, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "004-m1.m1", 0x20000, 0xc71fbb3b, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "004-v11.v11", 0x080000, 0xb2fb2153, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "004-v12.v12", 0x080000, 0x8503317b, REGION_SMP1, 0x080000, LOAD_NORMAL },
  { "004-v21.v21", 0x080000, 0x4999fb27, REGION_YMSND_DELTAT, 0x000000, LOAD_NORMAL },
  { "004-v22.v22", 0x080000, 0x776fa2a2, REGION_YMSND_DELTAT, 0x080000, LOAD_NORMAL },
  { "004-v23.v23", 0x080000, 0xb3e7eeea, REGION_YMSND_DELTAT, 0x100000, LOAD_NORMAL },
  { "004-c1.c1", 0x80000, 0xf1ae16bc, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "004-c2.c2", 0x80000, 0xbdc13520, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "004-c3.c3", 0x80000, 0x9c571a37, REGION_SPRITES, 0x100000, LOAD_8_16 },
  { "004-c4.c4", 0x80000, 0x7e81cb29, REGION_SPRITES, 0x100001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( mahretsu, neogeo, "Mahjong Kyo Retsuden (NGM-004)(NGH-004)", SNK, 1990, GAME_MISC);

static struct ROM_INFO rom_ridhero[] = /* MVS AND AES VERSION */
{
  { "006-p1.p1", 0x080000, 0xd4aaf597, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "006-s1.s1", 0x20000, 0xeb5189f0, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "006-m1.m1", 0x40000, 0x92e7b4fe, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "006-v11.v11", 0x080000, 0xcdf74a42, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "006-v12.v12", 0x080000, 0xe2fd2371, REGION_SMP1, 0x080000, LOAD_NORMAL },
  { "006-v21.v21", 0x080000, 0x94092bce, REGION_YMSND_DELTAT, 0x000000, LOAD_NORMAL },
  { "006-v22.v22", 0x080000, 0x4e2cd7c3, REGION_YMSND_DELTAT, 0x080000, LOAD_NORMAL },
  { "006-v23.v23", 0x080000, 0x069c71ed, REGION_YMSND_DELTAT, 0x100000, LOAD_NORMAL },
  { "006-v24.v24", 0x080000, 0x89fbb825, REGION_YMSND_DELTAT, 0x180000, LOAD_NORMAL },
  { "006-c1.c1", 0x080000, 0x4a5c7f78, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "006-c2.c2", 0x080000, 0xe0b70ece, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "006-c3.c3", 0x080000, 0x8acff765, REGION_SPRITES, 0x100000, LOAD_8_16 },
  { "006-c4.c4", 0x080000, 0x205e3208, REGION_SPRITES, 0x100001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ridheroh[] = // clone of ridhero
{
	/* Chip label p1h does not exist, renamed temporarly to pg1, marked BAD_DUMP. This needs to be verified. */
  { "006-pg1.p1", 0x080000, 0x52445646, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( ridhero, neogeo, "Riding Hero (NGM-006)(NGH-006)", SNK, 1990, GAME_RACE);
CLNEI( ridheroh, ridhero, "Riding Hero (set 2)", SNK, 1990, GAME_RACE);

static struct ROM_INFO rom_alpham2[] = /* MVS AND AES VERSION */
{
  { "007-p1.p1", 0x080000, 0x5b266f47, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "007-p2.p2", 0x020000, 0xeb9c1044, REGION_CPU1, 0x080000, LOAD_SWAP_16 },
  { "007-s1.s1", 0x20000, 0x85ec9acf, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "007-m1.m1", 0x20000, 0x28dfe2cd, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "007-v1.v1", 0x100000, 0xcd5db931, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "007-v2.v2", 0x100000, 0x63e9b574, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "007-c1.c1", 0x100000, 0x8fba8ff3, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "007-c2.c2", 0x100000, 0x4dad2945, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "007-c3.c3", 0x080000, 0x68c2994e, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "007-c4.c4", 0x080000, 0x7d588349, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_alpham2p[] = /* early prototype - all roms were hand labeled with CRCs, dumps verified against them clone of alpham2 */
{
  { "proto_007-p1.p1", 0x080000, 0xc763e52a, REGION_CPU1, 0x000001, LOAD_8_16 },
  { "proto_007-p2.p2", 0x080000, 0x7a0b435c, REGION_CPU1, 0x000000, LOAD_8_16 },
  { "proto_007-s1.s1", 0x20000, 0xefc9ae2e, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "proto_007-m1.m1", 0x20000, 0x5976b464, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "proto_007-m1.m1", 0x20000, 0x5976b464, REGION_ROM2, 0x10000, LOAD_NORMAL },
  { "proto_007-v11.v11", 0x080000, 0x18eaa9e1, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "proto_007-v12.v12", 0x080000, 0x2487d495, REGION_SMP1, 0x080000, LOAD_NORMAL },
  { "proto_007-v13.v13", 0x080000, 0x25e60f25, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "proto_007-v21.v21", 0x080000, 0xac44b75a, REGION_SMP1, 0x180000, LOAD_NORMAL },
/* Ignored : 	ROM_COPY( "ymsnd", 0x180000, 0x00000, 0x80000 ) */
  { "proto_007-c1.c1", 0x80000, 0x24841639, REGION_SPRITES, 0x000000, LOAD_8_32 },
  { "proto_007-c2.c2", 0x80000, 0x912763ab, REGION_SPRITES, 0x000002, LOAD_8_32 },
  { "proto_007-c3.c3", 0x80000, 0x0743bde2, REGION_SPRITES, 0x000001, LOAD_8_32 },
  { "proto_007-c4.c4", 0x80000, 0x61240212, REGION_SPRITES, 0x000003, LOAD_8_32 },
  { "proto_007-c5.c5", 0x80000, 0xcf9f4c53, REGION_SPRITES, 0x200000, LOAD_8_32 },
  { "proto_007-c6.c6", 0x80000, 0x3d903b19, REGION_SPRITES, 0x200002, LOAD_8_32 },
  { "proto_007-c7.c7", 0x80000, 0xe41e3875, REGION_SPRITES, 0x200001, LOAD_8_32 },
  { "proto_007-c8.c8", 0x80000, 0x4483e2cf, REGION_SPRITES, 0x200003, LOAD_8_32 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( alpham2, neogeo, "Alpha Mission II / ASO II - Last Guardian (NGM-007)(NGH-007)", SNK, 1991, GAME_SHOOT);
CLNEI( alpham2p, alpham2, "Alpha Mission II / ASO II - Last Guardian (prototype)", SNK, 1991, GAME_SHOOT);

static struct ROM_INFO rom_cyberlip[] = // clone of neogeo
{
  { "010-p1.p1", 0x080000, 0x69a6b42d, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "010-s1.s1", 0x20000, 0x79a35264, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "010-m1.m1", 0x20000, 0x8be3a078, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "010-v11.v11", 0x080000, 0x90224d22, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "010-v12.v12", 0x080000, 0xa0cf1834, REGION_SMP1, 0x080000, LOAD_NORMAL },
  { "010-v13.v13", 0x080000, 0xae38bc84, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "010-v14.v14", 0x080000, 0x70899bd2, REGION_SMP1, 0x180000, LOAD_NORMAL },
  { "010-v21.v21", 0x080000, 0x586f4cb2, REGION_YMSND_DELTAT, 0x000000, LOAD_NORMAL },
  { "010-c1.c1", 0x80000, 0x8bba5113, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "010-c2.c2", 0x80000, 0xcbf66432, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "010-c3.c3", 0x80000, 0xe4f86efc, REGION_SPRITES, 0x100000, LOAD_8_16 },
  { "010-c4.c4", 0x80000, 0xf7be4674, REGION_SPRITES, 0x100001, LOAD_8_16 },
  { "010-c5.c5", 0x80000, 0xe8076da0, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "010-c6.c6", 0x80000, 0xc495c567, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( cyberlip, neogeo, "Cyber-Lip (NGM-010)", SNK, 1990, GAME_PLATFORM);

static struct ROM_INFO rom_superspy[] = /* MVS AND AES VERSION */
{
  { "011-p1.p1", 0x080000, 0xc7f944b5, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "sp2.p2", 0x020000, 0x811a4faf, REGION_CPU1, 0x080000, LOAD_SWAP_16 },
  { "011-s1.s1", 0x20000, 0xec5fdb96, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "011-m1.m1", 0x40000, 0xca661f1b, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "011-v11.v11", 0x100000, 0x5c674d5c, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "011-v12.v12", 0x080000, 0x9f513d5a, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "011-v21.v21", 0x080000, 0x426cd040, REGION_YMSND_DELTAT, 0x000000, LOAD_NORMAL },
  { "011-c1.c1", 0x100000, 0xcae7be57, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "011-c2.c2", 0x100000, 0x9e29d986, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "011-c3.c3", 0x100000, 0x14832ff2, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "011-c4.c4", 0x100000, 0xb7f63162, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( superspy, neogeo, "The Super Spy (NGM-011)(NGH-011)", SNK, 1990, GAME_FIGHT);

static struct ROM_INFO rom_mutnat[] = /* MVS AND AES VERSION */
{
  { "014-p1.p1", 0x080000, 0x6f1699c8, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "014-s1.s1", 0x20000, 0x99419733, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "014-m1.m1", 0x20000, 0xb6683092, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "014-v1.v1", 0x100000, 0x25419296, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "014-v2.v2", 0x100000, 0x0de53d5e, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "014-c1.c1", 0x100000, 0x5e4381bf, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "014-c2.c2", 0x100000, 0x69ba4e18, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "014-c3.c3", 0x100000, 0x890327d5, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "014-c4.c4", 0x100000, 0xe4002651, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( mutnat, neogeo, "Mutation Nation (NGM-014)(NGH-014)", SNK, 1992, GAME_BEAT);

static struct ROM_INFO rom_kotm[] = /* MVS VERSION */
{
  { "016-p1.p1", 0x080000, 0x1b818731, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "016-p2.p2", 0x020000, 0x12afdc2b, REGION_CPU1, 0x080000, LOAD_SWAP_16 },
  { "016-s1.s1", 0x20000, 0x1a2eeeb3, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "016-m1.m1", 0x20000, 0x9da9ca10, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "016-v1.v1", 0x100000, 0x86c0a502, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "016-v2.v2", 0x100000, 0x5bc23ec5, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "016-c1.c1", 0x100000, 0x71471c25, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "016-c2.c2", 0x100000, 0x320db048, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "016-c3.c3", 0x100000, 0x98de7995, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "016-c4.c4", 0x100000, 0x070506e2, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_kotmh[] = /* AES VERSION clone of kotm */
{
  { "016-hp1.p1", 0x080000, 0xb774621e, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "016-p2.p2", 0x020000, 0x12afdc2b, REGION_CPU1, 0x080000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kotm, neogeo, "King of the Monsters (set 1)", SNK, 1991, GAME_BEAT);
CLNEI( kotmh, kotm, "King of the Monsters (set 2)", SNK, 1991, GAME_BEAT);

static struct ROM_INFO rom_sengoku[] = /* MVS AND AES VERSION */
{
  { "017-p1.p1", 0x080000, 0xf8a63983, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "017-p2.p2", 0x020000, 0x3024bbb3, REGION_CPU1, 0x080000, LOAD_SWAP_16 },
  { "017-s1.s1", 0x20000, 0xb246204d, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "017-m1.m1", 0x20000, 0x9b4f34c6, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "017-v1.v1", 0x100000, 0x23663295, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "017-v2.v2", 0x100000, 0xf61e6765, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "017-c1.c1", 0x100000, 0xb4eb82a1, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "017-c2.c2", 0x100000, 0xd55c550d, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "017-c3.c3", 0x100000, 0xed51ef65, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "017-c4.c4", 0x100000, 0xf4f3c9cb, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sengokuh[] = /* AES VERSION (US) clone of sengoku */
{
  { "017-hp1.p1", 0x080000, 0x33eccae0, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "017-p2.p2", 0x020000, 0x3024bbb3, REGION_CPU1, 0x080000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( sengoku, neogeo, "Sengoku / Sengoku Denshou (NGM-017)(NGH-017)", SNK, 1991, GAME_BEAT);
CLNEI( sengokuh, sengoku, "Sengoku / Sengoku Denshou (NGH-017)(US)", SNK, 1991, GAME_BEAT);

static struct ROM_INFO rom_burningf[] = /* MVS AND AES VERSION */
{
  { "018-p1.p1", 0x080000, 0x4092c8db, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "018-s1.s1", 0x20000, 0x6799ea0d, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "018-m1.m1", 0x20000, 0x0c939ee2, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "018-v1.v1", 0x100000, 0x508c9ffc, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "018-v2.v2", 0x100000, 0x854ef277, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "018-c1.c1", 0x100000, 0x25a25e9b, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "018-c2.c2", 0x100000, 0xd4378876, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "018-c3.c3", 0x100000, 0x862b60da, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "018-c4.c4", 0x100000, 0xe2e0aff7, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_burningfh[] = /* AES VERSION (US) clone of burningf */
{
  { "018-hp1.p1", 0x080000, 0xddffcbf4, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( burningf, neogeo, "Burning Fight (NGM-018)(NGH-018)", SNK, 1991, GAME_BEAT);
CLNEI( burningfh, burningf, "Burning Fight (NGH-018)(US)", SNK, 1991, GAME_BEAT);

static struct ROM_INFO rom_lbowling[] = /* MVS AND AES VERSION */
{
  { "019-p1.p1", 0x080000, 0xa2de8445, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "019-s1.s1", 0x20000, 0x5fcdc0ed, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "019-m1.m1", 0x20000, 0xd568c17d, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "019-v11.v11", 0x080000, 0x0fb74872, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "019-v12.v12", 0x080000, 0x029faa57, REGION_SMP1, 0x080000, LOAD_NORMAL },
  { "019-v21.v21", 0x080000, 0x2efd5ada, REGION_YMSND_DELTAT, 0x000000, LOAD_NORMAL },
  { "019-c1.c1", 0x080000, 0x4ccdef18, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "019-c2.c2", 0x080000, 0xd4dd0802, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( lbowling, neogeo, "League Bowling (NGM-019)(NGH-019)", SNK, 1990, GAME_SPORTS);

static struct ROM_INFO rom_gpilots[] = /* MVS AND AES VERSION clone of neogeo */
{
  { "020-p1.p1", 0x080000, 0xe6f2fe64, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "020-p2.p2", 0x020000, 0xedcb22ac, REGION_CPU1, 0x080000, LOAD_SWAP_16 },
  { "020-s1.s1", 0x20000, 0xa6d83d53, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "020-m1.m1", 0x20000, 0x48409377, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "020-v11.v11", 0x100000, 0x1b526c8b, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "020-v12.v12", 0x080000, 0x4a9e6f03, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "020-v21.v21", 0x080000, 0x7abf113d, REGION_YMSND_DELTAT, 0x000000, LOAD_NORMAL },
  { "020-c1.c1", 0x100000, 0xbd6fe78e, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "020-c2.c2", 0x100000, 0x5f4a925c, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "020-c3.c3", 0x100000, 0xd1e42fd0, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "020-c4.c4", 0x100000, 0xedde439b, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_gpilotsh[] = /* AES VERSION (US) clone of gpilots */
{
  { "020-hp1.p1", 0x080000, 0x7cdb01ce, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "020-p2.p2", 0x020000, 0xedcb22ac, REGION_CPU1, 0x080000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( gpilots, neogeo, "Ghost Pilots (NGM-020)(NGH-020)", SNK, 1991, GAME_SHOOT);
CLNEI( gpilotsh, gpilots, "Ghost Pilots (NGH-020)(US)", SNK, 1991, GAME_SHOOT);

static struct ROM_INFO rom_joyjoy[] = /* MVS AND AES VERSION clone of neogeo */
{
  { "021-p1.p1", 0x080000, 0x39c3478f, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "021-s1.s1", 0x20000, 0x6956d778, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "021-m1.m1", 0x40000, 0x5a4be5e8, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "021-v11.v11", 0x080000, 0x66c1e5c4, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "021-v21.v21", 0x080000, 0x8ed20a86, REGION_YMSND_DELTAT, 0x000000, LOAD_NORMAL },
  { "021-c1.c1", 0x080000, 0x509250ec, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "021-c2.c2", 0x080000, 0x09ed5258, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( joyjoy, neogeo, "Puzzled / Joy Joy Kid (NGM-021)(NGH-021)", SNK, 1990, GAME_PUZZLE);

static struct ROM_INFO rom_lresort[] = // clone of neogeo
{
  { "024-p1.p1", 0x080000, 0x89c4ab97, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "024-s1.s1", 0x20000, 0x5cef5cc6, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "024-m1.m1", 0x20000, 0xcec19742, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "024-v1.v1", 0x100000, 0xefdfa063, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "024-v2.v2", 0x100000, 0x3c7997c0, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "024-c1.c1", 0x100000, 0x3617c2dc, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "024-c2.c2", 0x100000, 0x3f0a7fd8, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "024-c3.c3", 0x080000, 0xe9f745f8, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "024-c4.c4", 0x080000, 0x7382fefb, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( lresort, neogeo, "Last Resort", SNK, 1992, GAME_SHOOT);

static struct ROM_INFO rom_eightman[] = /* MVS AND AES VERSION clone of neogeo */
{
  { "025-p1.p1", 0x080000, 0x43344cb0, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "025-s1.s1", 0x20000, 0xa402202b, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "025-m1.m1", 0x20000, 0x9927034c, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "025-v1.v1", 0x100000, 0x4558558a, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "025-v2.v2", 0x100000, 0xc5e052e9, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "025-c1.c1", 0x100000, 0x555e16a4, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "025-c2.c2", 0x100000, 0xe1ee51c3, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "025-c3.c3", 0x080000, 0x0923d5b0, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "025-c4.c4", 0x080000, 0xe3eca67b, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( eightman, neogeo, "Eight Man (NGM-025)(NGH-025)", SNK, 1991, GAME_BEAT);

static struct ROM_INFO rom_legendos[] = // clone of neogeo
{
  { "029-p1.p1", 0x080000, 0x9d563f19, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "029-s1.s1", 0x20000, 0xbcd502f0, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "029-m1.m1", 0x20000, 0x6f2843f0, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "029-v1.v1", 0x100000, 0x85065452, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "029-c1.c1", 0x100000, 0x2f5ab875, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "029-c2.c2", 0x100000, 0x318b2711, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "029-c3.c3", 0x100000, 0x6bc52cb2, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "029-c4.c4", 0x100000, 0x37ef298c, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( legendos, neogeo, "Legend of Success Joe / Ashita no Joe Densetsu", SNK, 1991, GAME_BEAT);
