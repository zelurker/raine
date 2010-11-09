/******************************************************************************/
/*                                                                            */
/*               EARTH JOKER (C) 1993 TAITO VISCO CORPORATION                 */
/*                                                                            */
/*                 MAZE OF FLOTT (C) 1989 TAITO CORPORATION                   */
/*                                                                            */
/*                    GALMEDES (C) 1992 VISCO CORPORATION                     */
/*                                                                            */
/*           KOKONTOUZAI ETO MONOGATARI (C) 1994 VISCO CORPORATION            */
/*                                                                            */
/******************************************************************************/

#include "gameinc.h"
#include "earthjkr.h"
#include "tc100scn.h"
#include "tc110pcr.h"
#include "tc002obj.h"
#include "tc220ioc.h"
#include "sasound.h"		// sample support routines
#include "taitosnd.h"
#include "msm5205.h"
#include "2151intf.h"
#include "timer.h"
#include "blit.h" // clear_game_screen

/******************************************************************************/

static struct DIR_INFO earth_joker_dirs[] =
{
   { "earth_joker", },
   { "earthjkr", },
   { NULL, },
};

static struct ROM_INFO earth_joker_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x000000,  0x00020000,
               "ej_3b.rom",  0xbdd86fc2,    "ej_3a.rom",  0x9c8050c6),
   {   "ej_30e.rom", 0x00080000, 0x49d1f77f, REGION_ROM1, 0x080000, LOAD_NORMAL, },
   {     "ej_1.rom", 0x00010000, 0xcb4891db, 0, 0, 0, },
   {     "ej_2.rom", 0x00010000, 0x42ba2566, 0, 0, 0, },
   {     "ej_0.rom", 0x00010000, 0xb612086f, 0, 0, 0, },
   {   "ej_chr.rom", 0x00080000, 0xac675297, 0, 0, 0, },
   {   "ej_obj.rom", 0x00080000, 0x5f21ac47, 0, 0, 0, },
   {           NULL,          0,          0, 0, 0, 0, },
};

static struct INPUT_INFO earth_joker_inputs[] =
{
   { KB_DEF_COIN1,        MSG_COIN1,               0x01A00E, 0x04, BIT_ACTIVE_0 },
   { KB_DEF_COIN2,        MSG_COIN2,               0x01A00E, 0x08, BIT_ACTIVE_0 },
   { KB_DEF_TILT,         MSG_TILT,                0x01A00E, 0x01, BIT_ACTIVE_0 },
   { KB_DEF_SERVICE,      MSG_SERVICE,             0x01A00E, 0x02, BIT_ACTIVE_0 },

   { KB_DEF_P1_START,     MSG_P1_START,            0x01A00E, 0x40, BIT_ACTIVE_0 },
   { KB_DEF_P1_UP,        MSG_P1_UP,               0x01A004, 0x01, BIT_ACTIVE_0 },
   { KB_DEF_P1_DOWN,      MSG_P1_DOWN,             0x01A004, 0x02, BIT_ACTIVE_0 },
   { KB_DEF_P1_LEFT,      MSG_P1_LEFT,             0x01A004, 0x04, BIT_ACTIVE_0 },
   { KB_DEF_P1_RIGHT,     MSG_P1_RIGHT,            0x01A004, 0x08, BIT_ACTIVE_0 },
   { KB_DEF_P1_B1,        MSG_P1_B1,               0x01A004, 0x10, BIT_ACTIVE_0 },
   { KB_DEF_P1_B2,        MSG_P1_B2,               0x01A004, 0x20, BIT_ACTIVE_0 },

   { KB_DEF_P2_START,     MSG_P2_START,            0x01A00E, 0x80, BIT_ACTIVE_0 },
   { KB_DEF_P2_UP,        MSG_P2_UP,               0x01A006, 0x01, BIT_ACTIVE_0 },
   { KB_DEF_P2_DOWN,      MSG_P2_DOWN,             0x01A006, 0x02, BIT_ACTIVE_0 },
   { KB_DEF_P2_LEFT,      MSG_P2_LEFT,             0x01A006, 0x04, BIT_ACTIVE_0 },
   { KB_DEF_P2_RIGHT,     MSG_P2_RIGHT,            0x01A006, 0x08, BIT_ACTIVE_0 },
   { KB_DEF_P2_B1,        MSG_P2_B1,               0x01A006, 0x10, BIT_ACTIVE_0 },
   { KB_DEF_P2_B2,        MSG_P2_B2,               0x01A006, 0x20, BIT_ACTIVE_0 },

   { 0,                   NULL,                    0,        0,    0            },
};

#define COINAGE_EARTHJOKER \
   { MSG_COIN1,               0x30, 0x04 },\
   { MSG_2COIN_1PLAY,         0x10, 0x00 },\
   { MSG_1COIN_1PLAY,         0x30, 0x00 },\
   { MSG_2COIN_3PLAY,         0x00, 0x00 },\
   { MSG_1COIN_2PLAY,         0x20, 0x00 },\
   { MSG_COIN2,               0xC0, 0x04 },\
   { MSG_2COIN_1PLAY,         0x40, 0x00 },\
   { MSG_1COIN_1PLAY,         0xC0, 0x00 },\
   { MSG_2COIN_3PLAY,         0x00, 0x00 },\
   { MSG_1COIN_2PLAY,         0x80, 0x00 },

static struct DSW_DATA dsw_data_earth_joker_0[] =
{
   { MSG_UNKNOWN,             0x01, 0x02 },
   { MSG_OFF,                 0x01},
   { MSG_ON,                  0x00},
   { MSG_SCREEN,              0x02, 0x02 },
   { MSG_NORMAL,              0x02},
   { MSG_INVERT,              0x00},
   { MSG_SERVICE,             0x04, 0x02 },
   { MSG_OFF,                 0x04},
   { MSG_ON,                  0x00},
   { MSG_DEMO_SOUND,          0x08, 0x02 },
   { MSG_OFF,                 0x00},
   { MSG_ON,                  0x08},
   COINAGE_EARTHJOKER
   { NULL,                    0,   },
};

