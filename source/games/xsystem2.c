/******************************************************************************/
/*                                                                            */
/*            TAITO X-SYSTEM/Z80 (C) 1987-1989 TAITO CORPORATION              */
/*                                                                            */
/*                           Z80+Z80+YM2203+SETA-X1                           */
/*                                                                            */
/******************************************************************************/

#include "gameinc.h"
#include "xsystem2.h"
#include "setax1.h"
#include "2203intf.h"
#include "sasound.h"		// sample support routines

/*

Supported romsets:

 0 - B97 - Insector X               - 1989 - 'TNZS'-System - Screen Rotate 180ｰ
 1 - B53 - The NZ Story Extra       - 1988 - 'TNZS'-System - Screen Normal
 2 - B08 - Arkanoid 2 Rev of Doh    - 1987 - 'TNZS'-System - Screen Rotate 270ｰ
 3 - B53 - The New Zealand Story    - 1988 - 'TNZS'-System - Screen Normal
 4 - B08 - Arkanoid 2 American      - 1987 - 'TNZS'-System - Screen Rotate 270ｰ
 5 - B06 - Extermination            - 1987 - 'TNZS'-System - Screen Rotate 270ｰ
 6 - B53 - The NZ Story Bootleg     - 1988 - 'TNZS'-System - Screen Normal
 7 - B35 - Kageki                   - 1988 - 'TNZS'-System - Screen Rotate 90ｰ
 8 - B44 - Chuka Taisen             - 1988 - 'TNZS'-System - Screen Rotate 180ｰ
 9 - B19 - Dr Toppel Tankentai      - 1987 - 'TNZS'-System - Screen Rotate 90ｰ
10 - A98 - Plump Pop                - 1987 - 'TNZS'-System - Screen Normal

*/

static struct DIR_INFO arkanoid_2_american_dirs[] =
{
   { "arkanoid_2_american", },
   { "ark2us", },
   { "arknid2u", },
   { "arkanoi2", },
   { ROMOF("arknoid2"), },
   { CLONEOF("arknoid2"), },
   { NULL, },
};

static struct ROM_INFO arkanoid_2_american_roms[] =
{
   {   "b08-01.13a", 0x00020000, 0x2ccc86b4, 0, 0, 0, },
   {   "b08-02.10a", 0x00020000, 0x056a985f, 0, 0, 0, },
   {   "b08-03.7a", 0x00020000, 0x274a795f, 0, 0, 0, },
   {   "b08-04.4a", 0x00020000, 0x9754f703, 0, 0, 0, },
   {   "b08-07.16f", 0x00000200, 0xea34d9f7, 0, 0, 0, },
   {   "b08-08.15f", 0x00000200, 0xa4f7ebd9, 0, 0, 0, },
   {   "b08_11.11c", 0x00010000, 0x99555231, 0, 0, 0, },
   {   "b08_12.3e", 0x00010000, 0xdc84e27d, 0, 0, 0, },
   {           NULL,          0,          0, 0, 0, 0, },
};

static struct INPUT_INFO arkanoid_2_inputs[] =
{
   { KB_DEF_COIN1,        MSG_COIN1,               0x020004, 0x02, BIT_ACTIVE_0 },
   { KB_DEF_COIN2,        MSG_COIN2,               0x020004, 0x01, BIT_ACTIVE_0 },
   { KB_DEF_TILT,         MSG_TILT,                0x020005, 0x02, BIT_ACTIVE_0 },
   { KB_DEF_SERVICE,      MSG_SERVICE,             0x020005, 0x01, BIT_ACTIVE_0 },

   { KB_DEF_P1_START,     MSG_P1_START,            0x020003, 0x80, BIT_ACTIVE_0 },
   { KB_DEF_P1_UP,        MSG_P1_UP,               0x020006, 0x04, BIT_ACTIVE_1 },
   { KB_DEF_P1_DOWN,      MSG_P1_DOWN,             0x020006, 0x08, BIT_ACTIVE_1 },
   { KB_DEF_P1_LEFT,      MSG_P1_LEFT,             0x020006, 0x01, BIT_ACTIVE_1 },
   { KB_DEF_P1_RIGHT,     MSG_P1_RIGHT,            0x020006, 0x02, BIT_ACTIVE_1 },
   { KB_DEF_P1_B1,        MSG_P1_B1,               0x020003, 0x10, BIT_ACTIVE_0 },
   { KB_DEF_P1_B2,        MSG_P1_B2,               0x020003, 0x20, BIT_ACTIVE_0 },

   { KB_DEF_P2_START,     MSG_P2_START,            0x020003, 0x08, BIT_ACTIVE_0 },
   { KB_DEF_P2_UP,        MSG_P2_UP,               0x020007, 0x04, BIT_ACTIVE_1 },
   { KB_DEF_P2_DOWN,      MSG_P2_DOWN,             0x020007, 0x08, BIT_ACTIVE_1 },
   { KB_DEF_P2_LEFT,      MSG_P2_LEFT,             0x020007, 0x01, BIT_ACTIVE_1 },
   { KB_DEF_P2_RIGHT,     MSG_P2_RIGHT,            0x020007, 0x02, BIT_ACTIVE_1 },
   { KB_DEF_P2_B1,        MSG_P2_B1,               0x020003, 0x01, BIT_ACTIVE_0 },
   { KB_DEF_P2_B2,        MSG_P2_B2,               0x020003, 0x02, BIT_ACTIVE_0 },

   { 0,                   NULL,                    0,        0,    0            },
};

static struct DSW_DATA dsw_data_arkanoid_2_0[] =
{
   { "Cabinet",               0x01, 0x02 },
   { "Table",                 0x01},
   { "Upright",               0x00},
   { MSG_SCREEN,              0x02, 0x02 },
   { MSG_NORMAL,              0x02},
   { MSG_INVERT,              0x00},
   { MSG_TEST_MODE,           0x04, 0x02 },
   { MSG_OFF,                 0x04},
   { MSG_ON,                  0x00},
   { MSG_DEMO_SOUND,          0x08, 0x02 },
   { MSG_ON,                  0x08},
   { MSG_OFF,                 0x00},
   { MSG_COIN1,               0x30, 0x04 },
   { MSG_1COIN_1PLAY,         0x30},
   { MSG_1COIN_2PLAY,         0x20},
   { MSG_2COIN_1PLAY,         0x10},
   { MSG_2COIN_3PLAY,         0x00},
   { MSG_COIN2,               0xC0, 0x04 },
   { MSG_1COIN_1PLAY,         0xC0},
   { MSG_1COIN_2PLAY,         0x80},
   { MSG_2COIN_1PLAY,         0x40},
   { MSG_2COIN_3PLAY,         0x00},
   { NULL,                    0,   },
};

static struct DSW_DATA dsw_data_arkanoid_2_1[] =
{
   { MSG_DIFFICULTY,          0x03, 0x04 },
   { MSG_NORMAL,              0x03},
   { MSG_EASY,                0x02},
   { MSG_HARD,                0x01},
   { MSG_HARDEST,             0x00},
   { "Extra Life",            0x0C, 0x04 },
   { "100k 200k",             0x0C},
   { "100k",                  0x08},
   { "50k",                   0x04},
   { "50k 150k",              0x00},
   { "Lives",                 0x30, 0x04 },
   { "3",                     0x30},
   { "2",                     0x20},
   { "4",                     0x10},
   { "5",                     0x00},
   { "Continue",              0x80, 0x02 },
   { MSG_OFF,                 0x80},
   { MSG_ON,                  0x00},
   { NULL,                    0,   },
};

static struct DSW_INFO arkanoid_2_dsw[] =
{
   { 0x000000, 0xFF, dsw_data_arkanoid_2_0 },
   { 0x000001, 0x7F, dsw_data_arkanoid_2_1 },
   { 0,        0,    NULL,      },
};

/*

yes, we have games using every possible orientation

*/

static struct VIDEO_INFO tnzs_sys_video =
{
   DrawTNZSSystem,
   256,
   224,
   32,
   VIDEO_ROTATE_NORMAL |
   VIDEO_ROTATABLE,
};

static struct VIDEO_INFO tnzs_sys_r90_video =
{
   DrawTNZSSystem,
   256,
   224,
   32,
   VIDEO_ROTATE_90 |
   VIDEO_ROTATABLE,
};

static struct VIDEO_INFO tnzs_sys_r180_video =
{
   DrawTNZSSystem,
   256,
   224,
   32,
   VIDEO_ROTATE_180 |
   VIDEO_ROTATABLE,
};

static struct VIDEO_INFO tnzs_sys_r270_video =
{
   DrawTNZSSystem,
   256,
   224,
   32,
   VIDEO_ROTATE_270 |
   VIDEO_ROTATABLE,
};

#if 0
static UINT16 input_port_0_r(UINT16 offset) {
  return get_dsw(0);
}

static UINT16 input_port_1_r(UINT16 offset) {
  return get_dsw(1);
}
#endif

static struct YM2203interface ym2203_interface =
{
   1,
   3000000,
   { YM2203_VOL(190,65) },
   { 0 }, // input_port_0_r },		/* DSW1 connected to port A */
   { 0 }, // input_port_1_r },		/* DSW2 connected to port B */
   { 0 },
   { 0 },
   { NULL },
};

#if 0
static int kageki_csport_sel = 0;
static UINT16 kageki_csport_r(UINT16 offset)
{
	int	dsw, dsw1, dsw2;

	dsw1 = get_dsw(0); 		/* DSW1 */
	dsw2 = get_dsw(1); 		/* DSW2 */

	switch (kageki_csport_sel)
	  {
	  case	0x00:			/* DSW2 5,1 / DSW1 5,1 */
	    dsw = (((dsw2 & 0x10) >> 1) | ((dsw2 & 0x01) << 2) | ((dsw1 & 0x10) >> 3) | ((dsw1 & 0x01) >> 0));
	    break;
	  case	0x01:			/* DSW2 7,3 / DSW1 7,3 */
	    dsw = (((dsw2 & 0x40) >> 3) | ((dsw2 & 0x04) >> 0) | ((dsw1 & 0x40) >> 5) | ((dsw1 & 0x04) >> 2));
	    break;
	  case	0x02:			/* DSW2 6,2 / DSW1 6,2 */
	    dsw = (((dsw2 & 0x20) >> 2) | ((dsw2 & 0x02) << 1) | ((dsw1 & 0x20) >> 4) | ((dsw1 & 0x02) >> 1));
	    break;
	  case	0x03:			/* DSW2 8,4 / DSW1 8,4 */
	    dsw = (((dsw2 & 0x80) >> 4) | ((dsw2 & 0x08) >> 1) | ((dsw1 & 0x80) >> 6) | ((dsw1 & 0x08) >> 3));
	    break;
	  default:
	    dsw = 0x00;
	    /*	logerror("kageki_csport_sel error !! (0x%08X)\n", kageki_csport_sel); */
	  }
	return (dsw & 0xff);
}
#endif

/* max samples */
#define	MAX_SAMPLES	0x2f

#if 0

static void kageki_csport_w(UINT16 offset,UINT16 data)
{
  char mess[80];
  if (data > 0x3f)
    {
      /* read dipsw port */
      kageki_csport_sel = (data & 0x03);
    } else {
#if 0
      if (data > MAX_SAMPLES)
	{
	  /* stop samples */
	  sample_stop(0);
	  sprintf(mess, "VOICE:%02X STOP", data);
	} else {
	  /* play samples */
	  sample_start(0, data, 0);
	  sprintf(mess, "VOICE:%02X PLAY", data);
	}
      /*	usrintf_showmessage(mess); */
#endif
    }
}

#endif

static struct YM2203interface kageki_ym2203_interface =
{
	1,					/* 1 chip */
	3000000,				/* 12000000/4 ??? */
	{ YM2203_VOL(35, 15) },
	{ 0 }, // kageki_csport_r },
	{ 0 },
	{ 0 },
	{ 0 } // kageki_csport_w },
};

static struct SOUND_INFO tnzs_sys_sound[] =
{
   { SOUND_YM2203,  &ym2203_interface,    },
   { 0,             NULL,                 },
};

static struct SOUND_INFO kageki_sound[] =
{
   { SOUND_YM2203,  &kageki_ym2203_interface,    },
   { 0,             NULL,                 },
};

GAME( arkanoid_2_american ,
   arkanoid_2_american_dirs,
   arkanoid_2_american_roms,
   arkanoid_2_inputs,
   arkanoid_2_dsw,
   NULL,

   LoadArk2DohUS,
   ClearArk2DohUS,
   &tnzs_sys_r270_video,
   ExecuteTNZSSystemFrame,
   "arknid2u",
   "Arkanoid 2 American",
   "アルカノイド　リベンジオブＤＯＨ American",
   COMPANY_ID_TAITO,
   "B08",
   1987,
   tnzs_sys_sound,
   GAME_BREAKOUT
);

static struct DIR_INFO arkanoid_2_rev_of_doh_dirs[] =
{
   { "arkanoid_2_rev_of_doh", },
   { "arkanoi2", },
   { "arknoid2", },
   { NULL, },
};

static struct ROM_INFO arkanoid_2_rev_of_doh_roms[] =
{
   {   "b08-01.13a", 0x00020000, 0x2ccc86b4, 0, 0, 0, },
   {   "b08-02.10a", 0x00020000, 0x056a985f, 0, 0, 0, },
   {   "b08-03.7a", 0x00020000, 0x274a795f, 0, 0, 0, },
   {   "b08-04.4a", 0x00020000, 0x9754f703, 0, 0, 0, },
   {   "b08-07.16f", 0x00000200, 0xea34d9f7, 0, 0, 0, },
   {   "b08-08.15f", 0x00000200, 0xa4f7ebd9, 0, 0, 0, },
   {    "b08_05.11c", 0x00010000, 0x136edf9d, 0, 0, 0, },
   {    "b08_13.3e", 0x00010000, 0xe8035ef1, 0, 0, 0, },
   {           NULL,          0,          0, 0, 0, 0, },
};

GAME( arkanoid_2_rev_of_doh ,
   arkanoid_2_rev_of_doh_dirs,
   arkanoid_2_rev_of_doh_roms,
   arkanoid_2_inputs,
   arkanoid_2_dsw,
   NULL,

   LoadArk2Doh,
   ClearArk2Doh,
   &tnzs_sys_r270_video,
   ExecuteTNZSSystemFrame,
   "arknoid2",
   "Arkanoid 2 Revenge of Doh",
   "アルカノイド　リベンジオブＤＯＨ",
   COMPANY_ID_TAITO,
   "B08",
   1987,
   tnzs_sys_sound,
   GAME_BREAKOUT
);

static struct DIR_INFO arkanoid_2_japanese_dirs[] =
{
   { "arkanoid_2_japanese", },
   { "ark2jp", },
   { "arknid2j", },
   { "arkanoi2", },
   { ROMOF("arknoid2"), },
   { CLONEOF("arknoid2"), },
   { NULL, },
};

static struct ROM_INFO arkanoid_2_japanese_roms[] =
{
   {   "b08-01.13a", 0x00020000, 0x2ccc86b4, 0, 0, 0, },
   {   "b08-02.10a", 0x00020000, 0x056a985f, 0, 0, 0, },
   {   "b08-03.7a", 0x00020000, 0x274a795f, 0, 0, 0, },
   {   "b08-04.4a", 0x00020000, 0x9754f703, 0, 0, 0, },
   {   "b08-07.16f", 0x00000200, 0xea34d9f7, 0, 0, 0, },
   {   "b08-08.15f", 0x00000200, 0xa4f7ebd9, 0, 0, 0, },
   {    "b08_05.11c", 0x00010000, 0x136edf9d, 0, 0, 0, },
   {       "b08_06.3e", 0x00010000, 0xadfcd40c, 0, 0, 0, },
   {           NULL,          0,          0, 0, 0, 0, },
};

GAME( arkanoid_2_japanese ,
   arkanoid_2_japanese_dirs,
   arkanoid_2_japanese_roms,
   arkanoid_2_inputs,
   arkanoid_2_dsw,
   NULL,

   LoadArk2DohJP,
   ClearArk2Doh,
   &tnzs_sys_r270_video,
   ExecuteTNZSSystemFrame,
   "arknid2j",
   "Arkanoid 2 Japanese",
   "アルカノイド　リベンジオブＤＯＨ",
   COMPANY_ID_TAITO,
   "B08",
   1987,
   tnzs_sys_sound,
   GAME_BREAKOUT
);

static struct DIR_INFO insector_x_dirs[] =
{
   { "insector_x", },
   { "insectrx", },
   { "insectx", },
   { NULL, },
};

static struct ROM_INFO insector_x_roms[] =
{
   {   "insector.r16", 0x00080000, 0xdb5a7434, 0, 0, 0, },
   {   "insector.r15", 0x00080000, 0xd00294b1, 0, 0, 0, },
   {   "insector.u32", 0x00020000, 0x18eef387, 0, 0, 0, },
   {   "insector.u38", 0x00010000, 0x324b28c9, 0, 0, 0, },
   {           NULL,          0,          0, 0, 0, 0, },
};

static struct INPUT_INFO insector_x_inputs[] =
{
   { KB_DEF_COIN1,        MSG_COIN1,               0x020004, 0x08, BIT_ACTIVE_0 },
   { KB_DEF_COIN2,        MSG_COIN2,               0x020004, 0x04, BIT_ACTIVE_0 },
   { KB_DEF_TILT,         MSG_TILT,                0x020004, 0x02, BIT_ACTIVE_0 },
   { KB_DEF_SERVICE,      MSG_SERVICE,             0x020004, 0x01, BIT_ACTIVE_0 },

   { KB_DEF_P1_START,     MSG_P1_START,            0x020002, 0x80, BIT_ACTIVE_0 },
   { KB_DEF_P1_UP,        MSG_P1_UP,               0x020002, 0x04, BIT_ACTIVE_0 },
   { KB_DEF_P1_DOWN,      MSG_P1_DOWN,             0x020002, 0x08, BIT_ACTIVE_0 },
   { KB_DEF_P1_LEFT,      MSG_P1_LEFT,             0x020002, 0x01, BIT_ACTIVE_0 },
   { KB_DEF_P1_RIGHT,     MSG_P1_RIGHT,            0x020002, 0x02, BIT_ACTIVE_0 },
   { KB_DEF_P1_B1,        MSG_P1_B1,               0x020002, 0x10, BIT_ACTIVE_0 },
   { KB_DEF_P1_B2,        MSG_P1_B2,               0x020002, 0x20, BIT_ACTIVE_0 },

   { KB_DEF_P2_START,     MSG_P2_START,            0x020003, 0x80, BIT_ACTIVE_0 },
   { KB_DEF_P2_UP,        MSG_P2_UP,               0x020003, 0x04, BIT_ACTIVE_0 },
   { KB_DEF_P2_DOWN,      MSG_P2_DOWN,             0x020003, 0x08, BIT_ACTIVE_0 },
   { KB_DEF_P2_LEFT,      MSG_P2_LEFT,             0x020003, 0x01, BIT_ACTIVE_0 },
   { KB_DEF_P2_RIGHT,     MSG_P2_RIGHT,            0x020003, 0x02, BIT_ACTIVE_0 },
   { KB_DEF_P2_B1,        MSG_P2_B1,               0x020003, 0x10, BIT_ACTIVE_0 },
   { KB_DEF_P2_B2,        MSG_P2_B2,               0x020003, 0x20, BIT_ACTIVE_0 },

   { 0,                   NULL,                    0,        0,    0            },
};

static struct DSW_DATA dsw_data_insector_x_0[] =
{
   { MSG_DSWA_BIT1,           0x01, 0x02 },
   { MSG_OFF,                 0x01},
   { MSG_ON,                  0x00},
   { MSG_SCREEN,              0x02, 0x02 },	// because the monitor is
   { MSG_NORMAL,              0x00},	// r180, invert is default?
   { MSG_INVERT,              0x02},
   { MSG_TEST_MODE,           0x04, 0x02 },
   { MSG_OFF,                 0x04},
   { MSG_ON,                  0x00},
   { MSG_DEMO_SOUND,          0x08, 0x02 },
   { MSG_ON,                  0x08},
   { MSG_OFF,                 0x00},
   { MSG_DSWA_BIT5,           0x10, 0x02 },
   { MSG_OFF,                 0x10},
   { MSG_ON,                  0x00},
   { MSG_DSWA_BIT6,           0x20, 0x02 },
   { MSG_OFF,                 0x20},
   { MSG_ON,                  0x00},
   { MSG_DSWA_BIT7,           0x40, 0x02 },
   { MSG_OFF,                 0x40},
   { MSG_ON,                  0x00},
   { MSG_DSWA_BIT8,           0x80, 0x02 },
   { MSG_OFF,                 0x80},
   { MSG_ON,                  0x00},
   { NULL,                    0,   },
};

static struct DSW_INFO insector_x_dsw[] =
{
   { 0x000000, 0xFD, dsw_data_insector_x_0 },
   { 0x000001, 0xFF, dsw_data_default_1 },
   { 0,        0,    NULL,      },
};

static struct ROMSW_DATA romsw_data_the_nz_story_0[] =
{
   { "Taito Japan (Notice)",  0x01 },
   { "Taito America",         0x02 },
   { "Taito Japan",           0x03 },
   { NULL,                    0    },
};

static struct ROMSW_INFO the_nz_story_romsw[] =
{
   { 0x2007FFF, 0x03, romsw_data_the_nz_story_0 },
   { 0,        0,    NULL },
};

GAME( insector_x ,
   insector_x_dirs,
   insector_x_roms,
   insector_x_inputs,
   insector_x_dsw,
   NULL,

   LoadInsectorX,
   ClearInsectorX,
   &tnzs_sys_r180_video,
   ExecuteInsectorXFrame,
   "insectx",
   "Insector X",
   "インセクターＸ",
   COMPANY_ID_TAITO,
   "B97",
   1989,
   tnzs_sys_sound,
   GAME_SHOOT
);

static struct DIR_INFO the_nz_story_dirs[] =
{
   { "the_nz_story", },
   { "tnzs", },
   { "tnzsj", },
   { "tnzso", },
   { NULL, },
};

static struct ROM_INFO the_nz_story_roms[] =
{
  { "b53-10.32", 0x20000, 0xa73745c6, REGION_ROM1, 0, LOAD_NORMAL },
  { "b53-11.38", 0x10000, 0x9784d443, REGION_ROM2, 0, LOAD_NORMAL },
   { "b53-08.8", 0x00020000, 0xc3519c2a, 0, 0, 0, },
   { "b53-07.7", 0x00020000, 0x2bf199e8, 0, 0, 0, },
   { "b53-06.6", 0x00020000, 0x92f35ed9, 0, 0, 0, },
   { "b53-05.5", 0x00020000, 0xedbb9581, 0, 0, 0, },
   { "b53-02.2", 0x00020000, 0x095d0dc0, 0, 0, 0, },
   { "b53-01.1", 0x00020000, 0x9800c54d, 0, 0, 0, },
   { "b53-04.4", 0x00020000, 0x59d2aef6, 0, 0, 0, },
   { "b53-03.3", 0x00020000, 0x74acfb9b, 0, 0, 0, },
   {           NULL,          0,          0, 0, 0, 0, },
};

static struct INPUT_INFO the_nz_story_inputs[] =
{
   { KB_DEF_COIN1,        MSG_COIN1,               0x020004, 0x01, BIT_ACTIVE_0 },
   { KB_DEF_COIN2,        MSG_COIN2,               0x020004, 0x02, BIT_ACTIVE_0 },
   { KB_DEF_TILT,         MSG_TILT,                0x020005, 0x02, BIT_ACTIVE_0 },
   { KB_DEF_SERVICE,      MSG_SERVICE,             0x020005, 0x01, BIT_ACTIVE_0 },

   { KB_DEF_P1_START,     MSG_P1_START,            0x020003, 0x80, BIT_ACTIVE_1 },
   { KB_DEF_P1_UP,        MSG_P1_UP,               0x020006, 0x04, BIT_ACTIVE_1 },
   { KB_DEF_P1_DOWN,      MSG_P1_DOWN,             0x020006, 0x08, BIT_ACTIVE_1 },
   { KB_DEF_P1_LEFT,      MSG_P1_LEFT,             0x020006, 0x01, BIT_ACTIVE_1 },
   { KB_DEF_P1_RIGHT,     MSG_P1_RIGHT,            0x020006, 0x02, BIT_ACTIVE_1 },
   { KB_DEF_P1_B1,        MSG_P1_B1,               0x020003, 0x10, BIT_ACTIVE_1 },
   { KB_DEF_P1_B2,        MSG_P1_B2,               0x020003, 0x20, BIT_ACTIVE_1 },

