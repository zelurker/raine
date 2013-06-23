#define DRV_DEF_LOAD load_neocd
#define DRV_DEF_EXEC execute_neocd
#define DRV_DEF_VIDEO &neocd_video
#define DRV_DEF_SOUND sound_neocd
#define DRV_DEF_CLEAR clear_neocd

#include "raine.h"
#include "games.h"
#include "68000/starhelp.h"
#include "cpumain.h"
#include "control.h"
#include "neocd/neocd.h"

extern struct SOUND_INFO sound_neocd[];

struct ROM_INFO rom_neogeo[] =
{
  { "sp-s2.sp1", 0x020000, 0x9036d879, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "sm1.sm1", 0x20000, 0x94416d67, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "000-lo.lo", 0x20000, 0x5a86cff2, REGION_ZOOMY, 0x00000, LOAD_NORMAL },
  { "sfix.sfix", 0x20000, 0xc2ea0cfd, REGION_FIXEDBIOS, 0x000000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

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
  { 0x1, 0xff, dsw_data_neogeo_0 },
  { 0, 0, NULL }
};

/* Inputs : the start is exactly like neocd, then next game/prev game replaces
 * SELECT, and then input_buffer[6] is used for coins ! */
static struct INPUT_INFO input_neogeo[] = // 2 players, 4 buttons
{
    INP0( P1_UP, 0, 1 ),
    INP0( P1_DOWN, 0, 2 ),
    INP0( P1_LEFT, 0, 4 ),
    INP0( P1_RIGHT, 0, 8 ),
  { KB_DEF_P1_B1, "Player1 A", 0x00, 0x10, BIT_ACTIVE_0 },
  { KB_DEF_P1_B2, "Player1 B", 0x00, 0x20, BIT_ACTIVE_0 },
  { KB_DEF_P1_B3, "Player1 C", 0x00, 0x40, BIT_ACTIVE_0 },
  { KB_DEF_P1_B4, "Player1 D", 0x00, 0x80, BIT_ACTIVE_0 },
  { KB_DEF_P1_B1B2,"Player1 A+B", 0, 0x30, BIT_ACTIVE_0 },
  { KB_DEF_P1_B3B4,"Player1 C+D", 0, 0xc0, BIT_ACTIVE_0 },
  { KB_DEF_P1_B2B3,"Player1 B+C", 0, 0x60, BIT_ACTIVE_0 },
  { KB_DEF_P1_B1B2B3,"Player1 A+B+C", 0, 0x70, BIT_ACTIVE_0 },
  { KB_DEF_P1_B2B3B4,"Player1 B+C+D", 0, 0xe0, BIT_ACTIVE_0 },

  { KB_DEF_P2_UP, MSG_P2_UP, 0x02, 0x01, BIT_ACTIVE_0 },
  { KB_DEF_P2_DOWN, MSG_P2_DOWN, 0x02, 0x02, BIT_ACTIVE_0 },
  { KB_DEF_P2_LEFT, MSG_P2_LEFT, 0x02, 0x04, BIT_ACTIVE_0 },
  { KB_DEF_P2_RIGHT, MSG_P2_RIGHT, 0x02, 0x08, BIT_ACTIVE_0 },
  { KB_DEF_P2_B1, "Player2 A", 0x02, 0x10, BIT_ACTIVE_0 },
  { KB_DEF_P2_B2, "Player2 B", 0x02, 0x20, BIT_ACTIVE_0 },
  { KB_DEF_P2_B3, "Player2 C", 0x02, 0x40, BIT_ACTIVE_0 },
  { KB_DEF_P2_B4, "Player2 D", 0x02, 0x80, BIT_ACTIVE_0 },
  { KB_DEF_P2_B1B2,"Player2 A+B", 2, 0x30, BIT_ACTIVE_0 },
  { KB_DEF_P2_B3B4,"Player2 C+D", 2, 0xc0, BIT_ACTIVE_0 },
  { KB_DEF_P2_B2B3,"Player2 B+C", 2, 0x60, BIT_ACTIVE_0 },
  { KB_DEF_P2_B1B2B3,"Player2 A+B+C", 2, 0x70, BIT_ACTIVE_0 },
  { KB_DEF_P2_B2B3B4,"Player2 B+C+D", 2, 0xe0, BIT_ACTIVE_0 },

  INP0( P1_START, 4, 1 ),
  { KB_DEF_NEXT_GAME, "Next Game", 0x04, 0x02, BIT_ACTIVE_0 },
  INP0( P2_START, 4, 4 ),
  { KB_DEF_PREV_GAME, "Prev Game", 0x04, 0x08, BIT_ACTIVE_0 },
  // Bit 4 (0x10) is 0 if the memory card is present !!!
  // neogeo doc :
  // bit 5 = mc 2 insertion status (0 = inserted)
  // bit 6 write protect 0 = write enable
  // bit 7 = neogeo mode : 0 = neogeo / 1 = mvs !!!

  INP0( COIN1, 6, 1 ),
  INP0( COIN2, 6, 2 ),
  INP0( TEST, 6, 4 ),
  /* having this ACTIVE_HIGH causes you to start with 2 credits using USA bios roms; if ACTIVE_HIGH + IN4 bit 6 ACTIVE_HIGH = AES 'mode' */
  INP0( UNKNOWN, 6, 8 ),
  INP0( UNKNOWN, 6, 0x10 ), // same as previous
  /* what is this? When ACTIVE_HIGH + IN4 bit 6 ACTIVE_LOW MVS-4 slot is detected */
  { KB_DEF_SPECIAL, MSG_UNKNOWN, 6, 0x20, BIT_ACTIVE_0 },
  { KB_DEF_SPECIAL, MSG_UNKNOWN, 6, 0xc0, BIT_ACTIVE_0 }, // calendar

  INP1( UNKNOWN, 8, 0x3f ),
  /* what is this? If ACTIVE_LOW, MVS-6 slot detected, when ACTIVE_HIGH MVS-1 slot (AES) detected */
  { KB_DEF_SPECIAL, MSG_UNKNOWN, 8, 0x40, BIT_ACTIVE_1 },
  INP0( SERVICE, 8, 0x80 ), // enter bios

  INP0( TEST, 11, 0x80 ), // well, it works in neogeo test mode at least !
  { 0, NULL,        0,        0,    0            },
};

GMEI( neogeo, "Neo-geo bios", SNK, 1990, GAME_MISC);

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