static struct DSW_DATA dsw_data_earth_joker_1[] =
{
   { MSG_DIFFICULTY,          0x03, 0x04 },
   { MSG_EASY,                0x02},
   { MSG_NORMAL,              0x03},
   { MSG_HARD,                0x01},
   { MSG_HARDEST,             0x00},
   { MSG_UNKNOWN,             0x04, 0x02 },
   { MSG_OFF,                 0x04},
   { MSG_ON,                  0x00},
   { MSG_UNKNOWN,             0x08, 0x02 },
   { MSG_OFF,                 0x08},
   { MSG_ON,                  0x00},
   { MSG_LIVES,               0x30, 0x04 },
   { "1",                     0x00},
   { "2",                     0x30},
   { "3",                     0x20},
   { "4",                     0x10},
   { "Romstar License",       0x40, 0x02 },
   { MSG_OFF,                 0x40},
   { MSG_ON,                  0x00},
   { MSG_UNKNOWN,             0x80, 0x02 },
   { MSG_OFF,                 0x80},
   { MSG_ON,                  0x00},
   { NULL,                    0,   },
};

static struct DSW_INFO earth_joker_dsw[] =
{
   { 0x01A000, 0xFF, dsw_data_earth_joker_0 },
   { 0x01A002, 0xEF, dsw_data_earth_joker_1 },
   { 0,        0,    NULL,      },
};

static struct VIDEO_INFO earth_joker_video =
{
   DrawEarthJoker,
   240,
   320,
   32,
   VIDEO_ROTATE_NORMAL,
};

GAME( earth_joker ,
   earth_joker_dirs,
   earth_joker_roms,
   earth_joker_inputs,
   earth_joker_dsw,
   NULL,

   LoadEarthJoker,
   ClearEarthJoker,
   &earth_joker_video,
   ExecuteEarthJokerFrame,
   "earthjkr",
   "Earth Joker",
   "アースジョーカー",
   COMPANY_ID_VISCO,
   NULL,
   1993,
   taito_ym2151_sound,
   GAME_SHOOT
);

/******************************************************************************/

static struct DIR_INFO maze_of_flott_dirs[] =
{
   { "maze_of_flott", },
   { "mofflott", },
   { NULL, },
};

static struct ROM_INFO maze_of_flott_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x000000,  0x00020000,
              "c17-09.bin",  0x05ee110f,   "c17-08.bin",  0xd0aacffd),
   {   "c17-03.bin", 0x00080000, 0x27047fc3, REGION_ROM1, 0x080000, LOAD_NORMAL, },
   {   "c17-06.bin", 0x00010000, 0x5c332125, 0, 0, 0, },
   {   "c17-07.bin", 0x00010000, 0xcdb7bc2c, 0, 0, 0, },
   {   "c17-04.bin", 0x00010000, 0xf4250410, 0, 0, 0, },
   {   "c17-05.bin", 0x00010000, 0x57ac4741, 0, 0, 0, },
   {   "c17-01.bin", 0x00080000, 0xe9466d42, 0, 0, 0, },
   {   "c17-02.bin", 0x00080000, 0x8860a8db, 0, 0, 0, },
   {           NULL,          0,          0, 0, 0, 0, },
};

static struct DSW_DATA dsw_data_maze_of_flott_0[] =
{
   { MSG_CABINET,             0x01, 0x02 },
   { MSG_UPRIGHT,             0x01},
   { MSG_TABLE,               0x00},
   { MSG_SCREEN,              0x02, 0x02 },
   { MSG_NORMAL,              0x02},
   { MSG_INVERT,              0x00},
   { MSG_SERVICE,             0x04, 0x02 },
   { MSG_OFF,                 0x04},
   { MSG_ON,                  0x00},
   { MSG_DEMO_SOUND,          0x08, 0x02 },
   { MSG_OFF,                 0x00},
   { MSG_ON,                  0x08},
   COINAGE_EARTHJOKER
   { NULL,                    0,   },
};

static struct DSW_DATA dsw_data_maze_of_flott_1[] =
{
   { MSG_DIFFICULTY,          0x03, 0x04 },
   { MSG_EASY,                0x02},
   { MSG_NORMAL,              0x03},
   { MSG_HARD,                0x01},
   { MSG_HARDEST,             0x00},
   { MSG_EXTRA_LIFE,          0x0C, 0x04 },
   { "20k and every 50k",     0x0C},
   { "50k and every 100k",    0x08},
   { "100k only",             0x04},
   { "None",                  0x00},
   { MSG_LIVES,               0x30, 0x04 },
   { "2",                     0x00},
   { "3",                     0x30},
   { "4",                     0x10},
   { "5",                     0x20},
   { "Invulnerability",       0x40, 0x02 },
   { MSG_OFF,                 0x40},
   { MSG_ON,                  0x00},
   { MSG_UNKNOWN,             0x80, 0x02 },
   { MSG_OFF,                 0x80},
   { MSG_ON,                  0x00},
   { NULL,                    0,   },
};

static struct DSW_INFO maze_of_flott_dsw[] =
{
   { 0x01A000, 0xFF, dsw_data_maze_of_flott_0 },
   { 0x01A002, 0xFF, dsw_data_maze_of_flott_1 },
   { 0,        0,    NULL,      },
};

static struct ROMSW_DATA romsw_data_maze_of_flott_0[] =
{
   { "Taito Japan",           0x01 },
   { "Taito Corporation",     0x02 },
   { NULL,                    0    },
};

static struct ROMSW_INFO maze_of_flott_romsw[] =
{
   { 0x03FFFF, 0x01, romsw_data_maze_of_flott_0 },
   { 0,        0,    NULL },
};

static struct YM2151interface ym2151_interface =
{
  1,			// 1 chip
  4000000,		// 4 MHz
  { YM3012_VOL(160,OSD_PAN_LEFT,160,OSD_PAN_RIGHT) },
  { z80_irq_handler },
  { NULL }
};

static struct msm5205_adpcm_list maze_of_flott_adpcm[] =
{
  { 0x000000,0x000900-1 },
  { 0x000900,0x001980-1 },
  { 0x001A00,0x002700-1 },
  { 0x002700,0x008300-1 },
  { 0x008300,0x00AD00-1 },
  { 0x00AD00,0x00DD00-1 },
  { 0x00DD00,0x00F800-1 },
  { 0x00F800,0x00FF80-1 },
};

static struct MSM5205buffer_interface msm5205_interface =
{
   1,
   { 8000 },
   { 220 },
   { maze_of_flott_adpcm },
   { sizeof(maze_of_flott_adpcm) / sizeof(struct msm5205_adpcm_list) },
   { NULL },
   { 0 },
   MSM5205_MONO,
};

static struct SOUND_INFO maze_of_flott_sound[] =
{
   { SOUND_YM2151J, &ym2151_interface,  },
   { SOUND_MSM5205_BUFF, &msm5205_interface, },
   { 0,             NULL,               },
};