   { KB_DEF_P2_START,     MSG_P2_START,            0x020008, 0x80, BIT_ACTIVE_1 },
   { KB_DEF_P2_UP,        MSG_P2_UP,               0x020007, 0x04, BIT_ACTIVE_1 },
   { KB_DEF_P2_DOWN,      MSG_P2_DOWN,             0x020007, 0x08, BIT_ACTIVE_1 },
   { KB_DEF_P2_LEFT,      MSG_P2_LEFT,             0x020007, 0x01, BIT_ACTIVE_1 },
   { KB_DEF_P2_RIGHT,     MSG_P2_RIGHT,            0x020007, 0x02, BIT_ACTIVE_1 },
   { KB_DEF_P2_B1,        MSG_P2_B1,               0x020008, 0x10, BIT_ACTIVE_1 },
   { KB_DEF_P2_B2,        MSG_P2_B2,               0x020008, 0x20, BIT_ACTIVE_1 },

   { 0,                   NULL,                    0,        0,    0            },
};

static struct DSW_DATA dsw_data_the_nz_story_0[] =
{
   { MSG_DSWA_BIT1,           0x01, 0x02 },
   { MSG_OFF,                 0x01},
   { MSG_ON,                  0x00},
   { MSG_SCREEN,              0x02, 0x02 },
   { MSG_NORMAL,              0x02},
   { MSG_INVERT,              0x00},
   { MSG_TEST_MODE,           0x04, 0x02 },
   { MSG_OFF,                 0x04},
   { MSG_ON,                  0x00},
   { MSG_DEMO_SOUND,          0x08, 0x02 },
   { MSG_ON,                  0x08},
   { MSG_OFF,                 0x00},
   { MSG_COIN1,               0x30, 0x04 },
   { MSG_1COIN_1PLAY,         0x30},
   { MSG_2COIN_1PLAY,         0x20},
   { MSG_3COIN_1PLAY,         0x10},
   { MSG_4COIN_1PLAY,         0x00},
   { MSG_COIN2,               0xC0, 0x04 },
   { MSG_1COIN_2PLAY,         0xC0},
   { MSG_1COIN_3PLAY,         0x80},
   { MSG_1COIN_4PLAY,         0x40},
   { MSG_1COIN_6PLAY,         0x00},
   { NULL,                    0,   },
};

static struct DSW_DATA dsw_data_the_nz_story_1[] =
{
   { MSG_DIFFICULTY,          0x03, 0x04 },
   { MSG_NORMAL,              0x03},
   { MSG_EASY,                0x02},
   { MSG_HARD,                0x01},
   { MSG_HARDEST,             0x00},
   { "Extra Life",            0x0C, 0x04 },
   { "70k, 200k",             0x0C, 0x00 },
   { "50k, 150k",             0x08, 0x00 },
   { "100k, 250k",            0x04, 0x00 },
   { "200k, 300k", 0x00 },
   { "Lives",                 0x30, 0x04 },
   { "3",                     0x30},
   { "2",                     0x20},
   { "5",                     0x10},
   { "4",                     0x00},
   { "Continue Play",         0x40, 0x02 },
   { MSG_ON,                  0x40},
   { MSG_OFF,                 0x00},
   { MSG_DSWB_BIT8,           0x80, 0x02 },
   { MSG_OFF,                 0x80},
   { MSG_ON,                  0x00},
   { NULL,                    0,   },
};

static struct DSW_INFO the_nz_story_dsw[] =
{
   { 0x000000, 0xFF, dsw_data_the_nz_story_0 },
   { 0x000001, 0xFF, dsw_data_the_nz_story_1 },
   { 0,        0,    NULL,      },
};

GAME( the_nz_story ,
   the_nz_story_dirs,
   the_nz_story_roms,
   the_nz_story_inputs,
   the_nz_story_dsw,
   the_nz_story_romsw,

   LoadTNZS,
   ClearTNZS,
   &tnzs_sys_video,
   ExecuteTNZSSystemFrame,
   "tnzs",
   "The New Zealand Story",
   "ニュージーランドストーリー",
   COMPANY_ID_TAITO,
   "B53",
   1988,
   tnzs_sys_sound,
   GAME_PLATFORM
);

static struct DIR_INFO the_nz_story_bootleg_dirs[] =
{
   { "the_nz_story_bootleg", },
   { "tnzsb", },
   { ROMOF("tnzs"), },
   { CLONEOF("tnzs"), },
   { NULL, },
};

static struct ROM_INFO the_nz_story_bootleg_roms[] =
{
  // Notice : while this code isn't updated to use the new bankswitch it can't
  // use automatic rom loading...
  { "nzsb5324.bin", 0x20000, 0xd66824c6, 0, 0, 0}, // REGION_ROM1, 0, LOAD_NORMAL },
  { "nzsb5325.bin", 0x10000, 0xd6ac4e71, 0, 0, 0}, // REGION_ROM2, 0, LOAD_NORMAL },
   { "nzsb5326.bin", 0x00010000, 0xcfd5649c, 0, 0, 0, },
   { "b53-08.8", 0x00020000, 0xc3519c2a, 0, 0, 0, },
   { "b53-07.7", 0x00020000, 0x2bf199e8, 0, 0, 0, },
   { "b53-06.6", 0x00020000, 0x92f35ed9, 0, 0, 0, },
   { "b53-05.5", 0x00020000, 0xedbb9581, 0, 0, 0, },
   { "b53-02.2", 0x00020000, 0x095d0dc0, 0, 0, 0, },
   { "b53-01.1", 0x00020000, 0x9800c54d, 0, 0, 0, },
   { "b53-04.4", 0x00020000, 0x59d2aef6, 0, 0, 0, },
   { "b53-03.3", 0x00020000, 0x74acfb9b, 0, 0, 0, },
   {           NULL,          0,          0, 0, 0, 0, },
};

static struct INPUT_INFO the_nz_story_bootleg_inputs[] =
{
   { KB_DEF_COIN1,        MSG_COIN1,               0x020004, 0x10, BIT_ACTIVE_0 },
   { KB_DEF_COIN2,        MSG_COIN2,               0x020004, 0x20, BIT_ACTIVE_0 },
   { KB_DEF_TILT,         MSG_TILT,                0x020004, 0x02, BIT_ACTIVE_0 },
   { KB_DEF_SERVICE,      MSG_SERVICE,             0x020004, 0x01, BIT_ACTIVE_0 },

   { KB_DEF_P1_START,     MSG_P1_START,            0x020002, 0x80, BIT_ACTIVE_0 },
   { KB_DEF_P1_UP,        MSG_P1_UP,               0x020002, 0x04, BIT_ACTIVE_0 },
   { KB_DEF_P1_DOWN,      MSG_P1_DOWN,             0x020002, 0x08, BIT_ACTIVE_0 },
   { KB_DEF_P1_LEFT,      MSG_P1_LEFT,             0x020002, 0x01, BIT_ACTIVE_0 },
   { KB_DEF_P1_RIGHT,     MSG_P1_RIGHT,            0x020002, 0x02, BIT_ACTIVE_0 },
   { KB_DEF_P1_B1,        MSG_P1_B1,               0x020002, 0x10, BIT_ACTIVE_0 },
   { KB_DEF_P1_B2,        MSG_P1_B2,               0x020002, 0x20, BIT_ACTIVE_0 },

   { KB_DEF_P2_START,     MSG_P2_START,            0x020003, 0x80, BIT_ACTIVE_0 },
   { KB_DEF_P2_UP,        MSG_P2_UP,               0x020003, 0x04, BIT_ACTIVE_0 },
   { KB_DEF_P2_DOWN,      MSG_P2_DOWN,             0x020003, 0x08, BIT_ACTIVE_0 },
   { KB_DEF_P2_LEFT,      MSG_P2_LEFT,             0x020003, 0x01, BIT_ACTIVE_0 },
   { KB_DEF_P2_RIGHT,     MSG_P2_RIGHT,            0x020003, 0x02, BIT_ACTIVE_0 },
   { KB_DEF_P2_B1,        MSG_P2_B1,               0x020003, 0x10, BIT_ACTIVE_0 },
   { KB_DEF_P2_B2,        MSG_P2_B2,               0x020003, 0x20, BIT_ACTIVE_0 },

   { 0,                   NULL,                    0,        0,    0            },
};

static struct YM2203interface ym2203_bootleg_interface =
{
   1,
   3000000,
   { YM2203_VOL(255,220) },
   {0},
   {0},
   {0},
   {0},
   {NULL}
};

static struct SOUND_INFO tnzs_bl_sound[] =
{
   { SOUND_YM2203,  &ym2203_bootleg_interface, },
   { 0,             NULL,                      },
};

GAME( the_nz_story_bootleg ,
   the_nz_story_bootleg_dirs,
   the_nz_story_bootleg_roms,
   the_nz_story_bootleg_inputs,
   the_nz_story_dsw,
   the_nz_story_romsw,

   LoadTNZSBL,
   ClearTNZSBL,
   &tnzs_sys_video,
   ExecuteTNZSSystemFrame,
   "tnzsb",
   "The New Zealand Story (bootleg)",
   "ニュージーランドストーリー (bootleg)",
   COMPANY_ID_BOOTLEG,
   NULL,
   1988,
   tnzs_bl_sound,
   GAME_PLATFORM
);

static struct DIR_INFO the_nz_story_extra_dirs[] =
{
   { "the_nz_story_extra", },
   { "tnzs2", },
   { ROMOF("tnzs"), },
   { CLONEOF("tnzs"), },
   { NULL, },
};

static struct ROM_INFO the_nz_story_extra_roms[] =
{
   {   "ns_a13.rom", 0x00020000, 0x7e0bd5bb, 0, 0, 0, },
   {   "ns_e-3.rom", 0x00010000, 0xc7662e96, 0, 0, 0, },
   {  "ns_c-11.rom", 0x00020000, 0x3c1dae7b, 0, 0, 0, },
   {   "ns_a12.rom", 0x00020000, 0x95880726, 0, 0, 0, },
   {   "ns_a10.rom", 0x00020000, 0x2bc4c053, 0, 0, 0, },
   {   "ns_a08.rom", 0x00020000, 0x8ff8d88c, 0, 0, 0, },
   {   "ns_a07.rom", 0x00020000, 0x291bcaca, 0, 0, 0, },
   {   "ns_a05.rom", 0x00020000, 0x6e762e20, 0, 0, 0, },
   {   "ns_a04.rom", 0x00020000, 0xe1fd1b9d, 0, 0, 0, },
   {   "ns_a02.rom", 0x00020000, 0x2ab06bda, 0, 0, 0, },
   {           NULL,          0,          0, 0, 0, 0, },
};

GAME( the_nz_story_extra ,
   the_nz_story_extra_dirs,
   the_nz_story_extra_roms,
   the_nz_story_inputs,
   the_nz_story_dsw,
   the_nz_story_romsw,

   LoadTNZSExtra,
   ClearTNZSExtra,
   &tnzs_sys_video,
   ExecuteTNZSSystemFrame,
   "tnzs2",
   "The New Zealand Story (extra)",
   "ニュージーランドストーリーエキストラ",
   COMPANY_ID_TAITO,
   "B53",
   1988,
   tnzs_sys_sound,
   GAME_PLATFORM
);

static struct DIR_INFO extermination_dirs[] =
{
   { "extermination", },
   { "extrmatn", },
   { NULL, },
};

static struct ROM_INFO extermination_roms[] =
{
   {   "b06-01.bin", 0x00020000, 0xd2afbf7e, 0, 0, 0, },
   {   "b06-02.bin", 0x00020000, 0xe0c2757a, 0, 0, 0, },
   {   "b06-03.bin", 0x00020000, 0xee80ab9d, 0, 0, 0, },
   {   "b06-04.bin", 0x00020000, 0x3697ace4, 0, 0, 0, },
   {   "b06-06.bin", 0x00010000, 0x744f2c84, 0, 0, 0, },
   {   "b06-08.bin", 0x00000200, 0x10c9aac3, 0, 0, 0, },
   {   "b06-09.bin", 0x00000200, 0xf388b361, 0, 0, 0, },
   {   "b06-20.bin", 0x00010000, 0x04e3fc1f, 0, 0, 0, },
   {   "b06-21.bin", 0x00010000, 0x1614d6a2, 0, 0, 0, },
   {           NULL,          0,          0, 0, 0, 0, },
};

static struct DSW_DATA dsw_data_extermination_0[] =
{
   { MSG_DSWA_BIT1,           0x01, 0x02 },
   { MSG_OFF,                 0x01},
   { MSG_ON,                  0x00},
   { MSG_SCREEN,              0x02, 0x02 },
   { MSG_NORMAL,              0x02},
   { MSG_INVERT,              0x00},
   { MSG_TEST_MODE,           0x04, 0x02 },
   { MSG_OFF,                 0x04},
   { MSG_ON,                  0x00},
   { MSG_DEMO_SOUND,          0x08, 0x02 },
   { MSG_ON,                  0x08},
   { MSG_OFF,                 0x00},
   { MSG_COIN1,               0x30, 0x04 },
   { MSG_1COIN_1PLAY,         0x30},
   { MSG_1COIN_2PLAY,         0x20},
   { MSG_2COIN_1PLAY,         0x10},
   { MSG_2COIN_3PLAY,         0x00},
   { MSG_COIN2,               0xC0, 0x04 },
   { MSG_1COIN_1PLAY,         0xC0},
   { MSG_1COIN_2PLAY,         0x80},
   { MSG_2COIN_1PLAY,         0x40},
   { MSG_2COIN_3PLAY,         0x00},
   { NULL,                    0,   },
};

static struct DSW_DATA dsw_data_extermination_1[] =
{
   { MSG_DSWB_BIT1,           0x01, 0x02 },
   { MSG_OFF,                 0x01},
   { MSG_ON,                  0x00},
   { MSG_DSWB_BIT2,           0x02, 0x02 },
   { MSG_OFF,                 0x02},
   { MSG_ON,                  0x00},
   { MSG_DSWB_BIT3,           0x04, 0x02 },
   { MSG_OFF,                 0x04},
   { MSG_ON,                  0x00},
   { MSG_DSWB_BIT4,           0x08, 0x02 },
   { MSG_OFF,                 0x08},
   { MSG_ON,                  0x00},
   { MSG_DSWB_BIT5,           0x10, 0x02 },
   { MSG_OFF,                 0x10},
   { MSG_ON,                  0x00},
   { MSG_DSWB_BIT6,           0x20, 0x02 },
   { MSG_OFF,                 0x20},
   { MSG_ON,                  0x00},
   { MSG_DIFFICULTY,          0xC0, 0x04 },	// It relates to how much health you lose
   { MSG_NORMAL,              0x80},	// when hit by monsters. It might not be
   { MSG_EASY,                0xC0},	// the actual game difficulty setting.
   { MSG_HARD,                0x40},
   { MSG_HARDEST,             0x00},
   { NULL,                    0,   },
};

static struct DSW_INFO extermination_dsw[] =
{
   { 0x000000, 0xFF, dsw_data_extermination_0 },
   { 0x000001, 0xFF, dsw_data_extermination_1 },
   { 0,        0,    NULL,      },
};

static struct ROMSW_DATA romsw_data_extermination_0[] =
{
   { "Taito Corporation for USA", 0x12 },
   { "World Games License",   0x16 },
   { "Taito Japan for USA",   0x1A },
   { NULL,                    0    },
};

static struct ROMSW_INFO extermination_romsw[] =
{
   { 0x20001E0, 0x16, romsw_data_extermination_0 },
   { 0,        0,    NULL },
};

GAME( extermination ,
   extermination_dirs,
   extermination_roms,
   the_nz_story_inputs,
   extermination_dsw,
   extermination_romsw,

   LoadExtermination,
   ClearExtermination,
   &tnzs_sys_r270_video,
   ExecuteTNZSSystemFrame,
   "extrmatn",
   "Extermination",
   "エクスターミネーション",
   COMPANY_ID_TAITO,
   "B06",
   1987,
   tnzs_sys_sound,
   GAME_SHOOT
);

static struct DIR_INFO kageki_dirs[] =
{
   { "kageki", },
   { NULL, },
};

static struct ROM_INFO kageki_roms[] =
{
   {   "b35-01.13a", 0x00020000, 0x01d83a69, 0, 0, 0, },
   {   "b35-02.12a", 0x00020000, 0xd8af47ac, 0, 0, 0, },
   {   "b35-03.10a", 0x00020000, 0x3cb68797, 0, 0, 0, },
   {    "b35-04.8a", 0x00020000, 0x71c03f91, 0, 0, 0, },
   {    "b35-05.7a", 0x00020000, 0xa4e20c08, 0, 0, 0, },
   {    "b35-06.5a", 0x00020000, 0x3f8ab658, 0, 0, 0, },
   {    "b35-07.4a", 0x00020000, 0x1b4af049, 0, 0, 0, },
   {    "b35-08.2a", 0x00020000, 0xdeb2268c, 0, 0, 0, },
   {    "b35-10.9c", 0x00010000, 0xb150457d, 0, 0, 0, },
   {   "b35-15.98g", 0x00010000, 0xe6212a0f, 0, 0, 0, },
   {   "b35-16.11c", 0x00010000, 0xa4e6fd58, 0, 0, 0, },
   {   "b35-17.43e", 0x00010000, 0xfdd9c246, 0, 0, 0, },
   {           NULL,          0,          0, 0, 0, 0, },
};

static struct INPUT_INFO kageki_inputs[] =
{
   { KB_DEF_COIN1,        MSG_COIN1,               0x020004, 0x10, BIT_ACTIVE_0 },
   { KB_DEF_COIN2,        MSG_COIN2,               0x020004, 0x20, BIT_ACTIVE_0 },
   { KB_DEF_TILT,         MSG_TILT,                0x020004, 0x02, BIT_ACTIVE_0 },
   { KB_DEF_SERVICE,      MSG_SERVICE,             0x020004, 0x01, BIT_ACTIVE_0 },

   { KB_DEF_P1_START,     MSG_P1_START,            0x020002, 0x80, BIT_ACTIVE_0 },
   { KB_DEF_P1_UP,        MSG_P1_UP,               0x020002, 0x04, BIT_ACTIVE_0 },
   { KB_DEF_P1_DOWN,      MSG_P1_DOWN,             0x020002, 0x08, BIT_ACTIVE_0 },
   { KB_DEF_P1_LEFT,      MSG_P1_LEFT,             0x020002, 0x01, BIT_ACTIVE_0 },
   { KB_DEF_P1_RIGHT,     MSG_P1_RIGHT,            0x020002, 0x02, BIT_ACTIVE_0 },
   { KB_DEF_P1_B1,        MSG_P1_B1,               0x020002, 0x10, BIT_ACTIVE_0 },
   { KB_DEF_P1_B2,        MSG_P1_B2,               0x020002, 0x20, BIT_ACTIVE_0 },

   { KB_DEF_P2_START,     MSG_P2_START,            0x020003, 0x80, BIT_ACTIVE_0 },
   { KB_DEF_P2_UP,        MSG_P2_UP,               0x020003, 0x04, BIT_ACTIVE_0 },
   { KB_DEF_P2_DOWN,      MSG_P2_DOWN,             0x020003, 0x08, BIT_ACTIVE_0 },
   { KB_DEF_P2_LEFT,      MSG_P2_LEFT,             0x020003, 0x01, BIT_ACTIVE_0 },
   { KB_DEF_P2_RIGHT,     MSG_P2_RIGHT,            0x020003, 0x02, BIT_ACTIVE_0 },
   { KB_DEF_P2_B1,        MSG_P2_B1,               0x020003, 0x10, BIT_ACTIVE_0 },
   { KB_DEF_P2_B2,        MSG_P2_B2,               0x020003, 0x20, BIT_ACTIVE_0 },

   { 0,                   NULL,                    0,        0,    0            },
};

static struct DSW_DATA dsw_data_kageki_0[] =
{
   { MSG_DSWA_BIT1,           0x01, 0x02 },
   { MSG_OFF,                 0x01},
   { MSG_ON,                  0x00},
   { MSG_SCREEN,              0x02, 0x02 },
   { MSG_NORMAL,              0x02},
   { MSG_INVERT,              0x00},
   { MSG_TEST_MODE,           0x04, 0x02 },
   { MSG_OFF,                 0x04},
   { MSG_ON,                  0x00},
   { MSG_DEMO_SOUND,          0x08, 0x02 },
   { MSG_ON,                  0x08},
   { MSG_OFF,                 0x00},
   { MSG_COIN1,               0x30, 0x04 },
   { MSG_1COIN_1PLAY,         0x30},
   { MSG_1COIN_2PLAY,         0x20},
   { MSG_2COIN_1PLAY,         0x10},
   { MSG_2COIN_3PLAY,         0x00},
   { MSG_COIN2,               0xC0, 0x04 },
   { MSG_1COIN_1PLAY,         0xC0},
   { MSG_1COIN_2PLAY,         0x80},
   { MSG_2COIN_1PLAY,         0x40},
   { MSG_2COIN_3PLAY,         0x00},
   { NULL,                    0,   },
};

static struct DSW_INFO kageki_dsw[] =
{
   { 0x000000, 0xFF, dsw_data_kageki_0 },
   { 0x000001, 0xFF, dsw_data_default_1 },
   { 0,        0,    NULL,      },
};


GAME( kageki ,
   kageki_dirs,
   kageki_roms,
   kageki_inputs,
   kageki_dsw,
   the_nz_story_romsw,	//kageki_romsw,

   load_kageki,
   clear_kageki,
   &tnzs_sys_r90_video,
   ExecuteTNZSSystemFrame,
   "kageki",
   "Kageki",
   "火激",
   COMPANY_ID_TAITO,
   "B35",
   1988,
   kageki_sound,
   GAME_BEAT
);

static struct DIR_INFO chuka_taisen_dirs[] =
{
   { "chuka_taisen", },
   { "chukatai", },
   { "chukatsn", },
   { NULL, },
};

static struct ROM_INFO chuka_taisen_roms[] =
{
   {   "b44-01.a13", 0x00020000, 0xaae7b3d5, 0, 0, 0, },
   {   "b44-02.a12", 0x00020000, 0x7f0b9568, 0, 0, 0, },
   {   "b44-03.a10", 0x00020000, 0x5a54a3b9, 0, 0, 0, },
   {   "b44-04.a08", 0x00020000, 0x3c5f544b, 0, 0, 0, },
   {   "b44-05.a07", 0x00020000, 0xd1b7e314, 0, 0, 0, },
   {   "b44-06.a05", 0x00020000, 0x269978a8, 0, 0, 0, },
   {   "b44-07.a04", 0x00020000, 0x3e0e737e, 0, 0, 0, },
   {   "b44-08.a02", 0x00020000, 0x6cb1e8fc, 0, 0, 0, },
   {       "b44.10", 0x00010000, 0x8c69e008, 0, 0, 0, },
   {       "b44.11", 0x00010000, 0x32484094, 0, 0, 0, },
   {       "b44.12w", 0x00010000,0xe80ecdca, 0, 0, 0, },
   //   {       "b44.12", 0x00010000, 0x0600ace6, 0, 0, 0, },
   {           NULL,          0,          0, 0, 0, 0, },
};

static struct INPUT_INFO chuka_taisen_inputs[] =
{
   { KB_DEF_COIN1,        MSG_COIN1,               0x020004, 0x01, BIT_ACTIVE_1 },
   { KB_DEF_COIN2,        MSG_COIN2,               0x020004, 0x02, BIT_ACTIVE_1 },
   { KB_DEF_TILT,         MSG_TILT,                0x020004, 0x04, BIT_ACTIVE_0 },
   { KB_DEF_SERVICE,      MSG_SERVICE,             0x020004, 0x08, BIT_ACTIVE_0 },

   { KB_DEF_P1_START,     MSG_P1_START,            0x020002, 0x80, BIT_ACTIVE_0 },
   { KB_DEF_P1_UP,        MSG_P1_UP,               0x020002, 0x04, BIT_ACTIVE_0 },
   { KB_DEF_P1_DOWN,      MSG_P1_DOWN,             0x020002, 0x08, BIT_ACTIVE_0 },
   { KB_DEF_P1_LEFT,      MSG_P1_LEFT,             0x020002, 0x01, BIT_ACTIVE_0 },
   { KB_DEF_P1_RIGHT,     MSG_P1_RIGHT,            0x020002, 0x02, BIT_ACTIVE_0 },
   { KB_DEF_P1_B1,        MSG_P1_B1,               0x020002, 0x10, BIT_ACTIVE_0 },
   { KB_DEF_P1_B2,        MSG_P1_B2,               0x020002, 0x20, BIT_ACTIVE_0 },

