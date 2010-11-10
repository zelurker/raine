/*
Hyper Pacman

its like snow bros, but with a bit more ram, more tiles, and different sound hardware, the original has protection


*/

#include "gameinc.h"
#include "sasound.h"		// sample support routines
#include "2151intf.h"
#include "adpcm.h"
#include "3812intf.h"
#include "blit.h" // clear_game_screen
#include "timer.h"
#include "savegame.h"
#include "emumain.h" // set_reset_function

static struct DIR_INFO hyperpac_dirs[] =
{
   { "hyper_pacman", },
   { "hyperpac", },
   { NULL, },
};

static struct DIR_INFO hyperpcb_dirs[] =
{
   { "hyper_pacman_bootleg", },
   { "hyperpcb", },
   { ROMOF("hyperpac") },
   { CLONEOF("hyperpac") },
   { NULL, },
};

static struct DIR_INFO twinkle_dirs[] =
{
   { "twinkle", },
   { NULL, },
};

static struct DIR_INFO _3in1semi_dirs[] =
{
   { "3in1semi", },
   { "xess_the_new_revolution", },
   { NULL, },
};


static struct DIR_INFO cookbib2_dirs[] =
{
   { "cookie_and_bibi_2", },
   { "cookbib2", },
   { NULL, },
};

static struct DIR_INFO moremore_dirs[] =
{
   { "more_more", },
   { "moremore", },
   { NULL, },
};


static struct DIR_INFO moremorp_dirs[] =
{
   { "more_more_plus", },
   { "moremorp", },
   { NULL, },
};

static struct DIR_INFO finalttr_dirs[] =
{
   { "final_tetris", },
   { "finalttr", },
   { NULL, },
};

static struct DIR_INFO snow_bros_dirs[] =
{
   { "snow_bros", },
   { "snowbros", },
   { NULL, },
};

static struct DIR_INFO snow_bros_alt_dirs[] =
{
   { "snow_bros_alt", },
   { "snowbroa", },
   { ROMOF("snowbros"), },
   { CLONEOF("snowbros"), },
   { NULL, },
};

static struct DIR_INFO snow_bros_japanese_dirs[] =
{
   { "snow_bros_japanese", },
   { "snowbroj", },
   { ROMOF("snowbros"), },
   { CLONEOF("snowbros"), },
   { NULL, },
};

static struct DIR_INFO snow_bros_alt_2_dirs[] =
{
   { "snow_bros_alt_2", },
   { "snowbra2", },
   { "snowbrob", },
   { ROMOF("snowbros"), },
   { CLONEOF("snowbros"), },
   { NULL, },
};

static struct DIR_INFO winter_bobble_dirs[] =
{
   { "winter_bobble", },
   { "wintbob", },
   { ROMOF("snowbros"), },
   { CLONEOF("snowbros"), },
   { NULL, },
};

static struct INPUT_INFO finalttr_inputs[] =
{
  INP0( P1_UP, 0x12401, 0x01 ),
  INP0( P1_DOWN, 0x12401, 0x02 ),
  INP0( P1_LEFT, 0x12401, 0x04 ),
  INP0( P1_RIGHT, 0x12401, 0x08 ),
  INP0( P1_B1, 0x12401, 0x10 ),
  INP0( P1_B2, 0x12401, 0x20 ),
  INP0( P1_B3, 0x12401, 0x40 ),

  INP0( P2_UP, 0x12403, 0x01 ),
  INP0( P2_DOWN, 0x12403, 0x02 ),
  INP0( P2_LEFT, 0x12403, 0x04 ),
  INP0( P2_RIGHT, 0x12403, 0x08 ),
  INP0( P2_B1, 0x12403, 0x10 ),
  INP0( P2_B2, 0x12403, 0x20 ),
  INP0( P2_B3, 0x12403, 0x40 ),

  INP0( P1_START, 0x12405, 0x01 ),
  INP0( P2_START, 0x12405, 0x02 ),
  INP0( COIN1, 0x12405, 0x04 ),
  INP0( COIN2, 0x12405, 0x08 ),
  INP0( TILT, 0x12405, 0x20 ),
  INP0( COIN3, 0x12405, 0x40 ),
   END_INPUT
};

static struct DSW_DATA dsw_data_finalttr_0[] =
{
  DSW_DEMO_SOUND( 0x0001, 0x0000),
  { MSG_UNKNOWN, 0x0002, 2 },
  { MSG_OFF, 0x0002},
  { MSG_ON, 0x0000},
  { MSG_UNKNOWN, 0x0004, 2 },
  { MSG_OFF, 0x0004},
  { MSG_ON, 0x0000},
  { MSG_COINAGE, 0x0038, 4 },
  { MSG_4COIN_1PLAY, 0x0000},
  { MSG_3COIN_1PLAY, 0x0010},
  { MSG_2COIN_1PLAY, 0x0030},
  { MSG_1COIN_1PLAY, 0x0038},
  { MSG_UNKNOWN, 0x0040, 2 },
  { MSG_OFF, 0x0040},
  { MSG_ON, 0x0000},
  { MSG_UNKNOWN, 0x0080, 2 },
  { MSG_OFF, 0x0080},
  { MSG_ON, 0x0000},
  { NULL, 0}
};

static struct DSW_DATA dsw_data_finalttr_1[] =
{
  { MSG_UNKNOWN, 0x0001, 2 },
  { MSG_OFF, 0x0001},
  { MSG_ON, 0x0000},
  { MSG_UNKNOWN, 0x0002, 2 },
  { MSG_OFF, 0x0002},
  { MSG_ON, 0x0000},
  { "Time", 0x000c, 4 },
  { "60 Seconds", 0x0000},
  { "90 Seconds", 0x000c},
  { "120 Seconds", 0x0008},
  { "150 Seconds", 0x0004},
  { MSG_UNKNOWN, 0x0010, 2 },
  { MSG_OFF, 0x0010},
  { MSG_ON, 0x0000},
  { MSG_UNKNOWN, 0x0020, 2 },
  { MSG_OFF, 0x0020},
  { MSG_ON, 0x0000},
  { MSG_UNKNOWN, 0x0040, 2 },
  { MSG_OFF, 0x0040},
  { MSG_ON, 0x0000},
  { MSG_UNKNOWN, 0x0080, 2 },
  { MSG_OFF, 0x0080},
  { MSG_ON, 0x0000},
  { NULL, 0}
};

static struct DSW_INFO finalttr_dsw[] =
{
  { 0x12400, 0xff, dsw_data_finalttr_0 },
  { 0x12402, 0xff, dsw_data_finalttr_1 },
  { 0, 0, NULL }
};

static int romset;