GAME( maze_of_flott ,
   maze_of_flott_dirs,
   maze_of_flott_roms,
   earth_joker_inputs,
   maze_of_flott_dsw,
   maze_of_flott_romsw,

   LoadMazeOfFlott,
   ClearMazeOfFlott,
   &earth_joker_video,
   ExecuteEarthJokerFrame,
   "mofflott",
   "Maze of Flott",
   "メイズオブフロット",
   COMPANY_ID_TAITO,
   "C17",
   1989,
   maze_of_flott_sound,
   GAME_RACE
);

/******************************************************************************/

static struct DIR_INFO galmedes_dirs[] =
{
   { "galmedes", },
   { NULL, },
};

static struct ROM_INFO galmedes_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x000000,  0x00020000,
             "gm-prg1.bin",  0x32a70753,  "gm-prg0.bin",  0xfae546a4),
   {   "gm-30.rom", 0x00080000, 0x4da2a407, REGION_ROM1, 0x080000, LOAD_NORMAL, },
   {   "gm-obj.bin", 0x00080000, 0x7a4a1315, 0, 0, 0, },
   {   "gm-scn.bin", 0x00080000, 0x3bab0581, 0, 0, 0, },
   {   "gm-snd.bin", 0x00010000, 0xd6f56c21, 0, 0, 0, },
   {           NULL,          0,          0, 0, 0, 0, },
};

static struct DSW_DATA dsw_data_galmedes_0[] =
{
   { MSG_UNKNOWN,             0x01, 0x02 },
   { MSG_OFF,                 0x01},
   { MSG_ON,                  0x00},
   { MSG_SCREEN,              0x02, 0x02 },
   { MSG_NORMAL,              0x02},
   { MSG_INVERT,              0x00},
   { MSG_SERVICE,             0x04, 0x02 },
   { MSG_OFF,                 0x04},
   { MSG_ON,                  0x00},
   { MSG_DEMO_SOUND,          0x08, 0x02 },
   { MSG_OFF,                 0x08},
   { MSG_ON,                  0x00},
   { MSG_COIN1,               0x30, 0x04 },
   { "4C/1C (W), 2C/3C (J)", 0x00 },
   { "3C/1C (W), 2C/1C (J)",  0x10, 0x00 },
   { "2C/1C (W), 1C/2C (J)",  0x20, 0x00 },
   { MSG_1COIN_1PLAY,         0x30},
   { MSG_COIN2,               0xC0, 0x04 },
   { "1C/2C (W), 1C/1C (J)",  0xC0, 0x00 },
   { "1C/3C (W), 1C/2C (J)",  0x80, 0x00 },
   { "1C/4C (W), 2C/1C (J)",  0x40, 0x00 },
   { "1C/6C (W), 2C/3C (J)", 0x00 },
   { NULL,                    0,   },
};

static struct DSW_DATA dsw_data_galmedes_1[] =
{
   { MSG_DIFFICULTY,          0x03, 0x04 },
   { MSG_EASY,                0x02},
   { MSG_NORMAL,              0x03},
   { MSG_HARD,                0x01},
   { MSG_HARDEST,             0x00},
   { MSG_EXTRA_LIFE,          0x0C, 0x04 },
   { "Every 100k",            0x08},
   { "100k and every 200k",   0x0C},
   { "150k and every 200k",   0x04},
   { "Every 200k",            0x00},
   { MSG_LIVES,               0x30, 0x04 },
   { "1",                     0x20},
   { "2",                     0x10},
   { "3",                     0x30},
   { "4",                     0x00},
   { MSG_UNKNOWN,             0x40, 0x02 },
   { MSG_OFF,                 0x40},
   { MSG_ON,                  0x00},
   { MSG_COINAGE,             0x80, 0x02 },
   { "Japanese",              0x80},
   { "World",                 0x00},
   { NULL,                    0,   },
};

static struct DSW_INFO galmedes_dsw[] =
{
   { 0x01A000, 0xF7, dsw_data_galmedes_0 },
   { 0x01A002, 0x7F, dsw_data_galmedes_1 },
   { 0,        0,    NULL,      },
};

GAME( galmedes ,
   galmedes_dirs,
   galmedes_roms,
   earth_joker_inputs,
   galmedes_dsw,
   NULL,

   LoadGalmedes,
   ClearGalmedes,
   &earth_joker_video,
   ExecuteEarthJokerFrame,
   "galmedes",
   "Galmedes",
   "ガルメデス",
   COMPANY_ID_VISCO,
   NULL,
   1992,
   taito_ym2151_sound,
   GAME_SHOOT
);

/******************************************************************************/

static struct DIR_INFO eto_dirs[] =
{
   { "eto", },
   { NULL, },
};

static struct ROM_INFO eto_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x000000,  0x00020000,
                "eto-1.23",  0x44286597,      "eto-0.8",  0x57b79370),
   {     "eto-2.30", 0x00080000, 0x12f46fb5, REGION_ROM1, 0x080000, LOAD_NORMAL, },
   {      "eto-4.3", 0x00080000, 0xa8768939, 0, 0, 0, },
   {      "eto-3.6", 0x00080000, 0xdd247397, 0, 0, 0, },
   {     "eto-5.27", 0x00010000, 0xb3689da0, 0, 0, 0, },
   {           NULL,          0,          0, 0, 0, 0, },
};

static struct DSW_DATA dsw_data_eto_0[] =
{
   { MSG_UNKNOWN,             0x01, 0x02 },
   { MSG_OFF,                 0x01},
   { MSG_ON,                  0x00},
   { MSG_SCREEN,              0x02, 0x02 },
   { MSG_NORMAL,              0x02},
   { MSG_INVERT,              0x00},
   { MSG_SERVICE,             0x04, 0x02 },
   { MSG_OFF,                 0x04},
   { MSG_ON,                  0x00},
   { MSG_UNKNOWN,             0x08, 0x02 },
   { MSG_OFF,                 0x08},
   { MSG_ON,                  0x00},
   COINAGE_EARTHJOKER
   { NULL,                    0,   },
};