   { KB_DEF_P2_START,     MSG_P2_START,            0x020003, 0x80, BIT_ACTIVE_0 },
   { KB_DEF_P2_UP,        MSG_P2_UP,               0x020003, 0x04, BIT_ACTIVE_0 },
   { KB_DEF_P2_DOWN,      MSG_P2_DOWN,             0x020003, 0x08, BIT_ACTIVE_0 },
   { KB_DEF_P2_LEFT,      MSG_P2_LEFT,             0x020003, 0x01, BIT_ACTIVE_0 },
   { KB_DEF_P2_RIGHT,     MSG_P2_RIGHT,            0x020003, 0x02, BIT_ACTIVE_0 },
   { KB_DEF_P2_B1,        MSG_P2_B1,               0x020003, 0x10, BIT_ACTIVE_0 },
   { KB_DEF_P2_B2,        MSG_P2_B2,               0x020003, 0x20, BIT_ACTIVE_0 },

   { 0,                   NULL,                    0,        0,    0            },
};

static struct DSW_DATA dsw_data_chuka_taisen_0[] =
{
   { MSG_DSWA_BIT1,           0x01, 0x02 },
   { MSG_OFF,                 0x01},
   { MSG_ON,                  0x00},
   { MSG_DSWA_BIT2,           0x02, 0x02 },
   { MSG_OFF,                 0x02},
   { MSG_ON,                  0x00},
   { MSG_TEST_MODE,           0x04, 0x02 },
   { MSG_OFF,                 0x04},
   { MSG_ON,                  0x00},
   { MSG_DEMO_SOUND,          0x08, 0x02 },
   { MSG_ON,                  0x08},
   { MSG_OFF,                 0x00},
   { MSG_COIN1,               0x30, 0x04 },
   { MSG_1COIN_1PLAY,         0x30},
   { MSG_1COIN_2PLAY,         0x20},
   { MSG_2COIN_1PLAY,         0x10},
   { MSG_2COIN_3PLAY,         0x00},
   { MSG_COIN2,               0xC0, 0x04 },
   { MSG_1COIN_1PLAY,         0xC0},
   { MSG_1COIN_2PLAY,         0x80},
   { MSG_2COIN_1PLAY,         0x40},
   { MSG_2COIN_3PLAY,         0x00},
   { NULL,                    0,   },
};

static struct DSW_INFO chuka_taisen_dsw[] =
{
   { 0x000000, 0xFF, dsw_data_chuka_taisen_0 },
   { 0x000001, 0xFF, dsw_data_default_1 },
   { 0,        0,    NULL,      },
};

GAME( chuka_taisen ,
   chuka_taisen_dirs,
   chuka_taisen_roms,
   chuka_taisen_inputs,
   chuka_taisen_dsw,
   the_nz_story_romsw,

   load_chuka_taisen,
   clear_chuka_taisen,
   &tnzs_sys_video,
   ExecuteTNZSSystemFrame,
   "chukatai",
   "Chuka Taisen",
   NULL,
   COMPANY_ID_TAITO,
   "B44",
   1988,
   tnzs_sys_sound,
   GAME_SHOOT
);

static struct ROMSW_DATA romsw_dr_toppel_0[] =
{
  { "Dr Toppel Tankentai (Japan)", 0x01 },
  { "Dr Toppel's Adventure (US)", 0x02 },
  { "Dr Toppel's Adventure (World)", 0x03 },
  { NULL, 0 },
};

static struct ROMSW_INFO dr_toppel_romsw[] =
{
  { 0x2007FFF, 0x03, romsw_dr_toppel_0 }, // Need to be in RAM2!!!
  { 0, 0, NULL },
};

static struct DIR_INFO dr_toppel_dirs[] =
{
   { "dr_toppel_tankentai", },
   { "dr_toppel", },
   { "drtoppel", },
   { NULL, },
};

static struct ROM_INFO dr_toppel_roms[] =
{
   {   "b19-01.bin", 0x00020000, 0xa7e8a0c1, 0, 0, 0, },
   {   "b19-02.bin", 0x00020000, 0x790ae654, 0, 0, 0, },
   {   "b19-03.bin", 0x00020000, 0x495c4c5a, 0, 0, 0, },
   {   "b19-04.bin", 0x00020000, 0x647007a0, 0, 0, 0, },
   {   "b19-05.bin", 0x00020000, 0x49f2b1a5, 0, 0, 0, },
   {   "b19-06.bin", 0x00020000, 0x2d39f1d0, 0, 0, 0, },
   {   "b19-07.bin", 0x00020000, 0x8bb06f41, 0, 0, 0, },
   {   "b19-08.bin", 0x00020000, 0x3584b491, 0, 0, 0, },
   {   "b19-09.bin", 0x00010000, 0x3e654f82, 0, 0, 0, },
   {   "b19-10.bin", 0x00010000, 0x7e72fd25, 0, 0, 0, },
   {   "b19-11w.bin", 0x00010000, 0x37a0d3fb, 0, 0, 0, },
   {   "b19-12.bin", 0x00000200, 0x5754e9d8, 0, 0, 0, },
   {   "b19-13.bin", 0x00000200, 0x6a547980, 0, 0, 0, },
   {           NULL,          0,          0, 0, 0, 0, },
};

static struct INPUT_INFO dr_toppel_inputs[] =
{
   { KB_DEF_COIN1,        MSG_COIN1,               0x020004, 0x01, BIT_ACTIVE_1 },
   { KB_DEF_COIN2,        MSG_COIN2,               0x020004, 0x02, BIT_ACTIVE_1 },
   { KB_DEF_TILT,         MSG_TILT,                0x020005, 0x02, BIT_ACTIVE_1 },
   { KB_DEF_SERVICE,      MSG_SERVICE,             0x020005, 0x01, BIT_ACTIVE_1 },

   { KB_DEF_P1_START,     MSG_P1_START,            0x020003, 0x80, BIT_ACTIVE_1 },
   { KB_DEF_P1_UP,        MSG_P1_UP,               0x020006, 0x04, BIT_ACTIVE_1 },
   { KB_DEF_P1_DOWN,      MSG_P1_DOWN,             0x020006, 0x08, BIT_ACTIVE_1 },
   { KB_DEF_P1_LEFT,      MSG_P1_LEFT,             0x020006, 0x01, BIT_ACTIVE_1 },
   { KB_DEF_P1_RIGHT,     MSG_P1_RIGHT,            0x020006, 0x02, BIT_ACTIVE_1 },
   { KB_DEF_P1_B1,        MSG_P1_B1,               0x020003, 0x10, BIT_ACTIVE_1 },
   { KB_DEF_P1_B2,        MSG_P1_B2,               0x020003, 0x20, BIT_ACTIVE_1 },

   { KB_DEF_P2_START,     MSG_P2_START,            0x020008, 0x80, BIT_ACTIVE_1 },
   { KB_DEF_P2_UP,        MSG_P2_UP,               0x020007, 0x04, BIT_ACTIVE_1 },
   { KB_DEF_P2_DOWN,      MSG_P2_DOWN,             0x020007, 0x08, BIT_ACTIVE_1 },
   { KB_DEF_P2_LEFT,      MSG_P2_LEFT,             0x020007, 0x01, BIT_ACTIVE_1 },
   { KB_DEF_P2_RIGHT,     MSG_P2_RIGHT,            0x020007, 0x02, BIT_ACTIVE_1 },
   { KB_DEF_P2_B1,        MSG_P2_B1,               0x020008, 0x10, BIT_ACTIVE_1 },
   { KB_DEF_P2_B2,        MSG_P2_B2,               0x020008, 0x20, BIT_ACTIVE_1 },

   { 0,                   NULL,                    0,        0,    0            },
};

static struct DSW_DATA dsw_data_dr_toppel_0[] =
{
   { "Cabinet",               0x01, 0x02 },
   { "Cocktail",              0x01},
   { "Upright",               0x00},
   { MSG_SCREEN,              0x02, 0x02 },
   { MSG_NORMAL,              0x02},
   { MSG_INVERT,              0x00},
   { MSG_TEST_MODE,           0x04, 0x02 },
   { MSG_OFF,                 0x04},
   { MSG_ON,                  0x00},
   { MSG_DEMO_SOUND,          0x08, 0x02 },
   { MSG_ON,                  0x08},
   { MSG_OFF,                 0x00},
   { MSG_COIN1,               0x30, 0x04 },
   { MSG_1COIN_1PLAY,         0x30},
   { MSG_2COIN_1PLAY,         0x20},
   { MSG_3COIN_1PLAY,         0x10},
   { MSG_4COIN_1PLAY,         0x00},
   { MSG_COIN2,               0xC0, 0x04 },
   { MSG_1COIN_2PLAY,         0xC0},
   { MSG_1COIN_3PLAY,         0x80},
   { MSG_1COIN_4PLAY,         0x40},
   { MSG_1COIN_6PLAY,         0x00},
   { NULL,                    0,   },
};

static struct DSW_DATA dsw_data_dr_toppel_1[] =
{
   { MSG_DIFFICULTY,          0x03, 0x04 },
   { MSG_NORMAL,              0x03},
   { MSG_EASY,                0x02},
   { MSG_HARD,                0x01},
   { MSG_HARDEST,             0x00},
   { MSG_DSWB_BIT3,           0x04, 0x02 },
   { MSG_OFF,                 0x04},
   { MSG_ON,                  0x00},
   { MSG_DSWB_BIT4,           0x08, 0x02 },
   { MSG_OFF,                 0x08},
   { MSG_ON,                  0x00},
   { "Lives",                 0x30, 0x04 },
   { "3",                     0x30},
   { "2",                     0x20},
   { "5",                     0x10},
   { "4",                     0x00},
   { MSG_DSWB_BIT7,           0x40, 0x02 },
   { MSG_OFF,                 0x40},
   { MSG_ON,                  0x00},
   { MSG_DSWB_BIT8,           0x80, 0x02 },
   { MSG_OFF,                 0x80},
   { MSG_ON,                  0x00},
   { NULL,                    0,   },
};

static struct DSW_INFO dr_toppel_dsw[] =
{
   { 0x000000, 0xFE, dsw_data_dr_toppel_0 },
   { 0x000001, 0xFF, dsw_data_dr_toppel_1 },
   { 0,        0,    NULL,      },
};

GAME( dr_toppel_tankentai ,
   dr_toppel_dirs,
   dr_toppel_roms,
      dr_toppel_inputs,
   dr_toppel_dsw,
   dr_toppel_romsw,

   LoadDrToppel,
   ClearDrToppel,
   &tnzs_sys_r90_video,
   ExecuteTNZSSystemFrame,
   "drtoppel",
   "Dr Toppel Tankentai",
   NULL,
   COMPANY_ID_TAITO,
   "B19",
   1987,
   tnzs_sys_sound,
   GAME_SHOOT
);

static struct DIR_INFO plump_pop_dirs[] =
{
   { "plump_pop", },
   { "plumppop", },
   { NULL, },
};

static struct ROM_INFO plump_pop_roms[] =
{
   {   "a98-01.bin", 0x00010000, 0xf3033dca, 0, 0, 0, },
   {   "a98-02.bin", 0x00010000, 0xf2d17b0c, 0, 0, 0, },
   {   "a98-03.bin", 0x00010000, 0x1a519b0a, 0, 0, 0, },
   {   "a98-04.bin", 0x00010000, 0xb64501a1, 0, 0, 0, },
   {   "a98-05.bin", 0x00010000, 0x45c36963, 0, 0, 0, },
   {   "a98-06.bin", 0x00010000, 0xe075341b, 0, 0, 0, },
   {   "a98-07.bin", 0x00010000, 0x8e16cd81, 0, 0, 0, },
   {   "a98-08.bin", 0x00010000, 0xbfa7609a, 0, 0, 0, },
   {   "a98-09.bin", 0x00010000, 0x107f9e06, 0, 0, 0, },
   {   "a98-10.bin", 0x00010000, 0xdf6e6af2, 0, 0, 0, },
   {   "a98-11.bin", 0x00010000, 0xbc56775c, 0, 0, 0, },
   {   "a98-12.bpr", 0x00000200, 0x90dc9da7, 0, 0, 0, },
   {   "a98-13.bpr", 0x00000200, 0x7cde2da5, 0, 0, 0, },
   {           NULL,          0,          0, 0, 0, 0, },
};

static struct INPUT_INFO plump_pop_inputs[] =
{
   { KB_DEF_COIN1,        MSG_COIN1,               0x020004, 0x01, BIT_ACTIVE_0 },
   { KB_DEF_COIN2,        MSG_COIN2,               0x020004, 0x02, BIT_ACTIVE_0 },
   { KB_DEF_TILT,         MSG_TILT,                0x020005, 0x02, BIT_ACTIVE_0 },
   { KB_DEF_SERVICE,      MSG_SERVICE,             0x020005, 0x01, BIT_ACTIVE_0 },

   { KB_DEF_P1_START,     MSG_P1_START,            0x020003, 0x80, BIT_ACTIVE_1 },
   { KB_DEF_P1_UP,        MSG_P1_UP,               0x020006, 0x04, BIT_ACTIVE_1 },
   { KB_DEF_P1_DOWN,      MSG_P1_DOWN,             0x020006, 0x08, BIT_ACTIVE_1 },
   { KB_DEF_P1_LEFT,      MSG_P1_LEFT,             0x020006, 0x01, BIT_ACTIVE_1 },
   { KB_DEF_P1_RIGHT,     MSG_P1_RIGHT,            0x020006, 0x02, BIT_ACTIVE_1 },
   { KB_DEF_P1_B1,        MSG_P1_B1,               0x020003, 0x10, BIT_ACTIVE_1 },
   { KB_DEF_P1_B2,        MSG_P1_B2,               0x020003, 0x20, BIT_ACTIVE_1 },

   { KB_DEF_P2_START,     MSG_P2_START,            0x020003, 0x08, BIT_ACTIVE_1 },
   { KB_DEF_P2_UP,        MSG_P2_UP,               0x020007, 0x04, BIT_ACTIVE_1 },
   { KB_DEF_P2_DOWN,      MSG_P2_DOWN,             0x020007, 0x08, BIT_ACTIVE_1 },
   { KB_DEF_P2_LEFT,      MSG_P2_LEFT,             0x020007, 0x01, BIT_ACTIVE_1 },
   { KB_DEF_P2_RIGHT,     MSG_P2_RIGHT,            0x020007, 0x02, BIT_ACTIVE_1 },
   { KB_DEF_P2_B1,        MSG_P2_B1,               0x020003, 0x01, BIT_ACTIVE_1 },
   { KB_DEF_P2_B2,        MSG_P2_B2,               0x020003, 0x02, BIT_ACTIVE_1 },

   { 0,                   NULL,                    0,        0,    0            },
};

static struct DSW_DATA dsw_data_plump_pop_0[] =
{
   { MSG_DSWA_BIT1,           0x01, 0x02 },
   { MSG_OFF,                 0x01},
   { MSG_ON,                  0x00},
   { MSG_SCREEN,              0x02, 0x02 },
   { MSG_NORMAL,              0x02},
   { MSG_INVERT,              0x00},
   { MSG_TEST_MODE,           0x04, 0x02 },
   { MSG_OFF,                 0x04},
   { MSG_ON,                  0x00},
   { MSG_DEMO_SOUND,          0x08, 0x02 },
   { MSG_ON,                  0x08},
   { MSG_OFF,                 0x00},
   { MSG_COIN1,               0x30, 0x04 },
   { MSG_1COIN_1PLAY,         0x30},
   { MSG_2COIN_1PLAY,         0x20},
   { MSG_3COIN_1PLAY,         0x10},
   { MSG_4COIN_1PLAY,         0x00},
   { MSG_COIN2,               0xC0, 0x04 },
   { MSG_1COIN_2PLAY,         0xC0},
   { MSG_1COIN_3PLAY,         0x80},
   { MSG_1COIN_4PLAY,         0x40},
   { MSG_1COIN_6PLAY,         0x00},
   { NULL,                    0,   },
};

static struct DSW_DATA dsw_data_plump_pop_1[] =
{
   { MSG_DIFFICULTY,          0x03, 0x04 },
   { MSG_NORMAL,              0x03},
   { MSG_EASY,                0x02},
   { MSG_HARD,                0x01},
   { MSG_HARDEST,             0x00},
   { "Extra Life",            0x0C, 0x04 },
   { "70k, 200k",             0x0C, 0x00 },
   { "50k, 150k",             0x08, 0x00 },
   { "100k, 250k",            0x04, 0x00 },
   { "200k, 300k", 0x00 },
   { "Lives",                 0x30, 0x04 },
   { "3",                     0x30},
   { "2",                     0x20},
   { "5",                     0x10},
   { "4",                     0x00},
   { "Continue Play",         0x40, 0x02 },
   { MSG_ON,                  0x40},
   { MSG_OFF,                 0x00},
   { MSG_DSWB_BIT8,           0x80, 0x02 },
   { MSG_OFF,                 0x80},
   { MSG_ON,                  0x00},
   { NULL,                    0,   },
};

static struct DSW_INFO plump_pop_dsw[] =
{
   { 0x000000, 0xFF, dsw_data_plump_pop_0 },
   { 0x000001, 0xFF, dsw_data_plump_pop_1 },
   { 0,        0,    NULL,      },
};

static struct ROMSW_DATA romsw_data_plump_pop_0[] =
{
   { "Taito Corp",            0x00 },
   { "Taito America",         0x04 },
   { "Taito Japan",           0x08 },
   { "Taito licensed",        0x0C },
   { NULL,                    0    },
};

static struct ROMSW_INFO plump_pop_romsw[] =
{
   { 0x1007FFF, 0x00, romsw_data_plump_pop_0 },
   { 0,        0,    NULL },
};

GAME( plump_pop ,
   plump_pop_dirs,
   plump_pop_roms,
   plump_pop_inputs,
   plump_pop_dsw,
   plump_pop_romsw,

   LoadPlumpPop,
   ClearPlumpPop,
   &tnzs_sys_video,
   ExecuteTNZSSystemFrame,
   "plumppop",
   "Plump Pop",
   NULL,
   COMPANY_ID_TAITO,
   "A98",
   1987,
   tnzs_sys_sound,
   GAME_BREAKOUT
);

static int romset;

static UINT8 *RAM2;
static UINT8 *RAM3;
static UINT8 *RAM_BANK;

static UINT8 *GFX_BG0_SOLID;

/******************************************************************************/
/* TNZS-SYSTEM YM2203 AND INPUT/DSW ACCESS                                    */
/******************************************************************************/

static UINT8 ym2203_reg;
static UINT8 kageki_dsw_bit;

static UINT8 tnzs_ym2203_rb(UINT16 offset)
{
   if((offset&1)==0){
      return YM2203_status_port_0_r(0);
   }
   else{
      switch(ym2203_reg){
         case 0x0e: return get_dsw(0);
         case 0x0f: return get_dsw(1);
         default:
	   return YM2203_read_port_0_r(1);
      }
   }
}

static void tnzs_ym2203_wb(UINT16 offset, UINT8 data)
{
   if((offset&1)==0){
      ym2203_reg = data;
      YM2203_control_port_0_w(0, data);
   }
   else{
      YM2203_write_port_0_w(1, data);
   }
}

static UINT8 kageki_ym2203_rb(UINT16 offset)
{
   UINT32 i,j;

   if((offset&1)==0){
      return YM2203_status_port_0_r(0);
   }
   else{
      if(ym2203_reg==0x0e){
         j   = (get_dsw(1)<<8) | get_dsw(0);
         j >>= kageki_dsw_bit;
         j  &= 0x1111;

         i  = (j >>  0) << 0;
         i |= (j >>  4) << 1;
         i |= (j >>  8) << 2;
         i |= (j >> 12) << 3;

         return i;
      }
      else
         return YM2203_read_port_0_r(1);
   }
}

static void kageki_ym2203_wb(UINT16 offset, UINT8 data)
{
   if((offset&1)==0){
      ym2203_reg = data;
      YM2203_control_port_0_w(0, data);
   }
   else{
#if 0
      if(ym2203_reg==0x0f){
         kageki_dsw_bit  = (data >> 1) & 1;
         kageki_dsw_bit |= (data << 1) & 2;
      }
      else
#endif
         YM2203_write_port_0_w(1, data);
   }
}

static UINT8 TNZSSystemDSWRead(UINT16 offset)
{
   return get_dsw(offset&1);
}

static UINT8 TNZSSystemInputRead(UINT16 offset)
{
   return RAM[0x20002+(offset&3)];
}

/******************************************************************************/
/* 'TNZS'-SYSTEM INPUT PROTECTION (M-CHIP)                                    */
/******************************************************************************/

int current_inputport,number_of_credits;

static int mcu_command,mcu_coinage_init,mcu_readcredits;
static int mcu_reportcoin,mcu_type;
static UINT8 mcu_coinage[4];
static unsigned char mcu_coinsA,mcu_coinsB,mcu_credits;

enum
{
	MCU_NONE,
	MCU_EXTRMATN,
	MCU_ARKANOID,
	MCU_DRTOPPEL,
	MCU_CHUKATAI,
	MCU_TNZS
};

static void mcu_reset(void)
{
  current_inputport = -3;
  number_of_credits = 0;
  mcu_command = mcu_credits = mcu_coinage_init = mcu_reportcoin = 0;
  mcu_coinage[0] = 1;
  mcu_coinage[1] = 1;
  mcu_coinage[2] = 1;
  mcu_coinage[3] = 1;
  mcu_coinsA = 0;
  mcu_coinsB = 0;
}

static void mcu_handle_coins(int coin)
{
  static int insertcoin;

  coin ^= 0xff;

  /* The coin inputs and coin counter is managed by the i8742 mcu. */
  /* Here we simulate it. */
  /* Chuka Taisen has a limit of 9 credits, so any */
  /* coins that could push it over 9 should be rejected */
  /* Coin/Play settings must also be taken into consideration */

  if (coin & 0x08)	/* tilt */
    mcu_reportcoin = coin;
  else if (coin && coin != insertcoin)
    {
      if (coin & 0x01)	/* coin A */
	{
	  if ((mcu_type == MCU_CHUKATAI) && ((mcu_credits+mcu_coinage[1]) > 9))
	    {
	      // coin_lockout_global_w(1); /* Lock all coin slots */
	    }
	  else
	    {
	      // logerror("Coin dropped into slot A\n");
	      // coin_lockout_global_w(0); /* Unlock all coin slots */
	      // coin_counter_w(0,1); coin_counter_w(0,0); /* Count slot A */
	      mcu_coinsA++;
	      if (mcu_coinsA >= mcu_coinage[0])
		{
		  mcu_coinsA -= mcu_coinage[0];
		  mcu_credits += mcu_coinage[1];
		}
	    }
	}
      if (coin & 0x02)	/* coin B */
	{
	  if ((mcu_type == MCU_CHUKATAI) && ((mcu_credits+mcu_coinage[3]) > 9))
	    {
	      //coin_lockout_global_w(1); /* Lock all coin slots */
	    }
	  else
	    {
	      logerror("Coin dropped into slot B\n");
	      //coin_lockout_global_w(0); /* Unlock all coin slots */
	      //coin_counter_w(1,1); coin_counter_w(1,0); /* Count slot B */
	      mcu_coinsB++;
	      if (mcu_coinsB >= mcu_coinage[2])
		{
		  mcu_coinsB -= mcu_coinage[2];
		  mcu_credits += mcu_coinage[3];
		}
	    }
	}
      if (coin & 0x04)	/* service */
	{
	  logerror("Coin dropped into service slot C\n");
	  mcu_credits++;
	}
      mcu_reportcoin = coin;
    }
  else
    {
      //coin_lockout_global_w(0); /* Unlock all coin slots */
      mcu_reportcoin = 0;
    }
  insertcoin = coin;
}