static struct ROM_INFO hyperpac_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x00001,  0x20000,
            "hyperpac.h12",  0x2cf0531a , "hyperpac.i12",  0x9c7d85b8 ),
  { "hyperpac.u1", 0x10000 , 0x03faf88e , REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "hyperpac.j15", 0x40000, 0xfb9f468d , REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "hyperpac.a4", 0x40000, 0xbd8673da , REGION_GFX1, 0x000000, LOAD_NORMAL },
  { "hyperpac.a5", 0x40000, 0x5d90cd82 , REGION_GFX1, 0x040000, LOAD_NORMAL },
  { "hyperpac.a6", 0x40000, 0x61d86e63 , REGION_GFX1, 0x080000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO twinkle_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x00001,  0x20000,
            "uh12.bin",  0xa99626fe , "ui12.bin",  0x5af73684 ),
  { "u1.bin",   0x10000, 0xe40481da , REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "uj15.bin", 0x40000, 0x0a534b37 , REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "ua4.bin",  0x80000, 0x6b64bb09 , REGION_GFX1, 0x000000, LOAD_NORMAL },
  { "protdata.bin", 0x000200, 0x00d3e4b4, REGION_PROMS, 0, LOAD_NORMAL, },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _3in1semi_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x00001,           0x40000,
            "u52",  0xb0e4a0f7 , "u74",  0x266862c4 ),
  { "u35",          0x10000, 0xe40481da , REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "u14",          0x40000, 0xc83c11be , REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "u75",          0x80000, 0xb66a0db6 , REGION_GFX1, 0x000000, LOAD_NORMAL },
  { "u76",          0x80000, 0x5f4b48ea , REGION_GFX1, 0x080000, LOAD_NORMAL },
  { "u77",          0x80000, 0xd44211e3 , REGION_GFX1, 0x100000, LOAD_NORMAL },
  { "u78",          0x80000, 0xaf596afc , REGION_GFX1, 0x180000, LOAD_NORMAL },
  { "protdata.bin", 0x00200, 0x85deba7c, REGION_PROMS, 0, LOAD_NORMAL, },
  { NULL, 0, 0, 0, 0, 0 }
};



static struct ROM_INFO cookbib2_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x00001,  0x40000,
            "cookbib2.02",  0xb2909460 , "cookbib2.01",  0x65aafde2 ),
  { "cookbib2.07", 0x10000, 0xf59f1c9a , REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "cookbib2.06", 0x20000, 0x5e6f76b8 , REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "cookbib2.05", 0x80000, 0x89fb38ce , REGION_GFX1, 0x000000, LOAD_NORMAL },
  { "cookbib2.04", 0x80000, 0xf240111f , REGION_GFX1, 0x080000, LOAD_NORMAL },
  { "cookbib2.03", 0x40000, 0xe1604821 , REGION_GFX1, 0x100000, LOAD_NORMAL },
  { "protdata.bin", 0x000200, 0xae6d8ed5, REGION_PROMS, 0, LOAD_NORMAL, },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO moremore_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x00001,  0x40000,
            "u52.bin",  0xcea4b246 , "u74.bin",  0x2acdcb88 ),
  { "u35.bin", 0x10000, 0x92dc95fc , REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "u14.bin", 0x40000, 0x90580088 , REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "u75.bin", 0x80000, 0xd671815c , REGION_GFX1, 0x000000, LOAD_NORMAL },
  { "u76.bin", 0x80000, 0xe0d479e8 , REGION_GFX1, 0x080000, LOAD_NORMAL },
  { "u77.bin", 0x80000, 0x60a281da , REGION_GFX1, 0x100000, LOAD_NORMAL },
  { "u78.bin", 0x80000, 0xe2723b4e , REGION_GFX1, 0x180000, LOAD_NORMAL },
  { "protdata.bin", 0x000200, 0x782dd2aa, REGION_PROMS, 0, LOAD_NORMAL, },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO moremorp_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x00001,  0x40000,
            "mmp_u52.bin",  0x66baf9b2 , "mmp_u74.bin",  0x7c6fede5 ),
  { "mmp_u35.bin", 0x10000, 0x4d098cad , REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "mmp_u14.bin", 0x40000, 0x211a2566 , REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "mmp_u75.bin", 0x80000, 0xaf9e824e , REGION_GFX1, 0x000000, LOAD_NORMAL },
  { "mmp_u76.bin", 0x80000, 0xc42af064 , REGION_GFX1, 0x080000, LOAD_NORMAL },
  { "mmp_u77.bin", 0x80000, 0x1d7396e1 , REGION_GFX1, 0x100000, LOAD_NORMAL },
  { "mmp_u78.bin", 0x80000, 0x5508d80b , REGION_GFX1, 0x180000, LOAD_NORMAL },
  { "protdata.bin", 0x000200, 0x782dd2aa, REGION_PROMS, 0, LOAD_NORMAL, },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO finalttr_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x00001, 0x20000,
            "9.5o",  0x58d3640e , "10.7o",  0xeecc83e5 ),
  { "12.5r",0x10000, 0x4bc21361 , REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "11.7p",0x20000, 0x2e331022 , REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "5.1d", 0x40000, 0x64a450f3 , REGION_GFX1, 0x000000, LOAD_NORMAL },
  { "6.1f", 0x40000, 0x7281a3cc , REGION_GFX1, 0x040000, LOAD_NORMAL },
  { "7.1g", 0x40000, 0xec80f442 , REGION_GFX1, 0x080000, LOAD_NORMAL },
  { "9.1h", 0x40000, 0x2ebd316d , REGION_GFX1, 0x0c0000, LOAD_NORMAL },
  { "protdata.bin", 0x000200, 0xd5bbb006, REGION_PROMS, 0, LOAD_NORMAL, },
  { NULL, 0, 0, 0, 0, 0 }
};



static struct ROM_INFO hyperpcb_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x00001,  0x20000,
            "hpacuh12.bin",  0x633ab2c6 , "hpacui12.bin",  0x23dc00d1 ),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO snowbros_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x000000,  0x00020000,
                 "sn6.bin",  0x4899ddcf,      "sn5.bin",  0xad310d3f),
   {   "snowbros.4", 0x00008000, 0xe6eab4e4, REGION_ROM2, 0x000000, LOAD_NORMAL, },
   {   "sbros-1.41", 0x00080000, 0x16f06b3a, REGION_GFX1, 0x000000, LOAD_NORMAL, },

	/* where were these from, a bootleg? */
/*    {          "ch0", 0x00020000, 0x36d84dfe, REGION_GFX1, 0x000000, LOAD_NORMAL, }, */
/*    {          "ch1", 0x00020000, 0x76347256, REGION_GFX1, 0x020000, LOAD_NORMAL, }, */
/*    {          "ch2", 0x00020000, 0xfdaa634c, REGION_GFX1, 0x040000, LOAD_NORMAL, }, */
/*    {          "ch3", 0x00020000, 0x34024aef, REGION_GFX1, 0x060000, LOAD_NORMAL, }, */
   {           NULL,          0,          0,           0,        0,           0, },
};

static struct ROM_INFO snowbroa_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x000000,  0x00020000,
             "snowbros.3a",  0x10cb37e1,  "snowbros.2a",  0xab91cc1e),
   {           NULL,          0,          0,           0,        0,           0, },
};

static struct ROM_INFO snowbroj_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x000000,  0x00020000,
              "snowbros.3",  0x3f504f9e,   "snowbros.2",  0x854b02bc),
   {           NULL,          0,          0,           0,        0,           0, },
};