static struct DSW_DATA dsw_data_eto_1[] =
{
   { MSG_DIFFICULTY,          0x03, 0x04 },
   { MSG_EASY,                0x02},
   { MSG_NORMAL,              0x03},
   { MSG_HARD,                0x01},
   { MSG_HARDEST,             0x00},
   { MSG_UNKNOWN,             0x04, 0x02 },
   { MSG_OFF,                 0x04},
   { MSG_ON,                  0x00},
   { MSG_UNKNOWN,             0x08, 0x02 },
   { MSG_OFF,                 0x08},
   { MSG_ON,                  0x00},
   { MSG_UNKNOWN,             0x10, 0x02 },
   { MSG_OFF,                 0x10},
   { MSG_ON,                  0x00},
   { MSG_UNKNOWN,             0x20, 0x02 },
   { MSG_OFF,                 0x20},
   { MSG_ON,                  0x00},
   { MSG_UNKNOWN,             0x40, 0x02 },
   { MSG_OFF,                 0x40},
   { MSG_ON,                  0x00},
   { MSG_UNKNOWN,             0x80, 0x02 },
   { MSG_OFF,                 0x80},
   { MSG_ON,                  0x00},
   { NULL,                    0,   },
};

static struct DSW_INFO eto_dsw[] =
{
   { 0x01A000, 0xFF, dsw_data_eto_0 },
   { 0x01A002, 0xFF, dsw_data_eto_1 },
   { 0,        0,    NULL,      },
};

static struct VIDEO_INFO eto_video =
{
   DrawEto,
   320,
   240,
   32,
   VIDEO_ROTATE_NORMAL,
};

GAME( eto ,
   eto_dirs,
   eto_roms,
   earth_joker_inputs,
   eto_dsw,
   NULL,

   LoadEto,
   ClearEto,
   &eto_video,
   ExecuteEarthJokerFrame,
   "eto",
   "Kokontouzai Eto Monogatari",
   NULL,
   COMPANY_ID_VISCO,
   NULL,
   1994,
   taito_ym2151_sound,
   GAME_PUZZLE
);

/******************************************************************************/

static UINT8 *RAM_VIDEO;
static UINT8 *RAM_SCROLL;
static UINT8 *RAM_OBJECT;
static UINT8 *RAM_INPUT;

static UINT8 *GFX_BG0;
static UINT8 *GFX_BG0_SOLID;

static UINT8 *GFX_SPR;
static UINT8 *GFX_SPR_SOLID;

static int romset;

void Maze_of_Flott_M5205_W(UINT16 offset, UINT8 data);

/*

Supported romsets:

0 - Earth Joker
1 - Maze of Flott
2 - Galmedes

Problems:

- Maze of Flott MSM5205 is missing
- Some parts use background layer line-line scroll

*/

void MemoryMap_EarthJoker(void)
{
   AddReadByte(0x000000, 0x0FFFFF, NULL, ROM+0x000000);			// 68000 ROM
   AddReadByte(0x100000, 0x103FFF, NULL, RAM+0x000000);			// 68000 RAM
   AddReadByte(0xD00000, 0xD007FF, NULL, RAM_OBJECT);			// OBJECT RAM
   AddReadByte(0xC00000, 0xC0FFFF, NULL, RAM_VIDEO);			// SCREEN RAM
   AddReadByte(0x400000, 0x40000F, NULL, RAM_INPUT);			// INPUT
   AddReadByte(0x000000, 0xFFFFFF, DefBadReadByte, NULL);		// <Bad Reads>
   AddReadByte(-1, -1, NULL, NULL);

   AddReadWord(0x000000, 0x0FFFFF, NULL, ROM+0x000000);			// 68000 ROM
   AddReadWord(0x100000, 0x103FFF, NULL, RAM+0x000000);			// 68000 RAM
   AddReadWord(0xD00000, 0xD007FF, NULL, RAM_OBJECT);			// OBJECT RAM
   AddReadWord(0xC00000, 0xC0FFFF, NULL, RAM_VIDEO);			// SCREEN RAM
   AddReadWord(0x200000, 0x200007, tc0110pcr_rw, NULL);			// COLOUR PORTS
   AddReadWord(0x400000, 0x40000F, NULL, RAM_INPUT);			// INPUT
   AddReadWord(0x3E0000, 0x3E0003, tc0140syt_read_main_68k, NULL); 	// SOUND
   AddReadWord(0x000000, 0xFFFFFF, DefBadReadWord, NULL);		// <Bad Reads>
   AddReadWord(-1, -1,NULL, NULL);

   AddWriteByte(0x100000, 0x103FFF, NULL, RAM+0x000000);		// 68000 RAM
   AddWriteByte(0xD00000, 0xD007FF, NULL, RAM_OBJECT);			// OBJECT RAM
   AddWriteByte(0xC06000, 0xC06FFF, tc0100scn_0_gfx_fg0_wb_r270, NULL);	// FG0 GFX RAM
   AddWriteByte(0xC00000, 0xC0FFFF, NULL, RAM_VIDEO);			// SCREEN RAM
   AddWriteByte(0x3A0000, 0x3A0001, NULL, RAM+0x01B010);		// SPRITE CTRL
   AddWriteByte(0x400000, 0x40000F, tc0220ioc_wb, NULL);		// INPUT
   AddWriteByte(0xAA0000, 0xAA0001, Stop68000, NULL);			// Trap Idle 68000
   AddWriteByte(0x000000, 0xFFFFFF, DefBadWriteByte, NULL);		// <Bad Writes>
   AddWriteByte(-1, -1, NULL, NULL);

   AddWriteWord(0x100000, 0x103FFF, NULL, RAM+0x000000);		// 68000 RAM
   AddWriteWord(0xD00000, 0xD007FF, NULL, RAM_OBJECT);			// OBJECT RAM
   AddWriteWord(0xC06000, 0xC06FFF, tc0100scn_0_gfx_fg0_ww_r270, NULL);	// FG0 GFX RAM
   AddWriteWord(0xC00000, 0xC0FFFF, NULL, RAM_VIDEO);			// SCREEN RAM
   AddWriteWord(0x200000, 0x200007, tc0110pcr_ww, NULL);		// COLOUR PORTS
   AddWriteWord(0xC20000, 0xC2000F, NULL, RAM_SCROLL);			// SCROLL RAM
   AddWriteWord(0x3A0000, 0x3A0001, NULL, RAM+0x01B010);		// SPRITE CTRL
   AddWriteWord(0x400000, 0x40000F, tc0220ioc_ww, NULL);		// INPUT
   AddWriteWord(0x3E0000, 0x3E0003, tc0140syt_write_main_68k, NULL);	// SOUND
   AddWriteWord(0x000000, 0xFFFFFF, DefBadWriteWord, NULL);		// <Bad Writes>
   AddWriteWord(-1, -1, NULL, NULL);
}