static UINT8 mcu_tnzs_read(UINT16 offset)
{
   UINT8 ret;

   offset &=1;

   if(offset == 0){
     if (current_inputport < 0) {
       switch(current_inputport){
       case -3: ret = 0x5a; break;
       case -2: ret = 0xa5; break;
       case -1: ret = 0x55; break;
       }
       current_inputport++;
       return ret;
     }
     switch (mcu_command)
       {
       case 0x01:
	 return RAM[0x20006] | RAM[0x20003]; /* player 1 joystick + buttons */
       case 0x02:
	 return RAM[0x20007] | RAM[0x20008]; /* player 2 joystick + buttons */
       case 0x1a:
	 return RAM[0x20004] | 0xc; // coin 1 & 2
       case 0x21:
	 return RAM[0x20005]; // tilt, service
       case 0x41:
	 return mcu_credits;

       case 0xa0:
	 /* Read the credit counter */
	 if (mcu_reportcoin & 0x08)
	   {
	     current_inputport = -3;
	     return 0xee;	/* tilt */
	   }
	 else return mcu_credits;

       case 0xa1:
	 /* Read the credit counter or the inputs */
	 if (mcu_readcredits == 0)
	   {
	     mcu_readcredits = 1;
	     if (mcu_reportcoin & 0x08)
	       {
		 current_inputport = -3;
		 return 0xee;	/* tilt */
		 /*						return 0x64;	   theres a reset input somewhere    */
	       }
	     else return mcu_credits;
	   }
	 /* buttons */
	 else return (RAM[0x20003] & 0xf0) | (RAM[0x20008] >> 4);

       default:
	 return 0xff; // should never happen...
       }
   }
   else
     {
       /*
	 status bits:
	 0 = mcu is ready to send data (read from c000)
	 1 = mcu has read data (from c000)
	 2 = unused
	 3 = unused
	 4-7 = coin code
	 0 = nothing
	 1,2,3 = coin switch pressed
	 e = tilt
       */
       if (mcu_reportcoin & 0x08) return 0xe1;	/* tilt */
       if (mcu_type == MCU_TNZS)
	 {
	   if (mcu_reportcoin & 0x01) return 0x31;	/* coin 1 (will trigger "coin inserted" sound) */
	   if (mcu_reportcoin & 0x02) return 0x21;	/* coin 2 (will trigger "coin inserted" sound) */
	   if (mcu_reportcoin & 0x04) return 0x11;	/* coin 3 (will NOT trigger "coin inserted" sound) */
	 } else {
	   if (mcu_reportcoin & 0x01) return 0x11;	/* coin 1 (will trigger "coin inserted" sound) */
	   if (mcu_reportcoin & 0x02) return 0x21;	/* coin 2 (will trigger "coin inserted" sound) */
	   if (mcu_reportcoin & 0x04) return 0x31;	/* coin 3 (will trigger "coin inserted" sound) */
	 }

       return 0x01;
     }
}

static void mcu_tnzs_write(UINT16 offset, UINT8 data)
{
   offset &=1;

   if(offset == 0){
     if (mcu_command == 0x41)
       {
	 mcu_credits = (mcu_credits + data) & 0xff;
       }
/*       current_inputport = (current_inputport + 5) % 6; */
/*       print_debug("writing %02x to 0xc000: count set back to %d\n", data, current_inputport); */
   } else {
     /*
       0xa0: read number of credits
       0xa1: read number of credits, then buttons
       0x01: read player 1 joystick + buttons
       0x02: read player 2 joystick + buttons
       0x1a: read coin switches
       0x21: read service & tilt switches
       0x4a+0x41: add value to number of credits
       0x84: coin 1 lockout (issued only in test mode)
       0x88: coin 2 lockout (issued only in test mode)
       0x80: release coin lockout (issued only in test mode)
       during initialization, a sequence of 4 bytes sets coin/credit settings
     */

     if (current_inputport < 0) // init
       {
	 /* set up coin/credit settings */
	 mcu_coinage[mcu_coinage_init++] = data;
	 if (mcu_coinage_init == 4) mcu_coinage_init = 0;	/* must not happen */
       }
     if (data == 0xa1)
       mcu_readcredits = 0;	/* reset input port number */
     /* Dr Toppel decrements credits differently. So handle it */
     if ((data == 0x09) && (mcu_type == MCU_DRTOPPEL))
       mcu_credits = (mcu_credits - 1) & 0xff;		/* Player 1 start */
     if ((data == 0x18) && (mcu_type == MCU_DRTOPPEL))
       mcu_credits = (mcu_credits - 2) & 0xff;		/* Player 2 start */
     mcu_command = data;
   }
}

/* number of input ports to be cycled through (coins, buttons etc.) */
#define ip_num 2

static UINT8 a2_io_read(UINT16 offset)
{
	int ret;

   offset &=1;

   if (offset == 0){
        switch(current_inputport){
			case -3: ret = 0x55; break;
			case -2: ret = 0xaa; break;
			case -1: ret = 0x5a; break;
			case ip_num: current_inputport = 0; /* fall through */
			case 0:  ret = number_of_credits; break;
			default:
			  ret = RAM[0x20002+(current_inputport)];
			  break;
	}
	current_inputport++;
	return ret;
   }
   else {
     int ret = RAM[0x20004];
     int mcu_data;
     if (!(ret & 1)) mcu_data = 0x11; // coin 1
     else if (!(ret & 2)) mcu_data = 0x21; // coin 2
     else if (!(RAM[0x20005] & 2)) mcu_data = 0xe1; // tilt
     else {
       mcu_data = 1;
     }
     return mcu_data;
   }
}


static void a2_io_write(UINT16 offset, UINT8 data)
{
   offset &=1;

	if (offset == 0)
		number_of_credits -= ((~data)&0xff)+1;	/* sub data from credits */
	else {
	  if (data == 0x15) // The info was from Steph, I just checked it !
	    number_of_credits--;
	  if (current_inputport>=0)	/* if the initial sequence is done */
	    current_inputport=0;	/* reset input port number */
	}
}

static UINT8 mchip_port_0;
static UINT8 mchip_port_1;

static UINT8 chuka_ioc_rb(UINT16 offset)
{
   UINT8 ret;

   if((offset&1) == 0){

      switch(mchip_port_1){

         case 0x03: // ??? (bit0 = service?)
            ret = 0x01;
         break;
         case 0x1F: // COINA/B
            ret = RAM[0x20002+2];
         break;
         case 0x41: // ???
            ret = number_of_credits;
         break;
         case 0x93: // ??? (EE = tilt)
            ret = 0xFF;
         break;
         case 0x94: // P1
            ret = RAM[0x20002+0];
         break;
         case 0x95: // P2
            ret = RAM[0x20002+1];
         break;
         default:
            ret = 0xFF;
         break;

      }

      mchip_port_1++;
   }
   else{ // Status (ready: bit0 = 1; bit1 = 0)

      ret = 0x01;

   }

   return ret;
}

static void chuka_ioc_wb(UINT16 offset, UINT8 data)
{

   if((offset&1) == 0)

      mchip_port_0 = data;

   else

      mchip_port_1 = data;

}

static UINT8 sound_data;
static UINT32 want_int;

static void tnzs_b1_sound_wb(UINT16 offset, UINT8 data)
{
   print_debug("sound_write:%02x\n",data);
   sound_data = data;
   want_int = 1;
}

static UINT8 tnzs_b1_sound_rb(UINT16 offset)
{
   UINT8 ret;

   print_debug("sound_read:%02x\n",sound_data);

   ret = sound_data;

   return ret;
}

/******************************************************************************/
/* 'TNZS'-SYSTEM Z80 ROM BANKING                                              */
/******************************************************************************/

static void MainBankSwitch(UINT16 offset, UINT8 data)
{
  if (data & 0x10) {
    z80_set_bank(1,data&7);
  }
}

static void SubBankSwitch(UINT16 offset, UINT8 data)
{
   if((data>=0x10) && (data<0x18))
     z80_set_bank(2,data & 3);
}

static void SubBankSwitch_alt(UINT16 offset, UINT8 data)
{
   z80_set_bank(2,data & 3);
}

void init_bank_rom(UINT8 *src, UINT8 *dst)
{
  z80_init_banks_area_ofs(1,src,0x20000,0x8000,0x4000,2);
}

void init_bank_rom2(UINT8 *src)
{
  z80_init_banks_area(2,src,0x10000,0x8000,0x2000);
}

void LoadInsectorX(void)
{
   int ta,tb,tc;
   UINT8 *TMP;

   romset=0;

   RAMSize=0x10000+0x10000+0x10+(0xC000*2);

   if(!(RAM=AllocateMem(RAMSize))) return;

   RAM2     = RAM+0x10000;
   RAM_BANK = RAM+0x10000+0x10000+0x10;

   memset(RAM+0x00000, 0x00, RAMSize);

   if(!(TMP=AllocateMem(0x20000))) return;

   if(!load_rom("insector.u32", TMP, 0x20000)) return;	// Z80 MAIN ROM

   // Skip Idle Z80
   // -------------

   TMP[0x01F1]=0x00;  // nop
   TMP[0x01F2]=0x00;  // nop
   TMP[0x01F3]=0x00;  // nop

   TMP[0x0206]=0xD3;  // OUTA (AAh)
   TMP[0x0207]=0xAA;  //

   SetStopZ80BMode2(0x0206);

   init_bank_rom(TMP,ROM);

   memcpy(RAM,TMP,0xC000);
   AddZ80BROMBase(RAM, 0x0038, 0x0066);

   AddZ80BReadByte(0x0000, 0xBFFF, NULL,			NULL);		// BANK ROM/RAM
   AddZ80BReadByte(0xC000, 0xF3FF, NULL,			RAM+0x00C000);	// OBJECT RAM/WORK RAM/VDC RAM
   AddZ80BReadByte(0xF800, 0xFBFF, NULL,			RAM+0x00F800);	// COLOR RAM
   AddZ80BReadByte(0x0000, 0xFFFF, DefBadReadZ80,		NULL);		// <bad reads>
   AddZ80BReadByte(-1, -1, NULL, NULL);

   AddZ80BWriteByte(0x8000, 0xBFFF, NULL,			NULL);		// BANK RAM
   AddZ80BWriteByte(0xC000, 0xF3FF, NULL,			RAM+0x00C000);	// OBJECT RAM/WORK RAM/VDC RAM
   AddZ80BWriteByte(0xF800, 0xFBFF, NULL,			RAM+0x00F800);	// COLOR RAM
   AddZ80BWriteByte(0xF600, 0xF600, MainBankSwitch,		NULL);		// ROM BANK
   AddZ80BWriteByte(0x0000, 0xFFFF, DefBadWriteZ80,		NULL);		// <bad writes>
   AddZ80BWriteByte(-1, -1, NULL, NULL);

   AddZ80BReadPort(0x00, 0xFF, DefBadReadZ80,			NULL);		// <bad reads>
   AddZ80BReadPort(-1, -1, NULL, NULL);

   AddZ80BWritePort(0xAA, 0xAA, StopZ80BMode2,			NULL);		// Trap Idle Z80
   AddZ80BWritePort(0x00, 0xFF, DefBadWriteZ80,			NULL);		// <bad reads>
   AddZ80BWritePort(-1, -1, NULL, NULL);

   AddZ80BInit();

   if(!load_rom("insector.u38", TMP, 0x10000)) return;		// Z80 SUB ROM

   // Skip Idle Z80
   // -------------

   TMP[0x01F9]=0xD3;  // OUTA (AAh)
   TMP[0x01FA]=0xAA;  //

   SetStopZ80CMode2(0x01F9);

   init_bank_rom2(TMP);

   memcpy(RAM2,TMP,0xA000);
   AddZ80CROMBase(RAM2, 0x0038, 0x0066);

   AddZ80CReadByte(0x0000, 0x9FFF, NULL,			NULL);		// BANK ROM
   AddZ80CReadByte(0xD000, 0xDFFF, NULL,			RAM2+0x00D000);	// WORK RAM
   AddZ80CReadByte(0xE000, 0xEFFF, NULL,			RAM +0x00E000);	// COMMON RAM
   AddZ80CReadByte(0xB000, 0xB001, tnzs_ym2203_rb,		NULL);		// YM2203/DSW
   AddZ80CReadByte(0xC000, 0xC002, TNZSSystemInputRead,		NULL);		// INPUT
   AddZ80CReadByte(0xF000, 0xF003, NULL,			RAM2+0x00F000);	// ???
   AddZ80CReadByte(0x0000, 0xFFFF, DefBadReadZ80,		NULL);		// <bad reads>
   AddZ80CReadByte(-1, -1, NULL, NULL);

   AddZ80CWriteByte(0xD000, 0xDFFF, NULL,			RAM2+0x00D000);	// WORK RAM
   AddZ80CWriteByte(0xE000, 0xEFFF, NULL,			RAM +0x00E000);	// COMMON RAM
   AddZ80CWriteByte(0xA000, 0xA000, SubBankSwitch,		NULL);		// ROM BANK
   AddZ80CWriteByte(0xB000, 0xB001, tnzs_ym2203_wb,		NULL);		// YM2203/DSW
   AddZ80CWriteByte(0x0000, 0xFFFF, DefBadWriteZ80,		NULL);		// <bad writes>
   AddZ80CWriteByte(-1, -1, NULL, NULL);

   AddZ80CReadPort(0x00, 0xFF, DefBadReadZ80,			NULL);		// <bad reads>
   AddZ80CReadPort(-1, -1, NULL, NULL);

   AddZ80CWritePort(0xAA, 0xAA, StopZ80CMode2,			NULL);		// Trap Idle Z80
   AddZ80CWritePort(0x00, 0xFF, DefBadWriteZ80,			NULL);		// <bad writes>
   AddZ80CWritePort(-1, -1, NULL, NULL);

   AddZ80CInit();

   FreeMem(TMP);

   /*-----------------------*/

   if(!(GFX=AllocateMem(0x200000))) return;
   if(!(TMP=AllocateMem(0x080000))) return;

   if(!load_rom("insector.r15", TMP, 0x80000)) return;
   tb=0;
   for(ta=0;ta<0x80000;ta+=2){
      tc=TMP[ta+1];
      GFX[tb+0]=((tc&0x80)>>7)<<3;
      GFX[tb+1]=((tc&0x40)>>6)<<3;
      GFX[tb+2]=((tc&0x20)>>5)<<3;
      GFX[tb+3]=((tc&0x10)>>4)<<3;
      GFX[tb+4]=((tc&0x08)>>3)<<3;
      GFX[tb+5]=((tc&0x04)>>2)<<3;
      GFX[tb+6]=((tc&0x02)>>1)<<3;
      GFX[tb+7]=((tc&0x01)>>0)<<3;
      tc=TMP[ta];
      GFX[tb+0]|=((tc&0x80)>>7)<<2;
      GFX[tb+1]|=((tc&0x40)>>6)<<2;
      GFX[tb+2]|=((tc&0x20)>>5)<<2;
      GFX[tb+3]|=((tc&0x10)>>4)<<2;
      GFX[tb+4]|=((tc&0x08)>>3)<<2;
      GFX[tb+5]|=((tc&0x04)>>2)<<2;
      GFX[tb+6]|=((tc&0x02)>>1)<<2;
      GFX[tb+7]|=((tc&0x01)>>0)<<2;
      tb+=16;
      if((tb&0x7F)==0){tb-=0x78;}
      else{if((tb&0x7F)==8){tb-=0x08;}}
   }
   if(!load_rom("insector.r16", TMP, 0x80000)) return;
   tb=0;
   for(ta=0;ta<0x80000;ta+=2){
      tc=TMP[ta+1];
      GFX[tb+0]|=((tc&0x80)>>7)<<1;
      GFX[tb+1]|=((tc&0x40)>>6)<<1;
      GFX[tb+2]|=((tc&0x20)>>5)<<1;
      GFX[tb+3]|=((tc&0x10)>>4)<<1;
      GFX[tb+4]|=((tc&0x08)>>3)<<1;
      GFX[tb+5]|=((tc&0x04)>>2)<<1;
      GFX[tb+6]|=((tc&0x02)>>1)<<1;
      GFX[tb+7]|=((tc&0x01)>>0)<<1;
      tc=TMP[ta];
      GFX[tb+0]|=((tc&0x80)>>7)<<0;
      GFX[tb+1]|=((tc&0x40)>>6)<<0;
      GFX[tb+2]|=((tc&0x20)>>5)<<0;
      GFX[tb+3]|=((tc&0x10)>>4)<<0;
      GFX[tb+4]|=((tc&0x08)>>3)<<0;
      GFX[tb+5]|=((tc&0x04)>>2)<<0;
      GFX[tb+6]|=((tc&0x02)>>1)<<0;
      GFX[tb+7]|=((tc&0x01)>>0)<<0;
      tb+=16;
      if((tb&0x7F)==0){tb-=0x78;}
      else{if((tb&0x7F)==8){tb-=0x08;}}
   }

   FreeMem(TMP);

   GFX_BG0_SOLID = make_solid_mask_16x16(GFX, 0x2000);

   InitPaletteMap(RAM+0xF800, 0x20, 0x10, 0x8000);

   set_colour_mapper(&col_map_xrrr_rrgg_gggb_bbbb);

   // Init seta_x1 emulation
   // ----------------------

   seta_x1.RAM_A	= RAM+0x0C000;
   seta_x1.RAM_B	= RAM+0x0F000;
   seta_x1.GFX		= GFX;
   seta_x1.MASK		= GFX_BG0_SOLID;
   seta_x1.bmp_x	= 32;
   seta_x1.bmp_y	= 32;
   seta_x1.bmp_w	= 256;
   seta_x1.bmp_h	= 224;
   seta_x1.tile_mask	= 0x1FFF;
   seta_x1.scr_x	= 0;
   seta_x1.scr_y	= 0;
}

void ClearInsectorX(void)
{
#ifdef RAINE_DEBUG
      save_debug("RAM.BIN", RAM, RAMSize, 0);
      //save_debug("GFX.BIN", GFX, 0x200000, 0);
#endif
}

void LoadTNZSExtra(void)
{
   int ta,tb,tc;
   UINT8 *TMP;

   romset=1;

   RAMSize=0x10000+0x10000+0x10+(0xC000*2);

   if(!(ROM =AllocateMem(0xC000*6))) return;

   if(!(RAM=AllocateMem(RAMSize))) return;

   RAM2     = RAM+0x10000;
   RAM_BANK = RAM+0x10000+0x10000+0x10;

   memset(RAM+0x00000, 0x00, RAMSize);

   if(!(TMP=AllocateMem(0x20000))) return;

   if(!load_rom("ns_c-11.rom", TMP, 0x20000)) return;	// Z80 MAIN ROM

   // Skip Idle Z80
   // -------------

   TMP[0x0AE9]=0xC9;  // ret

   TMP[0x0294]=0xD3;  // OUTA (AAh)
   TMP[0x0295]=0xAA;  //

   SetStopZ80BMode2(0x028B);

   init_bank_rom(TMP,ROM);

   memcpy(RAM,TMP,0x10000);
   AddZ80BROMBase(RAM, 0x0038, 0x0066);

   AddZ80BReadByte(0x0000, 0xBFFF, NULL,			NULL);		// BANK ROM/RAM
   AddZ80BReadByte(0xC000, 0xF3FF, NULL,			RAM+0x00C000);	// OBJECT RAM/WORK RAM/VDC RAM
   AddZ80BReadByte(0xF800, 0xFBFF, NULL,			RAM+0x00F800);	// COLOR RAM
   AddZ80BReadByte(0x0000, 0xFFFF, DefBadReadZ80,		NULL);		// <bad reads>
   AddZ80BReadByte(-1, -1, NULL, NULL);

   AddZ80BWriteByte(0x8000, 0xBFFF, NULL,			NULL);		// BANK RAM
   AddZ80BWriteByte(0xC000, 0xF3FF, NULL,			RAM+0x00C000);	// OBJECT RAM/WORK RAM/VDC RAM
   AddZ80BWriteByte(0xF800, 0xFBFF, NULL,			RAM+0x00F800);	// COLOR RAM
   AddZ80BWriteByte(0xF600, 0xF600, MainBankSwitch,		NULL);		// ROM BANK
   AddZ80BWriteByte(0x0000, 0xFFFF, DefBadWriteZ80,		NULL);		// <bad writes>
   AddZ80BWriteByte(-1, -1, NULL, NULL);

   AddZ80BReadPort(0x00, 0xFF, DefBadReadZ80,			NULL);		// <bad reads>
   AddZ80BReadPort(-1, -1, NULL, NULL);

   AddZ80BWritePort(0xAA, 0xAA, StopZ80BMode2,			NULL);		// Trap Idle Z80
   AddZ80BWritePort(0x00, 0xFF, DefBadWriteZ80,			NULL);		// <bad reads>
   AddZ80BWritePort(-1, -1, NULL, NULL);

   AddZ80BInit();

   if(!load_rom("ns_e-3.rom", TMP, 0x10000)) return;	// Z80 SUB ROM

   // Skip Idle Z80
   // -------------

   TMP[0x013E]=0xD3;  // OUTA (AAh)
   TMP[0x013F]=0xAA;  //

   SetStopZ80CMode2(0x0139);

   init_bank_rom2(TMP);

   memcpy(RAM2,TMP,0x10000);
   AddZ80CROMBase(RAM2, 0x0038, 0x0066);

   AddZ80CReadByte(0x0000, 0x9FFF, NULL,			NULL);		// BANK ROM
   AddZ80CReadByte(0xD000, 0xDFFF, NULL,			RAM2+0x00D000);	// WORK RAM
   AddZ80CReadByte(0xE000, 0xEFFF, NULL,			RAM +0x00E000);	// COMMON RAM
   AddZ80CReadByte(0xB000, 0xB001, tnzs_ym2203_rb,		NULL);		// YM2203/DSW
   AddZ80CReadByte(0xC000, 0xC001, mcu_tnzs_read,		NULL);		// INPUT
   AddZ80CReadByte(0xF000, 0xF003, NULL,			RAM2+0x00F000);	// ???
   AddZ80CReadByte(0x0000, 0xFFFF, DefBadReadZ80,		NULL);		// <bad reads>
   AddZ80CReadByte(-1, -1, NULL, NULL);

   AddZ80CWriteByte(0xD000, 0xDFFF, NULL,			RAM2+0x00D000);	// WORK RAM
   AddZ80CWriteByte(0xE000, 0xEFFF, NULL,			RAM +0x00E000);	// COMMON RAM
   AddZ80CWriteByte(0xA000, 0xA000, SubBankSwitch,		NULL);		// ROM BANK
   AddZ80CWriteByte(0xB000, 0xB001, tnzs_ym2203_wb,		NULL);		// YM2203/DSW
   AddZ80CWriteByte(0xC000, 0xC001, mcu_tnzs_write,		NULL);		// INPUT
   AddZ80CWriteByte(0x0000, 0xFFFF, DefBadWriteZ80,		NULL);		// <bad writes>
   AddZ80CWriteByte(-1, -1, NULL, NULL);

   AddZ80CReadPort(0x00, 0xFF, DefBadReadZ80,			NULL);		// <bad reads>
   AddZ80CReadPort(-1, -1, NULL, NULL);

   AddZ80CWritePort(0xAA, 0xAA, StopZ80CMode2,			NULL);		// Trap Idle Z80
   AddZ80CWritePort(0x00, 0xFF, DefBadWriteZ80,			NULL);		// <bad writes>
   AddZ80CWritePort(-1, -1, NULL, NULL);

   AddZ80CInit();
   z80_set_bank(2,0);

   FreeMem(TMP);
   mcu_type = MCU_TNZS;

   /*-----------------------*/

   if(!(GFX=AllocateMem(0x200000))) return;
   if(!(TMP=AllocateMem(0x080000))) return;

   if(!load_rom("ns_a13.rom", TMP+0x00000, 0x20000)) return;
   if(!load_rom("ns_a12.rom", TMP+0x20000, 0x20000)) return;
   if(!load_rom("ns_a10.rom", TMP+0x40000, 0x20000)) return;
   if(!load_rom("ns_a08.rom", TMP+0x60000, 0x20000)) return;
   tb=0;
   for(ta=0;ta<0x40000;ta++){
      tc=TMP[ta];
      GFX[tb+0]=((tc&0x80)>>7)<<0;
      GFX[tb+1]=((tc&0x40)>>6)<<0;
      GFX[tb+2]=((tc&0x20)>>5)<<0;
      GFX[tb+3]=((tc&0x10)>>4)<<0;
      GFX[tb+4]=((tc&0x08)>>3)<<0;
      GFX[tb+5]=((tc&0x04)>>2)<<0;
      GFX[tb+6]=((tc&0x02)>>1)<<0;
      GFX[tb+7]=((tc&0x01)>>0)<<0;
      tc=TMP[ta+0x40000];
      GFX[tb+0]|=((tc&0x80)>>7)<<1;
      GFX[tb+1]|=((tc&0x40)>>6)<<1;
      GFX[tb+2]|=((tc&0x20)>>5)<<1;
      GFX[tb+3]|=((tc&0x10)>>4)<<1;
      GFX[tb+4]|=((tc&0x08)>>3)<<1;
      GFX[tb+5]|=((tc&0x04)>>2)<<1;
      GFX[tb+6]|=((tc&0x02)>>1)<<1;
      GFX[tb+7]|=((tc&0x01)>>0)<<1;
      tb+=16;
      if((tb&0x7F)==0){tb-=0x78;}
      else{if((tb&0x7F)==8){tb-=0x08;}}
   }
   if(!load_rom("ns_a07.rom", TMP+0x00000, 0x20000)) return;
   if(!load_rom("ns_a05.rom", TMP+0x20000, 0x20000)) return;
   if(!load_rom("ns_a04.rom", TMP+0x40000, 0x20000)) return;
   if(!load_rom("ns_a02.rom", TMP+0x60000, 0x20000)) return;
   tb=0;
   for(ta=0;ta<0x40000;ta++){
      tc=TMP[ta];
      GFX[tb+0]|=((tc&0x80)>>7)<<2;
      GFX[tb+1]|=((tc&0x40)>>6)<<2;
      GFX[tb+2]|=((tc&0x20)>>5)<<2;
      GFX[tb+3]|=((tc&0x10)>>4)<<2;
      GFX[tb+4]|=((tc&0x08)>>3)<<2;
      GFX[tb+5]|=((tc&0x04)>>2)<<2;
      GFX[tb+6]|=((tc&0x02)>>1)<<2;
      GFX[tb+7]|=((tc&0x01)>>0)<<2;
      tc=TMP[ta+0x40000];
      GFX[tb+0]|=((tc&0x80)>>7)<<3;
      GFX[tb+1]|=((tc&0x40)>>6)<<3;
      GFX[tb+2]|=((tc&0x20)>>5)<<3;
      GFX[tb+3]|=((tc&0x10)>>4)<<3;
      GFX[tb+4]|=((tc&0x08)>>3)<<3;
      GFX[tb+5]|=((tc&0x04)>>2)<<3;
      GFX[tb+6]|=((tc&0x02)>>1)<<3;
      GFX[tb+7]|=((tc&0x01)>>0)<<3;
      tb+=16;
      if((tb&0x7F)==0){tb-=0x78;}
      else{if((tb&0x7F)==8){tb-=0x08;}}
   }

   FreeMem(TMP);

   GFX_BG0_SOLID = make_solid_mask_16x16(GFX, 0x2000);

   InitPaletteMap(RAM+0xF800, 0x20, 0x10, 0x8000);

   set_colour_mapper(&col_map_xrrr_rrgg_gggb_bbbb);

   // Init seta_x1 emulation
   // ----------------------

   seta_x1.RAM_A	= RAM+0x0C000;
   seta_x1.RAM_B	= RAM+0x0F000;
   seta_x1.GFX		= GFX;
   seta_x1.MASK		= GFX_BG0_SOLID;
   seta_x1.bmp_x	= 32;
   seta_x1.bmp_y	= 32;
   seta_x1.bmp_w	= 256;
   seta_x1.bmp_h	= 224;
   seta_x1.tile_mask	= 0x1FFF;
   seta_x1.scr_x	= 0;
   seta_x1.scr_y	= 0;
}