static struct ROM_INFO snowbra2_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x000000,  0x00020000,
                "sbros3-a",  0x301627d6,     "sbros2-a",  0xf6689f41),
   {           NULL,          0,          0, 0, 0, 0, },
};

static struct ROM_INFO wintbob_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x000000,  0x00010000,
                "wb03.bin",  0xdf56e168,     "wb01.bin",  0x05722f17),
  LOAD8_16(  REGION_ROM1,  0x020000,  0x00010000,
                "wb04.bin",  0x53be758d,     "wb02.bin",  0xfc8e292e),
   {     "wb05.bin", 0x00010000, 0x53fe59df, REGION_ROM2, 0x000000, LOAD_NORMAL, },
  LOAD8_16(  REGION_GFX1,  0x000001,  0x00010000,
                "wb06.bin",  0x68204937,     "wb07.bin",  0x53f40978),
  LOAD8_16(  REGION_GFX1,  0x040001,  0x00010000,
                "wb08.bin",  0x9497b88c,     "wb09.bin",  0x9be718ca),
  LOAD8_16(  REGION_GFX1,  0x060000,  0x00010000,
                "wb10.bin",  0x5fa22b1e,     "wb11.bin",  0x41cb4563),
  LOAD8_16(  REGION_GFX1,  0x020000,  0x00010000,
                "wb12.bin",  0xef4e04c7,     "wb13.bin",  0x426921de),
   {           NULL,          0,          0,           0,        0,         0,   },
};

static struct INPUT_INFO hyperpac_inputs[] =
{
  INP1( P1_UP, 0x12401, 0x01 ),
  INP1( P1_DOWN, 0x12401, 0x02 ),
  INP1( P1_LEFT, 0x12401, 0x04 ),
  INP1( P1_RIGHT, 0x12401, 0x08 ),
  INP1( P1_B1, 0x12401, 0x10 ),
  INP1( P1_B2, 0x12401, 0x20 ),
  INP1( P1_B3, 0x12401, 0x40 ),

  INP1( P2_UP, 0x12403, 0x01 ),
  INP1( P2_DOWN, 0x12403, 0x02 ),
  INP1( P2_LEFT, 0x12403, 0x04 ),
  INP1( P2_RIGHT, 0x12403, 0x08 ),
  INP1( P2_B1, 0x12403, 0x10 ),
  INP1( P2_B2, 0x12403, 0x20 ),
  INP1( P2_B3, 0x12403, 0x40 ),

  INP1( P1_START, 0x12405, 0x01 ),
  INP1( P2_START, 0x12405, 0x02 ),
  INP1( COIN1, 0x12405, 0x04 ),
  INP1( COIN2, 0x12405, 0x08 ),
   END_INPUT
};

static struct INPUT_INFO snow_bros_inputs[] =
{
   INP0( COIN1, 0x006405, 0x04 ),
   INP0( COIN2, 0x006405, 0x08 ),
   INP0( TILT, 0x006405, 0x20 ),
   INP0( P1_START, 0x006405, 0x01 ),
   INP0( P1_UP, 0x006401, 0x01 ),
   INP0( P1_DOWN, 0x006401, 0x02 ),
   INP0( P1_LEFT, 0x006401, 0x04 ),
   INP0( P1_RIGHT, 0x006401, 0x08 ),
   INP0( P1_B1, 0x006401, 0x10 ),
   INP0( P1_B2, 0x006401, 0x20 ),
   INP0( P2_START, 0x006405, 0x02 ),
   INP0( P2_UP, 0x006403, 0x01 ),
   INP0( P2_DOWN, 0x006403, 0x02 ),
   INP0( P2_LEFT, 0x006403, 0x04 ),
   INP0( P2_RIGHT, 0x006403, 0x08 ),
   INP0( P2_B1, 0x006403, 0x10 ),
   INP0( P2_B2, 0x006403, 0x20 ),
   END_INPUT
};

static struct DSW_DATA dsw_data_hyperpac_0[] =
{
  DSW_DEMO_SOUND( 0x0000, 0x0001),
  { MSG_LIVES, 0x0002, 2 },
  { "3" , 0x0002},
  { "5" , 0x0000},
  { MSG_COINAGE, 0x001c, 8 },
  { MSG_5COIN_1PLAY, 0x0000},
  { MSG_4COIN_1PLAY, 0x0004},
  { MSG_3COIN_1PLAY, 0x0008},
  { MSG_2COIN_1PLAY, 0x000c},
  { MSG_1COIN_1PLAY, 0x001c},
  { MSG_2COIN_3PLAY, 0x0014},
  { MSG_1COIN_1PLAY, 0x0018},
  { MSG_1COIN_3PLAY, 0x0010},
  { MSG_DIFFICULTY, 0x0060, 4 },
  { "Easy" , 0x0000},
  { "Normal" , 0x0060},
  { "Hard" , 0x0040},
  { "Hardest" , 0x0020},
  DSW_SERVICE( 0, 0x0080),
  { NULL, 0}
};

static struct DSW_DATA dsw_data_hyperpac_1[] =
{
  DSW_UNUSED( 0x0000, 0x0001),
  DSW_UNUSED( 0x0000, 0x0002),
  DSW_UNUSED( 0x0000, 0x0004),
  DSW_UNUSED( 0x0000, 0x0008),
  DSW_UNUSED( 0x0000, 0x0010),
  DSW_UNUSED( 0x0000, 0x0020),
  DSW_UNUSED( 0x0000, 0x0040),
  DSW_UNUSED( 0x0000, 0x0080),
  { NULL, 0}
};