void MemoryMap_Eto(void)
{
   AddReadByte(0x000000, 0x0FFFFF, NULL, ROM+0x000000);			// 68000 ROM
   AddReadByte(0x200000, 0x203FFF, NULL, RAM+0x000000);			// 68000 RAM
   AddReadByte(0xC00000, 0xC007FF, NULL, RAM_OBJECT);			// OBJECT RAM
   AddReadByte(0xD00000, 0xD0FFFF, NULL, RAM_VIDEO);			// SCREEN RAM
   AddReadByte(0x300000, 0x30000F, NULL, RAM_INPUT);			// INPUT
   AddReadByte(0x400000, 0x40000F, NULL, RAM_INPUT);			// INPUT (mirror)
   AddReadByte(0x000000, 0xFFFFFF, DefBadReadByte, NULL);		// <Bad Reads>
   AddReadByte(-1, -1, NULL, NULL);

   AddReadWord(0x000000, 0x0FFFFF, NULL, ROM+0x000000);			// 68000 ROM
   AddReadWord(0x200000, 0x203FFF, NULL, RAM+0x000000);			// 68000 RAM
   AddReadWord(0xC00000, 0xC007FF, NULL, RAM_OBJECT);			// OBJECT RAM
   AddReadWord(0xD00000, 0xD0FFFF, NULL, RAM_VIDEO);			// SCREEN RAM
   AddReadWord(0x100000, 0x100007, tc0110pcr_rw, NULL);			// COLOUR PORTS
   AddReadWord(0x300000, 0x30000F, NULL, RAM_INPUT);			// INPUT
   AddReadWord(0x400000, 0x40000F, NULL, RAM_INPUT);			// INPUT (mirror)
   AddReadWord(0x4E0000, 0x4E0003, tc0140syt_read_main_68k, NULL); 	// SOUND
   AddReadWord(0x000000, 0xFFFFFF, DefBadReadWord, NULL);		// <Bad Reads>
   AddReadWord(-1, -1,NULL, NULL);

   AddWriteByte(0x200000, 0x203FFF, NULL, RAM+0x000000);		// 68000 RAM
   AddWriteByte(0xC00000, 0xC007FF, NULL, RAM_OBJECT);			// OBJECT RAM
   AddWriteByte(0xD06000, 0xD06FFF, tc0100scn_0_gfx_fg0_wb, NULL);	// FG0 GFX RAM
   AddWriteByte(0xC00000, 0xC0FFFF, NULL, RAM_VIDEO);			// SCREEN RAM (mirror)
   AddWriteByte(0xD00000, 0xD0FFFF, NULL, RAM_VIDEO);			// SCREEN RAM
   AddWriteByte(0x4A0000, 0x4A0001, NULL, RAM+0x01B010);		// SPRITE CTRL
   AddWriteByte(0x300000, 0x30000F, tc0220ioc_wb, NULL);		// INPUT
   AddWriteByte(0xAA0000, 0xAA0001, Stop68000, NULL);			// Trap Idle 68000
   AddWriteByte(0x000000, 0xFFFFFF, DefBadWriteByte, NULL);		// <Bad Writes>
   AddWriteByte(-1, -1, NULL, NULL);

   AddWriteWord(0x200000, 0x203FFF, NULL, RAM+0x000000);		// 68000 RAM
   AddWriteWord(0xC00000, 0xC007FF, NULL, RAM_OBJECT);			// OBJECT RAM
   AddWriteWord(0xD06000, 0xD06FFF, tc0100scn_0_gfx_fg0_ww, NULL);	// FG0 GFX RAM
   AddWriteWord(0xC00000, 0xC0FFFF, NULL, RAM_VIDEO);			// SCREEN RAM (mirror)
   AddWriteWord(0xD00000, 0xD0FFFF, NULL, RAM_VIDEO);			// SCREEN RAM
   AddWriteWord(0x100000, 0x100007, tc0110pcr_ww, NULL);		// COLOUR PORTS
   AddWriteWord(0xD20000, 0xD2000F, NULL, RAM_SCROLL);			// SCROLL RAM
   AddWriteWord(0x4A0000, 0x4A0001, NULL, RAM+0x01B010);		// SPRITE CTRL
   AddWriteWord(0x300000, 0x30000F, tc0220ioc_ww, NULL);		// INPUT
   AddWriteWord(0x4E0000, 0x4E0003, tc0140syt_write_main_68k, NULL);	// SOUND
   AddWriteWord(0x000000, 0xFFFFFF, DefBadWriteWord, NULL);		// <Bad Writes>
   AddWriteWord(-1, -1, NULL, NULL);
}