void ClearTNZSExtra(void)
{
#ifdef RAINE_DEBUG
      save_debug("RAM.BIN", RAM, RAMSize, 0);
      //save_debug("GFX.BIN", GFX, 0x200000, 0);
#endif
}

static void load_arkanoid_2_actual(int set)
{
   int ta,tb,tc;
   UINT8 *TMP;

   romset=2;
   p1_trackball_x = 0;

   RAMSize=0x10000+0x10000+0x10+(0xC000*2);

   if(!(ROM =AllocateMem(0xC000*6))) return;

   if(!(RAM=AllocateMem(RAMSize))) return;

   RAM2     = RAM+0x10000;
   RAM_BANK = RAM+0x10000+0x10000+0x10;

   memset(RAM+0x00000, 0x00, RAMSize);

   if(!(TMP=AllocateMem(0x20000))) return;

   if(!load_rom_index(6, TMP, 0x10000)) return;	// Z80 MAIN ROM
   memset(TMP+0x10000,0x00,0x10000);

   if((set==0)||(set==2)){

   // Skip Idle Z80
   // -------------

   TMP[0x2605]=0x18;  // jr xx

   TMP[0x2630]=0xD3;  // OUTA (AAh)
   TMP[0x2631]=0xAA;  //

   SetStopZ80BMode2(0x2625);

   }

   if(set==1){

   // Skip Idle Z80
   // -------------

   TMP[0x2591]=0x18;  // jr xx

   TMP[0x25BC]=0xD3;  // OUTA (AAh)
   TMP[0x25BD]=0xAA;  //

   SetStopZ80BMode2(0x25B1);

   }

   init_bank_rom(TMP,ROM);

   memcpy(RAM,TMP,0x10000);
   AddZ80BROMBase(RAM, 0x0038, 0x0066);

   AddZ80BReadByte(0x0000, 0xBFFF, NULL,			NULL);		// BANK ROM/RAM
   AddZ80BReadByte(0xC000, 0xF3FF, NULL,			RAM+0x00C000);	// OBJECT RAM/WORK RAM/VDC RAM
   AddZ80BReadByte(0xF800, 0xFBFF, NULL,			RAM+0x00F800);	// COLOR RAM
   AddZ80BReadByte(0x0000, 0xFFFF, DefBadReadZ80,		NULL);		// <bad reads>
   AddZ80BReadByte(-1, -1, NULL, NULL);

   AddZ80BWriteByte(0x8000, 0xBFFF, NULL,			NULL);		// BANK RAM
   AddZ80BWriteByte(0xC000, 0xF3FF, NULL,			RAM+0x00C000);	// OBJECT RAM/WORK RAM/VDC RAM
   AddZ80BWriteByte(0xF800, 0xFBFF, NULL,			RAM+0x00F800);	// COLOR RAM
   AddZ80BWriteByte(0xF600, 0xF600, MainBankSwitch,		NULL);		// ROM BANK
   AddZ80BWriteByte(0x0000, 0xFFFF, DefBadWriteZ80,		NULL);		// <bad writes>
   AddZ80BWriteByte(-1, -1, NULL, NULL);

   AddZ80BReadPort(0x00, 0xFF, DefBadReadZ80,			NULL);		// <bad reads>
   AddZ80BReadPort(-1, -1, NULL, NULL);

   AddZ80BWritePort(0xAA, 0xAA, StopZ80BMode2,			NULL);		// Trap Idle Z80
   AddZ80BWritePort(0x00, 0xFF, DefBadWriteZ80,			NULL);		// <bad reads>
   AddZ80BWritePort(-1, -1, NULL, NULL);

   AddZ80BInit();

   if(!load_rom_index(7, TMP, 0x10000)) return;		// Z80 SUB ROM

   // Skip Idle Z80
   // -------------

   TMP[0x0A9A]=0x18;  // jr xx

   TMP[0x0B71]=0xD3;  // OUTA (AAh)
   TMP[0x0B72]=0xAA;  //

   SetStopZ80CMode2(0x0B71);

   init_bank_rom2(TMP);

   memcpy(RAM2,TMP,0x10000);
   AddZ80CROMBase(RAM2, 0x0038, 0x0066);

   AddZ80CReadByte(0x0000, 0x9FFF, NULL,			NULL);		// BANK ROM
   AddZ80CReadByte(0xD000, 0xDFFF, NULL,			RAM2+0x00D000);	// WORK RAM
   AddZ80CReadByte(0xE000, 0xEFFF, NULL,			RAM +0x00E000);	// COMMON RAM
   AddZ80CReadByte(0xB000, 0xB001, tnzs_ym2203_rb,		NULL);		// YM2203/DSW
   AddZ80CReadByte(0xC000, 0xC001, a2_io_read,			NULL);		// INPUT
   AddZ80CReadByte(0xF000, 0xF003, NULL,			RAM2+0x00F000);	// ???
   AddZ80CReadByte(0x0000, 0xFFFF, DefBadReadZ80,		NULL);		// <bad reads>
   AddZ80CReadByte(-1, -1, NULL, NULL);

   AddZ80CWriteByte(0xD000, 0xDFFF, NULL,			RAM2+0x00D000);	// WORK RAM
   AddZ80CWriteByte(0xE000, 0xEFFF, NULL,			RAM +0x00E000);	// COMMON RAM
   AddZ80CWriteByte(0xA000, 0xA000, SubBankSwitch,		NULL);		// ROM BANK
   AddZ80CWriteByte(0xB000, 0xB001, tnzs_ym2203_wb,		NULL);		// YM2203/DSW
   AddZ80CWriteByte(0xC000, 0xC001, a2_io_write,		NULL);		// INPUT
   AddZ80CWriteByte(0x0000, 0xFFFF, DefBadWriteZ80,		NULL);		// <bad writes>
   AddZ80CWriteByte(-1, -1, NULL, NULL);

   AddZ80CReadPort(0x00, 0xFF, DefBadReadZ80,			NULL);		// <bad reads>
   AddZ80CReadPort(-1, -1, NULL, NULL);

   AddZ80CWritePort(0xAA, 0xAA, StopZ80CMode2,			NULL);		// Trap Idle Z80
   AddZ80CWritePort(0x00, 0xFF, DefBadWriteZ80,			NULL);		// <bad writes>
   AddZ80CWritePort(-1, -1, NULL, NULL);

   AddZ80CInit();

   FreeMem(TMP);

   /*-----------------------*/

   if(!(GFX=AllocateMem(0x200000))) return;
   if(!(TMP=AllocateMem(0x080000))) return;

   if(!load_rom_index(4, TMP+0x000, 0x200)) return;	// COLOR ROM
   if(!load_rom_index(5, TMP+0x200, 0x200)) return;	// COLOR ROM
   for(ta=0;ta<512;ta++){
   RAM[ta+ta+0xF800]=TMP[ta+0x000];
   RAM[ta+ta+0xF801]=TMP[ta+0x200];
   }

   if(!load_rom_index(0, TMP+0x00000, 0x20000)) return;
   if(!load_rom_index(0, TMP+0x20000, 0x20000)) return;
   if(!load_rom_index(1, TMP+0x40000, 0x20000)) return;
   if(!load_rom_index(1, TMP+0x60000, 0x20000)) return;
   tb=0;
   for(ta=0;ta<0x40000;ta++){
      tc=TMP[ta];
      GFX[tb+0]=((tc&0x80)>>7)<<0;
      GFX[tb+1]=((tc&0x40)>>6)<<0;
      GFX[tb+2]=((tc&0x20)>>5)<<0;
      GFX[tb+3]=((tc&0x10)>>4)<<0;
      GFX[tb+4]=((tc&0x08)>>3)<<0;
      GFX[tb+5]=((tc&0x04)>>2)<<0;
      GFX[tb+6]=((tc&0x02)>>1)<<0;
      GFX[tb+7]=((tc&0x01)>>0)<<0;
      tc=TMP[ta+0x40000];
      GFX[tb+0]|=((tc&0x80)>>7)<<1;
      GFX[tb+1]|=((tc&0x40)>>6)<<1;
      GFX[tb+2]|=((tc&0x20)>>5)<<1;
      GFX[tb+3]|=((tc&0x10)>>4)<<1;
      GFX[tb+4]|=((tc&0x08)>>3)<<1;
      GFX[tb+5]|=((tc&0x04)>>2)<<1;
      GFX[tb+6]|=((tc&0x02)>>1)<<1;
      GFX[tb+7]|=((tc&0x01)>>0)<<1;
      tb+=16;
      if((tb&0x7F)==0){tb-=0x78;}
      else{if((tb&0x7F)==8){tb-=0x08;}}
   }
   if(!load_rom_index(2, TMP+0x00000, 0x20000)) return;
   if(!load_rom_index(2, TMP+0x20000, 0x20000)) return;
   if(!load_rom_index(3, TMP+0x40000, 0x20000)) return;
   if(!load_rom_index(3, TMP+0x60000, 0x20000)) return;
   tb=0;
   for(ta=0;ta<0x40000;ta++){
      tc=TMP[ta];
      GFX[tb+0]|=((tc&0x80)>>7)<<2;
      GFX[tb+1]|=((tc&0x40)>>6)<<2;
      GFX[tb+2]|=((tc&0x20)>>5)<<2;
      GFX[tb+3]|=((tc&0x10)>>4)<<2;
      GFX[tb+4]|=((tc&0x08)>>3)<<2;
      GFX[tb+5]|=((tc&0x04)>>2)<<2;
      GFX[tb+6]|=((tc&0x02)>>1)<<2;
      GFX[tb+7]|=((tc&0x01)>>0)<<2;
      tc=TMP[ta+0x40000];
      GFX[tb+0]|=((tc&0x80)>>7)<<3;
      GFX[tb+1]|=((tc&0x40)>>6)<<3;
      GFX[tb+2]|=((tc&0x20)>>5)<<3;
      GFX[tb+3]|=((tc&0x10)>>4)<<3;
      GFX[tb+4]|=((tc&0x08)>>3)<<3;
      GFX[tb+5]|=((tc&0x04)>>2)<<3;
      GFX[tb+6]|=((tc&0x02)>>1)<<3;
      GFX[tb+7]|=((tc&0x01)>>0)<<3;
      tb+=16;
      if((tb&0x7F)==0){tb-=0x78;}
      else{if((tb&0x7F)==8){tb-=0x08;}}
   }

   FreeMem(TMP);

   GFX_BG0_SOLID = make_solid_mask_16x16(GFX, 0x2000);

   InitPaletteMap(RAM+0xF800, 0x20, 0x10, 0x8000);

   set_colour_mapper(&col_map_xrrr_rrgg_gggb_bbbb);

   // Init seta_x1 emulation
   // ----------------------

   seta_x1.RAM_A	= RAM+0x0C000;
   seta_x1.RAM_B	= RAM+0x0F000;
   seta_x1.GFX		= GFX;
   seta_x1.MASK		= GFX_BG0_SOLID;
   seta_x1.bmp_x	= 32;
   seta_x1.bmp_y	= 32;
   seta_x1.bmp_w	= 224;
   seta_x1.bmp_h	= 256;
   seta_x1.tile_mask	= 0x1FFF;
   seta_x1.scr_x	= 0;
   seta_x1.scr_y	= 0;

   GameMouse=1;
}

void LoadArk2Doh(void)
{
   load_arkanoid_2_actual(0);
}

void LoadArk2DohUS(void)
{
   load_arkanoid_2_actual(1);
}

void LoadArk2DohJP(void)
{
   load_arkanoid_2_actual(2);
}

void ClearArk2Doh(void)
{
#ifdef RAINE_DEBUG
      save_debug("RAM.BIN", RAM, RAMSize, 0);
      save_debug("GFX.BIN", GFX, 0x200000, 0);
#endif
}

void LoadTNZS(void)
{
   int ta,tb,tc;
   UINT8 *TMP;

   romset=3;

   RAMSize=0x10000+0x10000+0x10+(0xC000*2);

   // if(!(ROM =AllocateMem(0xC000*6))) return;

   if(!(RAM=AllocateMem(RAMSize))) return;

   RAM2     = RAM+0x10000;
   RAM_BANK = RAM+0x10000+0x10000+0x10;

   memset(RAM+0x00000, 0x00, RAMSize);

   // if(!load_rom("b53_10.32", TMP, 0x20000)) return;	// Z80 MAIN ROM

   // Skip Idle Z80
   // -------------

   ROM[0x0CB6]=0xC9;  // ret

   ROM[0x02E3]=0xD3;  // OUTA (AAh)
   ROM[0x02E4]=0xAA;  //

   SetStopZ80BMode2(0x02DA);

   // init_bank_rom(TMP,ROM);
   // z80_init_banks(1,REGION_ROM1,0x8000,0x4000);
  z80_init_banks_area_ofs(1,ROM,0x20000,0x8000,0x4000,2);

   // memcpy(RAM,ROM,0x10000);
   AddZ80BROMBase(ROM, 0x0038, 0x0066);

   AddZ80BReadByte(0x0000, 0xBFFF, NULL,			NULL);		// BANK ROM/RAM
   AddZ80BReadByte(0xC000, 0xF3FF, NULL,			RAM+0x00C000);	// OBJECT RAM/WORK RAM/VDC RAM
   AddZ80BReadByte(0xF800, 0xFBFF, NULL,			RAM+0x00F800);	// COLOR RAM
   AddZ80BReadByte(0x0000, 0xFFFF, DefBadReadZ80,		NULL);		// <bad reads>
   AddZ80BReadByte(-1, -1, NULL, NULL);

   AddZ80BWriteByte(0x8000, 0xBFFF, NULL,			NULL);		// BANK RAM
   AddZ80BWriteByte(0xC000, 0xF3FF, NULL,			RAM+0x00C000);	// OBJECT RAM/WORK RAM/VDC RAM
   AddZ80BWriteByte(0xF800, 0xFBFF, NULL,			RAM+0x00F800);	// COLOR RAM
   AddZ80BWriteByte(0xF600, 0xF600, MainBankSwitch,		NULL);		// ROM BANK
   AddZ80BWriteByte(0x0000, 0xFFFF, DefBadWriteZ80,		NULL);		// <bad writes>
   AddZ80BWriteByte(-1, -1, NULL, NULL);

   AddZ80BReadPort(0x00, 0xFF, DefBadReadZ80,			NULL);		// <bad reads>
   AddZ80BReadPort(-1, -1, NULL, NULL);

   AddZ80BWritePort(0xAA, 0xAA, StopZ80BMode2,			NULL);		// Trap Idle Z80
   AddZ80BWritePort(0x00, 0xFF, DefBadWriteZ80,			NULL);		// <bad reads>
   AddZ80BWritePort(-1, -1, NULL, NULL);

   AddZ80BInit();

   // Skip Idle Z80
   // -------------

   Z80ROM[0x0692]=0xC9;  // ret

   Z80ROM[0x0018]=0xD3;  // OUTA (AAh)
   Z80ROM[0x0019]=0xAA;  //

   SetStopZ80CMode2(0x0013);

   // init_bank_rom2(TMP);
   z80_init_banks(2,REGION_ROM2,0x8000,0x2000);

   memcpy(RAM2,Z80ROM,0x10000);
   AddZ80CROMBase(Z80ROM, 0x0038, 0x0066);

   AddZ80CReadByte(0x0000, 0x9FFF, NULL,			NULL);		// BANK ROM
   AddZ80CReadByte(0xD000, 0xDFFF, NULL,			RAM2+0x00D000);	// WORK RAM
   AddZ80CReadByte(0xE000, 0xEFFF, NULL,			RAM +0x00E000);	// COMMON RAM
   AddZ80CReadByte(0xB000, 0xB001, tnzs_ym2203_rb,		NULL);		// YM2203/DSW
   AddZ80CReadByte(0xC000, 0xC001, mcu_tnzs_read,		NULL);		// INPUT
   AddZ80CReadByte(0xF000, 0xF003, NULL,			RAM2+0x00F000);	// ???
   AddZ80CReadByte(0x0000, 0xFFFF, DefBadReadZ80,		NULL);		// <bad reads>
   AddZ80CReadByte(-1, -1, NULL, NULL);

   AddZ80CWriteByte(0xD000, 0xDFFF, NULL,			RAM2+0x00D000);	// WORK RAM
   AddZ80CWriteByte(0xE000, 0xEFFF, NULL,			RAM +0x00E000);	// COMMON RAM
   AddZ80CWriteByte(0xA000, 0xA000, SubBankSwitch,		NULL);		// ROM BANK
   AddZ80CWriteByte(0xB000, 0xB001, tnzs_ym2203_wb,		NULL);		// YM2203/DSW
   AddZ80CWriteByte(0xC000, 0xC001, mcu_tnzs_write,		NULL);		// INPUT
   AddZ80CWriteByte(0x0000, 0xFFFF, DefBadWriteZ80,		NULL);		// <bad writes>
   AddZ80CWriteByte(-1, -1, NULL, NULL);

   AddZ80CReadPort(0x00, 0xFF, DefBadReadZ80,			NULL);	// <bad reads>
   AddZ80CReadPort(-1, -1, NULL, NULL);

   AddZ80CWritePort(0xAA, 0xAA, StopZ80CMode2,			NULL);	// Trap Idle Z80
   AddZ80CWritePort(0x00, 0xFF, DefBadWriteZ80,			NULL);	// <bad writes>
   AddZ80CWritePort(-1, -1, NULL, NULL);

   AddZ80CInit();

   mcu_type = MCU_TNZS;

   /*-----------------------*/

   if(!(GFX=AllocateMem(0x200000))) return;
   if(!(TMP=AllocateMem(0x080000))) return;

   if(!load_rom("b53-08.8", TMP+0x00000, 0x20000)) return;
   if(!load_rom("b53-07.7", TMP+0x20000, 0x20000)) return;
   if(!load_rom("b53-06.6", TMP+0x40000, 0x20000)) return;
   if(!load_rom("b53-05.5", TMP+0x60000, 0x20000)) return;
   tb=0;
   for(ta=0;ta<0x40000;ta++){
      tc=TMP[ta];
      GFX[tb+0]=((tc&0x80)>>7)<<0;
      GFX[tb+1]=((tc&0x40)>>6)<<0;
      GFX[tb+2]=((tc&0x20)>>5)<<0;
      GFX[tb+3]=((tc&0x10)>>4)<<0;
      GFX[tb+4]=((tc&0x08)>>3)<<0;
      GFX[tb+5]=((tc&0x04)>>2)<<0;
      GFX[tb+6]=((tc&0x02)>>1)<<0;
      GFX[tb+7]=((tc&0x01)>>0)<<0;
      tc=TMP[ta+0x40000];
      GFX[tb+0]|=((tc&0x80)>>7)<<1;
      GFX[tb+1]|=((tc&0x40)>>6)<<1;
      GFX[tb+2]|=((tc&0x20)>>5)<<1;
      GFX[tb+3]|=((tc&0x10)>>4)<<1;
      GFX[tb+4]|=((tc&0x08)>>3)<<1;
      GFX[tb+5]|=((tc&0x04)>>2)<<1;
      GFX[tb+6]|=((tc&0x02)>>1)<<1;
      GFX[tb+7]|=((tc&0x01)>>0)<<1;
      tb+=16;
      if((tb&0x7F)==0){tb-=0x78;}
      else{if((tb&0x7F)==8){tb-=0x08;}}
   }
   if(!load_rom("b53-04.4", TMP+0x00000, 0x20000)) return;
   if(!load_rom("b53-03.3", TMP+0x20000, 0x20000)) return;
   if(!load_rom("b53-02.2", TMP+0x40000, 0x20000)) return;
   if(!load_rom("b53-01.1", TMP+0x60000, 0x20000)) return;
   tb=0;
   for(ta=0;ta<0x40000;ta++){
      tc=TMP[ta];
      GFX[tb+0]|=((tc&0x80)>>7)<<2;
      GFX[tb+1]|=((tc&0x40)>>6)<<2;
      GFX[tb+2]|=((tc&0x20)>>5)<<2;
      GFX[tb+3]|=((tc&0x10)>>4)<<2;
      GFX[tb+4]|=((tc&0x08)>>3)<<2;
      GFX[tb+5]|=((tc&0x04)>>2)<<2;
      GFX[tb+6]|=((tc&0x02)>>1)<<2;
      GFX[tb+7]|=((tc&0x01)>>0)<<2;
      tc=TMP[ta+0x40000];
      GFX[tb+0]|=((tc&0x80)>>7)<<3;
      GFX[tb+1]|=((tc&0x40)>>6)<<3;
      GFX[tb+2]|=((tc&0x20)>>5)<<3;
      GFX[tb+3]|=((tc&0x10)>>4)<<3;
      GFX[tb+4]|=((tc&0x08)>>3)<<3;
      GFX[tb+5]|=((tc&0x04)>>2)<<3;
      GFX[tb+6]|=((tc&0x02)>>1)<<3;
      GFX[tb+7]|=((tc&0x01)>>0)<<3;
      tb+=16;
      if((tb&0x7F)==0){tb-=0x78;}
      else{if((tb&0x7F)==8){tb-=0x08;}}
   }

   FreeMem(TMP);

   GFX_BG0_SOLID = make_solid_mask_16x16(GFX, 0x2000);

   InitPaletteMap(RAM+0xF800, 0x20, 0x10, 0x8000);

   set_colour_mapper(&col_map_xrrr_rrgg_gggb_bbbb);

   // Init seta_x1 emulation
   // ----------------------

   seta_x1.RAM_A	= RAM+0x0C000;
   seta_x1.RAM_B	= RAM+0x0F000;
   seta_x1.GFX		= GFX;
   seta_x1.MASK		= GFX_BG0_SOLID;
   seta_x1.bmp_x	= 32;
   seta_x1.bmp_y	= 32;
   seta_x1.bmp_w	= 256;
   seta_x1.bmp_h	= 224;
   seta_x1.tile_mask	= 0x1FFF;
   seta_x1.scr_x	= 0;
   seta_x1.scr_y	= 0;
}

