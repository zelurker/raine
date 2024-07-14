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

// Bios names are in neocd_options.c in dialogs, not really convenient
// but this is not updated often...
static struct ROM_INFO rom_bios[] = // struct used to select bios
{
  LOAD_SW16( MAINBIOS, "sp-s2.sp1", 0x00000, 0x020000, 0x9036d879),
  LOAD_SW16( MAINBIOS, "sp-s.sp1", 0x00000, 0x020000, 0xc7f2fa45),
  LOAD_SW16( MAINBIOS, "sp-u2.sp1", 0x00000, 0x020000, 0xe72943de),
  LOAD_SW16( MAINBIOS, "sp-e.sp1", 0x00000, 0x020000, 0x2723a5b5),
  LOAD_SW16( MAINBIOS, "asia-s3.rom", 0x00000, 0x020000, 0x91b64be3),
  LOAD_SW16( MAINBIOS, "vs-bios.rom", 0x00000, 0x020000, 0xf0e8f27d),
  LOAD_SW16( MAINBIOS, "sp-j2.sp1", 0x00000, 0x020000, 0xacede59c),
  LOAD_SW16( MAINBIOS, "sp1.jipan.1024", 0x00000, 0x020000, 0x9fb0abe4),
  LOAD_SW16( MAINBIOS, "sp-45.sp1", 0x00000, 0x080000, 0x03cc9f6a),
  LOAD_SW16( MAINBIOS, "japan-j3.bin", 0x00000, 0x020000, 0xdff6d41f),
  LOAD_SW16( MAINBIOS, "sp-1v1_3db8c.bin", 0x00000, 0x020000, 0x162f0ebe),
  LOAD_SW16( MAINBIOS, "uni-bios_3_1.rom", 0x00000, 0x020000, 0x0c58093f),
  LOAD_SW16( MAINBIOS, "uni-bios_3_0.rom", 0x00000, 0x020000, 0xa97c89a9),
  LOAD_SW16( MAINBIOS, "uni-bios_2_3.rom", 0x00000, 0x020000, 0x27664eb5),
  LOAD_SW16( MAINBIOS, "uni-bios_2_2.rom", 0x00000, 0x020000, 0x2d50996a),
  LOAD_SW16( MAINBIOS, "uni-bios_2_1.rom", 0x00000, 0x020000, 0x8dabf76b),
  LOAD_SW16( MAINBIOS, "uni-bios_2_0.rom", 0x00000, 0x020000, 0x0c12c2ad),
  LOAD_SW16( MAINBIOS, "uni-bios_1_3.rom", 0x00000, 0x020000, 0xb24b44a0),
  LOAD_SW16( MAINBIOS, "uni-bios_1_2.rom", 0x00000, 0x020000, 0x4fa698e9),
  LOAD_SW16( MAINBIOS, "uni-bios_1_1.rom", 0x00000, 0x020000, 0x5dda0d84),
  LOAD_SW16( MAINBIOS, "uni-bios_1_0.rom", 0x00000, 0x020000, 0x0ce453a0),
  LOAD_SW16( MAINBIOS, "neodebug.rom", 0x00000, 0x020000, 0x698ebb7d),
  LOAD_SW16( MAINBIOS, "neo-epo.sp1", 0x00000, 0x020000, 0xd27a71f1),
  LOAD_SW16( MAINBIOS, "neo-po.sp1", 0x00000, 0x020000, 0x16d0c132),
  LOAD_SW16( MAINBIOS, "236-bios.sp1", 0x00000, 0x020000, 0x853e6b96),

  LOAD_SW16( MAINBIOS, "uni-bios_3_2.rom", 0x00000, 0x020000, 0xa4e8b9b3),
  LOAD_SW16( MAINBIOS, "uni-bios_3_3.rom", 0x00000, 0x020000, 0x24858466),
  LOAD_SW16( MAINBIOS, "uni-bios_4_0.rom", 0x00000, 0x020000, 0xa7aab458),
  { NULL, 0, 0, 0, 0, 0 }
};

struct ROM_INFO rom_neogeo[] =
{
  LOAD_SW16( MAINBIOS, "sp-s2.sp1", 0x00000, 0x020000, 0x9036d879),
  LOAD( ROM2, "sm1.sm1", 0x00000, 0x20000, 0x94416d67),
  LOAD( ZOOMY, "000-lo.lo", 0x00000, 0x20000, 0x5a86cff2),
  LOAD( FIXEDBIOS, "sfix.sfix", 0x000000, 0x20000, 0xc2ea0cfd),
  { NULL, 0, 0, 0, 0, 0 }
};

void set_neogeo_bios(int sel) {
    if (!strcmp(rom_neogeo[0].name,rom_bios[sel].name))
	return;
    rom_neogeo[0] = rom_bios[sel];
    if (current_game && !is_neocd() && current_game->load_game == &load_neocd){
	extern int loaded_roms; // from loadroms.c
	loaded_roms--;
	load_rom(rom_bios[sel].name,load_region[REGION_MAINBIOS],get_region_size(REGION_MAINBIOS));
    }
}

static struct DSW_DATA dsw_data_neogeo_0[] =
{
// _("SW:1")
  DSW_TEST_MODE( 0x0000, 0x0001 ),
// _("SW:2")
  { _("Coin Chutes?"), 0x0002, 2 },
  { "1?", 0x0000, 0x00 },
  { "2?", 0x0002, 0x00 },
// _("SW:3")
  { _("Autofire (in some games)"), 0x0004, 2 },
  { MSG_OFF, 0x0004, 0x00 },
  { MSG_ON, 0x0000, 0x00 },
// _("SW:4,5")
  { _("COMM Setting (Cabinet No.)"), 0x0018, 4 },
  { "1", 0x0018, 0x00 },
  { "2", 0x0010, 0x00 },
  { "3", 0x0008, 0x00 },
  { "4", 0x0000, 0x00 },
// _("SW:6")
  { _("COMM Setting (Link Enable)"), 0x0020, 2 },
  { MSG_OFF, 0x0020, 0x00 },
  { MSG_ON, 0x0000, 0x00 },
// _("SW:7")
  { MSG_FREE_PLAY, 0x0040, 2 },
  { MSG_OFF, 0x0040, 0x00 },
  { MSG_ON, 0x0000, 0x00 },
// _("SW:8")
  { _("Freeze"), 0x0080, 2 },
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
#ifndef RAINE_DOS
  { KB_DEF_P1_B1B2,"Player1 A+B", 1, 0x30, BIT_ACTIVE_0 },
  { KB_DEF_P1_B3B4,"Player1 C+D", 1, 0xc0, BIT_ACTIVE_0 },
  { KB_DEF_P1_B2B3,"Player1 B+C", 1, 0x60, BIT_ACTIVE_0 },
  { KB_DEF_P1_B1B2B3,"Player1 A+B+C", 1, 0x70, BIT_ACTIVE_0 },
  { KB_DEF_P1_B2B3B4,"Player1 B+C+D", 1, 0xe0, BIT_ACTIVE_0 },
#endif

  { KB_DEF_P2_UP, MSG_P2_UP, 0x03, 0x01, BIT_ACTIVE_0 },
  { KB_DEF_P2_DOWN, MSG_P2_DOWN, 0x03, 0x02, BIT_ACTIVE_0 },
  { KB_DEF_P2_LEFT, MSG_P2_LEFT, 0x03, 0x04, BIT_ACTIVE_0 },
  { KB_DEF_P2_RIGHT, MSG_P2_RIGHT, 0x03, 0x08, BIT_ACTIVE_0 },
  { KB_DEF_P2_B1, "Player2 A", 0x03, 0x10, BIT_ACTIVE_0 },
  { KB_DEF_P2_B2, "Player2 B", 0x03, 0x20, BIT_ACTIVE_0 },
  { KB_DEF_P2_B3, "Player2 C", 0x03, 0x40, BIT_ACTIVE_0 },
  { KB_DEF_P2_B4, "Player2 D", 0x03, 0x80, BIT_ACTIVE_0 },
#ifndef RAINE_DOS
  { KB_DEF_P2_B1B2,"Player2 A+B", 3, 0x30, BIT_ACTIVE_0 },
  { KB_DEF_P2_B3B4,"Player2 C+D", 3, 0xc0, BIT_ACTIVE_0 },
  { KB_DEF_P2_B2B3,"Player2 B+C", 3, 0x60, BIT_ACTIVE_0 },
  { KB_DEF_P2_B1B2B3,"Player2 A+B+C", 3, 0x70, BIT_ACTIVE_0 },
  { KB_DEF_P2_B2B3B4,"Player2 B+C+D", 3, 0xe0, BIT_ACTIVE_0 },
#endif

  INP0( P1_START, 5, 1 ),
  { KB_DEF_NEXT_GAME, "Select 1", 0x05, 0x02, BIT_ACTIVE_0 },
  INP0( P2_START, 5, 4 ),
  { KB_DEF_PREV_GAME, "Select 2", 0x05, 0x08, BIT_ACTIVE_0 },
  INP1( UNKNOWN, 5, 0x70), // memcard status
  INP0( UNKNOWN, 5, 0x80), // mvs/aes ?
  // Bit 4 (0x10) is 0 if the memory card is present !!!
  // neogeo doc :
  // bit 5 = mc 2 insertion status (0 = inserted)
  // bit 6 write protect 0 = write enable
  // bit 7 = neogeo mode : 0 = neogeo / 1 = mvs !!!

  INP0( COIN1, 6, 1 ),
  INP0( COIN2, 6, 2 ),
  INP0( SERVICE, 6, 4 ),
  /* having this ACTIVE_HIGH causes you to start with 2 credits using USA bios roms; if ACTIVE_HIGH + IN4 bit 6 ACTIVE_HIGH = AES 'mode' */
  INP0( COIN3, 6, 8 ),
  INP0( COIN4, 6, 0x10 ), // same as previous
  /* what is this? When ACTIVE_HIGH + IN4 bit 6 ACTIVE_LOW MVS-4 slot is detected */
  { KB_DEF_SPECIAL, MSG_UNKNOWN, 6, 0x20, BIT_ACTIVE_0 },
  { KB_DEF_SPECIAL, MSG_UNKNOWN, 6, 0xc0, BIT_ACTIVE_1 }, // calendar

  INP1( UNKNOWN, 8, 0x3f ),
  /* what is this? If ACTIVE_LOW, MVS-6 slot detected, when ACTIVE_HIGH MVS-1 slot (AES) detected */
  { KB_DEF_SPECIAL, MSG_UNKNOWN, 8, 0x40, BIT_ACTIVE_1 },
  INP0( TEST, 8, 0x80 ), // enter bios

  END_INPUT
};

static struct INPUT_INFO input_irrmaze[] = // trackball
{
    INCL_INP( neogeo ),
    /* These inputs are special : irrmaze expects a trackball to be mapped in place of the standard inputs
     * but its specific bios is lost. So we swap the contents of input_buffer[1] based on the controller variable,
     * which is changed many times by frame, see neocd.c for details about that. */

    INP0( UNUSED, 3, 0xf ), // trackball p2 / inputs
    INP0( P1_B1, 3, 0x10),
    INP0( P1_B2, 3, 0x20),
    INP0( P2_B1, 3, 0x40),
    INP0( P2_B2, 3, 0x80),

  END_INPUT
};

static struct INPUT_INFO input_popbounc[] = // trackball
{
    INCL_INP( irrmaze ),

    INP0( UNUSED, 1, 0xff ), // trackball p1 / inputs

    INP0( P1_UP, 2, 1 ),
    INP0( P1_DOWN, 2, 2 ),
    INP0( P1_LEFT, 2, 4 ),
    INP0( P1_RIGHT, 2, 8 ),
    INP0( P1_B1, 2, 0x90 ), // Actually bit7 is when control = paddle
    // and bit4 is normal button1 when control = joystick
    INP0( P1_B2, 2, 0x20 ), // I don't think b2 and b3 are used anyway... !
    INP0( P1_B3, 2, 0x40 ),
    INP0( UNUSED, 3, 0xff ), // trackball p2 / inputs

    INP0( P2_UP, 4, 1 ),
    INP0( P2_DOWN, 4, 2 ),
    INP0( P2_LEFT, 4, 4 ),
    INP0( P2_RIGHT, 4, 8 ),
    INP0( P2_B1, 4, 0x90 ),
    INP0( P2_B2, 4, 0x20 ),
    INP0( P2_B3, 4, 0x40 ),

    END_INPUT
};

GMEI( neogeo, "Neo-Geo bios", SNK, 1990, GAME_MISC);

int check_bios_presence(int sel) {
    return load_rom_dir(dir_neogeo,rom_bios[sel].name,NULL, rom_bios[sel].size,
		rom_bios[sel].crc32,0);
}