void AddEarthJoker68k(void)
{
   // Init tc0220ioc emulation
   // ------------------------

   tc0220ioc.RAM  = RAM_INPUT;
   tc0220ioc.ctrl = 0;		//TC0220_STOPCPU;
   reset_tc0220ioc();

   // Init tc0002obj emulation
   // ------------------------

   tc0002obj.RAM	= RAM_OBJECT;
   tc0002obj.GFX	= GFX_SPR;
   tc0002obj.MASK	= GFX_SPR_SOLID;
   tc0002obj.bmp_x	= 32;
   tc0002obj.bmp_y	= 32;
   if (romset==3) {
     tc0002obj.bmp_w	= 320;
     tc0002obj.bmp_h	= 240;
   }
   else {
     tc0002obj.bmp_w	= 240;
     tc0002obj.bmp_h	= 320;
   }
   tc0002obj.tile_mask	= 0x1FFF;
   if (romset==3) {
     tc0002obj.ofs_x	= 0;
     tc0002obj.ofs_y	= -8;
   }
   else {
     tc0002obj.ofs_x	= -8;
     tc0002obj.ofs_y	= 0;
   }

   // Init tc0100scn emulation
   // ------------------------

   tc0100scn[0].layer[0].RAM	=RAM_VIDEO+0x0000;
   tc0100scn[0].layer[0].GFX	=GFX_BG0;
   tc0100scn[0].layer[0].MASK	=GFX_BG0_SOLID;
   tc0100scn[0].layer[0].SCR	=RAM_SCROLL+0;
   tc0100scn[0].layer[0].type	=0;
   tc0100scn[0].layer[0].bmp_x	=32;
   tc0100scn[0].layer[0].bmp_y	=32;
   if (romset==3) {
     tc0100scn[0].layer[0].bmp_w =320;
     tc0100scn[0].layer[0].bmp_h =240;
   }
   else {
     tc0100scn[0].layer[0].bmp_w =240;
     tc0100scn[0].layer[0].bmp_h =320;
   }
   tc0100scn[0].layer[0].tile_mask=0x3FFF;
   tc0100scn[0].layer[0].scr_x	=17;
   tc0100scn[0].layer[0].scr_y	=8;

   tc0100scn[0].layer[1].RAM	=RAM_VIDEO+0x8000;
   tc0100scn[0].layer[1].GFX	=GFX_BG0;
   tc0100scn[0].layer[1].MASK	=GFX_BG0_SOLID;
   tc0100scn[0].layer[1].SCR	=RAM_SCROLL+2;
   tc0100scn[0].layer[1].type	=0;
   tc0100scn[0].layer[1].bmp_x	=32;
   tc0100scn[0].layer[1].bmp_y	=32;
   if (romset==3) {
     tc0100scn[0].layer[1].bmp_w =320;
     tc0100scn[0].layer[1].bmp_h =240;
   }
   else {
     tc0100scn[0].layer[1].bmp_w =240;
     tc0100scn[0].layer[1].bmp_h =320;
   }
   tc0100scn[0].layer[1].tile_mask=0x3FFF;
   tc0100scn[0].layer[1].scr_x	=17;
   tc0100scn[0].layer[1].scr_y	=8;

   tc0100scn[0].layer[2].RAM	=RAM_VIDEO+0x4000;
   tc0100scn[0].layer[2].GFX	=GFX_FG0;
   tc0100scn[0].layer[2].SCR	=RAM_SCROLL+4;
   tc0100scn[0].layer[2].type	=2;			// Correct
   tc0100scn[0].layer[2].bmp_x	=32;
   tc0100scn[0].layer[2].bmp_y	=32;
   if (romset==3) {
     tc0100scn[0].layer[2].bmp_w =320;
     tc0100scn[0].layer[2].bmp_h =240;
   }
   else {
     tc0100scn[0].layer[2].bmp_w =240;
     tc0100scn[0].layer[2].bmp_h =320;
   }
   tc0100scn[0].layer[2].scr_x	=17;			// Correct
   tc0100scn[0].layer[2].scr_y	=8;			// Correct

   tc0100scn[0].RAM     = RAM_VIDEO;
   tc0100scn[0].GFX_FG0 = GFX_FG0;

   init_tc0100scn(0);

   ByteSwap(ROM,0x100000);
   ByteSwap(RAM,0x20000);

   AddMemFetch(0x000000, 0x0FFFFF, ROM+0x000000-0x000000);	// 68000 ROM
   AddMemFetch(-1, -1, NULL);

   if (romset==3)
     MemoryMap_Eto();
   else
     MemoryMap_EarthJoker();

   AddInitMemory();	// Set Starscream mem pointers...
}

void LoadEarthJoker(void)
{
   int ta,tb;
   UINT8 *TMP;

   romset=0;

   RAMSize=0x38000;

   if(!(TMP=AllocateMem(0x80000))) return;
   if(!(RAM=AllocateMem(RAMSize))) return;
   if(!(GFX=AllocateMem(0x140000+0x100000))) return;

   GFX_SPR	= GFX+0x000000;
   GFX_BG0	= GFX+0x140000;

   if(!load_rom("ej_chr.rom", TMP, 0x80000)) return;	// 8x8 TILES
   tb=0;
   for(ta=0;ta<0x80000;ta+=2){
      GFX_BG0[tb++]=TMP[ta+1]>>4;
      GFX_BG0[tb++]=TMP[ta+1]&15;
      GFX_BG0[tb++]=TMP[ta+0]>>4;
      GFX_BG0[tb++]=TMP[ta+0]&15;
   }
   if(!load_rom("ej_obj.rom", TMP, 0x80000)) return;	// 16x16 SPRITES
   tb=0;
   for(ta=0;ta<0x80000;ta+=2){
      GFX_SPR[tb++]=TMP[ta+1]>>4;
      GFX_SPR[tb++]=TMP[ta+1]&15;
      GFX_SPR[tb++]=TMP[ta+0]>>4;
      GFX_SPR[tb++]=TMP[ta+0]&15;
   }
   if(!load_rom("ej_0.rom", TMP+0x00000, 0x10000)) return;	// 16x16 SPRITES
   if(!load_rom("ej_1.rom", TMP+0x10000, 0x10000)) return;	// 16x16 SPRITES
   for(ta=0;ta<0x10000;ta++){
      GFX_SPR[tb++]=TMP[ta+0x00000]>>4;
      GFX_SPR[tb++]=TMP[ta+0x00000]&15;
      GFX_SPR[tb++]=TMP[ta+0x10000]>>4;
      GFX_SPR[tb++]=TMP[ta+0x10000]&15;
   }

   FreeMem(TMP);

   Rotate8x8(GFX_BG0,0x4000);
   Flip8x8_X(GFX_BG0,0x4000);
   Rotate16x16(GFX_SPR,0x1400);
   Flip16x16_X(GFX_SPR,0x1400);

   /*-----[Sound Setup]-----*/

   Z80ROM=RAM+0x28000;
   if(!load_rom("ej_2.rom", Z80ROM, 0x10000)) return;	// Z80 SOUND ROM

   AddTaitoYM2151(0x01AF, 0x0143, 0x10000, NULL, NULL);

   /*-----------------------*/

   memset(RAM+0x00000,0x00,0x28000);

   RAM_VIDEO  = RAM+0x08000;
   RAM_SCROLL = RAM+0x1B000;
   RAM_OBJECT = RAM+0x18000;
   RAM_INPUT  = RAM+0x1A000;

   tc0110pcr_init(RAM+0x1C000, 1);

   set_colour_mapper(&col_map_xbbb_bbgg_gggr_rrrr);
   InitPaletteMap(RAM+0x1C000, 0x100, 0x10, 0x8000);


   GFX_FG0 = RAM+0x20000;

   GFX_BG0_SOLID = make_solid_mask_8x8  (GFX_BG0, 0x4000);
   GFX_SPR_SOLID = make_solid_mask_16x16(GFX_SPR, 0x1400);

   // Speed Hack
   // ----------

   WriteLong68k(&ROM[0x004D8],0x13FC0000);
   WriteLong68k(&ROM[0x004DC],0x00AA0000);

   // Fix ROM Checksum
   // ----------------

   WriteLong68k(&ROM[0x00B36],0x4E714E71);

   AddEarthJoker68k();
}