void ClearTNZS(void)
{
#ifdef RAINE_DEBUG
      save_debug("RAM.BIN", RAM, RAMSize, 0);
      save_debug("GFX.BIN", GFX, 0x200000, 0);
#endif
}

void LoadDrToppel(void)
{
   int ta,tb,tc;
   UINT8 *RAM_COLOUR, *TMP;

   romset=9;

   RAMSize=0x10000+0x10000+0x10+(0xC000*2)+0x400;

   if(!(ROM =AllocateMem(0xC000*6))) return;

   if(!(RAM=AllocateMem(RAMSize))) return;

   RAM2     = RAM+0x10000;
   RAM_BANK = RAM+0x10000+0x10000+0x10;
   RAM_COLOUR = RAM+0x10000+0x10000+0x10+(0xC000*2);

   memset(RAM+0x00000, 0x00, RAMSize);

   if(!(TMP=AllocateMem(0x20000))) return;

   if(!load_rom("b19-09.bin", TMP+0x00000, 0x10000)) return;	// Z80 MAIN ROM
   if(!load_rom("b19-10.bin", TMP+0x10000, 0x10000)) return;	// Z80 MAIN ROM

   // Patch RAM check out (this is too weird...)
   // When CPU0 starts up, it checks/zeros all the RAM. Fine, except that
   // CPU1 has previously put it's DSW info in there, and CPU0 goes and
   // spews all over it. This messes the game up.
   // So, we just remove the RAM check (seems the easiest way)
   TMP[0x2f03] = 0x3E; // LD A,0x00
   TMP[0x2f04] = 0x00;
   TMP[0x2f05] = 0x32; // LD (0xE0E4), A
   TMP[0x2f06] = 0xE4; // (return the success code)
   TMP[0x2f07] = 0xE0;
   TMP[0x2f08] = 0xC3; // JP 0x0070
   TMP[0x2f09] = 0x70;
   TMP[0x2f0A] = 0x00;

   init_bank_rom(TMP,ROM);

   memcpy(RAM,TMP,0xC000);
   AddZ80BROMBase(RAM, 0x0038, 0x0066);

   AddZ80BReadByte(0x0000, 0xBFFF, NULL,			NULL);		// BANK ROM/RAM
   AddZ80BReadByte(0xC000, 0xF3FF, NULL,			RAM+0x00C000);	// OBJECT RAM/WORK RAM/VDC RAM
   AddZ80BReadByte(0xF800, 0xFBFF, NULL,			RAM+0x00F800);	// COLOR RAM
 //AddZ80BReadByte(0xF600, 0xF600, main_bank_rb,		NULL);		// ROM BANK
   AddZ80BReadByte(0x0000, 0xFFFF, DefBadReadZ80,		NULL);		// <bad reads>
   AddZ80BReadByte(-1, -1, NULL, NULL);

   AddZ80BWriteByte(0x8000, 0xBFFF, NULL,			NULL);		// BANK RAM
   AddZ80BWriteByte(0xC000, 0xF3FF, NULL,			RAM+0x00C000);	// OBJECT RAM/WORK RAM/VDC RAM
   AddZ80BWriteByte(0xF800, 0xFBFF, NULL,			RAM+0x00F800);	// COLOR RAM
   AddZ80BWriteByte(0xF600, 0xF600, MainBankSwitch,		NULL);		// ROM BANK
   AddZ80BWriteByte(0x0000, 0xFFFF, DefBadWriteZ80,		NULL);		// <bad writes>
   AddZ80BWriteByte(-1, -1, NULL, NULL);

   AddZ80BReadPort(0x00, 0xFF, DefBadReadZ80,			NULL);		// <bad reads>
   AddZ80BReadPort(-1, -1, NULL, NULL);

   AddZ80BWritePort(0xAA, 0xAA, StopZ80BMode2,			NULL);		// Trap Idle Z80
   AddZ80BWritePort(0x00, 0xFF, DefBadWriteZ80,			NULL);		// <bad reads>
   AddZ80BWritePort(-1, -1, NULL, NULL);

   AddZ80BInit();

   if(!load_rom("b19-11w.bin", TMP, 0x10000)) return;      // Z80 SUB ROM

   // Skip Idle Z80
   // -------------

   TMP[0x017B]=0xD3;  // OUTA (AAh)
   TMP[0x017C]=0xAA;  //
   SetStopZ80CMode2(0x0175);

   init_bank_rom2(TMP);

   memcpy(RAM2,TMP,0x10000);
   AddZ80CROMBase(RAM2, 0x0038, 0x0066);

   AddZ80CReadByte(0x0000, 0x9FFF, NULL,			NULL);		// BANK ROM
   AddZ80CReadByte(0xD000, 0xDFFF, NULL,			RAM2+0x00D000);	// WORK RAM
   AddZ80CReadByte(0xE000, 0xEFFF, NULL,                        RAM +0x00E000);   // COMMON RAM
   AddZ80CReadByte(0xB000, 0xB001, tnzs_ym2203_rb,              NULL);      // YM2203/DSW
   AddZ80CReadByte(0xC000, 0xC001, mcu_tnzs_read,            NULL);      // INPUT
   AddZ80CReadByte(0xF000, 0xF003, NULL,                        RAM2+0x00F000);   // ???
   AddZ80CReadByte(0x0000, 0xFFFF, DefBadReadZ80,               NULL);      // <bad reads>
   AddZ80CReadByte(-1, -1, NULL, NULL);

   AddZ80CWriteByte(0xD000, 0xDFFF, NULL,			RAM2+0x00D000);	// WORK RAM
   AddZ80CWriteByte(0xE000, 0xEFFF, NULL,			RAM +0x00E000);	// COMMON RAM
   AddZ80CWriteByte(0xA000, 0xA000, SubBankSwitch,		NULL);		// ROM BANK
   AddZ80CWriteByte(0xB000, 0xB001, tnzs_ym2203_wb,             NULL);      // YM2203/DSW
   AddZ80CWriteByte(0xC000, 0xC001, mcu_tnzs_write,          NULL);      // INPUT
   AddZ80CWriteByte(0x0000, 0xFFFF, DefBadWriteZ80,		NULL);		// <bad writes>
   AddZ80CWriteByte(-1, -1, NULL, NULL);

   AddZ80CReadPort(0x00, 0xFF, DefBadReadZ80,			NULL);	// <bad reads>
   AddZ80CReadPort(-1, -1, NULL, NULL);

   AddZ80CWritePort(0xAA, 0xAA, StopZ80CMode2,			NULL);	// Trap Idle Z80
   AddZ80CWritePort(0x00, 0xFF, DefBadWriteZ80,			NULL);	// <bad writes>
   AddZ80CWritePort(-1, -1, NULL, NULL);

   AddZ80CInit();

   FreeMem(TMP);
   SubBankSwitch(0,0x10);
   mcu_type = MCU_DRTOPPEL;

   /*-----------------------*/

   if(!(GFX=AllocateMem(0x200000))) return;
   if(!(TMP=AllocateMem(0x080000))) return;

   if(!load_rom("b19-01.bin", TMP+0x00000, 0x20000)) return;
   if(!load_rom("b19-02.bin", TMP+0x20000, 0x20000)) return;
   if(!load_rom("b19-03.bin", TMP+0x40000, 0x20000)) return;
   if(!load_rom("b19-04.bin", TMP+0x60000, 0x20000)) return;
   tb=0;
   for(ta=0;ta<0x40000;ta++){
      tc=TMP[ta];
      GFX[tb+0]=((tc&0x80)>>7)<<0;
      GFX[tb+1]=((tc&0x40)>>6)<<0;
      GFX[tb+2]=((tc&0x20)>>5)<<0;
      GFX[tb+3]=((tc&0x10)>>4)<<0;
      GFX[tb+4]=((tc&0x08)>>3)<<0;
      GFX[tb+5]=((tc&0x04)>>2)<<0;
      GFX[tb+6]=((tc&0x02)>>1)<<0;
      GFX[tb+7]=((tc&0x01)>>0)<<0;
      tc=TMP[ta+0x40000];
      GFX[tb+0]|=((tc&0x80)>>7)<<1;
      GFX[tb+1]|=((tc&0x40)>>6)<<1;
      GFX[tb+2]|=((tc&0x20)>>5)<<1;
      GFX[tb+3]|=((tc&0x10)>>4)<<1;
      GFX[tb+4]|=((tc&0x08)>>3)<<1;
      GFX[tb+5]|=((tc&0x04)>>2)<<1;
      GFX[tb+6]|=((tc&0x02)>>1)<<1;
      GFX[tb+7]|=((tc&0x01)>>0)<<1;
      tb+=16;
      if((tb&0x7F)==0){tb-=0x78;}
      else{if((tb&0x7F)==8){tb-=0x08;}}
   }
   if(!load_rom("b19-05.bin", TMP+0x00000, 0x20000)) return;
   if(!load_rom("b19-06.bin", TMP+0x20000, 0x20000)) return;
   if(!load_rom("b19-07.bin", TMP+0x40000, 0x20000)) return;
   if(!load_rom("b19-08.bin", TMP+0x60000, 0x20000)) return;
   tb=0;
   for(ta=0;ta<0x40000;ta++){
      tc=TMP[ta];
      GFX[tb+0]|=((tc&0x80)>>7)<<2;
      GFX[tb+1]|=((tc&0x40)>>6)<<2;
      GFX[tb+2]|=((tc&0x20)>>5)<<2;
      GFX[tb+3]|=((tc&0x10)>>4)<<2;
      GFX[tb+4]|=((tc&0x08)>>3)<<2;
      GFX[tb+5]|=((tc&0x04)>>2)<<2;
      GFX[tb+6]|=((tc&0x02)>>1)<<2;
      GFX[tb+7]|=((tc&0x01)>>0)<<2;
      tc=TMP[ta+0x40000];
      GFX[tb+0]|=((tc&0x80)>>7)<<3;
      GFX[tb+1]|=((tc&0x40)>>6)<<3;
      GFX[tb+2]|=((tc&0x20)>>5)<<3;
      GFX[tb+3]|=((tc&0x10)>>4)<<3;
      GFX[tb+4]|=((tc&0x08)>>3)<<3;
      GFX[tb+5]|=((tc&0x04)>>2)<<3;
      GFX[tb+6]|=((tc&0x02)>>1)<<3;
      GFX[tb+7]|=((tc&0x01)>>0)<<3;
      tb+=16;
      if((tb&0x7F)==0){tb-=0x78;}
      else{if((tb&0x7F)==8){tb-=0x08;}}
   }

   tb=0;
   if(!load_rom("b19-12.bin", TMP+0x00000, 0x200)) return;
   if(!load_rom("b19-13.bin", TMP+0x00200, 0x200)) return;
   for(ta=0;ta<0x200;ta++,tb+=2){
      tc  =  TMP[ta+0x000];
      tc |= (TMP[ta+0x200]) << 8;
      WriteWord(&RAM_COLOUR[tb], tc);
   }
   FreeMem(TMP);

   GFX_BG0_SOLID = make_solid_mask_16x16(GFX, 0x2000);

   InitPaletteMap(RAM_COLOUR, 0x20, 0x10, 0x8000);

   set_colour_mapper(&col_map_xrrr_rrgg_gggb_bbbb);

   // Init seta_x1 emulation
   // ----------------------

   seta_x1.RAM_A	= RAM+0x0C000;
   seta_x1.RAM_B	= RAM+0x0F000;
   seta_x1.GFX		= GFX;
   seta_x1.MASK		= GFX_BG0_SOLID;
   seta_x1.bmp_x	= 32;
   seta_x1.bmp_y	= 32;
   seta_x1.bmp_w	= 256;
   seta_x1.bmp_h	= 224;
   seta_x1.tile_mask    = 0x1FFF;
   seta_x1.scr_x	= 0;
   seta_x1.scr_y	= 0;
}

void ClearDrToppel(void)
{
#ifdef RAINE_DEBUG
      save_debug("RAM.BIN", RAM, RAMSize, 0);
#endif
}

void LoadTNZSBL(void)
{
   int ta,tb,tc;
   UINT8 *TMP;

   romset=6;

   RAMSize=0x10000+0x10000+0x10+(0xC000*2)+0x10000;

   if(!(ROM =AllocateMem(0xC000*6))) return;

   if(!(RAM=AllocateMem(RAMSize))) return;

   RAM2     = RAM+0x10000;
   RAM3     = RAM+0x10000+0x10000+0x10+(0xC000*2);
   RAM_BANK = RAM+0x10000+0x10000+0x10;

   memset(RAM+0x00000, 0x00, RAMSize);

   if(!(TMP=AllocateMem(0x20000))) return;

   if(!load_rom("nzsb5326.bin", TMP, 0x10000)) return;	// EXTRA SOUND Z80

   TMP[0x0072]=0x00;
   TMP[0x0073]=0x00;

   // Skip Idle Z80
   // -------------

   //TMP[0x0CB4]=0xC9;  // ret

   TMP[0x0172]=0xD3;  // OUTA (AAh)
   TMP[0x0173]=0xAA;  //

   SetStopZ80Mode2(0x016C);

   memcpy(RAM3,TMP,0x10000);
   AddZ80AROMBase(RAM3, 0x0038, 0x0066);

   AddZ80AReadByte(0x0000, 0x7FFF, NULL,			RAM3+0x0000);	// ROM
   AddZ80AReadByte(0xC000, 0xDFFF, NULL,			RAM3+0xC000);	// RAM
   AddZ80AReadByte(0x0000, 0xFFFF, DefBadReadZ80,		NULL);		// <bad reads>
   AddZ80AReadByte(-1, -1, NULL, NULL);

   AddZ80AWriteByte(0xC000, 0xDFFF, NULL,			RAM3+0xC000);	// RAM
   AddZ80AWriteByte(0x0000, 0xFFFF, DefBadWriteZ80,		NULL);		// <bad writes>
   AddZ80AWriteByte(-1, -1, NULL, NULL);

   AddZ80AReadPort(0x00, 0x01, tnzs_ym2203_rb,	        NULL);		// YM2203
   AddZ80AReadPort(0x02, 0x02, tnzs_b1_sound_rb,	        NULL);		// YM2203
   AddZ80AReadPort(0x00, 0xFF, DefBadReadZ80,			NULL);		// <bad reads>
   AddZ80AReadPort(-1, -1, NULL, NULL);

   AddZ80AWritePort(0x00, 0x01, tnzs_ym2203_wb,	        NULL);		// YM2203
   AddZ80AWritePort(0xAA, 0xAA, StopZ80Mode2,			NULL);		// Trap Idle Z80
   AddZ80AWritePort(0x00, 0xFF, DefBadWriteZ80,			NULL);		// <bad reads>
   AddZ80AWritePort(-1, -1, NULL, NULL);

   AddZ80AInit();

   if(!load_rom("nzsb5324.bin", TMP, 0x20000)) return;	// Z80 MAIN ROM

   // Skip Idle Z80
   // -------------

   TMP[0x0CB4]=0xC9;  // ret

   TMP[0x02D8]=0xD3;  // OUTA (AAh)
   TMP[0x02D9]=0xAA;  //

   SetStopZ80BMode2(0x02CF);

   init_bank_rom(TMP,ROM);

   memcpy(RAM,TMP,0x10000);
   AddZ80BROMBase(RAM, 0x0038, 0x0066);

   AddZ80BReadByte(0x0000, 0xBFFF, NULL,			NULL);		// BANK ROM/RAM
   AddZ80BReadByte(0xC000, 0xF3FF, NULL,			RAM+0x00C000);	// OBJECT RAM/WORK RAM/VDC RAM
   AddZ80BReadByte(0xF800, 0xFBFF, NULL,			RAM+0x00F800);	// COLOR RAM
   AddZ80BReadByte(0x0000, 0xFFFF, DefBadReadZ80,		NULL);		// <bad reads>
   AddZ80BReadByte(-1, -1, NULL, NULL);

   AddZ80BWriteByte(0x8000, 0xBFFF, NULL,			NULL);		// BANK RAM
   AddZ80BWriteByte(0xC000, 0xF3FF, NULL,			RAM+0x00C000);	// OBJECT RAM/WORK RAM/VDC RAM
   AddZ80BWriteByte(0xF800, 0xFBFF, NULL,			RAM+0x00F800);	// COLOR RAM
   AddZ80BWriteByte(0xF600, 0xF600, MainBankSwitch,		NULL);		// ROM BANK
   AddZ80BWriteByte(0x0000, 0xFFFF, DefBadWriteZ80,		NULL);		// <bad writes>
   AddZ80BWriteByte(-1, -1, NULL, NULL);

   AddZ80BReadPort(0x00, 0xFF, DefBadReadZ80,			NULL);		// <bad reads>
   AddZ80BReadPort(-1, -1, NULL, NULL);

   AddZ80BWritePort(0xAA, 0xAA, StopZ80BMode2,			NULL);		// Trap Idle Z80
   AddZ80BWritePort(0x00, 0xFF, DefBadWriteZ80,			NULL);		// <bad reads>
   AddZ80BWritePort(-1, -1, NULL, NULL);

   AddZ80BInit();

   if(!load_rom("nzsb5325.bin", TMP, 0x10000)) return;		// Z80 SUB ROM

   // Skip Idle Z80
   // -------------

   TMP[0x068C]=0xC9;  // ret

   TMP[0x0016]=0xD3;  // OUTA (AAh)
   TMP[0x0017]=0xAA;  //

   SetStopZ80CMode2(0x0013);

   init_bank_rom2(TMP);

   memcpy(RAM2,TMP,0x10000);
   AddZ80CROMBase(RAM2, 0x0038, 0x0066);

   AddZ80CReadByte(0x0000, 0x9FFF, NULL,			NULL);		// BANK ROM
   AddZ80CReadByte(0xD000, 0xDFFF, NULL,			RAM2+0x00D000);	// WORK RAM
   AddZ80CReadByte(0xE000, 0xEFFF, NULL,			RAM +0x00E000);	// COMMON RAM
   AddZ80CReadByte(0xF000, 0xF3FF, NULL,			RAM+0x00F800);	// COLOR RAM
   AddZ80CReadByte(0xB002, 0xB003, TNZSSystemDSWRead,	        NULL);		// DSW
   AddZ80CReadByte(0xC000, 0xC002, TNZSSystemInputRead,		NULL);		// INPUT
   AddZ80CReadByte(0x0000, 0xFFFF, DefBadReadZ80,		NULL);		// <bad reads>
   AddZ80CReadByte(-1, -1, NULL, NULL);

   AddZ80CWriteByte(0xD000, 0xDFFF, NULL,			RAM2+0x00D000);	// WORK RAM
   AddZ80CWriteByte(0xE000, 0xEFFF, NULL,			RAM +0x00E000);	// COMMON RAM
   AddZ80CWriteByte(0xF000, 0xF3FF, NULL,			RAM+0x00F800);	// COLOR RAM
   AddZ80CWriteByte(0xA000, 0xA000, SubBankSwitch_alt,		NULL);		// ROM BANK
   AddZ80CWriteByte(0xB004, 0xB004, tnzs_b1_sound_wb,	        NULL);		// IO
   AddZ80CWriteByte(0x0000, 0xFFFF, DefBadWriteZ80,		NULL);		// <bad writes>
   AddZ80CWriteByte(-1, -1, NULL, NULL);

   AddZ80CReadPort(0x00, 0xFF, DefBadReadZ80,			NULL);	// <bad reads>
   AddZ80CReadPort(-1, -1, NULL, NULL);

   AddZ80CWritePort(0xAA, 0xAA, StopZ80CMode2,			NULL);	// Trap Idle Z80
   AddZ80CWritePort(0x00, 0xFF, DefBadWriteZ80,			NULL);	// <bad writes>
   AddZ80CWritePort(-1, -1, NULL, NULL);

   AddZ80CInit();

   FreeMem(TMP);

   /*-----------------------*/

   if(!(GFX=AllocateMem(0x200000))) return;
   if(!(TMP=AllocateMem(0x080000))) return;

   if(!load_rom("b53-08.8", TMP+0x00000, 0x20000)) return;
   if(!load_rom("b53-07.7", TMP+0x20000, 0x20000)) return;
   if(!load_rom("b53-06.6", TMP+0x40000, 0x20000)) return;
   if(!load_rom("b53-05.5", TMP+0x60000, 0x20000)) return;
   tb=0;
   for(ta=0;ta<0x40000;ta++){
      tc=TMP[ta];
      GFX[tb+0]=((tc&0x80)>>7)<<0;
      GFX[tb+1]=((tc&0x40)>>6)<<0;
      GFX[tb+2]=((tc&0x20)>>5)<<0;
      GFX[tb+3]=((tc&0x10)>>4)<<0;
      GFX[tb+4]=((tc&0x08)>>3)<<0;
      GFX[tb+5]=((tc&0x04)>>2)<<0;
      GFX[tb+6]=((tc&0x02)>>1)<<0;
      GFX[tb+7]=((tc&0x01)>>0)<<0;
      tc=TMP[ta+0x40000];
      GFX[tb+0]|=((tc&0x80)>>7)<<1;
      GFX[tb+1]|=((tc&0x40)>>6)<<1;
      GFX[tb+2]|=((tc&0x20)>>5)<<1;
      GFX[tb+3]|=((tc&0x10)>>4)<<1;
      GFX[tb+4]|=((tc&0x08)>>3)<<1;
      GFX[tb+5]|=((tc&0x04)>>2)<<1;
      GFX[tb+6]|=((tc&0x02)>>1)<<1;
      GFX[tb+7]|=((tc&0x01)>>0)<<1;
      tb+=16;
      if((tb&0x7F)==0){tb-=0x78;}
      else{if((tb&0x7F)==8){tb-=0x08;}}
   }
   if(!load_rom("b53-04.4", TMP+0x00000, 0x20000)) return;
   if(!load_rom("b53-03.3", TMP+0x20000, 0x20000)) return;
   if(!load_rom("b53-02.2", TMP+0x40000, 0x20000)) return;
   if(!load_rom("b53-01.1", TMP+0x60000, 0x20000)) return;
   tb=0;
   for(ta=0;ta<0x40000;ta++){
      tc=TMP[ta];
      GFX[tb+0]|=((tc&0x80)>>7)<<2;
      GFX[tb+1]|=((tc&0x40)>>6)<<2;
      GFX[tb+2]|=((tc&0x20)>>5)<<2;
      GFX[tb+3]|=((tc&0x10)>>4)<<2;
      GFX[tb+4]|=((tc&0x08)>>3)<<2;
      GFX[tb+5]|=((tc&0x04)>>2)<<2;
      GFX[tb+6]|=((tc&0x02)>>1)<<2;
      GFX[tb+7]|=((tc&0x01)>>0)<<2;
      tc=TMP[ta+0x40000];
      GFX[tb+0]|=((tc&0x80)>>7)<<3;
      GFX[tb+1]|=((tc&0x40)>>6)<<3;
      GFX[tb+2]|=((tc&0x20)>>5)<<3;
      GFX[tb+3]|=((tc&0x10)>>4)<<3;
      GFX[tb+4]|=((tc&0x08)>>3)<<3;
      GFX[tb+5]|=((tc&0x04)>>2)<<3;
      GFX[tb+6]|=((tc&0x02)>>1)<<3;
      GFX[tb+7]|=((tc&0x01)>>0)<<3;
      tb+=16;
      if((tb&0x7F)==0){tb-=0x78;}
      else{if((tb&0x7F)==8){tb-=0x08;}}
   }

   FreeMem(TMP);

   GFX_BG0_SOLID = make_solid_mask_16x16(GFX, 0x2000);

   InitPaletteMap(RAM+0xF800, 0x20, 0x10, 0x8000);

   set_colour_mapper(&col_map_xrrr_rrgg_gggb_bbbb);

   // Init seta_x1 emulation
   // ----------------------

   seta_x1.RAM_A	= RAM+0x0C000;
   seta_x1.RAM_B	= RAM+0x0F000;
   seta_x1.GFX		= GFX;
   seta_x1.MASK		= GFX_BG0_SOLID;
   seta_x1.bmp_x	= 32;
   seta_x1.bmp_y	= 32;
   seta_x1.bmp_w	= 256;
   seta_x1.bmp_h	= 224;
   seta_x1.tile_mask	= 0x1FFF;
   seta_x1.scr_x	= 0;
   seta_x1.scr_y	= 0;
}