static struct ROM_INFO rom_nam1975[] = /* MVS AND AES VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "001-p1.p1", 0x000000, 0x080000, 0xcc9fc951),
  LOAD( FIXED, "001-s1.s1", 0x000000, 0x20000, 0x7988ba51),
  LOAD( ROM2, "001-m1.m1", 0x00000, 0x40000, 0xba874463),
	// AES has different label, data is the same: 001-v1.v1
  LOAD( SMP1, "001-v11.v11", 0x000000, 0x080000, 0xa7c3d5e5),
	// AES has different label, data is the same: 001-v2.v21
  LOAD( YMSND_DELTAT, "001-v21.v21", 0x000000, 0x080000, 0x55e670b3),
  LOAD( YMSND_DELTAT, "001-v22.v22", 0x080000, 0x080000, 0xab0d8368),
  LOAD( YMSND_DELTAT, "001-v23.v23", 0x100000, 0x080000, 0xdf468e28),
  LOAD_16_8( SPRITES, "001-c1.c1", 0x000000, 0x80000, 0x32ea98e1),
  LOAD_16_8( SPRITES, "001-c2.c2", 0x000001, 0x80000, 0xcbc4064c),
  LOAD_16_8( SPRITES, "001-c3.c3", 0x100000, 0x80000, 0x0151054c),
  LOAD_16_8( SPRITES, "001-c4.c4", 0x100001, 0x80000, 0x0a32570d),
  LOAD_16_8( SPRITES, "001-c5.c5", 0x200000, 0x80000, 0x90b74cc2),
  LOAD_16_8( SPRITES, "001-c6.c6", 0x200001, 0x80000, 0xe62bed58),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( nam1975, neogeo, "NAM-1975 (NGM-001)(NGH-001)", SNK, 1990, GAME_SHOOT);

static struct ROM_INFO rom_bstars[] = /* MVS VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "002-pg.p1", 0x000000, 0x080000, 0xc100b5f5),
  LOAD( FIXED, "002-s1.s1", 0x000000, 0x20000, 0x1a7fd0c6),
  LOAD( ROM2, "002-m1.m1", 0x00000, 0x40000, 0x4ecaa4ee),
  LOAD( SMP1, "002-v11.v11", 0x000000, 0x080000, 0xb7b925bd),
  LOAD( SMP1, "002-v12.v12", 0x080000, 0x080000, 0x329f26fc),
  LOAD( SMP1, "002-v13.v13", 0x100000, 0x080000, 0x0c39f3c8),
  LOAD( SMP1, "002-v14.v14", 0x180000, 0x080000, 0xc7e11c38),
  LOAD( YMSND_DELTAT, "002-v21.v21", 0x000000, 0x080000, 0x04a733d1),
  LOAD_16_8( SPRITES, "002-c1.c1", 0x000000, 0x080000, 0xaaff2a45),
  LOAD_16_8( SPRITES, "002-c2.c2", 0x000001, 0x080000, 0x3ba0f7e4),
  LOAD_16_8( SPRITES, "002-c3.c3", 0x100000, 0x080000, 0x96f0fdfa),
  LOAD_16_8( SPRITES, "002-c4.c4", 0x100001, 0x080000, 0x5fd87f2f),
  LOAD_16_8( SPRITES, "002-c5.c5", 0x200000, 0x080000, 0x807ed83b),
  LOAD_16_8( SPRITES, "002-c6.c6", 0x200001, 0x080000, 0x5a3cad41),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI(bstars, neogeo, "Baseball Stars Professional (NGM-002)", SNK, 1990, GAME_SPORTS);

static struct ROM_INFO rom_bstarsh[] = /* AES VERSION clone of bstars */
{
  LOAD_SW16( CPU1, "002-p1.p1", 0x000000, 0x080000, 0x3bc7790e),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( bstarsh, bstars, "Baseball Stars Professional (NGH-002)", SNK, 1990, GAME_SPORTS);

static struct ROM_INFO rom_tpgolf[] = /* MVS AND AES VERSION */
{
  LOAD_SW16( CPU1, "003-p1.p1", 0x000000, 0x080000, 0xf75549ba),
  LOAD_SW16( CPU1, "003-p2.p2", 0x080000, 0x080000, 0xb7809a8f),
  LOAD( FIXED, "003-s1.s1", 0x000000, 0x20000, 0x7b3eb9b1),
  LOAD( ROM2, "003-m1.m1", 0x00000, 0x20000, 0x4cc545e6),
	// AES has different label, data is the same (also found on MVS): 003-v1.v11
  LOAD( SMP1, "003-v11.v11", 0x000000, 0x080000, 0xff97f1cb),
	// AES has different label, data is the same (also found on MVS): 003-v2.v21
  LOAD( YMSND_DELTAT, "003-v21.v21", 0x000000, 0x080000, 0xd34960c6),
  LOAD( YMSND_DELTAT, "003-v22.v22", 0x080000, 0x080000, 0x9a5f58d4),
  LOAD( YMSND_DELTAT, "003-v23.v23", 0x100000, 0x080000, 0x30f53e54),
  LOAD( YMSND_DELTAT, "003-v24.v24", 0x180000, 0x080000, 0x5ba0f501),
  LOAD_16_8( SPRITES, "003-c1.c1", 0x000000, 0x80000, 0x0315fbaf),
  LOAD_16_8( SPRITES, "003-c2.c2", 0x000001, 0x80000, 0xb4c15d59),
  LOAD_16_8( SPRITES, "003-c3.c3", 0x100000, 0x80000, 0x8ce3e8da),
  LOAD_16_8( SPRITES, "003-c4.c4", 0x100001, 0x80000, 0x29725969),
  LOAD_16_8( SPRITES, "003-c5.c5", 0x200000, 0x80000, 0x9a7146da),
  LOAD_16_8( SPRITES, "003-c6.c6", 0x200001, 0x80000, 0x1e63411a),
  LOAD_16_8( SPRITES, "003-c7.c7", 0x300000, 0x80000, 0x2886710c),
  LOAD_16_8( SPRITES, "003-c8.c8", 0x300001, 0x80000, 0x422af22d),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( tpgolf, neogeo, "Top Player's Golf (NGM-003)(NGH-003)", SNK, 1990, GAME_SPORTS);

static struct ROM_INFO rom_maglord[] = /* MVS VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "005-pg1.p1", 0x000000, 0x080000, 0xbd0a492d),
  LOAD( FIXED, "005-s1.s1", 0x000000, 0x20000, 0x1c5369a2),
  LOAD( ROM2, "005-m1.m1", 0x00000, 0x40000, 0x26259f0f),
  LOAD( SMP1, "005-v11.v11", 0x000000, 0x080000, 0xcc0455fd),
  LOAD( YMSND_DELTAT, "005-v21.v21", 0x000000, 0x080000, 0xf94ab5b7),
  LOAD( YMSND_DELTAT, "005-v22.v22", 0x080000, 0x080000, 0x232cfd04),
  LOAD_16_8( SPRITES, "005-c1.c1", 0x000000, 0x80000, 0x806aee34),
  LOAD_16_8( SPRITES, "005-c2.c2", 0x000001, 0x80000, 0x34aa9a86),
  LOAD_16_8( SPRITES, "005-c3.c3", 0x100000, 0x80000, 0xc4c2b926),
  LOAD_16_8( SPRITES, "005-c4.c4", 0x100001, 0x80000, 0x9c46dcf4),
  LOAD_16_8( SPRITES, "005-c5.c5", 0x200000, 0x80000, 0x69086dec),
  LOAD_16_8( SPRITES, "005-c6.c6", 0x200001, 0x80000, 0xab7ac142),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_maglordh[] = /* AES VERSION clone of maglord */
{
  LOAD_SW16( CPU1, "005-p1.p1", 0x000000, 0x080000, 0x599043c5),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( maglord, neogeo, "Magician Lord (NGM-005)", ALPHA, 1990, GAME_PLATFORM);

CLNEI( maglordh, maglord, "Magician Lord (NGH-005)", ALPHA, 1990, GAME_PLATFORM);

static struct ROM_INFO rom_ridhero[] = /* MVS AND AES VERSION */
{
  LOAD_SW16( CPU1, "006-p1.p1", 0x000000, 0x080000, 0xd4aaf597),
  LOAD( FIXED, "006-s1.s1", 0x000000, 0x20000, 0xeb5189f0),
  LOAD( ROM2, "006-m1.m1", 0x00000, 0x40000, 0x92e7b4fe),
  LOAD( SMP1, "006-v11.v11", 0x000000, 0x080000, 0xcdf74a42),
  LOAD( SMP1, "006-v12.v12", 0x080000, 0x080000, 0xe2fd2371),
  LOAD( YMSND_DELTAT, "006-v21.v21", 0x000000, 0x080000, 0x94092bce),
  LOAD( YMSND_DELTAT, "006-v22.v22", 0x080000, 0x080000, 0x4e2cd7c3),
  LOAD( YMSND_DELTAT, "006-v23.v23", 0x100000, 0x080000, 0x069c71ed),
  LOAD( YMSND_DELTAT, "006-v24.v24", 0x180000, 0x080000, 0x89fbb825),
  LOAD_16_8( SPRITES, "006-c1.c1", 0x000000, 0x080000, 0x4a5c7f78),
  LOAD_16_8( SPRITES, "006-c2.c2", 0x000001, 0x080000, 0xe0b70ece),
  LOAD_16_8( SPRITES, "006-c3.c3", 0x100000, 0x080000, 0x8acff765),
  LOAD_16_8( SPRITES, "006-c4.c4", 0x100001, 0x080000, 0x205e3208),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ridheroh[] = // clone of ridhero
{
	/* Chip label p1h does not exist, renamed temporarly to pg1, marked BAD_DUMP. This needs to be verified. */
  LOAD_SW16( CPU1, "006-pg1.p1", 0x000000, 0x080000, 0x52445646),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( ridhero, neogeo, "Riding Hero (NGM-006)(NGH-006)", SNK, 1990, GAME_RACE);
CLNEI( ridheroh, ridhero, "Riding Hero (set 2)", SNK, 1990, GAME_RACE);

static struct ROM_INFO rom_alpham2[] = /* MVS AND AES VERSION */
{
  LOAD_SW16( CPU1, "007-p1.p1", 0x000000, 0x080000, 0x5b266f47),
  LOAD_SW16( CPU1, "007-p2.p2", 0x080000, 0x020000, 0xeb9c1044),
  LOAD( FIXED, "007-s1.s1", 0x000000, 0x20000, 0x85ec9acf),
  LOAD( ROM2, "007-m1.m1", 0x00000, 0x20000, 0x28dfe2cd),
  LOAD( SMP1, "007-v1.v1", 0x000000, 0x100000, 0xcd5db931),
  LOAD( SMP1, "007-v2.v2", 0x100000, 0x100000, 0x63e9b574),
  LOAD_16_8( SPRITES, "007-c1.c1", 0x000000, 0x100000, 0x8fba8ff3),
  LOAD_16_8( SPRITES, "007-c2.c2", 0x000001, 0x100000, 0x4dad2945),
  LOAD_16_8( SPRITES, "007-c3.c3", 0x200000, 0x080000, 0x68c2994e),
  LOAD_16_8( SPRITES, "007-c4.c4", 0x200001, 0x080000, 0x7d588349),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_alpham2p[] = /* early prototype - all roms were hand labeled with CRCs, dumps verified against them clone of alpham2 */
{
  LOAD_16_8( CPU1, "proto_007-p1.p1", 0x000001, 0x080000, 0xc763e52a),
  LOAD_16_8( CPU1, "proto_007-p2.p2", 0x000000, 0x080000, 0x7a0b435c),
  LOAD( FIXED, "proto_007-s1.s1", 0x000000, 0x20000, 0xefc9ae2e),
  LOAD( ROM2, "proto_007-m1.m1", 0x00000, 0x20000, 0x5976b464),
  LOAD( SMP1, "proto_007-v11.v11", 0x000000, 0x080000, 0x18eaa9e1),
  LOAD( SMP1, "proto_007-v12.v12", 0x080000, 0x080000, 0x2487d495),
  LOAD( SMP1, "proto_007-v13.v13", 0x100000, 0x080000, 0x25e60f25),
  LOAD( SMP1, "proto_007-v21.v21", 0x180000, 0x080000, 0xac44b75a),
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

static struct ROM_INFO rom_ncombat[] = /* MVS VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "009-p1.p1", 0x000000, 0x080000, 0xb45fcfbf),
  LOAD( FIXED, "009-s1.s1", 0x000000, 0x20000, 0xd49afee8),
  LOAD( ROM2, "009-m1.m1", 0x00000, 0x20000, 0xb5819863),
  LOAD( SMP1, "009-v11.v11", 0x000000, 0x080000, 0xcf32a59c),
  LOAD( SMP1, "009-v12.v12", 0x080000, 0x080000, 0x7b3588b7),
  LOAD( SMP1, "009-v13.v13", 0x100000, 0x080000, 0x505a01b5),
  LOAD( YMSND_DELTAT, "009-v21.v21", 0x000000, 0x080000, 0x365f9011),
  LOAD_16_8( SPRITES, "009-c1.c1", 0x000000, 0x80000, 0x33cc838e),
  LOAD_16_8( SPRITES, "009-c2.c2", 0x000001, 0x80000, 0x26877feb),
  LOAD_16_8( SPRITES, "009-c3.c3", 0x100000, 0x80000, 0x3b60a05d),
  LOAD_16_8( SPRITES, "009-c4.c4", 0x100001, 0x80000, 0x39c2d039),
  LOAD_16_8( SPRITES, "009-c5.c5", 0x200000, 0x80000, 0x67a4344e),
  LOAD_16_8( SPRITES, "009-c6.c6", 0x200001, 0x80000, 0x2eca8b19),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( ncombat, neogeo, "Ninja Combat (NGM-009)", ALPHA, 1990, GAME_BEAT);

static struct ROM_INFO rom_ncombath[] = /* AES VERSION clone of ncombat */
{
  LOAD_SW16( CPU1, "009-pg1.p1", 0x000000, 0x080000, 0x8e9f0add),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( ncombath, ncombat, "Ninja Combat (NGH-009)", ALPHA, 1990, GAME_BEAT);

static struct ROM_INFO rom_cyberlip[] = // clone of neogeo
{
  LOAD_SW16( CPU1, "010-p1.p1", 0x000000, 0x080000, 0x69a6b42d),
  LOAD( FIXED, "010-s1.s1", 0x000000, 0x20000, 0x79a35264),
  LOAD( ROM2, "010-m1.m1", 0x00000, 0x20000, 0x8be3a078),
  LOAD( SMP1, "010-v11.v11", 0x000000, 0x080000, 0x90224d22),
  LOAD( SMP1, "010-v12.v12", 0x080000, 0x080000, 0xa0cf1834),
  LOAD( SMP1, "010-v13.v13", 0x100000, 0x080000, 0xae38bc84),
  LOAD( SMP1, "010-v14.v14", 0x180000, 0x080000, 0x70899bd2),
  LOAD( YMSND_DELTAT, "010-v21.v21", 0x000000, 0x080000, 0x586f4cb2),
  LOAD_16_8( SPRITES, "010-c1.c1", 0x000000, 0x80000, 0x8bba5113),
  LOAD_16_8( SPRITES, "010-c2.c2", 0x000001, 0x80000, 0xcbf66432),
  LOAD_16_8( SPRITES, "010-c3.c3", 0x100000, 0x80000, 0xe4f86efc),
  LOAD_16_8( SPRITES, "010-c4.c4", 0x100001, 0x80000, 0xf7be4674),
  LOAD_16_8( SPRITES, "010-c5.c5", 0x200000, 0x80000, 0xe8076da0),
  LOAD_16_8( SPRITES, "010-c6.c6", 0x200001, 0x80000, 0xc495c567),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( cyberlip, neogeo, "Cyber-Lip (NGM-010)", SNK, 1990, GAME_PLATFORM);

static struct ROM_INFO rom_superspy[] = /* MVS AND AES VERSION */
{
  LOAD_SW16( CPU1, "011-p1.p1", 0x000000, 0x080000, 0xc7f944b5),
  LOAD_SW16( CPU1, "sp2.p2", 0x080000, 0x020000, 0x811a4faf),
  LOAD( FIXED, "011-s1.s1", 0x000000, 0x20000, 0xec5fdb96),
  LOAD( ROM2, "011-m1.m1", 0x00000, 0x40000, 0xca661f1b),
  LOAD( SMP1, "011-v11.v11", 0x000000, 0x100000, 0x5c674d5c),
  LOAD( SMP1, "011-v12.v12", 0x100000, 0x080000, 0x9f513d5a),
  LOAD( YMSND_DELTAT, "011-v21.v21", 0x000000, 0x080000, 0x426cd040),
  LOAD_16_8( SPRITES, "011-c1.c1", 0x000000, 0x100000, 0xcae7be57),
  LOAD_16_8( SPRITES, "011-c2.c2", 0x000001, 0x100000, 0x9e29d986),
  LOAD_16_8( SPRITES, "011-c3.c3", 0x200000, 0x100000, 0x14832ff2),
  LOAD_16_8( SPRITES, "011-c4.c4", 0x200001, 0x100000, 0xb7f63162),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( superspy, neogeo, "The Super Spy (NGM-011)(NGH-011)", SNK, 1990, GAME_FIGHT);

static struct ROM_INFO rom_mutnat[] = /* MVS AND AES VERSION */
{
  LOAD_SW16( CPU1, "014-p1.p1", 0x000000, 0x080000, 0x6f1699c8),
  LOAD( FIXED, "014-s1.s1", 0x000000, 0x20000, 0x99419733),
  LOAD( ROM2, "014-m1.m1", 0x00000, 0x20000, 0xb6683092),
  LOAD( SMP1, "014-v1.v1", 0x000000, 0x100000, 0x25419296),
  LOAD( SMP1, "014-v2.v2", 0x100000, 0x100000, 0x0de53d5e),
  LOAD_16_8( SPRITES, "014-c1.c1", 0x000000, 0x100000, 0x5e4381bf),
  LOAD_16_8( SPRITES, "014-c2.c2", 0x000001, 0x100000, 0x69ba4e18),
  LOAD_16_8( SPRITES, "014-c3.c3", 0x200000, 0x100000, 0x890327d5),
  LOAD_16_8( SPRITES, "014-c4.c4", 0x200001, 0x100000, 0xe4002651),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( mutnat, neogeo, "Mutation Nation (NGM-014)(NGH-014)", SNK, 1992, GAME_BEAT);

static struct ROM_INFO rom_kotm[] = /* MVS VERSION */
{
  LOAD_SW16( CPU1, "016-p1.p1", 0x000000, 0x080000, 0x1b818731),
  LOAD_SW16( CPU1, "016-p2.p2", 0x080000, 0x020000, 0x12afdc2b),
  LOAD( FIXED, "016-s1.s1", 0x000000, 0x20000, 0x1a2eeeb3),
  LOAD( ROM2, "016-m1.m1", 0x00000, 0x20000, 0x9da9ca10),
  LOAD( SMP1, "016-v1.v1", 0x000000, 0x100000, 0x86c0a502),
  LOAD( SMP1, "016-v2.v2", 0x100000, 0x100000, 0x5bc23ec5),
  LOAD_16_8( SPRITES, "016-c1.c1", 0x000000, 0x100000, 0x71471c25),
  LOAD_16_8( SPRITES, "016-c2.c2", 0x000001, 0x100000, 0x320db048),
  LOAD_16_8( SPRITES, "016-c3.c3", 0x200000, 0x100000, 0x98de7995),
  LOAD_16_8( SPRITES, "016-c4.c4", 0x200001, 0x100000, 0x070506e2),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_kotmh[] = /* AES VERSION clone of kotm */
{
  LOAD_SW16( CPU1, "016-hp1.p1", 0x000000, 0x080000, 0xb774621e),
  LOAD_SW16( CPU1, "016-p2.p2", 0x080000, 0x020000, 0x12afdc2b),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kotm, neogeo, "King of the Monsters (set 1)", SNK, 1991, GAME_BEAT);
CLNEI( kotmh, kotm, "King of the Monsters (set 2)", SNK, 1991, GAME_BEAT);

static struct ROM_INFO rom_sengoku[] = /* MVS AND AES VERSION */
{
  LOAD_SW16( CPU1, "017-p1.p1", 0x000000, 0x080000, 0xf8a63983),
  LOAD_SW16( CPU1, "017-p2.p2", 0x080000, 0x020000, 0x3024bbb3),
  LOAD( FIXED, "017-s1.s1", 0x000000, 0x20000, 0xb246204d),
  LOAD( ROM2, "017-m1.m1", 0x00000, 0x20000, 0x9b4f34c6),
  LOAD( SMP1, "017-v1.v1", 0x000000, 0x100000, 0x23663295),
  LOAD( SMP1, "017-v2.v2", 0x100000, 0x100000, 0xf61e6765),
  LOAD_16_8( SPRITES, "017-c1.c1", 0x000000, 0x100000, 0xb4eb82a1),
  LOAD_16_8( SPRITES, "017-c2.c2", 0x000001, 0x100000, 0xd55c550d),
  LOAD_16_8( SPRITES, "017-c3.c3", 0x200000, 0x100000, 0xed51ef65),
  LOAD_16_8( SPRITES, "017-c4.c4", 0x200001, 0x100000, 0xf4f3c9cb),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sengokuh[] = /* AES VERSION (US) clone of sengoku */
{
  LOAD_SW16( CPU1, "017-hp1.p1", 0x000000, 0x080000, 0x33eccae0),
  LOAD_SW16( CPU1, "017-p2.p2", 0x080000, 0x020000, 0x3024bbb3),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( sengoku, neogeo, "Sengoku / Sengoku Denshou (NGM-017)(NGH-017)", SNK, 1991, GAME_BEAT);
CLNEI( sengokuh, sengoku, "Sengoku / Sengoku Denshou (NGH-017)(US)", SNK, 1991, GAME_BEAT);

static struct ROM_INFO rom_burningf[] = /* MVS AND AES VERSION */
{
  LOAD_SW16( CPU1, "018-p1.p1", 0x000000, 0x080000, 0x4092c8db),
  LOAD( FIXED, "018-s1.s1", 0x000000, 0x20000, 0x6799ea0d),
  LOAD( ROM2, "018-m1.m1", 0x00000, 0x20000, 0x0c939ee2),
  LOAD( SMP1, "018-v1.v1", 0x000000, 0x100000, 0x508c9ffc),
  LOAD( SMP1, "018-v2.v2", 0x100000, 0x100000, 0x854ef277),
  LOAD_16_8( SPRITES, "018-c1.c1", 0x000000, 0x100000, 0x25a25e9b),
  LOAD_16_8( SPRITES, "018-c2.c2", 0x000001, 0x100000, 0xd4378876),
  LOAD_16_8( SPRITES, "018-c3.c3", 0x200000, 0x100000, 0x862b60da),
  LOAD_16_8( SPRITES, "018-c4.c4", 0x200001, 0x100000, 0xe2e0aff7),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_burningfh[] = /* AES VERSION (US) clone of burningf */
{
  LOAD_SW16( CPU1, "018-hp1.p1", 0x000000, 0x080000, 0xddffcbf4),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( burningf, neogeo, "Burning Fight (NGM-018)(NGH-018)", SNK, 1991, GAME_BEAT);
CLNEI( burningfh, burningf, "Burning Fight (NGH-018)(US)", SNK, 1991, GAME_BEAT);

static struct ROM_INFO rom_lbowling[] = /* MVS AND AES VERSION */
{
  LOAD_SW16( CPU1, "019-p1.p1", 0x000000, 0x080000, 0xa2de8445),
  LOAD( FIXED, "019-s1.s1", 0x000000, 0x20000, 0x5fcdc0ed),
  LOAD( ROM2, "019-m1.m1", 0x00000, 0x20000, 0xd568c17d),
  LOAD( SMP1, "019-v11.v11", 0x000000, 0x080000, 0x0fb74872),
  LOAD( SMP1, "019-v12.v12", 0x080000, 0x080000, 0x029faa57),
  LOAD( YMSND_DELTAT, "019-v21.v21", 0x000000, 0x080000, 0x2efd5ada),
  LOAD_16_8( SPRITES, "019-c1.c1", 0x000000, 0x080000, 0x4ccdef18),
  LOAD_16_8( SPRITES, "019-c2.c2", 0x000001, 0x080000, 0xd4dd0802),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( lbowling, neogeo, "League Bowling (NGM-019)(NGH-019)", SNK, 1990, GAME_SPORTS);

static struct ROM_INFO rom_gpilots[] = /* MVS AND AES VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "020-p1.p1", 0x000000, 0x080000, 0xe6f2fe64),
  LOAD_SW16( CPU1, "020-p2.p2", 0x080000, 0x020000, 0xedcb22ac),
  LOAD( FIXED, "020-s1.s1", 0x000000, 0x20000, 0xa6d83d53),
  LOAD( ROM2, "020-m1.m1", 0x00000, 0x20000, 0x48409377),
  LOAD( SMP1, "020-v11.v11", 0x000000, 0x100000, 0x1b526c8b),
  LOAD( SMP1, "020-v12.v12", 0x100000, 0x080000, 0x4a9e6f03),
  LOAD( YMSND_DELTAT, "020-v21.v21", 0x000000, 0x080000, 0x7abf113d),
  LOAD_16_8( SPRITES, "020-c1.c1", 0x000000, 0x100000, 0xbd6fe78e),
  LOAD_16_8( SPRITES, "020-c2.c2", 0x000001, 0x100000, 0x5f4a925c),
  LOAD_16_8( SPRITES, "020-c3.c3", 0x200000, 0x100000, 0xd1e42fd0),
  LOAD_16_8( SPRITES, "020-c4.c4", 0x200001, 0x100000, 0xedde439b),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_gpilotsh[] = /* AES VERSION (US) clone of gpilots */
{
  LOAD_SW16( CPU1, "020-hp1.p1", 0x000000, 0x080000, 0x7cdb01ce),
  LOAD_SW16( CPU1, "020-p2.p2", 0x080000, 0x020000, 0xedcb22ac),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( gpilots, neogeo, "Ghost Pilots (NGM-020)(NGH-020)", SNK, 1991, GAME_SHOOT);
CLNEI( gpilotsh, gpilots, "Ghost Pilots (NGH-020)(US)", SNK, 1991, GAME_SHOOT);

static struct ROM_INFO rom_joyjoy[] = /* MVS AND AES VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "021-p1.p1", 0x000000, 0x080000, 0x39c3478f),
  LOAD( FIXED, "021-s1.s1", 0x000000, 0x20000, 0x6956d778),
  LOAD( ROM2, "021-m1.m1", 0x00000, 0x40000, 0x5a4be5e8),
  LOAD( SMP1, "021-v11.v11", 0x000000, 0x080000, 0x66c1e5c4),
  LOAD( YMSND_DELTAT, "021-v21.v21", 0x000000, 0x080000, 0x8ed20a86),
  LOAD_16_8( SPRITES, "021-c1.c1", 0x000000, 0x080000, 0x509250ec),
  LOAD_16_8( SPRITES, "021-c2.c2", 0x000001, 0x080000, 0x09ed5258),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( joyjoy, neogeo, "Puzzled / Joy Joy Kid (NGM-021)(NGH-021)", SNK, 1990, GAME_PUZZLE);

static struct ROM_INFO rom_bjourney[] = /* MVS AND AES VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "022-p1.p1", 0x000000, 0x100000, 0x6a2f6d4a),
  LOAD( FIXED, "022-s1.s1", 0x000000, 0x20000, 0x843c3624),
  LOAD( ROM2, "022-m1.m1", 0x00000, 0x20000, 0x8e1d4ab6),
  LOAD( SMP1, "022-v11.v11", 0x000000, 0x100000, 0x2cb4ad91),
  LOAD( SMP1, "022-v22.v22", 0x100000, 0x100000, 0x65a54d13),
  LOAD_16_8( SPRITES, "022-c1.c1", 0x000000, 0x100000, 0x4d47a48c),
  LOAD_16_8( SPRITES, "022-c2.c2", 0x000001, 0x100000, 0xe8c1491a),
  LOAD_16_8( SPRITES, "022-c3.c3", 0x200000, 0x080000, 0x66e69753),
  LOAD_16_8( SPRITES, "022-c4.c4", 0x200001, 0x080000, 0x71bfd48a),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( bjourney, neogeo, "Blue's Journey / Raguy (ALM-001)(ALH-001)", ALPHA, 1990, GAME_PLATFORM);

static struct ROM_INFO rom_lresort[] = // clone of neogeo
{
  LOAD_SW16( CPU1, "024-p1.p1", 0x000000, 0x080000, 0x89c4ab97),
  LOAD( FIXED, "024-s1.s1", 0x000000, 0x20000, 0x5cef5cc6),
  LOAD( ROM2, "024-m1.m1", 0x00000, 0x20000, 0xcec19742),
  LOAD( SMP1, "024-v1.v1", 0x000000, 0x100000, 0xefdfa063),
  LOAD( SMP1, "024-v2.v2", 0x100000, 0x100000, 0x3c7997c0),
  LOAD_16_8( SPRITES, "024-c1.c1", 0x000000, 0x100000, 0x3617c2dc),
  LOAD_16_8( SPRITES, "024-c2.c2", 0x000001, 0x100000, 0x3f0a7fd8),
  LOAD_16_8( SPRITES, "024-c3.c3", 0x200000, 0x080000, 0xe9f745f8),
  LOAD_16_8( SPRITES, "024-c4.c4", 0x200001, 0x080000, 0x7382fefb),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( lresort, neogeo, "Last Resort", SNK, 1992, GAME_SHOOT);

static struct ROM_INFO rom_eightman[] = /* MVS AND AES VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "025-p1.p1", 0x000000, 0x080000, 0x43344cb0),
  LOAD( FIXED, "025-s1.s1", 0x000000, 0x20000, 0xa402202b),
  LOAD( ROM2, "025-m1.m1", 0x00000, 0x20000, 0x9927034c),
  LOAD( SMP1, "025-v1.v1", 0x000000, 0x100000, 0x4558558a),
  LOAD( SMP1, "025-v2.v2", 0x100000, 0x100000, 0xc5e052e9),
  LOAD_16_8( SPRITES, "025-c1.c1", 0x000000, 0x100000, 0x555e16a4),
  LOAD_16_8( SPRITES, "025-c2.c2", 0x000001, 0x100000, 0xe1ee51c3),
  LOAD_16_8( SPRITES, "025-c3.c3", 0x200000, 0x080000, 0x0923d5b0),
  LOAD_16_8( SPRITES, "025-c4.c4", 0x200001, 0x080000, 0xe3eca67b),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( eightman, neogeo, "Eight Man (NGM-025)(NGH-025)", SNK, 1991, GAME_BEAT);

static struct ROM_INFO rom_legendos[] = // clone of neogeo
{
  LOAD_SW16( CPU1, "029-p1.p1", 0x000000, 0x080000, 0x9d563f19),
  LOAD( FIXED, "029-s1.s1", 0x000000, 0x20000, 0xbcd502f0),
  LOAD( ROM2, "029-m1.m1", 0x00000, 0x20000, 0x6f2843f0),
  LOAD( SMP1, "029-v1.v1", 0x000000, 0x100000, 0x85065452),
  LOAD_16_8( SPRITES, "029-c1.c1", 0x000000, 0x100000, 0x2f5ab875),
  LOAD_16_8( SPRITES, "029-c2.c2", 0x000001, 0x100000, 0x318b2711),
  LOAD_16_8( SPRITES, "029-c3.c3", 0x200000, 0x100000, 0x6bc52cb2),
  LOAD_16_8( SPRITES, "029-c4.c4", 0x200001, 0x100000, 0x37ef298c),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( legendos, neogeo, "Legend of Success Joe / Ashita no Joe Densetsu", SNK, 1991, GAME_BEAT);

static struct ROM_INFO rom_2020bb[] = // clone of neogeo
{
  LOAD_SW16( CPU1, "030-p1.p1", 0x000000, 0x080000, 0xd396c9cb),
  LOAD( FIXED, "030-s1.s1", 0x000000, 0x20000, 0x7015b8fc),
  LOAD( ROM2, "030-m1.m1", 0x00000, 0x20000, 0x4cf466ec),
  LOAD( SMP1, "030-v1.v1", 0x000000, 0x100000, 0xd4ca364e),
  LOAD( SMP1, "030-v2.v2", 0x100000, 0x100000, 0x54994455),
  LOAD_16_8( SPRITES, "030-c1.c1", 0x000000, 0x100000, 0x4f5e19bd),
  LOAD_16_8( SPRITES, "030-c2.c2", 0x000001, 0x100000, 0xd6314bf0),
  LOAD_16_8( SPRITES, "030-c3.c3", 0x200000, 0x100000, 0x47fddfee),
  LOAD_16_8( SPRITES, "030-c4.c4", 0x200001, 0x100000, 0x780d1c4e),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( 2020bb, neogeo, "2020 Super Baseball (set 1)", SNK, 1991, GAME_SPORTS);

static struct ROM_INFO rom_socbrawl[] = /* MVS VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "031-pg1.p1", 0x000000, 0x080000, 0x17f034a7),
  LOAD( FIXED, "031-s1.s1", 0x000000, 0x20000, 0x4c117174),
  LOAD( ROM2, "031-m1.m1", 0x00000, 0x20000, 0xcb37427c),
  LOAD( SMP1, "031-v1.v1", 0x000000, 0x100000, 0xcc78497e),
  LOAD( SMP1, "031-v2.v2", 0x100000, 0x100000, 0xdda043c6),
  LOAD_16_8( SPRITES, "031-c1.c1", 0x000000, 0x100000, 0xbd0a4eb8),
  LOAD_16_8( SPRITES, "031-c2.c2", 0x000001, 0x100000, 0xefde5382),
  LOAD_16_8( SPRITES, "031-c3.c3", 0x200000, 0x080000, 0x580f7f33),
  LOAD_16_8( SPRITES, "031-c4.c4", 0x200001, 0x080000, 0xed297de8),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( socbrawl, neogeo, "Soccer Brawl (NGM-031)", SNK, 1991, GAME_SPORTS);

static struct ROM_INFO rom_socbrawlh[] = /* AES VERSION clone of socbrawl */
{
  LOAD_SW16( CPU1, "031-p1.p1", 0x000000, 0x080000, 0xa2801c24),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( socbrawlh, socbrawl, "Soccer Brawl (NGH-031)", SNK, 1991, GAME_SPORTS);

static struct ROM_INFO rom_fatfury1[] = /* MVS AND AES VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "033-p1.p1", 0x000000, 0x080000, 0x47ebdc2f),
  LOAD_SW16( CPU1, "033-p2.p2", 0x080000, 0x020000, 0xc473af1c),
  LOAD( FIXED, "033-s1.s1", 0x000000, 0x20000, 0x3c3bdf8c),
  LOAD( ROM2, "033-m1.m1", 0x00000, 0x20000, 0x5be10ffd),
  LOAD( SMP1, "033-v1.v1", 0x000000, 0x100000, 0x212fd20d),
  LOAD( SMP1, "033-v2.v2", 0x100000, 0x100000, 0xfa2ae47f),
  LOAD_16_8( SPRITES, "033-c1.c1", 0x000000, 0x100000, 0x74317e54),
  LOAD_16_8( SPRITES, "033-c2.c2", 0x000001, 0x100000, 0x5bb952f3),
  LOAD_16_8( SPRITES, "033-c3.c3", 0x200000, 0x100000, 0x9b714a7c),
  LOAD_16_8( SPRITES, "033-c4.c4", 0x200001, 0x100000, 0x9397476a),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( fatfury1, neogeo, "Fatal Fury - King of Fighters / Garou Densetsu - shukumei no tatakai (NGM-033)(NGH-033)", SNK, 1991, GAME_BEAT);

static struct ROM_INFO rom_roboarmy[] = // clone of neogeo
{
  LOAD_SW16( CPU1, "032-p1.p1", 0x000000, 0x080000, 0xcd11cbd4),
  LOAD( FIXED, "032-s1.s1", 0x000000, 0x20000, 0xac0daa1b),
  LOAD( ROM2, "032-m1.m1", 0x00000, 0x20000, 0x35ec952d),
  LOAD( SMP1, "032-v1.v1", 0x000000, 0x100000, 0x63791533),
  LOAD( SMP1, "032-v2.v2", 0x100000, 0x100000, 0xeb95de70),
  LOAD_16_8( SPRITES, "032-c1.c1", 0x000000, 0x100000, 0x97984c6c),
  LOAD_16_8( SPRITES, "032-c2.c2", 0x000001, 0x100000, 0x65773122),
  LOAD_16_8( SPRITES, "032-c3.c3", 0x200000, 0x080000, 0x40adfccd),
  LOAD_16_8( SPRITES, "032-c4.c4", 0x200001, 0x080000, 0x462571de),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( roboarmy, neogeo, "Robo Army", SNK, 1991, GAME_BEAT);

static struct ROM_INFO rom_fbfrenzy[] = /* MVS AND AES VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "034-p1.p1", 0x000000, 0x080000, 0xcdef6b19),
  LOAD( FIXED, "034-s1.s1", 0x000000, 0x20000, 0x8472ed44),
  LOAD( ROM2, "034-m1.m1", 0x00000, 0x20000, 0xf41b16b8),
  LOAD( SMP1, "034-v1.v1", 0x000000, 0x100000, 0x50c9d0dd),
  LOAD( SMP1, "034-v2.v2", 0x100000, 0x100000, 0x5aa15686),
  LOAD_16_8( SPRITES, "034-c1.c1", 0x000000, 0x100000, 0x91c56e78),
  LOAD_16_8( SPRITES, "034-c2.c2", 0x000001, 0x100000, 0x9743ea2f),
  LOAD_16_8( SPRITES, "034-c3.c3", 0x200000, 0x080000, 0xe5aa65f5),
  LOAD_16_8( SPRITES, "034-c4.c4", 0x200001, 0x080000, 0x0eb138cc),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( fbfrenzy, neogeo, "Football Frenzy (NGM-034)(NGH-034)", SNK, 1992, GAME_SPORTS);

static struct ROM_INFO rom_crsword[] = /* MVS AND AES VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "037-p1.p1", 0x000000, 0x080000, 0xe7f2553c),
  LOAD( FIXED, "037-s1.s1", 0x000000, 0x20000, 0x74651f27),
  LOAD( ROM2, "037-m1.m1", 0x00000, 0x20000, 0x9504b2c6),
  LOAD( SMP1, "037-v1.v1", 0x000000, 0x100000, 0x61fedf65),
  LOAD_16_8( SPRITES, "037-c1.c1", 0x000000, 0x100000, 0x09df6892),
  LOAD_16_8( SPRITES, "037-c2.c2", 0x000001, 0x100000, 0xac122a78),
  LOAD_16_8( SPRITES, "037-c3.c3", 0x200000, 0x100000, 0x9d7ed1ca),
  LOAD_16_8( SPRITES, "037-c4.c4", 0x200001, 0x100000, 0x4a24395d),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( crsword, neogeo, "Crossed Swords (ALM-002)(ALH-002)", ALPHA, 1991, GAME_FIGHT);

static struct ROM_INFO rom_trally[] = /* MVS AND AES VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "038-p1.p1", 0x000000, 0x080000, 0x1e52a576),
  LOAD_SW16( CPU1, "038-p2.p2", 0x080000, 0x080000, 0xa5193e2f),
  LOAD( FIXED, "038-s1.s1", 0x000000, 0x20000, 0xfff62ae3),
  LOAD( ROM2, "038-m1.m1", 0x00000, 0x20000, 0x0908707e),
  LOAD( SMP1, "038-v1.v1", 0x000000, 0x100000, 0x5ccd9fd5),
  LOAD( SMP1, "038-v2.v2", 0x100000, 0x080000, 0xddd8d1e6),
  LOAD_16_8( SPRITES, "038-c1.c1", 0x000000, 0x100000, 0xc58323d4),
  LOAD_16_8( SPRITES, "038-c2.c2", 0x000001, 0x100000, 0xbba9c29e),
  LOAD_16_8( SPRITES, "038-c3.c3", 0x200000, 0x080000, 0x3bb7b9d6),
  LOAD_16_8( SPRITES, "038-c4.c4", 0x200001, 0x080000, 0xa4513ecf),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( trally, neogeo, "Thrash Rally (ALM-003)(ALH-003)", ALPHA, 1991, GAME_MISC);

static struct ROM_INFO rom_kotm2[] = /* MVS AND AES VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "039-p1.p1", 0x000000, 0x080000, 0xb372d54c),
  LOAD_SW16( CPU1, "039-p2.p2", 0x080000, 0x080000, 0x28661afe),
  LOAD( FIXED, "039-s1.s1", 0x000000, 0x20000, 0x63ee053a),
  LOAD( ROM2, "039-m1.m1", 0x00000, 0x20000, 0x0c5b2ad5),
  LOAD( SMP1, "039-v2.v2", 0x000000, 0x200000, 0x86d34b25),
  LOAD( SMP1, "039-v4.v4", 0x200000, 0x100000, 0x8fa62a0b),
  LOAD_16_8( SPRITES, "039-c1.c1", 0, 0x100000, 0x6d1c4aa9),
  { "039-c1.c1", 0x100000, 0x6d1c4aa9, REGION_SPRITES, 0x400000, LOAD_CONTINUE },
  LOAD_16_8( SPRITES, "039-c2.c2", 1, 0x100000, 0xf7b75337),
  { "039-c2.c2", 0x100000, 0xf7b75337, REGION_SPRITES, 0x400001, LOAD_CONTINUE },
  LOAD_16_8( SPRITES, "039-c3.c3", 0x200000, 0x080000, 0xbfc4f0b2),
  LOAD_16_8( SPRITES, "039-c4.c4", 0x200001, 0x080000, 0x81c9c250),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kotm2, neogeo, "King of the Monsters 2 - The Next Thing (NGM-039)(NGH-039)", SNK, 1992, GAME_BEAT);

static struct ROM_INFO rom_sengoku2[] = // clone of neogeo
{
  LOAD_SW16( CPU1, "040-p1.p1", 0x000000, 0x100000, 0x6dde02c2),
  LOAD( FIXED, "040-s1.s1", 0x000000, 0x20000, 0xcd9802a3),
  LOAD( ROM2, "040-m1.m1", 0x00000, 0x20000, 0xd4de4bca),
  LOAD( SMP1, "040-v1.v1", 0x000000, 0x200000, 0x71cb4b5d),
  LOAD( SMP1, "040-v2.v2", 0x200000, 0x100000, 0xc5cece01),
  LOAD_16_8( SPRITES, "040-c1.c1", 0x000000, 0x100000, 0xfaa8ea99),
  { "040-c1.c1", 0x100000 , 0xfaa8ea99, REGION_SPRITES, 0x400000, LOAD_CONTINUE },
  LOAD_16_8( SPRITES, "040-c2.c2", 0x000001, 0x100000, 0x87d0ec65),
  { "040-c2.c2", 0x100000 , 0x87d0ec65, REGION_SPRITES, 0x400001, LOAD_CONTINUE },
  LOAD_16_8( SPRITES, "040-c3.c3", 0x200000, 0x080000, 0x24b5ba80),
  LOAD_16_8( SPRITES, "040-c4.c4", 0x200001, 0x080000, 0x1c9e9930),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( sengoku2, neogeo, "Sengoku 2 / Sengoku Denshou 2", SNK, 1993, GAME_BEAT);

static struct ROM_INFO rom_bstars2[] = // clone of neogeo
{
  LOAD_SW16( CPU1, "041-p1.p1", 0x000000, 0x080000, 0x523567fd),
  LOAD( FIXED, "041-s1.s1", 0x000000, 0x20000, 0x015c5c94),
  LOAD( ROM2, "041-m1.m1", 0x00000, 0x20000, 0x15c177a6),
  LOAD( SMP1, "041-v1.v1", 0x000000, 0x100000, 0xcb1da093),
  LOAD( SMP1, "041-v2.v2", 0x100000, 0x100000, 0x1c954a9d),
  LOAD( SMP1, "041-v3.v3", 0x200000, 0x080000, 0xafaa0180),
  LOAD_16_8( SPRITES, "041-c1.c1", 0x000000, 0x100000, 0xb39a12e1),
  LOAD_16_8( SPRITES, "041-c2.c2", 0x000001, 0x100000, 0x766cfc2f),
  LOAD_16_8( SPRITES, "041-c3.c3", 0x200000, 0x100000, 0xfb31339d),
  LOAD_16_8( SPRITES, "041-c4.c4", 0x200001, 0x100000, 0x70457a0c),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( bstars2, neogeo, "Baseball Stars 2", SNK, 1992, GAME_SPORTS);

static struct ROM_INFO rom_3countb[] = // clone of neogeo
{
	/* The original p1 is 8mbit; also found sets with p1 / p2 4mbit on eprom. */
  LOAD_SW16( CPU1, "043-p1.p1", 0x000000, 0x100000, 0xffbdd928),
  LOAD( FIXED, "043-s1.s1", 0x000000, 0x20000, 0xc362d484),
  LOAD( ROM2, "043-m1.m1", 0x00000, 0x20000, 0x7eab59cb),
  LOAD( SMP1, "043-v1.v1", 0x000000, 0x200000, 0x63688ce8),
  LOAD( SMP1, "043-v2.v2", 0x200000, 0x200000, 0xc69a827b),
  LOAD_16_8( SPRITES, "043-c1.c1", 0x000000, 0x100000, 0xbad2d67f),
  { "043-c1.c1", 0x100000 , 0xbad2d67f, REGION_SPRITES, 0x400000, LOAD_CONTINUE },
  LOAD_16_8( SPRITES, "043-c2.c2", 0x000001, 0x100000, 0xa7fbda95),
  { "043-c2.c2", 0x100000 , 0xa7fbda95, REGION_SPRITES, 0x400001, LOAD_CONTINUE },
  LOAD_16_8( SPRITES, "043-c3.c3", 0x200000, 0x100000, 0xf00be011),
  { "043-c3.c3", 0x100000 , 0xf00be011, REGION_SPRITES, 0x600000, LOAD_CONTINUE },
  LOAD_16_8( SPRITES, "043-c4.c4", 0x200001, 0x100000, 0x1887e5c0),
  { "043-c4.c4", 0x100000 , 0x1887e5c0, REGION_SPRITES, 0x600001, LOAD_CONTINUE },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( 3countb, neogeo, "3 Count Bout / Fire Suplex (NGM-043)(NGH-043)", SNK, 1993, GAME_FIGHT);

static struct ROM_INFO rom_aof[] = /* MVS AND AES VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "044-p1.p1", 0x000000, 0x080000, 0xca9f7a6d),
  LOAD( FIXED, "044-s1.s1", 0x000000, 0x20000, 0x89903f39),
  LOAD( ROM2, "044-m1.m1", 0x00000, 0x20000, 0x0987e4bb),
  LOAD( SMP1, "044-v2.v2", 0x000000, 0x200000, 0x3ec632ea),
  LOAD( SMP1, "044-v4.v4", 0x200000, 0x200000, 0x4b0f8e23),
  LOAD_16_8( SPRITES, "044-c1.c1", 0x000000, 0x100000, 0xddab98a7),
  { "044-c1.c1", 0x100000 , 0xddab98a7, REGION_SPRITES, 0x400000, LOAD_CONTINUE },
  LOAD_16_8( SPRITES, "044-c2.c2", 0x000001, 0x100000, 0xd8ccd575),
  { "044-c2.c2", 0x100000 , 0xd8ccd575, REGION_SPRITES, 0x400001, LOAD_CONTINUE },
  LOAD_16_8( SPRITES, "044-c3.c3", 0x200000, 0x100000, 0x403e898a),
  { "044-c3.c3", 0x100000 , 0x403e898a, REGION_SPRITES, 0x600000, LOAD_CONTINUE },
  LOAD_16_8( SPRITES, "044-c4.c4", 0x200001, 0x100000, 0x6235fbaa),
  { "044-c4.c4", 0x100000 , 0x6235fbaa, REGION_SPRITES, 0x600001, LOAD_CONTINUE },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( aof, neogeo, "Art of Fighting / Ryuuko no Ken (NGM-044)(NGH-044)", SNK, 1992, GAME_BEAT);

static struct ROM_INFO rom_samsho[] = /* MVS VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "045-p1.p1", 0x000000, 0x100000, 0xdfe51bf0),
  LOAD_SW16( CPU1, "045-pg2.sp2", 0x100000, 0x100000, 0x46745b94),
  LOAD( FIXED, "045-s1.s1", 0x000000, 0x20000, 0x9142a4d3),
  LOAD( ROM2, "045-m1.m1", 0x00000, 0x20000, 0x95170640),
  LOAD( SMP1, "045-v1.v1", 0x000000, 0x200000, 0x37f78a9b),
  LOAD( SMP1, "045-v2.v2", 0x200000, 0x200000, 0x568b20cf),
  LOAD_16_8( SPRITES, "045-c1.c1", 0x000000, 0x200000, 0x2e5873a4),
  LOAD_16_8( SPRITES, "045-c2.c2", 0x000001, 0x200000, 0x04febb10),
  LOAD_16_8( SPRITES, "045-c3.c3", 0x400000, 0x200000, 0xf3dabd1e),
  LOAD_16_8( SPRITES, "045-c4.c4", 0x400001, 0x200000, 0x935c62f0),
  LOAD_16_8( SPRITES, "045-c51.c5", 0x800000, 0x100000, 0x81932894),
  LOAD_16_8( SPRITES, "045-c61.c6", 0x800001, 0x100000, 0xbe30612e),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( samsho, neogeo, "Samurai Shodown / Samurai Spirits (NGM-045)", SNK, 1993, GAME_BEAT);

static struct ROM_INFO rom_samshoh[] = /* AES VERSION clone of samsho */
{
  LOAD_SW16( CPU1, "045-p1.p1", 0x000000, 0x100000, 0xdfe51bf0),
  LOAD_SW16( CPU1, "045-p2.sp2", 0x100000, 0x080000, 0x38ee9ba9),
  LOAD_16_8( SPRITES, "045-c1.c1", 0x000000, 0x200000, 0x2e5873a4),
  LOAD_16_8( SPRITES, "045-c2.c2", 0x000001, 0x200000, 0x04febb10),
  LOAD_16_8( SPRITES, "045-c3.c3", 0x400000, 0x200000, 0xf3dabd1e),
  LOAD_16_8( SPRITES, "045-c4.c4", 0x400001, 0x200000, 0x935c62f0),
  LOAD_16_8( SPRITES, "045-c5.c5", 0x800000, 0x080000, 0xa2bb8284),
  LOAD_16_8( SPRITES, "045-c6.c6", 0x800001, 0x080000, 0x4fa71252),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( samshoh, samsho, "Samurai Shodown / Samurai Spirits (NGH-045)", SNK, 1993, GAME_BEAT);

static struct ROM_INFO rom_tophuntr[] = /* MVS VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "046-p1.p1", 0x000000, 0x100000, 0x69fa9e29),
  LOAD_SW16( CPU1, "046-p2.sp2", 0x100000, 0x100000, 0xf182cb3e),
  LOAD( FIXED, "046-s1.s1", 0x000000, 0x20000, 0x14b01d7b),
  LOAD( ROM2, "046-m1.m1", 0x00000, 0x20000, 0x3f84bb9f),
  LOAD( SMP1, "046-v1.v1", 0x000000, 0x100000, 0xc1f9c2db),
  LOAD( SMP1, "046-v2.v2", 0x100000, 0x100000, 0x56254a64),
  LOAD( SMP1, "046-v3.v3", 0x200000, 0x100000, 0x58113fb1),
  LOAD( SMP1, "046-v4.v4", 0x300000, 0x100000, 0x4f54c187),
  LOAD_16_8( SPRITES, "046-c1.c1", 0x000000, 0x100000, 0xfa720a4a),
  LOAD_16_8( SPRITES, "046-c2.c2", 0x000001, 0x100000, 0xc900c205),
  LOAD_16_8( SPRITES, "046-c3.c3", 0x200000, 0x100000, 0x880e3c25),
  LOAD_16_8( SPRITES, "046-c4.c4", 0x200001, 0x100000, 0x7a2248aa),
  LOAD_16_8( SPRITES, "046-c5.c5", 0x400000, 0x100000, 0x4b735e45),
  LOAD_16_8( SPRITES, "046-c6.c6", 0x400001, 0x100000, 0x273171df),
  LOAD_16_8( SPRITES, "046-c7.c7", 0x600000, 0x100000, 0x12829c4c),
  LOAD_16_8( SPRITES, "046-c8.c8", 0x600001, 0x100000, 0xc944e03d),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( tophuntr, neogeo, "Top Hunter - Roddy & Cathy (NGM-046)", SNK, 1994, GAME_PLATFORM);

static struct ROM_INFO rom_fatfury2[] = /* MVS AND AES VERSION clone of neogeo */
{
	/* The original p1 is 8mbit; also found sets with p1 / p2 4mbit on eprom. */
  LOAD_SW16( CPU1, "047-p1.p1", 0x000000, 0x100000, 0xecfdbb69),
  LOAD( FIXED, "047-s1.s1", 0x000000, 0x20000, 0xd7dbbf39),
  LOAD( ROM2, "047-m1.m1", 0x00000, 0x20000, 0x820b0ba7),
  LOAD( SMP1, "047-v1.v1", 0x000000, 0x200000, 0xd9d00784),
  LOAD( SMP1, "047-v2.v2", 0x200000, 0x200000, 0x2c9a4b33),
  LOAD_16_8( SPRITES, "047-c1.c1", 0x000000, 0x100000, 0xf72a939e),
  { "047-c1.c1", 0x100000 , 0xf72a939e, REGION_SPRITES, 0x400000, LOAD_CONTINUE },
  LOAD_16_8( SPRITES, "047-c2.c2", 0x000001, 0x100000, 0x05119a0d),
  { "047-c2.c2", 0x100000 , 0x05119a0d, REGION_SPRITES, 0x400001, LOAD_CONTINUE },
  LOAD_16_8( SPRITES, "047-c3.c3", 0x200000, 0x100000, 0x01e00738),
  { "047-c3.c3", 0x100000 , 0x01e00738, REGION_SPRITES, 0x600000, LOAD_CONTINUE },
  LOAD_16_8( SPRITES, "047-c4.c4", 0x200001, 0x100000, 0x9fe27432),
  { "047-c4.c4", 0x100000 , 0x9fe27432, REGION_SPRITES, 0x600001, LOAD_CONTINUE },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( fatfury2, neogeo, "Fatal Fury 2 / Garou Densetsu 2 - arata-naru tatakai (NGM-047)(NGH-047)", SNK, 1992, GAME_BEAT);

static struct ROM_INFO rom_androdun[] = /* MVS AND AES VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "049-p1.p1", 0x000000, 0x080000, 0x3b857da2),
  LOAD_SW16( CPU1, "049-p2.p2", 0x080000, 0x080000, 0x2f062209),
  LOAD( FIXED, "049-s1.s1", 0x000000, 0x20000, 0x6349de5d),
  LOAD( ROM2, "049-m1.m1", 0x00000, 0x20000, 0xedd2acf4),
  LOAD( SMP1, "049-v1.v1", 0x000000, 0x100000, 0xce43cb89),
  LOAD_16_8( SPRITES, "049-c1.c1", 0x000000, 0x100000, 0x7ace6db3),
  LOAD_16_8( SPRITES, "049-c2.c2", 0x000001, 0x100000, 0xb17024f7),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( androdun, neogeo, "Andro Dunos (NGM-049)(NGH-049)", VISCO, 1992, GAME_SHOOT);

static struct ROM_INFO rom_ncommand[] = // clone of neogeo
{
  LOAD_SW16( CPU1, "050-p1.p1", 0x000000, 0x100000, 0x4e097c40),
  LOAD( FIXED, "050-s1.s1", 0x000000, 0x20000, 0xdb8f9c8e),
  LOAD( ROM2, "050-m1.m1", 0x00000, 0x20000, 0x6fcf07d3),
  LOAD( SMP1, "050-v1.v1", 0x000000, 0x100000, 0x23c3ab42),
  LOAD( SMP1, "050-v2.v2", 0x100000, 0x080000, 0x80b8a984),
  LOAD_16_8( SPRITES, "050-c1.c1", 0x000000, 0x100000, 0x87421a0a),
  LOAD_16_8( SPRITES, "050-c2.c2", 0x000001, 0x100000, 0xc4cf5548),
  LOAD_16_8( SPRITES, "050-c3.c3", 0x200000, 0x100000, 0x03422c1e),
  LOAD_16_8( SPRITES, "050-c4.c4", 0x200001, 0x100000, 0x0845eadb),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( ncommand, neogeo, "Ninja Commando", ALPHA, 1992, GAME_SHOOT);

static struct ROM_INFO rom_viewpoin[] = // clone of neogeo
{
  LOAD_SW16( CPU1, "051-p1.p1", 0x000000, 0x100000, 0x17aa899d),
  LOAD( FIXED, "051-s1.s1", 0x000000, 0x20000, 0x9fea5758),
  LOAD( ROM2, "051-m1.m1", 0x00000, 0x20000, 0x8e69f29a),
	/* What board was originally used (labels 051-V2 and 051-V4)? MVS is twice confirmed on NEO-MVS PROG-G2 */
  LOAD( SMP1, "051-v2.v1", 0x000000, 0x200000, 0x019978b6),
  LOAD( SMP1, "051-v4.v2", 0x200000, 0x200000, 0x5758f38c),
  LOAD_16_8( SPRITES, "051-c1.c1", 0x000000, 0x100000, 0xd624c132),
  { "051-c1.c1", 0x100000 , 0xd624c132, REGION_SPRITES, 0x400000, LOAD_CONTINUE },
  LOAD_16_8( SPRITES, "051-c2.c2", 0x000001, 0x100000, 0x40d69f1e),
  { "051-c2.c2", 0x100000 , 0x40d69f1e, REGION_SPRITES, 0x400001, LOAD_CONTINUE },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( viewpoin, neogeo, "Viewpoint", SAMMY, 1992, GAME_SHOOT);

static struct ROM_INFO rom_ssideki[] = // clone of neogeo
{
  LOAD_SW16( CPU1, "052-p1.p1", 0x000000, 0x080000, 0x9cd97256),
  LOAD( FIXED, "052-s1.s1", 0x000000, 0x20000, 0x97689804),
  LOAD( ROM2, "052-m1.m1", 0x00000, 0x20000, 0x49f17d2d),
  LOAD( SMP1, "052-v1.v1", 0x000000, 0x200000, 0x22c097a5),
  LOAD_16_8( SPRITES, "052-c1.c1", 0x000000, 0x100000, 0x53e1c002),
  { "052-c1.c1", 0x100000 , 0x53e1c002, REGION_SPRITES, 0x400000, LOAD_CONTINUE },
  LOAD_16_8( SPRITES, "052-c2.c2", 0x000001, 0x100000, 0x776a2d1f),
  { "052-c2.c2", 0x100000 , 0x776a2d1f, REGION_SPRITES, 0x400001, LOAD_CONTINUE },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( ssideki, neogeo, "Super Sidekicks / Tokuten Ou", SNK, 1992, GAME_SPORTS);

static struct ROM_INFO rom_wh1[] = /* MVS VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "053-epr.p1", 0x000000, 0x080000, 0xd42e1e9a),
	/* P's on eprom, correct chip label unknown */
  LOAD_SW16( CPU1, "053-epr.p2", 0x080000, 0x080000, 0x0e33e8a3),
  LOAD( FIXED, "053-s1.s1", 0x000000, 0x20000, 0x8c2c2d6b),
  LOAD( ROM2, "053-m1.m1", 0x00000, 0x20000, 0x1bd9d04b),
  LOAD( SMP1, "053-v2.v2", 0x000000, 0x200000, 0xa68df485),
  LOAD( SMP1, "053-v4.v4", 0x200000, 0x100000, 0x7bea8f66),
  LOAD_16_8( SPRITES, "053-c1.c1", 0x000000, 0x100000, 0x85eb5bce),
  { "053-c1.c1", 0x100000 , 0x85eb5bce, REGION_SPRITES, 0x400000, LOAD_CONTINUE },
  LOAD_16_8( SPRITES, "053-c2.c2", 0x000001, 0x100000, 0xec93b048),
  { "053-c2.c2", 0x100000 , 0xec93b048, REGION_SPRITES, 0x400001, LOAD_CONTINUE },
  LOAD_16_8( SPRITES, "053-c3.c3", 0x200000, 0x100000, 0x0dd64965),
  LOAD_16_8( SPRITES, "053-c4.c4", 0x200001, 0x100000, 0x9270d954),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( wh1, neogeo, "World Heroes (ALM-005)", ALPHA, 1992, GAME_BEAT);

static struct ROM_INFO rom_kof94[] = /* MVS AND AES VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "055-p1.p1", 0x100000, 0x100000, 0xf10a2042),
  { "055-p1.p1", 0x100000 , 0xf10a2042, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD( FIXED, "055-s1.s1", 0x000000, 0x20000, 0x825976c1),
  LOAD( ROM2, "055-m1.m1", 0x00000, 0x20000, 0xf6e77cf5),
  LOAD( SMP1, "055-v1.v1", 0x000000, 0x200000, 0x8889596d),
  LOAD( SMP1, "055-v2.v2", 0x200000, 0x200000, 0x25022b27),
  LOAD( SMP1, "055-v3.v3", 0x400000, 0x200000, 0x83cf32c0),
  LOAD_16_8( SPRITES, "055-c1.c1", 0x000000, 0x200000, 0xb96ef460),
  LOAD_16_8( SPRITES, "055-c2.c2", 0x000001, 0x200000, 0x15e096a7),
  LOAD_16_8( SPRITES, "055-c3.c3", 0x400000, 0x200000, 0x54f66254),
  LOAD_16_8( SPRITES, "055-c4.c4", 0x400001, 0x200000, 0x0b01765f),
  LOAD_16_8( SPRITES, "055-c5.c5", 0x800000, 0x200000, 0xee759363),
  LOAD_16_8( SPRITES, "055-c6.c6", 0x800001, 0x200000, 0x498da52c),
  LOAD_16_8( SPRITES, "055-c7.c7", 0xc00000, 0x200000, 0x62f66888),
  LOAD_16_8( SPRITES, "055-c8.c8", 0xc00001, 0x200000, 0xfe0a235d),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kof94, neogeo, "The King of Fighters '94 (NGM-055)(NGH-055)", SNK, 1994, GAME_BEAT);

static struct ROM_INFO rom_aof2[] = /* MVS VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "056-p1.p1", 0x000000, 0x100000, 0xa3b1d021),
  LOAD( FIXED, "056-s1.s1", 0x000000, 0x20000, 0x8b02638e),
  LOAD( ROM2, "056-m1.m1", 0x00000, 0x20000, 0xf27e9d52),
  LOAD( SMP1, "056-v1.v1", 0x000000, 0x200000, 0x4628fde0),
  LOAD( SMP1, "056-v2.v2", 0x200000, 0x200000, 0xb710e2f2),
  LOAD( SMP1, "056-v3.v3", 0x400000, 0x100000, 0xd168c301),
	/* Different layout with 4xC (32mbit) also exists; chip labels are 056-C13, 056-C24, 056-C57 and 056-C68 */
  LOAD_16_8( SPRITES, "056-c1.c1", 0x000000, 0x200000, 0x17b9cbd2),
  LOAD_16_8( SPRITES, "056-c2.c2", 0x000001, 0x200000, 0x5fd76b67),
  LOAD_16_8( SPRITES, "056-c3.c3", 0x400000, 0x200000, 0xd2c88768),
  LOAD_16_8( SPRITES, "056-c4.c4", 0x400001, 0x200000, 0xdb39b883),
  LOAD_16_8( SPRITES, "056-c5.c5", 0x800000, 0x200000, 0xc3074137),
  LOAD_16_8( SPRITES, "056-c6.c6", 0x800001, 0x200000, 0x31de68d3),
  LOAD_16_8( SPRITES, "056-c7.c7", 0xc00000, 0x200000, 0x3f36df57),
  LOAD_16_8( SPRITES, "056-c8.c8", 0xc00001, 0x200000, 0xe546d7a8),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( aof2, neogeo, "Art of Fighting 2 / Ryuuko no Ken 2 (NGM-056)", SNK, 1994, GAME_BEAT);

static struct ROM_INFO rom_aof2a[] = /* AES VERSION clone of aof2 */
{
	/* the rom below acts as a patch to the program rom in the cart, replacing the first 512kb */
  LOAD_SW16( CPU1, "056-p1.p1", 0x000000, 0x100000, 0xa3b1d021),
	/* P is on eprom, correct chip label unknown */
  LOAD_SW16( CPU1, "056-epr.ep1", 0x000000, 0x80000, 0x75d6301c),
  LOAD_16_8( SPRITES, "056-c1.c1", 0x000000, 0x200000, 0x17b9cbd2),
  LOAD_16_8( SPRITES, "056-c2.c2", 0x000001, 0x200000, 0x5fd76b67),
  LOAD_16_8( SPRITES, "056-c3.c3", 0x400000, 0x200000, 0xd2c88768),
  LOAD_16_8( SPRITES, "056-c4.c4", 0x400001, 0x200000, 0xdb39b883),
  LOAD_16_8( SPRITES, "056-c5.c5", 0x800000, 0x200000, 0xc3074137),
  LOAD_16_8( SPRITES, "056-c6.c6", 0x800001, 0x200000, 0x31de68d3),
  LOAD_16_8( SPRITES, "056-c7.c7", 0xc00000, 0x200000, 0x3f36df57),
  LOAD_16_8( SPRITES, "056-c8.c8", 0xc00001, 0x200000, 0xe546d7a8),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( aof2a, aof2, "Art of Fighting 2 / Ryuuko no Ken 2 (NGH-056)", SNK, 1994, GAME_BEAT);

static struct ROM_INFO rom_wh2[] = /* MVS AND AES VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "057-p1.p1", 0x100000, 0x100000, 0x65a891d9),
  { "057-p1.p1", 0x100000 , 0x65a891d9, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD( FIXED, "057-s1.s1", 0x000000, 0x20000, 0xfcaeb3a4),
  LOAD( ROM2, "057-m1.m1", 0x00000, 0x20000, 0x8fa3bc77),
  LOAD( SMP1, "057-v1.v1", 0x000000, 0x200000, 0x8877e301),
  LOAD( SMP1, "057-v2.v2", 0x200000, 0x200000, 0xc1317ff4),
  LOAD_16_8( SPRITES, "057-c1.c1", 0x000000, 0x200000, 0x21c6bb91),
  LOAD_16_8( SPRITES, "057-c2.c2", 0x000001, 0x200000, 0xa3999925),
  LOAD_16_8( SPRITES, "057-c3.c3", 0x400000, 0x200000, 0xb725a219),
  LOAD_16_8( SPRITES, "057-c4.c4", 0x400001, 0x200000, 0x8d96425e),
  LOAD_16_8( SPRITES, "057-c5.c5", 0x800000, 0x200000, 0xb20354af),
  LOAD_16_8( SPRITES, "057-c6.c6", 0x800001, 0x200000, 0xb13d1de3),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( wh2, neogeo,              "World Heroes 2 (ALM-006)(ALH-006)", ADK, 1993, GAME_BEAT);

static struct ROM_INFO rom_fatfursp[] = /* MVS AND AES VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "058-p1.p1", 0x000000, 0x100000, 0x2f585ba2),
  LOAD_SW16( CPU1, "058-p2.sp2", 0x100000, 0x080000, 0xd7c71a6b),
  LOAD( FIXED, "058-s1.s1", 0x000000, 0x20000, 0x2df03197),
  LOAD( ROM2, "058-m1.m1", 0x00000, 0x20000, 0xccc5186e),
  LOAD( SMP1, "058-v1.v1", 0x000000, 0x200000, 0x55d7ce84),
  LOAD( SMP1, "058-v2.v2", 0x200000, 0x200000, 0xee080b10),
  LOAD( SMP1, "058-v3.v3", 0x400000, 0x100000, 0xf9eb3d4a),
  LOAD_16_8( SPRITES, "058-c1.c1", 0x000000, 0x200000, 0x044ab13c),
  LOAD_16_8( SPRITES, "058-c2.c2", 0x000001, 0x200000, 0x11e6bf96),
  LOAD_16_8( SPRITES, "058-c3.c3", 0x400000, 0x200000, 0x6f7938d5),
  LOAD_16_8( SPRITES, "058-c4.c4", 0x400001, 0x200000, 0x4ad066ff),
  LOAD_16_8( SPRITES, "058-c5.c5", 0x800000, 0x200000, 0x49c5e0bf),
  LOAD_16_8( SPRITES, "058-c6.c6", 0x800001, 0x200000, 0x8ff1f43d),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( fatfursp, neogeo, "Fatal Fury Special / Garou Densetsu Special (set 1)(NGM-058)(NGH-058)", SNK, 1993, GAME_BEAT);

static struct ROM_INFO rom_fatfurspa[] = /* MVS AND AES VERSION clone of fatfursp */
{
  LOAD_SW16( CPU1, "058-p1.p1", 0x000000, 0x100000, 0x2f585ba2),
	/* the rom below acts as a patch to the program rom in the cart, replacing the first 512kb */
  LOAD_SW16( CPU1, "058-p2.sp2", 0x100000, 0x080000, 0xd7c71a6b),
	/* P is on eprom, correct chip label unknown */
  LOAD_SW16( CPU1, "058-epr.ep1", 0x000000, 0x080000, 0x9f0c1e1a),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( fatfurspa, fatfursp, "Fatal Fury Special / Garou Densetsu Special (set 2)(NGM-058)(NGH-058)", SNK, 1993, GAME_BEAT);

static struct ROM_INFO rom_savagere[] = // clone of neogeo
{
  LOAD_SW16( CPU1, "059-p1.p1", 0x100000, 0x100000, 0x01d4e9c0),
  { "059-p1.p1", 0x100000 , 0x01d4e9c0, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD( FIXED, "059-s1.s1", 0x000000, 0x20000, 0xe08978ca),
  LOAD( ROM2, "059-m1.m1", 0x00000, 0x20000, 0x29992eba),
  LOAD( SMP1, "059-v1.v1", 0x000000, 0x200000, 0x530c50fd),
  LOAD( SMP1, "059-v2.v2", 0x200000, 0x200000, 0xeb6f1cdb),
  LOAD( SMP1, "059-v3.v3", 0x400000, 0x200000, 0x7038c2f9),
  LOAD_16_8( SPRITES, "059-c1.c1", 0x000000, 0x200000, 0x763ba611),
  LOAD_16_8( SPRITES, "059-c2.c2", 0x000001, 0x200000, 0xe05e8ca6),
  LOAD_16_8( SPRITES, "059-c3.c3", 0x400000, 0x200000, 0x3e4eba4b),
  LOAD_16_8( SPRITES, "059-c4.c4", 0x400001, 0x200000, 0x3c2a3808),
  LOAD_16_8( SPRITES, "059-c5.c5", 0x800000, 0x200000, 0x59013f9e),
  LOAD_16_8( SPRITES, "059-c6.c6", 0x800001, 0x200000, 0x1c8d5def),
  LOAD_16_8( SPRITES, "059-c7.c7", 0xc00000, 0x200000, 0xc88f7035),
  LOAD_16_8( SPRITES, "059-c8.c8", 0xc00001, 0x200000, 0x484ce3ba),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( savagere, neogeo, "Savage Reign / Fu'un Mokushiroku - kakutou sousei", SNK, 1995, GAME_BEAT);

static struct ROM_INFO rom_fightfev[] = /* MVS ONLY RELEASE clone of neogeo */
{
  LOAD_SW16( CPU1, "060-p1.p1", 0x0000000, 0x100000, 0x2a104b50),
  LOAD( FIXED, "060-s1.s1", 0x000000, 0x20000, 0x7f012104),
  LOAD( ROM2, "060-m1.m1", 0x00000, 0x20000, 0x0b7c4e65),
  LOAD( SMP1, "060-v1.v1", 0x000000, 0x200000, 0xf417c215),
  LOAD( SMP1, "060-v2.v2", 0x200000, 0x100000, 0xefcff7cf),
  LOAD_16_8( SPRITES, "060-c1.c1", 0x0000000, 0x200000, 0x8908fff9),
  LOAD_16_8( SPRITES, "060-c2.c2", 0x0000001, 0x200000, 0xc6649492),
  LOAD_16_8( SPRITES, "060-c3.c3", 0x0400000, 0x200000, 0x0956b437),
  LOAD_16_8( SPRITES, "060-c4.c4", 0x0400001, 0x200000, 0x026f3b62),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( fightfev, neogeo, "Fight Fever (set 1)", VICCOM, 1994, GAME_BEAT);

static struct ROM_INFO rom_ssideki2[] = /* MVS AND AES VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "061-p1.p1", 0x000000, 0x100000, 0x5969e0dc),
  LOAD( FIXED, "061-s1.s1", 0x000000, 0x20000, 0x226d1b68),
  LOAD( ROM2, "061-m1.m1", 0x00000, 0x20000, 0x156f6951),
  LOAD( SMP1, "061-v1.v1", 0x000000, 0x200000, 0xf081c8d3),
  LOAD( SMP1, "061-v2.v2", 0x200000, 0x200000, 0x7cd63302),
	/* Different layout with 8xC (8 mbit) also exists; naming sheme 061-Cx */
  LOAD_16_8( SPRITES, "061-c1-16.c1", 0x000000, 0x200000, 0xa626474f),
  LOAD_16_8( SPRITES, "061-c2-16.c2", 0x000001, 0x200000, 0xc3be42ae),
  LOAD_16_8( SPRITES, "061-c3-16.c3", 0x400000, 0x200000, 0x2a7b98b9),
  LOAD_16_8( SPRITES, "061-c4-16.c4", 0x400001, 0x200000, 0xc0be9a1f),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( ssideki2, neogeo, "Super Sidekicks 2 - The World Championship / Tokuten Ou 2 - real fight football (NGM-061)(NGH-061)", SNK, 1994, GAME_SPORTS);

static struct ROM_INFO rom_spinmast[] =
{
  LOAD_SW16( CPU1, "062-p1.p1", 0x000000, 0x100000, 0x37aba1aa),
  LOAD_SW16( CPU1, "062-p2.sp2", 0x100000, 0x100000, 0xf025ab77),
  LOAD( FIXED, "062-s1.s1", 0x000000, 0x20000, 0x289e2bbe),
  LOAD( ROM2, "062-m1.m1", 0x00000, 0x20000, 0x76108b2f),
  LOAD( SMP1, "062-v1.v1", 0x000000, 0x100000, 0xcc281aef),
  LOAD_16_8( SPRITES, "062-c1.c1", 0x000000, 0x100000, 0xa9375aa2),
  LOAD_16_8( SPRITES, "062-c2.c2", 0x000001, 0x100000, 0x0e73b758),
  LOAD_16_8( SPRITES, "062-c3.c3", 0x200000, 0x100000, 0xdf51e465),
  LOAD_16_8( SPRITES, "062-c4.c4", 0x200001, 0x100000, 0x38517e90),
  LOAD_16_8( SPRITES, "062-c5.c5", 0x400000, 0x100000, 0x7babd692),
  LOAD_16_8( SPRITES, "062-c6.c6", 0x400001, 0x100000, 0xcde5ade5),
  LOAD_16_8( SPRITES, "062-c7.c7", 0x600000, 0x100000, 0xbb2fd7c0),
  LOAD_16_8( SPRITES, "062-c8.c8", 0x600001, 0x100000, 0x8d7be933),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( spinmast, neogeo, "Spin Master / Miracle Adventure", DECO, 1993, GAME_BEAT);

static struct ROM_INFO rom_samsho2[] = /* MVS AND AES VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "063-p1.p1", 0x100000, 0x100000, 0x22368892),
  { "063-p1.p1", 0x100000 , 0x22368892, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD( FIXED, "063-s1.s1", 0x000000, 0x20000, 0x64a5cd66),
  LOAD( ROM2, "063-m1.m1", 0x00000, 0x20000, 0x56675098),
  LOAD( SMP1, "063-v1.v1", 0x000000, 0x200000, 0x37703f91),
  LOAD( SMP1, "063-v2.v2", 0x200000, 0x200000, 0x0142bde8),
  LOAD( SMP1, "063-v3.v3", 0x400000, 0x200000, 0xd07fa5ca),
  LOAD( SMP1, "063-v4.v4", 0x600000, 0x100000, 0x24aab4bb),
  LOAD_16_8( SPRITES, "063-c1.c1", 0x000000, 0x200000, 0x86cd307c),
  LOAD_16_8( SPRITES, "063-c2.c2", 0x000001, 0x200000, 0xcdfcc4ca),
  LOAD_16_8( SPRITES, "063-c3.c3", 0x400000, 0x200000, 0x7a63ccc7),
  LOAD_16_8( SPRITES, "063-c4.c4", 0x400001, 0x200000, 0x751025ce),
  LOAD_16_8( SPRITES, "063-c5.c5", 0x800000, 0x200000, 0x20d3a475),
  LOAD_16_8( SPRITES, "063-c6.c6", 0x800001, 0x200000, 0xae4c0a88),
  LOAD_16_8( SPRITES, "063-c7.c7", 0xc00000, 0x200000, 0x2df3cbcf),
  LOAD_16_8( SPRITES, "063-c8.c8", 0xc00001, 0x200000, 0x1ffc6dfa),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( samsho2, neogeo, "Samurai Shodown II / Shin Samurai Spirits - Haohmaru jigokuhen (NGM-063)(NGH-063)", SNK, 1994, GAME_BEAT);

static struct ROM_INFO rom_samsho2pe[] = /* samsho2pe, from finalburnneo git, clone of samsho2 of course */
{
    LOAD_SW16( CPU1, "063-p1pe.p1",	0, 0x100000, 0x03bfaaaf ),
    LOAD_SW16( CPU1, "063-p2pe.sp2",	0x100000, 0x100000, 0xa3a39ea4 ),
    LOAD_SW16( CPU1, "063-p3pe.p3",	0x200000, 0x020000, 0x82ce7ad7 ),
    ROM_END
};

CLNEI( samsho2pe, samsho2, "Samurai Shodown II / Shin Samurai Spirits - Haohmaru Jigokuhen (Perfect V. 2.2, Hack)", HACK, 2024, GAME_BEAT);

static struct ROM_INFO rom_samsho2k[] = /* KOREAN VERSION clone of samsho2 */
{
  LOAD_SW16( CPU1, "063-p1-kan.p1", 0x100000, 0x100000, 0x147cc6d7),
	// the roms below apply as patch over the main program (I haven't checked what they change, the game boots as the Korean version even with just the above program)
  { "063-p1-kan.p1", 0x100000 , 0x147cc6d7, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD_SW16( CPU1, "063-ep1-kan.ep1", 0x000000, 0x080000, 0xfa32e2d8),
  LOAD_SW16( CPU1, "063-ep2-kan.ep2", 0x080000, 0x080000, 0x70b1a4d9),
  LOAD( FIXED, "063-s1-kan.s1", 0x000000, 0x20000, 0xff08f80b),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( samsho2k, samsho2, "Saulabi Spirits / Jin Saulabi Tu Hon (Korean release of Samurai Shodown II)", SNK, 1994, GAME_BEAT);

static struct ROM_INFO rom_wjammers[] =
{
  LOAD_SW16( CPU1, "065-p1.p1", 0x000000, 0x100000, 0x6692c140),
  LOAD( FIXED, "065-s1.s1", 0x000000, 0x20000, 0x074b5723),
  LOAD( ROM2, "065-m1.m1", 0x00000, 0x20000, 0x52c23cfc),
  LOAD( SMP1, "065-v1.v1", 0x000000, 0x100000, 0xce8b3698),
  LOAD( SMP1, "065-v2.v2", 0x100000, 0x100000, 0x659f9b96),
  LOAD( SMP1, "065-v3.v3", 0x200000, 0x100000, 0x39f73061),
  LOAD( SMP1, "065-v4.v4", 0x300000, 0x100000, 0x5dee7963),
  LOAD_16_8( SPRITES, "065-c1.c1", 0x000000, 0x100000, 0xc7650204),
  LOAD_16_8( SPRITES, "065-c2.c2", 0x000001, 0x100000, 0xd9f3e71d),
  LOAD_16_8( SPRITES, "065-c3.c3", 0x200000, 0x100000, 0x40986386),
  LOAD_16_8( SPRITES, "065-c4.c4", 0x200001, 0x100000, 0x715e15ff),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_karnovr[] =
{
  LOAD_SW16( CPU1, "066-p1.p1", 0x000000, 0x100000, 0x8c86fd22),
  LOAD( FIXED, "066-s1.s1", 0x000000, 0x20000, 0xbae5d5e5),
  LOAD( ROM2, "066-m1.m1", 0x00000, 0x20000, 0x030beae4),
  LOAD( SMP1, "066-v1.v1", 0x000000, 0x200000, 0x0b7ea37a),
  LOAD_16_8( SPRITES, "066-c1.c1", 0x000000, 0x200000, 0x09dfe061),
  LOAD_16_8( SPRITES, "066-c2.c2", 0x000001, 0x200000, 0xe0f6682a),
  LOAD_16_8( SPRITES, "066-c3.c3", 0x400000, 0x200000, 0xa673b4f7),
  LOAD_16_8( SPRITES, "066-c4.c4", 0x400001, 0x200000, 0xcb3dc5f4),
  LOAD_16_8( SPRITES, "066-c5.c5", 0x800000, 0x200000, 0x9a28785d),
  LOAD_16_8( SPRITES, "066-c6.c6", 0x800001, 0x200000, 0xc15c01ed),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( wjammers, neogeo, "Windjammers / Flying Power Disc", DECO, 1994, GAME_SPORTS);

CLNEI( karnovr, neogeo, "Karnov's Revenge / Fighter's History Dynamite", DECO, 1994, GAME_BEAT);

static struct ROM_INFO rom_gururin[] = /* MVS ONLY RELEASE */
{
  LOAD_SW16( CPU1, "067-p1.p1", 0x000000, 0x80000, 0x4cea8a49),
  LOAD( FIXED, "067-s1.s1", 0x000000, 0x20000, 0xb119e1eb),
  LOAD( ROM2, "067-m1.m1", 0x00000, 0x20000, 0x9e3c6328),
  LOAD( SMP1, "067-v1.v1", 0x000000, 0x80000, 0xcf23afd0),
  LOAD_16_8( SPRITES, "067-c1.c1", 0x000000, 0x200000, 0x35866126),
  LOAD_16_8( SPRITES, "067-c2.c2", 0x000001, 0x200000, 0x9db64084),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( gururin, neogeo, "Gururin", FACE, 1994, GAME_PUZZLE);

static struct ROM_INFO rom_pspikes2[] = /* MVS ONLY RELEASE clone of neogeo */
{
  LOAD_SW16( CPU1, "068-pg1.p1", 0x000000, 0x100000, 0x105a408f),
  LOAD( FIXED, "068-sg1.s1", 0x000000, 0x20000, 0x18082299),
  LOAD( ROM2, "068-mg1.m1", 0x00000, 0x20000, 0xb1c7911e),
  LOAD( SMP1, "068-v1.v1", 0x000000, 0x100000, 0x2ced86df),
  LOAD( SMP1, "068-v2.v2", 0x100000, 0x100000, 0x970851ab),
  LOAD( SMP1, "068-v3.v3", 0x200000, 0x100000, 0x81ff05aa),
  LOAD_16_8( SPRITES, "068-c1.c1", 0x000000, 0x100000, 0x7f250f76),
  LOAD_16_8( SPRITES, "068-c2.c2", 0x000001, 0x100000, 0x20912873),
  LOAD_16_8( SPRITES, "068-c3.c3", 0x200000, 0x100000, 0x4b641ba1),
  LOAD_16_8( SPRITES, "068-c4.c4", 0x200001, 0x100000, 0x35072596),
  LOAD_16_8( SPRITES, "068-c5.c5", 0x400000, 0x100000, 0x151dd624),
  LOAD_16_8( SPRITES, "068-c6.c6", 0x400001, 0x100000, 0xa6722604),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( pspikes2, neogeo, "Power Spikes II (NGM-068)", VIDEOSYSTEM, 1994, GAME_SPORTS);

static struct ROM_INFO rom_fatfury3[] = /* MVS AND AES VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "069-p1.p1", 0x000000, 0x100000, 0xa8bcfbbc),
  LOAD_SW16( CPU1, "069-sp2.sp2", 0x100000, 0x200000, 0xdbe963ed),
  LOAD( FIXED, "069-s1.s1", 0x000000, 0x20000, 0x0b33a800),
  LOAD( ROM2, "069-m1.m1", 0x00000, 0x20000, 0xfce72926),
  LOAD( SMP1, "069-v1.v1", 0x000000, 0x400000, 0x2bdbd4db),
  LOAD( SMP1, "069-v2.v2", 0x400000, 0x400000, 0xa698a487),
  LOAD( SMP1, "069-v3.v3", 0x800000, 0x200000, 0x581c5304),
  LOAD_16_8( SPRITES, "069-c1.c1", 0x0000000, 0x400000, 0xe302f93c),
  LOAD_16_8( SPRITES, "069-c2.c2", 0x0000001, 0x400000, 0x1053a455),
  LOAD_16_8( SPRITES, "069-c3.c3", 0x0800000, 0x400000, 0x1c0fde2f),
  LOAD_16_8( SPRITES, "069-c4.c4", 0x0800001, 0x400000, 0xa25fc3d0),
  LOAD_16_8( SPRITES, "069-c5.c5", 0x1000000, 0x200000, 0xb3ec6fa6),
  LOAD_16_8( SPRITES, "069-c6.c6", 0x1000001, 0x200000, 0x69210441),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( fatfury3, neogeo, "Fatal Fury 3 - Road to the Final Victory / Garou Densetsu 3 - haruka-naru tatakai (NGM-069)(NGH-069)", SNK, 1995, GAME_BEAT);

static struct ROM_INFO rom_zupapa[] = /* Original Version - Encrypted GFX */ /* MVS ONLY RELEASE */
{
  LOAD_SW16( CPU1, "070-p1.p1", 0x000000, 0x100000, 0x5a96203e),
	/* The Encrypted Boards do _not_ have an s1 rom, data for it comes from the Cx ROMs */
  // Dummy region_fixed, overwritten by the decrypt functions but the size is
  // important
  LOAD( FIXED, "sfix.sfix", 0x000000, 0x20000, 0xc2ea0cfd),
/* Ignored : 	ROM_FILL( 0x000000, 0x20000, 0 ) */
  LOAD( ROM2, "070-epr.m1", 0x00000, 0x20000, 0x5a3b3191),
	/* M1 on eprom, correct chip label unknown */
  LOAD( SMP1, "070-v1.v1", 0x000000, 0x200000, 0xd3a7e1ff),
	/* Encrypted */
  LOAD_16_8( SPRITES, "070-c1.c1", 0x0000000, 0x800000, 0xf8ad02d8),
  LOAD_16_8( SPRITES, "070-c2.c2", 0x0000001, 0x800000, 0x70156dde),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( zupapa, neogeo, "Zupapa!" , SNK, 2001, GAME_PLATFORM);

static struct ROM_INFO rom_panicbom[] = /* MVS ONLY RELEASE */
{
  LOAD_SW16( CPU1, "073-p1.p1", 0x000000, 0x080000, 0xadc356ad),
  LOAD( FIXED, "073-s1.s1", 0x000000, 0x20000, 0xb876de7e),
  LOAD( ROM2, "073-m1.m1", 0x00000, 0x20000, 0x3cdf5d88),
  LOAD( SMP1, "073-v1.v1", 0x000000, 0x200000, 0x7fc86d2f),
  LOAD( SMP1, "073-v2.v2", 0x200000, 0x100000, 0x082adfc7),
  LOAD_16_8( SPRITES, "073-c1.c1", 0x000000, 0x100000, 0x8582e1b5),
  LOAD_16_8( SPRITES, "073-c2.c2", 0x000001, 0x100000, 0xe15a093b),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( panicbom, neogeo, "Panic Bomber", EIGHTING, 1994, GAME_PUZZLE);

static struct ROM_INFO rom_aodk[] = /* MVS AND AES VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "074-p1.p1", 0x100000, 0x100000, 0x62369553),
  { "074-p1.p1", 0x100000 , 0x62369553, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD( FIXED, "074-s1.s1", 0x000000, 0x20000, 0x96148d2b),
  LOAD( ROM2, "074-m1.m1", 0x00000, 0x20000, 0x5a52a9d1),
  LOAD( SMP1, "074-v1.v1", 0x000000, 0x200000, 0x7675b8fa),
  LOAD( SMP1, "074-v2.v2", 0x200000, 0x200000, 0xa9da86e9),
  LOAD_16_8( SPRITES, "074-c1.c1", 0x000000, 0x200000, 0xa0b39344),
  LOAD_16_8( SPRITES, "074-c2.c2", 0x000001, 0x200000, 0x203f6074),
  LOAD_16_8( SPRITES, "074-c3.c3", 0x400000, 0x200000, 0x7fff4d41),
  LOAD_16_8( SPRITES, "074-c4.c4", 0x400001, 0x200000, 0x48db3e0a),
  LOAD_16_8( SPRITES, "074-c5.c5", 0x800000, 0x200000, 0xc74c5e51),
  LOAD_16_8( SPRITES, "074-c6.c6", 0x800001, 0x200000, 0x73e8e7e0),
  LOAD_16_8( SPRITES, "074-c7.c7", 0xc00000, 0x200000, 0xac7daa01),
  LOAD_16_8( SPRITES, "074-c8.c8", 0xc00001, 0x200000, 0x14e7ad71),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( aodk, neogeo,        "Aggressors of Dark Kombat / Tsuukai GANGAN Koushinkyoku (ADM-008)(ADH-008)", ADK, 1994, GAME_BEAT);

static struct ROM_INFO rom_sonicwi2[] =
{
  LOAD_SW16( CPU1, "075-p1.p1", 0x100000, 0x100000, 0x92871738),
  { "075-p1.p1", 0x100000 , 0x92871738, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD( FIXED, "075-s1.s1", 0x000000, 0x20000, 0xc9eec367),
  LOAD( ROM2, "075-m1.m1", 0x00000, 0x20000, 0xbb828df1),
  LOAD( SMP1, "075-v1.v1", 0x000000, 0x200000, 0x7577e949),
  LOAD( SMP1, "075-v2.v2", 0x200000, 0x100000, 0x021760cd),
  LOAD_16_8( SPRITES, "075-c1.c1", 0x000000, 0x200000, 0x3278e73e),
  LOAD_16_8( SPRITES, "075-c2.c2", 0x000001, 0x200000, 0xfe6355d6),
  LOAD_16_8( SPRITES, "075-c3.c3", 0x400000, 0x200000, 0xc1b438f1),
  LOAD_16_8( SPRITES, "075-c4.c4", 0x400001, 0x200000, 0x1f777206),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( sonicwi2, neogeo, "Aero Fighters 2 / Sonic Wings 2", VIDEOSYSTEM, 1994, GAME_SHOOT);

static struct ROM_INFO rom_zedblade[] = /* MVS ONLY RELEASE */
{
  LOAD_SW16( CPU1, "076-p1.p1", 0x000000, 0x080000, 0xd7c1effd),
  LOAD( FIXED, "076-s1.s1", 0x000000, 0x20000, 0xf4c25dd5),
  LOAD( ROM2, "076-m1.m1", 0x00000, 0x20000, 0x7b5f3d0a),
  LOAD( SMP1, "076-v1.v1", 0x000000, 0x200000, 0x1a21d90c),
  LOAD( SMP1, "076-v2.v2", 0x200000, 0x200000, 0xb61686c3),
  LOAD( SMP1, "076-v3.v3", 0x400000, 0x100000, 0xb90658fa),
  LOAD_16_8( SPRITES, "076-c1.c1", 0x000000, 0x200000, 0x4d9cb038),
  LOAD_16_8( SPRITES, "076-c2.c2", 0x000001, 0x200000, 0x09233884),
  LOAD_16_8( SPRITES, "076-c3.c3", 0x400000, 0x200000, 0xd06431e3),
  LOAD_16_8( SPRITES, "076-c4.c4", 0x400001, 0x200000, 0x4b1c089b),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( zedblade, neogeo, "Zed Blade / Operation Ragnarok", NMK, 1994, GAME_SHOOT);

static struct ROM_INFO rom_galaxyfg[] =
{
  LOAD_SW16( CPU1, "078-p1.p1", 0x100000, 0x100000, 0x45906309),
  { "078-p1.p1", 0x100000 , 0x45906309, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD( FIXED, "078-s1.s1", 0x000000, 0x20000, 0x72f8923e),
  LOAD( ROM2, "078-m1.m1", 0x00000, 0x20000, 0x8e9e3b10),
  LOAD( SMP1, "078-v1.v1", 0x000000, 0x200000, 0xe3b735ac),
  LOAD( SMP1, "078-v2.v2", 0x200000, 0x200000, 0x6a8e78c2),
  LOAD( SMP1, "078-v3.v3", 0x400000, 0x100000, 0x70bca656),
  LOAD_16_8( SPRITES, "078-c1.c1", 0x000000, 0x200000, 0xc890c7c0),
  LOAD_16_8( SPRITES, "078-c2.c2", 0x000001, 0x200000, 0xb6d25419),
  LOAD_16_8( SPRITES, "078-c3.c3", 0x400000, 0x200000, 0x9d87e761),
  LOAD_16_8( SPRITES, "078-c4.c4", 0x400001, 0x200000, 0x765d7cb8),
  LOAD_16_8( SPRITES, "078-c5.c5", 0x800000, 0x200000, 0xe6b77e6a),
  LOAD_16_8( SPRITES, "078-c6.c6", 0x800001, 0x200000, 0xd779a181),
  LOAD_16_8( SPRITES, "078-c7.c7", 0xc00000, 0x100000, 0x4f27d580),
  LOAD_16_8( SPRITES, "078-c8.c8", 0xc00001, 0x100000, 0x0a7cc0d8),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( galaxyfg, neogeo, "Galaxy Fight - Universal Warriors", SUNSOFT, 1995, GAME_BEAT);

static struct ROM_INFO rom_strhoop[] = /* MVS AND AES VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "079-p1.p1", 0x000000, 0x100000, 0x5e78328e),
  LOAD( FIXED, "079-s1.s1", 0x000000, 0x20000, 0x3ac06665),
  LOAD( ROM2, "079-m1.m1", 0x00000, 0x20000, 0xbee3455a),
  LOAD( SMP1, "079-v1.v1", 0x000000, 0x200000, 0x718a2400),
	/* AES 079-v2 is only 4 mbit (TC534200), data is the same */
  LOAD( SMP1, "079-v2.v2", 0x200000, 0x100000, 0x720774eb),
  LOAD_16_8( SPRITES, "079-c1.c1", 0x000000, 0x200000, 0x0581c72a),
  LOAD_16_8( SPRITES, "079-c2.c2", 0x000001, 0x200000, 0x5b9b8fb6),
  LOAD_16_8( SPRITES, "079-c3.c3", 0x400000, 0x200000, 0xcd65bb62),
  LOAD_16_8( SPRITES, "079-c4.c4", 0x400001, 0x200000, 0xa4c90213),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( strhoop, neogeo, "Street Hoop / Street Slam / Dunk Dream (DEM-004) (DEH-004)", DECO, 1994, GAME_SPORTS);

static struct ROM_INFO rom_ssideki3[] = // clone of neogeo
{
  LOAD_SW16( CPU1, "081-p1.p1", 0x100000, 0x100000, 0x6bc27a3d),
  { "081-p1.p1", 0x100000 , 0x6bc27a3d, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD( FIXED, "081-s1.s1", 0x000000, 0x20000, 0x7626da34),
  LOAD( ROM2, "081-m1.m1", 0x00000, 0x20000, 0x82fcd863),
  LOAD( SMP1, "081-v1.v1", 0x000000, 0x200000, 0x201fa1e1),
  LOAD( SMP1, "081-v2.v2", 0x200000, 0x200000, 0xacf29d96),
  LOAD( SMP1, "081-v3.v3", 0x400000, 0x200000, 0xe524e415),
  LOAD_16_8( SPRITES, "081-c1.c1", 0x000000, 0x200000, 0x1fb68ebe),
  LOAD_16_8( SPRITES, "081-c2.c2", 0x000001, 0x200000, 0xb28d928f),
  LOAD_16_8( SPRITES, "081-c3.c3", 0x400000, 0x200000, 0x3b2572e8),
  LOAD_16_8( SPRITES, "081-c4.c4", 0x400001, 0x200000, 0x47d26a7c),
  LOAD_16_8( SPRITES, "081-c5.c5", 0x800000, 0x200000, 0x17d42f0d),
  LOAD_16_8( SPRITES, "081-c6.c6", 0x800001, 0x200000, 0x6b53fb75),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( ssideki3, neogeo, "Super Sidekicks 3 - The Next Glory / Tokuten Ou 3 - eikou e no michi", SNK, 1995, GAME_SPORTS);

static struct ROM_INFO rom_doubledr[] = // clone of neogeo
{
  LOAD_SW16( CPU1, "082-p1.p1", 0x100000, 0x100000, 0x34ab832a),
  { "082-p1.p1", 0x100000 , 0x34ab832a, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD( FIXED, "082-s1.s1", 0x000000, 0x20000, 0xbef995c5),
  LOAD( ROM2, "082-m1.m1", 0x00000, 0x20000, 0x10b144de),
  LOAD( SMP1, "082-v1.v1", 0x000000, 0x200000, 0xcc1128e4),
  LOAD( SMP1, "082-v2.v2", 0x200000, 0x200000, 0xc3ff5554),
  LOAD_16_8( SPRITES, "082-c1.c1", 0x000000, 0x200000, 0xb478c725),
  LOAD_16_8( SPRITES, "082-c2.c2", 0x000001, 0x200000, 0x2857da32),
  LOAD_16_8( SPRITES, "082-c3.c3", 0x400000, 0x200000, 0x8b0d378e),
  LOAD_16_8( SPRITES, "082-c4.c4", 0x400001, 0x200000, 0xc7d2f596),
  LOAD_16_8( SPRITES, "082-c5.c5", 0x800000, 0x200000, 0xec87bff6),
  LOAD_16_8( SPRITES, "082-c6.c6", 0x800001, 0x200000, 0x844a8a11),
  LOAD_16_8( SPRITES, "082-c7.c7", 0xc00000, 0x100000, 0x727c4d02),
  LOAD_16_8( SPRITES, "082-c8.c8", 0xc00001, 0x100000, 0x69a5fa37),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( doubledr, neogeo, "Double Dragon (Neo-Geo)", TECHNOS, 1995, GAME_BEAT);

static struct ROM_INFO rom_pbobblen[] = /* MVS ONLY RELEASE clone of neogeo */
{
  LOAD_SW16( CPU1, "d96-07.ep1", 0x000000, 0x080000, 0x6102ca14),
  LOAD( FIXED, "d96-04.s1", 0x000000, 0x20000, 0x9caae538),
  LOAD( ROM2, "d96-06.m1", 0x00000, 0x20000, 0xf424368a),
  LOAD( SMP1, "068-v1.v1", 0x000000, 0x100000, 0x2ced86df),
  LOAD( SMP1, "068-v2.v2", 0x100000, 0x100000, 0x970851ab),
  LOAD( SMP1, "d96-01.v3", 0x200000, 0x100000, 0x0840cbc4),
  LOAD( SMP1, "d96-05.v4", 0x300000, 0x080000, 0x0a548948),
  LOAD_16_8( SPRITES, "068-c1.c1", 0x000000, 0x100000, 0x7f250f76),
  LOAD_16_8( SPRITES, "068-c2.c2", 0x000001, 0x100000, 0x20912873),
  LOAD_16_8( SPRITES, "068-c3.c3", 0x200000, 0x100000, 0x4b641ba1),
  LOAD_16_8( SPRITES, "068-c4.c4", 0x200001, 0x100000, 0x35072596),
  LOAD_16_8( SPRITES, "d96-02.c5", 0x400000, 0x80000, 0xe89ad494),
  LOAD_16_8( SPRITES, "d96-03.c6", 0x400001, 0x80000, 0x4b42d7eb),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( pbobblen, neogeo, "Puzzle Bobble / Bust-A-Move (Neo-Geo) (NGM-083)", TAITO, 1994, GAME_PUZZLE);

static struct ROM_INFO rom_kof95[] = /* MVS VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "084-p1.p1", 0x100000, 0x100000, 0x2cba2716),
  { "084-p1.p1", 0x100000 , 0x2cba2716, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD( FIXED, "084-s1.s1", 0x000000, 0x20000, 0xde716f8a),
  LOAD( ROM2, "084-m1.m1", 0x00000, 0x20000, 0x6f2d7429),
  LOAD( SMP1, "084-v1.v1", 0x000000, 0x400000, 0x84861b56),
	/* 600000-7fffff empty */
  LOAD( SMP1, "084-v2.v2", 0x400000, 0x200000, 0xb38a2803),
  LOAD( SMP1, "084-v3.v3", 0x800000, 0x100000, 0xd683a338),
  LOAD_16_8( SPRITES, "084-c1.c1", 0x0000000, 0x400000, 0xfe087e32),
  LOAD_16_8( SPRITES, "084-c2.c2", 0x0000001, 0x400000, 0x07864e09),
  LOAD_16_8( SPRITES, "084-c3.c3", 0x0800000, 0x400000, 0xa4e65d1b),
  LOAD_16_8( SPRITES, "084-c4.c4", 0x0800001, 0x400000, 0xc1ace468),
  LOAD_16_8( SPRITES, "084-c5.c5", 0x1000000, 0x200000, 0x8a2c1edc),
	/* 1400000-17fffff empty */
  LOAD_16_8( SPRITES, "084-c6.c6", 0x1000001, 0x200000, 0xf593ac35),
  LOAD_16_8( SPRITES, "084-c7.c7", 0x1800000, 0x100000, 0x9904025f),
  LOAD_16_8( SPRITES, "084-c8.c8", 0x1800001, 0x100000, 0x78eb0f9b),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kof95, neogeo, "The King of Fighters '95 (NGM-084)", SNK, 1995, GAME_BEAT);

static struct ROM_INFO rom_kof95h[] = /* MVS AND AES VERSION clone of kof95 */
{
  LOAD_SW16( CPU1, "084-pg1.p1", 0x100000, 0x100000, 0x5e54cf95),
  { "084-pg1.p1", 0x100000 , 0x5e54cf95, REGION_CPU1, 0x000000, LOAD_CONTINUE },
	/* 600000-7fffff empty */
	/* 1400000-17fffff empty */
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kof95h, kof95, "The King of Fighters '95 (NGH-084)", SNK, 1995, GAME_BEAT);

static struct ROM_INFO rom_tws96[] = /* MVS ONLY RELEASE */
{
  LOAD_SW16( CPU1, "086-p1.p1", 0x000000, 0x100000, 0x03e20ab6),
  LOAD( FIXED, "086-s1.s1", 0x000000, 0x20000, 0x6f5e2b3a),
  LOAD( ROM2, "086-m1.m1", 0x00000, 0x20000, 0xcb82bc5d),
  LOAD( SMP1, "086-v1.v1", 0x000000, 0x200000, 0x97bf1986),
  LOAD( SMP1, "086-v2.v2", 0x200000, 0x200000, 0xb7eb05df),
  LOAD_16_8( SPRITES, "086-c1.c1", 0x000000, 0x400000, 0x2611bc2a),
  LOAD_16_8( SPRITES, "086-c2.c2", 0x000001, 0x400000, 0x6b0d6827),
  LOAD_16_8( SPRITES, "086-c3.c3", 0x800000, 0x100000, 0x750ddc0c),
  LOAD_16_8( SPRITES, "086-c4.c4", 0x800001, 0x100000, 0x7a6e7d82),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( tws96, neogeo, "Tecmo World Soccer '96", TECMO, 1996, GAME_SPORTS);

static struct ROM_INFO rom_samsho3[] = /* MVS VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "087-epr.ep1", 0x000000, 0x080000, 0x23e09bb8),
  LOAD_SW16( CPU1, "087-epr.ep2", 0x080000, 0x080000, 0x256f5302),
  LOAD_SW16( CPU1, "087-epr.ep3", 0x100000, 0x080000, 0xbf2db5dd),
	/* P's on eprom, correct chip label unknown */
  LOAD_SW16( CPU1, "087-epr.ep4", 0x180000, 0x080000, 0x53e60c58),
  LOAD_SW16( CPU1, "087-p5.p5", 0x200000, 0x100000, 0xe86ca4af),
  LOAD( FIXED, "087-s1.s1", 0x000000, 0x20000, 0x74ec7d9f),
  LOAD( ROM2, "087-m1.m1", 0x00000, 0x20000, 0x8e6440eb),
  LOAD( SMP1, "087-v1.v1", 0x000000, 0x400000, 0x84bdd9a0),
  LOAD( SMP1, "087-v2.v2", 0x400000, 0x200000, 0xac0f261a),
  LOAD_16_8( SPRITES, "087-c1.c1", 0x0000000, 0x400000, 0x07a233bc),
  LOAD_16_8( SPRITES, "087-c2.c2", 0x0000001, 0x400000, 0x7a413592),
  LOAD_16_8( SPRITES, "087-c3.c3", 0x0800000, 0x400000, 0x8b793796),
  LOAD_16_8( SPRITES, "087-c4.c4", 0x0800001, 0x400000, 0x728fbf11),
  LOAD_16_8( SPRITES, "087-c5.c5", 0x1000000, 0x400000, 0x172ab180),
  LOAD_16_8( SPRITES, "087-c6.c6", 0x1000001, 0x400000, 0x002ff8f3),
  LOAD_16_8( SPRITES, "087-c7.c7", 0x1800000, 0x100000, 0xae450e3d),
  LOAD_16_8( SPRITES, "087-c8.c8", 0x1800001, 0x100000, 0xa9e82717),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( samsho3, neogeo, "Samurai Shodown III / Samurai Spirits - Zankurou Musouken (NGM-087)", SNK, 1995, GAME_BEAT);

static struct ROM_INFO rom_samsho3h[] = /* AES VERSION clone of samsho3 */
{
  LOAD_SW16( CPU1, "087-pg1.p1", 0x000000, 0x100000, 0x282a336e),
  LOAD_SW16( CPU1, "087-p2.sp2", 0x100000, 0x200000, 0x9bbe27e0),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_fswords[] = /* KOREAN VERSION clone of samsho3 */
{
  LOAD_SW16( CPU1, "187-p1k.p1", 0x000000, 0x100000, 0xc8e7c075),
  LOAD_SW16( CPU1, "087-p2.sp2", 0x100000, 0x200000, 0x9bbe27e0),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( samsho3h, samsho3, "Samurai Shodown III / Samurai Spirits - Zankurou Musouken (NGH-087)", SNK, 1995, GAME_BEAT);
CLNEI( fswords, samsho3, "Fighters Swords (Korean release of Samurai Shodown III)", SNK, 1995, GAME_BEAT);

static struct ROM_INFO rom_stakwin[] =
{
  LOAD_SW16( CPU1, "088-p1.p1", 0x100000, 0x100000, 0xbd5814f6),
  { "088-p1.p1", 0x100000, 0xbd5814f6, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD( FIXED, "088-s1.s1", 0x000000, 0x20000, 0x073cb208),
  LOAD( ROM2, "088-m1.m1", 0x00000, 0x20000, 0x2fe1f499),
  LOAD( SMP1, "088-v1.v1", 0x000000, 0x200000, 0xb7785023),
  LOAD_16_8( SPRITES, "088-c1.c1", 0x000000, 0x200000, 0x6e733421),
  LOAD_16_8( SPRITES, "088-c2.c2", 0x000001, 0x200000, 0x4d865347),
  LOAD_16_8( SPRITES, "088-c3.c3", 0x400000, 0x200000, 0x8fa5a9eb),
  LOAD_16_8( SPRITES, "088-c4.c4", 0x400001, 0x200000, 0x4604f0dc),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( stakwin, neogeo, "Stakes Winner / Stakes Winner - GI kinzen seiha e no michi", SAURUS, 1995, GAME_SPORTS);

static struct ROM_INFO rom_pulstar[] = // clone of neogeo
{
  LOAD_SW16( CPU1, "089-p1.p1", 0x000000, 0x100000, 0x5e5847a2),
  LOAD_SW16( CPU1, "089-p2.sp2", 0x100000, 0x200000, 0x028b774c),
  LOAD( FIXED, "089-s1.s1", 0x000000, 0x20000, 0xc79fc2c8),
  LOAD( ROM2, "089-m1.m1", 0x00000, 0x20000, 0xff3df7c7),
  LOAD( SMP1, "089-v1.v1", 0x000000, 0x400000, 0x6f726ecb),
  LOAD( SMP1, "089-v2.v2", 0x400000, 0x400000, 0x9d2db551),
  LOAD_16_8( SPRITES, "089-c1.c1", 0x0000000, 0x400000, 0xf4e97332),
  LOAD_16_8( SPRITES, "089-c2.c2", 0x0000001, 0x400000, 0x836d14da),
  LOAD_16_8( SPRITES, "089-c3.c3", 0x0800000, 0x400000, 0x913611c4),
  LOAD_16_8( SPRITES, "089-c4.c4", 0x0800001, 0x400000, 0x44cef0e3),
  LOAD_16_8( SPRITES, "089-c5.c5", 0x1000000, 0x400000, 0x89baa1d7),
  LOAD_16_8( SPRITES, "089-c6.c6", 0x1000001, 0x400000, 0xb2594d56),
  LOAD_16_8( SPRITES, "089-c7.c7", 0x1800000, 0x200000, 0x6a5618ca),
  LOAD_16_8( SPRITES, "089-c8.c8", 0x1800001, 0x200000, 0xa223572d),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( pulstar, neogeo, "Pulstar", AICOM, 1995, GAME_SHOOT);

static struct ROM_INFO rom_whp[] = // clone of neogeo
{
  LOAD_SW16( CPU1, "090-p1.p1", 0x100000, 0x100000, 0xafaa4702),
  { "090-p1.p1", 0x100000 , 0xafaa4702, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD( FIXED, "090-s1.s1", 0x000000, 0x20000, 0x174a880f),
  LOAD( ROM2, "090-m1.m1", 0x00000, 0x20000, 0x28065668),
  LOAD( SMP1, "090-v1.v1", 0x000000, 0x200000, 0x30cf2709),
  LOAD( SMP1, "064-v2.v2", 0x200000, 0x200000, 0xb6527edd),
  LOAD( SMP1, "090-v3.v3", 0x400000, 0x200000, 0x1908a7ce),
  LOAD_16_8( SPRITES, "090-c1.c1", 0x0000000, 0x400000, 0xcd30ed9b),
  LOAD_16_8( SPRITES, "090-c2.c2", 0x0000001, 0x400000, 0x10eed5ee),
  LOAD_16_8( SPRITES, "064-c3.c3", 0x0800000, 0x200000, 0x436d1b31),
	/* 0c00000-0ffffff empty */
  LOAD_16_8( SPRITES, "064-c4.c4", 0x0800001, 0x200000, 0xf9c8dd26),
  LOAD_16_8( SPRITES, "064-c5.c5", 0x1000000, 0x200000, 0x8e34a9f4),
	/* 1400000-17fffff empty */
  LOAD_16_8( SPRITES, "064-c6.c6", 0x1000001, 0x200000, 0xa43e4766),
  LOAD_16_8( SPRITES, "064-c7.c7", 0x1800000, 0x200000, 0x59d97215),
  LOAD_16_8( SPRITES, "064-c8.c8", 0x1800001, 0x200000, 0xfc092367),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( whp, neogeo,        "World Heroes Perfect", ADK, 1995, GAME_BEAT);

static struct ROM_INFO rom_kabukikl[] =
{
  LOAD_SW16( CPU1, "092-p1.p1", 0x100000, 0x100000, 0x28ec9b77),
  { "092-p1.p1", 0x100000 , 0x28ec9b77, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD( FIXED, "092-s1.s1", 0x000000, 0x20000, 0xa3d68ee2),
  LOAD( ROM2, "092-m1.m1", 0x00000, 0x20000, 0x91957ef6),
  LOAD( SMP1, "092-v1.v1", 0x000000, 0x200000, 0x69e90596),
  LOAD( SMP1, "092-v2.v2", 0x200000, 0x200000, 0x7abdb75d),
  LOAD( SMP1, "092-v3.v3", 0x400000, 0x200000, 0xeccc98d3),
  LOAD( SMP1, "092-v4.v4", 0x600000, 0x100000, 0xa7c9c949),
  LOAD_16_8( SPRITES, "092-c1.c1", 0x000000, 0x400000, 0x2a9fab01),
  LOAD_16_8( SPRITES, "092-c2.c2", 0x000001, 0x400000, 0x6d2bac02),
  LOAD_16_8( SPRITES, "092-c3.c3", 0x800000, 0x400000, 0x5da735d6),
  LOAD_16_8( SPRITES, "092-c4.c4", 0x800001, 0x400000, 0xde07f997),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kabukikl, neogeo, "Kabuki Klash - Far East of Eden / Tengai Makyou - Shin Den", HUDSON, 1995, GAME_BEAT);

static struct ROM_INFO rom_neobombe[] = /* MVS ONLY RELEASE */
{
  LOAD_SW16( CPU1, "093-p1.p1", 0x000000, 0x100000, 0xa1a71d0d),
  LOAD( FIXED, "093-s1.s1", 0x000000, 0x20000, 0x4b3fa119),
  LOAD( ROM2, "093-m1.m1", 0x00000, 0x20000, 0xe81e780b),
  LOAD( SMP1, "093-v1.v1", 0x000000, 0x400000, 0x02abd4b0),
  LOAD( SMP1, "093-v2.v2", 0x400000, 0x200000, 0xa92b8b3d),
  LOAD_16_8( SPRITES, "093-c1.c1", 0x000000, 0x400000, 0xd1f328f8),
  LOAD_16_8( SPRITES, "093-c2.c2", 0x000001, 0x400000, 0x82c49540),
  LOAD_16_8( SPRITES, "093-c3.c3", 0x800000, 0x080000, 0xe37578c5),
  LOAD_16_8( SPRITES, "093-c4.c4", 0x800001, 0x080000, 0x59826783),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( neobombe, neogeo, "Neo Bomberman", HUDSON, 1997, GAME_MAZE);

static struct ROM_INFO rom_gowcaizr[] =
{
  LOAD_SW16( CPU1, "094-p1.p1", 0x100000, 0x100000, 0x33019545),
  { "094-p1.p1", 0x100000 , 0x33019545, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD( FIXED, "094-s1.s1", 0x000000, 0x20000, 0x2f8748a2),
  LOAD( ROM2, "094-m1.m1", 0x00000, 0x20000, 0x78c851cb),
  LOAD( SMP1, "094-v1.v1", 0x000000, 0x200000, 0x6c31223c),
  LOAD( SMP1, "094-v2.v2", 0x200000, 0x200000, 0x8edb776c),
  LOAD( SMP1, "094-v3.v3", 0x400000, 0x100000, 0xc63b9285),
  LOAD_16_8( SPRITES, "094-c1.c1", 0x000000, 0x200000, 0x042f6af5),
  LOAD_16_8( SPRITES, "094-c2.c2", 0x000001, 0x200000, 0x0fbcd046),
  LOAD_16_8( SPRITES, "094-c3.c3", 0x400000, 0x200000, 0x58bfbaa1),
  LOAD_16_8( SPRITES, "094-c4.c4", 0x400001, 0x200000, 0x9451ee73),
  LOAD_16_8( SPRITES, "094-c5.c5", 0x800000, 0x200000, 0xff9cf48c),
  LOAD_16_8( SPRITES, "094-c6.c6", 0x800001, 0x200000, 0x31bbd918),
  LOAD_16_8( SPRITES, "094-c7.c7", 0xc00000, 0x200000, 0x2091ec04),
  LOAD_16_8( SPRITES, "094-c8.c8", 0xc00001, 0x200000, 0xd80dd241),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( gowcaizr, neogeo, "Voltage Fighter - Gowcaizer / Choujin Gakuen Gowcaizer", TECHNOS, 1995, GAME_BEAT);

static struct ROM_INFO rom_rbff1[] = /* MVS AND AES VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "095-p1.p1", 0x000000, 0x100000, 0x63b4d8ae),
  LOAD_SW16( CPU1, "095-p2.sp2", 0x100000, 0x200000, 0xcc15826e),
  LOAD( FIXED, "095-s1.s1", 0x000000, 0x20000, 0xb6bf5e08),
  LOAD( ROM2, "095-m1.m1", 0x00000, 0x20000, 0x653492a7),
  LOAD( SMP1, "069-v1.v1", 0x000000, 0x400000, 0x2bdbd4db),
  LOAD( SMP1, "069-v2.v2", 0x400000, 0x400000, 0xa698a487),
  LOAD( SMP1, "095-v3.v3", 0x800000, 0x400000, 0x189d1c6c),
  LOAD_16_8( SPRITES, "069-c1.c1", 0x0000000, 0x400000, 0xe302f93c),
  LOAD_16_8( SPRITES, "069-c2.c2", 0x0000001, 0x400000, 0x1053a455),
  LOAD_16_8( SPRITES, "069-c3.c3", 0x0800000, 0x400000, 0x1c0fde2f),
  LOAD_16_8( SPRITES, "069-c4.c4", 0x0800001, 0x400000, 0xa25fc3d0),
  LOAD_16_8( SPRITES, "095-c5.c5", 0x1000000, 0x400000, 0x8b9b65df),
  LOAD_16_8( SPRITES, "095-c6.c6", 0x1000001, 0x400000, 0x3e164718),
  LOAD_16_8( SPRITES, "095-c7.c7", 0x1800000, 0x200000, 0xca605e12),
  LOAD_16_8( SPRITES, "095-c8.c8", 0x1800001, 0x200000, 0x4e6beb6c),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( rbff1, neogeo, "Real Bout Fatal Fury / Real Bout Garou Densetsu (NGM-095)(NGH-095)", SNK, 1995, GAME_BEAT);

static struct ROM_INFO rom_aof3[] = // clone of neogeo
{
  LOAD_SW16( CPU1, "096-p1.p1", 0x000000, 0x100000, 0x9edb420d),
  LOAD_SW16( CPU1, "096-p2.sp2", 0x100000, 0x200000, 0x4d5a2602),
  LOAD( FIXED, "096-s1.s1", 0x000000, 0x20000, 0xcc7fd344),
  LOAD( ROM2, "096-m1.m1", 0x00000, 0x20000, 0xcb07b659),
  LOAD( SMP1, "096-v1.v1", 0x000000, 0x200000, 0xe2c32074),
  LOAD( SMP1, "096-v2.v2", 0x200000, 0x200000, 0xa290eee7),
  LOAD( SMP1, "096-v3.v3", 0x400000, 0x200000, 0x199d12ea),
  LOAD_16_8( SPRITES, "096-c1.c1", 0x0000000, 0x400000, 0xf17b8d89),
  LOAD_16_8( SPRITES, "096-c2.c2", 0x0000001, 0x400000, 0x3840c508),
  LOAD_16_8( SPRITES, "096-c3.c3", 0x0800000, 0x400000, 0x55f9ee1e),
  LOAD_16_8( SPRITES, "096-c4.c4", 0x0800001, 0x400000, 0x585b7e47),
  LOAD_16_8( SPRITES, "096-c5.c5", 0x1000000, 0x400000, 0xc75a753c),
  LOAD_16_8( SPRITES, "096-c6.c6", 0x1000001, 0x400000, 0x9a9d2f7a),
  LOAD_16_8( SPRITES, "096-c7.c7", 0x1800000, 0x200000, 0x51bd8ab2),
  LOAD_16_8( SPRITES, "096-c8.c8", 0x1800001, 0x200000, 0x9a34f99c),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( aof3, neogeo, "Art of Fighting 3 - The Path of the Warrior / Art of Fighting - Ryuuko no Ken Gaiden", SNK, 1996, GAME_BEAT);

static struct ROM_INFO rom_aof3k[] = /* KOREAN VERSION */
{
  LOAD_SW16( CPU1, "196-p1k.p1", 0x000000, 0x100000, 0xa0780789),
  LOAD_SW16( CPU1, "096-p2.sp2", 0x100000, 0x200000, 0x4d5a2602),
  LOAD( FIXED, "096-s1.s1", 0x000000, 0x20000, 0xcc7fd344),
  LOAD( ROM2, "096-m1.m1", 0x00000, 0x20000, 0xcb07b659),
  LOAD( SMP1, "096-v1.v1", 0x000000, 0x200000, 0xe2c32074),
  LOAD( SMP1, "096-v2.v2", 0x200000, 0x200000, 0xa290eee7),
  LOAD( SMP1, "096-v3.v3", 0x400000, 0x200000, 0x199d12ea),
  LOAD_16_8( SPRITES, "096-c1.c1", 0x0000000, 0x400000, 0xf17b8d89),
  LOAD_16_8( SPRITES, "096-c2.c2", 0x0000001, 0x400000, 0x3840c508),
  LOAD_16_8( SPRITES, "096-c3.c3", 0x0800000, 0x400000, 0x55f9ee1e),
  LOAD_16_8( SPRITES, "096-c4.c4", 0x0800001, 0x400000, 0x585b7e47),
  LOAD_16_8( SPRITES, "096-c5.c5", 0x1000000, 0x400000, 0xc75a753c),
  LOAD_16_8( SPRITES, "096-c6.c6", 0x1000001, 0x400000, 0x9a9d2f7a),
  LOAD_16_8( SPRITES, "096-c7.c7", 0x1800000, 0x200000, 0x51bd8ab2),
  LOAD_16_8( SPRITES, "096-c8.c8", 0x1800001, 0x200000, 0x9a34f99c),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( aof3k, aof3, "Art of Fighting 3 - The Path of the Warrior (Korean release)", SNK, 1996, GAME_BEAT);

static struct ROM_INFO rom_sonicwi3[] =
{
  LOAD_SW16( CPU1, "097-p1.p1", 0x100000, 0x100000, 0x0547121d),
  { "097-p1.p1", 0x100000 , 0x0547121d, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD( FIXED, "097-s1.s1", 0x000000, 0x20000, 0x8dd66743),
  LOAD( ROM2, "097-m1.m1", 0x00000, 0x20000, 0xb20e4291),
  LOAD( SMP1, "097-v1.v1", 0x000000, 0x400000, 0x6f885152),
  LOAD( SMP1, "097-v2.v2", 0x400000, 0x200000, 0x3359e868),
  LOAD_16_8( SPRITES, "097-c1.c1", 0x000000, 0x400000, 0x33d0d589),
  LOAD_16_8( SPRITES, "097-c2.c2", 0x000001, 0x400000, 0x186f8b43),
  LOAD_16_8( SPRITES, "097-c3.c3", 0x800000, 0x200000, 0xc339fff5),
  LOAD_16_8( SPRITES, "097-c4.c4", 0x800001, 0x200000, 0x84a40c6e),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( sonicwi3, neogeo, "Aero Fighters 3 / Sonic Wings 3", VIDEOSYSTEM, 1995, GAME_SHOOT);

static struct ROM_INFO rom_turfmast[] =
{
  LOAD_SW16( CPU1, "200-p1.p1", 0x100000, 0x100000, 0x28c83048),
  { "200-p1.p1", 0x100000, 0x28c83048, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD( FIXED, "200-s1.s1", 0x000000, 0x20000, 0x9a5402b2),
  LOAD( ROM2, "200-m1.m1", 0x00000, 0x20000, 0x9994ac00),
  LOAD( SMP1, "200-v1.v1", 0x000000, 0x200000, 0x00fd48d2),
  LOAD( SMP1, "200-v2.v2", 0x200000, 0x200000, 0x082acb31),
  LOAD( SMP1, "200-v3.v3", 0x400000, 0x200000, 0x7abca053),
  LOAD( SMP1, "200-v4.v4", 0x600000, 0x200000, 0x6c7b4902),
  LOAD_16_8( SPRITES, "200-c1.c1", 0x000000, 0x400000, 0x8e7bf41a),
  LOAD_16_8( SPRITES, "200-c2.c2", 0x000001, 0x400000, 0x5a65a8ce),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( turfmast, neogeo, "Neo Turf Masters / Big Tournament Golf", NAZCA, 1996, GAME_SPORTS);

static struct ROM_INFO rom_mslug[] =
{
  LOAD_SW16( CPU1, "201-p1.p1", 0x100000, 0x100000, 0x08d8daa5),
  { "201-p1.p1", 0x100000 , 0x08d8daa5, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD( FIXED, "201-s1.s1", 0x000000, 0x20000, 0x2f55958d),
  LOAD( ROM2, "201-m1.m1", 0x00000, 0x20000, 0xc28b3253),
  LOAD( SMP1, "201-v1.v1", 0x000000, 0x400000, 0x23d22ed1),
  LOAD( SMP1, "201-v2.v2", 0x400000, 0x400000, 0x472cf9db),
  LOAD_16_8( SPRITES, "201-c1.c1", 0x000000, 0x400000, 0x72813676),
  LOAD_16_8( SPRITES, "201-c2.c2", 0x000001, 0x400000, 0x96f62574),
  LOAD_16_8( SPRITES, "201-c3.c3", 0x800000, 0x400000, 0x5121456a),
  LOAD_16_8( SPRITES, "201-c4.c4", 0x800001, 0x400000, 0xf4ad59a3),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( mslug, neogeo, "Metal Slug - Super Vehicle-001", NAZCA, 1996, GAME_SHOOT);

static struct ROM_INFO rom_puzzledp[] = /* MVS ONLY RELEASE clone of neogeo */
{
  LOAD_SW16( CPU1, "202-p1.p1", 0x000000, 0x080000, 0x2b61415b),
  LOAD( FIXED, "202-s1.s1", 0x000000, 0x20000, 0xcd19264f),
  LOAD( ROM2, "202-m1.m1", 0x00000, 0x20000, 0x9c0291ea),
  LOAD( SMP1, "202-v1.v1", 0x000000, 0x080000, 0xdebeb8fb),
  LOAD_16_8( SPRITES, "202-c1.c1", 0x000000, 0x100000, 0xcc0095ef),
  LOAD_16_8( SPRITES, "202-c2.c2", 0x000001, 0x100000, 0x42371307),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( puzzledp, neogeo, "Puzzle De Pon!", TAITO, 1995, GAME_PUZZLE);

static struct ROM_INFO rom_neomrdo[] = /* MVS ONLY RELEASE */
{
  LOAD_SW16( CPU1, "207-p1.p1", 0x000000, 0x100000, 0x334ea51e),
  LOAD( FIXED, "207-s1.s1", 0x000000, 0x20000, 0x6aebafce),
  LOAD( ROM2, "207-m1.m1", 0x00000, 0x20000, 0xb5b74a95),
  LOAD( SMP1, "207-v1.v1", 0x000000, 0x200000, 0x4143c052),
  LOAD_16_8( SPRITES, "207-c1.c1", 0x000000, 0x200000, 0xc7541b9d),
  LOAD_16_8( SPRITES, "207-c2.c2", 0x000001, 0x200000, 0xf57166d2),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( neomrdo, neogeo, "Neo Mr. Do!", VISCO, 1996, GAME_MAZE);

static struct ROM_INFO rom_sdodgeb[] = /* MVS ONLY RELEASE */
{
  LOAD_SW16( CPU1, "208-p1.p1", 0x100000, 0x100000, 0x127f3d32),
  { "208-p1.p1", 0x100000 , 0x127f3d32, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD( FIXED, "208-s1.s1", 0x000000, 0x20000, 0x64abd6b3),
  LOAD( ROM2, "208-m1.m1", 0x00000, 0x20000, 0x0a5f3325),
  LOAD( SMP1, "208-v1.v1", 0x000000, 0x400000, 0xe7899a24),
  LOAD_16_8( SPRITES, "208-c1.c1", 0x0000000, 0x400000, 0x93d8619b),
  LOAD_16_8( SPRITES, "208-c2.c2", 0x0000001, 0x400000, 0x1c737bb6),
  LOAD_16_8( SPRITES, "208-c3.c3", 0x0800000, 0x200000, 0x14cb1703),
  LOAD_16_8( SPRITES, "208-c4.c4", 0x0800001, 0x200000, 0xc7165f19),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( sdodgeb, neogeo, "Super Dodge Ball / Kunio no Nekketsu Toukyuu Densetsu", TECHNOS, 1996, GAME_SPORTS);

static struct ROM_INFO rom_goalx3[] = /* MVS ONLY RELEASE */
{
  LOAD_SW16( CPU1, "209-p1.p1", 0x100000, 0x100000, 0x2a019a79),
  { "209-p1.p1", 0x100000 , 0x2a019a79, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD( FIXED, "209-s1.s1", 0x000000, 0x20000, 0xc0eaad86),
  LOAD( ROM2, "209-m1.m1", 0x00000, 0x20000, 0xcd758325),
  LOAD( SMP1, "209-v1.v1", 0x000000, 0x200000, 0xef214212),
  LOAD_16_8( SPRITES, "209-c1.c1", 0x000000, 0x400000, 0xb49d980e),
  LOAD_16_8( SPRITES, "209-c2.c2", 0x000001, 0x400000, 0x5649b015),
  LOAD_16_8( SPRITES, "209-c3.c3", 0x800000, 0x100000, 0x5f91bace),
  LOAD_16_8( SPRITES, "209-c4.c4", 0x800001, 0x100000, 0x1e9f76f2),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( goalx3, neogeo, "Goal! Goal! Goal!", VISCO, 1995, GAME_SPORTS);

static struct ROM_INFO rom_overtop[] = // clone of neogeo
{
  LOAD_SW16( CPU1, "212-p1.p1", 0x100000, 0x100000, 0x16c063a9),
  { "212-p1.p1", 0x100000 , 0x16c063a9, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD( FIXED, "212-s1.s1", 0x000000, 0x20000, 0x481d3ddc),
  LOAD( ROM2, "212-m1.m1", 0x00000, 0x20000, 0xfcab6191),
  LOAD( SMP1, "212-v1.v1", 0x000000, 0x400000, 0x013d4ef9),
  LOAD_16_8( SPRITES, "212-c1.c1", 0x0000000, 0x400000, 0x50f43087),
  LOAD_16_8( SPRITES, "212-c2.c2", 0x0000001, 0x400000, 0xa5b39807),
  LOAD_16_8( SPRITES, "212-c3.c3", 0x0800000, 0x400000, 0x9252ea02),
  LOAD_16_8( SPRITES, "212-c4.c4", 0x0800001, 0x400000, 0x5f41a699),
  LOAD_16_8( SPRITES, "212-c5.c5", 0x1000000, 0x200000, 0xfc858bef),
  LOAD_16_8( SPRITES, "212-c6.c6", 0x1000001, 0x200000, 0x0589c15e),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( overtop, neogeo,"Over Top", ADK, 1996, GAME_MISC);

static struct ROM_INFO rom_neodrift[] = /* MVS ONLY RELEASE */
{
  LOAD_SW16( CPU1, "213-p1.p1", 0x100000, 0x100000, 0xe397d798),
  { "213-p1.p1", 0x100000, 0xe397d798, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD( FIXED, "213-s1.s1", 0x000000, 0x20000, 0xb76b61bc),
  LOAD( ROM2, "213-m1.m1", 0x00000, 0x20000, 0x200045f1),
  LOAD( SMP1, "213-v1.v1", 0x000000, 0x200000, 0xa421c076),
  LOAD( SMP1, "213-v2.v2", 0x200000, 0x200000, 0x233c7dd9),
  LOAD_16_8( SPRITES, "213-c1.c1", 0x000000, 0x400000, 0x3edc8bd3),
  LOAD_16_8( SPRITES, "213-c2.c2", 0x000001, 0x400000, 0x46ae5f16),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( neodrift, neogeo, "Neo Drift Out - New Technology", VISCO, 1996, GAME_RACE);

static struct ROM_INFO rom_kof96[] = /* MVS VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "214-p1.p1", 0x000000, 0x100000, 0x52755d74),
  LOAD_SW16( CPU1, "214-p2.sp2", 0x100000, 0x200000, 0x002ccb73),
  LOAD( FIXED, "214-s1.s1", 0x000000, 0x20000, 0x1254cbdb),
  LOAD( ROM2, "214-m1.m1", 0x00000, 0x20000, 0xdabc427c),
  LOAD( SMP1, "214-v1.v1", 0x000000, 0x400000, 0x63f7b045),
  LOAD( SMP1, "214-v2.v2", 0x400000, 0x400000, 0x25929059),
  LOAD( SMP1, "214-v3.v3", 0x800000, 0x200000, 0x92a2257d),
  LOAD_16_8( SPRITES, "214-c1.c1", 0x0000000, 0x400000, 0x7ecf4aa2),
  LOAD_16_8( SPRITES, "214-c2.c2", 0x0000001, 0x400000, 0x05b54f37),
  LOAD_16_8( SPRITES, "214-c3.c3", 0x0800000, 0x400000, 0x64989a65),
  LOAD_16_8( SPRITES, "214-c4.c4", 0x0800001, 0x400000, 0xafbea515),
  LOAD_16_8( SPRITES, "214-c5.c5", 0x1000000, 0x400000, 0x2a3bbd26),
  LOAD_16_8( SPRITES, "214-c6.c6", 0x1000001, 0x400000, 0x44d30dc7),
  LOAD_16_8( SPRITES, "214-c7.c7", 0x1800000, 0x400000, 0x3687331b),
  LOAD_16_8( SPRITES, "214-c8.c8", 0x1800001, 0x400000, 0xfa1461ad),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kof96, neogeo, "The King of Fighters '96 (NGM-214)", SNK, 1996, GAME_BEAT);

static struct ROM_INFO rom_kof96h[] = // clone of kof96
{
  LOAD_SW16( CPU1, "214-pg1.p1", 0x000000, 0x100000, 0xbd3757c9),
  LOAD_SW16( CPU1, "214-p2.sp2", 0x100000, 0x200000, 0x002ccb73),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kof96h, kof96, "The King of Fighters '96 (NGH-214)", SNK, 1996, GAME_BEAT);

static struct ROM_INFO rom_ssideki4[] = // clone of neogeo
{
  LOAD_SW16( CPU1, "215-p1.p1", 0x100000, 0x100000, 0x519b4ba3),
  { "215-p1.p1", 0x100000 , 0x519b4ba3, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD( FIXED, "215-s1.s1", 0x000000, 0x20000, 0xf0fe5c36),
  LOAD( ROM2, "215-m1.m1", 0x00000, 0x20000, 0xa932081d),
  LOAD( SMP1, "215-v1.v1", 0x000000, 0x400000, 0x877d1409),
  LOAD( SMP1, "215-v2.v2", 0x400000, 0x200000, 0x1bfa218b),
  LOAD_16_8( SPRITES, "215-c1.c1", 0x0000000, 0x400000, 0x8ff444f5),
  LOAD_16_8( SPRITES, "215-c2.c2", 0x0000001, 0x400000, 0x5b155037),
  LOAD_16_8( SPRITES, "215-c3.c3", 0x0800000, 0x400000, 0x456a073a),
  LOAD_16_8( SPRITES, "215-c4.c4", 0x0800001, 0x400000, 0x43c182e1),
  LOAD_16_8( SPRITES, "215-c5.c5", 0x1000000, 0x200000, 0x0c6f97ec),
  LOAD_16_8( SPRITES, "215-c6.c6", 0x1000001, 0x200000, 0x329c5e1b),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( ssideki4, neogeo, "The Ultimate 11 - The SNK Football Championship / Tokuten Ou - Honoo no Libero", SNK, 1996, GAME_SPORTS);

static struct ROM_INFO rom_kizuna[] = // clone of neogeo
{
  LOAD_SW16( CPU1, "216-p1.p1", 0x100000, 0x100000, 0x75d2b3de),
  { "216-p1.p1", 0x100000, 0x75d2b3de, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD( FIXED, "216-s1.s1", 0x000000, 0x20000, 0xefdc72d7),
  LOAD( ROM2, "216-m1.m1", 0x00000, 0x20000, 0x1b096820),
  LOAD( SMP1, "059-v1.v1", 0x000000, 0x200000, 0x530c50fd),
  LOAD( SMP1, "216-v2.v2", 0x200000, 0x200000, 0x03667a8d),
  LOAD( SMP1, "059-v3.v3", 0x400000, 0x200000, 0x7038c2f9),
  LOAD( SMP1, "216-v4.v4", 0x600000, 0x200000, 0x31b99bd6),
  LOAD_16_8( SPRITES, "059-c1.c1", 0x0000000, 0x200000, 0x763ba611),
	/* 400000-7fffff empty */
  LOAD_16_8( SPRITES, "059-c2.c2", 0x0000001, 0x200000, 0xe05e8ca6),
  LOAD_16_8( SPRITES, "216-c3.c3", 0x0800000, 0x400000, 0x665c9f16),
  LOAD_16_8( SPRITES, "216-c4.c4", 0x0800001, 0x400000, 0x7f5d03db),
  LOAD_16_8( SPRITES, "059-c5.c5", 0x1000000, 0x200000, 0x59013f9e),
	/* 1400000-17fffff empty */
  LOAD_16_8( SPRITES, "059-c6.c6", 0x1000001, 0x200000, 0x1c8d5def),
  LOAD_16_8( SPRITES, "059-c7.c7", 0x1800000, 0x200000, 0xc88f7035),
  LOAD_16_8( SPRITES, "059-c8.c8", 0x1800001, 0x200000, 0x484ce3ba),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kizuna, neogeo, "Kizuna Encounter - Super Tag Battle / Fu'un Super Tag Battle", SNK, 1996, GAME_BEAT);

static struct ROM_INFO rom_ninjamas[] =
{
  LOAD_SW16( CPU1, "217-p1.p1", 0x000000, 0x100000, 0x3e97ed69),
  LOAD_SW16( CPU1, "217-p2.sp2", 0x100000, 0x200000, 0x191fca88),
  LOAD( FIXED, "217-s1.s1", 0x000000, 0x20000, 0x8ff782f0),
  LOAD( ROM2, "217-m1.m1", 0x00000, 0x20000, 0xd00fb2af),
  LOAD( SMP1, "217-v1.v1", 0x000000, 0x400000, 0x1c34e013),
  LOAD( SMP1, "217-v2.v2", 0x400000, 0x200000, 0x22f1c681),
  LOAD_16_8( SPRITES, "217-c1.c1", 0x0000000, 0x400000, 0x5fe97bc4),
  LOAD_16_8( SPRITES, "217-c2.c2", 0x0000001, 0x400000, 0x886e0d66),
  LOAD_16_8( SPRITES, "217-c3.c3", 0x0800000, 0x400000, 0x59e8525f),
  LOAD_16_8( SPRITES, "217-c4.c4", 0x0800001, 0x400000, 0x8521add2),
  LOAD_16_8( SPRITES, "217-c5.c5", 0x1000000, 0x400000, 0xfb1896e5),
  LOAD_16_8( SPRITES, "217-c6.c6", 0x1000001, 0x400000, 0x1c98c54b),
  LOAD_16_8( SPRITES, "217-c7.c7", 0x1800000, 0x400000, 0x8b0ede2e),
  LOAD_16_8( SPRITES, "217-c8.c8", 0x1800001, 0x400000, 0xa085bb61),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( ninjamas, neogeo, "Ninja Master's - haoh-ninpo-cho", ADK, 1996, GAME_BEAT);

static struct ROM_INFO rom_ragnagrd[] =
{
  LOAD_SW16( CPU1, "218-p1.p1", 0x100000, 0x100000, 0xca372303),
  { "218-p1.p1", 0x100000 , 0xca372303, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD( FIXED, "218-s1.s1", 0x000000, 0x20000, 0x7d402f9a),
  LOAD( ROM2, "218-m1.m1", 0x00000, 0x20000, 0x17028bcf),
  LOAD( SMP1, "218-v1.v1", 0x000000, 0x400000, 0x61eee7f4),
  LOAD( SMP1, "218-v2.v2", 0x400000, 0x400000, 0x6104e20b),
  LOAD_16_8( SPRITES, "218-c1.c1", 0x0000000, 0x400000, 0xc31500a4),
  LOAD_16_8( SPRITES, "218-c2.c2", 0x0000001, 0x400000, 0x98aba1f9),
  LOAD_16_8( SPRITES, "218-c3.c3", 0x0800000, 0x400000, 0x833c163a),
  LOAD_16_8( SPRITES, "218-c4.c4", 0x0800001, 0x400000, 0xc1a30f69),
  LOAD_16_8( SPRITES, "218-c5.c5", 0x1000000, 0x400000, 0x6b6de0ff),
  LOAD_16_8( SPRITES, "218-c6.c6", 0x1000001, 0x400000, 0x94beefcf),
  LOAD_16_8( SPRITES, "218-c7.c7", 0x1800000, 0x400000, 0xde6f9b28),
  LOAD_16_8( SPRITES, "218-c8.c8", 0x1800001, 0x400000, 0xd9b311f6),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( ragnagrd, neogeo, "Ragnagard / Shin-Oh-Ken", SAURUS, 1996, GAME_BEAT);

static struct ROM_INFO rom_pgoal[] = /* MVS ONLY RELEASE clone of neogeo */
{
  LOAD_SW16( CPU1, "219-p1.p1", 0x100000, 0x100000, 0x6af0e574),
  { "219-p1.p1", 0x100000 , 0x6af0e574, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD( FIXED, "219-s1.s1", 0x000000, 0x20000, 0x002f3c88),
  LOAD( ROM2, "219-m1.m1", 0x00000, 0x20000, 0x958efdc8),
  LOAD( SMP1, "219-v1.v1", 0x000000, 0x400000, 0xd0ae33d9),
  LOAD_16_8( SPRITES, "219-c1.c1", 0x0000000, 0x400000, 0x67fec4dc),
  LOAD_16_8( SPRITES, "219-c2.c2", 0x0000001, 0x400000, 0x86ed01f2),
  LOAD_16_8( SPRITES, "219-c3.c3", 0x0800000, 0x200000, 0x5fdad0a5),
  LOAD_16_8( SPRITES, "219-c4.c4", 0x0800001, 0x200000, 0xf57b4a1c),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( pgoal, neogeo, "Pleasure Goal / Futsal - 5 on 5 Mini Soccer (NGM-219)", SAURUS, 1996, GAME_SPORTS);

static struct ROM_INFO rom_ironclad[] = /* Prototype - crcs should match the ones of the unreleased dump. clone of neogeo */
{
  LOAD_SW16( CPU1, "proto_220-p1.p1", 0x100000, 0x100000, 0x62a942c6),
  { "proto_220-p1.p1", 0x100000 , 0x62a942c6, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD( FIXED, "proto_220-s1.s1", 0x000000, 0x20000, 0x372fe217),
  LOAD( ROM2, "proto_220-m1.m1", 0x00000, 0x20000, 0x3a08bb63),
  LOAD( SMP1, "proto_220-v1.v1", 0x000000, 0x400000, 0x8f30a215),
  LOAD_16_8( SPRITES, "proto_220-c1.c1", 0x000000, 0x400000, 0x9aa2b7dc),
  LOAD_16_8( SPRITES, "proto_220-c2.c2", 0x000001, 0x400000, 0x8a2ad708),
  LOAD_16_8( SPRITES, "proto_220-c3.c3", 0x800000, 0x400000, 0xd67fb15a),
  LOAD_16_8( SPRITES, "proto_220-c4.c4", 0x800001, 0x400000, 0xe73ea38b),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( ironclad, neogeo, "Iron clad - Choutetsu Brikin'ger (Prototype)", SAURUS, 1996, GAME_SHOOT);

static struct ROM_INFO rom_ironclado[] = /* Prototype - bootleg/hack based on later release. clone of ironclad */
{
  LOAD_SW16( CPU1, "proto_220-p1o.p1", 0x100000, 0x100000, 0xce37e3a0),
  { "proto_220-p1o.p1", 0x100000 , 0xce37e3a0, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( ironclado, ironclad, "Iron clad - Choutetsu Brikin'ger (Prototype, bootleg)", BOOTLEG, 1996, GAME_SHOOT);

static struct ROM_INFO rom_magdrop2[] =
{
  LOAD_SW16( CPU1, "221-p1.p1", 0x000000, 0x80000, 0x7be82353),
  LOAD( FIXED, "221-s1.s1", 0x000000, 0x20000, 0x2a4063a3),
  LOAD( ROM2, "221-m1.m1", 0x00000, 0x20000, 0xbddae628),
  LOAD( SMP1, "221-v1.v1", 0x000000, 0x200000, 0x7e5e53e4),
  LOAD_16_8( SPRITES, "221-c1.c1", 0x000000, 0x400000, 0x1f862a14),
  LOAD_16_8( SPRITES, "221-c2.c2", 0x000001, 0x400000, 0x14b90536),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( magdrop2, neogeo, "Magical Drop II", DECO, 1996, GAME_PUZZLE);

static struct ROM_INFO rom_samsho4[] = /* MVS AND AES VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "222-p1.p1", 0x000000, 0x100000, 0x1a5cb56d),
  LOAD_SW16( CPU1, "222-p2.sp2", 0x100000, 0x400000, 0xb023cd8b),
  LOAD( FIXED, "222-s1.s1", 0x000000, 0x20000, 0x8d3d3bf9),
  LOAD( ROM2, "222-m1.m1", 0x00000, 0x20000, 0x7615bc1b),
  LOAD( SMP1, "222-v1.v1", 0x000000, 0x400000, 0x7d6ba95f),
  LOAD( SMP1, "222-v2.v2", 0x400000, 0x400000, 0x6c33bb5d),
  LOAD( SMP1, "222-v3.v3", 0x800000, 0x200000, 0x831ea8c0),
  LOAD_16_8( SPRITES, "222-c1.c1", 0x0000000, 0x400000, 0x68f2ed95),
  LOAD_16_8( SPRITES, "222-c2.c2", 0x0000001, 0x400000, 0xa6e9aff0),
  LOAD_16_8( SPRITES, "222-c3.c3", 0x0800000, 0x400000, 0xc91b40f4),
  LOAD_16_8( SPRITES, "222-c4.c4", 0x0800001, 0x400000, 0x359510a4),
  LOAD_16_8( SPRITES, "222-c5.c5", 0x1000000, 0x400000, 0x9cfbb22d),
  LOAD_16_8( SPRITES, "222-c6.c6", 0x1000001, 0x400000, 0x685efc32),
  LOAD_16_8( SPRITES, "222-c7.c7", 0x1800000, 0x400000, 0xd0f86f0d),
  LOAD_16_8( SPRITES, "222-c8.c8", 0x1800001, 0x400000, 0xadfc50e3),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( samsho4, neogeo, "Samurai Shodown IV - Amakusa's Revenge / Samurai Spirits - Amakusa Kourin (NGM-222)(NGH-222)", SNK, 1996, GAME_BEAT);

static struct ROM_INFO rom_samsho4k[] = /* KOREAN VERSION clone of samsho4 */
{
  LOAD_SW16( CPU1, "222-p1k.p1", 0x000000, 0x100000, 0x06e0a25d),
  LOAD_SW16( CPU1, "222-p2.sp2", 0x100000, 0x400000, 0xb023cd8b),
  LOAD( FIXED, "222-s1k.s1", 0x000000, 0x20000, 0xd313687d),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( samsho4k, samsho4, "Pae Wang Jeon Seol / Legend of a Warrior (Korean censored Samurai Shodown IV)", SNK, 1996, GAME_BEAT);

static struct ROM_INFO rom_rbffspec[] = // clone of neogeo
{
  LOAD_SW16( CPU1, "223-p1.p1", 0x000000, 0x100000, 0xf84a2d1d),
  LOAD_SW16( CPU1, "223-p2.sp2", 0x100000, 0x400000, 0xaddd8f08),
  LOAD( FIXED, "223-s1.s1", 0x000000, 0x20000, 0x7ecd6e8c),
  LOAD( ROM2, "223-m1.m1", 0x00000, 0x20000, 0x3fee46bf),
  LOAD( SMP1, "223-v1.v1", 0x000000, 0x400000, 0x76673869),
  LOAD( SMP1, "223-v2.v2", 0x400000, 0x400000, 0x7a275acd),
  LOAD( SMP1, "223-v3.v3", 0x800000, 0x400000, 0x5a797fd2),
  LOAD_16_8( SPRITES, "223-c1.c1", 0x0000000, 0x400000, 0xebab05e2),
  LOAD_16_8( SPRITES, "223-c2.c2", 0x0000001, 0x400000, 0x641868c3),
  LOAD_16_8( SPRITES, "223-c3.c3", 0x0800000, 0x400000, 0xca00191f),
  LOAD_16_8( SPRITES, "223-c4.c4", 0x0800001, 0x400000, 0x1f23d860),
  LOAD_16_8( SPRITES, "223-c5.c5", 0x1000000, 0x400000, 0x321e362c),
  LOAD_16_8( SPRITES, "223-c6.c6", 0x1000001, 0x400000, 0xd8fcef90),
  LOAD_16_8( SPRITES, "223-c7.c7", 0x1800000, 0x400000, 0xbc80dd2d),
  LOAD_16_8( SPRITES, "223-c8.c8", 0x1800001, 0x400000, 0x5ad62102),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( rbffspec, neogeo, "Real Bout Fatal Fury Special / Real Bout Garou Densetsu Special", SNK, 1996, GAME_BEAT);

static struct ROM_INFO rom_twinspri[] = // clone of neogeo
{
  LOAD_SW16( CPU1, "224-p1.p1", 0x100000, 0x100000, 0x7697e445),
  { "224-p1.p1", 0x100000 , 0x7697e445, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD( FIXED, "224-s1.s1", 0x000000, 0x20000, 0xeeed5758),
  LOAD( ROM2, "224-m1.m1", 0x00000, 0x20000, 0x364d6f96),
  LOAD( SMP1, "224-v1.v1", 0x000000, 0x400000, 0xff57f088),
  LOAD( SMP1, "224-v2.v2", 0x400000, 0x200000, 0x7ad26599),
  LOAD_16_8( SPRITES, "224-c1.c1", 0x000000, 0x400000, 0xf7da64ab),
  LOAD_16_8( SPRITES, "224-c2.c2", 0x000001, 0x400000, 0x4c09bbfb),
  LOAD_16_8( SPRITES, "224-c3.c3", 0x800000, 0x100000, 0xc59e4129),
  LOAD_16_8( SPRITES, "224-c4.c4", 0x800001, 0x100000, 0xb5532e53),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( twinspri, neogeo,        "Twinkle Star Sprites", ADK, 1996, GAME_SHOOT);

static struct ROM_INFO rom_wakuwak7[] =
{
  LOAD_SW16( CPU1, "225-p1.p1", 0x000000, 0x100000, 0xb14da766),
  LOAD_SW16( CPU1, "225-p2.sp2", 0x100000, 0x200000, 0xfe190665),
  LOAD( FIXED, "225-s1.s1", 0x000000, 0x20000, 0x71c4b4b5),
  LOAD( ROM2, "225-m1.m1", 0x00000, 0x20000, 0x0634bba6),
  LOAD( SMP1, "225-v1.v1", 0x000000, 0x400000, 0x6195c6b4),
  LOAD( SMP1, "225-v2.v2", 0x400000, 0x400000, 0x6159c5fe),
  LOAD_16_8( SPRITES, "225-c1.c1", 0x0000000, 0x400000, 0xee4fea54),
  LOAD_16_8( SPRITES, "225-c2.c2", 0x0000001, 0x400000, 0x0c549e2d),
  LOAD_16_8( SPRITES, "225-c3.c3", 0x0800000, 0x400000, 0xaf0897c0),
  LOAD_16_8( SPRITES, "225-c4.c4", 0x0800001, 0x400000, 0x4c66527a),
  LOAD_16_8( SPRITES, "225-c5.c5", 0x1000000, 0x400000, 0x8ecea2b5),
  LOAD_16_8( SPRITES, "225-c6.c6", 0x1000001, 0x400000, 0x0eb11a6d),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( wakuwak7, neogeo, "Waku Waku 7", SUNSOFT, 1996, GAME_BEAT);

static struct ROM_INFO rom_stakwin2[] =
{
  LOAD_SW16( CPU1, "227-p1.p1", 0x100000, 0x100000, 0xdaf101d2),
  { "227-p1.p1", 0x100000 , 0xdaf101d2, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD( FIXED, "227-s1.s1", 0x000000, 0x20000, 0x2a8c4462),
  LOAD( ROM2, "227-m1.m1", 0x00000, 0x20000, 0xc8e5e0f9),
  LOAD( SMP1, "227-v1.v1", 0x000000, 0x400000, 0xb8f24181),
  LOAD( SMP1, "227-v2.v2", 0x400000, 0x400000, 0xee39e260),
  LOAD_16_8( SPRITES, "227-c1.c1", 0x0000000, 0x400000, 0x7d6c2af4),
  LOAD_16_8( SPRITES, "227-c2.c2", 0x0000001, 0x400000, 0x7e402d39),
  LOAD_16_8( SPRITES, "227-c3.c3", 0x0800000, 0x200000, 0x93dfd660),
  LOAD_16_8( SPRITES, "227-c4.c4", 0x0800001, 0x200000, 0x7efea43a),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( stakwin2, neogeo, "Stakes Winner 2", SAURUS, 1996, GAME_SPORTS);

static struct ROM_INFO rom_ghostlop[] = /* Prototype clone of neogeo */
{
  LOAD_SW16( CPU1, "proto_228-p1.p1", 0x000000, 0x100000, 0x6033172e),
  LOAD( FIXED, "proto_228-s1.s1", 0x000000, 0x20000, 0x83c24e81),
  LOAD( ROM2, "proto_228-m1.m1", 0x00000, 0x20000, 0xfd833b33),
  LOAD( SMP1, "proto_228-v1.v1", 0x000000, 0x200000, 0xc603fce6),
  LOAD_16_8( SPRITES, "proto_228-c1.c1", 0x000000, 0x400000, 0xbfc99efe),
  LOAD_16_8( SPRITES, "proto_228-c2.c2", 0x000001, 0x400000, 0x69788082),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( ghostlop, neogeo, "Ghostlop (prototype)", DECO, 1996, GAME_PUZZLE);

static struct ROM_INFO rom_breakers[] = // clone of neogeo
{
  LOAD_SW16( CPU1, "230-p1.p1", 0x100000, 0x100000, 0xed24a6e6),
  { "230-p1.p1", 0x100000 , 0xed24a6e6, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD( FIXED, "230-s1.s1", 0x000000, 0x20000, 0x076fb64c),
  LOAD( ROM2, "230-m1.m1", 0x00000, 0x20000, 0x3951a1c1),
  LOAD( SMP1, "230-v1.v1", 0x000000, 0x400000, 0x7f9ed279),
  LOAD( SMP1, "230-v2.v2", 0x400000, 0x400000, 0x1d43e420),
  LOAD_16_8( SPRITES, "230-c1.c1", 0x000000, 0x400000, 0x68d4ae76),
  LOAD_16_8( SPRITES, "230-c2.c2", 0x000001, 0x400000, 0xfdee05cd),
  LOAD_16_8( SPRITES, "230-c3.c3", 0x800000, 0x400000, 0x645077f3),
  LOAD_16_8( SPRITES, "230-c4.c4", 0x800001, 0x400000, 0x63aeb74c),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( breakers, neogeo, "Breakers", VISCO, 1996, GAME_BEAT);

static struct ROM_INFO rom_miexchng[] = /* MVS ONLY RELEASE clone of neogeo */
{
  LOAD_SW16( CPU1, "231-p1.p1", 0x000000, 0x80000, 0x61be1810),
  LOAD( FIXED, "231-s1.s1", 0x000000, 0x20000, 0xfe0c0c53),
  LOAD( ROM2, "231-m1.m1", 0x00000, 0x20000, 0xde41301b),
  LOAD( SMP1, "231-v1.v1", 0x000000, 0x400000, 0x113fb898),
  LOAD_16_8( SPRITES, "231-c1.c1", 0x000000, 0x200000, 0x6c403ba3),
  LOAD_16_8( SPRITES, "231-c2.c2", 0x000001, 0x200000, 0x554bcd9b),
  LOAD_16_8( SPRITES, "231-c3.c3", 0x400000, 0x100000, 0x4f6f7a63),
  LOAD_16_8( SPRITES, "231-c4.c4", 0x400001, 0x100000, 0x2e35e71b),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( miexchng, neogeo, "Money Puzzle Exchanger / Money Idol Exchanger", FACE, 1997, GAME_PUZZLE);

static struct ROM_INFO rom_kof97[] = /* MVS VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "232-p1.p1", 0x000000, 0x100000, 0x7db81ad9),
  LOAD_SW16( CPU1, "232-p2.sp2", 0x100000, 0x400000, 0x158b23f6),
  LOAD( FIXED, "232-s1.s1", 0x000000, 0x20000, 0x8514ecf5),
  LOAD( ROM2, "232-m1.m1", 0x00000, 0x20000, 0x45348747),
  LOAD( SMP1, "232-v1.v1", 0x000000, 0x400000, 0x22a2b5b5),
  LOAD( SMP1, "232-v2.v2", 0x400000, 0x400000, 0x2304e744),
  LOAD( SMP1, "232-v3.v3", 0x800000, 0x400000, 0x759eb954),
  LOAD_16_8( SPRITES, "232-c1.c1", 0x0000000, 0x800000, 0x5f8bf0a1),
  LOAD_16_8( SPRITES, "232-c2.c2", 0x0000001, 0x800000, 0xe4d45c81),
  LOAD_16_8( SPRITES, "232-c3.c3", 0x1000000, 0x800000, 0x581d6618),
  LOAD_16_8( SPRITES, "232-c4.c4", 0x1000001, 0x800000, 0x49bb1e68),
  LOAD_16_8( SPRITES, "232-c5.c5", 0x2000000, 0x400000, 0x34fc4e51),
  LOAD_16_8( SPRITES, "232-c6.c6", 0x2000001, 0x400000, 0x4ff4d47b),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kof97, neogeo, "The King of Fighters '97 (NGM-2320)", SNK, 1997, GAME_BEAT);

static struct ROM_INFO rom_kof97h[] = /* AES VERSION clone of kof97 */
{
  LOAD_SW16( CPU1, "232-pg1.p1", 0x000000, 0x100000, 0x5c2400b7),
  LOAD_SW16( CPU1, "232-p2.sp2", 0x100000, 0x400000, 0x158b23f6),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kof97h, kof97, "The King of Fighters '97 (NGH-2320)", SNK, 1997, GAME_BEAT);

static struct ROM_INFO rom_magdrop3[] = // clone of neogeo
{
  LOAD_SW16( CPU1, "233-p1.p1", 0x000000, 0x100000, 0x931e17fa),
  LOAD( FIXED, "233-s1.s1", 0x000000, 0x20000, 0x7399e68a),
  LOAD( ROM2, "233-m1.m1", 0x00000, 0x20000, 0x5beaf34e),
  LOAD( SMP1, "233-v1.v1", 0x000000, 0x400000, 0x58839298),
  LOAD( SMP1, "233-v2.v2", 0x400000, 0x080000, 0xd5e30df4),
  LOAD_16_8( SPRITES, "233-c1.c1", 0x000000, 0x400000, 0x65e3f4c4),
  LOAD_16_8( SPRITES, "233-c2.c2", 0x000001, 0x400000, 0x35dea6c9),
  LOAD_16_8( SPRITES, "233-c3.c3", 0x800000, 0x400000, 0x0ba2c502),
  LOAD_16_8( SPRITES, "233-c4.c4", 0x800001, 0x400000, 0x70dbbd6d),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( magdrop3, neogeo, "Magical Drop III", DECO, 1997, GAME_PUZZLE);

static struct ROM_INFO rom_lastblad[] = /* MVS VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "234-p1.p1", 0x000000, 0x100000, 0xe123a5a3),
  LOAD_SW16( CPU1, "234-p2.sp2", 0x100000, 0x400000, 0x0fdc289e),
  LOAD( FIXED, "234-s1.s1", 0x000000, 0x20000, 0x95561412),
  LOAD( ROM2, "234-m1.m1", 0x00000, 0x20000, 0x087628ea),
  LOAD( SMP1, "234-v1.v1", 0x000000, 0x400000, 0xed66b76f),
  LOAD( SMP1, "234-v2.v2", 0x400000, 0x400000, 0xa0e7f6e2),
  LOAD( SMP1, "234-v3.v3", 0x800000, 0x400000, 0xa506e1e2),
  LOAD( SMP1, "234-v4.v4", 0xc00000, 0x400000, 0x0e34157f),
  LOAD_16_8( SPRITES, "234-c1.c1", 0x0000000, 0x800000, 0x9f7e2bd3),
  LOAD_16_8( SPRITES, "234-c2.c2", 0x0000001, 0x800000, 0x80623d3c),
  LOAD_16_8( SPRITES, "234-c3.c3", 0x1000000, 0x800000, 0x91ab1a30),
  LOAD_16_8( SPRITES, "234-c4.c4", 0x1000001, 0x800000, 0x3d60b037),
  LOAD_16_8( SPRITES, "234-c5.c5", 0x2000000, 0x400000, 0x1ba80cee),
  LOAD_16_8( SPRITES, "234-c6.c6", 0x2000001, 0x400000, 0xbeafd091),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_lastbladh[] = /* AES VERSION clone of lastblad */
{
  LOAD_SW16( CPU1, "234-pg1.p1", 0x000000, 0x100000, 0xcd01c06d),
  LOAD_SW16( CPU1, "234-p2.sp2", 0x100000, 0x400000, 0x0fdc289e),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( lastblad, neogeo, "The Last Blade / Bakumatsu Roman - Gekka no Kenshi (NGM-2340)", SNK, 1997, GAME_BEAT);
CLNEI( lastbladh, lastblad, "The Last Blade / Bakumatsu Roman - Gekka no Kenshi (NGH-2340)", SNK, 1997, GAME_BEAT);

static struct ROM_INFO rom_puzzldpr[] = /* MVS ONLY RELEASE clone of puzzledp */
{
  LOAD_SW16( CPU1, "235-p1.p1", 0x000000, 0x080000, 0xafed5de2),
  LOAD( FIXED, "235-s1.s1", 0x000000, 0x10000, 0x5a68d91e),
  FILL( FIXED, 0x10000, 0x10000, 0),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( puzzldpr, puzzledp, "Puzzle De Pon! R!", TAITO, 1997, GAME_PUZZLE);

static struct ROM_INFO rom_irrmaze[] = /* MVS ONLY RELEASE clone of neogeo */
{
  LOAD_SW16( CPU1, "236-p1.p1", 0x100000, 0x100000, 0x4c2ff660),
  { "236-p1.p1", 0x100000 , 0x4c2ff660, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD( FIXED, "236-s1.s1", 0x000000, 0x20000, 0x5d1ca640),
	/* special BIOS with trackball support, we only have one Irritating Maze bios and thats asia */
  LOAD( ROM2, "236-m1.m1", 0x00000, 0x20000, 0x880a1abd),
  LOAD( SMP1, "236-v1.v1", 0x000000, 0x200000, 0x5f89c3b4),
  LOAD( YMSND_DELTAT, "236-v2.v2", 0x000000, 0x100000, 0x1e843567),
  LOAD_16_8( SPRITES, "236-c1.c1", 0x000000, 0x400000, 0xc1d47902),
  LOAD_16_8( SPRITES, "236-c2.c2", 0x000001, 0x400000, 0xe15f972e),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( irrmaze, neogeo, "The Irritating Maze / Ultra Denryu Iraira Bou", SNK, 1997, GAME_PUZZLE,
	.input = input_irrmaze);

static struct ROM_INFO rom_popbounc[] = /* MVS ONLY RELEASE */
{
  LOAD_SW16( CPU1, "237-p1.p1", 0x000000, 0x100000, 0xbe96e44f),
  LOAD( FIXED, "237-s1.s1", 0x000000, 0x20000, 0xb61cf595),
  LOAD( ROM2, "237-m1.m1", 0x00000, 0x20000, 0xd4c946dd),
  LOAD( SMP1, "237-v1.v1", 0x000000, 0x200000, 0xedcb1beb),
  LOAD_16_8( SPRITES, "237-c1.c1", 0x000000, 0x200000, 0xeda42d66),
  LOAD_16_8( SPRITES, "237-c2.c2", 0x000001, 0x200000, 0x5e633c65),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( popbounc, neogeo, "Pop 'n Bounce / Gapporin", VIDEOSYSTEM, 1997, GAME_BREAKOUT,
	.input = input_popbounc);

static struct ROM_INFO rom_shocktro[] = /* MVS ONLY RELEASE clone of neogeo */
{
  LOAD_SW16( CPU1, "238-p1.p1", 0x000000, 0x100000, 0x5677456f),
  LOAD_SW16( CPU1, "238-p2.sp2", 0x100000, 0x400000, 0x5b4a09c5),
  LOAD( FIXED, "238-s1.s1", 0x000000, 0x20000, 0x1f95cedb),
  LOAD( ROM2, "238-m1.m1", 0x00000, 0x20000, 0x075b9518),
  LOAD( SMP1, "238-v1.v1", 0x000000, 0x400000, 0x260c0bef),
  LOAD( SMP1, "238-v2.v2", 0x400000, 0x200000, 0x4ad7d59e),
  LOAD_16_8( SPRITES, "238-c1.c1", 0x0000000, 0x400000, 0x90c6a181),
  LOAD_16_8( SPRITES, "238-c2.c2", 0x0000001, 0x400000, 0x888720f0),
  LOAD_16_8( SPRITES, "238-c3.c3", 0x0800000, 0x400000, 0x2c393aa3),
  LOAD_16_8( SPRITES, "238-c4.c4", 0x0800001, 0x400000, 0xb9e909eb),
  LOAD_16_8( SPRITES, "238-c5.c5", 0x1000000, 0x400000, 0xc22c68eb),
  LOAD_16_8( SPRITES, "238-c6.c6", 0x1000001, 0x400000, 0x119323cd),
  LOAD_16_8( SPRITES, "238-c7.c7", 0x1800000, 0x400000, 0xa72ce7ed),
  LOAD_16_8( SPRITES, "238-c8.c8", 0x1800001, 0x400000, 0x1c7c2efb),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( shocktro, neogeo, "Shock Troopers (set 1)", SAURUS, 1997, GAME_SHOOT);

static struct ROM_INFO rom_blazstar[] = // clone of neogeo
{
  LOAD_SW16( CPU1, "239-p1.p1", 0x000000, 0x100000, 0x183682f8),
  LOAD_SW16( CPU1, "239-p2.sp2", 0x100000, 0x200000, 0x9a9f4154),
  LOAD( FIXED, "239-s1.s1", 0x000000, 0x20000, 0xd56cb498),
  LOAD( ROM2, "239-m1.m1", 0x00000, 0x20000, 0xd31a3aea),
  LOAD( SMP1, "239-v1.v1", 0x000000, 0x400000, 0x1b8d5bf7),
  LOAD( SMP1, "239-v2.v2", 0x400000, 0x400000, 0x74cf0a70),
  LOAD_16_8( SPRITES, "239-c1.c1", 0x0000000, 0x400000, 0x84f6d584),
  LOAD_16_8( SPRITES, "239-c2.c2", 0x0000001, 0x400000, 0x05a0cb22),
  LOAD_16_8( SPRITES, "239-c3.c3", 0x0800000, 0x400000, 0x5fb69c9e),
  LOAD_16_8( SPRITES, "239-c4.c4", 0x0800001, 0x400000, 0x0be028c4),
  LOAD_16_8( SPRITES, "239-c5.c5", 0x1000000, 0x400000, 0x74bae5f8),
  LOAD_16_8( SPRITES, "239-c6.c6", 0x1000001, 0x400000, 0x4e0700d2),
  LOAD_16_8( SPRITES, "239-c7.c7", 0x1800000, 0x400000, 0x010ff4fd),
  LOAD_16_8( SPRITES, "239-c8.c8", 0x1800001, 0x400000, 0xdb60460e),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( blazstar, neogeo, "Blazing Star", YUMEKOBO, 1998, GAME_SHOOT);

static struct ROM_INFO rom_rbff2[] = /* MVS VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "240-p1.p1", 0x000000, 0x100000, 0x80e41205),
  LOAD_SW16( CPU1, "240-p2.sp2", 0x100000, 0x400000, 0x960aa88d),
  LOAD( FIXED, "240-s1.s1", 0x000000, 0x20000, 0xda3b40de),
  LOAD( ROM2, "240-m1.m1", 0x00000, 0x40000, 0xed482791),
  LOAD( SMP1, "240-v1.v1", 0x000000, 0x400000, 0xf796265a),
  LOAD( SMP1, "240-v2.v2", 0x400000, 0x400000, 0x2cb3f3bb),
  LOAD( SMP1, "240-v3.v3", 0x800000, 0x400000, 0x8fe1367a),
  LOAD( SMP1, "240-v4.v4", 0xc00000, 0x200000, 0x996704d8),
  LOAD_16_8( SPRITES, "240-c1.c1", 0x0000000, 0x800000, 0xeffac504),
  LOAD_16_8( SPRITES, "240-c2.c2", 0x0000001, 0x800000, 0xed182d44),
  LOAD_16_8( SPRITES, "240-c3.c3", 0x1000000, 0x800000, 0x22e0330a),
  LOAD_16_8( SPRITES, "240-c4.c4", 0x1000001, 0x800000, 0xc19a07eb),
  LOAD_16_8( SPRITES, "240-c5.c5", 0x2000000, 0x800000, 0x244dff5a),
  LOAD_16_8( SPRITES, "240-c6.c6", 0x2000001, 0x800000, 0x4609e507),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_rbff2h[] = /* AES VERSION clone of rbff2 */
{
  LOAD_SW16( CPU1, "240-pg1.p1", 0x000000, 0x100000, 0xb6969780),
  LOAD_SW16( CPU1, "240-p2.sp2", 0x100000, 0x400000, 0x960aa88d),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( rbff2, neogeo, "Real Bout Fatal Fury 2 - The Newcomers / Real Bout Garou Densetsu 2 - the newcomers (NGM-2400)", SNK, 1998, GAME_BEAT);
CLNEI( rbff2h, rbff2, "Real Bout Fatal Fury 2 - The Newcomers / Real Bout Garou Densetsu 2 - the newcomers (NGH-2400)", SNK, 1998, GAME_BEAT);

static struct ROM_INFO rom_mslug2[] = /* MVS AND AES VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "241-p1.p1", 0x000000, 0x100000, 0x2a53c5da),
  LOAD_SW16( CPU1, "241-p2.sp2", 0x100000, 0x200000, 0x38883f44),
  LOAD( FIXED, "241-s1.s1", 0x000000, 0x20000, 0xf3d32f0f),
  LOAD( ROM2, "241-m1.m1", 0x00000, 0x20000, 0x94520ebd),
  LOAD( SMP1, "241-v1.v1", 0x000000, 0x400000, 0x99ec20e8),
  LOAD( SMP1, "241-v2.v2", 0x400000, 0x400000, 0xecb16799),
  LOAD_16_8( SPRITES, "241-c1.c1", 0x0000000, 0x800000, 0x394b5e0d),
  LOAD_16_8( SPRITES, "241-c2.c2", 0x0000001, 0x800000, 0xe5806221),
  LOAD_16_8( SPRITES, "241-c3.c3", 0x1000000, 0x800000, 0x9f6bfa6f),
  LOAD_16_8( SPRITES, "241-c4.c4", 0x1000001, 0x800000, 0x7d3e306f),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( mslug2, neogeo, "Metal Slug 2 - Super Vehicle-001/II (NGM-2410)(NGH-2410)", SNK, 1998, GAME_SHOOT);

static struct ROM_INFO rom_kof98[] = /* encrypted code + protection clone of neogeo */
 /* MVS VERSION */{
  LOAD_SW16( CPU1, "242-p1.p1", 0x000000, 0x200000, 0x8893df89),
  LOAD_SW16( CPU1, "242-p2.sp2", 0x200000, 0x400000, 0x980aba4c),
  LOAD( FIXED, "242-s1.s1", 0x000000, 0x20000, 0x7f7b4805),
  LOAD( ROM2, "242-m1.m1", 0x00000, 0x40000, 0x4ef7016b),
  LOAD( SMP1, "242-v1.v1", 0x000000, 0x400000, 0xb9ea8051),
  LOAD( SMP1, "242-v2.v2", 0x400000, 0x400000, 0xcc11106e),
  LOAD( SMP1, "242-v3.v3", 0x800000, 0x400000, 0x044ea4e1),
  LOAD( SMP1, "242-v4.v4", 0xc00000, 0x400000, 0x7985ea30),
  LOAD_16_8( SPRITES, "242-c1.c1", 0x0000000, 0x800000, 0xe564ecd6),
  LOAD_16_8( SPRITES, "242-c2.c2", 0x0000001, 0x800000, 0xbd959b60),
  LOAD_16_8( SPRITES, "242-c3.c3", 0x1000000, 0x800000, 0x22127b4f),
  LOAD_16_8( SPRITES, "242-c4.c4", 0x1000001, 0x800000, 0x0b4fa044),
  LOAD_16_8( SPRITES, "242-c5.c5", 0x2000000, 0x800000, 0x9d10bed3),
  LOAD_16_8( SPRITES, "242-c6.c6", 0x2000001, 0x800000, 0xda07b6a2),
  LOAD_16_8( SPRITES, "242-c7.c7", 0x3000000, 0x800000, 0xf6d7a38a),
  LOAD_16_8( SPRITES, "242-c8.c8", 0x3000001, 0x800000, 0xc823e045),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kof98, neogeo, "The King of Fighters '98 - The Slugfest / King of Fighters '98 - dream match never ends (NGM-2420)", SNK, 1998, GAME_BEAT);

static struct ROM_INFO rom_kof98h[] = /* AES VERSION clone of kof98 */
{
  LOAD_SW16( CPU1, "242-pn1.p1", 0x000000, 0x100000, 0x61ac868a),
  LOAD_SW16( CPU1, "242-p2.sp2", 0x100000, 0x400000, 0x980aba4c),
  LOAD( ROM2, "242-mg1.m1", 0x00000, 0x40000, 0x4e7a6b1b),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kof98h, kof98, "The King of Fighters '98 - The Slugfest / King of Fighters '98 - dream match never ends (NGH-2420)", SNK, 1998, GAME_BEAT);

static struct ROM_INFO rom_lastbld2[] = /* MVS AND AES VERSION clone of neogeo */
 /* later revision */{
  LOAD_SW16( CPU1, "243-pg1.p1", 0x000000, 0x100000, 0xaf1e6554),
  LOAD_SW16( CPU1, "243-pg2.sp2", 0x100000, 0x400000, 0xadd4a30b),
  LOAD( FIXED, "243-s1.s1", 0x000000, 0x20000, 0xc9cd2298),
  LOAD( ROM2, "243-m1.m1", 0x00000, 0x20000, 0xacf12d10),
  LOAD( SMP1, "243-v1.v1", 0x000000, 0x400000, 0xf7ee6fbb),
  LOAD( SMP1, "243-v2.v2", 0x400000, 0x400000, 0xaa9e4df6),
  LOAD( SMP1, "243-v3.v3", 0x800000, 0x400000, 0x4ac750b2),
  LOAD( SMP1, "243-v4.v4", 0xc00000, 0x400000, 0xf5c64ba6),
  LOAD_16_8( SPRITES, "243-c1.c1", 0x0000000, 0x800000, 0x5839444d),
  LOAD_16_8( SPRITES, "243-c2.c2", 0x0000001, 0x800000, 0xdd087428),
  LOAD_16_8( SPRITES, "243-c3.c3", 0x1000000, 0x800000, 0x6054cbe0),
  LOAD_16_8( SPRITES, "243-c4.c4", 0x1000001, 0x800000, 0x8bd2a9d2),
  LOAD_16_8( SPRITES, "243-c5.c5", 0x2000000, 0x800000, 0x6a503dcf),
  LOAD_16_8( SPRITES, "243-c6.c6", 0x2000001, 0x800000, 0xec9c36d0),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( lastbld2, neogeo, "The Last Blade 2 / Bakumatsu Roman - Dai Ni Maku Gekka no Kenshi (NGM-2430)(NGH-2430)", SNK, 1998, GAME_BEAT);

static struct ROM_INFO rom_neocup98[] = // clone of neogeo
{
  LOAD_SW16( CPU1, "244-p1.p1", 0x100000, 0x100000, 0xf8fdb7a5),
  { "244-p1.p1", 0x100000 , 0xf8fdb7a5, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD( FIXED, "244-s1.s1", 0x000000, 0x20000, 0x9bddb697),
  LOAD( ROM2, "244-m1.m1", 0x00000, 0x20000, 0xa701b276),
  LOAD( SMP1, "244-v1.v1", 0x000000, 0x400000, 0x79def46d),
  LOAD( SMP1, "244-v2.v2", 0x400000, 0x200000, 0xb231902f),
  LOAD_16_8( SPRITES, "244-c1.c1", 0x000000, 0x800000, 0xc7a62b23),
  LOAD_16_8( SPRITES, "244-c2.c2", 0x000001, 0x800000, 0x33aa0f35),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( neocup98, neogeo, "Neo-Geo Cup '98 - The Road to the Victory", SNK, 1998, GAME_SPORTS);

static struct ROM_INFO rom_breakrev[] = /* MVS ONLY RELEASE clone of neogeo */
{
  LOAD_SW16( CPU1, "245-p1.p1", 0x100000, 0x100000, 0xc828876d),
  { "245-p1.p1", 0x100000 , 0xc828876d, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD( FIXED, "245-s1.s1", 0x000000, 0x20000, 0xe7660a5d),
  LOAD( ROM2, "245-m1.m1", 0x00000, 0x20000, 0x00f31c66),
  LOAD( SMP1, "245-v1.v1", 0x000000, 0x400000, 0xe255446c),
  LOAD( SMP1, "245-v2.v2", 0x400000, 0x400000, 0x9068198a),
	/* The chip labels and sizes are correct */
  LOAD_16_8( SPRITES, "245-c1.c1", 0x0000000, 0x400000, 0x68d4ae76),
  LOAD_16_8( SPRITES, "245-c2.c2", 0x0000001, 0x400000, 0xfdee05cd),
  LOAD_16_8( SPRITES, "245-c3.c3", 0x0800000, 0x400000, 0x645077f3),
  LOAD_16_8( SPRITES, "245-c4.c4", 0x0800001, 0x400000, 0x63aeb74c),
  LOAD_16_8( SPRITES, "245-c5.c5", 0x1000000, 0x400000, 0xb5f40e7f),
  LOAD_16_8( SPRITES, "245-c6.c6", 0x1000001, 0x400000, 0xd0337328),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( breakrev, neogeo, "Breakers Revenge", VISCO, 1998, GAME_BEAT);

static struct ROM_INFO rom_shocktr2[] = // clone of neogeo
{
  LOAD_SW16( CPU1, "246-p1.p1", 0x000000, 0x100000, 0x6d4b7781),
  LOAD_SW16( CPU1, "246-p2.sp2", 0x100000, 0x400000, 0x72ea04c3),
  LOAD( FIXED, "246-s1.s1", 0x000000, 0x20000, 0x2a360637),
  LOAD( ROM2, "246-m1.m1", 0x00000, 0x20000, 0xd0604ad1),
  LOAD( SMP1, "246-v1.v1", 0x000000, 0x400000, 0x16986fc6),
  LOAD( SMP1, "246-v2.v2", 0x400000, 0x400000, 0xada41e83),
  LOAD( SMP1, "246-v3.v3", 0x800000, 0x200000, 0xa05ba5db),
  LOAD_16_8( SPRITES, "246-c1.c1", 0x0000000, 0x800000, 0x47ac9ec5),
  LOAD_16_8( SPRITES, "246-c2.c2", 0x0000001, 0x800000, 0x7bcab64f),
  LOAD_16_8( SPRITES, "246-c3.c3", 0x1000000, 0x800000, 0xdb2f73e8),
  LOAD_16_8( SPRITES, "246-c4.c4", 0x1000001, 0x800000, 0x5503854e),
  LOAD_16_8( SPRITES, "246-c5.c5", 0x2000000, 0x800000, 0x055b3701),
  LOAD_16_8( SPRITES, "246-c6.c6", 0x2000001, 0x800000, 0x7e2caae1),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( shocktr2, neogeo, "Shock Troopers - 2nd Squad", SAURUS, 1998, GAME_SHOOT);

static struct ROM_INFO rom_flipshot[] = /* MVS ONLY RELEASE clone of neogeo */
{
  LOAD_SW16( CPU1, "247-p1.p1", 0x000000, 0x100000, 0x95779094),
  LOAD( FIXED, "247-s1.s1", 0x000000, 0x20000, 0x6300185c),
  LOAD( ROM2, "247-m1.m1", 0x00000, 0x20000, 0xa9fe0144),
  LOAD( SMP1, "247-v1.v1", 0x000000, 0x200000, 0x42ec743d),
  LOAD_16_8( SPRITES, "247-c1.c1", 0x000000, 0x200000, 0xc9eedcb2),
  LOAD_16_8( SPRITES, "247-c2.c2", 0x000001, 0x200000, 0x7d6d6e87),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( flipshot, neogeo, "Battle Flip Shot", VISCO, 1998, GAME_MISC);

static struct ROM_INFO rom_pbobbl2n[] = /* MVS ONLY RELEASE clone of neogeo */
{
  LOAD_SW16( CPU1, "248-p1.p1", 0x000000, 0x100000, 0x9d6c0754),
  LOAD( FIXED, "248-s1.s1", 0x000000, 0x20000, 0x0a3fee41),
  LOAD( ROM2, "248-m1.m1", 0x00000, 0x20000, 0x883097a9),
  LOAD( SMP1, "248-v1.v1", 0x000000, 0x400000, 0x57fde1fa),
  LOAD( SMP1, "248-v2.v2", 0x400000, 0x400000, 0x4b966ef3),
  LOAD_16_8( SPRITES, "248-c1.c1", 0x000000, 0x400000, 0xd9115327),
  LOAD_16_8( SPRITES, "248-c2.c2", 0x000001, 0x400000, 0x77f9fdac),
  LOAD_16_8( SPRITES, "248-c3.c3", 0x800000, 0x100000, 0x8890bf7c),
  LOAD_16_8( SPRITES, "248-c4.c4", 0x800001, 0x100000, 0x8efead3f),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( pbobbl2n, neogeo, "Puzzle Bobble 2 / Bust-A-Move Again (Neo-Geo)", TAITO, 1999, GAME_PUZZLE);

static struct ROM_INFO rom_ctomaday[] = /* MVS ONLY RELEASE */
{
  LOAD_SW16( CPU1, "249-p1.p1", 0x100000, 0x100000, 0xc9386118),
  { "249-p1.p1", 0x100000 , 0xc9386118, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD( FIXED, "249-s1.s1", 0x000000, 0x20000, 0xdc9eb372),
  LOAD( ROM2, "249-m1.m1", 0x00000, 0x20000, 0x80328a47),
  LOAD( SMP1, "249-v1.v1", 0x000000, 0x400000, 0xde7c8f27),
  LOAD( SMP1, "249-v2.v2", 0x400000, 0x100000, 0xc8e40119),
  LOAD_16_8( SPRITES, "249-c1.c1", 0x000000, 0x400000, 0x041fb8ee),
  LOAD_16_8( SPRITES, "249-c2.c2", 0x000001, 0x400000, 0x74f3cdf4),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( ctomaday, neogeo, "Captain Tomaday", VISCO, 1999, GAME_SHOOT);

static struct ROM_INFO rom_mslugx[] = /* MVS AND AES VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "250-p1.p1", 0x000000, 0x100000, 0x81f1f60b),
  LOAD_SW16( CPU1, "250-p2.ep1", 0x100000, 0x400000, 0x1fda2e12),
  LOAD( FIXED, "250-s1.s1", 0x000000, 0x20000, 0xfb6f441d),
  LOAD( ROM2, "250-m1.m1", 0x00000, 0x20000, 0xfd42a842),
  LOAD( SMP1, "250-v1.v1", 0x000000, 0x400000, 0xc79ede73),
  LOAD( SMP1, "250-v2.v2", 0x400000, 0x400000, 0xea9aabe1),
  LOAD( SMP1, "250-v3.v3", 0x800000, 0x200000, 0x2ca65102),
  LOAD_16_8( SPRITES, "250-c1.c1", 0x0000000, 0x800000, 0x09a52c6f),
  LOAD_16_8( SPRITES, "250-c2.c2", 0x0000001, 0x800000, 0x31679821),
  LOAD_16_8( SPRITES, "250-c3.c3", 0x1000000, 0x800000, 0xfd602019),
  LOAD_16_8( SPRITES, "250-c4.c4", 0x1000001, 0x800000, 0x31354513),
  LOAD_16_8( SPRITES, "250-c5.c5", 0x2000000, 0x800000, 0xa4b56124),
  LOAD_16_8( SPRITES, "250-c6.c6", 0x2000001, 0x800000, 0x83e3e69d),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( mslugx, neogeo, "Metal Slug X - Super Vehicle-001 (NGM-2500)(NGH-2500)", SNK, 1999, GAME_SHOOT);

static struct ROM_INFO rom_kof99[] = /* Original Version - Encrypted Code & GFX clone of neogeo */
 /* MVS VERSION */{
  LOAD_SW16( CPU1, "ka.neo-sma", 0x0c0000, 0x040000, 0x7766d09e),
  LOAD_SW16( CPU1, "251-p1.p1", 0x100000, 0x400000, 0x006e4532),
  LOAD_SW16( CPU1, "251-p2.p2", 0x500000, 0x400000, 0x90175f15),
	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
 FILL( FIXED , 0x000000, 0x20000, 0),
  LOAD( ROM2, "251-m1.m1", 0x00000, 0x20000, 0x5e74539c),
  LOAD( SMP1, "251-v1.v1", 0x000000, 0x400000, 0xef2eecc8),
  LOAD( SMP1, "251-v2.v2", 0x400000, 0x400000, 0x73e211ca),
  LOAD( SMP1, "251-v3.v3", 0x800000, 0x400000, 0x821901da),
  LOAD( SMP1, "251-v4.v4", 0xc00000, 0x200000, 0xb49e6178),
	/* Encrypted */
  LOAD_16_8( SPRITES, "251-c1.c1", 0x0000000, 0x800000, 0x0f9e93fe),
  LOAD_16_8( SPRITES, "251-c2.c2", 0x0000001, 0x800000, 0xe71e2ea3),
  LOAD_16_8( SPRITES, "251-c3.c3", 0x1000000, 0x800000, 0x238755d2),
  LOAD_16_8( SPRITES, "251-c4.c4", 0x1000001, 0x800000, 0x438c8b22),
  LOAD_16_8( SPRITES, "251-c5.c5", 0x2000000, 0x800000, 0x0b0abd0a),
  LOAD_16_8( SPRITES, "251-c6.c6", 0x2000001, 0x800000, 0x65bbf281),
  LOAD_16_8( SPRITES, "251-c7.c7", 0x3000000, 0x800000, 0xff65f62e),
  LOAD_16_8( SPRITES, "251-c8.c8", 0x3000001, 0x800000, 0x8d921c68),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kof99, neogeo, "The King of Fighters '99 - Millennium Battle (NGM-2510)" , SNK, 1999, GAME_BEAT);

static struct ROM_INFO rom_kof99h[] = /* Original Version - Encrypted Code & GFX clone of kof99 */
 /* AES VERSION */{
  LOAD_SW16( CPU1, "kc.neo-sma", 0x0c0000, 0x040000, 0x6c9d0647),
  LOAD_SW16( CPU1, "251-p1.p1", 0x100000, 0x400000, 0x006e4532),
  LOAD_SW16( CPU1, "251-pg2.p2", 0x500000, 0x400000, 0xd9057f51),
	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
/* Ignored : 	ROM_FILL( 0x000000, 0x20000, 0 ) */
	/* Encrypted */
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kof99h, kof99, "The King of Fighters '99 - Millennium Battle (NGH-2510)" , SNK, 1999, GAME_BEAT);

static struct ROM_INFO rom_ganryu[] = /* Original Version - Encrypted GFX */ /* MVS ONLY RELEASE */
{
  LOAD_SW16( CPU1, "252-p1.p1", 0x100000, 0x100000, 0x4b8ac4fb),
  { "252-p1.p1", 0x100000 , 0x4b8ac4fb, REGION_CPU1, 0x000000, LOAD_CONTINUE },
	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
  FILL( FIXED, 0x000000, 0x20000, 0),
  LOAD( ROM2, "252-m1.m1", 0x00000, 0x20000, 0x30cc4099),
  LOAD( SMP1, "252-v1.v1", 0x000000, 0x400000, 0xe5946733),
	/* Encrypted */
  LOAD_16_8( SPRITES, "252-c1.c1", 0x0000000, 0x800000, 0x50ee7882),
  LOAD_16_8( SPRITES, "252-c2.c2", 0x0000001, 0x800000, 0x62585474),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( ganryu, neogeo, "Ganryu / Musashi Ganryuki" , VISCO, 1999, GAME_BEAT);

static struct ROM_INFO rom_garou[] = /* Original Version - Encrypted GFX clone of neogeo */
 /* MVS VERSION - later revision */{
  LOAD_SW16( CPU1, "kf.neo-sma", 0x0c0000, 0x040000, 0x98bc93dc),
  LOAD_SW16( CPU1, "253-ep1.p1", 0x100000, 0x200000, 0xea3171a4),
  LOAD_SW16( CPU1, "253-ep2.p2", 0x300000, 0x200000, 0x382f704b),
  LOAD_SW16( CPU1, "253-ep3.p3", 0x500000, 0x200000, 0xe395bfdd),
  LOAD_SW16( CPU1, "253-ep4.p4", 0x700000, 0x200000, 0xda92c08e),
	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
  FILL( FIXED, 0x000000, 0x80000, 0),
  LOAD( ROM2, "253-m1.m1", 0x00000, 0x40000, 0x36a806be),
  LOAD( SMP1, "253-v1.v1", 0x000000, 0x400000, 0x263e388c),
  LOAD( SMP1, "253-v2.v2", 0x400000, 0x400000, 0x2c6bc7be),
  LOAD( SMP1, "253-v3.v3", 0x800000, 0x400000, 0x0425b27d),
  LOAD( SMP1, "253-v4.v4", 0xc00000, 0x400000, 0xa54be8a9),
	/* Encrypted */
  LOAD_16_8( SPRITES, "253-c1.c1", 0x0000000, 0x800000, 0x0603e046),
  LOAD_16_8( SPRITES, "253-c2.c2", 0x0000001, 0x800000, 0x0917d2a4),
  LOAD_16_8( SPRITES, "253-c3.c3", 0x1000000, 0x800000, 0x6737c92d),
  LOAD_16_8( SPRITES, "253-c4.c4", 0x1000001, 0x800000, 0x5ba92ec6),
  LOAD_16_8( SPRITES, "253-c5.c5", 0x2000000, 0x800000, 0x3eab5557),
  LOAD_16_8( SPRITES, "253-c6.c6", 0x2000001, 0x800000, 0x308d098b),
  LOAD_16_8( SPRITES, "253-c7.c7", 0x3000000, 0x800000, 0xc0e995ae),
  LOAD_16_8( SPRITES, "253-c8.c8", 0x3000001, 0x800000, 0x21a11303),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( garou, neogeo, "Garou - Mark of the Wolves (NGM-2530)" , SNK, 1999, GAME_BEAT);

static struct ROM_INFO rom_garouh[] = /* Original Version - Encrypted GFX clone of garou */
 /* MVS AND AES VERSION - earlier revision */{
  LOAD_SW16( CPU1, "ke.neo-sma", 0x0c0000, 0x040000, 0x96c72233),
  LOAD_SW16( CPU1, "253-p1.p1", 0x100000, 0x400000, 0x18ae5d7e),
  LOAD_SW16( CPU1, "253-p2.p2", 0x500000, 0x400000, 0xafffa779),
	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
/* Ignored : 	ROM_FILL( 0x000000, 0x20000, 0 ) */
	/* Encrypted */
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( garouh, garou, "Garou - Mark of the Wolves (NGM-2530)(NGH-2530)" , SNK, 1999, GAME_BEAT);

static struct ROM_INFO rom_s1945p[] = /* Original Version, Encrypted GFX Roms */ /* MVS ONLY RELEASE */
{
  LOAD_SW16( CPU1, "254-p1.p1", 0x000000, 0x100000, 0xff8efcff),
  LOAD_SW16( CPU1, "254-p2.sp2", 0x100000, 0x400000, 0xefdfd4dd),
	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
  FILL( FIXED, 0x000000, 0x20000, 0),
  LOAD( ROM2, "254-m1.m1", 0x00000, 0x20000, 0x994b4487),
  LOAD( SMP1, "254-v1.v1", 0x000000, 0x400000, 0x844f58fb),
  LOAD( SMP1, "254-v2.v2", 0x400000, 0x400000, 0xd9a248f0),
  LOAD( SMP1, "254-v3.v3", 0x800000, 0x400000, 0x0b0d2d33),
  LOAD( SMP1, "254-v4.v4", 0xc00000, 0x400000, 0x6d13dc91),
  LOAD_16_8( SPRITES, "254-c1.c1", 0x0000000, 0x800000, 0xae6fc8ef),
  LOAD_16_8( SPRITES, "254-c2.c2", 0x0000001, 0x800000, 0x436fa176),
  LOAD_16_8( SPRITES, "254-c3.c3", 0x1000000, 0x800000, 0xe53ff2dc),
  LOAD_16_8( SPRITES, "254-c4.c4", 0x1000001, 0x800000, 0x818672f0),
  LOAD_16_8( SPRITES, "254-c5.c5", 0x2000000, 0x800000, 0x4580eacd),
  LOAD_16_8( SPRITES, "254-c6.c6", 0x2000001, 0x800000, 0xe34970fc),
  LOAD_16_8( SPRITES, "254-c7.c7", 0x3000000, 0x800000, 0xf2323239),
  LOAD_16_8( SPRITES, "254-c8.c8", 0x3000001, 0x800000, 0x66848c7d),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( s1945p, neogeo, "Strikers 1945 Plus" , PSIKYO, 1999, GAME_SHOOT);

static struct ROM_INFO rom_preisle2[] = /* Original Version, Encrypted GFX */ /* MVS ONLY RELEASE */
{
  LOAD_SW16( CPU1, "255-p1.p1", 0x000000, 0x100000, 0xdfa3c0f3),
  LOAD_SW16( CPU1, "255-p2.sp2", 0x100000, 0x400000, 0x42050b80),
	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
  FILL( FIXED , 0x000000, 0x20000, 0),
  LOAD( ROM2, "255-m1.m1", 0x00000, 0x20000, 0x8efd4014),
  LOAD( SMP1, "255-v1.v1", 0x000000, 0x400000, 0x5a14543d),
  LOAD( SMP1, "255-v2.v2", 0x400000, 0x200000, 0x6610d91a),
	/* Encrypted */
  LOAD_16_8( SPRITES, "255-c1.c1", 0x0000000, 0x800000, 0xea06000b),
  LOAD_16_8( SPRITES, "255-c2.c2", 0x0000001, 0x800000, 0x04e67d79),
  LOAD_16_8( SPRITES, "255-c3.c3", 0x1000000, 0x800000, 0x60e31e08),
  LOAD_16_8( SPRITES, "255-c4.c4", 0x1000001, 0x800000, 0x40371d69),
  LOAD_16_8( SPRITES, "255-c5.c5", 0x2000000, 0x800000, 0x0b2e6adf),
  LOAD_16_8( SPRITES, "255-c6.c6", 0x2000001, 0x800000, 0xb001bdd3),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( preisle2, neogeo, "Prehistoric Isle 2" , YUMEKOBO, 1999, GAME_SHOOT);

static struct ROM_INFO rom_mslug3[] = /* Original Version - Encrypted Code & GFX clone of neogeo */
 /* revision 2000.4.1 */ /* MVS VERSION */{
	/* The SMA for this release has a green colour marking; the older revision has a white colour marking */
  LOAD_SW16( CPU1, "neo-sma", 0x0c0000, 0x040000, 0x9cd55736),
  LOAD_SW16( CPU1, "256-pg1.p1", 0x100000, 0x400000, 0xb07edfd5),
  LOAD_SW16( CPU1, "256-pg2.p2", 0x500000, 0x400000, 0x6097c26b),
	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
  FILL( FIXED , 0x000000, 0x80000, 0),
  LOAD( ROM2, "256-m1.m1", 0x00000, 0x80000, 0xeaeec116),
  LOAD( SMP1, "256-v1.v1", 0x000000, 0x400000, 0xf2690241),
  LOAD( SMP1, "256-v2.v2", 0x400000, 0x400000, 0x7e2a10bd),
  LOAD( SMP1, "256-v3.v3", 0x800000, 0x400000, 0x0eaec17c),
  LOAD( SMP1, "256-v4.v4", 0xc00000, 0x400000, 0x9b4b22d4),
	/* Encrypted */
  LOAD_16_8( SPRITES, "256-c1.c1", 0x0000000, 0x800000, 0x5a79c34e),
  LOAD_16_8( SPRITES, "256-c2.c2", 0x0000001, 0x800000, 0x944c362c),
  LOAD_16_8( SPRITES, "256-c3.c3", 0x1000000, 0x800000, 0x6e69d36f),
  LOAD_16_8( SPRITES, "256-c4.c4", 0x1000001, 0x800000, 0xb755b4eb),
  LOAD_16_8( SPRITES, "256-c5.c5", 0x2000000, 0x800000, 0x7aacab47),
  LOAD_16_8( SPRITES, "256-c6.c6", 0x2000001, 0x800000, 0xc698fd5d),
  LOAD_16_8( SPRITES, "256-c7.c7", 0x3000000, 0x800000, 0xcfceddd2),
  LOAD_16_8( SPRITES, "256-c8.c8", 0x3000001, 0x800000, 0x4d9be34c),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( mslug3, neogeo, "Metal Slug 3 (NGM-2560)" , SNK, 2000, GAME_SHOOT);

static struct ROM_INFO rom_mslug3h[] = /* Original Version - Encrypted GFX clone of mslug3 */
 /* revision 2000.3.17 */ /* AES VERSION */{
  LOAD_SW16( CPU1, "256-ph1.p1", 0x000000, 0x100000, 0x9c42ca85),
  LOAD_SW16( CPU1, "256-ph2.sp2", 0x100000, 0x400000, 0x1f3d8ce8),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( mslug3h, mslug3, "Metal Slug 3 (NGH-2560)" , SNK, 2000, GAME_SHOOT);

static struct ROM_INFO rom_kof2000[] = /* Original Version, Encrypted Code + Sound + GFX Roms clone of neogeo */
 /* MVS AND AES VERSION */{
  LOAD_SW16( CPU1, "neo-sma", 0x0c0000, 0x040000, 0x71c6e6bb),
  LOAD_SW16( CPU1, "257-p1.p1", 0x100000, 0x400000, 0x60947b4c),
  LOAD_SW16( CPU1, "257-p2.p2", 0x500000, 0x400000, 0x1b7ec415),
	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
  FILL( FIXED , 0x000000, 0x80000, 0),
	/* Encrypted */
  LOAD( AUDIOCRYPT, "257-m1.m1", 0x00000, 0x40000, 0x4b749113),
  FILL( AUDIOCRYPT, 0x40000, 0x40000, 0),

  LOAD( SMP1, "257-v1.v1", 0x000000, 0x400000, 0x17cde847),
  LOAD( SMP1, "257-v2.v2", 0x400000, 0x400000, 0x1afb20ff),
  LOAD( SMP1, "257-v3.v3", 0x800000, 0x400000, 0x4605036a),
  LOAD( SMP1, "257-v4.v4", 0xc00000, 0x400000, 0x764bbd6b),
  LOAD_16_8( SPRITES, "257-c1.c1", 0x0000000, 0x800000, 0xcef1cdfa),
  LOAD_16_8( SPRITES, "257-c2.c2", 0x0000001, 0x800000, 0xf7bf0003),
  LOAD_16_8( SPRITES, "257-c3.c3", 0x1000000, 0x800000, 0x101e6560),
  LOAD_16_8( SPRITES, "257-c4.c4", 0x1000001, 0x800000, 0xbd2fc1b1),
  LOAD_16_8( SPRITES, "257-c5.c5", 0x2000000, 0x800000, 0x89775412),
  LOAD_16_8( SPRITES, "257-c6.c6", 0x2000001, 0x800000, 0xfa7200d5),
  LOAD_16_8( SPRITES, "257-c7.c7", 0x3000000, 0x800000, 0x7da11fe4),
  LOAD_16_8( SPRITES, "257-c8.c8", 0x3000001, 0x800000, 0xb1afa60b),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kof2000, neogeo, "The King of Fighters 2000 (NGM-2570) (NGH-2570)" , SNK, 2000, GAME_BEAT);

static struct ROM_INFO rom_bangbead[] = /* Original Version - Encrypted GFX */ /* MVS ONLY RELEASE */
{
  LOAD_SW16( CPU1, "259-p1.p1", 0x100000, 0x100000, 0x88a37f8b),
  { "259-p1.p1", 0x100000 , 0x88a37f8b, REGION_CPU1, 0x000000, LOAD_CONTINUE },
	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
  FILL( FIXED , 0x000000, 0x20000, 0),
  LOAD( ROM2, "259-m1.m1", 0x00000, 0x20000, 0x85668ee9),
  LOAD( SMP1, "259-v1.v1", 0x000000, 0x400000, 0x088eb8ab),
  LOAD( SMP1, "259-v2.v2", 0x400000, 0x100000, 0x97528fe9),
	/* Encrypted */
  LOAD_16_8( SPRITES, "259-c1.c1", 0x0000000, 0x800000, 0x1f537f74),
  LOAD_16_8( SPRITES, "259-c2.c2", 0x0000001, 0x800000, 0x0efd98ff),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( bangbead, neogeo, "Bang Bead", VISCO, 2000, GAME_SPORTS);

static struct ROM_INFO rom_nitd[] = /* Original Version - Encrypted GFX */ /* MVS ONLY RELEASE */
{
  LOAD_SW16( CPU1, "260-p1.p1", 0x000000, 0x080000, 0x61361082),
	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
  FILL( FIXED , 0x000000, 0x20000, 0),
  LOAD( ROM2, "260-m1.m1", 0x00000, 0x80000, 0x6407c5e5),
  LOAD( SMP1, "260-v1.v1", 0x000000, 0x400000, 0x24b0480c),
	/* Encrypted */
  LOAD_16_8( SPRITES, "260-c1.c1", 0x0000000, 0x800000, 0x147b0c7f),
  LOAD_16_8( SPRITES, "260-c2.c2", 0x0000001, 0x800000, 0xd2b04b0d),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( nitd, neogeo, "Nightmare in the Dark" , ELEVEN, 2000, GAME_PLATFORM);

static struct ROM_INFO rom_sengoku3[] = /* Original Version - Encrypted GFX */
{
  LOAD_SW16( CPU1, "261-ph1.p1", 0x100000, 0x100000, 0xe0d4bc0a),
  { "261-ph1.p1", 0x100000 , 0xe0d4bc0a, REGION_CPU1, 0x000000, LOAD_CONTINUE },
	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
  FILL( FIXED , 0x000000, 0x20000, 0),
  LOAD( ROM2, "261-m1.m1", 0x00000, 0x80000, 0x7d501c39),
  LOAD( SMP1, "261-v1.v1", 0x000000, 0x400000, 0x64c30081),
  LOAD( SMP1, "261-v2.v2", 0x400000, 0x400000, 0x392a9c47),
  LOAD( SMP1, "261-v3.v3", 0x800000, 0x400000, 0xc1a7ebe3),
  LOAD( SMP1, "261-v4.v4", 0xc00000, 0x200000, 0x9000d085),
  LOAD_16_8( SPRITES, "261-c1.c1", 0x0000000, 0x800000, 0xded84d9c),
  LOAD_16_8( SPRITES, "261-c2.c2", 0x0000001, 0x800000, 0xb8eb4348),
  LOAD_16_8( SPRITES, "261-c3.c3", 0x1000000, 0x800000, 0x84e2034a),
  LOAD_16_8( SPRITES, "261-c4.c4", 0x1000001, 0x800000, 0x0b45ae53),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( sengoku3, neogeo, "Sengoku 3 / Sengoku Densho 2001" , NOISE, 2001, GAME_BEAT);

static struct ROM_INFO rom_kof2001[] = /* MVS VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "262-p1-08-e0.p1", 0x000000, 0x100000, 0x9381750d),
	/* The first/early production run sets have proms with above labels. Some later? sets found have eproms instead of proms */
  LOAD_SW16( CPU1, "262-p2-08-e0.sp2", 0x100000, 0x400000, 0x8e0d8329),
  FILL( FIXED , 0x000000, 0x20000, 0),
	/* Encrypted */
  LOAD( AUDIOCRYPT, "265-262-m1.m1", 0x00000, 0x40000, 0xa7f8119f),
  FILL( AUDIOCRYPT, 0x40000, 0x40000, 0),
  LOAD( SMP1, "262-v1-08-e0.v1", 0x000000, 0x400000, 0x83d49ecf),
  LOAD( SMP1, "262-v2-08-e0.v2", 0x400000, 0x400000, 0x003f1843),
  LOAD( SMP1, "262-v3-08-e0.v3", 0x800000, 0x400000, 0x2ae38dbe),
  LOAD( SMP1, "262-v4-08-e0.v4", 0xc00000, 0x400000, 0x26ec4dd9),
	/* Encrypted */
  LOAD_16_8( SPRITES, "262-c1-08-e0.c1", 0x0000000, 0x800000, 0x99cc785a),
  LOAD_16_8( SPRITES, "262-c2-08-e0.c2", 0x0000001, 0x800000, 0x50368cbf),
  LOAD_16_8( SPRITES, "262-c3-08-e0.c3", 0x1000000, 0x800000, 0xfb14ff87),
  LOAD_16_8( SPRITES, "262-c4-08-e0.c4", 0x1000001, 0x800000, 0x4397faf8),
  LOAD_16_8( SPRITES, "262-c5-08-e0.c5", 0x2000000, 0x800000, 0x91f24be4),
  LOAD_16_8( SPRITES, "262-c6-08-e0.c6", 0x2000001, 0x800000, 0xa31e4403),
  LOAD_16_8( SPRITES, "262-c7-08-e0.c7", 0x3000000, 0x800000, 0x54d9d1ec),
  LOAD_16_8( SPRITES, "262-c8-08-e0.c8", 0x3000001, 0x800000, 0x59289a6b),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kof2001, neogeo, "The King of Fighters 2001 (NGM-262?)" , EOLITH_S, 2001, GAME_BEAT);

static struct ROM_INFO rom_kof2001h[] = /* AES VERSION clone of kof2001 */
{
  LOAD_SW16( CPU1, "262-pg1.p1", 0x000000, 0x100000, 0x2af7e741),
  LOAD_SW16( CPU1, "262-pg2.sp2", 0x100000, 0x400000, 0x91eea062),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kof2001h, kof2001, "The King of Fighters 2001 (NGH-2621)" , EOLITH_S, 2001, GAME_BEAT);

static struct ROM_INFO rom_mslug4[] = /* Original Version - Encrypted GFX clone of neogeo */
 /* MVS VERSION */{
  LOAD_SW16( CPU1, "263-p1.p1", 0x000000, 0x100000, 0x27e4def3),
  LOAD_SW16( CPU1, "263-p2.sp2", 0x100000, 0x400000, 0xfdb7aed8),
	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
  FILL( FIXED , 0x000000, 0x80000, 0),
	/* Encrypted */
  LOAD( AUDIOCRYPT, "263-m1.m1", 0x00000, 0x20000, 0x46ac8228),
  FILL( AUDIOCRYPT, 0x20000, 0x60000, 0),
	/* Encrypted */
  LOAD( SMP1, "263-v1.v1", 0x000000, 0x800000, 0x01e9b9cd),
  LOAD( SMP1, "263-v2.v2", 0x800000, 0x800000, 0x4ab2bf81),
	/* Encrypted */
  LOAD_16_8( SPRITES, "263-c1.c1", 0x0000000, 0x800000, 0x84865f8a),
  LOAD_16_8( SPRITES, "263-c2.c2", 0x0000001, 0x800000, 0x81df97f2),
  LOAD_16_8( SPRITES, "263-c3.c3", 0x1000000, 0x800000, 0x1a343323),
  LOAD_16_8( SPRITES, "263-c4.c4", 0x1000001, 0x800000, 0x942cfb44),
  LOAD_16_8( SPRITES, "263-c5.c5", 0x2000000, 0x800000, 0xa748854f),
  LOAD_16_8( SPRITES, "263-c6.c6", 0x2000001, 0x800000, 0x5c8ba116),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( mslug4, neogeo, "Metal Slug 4 (NGM-2630)", MEGA_P, 2002, GAME_SHOOT);

static struct ROM_INFO rom_mslug4h[] = /* Original Version - Encrypted GFX clone of mslug4 */
 /* AES VERSION */{
  LOAD_SW16( CPU1, "263-ph1.p1", 0x000000, 0x100000, 0xc67f5c8d),
  LOAD_SW16( CPU1, "263-ph2.sp2", 0x100000, 0x400000, 0xbc3ec89e),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( mslug4h, mslug4, "Metal Slug 4 (NGH-2630)", MEGA_P, 2002, GAME_SHOOT);

static struct ROM_INFO rom_rotd[] = /* Encrypted Set clone of neogeo */
 /* MVS VERSION */{
  LOAD_SW16( CPU1, "264-p1.p1", 0x000000, 0x800000, 0xb8cc969d),
	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
  FILL( FIXED , 0x000000, 0x20000, 0),
	/* Encrypted */
  LOAD( AUDIOCRYPT, "264-m1.m1", 0x00000, 0x20000, 0x4dbd7b43),
  FILL( AUDIOCRYPT, 0x20000, 0x60000, 0),
	/* Encrypted */
  LOAD( SMP1, "264-v1.v1", 0x000000, 0x800000, 0xfa005812),
  LOAD( SMP1, "264-v2.v2", 0x800000, 0x800000, 0xc3dc8bf0),
	/* Encrypted */
  LOAD_16_8( SPRITES, "264-c1.c1", 0x0000000, 0x800000, 0x4f148fee),
  LOAD_16_8( SPRITES, "264-c2.c2", 0x0000001, 0x800000, 0x7cf5ff72),
  LOAD_16_8( SPRITES, "264-c3.c3", 0x1000000, 0x800000, 0x64d84c98),
  LOAD_16_8( SPRITES, "264-c4.c4", 0x1000001, 0x800000, 0x2f394a95),
  LOAD_16_8( SPRITES, "264-c5.c5", 0x2000000, 0x800000, 0x6b99b978),
  LOAD_16_8( SPRITES, "264-c6.c6", 0x2000001, 0x800000, 0x847d5c7d),
  LOAD_16_8( SPRITES, "264-c7.c7", 0x3000000, 0x800000, 0x231d681e),
  LOAD_16_8( SPRITES, "264-c8.c8", 0x3000001, 0x800000, 0xc5edb5c4),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( rotd, neogeo, "Rage of the Dragons (NGM-264?)", EVOGA_P, 2002, GAME_BEAT);

static struct ROM_INFO rom_kof2002[] = /* Encrypted Set clone of neogeo */
 /* MVS AND AES VERSION */{
  LOAD_SW16( CPU1, "265-p1.p1", 0x000000, 0x100000, 0x9ede7323),
  LOAD_SW16( CPU1, "265-p2.sp2", 0x100000, 0x400000, 0x327266b8),
	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
  FILL( FIXED , 0x000000, 0x20000, 0),
	/* Encrypted */
  LOAD( AUDIOCRYPT, "265-m1.m1", 0x00000, 0x20000, 0x85aaa632),
  FILL( AUDIOCRYPT , 0x20000, 0x60000, 0),
	/* Encrypted */
  LOAD( SMP1, "265-v1.v1", 0x000000, 0x800000, 0x15e8f3f5),
  LOAD( SMP1, "265-v2.v2", 0x800000, 0x800000, 0xda41d6f9),
	/* Encrypted */
  LOAD_16_8( SPRITES, "265-c1.c1", 0x0000000, 0x800000, 0x2b65a656),
  LOAD_16_8( SPRITES, "265-c2.c2", 0x0000001, 0x800000, 0xadf18983),
  LOAD_16_8( SPRITES, "265-c3.c3", 0x1000000, 0x800000, 0x875e9fd7),
  LOAD_16_8( SPRITES, "265-c4.c4", 0x1000001, 0x800000, 0x2da13947),
  LOAD_16_8( SPRITES, "265-c5.c5", 0x2000000, 0x800000, 0x61bd165d),
  LOAD_16_8( SPRITES, "265-c6.c6", 0x2000001, 0x800000, 0x03fdd1eb),
  LOAD_16_8( SPRITES, "265-c7.c7", 0x3000000, 0x800000, 0x1a2749d8),
  LOAD_16_8( SPRITES, "265-c8.c8", 0x3000001, 0x800000, 0xab0bb549),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kof2002, neogeo, "The King of Fighters 2002 (NGM-2650)(NGH-2650)" , EOLITH_P, 2002, GAME_BEAT);

static struct ROM_INFO rom_matrim[] = /* Encrypted Set clone of neogeo */
 /* MVS AND AES VERSION */{
  LOAD_SW16( CPU1, "266-p1.p1", 0x000000, 0x100000, 0x5d4c2dc7),
  LOAD_SW16( CPU1, "266-p2.sp2", 0x100000, 0x400000, 0xa14b1906),
	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
  FILL( FIXED , 0x000000, 0x80000, 0),
	/* Encrypted */
  LOAD( AUDIOCRYPT, "266-m1.m1", 0x00000, 0x20000, 0x456c3e6c),
  FILL( AUDIOCRYPT , 0x020000, 0x60000, 0),
	/* Encrypted */
  LOAD( SMP1, "266-v1.v1", 0x000000, 0x800000, 0xa4f83690),
  LOAD( SMP1, "266-v2.v2", 0x800000, 0x800000, 0xd0f69eda),
	/* Encrypted */
  LOAD_16_8( SPRITES, "266-c1.c1", 0x0000000, 0x800000, 0x505f4e30),
  LOAD_16_8( SPRITES, "266-c2.c2", 0x0000001, 0x800000, 0x3cb57482),
  LOAD_16_8( SPRITES, "266-c3.c3", 0x1000000, 0x800000, 0xf1cc6ad0),
  LOAD_16_8( SPRITES, "266-c4.c4", 0x1000001, 0x800000, 0x45b806b7),
  LOAD_16_8( SPRITES, "266-c5.c5", 0x2000000, 0x800000, 0x9a15dd6b),
  LOAD_16_8( SPRITES, "266-c6.c6", 0x2000001, 0x800000, 0x281cb939),
  LOAD_16_8( SPRITES, "266-c7.c7", 0x3000000, 0x800000, 0x4b71f780),
  LOAD_16_8( SPRITES, "266-c8.c8", 0x3000001, 0x800000, 0x29873d33),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( matrim, neogeo, "Matrimelee / Shin Gouketsuji Ichizoku Toukon (NGM-2660) (NGH-2660)", NOISE, 2002, GAME_BEAT);

static struct ROM_INFO rom_pnyaa[] = /* Encrypted Set */ /* MVS ONLY RELEASE */
{
  LOAD_SW16( CPU1, "267-p1.p1", 0x000000, 0x100000, 0x112fe2c0),
	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
  FILL( FIXED , 0x000000, 0x20000, 0),
	/* Encrypted */
  LOAD( AUDIOCRYPT, "267-m1.m1", 0x00000, 0x80000, 0xc7853ccd),
	/* Encrypted */
  LOAD( SMP1, "267-v1.v1", 0x000000, 0x400000, 0xe2e8e917),
	/* Encrypted */
  LOAD_16_8( SPRITES, "267-c1.c1", 0x0000000, 0x800000, 0x5eebee65),
  LOAD_16_8( SPRITES, "267-c2.c2", 0x0000001, 0x800000, 0x2b67187b),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( pnyaa, neogeo, "Pochi and Nyaa", AIKY, 2003, GAME_PUZZLE);

static struct ROM_INFO rom_mslug5[] = /* Encrypted Set clone of neogeo */
 /* MVS VERSION */{
  LOAD_32_SWAP_16( CPU1, "268-p1cr.p1", 0x000000, 0x400000, 0xd0466792),
  LOAD_32_SWAP_16( CPU1, "268-p2cr.p2", 0x000002, 0x400000, 0xfbf6b61e),
	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
  FILL( FIXED , 0x000000, 0x20000, 0),
	/* Encrypted */
  LOAD( AUDIOCRYPT, "268-m1.m1", 0x00000, 0x80000, 0x4a5a6e0e),
	/* Encrypted */
  LOAD( SMP1, "268-v1c.v1", 0x000000, 0x800000, 0xae31d60c),
  LOAD( SMP1, "268-v2c.v2", 0x800000, 0x800000, 0xc40613ed),
	/* Encrypted */
  LOAD_16_8( SPRITES, "268-c1c.c1", 0x0000000, 0x800000, 0xab7c389a),
  LOAD_16_8( SPRITES, "268-c2c.c2", 0x0000001, 0x800000, 0x3560881b),
  LOAD_16_8( SPRITES, "268-c3c.c3", 0x1000000, 0x800000, 0x3af955ea),
  LOAD_16_8( SPRITES, "268-c4c.c4", 0x1000001, 0x800000, 0xc329c373),
  LOAD_16_8( SPRITES, "268-c5c.c5", 0x2000000, 0x800000, 0x959c8177),
  LOAD_16_8( SPRITES, "268-c6c.c6", 0x2000001, 0x800000, 0x010a831b),
  LOAD_16_8( SPRITES, "268-c7c.c7", 0x3000000, 0x800000, 0x6d72a969),
  LOAD_16_8( SPRITES, "268-c8c.c8", 0x3000001, 0x800000, 0x551d720e),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( mslug5, neogeo, "Metal Slug 5 (NGM-2680)", SNK_PLAYMORE, 2003, GAME_SHOOT);

static struct ROM_INFO rom_mslug5h[] = /* Encrypted Set clone of mslug5 */
 /* AES release of the game but is also found in later MVS carts */{
  LOAD_32_SWAP_16( CPU1, "268-p1c.p1", 0x000000, 0x400000, 0x3636690a),
  LOAD_32_SWAP_16( CPU1, "268-p2c.p2", 0x000002, 0x400000, 0x8dfc47a2),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( mslug5h, mslug5, "Metal Slug 5 (NGH-2680)", SNK_PLAYMORE, 2003, GAME_SHOOT);

static struct ROM_INFO rom_svc[] = /* Encrypted Set clone of neogeo */
 /* MVS AND AES VERSION */{
  LOAD_32_SWAP_16( CPU1, "269-p1.p1", 0x000000, 0x400000, 0x38e2005e),
  LOAD_32_SWAP_16( CPU1, "269-p2.p2", 0x000002, 0x400000, 0x6d13797c),
	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
  FILL( FIXED , 0x000000, 0x80000, 0),
	/* Encrypted */
  LOAD( AUDIOCRYPT, "269-m1.m1", 0x00000, 0x80000, 0xf6819d00),
	/* Encrypted */
  LOAD( SMP1, "269-v1.v1", 0x000000, 0x800000, 0xc659b34c),
  LOAD( SMP1, "269-v2.v2", 0x800000, 0x800000, 0xdd903835),
	/* Encrypted */
  LOAD_16_8( SPRITES, "269-c1r.c1", 0x0000000, 0x800000, 0x887b4068),
  LOAD_16_8( SPRITES, "269-c2r.c2", 0x0000001, 0x800000, 0x4e8903e4),
  LOAD_16_8( SPRITES, "269-c3r.c3", 0x1000000, 0x800000, 0x7d9c55b0),
  LOAD_16_8( SPRITES, "269-c4r.c4", 0x1000001, 0x800000, 0x8acb5bb6),
  LOAD_16_8( SPRITES, "269-c5r.c5", 0x2000000, 0x800000, 0x097a4157),
  LOAD_16_8( SPRITES, "269-c6r.c6", 0x2000001, 0x800000, 0xe19df344),
  LOAD_16_8( SPRITES, "269-c7r.c7", 0x3000000, 0x800000, 0xd8f0340b),
  LOAD_16_8( SPRITES, "269-c8r.c8", 0x3000001, 0x800000, 0x2570b71b),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( svc, neogeo, "SNK vs. Capcom - SVC Chaos (NGM-2690)(NGH-2690)", SNK_PLAYMORE, 2003, GAME_BEAT);

static struct ROM_INFO rom_samsho5[] = /* Encrypted Set clone of neogeo */
 /* MVS VERSION */{
  LOAD_SW16( CPU1, "270-p1.p1", 0x000000, 0x400000, 0x4a2a09e6),
  LOAD_SW16( CPU1, "270-p2.sp2", 0x400000, 0x400000, 0xe0c74c85),
	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
  FILL( FIXED , 0x000000, 0x20000, 0),
	/* Encrypted */
  LOAD( AUDIOCRYPT, "270-m1.m1", 0x00000, 0x80000, 0x49c9901a),
	/* Encrypted */
  LOAD( SMP1, "270-v1.v1", 0x000000, 0x800000, 0x62e434eb),
  LOAD( SMP1, "270-v2.v2", 0x800000, 0x800000, 0x180f3c9a),
	/* Encrypted */
  LOAD_16_8( SPRITES, "270-c1.c1", 0x0000000, 0x800000, 0x14ffffac),
  LOAD_16_8( SPRITES, "270-c2.c2", 0x0000001, 0x800000, 0x401f7299),
  LOAD_16_8( SPRITES, "270-c3.c3", 0x1000000, 0x800000, 0x838f0260),
  LOAD_16_8( SPRITES, "270-c4.c4", 0x1000001, 0x800000, 0x041560a5),
  LOAD_16_8( SPRITES, "270-c5.c5", 0x2000000, 0x800000, 0xbd30b52d),
  LOAD_16_8( SPRITES, "270-c6.c6", 0x2000001, 0x800000, 0x86a69c70),
  LOAD_16_8( SPRITES, "270-c7.c7", 0x3000000, 0x800000, 0xd28fbc3c),
  LOAD_16_8( SPRITES, "270-c8.c8", 0x3000001, 0x800000, 0x02c530a6),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( samsho5, neogeo, "Samurai Shodown V / Samurai Spirits Zero (NGM-2700)", SNK_PLAYMORE, 2003, GAME_BEAT);

static struct ROM_INFO rom_samsho5h[] = /* Encrypted Set, Alternate Set clone of samsho5 */
 /* AES VERSION */{
  LOAD_SW16( CPU1, "270-p1c.p1", 0x000000, 0x400000, 0xbf956089),
  LOAD_SW16( CPU1, "270-p2c.sp2", 0x400000, 0x400000, 0x943a6b1d),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( samsho5h, samsho5, "Samurai Shodown V / Samurai Spirits Zero (NGH-2700)", SNK_PLAYMORE, 2003, GAME_BEAT);

static struct ROM_INFO rom_kof2003[] = /* Encrypted Code + Sound + GFX Roms clone of neogeo */
 /* MVS VERSION */{
  LOAD_32_SWAP_16( CPU1, "271-p1c.p1", 0x000000, 0x400000, 0x530ecc14),
  LOAD_32_SWAP_16( CPU1, "271-p2c.p2", 0x000002, 0x400000, 0xfd568da9),
  LOAD_SW16( CPU1, "271-p3c.p3", 0x800000, 0x100000, 0xaec5b4a9),
	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
  FILL( FIXED , 0x000000, 0x80000, 0),
	/* Encrypted */
  LOAD( AUDIOCRYPT, "271-m1c.m1", 0x00000, 0x80000, 0xf5515629),
	/* Encrypted */
  LOAD( SMP1, "271-v1c.v1", 0x000000, 0x800000, 0xffa3f8c7),
  LOAD( SMP1, "271-v2c.v2", 0x800000, 0x800000, 0x5382c7d1),
	/* Encrypted */
  LOAD_16_8( SPRITES, "271-c1c.c1", 0x0000000, 0x800000, 0xb1dc25d0),
  LOAD_16_8( SPRITES, "271-c2c.c2", 0x0000001, 0x800000, 0xd5362437),
  LOAD_16_8( SPRITES, "271-c3c.c3", 0x1000000, 0x800000, 0x0a1fbeab),
  LOAD_16_8( SPRITES, "271-c4c.c4", 0x1000001, 0x800000, 0x87b19a0c),
  LOAD_16_8( SPRITES, "271-c5c.c5", 0x2000000, 0x800000, 0x704ea371),
  LOAD_16_8( SPRITES, "271-c6c.c6", 0x2000001, 0x800000, 0x20a1164c),
  LOAD_16_8( SPRITES, "271-c7c.c7", 0x3000000, 0x800000, 0x189aba7f),
  LOAD_16_8( SPRITES, "271-c8c.c8", 0x3000001, 0x800000, 0x20ec4fdc),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kof2003, neogeo, "The King of Fighters 2003 (NGM-2710)", SNK_PLAYMORE, 2003, GAME_BEAT);

static struct ROM_INFO rom_kof2003h[] = /* Encrypted Code + Sound + GFX Roms clone of kof2003 */
 /* AES VERSION */{
  LOAD_32_SWAP_16( CPU1, "271-p1k.p1", 0x000000, 0x400000, 0xd0d0ae3e),
  LOAD_32_SWAP_16( CPU1, "271-p2k.p2", 0x000002, 0x400000, 0xfb3f79d9),
  LOAD_SW16( CPU1, "271-p3k.p3", 0x800000, 0x100000, 0x232702ad),
	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
  FILL( FIXED , 0x000000, 0x80000, 0),
	/* Encrypted */
  LOAD( AUDIOCRYPT, "271-m1k.m1", 0x00000, 0x80000, 0x48d9affe),
	/* Encrypted */
  LOAD_16_8( SPRITES, "271-c1k.c1", 0x0000000, 0x800000, 0xefb9dd24),
  LOAD_16_8( SPRITES, "271-c2k.c2", 0x0000001, 0x800000, 0x3fb90447),
  LOAD_16_8( SPRITES, "271-c3k.c3", 0x1000000, 0x800000, 0x27950f28),
  LOAD_16_8( SPRITES, "271-c4k.c4", 0x1000001, 0x800000, 0x735177f8),
  LOAD_16_8( SPRITES, "271-c5k.c5", 0x2000000, 0x800000, 0xa51b7c0f),
  LOAD_16_8( SPRITES, "271-c6k.c6", 0x2000001, 0x800000, 0xd5cae4e0),
  LOAD_16_8( SPRITES, "271-c7k.c7", 0x3000000, 0x800000, 0xe65ae2d0),
  LOAD_16_8( SPRITES, "271-c8k.c8", 0x3000001, 0x800000, 0x312f528c),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kof2003h, kof2003, "The King of Fighters 2003 (NGH-2710)", SNK_PLAYMORE, 2003, GAME_BEAT);

static struct ROM_INFO rom_samsh5sp[] = /* Encrypted Set clone of neogeo */
 /* MVS VERSION */{
  LOAD_SW16( CPU1, "272-p1.p1", 0x000000, 0x400000, 0xfb7a6bba),
  LOAD_SW16( CPU1, "272-p2.sp2", 0x400000, 0x400000, 0x63492ea6),
	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
  FILL( FIXED , 0x000000, 0x20000, 0),
	/* Encrypted */
  LOAD( AUDIOCRYPT, "272-m1.m1", 0x00000, 0x80000, 0xadeebf40),
	/* Encrypted */
  LOAD( SMP1, "272-v1.v1", 0x000000, 0x800000, 0x76a94127),
  LOAD( SMP1, "272-v2.v2", 0x800000, 0x800000, 0x4ba507f1),
	/* Encrypted */
  LOAD_16_8( SPRITES, "272-c1.c1", 0x0000000, 0x800000, 0x4f97661a),
  LOAD_16_8( SPRITES, "272-c2.c2", 0x0000001, 0x800000, 0xa3afda4f),
  LOAD_16_8( SPRITES, "272-c3.c3", 0x1000000, 0x800000, 0x8c3c7502),
  LOAD_16_8( SPRITES, "272-c4.c4", 0x1000001, 0x800000, 0x32d5e2e2),
  LOAD_16_8( SPRITES, "272-c5.c5", 0x2000000, 0x800000, 0x6ce085bc),
  LOAD_16_8( SPRITES, "272-c6.c6", 0x2000001, 0x800000, 0x05c8dc8e),
  LOAD_16_8( SPRITES, "272-c7.c7", 0x3000000, 0x800000, 0x1417b742),
  LOAD_16_8( SPRITES, "272-c8.c8", 0x3000001, 0x800000, 0xd49773cd),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( samsh5sp, neogeo, "Samurai Shodown V Special / Samurai Spirits Zero Special (NGM-2720)", SNK_PLAYMORE, 2004, GAME_BEAT);

static struct ROM_INFO rom_samsh5fe[] =
{
  LOAD_SW16( CPU1, "272-p1.p1", 0x000000, 0x400000, 0x9e72cc83),
  LOAD_SW16( CPU1, "272-p2.sp2", 0x400000, 0x400000, 0x1f5469bc),
  LOAD( FIXED, "272-s1.bin", 0, 0x020000, 0xc297f973 ),

  LOAD( CPU2, "272-m1d.bin", 0x00000, 0x80000, 0x203d744e),
  LOAD( SMP1, "272-v1d.bin", 0x000000, 0x800000, 0x28d57d10),
  LOAD( SMP1, "272-v2d.bin", 0x800000, 0x800000, 0x95fe7646),

  LOAD_16_8( SPRITES, "272-c1d.bin", 0x0000000, 0x800000, 0xec9fda8d),
  LOAD_16_8( SPRITES, "272-c2d.bin", 0x0000001, 0x800000, 0xd2fc888d),
  LOAD_16_8( SPRITES, "272-c3d.bin", 0x1000000, 0x800000, 0xb0ea781b),
  LOAD_16_8( SPRITES, "272-c4d.bin", 0x1000001, 0x800000, 0xd34ac591),
  LOAD_16_8( SPRITES, "272-c5d.bin", 0x2000000, 0x800000, 0x1b5c1ea2),
  LOAD_16_8( SPRITES, "272-c6d.bin", 0x2000001, 0x800000, 0xdeeaad58),
  LOAD_16_8( SPRITES, "272-c7d.bin", 0x3000000, 0x800000, 0xdce0119f),
  LOAD_16_8( SPRITES, "272-c8d.bin", 0x3000001, 0x800000, 0x3bf256c5),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( samsh5fe, neogeo, "Samurai Shodown V Special Final Edition (NGM-2720)", SNK_PLAYMORE, 2004, GAME_BEAT);

static struct ROM_INFO rom_samsh5pf[] =
{
  LOAD_SW16( CPU1, "273-p1.bin", 0x000000, 0x800000, 0xe3d18d3a),
  LOAD( FIXED, "273-s1.bin", 0, 0x020000, 0xabb3baf9 ),

  LOAD( CPU2, "273-m1d.bin", 0x00000, 0x20000, 0x654e9236),

  LOAD_16_8( SPRITES, "273-c1d.bin", 0x0000000, 0x800000, 0xec9fda8d),
  LOAD_16_8( SPRITES, "273-c2d.bin", 0x0000001, 0x800000, 0xd2fc888d),
  LOAD_16_8( SPRITES, "273-c3d.bin", 0x1000000, 0x800000, 0xb0ea781b),
  LOAD_16_8( SPRITES, "273-c4d.bin", 0x1000001, 0x800000, 0xd34ac591),
  LOAD_16_8( SPRITES, "273-c5d.bin", 0x2000000, 0x800000, 0x1b5c1ea2),
  LOAD_16_8( SPRITES, "273-c6d.bin", 0x2000001, 0x800000, 0xdeeaad58),
  LOAD_16_8( SPRITES, "273-c7d.bin", 0x3000000, 0x800000, 0xaf90afc8),
  LOAD_16_8( SPRITES, "273-c8d.bin", 0x3000001, 0x800000, 0xb14872da),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( samsh5pf, samsh5fe, "Samurai Shodown V Perfect (NGM-2730)", BOOTLEG, 2020, GAME_BEAT);

static struct ROM_INFO rom_samsh5sph[] = /* Encrypted Set clone of samsh5sp */
 /* AES VERSION, 2nd bugfix release */{
  LOAD_SW16( CPU1, "272-p1ca.p1", 0x000000, 0x400000, 0xc30a08dd),
	/* Correct chip labels unknown */
  LOAD_SW16( CPU1, "272-p2ca.sp2", 0x400000, 0x400000, 0xbd64a518),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( samsh5sph, samsh5sp, "Samurai Shodown V Special / Samurai Spirits Zero Special (NGH-2720) (2nd release, less censored)", SNK_PLAYMORE, 2004, GAME_BEAT);

static struct ROM_INFO rom_lans2004[] = // clone of shocktr2
{
  LOAD_SW16( CPU1, "lnsq-p1.bin", 0x000000, 0x200000, 0xb40a879a),
  LOAD_SW16( CPU1, "lnsq-p21.bin", 0x200000, 0x200000, 0xecdb2d42),
  LOAD_SW16( CPU1, "lnsq-p22.bin", 0x400000, 0x200000, 0xfac5e2e7),
  LOAD( FIXED, "lnsq-s1.bin", 0x000000, 0x20000, 0x39e82897),
  LOAD( SMP1, "lnsq-v1.bin", 0x000000, 0x400000, 0x4408ebc3),
  LOAD( SMP1, "lnsq-v2.bin", 0x400000, 0x400000, 0x3d953975),
  LOAD( SMP1, "lnsq-v3.bin", 0x800000, 0x200000, 0x437d1d8e),
  LOAD_16_8( SPRITES, "lnsq-c1.bin", 0x0000000, 0x800000, 0xb83de59f),
  LOAD_16_8( SPRITES, "lnsq-c2.bin", 0x0000001, 0x800000, 0xe08969fd),
  LOAD_16_8( SPRITES, "lnsq-c3.bin", 0x1000000, 0x800000, 0x013f2cda),
  LOAD_16_8( SPRITES, "lnsq-c4.bin", 0x1000001, 0x800000, 0xd8c3a758),
  LOAD_16_8( SPRITES, "lnsq-c5.bin", 0x2000000, 0x800000, 0x75500b82),
  LOAD_16_8( SPRITES, "lnsq-c6.bin", 0x2000001, 0x800000, 0x670ac13b),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( lans2004, shocktr2, "Lansquenet 2004 (Shock Troopers - 2nd Squad bootleg)", BOOTLEG, 1998, GAME_SHOOT);

static struct ROM_INFO rom_diggerma[] = /* Unlicensed Prototype, no official game ID # clone of neogeo */
{
  LOAD_SW16( CPU1, "dig-p1.bin", 0x000000, 0x080000, 0xeda433d7),
  LOAD( FIXED, "dig-s1.bin", 0x000000, 0x10000, 0x75a88c1f),
  LOAD( ROM2, "dig-m1.bin", 0x00000, 0x10000, 0x833cdf1b),
  LOAD( ROM2, "dig-m1.bin", 0x10000, 0x10000, 0x833cdf1b),
  LOAD( SMP1, "dig-v1.bin", 0x000000, 0x080000, 0xee15bda4),
  LOAD_16_8( SPRITES, "dig-c1.bin", 0x000000, 0x080000, 0x3db0a4ed),
  LOAD_16_8( SPRITES, "dig-c2.bin", 0x000001, 0x080000, 0x3e632161),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( diggerma, neogeo, "Digger Man (prototype)", KYLE_HODGETTS, 2000, GAME_MAZE);

static struct ROM_INFO rom_kof97pls[] = // clone of kof97
{
  LOAD_SW16( CPU1, "kf97-p1p.bin", 0x000000, 0x100000, 0xc01fda46),
  LOAD_SW16( CPU1, "kf97-p2p.bin", 0x100000, 0x400000, 0x5502b020),
  LOAD( FIXED, "kf97-s1p.bin", 0x000000, 0x20000, 0x73254270),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kof97pls, kof97, "The King of Fighters '97 Plus (bootleg)", BOOTLEG, 1997, GAME_BEAT);

static struct ROM_INFO rom_kof97oro[] = // clone of kof97
{
  LOAD_SW16( CPU1, "orochi-p1.bin", 0x0000000, 0x100000, 0x6dcb2946),
  LOAD_SW16( CPU1, "orochi-p21.bin", 0x0200000, 0x100000, 0x6e1c4d8c),
  { "orochi-p21.bin", 0x100000 , 0x6e1c4d8c, REGION_CPU1, 0x100000, LOAD_CONTINUE },
  LOAD_SW16( CPU1, "orochi-p29.bin", 0x0400000, 0x100000, 0x4c7c0221),
  { "orochi-p29.bin", 0x100000 , 0x4c7c0221, REGION_CPU1, 0x300000, LOAD_CONTINUE },
  LOAD( FIXED, "orochi-s1.bin", 0x000000, 0x20000, 0x4ee2149a),
	// The C1 and C2 here are reconstructed but very likely to be correct.
  LOAD_16_8( SPRITES, "orochi-c1.bin", 0x0000000, 0x1000000, 0xf13e841c),
  LOAD_16_8( SPRITES, "orochi-c2.bin", 0x0000001, 0x1000000, 0x2db1f6d3),
  LOAD_16_8( SPRITES, "orochi-c51.bin", 0x2000000, 0x0200000, 0xa90340cb),
  LOAD_16_8( SPRITES, "orochi-c61.bin", 0x2000001, 0x0200000, 0x188e351a),
  LOAD_16_8( SPRITES, "orochi-c52.bin", 0x2400000, 0x0200000, 0xd4eec50a),
  LOAD_16_8( SPRITES, "orochi-c62.bin", 0x2400001, 0x0200000, 0x031b1ad5),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kof97oro, kof97, "The King of Fighters '97 Oroshi Plus 2003 (bootleg)", BOOTLEG, 1997, GAME_BEAT);

// Other companies

static struct ROM_INFO rom_wh2j[] = /* MVS AND AES VERSION clone of neogeo */
{
  LOAD_SW16( CPU1, "064-p1.p1", 0x100000, 0x100000, 0x385a2e86),
  { "064-p1.p1", 0x100000 , 0x385a2e86, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  LOAD( FIXED, "064-s1.s1", 0x000000, 0x20000, 0x2a03998a),
  LOAD( ROM2, "064-m1.m1", 0x00000, 0x20000, 0xd2eec9d3),
  LOAD( SMP1, "064-v1.v1", 0x000000, 0x200000, 0xaa277109),
  LOAD( SMP1, "064-v2.v2", 0x200000, 0x200000, 0xb6527edd),
  LOAD_16_8( SPRITES, "064-c1.c1", 0x000000, 0x200000, 0x2ec87cea),
  LOAD_16_8( SPRITES, "064-c2.c2", 0x000001, 0x200000, 0x526b81ab),
  LOAD_16_8( SPRITES, "064-c3.c3", 0x400000, 0x200000, 0x436d1b31),
  LOAD_16_8( SPRITES, "064-c4.c4", 0x400001, 0x200000, 0xf9c8dd26),
  LOAD_16_8( SPRITES, "064-c5.c5", 0x800000, 0x200000, 0x8e34a9f4),
  LOAD_16_8( SPRITES, "064-c6.c6", 0x800001, 0x200000, 0xa43e4766),
  LOAD_16_8( SPRITES, "064-c7.c7", 0xc00000, 0x200000, 0x59d97215),
  LOAD_16_8( SPRITES, "064-c8.c8", 0xc00001, 0x200000, 0xfc092367),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( wh2j, neogeo,        "World Heroes 2 Jet (ADM-007)(ADH-007)", ADK, 1994, GAME_BEAT);

static struct ROM_INFO rom_mosyougi[] = // clone of neogeo
{
  LOAD_SW16( CPU1, "203-p1.p1", 0x000000, 0x100000, 0x7ba70e2d),
  LOAD( FIXED, "203-s1.s1", 0x000000, 0x20000, 0xbfdc8309),
  LOAD( ROM2, "203-m1.m1", 0x00000, 0x20000, 0xa602c2c2),
  LOAD( SMP1, "203-v1.v1", 0x000000, 0x200000, 0xbaa2b9a5),
  LOAD_16_8( SPRITES, "203-c1.c1", 0x000000, 0x200000, 0xbba9e8c0),
  LOAD_16_8( SPRITES, "203-c2.c2", 0x000001, 0x200000, 0x2574be03),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( mosyougi, neogeo,        "Syougi No Tatsujin - Master of Syougi", ADK, 1995, GAME_PUZZLE);

static struct ROM_INFO rom_zintrckb[] = // clone of neogeo
{
  LOAD_SW16( CPU1, "zin-p1.bin", 0x000000, 0x100000, 0x06c8fca7),
  LOAD( FIXED, "zin-s1.bin", 0x000000, 0x20000, 0xa7ab0e81),
  LOAD( ROM2, "zin-m1.bin", 0x00000, 0x20000, 0xfd9627ca),
  LOAD( SMP1, "zin-v1.bin", 0x000000, 0x200000, 0xc09f74f1),
  LOAD_16_8( SPRITES, "zin-c1.bin", 0x000000, 0x200000, 0x76aee189),
  LOAD_16_8( SPRITES, "zin-c2.bin", 0x000001, 0x200000, 0x844ed4b3),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( zintrckb, neogeo,             "Zintrick / Oshidashi Zentrix (hack)", HACK, 1996, GAME_MISC);

static struct ROM_INFO rom_b2b[] = /* testing bang bang buster... */
{
  LOAD_SW16( CPU1, "071-p1.bin", 0x000000, 0x80000, 0x7687197d),
  LOAD( FIXED, "071-s1.bin", 0x000000, 0x20000, 0x44e5f154),
  LOAD( ROM2, "071-m1.bin", 0x00000, 0x20000, 0x6da739ad),
  LOAD( SMP1, "071-v1.bin", 0x000000, 0x100000, 0x50feffb0),
  LOAD_16_8( SPRITES, "071-c1.bin", 0x0000000, 0x200000, 0x23d84a7a),
  LOAD_16_8( SPRITES, "071-c2.bin", 0x0000001, 0x200000, 0xce7b6248),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( b2b, neogeo, "Bang Bang busters", VISCO, 2000, GAME_PLATFORM); // The rom shows 2000 on title screen, but released in 2010 ?

static struct ROM_INFO rom_totcarib[] = /* testing treasure of the caribeans... */
{
  LOAD_SW16( CPU1, "totc-p1.bin", 0x000000, 0x100000, 0x99604539),
  LOAD( FIXED, "totc-s1.bin", 0x000000, 0x20000, 0x0a3fee41),
  LOAD( ROM2, "totc-m1.bin", 0x00000, 0x20000, 0x18b23ace),
  LOAD( SMP1, "totc-v1.bin", 0x000000, 0x200000, 0x15c7f9e6),
  LOAD( SMP1, "totc-v2.bin", 0x200000, 0x200000, 0x1b264559),
  LOAD( SMP1, "totc-v3.bin", 0x400000, 0x100000, 0x84b62c5d),
  LOAD_16_8( SPRITES, "totc-c1.bin", 0x0000000, 0x200000, 0xcdd6600f),
  LOAD_16_8( SPRITES, "totc-c2.bin", 0x0000001, 0x200000, 0xf362c271),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( totcarib, neogeo, "Treasure of the Caribeans", FACE, 2011, GAME_MISC); // 2011, really ???

// and some kof clones...

static struct ROM_INFO rom_kof99dc[] = /* decrypted version with DC hack */
{
  LOAD_SW16( CPU1, "152dc-p1.p1", 0x000000, 0x100000, 0x80a36a1b),
  LOAD_SW16( CPU1, "152dc-p2.sp2", 0x100000, 0x400000, 0x62b11826),
  LOAD( FIXED, "kf99dc_s1.rom", 0x000000, 0x20000, 0x697c1f85),
  LOAD_16_8( SPRITES, "kf99dc_c1.rom", 0x0000000, 0x800000, 0x598cc558),
  LOAD_16_8( SPRITES, "kf99dc_c2.rom", 0x0000001, 0x800000, 0x5b4c297b),
  LOAD_16_8( SPRITES, "kf99n_c3.rom", 0x1000000, 0x800000, 0xb047c9d5),
  LOAD_16_8( SPRITES, "kf99n_c4.rom", 0x1000001, 0x800000, 0x6bc8e4b1),
  LOAD_16_8( SPRITES, "kf99n_c5.rom", 0x2000000, 0x800000, 0x9746268c),
  LOAD_16_8( SPRITES, "kf99n_c6.rom", 0x2000001, 0x800000, 0x238b3e71),
  LOAD_16_8( SPRITES, "kf99dc_c7.rom", 0x3000000, 0x800000, 0x3ac342ad),
  LOAD_16_8( SPRITES, "kf99dc_c8.rom", 0x3000001, 0x800000, 0x500aea52),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kof99dc, kof99, "The King of Fighters '99 Dreamcast", SNK, 1999, GAME_BEAT);

static struct ROM_INFO rom_kof99pbs[] = /* decrypted version with Boss hack */
{
  LOAD_SW16( CPU1, "251pbs-p1.p1", 0x000000, 0x100000, 0xa4deff9a),
  LOAD_SW16( CPU1, "251pbs-p2.sp2", 0x100000, 0x400000, 0x23339a53),
  LOAD( FIXED, "251ps-s1.s1", 0x000000, 0x20000, 0x0b3d79d3),
  LOAD_16_8( SPRITES, "proto_251-c1.c1", 0x0000000, 0x800000, 0xe5d8ffa4),
  LOAD_16_8( SPRITES, "proto_251-c2.c2", 0x0000001, 0x800000, 0xd822778f),
  LOAD_16_8( SPRITES, "proto_251-c3.c3", 0x1000000, 0x800000, 0xf20959e8),
  LOAD_16_8( SPRITES, "proto_251-c4.c4", 0x1000001, 0x800000, 0x54ffbe9f),
  LOAD_16_8( SPRITES, "proto_251-c5.c5", 0x2000000, 0x800000, 0xd87a3bbc),
  LOAD_16_8( SPRITES, "proto_251-c6.c6", 0x2000001, 0x800000, 0x4d40a691),
  LOAD_16_8( SPRITES, "251pbs-c7.c7", 0x3000000, 0x800000, 0xa0fb0a24),
  LOAD_16_8( SPRITES, "251pbs-c8.c8", 0x3000001, 0x800000, 0xfbc1112c),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kof99pbs, kof99, "The King of Fighters '99 Perfect Boss", SNK, 1999, GAME_BEAT);

static struct ROM_INFO rom_kof2000ps2[] = /* decrypted version with PS2 hack */
{
  LOAD_SW16( CPU1, "257ps2-p1.bin", 0x000000, 0x100000, 0x56941018),
  LOAD_SW16( CPU1, "257ps2-p2.bin", 0x100000, 0x400000, 0x1669a5ad),

  LOAD_16_8( SPRITES, "257-c1_decrypted.bin", 0x0000000, 0x800000, 0xabcdd424),
  LOAD_16_8( SPRITES, "257-c2_decrypted.bin", 0x0000001, 0x800000, 0xcda33778),
  LOAD_16_8( SPRITES, "257-c3_decrypted.bin", 0x1000000, 0x800000, 0x087fb15b),
  LOAD_16_8( SPRITES, "257-c4_decrypted.bin", 0x1000001, 0x800000, 0xfe9dfde4),
  LOAD_16_8( SPRITES, "257-c5_decrypted.bin", 0x2000000, 0x800000, 0x03ee4bf4),
  LOAD_16_8( SPRITES, "257-c6_decrypted.bin", 0x2000001, 0x800000, 0x8599cc5b),
  LOAD_16_8( SPRITES, "257-c7_decrypted.bin", 0x3000000, 0x800000, 0x93c343ec),
  LOAD_16_8( SPRITES, "257-c8_decrypted.bin", 0x3000001, 0x800000, 0xba92f698),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kof2000ps2, kof2000, "The King of Fighters 2000 Playstation 2 (EGHT HACK)", SNK, 2000, GAME_BEAT);

static struct ROM_INFO rom_kof2k1bs[] = /* decrypted version with PS2 hack */
{
  LOAD_SW16( CPU1, "kof2k1bs_p1.rom", 0x000000, 0x100000, 0xb5becb3c),
  LOAD_SW16( CPU1, "kof2k1bs_p2.rom", 0x100000, 0x400000, 0x295d0c5c),
  LOAD( FIXED, "kof2k1bs_s1.rom", 0x000000, 0x20000, 0x19b6587b),
  LOAD( ROM2, "kof2k1bs_m1.rom", 0x00000, 0x20000, 0x2fb0a8a5),
  LOAD_16_8( SPRITES, "kof2k1bs_c1.rom", 0x0000000, 0x800000, 0x103225b1),
  LOAD_16_8( SPRITES, "kof2k1bs_c2.rom", 0x0000001, 0x800000, 0xf9d05d99),
  LOAD_16_8( SPRITES, "kof2k1bs_c3.rom", 0x1000000, 0x800000, 0x4c7ec427),
  LOAD_16_8( SPRITES, "kof2k1bs_c4.rom", 0x1000001, 0x800000, 0x1d237aa6),
  LOAD_16_8( SPRITES, "kof2k1bs_c5.rom", 0x2000000, 0x800000, 0xc2256db5),
  LOAD_16_8( SPRITES, "kof2k1bs_c6.rom", 0x2000001, 0x800000, 0x8d6565a9),
  LOAD_16_8( SPRITES, "kof2k1bs_c7.rom", 0x3000000, 0x800000, 0x8d11aed2),
  LOAD_16_8( SPRITES, "kof2k1bs_c8.rom", 0x3000001, 0x800000, 0x9e7168f5),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kof2k1bs, kof2001, "The King of Fighters 2001 Playstation 2 (EGCG HACK)", SNK, 2001, GAME_BEAT);

static struct ROM_INFO rom_kf2k3ps2[] = /* decrypted version with PS2 hack */
 /* AES VERSION */{
  LOAD_SW16( CPU1, "kf2k3ps2-p1.bin", 0x000000, 0x800000, 0x8e66056d),
  LOAD( FIXED, "kf2k3ps2-s1.bin", 0x000000, 0x20000, 0xc2e48e98),
  LOAD( ROM2, "kf2k3ps2-m1.bin", 0x00000, 0x80000, 0x4374cb5c),
  LOAD( SMP1, "kf2k3ps2-v1.bin", 0x000000, 0x800000, 0xdd6c6a85),
  LOAD( SMP1, "kf2k3ps2-v2.bin", 0x800000, 0x800000, 0x0e84f8c1),
  LOAD_16_8( SPRITES, "kf2k3ps2-c1.bin", 0x0000000, 0x800000, 0xe42fc226),
  LOAD_16_8( SPRITES, "kf2k3ps2-c2.bin", 0x0000001, 0x800000, 0x1b5e3b58),
  LOAD_16_8( SPRITES, "kf2k3ps2-c3.bin", 0x1000000, 0x800000, 0xd334fdd9),
  LOAD_16_8( SPRITES, "kf2k3ps2-c4.bin", 0x1000001, 0x800000, 0x0d457699),
  LOAD_16_8( SPRITES, "kf2k3ps2-c5.bin", 0x2000000, 0x800000, 0x8a91aae4),
  LOAD_16_8( SPRITES, "kf2k3ps2-c6.bin", 0x2000001, 0x800000, 0x9f8674b8),
  LOAD_16_8( SPRITES, "kf2k3ps2-c7.bin", 0x3000000, 0x800000, 0x0ff993d7),
  LOAD_16_8( SPRITES, "kf2k3ps2-c8.bin", 0x3000001, 0x800000, 0x22e36002),
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kf2k3ps2, neogeo, "The King of Fighters 2003 Playstation 2 (EGCG HACK)", SNK, 2003, GAME_BEAT);

static struct ROM_INFO rom_kf2k5uni[] = // clone of kof2002
{
  { REGION_EMPTY, 0, 0 , REGION_AUDIOCRYPT, 0, LOAD_NORMAL },
  LOAD_SW16( CPU1, "5006-p2a.bin", 0x000000, 0x400000, 0xced883a2),
  LOAD_SW16( CPU1, "5006-p1.bin", 0x400000, 0x400000, 0x72c39c46),
  LOAD( FIXED, "5006-s1.bin", 0x000000, 0x20000, 0x91f8c544),
  LOAD( ROM2, "5006-m1.bin", 	 0x00000, 0x20000, 0x9050bfe7),
  LOAD( ROM2, "5006-m1.bin", 0x10000, 0x20000, 0x9050bfe7),
  LOAD( SMP1, "kf10-v1.bin", 0x000000, 0x800000, 0x0fc9a58d),
  LOAD( SMP1, "kf10-v2.bin", 0x800000, 0x800000, 0xb8c475a4),
  LOAD_16_8( SPRITES, "kf10-c1a.bin", 0x0000000, 0x400000, 0x3bbc0364), /* Plane 0,1 */
  LOAD_16_8( SPRITES, "kf10-c2a.bin", 0x0000001, 0x400000, 0x91230075), /* Plane 2,3 */
  LOAD_16_8( SPRITES, "kf10-c1b.bin", 0x0800000, 0x400000, 0xb5abfc28), /* Plane 0,1 */
  LOAD_16_8( SPRITES, "kf10-c2b.bin", 0x0800001, 0x400000, 0x6cc4c6e1), /* Plane 2,3 */
  LOAD_16_8( SPRITES, "kf10-c3a.bin", 0x1000000, 0x400000, 0x5b3d4a16), /* Plane 0,1 */
  LOAD_16_8( SPRITES, "kf10-c4a.bin", 0x1000001, 0x400000, 0xc6f3419b), /* Plane 2,3 */
  LOAD_16_8( SPRITES, "kf10-c3b.bin", 0x1800000, 0x400000, 0x9d2bba19), /* Plane 0,1 */
  LOAD_16_8( SPRITES, "kf10-c4b.bin", 0x1800001, 0x400000, 0x5a4050cb), /* Plane 2,3 */
  LOAD_16_8( SPRITES, "kf10-c5a.bin", 0x2000000, 0x400000, 0xa289d1e1), /* Plane 0,1 */
  LOAD_16_8( SPRITES, "kf10-c6a.bin", 0x2000001, 0x400000, 0xe6494b5d), /* Plane 2,3 */
  LOAD_16_8( SPRITES, "kf10-c5b.bin", 0x2800000, 0x400000, 0x404fff02), /* Plane 0,1 */
  LOAD_16_8( SPRITES, "kf10-c6b.bin", 0x2800001, 0x400000, 0xf2ccfc9e), /* Plane 2,3 */
  LOAD_16_8( SPRITES, "kf10-c7a.bin", 0x3000000, 0x400000, 0xbe79c5a8), /* Plane 0,1 */
  LOAD_16_8( SPRITES, "kf10-c8a.bin", 0x3000001, 0x400000, 0xa5952ca4), /* Plane 2,3 */
  LOAD_16_8( SPRITES, "kf10-c7b.bin", 0x3800000, 0x400000, 0x3fdb3542), /* Plane 0,1 */
  LOAD_16_8( SPRITES, "kf10-c8b.bin", 0x3800001, 0x400000, 0x661b7a52), /* Plane 2,3 */
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kf2k5uni,kof2002,"The King of Fighters 10th Anniversary 2005 Unique (The King of Fighters 2002 bootleg)",BOOTLEG,2004, GAME_BEAT );