void LoadMazeOfFlott(void)
{
   int ta,tb;
   UINT8 *TMP;

   romset=1;

   RAMSize=0x38000;

   if(!(TMP=AllocateMem(0x80000))) return;
   if(!(RAM=AllocateMem(RAMSize))) return;
   if(!(GFX=AllocateMem(0x140000+0x100000))) return;

   GFX_SPR	= GFX+0x000000;
   GFX_BG0	= GFX+0x140000;

   if(!load_rom("c17-01.bin", TMP, 0x80000)) return;	// 8x8 TILES
   tb=0;
   for(ta=0;ta<0x80000;ta+=2){
      GFX_BG0[tb++]=TMP[ta+1]>>4;
      GFX_BG0[tb++]=TMP[ta+1]&15;
      GFX_BG0[tb++]=TMP[ta+0]>>4;
      GFX_BG0[tb++]=TMP[ta+0]&15;
   }
   if(!load_rom("c17-02.bin", TMP, 0x80000)) return;	// 16x16 SPRITES
   tb=0;
   for(ta=0;ta<0x80000;ta+=2){
      GFX_SPR[tb++]=TMP[ta+1]>>4;
      GFX_SPR[tb++]=TMP[ta+1]&15;
      GFX_SPR[tb++]=TMP[ta+0]>>4;
      GFX_SPR[tb++]=TMP[ta+0]&15;
   }
   if(!load_rom("c17-04.bin", TMP+0x00000, 0x10000)) return;	// 16x16 SPRITES
   if(!load_rom("c17-05.bin", TMP+0x10000, 0x10000)) return;	// 16x16 SPRITES
   for(ta=0;ta<0x10000;ta++){
      GFX_SPR[tb++]=TMP[ta+0x00000]>>4;
      GFX_SPR[tb++]=TMP[ta+0x00000]&15;
      GFX_SPR[tb++]=TMP[ta+0x10000]>>4;
      GFX_SPR[tb++]=TMP[ta+0x10000]&15;
   }

   FreeMem(TMP);

   Rotate8x8(GFX_BG0,0x4000);
   Flip8x8_X(GFX_BG0,0x4000);
   Rotate16x16(GFX_SPR,0x1400);
   Flip16x16_X(GFX_SPR,0x1400);

   /*-----[Sound Setup]-----*/

   Z80ROM=RAM+0x28000;
   if(!load_rom("c17-07.bin", Z80ROM, 0x10000)) return;	// Z80 SOUND ROM

   if(!(PCMROM=AllocateMem(0x10000))) return;
   if(!load_rom("c17-06.bin", PCMROM, 0x10000)) return;

   AddTaitoYM2151(0x01C0, 0x01AB, 0x10000, (UINT8 *) Maze_of_Flott_M5205_W, NULL);

   msm5205_interface.rom[0] = PCMROM;
   msm5205_interface.romsize[0] = 0x10000;
   msm5205_interface.updatemode = MSM5205_MONO;

   /*-----------------------*/

   memset(RAM+0x00000,0x00,0x28000);

   RAM_VIDEO  = RAM+0x08000;
   RAM_SCROLL = RAM+0x1B000;
   RAM_OBJECT = RAM+0x18000;
   RAM_INPUT  = RAM+0x1A000;

   tc0110pcr_init(RAM+0x1C000, 1);

   set_colour_mapper(&col_map_xbbb_bbgg_gggr_rrrr);
   InitPaletteMap(RAM+0x1C000, 0x100, 0x10, 0x8000);

   GFX_FG0 = RAM+0x20000;

   GFX_BG0_SOLID = make_solid_mask_8x8  (GFX_BG0, 0x4000);
   GFX_SPR_SOLID = make_solid_mask_16x16(GFX_SPR, 0x1400);

   // Speed Hack
   // ----------

   WriteLong68k(&ROM[0x005CA],0x13FC0000);
   WriteLong68k(&ROM[0x005CE],0x00AA0000);

   WriteLong68k(&ROM[0x0368C],0x13FC0000);
   WriteLong68k(&ROM[0x03690],0x00AA0000);

   // Fix ROM Checksum
   // ----------------

   WriteLong68k(&ROM[0x030F6],0x4E714E71);

   // Fix Bad Writes
   // --------------

   WriteLong68k(&ROM[0x14C6A],0x4E714E71);
   WriteLong68k(&ROM[0x14C7A],0x4E714E71);

   AddEarthJoker68k();
}

void LoadGalmedes(void)
{
   int ta,tb;
   UINT8 *TMP;

   romset=2;

   RAMSize=0x38000;

   if(!(TMP=AllocateMem(0x80000))) return;
   if(!(RAM=AllocateMem(RAMSize))) return;
   if(!(GFX=AllocateMem(0x140000+0x100000))) return;

   GFX_SPR	= GFX+0x000000;
   GFX_BG0	= GFX+0x140000;

   if(!load_rom("gm-scn.bin", TMP, 0x80000)) return;	// 8x8 TILES
   tb=0;
   for(ta=0;ta<0x80000;ta+=2){
      GFX_BG0[tb++]=TMP[ta+1]>>4;
      GFX_BG0[tb++]=TMP[ta+1]&15;
      GFX_BG0[tb++]=TMP[ta+0]>>4;
      GFX_BG0[tb++]=TMP[ta+0]&15;
   }
   if(!load_rom("gm-obj.bin", TMP, 0x80000)) return;	// 16x16 SPRITES
   tb=0;
   for(ta=0;ta<0x80000;ta+=2){
      GFX_SPR[tb++]=TMP[ta+1]>>4;
      GFX_SPR[tb++]=TMP[ta+1]&15;
      GFX_SPR[tb++]=TMP[ta+0]>>4;
      GFX_SPR[tb++]=TMP[ta+0]&15;
   }
   memset(GFX_SPR+0x100000,0x00,0x40000);			// <Unused in this game>

   FreeMem(TMP);

   Rotate8x8(GFX_BG0,0x4000);
   Flip8x8_X(GFX_BG0,0x4000);
   Rotate16x16(GFX_SPR,0x1400);
   Flip16x16_X(GFX_SPR,0x1400);

   /*-----[Sound Setup]-----*/

   Z80ROM=RAM+0x28000;
   if(!load_rom("gm-snd.bin", Z80ROM, 0x10000)) return;	// Z80 SOUND ROM

   AddTaitoYM2151(0x01AF, 0x0143, 0x10000, NULL, NULL);

   /*-----------------------*/

   memset(RAM+0x00000,0x00,0x28000);

   RAM_VIDEO  = RAM+0x08000;
   RAM_SCROLL = RAM+0x1B000;
   RAM_OBJECT = RAM+0x18000;
   RAM_INPUT  = RAM+0x1A000;

   tc0110pcr_init(RAM+0x1C000, 1);

   set_colour_mapper(&col_map_xbbb_bbgg_gggr_rrrr);
   InitPaletteMap(RAM+0x1C000, 0x100, 0x10, 0x8000);


   GFX_FG0 = RAM+0x20000;

   GFX_BG0_SOLID = make_solid_mask_8x8  (GFX_BG0, 0x4000);
   GFX_SPR_SOLID = make_solid_mask_16x16(GFX_SPR, 0x1400);

   // Speed Hack
   // ----------

   WriteLong68k(&ROM[0x800A4],0x13FC0000);
   WriteLong68k(&ROM[0x800A8],0x00AA0000);

   AddEarthJoker68k();
}