void ClearTNZSBL(void)
{
#ifdef RAINE_DEBUG
      save_debug("RAM.BIN", RAM, RAMSize, 0);
      save_debug("GFX.BIN", GFX, 0x200000, 0);
#endif
}

void ClearArk2DohUS(void)
{
#ifdef RAINE_DEBUG
      save_debug("RAM.BIN", RAM, RAMSize, 0);
      save_debug("GFX.BIN", GFX, 0x200000, 0);
#endif
}

void LoadExtermination(void)
{
   int ta,tb,tc;
   UINT8 *TMP;

   romset=5;

   RAMSize=0x10000+0x10000+0x10+(0xC000*2);

   if(!(ROM =AllocateMem(0xC000*6))) return;

   if(!(RAM=AllocateMem(RAMSize))) return;

   RAM2     = RAM+0x10000;
   RAM_BANK = RAM+0x10000+0x10000+0x10;

   memset(RAM+0x00000, 0x00, RAMSize);

   if(!(TMP=AllocateMem(0x20000))) return;

   if(!load_rom("b06-20.bin", TMP+0x00000, 0x10000)) return;	// Z80 MAIN ROM
   if(!load_rom("b06-21.bin", TMP+0x10000, 0x10000)) return;	// Z80 MAIN ROM

   // Fix SUB WAIT

   TMP[0x02d2]=0x00;
   TMP[0x02d3]=0x00;

   // Fix HW ERROR

   TMP[0x032b]=0xC9;

   // Fix ROM ERROR

   TMP[0x08dc]=0xC9;

   // Skip Idle Z80

   TMP[0x4219]=0xD3;  // OUTA (AAh)
   TMP[0x421A]=0xAA;  //

   SetStopZ80BMode2(0x4219);

   init_bank_rom(TMP,ROM);

   memcpy(RAM,TMP,0x10000);
   AddZ80BROMBase(RAM, 0x0038, 0x0066);

   AddZ80BReadByte(0x0000, 0xBFFF, NULL,			NULL);		// BANK ROM/RAM
   AddZ80BReadByte(0xC000, 0xF3FF, NULL,			RAM+0x00C000);	// OBJECT RAM/WORK RAM/VDC RAM
   AddZ80BReadByte(0xF800, 0xFBFF, NULL,			RAM+0x00F800);	// COLOR RAM
   AddZ80BReadByte(0x0000, 0xFFFF, DefBadReadZ80,		NULL);		// <bad reads>
   AddZ80BReadByte(-1, -1, NULL, NULL);

   AddZ80BWriteByte(0x8000, 0xBFFF, NULL,			NULL);		// BANK RAM
   AddZ80BWriteByte(0xC000, 0xF3FF, NULL,			RAM+0x00C000);	// OBJECT RAM/WORK RAM/VDC RAM
   AddZ80BWriteByte(0xF800, 0xFBFF, NULL,			RAM+0x00F800);	// COLOR RAM
   AddZ80BWriteByte(0xF600, 0xF600, MainBankSwitch,		NULL);		// ROM BANK
   AddZ80BWriteByte(0x0000, 0xFFFF, DefBadWriteZ80,		NULL);		// <bad writes>
   AddZ80BWriteByte(-1, -1, NULL, NULL);

   AddZ80BReadPort(0x00, 0xFF, DefBadReadZ80,			NULL);		// <bad reads>
   AddZ80BReadPort(-1, -1, NULL, NULL);

   AddZ80BWritePort(0xAA, 0xAA, StopZ80BMode2,			NULL);		// Trap Idle Z80
   AddZ80BWritePort(0x00, 0xFF, DefBadWriteZ80,			NULL);		// <bad reads>
   AddZ80BWritePort(-1, -1, NULL, NULL);

   AddZ80BInit();

   if(!load_rom("b06-06.bin", TMP, 0x10000)) return;		// Z80 SUB ROM

   // Skip Idle Z80

   TMP[0x0140]=0xD3;  // OUTA (AAh)
   TMP[0x0141]=0xAA;  //

   SetStopZ80CMode2(0x013B);

   init_bank_rom2(TMP);

   memcpy(RAM2,TMP,0x10000);
   AddZ80CROMBase(RAM2, 0x0038, 0x0066);

   AddZ80CReadByte(0x0000, 0x9FFF, NULL,			NULL);		// BANK ROM
   AddZ80CReadByte(0xD000, 0xDFFF, NULL,			RAM2+0x00D000);	// WORK RAM
   AddZ80CReadByte(0xE000, 0xEFFF, NULL,			RAM +0x00E000);	// COMMON RAM
   AddZ80CReadByte(0xB000, 0xB001, tnzs_ym2203_rb,		NULL);		// YM2203/DSW
   AddZ80CReadByte(0xC000, 0xC001, mcu_tnzs_read,		NULL);		// INPUT
   AddZ80CReadByte(0xF000, 0xF003, NULL,			RAM2+0x00F000);	// ???
   AddZ80CReadByte(0x0000, 0xFFFF, DefBadReadZ80,		NULL);		// <bad reads>
   AddZ80CReadByte(-1, -1, NULL, NULL);

   AddZ80CWriteByte(0xD000, 0xDFFF, NULL,			RAM2+0x00D000);	// WORK RAM
   AddZ80CWriteByte(0xE000, 0xEFFF, NULL,			RAM +0x00E000);	// COMMON RAM
   AddZ80CWriteByte(0xA000, 0xA000, SubBankSwitch,		NULL);		// ROM BANK
   AddZ80CWriteByte(0xB000, 0xB001, tnzs_ym2203_wb,		NULL);		// YM2203/DSW
   AddZ80CWriteByte(0xC000, 0xC001, mcu_tnzs_write,		NULL);		// INPUT
   AddZ80CWriteByte(0x0000, 0xFFFF, DefBadWriteZ80,		NULL);		// <bad writes>
   AddZ80CWriteByte(-1, -1, NULL, NULL);

   AddZ80CReadPort(0x00, 0xFF, DefBadReadZ80,			NULL);		// <bad reads>
   AddZ80CReadPort(-1, -1, NULL, NULL);

   AddZ80CWritePort(0xAA, 0xAA, StopZ80CMode2,			NULL);		// Trap Idle Z80
   AddZ80CWritePort(0x00, 0xFF, DefBadWriteZ80,			NULL);		// <bad writes>
   AddZ80CWritePort(-1, -1, NULL, NULL);

   AddZ80CInit();

   FreeMem(TMP);
   mcu_type = MCU_EXTRMATN;

   /*-----------------------*/

   if(!(GFX=AllocateMem(0x200000))) return;
   if(!(TMP=AllocateMem(0x080000))) return;

   if(!load_rom("b06-08.bin", TMP+0x000, 0x200)) return;	// COLOR ROM
   if(!load_rom("b06-09.bin", TMP+0x200, 0x200)) return;	// COLOR ROM
   for(ta=0;ta<512;ta++){
   RAM[ta+ta+0xF800]=TMP[ta+0x000];
   RAM[ta+ta+0xF801]=TMP[ta+0x200];
   }

   if(!load_rom("b06-01.bin", TMP+0x00000, 0x20000)) return;
   if(!load_rom("b06-02.bin", TMP+0x20000, 0x20000)) return;
   tb=0;
   for(ta=0;ta<0x20000;ta++){
      tc=TMP[ta];
      GFX[tb+0]=((tc&0x80)>>7)<<0;
      GFX[tb+1]=((tc&0x40)>>6)<<0;
      GFX[tb+2]=((tc&0x20)>>5)<<0;
      GFX[tb+3]=((tc&0x10)>>4)<<0;
      GFX[tb+4]=((tc&0x08)>>3)<<0;
      GFX[tb+5]=((tc&0x04)>>2)<<0;
      GFX[tb+6]=((tc&0x02)>>1)<<0;
      GFX[tb+7]=((tc&0x01)>>0)<<0;
      tc=TMP[ta+0x20000];
      GFX[tb+0]|=((tc&0x80)>>7)<<1;
      GFX[tb+1]|=((tc&0x40)>>6)<<1;
      GFX[tb+2]|=((tc&0x20)>>5)<<1;
      GFX[tb+3]|=((tc&0x10)>>4)<<1;
      GFX[tb+4]|=((tc&0x08)>>3)<<1;
      GFX[tb+5]|=((tc&0x04)>>2)<<1;
      GFX[tb+6]|=((tc&0x02)>>1)<<1;
      GFX[tb+7]|=((tc&0x01)>>0)<<1;
      tb+=16;
      if((tb&0x7F)==0){tb-=0x78;}
      else{if((tb&0x7F)==8){tb-=0x08;}}
   }
   if(!load_rom("b06-03.bin", TMP+0x00000, 0x20000)) return;
   if(!load_rom("b06-04.bin", TMP+0x20000, 0x20000)) return;
   tb=0;
   for(ta=0;ta<0x20000;ta++){
      tc=TMP[ta];
      GFX[tb+0]|=((tc&0x80)>>7)<<2;
      GFX[tb+1]|=((tc&0x40)>>6)<<2;
      GFX[tb+2]|=((tc&0x20)>>5)<<2;
      GFX[tb+3]|=((tc&0x10)>>4)<<2;
      GFX[tb+4]|=((tc&0x08)>>3)<<2;
      GFX[tb+5]|=((tc&0x04)>>2)<<2;
      GFX[tb+6]|=((tc&0x02)>>1)<<2;
      GFX[tb+7]|=((tc&0x01)>>0)<<2;
      tc=TMP[ta+0x20000];
      GFX[tb+0]|=((tc&0x80)>>7)<<3;
      GFX[tb+1]|=((tc&0x40)>>6)<<3;
      GFX[tb+2]|=((tc&0x20)>>5)<<3;
      GFX[tb+3]|=((tc&0x10)>>4)<<3;
      GFX[tb+4]|=((tc&0x08)>>3)<<3;
      GFX[tb+5]|=((tc&0x04)>>2)<<3;
      GFX[tb+6]|=((tc&0x02)>>1)<<3;
      GFX[tb+7]|=((tc&0x01)>>0)<<3;
      tb+=16;
      if((tb&0x7F)==0){tb-=0x78;}
      else{if((tb&0x7F)==8){tb-=0x08;}}
   }

   FreeMem(TMP);

   GFX_BG0_SOLID = make_solid_mask_16x16(GFX, 0x2000);

   InitPaletteMap(RAM+0xF800, 0x20, 0x10, 0x8000);

   set_colour_mapper(&col_map_xrrr_rrgg_gggb_bbbb);

   // Init seta_x1 emulation
   // ----------------------

   seta_x1.RAM_A	= RAM+0x0C000;
   seta_x1.RAM_B	= RAM+0x0F000;
   seta_x1.GFX		= GFX;
   seta_x1.MASK		= GFX_BG0_SOLID;
   seta_x1.bmp_x	= 32;
   seta_x1.bmp_y	= 32;
   seta_x1.bmp_w	= 224;
   seta_x1.bmp_h	= 256;
   seta_x1.tile_mask	= 0x1FFF;
   seta_x1.scr_x	= 0;
   seta_x1.scr_y	= 0;
}

void ClearExtermination(void)
{
#ifdef RAINE_DEBUG
      save_debug("RAM.BIN", RAM, RAMSize, 0);
      save_debug("GFX.BIN", GFX, 0x200000, 0);
#endif
}

static UINT16 f6read(UINT16 offset) {
  return 0xff;
}

void load_kageki(void)
{
   int ta,tb,tc;
   UINT8 *TMP;

   romset=7;

   RAMSize=0x10000+0x10000+0x10+(0xC000*2);

   if(!(ROM =AllocateMem(0xC000*6))) return;

   if(!(RAM=AllocateMem(RAMSize))) return;

   RAM2     = RAM+0x10000;
   RAM_BANK = RAM+0x10000+0x10000+0x10;

   memset(RAM+0x00000, 0x00, RAMSize);

   if(!(TMP=AllocateMem(0x20000))) return;

   if(!load_rom("b35-16.11c", TMP+0x00000, 0x10000)) return;	// Z80 MAIN ROM
   if(!load_rom("b35-10.9c",  TMP+0x10000, 0x10000)) return;	// Z80 MAIN ROM

/*
   TMP[0x4219]=0xD3;  // OUTA (AAh)
   TMP[0x421A]=0xAA;  //

   SetStopZ80BMode2(0x4219);
*/
   init_bank_rom(TMP,ROM);

   memcpy(RAM,TMP,0x10000);
   AddZ80BROMBase(RAM, 0x0038, 0x0066);

   AddZ80BReadByte(0x0000, 0xBFFF, NULL,			NULL);		// BANK ROM/RAM
   AddZ80BReadByte(0xC000, 0xF3FF, NULL,			RAM+0x00C000);	// OBJECT RAM/WORK RAM/VDC RAM
   AddZ80BReadByte(0xF800, 0xFBFF, NULL,			RAM+0x00F800);	// COLOR RAM
   AddZ80BReadByte(0xf600, 0xf600, f6read, NULL);

   AddZ80BReadByte(0x0000, 0xFFFF, DefBadReadZ80,		NULL);		// <bad reads>
   AddZ80BReadByte(-1, -1, NULL, NULL);

   AddZ80BWriteByte(0x8000, 0xBFFF, NULL,			NULL);		// BANK RAM
   AddZ80BWriteByte(0xC000, 0xF3FF, NULL,			RAM+0x00C000);	// OBJECT RAM/WORK RAM/VDC RAM
   AddZ80BWriteByte(0xF800, 0xFBFF, NULL,			RAM+0x00F800);	// COLOR RAM
   AddZ80BWriteByte(0xF600, 0xF600, MainBankSwitch,		NULL);		// ROM BANK
   AddZ80BWriteByte(0x0000, 0xFFFF, DefBadWriteZ80,		NULL);		// <bad writes>
   AddZ80BWriteByte(-1, -1, NULL, NULL);

   AddZ80BReadPort(0x00, 0xFF, DefBadReadZ80,			NULL);		// <bad reads>
   AddZ80BReadPort(-1, -1, NULL, NULL);

   AddZ80BWritePort(0xAA, 0xAA, StopZ80BMode2,			NULL);		// Trap Idle Z80
   AddZ80BWritePort(0x00, 0xFF, DefBadWriteZ80,			NULL);		// <bad reads>
   AddZ80BWritePort(-1, -1, NULL, NULL);

   AddZ80BInit();

   if(!load_rom("b35-17.43e", TMP, 0x10000)) return;		// Z80 SUB ROM

   // fix i/o error

/*     TMP[0x0096]=0x00; */
/*     TMP[0x0097]=0x00; */
/*     TMP[0x0098]=0x00; */

   // speed hack

   TMP[0x0109]=0xD3;
   TMP[0x010A]=0xAA;

   SetStopZ80CMode2(0x0108);

   init_bank_rom2(TMP);

   memcpy(RAM2,TMP,0x10000);
   AddZ80CROMBase(RAM2, 0x0038, 0x0066);

   AddZ80CReadByte(0x0000, 0x9FFF, NULL,			NULL);		// BANK ROM
   AddZ80CReadByte(0xD000, 0xDFFF, NULL,			RAM2+0x00D000);	// WORK RAM
   AddZ80CReadByte(0xE000, 0xEFFF, NULL,			RAM +0x00E000);	// COMMON RAM
   AddZ80CReadByte(0xB000, 0xB001, kageki_ym2203_rb,		NULL);		// YM2203/DSW
   AddZ80CReadByte(0xC000, 0xC002, TNZSSystemInputRead,		NULL);		// INPUT
   AddZ80CReadByte(0xF000, 0xF003, NULL,			RAM2+0x00F000);	// ???
   AddZ80CReadByte(0x0000, 0xFFFF, DefBadReadZ80,		NULL);		// <bad reads>
   AddZ80CReadByte(-1, -1, NULL, NULL);

   AddZ80CWriteByte(0xD000, 0xDFFF, NULL,			RAM2+0x00D000);	// WORK RAM
   AddZ80CWriteByte(0xE000, 0xEFFF, NULL,			RAM +0x00E000);	// COMMON RAM
   AddZ80CWriteByte(0xA000, 0xA000, SubBankSwitch,		NULL);		// ROM BANK
   AddZ80CWriteByte(0xB000, 0xB001, kageki_ym2203_wb,		NULL);		// YM2203/DSW
   AddZ80CWriteByte(0x0000, 0xFFFF, DefBadWriteZ80,		NULL);		// <bad writes>
   AddZ80CWriteByte(-1, -1, NULL, NULL);

   AddZ80CReadPort(0x00, 0xFF, DefBadReadZ80,			NULL);		// <bad reads>
   AddZ80CReadPort(-1, -1, NULL, NULL);

   AddZ80CWritePort(0xAA, 0xAA, StopZ80CMode2,			NULL);		// Trap Idle Z80
   AddZ80CWritePort(0x00, 0xFF, DefBadWriteZ80,			NULL);		// <bad writes>
   AddZ80CWritePort(-1, -1, NULL, NULL);

   AddZ80CInit();

   FreeMem(TMP);

   /*-----------------------*/

   if(!(GFX=AllocateMem(0x200000))) return;
   if(!(TMP=AllocateMem(0x080000))) return;

   if(!load_rom("b35-01.13a", TMP+0x00000, 0x20000)) return;
   if(!load_rom("b35-02.12a", TMP+0x20000, 0x20000)) return;
   if(!load_rom("b35-03.10a", TMP+0x40000, 0x20000)) return;
   if(!load_rom("b35-04.8a",  TMP+0x60000, 0x20000)) return;
   tb=0;
   for(ta=0;ta<0x40000;ta++){
      tc=TMP[ta];
      GFX[tb+0]=((tc&0x80)>>7)<<0;
      GFX[tb+1]=((tc&0x40)>>6)<<0;
      GFX[tb+2]=((tc&0x20)>>5)<<0;
      GFX[tb+3]=((tc&0x10)>>4)<<0;
      GFX[tb+4]=((tc&0x08)>>3)<<0;
      GFX[tb+5]=((tc&0x04)>>2)<<0;
      GFX[tb+6]=((tc&0x02)>>1)<<0;
      GFX[tb+7]=((tc&0x01)>>0)<<0;
      tc=TMP[ta+0x40000];
      GFX[tb+0]|=((tc&0x80)>>7)<<1;
      GFX[tb+1]|=((tc&0x40)>>6)<<1;
      GFX[tb+2]|=((tc&0x20)>>5)<<1;
      GFX[tb+3]|=((tc&0x10)>>4)<<1;
      GFX[tb+4]|=((tc&0x08)>>3)<<1;
      GFX[tb+5]|=((tc&0x04)>>2)<<1;
      GFX[tb+6]|=((tc&0x02)>>1)<<1;
      GFX[tb+7]|=((tc&0x01)>>0)<<1;
      tb+=16;
      if((tb&0x7F)==0){tb-=0x78;}
      else{if((tb&0x7F)==8){tb-=0x08;}}
   }
   if(!load_rom("b35-05.7a", TMP+0x00000, 0x20000)) return;
   if(!load_rom("b35-06.5a", TMP+0x20000, 0x20000)) return;
   if(!load_rom("b35-07.4a", TMP+0x40000, 0x20000)) return;
   if(!load_rom("b35-08.2a", TMP+0x60000, 0x20000)) return;
   tb=0;
   for(ta=0;ta<0x40000;ta++){
      tc=TMP[ta];
      GFX[tb+0]|=((tc&0x80)>>7)<<2;
      GFX[tb+1]|=((tc&0x40)>>6)<<2;
      GFX[tb+2]|=((tc&0x20)>>5)<<2;
      GFX[tb+3]|=((tc&0x10)>>4)<<2;
      GFX[tb+4]|=((tc&0x08)>>3)<<2;
      GFX[tb+5]|=((tc&0x04)>>2)<<2;
      GFX[tb+6]|=((tc&0x02)>>1)<<2;
      GFX[tb+7]|=((tc&0x01)>>0)<<2;
      tc=TMP[ta+0x40000];
      GFX[tb+0]|=((tc&0x80)>>7)<<3;
      GFX[tb+1]|=((tc&0x40)>>6)<<3;
      GFX[tb+2]|=((tc&0x20)>>5)<<3;
      GFX[tb+3]|=((tc&0x10)>>4)<<3;
      GFX[tb+4]|=((tc&0x08)>>3)<<3;
      GFX[tb+5]|=((tc&0x04)>>2)<<3;
      GFX[tb+6]|=((tc&0x02)>>1)<<3;
      GFX[tb+7]|=((tc&0x01)>>0)<<3;
      tb+=16;
      if((tb&0x7F)==0){tb-=0x78;}
      else{if((tb&0x7F)==8){tb-=0x08;}}
   }

   FreeMem(TMP);

   GFX_BG0_SOLID = make_solid_mask_16x16(GFX, 0x2000);

   InitPaletteMap(RAM+0xF800, 0x20, 0x10, 0x8000);

   set_colour_mapper(&col_map_xrrr_rrgg_gggb_bbbb);

   // Init seta_x1 emulation
   // ----------------------

   seta_x1.RAM_A	= RAM+0x0C000;
   seta_x1.RAM_B	= RAM+0x0F000;
   seta_x1.GFX		= GFX;
   seta_x1.MASK		= GFX_BG0_SOLID;
   seta_x1.bmp_x	= 32;
   seta_x1.bmp_y	= 32;
   seta_x1.bmp_w	= 224;
   seta_x1.bmp_h	= 256;
   seta_x1.tile_mask	= 0x1FFF;
   seta_x1.scr_x	= 0;
   seta_x1.scr_y	= 0;
}

void clear_kageki(void)
{
#ifdef RAINE_DEBUG
      save_debug("RAM.BIN", RAM, RAMSize, 0);
#endif
}

