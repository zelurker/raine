#define DRV_DEF_LOAD load_cps1_10
#define DRV_DEF_VIDEO &video_cps1
#define DRV_DEF_EXEC execute_cps1_frame
#define DRV_DEF_SOUND sound_cps1
/*****************************************************************************/
/*                                                                           */
/*                   CAPCOM SYSTEM 1 / CPS1 (C) 1990 CAPCOM                  */
/* This contains only declarations for the gmaes. No code here               */
/*****************************************************************************/

#include "gameinc.h"
#include "cps1.h"
#include "sasound.h"
#include "timer.h"
#include "2151intf.h"
#include "adpcm.h"
#include "qsound.h"

// Roms structs (generated 6/1/2004)
// Updated 1/22/2009 !
// Last update : 7/9/2013 !

static struct ROM_INFO rom_forgottn[] =
{
  LOAD_16_8( CPU1, "lw11.12f", 0x00000, 0x20000, 0x73e920b7),
  LOAD_16_8( CPU1, "lw15.12h", 0x00001, 0x20000, 0x50d7012d),
  LOAD_16_8( CPU1, "lw10.13f", 0x40000, 0x20000, 0xbea45994),
  LOAD_16_8( CPU1, "lw14.13h", 0x40001, 0x20000, 0x539b2339),
  LOAD_SW16( CPU1, "lw-07.10g", 0x80000, 0x80000, 0xfd252a26),
  { "lw_2.2b", 0x20000, 0x4bd75fee, REGION_GFX1, 0x000000, LOAD_8_64 },
  { "lw_1.2a", 0x20000, 0x65f41485, REGION_GFX1, 0x000001, LOAD_8_64 },
  { "lw-08.9b", 0x80000, 0x25a8e43c, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "lw-05.6d", 0x80000, 0xe4552fd7, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "lw_30.8h", 0x20000, 0xb385954e, REGION_GFX1, 0x000006, LOAD_8_64 },
  { "lw_29.8f", 0x20000, 0x7bda1ac6, REGION_GFX1, 0x000007, LOAD_8_64 },
  { "lw_4.3b", 0x20000, 0x50cf757f, REGION_GFX1, 0x100000, LOAD_8_64 },
  { "lw_3.3a", 0x20000, 0xc03ef278, REGION_GFX1, 0x100001, LOAD_8_64 },
  { "lw_32.9h", 0x20000, 0x30967a15, REGION_GFX1, 0x100006, LOAD_8_64 },
  { "lw_31.9f", 0x20000, 0xc49d37fb, REGION_GFX1, 0x100007, LOAD_8_64 },
  { "lw-02.6b", 0x80000, 0x43e6c5c8, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "lw_14.10b", 0x20000, 0x82862cce, REGION_GFX1, 0x200002, LOAD_8_64 },
  { "lw_13.10a", 0x20000, 0xb81c0e96, REGION_GFX1, 0x200003, LOAD_8_64 },
  { "lw-06.9d", 0x80000, 0x5b9edffc, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "lw_26.10e", 0x20000, 0x57bcd032, REGION_GFX1, 0x200006, LOAD_8_64 },
  { "lw_25.10c", 0x20000, 0xbac91554, REGION_GFX1, 0x200007, LOAD_8_64 },
  { "lw_16.11b", 0x20000, 0x40b26554, REGION_GFX1, 0x300002, LOAD_8_64 },
  { "lw_15.11a", 0x20000, 0x1b7d2e07, REGION_GFX1, 0x300003, LOAD_8_64 },
  { "lw_28.11e", 0x20000, 0xa805ad30, REGION_GFX1, 0x300006, LOAD_8_64 },
  { "lw_27.11c", 0x20000, 0x103c1bd2, REGION_GFX1, 0x300007, LOAD_8_64 },
/* Ignored : 	ROM_COPY( "gfx", 0x200000, 0x000000, 0x8000 ) */
  LOAD( ROM2, "lw_00.13c", 0x00000, 0x10000, 0x59df2a63),
  LOAD( SMP1, "lw-03u.12e", 0x00000, 0x20000, 0x807d051f),
  LOAD( SMP1, "lw-04u.13e", 0x20000, 0x20000, 0xe6cd098e),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_forgottnu[] = // clone of forgottn
{
  LOAD_16_8( CPU1, "lw11c.12f", 0x00000, 0x20000, 0xe62742b6),
  LOAD_16_8( CPU1, "lw15c.12h", 0x00001, 0x20000, 0x1b70f216),
  LOAD_16_8( CPU1, "lw10c.13f", 0x40000, 0x20000, 0x8f5ea3f5),
  LOAD_16_8( CPU1, "lw14c.13h", 0x40001, 0x20000, 0x708e7472),
  LOAD_SW16( CPU1, "lw-07.10g", 0x80000, 0x80000, 0xfd252a26),
  { "lw_2.2b", 0x20000, 0x4bd75fee, REGION_GFX1, 0x000000, LOAD_8_64 },
  { "lw_1.2a", 0x20000, 0x65f41485, REGION_GFX1, 0x000001, LOAD_8_64 },
  { "lw-08.9b", 0x80000, 0x25a8e43c, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "lw-05.6d", 0x80000, 0xe4552fd7, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "lw_30.8h", 0x20000, 0xb385954e, REGION_GFX1, 0x000006, LOAD_8_64 },
  { "lw_29.8f", 0x20000, 0x7bda1ac6, REGION_GFX1, 0x000007, LOAD_8_64 },
  { "lw_4.3b", 0x20000, 0x50cf757f, REGION_GFX1, 0x100000, LOAD_8_64 },
  { "lw_3.3a", 0x20000, 0xc03ef278, REGION_GFX1, 0x100001, LOAD_8_64 },
  { "lw_32.9h", 0x20000, 0x30967a15, REGION_GFX1, 0x100006, LOAD_8_64 },
  { "lw_31.9f", 0x20000, 0xc49d37fb, REGION_GFX1, 0x100007, LOAD_8_64 },
  { "lw-02.6b", 0x80000, 0x43e6c5c8, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "lw_14.10b", 0x20000, 0x82862cce, REGION_GFX1, 0x200002, LOAD_8_64 },
  { "lw_13.10a", 0x20000, 0xb81c0e96, REGION_GFX1, 0x200003, LOAD_8_64 },
  { "lw-06.9d", 0x80000, 0x5b9edffc, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "lw-13.10d", 0x80000, 0x8e058ef5, REGION_GFX1, 0x200006, LOAD_16_64 },
  { "lw_16.11b", 0x20000, 0x40b26554, REGION_GFX1, 0x300002, LOAD_8_64 },
  { "lw_15.11a", 0x20000, 0x1b7d2e07, REGION_GFX1, 0x300003, LOAD_8_64 },
/* Ignored : 	ROM_COPY( "gfx", 0x200000, 0x000000, 0x8000 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_forgottnu1[] = // clone of forgottn
{
  LOAD_16_8( CPU1, "lw11c.14f", 0x00000, 0x20000, 0xe62742b6),
  LOAD_16_8( CPU1, "lw15c.14g", 0x00001, 0x20000, 0x1b70f216),
  LOAD_16_8( CPU1, "lw10c.13f", 0x40000, 0x20000, 0x8f5ea3f5),
  LOAD_16_8( CPU1, "lw14c.13g", 0x40001, 0x20000, 0x708e7472),
  LOAD_SW16( CPU1, "lw-07.13e", 0x80000, 0x80000, 0xfd252a26),
  { "lw-01.9d", 0x80000, 0x0318f298, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "lw-08.9f", 0x80000, 0x25a8e43c, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "lw-05.9e", 0x80000, 0xe4552fd7, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "lw-12.9g", 0x80000, 0x8e6a832b, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "lw-02.12d", 0x80000, 0x43e6c5c8, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "lw-09.12f", 0x80000, 0x899cb4ad, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "lw-06.12e", 0x80000, 0x5b9edffc, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "lw-13.12g", 0x80000, 0x8e058ef5, REGION_GFX1, 0x200006, LOAD_16_64 },
/* Ignored : 	ROM_COPY( "gfx", 0x200000, 0x000000, 0x8000 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_forgottnua[] = // clone of forgottn
{
  LOAD_16_8( CPU1, "lwu_11a.14f", 0x00000, 0x20000, 0xddf78831),
  LOAD_16_8( CPU1, "lwu_15a.14g", 0x00001, 0x20000, 0xf7ce2097),
  LOAD_16_8( CPU1, "lwu_10a.13f", 0x40000, 0x20000, 0x8cb38c81),
  LOAD_16_8( CPU1, "lwu_14a.13g", 0x40001, 0x20000, 0xd70ef9fd),
  LOAD_SW16( CPU1, "lw-07.13e", 0x80000, 0x80000, 0xfd252a26),
  { "lw-01.9d", 0x80000, 0x0318f298, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "lw-08.9f", 0x80000, 0x25a8e43c, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "lw-05.9e", 0x80000, 0xe4552fd7, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "lw-12.9g", 0x80000, 0x8e6a832b, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "lw-02.12d", 0x80000, 0x43e6c5c8, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "lw-09.12f", 0x80000, 0x899cb4ad, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "lw-06.12e", 0x80000, 0x5b9edffc, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "lw-13.12g", 0x80000, 0x8e058ef5, REGION_GFX1, 0x200006, LOAD_16_64 },
/* Ignored : 	ROM_COPY( "gfx", 0x200000, 0x000000, 0x8000 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_forgottnuaa[] = /* 1 byte difference to parent set. Region byte or pointer to the US "warning" screen at boot clone of forgottn */
{
  LOAD_16_8( CPU1, "lwu_11aa.14f", 0x00000, 0x20000, 0x73e920b7),
  LOAD_16_8( CPU1, "lwu_15aa.14g", 0x00001, 0x20000, 0xe47524b9),
  LOAD_16_8( CPU1, "lwu_10aa.13f", 0x40000, 0x20000, 0xbea45994),
  LOAD_16_8( CPU1, "lwu_14aa.13g", 0x40001, 0x20000, 0x539b2339),
  LOAD_SW16( CPU1, "lw-07.13e", 0x80000, 0x80000, 0xfd252a26),
  { "lw-01.9d", 0x80000, 0x0318f298, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "lw-08.9f", 0x80000, 0x25a8e43c, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "lw-05.9e", 0x80000, 0xe4552fd7, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "lw-12.9g", 0x80000, 0x8e6a832b, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "lw-02.12d", 0x80000, 0x43e6c5c8, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "lw-09.12f", 0x80000, 0x899cb4ad, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "lw-06.12e", 0x80000, 0x5b9edffc, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "lw-13.12g", 0x80000, 0x8e058ef5, REGION_GFX1, 0x200006, LOAD_16_64 },
/* Ignored : 	ROM_COPY( "gfx", 0x200000, 0x000000, 0x8000 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_lostwrld[] = // clone of forgottn
{
  LOAD_16_8( CPU1, "lw_11c.14f", 0x00000, 0x20000, 0x67e42546),
  LOAD_16_8( CPU1, "lw_15c.14g", 0x00001, 0x20000, 0x402e2a46),
  LOAD_16_8( CPU1, "lw_10c.13f", 0x40000, 0x20000, 0xc46479d7),
  LOAD_16_8( CPU1, "lw_14c.13g", 0x40001, 0x20000, 0x97670f4a),
  LOAD_SW16( CPU1, "lw-07.13e", 0x80000, 0x80000, 0xfd252a26),
  { "lw-01.9d", 0x80000, 0x0318f298, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "lw-08.9f", 0x80000, 0x25a8e43c, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "lw-05.9e", 0x80000, 0xe4552fd7, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "lw-12.9g", 0x80000, 0x8e6a832b, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "lw-02.12d", 0x80000, 0x43e6c5c8, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "lw-09.12f", 0x80000, 0x899cb4ad, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "lw-06.12e", 0x80000, 0x5b9edffc, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "lw-13.12g", 0x80000, 0x8e058ef5, REGION_GFX1, 0x200006, LOAD_16_64 },
/* Ignored : 	ROM_COPY( "gfx", 0x200000, 0x000000, 0x8000 ) */
  LOAD( SMP1, "lw-03.14c", 0x00000, 0x20000, 0xce2159e7),
  LOAD( SMP1, "lw-04.13c", 0x20000, 0x20000, 0x39305536),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_lostwrldo[] = // clone of forgottn
{
  LOAD_16_8( CPU1, "lw_11.14f", 0x00000, 0x20000, 0x61e2cc56),
  LOAD_16_8( CPU1, "lw_15.14g", 0x00001, 0x20000, 0x8a0c18d3),
  LOAD_16_8( CPU1, "lw_10.13f", 0x40000, 0x20000, 0x23bca4d5),
  LOAD_16_8( CPU1, "lw_14.13g", 0x40001, 0x20000, 0x3a023771),
  LOAD_SW16( CPU1, "lw-07.13e", 0x80000, 0x80000, 0xfd252a26),
  { "lw-01.9d", 0x80000, 0x0318f298, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "lw-08.9f", 0x80000, 0x25a8e43c, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "lw-05.9e", 0x80000, 0xe4552fd7, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "lw-12.9g", 0x80000, 0x8e6a832b, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "lw-02.12d", 0x80000, 0x43e6c5c8, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "lw-09.12f", 0x80000, 0x899cb4ad, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "lw-06.12e", 0x80000, 0x5b9edffc, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "lw-13.12g", 0x80000, 0x8e058ef5, REGION_GFX1, 0x200006, LOAD_16_64 },
/* Ignored : 	ROM_COPY( "gfx", 0x200000, 0x000000, 0x8000 ) */
  LOAD( SMP1, "lw-03.14c", 0x00000, 0x20000, 0xce2159e7),
  LOAD( SMP1, "lw-04.13c", 0x20000, 0x20000, 0x39305536),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ghouls[] =
{
  LOAD_16_8( CPU1, "dme_29.10h", 0x00000, 0x20000, 0x166a58a2),
  LOAD_16_8( CPU1, "dme_30.10j", 0x00001, 0x20000, 0x7ac8407a),
  LOAD_16_8( CPU1, "dme_27.9h", 0x40000, 0x20000, 0xf734b2be),
  LOAD_16_8( CPU1, "dme_28.9j", 0x40001, 0x20000, 0x03d3e714),
  LOAD( CPU1, "dm-17.7j", 0x80000, 0x80000, 0x3ea1b0f2),
  { "dm-05.3a", 0x80000, 0x0ba9c0b0, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "dm-07.3f", 0x80000, 0x5d760ab9, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "dm-06.3c", 0x80000, 0x4ba90b59, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "dm-08.3g", 0x80000, 0x4bdee9de, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "09.4a", 0x10000, 0xae24bb19, REGION_GFX1, 0x200000, LOAD_8_64 },
  { "18.7a", 0x10000, 0xd34e271a, REGION_GFX1, 0x200001, LOAD_8_64 },
  { "13.4e", 0x10000, 0x3f70dd37, REGION_GFX1, 0x200002, LOAD_8_64 },
  { "22.7e", 0x10000, 0x7e69e2e6, REGION_GFX1, 0x200003, LOAD_8_64 },
  { "11.4c", 0x10000, 0x37c9b6c6, REGION_GFX1, 0x200004, LOAD_8_64 },
  { "20.7c", 0x10000, 0x2f1345b4, REGION_GFX1, 0x200005, LOAD_8_64 },
  { "15.4g", 0x10000, 0x3c2a212a, REGION_GFX1, 0x200006, LOAD_8_64 },
  { "24.7g", 0x10000, 0x889aac05, REGION_GFX1, 0x200007, LOAD_8_64 },
  { "10.4b", 0x10000, 0xbcc0f28c, REGION_GFX1, 0x280000, LOAD_8_64 },
  { "19.7b", 0x10000, 0x2a40166a, REGION_GFX1, 0x280001, LOAD_8_64 },
  { "14.4f", 0x10000, 0x20f85c03, REGION_GFX1, 0x280002, LOAD_8_64 },
  { "23.7f", 0x10000, 0x8426144b, REGION_GFX1, 0x280003, LOAD_8_64 },
  { "12.4d", 0x10000, 0xda088d61, REGION_GFX1, 0x280004, LOAD_8_64 },
  { "21.7d", 0x10000, 0x17e11df0, REGION_GFX1, 0x280005, LOAD_8_64 },
  { "16.4h", 0x10000, 0xf187ba1c, REGION_GFX1, 0x280006, LOAD_8_64 },
  { "25.7h", 0x10000, 0x29f79c78, REGION_GFX1, 0x280007, LOAD_8_64 },
  LOAD( ROM2, "26.10a", 0x00000, 0x10000, 0x3692f6e5),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ghoulsu[] = // clone of ghouls
{
  LOAD_16_8( CPU1, "dmu_29.10h", 0x00000, 0x20000, 0x334d85b2),
  LOAD_16_8( CPU1, "dmu_30.10j", 0x00001, 0x20000, 0xcee8ceb5),
  LOAD_16_8( CPU1, "dmu_27.9h", 0x40000, 0x20000, 0x4a524140),
  LOAD_16_8( CPU1, "dmu_28.9j", 0x40001, 0x20000, 0x94aae205),
  LOAD( CPU1, "dm-17.7j", 0x80000, 0x80000, 0x3ea1b0f2),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_daimakai[] = // clone of ghouls
{
  LOAD_16_8( CPU1, "dmj_38.12f", 0x00000, 0x20000, 0x82fd1798),
  LOAD_16_8( CPU1, "dmj_39.12h", 0x00001, 0x20000, 0x35366ccc),
  LOAD_16_8( CPU1, "dmj_40.13f", 0x40000, 0x20000, 0xa17c170a),
  LOAD_16_8( CPU1, "dmj_41.13h", 0x40001, 0x20000, 0x6af0b391),
  LOAD_16_8( CPU1, "dm_33.10f", 0x80000, 0x20000, 0x384d60c4),
  LOAD_16_8( CPU1, "dm_34.10h", 0x80001, 0x20000, 0x19abe30f),
  LOAD_16_8( CPU1, "dm_35.11f", 0xc0000, 0x20000, 0xc04b85c8),
  LOAD_16_8( CPU1, "dm_36.11h", 0xc0001, 0x20000, 0x89be83de),
  { "dm_02.4b", 0x20000, 0x8b98dc48, REGION_GFX1, 0x000000, LOAD_8_64 },
  { "dm_01.4a", 0x20000, 0x80896c33, REGION_GFX1, 0x000001, LOAD_8_64 },
  { "dm_10.9b", 0x20000, 0xc2e7d9ef, REGION_GFX1, 0x000002, LOAD_8_64 },
  { "dm_09.9a", 0x20000, 0xc9c4afa5, REGION_GFX1, 0x000003, LOAD_8_64 },
  { "dm_18.5e", 0x20000, 0x1aa0db99, REGION_GFX1, 0x000004, LOAD_8_64 },
  { "dm_17.5c", 0x20000, 0xdc6ed8ad, REGION_GFX1, 0x000005, LOAD_8_64 },
  { "dm_30.8h", 0x20000, 0xd9d3f8bd, REGION_GFX1, 0x000006, LOAD_8_64 },
  { "dm_29.8f", 0x20000, 0x49a48796, REGION_GFX1, 0x000007, LOAD_8_64 },
  { "dm_04.5b", 0x20000, 0xa4f4f8f0, REGION_GFX1, 0x100000, LOAD_8_64 },
  { "dm_03.5a", 0x20000, 0xb1033e62, REGION_GFX1, 0x100001, LOAD_8_64 },
  { "dm_12.10b", 0x20000, 0x10fdd76a, REGION_GFX1, 0x100002, LOAD_8_64 },
  { "dm_11.10a", 0x20000, 0x9040cb04, REGION_GFX1, 0x100003, LOAD_8_64 },
  { "dm_20.7e", 0x20000, 0x281d0b3e, REGION_GFX1, 0x100004, LOAD_8_64 },
  { "dm_19.7c", 0x20000, 0x2623b52f, REGION_GFX1, 0x100005, LOAD_8_64 },
  { "dm_32.9h", 0x20000, 0x99692344, REGION_GFX1, 0x100006, LOAD_8_64 },
  { "dm_31.9f", 0x20000, 0x54acb729, REGION_GFX1, 0x100007, LOAD_8_64 },
  { "dm_06.7b", 0x10000, 0xae24bb19, REGION_GFX1, 0x200000, LOAD_8_64 },
  { "dm_05.7a", 0x10000, 0xd34e271a, REGION_GFX1, 0x200001, LOAD_8_64 },
  { "dm_14.11b", 0x10000, 0x3f70dd37, REGION_GFX1, 0x200002, LOAD_8_64 },
  { "dm_13.11a", 0x10000, 0x7e69e2e6, REGION_GFX1, 0x200003, LOAD_8_64 },
  { "dm_22.8e", 0x10000, 0x37c9b6c6, REGION_GFX1, 0x200004, LOAD_8_64 },
  { "dm_21.8c", 0x10000, 0x2f1345b4, REGION_GFX1, 0x200005, LOAD_8_64 },
  { "dm_26.10e", 0x10000, 0x3c2a212a, REGION_GFX1, 0x200006, LOAD_8_64 },
  { "dm_25.10c", 0x10000, 0x889aac05, REGION_GFX1, 0x200007, LOAD_8_64 },
  { "dm_08.8b", 0x10000, 0xbcc0f28c, REGION_GFX1, 0x280000, LOAD_8_64 },
  { "dm_07.8a", 0x10000, 0x2a40166a, REGION_GFX1, 0x280001, LOAD_8_64 },
  { "dm_16.12b", 0x10000, 0x20f85c03, REGION_GFX1, 0x280002, LOAD_8_64 },
  { "dm_15.12a", 0x10000, 0x8426144b, REGION_GFX1, 0x280003, LOAD_8_64 },
  { "dm_24.9e", 0x10000, 0xda088d61, REGION_GFX1, 0x280004, LOAD_8_64 },
  { "dm_23.9c", 0x10000, 0x17e11df0, REGION_GFX1, 0x280005, LOAD_8_64 },
  { "dm_28.11e", 0x10000, 0xf187ba1c, REGION_GFX1, 0x280006, LOAD_8_64 },
  { "dm_27.11c", 0x10000, 0x29f79c78, REGION_GFX1, 0x280007, LOAD_8_64 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_daimakair[] = // clone of ghouls
{
  LOAD_SW16( CPU1, "damj_23.8f", 0x00000, 0x80000, 0xc3b248ec),
  LOAD_SW16( CPU1, "damj_22.7f", 0x80000, 0x80000, 0x595ff2f3),
  { "dam_01.3a", 0x80000, 0x0ba9c0b0, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "dam_02.4a", 0x80000, 0x5d760ab9, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "dam_03.5a", 0x80000, 0x4ba90b59, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "dam_04.6a", 0x80000, 0x4bdee9de, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "dam_05.7a", 0x80000, 0x7dc61b94, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "dam_06.8a", 0x80000, 0xfde89758, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "dam_07.9a", 0x80000, 0xec351d78, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "dam_08.10a", 0x80000, 0xee2acc1e, REGION_GFX1, 0x200006, LOAD_16_64 },
  LOAD( ROM2, "dam_09.12a", 0x00000, 0x20000, 0x0656ff53),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_strider[] =
{
  LOAD_16_8( CPU1, "30.11f", 0x00000, 0x20000, 0xda997474),
  LOAD_16_8( CPU1, "35.11h", 0x00001, 0x20000, 0x5463aaa3),
  LOAD_16_8( CPU1, "31.12f", 0x40000, 0x20000, 0xd20786db),
  LOAD_16_8( CPU1, "36.12h", 0x40001, 0x20000, 0x21aa2863),
  LOAD_SW16( CPU1, "st-14.8h", 0x80000, 0x80000, 0x9b3cfc08),
  { "st-2.8a", 0x80000, 0x4eee9aea, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "st-11.10a", 0x80000, 0x2d7f21e4, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "st-5.4a", 0x80000, 0x7705aa46, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "st-9.6a", 0x80000, 0x5b18b722, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "st-1.7a", 0x80000, 0x005f000b, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "st-10.9a", 0x80000, 0xb9441519, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "st-4.3a", 0x80000, 0xb7d04e8b, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "st-8.5a", 0x80000, 0x6b4713b4, REGION_GFX1, 0x200006, LOAD_16_64 },
/* Ignored : 	ROM_COPY( "gfx", 0x000000, 0x000000, 0x8000 ) */
  LOAD( ROM2, "09.12b", 0x00000, 0x10000, 0x2ed403bc),
  LOAD( SMP1, "18.11c", 0x00000, 0x20000, 0x4386bc80),
  LOAD( SMP1, "19.12c", 0x20000, 0x20000, 0x444536d7),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_striderua[] = // clone of strider
{
  LOAD_16_8( CPU1, "30(__striderua).11f", 0x00000, 0x20000, 0x66aec273),
  LOAD_16_8( CPU1, "35(__striderua).11h", 0x00001, 0x20000, 0x50e0e865),
  LOAD_16_8( CPU1, "31(__striderua).12f", 0x40000, 0x20000, 0xeae93bd1),
  LOAD_16_8( CPU1, "36(__striderua).12h", 0x40001, 0x20000, 0xb904a31d),
  LOAD_SW16( CPU1, "st-14.8h", 0x80000, 0x80000, 0x9b3cfc08),
/* Ignored : 	ROM_COPY( "gfx", 0x000000, 0x000000, 0x8000 ) */
  LOAD( ROM2, "09(__striderua).12b", 0x00000, 0x10000, 0x08d63519),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_striderj[] = // clone of strider
{
  LOAD_16_8( CPU1, "sth36.bin", 0x00000, 0x20000, 0x53c7b006),
  LOAD_16_8( CPU1, "sth42.bin", 0x00001, 0x20000, 0x4037f65f),
  LOAD_16_8( CPU1, "sth37.bin", 0x40000, 0x20000, 0x80e8877d),
  LOAD_16_8( CPU1, "sth43.bin", 0x40001, 0x20000, 0x6b3fa466),
  LOAD_16_8( CPU1, "sth34.bin", 0x80000, 0x20000, 0xbea770b5),
  LOAD_16_8( CPU1, "sth40.bin", 0x80001, 0x20000, 0x43b922dc),
  LOAD_16_8( CPU1, "sth35.bin", 0xc0000, 0x20000, 0x5cc429da),
  LOAD_16_8( CPU1, "sth41.bin", 0xc0001, 0x20000, 0x50af457f),
  { "sth09.bin", 0x20000, 0x1ef6bfbd, REGION_GFX1, 0x000000, LOAD_8_64 },
  { "sth01.bin", 0x20000, 0x1e21b0c1, REGION_GFX1, 0x000001, LOAD_8_64 },
  { "sth13.bin", 0x20000, 0x063263ae, REGION_GFX1, 0x000002, LOAD_8_64 },
  { "sth05.bin", 0x20000, 0xec9f8714, REGION_GFX1, 0x000003, LOAD_8_64 },
  { "sth24.bin", 0x20000, 0x6356f4d2, REGION_GFX1, 0x000004, LOAD_8_64 },
  { "sth17.bin", 0x20000, 0xb4f73d86, REGION_GFX1, 0x000005, LOAD_8_64 },
  { "sth38.bin", 0x20000, 0xee5abfc2, REGION_GFX1, 0x000006, LOAD_8_64 },
  { "sth32.bin", 0x20000, 0x44a206a3, REGION_GFX1, 0x000007, LOAD_8_64 },
  { "sth10.bin", 0x20000, 0xdf3dd3bc, REGION_GFX1, 0x100000, LOAD_8_64 },
  { "sth02.bin", 0x20000, 0xc75f9ea0, REGION_GFX1, 0x100001, LOAD_8_64 },
  { "sth14.bin", 0x20000, 0x6c03e19d, REGION_GFX1, 0x100002, LOAD_8_64 },
  { "sth06.bin", 0x20000, 0xd84f5478, REGION_GFX1, 0x100003, LOAD_8_64 },
  { "sth25.bin", 0x20000, 0x921e506a, REGION_GFX1, 0x100004, LOAD_8_64 },
  { "sth18.bin", 0x20000, 0x5b318956, REGION_GFX1, 0x100005, LOAD_8_64 },
  { "sth39.bin", 0x20000, 0x9321d6aa, REGION_GFX1, 0x100006, LOAD_8_64 },
  { "sth33.bin", 0x20000, 0xb47ddfc7, REGION_GFX1, 0x100007, LOAD_8_64 },
  { "sth11.bin", 0x20000, 0x2484f241, REGION_GFX1, 0x200000, LOAD_8_64 },
  { "sth03.bin", 0x20000, 0xaaa07245, REGION_GFX1, 0x200001, LOAD_8_64 },
  { "sth15.bin", 0x20000, 0xe415d943, REGION_GFX1, 0x200002, LOAD_8_64 },
  { "sth07.bin", 0x20000, 0x97d072d2, REGION_GFX1, 0x200003, LOAD_8_64 },
  { "sth26.bin", 0x20000, 0x0ebfcb02, REGION_GFX1, 0x200004, LOAD_8_64 },
  { "sth19.bin", 0x20000, 0x257ce683, REGION_GFX1, 0x200005, LOAD_8_64 },
  { "sth28.bin", 0x20000, 0x98ac8cd1, REGION_GFX1, 0x200006, LOAD_8_64 },
  { "sth21.bin", 0x20000, 0x538d9423, REGION_GFX1, 0x200007, LOAD_8_64 },
  { "sth12.bin", 0x20000, 0xf670a477, REGION_GFX1, 0x300000, LOAD_8_64 },
  { "sth04.bin", 0x20000, 0x853d3e01, REGION_GFX1, 0x300001, LOAD_8_64 },
  { "sth16.bin", 0x20000, 0x4092019f, REGION_GFX1, 0x300002, LOAD_8_64 },
  { "sth08.bin", 0x20000, 0x2ce9b4c7, REGION_GFX1, 0x300003, LOAD_8_64 },
  { "sth27.bin", 0x20000, 0xf82c88d9, REGION_GFX1, 0x300004, LOAD_8_64 },
  { "sth20.bin", 0x20000, 0xeb584dd4, REGION_GFX1, 0x300005, LOAD_8_64 },
  { "sth29.bin", 0x20000, 0x34ae2997, REGION_GFX1, 0x300006, LOAD_8_64 },
  { "sth22.bin", 0x20000, 0x78dd9c48, REGION_GFX1, 0x300007, LOAD_8_64 },
/* Ignored : 	ROM_COPY( "gfx", 0x000000, 0x000000, 0x8000 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_striderjr[] = // clone of strider
{
  LOAD_SW16( CPU1, "sthj_23.8f", 0x00000, 0x80000, 0x046e7b12),
  LOAD_SW16( CPU1, "sthj_22.7f", 0x80000, 0x80000, 0x9b3cfc08),
/* Ignored : 	ROM_COPY( "gfx", 0x000000, 0x000000, 0x8000 ) */
  LOAD( ROM2, "sth_09.12a", 0x00000, 0x10000, 0x08d63519),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_dynwar[] =
{
  LOAD_16_8( CPU1, "30.11f", 0x00000, 0x20000, 0xf9ec6d68),
  LOAD_16_8( CPU1, "35.11h", 0x00001, 0x20000, 0xe41fff2f),
  LOAD_16_8( CPU1, "31.12f", 0x40000, 0x20000, 0xe3de76ff),
  LOAD_16_8( CPU1, "36.12h", 0x40001, 0x20000, 0x7a13cfbf),
  LOAD_SW16( CPU1, "tkm-9.8h", 0x80000, 0x80000, 0x93654bcf),
  { "tkm-5.7a", 0x80000, 0xf64bb6a0, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "tkm-8.9a", 0x80000, 0x21fe6274, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "tkm-6.3a", 0x80000, 0x0bf228cb, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "tkm-7.5a", 0x80000, 0x1255dfb1, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "tkm-1.8a", 0x80000, 0x44f7661e, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "tkm-4.10a", 0x80000, 0xa54c515d, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "tkm-2.4a", 0x80000, 0xca5c687c, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "tkm-3.6a", 0x80000, 0xf9fe6591, REGION_GFX1, 0x200006, LOAD_16_64 },
  LOAD( ROM2, "tke_17.12b", 0x00000, 0x10000, 0xb3b79d4f),
  LOAD( SMP1, "tke_18.11c", 0x00000, 0x20000, 0xac6e307d),
  LOAD( SMP1, "tke_19.12c", 0x20000, 0x20000, 0x068741db),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_dynwara[] = // clone of dynwar
{
  LOAD_16_8( CPU1, "tke_36.12f", 0x00000, 0x20000, 0x895991d1),
  LOAD_16_8( CPU1, "tke_42.12h", 0x00001, 0x20000, 0xc898d2e8),
  LOAD_16_8( CPU1, "tke_37.13f", 0x40000, 0x20000, 0xb228d58c),
  LOAD_16_8( CPU1, "tke_43.13h", 0x40001, 0x20000, 0x1a14375a),
  LOAD_16_8( CPU1, "34.10f", 0x80000, 0x20000, 0x8f663d00),
  LOAD_16_8( CPU1, "40.10h", 0x80001, 0x20000, 0x1586dbf3),
  LOAD_16_8( CPU1, "35.11f", 0xc0000, 0x20000, 0x9db93d7a),
  LOAD_16_8( CPU1, "41.11h", 0xc0001, 0x20000, 0x1aae69a4),
  { "09.4b", 0x20000, 0xc3e83c69, REGION_GFX1, 0x000000, LOAD_8_64 },
  { "01.4a", 0x20000, 0x187b2886, REGION_GFX1, 0x000001, LOAD_8_64 },
  { "13.9b", 0x20000, 0x0273d87d, REGION_GFX1, 0x000002, LOAD_8_64 },
  { "05.9a", 0x20000, 0x339378b8, REGION_GFX1, 0x000003, LOAD_8_64 },
  { "24.5e", 0x20000, 0xc6909b6f, REGION_GFX1, 0x000004, LOAD_8_64 },
  { "17.5c", 0x20000, 0x2e2f8320, REGION_GFX1, 0x000005, LOAD_8_64 },
  { "38.8h", 0x20000, 0xcd7923ed, REGION_GFX1, 0x000006, LOAD_8_64 },
  { "32.8f", 0x20000, 0x21a0a453, REGION_GFX1, 0x000007, LOAD_8_64 },
  { "10.5b", 0x20000, 0xff28f8d0, REGION_GFX1, 0x100000, LOAD_8_64 },
  { "02.5a", 0x20000, 0xcc83c02f, REGION_GFX1, 0x100001, LOAD_8_64 },
  { "14.10b", 0x20000, 0x58d9b32f, REGION_GFX1, 0x100002, LOAD_8_64 },
  { "06.10a", 0x20000, 0x6f9edd75, REGION_GFX1, 0x100003, LOAD_8_64 },
  { "25.7e", 0x20000, 0x152ea74a, REGION_GFX1, 0x100004, LOAD_8_64 },
  { "18.7c", 0x20000, 0x1833f932, REGION_GFX1, 0x100005, LOAD_8_64 },
  { "39.9h", 0x20000, 0xbc09b360, REGION_GFX1, 0x100006, LOAD_8_64 },
  { "33.9f", 0x20000, 0x89de1533, REGION_GFX1, 0x100007, LOAD_8_64 },
  { "11.7b", 0x20000, 0x29eaf490, REGION_GFX1, 0x200000, LOAD_8_64 },
  { "03.7a", 0x20000, 0x7bf51337, REGION_GFX1, 0x200001, LOAD_8_64 },
  { "15.11b", 0x20000, 0xd36cdb91, REGION_GFX1, 0x200002, LOAD_8_64 },
  { "07.11a", 0x20000, 0xe04af054, REGION_GFX1, 0x200003, LOAD_8_64 },
  { "26.8e", 0x20000, 0x07fc714b, REGION_GFX1, 0x200004, LOAD_8_64 },
  { "19.8c", 0x20000, 0x7114e5c6, REGION_GFX1, 0x200005, LOAD_8_64 },
  { "28.10e", 0x20000, 0xaf62bf07, REGION_GFX1, 0x200006, LOAD_8_64 },
  { "21.10c", 0x20000, 0x523f462a, REGION_GFX1, 0x200007, LOAD_8_64 },
  { "12.8b", 0x20000, 0x38652339, REGION_GFX1, 0x300000, LOAD_8_64 },
  { "04.8a", 0x20000, 0x4951bc0f, REGION_GFX1, 0x300001, LOAD_8_64 },
  { "16.12b", 0x20000, 0x381608ae, REGION_GFX1, 0x300002, LOAD_8_64 },
  { "08.12a", 0x20000, 0xb475d4e9, REGION_GFX1, 0x300003, LOAD_8_64 },
  { "27.9e", 0x20000, 0xa27e81fa, REGION_GFX1, 0x300004, LOAD_8_64 },
  { "20.9c", 0x20000, 0x002796dc, REGION_GFX1, 0x300005, LOAD_8_64 },
  { "29.11e", 0x20000, 0x6b41f82d, REGION_GFX1, 0x300006, LOAD_8_64 },
  { "22.11c", 0x20000, 0x52145369, REGION_GFX1, 0x300007, LOAD_8_64 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_dynwarj[] = // clone of dynwar
{
  LOAD_16_8( CPU1, "36.12f", 0x00000, 0x20000, 0x1a516657),
  LOAD_16_8( CPU1, "42.12h", 0x00001, 0x20000, 0x12a290a0),
  LOAD_16_8( CPU1, "37.13f", 0x40000, 0x20000, 0x932fc943),
  LOAD_16_8( CPU1, "43.13h", 0x40001, 0x20000, 0x872ad76d),
  LOAD_16_8( CPU1, "34.10f", 0x80000, 0x20000, 0x8f663d00),
  LOAD_16_8( CPU1, "40.10h", 0x80001, 0x20000, 0x1586dbf3),
  LOAD_16_8( CPU1, "35.11f", 0xc0000, 0x20000, 0x9db93d7a),
  LOAD_16_8( CPU1, "41.11h", 0xc0001, 0x20000, 0x1aae69a4),
  { "09.4b", 0x20000, 0xc3e83c69, REGION_GFX1, 0x000000, LOAD_8_64 },
  { "01.4a", 0x20000, 0x187b2886, REGION_GFX1, 0x000001, LOAD_8_64 },
  { "13.9b", 0x20000, 0x0273d87d, REGION_GFX1, 0x000002, LOAD_8_64 },
  { "05.9a", 0x20000, 0x339378b8, REGION_GFX1, 0x000003, LOAD_8_64 },
  { "24.5e", 0x20000, 0xc6909b6f, REGION_GFX1, 0x000004, LOAD_8_64 },
  { "17.5c", 0x20000, 0x2e2f8320, REGION_GFX1, 0x000005, LOAD_8_64 },
  { "38.8h", 0x20000, 0xcd7923ed, REGION_GFX1, 0x000006, LOAD_8_64 },
  { "32.8f", 0x20000, 0x21a0a453, REGION_GFX1, 0x000007, LOAD_8_64 },
  { "10.5b", 0x20000, 0xff28f8d0, REGION_GFX1, 0x100000, LOAD_8_64 },
  { "02.5a", 0x20000, 0xcc83c02f, REGION_GFX1, 0x100001, LOAD_8_64 },
  { "14(__dynwarj).10b", 0x20000, 0x18fb232c, REGION_GFX1, 0x100002, LOAD_8_64 },
  { "06.10a", 0x20000, 0x6f9edd75, REGION_GFX1, 0x100003, LOAD_8_64 },
  { "25.7e", 0x20000, 0x152ea74a, REGION_GFX1, 0x100004, LOAD_8_64 },
  { "18.7c", 0x20000, 0x1833f932, REGION_GFX1, 0x100005, LOAD_8_64 },
  { "39.9h", 0x20000, 0xbc09b360, REGION_GFX1, 0x100006, LOAD_8_64 },
  { "33.9f", 0x20000, 0x89de1533, REGION_GFX1, 0x100007, LOAD_8_64 },
  { "11.7b", 0x20000, 0x29eaf490, REGION_GFX1, 0x200000, LOAD_8_64 },
  { "03.7a", 0x20000, 0x7bf51337, REGION_GFX1, 0x200001, LOAD_8_64 },
  { "15.11b", 0x20000, 0xd36cdb91, REGION_GFX1, 0x200002, LOAD_8_64 },
  { "07.11a", 0x20000, 0xe04af054, REGION_GFX1, 0x200003, LOAD_8_64 },
  { "26.8e", 0x20000, 0x07fc714b, REGION_GFX1, 0x200004, LOAD_8_64 },
  { "19.8c", 0x20000, 0x7114e5c6, REGION_GFX1, 0x200005, LOAD_8_64 },
  { "28.10e", 0x20000, 0xaf62bf07, REGION_GFX1, 0x200006, LOAD_8_64 },
  { "21.10c", 0x20000, 0x523f462a, REGION_GFX1, 0x200007, LOAD_8_64 },
  { "12.8b", 0x20000, 0x38652339, REGION_GFX1, 0x300000, LOAD_8_64 },
  { "04.8a", 0x20000, 0x4951bc0f, REGION_GFX1, 0x300001, LOAD_8_64 },
  { "16.12b", 0x20000, 0x381608ae, REGION_GFX1, 0x300002, LOAD_8_64 },
  { "08.12a", 0x20000, 0xb475d4e9, REGION_GFX1, 0x300003, LOAD_8_64 },
  { "27.9e", 0x20000, 0xa27e81fa, REGION_GFX1, 0x300004, LOAD_8_64 },
  { "20.9c", 0x20000, 0x002796dc, REGION_GFX1, 0x300005, LOAD_8_64 },
  { "29.11e", 0x20000, 0x6b41f82d, REGION_GFX1, 0x300006, LOAD_8_64 },
  { "22.11c", 0x20000, 0x52145369, REGION_GFX1, 0x300007, LOAD_8_64 },
  LOAD( SMP1, "30.12e", 0x00000, 0x20000, 0x7e5f6cb4),
  LOAD( SMP1, "31.13e", 0x20000, 0x20000, 0x4a30c737),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_dynwarjr[] = // clone of dynwar
{
  LOAD_SW16( CPU1, "tk1j_23.8f", 0x00000, 0x80000, 0x088a3009),
  LOAD_SW16( CPU1, "tk1j_22.7f", 0x80000, 0x80000, 0x93654bcf),
  LOAD( ROM2, "tk1_09.12a", 0x00000, 0x20000, 0xdb77d899),
  LOAD( SMP1, "tk1_18.11c", 0x00000, 0x20000, 0x7e5f6cb4),
  LOAD( SMP1, "tk1_19.12c", 0x20000, 0x20000, 0x4a30c737),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_willow[] =
{
  LOAD_16_8( CPU1, "wlu_30.11f", 0x00000, 0x20000, 0xd604dbb1),
  LOAD_16_8( CPU1, "35.11h", 0x00001, 0x20000, 0x7a791e77),
  LOAD_16_8( CPU1, "wlu_31.12f", 0x40000, 0x20000, 0x0eb48a83),
  LOAD_16_8( CPU1, "wlu_36.12h", 0x40001, 0x20000, 0x36100209),
  LOAD_SW16( CPU1, "wlm-32.8h", 0x80000, 0x80000, 0xdfd9f643),
  { "wlm-7.7a", 0x80000, 0xafa74b73, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "wlm-5.9a", 0x80000, 0x12a0dc0b, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "wlm-3.3a", 0x80000, 0xc6f2abce, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "wlm-1.5a", 0x80000, 0x4aa4c6d3, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "wl_24.7d", 0x20000, 0x6f0adee5, REGION_GFX1, 0x200000, LOAD_8_64 },
  { "wl_14.7c", 0x20000, 0x9cf3027d, REGION_GFX1, 0x200001, LOAD_8_64 },
  { "wl_26.9d", 0x20000, 0xf09c8ecf, REGION_GFX1, 0x200002, LOAD_8_64 },
  { "wl_16.9c", 0x20000, 0xe35407aa, REGION_GFX1, 0x200003, LOAD_8_64 },
  { "wl_20.3d", 0x20000, 0x84992350, REGION_GFX1, 0x200004, LOAD_8_64 },
  { "wl_10.3c", 0x20000, 0xb87b5a36, REGION_GFX1, 0x200005, LOAD_8_64 },
  { "wl_22.5d", 0x20000, 0xfd3f89f0, REGION_GFX1, 0x200006, LOAD_8_64 },
  { "wl_12.5c", 0x20000, 0x7da49d69, REGION_GFX1, 0x200007, LOAD_8_64 },
  LOAD( ROM2, "wl_09.12b", 0x00000, 0x10000, 0xf6b3d060),
  LOAD( SMP1, "wl_18.11c", 0x00000, 0x20000, 0xbde23d4d),
  LOAD( SMP1, "wl_19.12c", 0x20000, 0x20000, 0x683898f5),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_willowo[] = // clone of willow
{
  LOAD_16_8( CPU1, "wlu_30.11f", 0x00000, 0x20000, 0xd604dbb1),
  LOAD_16_8( CPU1, "wlu_35.11h", 0x00001, 0x20000, 0xdaee72fe),
  LOAD_16_8( CPU1, "wlu_31.12f", 0x40000, 0x20000, 0x0eb48a83),
  LOAD_16_8( CPU1, "wlu_36.12h", 0x40001, 0x20000, 0x36100209),
  LOAD_SW16( CPU1, "wlm-32.8h", 0x80000, 0x80000, 0xdfd9f643),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_willowj[] = // clone of willow
{
  LOAD_16_8( CPU1, "wl_36.12f", 0x00000, 0x20000, 0x2b0d7cbc),
  LOAD_16_8( CPU1, "wl_42.12h", 0x00001, 0x20000, 0x1ac39615),
  LOAD_16_8( CPU1, "wl_37.13f", 0x40000, 0x20000, 0x30a717fa),
  LOAD_16_8( CPU1, "wl_43.13h", 0x40001, 0x20000, 0xd0dddc9e),
  LOAD_16_8( CPU1, "wl_34.10f", 0x80000, 0x20000, 0x23a84f7a),
  LOAD_16_8( CPU1, "wl_40.10h", 0x80001, 0x20000, 0xc7a0ed21),
  LOAD_16_8( CPU1, "wl_35.11f", 0xc0000, 0x20000, 0x5eff7951),
  LOAD_16_8( CPU1, "wl_41.11h", 0xc0001, 0x20000, 0x8d6477a3),
  { "wl_09.4b", 0x20000, 0x05aa71b4, REGION_GFX1, 0x000000, LOAD_8_64 },
  { "wl_01.4a", 0x20000, 0x08c2df12, REGION_GFX1, 0x000001, LOAD_8_64 },
  { "wl_13.9b", 0x20000, 0x1f7c87cd, REGION_GFX1, 0x000002, LOAD_8_64 },
  { "wl_05.9a", 0x20000, 0xf5254bf2, REGION_GFX1, 0x000003, LOAD_8_64 },
  { "wl_24.5e", 0x20000, 0xd9d73ba1, REGION_GFX1, 0x000004, LOAD_8_64 },
  { "wl_17.5c", 0x20000, 0xa652f30c, REGION_GFX1, 0x000005, LOAD_8_64 },
  { "wl_38.8h", 0x20000, 0xf6f9111b, REGION_GFX1, 0x000006, LOAD_8_64 },
  { "wl_32.8f", 0x20000, 0x10f64027, REGION_GFX1, 0x000007, LOAD_8_64 },
  { "wl_10.5b", 0x20000, 0xdbba0a3f, REGION_GFX1, 0x100000, LOAD_8_64 },
  { "wl_02.5a", 0x20000, 0x86fba7a5, REGION_GFX1, 0x100001, LOAD_8_64 },
  { "wl_14.10b", 0x20000, 0x7d5798b2, REGION_GFX1, 0x100002, LOAD_8_64 },
  { "wl_06.10a", 0x20000, 0x1f052948, REGION_GFX1, 0x100003, LOAD_8_64 },
  { "wl_25.7e", 0x20000, 0x857d17d2, REGION_GFX1, 0x100004, LOAD_8_64 },
  { "wl_18.7c", 0x20000, 0x316c7fbc, REGION_GFX1, 0x100005, LOAD_8_64 },
  { "wl_39.9h", 0x20000, 0xe6fce9b0, REGION_GFX1, 0x100006, LOAD_8_64 },
  { "wl_33.9f", 0x20000, 0xa15d5517, REGION_GFX1, 0x100007, LOAD_8_64 },
  { "wl_11.7b", 0x20000, 0x6f0adee5, REGION_GFX1, 0x200000, LOAD_8_64 },
  { "wl_03.7a", 0x20000, 0x9cf3027d, REGION_GFX1, 0x200001, LOAD_8_64 },
  { "wl_15.11b", 0x20000, 0xf09c8ecf, REGION_GFX1, 0x200002, LOAD_8_64 },
  { "wl_07.11a", 0x20000, 0xe35407aa, REGION_GFX1, 0x200003, LOAD_8_64 },
  { "wl_26.8e", 0x20000, 0x84992350, REGION_GFX1, 0x200004, LOAD_8_64 },
  { "wl_19.8c", 0x20000, 0xb87b5a36, REGION_GFX1, 0x200005, LOAD_8_64 },
  { "wl_28.10e", 0x20000, 0xfd3f89f0, REGION_GFX1, 0x200006, LOAD_8_64 },
  { "wl_21.10c", 0x20000, 0x7da49d69, REGION_GFX1, 0x200007, LOAD_8_64 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_unsquad[] =
{
  LOAD_16_8( CPU1, "aru_30.11f", 0x00000, 0x20000, 0x24d8f88d),
  LOAD_16_8( CPU1, "aru_35.11h", 0x00001, 0x20000, 0x8b954b59),
  LOAD_16_8( CPU1, "aru_31.12f", 0x40000, 0x20000, 0x33e9694b),
  LOAD_16_8( CPU1, "aru_36.12h", 0x40001, 0x20000, 0x7cc8fb9e),
  LOAD_SW16( CPU1, "ar-32m.8h", 0x80000, 0x80000, 0xae1d7fb0),
  { "ar-5m.7a", 0x80000, 0xbf4575d8, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "ar-7m.9a", 0x80000, 0xa02945f4, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "ar-1m.3a", 0x80000, 0x5965ca8d, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "ar-3m.5a", 0x80000, 0xac6db17d, REGION_GFX1, 0x000006, LOAD_16_64 },
  LOAD( ROM2, "ar_09.12b", 0x00000, 0x10000, 0xf3dd1367),
	/* 20000-3ffff empty */
  LOAD( SMP1, "aru_18.11c", 0x00000, 0x20000, 0x584b43a9),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_area88[] = // clone of unsquad
{
  LOAD_16_8( CPU1, "ar_36.12f", 0x00000, 0x20000, 0x65030392),
  LOAD_16_8( CPU1, "ar_42.12h", 0x00001, 0x20000, 0xc48170de),
  LOAD_16_8( CPU1, "ar_37.13f", 0x40000, 0x20000, 0x33e9694b),
  LOAD_16_8( CPU1, "ar_43.13h", 0x40001, 0x20000, 0x7cc8fb9e),
  LOAD_16_8( CPU1, "ar_34.10f", 0x80000, 0x20000, 0xf6e80386),
  LOAD_16_8( CPU1, "ar_40.10h", 0x80001, 0x20000, 0xbe36c145),
  LOAD_16_8( CPU1, "ar_35.11f", 0xc0000, 0x20000, 0x86d98ff3),
  LOAD_16_8( CPU1, "ar_41.11h", 0xc0001, 0x20000, 0x758893d3),
  { "ar_09.4b", 0x20000, 0xdb9376f8, REGION_GFX1, 0x000000, LOAD_8_64 },
  { "ar_01.4a", 0x20000, 0x392151b4, REGION_GFX1, 0x000001, LOAD_8_64 },
  { "ar_13.9b", 0x20000, 0x81436481, REGION_GFX1, 0x000002, LOAD_8_64 },
  { "ar_05.9a", 0x20000, 0xe246ed9f, REGION_GFX1, 0x000003, LOAD_8_64 },
  { "ar_24.5e", 0x20000, 0x9cd6e2a3, REGION_GFX1, 0x000004, LOAD_8_64 },
  { "ar_17.5c", 0x20000, 0x0b8e0df4, REGION_GFX1, 0x000005, LOAD_8_64 },
  { "ar_38.8h", 0x20000, 0x8b9e75b9, REGION_GFX1, 0x000006, LOAD_8_64 },
  { "ar_32.8f", 0x20000, 0xdb6acdcf, REGION_GFX1, 0x000007, LOAD_8_64 },
  { "ar_10.5b", 0x20000, 0x4219b622, REGION_GFX1, 0x100000, LOAD_8_64 },
  { "ar_02.5a", 0x20000, 0xbac5dec5, REGION_GFX1, 0x100001, LOAD_8_64 },
  { "ar_14.10b", 0x20000, 0xe6bae179, REGION_GFX1, 0x100002, LOAD_8_64 },
  { "ar_06.10a", 0x20000, 0xc8f04223, REGION_GFX1, 0x100003, LOAD_8_64 },
  { "ar_25.7e", 0x20000, 0x15ccf981, REGION_GFX1, 0x100004, LOAD_8_64 },
  { "ar_18.7c", 0x20000, 0x9336db6a, REGION_GFX1, 0x100005, LOAD_8_64 },
  { "ar_39.9h", 0x20000, 0x9b8e1363, REGION_GFX1, 0x100006, LOAD_8_64 },
  { "ar_33.9f", 0x20000, 0x3968f4b5, REGION_GFX1, 0x100007, LOAD_8_64 },
	/* 20000-3ffff empty */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_area88r[] = // clone of unsquad
{
  LOAD_SW16( CPU1, "araj_23.8f", 0x00000, 0x80000, 0x7045d6cb),
  LOAD_SW16( CPU1, "araj_22.7f", 0x80000, 0x80000, 0x9913002e),
  LOAD( ROM2, "ara_09.12a", 0x00000, 0x20000, 0xaf88359c),
	/* 20000-3ffff empty */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ffight[] =
{
  LOAD_16_8( CPU1, "ff_36.11f", 0x00000, 0x20000, 0xf9a5ce83),
  LOAD_16_8( CPU1, "ff_42.11h", 0x00001, 0x20000, 0x65f11215),
  LOAD_16_8( CPU1, "ff_37.12f", 0x40000, 0x20000, 0xe1033784),
  LOAD_16_8( CPU1, "ffe_43.12h", 0x40001, 0x20000, 0x995e968a),
  LOAD_SW16( CPU1, "ff-32m.8h", 0x80000, 0x80000, 0xc747696e),
  { "ff-5m.7a", 0x80000, 0x9c284108, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "ff-7m.9a", 0x80000, 0xa7584dfb, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "ff-1m.3a", 0x80000, 0x0b605e44, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "ff-3m.5a", 0x80000, 0x52291cd2, REGION_GFX1, 0x000006, LOAD_16_64 },
  LOAD( ROM2, "ff_09.12b", 0x00000, 0x10000, 0xb8367eb5),
  LOAD( SMP1, "ff_18.11c", 0x00000, 0x20000, 0x375c66e7),
  LOAD( SMP1, "ff_19.12c", 0x20000, 0x20000, 0x1ef137f9),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ffighta[] = // clone of ffight
{
  LOAD_16_8( CPU1, "ffe_30.11f", 0x00000, 0x20000, 0x2347bf51),
  LOAD_16_8( CPU1, "ffe_35.11h", 0x00001, 0x20000, 0x5f694ecc),
  LOAD_16_8( CPU1, "ffe_31.12f", 0x40000, 0x20000, 0x6dc6b792),
  LOAD_16_8( CPU1, "ffe_36.12h", 0x40001, 0x20000, 0xb36a0b99),
  LOAD_SW16( CPU1, "ff-32m.8h", 0x80000, 0x80000, 0xc747696e),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ffightu[] = // clone of ffight
{
  LOAD_16_8( CPU1, "ff_36.11f", 0x00000, 0x20000, 0xf9a5ce83),
  LOAD_16_8( CPU1, "ff_42.11h", 0x00001, 0x20000, 0x65f11215),
  LOAD_16_8( CPU1, "ff_37.12f", 0x40000, 0x20000, 0xe1033784),
  LOAD_16_8( CPU1, "ffu_43.12h", 0x40001, 0x20000, 0x4ca65947),
  LOAD_SW16( CPU1, "ff-32m.8h", 0x80000, 0x80000, 0xc747696e),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ffightu1[] = // clone of ffight
{
  LOAD_16_8( CPU1, "ff_36.11f", 0x00000, 0x20000, 0xf9a5ce83),
  LOAD_16_8( CPU1, "ff_42.11h", 0x00001, 0x20000, 0x65f11215),
  LOAD_16_8( CPU1, "ff_37.12f", 0x40000, 0x20000, 0xe1033784),
  LOAD_16_8( CPU1, "ffu_43.12h", 0x40001, 0x20000, 0x4ca65947),
  LOAD_16_8( CPU1, "ff_34.9f", 0x80000, 0x20000, 0x0c8dc3fc),
  LOAD_16_8( CPU1, "ff_40.9h", 0x80001, 0x20000, 0x8075bab9),
  LOAD_16_8( CPU1, "ff_35.10f", 0xc0000, 0x20000, 0x4a934121),
  LOAD_16_8( CPU1, "ff_41.10h", 0xc0001, 0x20000, 0x2af68154),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ffightua[] = // clone of ffight
{
  LOAD_16_8( CPU1, "ffu_36.11f", 0x00000, 0x20000, 0xe2a48af9),
  LOAD_16_8( CPU1, "ffu_42.11h", 0x00001, 0x20000, 0xf4bb480e),
  LOAD_16_8( CPU1, "ffu_37.12f", 0x40000, 0x20000, 0xc371c667),
  LOAD_16_8( CPU1, "ffu_43(__ffightua).12h", 0x40001, 0x20000, 0x2f5771f9),
  LOAD_SW16( CPU1, "ff-32m.8h", 0x80000, 0x80000, 0xc747696e),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ffightub[] = // clone of ffight
{
  LOAD_16_8( CPU1, "ffu_30.11f", 0x00000, 0x20000, 0xed988977),
  LOAD_16_8( CPU1, "ffu_35.11h", 0x00001, 0x20000, 0x07bf1c21),
  LOAD_16_8( CPU1, "ffu_31.12f", 0x40000, 0x20000, 0xdba5a476),
  LOAD_16_8( CPU1, "ffu_36.12h", 0x40001, 0x20000, 0x4d89f542),
  LOAD_SW16( CPU1, "ff-32m.8h", 0x80000, 0x80000, 0xc747696e),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ffightj[] = // clone of ffight
{
  LOAD_16_8( CPU1, "ff36.bin", 0x00000, 0x20000, 0xf9a5ce83),
  LOAD_16_8( CPU1, "ff42.bin", 0x00001, 0x20000, 0x65f11215),
  LOAD_16_8( CPU1, "ff37.bin", 0x40000, 0x20000, 0xe1033784),
  LOAD_16_8( CPU1, "ff43.bin", 0x40001, 0x20000, 0xb6dee1c3),
  LOAD_16_8( CPU1, "ffj_34.10f", 0x80000, 0x20000, 0x0c8dc3fc),
  LOAD_16_8( CPU1, "ffj_40.10h", 0x80001, 0x20000, 0x8075bab9),
  LOAD_16_8( CPU1, "ffj_35.11f", 0xc0000, 0x20000, 0x4a934121),
  LOAD_16_8( CPU1, "ffj_41.11h", 0xc0001, 0x20000, 0x2af68154),
  { "ffj_09.4b", 0x20000, 0x5b116d0d, REGION_GFX1, 0x000000, LOAD_8_64 },
  { "ffj_01.4a", 0x20000, 0x815b1797, REGION_GFX1, 0x000001, LOAD_8_64 },
  { "ffj_13.9b", 0x20000, 0x8721a7da, REGION_GFX1, 0x000002, LOAD_8_64 },
  { "ffj_05.9a", 0x20000, 0xd0fcd4b5, REGION_GFX1, 0x000003, LOAD_8_64 },
  { "ffj_24.5e", 0x20000, 0xa1ab607a, REGION_GFX1, 0x000004, LOAD_8_64 },
  { "ffj_17.5c", 0x20000, 0x2dc18cf4, REGION_GFX1, 0x000005, LOAD_8_64 },
  { "ffj_38.8h", 0x20000, 0x6535a57f, REGION_GFX1, 0x000006, LOAD_8_64 },
  { "ffj_32.8f", 0x20000, 0xc8bc4a57, REGION_GFX1, 0x000007, LOAD_8_64 },
  { "ffj_10.5b", 0x20000, 0x624a924a, REGION_GFX1, 0x100000, LOAD_8_64 },
  { "ffj_02.5a", 0x20000, 0x5d91f694, REGION_GFX1, 0x100001, LOAD_8_64 },
  { "ffj_14.10b", 0x20000, 0x0a2e9101, REGION_GFX1, 0x100002, LOAD_8_64 },
  { "ffj_06.10a", 0x20000, 0x1c18f042, REGION_GFX1, 0x100003, LOAD_8_64 },
  { "ffj_25.7e", 0x20000, 0x6e8181ea, REGION_GFX1, 0x100004, LOAD_8_64 },
  { "ffj_18.7c", 0x20000, 0xb19ede59, REGION_GFX1, 0x100005, LOAD_8_64 },
  { "ffj_39.9h", 0x20000, 0x9416b477, REGION_GFX1, 0x100006, LOAD_8_64 },
  { "ffj_33.9f", 0x20000, 0x7369fa07, REGION_GFX1, 0x100007, LOAD_8_64 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ffightj1[] = // clone of ffight
{
  LOAD_16_8( CPU1, "ffj_36.12f", 0x00000, 0x20000, 0xe2a48af9),
  LOAD_16_8( CPU1, "ffj_42.12h", 0x00001, 0x20000, 0xf4bb480e),
  LOAD_16_8( CPU1, "ffj_37.13f", 0x40000, 0x20000, 0xc371c667),
  LOAD_16_8( CPU1, "ffj_43.13h", 0x40001, 0x20000, 0x6f81f194),
  LOAD_16_8( CPU1, "ffj_34.10f", 0x80000, 0x20000, 0x0c8dc3fc),
  LOAD_16_8( CPU1, "ffj_40.10h", 0x80001, 0x20000, 0x8075bab9),
  LOAD_16_8( CPU1, "ffj_35.11f", 0xc0000, 0x20000, 0x4a934121),
  LOAD_16_8( CPU1, "ffj_41.11h", 0xc0001, 0x20000, 0x2af68154),
  { "ffj_09.4b", 0x20000, 0x5b116d0d, REGION_GFX1, 0x000000, LOAD_8_64 },
  { "ffj_01.4a", 0x20000, 0x815b1797, REGION_GFX1, 0x000001, LOAD_8_64 },
  { "ffj_13.9b", 0x20000, 0x8721a7da, REGION_GFX1, 0x000002, LOAD_8_64 },
  { "ffj_05.9a", 0x20000, 0xd0fcd4b5, REGION_GFX1, 0x000003, LOAD_8_64 },
  { "ffj_24.5e", 0x20000, 0xa1ab607a, REGION_GFX1, 0x000004, LOAD_8_64 },
  { "ffj_17.5c", 0x20000, 0x2dc18cf4, REGION_GFX1, 0x000005, LOAD_8_64 },
  { "ffj_38.8h", 0x20000, 0x6535a57f, REGION_GFX1, 0x000006, LOAD_8_64 },
  { "ffj_32.8f", 0x20000, 0xc8bc4a57, REGION_GFX1, 0x000007, LOAD_8_64 },
  { "ffj_10.5b", 0x20000, 0x624a924a, REGION_GFX1, 0x100000, LOAD_8_64 },
  { "ffj_02.5a", 0x20000, 0x5d91f694, REGION_GFX1, 0x100001, LOAD_8_64 },
  { "ffj_14.10b", 0x20000, 0x0a2e9101, REGION_GFX1, 0x100002, LOAD_8_64 },
  { "ffj_06.10a", 0x20000, 0x1c18f042, REGION_GFX1, 0x100003, LOAD_8_64 },
  { "ffj_25.7e", 0x20000, 0x6e8181ea, REGION_GFX1, 0x100004, LOAD_8_64 },
  { "ffj_18.7c", 0x20000, 0xb19ede59, REGION_GFX1, 0x100005, LOAD_8_64 },
  { "ffj_39.9h", 0x20000, 0x9416b477, REGION_GFX1, 0x100006, LOAD_8_64 },
  { "ffj_33.9f", 0x20000, 0x7369fa07, REGION_GFX1, 0x100007, LOAD_8_64 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ffightj2[] = // clone of ffight
{
  LOAD_16_8( CPU1, "ffj_36a.12f", 0x00000, 0x20000, 0x088ed1c9),
  LOAD_16_8( CPU1, "ffj_42a.12h", 0x00001, 0x20000, 0xc4c491e6),
  LOAD_16_8( CPU1, "ffj_37a.13f", 0x40000, 0x20000, 0x708557ff),
  LOAD_16_8( CPU1, "ffj_43a.13h", 0x40001, 0x20000, 0xc004004a),
  LOAD_16_8( CPU1, "ff_34.10f", 0x80000, 0x20000, 0x0c8dc3fc),
  LOAD_16_8( CPU1, "ff_40.10h", 0x80001, 0x20000, 0x8075bab9),
  LOAD_16_8( CPU1, "ff_35.11f", 0xc0000, 0x20000, 0x4a934121),
  LOAD_16_8( CPU1, "ff_41.11h", 0xc0001, 0x20000, 0x2af68154),
  { "ff_09.4b", 0x20000, 0x5b116d0d, REGION_GFX1, 0x000000, LOAD_8_64 },
  { "ff_01.4a", 0x20000, 0x815b1797, REGION_GFX1, 0x000001, LOAD_8_64 },
  { "ff_13.9b", 0x20000, 0x8721a7da, REGION_GFX1, 0x000002, LOAD_8_64 },
  { "ff_05.9a", 0x20000, 0xd0fcd4b5, REGION_GFX1, 0x000003, LOAD_8_64 },
  { "ff_24.5e", 0x20000, 0xa1ab607a, REGION_GFX1, 0x000004, LOAD_8_64 },
  { "ff_17.5c", 0x20000, 0x2dc18cf4, REGION_GFX1, 0x000005, LOAD_8_64 },
  { "ff_38.8h", 0x20000, 0x6535a57f, REGION_GFX1, 0x000006, LOAD_8_64 },
  { "ff_32.8f", 0x20000, 0xc8bc4a57, REGION_GFX1, 0x000007, LOAD_8_64 },
  { "ff_10.5b", 0x20000, 0x624a924a, REGION_GFX1, 0x100000, LOAD_8_64 },
  { "ff_02.5a", 0x20000, 0x5d91f694, REGION_GFX1, 0x100001, LOAD_8_64 },
  { "ff_14.10b", 0x20000, 0x0a2e9101, REGION_GFX1, 0x100002, LOAD_8_64 },
  { "ff_06.10a", 0x20000, 0x1c18f042, REGION_GFX1, 0x100003, LOAD_8_64 },
  { "ff_25.7e", 0x20000, 0x6e8181ea, REGION_GFX1, 0x100004, LOAD_8_64 },
  { "ff_18.7c", 0x20000, 0xb19ede59, REGION_GFX1, 0x100005, LOAD_8_64 },
  { "ff_39.9h", 0x20000, 0x9416b477, REGION_GFX1, 0x100006, LOAD_8_64 },
  { "ff_33.9f", 0x20000, 0x7369fa07, REGION_GFX1, 0x100007, LOAD_8_64 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ffightjh[] = // clone of ffight
{
  LOAD_SW16( CPU1, "ff_23.8f", 0x00000, 0x80000, 0xae3dda7f),
  LOAD_SW16( CPU1, "ff_22.7f", 0x80000, 0x80000, 0xb2d5a3aa),
  { "ff_1.3a", 0x80000, 0x969d18e2, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "ff_2.4a", 0x80000, 0x02b59f99, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "ff_3.5a", 0x80000, 0x01d507ae, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "ff_4.6a", 0x80000, 0xf7c4ceb0, REGION_GFX1, 0x000006, LOAD_16_64 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_1941[] =
{
  LOAD_16_8( CPU1, "41em_30.11f", 0x00000, 0x20000, 0x4249ec61),
  LOAD_16_8( CPU1, "41em_35.11h", 0x00001, 0x20000, 0xddbee5eb),
  LOAD_16_8( CPU1, "41em_31.12f", 0x40000, 0x20000, 0x584e88e5),
  LOAD_16_8( CPU1, "41em_36.12h", 0x40001, 0x20000, 0x3cfc31d0),
  LOAD_SW16( CPU1, "41-32m.8h", 0x80000, 0x80000, 0x4e9648ca),
  { "41-5m.7a", 0x80000, 0x01d1cb11, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "41-7m.9a", 0x80000, 0xaeaa3509, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "41-1m.3a", 0x80000, 0xff77985a, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "41-3m.5a", 0x80000, 0x983be58f, REGION_GFX1, 0x000006, LOAD_16_64 },
  LOAD( ROM2, "41_9.12b", 0x00000, 0x10000, 0x0f9d8527),
  LOAD( SMP1, "41_18.11c", 0x00000, 0x20000, 0xd1f15aeb),
  LOAD( SMP1, "41_19.12c", 0x20000, 0x20000, 0x15aec3a6),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_1941r1[] = // clone of 1941
{
  LOAD_16_8( CPU1, "41e_30.11f", 0x00000, 0x20000, 0x9deb1e75),
  LOAD_16_8( CPU1, "41e_35.11h", 0x00001, 0x20000, 0xd63942b3),
  LOAD_16_8( CPU1, "41e_31.12f", 0x40000, 0x20000, 0xdf201112),
  LOAD_16_8( CPU1, "41e_36.12h", 0x40001, 0x20000, 0x816a818f),
  LOAD_SW16( CPU1, "41-32m.8h", 0x80000, 0x80000, 0x4e9648ca),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_1941u[] = // clone of 1941
{
  LOAD_16_8( CPU1, "41u_30.11f", 0x00000, 0x20000, 0xbe5439d0),
  LOAD_16_8( CPU1, "41u_35.11h", 0x00001, 0x20000, 0x6ac96595),
  LOAD_16_8( CPU1, "41u_31.12f", 0x40000, 0x20000, 0x9811d6eb),
  LOAD_16_8( CPU1, "41u_36.12h", 0x40001, 0x20000, 0xa87e6137),
  LOAD_SW16( CPU1, "41-32m.8h", 0x80000, 0x80000, 0x4e9648ca),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_1941j[] = // clone of 1941
{
  LOAD_16_8( CPU1, "41_36.12f", 0x00000, 0x20000, 0x7fbd42ab),
  LOAD_16_8( CPU1, "41_42.12h", 0x00001, 0x20000, 0xc7781f89),
  LOAD_16_8( CPU1, "41_37.13f", 0x40000, 0x20000, 0xc6464b0b),
  LOAD_16_8( CPU1, "41_43.13h", 0x40001, 0x20000, 0x440fc0b5),
  LOAD_16_8( CPU1, "41_34.10f", 0x80000, 0x20000, 0xb5f341ec),
  LOAD_16_8( CPU1, "41_40.10h", 0x80001, 0x20000, 0x3979837d),
  LOAD_16_8( CPU1, "41_35.11f", 0xc0000, 0x20000, 0x95cc979a),
  LOAD_16_8( CPU1, "41_41.11h", 0xc0001, 0x20000, 0x57496819),
  { "41_09.4b", 0x20000, 0xbe1b6bc2, REGION_GFX1, 0x000000, LOAD_8_64 },
  { "41_01.4a", 0x20000, 0xd8946fc1, REGION_GFX1, 0x000001, LOAD_8_64 },
  { "41_13.9b", 0x20000, 0x2e06d0ec, REGION_GFX1, 0x000002, LOAD_8_64 },
  { "41_05.9a", 0x20000, 0xd8ba28e0, REGION_GFX1, 0x000003, LOAD_8_64 },
  { "41_24.5e", 0x20000, 0x5aa43cee, REGION_GFX1, 0x000004, LOAD_8_64 },
  { "41_17.5c", 0x20000, 0xbbeff902, REGION_GFX1, 0x000005, LOAD_8_64 },
  { "41_38.8h", 0x20000, 0x8889c0aa, REGION_GFX1, 0x000006, LOAD_8_64 },
  { "41_32.8f", 0x20000, 0xf0168249, REGION_GFX1, 0x000007, LOAD_8_64 },
  { "41_10.5b", 0x20000, 0xb7eb6a6d, REGION_GFX1, 0x100000, LOAD_8_64 },
  { "41_02.5a", 0x20000, 0x802e8153, REGION_GFX1, 0x100001, LOAD_8_64 },
  { "41_14.10b", 0x20000, 0x5a33f676, REGION_GFX1, 0x100002, LOAD_8_64 },
  { "41_06.10a", 0x20000, 0x4e53650b, REGION_GFX1, 0x100003, LOAD_8_64 },
  { "41_25.7e", 0x20000, 0x94add360, REGION_GFX1, 0x100004, LOAD_8_64 },
  { "41_18.7c", 0x20000, 0xa5e1c1f3, REGION_GFX1, 0x100005, LOAD_8_64 },
  { "41_39.9h", 0x20000, 0x5b5c3949, REGION_GFX1, 0x100006, LOAD_8_64 },
  { "41_33.9f", 0x20000, 0x7a31b0e2, REGION_GFX1, 0x100007, LOAD_8_64 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mercs[] =
{
  LOAD_16_8( CPU1, "so2_30e.11f", 0x00000, 0x20000, 0xe17f9bf7),
  LOAD_16_8( CPU1, "so2_35e.11h", 0x00001, 0x20000, 0x78e63575),
  LOAD_16_8( CPU1, "so2_31e.12f", 0x40000, 0x20000, 0x51204d36),
  LOAD_16_8( CPU1, "so2_36e.12h", 0x40001, 0x20000, 0x9cfba8b4),
  LOAD_SW16( CPU1, "so2-32m.8h", 0x80000, 0x80000, 0x2eb5cf0c),
  { "so2-6m.8a", 0x80000, 0xaa6102af, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "so2-8m.10a", 0x80000, 0x839e6869, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "so2-2m.4a", 0x80000, 0x597c2875, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "so2-4m.6a", 0x80000, 0x912a9ca0, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "so2_24.7d", 0x20000, 0x3f254efe, REGION_GFX1, 0x200000, LOAD_8_64 },
  { "so2_14.7c", 0x20000, 0xf5a8905e, REGION_GFX1, 0x200001, LOAD_8_64 },
  { "so2_26.9d", 0x20000, 0xf3aa5a4a, REGION_GFX1, 0x200002, LOAD_8_64 },
  { "so2_16.9c", 0x20000, 0xb43cd1a8, REGION_GFX1, 0x200003, LOAD_8_64 },
  { "so2_20.3d", 0x20000, 0x8ca751a3, REGION_GFX1, 0x200004, LOAD_8_64 },
  { "so2_10.3c", 0x20000, 0xe9f569fd, REGION_GFX1, 0x200005, LOAD_8_64 },
  { "so2_22.5d", 0x20000, 0xfce9a377, REGION_GFX1, 0x200006, LOAD_8_64 },
  { "so2_12.5c", 0x20000, 0xb7df8a06, REGION_GFX1, 0x200007, LOAD_8_64 },
  LOAD( ROM2, "so2_09.12b", 0x00000, 0x10000, 0xd09d7c7a),
  LOAD( SMP1, "so2_18.11c", 0x00000, 0x20000, 0xbbea1643),
  LOAD( SMP1, "so2_19.12c", 0x20000, 0x20000, 0xac58aa71),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mercsu[] = // clone of mercs
{
  LOAD_16_8( CPU1, "so2_30a.11f", 0x00000, 0x20000, 0xe4e725d7),
  LOAD_16_8( CPU1, "so2_35a.11h", 0x00001, 0x20000, 0xe7843445),
  LOAD_16_8( CPU1, "so2_31a.12f", 0x40000, 0x20000, 0xc0b91dea),
  LOAD_16_8( CPU1, "so2_36a.12h", 0x40001, 0x20000, 0x591edf6c),
  LOAD_SW16( CPU1, "so2-32m.8h", 0x80000, 0x80000, 0x2eb5cf0c),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mercsur1[] = // clone of mercs
{
  LOAD_16_8( CPU1, "so2_30.11f", 0x00000, 0x20000, 0xe17f9bf7),
  LOAD_16_8( CPU1, "so2_35.11h", 0x00001, 0x20000, 0x4477df61),
  LOAD_16_8( CPU1, "so2_31.12f", 0x40000, 0x20000, 0x51204d36),
  LOAD_16_8( CPU1, "so2_36.12h", 0x40001, 0x20000, 0x9cfba8b4),
  LOAD_SW16( CPU1, "so2-32m.8h", 0x80000, 0x80000, 0x2eb5cf0c),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mercsj[] = // clone of mercs
{
  LOAD_16_8( CPU1, "so2_36.12f", 0x00000, 0x20000, 0xe17f9bf7),
  LOAD_16_8( CPU1, "so2_42.12h", 0x00001, 0x20000, 0x2c3884c6),
  LOAD_16_8( CPU1, "so2_37.13f", 0x40000, 0x20000, 0x51204d36),
  LOAD_16_8( CPU1, "so2_43.13h", 0x40001, 0x20000, 0x9cfba8b4),
  LOAD_16_8( CPU1, "so2_34.10f", 0x80000, 0x20000, 0xb8dae95f),
  LOAD_16_8( CPU1, "so2_40.10h", 0x80001, 0x20000, 0xde37771c),
  LOAD_16_8( CPU1, "so2_35.11f", 0xc0000, 0x20000, 0x7d24394d),
  LOAD_16_8( CPU1, "so2_41.11h", 0xc0001, 0x20000, 0x914f85e0),
  { "so2_09.4b", 0x20000, 0x690c261d, REGION_GFX1, 0x000000, LOAD_8_64 },
  { "so2_01.4a", 0x20000, 0x31fd2715, REGION_GFX1, 0x000001, LOAD_8_64 },
  { "so2_13.9b", 0x20000, 0xb5e48282, REGION_GFX1, 0x000002, LOAD_8_64 },
  { "so2_05.9a", 0x20000, 0x54bed82c, REGION_GFX1, 0x000003, LOAD_8_64 },
  { "so2_24.5e", 0x20000, 0x78b6f0cb, REGION_GFX1, 0x000004, LOAD_8_64 },
  { "so2_17.5c", 0x20000, 0xe78bb308, REGION_GFX1, 0x000005, LOAD_8_64 },
  { "so2_38.8h", 0x20000, 0x0010a9a2, REGION_GFX1, 0x000006, LOAD_8_64 },
  { "so2_32.8f", 0x20000, 0x75dffc9a, REGION_GFX1, 0x000007, LOAD_8_64 },
  { "so2_10.5b", 0x20000, 0x2f871714, REGION_GFX1, 0x100000, LOAD_8_64 },
  { "so2_02.5a", 0x20000, 0xb4b2a0b7, REGION_GFX1, 0x100001, LOAD_8_64 },
  { "so2_14.10b", 0x20000, 0x737a744b, REGION_GFX1, 0x100002, LOAD_8_64 },
  { "so2_06.10a", 0x20000, 0x9d756f51, REGION_GFX1, 0x100003, LOAD_8_64 },
  { "so2_25.7e", 0x20000, 0x6d0e05d6, REGION_GFX1, 0x100004, LOAD_8_64 },
  { "so2_18.7c", 0x20000, 0x96f61f4e, REGION_GFX1, 0x100005, LOAD_8_64 },
  { "so2_39.9h", 0x20000, 0xd52ba336, REGION_GFX1, 0x100006, LOAD_8_64 },
  { "so2_33.9f", 0x20000, 0x39b90d25, REGION_GFX1, 0x100007, LOAD_8_64 },
  { "so2_11.7b", 0x20000, 0x3f254efe, REGION_GFX1, 0x200000, LOAD_8_64 },
  { "so2_03.7a", 0x20000, 0xf5a8905e, REGION_GFX1, 0x200001, LOAD_8_64 },
  { "so2_15.11b", 0x20000, 0xf3aa5a4a, REGION_GFX1, 0x200002, LOAD_8_64 },
  { "so2_07.11a", 0x20000, 0xb43cd1a8, REGION_GFX1, 0x200003, LOAD_8_64 },
  { "so2_26.8e", 0x20000, 0x8ca751a3, REGION_GFX1, 0x200004, LOAD_8_64 },
  { "so2_19.8c", 0x20000, 0xe9f569fd, REGION_GFX1, 0x200005, LOAD_8_64 },
  { "so2_28.10e", 0x20000, 0xfce9a377, REGION_GFX1, 0x200006, LOAD_8_64 },
  { "so2_21.10c", 0x20000, 0xb7df8a06, REGION_GFX1, 0x200007, LOAD_8_64 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mtwins[] =
{
  LOAD_16_8( CPU1, "che_30.11f", 0x00000, 0x20000, 0x9a2a2db1),
  LOAD_16_8( CPU1, "che_35.11h", 0x00001, 0x20000, 0xa7f96b02),
  LOAD_16_8( CPU1, "che_31.12f", 0x40000, 0x20000, 0xbbff8a99),
  LOAD_16_8( CPU1, "che_36.12h", 0x40001, 0x20000, 0x0fa00c39),
  LOAD_SW16( CPU1, "ck-32m.8h", 0x80000, 0x80000, 0x9b70bd41),
  { "ck-5m.7a", 0x80000, 0x4ec75f15, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "ck-7m.9a", 0x80000, 0xd85d00d6, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "ck-1m.3a", 0x80000, 0xf33ca9d4, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "ck-3m.5a", 0x80000, 0x0ba2047f, REGION_GFX1, 0x000006, LOAD_16_64 },
  LOAD( ROM2, "ch_09.12b", 0x00000, 0x10000, 0x4d4255b7),
  LOAD( SMP1, "ch_18.11c", 0x00000, 0x20000, 0xf909e8de),
  LOAD( SMP1, "ch_19.12c", 0x20000, 0x20000, 0xfc158cf7),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_chikij[] = // clone of mtwins
{
  LOAD_16_8( CPU1, "chj_36a.12f", 0x00000, 0x20000, 0xec1328d8),
  LOAD_16_8( CPU1, "chj_42a.12h", 0x00001, 0x20000, 0x4ae13503),
  LOAD_16_8( CPU1, "chj_37a.13f", 0x40000, 0x20000, 0x46d2cf7b),
  LOAD_16_8( CPU1, "chj_43a.13h", 0x40001, 0x20000, 0x8d387fe8),
  LOAD_16_8( CPU1, "ch_34.10f", 0x80000, 0x20000, 0x609ed2f9),
  LOAD_16_8( CPU1, "ch_40.10h", 0x80001, 0x20000, 0xbe0d8301),
  LOAD_16_8( CPU1, "ch_35.11f", 0xc0000, 0x20000, 0xb810867f),
  LOAD_16_8( CPU1, "ch_41.11h", 0xc0001, 0x20000, 0x8ad96155),
  { "ch_09.4b", 0x20000, 0x567ab3ca, REGION_GFX1, 0x000000, LOAD_8_64 },
  { "ch_01.4a", 0x20000, 0x7f3b7b56, REGION_GFX1, 0x000001, LOAD_8_64 },
  { "ch_13.9b", 0x20000, 0x12a7a8ba, REGION_GFX1, 0x000002, LOAD_8_64 },
  { "ch_05.9a", 0x20000, 0x6c1afb9a, REGION_GFX1, 0x000003, LOAD_8_64 },
  { "ch_24.5e", 0x20000, 0x9cb6e6bc, REGION_GFX1, 0x000004, LOAD_8_64 },
  { "ch_17.5c", 0x20000, 0xfe490846, REGION_GFX1, 0x000005, LOAD_8_64 },
  { "ch_38.8h", 0x20000, 0x6e5c8cb6, REGION_GFX1, 0x000006, LOAD_8_64 },
  { "ch_32.8f", 0x20000, 0x317d27b0, REGION_GFX1, 0x000007, LOAD_8_64 },
  { "ch_10.5b", 0x20000, 0xe8251a9b, REGION_GFX1, 0x100000, LOAD_8_64 },
  { "ch_02.5a", 0x20000, 0x7c8c88fb, REGION_GFX1, 0x100001, LOAD_8_64 },
  { "ch_14.10b", 0x20000, 0x4012ec4b, REGION_GFX1, 0x100002, LOAD_8_64 },
  { "ch_06.10a", 0x20000, 0x81884b2b, REGION_GFX1, 0x100003, LOAD_8_64 },
  { "ch_25.7e", 0x20000, 0x1dfcbac5, REGION_GFX1, 0x100004, LOAD_8_64 },
  { "ch_18.7c", 0x20000, 0x516a34d1, REGION_GFX1, 0x100005, LOAD_8_64 },
  { "ch_39.9h", 0x20000, 0x872fb2a4, REGION_GFX1, 0x100006, LOAD_8_64 },
  { "ch_33.9f", 0x20000, 0x30dc5ded, REGION_GFX1, 0x100007, LOAD_8_64 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_msword[] =
{
  LOAD_16_8( CPU1, "mse_30.11f", 0x00000, 0x20000, 0x03fc8dbc),
  LOAD_16_8( CPU1, "mse_35.11h", 0x00001, 0x20000, 0xd5bf66cd),
  LOAD_16_8( CPU1, "mse_31.12f", 0x40000, 0x20000, 0x30332bcf),
  LOAD_16_8( CPU1, "mse_36.12h", 0x40001, 0x20000, 0x8f7d6ce9),
  LOAD_SW16( CPU1, "ms-32m.8h", 0x80000, 0x80000, 0x2475ddfc),
  { "ms-5m.7a", 0x80000, 0xc00fe7e2, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "ms-7m.9a", 0x80000, 0x4ccacac5, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "ms-1m.3a", 0x80000, 0x0d2bbe00, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "ms-3m.5a", 0x80000, 0x3a1a5bf4, REGION_GFX1, 0x000006, LOAD_16_64 },
  LOAD( ROM2, "ms_09.12b", 0x00000, 0x10000, 0x57b29519),
  LOAD( SMP1, "ms_18.11c", 0x00000, 0x20000, 0xfb64e90d),
  LOAD( SMP1, "ms_19.12c", 0x20000, 0x20000, 0x74f892b9),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mswordr1[] = // clone of msword
{
  LOAD_16_8( CPU1, "ms_30.11f", 0x00000, 0x20000, 0x21c1f078),
  LOAD_16_8( CPU1, "ms_35.11h", 0x00001, 0x20000, 0xa540a73a),
  LOAD_16_8( CPU1, "ms_31.12f", 0x40000, 0x20000, 0xd7e762b5),
  LOAD_16_8( CPU1, "ms_36.12h", 0x40001, 0x20000, 0x66f2dcdb),
  LOAD_SW16( CPU1, "ms-32m.8h", 0x80000, 0x80000, 0x2475ddfc),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mswordu[] = // clone of msword
{
  LOAD_16_8( CPU1, "msu_30.11f", 0x00000, 0x20000, 0xd963c816),
  LOAD_16_8( CPU1, "msu_35.11h", 0x00001, 0x20000, 0x72f179b3),
  LOAD_16_8( CPU1, "msu_31.12f", 0x40000, 0x20000, 0x20cd7904),
  LOAD_16_8( CPU1, "msu_36.12h", 0x40001, 0x20000, 0xbf88c080),
  LOAD_SW16( CPU1, "ms-32m.8h", 0x80000, 0x80000, 0x2475ddfc),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mswordj[] = // clone of msword
{
  LOAD_16_8( CPU1, "msj_36.12f", 0x00000, 0x20000, 0x04f0ef50),
  LOAD_16_8( CPU1, "msj_42.12h", 0x00001, 0x20000, 0x9fcbb9cd),
  LOAD_16_8( CPU1, "msj_37.13f", 0x40000, 0x20000, 0x6c060d70),
  LOAD_16_8( CPU1, "msj_43.13h", 0x40001, 0x20000, 0xaec77787),
  LOAD_16_8( CPU1, "ms_34.10f", 0x80000, 0x20000, 0x0e59a62d),
  LOAD_16_8( CPU1, "ms_40.10h", 0x80001, 0x20000, 0xbabade3a),
  LOAD_16_8( CPU1, "ms_35.11f", 0xc0000, 0x20000, 0x03da99d1),
  LOAD_16_8( CPU1, "ms_41.11h", 0xc0001, 0x20000, 0xfadf99ea),
  { "ms_09.4b", 0x20000, 0x4adee6f6, REGION_GFX1, 0x000000, LOAD_8_64 },
  { "ms_01.4a", 0x20000, 0xf7ab1b88, REGION_GFX1, 0x000001, LOAD_8_64 },
  { "ms_13.9b", 0x20000, 0xe01adc4b, REGION_GFX1, 0x000002, LOAD_8_64 },
  { "ms_05.9a", 0x20000, 0xf62c2369, REGION_GFX1, 0x000003, LOAD_8_64 },
  { "ms_24.5e", 0x20000, 0xbe64a3a1, REGION_GFX1, 0x000004, LOAD_8_64 },
  { "ms_17.5c", 0x20000, 0x0bc1665f, REGION_GFX1, 0x000005, LOAD_8_64 },
  { "ms_38.8h", 0x20000, 0x904a2ed5, REGION_GFX1, 0x000006, LOAD_8_64 },
  { "ms_32.8f", 0x20000, 0x3d89c530, REGION_GFX1, 0x000007, LOAD_8_64 },
  { "ms_10.5b", 0x20000, 0xf02c0718, REGION_GFX1, 0x100000, LOAD_8_64 },
  { "ms_02.5a", 0x20000, 0xd071a405, REGION_GFX1, 0x100001, LOAD_8_64 },
  { "ms_14.10b", 0x20000, 0xdfb2e4df, REGION_GFX1, 0x100002, LOAD_8_64 },
  { "ms_06.10a", 0x20000, 0xd3ce2a91, REGION_GFX1, 0x100003, LOAD_8_64 },
  { "ms_25.7e", 0x20000, 0x0f199d56, REGION_GFX1, 0x100004, LOAD_8_64 },
  { "ms_18.7c", 0x20000, 0x1ba76df2, REGION_GFX1, 0x100005, LOAD_8_64 },
  { "ms_39.9h", 0x20000, 0x01efce86, REGION_GFX1, 0x100006, LOAD_8_64 },
  { "ms_33.9f", 0x20000, 0xce25defc, REGION_GFX1, 0x100007, LOAD_8_64 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_cawing[] =
{
  LOAD_16_8( CPU1, "cae_30a.11f", 0x00000, 0x20000, 0x91fceacd),
  LOAD_16_8( CPU1, "cae_35a.11h", 0x00001, 0x20000, 0x3ef03083),
  LOAD_16_8( CPU1, "cae_31a.12f", 0x40000, 0x20000, 0xe5b75caf),
  LOAD_16_8( CPU1, "cae_36a.12h", 0x40001, 0x20000, 0xc73fd713),
  LOAD_SW16( CPU1, "ca-32m.8h", 0x80000, 0x80000, 0x0c4837d4),
  { "ca-5m.7a", 0x80000, 0x66d4cc37, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "ca-7m.9a", 0x80000, 0xb6f896f2, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "ca-1m.3a", 0x80000, 0x4d0620fd, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "ca-3m.5a", 0x80000, 0x0b0341c3, REGION_GFX1, 0x000006, LOAD_16_64 },
  LOAD( ROM2, "ca_9.12b", 0x00000, 0x10000, 0x96fe7485),
  LOAD( SMP1, "ca_18.11c", 0x00000, 0x20000, 0x4a613a2c),
  LOAD( SMP1, "ca_19.12c", 0x20000, 0x20000, 0x74584493),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_cawingr1[] = // clone of cawing
{
  LOAD_16_8( CPU1, "cae_30.11f", 0x00000, 0x20000, 0x23305cd5),
  LOAD_16_8( CPU1, "cae_35.11h", 0x00001, 0x20000, 0x69419113),
  LOAD_16_8( CPU1, "cae_31.12f", 0x40000, 0x20000, 0x9008dfb3),
  LOAD_16_8( CPU1, "cae_36.12h", 0x40001, 0x20000, 0x4dbf6f8e),
  LOAD_SW16( CPU1, "ca-32m.8h", 0x80000, 0x80000, 0x0c4837d4),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_cawingu[] = // clone of cawing
{
  LOAD_16_8( CPU1, "cau_30a.11f", 0x00000, 0x20000, 0x91fceacd),
  LOAD_16_8( CPU1, "cau_35a.11h", 0x00001, 0x20000, 0xf090d9b2),
  LOAD_16_8( CPU1, "cau_31a.12f", 0x40000, 0x20000, 0xe5b75caf),
  LOAD_16_8( CPU1, "cau_36a.12h", 0x40001, 0x20000, 0xc73fd713),
  LOAD_SW16( CPU1, "ca-32m.8h", 0x80000, 0x80000, 0x0c4837d4),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_cawingj[] = // clone of cawing
{
  LOAD_16_8( CPU1, "caj_36a.12f", 0x00000, 0x20000, 0x91fceacd),
  LOAD_16_8( CPU1, "caj_42a.12h", 0x00001, 0x20000, 0x039f8362),
  LOAD_16_8( CPU1, "caj_37a.13f", 0x40000, 0x20000, 0xe5b75caf),
  LOAD_16_8( CPU1, "caj_43a.13h", 0x40001, 0x20000, 0xc73fd713),
  LOAD_16_8( CPU1, "caj_34.10f", 0x80000, 0x20000, 0x51ea57f4),
  LOAD_16_8( CPU1, "caj_40.10h", 0x80001, 0x20000, 0x2ab71ae1),
  LOAD_16_8( CPU1, "caj_35.11f", 0xc0000, 0x20000, 0x01d71973),
  LOAD_16_8( CPU1, "caj_41.11h", 0xc0001, 0x20000, 0x3a43b538),
  { "caj_09.4b", 0x20000, 0x41b0f9a6, REGION_GFX1, 0x000000, LOAD_8_64 },
  { "caj_01.4a", 0x20000, 0x1002d0b8, REGION_GFX1, 0x000001, LOAD_8_64 },
  { "caj_13.9b", 0x20000, 0x6f3948b2, REGION_GFX1, 0x000002, LOAD_8_64 },
  { "caj_05.9a", 0x20000, 0x207373d7, REGION_GFX1, 0x000003, LOAD_8_64 },
  { "caj_24.5e", 0x20000, 0xe356aad7, REGION_GFX1, 0x000004, LOAD_8_64 },
  { "caj_17.5c", 0x20000, 0x540f2fd8, REGION_GFX1, 0x000005, LOAD_8_64 },
  { "caj_38.8h", 0x20000, 0x2464d4ab, REGION_GFX1, 0x000006, LOAD_8_64 },
  { "caj_32.8f", 0x20000, 0x9b5836b3, REGION_GFX1, 0x000007, LOAD_8_64 },
  { "caj_10.5b", 0x20000, 0xbf8a5f52, REGION_GFX1, 0x100000, LOAD_8_64 },
  { "caj_02.5a", 0x20000, 0x125b018d, REGION_GFX1, 0x100001, LOAD_8_64 },
  { "caj_14.10b", 0x20000, 0x8458e7d7, REGION_GFX1, 0x100002, LOAD_8_64 },
  { "caj_06.10a", 0x20000, 0xcf80e164, REGION_GFX1, 0x100003, LOAD_8_64 },
  { "caj_25.7e", 0x20000, 0xcdd0204d, REGION_GFX1, 0x100004, LOAD_8_64 },
  { "caj_18.7c", 0x20000, 0x29c1d4b1, REGION_GFX1, 0x100005, LOAD_8_64 },
  { "caj_39.9h", 0x20000, 0xeea23b67, REGION_GFX1, 0x100006, LOAD_8_64 },
  { "caj_33.9f", 0x20000, 0xdde3891f, REGION_GFX1, 0x100007, LOAD_8_64 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_nemo[] =
{
  LOAD_16_8( CPU1, "nme_30a.11f", 0x00000, 0x20000, 0xd2c03e56),
  LOAD_16_8( CPU1, "nme_35a.11h", 0x00001, 0x20000, 0x5fd31661),
  LOAD_16_8( CPU1, "nme_31a.12f", 0x40000, 0x20000, 0xb2bd4f6f),
  LOAD_16_8( CPU1, "nme_36a.12h", 0x40001, 0x20000, 0xee9450e3),
  LOAD_SW16( CPU1, "nm-32m.8h", 0x80000, 0x80000, 0xd6d1add3),
  { "nm-5m.7a", 0x80000, 0x487b8747, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "nm-7m.9a", 0x80000, 0x203dc8c6, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "nm-1m.3a", 0x80000, 0x9e878024, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "nm-3m.5a", 0x80000, 0xbb01e6b6, REGION_GFX1, 0x000006, LOAD_16_64 },
  LOAD( ROM2, "nme_09.12b", 0x00000, 0x10000, 0x0f4b0581),
  LOAD( SMP1, "nme_18.11c", 0x00000, 0x20000, 0xbab333d4),
  LOAD( SMP1, "nme_19.12c", 0x20000, 0x20000, 0x2650a0a8),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_nemoj[] = // clone of nemo
{
  LOAD_16_8( CPU1, "nmj_36a.12f", 0x00000, 0x20000, 0xdaeceabb),
  LOAD_16_8( CPU1, "nmj_42a.12h", 0x00001, 0x20000, 0x55024740),
  LOAD_16_8( CPU1, "nmj_37a.13f", 0x40000, 0x20000, 0x619068b6),
  LOAD_16_8( CPU1, "nmj_43a.13h", 0x40001, 0x20000, 0xa948a53b),
  LOAD_16_8( CPU1, "nm_34.10f", 0x80000, 0x20000, 0x5737feed),
  LOAD_16_8( CPU1, "nm_40.10h", 0x80001, 0x20000, 0x8a4099f3),
  LOAD_16_8( CPU1, "nm_35.11f", 0xc0000, 0x20000, 0xbd11a7f8),
  LOAD_16_8( CPU1, "nm_41.11h", 0xc0001, 0x20000, 0x6309603d),
  { "nm_09.4b", 0x20000, 0x9d60d286, REGION_GFX1, 0x000000, LOAD_8_64 },
  { "nm_01.4a", 0x20000, 0x8a83f7c4, REGION_GFX1, 0x000001, LOAD_8_64 },
  { "nm_13.9b", 0x20000, 0xa4909fe0, REGION_GFX1, 0x000002, LOAD_8_64 },
  { "nm_05.9a", 0x20000, 0x16db1e61, REGION_GFX1, 0x000003, LOAD_8_64 },
  { "nm_24.5e", 0x20000, 0x3312c648, REGION_GFX1, 0x000004, LOAD_8_64 },
  { "nm_17.5c", 0x20000, 0xccfc50e2, REGION_GFX1, 0x000005, LOAD_8_64 },
  { "nm_38.8h", 0x20000, 0xae98a997, REGION_GFX1, 0x000006, LOAD_8_64 },
  { "nm_32.8f", 0x20000, 0xb3704dde, REGION_GFX1, 0x000007, LOAD_8_64 },
  { "nm_10.5b", 0x20000, 0x33c1388c, REGION_GFX1, 0x100000, LOAD_8_64 },
  { "nm_02.5a", 0x20000, 0x84c69469, REGION_GFX1, 0x100001, LOAD_8_64 },
  { "nm_14.10b", 0x20000, 0x66612270, REGION_GFX1, 0x100002, LOAD_8_64 },
  { "nm_06.10a", 0x20000, 0x8b9bcf95, REGION_GFX1, 0x100003, LOAD_8_64 },
  { "nm_25.7e", 0x20000, 0xacfc84d2, REGION_GFX1, 0x100004, LOAD_8_64 },
  { "nm_18.7c", 0x20000, 0x4347deed, REGION_GFX1, 0x100005, LOAD_8_64 },
  { "nm_39.9h", 0x20000, 0x6a274ecd, REGION_GFX1, 0x100006, LOAD_8_64 },
  { "nm_33.9f", 0x20000, 0xc469dc74, REGION_GFX1, 0x100007, LOAD_8_64 },
  LOAD( ROM2, "nm_23.13b", 0x00000, 0x10000, 0x8d3c5a42),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2[] =
{
  LOAD_16_8( CPU1, "sf2e_30g.11e", 0x00000, 0x20000, 0xfe39ee33),
  LOAD_16_8( CPU1, "sf2e_37g.11f", 0x00001, 0x20000, 0xfb92cd74),
  LOAD_16_8( CPU1, "sf2e_31g.12e", 0x40000, 0x20000, 0x69a0a301),
  LOAD_16_8( CPU1, "sf2e_38g.12f", 0x40001, 0x20000, 0x5e22db70),
  LOAD_16_8( CPU1, "sf2e_28g.9e", 0x80000, 0x20000, 0x8bf9f1e5),
  LOAD_16_8( CPU1, "sf2e_35g.9f", 0x80001, 0x20000, 0x626ef934),
  LOAD_16_8( CPU1, "sf2_29b.10e", 0xc0000, 0x20000, 0xbb4af315),
  LOAD_16_8( CPU1, "sf2_36b.10f", 0xc0001, 0x20000, 0xc02a13eb),
  { "sf2-5m.4a", 0x80000, 0x22c9cc8e, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "sf2-7m.6a", 0x80000, 0x57213be8, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "sf2-1m.3a", 0x80000, 0xba529b4f, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "sf2-3m.5a", 0x80000, 0x4b1b33a8, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "sf2-6m.4c", 0x80000, 0x2c7e2229, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "sf2-8m.6c", 0x80000, 0xb5548f17, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "sf2-2m.3c", 0x80000, 0x14b84312, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "sf2-4m.5c", 0x80000, 0x5e9cd89a, REGION_GFX1, 0x200006, LOAD_16_64 },
  { "sf2-13m.4d", 0x80000, 0x994bfa58, REGION_GFX1, 0x400000, LOAD_16_64 },
  { "sf2-15m.6d", 0x80000, 0x3e66ad9d, REGION_GFX1, 0x400002, LOAD_16_64 },
  { "sf2-9m.3d", 0x80000, 0xc1befaa8, REGION_GFX1, 0x400004, LOAD_16_64 },
  { "sf2-11m.5d", 0x80000, 0x0627c831, REGION_GFX1, 0x400006, LOAD_16_64 },
  LOAD( ROM2, "sf2_9.12a", 0x00000, 0x10000, 0xa4823a1b),
  LOAD( SMP1, "sf2_18.11c", 0x00000, 0x20000, 0x7f162009),
  LOAD( SMP1, "sf2_19.12c", 0x20000, 0x20000, 0xbeade53f),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2eb[] = // clone of sf2
{
  LOAD_16_8( CPU1, "sf2e_30b.11e", 0x00000, 0x20000, 0x57bd7051),
  LOAD_16_8( CPU1, "sf2e_37b.11f", 0x00001, 0x20000, 0x62691cdd),
  LOAD_16_8( CPU1, "sf2e_31b.12e", 0x40000, 0x20000, 0xa673143d),
  LOAD_16_8( CPU1, "sf2e_38b.12f", 0x40001, 0x20000, 0x4c2ccef7),
  LOAD_16_8( CPU1, "sf2_28b.9e", 0x80000, 0x20000, 0x4009955e),
  LOAD_16_8( CPU1, "sf2_35b.9f", 0x80001, 0x20000, 0x8c1f3994),
  LOAD_16_8( CPU1, "sf2_29b.10e", 0xc0000, 0x20000, 0xbb4af315),
  LOAD_16_8( CPU1, "sf2_36b.10f", 0xc0001, 0x20000, 0xc02a13eb),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2ee[] = // clone of sf2
{
  LOAD_16_8( CPU1, "sf2e_30e.11e", 0x00000, 0x20000, 0xf37cd088),
  LOAD_16_8( CPU1, "sf2e_37e.11f", 0x00001, 0x20000, 0xc39468e6),
  LOAD_16_8( CPU1, "sf2e_31e.12e", 0x40000, 0x20000, 0x7c4771b4),
  LOAD_16_8( CPU1, "sf2e_38e.12f", 0x40001, 0x20000, 0xa4bd0cd9),
  LOAD_16_8( CPU1, "sf2e_28e.9e", 0x80000, 0x20000, 0xe3b95625),
  LOAD_16_8( CPU1, "sf2e_35e.9f", 0x80001, 0x20000, 0x3648769a),
  LOAD_16_8( CPU1, "sf2_29b.10e", 0xc0000, 0x20000, 0xbb4af315),
  LOAD_16_8( CPU1, "sf2_36b.10f", 0xc0001, 0x20000, 0xc02a13eb),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2ua[] = // clone of sf2
{
  LOAD_16_8( CPU1, "sf2u_30a.11e", 0x00000, 0x20000, 0x08beb861),
  LOAD_16_8( CPU1, "sf2u_37a.11f", 0x00001, 0x20000, 0xb7638d69),
  LOAD_16_8( CPU1, "sf2u_31a.12e", 0x40000, 0x20000, 0x0d5394e0),
  LOAD_16_8( CPU1, "sf2u_38a.12f", 0x40001, 0x20000, 0x42d6a79e),
  LOAD_16_8( CPU1, "sf2u_28a.9e", 0x80000, 0x20000, 0x387a175c),
  LOAD_16_8( CPU1, "sf2u_35a.9f", 0x80001, 0x20000, 0xa1a5adcc),
  LOAD_16_8( CPU1, "sf2_29b.10e", 0xc0000, 0x20000, 0xbb4af315),
  LOAD_16_8( CPU1, "sf2_36b.10f", 0xc0001, 0x20000, 0xc02a13eb),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2ub[] = // clone of sf2
{
  LOAD_16_8( CPU1, "sf2u_30b.11e", 0x00000, 0x20000, 0x57bd7051),
  LOAD_16_8( CPU1, "sf2u_37b.11f", 0x00001, 0x20000, 0x4a54d479),
  LOAD_16_8( CPU1, "sf2u_31b.12e", 0x40000, 0x20000, 0xa673143d),
  LOAD_16_8( CPU1, "sf2u_38b.12f", 0x40001, 0x20000, 0x4c2ccef7),
  LOAD_16_8( CPU1, "sf2u_28b.9e", 0x80000, 0x20000, 0x4009955e),
  LOAD_16_8( CPU1, "sf2u_35b.9f", 0x80001, 0x20000, 0x8c1f3994),
  LOAD_16_8( CPU1, "sf2_29b.10e", 0xc0000, 0x20000, 0xbb4af315),
  LOAD_16_8( CPU1, "sf2_36b.10f", 0xc0001, 0x20000, 0xc02a13eb),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2uc[] = // clone of sf2
{
  LOAD_16_8( CPU1, "sf2u_30c.11e", 0x00000, 0x20000, 0x6cb59385),
  LOAD_16_8( CPU1, "sf2u_37c.11f", 0x00001, 0x20000, 0x32e2c278),
  LOAD_16_8( CPU1, "sf2u_31c.12e", 0x40000, 0x20000, 0xc4fff4a9),
  LOAD_16_8( CPU1, "sf2u_38c.12f", 0x40001, 0x20000, 0x8210fc0e),
  LOAD_16_8( CPU1, "sf2u_28c.9e", 0x80000, 0x20000, 0x6eddd5e8),
  LOAD_16_8( CPU1, "sf2u_35c.9f", 0x80001, 0x20000, 0x6bcb404c),
  LOAD_16_8( CPU1, "sf2_29b.10e", 0xc0000, 0x20000, 0xbb4af315),
  LOAD_16_8( CPU1, "sf2_36b.10f", 0xc0001, 0x20000, 0xc02a13eb),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2ud[] = // clone of sf2
{
  LOAD_16_8( CPU1, "sf2u_30d.11e", 0x00000, 0x20000, 0x4bb2657c),
  LOAD_16_8( CPU1, "sf2u_37d.11f", 0x00001, 0x20000, 0xb33b42f2),
  LOAD_16_8( CPU1, "sf2u_31d.12e", 0x40000, 0x20000, 0xd57b67d7),
  LOAD_16_8( CPU1, "sf2u_38d.12f", 0x40001, 0x20000, 0x9c8916ef),
  LOAD_16_8( CPU1, "sf2u_28d.9e", 0x80000, 0x20000, 0x175819d1),
  LOAD_16_8( CPU1, "sf2u_35d.9f", 0x80001, 0x20000, 0x82060da4),
  LOAD_16_8( CPU1, "sf2_29b.10e", 0xc0000, 0x20000, 0xbb4af315),
  LOAD_16_8( CPU1, "sf2_36b.10f", 0xc0001, 0x20000, 0xc02a13eb),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2ue[] = // clone of sf2
{
  LOAD_16_8( CPU1, "sf2u_30e.11e", 0x00000, 0x20000, 0xf37cd088),
  LOAD_16_8( CPU1, "sf2u_37e.11f", 0x00001, 0x20000, 0x6c61a513),
  LOAD_16_8( CPU1, "sf2u_31e.12e", 0x40000, 0x20000, 0x7c4771b4),
  LOAD_16_8( CPU1, "sf2u_38e.12f", 0x40001, 0x20000, 0xa4bd0cd9),
  LOAD_16_8( CPU1, "sf2u_28e.9e", 0x80000, 0x20000, 0xe3b95625),
  LOAD_16_8( CPU1, "sf2u_35e.9f", 0x80001, 0x20000, 0x3648769a),
  LOAD_16_8( CPU1, "sf2_29b.10e", 0xc0000, 0x20000, 0xbb4af315),
  LOAD_16_8( CPU1, "sf2_36b.10f", 0xc0001, 0x20000, 0xc02a13eb),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2uf[] = // clone of sf2
{
  LOAD_16_8( CPU1, "sf2u_30f.11e", 0x00000, 0x20000, 0xfe39ee33),
  LOAD_16_8( CPU1, "sf2u_37f.11f", 0x00001, 0x20000, 0x169e7388),
  LOAD_16_8( CPU1, "sf2u_31f.12e", 0x40000, 0x20000, 0x69a0a301),
  LOAD_16_8( CPU1, "sf2u_38f.12f", 0x40001, 0x20000, 0x1510e4e2),
  LOAD_16_8( CPU1, "sf2u_28f.9e", 0x80000, 0x20000, 0xacd8175b),
  LOAD_16_8( CPU1, "sf2u_35f.9f", 0x80001, 0x20000, 0xc0a80bd1),
  LOAD_16_8( CPU1, "sf2_29b.10e", 0xc0000, 0x20000, 0xbb4af315),
  LOAD_16_8( CPU1, "sf2_36b.10f", 0xc0001, 0x20000, 0xc02a13eb),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2ug[] = // clone of sf2
{
  LOAD_16_8( CPU1, "sf2u_30g.11e", 0x00000, 0x20000, 0xfe39ee33),
  LOAD_16_8( CPU1, "sf2u_37g.11f", 0x00001, 0x20000, 0x5886cae7),
  LOAD_16_8( CPU1, "sf2u_31g.12e", 0x40000, 0x20000, 0x69a0a301),
  LOAD_16_8( CPU1, "sf2u_38g.12f", 0x40001, 0x20000, 0x5e22db70),
  LOAD_16_8( CPU1, "sf2u_28g.9e", 0x80000, 0x20000, 0x8bf9f1e5),
  LOAD_16_8( CPU1, "sf2u_35g.9f", 0x80001, 0x20000, 0x626ef934),
  LOAD_16_8( CPU1, "sf2_29b.10e", 0xc0000, 0x20000, 0xbb4af315),
  LOAD_16_8( CPU1, "sf2_36b.10f", 0xc0001, 0x20000, 0xc02a13eb),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2ui[] = // clone of sf2
{
  LOAD_16_8( CPU1, "sf2u_30i.11e", 0x00000, 0x20000, 0xfe39ee33),
  LOAD_16_8( CPU1, "sf2u_37i.11f", 0x00001, 0x20000, 0x9df707dd),
  LOAD_16_8( CPU1, "sf2u_31i.12e", 0x40000, 0x20000, 0x69a0a301),
  LOAD_16_8( CPU1, "sf2u_38i.12f", 0x40001, 0x20000, 0x4cb46daf),
  LOAD_16_8( CPU1, "sf2u_28i.9e", 0x80000, 0x20000, 0x1580be4c),
  LOAD_16_8( CPU1, "sf2u_35i.9f", 0x80001, 0x20000, 0x1468d185),
  LOAD_16_8( CPU1, "sf2_29b.10e", 0xc0000, 0x20000, 0xbb4af315),
  LOAD_16_8( CPU1, "sf2_36b.10f", 0xc0001, 0x20000, 0xc02a13eb),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2uk[] = // clone of sf2
{
  LOAD_16_8( CPU1, "sf2u_30k.11e", 0x00000, 0x20000, 0x8f66076c),
  LOAD_16_8( CPU1, "sf2u_37k.11f", 0x00001, 0x20000, 0x4e1f6a83),
  LOAD_16_8( CPU1, "sf2u_31k.12e", 0x40000, 0x20000, 0xf9f89f60),
  LOAD_16_8( CPU1, "sf2u_38k.12f", 0x40001, 0x20000, 0x6ce0a85a),
  LOAD_16_8( CPU1, "sf2u_28k.9e", 0x80000, 0x20000, 0x8e958f31),
  LOAD_16_8( CPU1, "sf2u_35k.9f", 0x80001, 0x20000, 0xfce76fad),
  LOAD_16_8( CPU1, "sf2u_29a.10e", 0xc0000, 0x20000, 0xbb4af315),
  LOAD_16_8( CPU1, "sf2u_36a.10f", 0xc0001, 0x20000, 0xc02a13eb),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2j[] = // clone of sf2
{
  LOAD_16_8( CPU1, "sf2j30.bin", 0x00000, 0x20000, 0x79022b31),
  LOAD_16_8( CPU1, "sf2j37.bin", 0x00001, 0x20000, 0x516776ec),
  LOAD_16_8( CPU1, "sf2j31.bin", 0x40000, 0x20000, 0xfe15cb39),
  LOAD_16_8( CPU1, "sf2j38.bin", 0x40001, 0x20000, 0x38614d70),
  LOAD_16_8( CPU1, "sf2j28.bin", 0x80000, 0x20000, 0xd283187a),
  LOAD_16_8( CPU1, "sf2j35.bin", 0x80001, 0x20000, 0xd28158e4),
  LOAD_16_8( CPU1, "sf2_29a.bin", 0xc0000, 0x20000, 0xbb4af315),
  LOAD_16_8( CPU1, "sf2_36a.bin", 0xc0001, 0x20000, 0xc02a13eb),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2ja[] = // clone of sf2
{
  LOAD_16_8( CPU1, "sf2j_30a.11e", 0x00000, 0x20000, 0x57bd7051),
  LOAD_16_8( CPU1, "sf2j_37a.11f", 0x00001, 0x20000, 0x1e1f6844),
  LOAD_16_8( CPU1, "sf2j_31a.12e", 0x40000, 0x20000, 0xa673143d),
  LOAD_16_8( CPU1, "sf2j_38a.12f", 0x40001, 0x20000, 0x4c2ccef7),
  LOAD_16_8( CPU1, "sf2j_28a.9e", 0x80000, 0x20000, 0x4009955e),
  LOAD_16_8( CPU1, "sf2j_35a.9f", 0x80001, 0x20000, 0x8c1f3994),
  LOAD_16_8( CPU1, "sf2j_29a.10e", 0xc0000, 0x20000, 0xbb4af315),
  LOAD_16_8( CPU1, "sf2j_36a.10f", 0xc0001, 0x20000, 0xc02a13eb),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2jc[] = // clone of sf2
{
  LOAD_16_8( CPU1, "sf2j_30c.11e", 0x00000, 0x20000, 0x8add35ec),
  LOAD_16_8( CPU1, "sf2j_37c.11f", 0x00001, 0x20000, 0x0d74a256),
  LOAD_16_8( CPU1, "sf2j_31c.12e", 0x40000, 0x20000, 0xc4fff4a9),
  LOAD_16_8( CPU1, "sf2j_38c.12f", 0x40001, 0x20000, 0x8210fc0e),
  LOAD_16_8( CPU1, "sf2j_28c.9e", 0x80000, 0x20000, 0x6eddd5e8),
  LOAD_16_8( CPU1, "sf2j_35c.9f", 0x80001, 0x20000, 0x6bcb404c),
  LOAD_16_8( CPU1, "sf2j_29a.10e", 0xc0000, 0x20000, 0xbb4af315),
  LOAD_16_8( CPU1, "sf2j_36a.10f", 0xc0001, 0x20000, 0xc02a13eb),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2ebbl[] = // clone of sf2
{
  LOAD_16_8( CPU1, "12.bin", 0x00000, 0x40000, 0xa258b4d5),
  LOAD_16_8( CPU1, "09.bin", 0x00001, 0x40000, 0x59ccd474),
  LOAD_16_8( CPU1, "11.bin", 0x80000, 0x40000, 0x82097d63),
  LOAD_16_8( CPU1, "10.bin", 0x80001, 0x40000, 0x0c83844d),
	/* The 12 MASK roms on this PCB match the original roms exactly */
  { "1b_yf082.bin", 0x80000, 0x22c9cc8e, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "1d_yf028.bin", 0x80000, 0x57213be8, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "1a_yf087.bin", 0x80000, 0xba529b4f, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "1c_yf088.bin", 0x80000, 0x4b1b33a8, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "1f_yf085.bin", 0x80000, 0x2c7e2229, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "1h_yf115.bin", 0x80000, 0xb5548f17, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "1e_yf111.bin", 0x80000, 0x14b84312, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "1g_yf002.bin", 0x80000, 0x5e9cd89a, REGION_GFX1, 0x200006, LOAD_16_64 },
  { "1j_yf117.bin", 0x80000, 0x994bfa58, REGION_GFX1, 0x400000, LOAD_16_64 },
  { "1l_ye040.bin", 0x80000, 0x3e66ad9d, REGION_GFX1, 0x400002, LOAD_16_64 },
  { "1i_yf038.bin", 0x80000, 0xc1befaa8, REGION_GFX1, 0x400004, LOAD_16_64 },
	/* These map over the MASK roms on this bootleg to get rid of the CAPCOM logo (wasteful, but correct) */
  { "1k_ye039.bin", 0x80000, 0x0627c831, REGION_GFX1, 0x400006, LOAD_16_64 },
  { "05.bin", 0x10000, 0xa505621e, REGION_GFX1, 0x400000, LOAD_8_64 },
  { "05.bin", 0x10000 , 0xa505621e, REGION_GFX1, 0x400004, LOAD_CONTINUE },
  { "07.bin", 0x10000, 0xde6271fb, REGION_GFX1, 0x400002, LOAD_8_64 },
  { "07.bin", 0x10000 , 0xde6271fb, REGION_GFX1, 0x400006, LOAD_CONTINUE },
  { "06.bin", 0x10000, 0x23775344, REGION_GFX1, 0x400001, LOAD_8_64 },
  { "06.bin", 0x10000 , 0x23775344, REGION_GFX1, 0x400005, LOAD_CONTINUE },
  { "08.bin", 0x10000, 0x81c9550f, REGION_GFX1, 0x400003, LOAD_8_64 },
  { "08.bin", 0x10000 , 0x81c9550f, REGION_GFX1, 0x400007, LOAD_CONTINUE },
  LOAD( USER1, "04.bin", 0x00000, 0x10000, 0x13ea1c44),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2qp1[] = // clone of sf2
{
  LOAD_SW16( CPU1, "stfii-qkn-cps-17.33", 0x000000, 0x40000, 0x3a9458ee),
  { "stfii-qkn-cps-17.33", 0x40000 , 0x3a9458ee, REGION_CPU1, 0xc0000, LOAD_CONTINUE },
  LOAD_SW16( CPU1, "stfii-qkn-cps-17.34", 0x080000, 0x40000, 0x4ed215d8),
  { "stfii-qkn-cps-17.34", 0x40000 , 0x4ed215d8, REGION_CPU1, 0x40000, LOAD_CONTINUE },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2thndr[] = // clone of sf2
{
  LOAD_16_8( CPU1, "17_30.11e", 0x00000, 0x20000, 0xd3cd6d18),
  LOAD_16_8( CPU1, "17_37.11f", 0x00001, 0x20000, 0xe892716e),
  LOAD_16_8( CPU1, "sf2u_31b.12e", 0x40000, 0x20000, 0xa673143d),
  LOAD_16_8( CPU1, "sf2u_38b.12f", 0x40001, 0x20000, 0x4c2ccef7),
  LOAD_16_8( CPU1, "sf2u_28b.9e", 0x80000, 0x20000, 0x4009955e),
  LOAD_16_8( CPU1, "sf2u_35b.9f", 0x80001, 0x20000, 0x8c1f3994),
  LOAD_16_8( CPU1, "17_29.10e", 0xc0000, 0x20000, 0x8830b54d),
  LOAD_16_8( CPU1, "17_36.10f", 0xc0001, 0x20000, 0x3f13ada3),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_3wonders[] =
{
  LOAD_16_8( CPU1, "rte_30a.11f", 0x00000, 0x20000, 0xef5b8b33),
  LOAD_16_8( CPU1, "rte_35a.11h", 0x00001, 0x20000, 0x7d705529),
  LOAD_16_8( CPU1, "rte_31a.12f", 0x40000, 0x20000, 0x32835e5e),
  LOAD_16_8( CPU1, "rte_36a.12h", 0x40001, 0x20000, 0x7637975f),
  LOAD_16_8( CPU1, "rt_28a.9f", 0x80000, 0x20000, 0x054137c8),
  LOAD_16_8( CPU1, "rt_33a.9h", 0x80001, 0x20000, 0x7264cb1b),
  LOAD_16_8( CPU1, "rte_29a.10f", 0xc0000, 0x20000, 0xcddaa919),
  LOAD_16_8( CPU1, "rte_34a.10h", 0xc0001, 0x20000, 0xed52e7e5),
  { "rt-5m.7a", 0x80000, 0x86aef804, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "rt-7m.9a", 0x80000, 0x4f057110, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "rt-1m.3a", 0x80000, 0x902489d0, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "rt-3m.5a", 0x80000, 0xe35ce720, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "rt-6m.8a", 0x80000, 0x13cb0e7c, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "rt-8m.10a", 0x80000, 0x1f055014, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "rt-2m.4a", 0x80000, 0xe9a034f4, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "rt-4m.6a", 0x80000, 0xdf0eea8b, REGION_GFX1, 0x200006, LOAD_16_64 },
  LOAD( ROM2, "rt_9.12b", 0x00000, 0x10000, 0xabfca165),
  LOAD( SMP1, "rt_18.11c", 0x00000, 0x20000, 0x26b211ab),
  LOAD( SMP1, "rt_19.12c", 0x20000, 0x20000, 0xdbe64ad0),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_3wondersr1[] = // clone of 3wonders
{
  LOAD_16_8( CPU1, "rte_30.11f", 0x00000, 0x20000, 0x0d541519),
  LOAD_16_8( CPU1, "rte_35.11h", 0x00001, 0x20000, 0x73dd0e20),
  LOAD_16_8( CPU1, "rte_31.12f", 0x40000, 0x20000, 0x33e0337d),
  LOAD_16_8( CPU1, "rte_36.12h", 0x40001, 0x20000, 0xa8865243),
  LOAD_16_8( CPU1, "rte_28.9f", 0x80000, 0x20000, 0x054137c8),
  LOAD_16_8( CPU1, "rte_33.9h", 0x80001, 0x20000, 0x7264cb1b),
  LOAD_16_8( CPU1, "rte_29.10f", 0xc0000, 0x20000, 0x9a8df1e4),
  LOAD_16_8( CPU1, "rte_34.10h", 0xc0001, 0x20000, 0x6348a79d),
  LOAD( ROM2, "rt_9(__3wondersr1).12b", 0x00000, 0x10000, 0x7d5a77a7),
  LOAD( SMP1, "rt_18(__3wondersr1).11c", 0x00000, 0x20000, 0xf6dc0d3d),
  LOAD( SMP1, "rt_19.12c", 0x20000, 0x20000, 0xdbe64ad0),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_3wondersu[] = // clone of 3wonders
{
  LOAD_16_8( CPU1, "rtu_30a.11f", 0x00000, 0x20000, 0x0b156fd8),
  LOAD_16_8( CPU1, "rtu_35a.11h", 0x00001, 0x20000, 0x57350bf4),
  LOAD_16_8( CPU1, "rtu_31a.12f", 0x40000, 0x20000, 0x0e723fcc),
  LOAD_16_8( CPU1, "rtu_36a.12h", 0x40001, 0x20000, 0x523a45dc),
  LOAD_16_8( CPU1, "rt_28a.9f", 0x80000, 0x20000, 0x054137c8),
  LOAD_16_8( CPU1, "rt_33a.9h", 0x80001, 0x20000, 0x7264cb1b),
  LOAD_16_8( CPU1, "rtu_29a.10f", 0xc0000, 0x20000, 0x37ba3e20),
  LOAD_16_8( CPU1, "rtu_34a.10h", 0xc0001, 0x20000, 0xf99f46c0),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_wonder3[] = // clone of 3wonders
{
  LOAD_16_8( CPU1, "rtj_36.12f", 0x00000, 0x20000, 0xe3741247),
  LOAD_16_8( CPU1, "rtj_42.12h", 0x00001, 0x20000, 0xb4baa117),
  LOAD_16_8( CPU1, "rtj_37.13f", 0x40000, 0x20000, 0xa1f677b0),
  LOAD_16_8( CPU1, "rtj_43.13h", 0x40001, 0x20000, 0x85337a47),
  LOAD_16_8( CPU1, "rt_34.10f", 0x80000, 0x20000, 0x054137c8),
  LOAD_16_8( CPU1, "rt_40.10h", 0x80001, 0x20000, 0x7264cb1b),
  LOAD_16_8( CPU1, "rtj_35.11f", 0xc0000, 0x20000, 0xe72f9ea3),
  LOAD_16_8( CPU1, "rtj_41.11h", 0xc0001, 0x20000, 0xa11ee998),
  { "rt_09.4b", 0x20000, 0x2c40e480, REGION_GFX1, 0x000000, LOAD_8_64 },
  { "rt_01.4a", 0x20000, 0x3e11f8cd, REGION_GFX1, 0x000001, LOAD_8_64 },
  { "rt_13.9b", 0x20000, 0x51009117, REGION_GFX1, 0x000002, LOAD_8_64 },
  { "rt_05.9a", 0x20000, 0x283fd470, REGION_GFX1, 0x000003, LOAD_8_64 },
  { "rt_24.5e", 0x20000, 0xee4484ce, REGION_GFX1, 0x000004, LOAD_8_64 },
  { "rt_17.5c", 0x20000, 0xe5dcddeb, REGION_GFX1, 0x000005, LOAD_8_64 },
  { "rt_38.8h", 0x20000, 0xb2940c2d, REGION_GFX1, 0x000006, LOAD_8_64 },
  { "rt_32.8f", 0x20000, 0x08e2b758, REGION_GFX1, 0x000007, LOAD_8_64 },
  { "rt_10.5b", 0x20000, 0xe3f3ff94, REGION_GFX1, 0x100000, LOAD_8_64 },
  { "rt_02.5a", 0x20000, 0xfcffd73c, REGION_GFX1, 0x100001, LOAD_8_64 },
  { "rt_14.10b", 0x20000, 0x5c546d9a, REGION_GFX1, 0x100002, LOAD_8_64 },
  { "rt_06.10a", 0x20000, 0xd9650bc4, REGION_GFX1, 0x100003, LOAD_8_64 },
  { "rt_25.7e", 0x20000, 0x11b28831, REGION_GFX1, 0x100004, LOAD_8_64 },
  { "rt_18.7c", 0x20000, 0xce1afb7c, REGION_GFX1, 0x100005, LOAD_8_64 },
  { "rt_39.9h", 0x20000, 0xea7ac9ee, REGION_GFX1, 0x100006, LOAD_8_64 },
  { "rt_33.9f", 0x20000, 0xd6a99384, REGION_GFX1, 0x100007, LOAD_8_64 },
  { "rt_11.7b", 0x20000, 0x04f3c298, REGION_GFX1, 0x200000, LOAD_8_64 },
  { "rt_03.7a", 0x20000, 0x98087e08, REGION_GFX1, 0x200001, LOAD_8_64 },
  { "rt_15.11b", 0x20000, 0xb6aba565, REGION_GFX1, 0x200002, LOAD_8_64 },
  { "rt_07.11a", 0x20000, 0xc62defa1, REGION_GFX1, 0x200003, LOAD_8_64 },
  { "rt_26.8e", 0x20000, 0x532f542e, REGION_GFX1, 0x200004, LOAD_8_64 },
  { "rt_19.8c", 0x20000, 0x1f0f72bd, REGION_GFX1, 0x200005, LOAD_8_64 },
  { "rt_28.10e", 0x20000, 0x6064e499, REGION_GFX1, 0x200006, LOAD_8_64 },
  { "rt_21.10c", 0x20000, 0x20012ddc, REGION_GFX1, 0x200007, LOAD_8_64 },
  { "rt_12.8b", 0x20000, 0xe54664cc, REGION_GFX1, 0x300000, LOAD_8_64 },
  { "rt_04.8a", 0x20000, 0x4d7b9a1a, REGION_GFX1, 0x300001, LOAD_8_64 },
  { "rt_16.12b", 0x20000, 0x37c96cfc, REGION_GFX1, 0x300002, LOAD_8_64 },
  { "rt_08.12a", 0x20000, 0x75f4975b, REGION_GFX1, 0x300003, LOAD_8_64 },
  { "rt_27.9e", 0x20000, 0xec6edc0f, REGION_GFX1, 0x300004, LOAD_8_64 },
  { "rt_20.9c", 0x20000, 0x4fe52659, REGION_GFX1, 0x300005, LOAD_8_64 },
  { "rt_29.11e", 0x20000, 0x8fa77f9f, REGION_GFX1, 0x300006, LOAD_8_64 },
  { "rt_22.11c", 0x20000, 0x228a0d4a, REGION_GFX1, 0x300007, LOAD_8_64 },
//  ROM_LOAD( "rt_23(__alt).13b", 0x00000, 0x08000, CRC(7d5a77a7) SHA1(4f2a6534d7a9e518061102c1dd7d7aca39600c33) )  /* dumped from another board, 1 byte different, pcb verified */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_3wondersh[] = // clone of 3wonders
{
  LOAD_16_8( CPU1, "22.bin", 0x00000, 0x20000, 0xe6071884),
  LOAD_16_8( CPU1, "26.bin", 0x00001, 0x20000, 0xa28447b7),
  LOAD_16_8( CPU1, "23.bin", 0x40000, 0x20000, 0xfd3d6509),
  LOAD_16_8( CPU1, "27.bin", 0x40001, 0x20000, 0x999cba3d),
  LOAD_16_8( CPU1, "rt_28a.9f", 0x80000, 0x20000, 0x054137c8),
  LOAD_16_8( CPU1, "rt_33a.9h", 0x80001, 0x20000, 0x7264cb1b),
  LOAD_16_8( CPU1, "rte_29a.10f", 0xc0000, 0x20000, 0xcddaa919),
  LOAD_16_8( CPU1, "rte_34a.10h", 0xc0001, 0x20000, 0xed52e7e5),
  { "16.bin", 0x40000, 0xc997bca2, REGION_GFX1, 0x000000, LOAD_8_64 },
  { "6.bin", 0x40000, 0x3eea321a, REGION_GFX1, 0x000001, LOAD_8_64 },
  { "18.bin", 0x40000, 0x98acdfd4, REGION_GFX1, 0x000002, LOAD_8_64 },
  { "8.bin", 0x40000, 0xdc9ca6f9, REGION_GFX1, 0x000003, LOAD_8_64 },
  { "12.bin", 0x40000, 0x0d8a6007, REGION_GFX1, 0x000004, LOAD_8_64 },
  { "2.bin", 0x40000, 0xd75563b9, REGION_GFX1, 0x000005, LOAD_8_64 },
  { "14.bin", 0x40000, 0x84369a28, REGION_GFX1, 0x000006, LOAD_8_64 },
  { "4.bin", 0x40000, 0xd4831578, REGION_GFX1, 0x000007, LOAD_8_64 },
  { "17.bin", 0x40000, 0x040edff5, REGION_GFX1, 0x200000, LOAD_8_64 },
  { "7.bin", 0x40000, 0xc7c0468c, REGION_GFX1, 0x200001, LOAD_8_64 },
  { "19.bin", 0x40000, 0x9fef114f, REGION_GFX1, 0x200002, LOAD_8_64 },
  { "9.bin", 0x40000, 0x48cbfba5, REGION_GFX1, 0x200003, LOAD_8_64 },
  { "13.bin", 0x40000, 0x8fc3d7d1, REGION_GFX1, 0x200004, LOAD_8_64 },
  { "3.bin", 0x40000, 0xc65e9a86, REGION_GFX1, 0x200005, LOAD_8_64 },
  { "15.bin", 0x40000, 0xf239341a, REGION_GFX1, 0x200006, LOAD_8_64 },
  { "5.bin", 0x40000, 0x947ac944, REGION_GFX1, 0x200007, LOAD_8_64 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_kod[] =
{
  LOAD_16_8( CPU1, "kde_30a.11e", 0x00000, 0x20000, 0xfcb5efe2),
  LOAD_16_8( CPU1, "kde_37a.11f", 0x00001, 0x20000, 0xf22e5266),
  LOAD_16_8( CPU1, "kde_31a.12e", 0x40000, 0x20000, 0xc710d722),
  LOAD_16_8( CPU1, "kde_38a.12f", 0x40001, 0x20000, 0x57d6ed3a),
  LOAD_16_8( CPU1, "kd_28.9e", 0x80000, 0x20000, 0x9367bcd9),
  LOAD_16_8( CPU1, "kd_35.9f", 0x80001, 0x20000, 0x4ca6a48a),
  LOAD_16_8( CPU1, "kd_29.10e", 0xc0000, 0x20000, 0x0360fa72),
  LOAD_16_8( CPU1, "kd_36a.10f", 0xc0001, 0x20000, 0x95a3cef8),
  { "kd-5m.4a", 0x80000, 0xe45b8701, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "kd-7m.6a", 0x80000, 0xa7750322, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "kd-1m.3a", 0x80000, 0x5f74bf78, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "kd-3m.5a", 0x80000, 0x5e5303bf, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "kd-6m.4c", 0x80000, 0x113358f3, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "kd-8m.6c", 0x80000, 0x38853c44, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "kd-2m.3c", 0x80000, 0x9ef36604, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "kd-4m.5c", 0x80000, 0x402b9b4f, REGION_GFX1, 0x200006, LOAD_16_64 },
  LOAD( ROM2, "kd_9.12a", 0x00000, 0x10000, 0xbac6ec26),
  LOAD( SMP1, "kd_18.11c", 0x00000, 0x20000, 0x4c63181d),
  LOAD( SMP1, "kd_19.12c", 0x20000, 0x20000, 0x92941b80),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_kodr1[] = // clone of kod
{
  LOAD_16_8( CPU1, "kde_30.11e", 0x00000, 0x20000, 0xc7414fd4),
  LOAD_16_8( CPU1, "kde_37.11f", 0x00001, 0x20000, 0xa5bf40d2),
  LOAD_16_8( CPU1, "kde_31.12e", 0x40000, 0x20000, 0x1fffc7bd),
  LOAD_16_8( CPU1, "kde_38.12f", 0x40001, 0x20000, 0x89e57a82),
  LOAD_16_8( CPU1, "kde_28.9e", 0x80000, 0x20000, 0x9367bcd9),
  LOAD_16_8( CPU1, "kde_35.9f", 0x80001, 0x20000, 0x4ca6a48a),
  LOAD_16_8( CPU1, "kde_29.10e", 0xc0000, 0x20000, 0x6a0ba878),
  LOAD_16_8( CPU1, "kde_36.10f", 0xc0001, 0x20000, 0xb509b39d),
  LOAD( ROM2, "kd_9(__kodr1).12a", 0x00000, 0x10000, 0xf5514510),
  LOAD( SMP1, "kd_18(__kodr1).11c", 0x00000, 0x20000, 0x69ecb2c8),
  LOAD( SMP1, "kd_19(__kodr1).12c", 0x20000, 0x20000, 0x02d851c1),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_kodu[] = // clone of kod
{
  LOAD_16_8( CPU1, "kdu_30b.11e", 0x00000, 0x20000, 0x825817f9),
  LOAD_16_8( CPU1, "kdu_37b.11f", 0x00001, 0x20000, 0xd2422dfb),
  LOAD_16_8( CPU1, "kdu_31b.12e", 0x40000, 0x20000, 0x9af36039),
  LOAD_16_8( CPU1, "kdu_38b.12f", 0x40001, 0x20000, 0xbe8405a1),
  LOAD_16_8( CPU1, "kdu_28.9e", 0x80000, 0x20000, 0x9367bcd9),
  LOAD_16_8( CPU1, "kdu_35.9f", 0x80001, 0x20000, 0x4ca6a48a),
  LOAD_16_8( CPU1, "kdu_29.10e", 0xc0000, 0x20000, 0x0360fa72),
  LOAD_16_8( CPU1, "kdu_36a.10f", 0xc0001, 0x20000, 0x95a3cef8),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_kodj[] = // clone of kod
{
  LOAD_16_8( CPU1, "kdj_30a.11e", 0x00000, 0x20000, 0xebc788ad),
  LOAD_16_8( CPU1, "kdj_37a.11f", 0x00001, 0x20000, 0xe55c3529),
  LOAD_16_8( CPU1, "kdj_31a.12e", 0x40000, 0x20000, 0xc710d722),
  LOAD_16_8( CPU1, "kdj_38a.12f", 0x40001, 0x20000, 0x57d6ed3a),
  LOAD_SW16( CPU1, "kd_33.6f", 0x80000, 0x80000, 0x9bd7ad4b),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_kodja[] = // clone of kod
{
  LOAD_16_8( CPU1, "kdj_36a.12f", 0x00000, 0x20000, 0xebc788ad),
  LOAD_16_8( CPU1, "kdj_42a.12h", 0x00001, 0x20000, 0xe55c3529),
  LOAD_16_8( CPU1, "kdj_37a.13f", 0x40000, 0x20000, 0xc710d722),
  LOAD_16_8( CPU1, "kdj_43a.13h", 0x40001, 0x20000, 0x57d6ed3a),
  LOAD_16_8( CPU1, "kd_34.10f", 0x80000, 0x20000, 0x9367bcd9),
  LOAD_16_8( CPU1, "kd_40.10h", 0x80001, 0x20000, 0x4ca6a48a),
  LOAD_16_8( CPU1, "kd_35.11f", 0xc0000, 0x20000, 0x0360fa72),
  LOAD_16_8( CPU1, "kd_41a.11h", 0xc0001, 0x20000, 0x95a3cef8),
  { "kd_9.4b", 0x20000, 0x401a98e3, REGION_GFX1, 0x000000, LOAD_8_64 },
  { "kd_1.4a", 0x20000, 0x5894399a, REGION_GFX1, 0x000001, LOAD_8_64 },
  { "kd_13.9b", 0x20000, 0xb6685131, REGION_GFX1, 0x000002, LOAD_8_64 },
  { "kd_5.9a", 0x20000, 0xc29b9ab3, REGION_GFX1, 0x000003, LOAD_8_64 },
  { "kd_24.5e", 0x20000, 0x97008fdb, REGION_GFX1, 0x000004, LOAD_8_64 },
  { "kd_17.5c", 0x20000, 0xdc9a83d3, REGION_GFX1, 0x000005, LOAD_8_64 },
  { "kd_38.8h", 0x20000, 0x9c3dd2d1, REGION_GFX1, 0x000006, LOAD_8_64 },
  { "kd_32.8f", 0x20000, 0x1b2a802a, REGION_GFX1, 0x000007, LOAD_8_64 },
  { "kd_10.5b", 0x20000, 0xe788ae96, REGION_GFX1, 0x100000, LOAD_8_64 },
  { "kd_2.5a", 0x20000, 0xb022e3e3, REGION_GFX1, 0x100001, LOAD_8_64 },
  { "kd_14.10b", 0x20000, 0x4840c5ef, REGION_GFX1, 0x100002, LOAD_8_64 },
  { "kd_6.10a", 0x20000, 0x519faee4, REGION_GFX1, 0x100003, LOAD_8_64 },
  { "kd_25.7e", 0x20000, 0x5d0fa853, REGION_GFX1, 0x100004, LOAD_8_64 },
  { "kd_18.7c", 0x20000, 0x6ad3b2bb, REGION_GFX1, 0x100005, LOAD_8_64 },
  { "kd_39.9h", 0x20000, 0xd7920213, REGION_GFX1, 0x100006, LOAD_8_64 },
  { "kd_33.9f", 0x20000, 0x65c2bed6, REGION_GFX1, 0x100007, LOAD_8_64 },
  { "kd_11.7b", 0x20000, 0x147e3310, REGION_GFX1, 0x200000, LOAD_8_64 },
  { "kd_3.7a", 0x20000, 0x5d18bc83, REGION_GFX1, 0x200001, LOAD_8_64 },
  { "kd_15.11b", 0x20000, 0x57359746, REGION_GFX1, 0x200002, LOAD_8_64 },
  { "kd_7.11a", 0x20000, 0x7fe03079, REGION_GFX1, 0x200003, LOAD_8_64 },
  { "kd_26.8e", 0x20000, 0x57e5fab5, REGION_GFX1, 0x200004, LOAD_8_64 },
  { "kd_19.8c", 0x20000, 0xb1f30f7c, REGION_GFX1, 0x200005, LOAD_8_64 },
  { "kd_28.10e", 0x20000, 0x3a424135, REGION_GFX1, 0x200006, LOAD_8_64 },
  { "kd_21.10c", 0x20000, 0xce10d2c3, REGION_GFX1, 0x200007, LOAD_8_64 },
  { "kd_12.8b", 0x20000, 0xa6042aa2, REGION_GFX1, 0x300000, LOAD_8_64 },
  { "kd_4.8a", 0x20000, 0x0ce0ba30, REGION_GFX1, 0x300001, LOAD_8_64 },
  { "kd_16.12b", 0x20000, 0x63dcb7e0, REGION_GFX1, 0x300002, LOAD_8_64 },
  { "kd_8.12a", 0x20000, 0xc69b77ae, REGION_GFX1, 0x300003, LOAD_8_64 },
  { "kd_27.9e", 0x20000, 0x40d7bfed, REGION_GFX1, 0x300004, LOAD_8_64 },
  { "kd_20.9c", 0x20000, 0x01c1f399, REGION_GFX1, 0x300005, LOAD_8_64 },
  { "kd_29.11e", 0x20000, 0xa1eeac03, REGION_GFX1, 0x300006, LOAD_8_64 },
  { "kd_22.11c", 0x20000, 0x5ade98eb, REGION_GFX1, 0x300007, LOAD_8_64 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_captcomm[] =
{
  LOAD_SW16( CPU1, "cce_23f.8f", 0x000000, 0x80000, 0x42c814c5),
  LOAD_SW16( CPU1, "cc_22f.7f", 0x080000, 0x80000, 0x0fd34195),
  LOAD_16_8( CPU1, "cc_24f.9e", 0x100000, 0x20000, 0x3a794f25),
  LOAD_16_8( CPU1, "cc_28f.9f", 0x100001, 0x20000, 0xfc3c2906),
  { "cc-5m.3a", 0x80000, 0x7261d8ba, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "cc-7m.5a", 0x80000, 0x6a60f949, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "cc-1m.4a", 0x80000, 0x00637302, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "cc-3m.6a", 0x80000, 0xcc87cf61, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "cc-6m.7a", 0x80000, 0x28718bed, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "cc-8m.9a", 0x80000, 0xd4acc53a, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "cc-2m.8a", 0x80000, 0x0c69f151, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "cc-4m.10a", 0x80000, 0x1f9ebb97, REGION_GFX1, 0x200006, LOAD_16_64 },
  LOAD( ROM2, "cc_09.11a", 0x00000, 0x10000, 0x698e8b58),
  LOAD( SMP1, "cc_18.11c", 0x00000, 0x20000, 0x6de2c2db),
  LOAD( SMP1, "cc_19.12c", 0x20000, 0x20000, 0xb99091ae),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_captcommr1[] = // clone of captcomm
{
  LOAD_SW16( CPU1, "cce_23d.8f", 0x000000, 0x80000, 0x19c58ece),
  LOAD_SW16( CPU1, "cc_22d.7f", 0x080000, 0x80000, 0xa91949b7),
  LOAD_16_8( CPU1, "cc_24d.9e", 0x100000, 0x20000, 0x680e543f),
  LOAD_16_8( CPU1, "cc_28d.9f", 0x100001, 0x20000, 0x8820039f),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_captcommu[] = // clone of captcomm
{
  LOAD_SW16( CPU1, "ccu_23b.8f", 0x000000, 0x80000, 0x03da44fd),
  LOAD_SW16( CPU1, "ccu_22c.7f", 0x080000, 0x80000, 0x9b82a052),
  LOAD_16_8( CPU1, "ccu_24b.9e", 0x100000, 0x20000, 0x84ff99b2),
  LOAD_16_8( CPU1, "ccu_28b.9f", 0x100001, 0x20000, 0xfbcec223),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_captcommj[] = // clone of captcomm
{
  LOAD_SW16( CPU1, "ccj_23f.8f", 0x000000, 0x80000, 0x5b482b62),
  LOAD_SW16( CPU1, "ccj_22f.7f", 0x080000, 0x80000, 0x0fd34195),
  LOAD_16_8( CPU1, "ccj_24f.9e", 0x100000, 0x20000, 0x3a794f25),
  LOAD_16_8( CPU1, "ccj_28f.9f", 0x100001, 0x20000, 0xfc3c2906),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_captcommjr1[] = // clone of captcomm
{
  LOAD_SW16( CPU1, "ccj_23b.8f", 0x000000, 0x80000, 0xe2a2d80e),
  LOAD_SW16( CPU1, "ccj_22c.7f", 0x080000, 0x80000, 0x9b82a052),
  LOAD_16_8( CPU1, "ccj_24b.9e", 0x100000, 0x20000, 0x84ff99b2),
  LOAD_16_8( CPU1, "ccj_28b.9f", 0x100001, 0x20000, 0xfbcec223),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_captcommb[] = // clone of captcomm
{
  LOAD_16_8( CPU1, "25.bin", 0x000000, 0x80000, 0xcb71ed7a),
  LOAD_16_8( CPU1, "27.bin", 0x000001, 0x80000, 0x47cb2e87),
  LOAD_16_8( CPU1, "24.bin", 0x100000, 0x40000, 0x79794279),
  LOAD_16_8( CPU1, "26.bin", 0x100001, 0x40000, 0xb01077ba),
  { "c91e-01.bin", 0x40000, 0xf863071c, REGION_GFX1, 0x000000, LOAD_8_64 },
  { "c91e-01.bin", 0x40000, 0xf863071c, REGION_GFX1, 0x000004, LOAD_CONTINUE },
  { "c91e-01.bin", 0x40000, 0xf863071c, REGION_GFX1, 0x200000, LOAD_CONTINUE },
  { "c91e-01.bin", 0x40000, 0xf863071c, REGION_GFX1, 0x200004, LOAD_CONTINUE },
  { "c91e-02.bin", 0x40000, 0x4b03c308, REGION_GFX1, 0x000001, LOAD_8_64 },
  { "c91e-02.bin", 0x40000, 0x4b03c308, REGION_GFX1, 0x000005, LOAD_CONTINUE },
  { "c91e-02.bin", 0x40000, 0x4b03c308, REGION_GFX1, 0x200001, LOAD_CONTINUE },
  { "c91e-02.bin", 0x40000, 0x4b03c308, REGION_GFX1, 0x200005, LOAD_CONTINUE },
  { "c91e-03.bin", 0x40000, 0x3383ea96, REGION_GFX1, 0x000002, LOAD_8_64 },
  { "c91e-03.bin", 0x40000, 0x3383ea96, REGION_GFX1, 0x000006, LOAD_CONTINUE },
  { "c91e-03.bin", 0x40000, 0x3383ea96, REGION_GFX1, 0x200002, LOAD_CONTINUE },
  { "c91e-03.bin", 0x40000, 0x3383ea96, REGION_GFX1, 0x200006, LOAD_CONTINUE },
  { "c91e-04.bin", 0x40000, 0xb8e1f4cf, REGION_GFX1, 0x000003, LOAD_8_64 },
  { "c91e-04.bin", 0x40000, 0xb8e1f4cf, REGION_GFX1, 0x000007, LOAD_CONTINUE },
  { "c91e-04.bin", 0x40000, 0xb8e1f4cf, REGION_GFX1, 0x200003, LOAD_CONTINUE },
  { "c91e-04.bin", 0x40000, 0xb8e1f4cf, REGION_GFX1, 0x200007, LOAD_CONTINUE },
  LOAD( SMP1, "c91e-05.bin", 0x00000, 0x40000, 0x096115fb),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_knights[] =
{
  LOAD_SW16( CPU1, "kr_23e.8f", 0x00000, 0x80000, 0x1b3997eb),
  LOAD_SW16( CPU1, "kr_22.7f", 0x80000, 0x80000, 0xd0b671a9),
  { "kr-5m.3a", 0x80000, 0x9e36c1a4, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "kr-7m.5a", 0x80000, 0xc5832cae, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "kr-1m.4a", 0x80000, 0xf095be2d, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "kr-3m.6a", 0x80000, 0x179dfd96, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "kr-6m.7a", 0x80000, 0x1f4298d2, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "kr-8m.9a", 0x80000, 0x37fa8751, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "kr-2m.8a", 0x80000, 0x0200bc3d, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "kr-4m.10a", 0x80000, 0x0bb2b4e7, REGION_GFX1, 0x200006, LOAD_16_64 },
  LOAD( ROM2, "kr_09.11a", 0x00000, 0x10000, 0x5e44d9ee),
  LOAD( SMP1, "kr_18.11c", 0x00000, 0x20000, 0xda69d15f),
  LOAD( SMP1, "kr_19.12c", 0x20000, 0x20000, 0xbfc654e9),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_knightsu[] = // clone of knights
{
  LOAD_SW16( CPU1, "kr_23u.8f", 0x00000, 0x80000, 0x252bc2ba),
  LOAD_SW16( CPU1, "kr_22.7f", 0x80000, 0x80000, 0xd0b671a9),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_knightsj[] = // clone of knights
{
  LOAD_SW16( CPU1, "kr_23j.8f", 0x00000, 0x80000, 0xeae7417f),
  LOAD_SW16( CPU1, "kr_22.7f", 0x80000, 0x80000, 0xd0b671a9),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_knightsja[] = // clone of knights
{
  LOAD_16_8( CPU1, "krj_36.12f", 0x00000, 0x20000, 0xad3d1a8e),
  LOAD_16_8( CPU1, "krj_42.12h", 0x00001, 0x20000, 0xe694a491),
  LOAD_16_8( CPU1, "krj_37.13f", 0x40000, 0x20000, 0x85596094),
  LOAD_16_8( CPU1, "krj_43.13h", 0x40001, 0x20000, 0x9198bf8f),
  LOAD_16_8( CPU1, "kr_34.10f", 0x80000, 0x20000, 0xfe6eb08d),
  LOAD_16_8( CPU1, "kr_40.10h", 0x80001, 0x20000, 0x1172806d),
  LOAD_16_8( CPU1, "kr_35.11f", 0xc0000, 0x20000, 0xf854b020),
  LOAD_16_8( CPU1, "kr_41.11h", 0xc0001, 0x20000, 0xeb52e78d),
  { "kr_09.4b", 0x20000, 0x08b76e10, REGION_GFX1, 0x000000, LOAD_8_64 },
  { "kr_01.4a", 0x20000, 0x40cecf5c, REGION_GFX1, 0x000001, LOAD_8_64 },
  { "kr_13.9b", 0x20000, 0x435aaa03, REGION_GFX1, 0x000002, LOAD_8_64 },
  { "kr_05.9a", 0x20000, 0x5b8a615b, REGION_GFX1, 0x000003, LOAD_8_64 },
  { "kr_24.5e", 0x20000, 0xde65153e, REGION_GFX1, 0x000004, LOAD_8_64 },
  { "kr_17.5c", 0x20000, 0xb171c968, REGION_GFX1, 0x000005, LOAD_8_64 },
  { "kr_38.8h", 0x20000, 0xf4466bf4, REGION_GFX1, 0x000006, LOAD_8_64 },
  { "kr_32.8f", 0x20000, 0x87380ddd, REGION_GFX1, 0x000007, LOAD_8_64 },
  { "kr_10.5b", 0x20000, 0x37006d66, REGION_GFX1, 0x100000, LOAD_8_64 },
  { "kr_02.5a", 0x20000, 0xb54612e3, REGION_GFX1, 0x100001, LOAD_8_64 },
  { "kr_14.10b", 0x20000, 0x0ae88766, REGION_GFX1, 0x100002, LOAD_8_64 },
  { "kr_06.10a", 0x20000, 0xecb1a09a, REGION_GFX1, 0x100003, LOAD_8_64 },
  { "kr_25.7e", 0x20000, 0x9aace189, REGION_GFX1, 0x100004, LOAD_8_64 },
  { "kr_18.7c", 0x20000, 0x09fa14a5, REGION_GFX1, 0x100005, LOAD_8_64 },
  { "kr_39.9h", 0x20000, 0xfd8a9aeb, REGION_GFX1, 0x100006, LOAD_8_64 },
  { "kr_33.9f", 0x20000, 0x11803e95, REGION_GFX1, 0x100007, LOAD_8_64 },
  { "kr_11.7b", 0x20000, 0xa967ceb3, REGION_GFX1, 0x200000, LOAD_8_64 },
  { "kr_03.7a", 0x20000, 0xea10db07, REGION_GFX1, 0x200001, LOAD_8_64 },
  { "kr_15.11b", 0x20000, 0x8140b83b, REGION_GFX1, 0x200002, LOAD_8_64 },
  { "kr_07.11a", 0x20000, 0x6af10648, REGION_GFX1, 0x200003, LOAD_8_64 },
  { "kr_26.8e", 0x20000, 0x8865d86b, REGION_GFX1, 0x200004, LOAD_8_64 },
  { "kr_19.8c", 0x20000, 0x029f4abe, REGION_GFX1, 0x200005, LOAD_8_64 },
  { "kr_28.10e", 0x20000, 0x5f84f92f, REGION_GFX1, 0x200006, LOAD_8_64 },
  { "kr_21.10c", 0x20000, 0x01b35065, REGION_GFX1, 0x200007, LOAD_8_64 },
  { "kr_12.8b", 0x20000, 0x03d945b1, REGION_GFX1, 0x300000, LOAD_8_64 },
  { "kr_04.8a", 0x20000, 0xe30c8388, REGION_GFX1, 0x300001, LOAD_8_64 },
  { "kr_16.12b", 0x20000, 0x40c39d1b, REGION_GFX1, 0x300002, LOAD_8_64 },
  { "kr_08.12a", 0x20000, 0xd310c9e8, REGION_GFX1, 0x300003, LOAD_8_64 },
  { "kr_27.9e", 0x20000, 0x3e041444, REGION_GFX1, 0x300004, LOAD_8_64 },
  { "kr_20.9c", 0x20000, 0xbd4bffb8, REGION_GFX1, 0x300005, LOAD_8_64 },
  { "kr_29.11e", 0x20000, 0x1387a076, REGION_GFX1, 0x300006, LOAD_8_64 },
  { "kr_22.11c", 0x20000, 0xfd351922, REGION_GFX1, 0x300007, LOAD_8_64 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2ce[] =
{
  LOAD_SW16( CPU1, "s92e_23b.8f", 0x000000, 0x80000, 0x0aaa1a3a),
  LOAD_SW16( CPU1, "s92_22b.7f", 0x080000, 0x80000, 0x2bbe15ed),
  LOAD_SW16( CPU1, "s92_21a.6f", 0x100000, 0x80000, 0x925a7877),
  { "s92-1m.3a", 0x80000, 0x03b0d852, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "s92-3m.5a", 0x80000, 0x840289ec, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "s92-2m.4a", 0x80000, 0xcdb5f027, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "s92-4m.6a", 0x80000, 0xe2799472, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "s92-5m.7a", 0x80000, 0xba8a2761, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "s92-7m.9a", 0x80000, 0xe584bfb5, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "s92-6m.8a", 0x80000, 0x21e3f87d, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "s92-8m.10a", 0x80000, 0xbefc47df, REGION_GFX1, 0x200006, LOAD_16_64 },
  { "s92-10m.3c", 0x80000, 0x960687d5, REGION_GFX1, 0x400000, LOAD_16_64 },
  { "s92-12m.5c", 0x80000, 0x978ecd18, REGION_GFX1, 0x400002, LOAD_16_64 },
  { "s92-11m.4c", 0x80000, 0xd6ec9a0a, REGION_GFX1, 0x400004, LOAD_16_64 },
  { "s92-13m.6c", 0x80000, 0xed2c67f6, REGION_GFX1, 0x400006, LOAD_16_64 },
  LOAD( ROM2, "s92_09.11a", 0x00000, 0x10000, 0x08f6b60e),
  LOAD( SMP1, "s92_18.11c", 0x00000, 0x20000, 0x7f162009),
  LOAD( SMP1, "s92_19.12c", 0x20000, 0x20000, 0xbeade53f),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2ceea[] = // clone of sf2ce
{
  LOAD_SW16( CPU1, "s92e_23a.8f", 0x000000, 0x80000, 0x3f846b74),
  LOAD_SW16( CPU1, "s92_22a.7f", 0x080000, 0x80000, 0x99f1cca4),
  LOAD_SW16( CPU1, "s92_21a.6f", 0x100000, 0x80000, 0x925a7877),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2ceua[] = // clone of sf2ce
{
  LOAD_SW16( CPU1, "s92u_23a.8f", 0x000000, 0x80000, 0xac44415b),
  LOAD_SW16( CPU1, "s92_22a.7f", 0x080000, 0x80000, 0x99f1cca4),
  LOAD_SW16( CPU1, "s92_21a.6f", 0x100000, 0x80000, 0x925a7877),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2ceub[] = // clone of sf2ce
{
  LOAD_SW16( CPU1, "s92u_23b.8f", 0x000000, 0x80000, 0x996a3015),
  LOAD_SW16( CPU1, "s92_22b.7f", 0x080000, 0x80000, 0x2bbe15ed),
  LOAD_SW16( CPU1, "s92_21a.6f", 0x100000, 0x80000, 0x925a7877),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2ceuc[] = // clone of sf2ce
{
  LOAD_SW16( CPU1, "s92u_23c.8f", 0x000000, 0x80000, 0x0a8b6aa2),
  LOAD_SW16( CPU1, "s92_22c.7f", 0x080000, 0x80000, 0x5fd8630b),
  LOAD_SW16( CPU1, "s92_21a.6f", 0x100000, 0x80000, 0x925a7877),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2ceja[] = // clone of sf2ce
{
  LOAD_SW16( CPU1, "s92j_23a.8f", 0x000000, 0x80000, 0x4f42bb5a),
  LOAD_SW16( CPU1, "s92j_22a.7f", 0x080000, 0x80000, 0xc4f64bcd),
  LOAD_SW16( CPU1, "s92_21a.6f", 0x100000, 0x80000, 0x925a7877),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2cejb[] = // clone of sf2ce
{
  LOAD_SW16( CPU1, "s92j_23b.8f", 0x000000, 0x80000, 0x140876c5),
  LOAD_SW16( CPU1, "s92j_22b.7f", 0x080000, 0x80000, 0x2fbb3bfe),
  LOAD_SW16( CPU1, "s92_21a.6f", 0x100000, 0x80000, 0x925a7877),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2cejc[] = // clone of sf2ce
{
  LOAD_SW16( CPU1, "s92j_23c.8f", 0x000000, 0x80000, 0xf0120635),
  LOAD_SW16( CPU1, "s92j_22c.7f", 0x080000, 0x80000, 0x8c0b2ed6),
  LOAD_SW16( CPU1, "s92j_21a.6f", 0x100000, 0x80000, 0x925a7877),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2rb[] = // clone of sf2ce
{
  LOAD( CPU1, "sf2d__23.rom", 0x000000, 0x80000, 0x450532b0),
  LOAD( CPU1, "sf2d__22.rom", 0x080000, 0x80000, 0xfe9d9cf5),
  LOAD_SW16( CPU1, "s92_21a.bin", 0x100000, 0x80000, 0x925a7877),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2rb2[] = // clone of sf2ce
{
  LOAD_16_8( CPU1, "27.bin", 0x000000, 0x20000, 0x40296ecd),
  LOAD_16_8( CPU1, "31.bin", 0x000001, 0x20000, 0x87954a41),
  LOAD_16_8( CPU1, "26.bin", 0x040000, 0x20000, 0xa6974195),
  LOAD_16_8( CPU1, "30.bin", 0x040001, 0x20000, 0x8141fe32),
  LOAD_16_8( CPU1, "25.bin", 0x080000, 0x20000, 0x9ef8f772),
  LOAD_16_8( CPU1, "29.bin", 0x080001, 0x20000, 0x7d9c479c),
  LOAD_16_8( CPU1, "24.bin", 0x0c0000, 0x20000, 0x93579684),
  LOAD_16_8( CPU1, "28.bin", 0x0c0001, 0x20000, 0xff728865),
  LOAD_SW16( CPU1, "s92_21a.bin", 0x100000, 0x80000, 0x925a7877),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2rb3[] = // clone of sf2ce
{
  LOAD_SW16( CPU1, "sf2_ce_rb.23", 0x000000, 0x80000, 0x202f9e50),
  LOAD_SW16( CPU1, "sf2_ce_rb.22", 0x080000, 0x80000, 0x145e5219),
  LOAD_SW16( CPU1, "s92_21a.bin", 0x100000, 0x80000, 0x925a7877),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2red[] = // clone of sf2ce
{
  LOAD_SW16( CPU1, "sf2red.23", 0x000000, 0x80000, 0x40276abb),
  LOAD_SW16( CPU1, "sf2red.22", 0x080000, 0x80000, 0x18daf387),
  LOAD_SW16( CPU1, "sf2red.21", 0x100000, 0x80000, 0x52c486bb),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2v004[] = // clone of sf2ce
{
  LOAD_SW16( CPU1, "sf2v004.23", 0x000000, 0x80000, 0x52d19f2c),
  LOAD_SW16( CPU1, "sf2v004.22", 0x080000, 0x80000, 0x4b26fde7),
  LOAD_SW16( CPU1, "sf2red.21", 0x100000, 0x80000, 0x52c486bb),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2acc[] = // clone of sf2ce
{
  LOAD_SW16( CPU1, "sf2ca_23-c.bin", 0x000000, 0x80000, 0xe7c8c5a6),
  LOAD_SW16( CPU1, "sf2ca_22-c.bin", 0x080000, 0x80000, 0x99f1cca4),
  LOAD_SW16( CPU1, "sf2ca_21-c.bin", 0x100000, 0x40000, 0xcf7fcc8c),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2acca[] = // clone of sf2ce
{
  LOAD_SW16( CPU1, "23-c.8f", 0x000000, 0x80000, 0x35f9517b),
  LOAD_SW16( CPU1, "22-c.7f", 0x080000, 0x80000, 0x99f1cca4),
  LOAD_SW16( CPU1, "21-c.6f", 0x100000, 0x80000, 0x2ab2034f),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2accp2[] = // clone of sf2ce
{
  LOAD_SW16( CPU1, "sf2ca-23.bin", 0x000000, 0x80000, 0x36c3ba2f),
  LOAD_SW16( CPU1, "sf2ca-22.bin", 0x080000, 0x80000, 0x0550453d),
	/* ROM space ends at 13ffff, but the code checks 180ca6 and */
	/* crashes if it isn't 0 - protection? */
  LOAD_SW16( CPU1, "sf2ca-21.bin", 0x100000, 0x40000, 0x4c1c43ba),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2dkot2[] = // clone of sf2ce
{
	/* The game reads values from 0x201201 and 0x281201 (at PC=E5452) and uses their difference to form a jump offset. */
	/* This arrangement seems to work */
  LOAD_SW16( CPU1, "turboii.23", 0x000000, 0x80000, 0x9bbfe420),
  LOAD_SW16( CPU1, "turboii.23", 0x280000, 0x80000, 0x9bbfe420),
  LOAD_SW16( CPU1, "turboii.22", 0x080000, 0x80000, 0x3e57ba19),
  LOAD_SW16( CPU1, "turboii.22", 0x200000, 0x80000, 0x3e57ba19),
  LOAD_SW16( CPU1, "turboii.21", 0x100000, 0x80000, 0xed4186bd),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2m2[] = // clone of sf2ce
{
  LOAD_16_8( CPU1, "ch222esp", 0x000000, 0x80000, 0x9e6d058a),
  LOAD_16_8( CPU1, "ch196esp", 0x000001, 0x80000, 0xed2ff437),
  LOAD_SW16( CPU1, "s92_21a.bin", 0x100000, 0x80000, 0x925a7877),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2m3[] = // clone of sf2ce
{
  LOAD_16_8( CPU1, "u222chp", 0x000000, 0x80000, 0xdb567b66),
  LOAD_16_8( CPU1, "u196chp", 0x000001, 0x80000, 0x95ea597e),
  LOAD_SW16( CPU1, "s92_21a.bin", 0x100000, 0x80000, 0x925a7877),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2m4[] = // clone of sf2ce
{
  LOAD_16_8( CPU1, "u222ne", 0x000000, 0x80000, 0x7133489e),
  LOAD_16_8( CPU1, "u196ne", 0x000001, 0x80000, 0xb07a4f90),
  LOAD_SW16( CPU1, "s92_21a.bin", 0x100000, 0x80000, 0x925a7877),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2m5[] = // clone of sf2ce
{
  LOAD_16_8( CPU1, "u222", 0x000000, 0x80000, 0x03991fba),
  LOAD_16_8( CPU1, "u196", 0x000001, 0x80000, 0x39f15a1e),
  LOAD_SW16( CPU1, "s92_21a.bin", 0x100000, 0x80000, 0x925a7877),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2m6[] = // clone of sf2ce
{
  LOAD_16_8( CPU1, "u222-6b", 0x000000, 0x80000, 0x0a3692be),
  LOAD_16_8( CPU1, "u196-6b", 0x000001, 0x80000, 0x80454da7),
  LOAD_SW16( CPU1, "s92_21a.bin", 0x100000, 0x80000, 0x925a7877),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2m7[] = // clone of sf2ce
{
  LOAD_16_8( CPU1, "u222-2i", 0x000000, 0x40000, 0x1ca7adbd),
  LOAD_16_8( CPU1, "u196-2i", 0x000001, 0x40000, 0xf758408c),
  LOAD_16_8( CPU1, "u222-2s", 0x080000, 0x40000, 0x720cea3e),
  LOAD_16_8( CPU1, "u196-2s", 0x080001, 0x40000, 0x9932832c),
  LOAD_SW16( CPU1, "s92_21a.bin", 0x100000, 0x80000, 0x925a7877),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2m8[] = // clone of sf2ce
{
  LOAD_16_8( CPU1, "yyc-2.2", 0x000000, 0x80000, 0xdb567b66),
  LOAD_16_8( CPU1, "yyc-3.4", 0x000001, 0x80000, 0x95ea597e),
  LOAD_16_8( CPU1, "yyc-4.1", 0x100000, 0x20000, 0x1073b7b6),
  LOAD_16_8( CPU1, "yyc-5.3", 0x100001, 0x20000, 0x924c6ce2),
  { "yyc-a", 0x40000, 0x8242621f, REGION_GFX1, 0x000000, LOAD_8_64 },
  { "yyc-a", 0x40000, 0x8242621f, REGION_GFX1, 0x000004, LOAD_CONTINUE },
  { "yyc-a", 0x40000, 0x8242621f, REGION_GFX1, 0x200000, LOAD_CONTINUE },
  { "yyc-a", 0x40000, 0x8242621f, REGION_GFX1, 0x200004, LOAD_CONTINUE },
  { "yyc-c", 0x40000, 0x0793a960, REGION_GFX1, 0x000001, LOAD_8_64 },
  { "yyc-c", 0x40000, 0x0793a960, REGION_GFX1, 0x000005, LOAD_CONTINUE },
  { "yyc-c", 0x40000, 0x0793a960, REGION_GFX1, 0x200001, LOAD_CONTINUE },
  { "yyc-c", 0x40000, 0x0793a960, REGION_GFX1, 0x200005, LOAD_CONTINUE },
  { "yyc-b", 0x40000, 0xb0159973, REGION_GFX1, 0x000002, LOAD_8_64 },
  { "yyc-b", 0x40000, 0xb0159973, REGION_GFX1, 0x000006, LOAD_CONTINUE },
  { "yyc-b", 0x40000, 0xb0159973, REGION_GFX1, 0x200002, LOAD_CONTINUE },
  { "yyc-b", 0x40000, 0xb0159973, REGION_GFX1, 0x200006, LOAD_CONTINUE },
  { "yyc-d", 0x40000, 0x92a8b572, REGION_GFX1, 0x000003, LOAD_8_64 },
  { "yyc-d", 0x40000, 0x92a8b572, REGION_GFX1, 0x000007, LOAD_CONTINUE },
  { "yyc-d", 0x40000, 0x92a8b572, REGION_GFX1, 0x200003, LOAD_CONTINUE },
	/* extra gfx layer roms loaded over the former ones to remove the capcom copyright logo */
  { "yyc-d", 0x40000, 0x92a8b572, REGION_GFX1, 0x200007, LOAD_CONTINUE },
  { "yyc-6.1", 0x10000, 0x94778332, REGION_GFX1, 0x400000, LOAD_8_64 },
  { "yyc-6.1", 0x10000 , 0x94778332, REGION_GFX1, 0x400004, LOAD_CONTINUE },
  { "yyc-8.9", 0x10000, 0xf95bc505, REGION_GFX1, 0x400002, LOAD_8_64 },
  { "yyc-8.9", 0x10000 , 0xf95bc505, REGION_GFX1, 0x400006, LOAD_CONTINUE },
  { "yyc-7.10", 0x10000, 0xd1e452d3, REGION_GFX1, 0x400001, LOAD_8_64 },
  { "yyc-7.10", 0x10000 , 0xd1e452d3, REGION_GFX1, 0x400005, LOAD_CONTINUE },
  { "yyc-9.8", 0x10000, 0x155824a9, REGION_GFX1, 0x400003, LOAD_8_64 },
	/* end of extra gfx layer roms */
  { "yyc-9.8", 0x10000 , 0x155824a9, REGION_GFX1, 0x400007, LOAD_CONTINUE },
  // Still need a rom to fill up until $600000, should add something for that
  FILL( GFX1, 0x5f0000, 0x10000, 0),
  LOAD( ROM2, "a-15.5", 0x00000, 0x10000, 0x6f07d2cb),
  LOAD( USER1, "c-27.7", 0x00000, 0x10000, 0x13ea1c44),
  LOAD( USER2, "yyc-e", 0x000000, 0x100000, 0x61138469),
  LOAD( USER2, "yyc-f", 0x100000, 0x100000, 0xb800dcdb),
  LOAD( SMP1, "b-16.6", 0x00000, 0x40000, 0x6cfffb11),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2yyc[] = // clone of sf2ce
{
  LOAD_16_8( CPU1, "b12.rom", 0x000000, 0x80000, 0x8f742fd5),
  LOAD_16_8( CPU1, "b14.rom", 0x000001, 0x80000, 0x8831ec7f),
  LOAD_16_8( CPU1, "b11.rom", 0x100000, 0x20000, 0x94a46525),
  LOAD_16_8( CPU1, "b11.rom", 0x140000, 0x20000, 0x94a46525),
  LOAD_16_8( CPU1, "b13.rom", 0x100001, 0x20000, 0x8fb3dd47),
  LOAD_16_8( CPU1, "b13.rom", 0x140001, 0x20000, 0x8fb3dd47),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2koryu[] = // clone of sf2ce
{
  LOAD_16_8( CPU1, "u222.rom", 0x000000, 0x80000, 0x9236a79a),
  LOAD_16_8( CPU1, "u196.rom", 0x000001, 0x80000, 0xb23a869d),
  LOAD_16_8( CPU1, "u221.rom", 0x100000, 0x20000, 0x64e6e091),
  LOAD_16_8( CPU1, "u195.rom", 0x100001, 0x20000, 0xc95e4443),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2dongb[] = // clone of sf2ce
{
  LOAD_SW16( CPU1, "1.8f", 0x000000, 0x80000, 0x19fffa37),
  LOAD_SW16( CPU1, "1.7f", 0x080000, 0x80000, 0x99f1cca4),
  LOAD_SW16( CPU1, "1.6f", 0x100000, 0x80000, 0x65c2c719),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_cworld2j[] =
{
  LOAD_16_8( CPU1, "q5_36.12f", 0x00000, 0x20000, 0x38a08099),
  LOAD_16_8( CPU1, "q5_42.12h", 0x00001, 0x20000, 0x4d29b3a4),
  LOAD_16_8( CPU1, "q5_37.13f", 0x40000, 0x20000, 0xeb547ebc),
  LOAD_16_8( CPU1, "q5_43.13h", 0x40001, 0x20000, 0x3ef65ea8),
  LOAD_16_8( CPU1, "q5_34.10f", 0x80000, 0x20000, 0x7fcc1317),
  LOAD_16_8( CPU1, "q5_40.10h", 0x80001, 0x20000, 0x7f14b7b4),
//  ROM_LOAD16_BYTE( "q5_35(__alt).11f", 0xc0000, 0x20000, CRC(59961612) SHA1(ded5144746dd9612f2db1415e96a826e215ad176) )   /* dumped from another board, 1 byte different, pcb verified */
  LOAD_16_8( CPU1, "q5_35.11f", 0xc0000, 0x20000, 0xabacee26),
  LOAD_16_8( CPU1, "q5_41.11h", 0xc0001, 0x20000, 0xd3654067),
  { "q5_09.4b", 0x20000, 0x48496d80, REGION_GFX1, 0x000000, LOAD_8_64 },
  { "q5_01.4a", 0x20000, 0xc5453f56, REGION_GFX1, 0x000001, LOAD_8_64 },
  { "q5_13.9b", 0x20000, 0xc741ac52, REGION_GFX1, 0x000002, LOAD_8_64 },
  { "q5_05.9a", 0x20000, 0x143e068f, REGION_GFX1, 0x000003, LOAD_8_64 },
  { "q5_24.5e", 0x20000, 0xb419d139, REGION_GFX1, 0x000004, LOAD_8_64 },
  { "q5_17.5c", 0x20000, 0xbd3b4d11, REGION_GFX1, 0x000005, LOAD_8_64 },
  { "q5_38.8h", 0x20000, 0x9c24670c, REGION_GFX1, 0x000006, LOAD_8_64 },
  { "q5_32.8f", 0x20000, 0x3ef9c7c2, REGION_GFX1, 0x000007, LOAD_8_64 },
  { "q5_10.5b", 0x20000, 0x119e5e93, REGION_GFX1, 0x100000, LOAD_8_64 },
  { "q5_02.5a", 0x20000, 0xa2cadcbe, REGION_GFX1, 0x100001, LOAD_8_64 },
  { "q5_14.10b", 0x20000, 0xa8755f82, REGION_GFX1, 0x100002, LOAD_8_64 },
  { "q5_06.10a", 0x20000, 0xc92a91fc, REGION_GFX1, 0x100003, LOAD_8_64 },
  { "q5_25.7e", 0x20000, 0x979237cb, REGION_GFX1, 0x100004, LOAD_8_64 },
  { "q5_18.7c", 0x20000, 0xc57da03c, REGION_GFX1, 0x100005, LOAD_8_64 },
  { "q5_39.9h", 0x20000, 0xa5839b25, REGION_GFX1, 0x100006, LOAD_8_64 },
  { "q5_33.9f", 0x20000, 0x04d03930, REGION_GFX1, 0x100007, LOAD_8_64 },
  LOAD( ROM2, "q5_23.13b", 0x00000, 0x10000, 0xe14dc524),
  LOAD( SMP1, "q5_30.12c", 0x00000, 0x20000, 0xd10c1b68),
  LOAD( SMP1, "q5_31.13c", 0x20000, 0x20000, 0x7d17e496),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_varth[] =
{
  LOAD_16_8( CPU1, "vae_30b.11f", 0x00000, 0x20000, 0xadb8d391),
  LOAD_16_8( CPU1, "vae_35b.11h", 0x00001, 0x20000, 0x44e5548f),
  LOAD_16_8( CPU1, "vae_31b.12f", 0x40000, 0x20000, 0x1749a71c),
  LOAD_16_8( CPU1, "vae_36b.12h", 0x40001, 0x20000, 0x5f2e2450),
  LOAD_16_8( CPU1, "vae_28b.9f", 0x80000, 0x20000, 0xe524ca50),
  LOAD_16_8( CPU1, "vae_33b.9h", 0x80001, 0x20000, 0xc0bbf8c9),
  LOAD_16_8( CPU1, "vae_29b.10f", 0xc0000, 0x20000, 0x6640996a),
  LOAD_16_8( CPU1, "vae_34b.10h", 0xc0001, 0x20000, 0xfa59be8a),
  { "va-5m.7a", 0x80000, 0xb1fb726e, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "va-7m.9a", 0x80000, 0x4c6588cd, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "va-1m.3a", 0x80000, 0x0b1ace37, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "va-3m.5a", 0x80000, 0x44dfe706, REGION_GFX1, 0x000006, LOAD_16_64 },
  LOAD( ROM2, "va_09.12b", 0x00000, 0x10000, 0x7a99446e),
  LOAD( SMP1, "va_18.11c", 0x00000, 0x20000, 0xde30510e),
  LOAD( SMP1, "va_19.12c", 0x20000, 0x20000, 0x0610a4ac),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_varthr1[] = // clone of varth
{
  LOAD_16_8( CPU1, "vae_30a.11f", 0x00000, 0x20000, 0x7fcd0091),
  LOAD_16_8( CPU1, "vae_35a.11h", 0x00001, 0x20000, 0x35cf9509),
  LOAD_16_8( CPU1, "vae_31a.12f", 0x40000, 0x20000, 0x15e5ee81),
  LOAD_16_8( CPU1, "vae_36a.12h", 0x40001, 0x20000, 0x153a201e),
  LOAD_16_8( CPU1, "vae_28a.9f", 0x80000, 0x20000, 0x7a0e0d25),
  LOAD_16_8( CPU1, "vae_33a.9h", 0x80001, 0x20000, 0xf2365922),
  LOAD_16_8( CPU1, "vae_29a.10f", 0xc0000, 0x20000, 0x5e2cd2c3),
  LOAD_16_8( CPU1, "vae_34a.10h", 0xc0001, 0x20000, 0x3d9bdf83),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_varthu[] = // clone of varth
{
  LOAD_SW16( CPU1, "vau_23a.8f", 0x00000, 0x80000, 0xfbe68726),
  LOAD_SW16( CPU1, "vau_22a.7f", 0x80000, 0x80000, 0x0ed71bbd),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_varthj[] = // clone of varth
{
  LOAD_16_8( CPU1, "vaj_36b.12f", 0x00000, 0x20000, 0x1d798d6a),
  LOAD_16_8( CPU1, "vaj_42b.12h", 0x00001, 0x20000, 0x0f720233),
  LOAD_16_8( CPU1, "vaj_37b.13f", 0x40000, 0x20000, 0x24414b17),
  LOAD_16_8( CPU1, "vaj_43b.13h", 0x40001, 0x20000, 0x34b4b06c),
  LOAD_16_8( CPU1, "vaj_34b.10f", 0x80000, 0x20000, 0x87c79aed),
  LOAD_16_8( CPU1, "vaj_40b.10h", 0x80001, 0x20000, 0x210b4bd0),
  LOAD_16_8( CPU1, "vaj_35b.11f", 0xc0000, 0x20000, 0x6b0da69f),
  LOAD_16_8( CPU1, "vaj_41b.11h", 0xc0001, 0x20000, 0x6542c8a4),
  { "va_09.4b", 0x20000, 0x183dfaa8, REGION_GFX1, 0x000000, LOAD_8_64 },
  { "va_01.4a", 0x20000, 0xc41312b5, REGION_GFX1, 0x000001, LOAD_8_64 },
  { "va_13.9b", 0x20000, 0x45537e69, REGION_GFX1, 0x000002, LOAD_8_64 },
  { "va_05.9a", 0x20000, 0x7065d4e9, REGION_GFX1, 0x000003, LOAD_8_64 },
  { "va_24.5e", 0x20000, 0x57191ccf, REGION_GFX1, 0x000004, LOAD_8_64 },
  { "va_17.5c", 0x20000, 0x054f5a5b, REGION_GFX1, 0x000005, LOAD_8_64 },
  { "va_38.8h", 0x20000, 0xe117a17e, REGION_GFX1, 0x000006, LOAD_8_64 },
  { "va_32.8f", 0x20000, 0x3b4f40b2, REGION_GFX1, 0x000007, LOAD_8_64 },
  { "va_10.5b", 0x20000, 0xd62750cd, REGION_GFX1, 0x100000, LOAD_8_64 },
  { "va_02.5a", 0x20000, 0x11590325, REGION_GFX1, 0x100001, LOAD_8_64 },
  { "va_14.10b", 0x20000, 0xdc2f4783, REGION_GFX1, 0x100002, LOAD_8_64 },
  { "va_06.10a", 0x20000, 0x06e833ac, REGION_GFX1, 0x100003, LOAD_8_64 },
  { "va_25.7e", 0x20000, 0x51d90690, REGION_GFX1, 0x100004, LOAD_8_64 },
  { "va_18.7c", 0x20000, 0xa17817c0, REGION_GFX1, 0x100005, LOAD_8_64 },
  { "va_39.9h", 0x20000, 0xb0b12f51, REGION_GFX1, 0x100006, LOAD_8_64 },
  { "va_33.9f", 0x20000, 0x4b003af7, REGION_GFX1, 0x100007, LOAD_8_64 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_qad[] =
{
  LOAD_16_8( CPU1, "qdu_36a.12f", 0x00000, 0x20000, 0xde9c24a0),
  LOAD_16_8( CPU1, "qdu_42a.12h", 0x00001, 0x20000, 0xcfe36f0c),
  LOAD_16_8( CPU1, "qdu_37a.13f", 0x40000, 0x20000, 0x10d22320),
  LOAD_16_8( CPU1, "qdu_43a.13h", 0x40001, 0x20000, 0x15e6beb9),
  { "qd_09.4b", 0x20000, 0x8c3f9f44, REGION_GFX1, 0x000000, LOAD_8_64 },
  { "qd_01.4a", 0x20000, 0xf688cf8f, REGION_GFX1, 0x000001, LOAD_8_64 },
  { "qd_13.9b", 0x20000, 0xafbd551b, REGION_GFX1, 0x000002, LOAD_8_64 },
  { "qd_05.9a", 0x20000, 0xc3db0910, REGION_GFX1, 0x000003, LOAD_8_64 },
  { "qd_24.5e", 0x20000, 0x2f1bd0ec, REGION_GFX1, 0x000004, LOAD_8_64 },
  { "qd_17.5c", 0x20000, 0xa812f9e2, REGION_GFX1, 0x000005, LOAD_8_64 },
  { "qd_38.8h", 0x20000, 0xccdddd1f, REGION_GFX1, 0x000006, LOAD_8_64 },
  { "qd_32.8f", 0x20000, 0xa8d295d3, REGION_GFX1, 0x000007, LOAD_8_64 },
  LOAD( ROM2, "qd_23.13b", 0x00000, 0x10000, 0xcfb5264b),
  LOAD( SMP1, "qdu_30.12c", 0x00000, 0x20000, 0xf190da84),
  LOAD( SMP1, "qdu_31.13c", 0x20000, 0x20000, 0xb7583f73),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_qadjr[] = // clone of qad
{
  LOAD_SW16( CPU1, "qad_23a.8f", 0x00000, 0x80000, 0x4d3553de),
  LOAD_SW16( CPU1, "qad_22a.7f", 0x80000, 0x80000, 0x3191ddd0),
  { "qad_01.3a", 0x80000, 0x9d853b57, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "qad_02.4a", 0x80000, 0xb35976c4, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "qad_03.5a", 0x80000, 0xcea4ca8c, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "qad_04.6a", 0x80000, 0x41b74d1b, REGION_GFX1, 0x000006, LOAD_16_64 },
  LOAD( ROM2, "qad_09.12a", 0x00000, 0x10000, 0x733161cc),
  LOAD( SMP1, "qad_18.11c", 0x00000, 0x20000, 0x2bfe6f6a),
  LOAD( SMP1, "qad_19.12c", 0x20000, 0x20000, 0x13d3236b),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_wof[] =
{
  LOAD_SW16( CPU1, "tk2e_23c.8f", 0x000000, 0x80000, 0x0d708505),
  LOAD_SW16( CPU1, "tk2e_22c.7f", 0x080000, 0x80000, 0x608c17e3),
  { "tk2-1m.3a", 0x80000, 0x0d9cb9bf, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "tk2-3m.5a", 0x80000, 0x45227027, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "tk2-2m.4a", 0x80000, 0xc5ca2460, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "tk2-4m.6a", 0x80000, 0xe349551c, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "tk2-5m.7a", 0x80000, 0x291f0f0b, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "tk2-7m.9a", 0x80000, 0x3edeb949, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "tk2-6m.8a", 0x80000, 0x1abd14d6, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "tk2-8m.10a", 0x80000, 0xb27948e3, REGION_GFX1, 0x200006, LOAD_16_64 },
  LOAD( ROM2, "tk2_qa.5k", 0x00000, 0x20000, 0xc9183a0d),
  LOAD( SMP1, "tk2-q1.1k", 0x000000, 0x80000, 0x611268cf),
  LOAD( SMP1, "tk2-q2.2k", 0x080000, 0x80000, 0x20f55ca9),
  LOAD( SMP1, "tk2-q3.3k", 0x100000, 0x80000, 0xbfcf6f52),
  LOAD( SMP1, "tk2-q4.4k", 0x180000, 0x80000, 0x36642e88),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_wofr1[] = // clone of wof
{
  LOAD_SW16( CPU1, "tk2e_23b.8f", 0x000000, 0x80000, 0x11fb2ed1),
  LOAD_SW16( CPU1, "tk2e_22b.7f", 0x080000, 0x80000, 0x479b3f24),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_wofu[] = // clone of wof
{
  LOAD_SW16( CPU1, "tk2u_23c.8f", 0x000000, 0x80000, 0x29b89c12),
  LOAD_SW16( CPU1, "tk2u_22c.7f", 0x080000, 0x80000, 0xf5af4774),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_wofa[] = // clone of wof
{
  LOAD_SW16( CPU1, "tk2a_23c.8f", 0x000000, 0x80000, 0x2e024628),
  LOAD_SW16( CPU1, "tk2a_22c.7f", 0x080000, 0x80000, 0x900ad4cd),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_wofj[] = // clone of wof
{
  LOAD_SW16( CPU1, "tk2j_23c.8f", 0x000000, 0x80000, 0x9b215a68),
  LOAD_SW16( CPU1, "tk2j_22c.7f", 0x080000, 0x80000, 0xb74b09ac),
  { "tk2_01.3a", 0x80000, 0x0d9cb9bf, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "tk2_02.4a", 0x80000, 0x45227027, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "tk2_03.5a", 0x80000, 0xc5ca2460, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "tk2_04.6a", 0x80000, 0xe349551c, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "tk2_05.7a", 0x80000, 0xe4a44d53, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "tk2_06.8a", 0x80000, 0x58066ba8, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "tk2_07.9a", 0x80000, 0xd706568e, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "tk2_08.10a", 0x80000, 0xd4a19a02, REGION_GFX1, 0x200006, LOAD_16_64 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_wofhfh[] = // clone of wof
{
  LOAD_SW16( CPU1, "23", 0x000000, 0x80000, 0x6ae4b312),
  LOAD_SW16( CPU1, "22", 0x080000, 0x80000, 0x94e8d01a),
  { "1", 0x80000, 0x0d9cb9bf, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "2", 0x80000, 0x45227027, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "3", 0x80000, 0xc5ca2460, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "4", 0x80000, 0xe349551c, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "5", 0x80000, 0x34949d7b, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "6", 0x80000, 0xdfa70971, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "7", 0x80000, 0x073686a6, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "8", 0x80000, 0x5300f8db, REGION_GFX1, 0x200006, LOAD_16_64 },
  LOAD( ROM2, "9", 0x00000, 0x20000, 0x86fe8a97),
  LOAD( SMP1, "18", 0x00000, 0x20000, 0xc04be720),
  LOAD( SMP1, "19", 0x20000, 0x20000, 0xfbb8d8c1),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2hf[] =
{
  LOAD_SW16( CPU1, "s2te_23.8f", 0x000000, 0x80000, 0x2dd72514),
  LOAD_SW16( CPU1, "s2te_22.7f", 0x080000, 0x80000, 0xaea6e035),
  LOAD_SW16( CPU1, "s2te_21.6f", 0x100000, 0x80000, 0xfd200288),
  { "s92-1m.3a", 0x80000, 0x03b0d852, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "s92-3m.5a", 0x80000, 0x840289ec, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "s92-2m.4a", 0x80000, 0xcdb5f027, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "s92-4m.6a", 0x80000, 0xe2799472, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "s92-5m.7a", 0x80000, 0xba8a2761, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "s92-7m.9a", 0x80000, 0xe584bfb5, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "s92-6m.8a", 0x80000, 0x21e3f87d, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "s92-8m.10a", 0x80000, 0xbefc47df, REGION_GFX1, 0x200006, LOAD_16_64 },
  { "s92-10m.3c", 0x80000, 0x960687d5, REGION_GFX1, 0x400000, LOAD_16_64 },
  { "s92-12m.5c", 0x80000, 0x978ecd18, REGION_GFX1, 0x400002, LOAD_16_64 },
  { "s92-11m.4c", 0x80000, 0xd6ec9a0a, REGION_GFX1, 0x400004, LOAD_16_64 },
  { "s92-13m.6c", 0x80000, 0xed2c67f6, REGION_GFX1, 0x400006, LOAD_16_64 },
  LOAD( ROM2, "s92_09.11a", 0x00000, 0x10000, 0x08f6b60e),
  LOAD( SMP1, "s92_18.11c", 0x00000, 0x20000, 0x7f162009),
  LOAD( SMP1, "s92_19.12c", 0x20000, 0x20000, 0xbeade53f),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2hfu[] = // clone of sf2hf
{
  LOAD_SW16( CPU1, "s2tu_23.8f", 0x000000, 0x80000, 0x89a1fc38),
  LOAD_SW16( CPU1, "s2tu_22.7f", 0x080000, 0x80000, 0xaea6e035),
  LOAD_SW16( CPU1, "s2tu_21.6f", 0x100000, 0x80000, 0xfd200288),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sf2hfj[] = // clone of sf2hf
{
  LOAD_SW16( CPU1, "s2tj_23.8f", 0x000000, 0x80000, 0xea73b4dc),
  LOAD_SW16( CPU1, "s2tj_22.7f", 0x080000, 0x80000, 0xaea6e035),
  LOAD_SW16( CPU1, "s2tj_21.6f", 0x100000, 0x80000, 0xfd200288),
  { "s92_01.3a", 0x80000, 0x03b0d852, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "s92_02.4a", 0x80000, 0x840289ec, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "s92_03.5a", 0x80000, 0xcdb5f027, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "s92_04.6a", 0x80000, 0xe2799472, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "s92_05.7a", 0x80000, 0xba8a2761, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "s92_06.8a", 0x80000, 0xe584bfb5, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "s92_07.9a", 0x80000, 0x21e3f87d, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "s92_08.10a", 0x80000, 0xbefc47df, REGION_GFX1, 0x200006, LOAD_16_64 },
  { "s2t_10.3c", 0x80000, 0x3c042686, REGION_GFX1, 0x400000, LOAD_16_64 },
  { "s2t_11.4c", 0x80000, 0x8b7e7183, REGION_GFX1, 0x400002, LOAD_16_64 },
  { "s2t_12.5c", 0x80000, 0x293c888c, REGION_GFX1, 0x400004, LOAD_16_64 },
  { "s2t_13.6c", 0x80000, 0x842b35a4, REGION_GFX1, 0x400006, LOAD_16_64 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_dino[] =
{
  LOAD_SW16( CPU1, "cde_23a.8f", 0x000000, 0x80000, 0x8f4e585e),
  LOAD_SW16( CPU1, "cde_22a.7f", 0x080000, 0x80000, 0x9278aa12),
  LOAD_SW16( CPU1, "cde_21a.6f", 0x100000, 0x80000, 0x66d23de2),
  { "cd-1m.3a", 0x80000, 0x8da4f917, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "cd-3m.5a", 0x80000, 0x6c40f603, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "cd-2m.4a", 0x80000, 0x09c8fc2d, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "cd-4m.6a", 0x80000, 0x637ff38f, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "cd-5m.7a", 0x80000, 0x470befee, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "cd-7m.9a", 0x80000, 0x22bfb7a3, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "cd-6m.8a", 0x80000, 0xe7599ac4, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "cd-8m.10a", 0x80000, 0x211b4b15, REGION_GFX1, 0x200006, LOAD_16_64 },
  LOAD( ROM2, "cd_q.5k", 0x00000, 0x20000, 0x605fdb0b),
  LOAD( SMP1, "cd-q1.1k", 0x000000, 0x80000, 0x60927775),
  LOAD( SMP1, "cd-q2.2k", 0x080000, 0x80000, 0x770f4c47),
  LOAD( SMP1, "cd-q3.3k", 0x100000, 0x80000, 0x2f273ffc),
  LOAD( SMP1, "cd-q4.4k", 0x180000, 0x80000, 0x2c67821d),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_dinou[] = // clone of dino
{
  LOAD_SW16( CPU1, "cdu_23a.8f", 0x000000, 0x80000, 0x7c2543cd),
  LOAD_SW16( CPU1, "cdu_22a.7f", 0x080000, 0x80000, 0xfab740a9),
  LOAD_SW16( CPU1, "cdu_21a.6f", 0x100000, 0x80000, 0x66d23de2),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_dinoj[] = // clone of dino
{
  LOAD_SW16( CPU1, "cdj_23a.8f", 0x000000, 0x80000, 0x5f3ece96),
  LOAD_SW16( CPU1, "cdj_22a.7f", 0x080000, 0x80000, 0xa0d8de29),
  LOAD_SW16( CPU1, "cdj_21a.6f", 0x100000, 0x80000, 0x66d23de2),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_dinohunt[] = // clone of dino
{
  LOAD_SW16( CPU1, "u23", 0x000000, 0x80000, 0x8d5ddc5d),
  LOAD_SW16( CPU1, "u22", 0x080000, 0x80000, 0xf72cd219),
  LOAD_SW16( CPU1, "u21", 0x100000, 0x80000, 0xbc275b76),
  LOAD_SW16( CPU1, "u20", 0x180000, 0x80000, 0x8987c975),
  { "u1", 0x80000, 0xa01a9fb5, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "u2", 0x80000, 0xbdf02c17, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "u3", 0x80000, 0x058beefa, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "u4", 0x80000, 0x5028a9f1, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "u5", 0x80000, 0xd77f89ea, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "u6", 0x80000, 0xbfbcb034, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "u7", 0x80000, 0xa2544d4e, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "u8", 0x80000, 0x8869bbb1, REGION_GFX1, 0x200006, LOAD_16_64 },
  LOAD( ROM2, "u9", 0x00000, 0x10000, 0x2eb16a83),
  LOAD( SMP1, "u18", 0x00000, 0x20000, 0x8d2899ba),
  LOAD( SMP1, "u19", 0x20000, 0x20000, 0xb34a4b42),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_punisher[] =
{
  LOAD_16_8( CPU1, "pse_26.11e", 0x000000, 0x20000, 0x389a99d2),
  LOAD_16_8( CPU1, "pse_30.11f", 0x000001, 0x20000, 0x68fb06ac),
  LOAD_16_8( CPU1, "pse_27.12e", 0x040000, 0x20000, 0x3eb181c3),
  LOAD_16_8( CPU1, "pse_31.12f", 0x040001, 0x20000, 0x37108e7b),
  LOAD_16_8( CPU1, "pse_24.9e", 0x080000, 0x20000, 0x0f434414),
  LOAD_16_8( CPU1, "pse_28.9f", 0x080001, 0x20000, 0xb732345d),
  LOAD_16_8( CPU1, "pse_25.10e", 0x0c0000, 0x20000, 0xb77102e2),
  LOAD_16_8( CPU1, "pse_29.10f", 0x0c0001, 0x20000, 0xec037bce),
  LOAD_SW16( CPU1, "ps_21.6f", 0x100000, 0x80000, 0x8affa5a9),
  { "ps-1m.3a", 0x80000, 0x77b7ccab, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "ps-3m.5a", 0x80000, 0x0122720b, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "ps-2m.4a", 0x80000, 0x64fa58d4, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "ps-4m.6a", 0x80000, 0x60da42c8, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "ps-5m.7a", 0x80000, 0xc54ea839, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "ps-7m.9a", 0x80000, 0x04c5acbd, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "ps-6m.8a", 0x80000, 0xa544f4cc, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "ps-8m.10a", 0x80000, 0x8f02f436, REGION_GFX1, 0x200006, LOAD_16_64 },
  LOAD( ROM2, "ps_q.5k", 0x00000, 0x20000, 0x49ff4446),
  LOAD( SMP1, "ps-q1.1k", 0x000000, 0x80000, 0x31fd8726),
  LOAD( SMP1, "ps-q2.2k", 0x080000, 0x80000, 0x980a9eef),
  LOAD( SMP1, "ps-q3.3k", 0x100000, 0x80000, 0x0dd44491),
  LOAD( SMP1, "ps-q4.4k", 0x180000, 0x80000, 0xbed42f03),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_punisheru[] = // clone of punisher
{
  LOAD_16_8( CPU1, "psu_26.11e", 0x000000, 0x20000, 0x9236d121),
  LOAD_16_8( CPU1, "psu_30.11f", 0x000001, 0x20000, 0x8320e501),
  LOAD_16_8( CPU1, "psu_27.12e", 0x040000, 0x20000, 0x61c960a1),
  LOAD_16_8( CPU1, "psu_31.12f", 0x040001, 0x20000, 0x78d4c298),
  LOAD_16_8( CPU1, "psu_24.9e", 0x080000, 0x20000, 0x1cfecad7),
  LOAD_16_8( CPU1, "psu_28.9f", 0x080001, 0x20000, 0xbdf921c1),
  LOAD_16_8( CPU1, "psu_25.10e", 0x0c0000, 0x20000, 0xc51acc94),
  LOAD_16_8( CPU1, "psu_29.10f", 0x0c0001, 0x20000, 0x52dce1ca),
  LOAD_SW16( CPU1, "ps_21.6f", 0x100000, 0x80000, 0x8affa5a9),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_punisherh[] = // clone of punisher
{
  LOAD_16_8( CPU1, "psh_26.11e", 0x000000, 0x20000, 0x6ad2bb83),
  LOAD_16_8( CPU1, "psh_30.11f", 0x000001, 0x20000, 0x058d3659),
  LOAD_16_8( CPU1, "psh_27.12e", 0x040000, 0x20000, 0x579f4fd3),
  LOAD_16_8( CPU1, "psh_31.12f", 0x040001, 0x20000, 0x2c9f70b5),
  LOAD_16_8( CPU1, "psh_24.9e", 0x080000, 0x20000, 0xfaa14841),
  LOAD_16_8( CPU1, "psh_28.9f", 0x080001, 0x20000, 0x5c5b1f20),
  LOAD_16_8( CPU1, "psh_25.10e", 0x0c0000, 0x20000, 0x724fdfda),
  LOAD_16_8( CPU1, "psh_29.10f", 0x0c0001, 0x20000, 0x779cf901),
  LOAD_SW16( CPU1, "ps_21.6f", 0x100000, 0x80000, 0x8affa5a9),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_punisherj[] = // clone of punisher
{
  LOAD_SW16( CPU1, "psj_23.8f", 0x000000, 0x80000, 0x6b2fda52),
  LOAD_SW16( CPU1, "psj_22.7f", 0x080000, 0x80000, 0xe01036bc),
  LOAD_SW16( CPU1, "psj_21.6f", 0x100000, 0x80000, 0x8affa5a9),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_punisherbz[] = // clone of punisher
{
  LOAD_SW16( CPU1, "23.096", 0x000000, 0x80000, 0xbfa45d23),
  LOAD_SW16( CPU1, "22.096", 0x080000, 0x80000, 0x092578a4),
  LOAD_SW16( CPU1, "21.096", 0x100000, 0x80000, 0xd21ccddb),
  LOAD_SW16( CPU1, "20.096", 0x180000, 0x80000, 0xf9f334ce),
  { "1.096", 0x80000, 0xad468e07, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "2.096", 0x80000, 0xb9fdb6b5, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "3.096", 0x80000, 0xbe0b1a78, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "4.096", 0x80000, 0xbba67a43, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "ps_gfx5.rom", 0x80000, 0xc54ea839, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "ps_gfx7.rom", 0x80000, 0x04c5acbd, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "ps_gfx6.rom", 0x80000, 0xa544f4cc, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "ps_gfx8.rom", 0x80000, 0x8f02f436, REGION_GFX1, 0x200006, LOAD_16_64 },
  LOAD( ROM2, "9.512", 0x00000, 0x10000, 0xb8367eb5),
  LOAD( SMP1, "18.010", 0x00000, 0x20000, 0x375c66e7),
  LOAD( SMP1, "19.010", 0x20000, 0x20000, 0xeb5ca884),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_slammast[] =
{
  LOAD_SW16( CPU1, "mbe_23e.8f", 0x000000, 0x80000, 0x5394057a),
  LOAD_16_8( CPU1, "mbe_24b.9e", 0x080000, 0x20000, 0x95d5e729),
  LOAD_16_8( CPU1, "mbe_28b.9f", 0x080001, 0x20000, 0xb1c7cbcb),
  LOAD_16_8( CPU1, "mbe_25b.10e", 0x0c0000, 0x20000, 0xa50d3fd4),
  LOAD_16_8( CPU1, "mbe_29b.10f", 0x0c0001, 0x20000, 0x08e32e56),
  LOAD_SW16( CPU1, "mbe_21a.6f", 0x100000, 0x80000, 0xd5007b05),
  LOAD_SW16( CPU1, "mbe_20a.5f", 0x180000, 0x80000, 0xaeb557b0),
  { "mb-1m.3a", 0x80000, 0x41468e06, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "mb-3m.5a", 0x80000, 0xf453aa9e, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "mb-2m.4a", 0x80000, 0x2ffbfea8, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "mb-4m.6a", 0x80000, 0x1eb9841d, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "mb-5m.7a", 0x80000, 0x506b9dc9, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "mb-7m.9a", 0x80000, 0xaff8c2fb, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "mb-6m.8a", 0x80000, 0xb76c70e9, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "mb-8m.10a", 0x80000, 0xe60c9556, REGION_GFX1, 0x200006, LOAD_16_64 },
  { "mb-10m.3c", 0x80000, 0x97976ff5, REGION_GFX1, 0x400000, LOAD_16_64 },
  { "mb-12m.5c", 0x80000, 0xb350a840, REGION_GFX1, 0x400002, LOAD_16_64 },
  { "mb-11m.4c", 0x80000, 0x8fb94743, REGION_GFX1, 0x400004, LOAD_16_64 },
  { "mb-13m.6c", 0x80000, 0xda810d5f, REGION_GFX1, 0x400006, LOAD_16_64 },
  LOAD( ROM2, "mb_qa.5k", 0x00000, 0x20000, 0xe21a03c4),
/* Ignored : 	ROM_COPY( "audiocpu", 0x00000, 0x00000, 0x8000 ) */
  LOAD( SMP1, "mb-q1.1k", 0x000000, 0x80000, 0x0630c3ce),
  LOAD( SMP1, "mb-q2.2k", 0x080000, 0x80000, 0x354f9c21),
  LOAD( SMP1, "mb-q3.3k", 0x100000, 0x80000, 0x7838487c),
  LOAD( SMP1, "mb-q4.4k", 0x180000, 0x80000, 0xab66e087),
  LOAD( SMP1, "mb-q5.1m", 0x200000, 0x80000, 0xc789fef2),
  LOAD( SMP1, "mb-q6.2m", 0x280000, 0x80000, 0xecb81b61),
  LOAD( SMP1, "mb-q7.3m", 0x300000, 0x80000, 0x041e49ba),
  LOAD( SMP1, "mb-q8.4m", 0x380000, 0x80000, 0x59fe702a),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_slammastu[] = // clone of slammast
{
  LOAD_SW16( CPU1, "mbu_23e.8f", 0x000000, 0x80000, 0x224f0062),
  LOAD_16_8( CPU1, "mbu_24b.9e", 0x080000, 0x20000, 0x95d5e729),
  LOAD_16_8( CPU1, "mbu_28b.9f", 0x080001, 0x20000, 0xb1c7cbcb),
  LOAD_16_8( CPU1, "mbu_25b.10e", 0x0c0000, 0x20000, 0xa50d3fd4),
  LOAD_16_8( CPU1, "mbu_29b.10f", 0x0c0001, 0x20000, 0x08e32e56),
  LOAD_SW16( CPU1, "mbu_21a.6f", 0x100000, 0x80000, 0xd5007b05),
  LOAD_SW16( CPU1, "mbu_20a.5f", 0x180000, 0x80000, 0xfc848af5),
/* Ignored : 	ROM_COPY( "audiocpu", 0x00000, 0x00000, 0x8000 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mbomberj[] = // clone of slammast
{
  LOAD_SW16( CPU1, "mbj_23e.8f", 0x000000, 0x80000, 0x0d06036a),
  LOAD_16_8( CPU1, "mbe_24b.9e", 0x080000, 0x20000, 0x95d5e729),
  LOAD_16_8( CPU1, "mbe_28b.9f", 0x080001, 0x20000, 0xb1c7cbcb),
  LOAD_16_8( CPU1, "mbe_25b.10e", 0x0c0000, 0x20000, 0xa50d3fd4),
  LOAD_16_8( CPU1, "mbe_29b.10f", 0x0c0001, 0x20000, 0x08e32e56),
  LOAD_SW16( CPU1, "mbj_21a.6f", 0x100000, 0x80000, 0xd5007b05),
  LOAD_SW16( CPU1, "mbj_20a.5f", 0x180000, 0x80000, 0xaeb557b0),
  { "mb_01.3a", 0x80000, 0xa53b1c81, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "mb_02.4a", 0x80000, 0x23fe10f6, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "mb_03.5a", 0x80000, 0xcb866c2f, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "mb_04.6a", 0x80000, 0xc9143e75, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "mb_05.7a", 0x80000, 0x506b9dc9, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "mb_06.8a", 0x80000, 0xaff8c2fb, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "mb_07.9a", 0x80000, 0xb76c70e9, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "mb_08.10a", 0x80000, 0xe60c9556, REGION_GFX1, 0x200006, LOAD_16_64 },
  { "mb_10.3c", 0x80000, 0x97976ff5, REGION_GFX1, 0x400000, LOAD_16_64 },
  { "mb_11.4c", 0x80000, 0xb350a840, REGION_GFX1, 0x400002, LOAD_16_64 },
  { "mb_12.5c", 0x80000, 0x8fb94743, REGION_GFX1, 0x400004, LOAD_16_64 },
  { "mb_13.6c", 0x80000, 0xda810d5f, REGION_GFX1, 0x400006, LOAD_16_64 },
/* Ignored : 	ROM_COPY( "audiocpu", 0x00000, 0x00000, 0x8000 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mbombrd[] =
{
  LOAD_16_8( CPU1, "mbde_26.11e", 0x000000, 0x20000, 0x72b7451c),
  LOAD_16_8( CPU1, "mbde_30.11f", 0x000001, 0x20000, 0xa036dc16),
  LOAD_16_8( CPU1, "mbde_27.12e", 0x040000, 0x20000, 0x4086f534),
  LOAD_16_8( CPU1, "mbde_31.12f", 0x040001, 0x20000, 0x085f47f0),
  LOAD_16_8( CPU1, "mbde_24.9e", 0x080000, 0x20000, 0xc20895a5),
  LOAD_16_8( CPU1, "mbde_28.9f", 0x080001, 0x20000, 0x2618d5e1),
  LOAD_16_8( CPU1, "mbde_25.10e", 0x0c0000, 0x20000, 0x9bdb6b11),
  LOAD_16_8( CPU1, "mbde_29.10f", 0x0c0001, 0x20000, 0x3f52d5e5),
  LOAD_SW16( CPU1, "mbde_21.6f", 0x100000, 0x80000, 0x690c026a),
  LOAD_SW16( CPU1, "mbde_20.5f", 0x180000, 0x80000, 0xb8b2139b),
  { "mb-1m.3a", 0x80000, 0x41468e06, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "mb-3m.5a", 0x80000, 0xf453aa9e, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "mb-2m.4a", 0x80000, 0x2ffbfea8, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "mb-4m.6a", 0x80000, 0x1eb9841d, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "mb-5m.7a", 0x80000, 0x506b9dc9, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "mb-7m.9a", 0x80000, 0xaff8c2fb, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "mb-6m.8a", 0x80000, 0xb76c70e9, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "mb-8m.10a", 0x80000, 0xe60c9556, REGION_GFX1, 0x200006, LOAD_16_64 },
  { "mb-10m.3c", 0x80000, 0x97976ff5, REGION_GFX1, 0x400000, LOAD_16_64 },
  { "mb-12m.5c", 0x80000, 0xb350a840, REGION_GFX1, 0x400002, LOAD_16_64 },
  { "mb-11m.4c", 0x80000, 0x8fb94743, REGION_GFX1, 0x400004, LOAD_16_64 },
  { "mb-13m.6c", 0x80000, 0xda810d5f, REGION_GFX1, 0x400006, LOAD_16_64 },
  LOAD( ROM2, "mb_q.5k", 0x00000, 0x20000, 0xd6fa76d1),
  LOAD( SMP1, "mb-q1.1k", 0x000000, 0x80000, 0x0630c3ce),
  LOAD( SMP1, "mb-q2.2k", 0x080000, 0x80000, 0x354f9c21),
  LOAD( SMP1, "mb-q3.3k", 0x100000, 0x80000, 0x7838487c),
  LOAD( SMP1, "mb-q4.4k", 0x180000, 0x80000, 0xab66e087),
  LOAD( SMP1, "mb-q5.1m", 0x200000, 0x80000, 0xc789fef2),
  LOAD( SMP1, "mb-q6.2m", 0x280000, 0x80000, 0xecb81b61),
  LOAD( SMP1, "mb-q7.3m", 0x300000, 0x80000, 0x041e49ba),
  LOAD( SMP1, "mb-q8.4m", 0x380000, 0x80000, 0x59fe702a),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mbombrdj[] = // clone of mbombrd
{
  LOAD_16_8( CPU1, "mbdj_26.11e", 0x000000, 0x20000, 0x72b7451c),
  LOAD_16_8( CPU1, "mbdj_30.11f", 0x000001, 0x20000, 0xbeff31cf),
  LOAD_16_8( CPU1, "mbdj_27.12e", 0x040000, 0x20000, 0x4086f534),
  LOAD_16_8( CPU1, "mbdj_31.12f", 0x040001, 0x20000, 0x085f47f0),
  LOAD_16_8( CPU1, "mbdj_24.9e", 0x080000, 0x20000, 0xc20895a5),
  LOAD_16_8( CPU1, "mbdj_28.9f", 0x080001, 0x20000, 0x2618d5e1),
  LOAD_16_8( CPU1, "mbdj_25.10e", 0x0c0000, 0x20000, 0x9bdb6b11),
  LOAD_16_8( CPU1, "mbdj_29.10f", 0x0c0001, 0x20000, 0x3f52d5e5),
  LOAD_SW16( CPU1, "mbdj_21.6f", 0x100000, 0x80000, 0x690c026a),
  LOAD_SW16( CPU1, "mbdj_20.5f", 0x180000, 0x80000, 0xb8b2139b),
  { "mb_01.3a", 0x80000, 0xa53b1c81, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "mb_02.4a", 0x80000, 0x23fe10f6, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "mb_03.5a", 0x80000, 0xcb866c2f, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "mb_04.6a", 0x80000, 0xc9143e75, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "mb_05.7a", 0x80000, 0x506b9dc9, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "mb_06.8a", 0x80000, 0xaff8c2fb, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "mb_07.9a", 0x80000, 0xb76c70e9, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "mb_08.10a", 0x80000, 0xe60c9556, REGION_GFX1, 0x200006, LOAD_16_64 },
  { "mb_10.3c", 0x80000, 0x97976ff5, REGION_GFX1, 0x400000, LOAD_16_64 },
  { "mb_11.4c", 0x80000, 0xb350a840, REGION_GFX1, 0x400002, LOAD_16_64 },
  { "mb_12.5c", 0x80000, 0x8fb94743, REGION_GFX1, 0x400004, LOAD_16_64 },
  { "mb_13.6c", 0x80000, 0xda810d5f, REGION_GFX1, 0x400006, LOAD_16_64 },
  LOAD( ROM2, "mb_qa.5k", 0x00000, 0x20000, 0xe21a03c4),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_pnickj[] =
{
  LOAD_16_8( CPU1, "pnij_36.12f", 0x00000, 0x20000, 0x2d4ffb2b),
  LOAD_16_8( CPU1, "pnij_42.12h", 0x00001, 0x20000, 0xc085dfaf),
  { "pnij_09.4b", 0x20000, 0x48177b0a, REGION_GFX1, 0x000000, LOAD_8_64 },
  { "pnij_01.4a", 0x20000, 0x01a0f311, REGION_GFX1, 0x000001, LOAD_8_64 },
  { "pnij_13.9b", 0x20000, 0x406451b0, REGION_GFX1, 0x000002, LOAD_8_64 },
  { "pnij_05.9a", 0x20000, 0x8c515dc0, REGION_GFX1, 0x000003, LOAD_8_64 },
  { "pnij_26.5e", 0x20000, 0xe2af981e, REGION_GFX1, 0x000004, LOAD_8_64 },
  { "pnij_18.5c", 0x20000, 0xf17a0e56, REGION_GFX1, 0x000005, LOAD_8_64 },
  { "pnij_38.8h", 0x20000, 0xeb75bd8c, REGION_GFX1, 0x000006, LOAD_8_64 },
  { "pnij_32.8f", 0x20000, 0x84560bef, REGION_GFX1, 0x000007, LOAD_8_64 },
  { "pnij_10.5b", 0x20000, 0xc2acc171, REGION_GFX1, 0x100000, LOAD_8_64 },
  { "pnij_02.5a", 0x20000, 0x0e21fc33, REGION_GFX1, 0x100001, LOAD_8_64 },
  { "pnij_14.10b", 0x20000, 0x7fe59b19, REGION_GFX1, 0x100002, LOAD_8_64 },
  { "pnij_06.10a", 0x20000, 0x79f4bfe3, REGION_GFX1, 0x100003, LOAD_8_64 },
  { "pnij_27.7e", 0x20000, 0x83d5cb0e, REGION_GFX1, 0x100004, LOAD_8_64 },
  { "pnij_19.7c", 0x20000, 0xaf08b230, REGION_GFX1, 0x100005, LOAD_8_64 },
  { "pnij_39.9h", 0x20000, 0x70fbe579, REGION_GFX1, 0x100006, LOAD_8_64 },
  { "pnij_33.9f", 0x20000, 0x3ed2c680, REGION_GFX1, 0x100007, LOAD_8_64 },
  LOAD( ROM2, "pnij_17.13b", 0x00000, 0x10000, 0xe86f787a),
  LOAD( SMP1, "pnij_24.12c", 0x00000, 0x20000, 0x5092257d),
  LOAD( SMP1, "pnij_25.13c", 0x20000, 0x20000, 0x22109aaa),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_qtono2j[] =
{
  LOAD_16_8( CPU1, "tn2j_30.11e", 0x00000, 0x20000, 0x9226eb5e),
  LOAD_16_8( CPU1, "tn2j_37.11f", 0x00001, 0x20000, 0xd1d30da1),
  LOAD_16_8( CPU1, "tn2j_31.12e", 0x40000, 0x20000, 0x015e6a8a),
  LOAD_16_8( CPU1, "tn2j_38.12f", 0x40001, 0x20000, 0x1f139bcc),
  LOAD_16_8( CPU1, "tn2j_28.9e", 0x80000, 0x20000, 0x86d27f71),
  LOAD_16_8( CPU1, "tn2j_35.9f", 0x80001, 0x20000, 0x7a1ab87d),
  LOAD_16_8( CPU1, "tn2j_29.10e", 0xc0000, 0x20000, 0x9c384e99),
  LOAD_16_8( CPU1, "tn2j_36.10f", 0xc0001, 0x20000, 0x4c4b2a0a),
  { "tn2-02m.4a", 0x80000, 0xf2016a34, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "tn2-04m.6a", 0x80000, 0x094e0fb1, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "tn2-01m.3a", 0x80000, 0xcb950cf9, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "tn2-03m.5a", 0x80000, 0x18a5bf59, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "tn2-11m.4c", 0x80000, 0xd0edd30b, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "tn2-13m.6c", 0x80000, 0x426621c3, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "tn2-10m.3c", 0x80000, 0xa34ece70, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "tn2-12m.5c", 0x80000, 0xe04ff2f4, REGION_GFX1, 0x200006, LOAD_16_64 },
  LOAD( ROM2, "tn2j_09.12a", 0x00000, 0x10000, 0xe464b969),
  LOAD( SMP1, "tn2j_18.11c", 0x00000, 0x20000, 0xa40bf9a7),
  LOAD( SMP1, "tn2j_19.12c", 0x20000, 0x20000, 0x5b3b931e),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_pang3[] =
{
  LOAD_SW16( CPU1, "pa3e_17a.11l", 0x00000, 0x80000, 0xa213fa80),
  LOAD_SW16( CPU1, "pa3e_16a.10l", 0x80000, 0x80000, 0x7169ea67),
  { "pa3-01m.2c", 0x100000, 0x068a152c, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "pa3-01m.2c", 0x100000 , 0x068a152c, REGION_GFX1, 0x000004, LOAD_CONTINUE },
  { "pa3-07m.2f", 0x100000, 0x3a4a619d, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "pa3-07m.2f", 0x100000 , 0x3a4a619d, REGION_GFX1, 0x000006, LOAD_CONTINUE },
  LOAD( ROM2, "pa3_11.11f", 0x00000, 0x08000, 0xcb1423a2),
  { "pa3_11.11f", 0x18000, 0xcb1423a2, REGION_ROM2, 0x00000, LOAD_IGNORE },
  LOAD( SMP1, "pa3_05.10d", 0x00000, 0x20000, 0x73a10d5d),
  LOAD( SMP1, "pa3_06.11d", 0x20000, 0x20000, 0xaffa4f82),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_pang3r1[] = // clone of pang3
{
  LOAD_SW16( CPU1, "pa3e_17.11l", 0x00000, 0x80000, 0xd7041d32),
  LOAD_SW16( CPU1, "pa3e_16.10l", 0x80000, 0x80000, 0x1be9a483),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_pang3j[] = // clone of pang3
{
  LOAD_SW16( CPU1, "pa3j_17.11l", 0x00000, 0x80000, 0x21f6e51f),
  LOAD_SW16( CPU1, "pa3j_16.10l", 0x80000, 0x80000, 0xca1d7897),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_pang3b[] = // clone of pang3
{
  LOAD_SW16( CPU1, "pa3w_17.11l", 0x00000, 0x80000, 0x12138234),
  LOAD_SW16( CPU1, "pa3w_16.10l", 0x80000, 0x80000, 0xd1ba585c),
  LOAD( ROM2, "pa3_11(__pang3b).11f", 0x00000, 0x08000, 0x90a08c46),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_megaman[] =
{
  LOAD_SW16( CPU1, "rcmu_23b.8f", 0x000000, 0x80000, 0x1cd33c7a),
  LOAD_SW16( CPU1, "rcmu_22b.7f", 0x080000, 0x80000, 0x708268c4),
  LOAD_SW16( CPU1, "rcmu_21a.6f", 0x100000, 0x80000, 0x4376ea95),
  { "rcm_01.3a", 0x80000, 0x6ecdf13f, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "rcm_02.4a", 0x80000, 0x944d4f0f, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "rcm_03.5a", 0x80000, 0x36f3073c, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "rcm_04.6a", 0x80000, 0x54e622ff, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "rcm_05.7a", 0x80000, 0x5dd131fd, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "rcm_06.8a", 0x80000, 0xf0faf813, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "rcm_07.9a", 0x80000, 0x826de013, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "rcm_08.10a", 0x80000, 0xfbff64cf, REGION_GFX1, 0x200006, LOAD_16_64 },
  { "rcm_10.3c", 0x80000, 0x4dc8ada9, REGION_GFX1, 0x400000, LOAD_16_64 },
  { "rcm_11.4c", 0x80000, 0xf2b9ee06, REGION_GFX1, 0x400002, LOAD_16_64 },
  { "rcm_12.5c", 0x80000, 0xfed5f203, REGION_GFX1, 0x400004, LOAD_16_64 },
  { "rcm_13.6c", 0x80000, 0x5069d4a9, REGION_GFX1, 0x400006, LOAD_16_64 },
  { "rcm_14.7c", 0x80000, 0x303be3bd, REGION_GFX1, 0x600000, LOAD_16_64 },
  { "rcm_15.8c", 0x80000, 0x4f2d372f, REGION_GFX1, 0x600002, LOAD_16_64 },
  { "rcm_16.9c", 0x80000, 0x93d97fde, REGION_GFX1, 0x600004, LOAD_16_64 },
  { "rcm_17.10c", 0x80000, 0x92371042, REGION_GFX1, 0x600006, LOAD_16_64 },
  LOAD( ROM2, "rcm_09.11a", 0x00000, 0x10000, 0x22ac8f5f),
  LOAD( SMP1, "rcm_18.11c", 0x00000, 0x20000, 0x80f1f8aa),
  LOAD( SMP1, "rcm_19.12c", 0x20000, 0x20000, 0xf257dbe1),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_megamana[] = // clone of megaman
{
  LOAD_SW16( CPU1, "rcma_23b.8f", 0x000000, 0x80000, 0x61e4a397),
  LOAD_SW16( CPU1, "rcma_22b.7f", 0x080000, 0x80000, 0x708268c4),
  LOAD_SW16( CPU1, "rcma_21a.6f", 0x100000, 0x80000, 0x4376ea95),
  LOAD( ROM2, "rcm_09.12a", 0x00000, 0x20000, 0x9632d6ef),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_rockmanj[] = // clone of megaman
{
  LOAD_SW16( CPU1, "rcm_23a.8f", 0x000000, 0x80000, 0xefd96cb2),
  LOAD_SW16( CPU1, "rcm_22a.7f", 0x080000, 0x80000, 0x8729a689),
  LOAD_SW16( CPU1, "rcm_21a.6f", 0x100000, 0x80000, 0x517ccde2),
  LOAD( ROM2, "rcm_09.12a", 0x00000, 0x20000, 0x9632d6ef),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ganbare[] =
{
  LOAD_SW16( CPU1, "mrnj_23d.8f", 0x00000, 0x80000, 0xf929be72),
  { "mrnj_01.3a", 0x80000, 0x3f878020, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "mrnj_02.4a", 0x80000, 0x3e5624d8, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "mrnj_03.5a", 0x80000, 0xd1e61f96, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "mrnj_04.6a", 0x80000, 0xd241971b, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "mrnj_05.7a", 0x80000, 0xc0a14562, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "mrnj_06.8a", 0x80000, 0xe6a71dfc, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "mrnj_07.9a", 0x80000, 0x99afb6c7, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "mrnj_08.10a", 0x80000, 0x52882c20, REGION_GFX1, 0x200006, LOAD_16_64 },
  LOAD( ROM2, "mrnj_09.12a", 0x00000, 0x10000, 0x62470d72),
  LOAD( SMP1, "mrnj_18.11c", 0x00000, 0x20000, 0x08e13940),
  LOAD( SMP1, "mrnj_19.12c", 0x20000, 0x20000, 0x5fa59927),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_wofch[] =
{
  LOAD_SW16( CPU1, "tk2(ch)_23.8f", 0x000000, 0x80000, 0x4e0b8dee),
  LOAD_SW16( CPU1, "tk2(ch)_22.7f", 0x080000, 0x80000, 0xd0937a8d),
  { "tk2-1m.3a", 0x80000, 0x0d9cb9bf, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "tk2-3m.5a", 0x80000, 0x45227027, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "tk2-2m.4a", 0x80000, 0xc5ca2460, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "tk2-4m.6a", 0x80000, 0xe349551c, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "tk2(ch)_05.7a", 0x80000, 0xe4a44d53, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "tk2(ch)_06.8a", 0x80000, 0x58066ba8, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "tk2(ch)_07.9a", 0x80000, 0xcc9006c9, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "tk2(ch)_08.10a", 0x80000, 0xd4a19a02, REGION_GFX1, 0x200006, LOAD_16_64 },
  LOAD( ROM2, "tk2_qa.5k", 0x00000, 0x20000, 0xc9183a0d),
  LOAD( SMP1, "tk2-q1.1k", 0x000000, 0x80000, 0x611268cf),
  LOAD( SMP1, "tk2-q2.2k", 0x080000, 0x80000, 0x20f55ca9),
  LOAD( SMP1, "tk2-q3.3k", 0x100000, 0x80000, 0xbfcf6f52),
  LOAD( SMP1, "tk2-q4.4k", 0x180000, 0x80000, 0x36642e88),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfach[] =
{
  LOAD_SW16( CPU1, "sfach23", 0x000000, 0x80000, 0x02a1a853),
  LOAD_SW16( CPU1, "sfza22", 0x080000, 0x80000, 0x8d9b2480),
  LOAD_SW16( CPU1, "sfzch21", 0x100000, 0x80000, 0x5435225d),
  LOAD_SW16( CPU1, "sfza20", 0x180000, 0x80000, 0x806e8f38),
  { "sfz01", 0x80000, 0x0dd53e62, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "sfz02", 0x80000, 0x94c31e3f, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "sfz03", 0x80000, 0x9584ac85, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "sfz04", 0x80000, 0xb983624c, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "sfz05", 0x80000, 0x2b47b645, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "sfz06", 0x80000, 0x74fd9fb1, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "sfz07", 0x80000, 0xbb2c734d, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "sfz08", 0x80000, 0x454f7868, REGION_GFX1, 0x200006, LOAD_16_64 },
  { "sfz10", 0x80000, 0x2a7d675e, REGION_GFX1, 0x400000, LOAD_16_64 },
  { "sfz11", 0x80000, 0xe35546c8, REGION_GFX1, 0x400002, LOAD_16_64 },
  { "sfz12", 0x80000, 0xf122693a, REGION_GFX1, 0x400004, LOAD_16_64 },
  { "sfz13", 0x80000, 0x7cf942c8, REGION_GFX1, 0x400006, LOAD_16_64 },
  { "sfz14", 0x80000, 0x09038c81, REGION_GFX1, 0x600000, LOAD_16_64 },
  { "sfz15", 0x80000, 0x1aa17391, REGION_GFX1, 0x600002, LOAD_16_64 },
  { "sfz16", 0x80000, 0x19a5abd6, REGION_GFX1, 0x600004, LOAD_16_64 },
  { "sfz17", 0x80000, 0x248b3b73, REGION_GFX1, 0x600006, LOAD_16_64 },
/* Ignored : 	ROM_COPY( "gfx", 0x000000, 0x000000, 0x8000 )    stars  */
  LOAD( ROM2, "sfz09", 0x00000, 0x10000, 0xc772628b),
  LOAD( SMP1, "sfz18", 0x00000, 0x20000, 0x61022b2d),
  LOAD( SMP1, "sfz19", 0x20000, 0x20000, 0x3b5886d5),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfzch[] =
{
  LOAD_SW16( CPU1, "sfzch23", 0x000000, 0x80000, 0x1140743f),
  LOAD_SW16( CPU1, "sfza22", 0x080000, 0x80000, 0x8d9b2480),
  LOAD_SW16( CPU1, "sfzch21", 0x100000, 0x80000, 0x5435225d),
  LOAD_SW16( CPU1, "sfza20", 0x180000, 0x80000, 0x806e8f38),
  { "sfz_01.3a", 0x80000, 0x0dd53e62, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "sfz_02.4a", 0x80000, 0x94c31e3f, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "sfz_03.5a", 0x80000, 0x9584ac85, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "sfz_04.6a", 0x80000, 0xb983624c, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "sfz_05.7a", 0x80000, 0x2b47b645, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "sfz_06.8a", 0x80000, 0x74fd9fb1, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "sfz_07.9a", 0x80000, 0xbb2c734d, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "sfz_08.10a", 0x80000, 0x454f7868, REGION_GFX1, 0x200006, LOAD_16_64 },
  { "sfz_10.3c", 0x80000, 0x2a7d675e, REGION_GFX1, 0x400000, LOAD_16_64 },
  { "sfz_11.4c", 0x80000, 0xe35546c8, REGION_GFX1, 0x400002, LOAD_16_64 },
  { "sfz_12.5c", 0x80000, 0xf122693a, REGION_GFX1, 0x400004, LOAD_16_64 },
  { "sfz_13.6c", 0x80000, 0x7cf942c8, REGION_GFX1, 0x400006, LOAD_16_64 },
  { "sfz_14.7c", 0x80000, 0x09038c81, REGION_GFX1, 0x600000, LOAD_16_64 },
  { "sfz_15.8c", 0x80000, 0x1aa17391, REGION_GFX1, 0x600002, LOAD_16_64 },
  { "sfz_16.9c", 0x80000, 0x19a5abd6, REGION_GFX1, 0x600004, LOAD_16_64 },
  { "sfz_17.10c", 0x80000, 0x248b3b73, REGION_GFX1, 0x600006, LOAD_16_64 },
/* Ignored : 	ROM_COPY( "gfx", 0x000000, 0x000000, 0x8000 ) */
  LOAD( ROM2, "sfz_09.12a", 0x00000, 0x10000, 0xc772628b),
  LOAD( SMP1, "sfz_18.11c", 0x00000, 0x20000, 0x61022b2d),
  LOAD( SMP1, "sfz_19.12c", 0x20000, 0x20000, 0x3b5886d5),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfzbch[] =
{
  LOAD_SW16( CPU1, "sfzbch23", 0x000000, 0x80000, 0x53699f68),
  LOAD_SW16( CPU1, "sfza22", 0x080000, 0x80000, 0x8d9b2480),
  LOAD_SW16( CPU1, "sfzch21", 0x100000, 0x80000, 0x5435225d),
  LOAD_SW16( CPU1, "sfza20", 0x180000, 0x80000, 0x806e8f38),
  { "sfz_01.3a", 0x80000, 0x0dd53e62, REGION_GFX1, 0x000000, LOAD_16_64 },
  { "sfz_02.4a", 0x80000, 0x94c31e3f, REGION_GFX1, 0x000002, LOAD_16_64 },
  { "sfz_03.5a", 0x80000, 0x9584ac85, REGION_GFX1, 0x000004, LOAD_16_64 },
  { "sfz_04.6a", 0x80000, 0xb983624c, REGION_GFX1, 0x000006, LOAD_16_64 },
  { "sfz_05.7a", 0x80000, 0x2b47b645, REGION_GFX1, 0x200000, LOAD_16_64 },
  { "sfz_06.8a", 0x80000, 0x74fd9fb1, REGION_GFX1, 0x200002, LOAD_16_64 },
  { "sfz_07.9a", 0x80000, 0xbb2c734d, REGION_GFX1, 0x200004, LOAD_16_64 },
  { "sfz_08.10a", 0x80000, 0x454f7868, REGION_GFX1, 0x200006, LOAD_16_64 },
  { "sfz_10.3c", 0x80000, 0x2a7d675e, REGION_GFX1, 0x400000, LOAD_16_64 },
  { "sfz_11.4c", 0x80000, 0xe35546c8, REGION_GFX1, 0x400002, LOAD_16_64 },
  { "sfz_12.5c", 0x80000, 0xf122693a, REGION_GFX1, 0x400004, LOAD_16_64 },
  { "sfz_13.6c", 0x80000, 0x7cf942c8, REGION_GFX1, 0x400006, LOAD_16_64 },
  { "sfz_14.7c", 0x80000, 0x09038c81, REGION_GFX1, 0x600000, LOAD_16_64 },
  { "sfz_15.8c", 0x80000, 0x1aa17391, REGION_GFX1, 0x600002, LOAD_16_64 },
  { "sfz_16.9c", 0x80000, 0x19a5abd6, REGION_GFX1, 0x600004, LOAD_16_64 },
  { "sfz_17.10c", 0x80000, 0x248b3b73, REGION_GFX1, 0x600006, LOAD_16_64 },
/* Ignored : 	ROM_COPY( "gfx", 0x000000, 0x000000, 0x8000 )    stars  */
  LOAD( ROM2, "sfz_09.12a", 0x00000, 0x10000, 0xc772628b),
  LOAD( SMP1, "sfz_18.11c", 0x00000, 0x20000, 0x61022b2d),
  LOAD( SMP1, "sfz_19.12c", 0x20000, 0x20000, 0x3b5886d5),
  { NULL, 0, 0, 0, 0, 0 }
};

// dsw (regenerated 6/1/2004)

#define CPS1_COINAGE_1 \
  { MSG_COIN1, 0x07, 8 },\
  { MSG_4COIN_1PLAY, 0x00, 0x00 },\
  { MSG_3COIN_1PLAY, 0x01, 0x00 },\
  { MSG_2COIN_1PLAY, 0x02, 0x00 },\
  { MSG_1COIN_1PLAY, 0x07, 0x00 },\
  { MSG_1COIN_1PLAY, 0x06, 0x00 },\
  { MSG_1COIN_3PLAY, 0x05, 0x00 },\
  { MSG_1COIN_4PLAY, 0x04, 0x00 },\
  { MSG_1COIN_6PLAY, 0x03, 0x00 },\
  { MSG_COIN2, 0x38, 8 },\
  { MSG_4COIN_1PLAY, 0x00, 0x00 },\
  { MSG_3COIN_1PLAY, 0x08, 0x00 },\
  { MSG_2COIN_1PLAY, 0x10, 0x00 },\
  { MSG_1COIN_1PLAY, 0x38, 0x00 },\
  { MSG_1COIN_1PLAY, 0x30, 0x00 },\
  { MSG_1COIN_3PLAY, 0x28, 0x00 },\
  { MSG_1COIN_4PLAY, 0x20, 0x00 },\
  { MSG_1COIN_6PLAY, 0x18, 0x00 },\

#define CPS1_COINAGE_2 \
  { MSG_COINAGE, 0x07, 8 },\
  { MSG_4COIN_1PLAY, 0x00, 0x00 },\
  { MSG_3COIN_1PLAY, 0x01, 0x00 },\
  { MSG_2COIN_1PLAY, 0x02, 0x00 },\
  { MSG_1COIN_1PLAY, 0x07, 0x00 },\
  { MSG_1COIN_1PLAY, 0x06, 0x00 },\
  { MSG_1COIN_3PLAY, 0x05, 0x00 },\
  { MSG_1COIN_4PLAY, 0x04, 0x00 },\
  { MSG_1COIN_6PLAY, 0x03, 0x00 },\

#define CPS1_COINAGE_3 \
  { MSG_COIN1, 0x07, 8 },\
  { MSG_4COIN_1PLAY, 0x01, 0x00 },\
  { MSG_3COIN_1PLAY, 0x02, 0x00 },\
  { MSG_2COIN_1PLAY, 0x03, 0x00 },\
  { "2 Coins/1 Credit (1 to continue)" , 0x00, 0x00 },\
  { MSG_1COIN_1PLAY, 0x07, 0x00 },\
  { MSG_1COIN_1PLAY, 0x06, 0x00 },\
  { MSG_1COIN_3PLAY, 0x05, 0x00 },\
  { MSG_1COIN_4PLAY, 0x04, 0x00 },\
  { MSG_COIN2, 0x38, 8 },\
  { MSG_4COIN_1PLAY, 0x08, 0x00 },\
  { MSG_3COIN_1PLAY, 0x10, 0x00 },\
  { MSG_2COIN_1PLAY, 0x18, 0x00 },\
  { "2 Coins/1 Credit (1 to continue)" , 0x00, 0x00 },\
  { MSG_1COIN_1PLAY, 0x38, 0x00 },\
  { MSG_1COIN_1PLAY, 0x30, 0x00 },\
  { MSG_1COIN_3PLAY, 0x28, 0x00 },\
  { MSG_1COIN_4PLAY, 0x20, 0x00 },\

#define CPS1_DIFFICULTY_1 \
  { MSG_DIFFICULTY, 0x07, 8 },\
  { "1 (Easiest)" , 0x07, 0x00 },\
  { "2" , 0x06, 0x00 },\
  { "3" , 0x05, 0x00 },\
  { "4 (Normal)" , 0x04, 0x00 },\
  { "5" , 0x03, 0x00 },\
  { "6" , 0x02, 0x00 },\
  { "7" , 0x01, 0x00 },\
  { "8 (Hardest)" , 0x00, 0x00 },\

#define CPS1_DIFFICULTY_2 \
  { MSG_DIFFICULTY, 0x07, 8 },\
  { "1 (Easiest)" , 0x04, 0x00 },\
  { "2" , 0x05, 0x00 },\
  { "3" , 0x06, 0x00 },\
  { "4 (Normal)" , 0x07, 0x00 },\
  { "5" , 0x03, 0x00 },\
  { "6" , 0x02, 0x00 },\
  { "7" , 0x01, 0x00 },\
  { "8 (Hardest)" , 0x00, 0x00 },\

static struct DSW_DATA dsw_data_forgottn_1[] =
{
	CPS1_COINAGE_1
  DSW_DEMO_SOUND( 0x40, 0x00),
  DSW_SCREEN( 0x80, 0x00),
  { NULL, 0}
};

static struct DSW_DATA dsw_data_forgottn_2[] =
{
	CPS1_DIFFICULTY_1
  { MSG_UNKNOWN, 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  DSW_UNUSED( 0x00, 0x10),
  DSW_UNUSED( 0x00, 0x20),
  DSW_SERVICE( 0, 0x40),
  { _("Freeze"), 0x80, 2 },
  { MSG_OFF, 0x80},
  { MSG_ON, 0x00},
  { NULL, 0}
};

static struct DSW_DATA dsw_data_forgottn_3[] =
{
  DSW_UNUSED( 0x00, 0x01),
  DSW_UNUSED( 0x00, 0x02),
  DSW_UNUSED( 0x00, 0x04),
  DSW_UNUSED( 0x00, 0x08),
  DSW_UNUSED( 0x00, 0x10),
  DSW_UNUSED( 0x00, 0x20),
  { MSG_UNKNOWN, 0x40, 2 },
  { MSG_OFF, 0x40},
  { MSG_ON, 0x00},
  DSW_UNUSED( 0x00, 0x80),
  { NULL, 0}
};

static struct DSW_INFO dsw_forgottn[] =
{
  { 0x1a, 0xff, dsw_data_forgottn_1 },
  { 0x1c, 0xff, dsw_data_forgottn_2 },
  { 0x1e, 0xff, dsw_data_forgottn_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_ganbare_2[] =
{
// _("SW(A):1")
// _("SW(A):2")
  { _("Medal Setup"), 0x01, 2 },
  { _("1 Medal 1 Credit"), 0x01, 0x00 },
  { _("Don't use"), 0x00, 0x00 },
// _("SW(A):3,4,5")
  { _("Coin Setup"), 0x02, 2 },
  { _("100 Yen"), 0x02, 0x00 },
  { _("10 Yen"), 0x00, 0x00 },
// _("SW(A):6,7")
  { _("Change Setup"), 0x1c, 8 },
  { "12", 0x04, 0x00 },
  { "11", 0x00, 0x00 },
  { "10", 0x1c, 0x00 },
  { "8", 0x18, 0x00 },
  { "7", 0x14, 0x00 },
  { "6", 0x10, 0x00 },
  { "5", 0x0c, 0x00 },
  { _("No change"), 0x08, 0x00 },
// _("SW(A):8")
  { _("10 Yen Setup"), 0x60, 4 },
  { MSG_1COIN_1PLAY, 0x60, 0x00 },
  { MSG_2COIN_1PLAY, 0x40, 0x00 },
  { MSG_3COIN_1PLAY, 0x20, 0x00 },
  { _("Don't use"), 0x00, 0x00 },
  { _("Payout Setup"), 0x80, 2 },
  { _("Credit Mode"), 0x80, 0x00 },
  { _("Payout Mode"), 0x00, 0x00 },
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_data_ganbare_3[] =
{
// _("SW(B):1,2,3")
  { _("Payout Rate Setup"), 0x07, 8 },
  { "90%", 0x01, 0x00 },
  { "85%", 0x00, 0x00 },
  { "80%", 0x07, 0x00 },
  { "75%", 0x06, 0x00 },
  { "70%", 0x05, 0x00 },
  { "65%", 0x04, 0x00 },
  { "60%", 0x03, 0x00 },
  { "55%", 0x02, 0x00 },
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_data_ganbare_4[] =
{
// _("SW(C):1,2")
// _("SW(C):7")
  DSW_DEMO_SOUND( 0x03, 0x00 ),
// _("SW(C):8")
  { _("Clear RAM"), 0x40, 2 },
  { MSG_NO, 0x40, 0x00 },
  { MSG_YES, 0x00, 0x00 },
  { _("Test Mode Display"), 0x80, 2 },
  { MSG_OFF, 0x80, 0x00 },
  { MSG_ON, 0x00, 0x00 },
  { NULL, 0, 0}
};

static struct DSW_INFO dsw_ganbare[] =
{
  { 0x1a, 0xff, dsw_data_ganbare_2 },
  { 0x1c, 0x07, dsw_data_ganbare_3 },
  { 0x1e, 0xc3, dsw_data_ganbare_4 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_ghouls_1[] =
{
	CPS1_COINAGE_1
  { MSG_CABINET, 0xc0, 2 },
  { _("Upright 2 Players"), 0x80 },
  { _("Upright 1 Player"), 0xc0 },
  { NULL, 0}
};

static struct DSW_DATA dsw_data_ghouls_2[] =
{
	CPS1_DIFFICULTY_2
  DSW_UNUSED( 0x00, 0x08),
  { MSG_EXTRA_LIFE, 0x30, 4 },
  { _("10K 30K and every 30K") , 0x20},
  { _("20K 50K and every 70K") , 0x10},
  { _("30K 60K and every 70K") , 0x30},
  { _("40K 70K and every 80K") , 0x00},
  DSW_UNUSED( 0x00, 0x40),
  DSW_UNUSED( 0x00, 0x80),
  { NULL, 0}
};

static struct DSW_DATA dsw_data_ghouls_3[] =
{
  { MSG_LIVES, 0x03, 4 },
  { "3" , 0x03},
  { "4" , 0x02},
  { "5" , 0x01},
  { "6" , 0x00},
  DSW_UNUSED( 0x00, 0x04),
  DSW_UNUSED( 0x00, 0x08),
  DSW_SCREEN( 0x10, 0x00),
  DSW_UNUSED( 0x00, 0x20),
  { _("Allow Continue"), 0x40, 2 },
  { MSG_NO, 0x00},
  { MSG_YES, 0x40},
  { _("Game Mode"),    0x80, 2 },
  { _("Game") , 0x80},
  { _("Test") , 0x00},
  { NULL, 0}
};

static struct DSW_INFO dsw_ghouls[] =
{
  { 0x1a, 0xff, dsw_data_ghouls_1 },
  { 0x1c, 0xff, dsw_data_ghouls_2 },
  { 0x1e, 0xff, dsw_data_ghouls_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_ghoulsu_2[] =
{
	CPS1_DIFFICULTY_2
  DSW_UNUSED( 0x00, 0x08),
  { MSG_EXTRA_LIFE, 0x30, 4 },
  { _("10K 30K and every 30K") , 0x20},
  { _("20K 50K and every 70K") , 0x10},
  { _("30K 60K and every 70K") , 0x30},
  { _("40K 70K and every 80K") , 0x00},
  DSW_UNUSED( 0x00, 0x40),
  { _("Freeze"), 0x80, 2 },
  { MSG_OFF, 0x80},
  { MSG_ON, 0x00},
  { NULL, 0}
};

static struct DSW_DATA dsw_data_ghoulsu_3[] =
{
  { MSG_LIVES, 0x03, 4 },
  { "2" , 0x00},
  { "3" , 0x03},
  { "4" , 0x02},
  { "5" , 0x01},
  DSW_UNUSED( 0x00, 0x04),
  DSW_UNUSED( 0x00, 0x08),
  DSW_SCREEN( 0x10, 0x00),
  DSW_UNUSED( 0x00, 0x20),
  { _("Allow Continue"), 0x40, 2 },
  { MSG_NO, 0x00},
  { MSG_YES, 0x40},
  { _("Game Mode"),    0x80, 2 },
  { _("Game") , 0x80},
  { _("Test") , 0x00},
  { NULL, 0}
};

static struct DSW_INFO dsw_ghoulsu[] =
{
  { 0x1a, 0xff, dsw_data_ghouls_1 },
  { 0x1c, 0xff, dsw_data_ghoulsu_2 },
  { 0x1e, 0xff, dsw_data_ghoulsu_3 },
  { 0, 0, NULL }
};

static struct DSW_INFO dsw_daimakai[] =
{
  { 0x1a, 0xff, dsw_data_ghouls_1 },
  { 0x1c, 0xff, dsw_data_ghoulsu_2 },
  { 0x1e, 0xff, dsw_data_ghouls_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_strider_2[] =
{
	CPS1_DIFFICULTY_2
  DSW_UNUSED( 0x00, 0x08),
  { MSG_EXTRA_LIFE, 0x30, 4 },
  { _("20K 40K then every 60K") , 0x30},
  { _("30K 50K then every 70K") , 0x20},
  { _("20K & 60K only") , 0x10},
  { _("30K & 60K only") , 0x00},
  { _("Internal Diff. on Life Loss"), 0xc0, 1 },
  { "-3" , 0xc0},
  { NULL, 0}
};

static struct DSW_DATA dsw_data_strider_3[] =
{
  { MSG_LIVES, 0x03, 4 },
  { "2" , 0x00},
  { "3" , 0x03},
  { "4" , 0x02},
  { "5" , 0x01},
  { _("Freeze"), 0x04, 2 },
  { MSG_OFF, 0x04},
  { MSG_ON, 0x00},
  { MSG_FREE_PLAY, 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  DSW_SCREEN( 0x10, 0x00),
  DSW_DEMO_SOUND( 0x20, 0x00),
  { _("Allow Continue"), 0x40, 2 },
  { MSG_NO, 0x00},
  { MSG_YES, 0x40},
  { _("Game Mode"),    0x80, 2 },
  { _("Game") , 0x80},
  { _("Test") , 0x00},
  { NULL, 0}
};

static struct DSW_INFO dsw_strider[] =
{
  { 0x1a, 0xff, dsw_data_ghouls_1 },
  { 0x1c, 0x3f, dsw_data_strider_2 },
  { 0x1e, 0xff, dsw_data_strider_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_stridrua_2[] =
{
	CPS1_DIFFICULTY_2
  { _("2 Coins to Start,1 to Continue"), 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  { MSG_EXTRA_LIFE, 0x30, 4 },
  { _("20K 40K then every 60K") , 0x30},
  { _("30K 50K then every 70K") , 0x20},
  { _("20K & 60K only") , 0x10},
  { _("30K & 60K only") , 0x00},
  { _("Internal Diff. on Life Loss"), 0xc0, 1 },
  { "-3" , 0xc0},
  { NULL, 0}
};

static struct DSW_INFO dsw_stridrua[] =
{
  { 0x1a, 0xff, dsw_data_ghouls_1 },
  { 0x1c, 0x3f, dsw_data_stridrua_2 },
  { 0x1e, 0xff, dsw_data_strider_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_dynwar_1[] =
{
	CPS1_COINAGE_3
  DSW_UNUSED( 0x00, 0x40),
  { MSG_FREE_PLAY, 0x80, 2 },
  { MSG_OFF, 0x80},
  { MSG_ON, 0x00},
  { NULL, 0}
};

static struct DSW_DATA dsw_data_dynwar_2[] =
{
	CPS1_DIFFICULTY_2
  DSW_UNUSED( 0x00, 0x08),
  DSW_UNUSED( 0x00, 0x10),
  DSW_UNUSED( 0x00, 0x20),
  DSW_UNUSED( 0x00, 0x40),
  DSW_UNUSED( 0x00, 0x80),
  { NULL, 0}
};

static struct DSW_DATA dsw_data_dynwar_3[] =
{
  { _("Freeze"), 0x01, 2 },
  { MSG_OFF, 0x01},
  { MSG_ON, 0x00},
  { _("Turbo Mode"), 0x02, 2 },
  { MSG_OFF, 0x02},
  { MSG_ON, 0x00},
  DSW_UNUSED( 0x00, 0x04),
  DSW_UNUSED( 0x00, 0x08),
  DSW_SCREEN( 0x10, 0x00),
  DSW_DEMO_SOUND( 0x20, 0x00),
  { _("Allow Continue"), 0x40, 2 },
  { MSG_NO, 0x00},
  { MSG_YES, 0x40},
  { _("Game Mode"),    0x80, 2 },
  { _("Game") , 0x80},
  { _("Test") , 0x00},
  { NULL, 0}
};

static struct DSW_INFO dsw_dynwar[] =
{
  { 0x1a, 0xff, dsw_data_dynwar_1 },
  { 0x1c, 0xff, dsw_data_dynwar_2 },
  { 0x1e, 0xff, dsw_data_dynwar_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_willow_1[] =
{
	CPS1_COINAGE_3
  { MSG_CABINET, 0xc0, 2 },
  { _("Upright 2 Players"), 0x80 },
  { _("Upright 1 Player"), 0xc0 },
  { NULL, 0}
};

static struct DSW_DATA dsw_data_willow_2[] =
{
	CPS1_DIFFICULTY_2
  { _("Nando Speed"), 0x18, 4 },
  { _("Slow") , 0x10},
  { _("Normal") , 0x18},
  { _("Fast") , 0x08},
  { _("Very Fast") , 0x00},
  DSW_UNUSED( 0x00, 0x20),
  DSW_UNUSED( 0x00, 0x40),
  { _("Stage Magic Continue"), 0x80, 2 },
  { MSG_OFF, 0x80},
  { MSG_ON, 0x00},
  { NULL, 0}
};

static struct DSW_DATA dsw_data_willow_3[] =
{
  { MSG_LIVES, 0x03, 4 },
  { "1" , 0x02},
  { "2" , 0x03},
  { "3" , 0x01},
  { "4" , 0x00},
  { _("Vitality"), 0x0c, 4 },
  { "2" , 0x00},
  { "3" , 0x0c},
  { "4" , 0x08},
  { "5" , 0x04},
  DSW_SCREEN( 0x10, 0x00),
  DSW_DEMO_SOUND( 0x20, 0x00),
  { _("Allow Continue"), 0x40, 2 },
  { MSG_NO, 0x00},
  { MSG_YES, 0x40},
  { _("Game Mode"),    0x80, 2 },
  { _("Game") , 0x80},
  { _("Test") , 0x00},
  { NULL, 0}
};

static struct DSW_INFO dsw_willow[] =
{
  { 0x1a, 0xff, dsw_data_willow_1 },
  { 0x1c, 0xff, dsw_data_willow_2 },
  { 0x1e, 0xff, dsw_data_willow_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_unsquad_1[] =
{
	CPS1_COINAGE_3
  DSW_UNUSED( 0x00, 0x40),
  DSW_UNUSED( 0x00, 0x80),
  { NULL, 0}
};

static struct DSW_DATA dsw_data_unsquad_2[] =
{
	CPS1_DIFFICULTY_1
  { _("Damage"), 0x18, 4 },
  { _("Small") , 0x10},
  { _("Normal") , 0x18},
  { _("Big") , 0x08},
  { _("Biggest") , 0x00},
  DSW_UNUSED( 0x00, 0x20),
  DSW_UNUSED( 0x00, 0x40),
  DSW_UNUSED( 0x00, 0x80),
  { NULL, 0}
};

static struct DSW_DATA dsw_data_unsquad_3[] =
{
  DSW_UNUSED( 0x00, 0x01),
  DSW_UNUSED( 0x00, 0x02),
  { MSG_FREE_PLAY, 0x04, 2 },
  { MSG_OFF, 0x04},
  { MSG_ON, 0x00},
  { _("Freeze"), 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  DSW_SCREEN( 0x10, 0x00),
  DSW_DEMO_SOUND( 0x00, 0x20),
  { _("Allow Continue"), 0x40, 2 },
  { MSG_NO, 0x40},
  { MSG_YES, 0x00},
  { _("Game Mode"),    0x80, 2 },
  { _("Game") , 0x80},
  { _("Test") , 0x00},
  { NULL, 0}
};

static struct DSW_INFO dsw_unsquad[] =
{
  { 0x1a, 0xff, dsw_data_unsquad_1 },
  { 0x1c, 0xff, dsw_data_unsquad_2 },
  { 0x1e, 0x9f, dsw_data_unsquad_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_ffight_1[] =
{
	CPS1_COINAGE_1
  { _("2 Coins to Start,1 to Continue"), 0x40, 2 },
  { MSG_OFF, 0x40},
  { MSG_ON, 0x00},
  DSW_UNUSED( 0x00, 0x80),
  { NULL, 0}
};

static struct DSW_DATA dsw_data_ffight_2[] =
{
  { _("Difficulty Level 1"), 0x07, 8 },
  { _("Easiest") , 0x07},
  { _("Easier") , 0x06},
  { _("Easy") , 0x05},
  { _("Normal") , 0x04},
  { _("Medium") , 0x03},
  { _("Hard") , 0x02},
  { _("Harder") , 0x01},
  { _("Hardest") , 0x00},
  { _("Difficulty Level 2"), 0x18, 4 },
  { _("Easy") , 0x18},
  { _("Normal") , 0x10},
  { _("Hard") , 0x08},
  { _("Hardest") , 0x00},
  { MSG_EXTRA_LIFE, 0x60, 4 },
  { _("100k") , 0x60},
  { _("200k") , 0x40},
  { _("100k and every 200k") , 0x20},
  { _("None") , 0x00},
  DSW_UNUSED( 0x00, 0x80),
  { NULL, 0}
};

static struct DSW_DATA dsw_data_ffight_3[] =
{
  { MSG_LIVES, 0x03, 4 },
  { "1" , 0x00},
  { "2" , 0x03},
  { "3" , 0x02},
  { "4" , 0x01},
  { MSG_FREE_PLAY, 0x04, 2 },
  { MSG_OFF, 0x04},
  { MSG_ON, 0x00},
  { _("Freeze"), 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  DSW_SCREEN( 0x10, 0x00),
  DSW_DEMO_SOUND( 0x00, 0x20),
  { _("Allow Continue"), 0x40, 2 },
  { MSG_NO, 0x40},
  { MSG_YES, 0x00},
  { _("Game Mode"),    0x80, 2 },
  { _("Game") , 0x80},
  { _("Test") , 0x00},
  { NULL, 0}
};

static struct DSW_INFO dsw_ffight[] =
{
  { 0x1a, 0xff, dsw_data_ffight_1 },
  { 0x1c, 0xf4, dsw_data_ffight_2 },
  { 0x1e, 0x9f, dsw_data_ffight_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data__1941_2[] =
{
	CPS1_DIFFICULTY_1
  { _("Life Bar"), 0x18, 4 },
  { _("More Slowly") , 0x18},
  { _("Slowly") , 0x10},
  { _("Quickly") , 0x08},
  { _("More Quickly") , 0x00},
  { _("Bullet's Speed"), 0x60, 4 },
  { _("Very Slow") , 0x60},
  { _("Slow") , 0x40},
  { _("Fast") , 0x20},
  { _("Very Fast") , 0x00},
  { _("Initial Vitality"), 0x80, 2 },
  { _("3 Bars") , 0x80},
  { _("4 Bars") , 0x00},
  { NULL, 0}
};

static struct DSW_DATA dsw_data__1941_3[] =
{
  { _("Throttle Game Speed"), 0x01, 2 },
  { MSG_OFF, 0x00},
  { MSG_ON, 0x01},
  DSW_UNUSED( 0x00, 0x02),
  { MSG_FREE_PLAY, 0x04, 2 },
  { MSG_OFF, 0x04},
  { MSG_ON, 0x00},
  { _("Freeze"), 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  DSW_SCREEN( 0x10, 0x00),
  DSW_DEMO_SOUND( 0x00, 0x20),
  { _("Allow Continue"), 0x40, 2 },
  { MSG_NO, 0x40},
  { MSG_YES, 0x00},
  { _("Game Mode"),    0x80, 2 },
  { _("Game") , 0x80},
  { _("Test") , 0x00},
  { NULL, 0}
};

static struct DSW_INFO dsw_1941[] =
{
  { 0x1a, 0xff, dsw_data_ffight_1 },
  { 0x1c, 0xff, dsw_data__1941_2 },
  { 0x1e, 0x9f, dsw_data__1941_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_mercs_1[] =
{
	CPS1_COINAGE_2
  DSW_UNUSED( 0x00, 0x08),
  DSW_UNUSED( 0x00, 0x10),
  DSW_UNUSED( 0x00, 0x20),
  { _("2 Coins to Start,1 to Continue"), 0x40, 2 },
  { MSG_OFF, 0x40},
  { MSG_ON, 0x00},
  DSW_UNUSED( 0x00, 0x80),
  { NULL, 0}
};

static struct DSW_DATA dsw_data_mercs_2[] =
{
	CPS1_DIFFICULTY_1
  { _("Coin Slots"), 0x08, 2 },
  { "1" , 0x00},
  { "3" , 0x08},
  { _("Max Players"), 0x10, 2 },
  { "2" , 0x00},
  { "3" , 0x10},
  DSW_UNUSED( 0x00, 0x20),
  DSW_UNUSED( 0x00, 0x40),
  DSW_UNUSED( 0x00, 0x80),
  { NULL, 0}
};

static struct DSW_DATA dsw_data_mercs_3[] =
{
  DSW_UNUSED( 0x00, 0x01),
  DSW_UNUSED( 0x00, 0x02),
  DSW_UNUSED( 0x00, 0x04),
  { _("Freeze"), 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  DSW_SCREEN( 0x10, 0x00),
  DSW_DEMO_SOUND( 0x00, 0x20),
  { _("Allow Continue"), 0x40, 2 },
  { MSG_NO, 0x40},
  { MSG_YES, 0x00},
  DSW_SERVICE( 0, 0x80),
  { NULL, 0}
};

static struct DSW_INFO dsw_mercs[] =
{
  { 0x1a, 0xff, dsw_data_mercs_1 },
  { 0x1c, 0xff, dsw_data_mercs_2 },
  { 0x1e, 0x9f, dsw_data_mercs_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_mtwins_1[] =
{
	CPS1_COINAGE_1
  DSW_UNUSED( 0x00, 0x40),
  DSW_UNUSED( 0x00, 0x80),
  { NULL, 0}
};

static struct DSW_DATA dsw_data_mtwins_2[] =
{
	CPS1_DIFFICULTY_1
  { MSG_LIVES, 0x38, 4},
	{ "1", 0x10 },
	{ "2", 0x18 },
	{ "3", 0x8 },
	{ "4", 0 },
  DSW_UNUSED( 0x00, 0x40),
  DSW_UNUSED( 0x00, 0x80),
  { NULL, 0}
};

static struct DSW_INFO dsw_mtwins[] =
{
  { 0x1a, 0xff, dsw_data_mtwins_1 },
  { 0x1c, 0xcf, dsw_data_mtwins_2 },
  { 0x1e, 0x9f, dsw_data_unsquad_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_msword_2[] =
{
  { _("Player's vitality consumption"), 0x07, 8 },
  { _("1 (Easiest)") , 0x07},
  { "2" , 0x06},
  { "3" , 0x05},
  { _("4 (Normal)") , 0x04},
  { "5" , 0x03},
  { "6" , 0x02},
  { "7" , 0x01},
  { _("8 (Hardest)") , 0x00},
  { _("Enemy's vitality and attacking power"), 0x38, 8 },
  { _("1 (Easiest)") , 0x20},
  { "2" , 0x28},
  { "3" , 0x30},
  { _("4 (Normal)") , 0x38},
  { "5" , 0x18},
  { "6" , 0x10},
  { "7" , 0x08},
  { _("8 (Hardest)") , 0x00},
  { _("Stage Select"), 0x40, 2 },
  { MSG_OFF, 0x40},
  { MSG_ON, 0x00},
  DSW_UNUSED( 0x00, 0x80),
  { NULL, 0}
};

static struct DSW_DATA dsw_data_msword_3[] =
{
  { _("Vitality Packs"), 0x03, 4 },
  { "1" , 0x00},
  { "2" , 0x03},
  { _("3 (2 when continue)") , 0x02},
  { _("4 (3 when continue)") , 0x01},
  { MSG_FREE_PLAY, 0x04, 2 },
  { MSG_OFF, 0x04},
  { MSG_ON, 0x00},
  { _("Freeze"), 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  DSW_SCREEN( 0x10, 0x00),
  DSW_DEMO_SOUND( 0x00, 0x20),
  { _("Allow Continue"), 0x40, 2 },
  { MSG_NO, 0x40},
  { MSG_YES, 0x00},
  { _("Game Mode"),    0x80, 2 },
  { _("Game") , 0x80},
  { _("Test") , 0x00},
  { NULL, 0}
};

static struct DSW_INFO dsw_msword[] =
{
  { 0x1a, 0xff, dsw_data_ffight_1 },
  { 0x1c, 0xbc, dsw_data_msword_2 },
  { 0x1e, 0x9f, dsw_data_msword_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_cawing_2[] =
{
  { _("Difficulty Level (Enemy's Strength)"), 0x07, 8 },
  { _("1 (Easiest)") , 0x07},
  { "2" , 0x06},
  { "3" , 0x05},
  { _("4 (Normal)") , 0x04},
  { "5" , 0x03},
  { "6" , 0x02},
  { "7" , 0x01},
  { _("8 (Hardest)") , 0x00},
  { _("Difficulty Level (Player's Strength)"), 0x18, 4 },
  { _("Easy") , 0x10},
  { _("Normal") , 0x18},
  { _("Hard") , 0x08},
  { _("Hardest") , 0x00},
  DSW_UNUSED( 0x00, 0x20),
  DSW_UNUSED( 0x00, 0x40),
  DSW_UNUSED( 0x00, 0x80),
  { NULL, 0}
};

static struct DSW_INFO dsw_cawing[] =
{
  { 0x1a, 0xff, dsw_data_ffight_1 },
  { 0x1c, 0xfc, dsw_data_cawing_2 },
  { 0x1e, 0x9f, dsw_data_unsquad_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_nemo_2[] =
{
	CPS1_DIFFICULTY_1
  { _("Life Bar"), 0x18, 2 },
  { _("Minimun") , 0x00},
  { _("Medium") , 0x18},
  DSW_UNUSED( 0x00, 0x20),
  DSW_UNUSED( 0x00, 0x40),
  DSW_UNUSED( 0x00, 0x80),
  { NULL, 0}
};

static struct DSW_DATA dsw_data_nemo_3[] =
{
  { MSG_LIVES, 0x03, 4 },
  { "1" , 0x02},
  { "2" , 0x03},
  { "3" , 0x01},
  { "4" , 0x00},
  { MSG_FREE_PLAY, 0x04, 2 },
  { MSG_OFF, 0x04},
  { MSG_ON, 0x00},
  { _("Freeze"), 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  DSW_SCREEN( 0x10, 0x00),
  DSW_DEMO_SOUND( 0x00, 0x20),
  { _("Allow Continue"), 0x40, 2 },
  { MSG_NO, 0x40},
  { MSG_YES, 0x00},
  { _("Game Mode"),    0x80, 2 },
  { _("Game") , 0x80},
  { _("Test") , 0x00},
  { NULL, 0}
};

static struct DSW_INFO dsw_nemo[] =
{
  { 0x1a, 0xff, dsw_data_ffight_1 },
  { 0x1c, 0xff, dsw_data_nemo_2 },
  { 0x1e, 0x9f, dsw_data_nemo_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_sf2_2[] =
{
	CPS1_DIFFICULTY_1
  DSW_UNUSED( 0x00, 0x08),
  DSW_UNUSED( 0x00, 0x10),
  DSW_UNUSED( 0x00, 0x20),
  DSW_UNUSED( 0x00, 0x40),
  DSW_UNUSED( 0x00, 0x80),
  { NULL, 0}
};

static struct DSW_DATA dsw_data_sf2m2_2[] =
{
	CPS1_DIFFICULTY_1
  DSW_UNUSED( 0x00, 0x08),
  DSW_UNUSED( 0x00, 0x20),
  DSW_UNUSED( 0x00, 0x40),
  DSW_UNUSED( 0x00, 0x80),
  { NULL, 0}
};

static struct DSW_INFO dsw_sf2[] =
{
  { 0x1a, 0xff, dsw_data_ffight_1 },
  { 0x1c, 0xff, dsw_data_sf2_2 },
  { 0x1e, 0x9f, dsw_data_unsquad_3 },
  { 0, 0, NULL }
};

static struct DSW_INFO dsw_sf2m2[] =
{
  { 0x1a, 0xff, dsw_data_ffight_1 },
  { 0x1c, 0xef, dsw_data_sf2m2_2 },
  { 0x1e, 0x9f, dsw_data_unsquad_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_sf2j_2[] =
{
	CPS1_DIFFICULTY_1
  { _("2 Players Game"), 0x08, 2 },
  { _("1 Credit/No Continue") , 0x08},
  { _("2 Credits/Winner Continue") , 0x00},
  { MSG_UNKNOWN, 0x10, 2 },
  { MSG_OFF, 0x10},
  { MSG_ON, 0x00},
  { MSG_UNKNOWN, 0x20, 2 },
  { MSG_OFF, 0x20},
  { MSG_ON, 0x00},
  { MSG_UNKNOWN, 0x40, 2 },
  { MSG_OFF, 0x40},
  { MSG_ON, 0x00},
  { MSG_UNKNOWN, 0x80, 2 },
  { MSG_OFF, 0x80},
  { MSG_ON, 0x00},
  { NULL, 0}
};

static struct DSW_DATA dsw_data_sf2j_3[] =
{
  { MSG_UNKNOWN, 0x01, 2 },
  { MSG_OFF, 0x01},
  { MSG_ON, 0x00},
  { MSG_UNKNOWN, 0x02, 2 },
  { MSG_OFF, 0x02},
  { MSG_ON, 0x00},
  { MSG_FREE_PLAY, 0x04, 2 },
  { MSG_OFF, 0x04},
  { MSG_ON, 0x00},
  { _("Freeze"), 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  DSW_SCREEN( 0x10, 0x00),
  DSW_DEMO_SOUND( 0x00, 0x20),
  { _("Allow Continue"), 0x40, 2 },
  { MSG_NO, 0x40},
  { MSG_YES, 0x00},
  { _("Game Mode"),    0x80, 2 },
  { _("Game") , 0x80},
  { _("Test") , 0x00},
  { NULL, 0}
};

static struct DSW_INFO dsw_sf2j[] =
{
  { 0x1a, 0xff, dsw_data_ffight_1 },
  { 0x1c, 0xff, dsw_data_sf2j_2 },
  { 0x1e, 0x9f, dsw_data_sf2j_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data__3wonders_1[] =
{
	CPS1_COINAGE_1
  { _("2 Coins to Start,1 to Continue"), 0x40, 2 },
  { MSG_OFF, 0x40},
  { MSG_ON, 0x00},
  { _("Freeze"), 0x80, 2 },
  { MSG_OFF, 0x80},
  { MSG_ON, 0x00},
  { NULL, 0}
};

static struct DSW_DATA dsw_data__3wonders_2[] =
{
  { _("Lives (Midnight Wanderers)"), 0x03, 4 },
  { "1" , 0x03},
  { "2" , 0x02},
  { "3" , 0x01},
  { "5" , 0x00},
  { _("Difficulty (Midnight Wanderers)"), 0x0c, 4 },
  { _("Easy") , 0x0c},
  { _("Normal") , 0x08},
  { _("Hard") , 0x04},
  { _("Hardest") , 0x00},
  { _("Lives (Chariot)"), 0x30, 4 },
  { "1" , 0x30},
  { "2" , 0x20},
  { "3" , 0x10},
  { "5" , 0x00},
  { _("Difficulty (Chariot)"), 0xc0, 4 },
  { _("Easy") , 0xc0},
  { _("Normal") , 0x80},
  { _("Hard") , 0x40},
  { _("Hardest") , 0x00},
  { NULL, 0}
};

static struct DSW_DATA dsw_data__3wonders_3[] =
{
  { _("Lives (Don't Pull)"), 0x03, 4 },
  { "1" , 0x03},
  { "2" , 0x02},
  { "3" , 0x01},
  { "5" , 0x00},
  { _("Difficulty (Don't Pull)"), 0x0c, 4 },
  { _("Easy") , 0x0c},
  { _("Normal") , 0x08},
  { _("Hard") , 0x04},
  { _("Hardest") , 0x00},
  DSW_SCREEN( 0x10, 0x00),
  DSW_DEMO_SOUND( 0x00, 0x20),
  { _("Allow Continue"), 0x40, 2 },
  { MSG_NO, 0x40},
  { MSG_YES, 0x00},
  { _("Game Mode"),    0x80, 2 },
  { _("Game") , 0x80},
  { _("Test") , 0x00},
  { NULL, 0}
};

static struct DSW_INFO dsw_3wonders[] =
{
  { 0x1a, 0xff, dsw_data__3wonders_1 },
  { 0x1c, 0x9a, dsw_data__3wonders_2 },
  { 0x1e, 0x99, dsw_data__3wonders_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_kod_1[] =
{
	CPS1_COINAGE_2
  { _("Coin Slots"), 0x08, 2 },
  { "1" , 0x00},
  { "3" , 0x08},
  { _("Max Players"), 0x10, 2 },
  { "2" , 0x00},
  { "3" , 0x10},
  DSW_UNUSED( 0x00, 0x20),
  { _("2 Coins to Start,1 to Continue"), 0x40, 2 },
  { MSG_OFF, 0x40},
  { MSG_ON, 0x00},
  DSW_UNUSED( 0x00, 0x80),
  { NULL, 0}
};

static struct DSW_DATA dsw_data_kod_2[] =
{
	CPS1_DIFFICULTY_1
  { MSG_LIVES, 0x38, 8 },
  { "1" , 0x30},
  { "2" , 0x38},
  { "3" , 0x28},
  { "4" , 0x20},
  { "5" , 0x18},
  { "6" , 0x10},
  { "7" , 0x08},
  { "8" , 0x00},
  { MSG_EXTRA_LIFE, 0xc0, 4 },
  { _("80k and every 400k") , 0x80},
  { _("100k and every 450k") , 0xc0},
  { _("160k and every 450k") , 0x40},
  { _("None") , 0x00},
  { NULL, 0}
};

static struct DSW_INFO dsw_kod[] =
{
  { 0x1a, 0xff, dsw_data_kod_1 },
  { 0x1c, 0xff, dsw_data_kod_2 },
  { 0x1e, 0x9f, dsw_data_unsquad_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_kodj_2[] =
{
	CPS1_DIFFICULTY_1
  { MSG_LIVES, 0x38, 8 },
  { "1" , 0x30},
  { "2" , 0x38},
  { "3" , 0x28},
  { "4" , 0x20},
  { "5" , 0x18},
  { "6" , 0x10},
  { "7" , 0x08},
  { "8" , 0x00},
  { MSG_EXTRA_LIFE, 0xc0, 4 },
  { _("80k and every 400k") , 0x80},
  { _("200k and every 450k") , 0xc0},
  { _("160k and every 450k") , 0x40},
  { _("None") , 0x00},
  { NULL, 0}
};

static struct DSW_INFO dsw_kodj[] =
{
  { 0x1a, 0xff, dsw_data_kod_1 },
  { 0x1c, 0xff, dsw_data_kodj_2 },
  { 0x1e, 0x9f, dsw_data_unsquad_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_captcomm_2[] =
{
  { _("Difficulty 1"), 0x07, 8 },
  { _("1 (Easiest)") , 0x07},
  { "2" , 0x06},
  { "3" , 0x05},
  { _("4 (Normal)") , 0x04},
  { "5" , 0x03},
  { "6" , 0x02},
  { "7" , 0x01},
  { _("8 (Hardest)") , 0x00},
  { _("Difficulty 2"), 0x18, 4 },
  { _("Easy") , 0x18},
  { _("Normal") , 0x10},
  { _("Hard") , 0x08},
  { _("Hardest") , 0x00},
  DSW_UNUSED( 0x00, 0x20),
  { _("Max Players"), 0xc0, 4 },
  { "1" , 0x40},
  { "2" , 0xc0},
  { "3" , 0x80},
  { "4" , 0x00},
  { NULL, 0}
};

static struct DSW_INFO dsw_captcomm[] =
{
  { 0x1a, 0xff, dsw_data_mercs_1 },
  { 0x1c, 0xf4, dsw_data_captcomm_2 },
  { 0x1e, 0x9f, dsw_data_ffight_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_knights_2[] =
{
  { _("Enemy's attack frequency"), 0x07, 8 },
  { _("1 (Easiest)") , 0x07},
  { "2" , 0x06},
  { "3" , 0x05},
  { _("4 (Normal)") , 0x04},
  { "5" , 0x03},
  { "6" , 0x02},
  { "7" , 0x01},
  { _("8 (Hardest)") , 0x00},
  { _("Enemy's attack power"), 0x38, 8 },
  { _("1 (Easiest)") , 0x10},
  { "2" , 0x08},
  { "3" , 0x00},
  { _("4 (Normal)") , 0x38},
  { "5" , 0x30},
  { "6" , 0x28},
  { "7" , 0x20},
  { _("8 (Hardest)") , 0x18},
  { _("Coin Slots"), 0x40, 2 },
  { "1" , 0x00},
  { "3" , 0x40},
  { _("Max Players"), 0x80, 2 },
  { "2" , 0x00},
  { "3" , 0x80},
  { NULL, 0}
};

static struct DSW_INFO dsw_knights[] =
{
  { 0x1a, 0xff, dsw_data_mercs_1 },
  { 0x1c, 0xfc, dsw_data_knights_2 },
  { 0x1e, 0x9f, dsw_data_ffight_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_varth_2[] =
{
	CPS1_DIFFICULTY_1
  { MSG_EXTRA_LIFE, 0x18, 4 },
  { _("600k and every 1.400k") , 0x18},
  { _("600k 2.000k and 4500k") , 0x10},
  { _("1.200k 3.500k") , 0x08},
  { _("2000k only") , 0x00},
  DSW_UNUSED( 0x00, 0x20),
  DSW_UNUSED( 0x00, 0x40),
  DSW_UNUSED( 0x00, 0x80),
  { NULL, 0}
};

static struct DSW_DATA dsw_data_varth_3[] =
{
  { MSG_LIVES, 0x03, 4 },
  { "1" , 0x02},
  { "2" , 0x01},
  { "3" , 0x03},
  { "4" , 0x00},
  { MSG_FREE_PLAY, 0x04, 2 },
  { MSG_OFF, 0x04},
  { MSG_ON, 0x00},
  { _("Freeze"), 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  DSW_SCREEN( 0x10, 0x00),
  DSW_DEMO_SOUND( 0x00, 0x20),
  { _("Allow Continue"), 0x40, 2 },
  { MSG_NO, 0x40},
  { MSG_YES, 0x00},
  { _("Game Mode"),    0x80, 2 },
  { _("Game") , 0x80},
  { _("Test") , 0x00},
  { NULL, 0}
};

static struct DSW_INFO dsw_varth[] =
{
  { 0x1a, 0xff, dsw_data_ffight_1 },
  { 0x1c, 0xff, dsw_data_varth_2 },
  { 0x1e, 0x9f, dsw_data_varth_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_cworld2j_1[] =
{
	CPS1_COINAGE_2
  { MSG_UNKNOWN, 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  { MSG_UNKNOWN, 0x10, 2 },
  { MSG_OFF, 0x10},
  { MSG_ON, 0x00},
  { MSG_UNKNOWN, 0x20, 2 },
  { MSG_OFF, 0x20},
  { MSG_ON, 0x00},
  { _("2 Coins to Start, 1 to Continue"), 0x40, 2 },
  { MSG_OFF, 0x40},
  { MSG_ON, 0x00},
  { _("Extended Test Mode"), 0x80, 2 },
  { MSG_OFF, 0x80},
  { MSG_ON, 0x00},
  { NULL, 0}
};

static struct DSW_DATA dsw_data_cworld2j_2[] =
{
  { MSG_DIFFICULTY, 0x07, 5 },
  { "0" , 0x06},
  { "1" , 0x05},
  { "2" , 0x04},
  { "3" , 0x03},
  { "4" , 0x02},
  { _("Extend"), 0x18, 3 },
  { _("N") , 0x18},
  { _("E") , 0x10},
  { _("D") , 0x00},
  { MSG_LIVES, 0xe0, 5 },
  { "1" , 0x00},
  { "2" , 0x80},
  { "3" , 0xe0},
  { "4" , 0xa0},
  { "5" , 0xc0},
  { NULL, 0}
};

static struct DSW_DATA dsw_data_cworld2j_3[] =
{
  { MSG_UNKNOWN, 0x01, 2 },
  { MSG_OFF, 0x01},
  { MSG_ON, 0x00},
  { MSG_UNKNOWN, 0x02, 2 },
  { MSG_OFF, 0x02},
  { MSG_ON, 0x00},
  { MSG_FREE_PLAY, 0x04, 2 },
  { MSG_OFF, 0x04},
  { MSG_ON, 0x00},
  { _("Freeze"), 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  DSW_SCREEN( 0x10, 0x00),
  DSW_DEMO_SOUND( 0x00, 0x20),
  { _("Allow Continue"), 0x40, 2 },
  { MSG_NO, 0x00},
  { MSG_YES, 0x40},
  { _("Game Mode"),    0x80, 2 },
  { _("Game") , 0x80},
  { _("Test") , 0x00},
  { NULL, 0}
};

static struct DSW_INFO dsw_cworld2j[] =
{
  { 0x1a, 0xff, dsw_data_cworld2j_1 },
  { 0x1c, 0xfe, dsw_data_cworld2j_2 },
  { 0x1e, 0xdf, dsw_data_cworld2j_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_wof_3[] =
{
  { _("Freeze"), 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  { NULL, 0}
};

static struct DSW_DATA dsw_data_dino_3[] =
{
  { _("Freeze"), 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  { _("TEST MODE"), 0x80, 2 },
  { MSG_OFF, 0x80},
  { MSG_ON, 0x00},
  { NULL, 0}
};

static struct DSW_INFO dsw_wof[] =
{
  { 0x1e, 0x08, dsw_data_wof_3 },
  { 0, 0, NULL }
};

static struct DSW_INFO dsw_dino[] =
{
  { 0x1e, 0x88, dsw_data_dino_3 },
  { 0, 0, NULL }
};

static struct DSW_INFO dsw_punisher[] =
{
  { 0x1e, 0x08, dsw_data_wof_3 },
  { 0, 0, NULL }
};

static struct DSW_INFO dsw_slammast[] =
{
  { 0x1e, 0x08, dsw_data_wof_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_pnickj_1[] =
{
	CPS1_COINAGE_2
  { _("Coin Slots"), 0x08, 2 },
  { "1" , 0x08},
  { "2" , 0x00},
  { MSG_UNKNOWN, 0x10, 2 },
  { MSG_OFF, 0x10},
  { MSG_ON, 0x00},
  { MSG_UNKNOWN, 0x20, 2 },
  { MSG_OFF, 0x20},
  { MSG_ON, 0x00},
  { MSG_UNKNOWN, 0x40, 2 },
  { MSG_OFF, 0x40},
  { MSG_ON, 0x00},
  { MSG_UNKNOWN, 0x80, 2 },
  { MSG_OFF, 0x80},
  { MSG_ON, 0x00},
  { NULL, 0}
};

static struct DSW_DATA dsw_data_pnickj_2[] =
{
	CPS1_DIFFICULTY_1
  { MSG_UNKNOWN, 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  { MSG_UNKNOWN, 0x10, 2 },
  { MSG_OFF, 0x10},
  { MSG_ON, 0x00},
  { MSG_UNKNOWN, 0x20, 2 },
  { MSG_OFF, 0x20},
  { MSG_ON, 0x00},
  { _("Vs Play Mode"), 0xc0, 4 },
  { _("1 Game Match") , 0xc0},
  { _("3 Games Match") , 0x80},
  { _("5 Games Match") , 0x40},
  { _("7 Games Match") , 0x00},
  { NULL, 0}
};

static struct DSW_DATA dsw_data_pnickj_3[] =
{
  { MSG_UNKNOWN, 0x01, 2 },
  { MSG_OFF, 0x01},
  { MSG_ON, 0x00},
  { MSG_UNKNOWN, 0x02, 2 },
  { MSG_OFF, 0x02},
  { MSG_ON, 0x00},
  { MSG_UNKNOWN, 0x04, 2 },
  { MSG_OFF, 0x04},
  { MSG_ON, 0x00},
  { _("Freeze"), 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  DSW_SCREEN( 0x10, 0x00),
  DSW_DEMO_SOUND( 0x00, 0x20),
  { MSG_UNKNOWN, 0x40, 2 },
  { MSG_OFF, 0x40},
  { MSG_ON, 0x00},
  { _("Game Mode"),    0x80, 2 },
  { _("Game") , 0x80},
  { _("Test") , 0x00},
  { NULL, 0}
};

static struct DSW_INFO dsw_pnickj[] =
{
  { 0x1a, 0xff, dsw_data_pnickj_1 },
  { 0x1c, 0xff, dsw_data_pnickj_2 },
  { 0x1e, 0xdf, dsw_data_pnickj_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_qad_1[] =
{
	CPS1_COINAGE_2
  { MSG_UNKNOWN, 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  { MSG_UNKNOWN, 0x10, 2 },
  { MSG_OFF, 0x10},
  { MSG_ON, 0x00},
  { MSG_UNKNOWN, 0x20, 2 },
  { MSG_OFF, 0x20},
  { MSG_ON, 0x00},
  { _("2 Coins to Start,1 to Continue"), 0x40, 2 },
  { MSG_OFF, 0x40},
  { MSG_ON, 0x00},
  DSW_UNUSED( 0x00, 0x80),
  { NULL, 0}
};

static struct DSW_DATA dsw_data_qad_2[] =
{
  { MSG_DIFFICULTY, 0x07},
  { _("Wisdom"), 0x18, 4 },
  { _("Easy") , 0x18},
  { _("Normal") , 0x10},
  { _("Hard") , 0x08},
  { _("Hardest") , 0x00},
  { MSG_LIVES, 0xe0, 5 },
  { "1" , 0x60},
  { "2" , 0x80},
  { "3" , 0xa0},
  { "4" , 0xc0},
  { "5" , 0xe0},
  { NULL, 0}
};

static struct DSW_DATA dsw_data_qad_3[] =
{
  DSW_UNUSED( 0x00, 0x01),
  DSW_UNUSED( 0x00, 0x02),
  { MSG_FREE_PLAY, 0x04, 2 },
  { MSG_OFF, 0x04},
  { MSG_ON, 0x00},
  { _("Freeze"), 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  DSW_SCREEN( 0x10, 0x00),
  DSW_DEMO_SOUND( 0x20, 0x00),
  { _("Allow Continue"), 0x40, 2 },
  { MSG_NO, 0x00},
  { MSG_YES, 0x40},
  { _("Game Mode"),    0x80, 2 },
  { _("Game") , 0x80},
  { _("Test") , 0x00},
  { NULL, 0}
};

static struct DSW_INFO dsw_qad[] =
{
  { 0x1a, 0xff, dsw_data_qad_1 },
  { 0x1c, 0xf4, dsw_data_qad_2 },
  { 0x1e, 0xff, dsw_data_qad_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_qadj_2[] =
{
  { MSG_DIFFICULTY, 0x07, 5 },
  { "0" , 0x07},
  { "1" , 0x06},
  { "2" , 0x05},
  { "3" , 0x04},
  { "4" , 0x03},
  { MSG_UNKNOWN, 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  { MSG_UNKNOWN, 0x10, 2 },
  { MSG_OFF, 0x10},
  { MSG_ON, 0x00},
  { MSG_LIVES, 0xe0, 3 },
  { "1" , 0xa0},
  { "2" , 0xc0},
  { "3" , 0xe0},
  { NULL, 0}
};

static struct DSW_INFO dsw_qadj[] =
{
  { 0x1a, 0xff, dsw_data_qad_1 },
  { 0x1c, 0xff, dsw_data_qadj_2 },
  { 0x1e, 0xdf, dsw_data_cworld2j_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_qtono2_1[] =
{
	CPS1_COINAGE_2
  { MSG_UNKNOWN, 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  { MSG_UNKNOWN, 0x10, 2 },
  { MSG_OFF, 0x10},
  { MSG_ON, 0x00},
  { MSG_UNKNOWN, 0x20, 2 },
  { MSG_OFF, 0x20},
  { MSG_ON, 0x00},
  { _("2 Coins to Start, 1 to Continue"), 0x40, 2 },
  { MSG_OFF, 0x40},
  { MSG_ON, 0x00},
  { MSG_UNKNOWN, 0x80, 2 },
  { MSG_OFF, 0x80},
  { MSG_ON, 0x00},
  { NULL, 0}
};

static struct DSW_DATA dsw_data_qtono2_2[] =
{
	CPS1_DIFFICULTY_1
  { MSG_UNKNOWN, 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  { MSG_UNKNOWN, 0x10, 2 },
  { MSG_OFF, 0x10},
  { MSG_ON, 0x00},
  { MSG_LIVES, 0xe0, 5 },
  { "1" , 0x60},
  { "2" , 0x80},
  { "3" , 0xe0},
  { "4" , 0xa0},
  { "5" , 0xc0},
  { NULL, 0}
};

static struct DSW_DATA dsw_data_qtono2_3[] =
{
  { MSG_UNKNOWN, 0x01, 2 },
  { MSG_OFF, 0x01},
  { MSG_ON, 0x00},
  { _("Infinite Lives"), 0x02, 2 },
  { MSG_OFF, 0x02},
  { MSG_ON, 0x00},
  { MSG_FREE_PLAY, 0x04, 2 },
  { MSG_OFF, 0x04},
  { MSG_ON, 0x00},
  { _("Freeze"), 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  DSW_SCREEN( 0x10, 0x00),
  DSW_DEMO_SOUND( 0x00, 0x20),
  { _("Allow Continue"), 0x40, 2 },
  { MSG_NO, 0x00},
  { MSG_YES, 0x40},
  { _("Game Mode"),    0x80, 2 },
  { _("Game") , 0x80},
  { _("Test") , 0x00},
  { NULL, 0}
};

static struct DSW_INFO dsw_qtono2[] =
{
  { 0x1a, 0xff, dsw_data_qtono2_1 },
  { 0x1c, 0xff, dsw_data_qtono2_2 },
  { 0x1e, 0xdf, dsw_data_qtono2_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_pang3[] =
{ // Sure about test mode, not sure about the other bits...
  { _("Game Mode"),    0x80, 2 },
  { _("Game") , 0x80},
  { _("Test") , 0x00},
  { NULL, 0}
};

static struct DSW_INFO dsw_pang3[] =
{
  { 0x1e, 0xff, dsw_data_pang3 },
  { 0, 0, NULL }
};

static struct DSW_INFO dsw_pang3j[] =
{
  { 0x1e, 0x08, dsw_data_wof_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_megaman_1[] =
{
  { MSG_COINAGE, 0x1f, 19 },
  { MSG_9COIN_1PLAY, 0x0f},
  { MSG_8COIN_1PLAY, 0x10},
  { MSG_7COIN_1PLAY, 0x11},
  { MSG_6COIN_1PLAY, 0x12},
  { MSG_5COIN_1PLAY, 0x13},
  { MSG_4COIN_1PLAY, 0x14},
  { MSG_3COIN_1PLAY, 0x15},
  { MSG_2COIN_1PLAY, 0x16},
  { _("2 Coins to Start,1 to Continue") , 0x0e},
  { MSG_1COIN_1PLAY, 0x1f},
  { MSG_1COIN_1PLAY, 0x1e},
  { MSG_1COIN_3PLAY, 0x1d},
  { MSG_1COIN_4PLAY, 0x1c},
  { MSG_1COIN_5PLAY, 0x1b},
  { MSG_1COIN_6PLAY, 0x1a},
  { MSG_1COIN_7PLAY, 0x19},
  { MSG_1COIN_8PLAY, 0x18},
  { MSG_1COIN_9PLAY, 0x17},
  { MSG_FREE_PLAY, 0x0d},
  { _("Coin slots"), 0x60},
  { MSG_UNKNOWN, 0x80, 2 },
  { MSG_OFF, 0x80},
  { MSG_ON, 0x00},
  { NULL, 0}
};

static struct DSW_DATA dsw_data_megaman_2[] =
{
  { MSG_DIFFICULTY, 0x03, 4 },
  { _("Easy") , 0x03},
  { _("Normal") , 0x02},
  { _("Hard") , 0x01},
  { _("Hardest") , 0x00},
  { _("Time"), 0x0c, 4 },
  { "100" , 0x0c},
  { "90" , 0x08},
  { "70" , 0x04},
  { "60" , 0x00},
  { MSG_UNKNOWN, 0x10, 2 },
  { MSG_OFF, 0x10},
  { MSG_ON, 0x00},
  { MSG_UNKNOWN, 0x20, 2 },
  { MSG_OFF, 0x20},
  { MSG_ON, 0x00},
  { _("Voice"), 0x40, 2 },
  { MSG_OFF, 0x00},
  { MSG_ON, 0x40},
  { MSG_UNKNOWN, 0x80, 2 },
  { MSG_OFF, 0x80},
  { MSG_ON, 0x00},
  { NULL, 0}
};

static struct DSW_DATA dsw_data_megaman_3[] =
{
  DSW_SCREEN( 0x01, 0x00),
  DSW_DEMO_SOUND( 0x02, 0x00),
  { _("Allow Continue"), 0x04, 2 },
  { MSG_NO, 0x00},
  { MSG_YES, 0x04},
  { MSG_UNKNOWN, 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  { MSG_UNKNOWN, 0x10, 2 },
  { MSG_OFF, 0x10},
  { MSG_ON, 0x00},
  { MSG_UNKNOWN, 0x20, 2 },
  { MSG_OFF, 0x20},
  { MSG_ON, 0x00},
  { MSG_UNKNOWN, 0x40, 2 },
  { MSG_OFF, 0x40},
  { MSG_ON, 0x00},
  { _("Game Mode"),    0x80, 2 },
  { _("Game") , 0x80},
  { _("Test") , 0x00},
  { NULL, 0}
};

static struct DSW_INFO dsw_megaman[] =
{
  { 0x1a, 0xff, dsw_data_megaman_1 },
  { 0x1c, 0xfe, dsw_data_megaman_2 },
  { 0x1e, 0xff, dsw_data_megaman_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_rockmanj_2[] =
{
  { MSG_DIFFICULTY, 0x03, 4 },
  { _("Easy") , 0x03},
  { _("Normal") , 0x02},
  { _("Hard") , 0x01},
  { _("Hardest") , 0x00},
  { _("Time"), 0x0c, 4 },
  { "100" , 0x0c},
  { "90" , 0x08},
  { "70" , 0x04},
  { "60" , 0x00},
  { MSG_UNKNOWN, 0x10, 2 },
  { MSG_OFF, 0x10},
  { MSG_ON, 0x00},
  { MSG_UNKNOWN, 0x20, 2 },
  { MSG_OFF, 0x20},
  { MSG_ON, 0x00},
  { MSG_UNKNOWN, 0x40, 2 },
  { MSG_OFF, 0x40},
  { MSG_ON, 0x00},
  { MSG_UNKNOWN, 0x80, 2 },
  { MSG_OFF, 0x80},
  { MSG_ON, 0x00},
  { NULL, 0}
};

static struct DSW_INFO dsw_rockmanj[] =
{
  { 0x1a, 0xff, dsw_data_megaman_1 },
  { 0x1c, 0xfe, dsw_data_rockmanj_2 },
  { 0x1e, 0xff, dsw_data_megaman_3 },
  { 0, 0, NULL }
};

// Inputs entierly remade, using INCL_INP... (jully 2013 !)

static struct INPUT_INFO input_cps1_3b[] =
{
  INP0( COIN1, 0x00, 0x01),
  INP0( COIN2, 0x00, 0x02),
  INP0( SERVICE, 0x00, 0x04),
  INP0( UNKNOWN, 0x00, 0x08),
  INP0( P1_START, 0x00, 0x10),
  INP0( P2_START, 0x00, 0x20),
  INP0( TEST, 0x00, 0x40),
  INP0( UNKNOWN, 0x00, 0x80),

  INP0( P1_RIGHT, 0x02, 0x01),
  INP0( P1_LEFT, 0x02, 0x02),
  INP0( P1_DOWN, 0x02, 0x04),
  INP0( P1_UP, 0x02, 0x08),
  INP0( P1_B1, 0x02, 0x10),
  INP0( P1_B2, 0x02, 0x20),
  INP0( P1_B3, 0x02, 0x40),
#ifndef RAINE_DOS
  INP0( P1_B1B2, 2, 0x30),
#endif
  INP0( UNKNOWN, 0x02, 0x80),
  INP0( P2_RIGHT, 0x03, 0x01),
  INP0( P2_LEFT, 0x03, 0x02),
  INP0( P2_DOWN, 0x03, 0x04),
  INP0( P2_UP, 0x03, 0x08),
  INP0( P2_B1, 0x03, 0x10),
  INP0( P2_B2, 0x03, 0x20),
  INP0( P2_B3, 0x03, 0x40),
#ifndef RAINE_DOS
  INP0( P2_B1B2, 3, 0x30),
#endif
  INP0( UNKNOWN, 0x03, 0x80),
  END_INPUT
};

static struct INPUT_INFO input_cps1_2b[] =
{
  INCL_INP( cps1_3b ),

  INP0( UNKNOWN, 0x02, 0x40), // no button 3
  INP0( UNKNOWN, 0x03, 0x40), // no button 3
  END_INPUT
};

static struct INPUT_INFO input_cps1_1b[] =
{
  INCL_INP( cps1_2b ),

  INP0( UNKNOWN, 0x02, 0x20), // no button 2
  INP0( UNKNOWN, 0x03, 0x20), // no button 2
  END_INPUT
};

static struct INPUT_INFO input_cps1_3players[] =
{
  INCL_INP( cps1_2b ),

  INP0( P3_RIGHT, 0x04, 0x01),
  INP0( P3_LEFT, 0x04, 0x02),
  INP0( P3_DOWN, 0x04, 0x04),
  INP0( P3_UP, 0x04, 0x08),
  INP0( P3_B1, 0x04, 0x10),
  INP0( P3_B2, 0x04, 0x20),
#ifndef RAINE_DOS
  INP0( P3_B1B2, 4, 0x30),
#endif
  INP0( COIN3, 0x04, 0x40),
  INP0( P3_START, 0x04, 0x80),
  END_INPUT
};

static struct INPUT_INFO input_cps1_4players[] =
{
  INCL_INP( cps1_3players ),

  INP0( P4_RIGHT, 0x06, 0x01),
  INP0( P4_LEFT, 0x06, 0x02),
  INP0( P4_DOWN, 0x06, 0x04),
  INP0( P4_UP, 0x06, 0x08),
  INP0( P4_B1, 0x06, 0x10),
  INP0( P4_B2, 0x06, 0x20),
#ifndef RAINE_DOS
  INP0( P4_B1B2, 6, 0x30),
#endif
  INP0( COIN4, 0x06, 0x40),
  INP0( P4_START, 0x06, 0x80),
  END_INPUT
};

static struct INPUT_INFO input_cps1_6b[] =
{
  INCL_INP( cps1_3b ),

  { KB_DEF_P1_B1, "P1 Jab Punch", 0x02, 0x10, BIT_ACTIVE_0 },
  { KB_DEF_P1_B2, "P1 Strong Punch", 0x02, 0x20, BIT_ACTIVE_0 },
  { KB_DEF_P1_B3, "P1 Fierce Punch", 0x02, 0x40, BIT_ACTIVE_0 },
  { KB_DEF_P2_B1, "P2 Jab Punch", 0x03, 0x10, BIT_ACTIVE_0 },
  { KB_DEF_P2_B2, "P2 Strong Punch", 0x03, 0x20, BIT_ACTIVE_0 },
  { KB_DEF_P2_B3, "P2 Fierce Punch", 0x03, 0x40, BIT_ACTIVE_0 },
#ifndef RAINE_DOS
  { KB_DEF_P1_B1B2B3, "P1 3P", 0x02, 0x70, BIT_ACTIVE_0 },
  { KB_DEF_P2_B1B2B3, "P2 3P", 0x03, 0x70, BIT_ACTIVE_0 },
#endif

  { KB_DEF_P1_B4, "P1 Short Kick", 0x04, 0x01, BIT_ACTIVE_0 },
  { KB_DEF_P1_B5, "P1 Forward Kick", 0x04, 0x02, BIT_ACTIVE_0 },
  { KB_DEF_P1_B6, "P1 Roundhouse Kick", 0x04, 0x04, BIT_ACTIVE_0 },
  INP0( UNKNOWN, 0x04, 0x08),
  { KB_DEF_P2_B4, "P2 Short Kick", 0x04, 0x10, BIT_ACTIVE_0 },
  { KB_DEF_P2_B5, "P2 Forward Kick", 0x04, 0x20, BIT_ACTIVE_0 },
  { KB_DEF_P2_B6, "P2 Roundhouse Kick", 0x04, 0x40, BIT_ACTIVE_0 },
  INP0( UNKNOWN, 0x04, 0x80),
  END_INPUT
};

static struct INPUT_INFO input_cps1_quiz[] =
{
  INCL_INP( cps1_3b ),

  INP0( UNKNOWN, 0x02, 0x01),
  INP0( UNKNOWN, 0x02, 0x02),
  INP0( UNKNOWN, 0x02, 0x04),
  INP0( UNKNOWN, 0x02, 0x08),
  INP0( P1_B4, 0x02, 0x80),
  INP0( UNKNOWN, 0x03, 0x01),
  INP0( UNKNOWN, 0x03, 0x02),
  INP0( UNKNOWN, 0x03, 0x04),
  INP0( UNKNOWN, 0x03, 0x08),
  INP0( P2_B4, 0x03, 0x80),
  END_INPUT
};

static struct INPUT_INFO input_forgottn[] =
{
  INCL_INP( cps1_1b ),

  INP0( UNKNOWN, 0x00, 0x40),
  INP0( B1_L, 0x02, 0x20 ), // change p1b2 to aim left (up)
  INP0( B1_R, 0x02, 0x40 ), // p1b3 = aim right (down)

  END_INPUT
};

static struct INPUT_INFO input_ghouls[] =
{
  INCL_INP( cps1_2b ),
  END_INPUT
};

static struct INPUT_INFO input_ghoulsu[] =
{
  INCL_INP( ghouls ),
  END_INPUT
};

static struct INPUT_INFO input_daimakai[] =
{
  INCL_INP( ghouls ),
  END_INPUT
};

static struct INPUT_INFO input_strider[] =
{
  INCL_INP( cps1_3b ),
  END_INPUT
};

static struct INPUT_INFO input_stridrua[] =
{
  INCL_INP( strider ),
  END_INPUT
};

static struct INPUT_INFO input_dynwar[] =
{
  INCL_INP( cps1_3b ),
  END_INPUT
};

static struct INPUT_INFO input_willow[] =
{
  INCL_INP( cps1_3b ),
  END_INPUT
};

static struct INPUT_INFO input_unsquad[] =
{
  INCL_INP( cps1_3b ),
  END_INPUT
};

static struct INPUT_INFO input_ffight[] =
{
  INCL_INP( cps1_3b ),

  END_INPUT
};

static struct INPUT_INFO input_1941[] =
{
  INCL_INP( cps1_2b ),
  END_INPUT
};

static struct INPUT_INFO input_mercs[] =
{
  INCL_INP( cps1_3players ),

  INP0( UNKNOWN, 0x00, 0x40),
  END_INPUT
};

static struct INPUT_INFO input_mtwins[] =
{
  INCL_INP( cps1_3b ),
  END_INPUT
};

static struct INPUT_INFO input_msword[] =
{
  INCL_INP( cps1_3b ),
  END_INPUT
};

static struct INPUT_INFO input_cawing[] =
{
  INCL_INP( cps1_3b ),
  END_INPUT
};

static struct INPUT_INFO input_nemo[] =
{
  INCL_INP( cps1_3b ),
  END_INPUT
};

static struct INPUT_INFO input_sf2[] =
{
  INCL_INP( cps1_6b ),
  END_INPUT
};

static struct INPUT_INFO input_sf2hack[] =
{
  INCL_INP( sf2 ),

  INP0( UNKNOWN, 0x04, 0xff),
  { KB_DEF_P1_B4, "P1 Short Kick", 0x05, 0x01, BIT_ACTIVE_0 },
  { KB_DEF_P1_B5, "P1 Forward Kick", 0x05, 0x02, BIT_ACTIVE_0 },
  { KB_DEF_P1_B6, "P1 Roundhouse Kick", 0x05, 0x04, BIT_ACTIVE_0 },
  INP0( UNKNOWN, 0x05, 0x08),
  { KB_DEF_P2_B4, "P2 Short Kick", 0x05, 0x10, BIT_ACTIVE_0 },
  { KB_DEF_P2_B5, "P2 Forward Kick", 0x05, 0x20, BIT_ACTIVE_0 },
  { KB_DEF_P2_B6, "P2 Roundhouse Kick", 0x05, 0x40, BIT_ACTIVE_0 },
  INP0( UNKNOWN, 0x05, 0x80),
  END_INPUT
};

static struct INPUT_INFO input_sf2m2[] =
{
  INCL_INP( sf2hack ),
  INP1( UNUSED, 0x1c, 0x10), // must be low !
  END_INPUT
};

static struct INPUT_INFO input_sf2m4[] =
{
  INCL_INP( sf2hack ),
  END_INPUT
};

static struct INPUT_INFO input_3wonders[] =
{
  INCL_INP( cps1_3b ),
  END_INPUT
};

static struct INPUT_INFO input_kod[] =
{
  INCL_INP( cps1_3players ),
  END_INPUT
};

static struct INPUT_INFO input_captcomm[] =
{
  INCL_INP( cps1_4players ),
  END_INPUT
};

static struct INPUT_INFO input_knights[] =
{
  INCL_INP( cps1_3players ),
  END_INPUT
};

static struct INPUT_INFO input_varth[] =
{
  INCL_INP( cps1_3b ),
  END_INPUT
};

static struct INPUT_INFO input_cworld2j[] =
{
  INCL_INP( cps1_quiz ),

  INP0( UNKNOWN, 0x08, 0xff),
  END_INPUT
};

static struct INPUT_INFO input_wof[] =
{
  INCL_INP( cps1_3players ),

  END_INPUT
};

static struct INPUT_INFO input_dino[] =
{
  INCL_INP( cps1_3players ),

  END_INPUT
};

static struct INPUT_INFO input_dinoh[] =
{
  INCL_INP( dino ),

  INP0( P1_RIGHT, 0x02, 0x01),
  INP0( P1_LEFT, 0x02, 0x02),
  INP0( P1_DOWN, 0x02, 0x04),
  INP0( P1_UP, 0x02, 0x08),
  INP0( P1_B1, 0x02, 0x10),
  INP0( P1_B2, 0x02, 0x20),
  INP0( P1_B3, 0x02, 0x40),
  INP0( UNKNOWN, 0x02, 0x80),
  INP0( P2_RIGHT, 0x03, 0x01),
  INP0( P2_LEFT, 0x03, 0x02),
  INP0( P2_DOWN, 0x03, 0x04),
  INP0( P2_UP, 0x03, 0x08),
  INP0( P2_B1, 0x03, 0x10),
  INP0( P2_B2, 0x03, 0x20),
  INP0( P2_B3, 0x03, 0x40),
  INP0( UNKNOWN, 0x03, 0x80),

  INP0( P3_RIGHT, 0x04, 0x01),
  INP0( P3_LEFT, 0x04, 0x02),
  INP0( P3_DOWN, 0x04, 0x04),
  INP0( P3_UP, 0x04, 0x08),
  INP0( P3_B1, 0x04, 0x10),
  INP0( P3_B2, 0x04, 0x20),
  INP0( P3_B3, 0x04, 0x80), //  PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(3)
  INP0( COIN3, 0x04, 0x40),
  INP0( P3_START, 0x04, 0x80),
  END_INPUT
};

static struct INPUT_INFO input_punisher[] =
{
  INCL_INP( cps1_2b ),

  INP0( UNKNOWN, 0x04, 0xff), // no player 3
  INP0( UNKNOWN, 0x06, 0xff), // no player 4

  END_INPUT
};

static struct INPUT_INFO input_punisherbz[] =
{
  INCL_INP( punisher ),

  INP0( UNKNOWN, 0x06, 0xff),

  INP0( P1_RIGHT, 0x02, 0x01),
  INP0( P1_LEFT, 0x02, 0x02),
  INP0( P1_DOWN, 0x02, 0x04),
  INP0( P1_UP, 0x02, 0x08),
  INP0( P1_B1, 0x02, 0x10),
  INP0( P1_B2, 0x02, 0x20),
  INP0( P1_B3, 0x02, 0x40),
  INP0( UNKNOWN, 0x02, 0x80),
  INP0( P2_RIGHT, 0x03, 0x01),
  INP0( P2_LEFT, 0x03, 0x02),
  INP0( P2_DOWN, 0x03, 0x04),
  INP0( P2_UP, 0x03, 0x08),
  INP0( P2_B1, 0x03, 0x10),
  INP0( P2_B2, 0x03, 0x20),
  INP0( P2_B3, 0x03, 0x40),
  INP0( UNKNOWN, 0x03, 0x80),
  END_INPUT
};

static struct INPUT_INFO input_slammast[] =
{
  INCL_INP( cps1_4players ),

  INP0( P1_B3, 0x02, 0x40),
  INP0( P3_B3, 0x02, 0x80),
  INP0( P2_B3, 0x03, 0x40),
  INP0( P4_B3, 0x03, 0x80),

  END_INPUT
};

static struct INPUT_INFO input_pnickj[] =
{
  INCL_INP( cps1_3b ),
  END_INPUT
};

static struct INPUT_INFO input_qad[] =
{
  INCL_INP( cps1_quiz ),

  INP0( UNKNOWN, 0x08, 0xff),
  END_INPUT
};

static struct INPUT_INFO input_qadjr[] =
{
  INCL_INP( qad ),

  INP0( UNKNOWN, 0x08, 0xff),

  INP0( UNKNOWN, 0x0a, 0xff),
  END_INPUT
};

static struct INPUT_INFO input_qtono2j[] =
{
  INCL_INP( cps1_quiz ),

  INP0( UNKNOWN, 0x08, 0xff),

  INP0( UNKNOWN, 0x0a, 0xff),
  END_INPUT
};

static struct INPUT_INFO input_pang3[] =
{
  INCL_INP( cps1_3b ),

  END_INPUT
};

static struct INPUT_INFO input_megaman[] =
{
  INCL_INP( cps1_3b ),
  END_INPUT
};

static struct INPUT_INFO input_rockmanj[] =
{
  INCL_INP( megaman ),
  END_INPUT
};

static struct INPUT_INFO input_wofhfh[] =
{
  INCL_INP( wof ),

  INP0( UNKNOWN, 0x06, 0xff),

  INP0( P1_B3, 0x02, 0x40),
  INP0( P2_B3, 0x03, 0x40),

  { KB_DEF_COIN3, "Coin 3 (P3 Button 3 in-game", 0x04, 0x40, BIT_ACTIVE_0 },
  END_INPUT
};

static struct INPUT_INFO input_ganbare[] =
{
  INP0( COIN1, 0x00, 0x01),
  INP0( COIN2, 0x00, 0x02),
  INP0( SERVICE, 0x00, 0x04),
  INP0( UNKNOWN, 0x00, 0x08),
  INP0( P1_START, 0x00, 0x10),
  INP0( P2_START, 0x00, 0x20),

  INP0( UNKNOWN, 0x00, 0x80),

  INP0( P1_RIGHT, 0x02, 0x01),
  INP0( P1_LEFT, 0x02, 0x02),
  INP0( UNKNOWN, 0x02, 0x04),
  INP0( UNKNOWN, 0x02, 0x08),
  INP0( P1_B1, 0x02, 0x10),
  INP0( P1_B2, 0x02, 0x20),
  INP0( UNKNOWN, 0x02, 0x40),
  INP0( UNKNOWN, 0x02, 0x80),
  INP0( UNKNOWN, 0x03, 0x01),
  INP0( UNKNOWN, 0x03, 0x02),
  INP0( UNKNOWN, 0x03, 0x04),
  INP0( UNKNOWN, 0x03, 0x08),
  INP0( UNKNOWN, 0x03, 0x10),
  INP0( UNKNOWN, 0x03, 0x20),
  INP0( UNKNOWN, 0x03, 0x40),
  INP0( UNKNOWN, 0x03, 0x80),
  END_INPUT
};

static struct INPUT_INFO input_sfzch[] =
{
  INP0( P1_B5, 0x00, 0x01),
  INP0( P2_B5, 0x00, 0x02),
  { KB_DEF_SERVICE, "Pause", 0x00, 0x04, BIT_ACTIVE_0 },
  INP0( SERVICE, 0x00, 0x08),
  INP0( P1_START, 0x00, 0x10),
  INP0( P2_START, 0x00, 0x20),
  INP0( P1_B6, 0x00, 0x40),
  INP0( P2_B6, 0x00, 0x80),

  INP0( P1_RIGHT, 0x02, 0x01),
  INP0( P1_LEFT, 0x02, 0x02),
  INP0( P1_DOWN, 0x02, 0x04),
  INP0( P1_UP, 0x02, 0x08),
  INP0( P1_B1, 0x02, 0x10),
  INP0( P1_B2, 0x02, 0x20),
  INP0( P1_B3, 0x02, 0x40),
  INP0( P1_B4, 0x02, 0x80),

  INP0( P2_RIGHT, 0x03, 0x01),
  INP0( P2_LEFT, 0x03, 0x02),
  INP0( P2_DOWN, 0x03, 0x04),
  INP0( P2_UP, 0x03, 0x08),
  INP0( P2_B1, 0x03, 0x10),
  INP0( P2_B2, 0x03, 0x20),
  INP0( P2_B3, 0x03, 0x40),
  INP0( P2_B4, 0x03, 0x80),

  END_INPUT
};

static struct INPUT_INFO input_wofch[] =
{
  INCL_INP( sfzch ),


  END_INPUT
};

// Stuff...

static struct GFX_LIST cps1_gfx[] =
{
  { REGION_GFX1, NULL }, //&cps1_object, },
   { 0,           NULL,               },
};

// in fact the video is 512x256 with a border of 16 pixels in height
// and 64 pixels for the width !
struct VIDEO_INFO video_cps1 =
{
   draw_cps1,
   384,
   224,
   32,
   VIDEO_ROTATE_NORMAL |
   VIDEO_ROTATABLE,
   cps1_gfx,
   59.64 // Verified by mame team...
};

struct VIDEO_INFO video_cps1_270 =
{
   draw_cps1,
   384,
   224,
   32,
   VIDEO_ROTATE_270 |
   VIDEO_ROTATABLE,
   cps1_gfx,
   59.64
};

extern void cps1_irq_handler_mus(int irq);

static struct YM2151interface ym2151_interface =
{
	1,  /* 1 chip */
	3579580,    /* 3.579580 MHz ? */
	{ YM3012_VOL(150,MIXER_PAN_LEFT,150,MIXER_PAN_RIGHT) },
	{ z80_irq_handler }
};

static struct OKIM6295interface okim6295_interface_6061 =
{
	1,  /* 1 chip */
	{ 6061 },
	{ REGION_SMP1 },
	{ 100 }
};

static struct OKIM6295interface okim6295_interface_7576 =
{
	1,  /* 1 chip */
	{ 7576 },
	{ REGION_SMP1 },
	{ 100 }
};

static struct SOUND_INFO sound_cps1[] =
{
   { SOUND_YM2151J,  &ym2151_interface,    },
   { SOUND_M6295, &okim6295_interface_7576     },
   { 0,              NULL,                 },
};

static struct QSound_interface qsound_interface =
{
	QSOUND_CLOCK,
	REGION_SMP1,
	{ 250,250 }
};

struct SOUND_INFO sound_qsound[2] =
{
   { SOUND_QSOUND, &qsound_interface     },
   { 0,              NULL,                 },
};

static struct SOUND_INFO sound_forgottn[] =
{
   { SOUND_YM2151J,  &ym2151_interface,    },
   { SOUND_M6295, &okim6295_interface_6061     },
   { 0,              NULL,                 },
};

#define dsw_sfzch NULL
#define dsw_sf2ce dsw_sf2
#define dsw_mbombrd dsw_slammast

static struct ROMSW_DATA romswd_sfzch[] =
{
  { "Japan", 0x0},
  { "USA",0x2},
  { "Hispanic", 0x4},
  { "Asia/Brazil",0x6},
  { NULL,                    0    },
};

static struct ROMSW_INFO romsw_sfzch[] =
{
   { 0xf41, 0x02, romswd_sfzch },
   { 0xf47, 0x02, romswd_sfzch },
   { 0,        0,    NULL },
};

GMEI( forgottn, "Forgotten Worlds (World)", CAPCOM, 1988, GAME_SHOOT,
	.sound = sound_forgottn);
CLNEI( forgottnu, forgottn, "Forgotten Worlds (USA, B-Board 88621B-2, Rev. C)", CAPCOM, 1988, GAME_SHOOT);
CLNEI( forgottnu1, forgottn, "Forgotten Worlds (USA, B-Board 88618B-2, Rev. C)", CAPCOM, 1988, GAME_SHOOT);
CLNEI( forgottnua, forgottn, "Forgotten Worlds (USA, B-Board 88618B-2, Rev. A)", CAPCOM, 1988, GAME_SHOOT);
CLNEI( forgottnuaa, forgottn, "Forgotten Worlds (USA, B-Board 88618B-2, Rev. AA)", CAPCOM, 1988, GAME_SHOOT);
CLNEI( lostwrld, forgottn, "Lost Worlds (Japan)", CAPCOM, 1988, GAME_SHOOT);
CLNEI( lostwrldo, forgottn, "Lost Worlds (Japan Old Ver.)", CAPCOM, 1988, GAME_SHOOT);
GMEI( ghouls, "Ghouls'n Ghosts (World)", CAPCOM, 1988, GAME_BEAT);
CLNEI( ghoulsu, ghouls, "Ghouls'n Ghosts (USA)", CAPCOM, 1988, GAME_BEAT,
	.dsw = dsw_ghoulsu, .input = input_ghoulsu);
CLNEI( daimakai, ghouls, "Daimakaimura (Japan)", CAPCOM, 1988, GAME_BEAT);
CLNEI( daimakair, ghouls, "Daimakaimura (Japan Resale Ver.)", CAPCOM, 1988, GAME_BEAT,
  .input = input_daimakai, .dsw = dsw_daimakai,
  .load_game = load_cps1_12);
GMEI( strider, "Strider (USA, B-Board 89624B-2)", CAPCOM, 1989, GAME_BEAT);
CLNEI( striderua, strider, "Strider (USA, B-Board 89624B-3)", CAPCOM, 1989, GAME_BEAT,
  .input = input_stridrua, .dsw = dsw_stridrua);
CLNEI( striderj, strider, "Strider Hiryu (Japan)", CAPCOM, 1989, GAME_BEAT);
CLNEI( striderjr, strider, "Strider Hiryu (Japan Resale Ver.)", CAPCOM, 1989, GAME_BEAT,
  .load_game = load_cps1_12);
GMEI( dynwar, "Dynasty Wars (USA, B-Board 89624B-?)", CAPCOM, 1989, GAME_BEAT);
CLNEI( dynwara, dynwar, "Dynasty Wars (USA, B-Board 88622B-3)", CAPCOM, 1989, GAME_BEAT);
CLNEI( dynwarj, dynwar, "Tenchi wo Kurau (Japan)", CAPCOM, 1989, GAME_BEAT);
CLNEI( dynwarjr, dynwar, "Tenchi wo Kurau (Japan Resale Ver.)", CAPCOM, 1989, GAME_BEAT,
  .load_game = load_cps1_12);
GMEI( willow, "Willow (USA)", CAPCOM, 1989, GAME_SHOOT);
CLNEI( willowo, willow, "Willow (USA Old Ver.)", CAPCOM, 1989, GAME_SHOOT);
CLNEI( willowj, willow, "Willow (Japan)", CAPCOM, 1989, GAME_SHOOT);
GMEI( unsquad, "U.N. Squadron (USA)", CAPCOM, 1989, GAME_SHOOT);
CLNEI( area88, unsquad, "Area 88 (Japan)", CAPCOM, 1989, GAME_SHOOT);
CLNEI( area88r, unsquad, "Area 88 (Japan Resale Ver.)", CAPCOM, 1989, GAME_SHOOT,
  .load_game = load_cps1_12);
GMEI( ffight, "Final Fight (World, set 1)", CAPCOM, 1989, GAME_BEAT);
CLNEI( ffighta, ffight, "Final Fight (World, set 2)", CAPCOM, 1989, GAME_BEAT);
CLNEI( ffightu, ffight, "Final Fight (USA, set 1)", CAPCOM, 1989, GAME_BEAT);
CLNEI( ffightu1, ffight, "Final Fight (USA, set 2)", CAPCOM, 1989, GAME_BEAT);
CLNEI( ffightua, ffight, "Final Fight (USA 900112)", CAPCOM, 1989, GAME_BEAT);
CLNEI( ffightub, ffight, "Final Fight (USA 900613)", CAPCOM, 1989, GAME_BEAT);
CLNEI( ffightj, ffight, "Final Fight (Japan)", CAPCOM, 1989, GAME_BEAT);
CLNEI( ffightj1, ffight, "Final Fight (Japan 900112)", CAPCOM, 1989, GAME_BEAT);
CLNEI( ffightj2, ffight, "Final Fight (Japan 900305)", CAPCOM, 1989, GAME_BEAT);
CLNEI( ffightjh, ffight, "Street Smart / Final Fight (Japan, hack)", BOOTLEG, 1989, GAME_BEAT);
GMEI( 1941, "1941: Counter Attack (World 900227)", CAPCOM, 1990, GAME_SHOOT,
  .video = &video_cps1_270);
CLNEI( 1941r1, 1941, "1941: Counter Attack (World)", CAPCOM, 1990, GAME_SHOOT,
  .video = &video_cps1_270);
CLNEI( 1941u, 1941, "1941: Counter Attack (USA 900227)", CAPCOM, 1990, GAME_SHOOT,
  .video = &video_cps1_270);
CLNEI( 1941j, 1941, "1941: Counter Attack (Japan)", CAPCOM, 1990, GAME_SHOOT,
  .video = &video_cps1_270);
GMEI( mercs, "Mercs (World 900302)", CAPCOM, 1990, GAME_SHOOT,
  .video = &video_cps1_270);
CLNEI( mercsu, mercs, "Mercs (USA 900608)", CAPCOM, 1990, GAME_SHOOT,
  .video = &video_cps1_270);
CLNEI( mercsur1, mercs, "Mercs (USA 900302)", CAPCOM, 1990, GAME_SHOOT,
  .video = &video_cps1_270);
CLNEI( mercsj, mercs, "Senjou no Ookami II (Japan 900302)", CAPCOM, 1990, GAME_SHOOT,
  .video = &video_cps1_270);
GMEI( mtwins, "Mega Twins (World 900619)", CAPCOM, 1990, GAME_BEAT);
CLNEI( chikij, mtwins, "Chiki Chiki Boys (Japan 900619)", CAPCOM, 1990, GAME_BEAT);
GMEI( msword, "Magic Sword: Heroic Fantasy (World 900725)", CAPCOM, 1990, GAME_BEAT);
CLNEI( mswordr1, msword, "Magic Sword: Heroic Fantasy (World 900623)", CAPCOM, 1990, GAME_BEAT);
CLNEI( mswordu, msword, "Magic Sword: Heroic Fantasy (USA 900725)", CAPCOM, 1990, GAME_BEAT);
CLNEI( mswordj, msword, "Magic Sword: Heroic Fantasy (Japan 900623)", CAPCOM, 1990, GAME_BEAT);
GMEI( cawing, "Carrier Air Wing (World 901012)", CAPCOM, 1990, GAME_SHOOT);
CLNEI( cawingr1, cawing, "Carrier Air Wing (World 901009)", CAPCOM, 1990, GAME_SHOOT);
CLNEI( cawingu, cawing, "Carrier Air Wing (USA 901012)", CAPCOM, 1990, GAME_SHOOT);
CLNEI( cawingj, cawing, "U.S. Navy (Japan 901012)", CAPCOM, 1990, GAME_SHOOT);
GMEI( nemo, "Nemo (World 901130)", CAPCOM, 1990, GAME_BEAT);
CLNEI( nemoj, nemo, "Nemo (Japan 901120)", CAPCOM, 1990, GAME_BEAT);
GMEI( sf2, "Street Fighter II: The World Warrior (World 910522)", CAPCOM, 1991, GAME_BEAT);
CLNEI( sf2eb, sf2, "Street Fighter II: The World Warrior (World 910214)", CAPCOM, 1991, GAME_BEAT);
CLNEI( sf2ee, sf2, "Street Fighter II: The World Warrior (World 910228)", CAPCOM, 1991, GAME_BEAT);
CLNEI( sf2ua, sf2, "Street Fighter II: The World Warrior (USA 910206)", CAPCOM, 1991, GAME_BEAT);
CLNEI( sf2ub, sf2, "Street Fighter II: The World Warrior (USA 910214)", CAPCOM, 1991, GAME_BEAT);
CLNEI( sf2uc, sf2, "Street Fighter II: The World Warrior (USA 910306)", CAPCOM, 1991, GAME_BEAT);
CLNEI( sf2ud, sf2, "Street Fighter II: The World Warrior (USA 910318)", CAPCOM, 1991, GAME_BEAT);
CLNEI( sf2ue, sf2, "Street Fighter II: The World Warrior (USA 910228)", CAPCOM, 1991, GAME_BEAT);
CLNEI( sf2uf, sf2, "Street Fighter II: The World Warrior (USA 910411)", CAPCOM, 1991, GAME_BEAT);
CLNEI( sf2ug, sf2, "Street Fighter II: The World Warrior (USA 910522, Rev. G)", CAPCOM, 1991, GAME_BEAT);
CLNEI( sf2ui, sf2, "Street Fighter II: The World Warrior (USA 910522, Rev. I)", CAPCOM, 1991, GAME_BEAT);
CLNEI( sf2uk, sf2, "Street Fighter II: The World Warrior (USA 911101)", CAPCOM, 1991, GAME_BEAT);
CLNEI( sf2j, sf2, "Street Fighter II: The World Warrior (Japan 911210)", CAPCOM, 1991, GAME_BEAT);
CLNEI( sf2ja, sf2, "Street Fighter II: The World Warrior (Japan 910214)", CAPCOM, 1991, GAME_BEAT,
  .dsw = dsw_sf2j);
CLNEI( sf2jc, sf2, "Street Fighter II: The World Warrior (Japan 910306)", CAPCOM, 1991, GAME_BEAT,
  .dsw = dsw_sf2j);
CLNEI( sf2ebbl, sf2, "Street Fighter II: The World Warrior (TAB Austria, bootleg, set 1)", BOOTLEG, 1992, GAME_BEAT,
  .input = input_sf2hack);
CLNEI( sf2qp1, sf2, "Street Fighter II: The World Warrior (Quicken Pt-I, bootleg)", BOOTLEG, 1991, GAME_BEAT);
CLNEI( sf2thndr, sf2, "Street Fighter II: The World Warrior (Thunder Edition, bootleg)", BOOTLEG, 1991, GAME_BEAT);
GMEI( 3wonders, "Three Wonders (World 910520)", CAPCOM, 1991, GAME_SHOOT|GAME_PUZZLE|GAME_PLATFORM);
CLNEI( 3wondersr1, 3wonders, "Three Wonders (World 910513)", CAPCOM, 1991, GAME_SHOOT|GAME_PUZZLE|GAME_PLATFORM);
CLNEI( 3wondersu, 3wonders, "Three Wonders (USA 910520)", CAPCOM, 1991, GAME_SHOOT|GAME_PUZZLE|GAME_PLATFORM);
CLNEI( wonder3, 3wonders, "Wonder 3 (Japan 910520)", CAPCOM, 1991, GAME_SHOOT|GAME_PUZZLE|GAME_PLATFORM);
CLNEI( 3wondersh, 3wonders, "Three Wonders (hack)", BOOTLEG, 1991, GAME_SHOOT|GAME_PUZZLE|GAME_PLATFORM);
GMEI( kod, "The King of Dragons (World 910805)", CAPCOM, 1991, GAME_BEAT);
CLNEI( kodr1, kod, "The King of Dragons (World 910711)", CAPCOM, 1991, GAME_BEAT);
CLNEI( kodu, kod, "The King of Dragons (USA 910910)", CAPCOM, 1991, GAME_BEAT);
CLNEI( kodj, kod, "The King of Dragons (Japan 910805, B-Board 90629B-3)", CAPCOM, 1991, GAME_BEAT,
	.dsw = dsw_kodj);
CLNEI( kodja, kod, "The King of Dragons (Japan 910805, B-Board 89625B-1)", CAPCOM, 1991, GAME_BEAT);
GMEI( captcomm, "Captain Commando (World 911202)", CAPCOM, 1991, GAME_BEAT);
CLNEI( captcommr1, captcomm, "Captain Commando (World 911014)", CAPCOM, 1991, GAME_BEAT);
CLNEI( captcommu, captcomm, "Captain Commando (USA 910928)", CAPCOM, 1991, GAME_BEAT);
CLNEI( captcommj, captcomm, "Captain Commando (Japan 911202)", CAPCOM, 1991, GAME_BEAT);
CLNEI( captcommjr1, captcomm, "Captain Commando (Japan 910928)", CAPCOM, 1991, GAME_BEAT);
CLNEI( captcommb, captcomm, "Captain Commando (bootleg)", BOOTLEG, 1991, GAME_BEAT | GAME_NOT_WORKING);
GMEI( knights, "Knights of the Round (World 911127)", CAPCOM, 1991, GAME_BEAT);
CLNEI( knightsu, knights, "Knights of the Round (USA 911127)", CAPCOM, 1991, GAME_BEAT);
CLNEI( knightsj, knights, "Knights of the Round (Japan 911127, B-Board 91634B-2)", CAPCOM, 1991, GAME_BEAT);
CLNEI( knightsja, knights, "Knights of the Round (Japan 911127, B-Board 89625B-1)", CAPCOM, 1991, GAME_BEAT);

#define input_sf2ce NULL
GMEI( sf2ce, "Street Fighter II': Champion Edition (World 920513)", CAPCOM, 1992, GAME_BEAT,
  .input = input_sf2, .dsw = dsw_sf2,
  .load_game = load_cps1_12);
CLNEI( sf2ceea, sf2ce, "Street Fighter II': Champion Edition (World 920313)", CAPCOM, 1992, GAME_BEAT,
  .input = input_sf2, .dsw = dsw_sf2,
  .load_game = load_cps1_12);
CLNEI( sf2ceua, sf2ce, "Street Fighter II': Champion Edition (USA 920313)", CAPCOM, 1992, GAME_BEAT,
  .input = input_sf2, .dsw = dsw_sf2,
  .load_game = load_cps1_12);
CLNEI( sf2ceub, sf2ce, "Street Fighter II': Champion Edition (USA 920513)", CAPCOM, 1992, GAME_BEAT,
  .input = input_sf2, .dsw = dsw_sf2,
  .load_game = load_cps1_12);
CLNEI( sf2ceuc, sf2ce, "Street Fighter II': Champion Edition (USA 920803)", CAPCOM, 1992, GAME_BEAT,
  .input = input_sf2, .dsw = dsw_sf2,
  .load_game = load_cps1_12);
CLNEI( sf2ceja, sf2ce, "Street Fighter II': Champion Edition (Japan 920322)", CAPCOM, 1992, GAME_BEAT,
  .input = input_sf2, .dsw = dsw_sf2,
  .load_game = load_cps1_12);
CLNEI( sf2cejb, sf2ce, "Street Fighter II': Champion Edition (Japan 920513)", CAPCOM, 1992, GAME_BEAT,
  .input = input_sf2, .dsw = dsw_sf2,
  .load_game = load_cps1_12);
CLNEI( sf2cejc, sf2ce, "Street Fighter II': Champion Edition (Japan 920803)", CAPCOM, 1992, GAME_BEAT,
  .input = input_sf2, .dsw = dsw_sf2,
  .load_game = load_cps1_12);
CLNEI( sf2rb, sf2ce, "Street Fighter II': Champion Edition (Rainbow, bootleg, set 1)", BOOTLEG, 1992, GAME_BEAT,
  .input = input_sf2, .dsw = dsw_sf2,
  .load_game = load_cps1_12);
CLNEI( sf2rb2, sf2ce, "Street Fighter II': Champion Edition (Rainbow, bootleg, set 2)", BOOTLEG, 1992, GAME_BEAT,
  .input = input_sf2, .dsw = dsw_sf2,
  .load_game = load_cps1_12);
CLNEI( sf2rb3, sf2ce, "Street Fighter II': Champion Edition (Rainbow, bootleg, set 3)", BOOTLEG, 1992, GAME_BEAT,
  .input = input_sf2, .dsw = dsw_sf2,
  .load_game = load_cps1_12);
CLNEI( sf2red, sf2ce, "Street Fighter II': Champion Edition (Red Wave, bootleg)", BOOTLEG, 1992, GAME_BEAT,
  .input = input_sf2, .dsw = dsw_sf2,
  .load_game = load_cps1_12);
CLNEI( sf2v004, sf2ce, "Street Fighter II': Champion Edition (V004, bootleg)", BOOTLEG, 1992, GAME_BEAT,
  .input = input_sf2, .dsw = dsw_sf2,
  .load_game = load_cps1_12);
CLNEI( sf2acc, sf2ce, "Street Fighter II': Champion Edition (Accelerator!, bootleg, set 1)", BOOTLEG, 1992, GAME_BEAT,
  .input = input_sf2, .dsw = dsw_sf2,
  .load_game = load_cps1_12);
CLNEI( sf2acca, sf2ce, "Street Fighter II': Champion Edition (Accelerator!, bootleg, set 2)", BOOTLEG, 1992, GAME_BEAT,
  .input = input_sf2, .dsw = dsw_sf2,
  .load_game = load_cps1_12);
CLNEI( sf2accp2, sf2ce, "Street Fighter II': Champion Edition (Accelerator Pt.II, bootleg)", BOOTLEG, 1992, GAME_BEAT,
  .input = input_sf2, .dsw = dsw_sf2,
  .load_game = load_cps1_12);
CLNEI( sf2dkot2, sf2ce, "Street Fighter II': Champion Edition (Double K.O. Turbo II, bootleg)", BOOTLEG, 1992, GAME_BEAT,
  .input = input_sf2, .dsw = dsw_sf2,
  .load_game = load_cps1_12);
CLNEI( sf2m2, sf2ce, "Street Fighter II': Champion Edition (M2, bootleg)", BOOTLEG, 1992, GAME_BEAT,
  .load_game = load_cps1_12,
  .input = input_sf2m2, .dsw = dsw_sf2m2);
CLNEI( sf2m3, sf2ce, "Street Fighter II': Champion Edition (M3, bootleg)", BOOTLEG, 1992, GAME_BEAT,
  .input = input_sf2hack,
  .load_game = load_sf2m3);
CLNEI( sf2m4, sf2ce, "Street Fighter II': Champion Edition (M4, bootleg)", BOOTLEG, 1992, GAME_BEAT,
  .load_game = load_cps1_12,
  .input = input_sf2m4);
CLNEI( sf2m5, sf2ce, "Street Fighter II': Champion Edition (M5, bootleg)", BOOTLEG, 1992, GAME_BEAT,
  .input = input_sf2hack,
  .load_game = load_cps1_12);
CLNEI( sf2m6, sf2ce, "Street Fighter II': Champion Edition (M6, bootleg)", BOOTLEG, 1992, GAME_BEAT,
  .input = input_sf2hack,
  .load_game = load_cps1_12);
CLNEI( sf2m7, sf2ce, "Street Fighter II': Champion Edition (M7, bootleg)", BOOTLEG, 1992, GAME_BEAT,
  .input = input_sf2hack,
  .load_game = load_cps1_12);
CLNEI( sf2m8, sf2ce, "Street Fighter II': Champion Edition (M8, bootleg)", BOOTLEG, 1992, GAME_BEAT,
  .input = input_sf2hack,
  .load_game = load_sf2m8);
CLNEI( sf2yyc, sf2ce, "Street Fighter II': Champion Edition (YYC, bootleg)", BOOTLEG, 1992, GAME_BEAT,
  .input = input_sf2hack,
  .load_game = load_cps1_12);
CLNEI( sf2koryu, sf2ce, "Street Fighter II': Champion Edition (Xiang Long, Chinese bootleg)", BOOTLEG, 1992, GAME_BEAT,
  .input = input_sf2hack,
  .load_game = load_cps1_12);
CLNEI( sf2dongb, sf2ce, "Street Fighter II': Champion Edition (Dongfang Bubai protection, bootleg)", BOOTLEG, 1992, GAME_BEAT,
  .input = input_sf2, .dsw = dsw_sf2,
  .load_game = load_cps1_12);
GMEI( cworld2j, "Adventure Quiz Capcom World 2 (Japan 920611)", CAPCOM, 1992, GAME_QUIZZ,
  .load_game = load_cps1_12);
GMEI( varth, "Varth: Operation Thunderstorm (World 920714)", CAPCOM, 1992, GAME_SHOOT,
  .video = &video_cps1_270,
  .load_game = load_cps1_12);
CLNEI( varthr1, varth, "Varth: Operation Thunderstorm (World 920612)", CAPCOM, 1992, GAME_SHOOT,
  .video = &video_cps1_270,
  .load_game = load_cps1_12);
CLNEI( varthu, varth, "Varth: Operation Thunderstorm (USA 920612)", CAPCOM, 1992, GAME_SHOOT,
  .video = &video_cps1_270,
  .load_game = load_cps1_12);
CLNEI( varthj, varth, "Varth: Operation Thunderstorm (Japan 920714)", CAPCOM, 1992, GAME_SHOOT,
  .video = &video_cps1_270,
  .load_game = load_cps1_12);
GMEI( qad, "Quiz & Dragons: Capcom Quiz Game (USA 920701)", CAPCOM, 1992, GAME_QUIZZ,
  .load_game = load_cps1_12);
CLNEI( qadjr, qad, "Quiz & Dragons: Capcom Quiz Game (Japan Resale Ver. 940921)", CAPCOM, 1994, GAME_QUIZZ,
  .load_game = load_cps1_12,
  .input = input_qadjr, .dsw = dsw_qadj);
GMEI( wof, "Warriors of Fate (World 921031)", CAPCOM, 1992, GAME_BEAT,
  .load_game = load_qsound, .sound = sound_qsound,
  .exec = execute_qsound_frame);
CLNEI( wofr1, wof, "Warriors of Fate (World 921002)", CAPCOM, 1992, GAME_BEAT,
  .load_game = load_qsound, .sound = sound_qsound,
  .exec = execute_qsound_frame);
CLNEI( wofu, wof, "Warriors of Fate (USA 921031)", CAPCOM, 1992, GAME_BEAT,
  .load_game = load_qsound, .sound = sound_qsound,
  .exec = execute_qsound_frame);
CLNEI( wofa, wof, "Sangokushi II (Asia 921005)", CAPCOM, 1992, GAME_BEAT,
  .load_game = load_qsound, .sound = sound_qsound,
  .exec = execute_qsound_frame);
CLNEI( wofj, wof, "Tenchi wo Kurau II: Sekiheki no Tatakai (Japan 921031)", CAPCOM, 1992, GAME_BEAT,
  .load_game = load_qsound, .sound = sound_qsound,
  .exec = execute_qsound_frame);
CLNEI( wofhfh, wof, "Huo Feng Huang (Chinese bootleg of Sangokushi II)", BOOTLEG, 1999, GAME_BEAT,
  .load_game = load_cps1_12,
  .input = input_wofhfh);
#define input_sf2hf NULL
#define dsw_sf2hf NULL
GMEI( sf2hf, "Street Fighter II': Hyper Fighting (World 921209)", CAPCOM, 1992, GAME_BEAT,
  .input = input_sf2, .dsw = dsw_sf2,
  .load_game = load_cps1_12);
CLNEI( sf2hfu, sf2hf, "Street Fighter II': Hyper Fighting (USA 921209)", CAPCOM, 1992, GAME_BEAT,
  .input = input_sf2, .dsw = dsw_sf2,
  .load_game = load_cps1_12);
CLNEI( sf2hfj, sf2hf, "Street Fighter II' Turbo: Hyper Fighting (Japan 921209)", CAPCOM, 1992, GAME_BEAT,
  .input = input_sf2, .dsw = dsw_sf2j,
  .load_game = load_cps1_12);
GMEI( dino, "Cadillacs and Dinosaurs (World 930201)", CAPCOM, 1993, GAME_BEAT,
  .load_game = load_qsound, .sound = sound_qsound,
  .exec = execute_qsound_frame);
CLNEI( dinou, dino, "Cadillacs and Dinosaurs (USA 930201)", CAPCOM, 1993, GAME_BEAT,
  .load_game = load_qsound, .sound = sound_qsound,
  .exec = execute_qsound_frame);
CLNEI( dinoj, dino, "Cadillacs: Kyouryuu Shin Seiki (Japan 930201)", CAPCOM, 1993, GAME_BEAT,
  .load_game = load_qsound, .sound = sound_qsound,
  .exec = execute_qsound_frame);
CLNEI( dinohunt, dino, "Dinosaur Hunter (Chinese bootleg of Cadillacs and Dinosaurs)", BOOTLEG, 1993, GAME_BEAT|GAME_NOT_WORKING,
  .input = input_dinoh, .dsw = dsw_dino,
  .load_game = load_cps1_12);
GMEI( punisher, "The Punisher (World 930422)", CAPCOM, 1993, GAME_BEAT,
  .load_game = load_qsound, .sound = sound_qsound,
  .exec = execute_qsound_frame);
CLNEI( punisheru, punisher, "The Punisher (USA 930422)", CAPCOM, 1993, GAME_BEAT,
  .load_game = load_qsound, .sound = sound_qsound,
  .exec = execute_qsound_frame);
CLNEI( punisherh, punisher, "The Punisher (Hispanic 930422)", CAPCOM, 1993, GAME_BEAT,
  .load_game = load_qsound, .sound = sound_qsound,
  .exec = execute_qsound_frame);
CLNEI( punisherj, punisher, "The Punisher (Japan 930422)", CAPCOM, 1993, GAME_BEAT,
  .load_game = load_qsound, .sound = sound_qsound,
  .exec = execute_qsound_frame);
CLNEI( punisherbz, punisher, "Biaofeng Zhanjing (Chinese bootleg of The Punisher)", BOOTLEG, 1993, GAME_BEAT,
  .load_game = load_cps1_12,
  .input = input_punisherbz);
GMEI( slammast, "Saturday Night Slam Masters (World 930713)", CAPCOM, 1993, GAME_SPORTS,
  .load_game = load_qsound, .sound = sound_qsound,
  .exec = execute_qsound_frame);
CLNEI( slammastu, slammast, "Saturday Night Slam Masters (USA 930713)", CAPCOM, 1993, GAME_SPORTS,
  .load_game = load_qsound, .sound = sound_qsound,
  .exec = execute_qsound_frame);
CLNEI( mbomberj, slammast, "Muscle Bomber: The Body Explosion (Japan 930713)", CAPCOM, 1993, GAME_SPORTS,
  .load_game = load_qsound, .sound = sound_qsound,
  .exec = execute_qsound_frame);
#define input_mbombrd NULL
GMEI( mbombrd, "Muscle Bomber Duo: Ultimate Team Battle (World 931206)", CAPCOM, 1993, GAME_SPORTS,
  .input = input_slammast, .dsw = dsw_slammast,
  .load_game = load_qsound, .sound = sound_qsound,
  .exec = execute_qsound_frame);
CLNEI( mbombrdj, mbombrd, "Muscle Bomber Duo: Heat Up Warriors (Japan 931206)", CAPCOM, 1993, GAME_SPORTS,
  .input = input_slammast, .dsw = dsw_slammast,
  .load_game = load_qsound, .sound = sound_qsound,
  .exec = execute_qsound_frame);
GMEI( pnickj, "Pnickies (Japan 940608)", CAPCOM, 1994, GAME_PUZZLE,
  .load_game = load_cps1_12);
#define dsw_qtono2j dsw_qtono2
GMEI( qtono2j, "Quiz Tonosama no Yabou 2: Zenkoku-ban (Japan 950123)", CAPCOM, 1995, GAME_QUIZZ,
  .load_game = load_cps1_12);
GMEI( megaman, "Mega Man: The Power Battle (CPS1, USA 951006)", CAPCOM, 1995, GAME_BEAT,
  .load_game = load_cps1_12);
CLNEI( megamana, megaman, "Mega Man: The Power Battle (CPS1, Asia 951006)", CAPCOM, 1995, GAME_BEAT,
  .load_game = load_cps1_12);
CLNEI( rockmanj, megaman, "Rockman: The Power Battle (CPS1, Japan 950922)", CAPCOM, 1995, GAME_BEAT,
  .load_game = load_cps1_12,
  .input = input_rockmanj, .dsw = dsw_rockmanj);
GMEI( ganbare, "Ganbare! Marine Kun (Japan 2K0411)", CAPCOM, 2000, GAME_MISC|GAME_NOT_WORKING,
	.exec = execute_ganbare, .input = input_ganbare, .dsw = dsw_ganbare);
GMEI( pang3, "Pang! 3 (Euro 950601)", MITCHELL, 1995, GAME_SHOOT,
  .load_game = load_cps1_12);
CLNEI( pang3r1, pang3, "Pang! 3 (Euro 950511)", MITCHELL, 1995, GAME_SHOOT,
  .load_game = load_cps1_12, .dsw = dsw_pang3j);
CLNEI( pang3j, pang3, "Pang! 3: Kaitou Tachi no Karei na Gogo (Japan 950511)", MITCHELL, 1995, GAME_SHOOT,
  .load_game = load_cps1_12, .dsw = dsw_pang3j);
CLNEI( pang3b, pang3, "Pang! 3 (bootleg)", BOOTLEG, 1995, GAME_SHOOT,
  .load_game = load_cps1_12, .dsw = dsw_pang3j);

// Home cps changer unit

#define dsw_wofch NULL
GMEI( wofch, "Tenchi wo Kurau II: Sekiheki no Tatakai (CPS Changer, Japan 921031)", CAPCOM, 1994, GAME_MISC,
  .load_game = load_qsound, .sound = sound_qsound,
  .exec = execute_qsound_frame);
GMEI( sfzch, "Street Fighter Zero (CPS Changer, Japan 951020)", CAPCOM, 1995, GAME_MISC,
  .load_game = load_cps1_12,
  .romsw = romsw_sfzch);
CLNEI( sfach, sfzch, "Street Fighter Alpha: Warriors' Dreams (CPS Changer, Publicity USA 950727)", CAPCOM, 1995, GAME_MISC,
  .load_game = load_cps1_12);
CLNEI( sfzbch, sfzch, "Street Fighter Zero (CPS Changer, Brazil 950727)", CAPCOM, 1995, GAME_MISC,
  .load_game = load_cps1_12);