void LoadEto(void)
{
   int ta,tb;
   UINT8 *TMP;

   romset=3;

   RAMSize=0x38000;

   if(!(TMP=AllocateMem(0x80000))) return;
   if(!(RAM=AllocateMem(RAMSize))) return;
   if(!(GFX=AllocateMem(0x140000+0x100000))) return;

   GFX_SPR	= GFX+0x000000;
   GFX_BG0	= GFX+0x140000;

   if(!load_rom("eto-4.3", TMP, 0x80000)) return;	// 8x8 TILES
   tb=0;
   for(ta=0;ta<0x80000;ta+=2){
      GFX_BG0[tb++]=TMP[ta+1]>>4;
      GFX_BG0[tb++]=TMP[ta+1]&15;
      GFX_BG0[tb++]=TMP[ta+0]>>4;
      GFX_BG0[tb++]=TMP[ta+0]&15;
   }
   if(!load_rom("eto-3.6", TMP, 0x80000)) return;	// 16x16 SPRITES
   tb=0;
   for(ta=0;ta<0x80000;ta+=2){
      GFX_SPR[tb++]=TMP[ta+1]>>4;
      GFX_SPR[tb++]=TMP[ta+1]&15;
      GFX_SPR[tb++]=TMP[ta+0]>>4;
      GFX_SPR[tb++]=TMP[ta+0]&15;
   }
   memset(GFX_SPR+0x100000,0x00,0x40000);			// <Unused in this game>

   FreeMem(TMP);

   /*-----[Sound Setup]-----*/

   Z80ROM=RAM+0x28000;
   if(!load_rom("eto-5.27", Z80ROM, 0x10000)) return;	// Z80 SOUND ROM

   AddTaitoYM2151(0x01CF, 0x01AB, 0x10000, NULL, NULL);		// 100% not sure

   /*-----------------------*/

   memset(RAM+0x00000,0x00,0x28000);

   RAM_VIDEO  = RAM+0x08000;
   RAM_SCROLL = RAM+0x1B000;
   RAM_OBJECT = RAM+0x18000;
   RAM_INPUT  = RAM+0x1A000;

   tc0110pcr_init(RAM+0x1C000, 1);

   set_colour_mapper(&col_map_xbbb_bbgg_gggr_rrrr);
   InitPaletteMap(RAM+0x1C000, 0x100, 0x10, 0x8000);


   GFX_FG0 = RAM+0x20000;

   GFX_BG0_SOLID = make_solid_mask_8x8  (GFX_BG0, 0x4000);
   GFX_SPR_SOLID = make_solid_mask_16x16(GFX_SPR, 0x1400);

   // Speed Hack
   // ----------

   // ...

   AddEarthJoker68k();
}

void ClearEarthJoker(void)
{
   RemoveTaitoYM2151();
}

void ExecuteEarthJokerFrame(void)
{
   cpu_execute_cycles(CPU_68K_0, CPU_FRAME_MHz(12,60));	// M68000 12MHz (60fps)
   cpu_interrupt(CPU_68K_0, 5);

   Taito2151_Frame();			// Z80 and YM2151
}

void DrawEarthJoker(void)
{
   ClearPaletteMap();

   // Init tc0100scn emulation
   // ------------------------

   tc0100scn_layer_count = 0;
   tc0100scn[0].ctrl = ReadWord(RAM_SCROLL+12);

   // Init tc0002obj emulation
   // ------------------------

   tc0002obj.ctrl	= ReadWord(&RAM[0x1B010]);

   // BG0
   // ---

   render_tc0100scn_layer_mapped_r270(0,0);

   // BG1+OBJECT
   // ----------

   if((tc0002obj.ctrl & 0x2000)==0){
      render_tc0100scn_layer_mapped_r270(0,1);
      if(tc0100scn_layer_count==0){
      tc0100scn_layer_count = 1;
      clear_game_screen(0);
      }
      render_tc0002obj_mapped_r270();
   }
   else{
      if(tc0100scn_layer_count==0){
      tc0100scn_layer_count = 1;
      clear_game_screen(0);
      }
      render_tc0002obj_mapped_r270();
      render_tc0100scn_layer_mapped_r270(0,1);
   }

   // FG0
   // ---

   render_tc0100scn_layer_mapped_r270(0,2);
}

void DrawEto(void)
{
   ClearPaletteMap();

   // Init tc0100scn emulation
   // ------------------------

   tc0100scn_layer_count = 0;
   tc0100scn[0].ctrl = ReadWord(RAM_SCROLL+12);

   // Init tc0002obj emulation
   // ------------------------

   tc0002obj.ctrl	= ReadWord(&RAM[0x1B010]);

   // BG0
   // ---

   render_tc0100scn_layer_mapped(0,0,0);

   // BG1+OBJECT
   // ----------

   if((tc0002obj.ctrl & 0x01)==0){
      render_tc0100scn_layer_mapped(0,1,1);
      if(tc0100scn_layer_count==0){
      tc0100scn_layer_count = 1;
      clear_game_screen(0);
      }
      render_tc0002obj_mapped();
   }
   else{
      if(tc0100scn_layer_count==0){
      tc0100scn_layer_count = 1;
      clear_game_screen(0);
      }
      render_tc0002obj_mapped();
      render_tc0100scn_layer_mapped(0,1,1);
   }

   // FG0
   // ---

   render_tc0100scn_layer_mapped(0,2,1);
}

/*-------[Maze of Flott MSM5205 Port]-------*/

void Maze_of_Flott_M5205_W(UINT16 offset, UINT8 data)
{
  int ta;

  if((offset&15)==0){

    switch(data){
      case 0x00: ta=0; break;
      case 0x09: ta=1; break;
      case 0x1A: ta=2; break;
      case 0x27: ta=3; break;
      case 0x83: ta=4; break;
      case 0xAD: ta=5; break;
      case 0xDD: ta=6; break;
      case 0xF8: ta=7; break;
      default:  ta=-1; break;
    }

    if(ta>=0){
      MSM5205buffer_request( 0, ta );
    }
  }
}