static struct DSW_INFO hyperpac_dsw[] =
{
  { 0x12400, 0xfe, dsw_data_hyperpac_0 },
  { 0x12402, 0xff, dsw_data_hyperpac_1 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_snow_bros_0[] =
{
   { "Country",               0x01, 0x02 },
   { "America",               0x01},
   { "Europe",                0x00},
   DSW_SCREEN( 0x02, 0x00),
   DSW_TEST_MODE( 0x00, 0x04),
   DSW_DEMO_SOUND( 0x08, 0x00),
   { MSG_COIN1,               0x30, 0x04 },
   { "1/1",                   0x30},
   { "1/2; 2/1",              0x20},
   { "2/1; 3/1",              0x10},
   { "2/3; 4/1",              0x00},
   { MSG_COIN2,               0xC0, 0x04 },
   { "1/1",                   0xC0},
   { "1/2; 1/3",              0x80},
   { "2/1; 1/4",              0x40},
   { "2/3; 1/6",              0x00},
   { NULL,                    0,   },
};

static struct DSW_DATA dsw_data_snow_bros_1[] =
{
   { MSG_DIFFICULTY,          0x03, 0x04 },
   { MSG_NORMAL,              0x03},
   { MSG_EASY,                0x02},
   { MSG_HARD,                0x01},
   { MSG_HARDEST,             0x00},
   { "Extra Life at",         0x0C, 0x04 },
   { "100k",                  0x0C},
   { "200k",                  0x08},
   { "100k 200k",             0x04},
   { "Nothing",               0x00},
   { "Lives",                 0x30, 0x04 },
   { "3",                     0x30},
   { "1",                     0x20},
   { "4",                     0x10},
   { "2",                     0x00},
   { "Invulnerability",                 0x40, 0x02 },
   { MSG_OFF,                 0x40},
   { MSG_ON,                  0x00},
   { "Continue",              0x80, 0x02 },
   { MSG_ON,                  0x80},
   { MSG_OFF,                 0x00},
   { NULL,                    0,   },
};

static struct DSW_INFO snow_bros_dsw[] =
{
   { 0x006400, 0xFF, dsw_data_snow_bros_0 },
   { 0x006402, 0xFF, dsw_data_snow_bros_1 },
   { 0,        0,    NULL,      },
};

static struct DSW_DATA dsw_data_moremore_0[] =
{
  DSW_DEMO_SOUND( 0x0000, 0x0001),
  { MSG_COINAGE, 0x000e, 8 },
  { MSG_5COIN_1PLAY, 0x0000},
  { MSG_4COIN_1PLAY, 0x0002},
  { MSG_3COIN_1PLAY, 0x0004},
  { MSG_2COIN_1PLAY, 0x0006},
  { MSG_1COIN_1PLAY, 0x000e},
  { MSG_2COIN_3PLAY, 0x000a},
  { MSG_1COIN_2PLAY, 0x000c},
  { MSG_1COIN_3PLAY, 0x0008},
  { MSG_DIFFICULTY, 0x0070, 8 },
  { "Level 1", 0x0020},
  { "Level 2", 0x0010},
  { "Level 3", 0x0000},
  { "Level 4", 0x0070},
  { "Level 5", 0x0060},
  { "Level 6", 0x0050},
  { "Level 7", 0x0040},
  { "Level 8", 0x0030},
  DSW_SERVICE( 0, 0x0080),
  { NULL, 0}
};

static struct DSW_DATA dsw_data_moremore_1[] =
{
  { MSG_FREE_PLAY, 0x0080, 2 },
  { MSG_OFF, 0x0080},
  { MSG_ON, 0x0000},
  { NULL, 0}
};

static struct DSW_INFO moremore_dsw[] =
{
  { 0x12400, 0xfe, dsw_data_moremore_0 },
  { 0x12402, 0xff, dsw_data_moremore_1 },
  { 0, 0, NULL }
};

static struct GFX_LAYOUT hyper_pacman_object =
{
   16, 16,
   RGN_FRAC(1,1),
   4,
   { 0, 1, 2, 3 },
   { 4, 0, 8*32+4, 8*32+0, 20,16, 8*32+20, 8*32+16,
     12, 8, 8*32+12, 8*32+8, 28, 24, 8*32+28, 8*32+24 },
   { 0*32, 2*32, 1*32, 3*32, 16*32+0*32, 16*32+2*32, 16*32+1*32, 16*32+3*32,
     4*32, 6*32, 5*32, 7*32, 16*32+4*32, 16*32+6*32, 16*32+5*32, 16*32+7*32 },
   32*32
};

static struct GFX_LIST hyper_pacman_gfx[] =
{
   { REGION_GFX1, &hyper_pacman_object, },
   { 0,           NULL,              },
};

static struct GFX_LAYOUT snow_bros_object =
{
   16, 16,
   RGN_FRAC(1,1),
   4,
   { 0, 1, 2, 3 },
   { STEP8(0, 4 ), STEP8(8*32,  4 ) },
   { STEP8(0, 32), STEP8(16*32, 32) },
   32*32,
};

static struct GFX_LIST snow_bros_gfx[] =
{
   { REGION_GFX1, &snow_bros_object, },
   { 0,           NULL,              },
};


static struct GFX_LAYOUT winter_bobble_object =
{
   16,16,
   RGN_FRAC(1,1),
   4,
   { 0, 1, 2, 3 },
   { STEP4(3*4,-4), STEP4(7*4,-4), STEP4(11*4,-4), STEP4(15*4,-4) },
   { STEP16(0,64) },
   16*64
};

static struct GFX_LIST winter_bobble_gfx[] =
{
   { REGION_GFX1, &winter_bobble_object, },
   { 0,           NULL,                  },
};

static struct YM2151interface ym2151_interface =
{
	1,
	4000000,	/* 4 MHz??? */
	{ YM3012_VOL(50,MIXER_PAN_LEFT,50,MIXER_PAN_RIGHT) },
	{ z80_irq_handler }
};

static struct OKIM6295interface okim6295_interface =
{
	1,			/* 1 chip */
	{ 7575 },		/* 7575Hz playback? */
	{ REGION_SOUND1 },
	{ 240 }
};

static struct SOUND_INFO hyper_pacman_sound[] =
{
   { SOUND_YM2151J,  &ym2151_interface,    },
   { SOUND_M6295,  &okim6295_interface    },
   { 0,             NULL,                 },
};

static UINT8 *vbl; // vbl location for speed hack

static UINT16 speed_hack(UINT32 offset) {
  UINT16 ret = ReadWord(vbl);
  if (ret) {
    Stop68000(offset,0);
  }
  return ret;
}

static UINT8 *RAM_SPR, *GFX_SPR_SOLID;

static UINT16 semicom_prot_latch = 0x0a0a;

static void install_protdata() {
  memcpy(RAM+0x2f000,load_region[REGION_PROMS],0x200);
  ByteSwap(RAM+0x02f000,0x200);
}


static void finalttr_install_protdata() {
  memcpy(RAM+0x22000,load_region[REGION_PROMS],0x200);
  ByteSwap(RAM+0x022000,0x200);
}

static void soundlatch_w(UINT32 offset, UINT16 data) {
  /* Actually this sound command is a little particular : normaly this driver
   * sends its commands by byte, not by word. finalttr is the only one using
   * word write for that, but it's better to filter out the 0 command or the
   * z80 is reseted at very bad moments (stopping sound completely).
   * Anyway, it works like that... */
  if (data)
    latch = data;
}

#define Z80_FRAME CPU_FRAME_MHz(4,60)

void load_hyper_pacman(void)
{
   RAMSize=0x30000;
   romset = 0;

   if(!(RAM=AllocateMem(RAMSize))) return;

   // Setup Z80 memory map
   // --------------------

   AddZ80AROMBase(Z80ROM, 0x0038, 0x0066);

   AddZ80AReadByte(0x0000, 0xcfFF, NULL, load_region[REGION_ROM2]); // Z80 ROM
   AddZ80AReadByte(0xd000, 0xd7ff, NULL, RAM);
   AddZ80AReadByte(0xf001, 0xf001, YM2151_status_port_0_r, NULL);
   AddZ80AReadByte(0xf008, 0xf008, NULL, (UINT8*)&latch);
   AddZ80AReadByte(0x0000, 0xFFFF, DefBadReadZ80,               NULL);
   AddZ80AReadByte(    -1,     -1, NULL,                        NULL);

   AddZ80AWriteByte(0xd000, 0xd7FF, NULL, RAM); // Z80 RAM
   AddZ80AWriteByte(0xf000, 0xf000, YM2151_register_port_0_w, NULL);
   AddZ80AWriteByte(0xf001, 0xf001, YM2151_data_port_0_w, NULL);
   AddZ80AWriteByte(0xf002, 0xf002, OKIM6295_data_0_w, NULL);
   AddZ80AWriteByte(0x0000, 0xFFFF, DefBadWriteZ80,             NULL);
   AddZ80AWriteByte(    -1,     -1, NULL,                       NULL);

   AddZ80AReadPort(0x00, 0xFF, DefBadReadZ80,           NULL);
   AddZ80AReadPort(  -1,   -1, NULL,                    NULL);

   AddZ80AWritePort(0x00, 0xFF, DefBadWriteZ80,         NULL);
   AddZ80AWritePort(  -1,   -1, NULL,                   NULL);

   AddZ80AInit();

   RAM_SPR = RAM+0x010000;
   GFX_SPR_SOLID = NULL;

   InitPaletteMap(RAM+0x12000, 0x10, 0x10, 0x8000);

   set_colour_mapper(&col_map_xbbb_bbgg_gggr_rrrr);

/*
 *  StarScream Main 68000 Setup
 */

   ByteSwap(ROM,get_region_size(REGION_CPU1));

   AddMemFetch(0x000000, 0x07FFFF, ROM+0x000000-0x000000);
   AddMemFetch(0x100000, 0x10fFFF, RAM+0x020000-0x100000);
   AddMemFetch(-1, -1, NULL);

   AddReadBW(0x000000, 0x07FFFF, NULL, ROM+0x000000);                 // 68000 ROM
   if (!strcmp(current_game->main_name,"hyperpac")) {
     /* Taken from mame... */
     /* simulate RAM initialization done by the protection MCU */
     /* not verified on real hardware */
     WriteWord(&RAM[0x2e000], 0x4ef9);
     WriteWord(&RAM[0x2e002], 0x0000);
     WriteWord(&RAM[0x2e004], 0x062c);

     WriteWord(&RAM[0x2e080], 0xfedc);
     WriteWord(&RAM[0x2e082], 0xba98);
     WriteWord(&RAM[0x2e084], 0x7654);
     WriteWord(&RAM[0x2e086], 0x3210);
     AddReadWord(0x102290, 0x102291, speed_hack, NULL);    // Trap Idle 68000
     vbl = &RAM[0x22290];
   } else if (!strcmp(current_game->main_name,"hyperpcb")) {
     AddReadWord(0x104490, 0x104491, speed_hack, NULL);    // Trap Idle 68000
     vbl = &RAM[0x24490];
   } else if (!strcmp(current_game->main_name,"cookbib2")) {
     /* this should really be init every reset */
     set_reset_function(install_protdata);
   } else if (!strcmp(current_game->main_name,"twinkle")) {
     /* this should really be init every reset */
     set_reset_function(install_protdata);
     romset = 1; // clear screen with pen f0
   } else if (!strcmp(current_game->main_name,"finalttr")) {
     /* this should really be init every reset */
     set_reset_function(finalttr_install_protdata);
//     romset = 1; // clear screen with pen f0
   } else if (!strcmp(current_game->main_name,"3in1semi")) { // doesn't work??
     romset = 1; // clear screen with pen f0
     /* this should really be init every reset */
     set_reset_function(install_protdata);
     AddReadBW(0x200000, 0x200001, NULL, (UINT8*)&semicom_prot_latch);      // extraprot
   } else if (!strcmp(current_game->main_name,"moremore")) {
     romset = 1; // clear screen with pen f0
     /* this should really be init every reset */
     set_reset_function(install_protdata);
     AddReadBW(0x200000, 0x200001, NULL, (UINT8*)&semicom_prot_latch);      // extraprot
   } else if (!strcmp(current_game->main_name,"moremorp")) {
     romset = 1; // clear screen with pen f0
     /* this should really be init every reset */
     set_reset_function(install_protdata);
     AddReadBW(0x200000, 0x200001, NULL, (UINT8*)&semicom_prot_latch);      // extraprot
     // This game has 3 counters for the 3 interrupts. To guess which is which is not easy!
/*      AddReadWord(0x1022b2,0x1022b3,speed_hack,NULL); */
/*      vbl = &RAM[0x222b2]; */
   }

   AddRWBW(0x100000, 0x10FFFF, NULL, RAM+0x020000);                 // 68000 RAM
   AddReadBW(0x500000, 0x50000F, NULL, RAM+0x012400);  // INPUT
   AddRWBW(0x600000, 0x6003FF, NULL, RAM+0x012000);    // COLOUR RAM

   AddRWBW(0x700000, 0x701FFF, NULL, RAM+0x010000);   // SPRITE RAM
   AddReadByte(0x000000, 0xFFFFFF, DefBadReadByte, NULL);      // <Bad Reads>
   AddReadByte(-1, -1, NULL, NULL);

   AddReadWord(0x000000, 0xFFFFFF, DefBadReadWord, NULL);      // <Bad Reads>
   AddReadWord(-1, -1, NULL, NULL);

   AddWriteByte(0x300001, 0x300001, NULL, (UINT8*)&latch+1); // sound
   AddWriteWord(0x300000, 0x300001, soundlatch_w, NULL); // , (UINT8*)&latch); // sound (only for finalttr !)
   AddWriteByte(0x000000, 0xFFFFFF, DefBadWriteByte, NULL);      // <Bad Writes>
   AddWriteByte(-1, -1, NULL, NULL);
   AddWriteWord(0x000000, 0xFFFFFF, DefBadWriteWord, NULL);      // <Bad Writes>
   AddWriteWord(-1, -1, NULL, NULL);

   AddInitMemory();     // Set Starscream mem pointers...
   // apparently we need to be a little higher than 4 Mhz...
   setup_z80_frame(CPU_Z80_0,Z80_FRAME);
   AddSaveData(SAVE_USER_1, (UINT8 *)&latch, sizeof(int));
}

#define FRAME CPU_FRAME_MHz(16,60)

void execute_hyper_pacman_frame(void)
{
  if (RaineSoundCard)
    execute_z80_audio_frame();
  cpu_execute_cycles(CPU_68K_0, FRAME/3);                 // Main 68000
  cpu_interrupt(CPU_68K_0, 4);
  cpu_execute_cycles(CPU_68K_0, FRAME/3);
  cpu_interrupt(CPU_68K_0, 3);
  cpu_execute_cycles(CPU_68K_0, FRAME/3);
  cpu_interrupt(CPU_68K_0, 2);
}

static void draw_hyper_pacman(void)
{
   int x,y,ta,tb;
   int zz;
   UINT8 *map;

   ClearPaletteMap();

   if (romset == 1) {
     // screen needs clearing with pen 0xf0 for more more plus but not cookie and bibi 2 (see last level)
     MAP_PALETTE_MAPPED_NEW(
            0xf,
            16,
            map
	    );
   } else {
     MAP_PALETTE_MAPPED_NEW(
            0x0,
            16,
            map
	    );
   }
   clear_game_screen(ReadLong(&map[0]));

   if (!GFX_SPR_SOLID)
     GFX_SPR_SOLID = gfx_solid[0];

   x=0;
   y=0;

   for(zz=0;zz<0x1fff;zz+=16){

      tb = RAM_SPR[zz+6];

      if(tb&4){
         x = (x + ((RAM_SPR[zz+ 8]) | ((tb&1)<<8))) & 0x1FF;
         y = (y + ((RAM_SPR[zz+10]) | ((tb&2)<<7))) & 0x1FF;
      }
      else{
         x = (32 + ((RAM_SPR[zz+ 8]) | ((tb&1)<<8))) & 0x1FF;
         y = (16 + ((RAM_SPR[zz+10]) | ((tb&2)<<7))) & 0x1FF;
      }

      if((x>16)&&(y>16)&&(x<256+32)&&(y<240+32)){

         ta = ((RAM_SPR[zz+12]) | (RAM_SPR[zz+14]<<8))&0x3FFF;

         if(GFX_SPR_SOLID[ta]){            // No pixels; skip

         MAP_PALETTE_MAPPED_NEW(
            tb>>4,
            16,
            map
         );

         if(GFX_SPR_SOLID[ta]==1){         // Some pixels; trans
            switch(RAM_SPR[zz+14]&0xC0){
            case 0x00: Draw16x16_Trans_Mapped_Rot(&GFX[ta<<8], x, y, map);        break;
            case 0x80: Draw16x16_Trans_Mapped_FlipY_Rot(&GFX[ta<<8], x, y, map);  break;
            case 0x40: Draw16x16_Trans_Mapped_FlipX_Rot(&GFX[ta<<8], x, y, map);  break;
            case 0xC0: Draw16x16_Trans_Mapped_FlipXY_Rot(&GFX[ta<<8], x, y, map); break;
            }
         }
         else{                  // all pixels; solid
            switch(RAM_SPR[zz+14]&0xC0){
            case 0x00: Draw16x16_Mapped_Rot(&GFX[ta<<8], x, y, map);        break;
            case 0x80: Draw16x16_Mapped_FlipY_Rot(&GFX[ta<<8], x, y, map);  break;
            case 0x40: Draw16x16_Mapped_FlipX_Rot(&GFX[ta<<8], x, y, map);  break;
            case 0xC0: Draw16x16_Mapped_FlipXY_Rot(&GFX[ta<<8], x, y, map); break;
            }
         }

         }

      }
   }

}

static struct VIDEO_INFO hyper_pacman_video =
{
   draw_hyper_pacman,
   256,
   224,
   32,
   VIDEO_ROTATE_NORMAL |
   VIDEO_ROTATABLE,
   hyper_pacman_gfx,
};

static struct VIDEO_INFO snow_bros_video =
{
   draw_hyper_pacman,
   256,
   224,
   32,
   VIDEO_ROTATE_NORMAL |
   VIDEO_ROTATABLE,
   snow_bros_gfx,
};

static struct VIDEO_INFO winter_bobble_video =
{
   draw_hyper_pacman,
   256,
   224,
   32,
   VIDEO_ROTATE_NORMAL |
   VIDEO_ROTATABLE,
   winter_bobble_gfx,
};

static struct YM3812interface ym3812_interface =
{
   1,              // 1 chip
   3579545,	/* 3.579545 MHz ? (hand tuned) */
   { 255 },        // Volume (emu only)
   { z80_irq_handler }
};

static struct SOUND_INFO snow_bros_sound[] =
{
   { SOUND_YM3812,  &ym3812_interface,    },
   { 0,             NULL,                 },
};

GAME( hyperpcb ,
   hyperpcb_dirs,
   hyperpcb_roms,
   hyperpac_inputs,
   hyperpac_dsw,
   NULL,

   load_hyper_pacman,
   NULL,
   &hyper_pacman_video,
   execute_hyper_pacman_frame,
   "hyperpcb",
   "Hyper Pacman (bootleg)",
   "SemiCom",
   COMPANY_ID_TOAPLAN,
   NULL,
   1990,
   hyper_pacman_sound,
   GAME_MISC
);

// necessary because hyperpcb is a clone set in mame, so we need the parent.
GAME( hyperpac ,
   hyperpac_dirs,
   hyperpac_roms,
   hyperpac_inputs,
   hyperpac_dsw,
   NULL,

   load_hyper_pacman,
   NULL,
   &hyper_pacman_video,
   execute_hyper_pacman_frame,
   "hyperpac",
   "Hyper Pacman",
   "SemiCom",
   COMPANY_ID_TOAPLAN,
   NULL,
   1990,
   hyper_pacman_sound,
   GAME_MISC
);

GAME( cookbib2 ,
   cookbib2_dirs,
   cookbib2_roms,
   hyperpac_inputs,
   hyperpac_dsw,
   NULL,

   load_hyper_pacman,
   NULL,
   &hyper_pacman_video,
   execute_hyper_pacman_frame,
   "cookbib2",
   "Cookie and Bibi 2",
   "SemiCom",
   COMPANY_ID_TOAPLAN,
   NULL,
   1996,
   hyper_pacman_sound,
   GAME_MISC
);

GAME( twinkle ,
   twinkle_dirs,
   twinkle_roms,
   hyperpac_inputs,
   moremore_dsw,
   NULL,

   load_hyper_pacman,
   NULL,
   &hyper_pacman_video,
   execute_hyper_pacman_frame,
   "twinkle",
   "Twinkle",
   "SemiCom",
   COMPANY_ID_TOAPLAN,
   NULL,
   1997,
   hyper_pacman_sound,
   GAME_MISC
);

GAME( 3in1semi ,
   _3in1semi_dirs,
   _3in1semi_roms,
   hyperpac_inputs,
   moremore_dsw,
   NULL,

   load_hyper_pacman,
   NULL,
   &hyper_pacman_video,
   execute_hyper_pacman_frame,
   "3in1semi",
   "Xess - The New Revolution (Semicom 3-in-1)",
   "SemiCom",
   COMPANY_ID_TOAPLAN,
   NULL,
   1997,
   hyper_pacman_sound,
   GAME_MISC
);

GAME( moremore ,
   moremore_dirs,
   moremore_roms,
   hyperpac_inputs,
   moremore_dsw,
   NULL,

   load_hyper_pacman,
   NULL,
   &hyper_pacman_video,
   execute_hyper_pacman_frame,
   "moremore",
   "More More",
   "SemiCom / Exit",
   COMPANY_ID_TOAPLAN,
   NULL,
   1999,
   hyper_pacman_sound,
   GAME_MISC
);

GAME( moremorp ,
   moremorp_dirs,
   moremorp_roms,
   hyperpac_inputs,
   hyperpac_dsw,
   NULL,

   load_hyper_pacman,
   NULL,
   &hyper_pacman_video,
   execute_hyper_pacman_frame,
   "moremorp",
   "More More Plus",
   "SemiCom / Exit",
   COMPANY_ID_TOAPLAN,
   NULL,
   1999,
   hyper_pacman_sound,
   GAME_MISC
);

GAME( finalttr ,
   finalttr_dirs,
   finalttr_roms,
   finalttr_inputs,
   finalttr_dsw,
   NULL,

   load_hyper_pacman,
   NULL,
   &hyper_pacman_video,
   execute_hyper_pacman_frame,
   "finalttr",
   "Final Tetris",
   "Jeil Computer System",
   COMPANY_ID_TOAPLAN,
   NULL,
   1993,
   hyper_pacman_sound,
   GAME_MISC
);

static int sport=0;

static void SoundWrite(UINT32 offset, UINT8 data)
{
   sport = data;

   print_debug("68000 Sends $%02x\n",sport);
   cpu_int_nmi(CPU_Z80_0);
}

static UINT16 SoundRead(UINT32 offset)
{
  if (RaineSoundCard) {
    return sport;
  } else
    return 3;           // Z80_OK
}

void SnowBrosPort4w(UINT16 address, UINT8 data)
{
   sport=data;
}

UINT16 SnowBrosPort4r(UINT16 address)
{
   print_debug("Z80 Receives $%02x\n",sport);
   return(sport);
}


void load_snow_bros(void)
{
   RAMSize=0x8000+0x10000;

   if(!(RAM=AllocateMem(RAMSize))) return;

   Z80ROM=RAM+0x8000;

   memcpy(Z80ROM+0x0000, load_region[REGION_ROM2], 0x8000);
   memset(Z80ROM+0x8000, 0x00,                     0x8000);

   // Fix Checksum
   // ------------

   Z80ROM[0x0156]=0x00; // NOP
   Z80ROM[0x0157]=0x00; // NOP
   Z80ROM[0x0158]=0x00; // NOP

   // Apply Speed Patch
   // -----------------

   if(is_current_game("wintbob"))
   {
      Z80ROM[0x0181]=0xD3; // OUTA (AAh)
      Z80ROM[0x0182]=0xAA; //

      SetStopZ80Mode2(0x0180);
   }
   else
   {
      Z80ROM[0x019D]=0xD3; // OUTA (AAh)
      Z80ROM[0x019E]=0xAA; //

      SetStopZ80Mode2(0x019C);
   }

   // Setup Z80 memory map
   // --------------------

   AddZ80AROMBase(Z80ROM, 0x0038, 0x0066);

   AddZ80AReadByte(0x0000, 0x87FF, NULL,                        Z80ROM+0x0000); // Z80 ROM/RAM
   AddZ80AReadByte(0x0000, 0xFFFF, DefBadReadZ80,               NULL);
   AddZ80AReadByte(    -1,     -1, NULL,                        NULL);

   AddZ80AWriteByte(0x8000, 0x87FF, NULL,                       Z80ROM+0x8000); // Z80 RAM
   AddZ80AWriteByte(0x0000, 0xFFFF, DefBadWriteZ80,             NULL);
   AddZ80AWriteByte(    -1,     -1, NULL,                       NULL);

   AddZ80AReadPort(0x02, 0x02, YM3812ReadZ80,           NULL);
   AddZ80AReadPort(0x04, 0x04, SnowBrosPort4r,          NULL);
   AddZ80AReadPort(0x00, 0xFF, DefBadReadZ80,           NULL);
   AddZ80AReadPort(  -1,   -1, NULL,                    NULL);

   AddZ80AWritePort(0x02, 0x03, YM3812WriteZ80,         NULL);
   AddZ80AWritePort(0x04, 0x04, SnowBrosPort4w,         NULL);
   AddZ80AWritePort(0xAA, 0xAA, StopZ80Mode2,           NULL);
   AddZ80AWritePort(0x00, 0xFF, DefBadWriteZ80,         NULL);
   AddZ80AWritePort(  -1,   -1, NULL,                   NULL);

   AddZ80AInit();

   RAM_SPR = RAM+0x004000;
   GFX_SPR_SOLID = NULL;

   memset(RAM+0x0000,0x00,0x6404);
   memset(RAM+0x6404,0xFF,0x001C);

   RAM[0x6400]=0x7F;
   RAM[0x6402]=0x7F;

   RAM[0x6412]=0x00;    // $300000: Sound Related
   RAM[0x6413]=0x03;

   // fix checksum

   WriteLong68k(&ROM[0x008EE],0x4E714E71);
   WriteLong68k(&ROM[0x008FE],0x4E714E71);

   // speed hack

   WriteLong68k(&ROM[0x003BA],0x027C7BFF);
   WriteLong68k(&ROM[0x003BE],0x13FC0000);
   WriteLong68k(&ROM[0x003C2],0x00AA0000);

   WriteLong68k(&ROM[0x00436],0x027C7AFF);
   WriteLong68k(&ROM[0x0043A],0x13FC0000);
   WriteLong68k(&ROM[0x0043E],0x00AA0000);

   WriteLong68k(&ROM[0x0045E],0x027C79FF);
   WriteLong68k(&ROM[0x00462],0x13FC0000);
   WriteLong68k(&ROM[0x00466],0x00AA0000);

   if(is_current_game("wintbob"))
   {
      WriteWord68k(&ROM[0x00DE],0x4EB9);
      WriteLong68k(&ROM[0x00E0],0x0000065A);

      WriteWord68k(&ROM[0x0142],0x4EB9);
      WriteLong68k(&ROM[0x0144],0x0000066C);

      WriteWord68k(&ROM[0x08B8],0x4EB9);
      WriteLong68k(&ROM[0x08BA],0x000009F4);
   }

   InitPaletteMap(RAM+0x06000, 0x10, 0x10, 0x8000);

   set_colour_mapper(&col_map_xbbb_bbgg_gggr_rrrr);

/*
 *  StarScream Main 68000 Setup
 */

   ByteSwap(ROM,get_region_size(REGION_CPU1));

   AddMemFetch(0x000000, 0x03FFFF, ROM+0x000000-0x000000);
   AddMemFetch(-1, -1, NULL);

   AddReadByte(0x000000, 0x03FFFF, NULL, ROM+0x000000);                 // 68000 ROM
   AddReadByte(0x100000, 0x103FFF, NULL, RAM+0x000000);                 // 68000 RAM
   AddReadByte(0x700000, 0x701FFF, NULL, RAM+0x004000);                 // SPRITE RAM
   AddReadByte(0x600000, 0x6003FF, NULL, RAM+0x006000);                 // COLOUR RAM
   AddReadByte(0x500000, 0x50000F, NULL, RAM+0x006400);                 // INPUT
   AddReadByte(0x200000, 0x200001, NULL, RAM+0x006410);                 // ???
   AddReadByte(0x300000, 0x300001, SoundRead, NULL);                    // SOUND COMM
   AddReadByte(0x400000, 0x400001, NULL, RAM+0x006414);                 // ???
   AddReadByte(0x800000, 0x800001, NULL, RAM+0x006416);                 // (Interrupt Repsonse)
   AddReadByte(0x900000, 0x900001, NULL, RAM+0x006418);                 // (Interrupt Repsonse)
   AddReadByte(0xA00000, 0xA00001, NULL, RAM+0x00641A);                 // (Interrupt Repsonse)
   AddReadByte(0x000000, 0xFFFFFF, DefBadReadByte, NULL);      // <Bad Reads>
   AddReadByte(-1, -1, NULL, NULL);

   AddReadWord(0x000000, 0x03FFFF, NULL, ROM+0x000000);                 // 68000 ROM
   AddReadWord(0x100000, 0x103FFF, NULL, RAM+0x000000);                 // 68000 RAM
   AddReadWord(0x700000, 0x701FFF, NULL, RAM+0x004000);
   AddReadWord(0x600000, 0x6003FF, NULL, RAM+0x006000);
   AddReadWord(0x500000, 0x50000F, NULL, RAM+0x006400);
   AddReadWord(0x200000, 0x200001, NULL, RAM+0x006410);
   AddReadWord(0x300000, 0x300001, SoundRead, NULL);
   AddReadWord(0x400000, 0x400001, NULL, RAM+0x006414);
   AddReadWord(0x800000, 0x800001, NULL, RAM+0x006416);
   AddReadWord(0x900000, 0x900001, NULL, RAM+0x006418);
   AddReadWord(0xA00000, 0xA00001, NULL, RAM+0x00641A);
   AddReadWord(0x000000, 0xFFFFFF, DefBadReadWord, NULL);      // <Bad Reads>
   AddReadWord(-1, -1, NULL, NULL);

   AddWriteByte(0x100000, 0x103FFF, NULL, RAM+0x000000);                // 68000 RAM
   AddWriteByte(0x700000, 0x701FFF, NULL, RAM+0x004000);
   AddWriteByte(0x600000, 0x6003FF, NULL, RAM+0x006000);
   AddWriteByte(0x500000, 0x50000F, NULL, RAM+0x006400);
   AddWriteByte(0x200000, 0x200001, NULL, RAM+0x006410);
   AddWriteByte(0x300000, 0x300001, SoundWrite, NULL);
   AddWriteByte(0x400000, 0x400001, NULL, RAM+0x006414);
   AddWriteByte(0x800000, 0x800001, NULL, RAM+0x006416);
   AddWriteByte(0x900000, 0x900001, NULL, RAM+0x006418);
   AddWriteByte(0xA00000, 0xA00001, NULL, RAM+0x00641A);
   AddWriteByte(0xAA0000, 0xAA0001, Stop68000, NULL);                   // Trap Idle 68000
   AddWriteByte(0x000000, 0xFFFFFF, DefBadWriteByte, NULL);      // <Bad Writes>
   AddWriteByte(-1, -1, NULL, NULL);

   AddWriteWord(0x100000, 0x103FFF, NULL, RAM+0x000000);                // 68000 RAM
   AddWriteWord(0x700000, 0x701FFF, NULL, RAM+0x004000);
   AddWriteWord(0x600000, 0x6003FF, NULL, RAM+0x006000);
   AddWriteWord(0x500000, 0x50000F, NULL, RAM+0x006400);
   AddWriteWord(0x200000, 0x200001, NULL, RAM+0x006410);
   AddWriteWord(0x300000, 0x300001, SoundWrite, NULL);
   AddWriteWord(0x400000, 0x400001, NULL, RAM+0x006414);
   AddWriteWord(0x800000, 0x800001, NULL, RAM+0x006416);
   AddWriteWord(0x900000, 0x900001, NULL, RAM+0x006418);
   AddWriteWord(0xA00000, 0xA00001, NULL, RAM+0x00641A);
   AddWriteWord(0x000000, 0xFFFFFF, DefBadWriteWord, NULL);      // <Bad Writes>
   AddWriteWord(-1, -1, NULL, NULL);

   AddInitMemory();     // Set Starscream mem pointers...
}

GAME( snow_bros ,
   snow_bros_dirs,
   snowbros_roms,
   snow_bros_inputs,
   snow_bros_dsw,
   NULL,

   load_snow_bros,
   NULL,
   &snow_bros_video,
   execute_hyper_pacman_frame,
   "snowbros",
   "Snow Bros",
   "\203x\203m\201{\203u\203\211\203U\201[\203Y American",
   COMPANY_ID_TOAPLAN,
   NULL,
   1990,
   snow_bros_sound,
   GAME_PLATFORM
);

GAME( snow_bros_alt ,
   snow_bros_alt_dirs,
   snowbroa_roms,
   snow_bros_inputs,
   snow_bros_dsw,
   NULL,

   load_snow_bros,
   NULL,
   &snow_bros_video,
   execute_hyper_pacman_frame,
   "snowbroa",
   "Snow Bros (alternate)",
   "\203x\203m\201[\203u\203\211\203U\201[\203Y (alternate)",
   COMPANY_ID_TOAPLAN,
   NULL,
   1990,
   snow_bros_sound,
   GAME_PLATFORM
);

GAME( snow_bros_japanese ,
   snow_bros_japanese_dirs,
   snowbroj_roms,
   snow_bros_inputs,
   snow_bros_dsw,
   NULL,

   load_snow_bros,
   NULL,
   &snow_bros_video,
   execute_hyper_pacman_frame,
   "snowbroj",
   "Snow Bros (Japanese)",
   "\203X\203m\201[\203u\203\211\203U\201[\203Y",
   COMPANY_ID_TOAPLAN,
   NULL,
   1990,
   snow_bros_sound,
   GAME_PLATFORM
);

GAME( snow_bros_alt_2 ,
   snow_bros_alt_2_dirs,
   snowbra2_roms,
   snow_bros_inputs,
   snow_bros_dsw,
   NULL,

   load_snow_bros,
   NULL,
   &snow_bros_video,
   execute_hyper_pacman_frame,
   "snowbrob",
   "Snow Bros (alternate 2)",
   "\203X\203m\201[\203u\203\211\203U\201[\203Y (alternate 2)",
   COMPANY_ID_TOAPLAN,
   "MIN16-02",
   1990,
   snow_bros_sound,
   GAME_PLATFORM
);

GAME( winter_bobble ,
   winter_bobble_dirs,
   wintbob_roms,
   snow_bros_inputs,
   snow_bros_dsw,
   NULL,

   load_snow_bros,
   NULL,
   &winter_bobble_video,
   execute_hyper_pacman_frame,
   "wintbob",
   "Winter Bobble",
      "\203X\203m\201[\203u\203\211\203U\201[\203Y\201i\212Cµ¯÷Å\201J",
   COMPANY_ID_BOOTLEG,
   NULL,
   1990,
   snow_bros_sound,
   GAME_PLATFORM
);

/*

 OBJECT RAM
 ----------

- 16 bytes per sprite definition (only odd)
- Sprites are 16x16; 16 colours

-----+--------+-----------------------------
Byte |Bit(s)  | Use
-----+76543210+-----------------------------
1/3/5|........| Unused
  7  |.......x| XPos - Sign Bit
  7  |......x.| YPos - Sign Bit
  7  |.....x..| Use Relative offsets
  7  |xxxx....| Palette Bank
  9  |xxxxxxxx| XPos
  B  |xxxxxxxx| YPos
  D  |xxxxxxxx| Sprite Number (low 8 bits)
  F  |....xxxx| Sprite Number (high 6 bits?)
  F  |.x......| Flip Sprite X-Axis
  F  |x.......| Flip Sprite Y-Axis
-----+--------+-----------------------------

*/