void load_chuka_taisen(void)
{
   int ta,tb,tc;
   UINT8 *TMP;

   romset=8;

   RAMSize=0x10000+0x10000+0x10+(0xC000*2);

   if(!(ROM =AllocateMem(0xC000*6))) return;

   if(!(RAM=AllocateMem(RAMSize))) return;

   RAM2     = RAM+0x10000;
   RAM_BANK = RAM+0x10000+0x10000+0x10;

   memset(RAM+0x00000, 0x00, RAMSize);

   if(!(TMP=AllocateMem(0x20000))) return;

   if(!load_rom("b44.10", TMP+0x00000, 0x10000)) return;	// Z80 MAIN ROM
   if(!load_rom("b44.11", TMP+0x10000, 0x10000)) return;	// Z80 MAIN ROM

   // fix i/o error (m-chip)

   TMP[0x01D6]=0x37;
   TMP[0x01D7]=0xC9;

   TMP[0x022D]=0x00;
   TMP[0x022E]=0x00;
   TMP[0x022F]=0x00;

   TMP[0x0234]=0x00;
   TMP[0x0235]=0x00;
   TMP[0x0236]=0x00;

   // tilt

   TMP[0x0189]=0x00;
   TMP[0x018A]=0x00;

   // coin error

   TMP[0x019a]=0xC9;

/*
   // Fix SUB WAIT

   TMP[0x02d2]=0x00;
   TMP[0x02d3]=0x00;

   // Fix HW ERROR

   TMP[0x032b]=0xC9;

   // Fix ROM ERROR

   TMP[0x08dc]=0xC9;

   // Skip Idle Z80

   TMP[0x4219]=0xD3;  // OUTA (AAh)
   TMP[0x421A]=0xAA;  //

   SetStopZ80BMode2(0x4219);
*/
   init_bank_rom(TMP,ROM);

   memcpy(RAM,TMP,0x10000);
   AddZ80BROMBase(RAM, 0x0038, 0x0066);

   AddZ80BReadByte(0x0000, 0xBFFF, NULL,			NULL);		// BANK ROM/RAM
   AddZ80BReadByte(0xC000, 0xF3FF, NULL,			RAM+0x00C000);	// OBJECT RAM/WORK RAM/VDC RAM
   AddZ80BReadByte(0xF800, 0xFBFF, NULL,			RAM+0x00F800);	// COLOR RAM
   AddZ80BReadByte(0x0000, 0xFFFF, DefBadReadZ80,		NULL);		// <bad reads>
   AddZ80BReadByte(-1, -1, NULL, NULL);

   AddZ80BWriteByte(0x8000, 0xBFFF, NULL,			NULL);		// BANK RAM
   AddZ80BWriteByte(0xC000, 0xF3FF, NULL,			RAM+0x00C000);	// OBJECT RAM/WORK RAM/VDC RAM
   AddZ80BWriteByte(0xF800, 0xFBFF, NULL,			RAM+0x00F800);	// COLOR RAM
   AddZ80BWriteByte(0xF600, 0xF600, MainBankSwitch,		NULL);		// ROM BANK
   AddZ80BWriteByte(0x0000, 0xFFFF, DefBadWriteZ80,		NULL);		// <bad writes>
   AddZ80BWriteByte(-1, -1, NULL, NULL);

   AddZ80BReadPort(0x00, 0xFF, DefBadReadZ80,			NULL);		// <bad reads>
   AddZ80BReadPort(-1, -1, NULL, NULL);

   AddZ80BWritePort(0xAA, 0xAA, StopZ80BMode2,			NULL);		// Trap Idle Z80
   AddZ80BWritePort(0x00, 0xFF, DefBadWriteZ80,			NULL);		// <bad reads>
   AddZ80BWritePort(-1, -1, NULL, NULL);

   AddZ80BInit();

   if(!load_rom("b44.12w", TMP, 0x10000)) return;		// Z80 SUB ROM

   // fix i/o error

   TMP[0x00CD]=0xC9;

   TMP[0x00F7]=0xC9;

   TMP[0x0165]=0x00;
   TMP[0x0166]=0x00;

   TMP[0x017C]=0x00;
   TMP[0x017D]=0x00;

   // speed hack

   TMP[0x0277]=0xD3;
   TMP[0x0278]=0xAA;

   SetStopZ80CMode2(0x0277);

   init_bank_rom2(TMP);

   memcpy(RAM2,TMP,0x10000);
   AddZ80CROMBase(RAM2, 0x0038, 0x0066);

   AddZ80CReadByte(0x0000, 0x9FFF, NULL,			NULL);		// BANK ROM
   AddZ80CReadByte(0xD000, 0xDFFF, NULL,			RAM2+0x00D000);	// WORK RAM
   AddZ80CReadByte(0xE000, 0xEFFF, NULL,			RAM +0x00E000);	// COMMON RAM
   AddZ80CReadByte(0xB000, 0xB001, tnzs_ym2203_rb,		NULL);		// YM2203/DSW
   AddZ80CReadByte(0xC000, 0xC001, chuka_ioc_rb,		NULL);		// INPUT
   AddZ80CReadByte(0xF000, 0xF003, NULL,			RAM2+0x00F000);	// ???
   AddZ80CReadByte(0x0000, 0xFFFF, DefBadReadZ80,		NULL);		// <bad reads>
   AddZ80CReadByte(-1, -1, NULL, NULL);

   AddZ80CWriteByte(0xD000, 0xDFFF, NULL,			RAM2+0x00D000);	// WORK RAM
   AddZ80CWriteByte(0xE000, 0xEFFF, NULL,			RAM +0x00E000);	// COMMON RAM
   AddZ80CWriteByte(0xA000, 0xA000, SubBankSwitch,		NULL);		// ROM BANK
   AddZ80CWriteByte(0xB000, 0xB001, tnzs_ym2203_wb,		NULL);		// YM2203/DSW
   AddZ80CWriteByte(0xC000, 0xC001, chuka_ioc_wb,		NULL);		// INPUT
   AddZ80CWriteByte(0x0000, 0xFFFF, DefBadWriteZ80,		NULL);		// <bad writes>
   AddZ80CWriteByte(-1, -1, NULL, NULL);

   AddZ80CReadPort(0x00, 0xFF, DefBadReadZ80,			NULL);		// <bad reads>
   AddZ80CReadPort(-1, -1, NULL, NULL);

   AddZ80CWritePort(0xAA, 0xAA, StopZ80CMode2,			NULL);		// Trap Idle Z80
   AddZ80CWritePort(0x00, 0xFF, DefBadWriteZ80,			NULL);		// <bad writes>
   AddZ80CWritePort(-1, -1, NULL, NULL);

   AddZ80CInit();

   FreeMem(TMP);

   /*-----------------------*/

   if(!(GFX=AllocateMem(0x200000))) return;
   if(!(TMP=AllocateMem(0x080000))) return;

   if(!load_rom("b44-01.a13", TMP+0x00000, 0x20000)) return;
   if(!load_rom("b44-02.a12", TMP+0x20000, 0x20000)) return;
   if(!load_rom("b44-03.a10", TMP+0x40000, 0x20000)) return;
   if(!load_rom("b44-04.a08", TMP+0x60000, 0x20000)) return;
   tb=0;
   for(ta=0;ta<0x40000;ta++){
      tc=TMP[ta];
      GFX[tb+0]=((tc&0x80)>>7)<<0;
      GFX[tb+1]=((tc&0x40)>>6)<<0;
      GFX[tb+2]=((tc&0x20)>>5)<<0;
      GFX[tb+3]=((tc&0x10)>>4)<<0;
      GFX[tb+4]=((tc&0x08)>>3)<<0;
      GFX[tb+5]=((tc&0x04)>>2)<<0;
      GFX[tb+6]=((tc&0x02)>>1)<<0;
      GFX[tb+7]=((tc&0x01)>>0)<<0;
      tc=TMP[ta+0x40000];
      GFX[tb+0]|=((tc&0x80)>>7)<<1;
      GFX[tb+1]|=((tc&0x40)>>6)<<1;
      GFX[tb+2]|=((tc&0x20)>>5)<<1;
      GFX[tb+3]|=((tc&0x10)>>4)<<1;
      GFX[tb+4]|=((tc&0x08)>>3)<<1;
      GFX[tb+5]|=((tc&0x04)>>2)<<1;
      GFX[tb+6]|=((tc&0x02)>>1)<<1;
      GFX[tb+7]|=((tc&0x01)>>0)<<1;
      tb+=16;
      if((tb&0x7F)==0){tb-=0x78;}
      else{if((tb&0x7F)==8){tb-=0x08;}}
   }
   if(!load_rom("b44-05.a07", TMP+0x00000, 0x20000)) return;
   if(!load_rom("b44-06.a05", TMP+0x20000, 0x20000)) return;
   if(!load_rom("b44-07.a04", TMP+0x40000, 0x20000)) return;
   if(!load_rom("b44-08.a02", TMP+0x60000, 0x20000)) return;
   tb=0;
   for(ta=0;ta<0x40000;ta++){
      tc=TMP[ta];
      GFX[tb+0]|=((tc&0x80)>>7)<<2;
      GFX[tb+1]|=((tc&0x40)>>6)<<2;
      GFX[tb+2]|=((tc&0x20)>>5)<<2;
      GFX[tb+3]|=((tc&0x10)>>4)<<2;
      GFX[tb+4]|=((tc&0x08)>>3)<<2;
      GFX[tb+5]|=((tc&0x04)>>2)<<2;
      GFX[tb+6]|=((tc&0x02)>>1)<<2;
      GFX[tb+7]|=((tc&0x01)>>0)<<2;
      tc=TMP[ta+0x40000];
      GFX[tb+0]|=((tc&0x80)>>7)<<3;
      GFX[tb+1]|=((tc&0x40)>>6)<<3;
      GFX[tb+2]|=((tc&0x20)>>5)<<3;
      GFX[tb+3]|=((tc&0x10)>>4)<<3;
      GFX[tb+4]|=((tc&0x08)>>3)<<3;
      GFX[tb+5]|=((tc&0x04)>>2)<<3;
      GFX[tb+6]|=((tc&0x02)>>1)<<3;
      GFX[tb+7]|=((tc&0x01)>>0)<<3;
      tb+=16;
      if((tb&0x7F)==0){tb-=0x78;}
      else{if((tb&0x7F)==8){tb-=0x08;}}
   }

   FreeMem(TMP);

   memset(GFX,0x00,0x100);
   GFX_BG0_SOLID = make_solid_mask_16x16(GFX, 0x2000);

   InitPaletteMap(RAM+0xF800, 0x20, 0x10, 0x8000);

   set_colour_mapper(&col_map_xrrr_rrgg_gggb_bbbb);

   // Init seta_x1 emulation
   // ----------------------

   seta_x1.RAM_A	= RAM+0x0C000;
   seta_x1.RAM_B	= RAM+0x0F000;
   seta_x1.GFX		= GFX;
   seta_x1.MASK		= GFX_BG0_SOLID;
   seta_x1.bmp_x	= 32;
   seta_x1.bmp_y	= 32;
   seta_x1.bmp_w	= 256;
   seta_x1.bmp_h	= 224;
   seta_x1.tile_mask	= 0x1FFF;
   seta_x1.scr_x	= 0;
   seta_x1.scr_y	= 0;

}

void clear_chuka_taisen(void)
{
#ifdef RAINE_DEBUG
      save_debug("RAM.BIN", RAM, RAMSize, 0);
#endif
}

void LoadPlumpPop(void)
{
   int ta,tb,tc;
   UINT8 *TMP, *RAM_COLOUR;

   romset=10;

   RAMSize=0x10000+0x10000+0x10+(0xC000*2)+0x400;

   if(!(ROM =AllocateMem(0xC000*6))) return;

   if(!(RAM=AllocateMem(RAMSize))) return;

   RAM2     = RAM+0x10000;
   RAM_BANK = RAM+0x10000+0x10000+0x10;
   RAM_COLOUR = RAM+0x10000+0x10000+0x10+(0xC000*2);

   memset(RAM+0x00000, 0x00, RAMSize);

   if(!(TMP=AllocateMem(0x20000))) return;

   if(!load_rom("a98-09.bin", TMP, 0x10000)) return;   // Z80 MAIN ROM
   if(!load_rom("a98-10.bin", TMP+0x10000, 0x10000)) return;   // Z80 MAIN ROM

   // Skip Idle Z80
   // -------------

   TMP[0x3753]=0xC9;  // ret

   TMP[0x0176]=0xD3;  // OUTA (AAh)
   TMP[0x0177]=0xAA;  //

   SetStopZ80BMode2(0x0176);

   init_bank_rom(TMP,ROM);

   memcpy(RAM,TMP,0x10000);
   AddZ80BROMBase(RAM, 0x0038, 0x0066);

   AddZ80BReadByte(0x0000, 0xBFFF, NULL,			NULL);		// BANK ROM/RAM
   AddZ80BReadByte(0xC000, 0xF3FF, NULL,			RAM+0x00C000);	// OBJECT RAM/WORK RAM/VDC RAM
   AddZ80BReadByte(0xF800, 0xFBFF, NULL,			RAM+0x00F800);	// COLOR RAM
   AddZ80BReadByte(0x0000, 0xFFFF, DefBadReadZ80,		NULL);		// <bad reads>
   AddZ80BReadByte(-1, -1, NULL, NULL);

   AddZ80BWriteByte(0x8000, 0xBFFF, NULL,			NULL);		// BANK RAM
   AddZ80BWriteByte(0xC000, 0xF3FF, NULL,			RAM+0x00C000);	// OBJECT RAM/WORK RAM/VDC RAM
   AddZ80BWriteByte(0xF800, 0xFBFF, NULL,			RAM+0x00F800);	// COLOR RAM
   AddZ80BWriteByte(0xF600, 0xF600, MainBankSwitch,		NULL);		// ROM BANK
   AddZ80BWriteByte(0x0000, 0xFFFF, DefBadWriteZ80,		NULL);		// <bad writes>
   AddZ80BWriteByte(-1, -1, NULL, NULL);

   AddZ80BReadPort(0x00, 0xFF, DefBadReadZ80,			NULL);		// <bad reads>
   AddZ80BReadPort(-1, -1, NULL, NULL);

   AddZ80BWritePort(0xAA, 0xAA, StopZ80BMode2,			NULL);		// Trap Idle Z80
   AddZ80BWritePort(0x00, 0xFF, DefBadWriteZ80,			NULL);		// <bad reads>
   AddZ80BWritePort(-1, -1, NULL, NULL);

   AddZ80BInit();

   if(!load_rom("a98-11.bin", TMP, 0x10000)) return;      // Z80 SUB ROM

   // Skip Idle Z80
   // -------------

   TMP[0x0140]=0xD3;  // OUTA (AAh)
   TMP[0x0141]=0xAA;  //

   SetStopZ80CMode2(0x0137);

   init_bank_rom2(TMP);

   memcpy(RAM2,TMP,0x10000);
   AddZ80CROMBase(RAM2, 0x0038, 0x0066);

   AddZ80CReadByte(0x0000, 0x9FFF, NULL,			NULL);		// BANK ROM
   AddZ80CReadByte(0xD000, 0xDFFF, NULL,			RAM2+0x00D000);	// WORK RAM
   AddZ80CReadByte(0xE000, 0xEFFF, NULL,			RAM +0x00E000);	// COMMON RAM
   AddZ80CReadByte(0xB000, 0xB001, tnzs_ym2203_rb,		NULL);		// YM2203/DSW
   AddZ80CReadByte(0xC000, 0xC001, mcu_tnzs_read,		NULL);		// INPUT
   AddZ80CReadByte(0xF000, 0xF003, NULL,			RAM2+0x00F000);	// ???
   AddZ80CReadByte(0x0000, 0xFFFF, DefBadReadZ80,		NULL);		// <bad reads>
   AddZ80CReadByte(-1, -1, NULL, NULL);

   AddZ80CWriteByte(0xD000, 0xDFFF, NULL,			RAM2+0x00D000);	// WORK RAM
   AddZ80CWriteByte(0xE000, 0xEFFF, NULL,			RAM +0x00E000);	// COMMON RAM
   AddZ80CWriteByte(0xA000, 0xA000, SubBankSwitch,		NULL);		// ROM BANK
   AddZ80CWriteByte(0xB000, 0xB001, tnzs_ym2203_wb,		NULL);		// YM2203/DSW
   AddZ80CWriteByte(0xC000, 0xC001, mcu_tnzs_write,		NULL);		// INPUT
   AddZ80CWriteByte(0x0000, 0xFFFF, DefBadWriteZ80,		NULL);		// <bad writes>
   AddZ80CWriteByte(-1, -1, NULL, NULL);

   AddZ80CReadPort(0x00, 0xFF, DefBadReadZ80,			NULL);	// <bad reads>
   AddZ80CReadPort(-1, -1, NULL, NULL);

   AddZ80CWritePort(0xAA, 0xAA, StopZ80CMode2,			NULL);	// Trap Idle Z80
   AddZ80CWritePort(0x00, 0xFF, DefBadWriteZ80,			NULL);	// <bad writes>
   AddZ80CWritePort(-1, -1, NULL, NULL);

   AddZ80CInit();

   FreeMem(TMP);
   mcu_type = MCU_DRTOPPEL;

   /*-----------------------*/

   if(!(GFX=AllocateMem(0x200000))) return;
   if(!(TMP=AllocateMem(0x080000))) return;

   if(!load_rom("a98-01.bin", TMP+0x00000, 0x10000)) return;
   if(!load_rom("a98-02.bin", TMP+0x20000, 0x10000)) return;
   if(!load_rom("a98-03.bin", TMP+0x40000, 0x10000)) return;
   if(!load_rom("a98-04.bin", TMP+0x60000, 0x10000)) return;
   tb=0;
   for(ta=0;ta<0x40000;ta++){
      tc=TMP[ta];
      GFX[tb+0]=((tc&0x80)>>7)<<0;
      GFX[tb+1]=((tc&0x40)>>6)<<0;
      GFX[tb+2]=((tc&0x20)>>5)<<0;
      GFX[tb+3]=((tc&0x10)>>4)<<0;
      GFX[tb+4]=((tc&0x08)>>3)<<0;
      GFX[tb+5]=((tc&0x04)>>2)<<0;
      GFX[tb+6]=((tc&0x02)>>1)<<0;
      GFX[tb+7]=((tc&0x01)>>0)<<0;
      tc=TMP[ta+0x40000];
      GFX[tb+0]|=((tc&0x80)>>7)<<1;
      GFX[tb+1]|=((tc&0x40)>>6)<<1;
      GFX[tb+2]|=((tc&0x20)>>5)<<1;
      GFX[tb+3]|=((tc&0x10)>>4)<<1;
      GFX[tb+4]|=((tc&0x08)>>3)<<1;
      GFX[tb+5]|=((tc&0x04)>>2)<<1;
      GFX[tb+6]|=((tc&0x02)>>1)<<1;
      GFX[tb+7]|=((tc&0x01)>>0)<<1;
      tb+=16;
      if((tb&0x7F)==0){tb-=0x78;}
      else{if((tb&0x7F)==8){tb-=0x08;}}
   }
   if(!load_rom("a98-05.bin", TMP+0x00000, 0x10000)) return;
   if(!load_rom("a98-06.bin", TMP+0x20000, 0x10000)) return;
   if(!load_rom("a98-07.bin", TMP+0x40000, 0x10000)) return;
   if(!load_rom("a98-08.bin", TMP+0x60000, 0x10000)) return;
   tb=0;
   for(ta=0;ta<0x40000;ta++){
      tc=TMP[ta];
      GFX[tb+0]|=((tc&0x80)>>7)<<2;
      GFX[tb+1]|=((tc&0x40)>>6)<<2;
      GFX[tb+2]|=((tc&0x20)>>5)<<2;
      GFX[tb+3]|=((tc&0x10)>>4)<<2;
      GFX[tb+4]|=((tc&0x08)>>3)<<2;
      GFX[tb+5]|=((tc&0x04)>>2)<<2;
      GFX[tb+6]|=((tc&0x02)>>1)<<2;
      GFX[tb+7]|=((tc&0x01)>>0)<<2;
      tc=TMP[ta+0x40000];
      GFX[tb+0]|=((tc&0x80)>>7)<<3;
      GFX[tb+1]|=((tc&0x40)>>6)<<3;
      GFX[tb+2]|=((tc&0x20)>>5)<<3;
      GFX[tb+3]|=((tc&0x10)>>4)<<3;
      GFX[tb+4]|=((tc&0x08)>>3)<<3;
      GFX[tb+5]|=((tc&0x04)>>2)<<3;
      GFX[tb+6]|=((tc&0x02)>>1)<<3;
      GFX[tb+7]|=((tc&0x01)>>0)<<3;
      tb+=16;
      if((tb&0x7F)==0){tb-=0x78;}
      else{if((tb&0x7F)==8){tb-=0x08;}}
   }

   tb=0;
   if(!load_rom("a98-12.bpr", TMP+0x00000, 0x200)) return;
   if(!load_rom("a98-13.bpr", TMP+0x00200, 0x200)) return;
   for(ta=0;ta<0x200;ta++,tb+=2){
      tc  =  TMP[ta+0x000];
      tc |= (TMP[ta+0x200]) << 8;
      WriteWord(&RAM_COLOUR[tb], tc);
   }
   FreeMem(TMP);

   GFX_BG0_SOLID = make_solid_mask_16x16(GFX, 0x2000);

   InitPaletteMap(RAM_COLOUR, 0x20, 0x10, 0x8000);

   set_colour_mapper(&col_map_xrrr_rrgg_gggb_bbbb);

   GameMouse=2;

   // Init seta_x1 emulation
   // ----------------------

   seta_x1.RAM_A	= RAM+0x0C000;
   seta_x1.RAM_B	= RAM+0x0F000;
   seta_x1.GFX		= GFX;
   seta_x1.MASK		= GFX_BG0_SOLID;
   seta_x1.bmp_x	= 32;
   seta_x1.bmp_y	= 32;
   seta_x1.bmp_w	= 256;
   seta_x1.bmp_h	= 224;
   seta_x1.tile_mask	= 0x1FFF;
   seta_x1.scr_x	= 0;
   seta_x1.scr_y	= 0;
}

void ClearPlumpPop(void)
{
#ifdef RAINE_DEBUG
      save_debug("RAM.BIN", RAM, RAMSize, 0);
#endif
}

void ExecuteTNZSSystemFrame(void)
{
   static UINT32 coin_flip_a,coin_flip_b;
   static UINT32 coin_store_a,coin_store_b;
   static int mx,my;
   static UINT8 coinage[4][2] =
   {
      { 1 , 1 },
      { 1 , 2 },
      { 2 , 1 },
      { 2 , 3 },
   };
   UINT32 ta;
   if(! (RAM[0x020004]&0x02)){
      if(! coin_flip_a){
         coin_flip_a = 1;
         coin_store_a ++;
         ta = ((~get_dsw(0)) >> 4) & 0x03;
         if(coin_store_a >= coinage[ta][0]){
            coin_store_a -= coinage[ta][0];
            number_of_credits += coinage[ta][1];
         }
         if(number_of_credits>9)
            number_of_credits = 9;
      }
   }
   else{
      coin_flip_a = 0;
   }

   if(! (RAM[0x020004]&0x01)){
      if(! coin_flip_b){
         coin_flip_b = 1;
         coin_store_b ++;
         ta = ((~get_dsw(0)) >> 6) & 0x03;
         if(coin_store_b >= coinage[ta][0]){
            coin_store_b -= coinage[ta][0];
            number_of_credits += coinage[ta][1];
         }
         if(number_of_credits>9)
            number_of_credits = 9;
      }
   }
   else{
      coin_flip_b = 0;
   }

   if(cpu_get_pc(CPU_Z80_1) == 0x0000){
     mcu_reset();
     memset(RAM+0xC000, 0x00, 0x3800);// clear RAM (avoids messy gfx on reset) (DONT CLEAR COLOR RAM!!)
   }

   if((romset==2)||(romset==4)||(romset==10)){

   GetMouseMickeys(&mx,&my);

   p1_trackball_x += mx;

   if((RAM[0x20002+4]&1)==0) p1_trackball_x+=12;
   if((RAM[0x20002+4]&2)==0) p1_trackball_x-=12;

   WriteWord(&RAM2[0x00F000],p1_trackball_x);

   if(*MouseB&1) {
     if (GameMouse==1) // Bit 0
       RAM[0x20003] &= ~0x10;
     else if (GameMouse==2)
       RAM[0x20003] |= 0x10;
   }
   if(*MouseB&2) {
     if (GameMouse==1) // Bit 0
       RAM[0x20003] &= ~0x20;
     else if (GameMouse==2)
       RAM[0x20003] |= 0x20;
   }
   }

   //print_ingame(60,"%04x:%04x",ReadWord68k(&RAM[0xF300]),ReadWord68k(&RAM[0xF302]));

   cpu_execute_cycles(CPU_Z80_1, CPU_FRAME_MHz(6,60));	// Main Z80 8MHz (60fps)
   // if(romset==8) cpu_execute_cycles(CPU_Z80_1, CPU_FRAME_MHz(8,60));	// must fix mz80!!!
   print_debug("Z80PC_MAIN:%04x\n",z80pc);

   cpu_interrupt(CPU_Z80_1, 0x38);

   cpu_execute_cycles(CPU_Z80_2, CPU_FRAME_MHz(6,60));	// Sub Z80 8MHz (60fps)
   print_debug("Z80PC_SUB:%04x\n",z80pc);
   cpu_interrupt(CPU_Z80_2, 0x38);

   if(romset==6){
     cpu_execute_cycles(CPU_Z80_0, CPU_FRAME_MHz(8,60));	// Sound Z80 8MHz (60fps)
     print_debug("Z80PC_SOUND:%04x\n",z80pc);
     cpu_int_nmi(CPU_Z80_0);
     if(want_int){
       want_int = 0;
       cpu_interrupt(CPU_Z80_0, 0x38);
     }
   } else if (romset == 3 || romset==1 || romset==5 || romset==10) { // tnzz, extermination, plumppop
     mcu_handle_coins(RAM[0x20004] | (RAM[0x20005] << 2));
   } else if (romset == 9) { // drtoppel
     mcu_handle_coins((RAM[0x20004] | (RAM[0x20005] << 2)) ^ 15);
     // mcu_handle_coins((RAM[0x20004] | (RAM[0x20003] << 2)) ^ 15);
   }
}

void ExecuteInsectorXFrame(void)
{
   int ta;

   for(ta=0;ta<100;ta++){

   if(cpu_get_pc(CPU_Z80_1) != 0x0206){
   cpu_execute_cycles(CPU_Z80_1, CPU_FRAME_MHz(8,60)/100);	// Main Z80 8MHz (60fps)
   }

   if(cpu_get_pc(CPU_Z80_2) != 0x01F9){
   cpu_execute_cycles(CPU_Z80_2, CPU_FRAME_MHz(8,60)/100);	// Sub Z80 8MHz (60fps)
   }

   }

   print_debug("Z80PC_MAIN:%04x\n",cpu_get_pc(CPU_Z80_1));
   cpu_interrupt(CPU_Z80_1, 0x38);

   print_debug("Z80PC_SUB:%04x\n",cpu_get_pc(CPU_Z80_2));
   cpu_interrupt(CPU_Z80_2, 0x38);
}

void DrawTNZSSystem(void)
{
   ClearPaletteMap();

   render_seta_x1_z80();
}

