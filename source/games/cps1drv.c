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
// Except for pang3 : this one uses a CONTINUE_ROM in its gfx1 area.
// There is no direct equivalent in raine for now. You must change it
// by hand to load_normal, and load the 2 sets one after the other.
// Updated 1/22/2009 !

static struct ROM_INFO _forgottn_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "lwu_11a.14f",  0xddf78831, "lwu_15a.14g",  0xf7ce2097),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "lwu_10a.13f",  0x8cb38c81, "lwu_14a.13g",  0xd70ef9fd),
  { "lw-07.13e", 0x80000, 0xfd252a26, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "lw-01.9d",  0x0318f298, "lw-08.9f",  0x25a8e43c,
            "lw-05.9e",  0xe4552fd7, "lw-12.9g",  0x8e6a832b),
  LOAD16_64(  REGION_GFX1,  0x200000,  0x80000,
            "lw-02.12d",  0x43e6c5c8, "lw-09.12f",  0x899cb4ad,
            "lw-06.12e",  0x5b9edffc, "lw-13.12g",  0x8e058ef5),
  { "lwu_00.14a", 0x10000, 0x59df2a63, REGION_ROM2, 0, LOAD_NORMAL },
  { "lw-03u.14c", 0x20000, 0x807d051f, REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "lw-04u.13c", 0x20000, 0xe6cd098e, REGION_SMP1, 0x20000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _lostwrld_roms[] = // clone of _forgottn
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "lw_11c.14f",  0x67e42546, "lw_15c.14g",  0x402e2a46),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "lw_10c.13f",  0xc46479d7, "lw_14c.13g",  0x97670f4a),
  { "lw-07.13e", 0x80000, 0xfd252a26, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  { "lw-03.14c", 0x20000, 0xce2159e7, REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "lw-04.13c", 0x20000, 0x39305536, REGION_SMP1, 0x20000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _ghouls_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "dme_29.10h",  0x166a58a2, "dme_30.10j",  0x7ac8407a),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "dme_27.9h",  0xf734b2be, "dme_28.9j",  0x03d3e714),
  { "dm-17.7j", 0x80000, 0x3ea1b0f2, REGION_ROM1, 0x80000, LOAD_NORMAL },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "dm-05.3a",  0x0ba9c0b0, "dm-07.3f",  0x5d760ab9,
            "dm-06.3c",  0x4ba90b59, "dm-08.3g",  0x4bdee9de),
  { "dm_09.4a", 0x10000, 0xae24bb19, REGION_GFX1, 0x200000, LOAD_8_64 },
  { "dm_18.7a", 0x10000, 0xd34e271a, REGION_GFX1, 0x200001, LOAD_8_64 },
  { "dm_13.4e", 0x10000, 0x3f70dd37, REGION_GFX1, 0x200002, LOAD_8_64 },
  { "dm_22.7e", 0x10000, 0x7e69e2e6, REGION_GFX1, 0x200003, LOAD_8_64 },
  { "dm_11.4c", 0x10000, 0x37c9b6c6, REGION_GFX1, 0x200004, LOAD_8_64 },
  { "dm_20.7c", 0x10000, 0x2f1345b4, REGION_GFX1, 0x200005, LOAD_8_64 },
  { "dm_15.4g", 0x10000, 0x3c2a212a, REGION_GFX1, 0x200006, LOAD_8_64 },
  { "dm_24.7g", 0x10000, 0x889aac05, REGION_GFX1, 0x200007, LOAD_8_64 },
  { "dm_10.4b", 0x10000, 0xbcc0f28c, REGION_GFX1, 0x280000, LOAD_8_64 },
  { "dm_19.7b", 0x10000, 0x2a40166a, REGION_GFX1, 0x280001, LOAD_8_64 },
  { "dm_14.4f", 0x10000, 0x20f85c03, REGION_GFX1, 0x280002, LOAD_8_64 },
  { "dm_23.7f", 0x10000, 0x8426144b, REGION_GFX1, 0x280003, LOAD_8_64 },
  { "dm_12.4d", 0x10000, 0xda088d61, REGION_GFX1, 0x280004, LOAD_8_64 },
  { "dm_21.7d", 0x10000, 0x17e11df0, REGION_GFX1, 0x280005, LOAD_8_64 },
  { "dm_16.4h", 0x10000, 0xf187ba1c, REGION_GFX1, 0x280006, LOAD_8_64 },
  { "dm_25.7h", 0x10000, 0x29f79c78, REGION_GFX1, 0x280007, LOAD_8_64 },
  { "dm_26.10a", 0x10000, 0x3692f6e5, REGION_ROM2, 0, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _ghoulsu_roms[] = // clone of _ghouls
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "dmu_29.10h",  0x334d85b2, "dmu_30.10j",  0xcee8ceb5),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "dmu_27.9h",  0x4a524140, "dmu_28.9j",  0x94aae205),
  { "dm-17.7j", 0x80000, 0x3ea1b0f2, REGION_ROM1, 0x80000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _daimakai_roms[] = // clone of _ghouls
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "dmj_38.12f",  0x82fd1798, "dmj_39.12h",  0x35366ccc),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "dmj_40.13f",  0xa17c170a, "dmj_41.13h",  0x6af0b391),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "dm_33.10f",  0x384d60c4, "dm_34.10h",  0x19abe30f),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "dm_35.11f",  0xc04b85c8, "dm_36.11h",  0x89be83de),
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

static struct ROM_INFO _daimakr2_roms[] = // clone of _ghouls
{
  { "damj_23.8f", 0x80000, 0xc3b248ec, REGION_ROM1, 0x00000, LOAD_SWAP_16 },
  { "damj_22.7f", 0x80000, 0x595ff2f3, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "dam_01.3a",  0x0ba9c0b0, "dam_02.4a",  0x5d760ab9,
            "dam_03.5a",  0x4ba90b59, "dam_04.6a",  0x4bdee9de),
  LOAD16_64(  REGION_GFX1,  0x200000,  0x80000,
            "dam_05.7a",  0x7dc61b94, "dam_06.8a",  0xfde89758,
            "dam_07.9a",  0xec351d78, "dam_08.10a",  0xee2acc1e),
  { "dam_09.12a", 0x20000, 0x0656ff53, REGION_ROM2, 0, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _strider_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "strider.30",  0xda997474, "strider.35",  0x5463aaa3),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "strider.31",  0xd20786db, "strider.36",  0x21aa2863),
  { "st-14.8h", 0x80000, 0x9b3cfc08, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "st-2.8a",  0x4eee9aea, "st-11.10a",  0x2d7f21e4,
            "st-5.4a",  0x7705aa46, "st-9.6a",  0x5b18b722),
  LOAD16_64(  REGION_GFX1,  0x200000,  0x80000,
            "st-1.7a",  0x005f000b, "st-10.9a",  0xb9441519,
            "st-4.3a",  0xb7d04e8b, "st-8.5a",  0x6b4713b4),
  { "strider.09", 0x10000, 0x2ed403bc, REGION_ROM2, 0, LOAD_NORMAL },
  { "strider.18", 0x20000, 0x4386bc80, REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "strider.19", 0x20000, 0x444536d7, REGION_SMP1, 0x20000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _stridrua_roms[] = // clone of _strider
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "strid.30",  0x66aec273, "strid.35",  0x50e0e865),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "strid.31",  0xeae93bd1, "strid.36",  0xb904a31d),
  { "st-14.8h", 0x80000, 0x9b3cfc08, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  { "strid.09", 0x10000, 0x08d63519, REGION_ROM2, 0, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _striderj_roms[] = // clone of _strider
{
  { "sthj23.bin", 0x80000, 0x046e7b12, REGION_ROM1, 0x00000, LOAD_SWAP_16 },
  { "st-14.8h", 0x80000, 0x9b3cfc08, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _stridrja_roms[] = // clone of _strider
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "sth36.bin",  0x53c7b006, "sth42.bin",  0x4037f65f),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "sth37.bin",  0x80e8877d, "sth43.bin",  0x6b3fa466),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "sth34.bin",  0xbea770b5, "sth40.bin",  0x43b922dc),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "sth35.bin",  0x5cc429da, "sth41.bin",  0x50af457f),
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
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _dynwar_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "30.11f",  0xf9ec6d68, "35.11h",  0xe41fff2f),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "31.12f",  0xe3de76ff, "36.12h",  0x7a13cfbf),
  { "tkm-9.8h", 0x80000, 0x93654bcf, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "tkm-5.7a",  0xf64bb6a0, "tkm-8.9a",  0x21fe6274,
            "tkm-6.3a",  0x0bf228cb, "tkm-7.5a",  0x1255dfb1),
  LOAD16_64(  REGION_GFX1,  0x200000,  0x80000,
            "tkm-1.8a",  0x44f7661e, "tkm-4.10a",  0xa54c515d,
            "tkm-2.4a",  0xca5c687c, "tkm-3.6a",  0xf9fe6591),
  { "tke_17.12b", 0x10000, 0xb3b79d4f, REGION_ROM2, 0, LOAD_NORMAL },
  { "tke_18.11c", 0x20000, 0xac6e307d, REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "tke_19.12c", 0x20000, 0x068741db, REGION_SMP1, 0x20000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _dynwaru_roms[] = // clone of _dynwar
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "36",  0x895991d1, "42",  0xc898d2e8),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "37",  0xb228d58c, "43",  0x1a14375a),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "34.bin",  0x8f663d00, "40.bin",  0x1586dbf3),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "35.bin",  0x9db93d7a, "41.bin",  0x1aae69a4),
  { "09.bin", 0x20000, 0xc3e83c69, REGION_GFX1, 0x000000, LOAD_8_64 },
  { "01.bin", 0x20000, 0x187b2886, REGION_GFX1, 0x000001, LOAD_8_64 },
  { "13.bin", 0x20000, 0x0273d87d, REGION_GFX1, 0x000002, LOAD_8_64 },
  { "05.bin", 0x20000, 0x339378b8, REGION_GFX1, 0x000003, LOAD_8_64 },
  { "24.bin", 0x20000, 0xc6909b6f, REGION_GFX1, 0x000004, LOAD_8_64 },
  { "17.bin", 0x20000, 0x2e2f8320, REGION_GFX1, 0x000005, LOAD_8_64 },
  { "38.bin", 0x20000, 0xcd7923ed, REGION_GFX1, 0x000006, LOAD_8_64 },
  { "32.bin", 0x20000, 0x21a0a453, REGION_GFX1, 0x000007, LOAD_8_64 },
  { "10.bin", 0x20000, 0xff28f8d0, REGION_GFX1, 0x100000, LOAD_8_64 },
  { "02.bin", 0x20000, 0xcc83c02f, REGION_GFX1, 0x100001, LOAD_8_64 },
  { "14", 0x20000, 0x58d9b32f, REGION_GFX1, 0x100002, LOAD_8_64 },
  { "06.bin", 0x20000, 0x6f9edd75, REGION_GFX1, 0x100003, LOAD_8_64 },
  { "25.bin", 0x20000, 0x152ea74a, REGION_GFX1, 0x100004, LOAD_8_64 },
  { "18.bin", 0x20000, 0x1833f932, REGION_GFX1, 0x100005, LOAD_8_64 },
  { "39.bin", 0x20000, 0xbc09b360, REGION_GFX1, 0x100006, LOAD_8_64 },
  { "33.bin", 0x20000, 0x89de1533, REGION_GFX1, 0x100007, LOAD_8_64 },
  { "11.bin", 0x20000, 0x29eaf490, REGION_GFX1, 0x200000, LOAD_8_64 },
  { "03.bin", 0x20000, 0x7bf51337, REGION_GFX1, 0x200001, LOAD_8_64 },
  { "15.bin", 0x20000, 0xd36cdb91, REGION_GFX1, 0x200002, LOAD_8_64 },
  { "07.bin", 0x20000, 0xe04af054, REGION_GFX1, 0x200003, LOAD_8_64 },
  { "26.bin", 0x20000, 0x07fc714b, REGION_GFX1, 0x200004, LOAD_8_64 },
  { "19.bin", 0x20000, 0x7114e5c6, REGION_GFX1, 0x200005, LOAD_8_64 },
  { "28.bin", 0x20000, 0xaf62bf07, REGION_GFX1, 0x200006, LOAD_8_64 },
  { "21.bin", 0x20000, 0x523f462a, REGION_GFX1, 0x200007, LOAD_8_64 },
  { "12.bin", 0x20000, 0x38652339, REGION_GFX1, 0x300000, LOAD_8_64 },
  { "04.bin", 0x20000, 0x4951bc0f, REGION_GFX1, 0x300001, LOAD_8_64 },
  { "16.bin", 0x20000, 0x381608ae, REGION_GFX1, 0x300002, LOAD_8_64 },
  { "08.bin", 0x20000, 0xb475d4e9, REGION_GFX1, 0x300003, LOAD_8_64 },
  { "27.bin", 0x20000, 0xa27e81fa, REGION_GFX1, 0x300004, LOAD_8_64 },
  { "20.bin", 0x20000, 0x002796dc, REGION_GFX1, 0x300005, LOAD_8_64 },
  { "29.bin", 0x20000, 0x6b41f82d, REGION_GFX1, 0x300006, LOAD_8_64 },
  { "22.bin", 0x20000, 0x52145369, REGION_GFX1, 0x300007, LOAD_8_64 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _dynwarj_roms[] = // clone of _dynwar
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "36.bin",  0x1a516657, "42.bin",  0x12a290a0),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "37.bin",  0x932fc943, "43.bin",  0x872ad76d),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "34.bin",  0x8f663d00, "40.bin",  0x1586dbf3),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "35.bin",  0x9db93d7a, "41.bin",  0x1aae69a4),
  { "09.bin", 0x20000, 0xc3e83c69, REGION_GFX1, 0x000000, LOAD_8_64 },
  { "01.bin", 0x20000, 0x187b2886, REGION_GFX1, 0x000001, LOAD_8_64 },
  { "13.bin", 0x20000, 0x0273d87d, REGION_GFX1, 0x000002, LOAD_8_64 },
  { "05.bin", 0x20000, 0x339378b8, REGION_GFX1, 0x000003, LOAD_8_64 },
  { "24.bin", 0x20000, 0xc6909b6f, REGION_GFX1, 0x000004, LOAD_8_64 },
  { "17.bin", 0x20000, 0x2e2f8320, REGION_GFX1, 0x000005, LOAD_8_64 },
  { "38.bin", 0x20000, 0xcd7923ed, REGION_GFX1, 0x000006, LOAD_8_64 },
  { "32.bin", 0x20000, 0x21a0a453, REGION_GFX1, 0x000007, LOAD_8_64 },
  { "10.bin", 0x20000, 0xff28f8d0, REGION_GFX1, 0x100000, LOAD_8_64 },
  { "02.bin", 0x20000, 0xcc83c02f, REGION_GFX1, 0x100001, LOAD_8_64 },
  { "14.bin", 0x20000, 0x18fb232c, REGION_GFX1, 0x100002, LOAD_8_64 },
  { "06.bin", 0x20000, 0x6f9edd75, REGION_GFX1, 0x100003, LOAD_8_64 },
  { "25.bin", 0x20000, 0x152ea74a, REGION_GFX1, 0x100004, LOAD_8_64 },
  { "18.bin", 0x20000, 0x1833f932, REGION_GFX1, 0x100005, LOAD_8_64 },
  { "39.bin", 0x20000, 0xbc09b360, REGION_GFX1, 0x100006, LOAD_8_64 },
  { "33.bin", 0x20000, 0x89de1533, REGION_GFX1, 0x100007, LOAD_8_64 },
  { "11.bin", 0x20000, 0x29eaf490, REGION_GFX1, 0x200000, LOAD_8_64 },
  { "03.bin", 0x20000, 0x7bf51337, REGION_GFX1, 0x200001, LOAD_8_64 },
  { "15.bin", 0x20000, 0xd36cdb91, REGION_GFX1, 0x200002, LOAD_8_64 },
  { "07.bin", 0x20000, 0xe04af054, REGION_GFX1, 0x200003, LOAD_8_64 },
  { "26.bin", 0x20000, 0x07fc714b, REGION_GFX1, 0x200004, LOAD_8_64 },
  { "19.bin", 0x20000, 0x7114e5c6, REGION_GFX1, 0x200005, LOAD_8_64 },
  { "28.bin", 0x20000, 0xaf62bf07, REGION_GFX1, 0x200006, LOAD_8_64 },
  { "21.bin", 0x20000, 0x523f462a, REGION_GFX1, 0x200007, LOAD_8_64 },
  { "12.bin", 0x20000, 0x38652339, REGION_GFX1, 0x300000, LOAD_8_64 },
  { "04.bin", 0x20000, 0x4951bc0f, REGION_GFX1, 0x300001, LOAD_8_64 },
  { "16.bin", 0x20000, 0x381608ae, REGION_GFX1, 0x300002, LOAD_8_64 },
  { "08.bin", 0x20000, 0xb475d4e9, REGION_GFX1, 0x300003, LOAD_8_64 },
  { "27.bin", 0x20000, 0xa27e81fa, REGION_GFX1, 0x300004, LOAD_8_64 },
  { "20.bin", 0x20000, 0x002796dc, REGION_GFX1, 0x300005, LOAD_8_64 },
  { "29.bin", 0x20000, 0x6b41f82d, REGION_GFX1, 0x300006, LOAD_8_64 },
  { "22.bin", 0x20000, 0x52145369, REGION_GFX1, 0x300007, LOAD_8_64 },
  { "30.bin", 0x20000, 0x7e5f6cb4, REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "31.bin", 0x20000, 0x4a30c737, REGION_SMP1, 0x20000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _willow_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "wlu_30.rom",  0xd604dbb1, "willow-u.35",  0x7a791e77),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "wlu_31.rom",  0x0eb48a83, "wlu_36.rom",  0x36100209),
  { "wl_32.rom", 0x80000, 0xdfd9f643, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "wl_gfx5.rom",  0xafa74b73, "wl_gfx7.rom",  0x12a0dc0b,
            "wl_gfx1.rom",  0xc6f2abce, "wl_gfx3.rom",  0x4aa4c6d3),
  { "wl_24.rom", 0x20000, 0x6f0adee5, REGION_GFX1, 0x200000, LOAD_8_64 },
  { "wl_14.rom", 0x20000, 0x9cf3027d, REGION_GFX1, 0x200001, LOAD_8_64 },
  { "wl_26.rom", 0x20000, 0xf09c8ecf, REGION_GFX1, 0x200002, LOAD_8_64 },
  { "wl_16.rom", 0x20000, 0xe35407aa, REGION_GFX1, 0x200003, LOAD_8_64 },
  { "wl_20.rom", 0x20000, 0x84992350, REGION_GFX1, 0x200004, LOAD_8_64 },
  { "wl_10.rom", 0x20000, 0xb87b5a36, REGION_GFX1, 0x200005, LOAD_8_64 },
  { "wl_22.rom", 0x20000, 0xfd3f89f0, REGION_GFX1, 0x200006, LOAD_8_64 },
  { "wl_12.rom", 0x20000, 0x7da49d69, REGION_GFX1, 0x200007, LOAD_8_64 },
  { "wl_09.rom", 0x10000, 0xf6b3d060, REGION_ROM2, 0, LOAD_NORMAL },
  { "wl_18.rom", 0x20000, 0xbde23d4d, REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "wl_19.rom", 0x20000, 0x683898f5, REGION_SMP1, 0x20000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _willowj_roms[] = // clone of _willow
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "wl36.bin",  0x2b0d7cbc, "wl42.bin",  0x1ac39615),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "wl37.bin",  0x30a717fa, "wl43.bin",  0xd0dddc9e),
  { "wl_32.rom", 0x80000, 0xdfd9f643, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _willowje_roms[] = // clone of _willow
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "wlu_30.rom",  0xd604dbb1, "wlu_35.rom",  0xdaee72fe),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "wlu_31.rom",  0x0eb48a83, "wlu_36.rom",  0x36100209),
  { "wl_32.rom", 0x80000, 0xdfd9f643, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _unsquad_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "aru_30.11f",  0x24d8f88d, "aru_35.11h",  0x8b954b59),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "aru_31.12f",  0x33e9694b, "aru_36.12h",  0x7cc8fb9e),
  { "ar-32m.8h", 0x80000, 0xae1d7fb0, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "ar-5m.7a",  0xbf4575d8, "ar-7m.9a",  0xa02945f4,
            "ar-1m.3a",  0x5965ca8d, "ar-3m.5a",  0xac6db17d),
  { "ar_09.12b", 0x10000, 0xf3dd1367, REGION_ROM2, 0, LOAD_NORMAL },
	/* 20000-3ffff empty */
  { "ar_18.11c", 0x20000, 0x584b43a9, REGION_SMP1, 0x00000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _area88_roms[] = // clone of _unsquad
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "ar_36.12f",  0x65030392, "ar_42.12h",  0xc48170de),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "ar_37.13f",  0x33e9694b, "ar_43.13h",  0x7cc8fb9e),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "ar_34.10f",  0xf6e80386, "ar_40.10h",  0xbe36c145),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "ar_35.11f",  0x86d98ff3, "ar_41.11h",  0x758893d3),
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

static struct ROM_INFO _ffight_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "ff_36.11f",  0xf9a5ce83, "ff_42.11h",  0x65f11215),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "ff_37.12f",  0xe1033784, "ffe_43.12h",  0x995e968a),
  { "ff-32m.8h", 0x80000, 0xc747696e, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "ff-5m.7a",  0x9c284108, "ff-7m.9a",  0xa7584dfb,
            "ff-1m.3a",  0x0b605e44, "ff-3m.5a",  0x52291cd2),
  { "ff_09.12b", 0x10000, 0xb8367eb5, REGION_ROM2, 0, LOAD_NORMAL },
  { "ff_18.11c", 0x20000, 0x375c66e7, REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "ff_19.12c", 0x20000, 0x1ef137f9, REGION_SMP1, 0x20000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _ffightu_roms[] = // clone of _ffight
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "ff_36.11f",  0xf9a5ce83, "ff_42.11h",  0x65f11215),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "ff_37.12f",  0xe1033784, "ff43.rom",  0x4ca65947),
  { "ff-32m.8h", 0x80000, 0xc747696e, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _ffightua_roms[] = // clone of _ffight
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "ffu_36.11f",  0xe2a48af9, "ffu_42.11h",  0xf4bb480e),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "ffu_37.12f",  0xc371c667, "ffu_43.12h",  0x2f5771f9),
	/* Note: the gfx ROMs were missing from this set. I used the ones from */
	/* the World version, assuming that if the scantily clad woman shouldn't */
	/* be seen in Europe, it shouldn't be seen in the USA as well. */
  { "ff-32m.8h", 0x80000, 0xc747696e, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _ffightub_roms[] = // clone of _ffight
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "ffu30",  0xed988977, "ffu35",  0x07bf1c21),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "ffu31",  0xdba5a476, "ffu36",  0x4d89f542),
  { "ff-32m.8h", 0x80000, 0xc747696e, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
	/* these roms (from the Japanese version) were on this PCB, but they don't belong here, they cause a corrupt
       Winners Don't use Drugs logo, so I'm using the proper USA roms instead */
	/*
    ROMX_LOAD( "20_44ee.010",     0x000004, 0x20000, CRC(a1ab607a) SHA1(56784c028b82d9e2affd9610f56fde57063e4c28) , ROM_SKIP(7) ) // == ff24.bin
    ROMX_LOAD( "10_f4d8.010",     0x000005, 0x20000, CRC(2dc18cf4) SHA1(5e3bd895600cd30d561a75a2fcb6cc8bc84f4bd1) , ROM_SKIP(7) ) // == ff17.bin
    ROMX_LOAD( "22_91be.010",     0x000006, 0x20000, CRC(6535a57f) SHA1(f4da9ec13cad7e3287e34dcceb0eb2d20107bad6) , ROM_SKIP(7) ) // == ff38.bin
    ROMX_LOAD( "12_b59f.010",     0x000007, 0x20000, CRC(c8bc4a57) SHA1(3eaf2b4e910fe1f79154020122d786d23a2e594a) , ROM_SKIP(7) ) // == ff32.bin
    ROMX_LOAD( "21_cc37.010",     0x100004, 0x20000, CRC(6e8181ea) SHA1(2c32bc0364650ee6ca0d24754a7a3401295ffcd5) , ROM_SKIP(7) ) // == ff25.bin
    ROMX_LOAD( "11_2268.010",     0x100005, 0x20000, CRC(b19ede59) SHA1(7e79ad9f17b36e042d774bef3bbb44018332ca01) , ROM_SKIP(7) ) // == ff18.bin
    ROMX_LOAD( "23_0b85.010",     0x100006, 0x20000, CRC(9416b477) SHA1(f2310dfcfe960e8b822c07849b594d54dfc2b2ca) , ROM_SKIP(7) ) // == ff39.bin
    ROMX_LOAD( "13_3346.010",     0x100007, 0x20000, CRC(7369fa07) SHA1(3b2750fe33729395217c96909b4b6c5f3d6e9943) , ROM_SKIP(7) ) // == ff33.bin
    */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _ffightj_roms[] = // clone of _ffight
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "ff36.bin",  0xf9a5ce83, "ff42.bin",  0x65f11215),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "ff37.bin",  0xe1033784, "ff43.bin",  0xb6dee1c3),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "ffj_34.10f",  0x0c8dc3fc, "ffj_40.10h",  0x8075bab9),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "ffj_35.11f",  0x4a934121, "ffj_41.11h",  0x2af68154),
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

static struct ROM_INFO _ffightj1_roms[] = // clone of _ffight
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "ff30-36.rom",  0x088ed1c9, "ff35-42.rom",  0xc4c491e6),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "ff31-37.rom",  0x708557ff, "ff36-43.rom",  0xc004004a),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "ffj_34.10f",  0x0c8dc3fc, "ffj_40.10h",  0x8075bab9),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "ffj_35.11f",  0x4a934121, "ffj_41.11h",  0x2af68154),
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

static struct ROM_INFO _ffightj2_roms[] = // clone of _ffight
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "ffj_36.12f",  0xe2a48af9, "ffj_42.12h",  0xf4bb480e),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "ffj_37.13f",  0xc371c667, "ffj_43.13h",  0x6f81f194),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "ffj_34.10f",  0x0c8dc3fc, "ffj_40.10h",  0x8075bab9),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "ffj_35.11f",  0x4a934121, "ffj_41.11h",  0x2af68154),
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

static struct ROM_INFO _1941_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "41e_30.rom",  0x9deb1e75, "41e_35.rom",  0xd63942b3),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "41e_31.rom",  0xdf201112, "41e_36.rom",  0x816a818f),
  { "41_32.rom", 0x80000, 0x4e9648ca, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "41_gfx5.rom",  0x01d1cb11, "41_gfx7.rom",  0xaeaa3509,
            "41_gfx1.rom",  0xff77985a, "41_gfx3.rom",  0x983be58f),
  { "41_09.rom", 0x10000, 0x0f9d8527, REGION_ROM2, 0, LOAD_NORMAL },
  { "41_18.rom", 0x20000, 0xd1f15aeb, REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "41_19.rom", 0x20000, 0x15aec3a6, REGION_SMP1, 0x20000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _1941j_roms[] = // clone of _1941
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "4136.bin",  0x7fbd42ab, "4142.bin",  0xc7781f89),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "4137.bin",  0xc6464b0b, "4143.bin",  0x440fc0b5),
  { "41_32.rom", 0x80000, 0x4e9648ca, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _mercs_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "so2_30e.11f",  0xe17f9bf7, "so2_35e.11h",  0x78e63575),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "so2_31e.12f",  0x51204d36, "so2_36e.12h",  0x9cfba8b4),
  { "so2-32m.8h", 0x80000, 0x2eb5cf0c, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "so2-6m.8a",  0xaa6102af, "so2-8m.10a",  0x839e6869,
            "so2-2m.4a",  0x597c2875, "so2-4m.6a",  0x912a9ca0),
  { "so2_24.7d", 0x20000, 0x3f254efe, REGION_GFX1, 0x200000, LOAD_8_64 },
  { "so2_14.7c", 0x20000, 0xf5a8905e, REGION_GFX1, 0x200001, LOAD_8_64 },
  { "so2_26.9d", 0x20000, 0xf3aa5a4a, REGION_GFX1, 0x200002, LOAD_8_64 },
  { "so2_16.9c", 0x20000, 0xb43cd1a8, REGION_GFX1, 0x200003, LOAD_8_64 },
  { "so2_20.3d", 0x20000, 0x8ca751a3, REGION_GFX1, 0x200004, LOAD_8_64 },
  { "so2_10.3c", 0x20000, 0xe9f569fd, REGION_GFX1, 0x200005, LOAD_8_64 },
  { "so2_22.5d", 0x20000, 0xfce9a377, REGION_GFX1, 0x200006, LOAD_8_64 },
  { "so2_12.5c", 0x20000, 0xb7df8a06, REGION_GFX1, 0x200007, LOAD_8_64 },
  { "so2_09.12b", 0x10000, 0xd09d7c7a, REGION_ROM2, 0, LOAD_NORMAL },
  { "so2_18.11c", 0x20000, 0xbbea1643, REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "so2_19.12c", 0x20000, 0xac58aa71, REGION_SMP1, 0x20000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _mercsu_roms[] = // clone of _mercs
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "so2_30.11f",  0xe17f9bf7, "s02_35.11h",  0x4477df61),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "so2_31.12f",  0x51204d36, "so2_36.12h",  0x9cfba8b4),
  { "so2-32m.8h", 0x80000, 0x2eb5cf0c, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _mercsua_roms[] = // clone of _mercs
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "so2_30a.11f",  0xe4e725d7, "so2_35a.11h",  0xe7843445),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "so2_31a.12f",  0xc0b91dea, "so2_36a.12h",  0x591edf6c),
  { "so2-32m.8h", 0x80000, 0x2eb5cf0c, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _mercsj_roms[] = // clone of _mercs
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "so2_36.bin",  0xe17f9bf7, "so2_42.bin",  0x2c3884c6),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "so2_37.bin",  0x51204d36, "so2_43.bin",  0x9cfba8b4),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "so2_34.bin",  0xb8dae95f, "so2_40.bin",  0xde37771c),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "so2_35.bin",  0x7d24394d, "so2_41.bin",  0x914f85e0),
  { "so2_09.bin", 0x20000, 0x690c261d, REGION_GFX1, 0x000000, LOAD_8_64 },
  { "so2_01.bin", 0x20000, 0x31fd2715, REGION_GFX1, 0x000001, LOAD_8_64 },
  { "so2_13.bin", 0x20000, 0xb5e48282, REGION_GFX1, 0x000002, LOAD_8_64 },
  { "so2_05.bin", 0x20000, 0x54bed82c, REGION_GFX1, 0x000003, LOAD_8_64 },
  { "so2_24.bin", 0x20000, 0x78b6f0cb, REGION_GFX1, 0x000004, LOAD_8_64 },
  { "so2_17.bin", 0x20000, 0xe78bb308, REGION_GFX1, 0x000005, LOAD_8_64 },
  { "so2_38.bin", 0x20000, 0x0010a9a2, REGION_GFX1, 0x000006, LOAD_8_64 },
  { "so2_32.bin", 0x20000, 0x75dffc9a, REGION_GFX1, 0x000007, LOAD_8_64 },
  { "so2_10.bin", 0x20000, 0x2f871714, REGION_GFX1, 0x100000, LOAD_8_64 },
  { "so2_02.bin", 0x20000, 0xb4b2a0b7, REGION_GFX1, 0x100001, LOAD_8_64 },
  { "so2_14.bin", 0x20000, 0x737a744b, REGION_GFX1, 0x100002, LOAD_8_64 },
  { "so2_06.bin", 0x20000, 0x9d756f51, REGION_GFX1, 0x100003, LOAD_8_64 },
  { "so2_25.bin", 0x20000, 0x6d0e05d6, REGION_GFX1, 0x100004, LOAD_8_64 },
  { "so2_18.bin", 0x20000, 0x96f61f4e, REGION_GFX1, 0x100005, LOAD_8_64 },
  { "so2_39.bin", 0x20000, 0xd52ba336, REGION_GFX1, 0x100006, LOAD_8_64 },
  { "so2_33.bin", 0x20000, 0x39b90d25, REGION_GFX1, 0x100007, LOAD_8_64 },
  { "so2_11.bin", 0x20000, 0x3f254efe, REGION_GFX1, 0x200000, LOAD_8_64 },
  { "so2_03.bin", 0x20000, 0xf5a8905e, REGION_GFX1, 0x200001, LOAD_8_64 },
  { "so2_15.bin", 0x20000, 0xf3aa5a4a, REGION_GFX1, 0x200002, LOAD_8_64 },
  { "so2_07.bin", 0x20000, 0xb43cd1a8, REGION_GFX1, 0x200003, LOAD_8_64 },
  { "so2_26.bin", 0x20000, 0x8ca751a3, REGION_GFX1, 0x200004, LOAD_8_64 },
  { "so2_19.bin", 0x20000, 0xe9f569fd, REGION_GFX1, 0x200005, LOAD_8_64 },
  { "so2_28.bin", 0x20000, 0xfce9a377, REGION_GFX1, 0x200006, LOAD_8_64 },
  { "so2_21.bin", 0x20000, 0xb7df8a06, REGION_GFX1, 0x200007, LOAD_8_64 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _mtwins_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "che_30.11f",  0x9a2a2db1, "che_35.11h",  0xa7f96b02),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "che_31.12f",  0xbbff8a99, "che_36.12h",  0x0fa00c39),
  { "ck-32m.8h", 0x80000, 0x9b70bd41, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "ck-5m.7a",  0x4ec75f15, "ck-7m.9a",  0xd85d00d6,
            "ck-1m.3a",  0xf33ca9d4, "ck-3m.5a",  0x0ba2047f),
  { "ch_09.12b", 0x10000, 0x4d4255b7, REGION_ROM2, 0, LOAD_NORMAL },
  { "ch_18.12b", 0x20000, 0xf909e8de, REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "ch_19.12c", 0x20000, 0xfc158cf7, REGION_SMP1, 0x20000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _chikij_roms[] = // clone of _mtwins
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "chj_36a.bin",  0xec1328d8, "chj_42a.bin",  0x4ae13503),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "chj_37a.bin",  0x46d2cf7b, "chj_43a.bin",  0x8d387fe8),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "ch_34.bin",  0x609ed2f9, "ch_40.bin",  0xbe0d8301),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "ch_35.bin",  0xb810867f, "ch_41.bin",  0x8ad96155),
  { "ch_09.bin", 0x20000, 0x567ab3ca, REGION_GFX1, 0x000000, LOAD_8_64 },
  { "ch_01.bin", 0x20000, 0x7f3b7b56, REGION_GFX1, 0x000001, LOAD_8_64 },
  { "ch_13.bin", 0x20000, 0x12a7a8ba, REGION_GFX1, 0x000002, LOAD_8_64 },
  { "ch_05.bin", 0x20000, 0x6c1afb9a, REGION_GFX1, 0x000003, LOAD_8_64 },
  { "ch_24.bin", 0x20000, 0x9cb6e6bc, REGION_GFX1, 0x000004, LOAD_8_64 },
  { "ch_17.bin", 0x20000, 0xfe490846, REGION_GFX1, 0x000005, LOAD_8_64 },
  { "ch_38.bin", 0x20000, 0x6e5c8cb6, REGION_GFX1, 0x000006, LOAD_8_64 },
  { "ch_32.bin", 0x20000, 0x317d27b0, REGION_GFX1, 0x000007, LOAD_8_64 },
  { "ch_10.bin", 0x20000, 0xe8251a9b, REGION_GFX1, 0x100000, LOAD_8_64 },
  { "ch_02.bin", 0x20000, 0x7c8c88fb, REGION_GFX1, 0x100001, LOAD_8_64 },
  { "ch_14.bin", 0x20000, 0x4012ec4b, REGION_GFX1, 0x100002, LOAD_8_64 },
  { "ch_06.bin", 0x20000, 0x81884b2b, REGION_GFX1, 0x100003, LOAD_8_64 },
  { "ch_25.bin", 0x20000, 0x1dfcbac5, REGION_GFX1, 0x100004, LOAD_8_64 },
  { "ch_18.bin", 0x20000, 0x516a34d1, REGION_GFX1, 0x100005, LOAD_8_64 },
  { "ch_39.bin", 0x20000, 0x872fb2a4, REGION_GFX1, 0x100006, LOAD_8_64 },
  { "ch_33.bin", 0x20000, 0x30dc5ded, REGION_GFX1, 0x100007, LOAD_8_64 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _msword_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "mse_30.11f",  0x03fc8dbc, "mse_35.11h",  0xd5bf66cd),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "mse_31.12f",  0x30332bcf, "mse_36.12h",  0x8f7d6ce9),
  { "ms-32m.8h", 0x80000, 0x2475ddfc, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "ms-5m.7a",  0xc00fe7e2, "ms-7m.9a",  0x4ccacac5,
            "ms-1m.3a",  0x0d2bbe00, "ms-3m.5a",  0x3a1a5bf4),
  { "ms_9.12b", 0x10000, 0x57b29519, REGION_ROM2, 0, LOAD_NORMAL },
  { "ms_18.11c", 0x20000, 0xfb64e90d, REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "ms_19.12c", 0x20000, 0x74f892b9, REGION_SMP1, 0x20000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _mswordr1_roms[] = // clone of _msword
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "ms_30.11f",  0x21c1f078, "ms_35.11h",  0xa540a73a),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "ms_31.12f",  0xd7e762b5, "ms_36.12h",  0x66f2dcdb),
  { "ms-32m.8h", 0x80000, 0x2475ddfc, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _mswordu_roms[] = // clone of _msword
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "msu_30.11f",  0xd963c816, "msu_35.11h",  0x72f179b3),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "msu_31.12f",  0x20cd7904, "msu_36.12h",  0xbf88c080),
  { "ms-32m.8h", 0x80000, 0x2475ddfc, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _mswordj_roms[] = // clone of _msword
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "msj_36.12f",  0x04f0ef50, "msj_42.12h",  0x9fcbb9cd),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "msj_37.13f",  0x6c060d70, "msj_43.13h",  0xaec77787),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "ms_34.10f",  0x0e59a62d, "ms_40.10h",  0xbabade3a),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "ms_35.11f",  0x03da99d1, "ms_41.11h",  0xfadf99ea),
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

static struct ROM_INFO _cawing_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "cae_30a.11f",  0x91fceacd, "cae_35a.11h",  0x3ef03083),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "cae_31a.12f",  0xe5b75caf, "cae_36a.12h",  0xc73fd713),
  { "ca-32m.8h", 0x80000, 0x0c4837d4, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "ca-5m.7a",  0x66d4cc37, "ca-7m.9a",  0xb6f896f2,
            "ca-1m.3a",  0x4d0620fd, "ca-3m.5a",  0x0b0341c3),
  { "ca_9.12b", 0x10000, 0x96fe7485, REGION_ROM2, 0, LOAD_NORMAL },
  { "ca_18.11c", 0x20000, 0x4a613a2c, REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "ca_19.12c", 0x20000, 0x74584493, REGION_SMP1, 0x20000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _cawingr1_roms[] = // clone of _cawing
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "cae_30.11f",  0x23305cd5, "cae_35.11h",  0x69419113),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "cae_31.12f",  0x9008dfb3, "cae_36.12h",  0x4dbf6f8e),
  { "ca-32m.8h", 0x80000, 0x0c4837d4, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _cawingu_roms[] = // clone of _cawing
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "cae_30a.11f",  0x91fceacd, "cau_35a.11h",  0xf090d9b2),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "cae_31a.12f",  0xe5b75caf, "cae_36a.12h",  0xc73fd713),
  { "ca-32m.8h", 0x80000, 0x0c4837d4, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _cawingj_roms[] = // clone of _cawing
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "caj_36a.12f",  0x91fceacd, "caj_42a.12h",  0x039f8362),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "caj_37a.13f",  0xe5b75caf, "caj_43a.13h",  0xc73fd713),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "caj_34.10f",  0x51ea57f4, "caj_40.10h",  0x2ab71ae1),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "caj_35.11f",  0x01d71973, "caj_41.11h",  0x3a43b538),
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

static struct ROM_INFO _nemo_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "nme_30a.11f",  0xd2c03e56, "nme_35a.11h",  0x5fd31661),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "nme_31a.12f",  0xb2bd4f6f, "nme_36a.12h",  0xee9450e3),
  { "nm-32m.8h", 0x80000, 0xd6d1add3, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "nm-5m.7a",  0x487b8747, "nm-7m.9a",  0x203dc8c6,
            "nm-1m.3a",  0x9e878024, "nm-3m.5a",  0xbb01e6b6),
  { "nm_09.12b", 0x10000, 0x0f4b0581, REGION_ROM2, 0, LOAD_NORMAL },
  { "nm_18.11c", 0x20000, 0xbab333d4, REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "nm_19.12c", 0x20000, 0x2650a0a8, REGION_SMP1, 0x20000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _nemoj_roms[] = // clone of _nemo
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "nmj_36a.12f",  0xdaeceabb, "nmj_42a.12h",  0x55024740),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "nmj_37a.13f",  0x619068b6, "nmj_43a.13h",  0xa948a53b),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "nm_34.10f",  0x5737feed, "nm_40.10h",  0x8a4099f3),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "nm_35.11f",  0xbd11a7f8, "nm_41.11h",  0x6309603d),
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
  { "nm_23.13b", 0x10000, 0x8d3c5a42, REGION_ROM2, 0, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _sf2_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "sf2e.30g",  0xfe39ee33, "sf2e.37g",  0xfb92cd74),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "sf2e.31g",  0x69a0a301, "sf2e.38g",  0x5e22db70),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "sf2e.28g",  0x8bf9f1e5, "sf2e.35g",  0x626ef934),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "sf2_29a.bin",  0xbb4af315, "sf2_36a.bin",  0xc02a13eb),
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "sf2_06.bin",  0x22c9cc8e, "sf2_08.bin",  0x57213be8,
            "sf2_05.bin",  0xba529b4f, "sf2_07.bin",  0x4b1b33a8),
  LOAD16_64(  REGION_GFX1,  0x200000,  0x80000,
            "sf2_15.bin",  0x2c7e2229, "sf2_17.bin",  0xb5548f17,
            "sf2_14.bin",  0x14b84312, "sf2_16.bin",  0x5e9cd89a),
  LOAD16_64(  REGION_GFX1,  0x400000,  0x80000,
            "sf2_25.bin",  0x994bfa58, "sf2_27.bin",  0x3e66ad9d,
            "sf2_24.bin",  0xc1befaa8, "sf2_26.bin",  0x0627c831),
  { "sf2_09.bin", 0x10000, 0xa4823a1b, REGION_ROM2, 0, LOAD_NORMAL },
  { "sf2_18.bin", 0x20000, 0x7f162009, REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "sf2_19.bin", 0x20000, 0xbeade53f, REGION_SMP1, 0x20000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _sf2eb_roms[] = // clone of _sf2
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "sf2_30a.bin",  0x57bd7051, "sf2e_37b.rom",  0x62691cdd),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "sf2_31a.bin",  0xa673143d, "sf2_38a.bin",  0x4c2ccef7),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "sf2_28a.bin",  0x4009955e, "sf2_35a.bin",  0x8c1f3994),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "sf2_29a.bin",  0xbb4af315, "sf2_36a.bin",  0xc02a13eb),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _sf2ua_roms[] = // clone of _sf2
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "sf2u.30a",  0x08beb861, "sf2u.37a",  0xb7638d69),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "sf2u.31a",  0x0d5394e0, "sf2u.38a",  0x42d6a79e),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "sf2u.28a",  0x387a175c, "sf2u.35a",  0xa1a5adcc),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "sf2_29a.bin",  0xbb4af315, "sf2_36a.bin",  0xc02a13eb),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _sf2ub_roms[] = // clone of _sf2
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "sf2_30a.bin",  0x57bd7051, "sf2u.37b",  0x4a54d479),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "sf2_31a.bin",  0xa673143d, "sf2_38a.bin",  0x4c2ccef7),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "sf2_28a.bin",  0x4009955e, "sf2_35a.bin",  0x8c1f3994),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "sf2_29a.bin",  0xbb4af315, "sf2_36a.bin",  0xc02a13eb),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _sf2ud_roms[] = // clone of _sf2
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "sf2u.30d",  0x4bb2657c, "sf2u.37d",  0xb33b42f2),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "sf2u.31d",  0xd57b67d7, "sf2u.38d",  0x9c8916ef),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "sf2u.28d",  0x175819d1, "sf2u.35d",  0x82060da4),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "sf2_29a.bin",  0xbb4af315, "sf2_36a.bin",  0xc02a13eb),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _sf2ue_roms[] = // clone of _sf2
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "sf2u.30e",  0xf37cd088, "sf2u.37e",  0x6c61a513),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "sf2u.31e",  0x7c4771b4, "sf2u.38e",  0xa4bd0cd9),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "sf2u.28e",  0xe3b95625, "sf2u.35e",  0x3648769a),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "sf2_29a.bin",  0xbb4af315, "sf2_36a.bin",  0xc02a13eb),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _sf2uf_roms[] = // clone of _sf2
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "sf2e.30g",  0xfe39ee33, "sf2u.37f",  0x169e7388),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "sf2e.31g",  0x69a0a301, "sf2u.38f",  0x1510e4e2),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "sf2u.28f",  0xacd8175b, "sf2u.35f",  0xc0a80bd1),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "sf2_29a.bin",  0xbb4af315, "sf2_36a.bin",  0xc02a13eb),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _sf2ui_roms[] = // clone of _sf2
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "sf2e.30g",  0xfe39ee33, "sf2u.37i",  0x9df707dd),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "sf2e.31g",  0x69a0a301, "sf2u.38i",  0x4cb46daf),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "sf2u.28i",  0x1580be4c, "sf2u.35i",  0x1468d185),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "sf2_29a.bin",  0xbb4af315, "sf2_36a.bin",  0xc02a13eb),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _sf2uk_roms[] = // clone of _sf2
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "sf2u.30k",  0x8f66076c, "sf2u.37k",  0x4e1f6a83),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "sf2u.31k",  0xf9f89f60, "sf2u.38k",  0x6ce0a85a),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "sf2u.28k",  0x8e958f31, "sf2u.35k",  0xfce76fad),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "sf2_29a.bin",  0xbb4af315, "sf2_36a.bin",  0xc02a13eb),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _sf2j_roms[] = // clone of _sf2
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "sf2j30.bin",  0x79022b31, "sf2j37.bin",  0x516776ec),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "sf2j31.bin",  0xfe15cb39, "sf2j38.bin",  0x38614d70),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "sf2j28.bin",  0xd283187a, "sf2j35.bin",  0xd28158e4),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "sf2_29a.bin",  0xbb4af315, "sf2_36a.bin",  0xc02a13eb),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _sf2ja_roms[] = // clone of _sf2
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "sf2_30a.bin",  0x57bd7051, "sf2j_37a.bin",  0x1e1f6844),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "sf2_31a.bin",  0xa673143d, "sf2_38a.bin",  0x4c2ccef7),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "sf2_28a.bin",  0x4009955e, "sf2_35a.bin",  0x8c1f3994),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "sf2_29a.bin",  0xbb4af315, "sf2_36a.bin",  0xc02a13eb),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _sf2jc_roms[] = // clone of _sf2
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "sf2_30c.bin",  0x8add35ec, "sf2j_37c.bin",  0x0d74a256),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "sf2_31c.bin",  0xc4fff4a9, "sf2_38c.bin",  0x8210fc0e),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "sf2_28c.bin",  0x6eddd5e8, "sf2_35c.bin",  0x6bcb404c),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "sf2_29a.bin",  0xbb4af315, "sf2_36a.bin",  0xc02a13eb),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _3wonders_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "rte_30a.11f",  0xef5b8b33, "rte_35a.11h",  0x7d705529),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "rte_31a.12f",  0x32835e5e, "rte_36a.12h",  0x7637975f),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "rt_28a.9f",  0x054137c8, "rt_33a.9h",  0x7264cb1b),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "rte_29a.10f",  0xcddaa919, "rte_34a.10h",  0xed52e7e5),
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "rt-5m.7a",  0x86aef804, "rt-7m.9a",  0x4f057110,
            "rt-1m.3a",  0x902489d0, "rt-3m.5a",  0xe35ce720),
  LOAD16_64(  REGION_GFX1,  0x200000,  0x80000,
            "rt-6m.8a",  0x13cb0e7c, "rt-8m.10a",  0x1f055014,
            "rt-2m.4a",  0xe9a034f4, "rt-4m.6a",  0xdf0eea8b),
  { "rt_9.12b", 0x10000, 0xabfca165, REGION_ROM2, 0, LOAD_NORMAL },
  { "rt_18.11c", 0x20000, 0x26b211ab, REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "rt_19.12c", 0x20000, 0xdbe64ad0, REGION_SMP1, 0x20000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _3wonderu_roms[] = // clone of _3wonders
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "3wonders.30",  0x0b156fd8, "3wonders.35",  0x57350bf4),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "3wonders.31",  0x0e723fcc, "3wonders.36",  0x523a45dc),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "rt_28a.9f",  0x054137c8, "rt_33a.9h",  0x7264cb1b),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "3wonders.29",  0x37ba3e20, "3wonders.34",  0xf99f46c0),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _wonder3_roms[] = // clone of _3wonders
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "rtj_36.12f",  0xe3741247, "rtj_42.12h",  0xb4baa117),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "rtj_37.13f",  0xa1f677b0, "rtj_43.13h",  0x85337a47),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "rt_34.10f",  0x054137c8, "rt_40.10h",  0x7264cb1b),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "rtj_35.11f",  0xe72f9ea3, "rtj_41.11h",  0xa11ee998),
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
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _3wonderh_roms[] = // clone of _3wonders
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "22.bin",  0xe6071884, "26.bin",  0xa28447b7),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "23.bin",  0xfd3d6509, "27.bin",  0x999cba3d),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "rt_28a.9f",  0x054137c8, "rt_33a.9h",  0x7264cb1b),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "rte_29a.10f",  0xcddaa919, "rte_34a.10h",  0xed52e7e5),
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

static struct ROM_INFO _kod_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "kod30.rom",  0xc7414fd4, "kod37.rom",  0xa5bf40d2),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "kod31.rom",  0x1fffc7bd, "kod38.rom",  0x89e57a82),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "kod28.rom",  0x9367bcd9, "kod35.rom",  0x4ca6a48a),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "kod29.rom",  0x6a0ba878, "kod36.rom",  0xb509b39d),
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "kod02.rom",  0xe45b8701, "kod04.rom",  0xa7750322,
            "kod01.rom",  0x5f74bf78, "kod03.rom",  0x5e5303bf),
  LOAD16_64(  REGION_GFX1,  0x200000,  0x80000,
            "kod11.rom",  0x113358f3, "kod13.rom",  0x38853c44,
            "kod10.rom",  0x9ef36604, "kod12.rom",  0x402b9b4f),
  { "kod09.rom", 0x10000, 0xf5514510, REGION_ROM2, 0, LOAD_NORMAL },
  { "kod18.rom", 0x20000, 0x69ecb2c8, REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "kod19.rom", 0x20000, 0x02d851c1, REGION_SMP1, 0x20000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _kodu_roms[] = // clone of _kod
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "kdu-30b.bin",  0x825817f9, "kdu-37b.bin",  0xd2422dfb),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "kdu-31b.bin",  0x9af36039, "kdu-38b.bin",  0xbe8405a1),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "kod28.rom",  0x9367bcd9, "kod35.rom",  0x4ca6a48a),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "kd-29.bin",  0x0360fa72, "kd-36a.bin",  0x95a3cef8),
  { "kd09.bin", 0x10000, 0xbac6ec26, REGION_ROM2, 0, LOAD_NORMAL },
  { "kd18.bin", 0x20000, 0x4c63181d, REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "kd19.bin", 0x20000, 0x92941b80, REGION_SMP1, 0x20000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _kodj_roms[] = // clone of _kod
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "kd30.bin",  0xebc788ad, "kd37.bin",  0xe55c3529),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "kd31.bin",  0xc710d722, "kd38.bin",  0x57d6ed3a),
  { "kd33.bin", 0x80000, 0x9bd7ad4b, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  { "kd09.bin", 0x10000, 0xbac6ec26, REGION_ROM2, 0, LOAD_NORMAL },
  { "kd18.bin", 0x20000, 0x4c63181d, REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "kd19.bin", 0x20000, 0x92941b80, REGION_SMP1, 0x20000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _captcomm_roms[] =
{
  { "cce_23d.rom", 0x80000, 0x19c58ece, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "cc_22d.rom", 0x80000, 0xa91949b7, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  LOAD8_16(  REGION_ROM1,  0x100000,  0x20000,
            "cc_24d.rom",  0x680e543f, "cc_28d.rom",  0x8820039f),
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "gfx_01.rom",  0x7261d8ba, "gfx_03.rom",  0x6a60f949,
            "gfx_02.rom",  0x00637302, "gfx_04.rom",  0xcc87cf61),
  LOAD16_64(  REGION_GFX1,  0x200000,  0x80000,
            "gfx_05.rom",  0x28718bed, "gfx_07.rom",  0xd4acc53a,
            "gfx_06.rom",  0x0c69f151, "gfx_08.rom",  0x1f9ebb97),
  { "cc_09.rom", 0x10000, 0x698e8b58, REGION_ROM2, 0, LOAD_NORMAL },
  { "cc_18.rom", 0x20000, 0x6de2c2db, REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "cc_19.rom", 0x20000, 0xb99091ae, REGION_SMP1, 0x20000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _captcomu_roms[] = // clone of _captcomm
{
  { "23b", 0x80000, 0x03da44fd, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "22c", 0x80000, 0x9b82a052, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  LOAD8_16(  REGION_ROM1,  0x100000,  0x20000,
            "24b",  0x84ff99b2, "28b",  0xfbcec223),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _captcomj_roms[] = // clone of _captcomm
{
  { "cc23.bin", 0x80000, 0x5b482b62, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "cc22.bin", 0x80000, 0x0fd34195, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  LOAD8_16(  REGION_ROM1,  0x100000,  0x20000,
            "cc24.bin",  0x3a794f25, "cc28.bin",  0xfc3c2906),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _knights_roms[] =
{
  { "kr_23e.rom", 0x80000, 0x1b3997eb, REGION_ROM1, 0x00000, LOAD_SWAP_16 },
  { "kr_22.rom", 0x80000, 0xd0b671a9, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "kr_gfx1.rom",  0x9e36c1a4, "kr_gfx3.rom",  0xc5832cae,
            "kr_gfx2.rom",  0xf095be2d, "kr_gfx4.rom",  0x179dfd96),
  LOAD16_64(  REGION_GFX1,  0x200000,  0x80000,
            "kr_gfx5.rom",  0x1f4298d2, "kr_gfx7.rom",  0x37fa8751,
            "kr_gfx6.rom",  0x0200bc3d, "kr_gfx8.rom",  0x0bb2b4e7),
  { "kr_09.rom", 0x10000, 0x5e44d9ee, REGION_ROM2, 0, LOAD_NORMAL },
  { "kr_18.rom", 0x20000, 0xda69d15f, REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "kr_19.rom", 0x20000, 0xbfc654e9, REGION_SMP1, 0x20000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _knightsu_roms[] = // clone of _knights
{
  { "kru23.rom", 0x80000, 0x252bc2ba, REGION_ROM1, 0x00000, LOAD_SWAP_16 },
  { "kr_22.rom", 0x80000, 0xd0b671a9, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _knightsj_roms[] = // clone of _knights
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "krj30.bin",  0xad3d1a8e, "krj37.bin",  0xe694a491),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "krj31.bin",  0x85596094, "krj38.bin",  0x9198bf8f),
  { "kr_22.rom", 0x80000, 0xd0b671a9, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _sf2ce_roms[] =
{
  { "sf2ce.23", 0x80000, 0x3f846b74, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sf2ce.22", 0x80000, 0x99f1cca4, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "s92_21a.bin", 0x80000, 0x925a7877, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "s92_01.bin",  0x03b0d852, "s92_02.bin",  0x840289ec,
            "s92_03.bin",  0xcdb5f027, "s92_04.bin",  0xe2799472),
  LOAD16_64(  REGION_GFX1,  0x200000,  0x80000,
            "s92_05.bin",  0xba8a2761, "s92_06.bin",  0xe584bfb5,
            "s92_07.bin",  0x21e3f87d, "s92_08.bin",  0xbefc47df),
  LOAD16_64(  REGION_GFX1,  0x400000,  0x80000,
            "s92_10.bin",  0x960687d5, "s92_11.bin",  0x978ecd18,
            "s92_12.bin",  0xd6ec9a0a, "s92_13.bin",  0xed2c67f6),
  { "s92_09.bin", 0x10000, 0x08f6b60e, REGION_ROM2, 0, LOAD_NORMAL },
  { "s92_18.bin", 0x20000, 0x7f162009, REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "s92_19.bin", 0x20000, 0xbeade53f, REGION_SMP1, 0x20000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _sf2ceua_roms[] = // clone of _sf2ce
{
  { "s92u-23a", 0x80000, 0xac44415b, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sf2ce.22", 0x80000, 0x99f1cca4, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "s92_21a.bin", 0x80000, 0x925a7877, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _sf2ceub_roms[] = // clone of _sf2ce
{
  { "s92-23b", 0x80000, 0x996a3015, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "s92-22b", 0x80000, 0x2bbe15ed, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "s92-21b", 0x80000, 0xb383cb1c, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _sf2ceuc_roms[] = // clone of _sf2ce
{
  { "s92-23c", 0x80000, 0x0a8b6aa2, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "s92-22c", 0x80000, 0x5fd8630b, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "s92-21b", 0x80000, 0xb383cb1c, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _sf2cej_roms[] = // clone of _sf2ce
{
  { "s92j_23b.bin", 0x80000, 0x140876c5, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "s92j_22b.bin", 0x80000, 0x2fbb3bfe, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "s92_21a.bin", 0x80000, 0x925a7877, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _sf2rb_roms[] = // clone of _sf2ce
{
  { "sf2d__23.rom", 0x80000, 0x450532b0, REGION_ROM1, 0x000000, LOAD_NORMAL },
  { "sf2d__22.rom", 0x80000, 0xfe9d9cf5, REGION_ROM1, 0x080000, LOAD_NORMAL },
  { "s92_21a.bin", 0x80000, 0x925a7877, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _sf2rb2_roms[] = // clone of _sf2ce
{
  LOAD8_16(  REGION_ROM1,  0x000000,  0x20000,
            "27.bin",  0x40296ecd, "31.bin",  0x87954a41),
  LOAD8_16(  REGION_ROM1,  0x040000,  0x20000,
            "26.bin",  0xa6974195, "30.bin",  0x8141fe32),
  LOAD8_16(  REGION_ROM1,  0x080000,  0x20000,
            "25.bin",  0x9ef8f772, "29.bin",  0x7d9c479c),
  LOAD8_16(  REGION_ROM1,  0x0c0000,  0x20000,
            "24.bin",  0x93579684, "28.bin",  0xff728865),
  { "s92_21a.bin", 0x80000, 0x925a7877, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _sf2rb3_roms[] = // clone of _sf2ce
{
  { "sf2_ce_rb.23", 0x80000, 0x202f9e50, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sf2_ce_rb.22", 0x80000, 0x145e5219, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "s92_21a.bin", 0x80000, 0x925a7877, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _sf2red_roms[] = // clone of _sf2ce
{
  { "sf2red.23", 0x80000, 0x40276abb, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sf2red.22", 0x80000, 0x18daf387, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sf2red.21", 0x80000, 0x52c486bb, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _sf2v004_roms[] = // clone of _sf2ce
{
  { "sf2v004.23", 0x80000, 0x52d19f2c, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sf2v004.22", 0x80000, 0x4b26fde7, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sf2red.21", 0x80000, 0x52c486bb, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _sf2accp2_roms[] = // clone of _sf2ce
{
  { "sf2ca-23.bin", 0x80000, 0x36c3ba2f, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sf2ca-22.bin", 0x80000, 0x0550453d, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
	/* ROM space ends at 13ffff, but the code checks 180ca6 and */
	/* crashes if it isn't 0 - protection? */
  { "sf2ca-21.bin", 0x40000, 0x4c1c43ba, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _sf2m2_roms[] = // clone of _sf2ce
{
  LOAD8_16(  REGION_ROM1,  0x000000,  0x80000,
            "ch222esp",  0x9e6d058a, "ch196esp",  0xed2ff437),
  { "s92_21a.bin", 0x80000, 0x925a7877, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _sf2m4_roms[] = // clone of _sf2ce
{
  LOAD8_16(  REGION_ROM1,  0x000000,  0x80000,
            "u222ne",  0x7133489e, "u196ne",  0xb07a4f90),
  { "s92_21a.bin", 0x80000, 0x925a7877, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _sf2m5_roms[] = // clone of _sf2ce
{
  LOAD8_16(  REGION_ROM1,  0x000000,  0x80000,
            "u222",  0x03991fba, "u196",  0x39f15a1e),
  { "s92_21a.bin", 0x80000, 0x925a7877, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _sf2m6_roms[] = // clone of _sf2ce
{
  LOAD8_16(  REGION_ROM1,  0x000000,  0x80000,
            "u222-6b",  0x0a3692be, "u196-6b",  0x80454da7),
  { "s92_21a.bin", 0x80000, 0x925a7877, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _sf2m7_roms[] = // clone of _sf2ce
{
  LOAD8_16(  REGION_ROM1,  0x000000,  0x40000,
            "u222-2i",  0x1ca7adbd, "u196-2i",  0xf758408c),
  LOAD8_16(  REGION_ROM1,  0x080000,  0x40000,
            "u222-2s",  0x720cea3e, "u196-2s",  0x9932832c),
  { "s92_21a.bin", 0x80000, 0x925a7877, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _sf2yyc_roms[] = // clone of _sf2ce
{
  LOAD8_16(  REGION_ROM1,  0x000000,  0x80000,
            "b12.rom",  0x8f742fd5, "b14.rom",  0x8831ec7f),
  LOAD8_16(  REGION_ROM1,  0x100000,  0x20000,
            "b11.rom",  0x94a46525, "b11.rom",  0x94a46525),
  LOAD8_16(  REGION_ROM1,  0x100001,  0x20000,
            "b13.rom",  0x8fb3dd47, "b13.rom",  0x8fb3dd47),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _sf2koryu_roms[] = // clone of _sf2ce
{
  LOAD8_16(  REGION_ROM1,  0x000000,  0x80000,
            "u222.rom",  0x9236a79a, "u196.rom",  0xb23a869d),
  LOAD8_16(  REGION_ROM1,  0x100000,  0x20000,
            "u221.rom",  0x64e6e091, "u195.rom",  0xc95e4443),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _varth_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "vae_30b.11f",  0xadb8d391, "vae_35b.11h",  0x44e5548f),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "vae_31b.12f",  0x1749a71c, "vae_36b.12h",  0x5f2e2450),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "vae_28b.9f",  0xe524ca50, "vae_33b.9h",  0xc0bbf8c9),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "vae_29b.10f",  0x6640996a, "vae_34b.10h",  0xfa59be8a),
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "va-5m.7a",  0xb1fb726e, "va-7m.9a",  0x4c6588cd,
            "va-1m.3a",  0x0b1ace37, "va-3m.5a",  0x44dfe706),
  { "va_09.12b", 0x10000, 0x7a99446e, REGION_ROM2, 0, LOAD_NORMAL },
  { "va_18.11c", 0x20000, 0xde30510e, REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "va_19.12c", 0x20000, 0x0610a4ac, REGION_SMP1, 0x20000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _varthr1_roms[] = // clone of _varth
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "vae_30a.11f",  0x7fcd0091, "vae_35a.11h",  0x35cf9509),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "vae_31a.12f",  0x15e5ee81, "vae_36a.12h",  0x153a201e),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "vae_28a.9f",  0x7a0e0d25, "vae_33a.9h",  0xf2365922),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "vae_29a.10f",  0x5e2cd2c3, "vae_34a.10h",  0x3d9bdf83),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _varthu_roms[] = // clone of _varth
{
  { "vau23a.bin", 0x80000, 0xfbe68726, REGION_ROM1, 0x00000, LOAD_SWAP_16 },
  { "vau22a.bin", 0x80000, 0x0ed71bbd, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _varthj_roms[] = // clone of _varth
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "vaj_36b.12f",  0x1d798d6a, "vaj_42b.12h",  0x0f720233),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "vaj_37b.13f",  0x24414b17, "vaj_43b.13h",  0x34b4b06c),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "vaj_34b.10f",  0x87c79aed, "vaj_40b.10h",  0x210b4bd0),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "vaj_35b.11f",  0x6b0da69f, "vaj_41b.11h",  0x6542c8a4),
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

static struct ROM_INFO _cworld2j_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "q536.bin",  0x38a08099, "q542.bin",  0x4d29b3a4),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "q537.bin",  0xeb547ebc, "q543.bin",  0x3ef65ea8),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "q534.bin",  0x7fcc1317, "q540.bin",  0x7f14b7b4),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "q535.bin",  0xabacee26, "q541.bin",  0xd3654067),
  { "q509.bin", 0x20000, 0x48496d80, REGION_GFX1, 0x000000, LOAD_8_64 },
  { "q501.bin", 0x20000, 0xc5453f56, REGION_GFX1, 0x000001, LOAD_8_64 },
  { "q513.bin", 0x20000, 0xc741ac52, REGION_GFX1, 0x000002, LOAD_8_64 },
  { "q505.bin", 0x20000, 0x143e068f, REGION_GFX1, 0x000003, LOAD_8_64 },
  { "q524.bin", 0x20000, 0xb419d139, REGION_GFX1, 0x000004, LOAD_8_64 },
  { "q517.bin", 0x20000, 0xbd3b4d11, REGION_GFX1, 0x000005, LOAD_8_64 },
  { "q538.bin", 0x20000, 0x9c24670c, REGION_GFX1, 0x000006, LOAD_8_64 },
  { "q532.bin", 0x20000, 0x3ef9c7c2, REGION_GFX1, 0x000007, LOAD_8_64 },
  { "q510.bin", 0x20000, 0x119e5e93, REGION_GFX1, 0x100000, LOAD_8_64 },
  { "q502.bin", 0x20000, 0xa2cadcbe, REGION_GFX1, 0x100001, LOAD_8_64 },
  { "q514.bin", 0x20000, 0xa8755f82, REGION_GFX1, 0x100002, LOAD_8_64 },
  { "q506.bin", 0x20000, 0xc92a91fc, REGION_GFX1, 0x100003, LOAD_8_64 },
  { "q525.bin", 0x20000, 0x979237cb, REGION_GFX1, 0x100004, LOAD_8_64 },
  { "q518.bin", 0x20000, 0xc57da03c, REGION_GFX1, 0x100005, LOAD_8_64 },
  { "q539.bin", 0x20000, 0xa5839b25, REGION_GFX1, 0x100006, LOAD_8_64 },
  { "q533.bin", 0x20000, 0x04d03930, REGION_GFX1, 0x100007, LOAD_8_64 },
  { "q523.bin", 0x10000, 0xe14dc524, REGION_ROM2, 0, LOAD_NORMAL },
  { "q530.bin", 0x20000, 0xd10c1b68, REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "q531.bin", 0x20000, 0x7d17e496, REGION_SMP1, 0x20000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _wof_roms[] =
{
  { "tk2e_23b.rom", 0x80000, 0x11fb2ed1, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "tk2e_22b.rom", 0x80000, 0x479b3f24, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "tk2_gfx1.rom",  0x0d9cb9bf, "tk2_gfx3.rom",  0x45227027,
            "tk2_gfx2.rom",  0xc5ca2460, "tk2_gfx4.rom",  0xe349551c),
  LOAD16_64(  REGION_GFX1,  0x200000,  0x80000,
            "tk2_gfx5.rom",  0x291f0f0b, "tk2_gfx7.rom",  0x3edeb949,
            "tk2_gfx6.rom",  0x1abd14d6, "tk2_gfx8.rom",  0xb27948e3),
  { "tk2_qa.rom", 0x20000, 0xc9183a0d, REGION_ROM2, 0, LOAD_NORMAL },
  { "tk2_q1.rom", 0x80000, 0x611268cf, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "tk2_q2.rom", 0x80000, 0x20f55ca9, REGION_SMP1, 0x080000, LOAD_NORMAL },
  { "tk2_q3.rom", 0x80000, 0xbfcf6f52, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "tk2_q4.rom", 0x80000, 0x36642e88, REGION_SMP1, 0x180000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _wofa_roms[] = // clone of _wof
{
  { "tk2a_23b.rom", 0x80000, 0x2e024628, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "tk2a_22b.rom", 0x80000, 0x900ad4cd, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _wofu_roms[] = // clone of _wof
{
  { "tk2u.23c", 0x80000, 0x29b89c12, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "tk2u.22c", 0x80000, 0xf5af4774, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _wofj_roms[] = // clone of _wof
{
  { "tk2j23c.bin", 0x80000, 0x9b215a68, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "tk2j22c.bin", 0x80000, 0xb74b09ac, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "tk2_gfx1.rom",  0x0d9cb9bf, "tk2_gfx3.rom",  0x45227027,
            "tk2_gfx2.rom",  0xc5ca2460, "tk2_gfx4.rom",  0xe349551c),
  LOAD16_64(  REGION_GFX1,  0x200000,  0x80000,
            "tk205.bin",  0xe4a44d53, "tk206.bin",  0x58066ba8,
            "tk207.bin",  0xd706568e, "tk208.bin",  0xd4a19a02),
  { NULL, 0, 0, 0, 0, 0 }
};

#define wofhfh_dsw wof_dsw
static struct ROM_INFO _wofhfh_roms[] = // clone of _wof
{
  { "23", 0x80000, 0x6ae4b312, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "22", 0x80000, 0x94e8d01a, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "1",  0x0d9cb9bf, "2",  0x45227027,
            "3",  0xc5ca2460, "4",  0xe349551c),
  LOAD16_64(  REGION_GFX1,  0x200000,  0x80000,
            "5",  0x34949d7b, "6",  0xdfa70971,
            "7",  0x073686a6, "8",  0x5300f8db),
  { "9", 0x20000, 0x86fe8a97, REGION_ROM2, 0, LOAD_NORMAL },
  { "18", 0x20000, 0xc04be720, REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "19", 0x20000, 0xfbb8d8c1, REGION_SMP1, 0x20000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _sf2hf_roms[] = // clone of _sf2ce
{
  { "s92e_23a.bin", 0x80000, 0x2DD72514, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sf2_22.bin", 0x80000, 0xaea6e035, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sf2_21.bin", 0x80000, 0xfd200288, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "s92_01.bin",  0x03b0d852, "s92_02.bin",  0x840289ec,
            "s92_03.bin",  0xcdb5f027, "s92_04.bin",  0xe2799472),
  LOAD16_64(  REGION_GFX1,  0x200000,  0x80000,
            "s92_05.bin",  0xba8a2761, "s92_06.bin",  0xe584bfb5,
            "s92_07.bin",  0x21e3f87d, "s92_08.bin",  0xbefc47df),
  LOAD16_64(  REGION_GFX1,  0x400000,  0x80000,
            "s2t_10.bin",  0x3c042686, "s2t_11.bin",  0x8b7e7183,
            "s2t_12.bin",  0x293c888c, "s2t_13.bin",  0x842b35a4),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _sf2t_roms[] = // clone of _sf2ce
{
  { "sf2.23", 0x80000, 0x89a1fc38, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sf2_22.bin", 0x80000, 0xaea6e035, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sf2_21.bin", 0x80000, 0xfd200288, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "s92_01.bin",  0x03b0d852, "s92_02.bin",  0x840289ec,
            "s92_03.bin",  0xcdb5f027, "s92_04.bin",  0xe2799472),
  LOAD16_64(  REGION_GFX1,  0x200000,  0x80000,
            "s92_05.bin",  0xba8a2761, "s92_06.bin",  0xe584bfb5,
            "s92_07.bin",  0x21e3f87d, "s92_08.bin",  0xbefc47df),
  LOAD16_64(  REGION_GFX1,  0x400000,  0x80000,
            "s2t_10.bin",  0x3c042686, "s2t_11.bin",  0x8b7e7183,
            "s2t_12.bin",  0x293c888c, "s2t_13.bin",  0x842b35a4),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _sf2tj_roms[] = // clone of _sf2ce
{
  { "s2tj_23.bin", 0x80000, 0xea73b4dc, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "s2t_22.bin", 0x80000, 0xaea6e035, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "s2t_21.bin", 0x80000, 0xfd200288, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "s92_01.bin",  0x03b0d852, "s92_02.bin",  0x840289ec,
            "s92_03.bin",  0xcdb5f027, "s92_04.bin",  0xe2799472),
  LOAD16_64(  REGION_GFX1,  0x200000,  0x80000,
            "s92_05.bin",  0xba8a2761, "s92_06.bin",  0xe584bfb5,
            "s92_07.bin",  0x21e3f87d, "s92_08.bin",  0xbefc47df),
  LOAD16_64(  REGION_GFX1,  0x400000,  0x80000,
            "s2t_10.bin",  0x3c042686, "s2t_11.bin",  0x8b7e7183,
            "s2t_12.bin",  0x293c888c, "s2t_13.bin",  0x842b35a4),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _dino_roms[] =
{
  { "cde_23a.rom", 0x80000, 0x8f4e585e, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "cde_22a.rom", 0x80000, 0x9278aa12, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "cde_21a.rom", 0x80000, 0x66d23de2, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "cd_gfx01.rom",  0x8da4f917, "cd_gfx03.rom",  0x6c40f603,
            "cd_gfx02.rom",  0x09c8fc2d, "cd_gfx04.rom",  0x637ff38f),
  LOAD16_64(  REGION_GFX1,  0x200000,  0x80000,
            "cd_gfx05.rom",  0x470befee, "cd_gfx07.rom",  0x22bfb7a3,
            "cd_gfx06.rom",  0xe7599ac4, "cd_gfx08.rom",  0x211b4b15),
  { "cd_q.rom", 0x20000, 0x605fdb0b, REGION_ROM2, 0, LOAD_NORMAL },
  { "cd_q1.rom", 0x80000, 0x60927775, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "cd_q2.rom", 0x80000, 0x770f4c47, REGION_SMP1, 0x080000, LOAD_NORMAL },
  { "cd_q3.rom", 0x80000, 0x2f273ffc, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "cd_q4.rom", 0x80000, 0x2c67821d, REGION_SMP1, 0x180000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _dinou_roms[] = // clone of _dino
{
  { "cdu.23a", 0x80000, 0x7c2543cd, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "cdu.22a", 0x80000, 0xfab740a9, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "cde_21a.rom", 0x80000, 0x66d23de2, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _dinoj_roms[] = // clone of _dino
{
  { "cdj-23a.8f", 0x80000, 0x5f3ece96, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "cdj-22a.7f", 0x80000, 0xa0d8de29, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "cde_21a.rom", 0x80000, 0x66d23de2, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _punisher_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x000000,  0x20000,
            "pse_26.rom",  0x389a99d2, "pse_30.rom",  0x68fb06ac),
  LOAD8_16(  REGION_ROM1,  0x040000,  0x20000,
            "pse_27.rom",  0x3eb181c3, "pse_31.rom",  0x37108e7b),
  LOAD8_16(  REGION_ROM1,  0x080000,  0x20000,
            "pse_24.rom",  0x0f434414, "pse_28.rom",  0xb732345d),
  LOAD8_16(  REGION_ROM1,  0x0c0000,  0x20000,
            "pse_25.rom",  0xb77102e2, "pse_29.rom",  0xec037bce),
  { "ps_21.rom", 0x80000, 0x8affa5a9, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "ps_gfx1.rom",  0x77b7ccab, "ps_gfx3.rom",  0x0122720b,
            "ps_gfx2.rom",  0x64fa58d4, "ps_gfx4.rom",  0x60da42c8),
  LOAD16_64(  REGION_GFX1,  0x200000,  0x80000,
            "ps_gfx5.rom",  0xc54ea839, "ps_gfx7.rom",  0x04c5acbd,
            "ps_gfx6.rom",  0xa544f4cc, "ps_gfx8.rom",  0x8f02f436),
  { "ps_q.rom", 0x20000, 0x49ff4446, REGION_ROM2, 0, LOAD_NORMAL },
  { "ps_q1.rom", 0x80000, 0x31fd8726, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "ps_q2.rom", 0x80000, 0x980a9eef, REGION_SMP1, 0x080000, LOAD_NORMAL },
  { "ps_q3.rom", 0x80000, 0x0dd44491, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "ps_q4.rom", 0x80000, 0xbed42f03, REGION_SMP1, 0x180000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _punishru_roms[] = // clone of _punisher
{
  LOAD8_16(  REGION_ROM1,  0x000000,  0x20000,
            "psu26.rom",  0x9236d121, "psu30.rom",  0x8320e501),
  LOAD8_16(  REGION_ROM1,  0x040000,  0x20000,
            "psu27.rom",  0x61c960a1, "psu31.rom",  0x78d4c298),
  LOAD8_16(  REGION_ROM1,  0x080000,  0x20000,
            "psu24.rom",  0x1cfecad7, "psu28.rom",  0xbdf921c1),
  LOAD8_16(  REGION_ROM1,  0x0c0000,  0x20000,
            "psu25.rom",  0xc51acc94, "psu29.rom",  0x52dce1ca),
  { "ps_21.rom", 0x80000, 0x8affa5a9, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _punishrj_roms[] = // clone of _punisher
{
  { "psj23.bin", 0x80000, 0x6b2fda52, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "psj22.bin", 0x80000, 0xe01036bc, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "ps_21.rom", 0x80000, 0x8affa5a9, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _slammast_roms[] =
{
  { "mbe_23e.rom", 0x80000, 0x5394057a, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  LOAD8_16(  REGION_ROM1,  0x080000,  0x20000,
            "mbe_24b.rom",  0x95d5e729, "mbe_28b.rom",  0xb1c7cbcb),
  LOAD8_16(  REGION_ROM1,  0x0c0000,  0x20000,
            "mbe_25b.rom",  0xa50d3fd4, "mbe_29b.rom",  0x08e32e56),
  { "mbe_21a.rom", 0x80000, 0xd5007b05, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "mbe_20a.rom", 0x80000, 0xaeb557b0, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "mb_gfx01.rom",  0x41468e06, "mb_gfx03.rom",  0xf453aa9e,
            "mb_gfx02.rom",  0x2ffbfea8, "mb_gfx04.rom",  0x1eb9841d),
  LOAD16_64(  REGION_GFX1,  0x200000,  0x80000,
            "mb_05.bin",  0x506b9dc9, "mb_07.bin",  0xaff8c2fb,
            "mb_06.bin",  0xb76c70e9, "mb_08.bin",  0xe60c9556),
  LOAD16_64(  REGION_GFX1,  0x400000,  0x80000,
            "mb_10.bin",  0x97976ff5, "mb_12.bin",  0xb350a840,
            "mb_11.bin",  0x8fb94743, "mb_13.bin",  0xda810d5f),
  { "mb_qa.rom", 0x20000, 0xe21a03c4, REGION_ROM2, 0, LOAD_NORMAL },
/* Ignored : 	ROM_COPY( "audio", 0x00000, 0x00000, 0x8000 ) */
  { "mb_q1.bin", 0x80000, 0x0630c3ce, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "mb_q2.bin", 0x80000, 0x354f9c21, REGION_SMP1, 0x080000, LOAD_NORMAL },
  { "mb_q3.bin", 0x80000, 0x7838487c, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "mb_q4.bin", 0x80000, 0xab66e087, REGION_SMP1, 0x180000, LOAD_NORMAL },
  { "mb_q5.bin", 0x80000, 0xc789fef2, REGION_SMP1, 0x200000, LOAD_NORMAL },
  { "mb_q6.bin", 0x80000, 0xecb81b61, REGION_SMP1, 0x280000, LOAD_NORMAL },
  { "mb_q7.bin", 0x80000, 0x041e49ba, REGION_SMP1, 0x300000, LOAD_NORMAL },
  { "mb_q8.bin", 0x80000, 0x59fe702a, REGION_SMP1, 0x380000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _slammasu_roms[] = // clone of _slammast
{
  { "mbu-23e.rom", 0x80000, 0x224f0062, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  LOAD8_16(  REGION_ROM1,  0x080000,  0x20000,
            "mbe_24b.rom",  0x95d5e729, "mbe_28b.rom",  0xb1c7cbcb),
  LOAD8_16(  REGION_ROM1,  0x0c0000,  0x20000,
            "mbe_25b.rom",  0xa50d3fd4, "mbe_29b.rom",  0x08e32e56),
  { "mbe_21a.rom", 0x80000, 0xd5007b05, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "mbu-20a.rom", 0x80000, 0xfc848af5, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
/* Ignored : 	ROM_COPY( "audio", 0x00000, 0x00000, 0x8000 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _mbomberj_roms[] = // clone of _slammast
{
  { "mbj23e", 0x80000, 0x0d06036a, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  LOAD8_16(  REGION_ROM1,  0x080000,  0x20000,
            "mbe_24b.rom",  0x95d5e729, "mbe_28b.rom",  0xb1c7cbcb),
  LOAD8_16(  REGION_ROM1,  0x0c0000,  0x20000,
            "mbe_25b.rom",  0xa50d3fd4, "mbe_29b.rom",  0x08e32e56),
  { "mbe_21a.rom", 0x80000, 0xd5007b05, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "mbe_20a.rom", 0x80000, 0xaeb557b0, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "mbj_01.bin",  0xa53b1c81, "mbj_03.bin",  0x23fe10f6,
            "mbj_02.bin",  0xcb866c2f, "mbj_04.bin",  0xc9143e75),
  LOAD16_64(  REGION_GFX1,  0x200000,  0x80000,
            "mb_05.bin",  0x506b9dc9, "mb_07.bin",  0xaff8c2fb,
            "mb_06.bin",  0xb76c70e9, "mb_08.bin",  0xe60c9556),
  LOAD16_64(  REGION_GFX1,  0x400000,  0x80000,
            "mb_10.bin",  0x97976ff5, "mb_12.bin",  0xb350a840,
            "mb_11.bin",  0x8fb94743, "mb_13.bin",  0xda810d5f),
/* Ignored : 	ROM_COPY( "audio", 0x00000, 0x00000, 0x8000 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _mbombrd_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x000000,  0x20000,
            "mbd_26.bin",  0x72b7451c, "mbde_30.rom",  0xa036dc16),
  LOAD8_16(  REGION_ROM1,  0x040000,  0x20000,
            "mbd_27.bin",  0x4086f534, "mbd_31.bin",  0x085f47f0),
  LOAD8_16(  REGION_ROM1,  0x080000,  0x20000,
            "mbd_24.bin",  0xc20895a5, "mbd_28.bin",  0x2618d5e1),
  LOAD8_16(  REGION_ROM1,  0x0c0000,  0x20000,
            "mbd_25.bin",  0x9bdb6b11, "mbd_29.bin",  0x3f52d5e5),
  { "mbd_21.bin", 0x80000, 0x690c026a, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "mbd_20.bin", 0x80000, 0xb8b2139b, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "mb_gfx01.rom",  0x41468e06, "mb_gfx03.rom",  0xf453aa9e,
            "mb_gfx02.rom",  0x2ffbfea8, "mb_gfx04.rom",  0x1eb9841d),
  LOAD16_64(  REGION_GFX1,  0x200000,  0x80000,
            "mb_05.bin",  0x506b9dc9, "mb_07.bin",  0xaff8c2fb,
            "mb_06.bin",  0xb76c70e9, "mb_08.bin",  0xe60c9556),
  LOAD16_64(  REGION_GFX1,  0x400000,  0x80000,
            "mb_10.bin",  0x97976ff5, "mb_12.bin",  0xb350a840,
            "mb_11.bin",  0x8fb94743, "mb_13.bin",  0xda810d5f),
  { "mb_q.bin", 0x20000, 0xd6fa76d1, REGION_ROM2, 0, LOAD_NORMAL },
  { "mb_q1.bin", 0x80000, 0x0630c3ce, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "mb_q2.bin", 0x80000, 0x354f9c21, REGION_SMP1, 0x080000, LOAD_NORMAL },
  { "mb_q3.bin", 0x80000, 0x7838487c, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "mb_q4.bin", 0x80000, 0xab66e087, REGION_SMP1, 0x180000, LOAD_NORMAL },
  { "mb_q5.bin", 0x80000, 0xc789fef2, REGION_SMP1, 0x200000, LOAD_NORMAL },
  { "mb_q6.bin", 0x80000, 0xecb81b61, REGION_SMP1, 0x280000, LOAD_NORMAL },
  { "mb_q7.bin", 0x80000, 0x041e49ba, REGION_SMP1, 0x300000, LOAD_NORMAL },
  { "mb_q8.bin", 0x80000, 0x59fe702a, REGION_SMP1, 0x380000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _mbombrdj_roms[] = // clone of _mbombrd
{
  LOAD8_16(  REGION_ROM1,  0x000000,  0x20000,
            "mbd_26.bin",  0x72b7451c, "mbdj_30.bin",  0xbeff31cf),
  LOAD8_16(  REGION_ROM1,  0x040000,  0x20000,
            "mbd_27.bin",  0x4086f534, "mbd_31.bin",  0x085f47f0),
  LOAD8_16(  REGION_ROM1,  0x080000,  0x20000,
            "mbd_24.bin",  0xc20895a5, "mbd_28.bin",  0x2618d5e1),
  LOAD8_16(  REGION_ROM1,  0x0c0000,  0x20000,
            "mbd_25.bin",  0x9bdb6b11, "mbd_29.bin",  0x3f52d5e5),
  { "mbd_21.bin", 0x80000, 0x690c026a, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "mbd_20.bin", 0x80000, 0xb8b2139b, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "mbj_01.bin",  0xa53b1c81, "mbj_03.bin",  0x23fe10f6,
            "mbj_02.bin",  0xcb866c2f, "mbj_04.bin",  0xc9143e75),
  LOAD16_64(  REGION_GFX1,  0x200000,  0x80000,
            "mb_05.bin",  0x506b9dc9, "mb_07.bin",  0xaff8c2fb,
            "mb_06.bin",  0xb76c70e9, "mb_08.bin",  0xe60c9556),
  LOAD16_64(  REGION_GFX1,  0x400000,  0x80000,
            "mb_10.bin",  0x97976ff5, "mb_12.bin",  0xb350a840,
            "mb_11.bin",  0x8fb94743, "mb_13.bin",  0xda810d5f),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _pnickj_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "pnij36.bin",  0x2d4ffb2b, "pnij42.bin",  0xc085dfaf),
  { "pnij09.bin", 0x20000, 0x48177b0a, REGION_GFX1, 0x000000, LOAD_8_64 },
  { "pnij01.bin", 0x20000, 0x01a0f311, REGION_GFX1, 0x000001, LOAD_8_64 },
  { "pnij13.bin", 0x20000, 0x406451b0, REGION_GFX1, 0x000002, LOAD_8_64 },
  { "pnij05.bin", 0x20000, 0x8c515dc0, REGION_GFX1, 0x000003, LOAD_8_64 },
  { "pnij26.bin", 0x20000, 0xe2af981e, REGION_GFX1, 0x000004, LOAD_8_64 },
  { "pnij18.bin", 0x20000, 0xf17a0e56, REGION_GFX1, 0x000005, LOAD_8_64 },
  { "pnij38.bin", 0x20000, 0xeb75bd8c, REGION_GFX1, 0x000006, LOAD_8_64 },
  { "pnij32.bin", 0x20000, 0x84560bef, REGION_GFX1, 0x000007, LOAD_8_64 },
  { "pnij10.bin", 0x20000, 0xc2acc171, REGION_GFX1, 0x100000, LOAD_8_64 },
  { "pnij02.bin", 0x20000, 0x0e21fc33, REGION_GFX1, 0x100001, LOAD_8_64 },
  { "pnij14.bin", 0x20000, 0x7fe59b19, REGION_GFX1, 0x100002, LOAD_8_64 },
  { "pnij06.bin", 0x20000, 0x79f4bfe3, REGION_GFX1, 0x100003, LOAD_8_64 },
  { "pnij27.bin", 0x20000, 0x83d5cb0e, REGION_GFX1, 0x100004, LOAD_8_64 },
  { "pnij19.bin", 0x20000, 0xaf08b230, REGION_GFX1, 0x100005, LOAD_8_64 },
  { "pnij39.bin", 0x20000, 0x70fbe579, REGION_GFX1, 0x100006, LOAD_8_64 },
  { "pnij33.bin", 0x20000, 0x3ed2c680, REGION_GFX1, 0x100007, LOAD_8_64 },
  { "pnij17.bin", 0x10000, 0xe86f787a, REGION_ROM2, 0, LOAD_NORMAL },
  { "pnij24.bin", 0x20000, 0x5092257d, REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "pnij25.bin", 0x20000, 0x22109aaa, REGION_SMP1, 0x20000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _qad_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "qdu_36a.12f",  0xde9c24a0, "qdu_42a.12h",  0xcfe36f0c),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "qdu_37a.13f",  0x10d22320, "qdu_43a.13h",  0x15e6beb9),
  { "qdu_09.4b", 0x20000, 0x8c3f9f44, REGION_GFX1, 0x000000, LOAD_8_64 },
  { "qdu_01.4a", 0x20000, 0xf688cf8f, REGION_GFX1, 0x000001, LOAD_8_64 },
  { "qdu_13.9b", 0x20000, 0xafbd551b, REGION_GFX1, 0x000002, LOAD_8_64 },
  { "qdu_05.9a", 0x20000, 0xc3db0910, REGION_GFX1, 0x000003, LOAD_8_64 },
  { "qdu_24.5e", 0x20000, 0x2f1bd0ec, REGION_GFX1, 0x000004, LOAD_8_64 },
  { "qdu_17.5c", 0x20000, 0xa812f9e2, REGION_GFX1, 0x000005, LOAD_8_64 },
  { "qdu_38.8h", 0x20000, 0xccdddd1f, REGION_GFX1, 0x000006, LOAD_8_64 },
  { "qdu_32.8f", 0x20000, 0xa8d295d3, REGION_GFX1, 0x000007, LOAD_8_64 },
  { "qdu_23.13b", 0x10000, 0xcfb5264b, REGION_ROM2, 0, LOAD_NORMAL },
  { "qdu_30.12c", 0x20000, 0xf190da84, REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "qdu_31.13c", 0x20000, 0xb7583f73, REGION_SMP1, 0x20000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _qadj_roms[] = // clone of _qad
{
  { "qad23a.bin", 0x80000, 0x4d3553de, REGION_ROM1, 0x00000, LOAD_SWAP_16 },
  { "qad22a.bin", 0x80000, 0x3191ddd0, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "qad01.bin",  0x9d853b57, "qad02.bin",  0xb35976c4,
            "qad03.bin",  0xcea4ca8c, "qad04.bin",  0x41b74d1b),
  { "qad09.bin", 0x10000, 0x733161cc, REGION_ROM2, 0, LOAD_NORMAL },
  { "qad18.bin", 0x20000, 0x2bfe6f6a, REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "qad19.bin", 0x20000, 0x13d3236b, REGION_SMP1, 0x20000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _qtono2_roms[] =
{
  LOAD8_16(  REGION_ROM1,  0x00000,  0x20000,
            "tn2j-30.11e",  0x9226eb5e, "tn2j-37.11f",  0xd1d30da1),
  LOAD8_16(  REGION_ROM1,  0x40000,  0x20000,
            "tn2j-31.12e",  0x015e6a8a, "tn2j-38.12f",  0x1f139bcc),
  LOAD8_16(  REGION_ROM1,  0x80000,  0x20000,
            "tn2j-28.9e",  0x86d27f71, "tn2j-35.9f",  0x7a1ab87d),
  LOAD8_16(  REGION_ROM1,  0xc0000,  0x20000,
            "tn2j-29.10e",  0x9c384e99, "tn2j-36.10f",  0x4c4b2a0a),
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "tn2-02m.4a",  0xf2016a34, "tn2-04m.6a",  0x094e0fb1,
            "tn2-01m.3a",  0xcb950cf9, "tn2-03m.5a",  0x18a5bf59),
  LOAD16_64(  REGION_GFX1,  0x200000,  0x80000,
            "tn2-11m.4c",  0xd0edd30b, "tn2-13m.6c",  0x426621c3,
            "tn2-10m.3c",  0xa34ece70, "tn2-12m.5c",  0xe04ff2f4),
  { "tn2j-09.12a", 0x08000, 0x6d8edcef, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "tn2j-18.11c", 0x20000, 0xa40bf9a7, REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "tn2j-19.12c", 0x20000, 0x5b3b931e, REGION_SMP1, 0x20000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _pang3_roms[] =
{
  { "pa3w-17.11l", 0x80000, 0x12138234, REGION_ROM1, 0x00000, LOAD_SWAP_16 },
  { "pa3w-16.10l", 0x80000, 0xd1ba585c, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  { "pa3-01m.2c", 0x200000, 0x068a152c, REGION_GFX1, 0, LOAD_NORMAL },
  { "pa3-07m.2f", 0x200000, 0x3a4a619d, REGION_GFX1, 0x200000, LOAD_NORMAL },
  { "pa3-11.11f", 0x08000, 0x90a08c46, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "pa3-05.10d", 0x20000, 0x73a10d5d, REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "pa3-06.11d", 0x20000, 0xaffa4f82, REGION_SMP1, 0x20000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _pang3j_roms[] = // clone of _pang3
{
  { "pa3j-17.11l", 0x80000, 0x21f6e51f, REGION_ROM1, 0x00000, LOAD_SWAP_16 },
  { "pa3j-16.10l", 0x80000, 0xca1d7897, REGION_ROM1, 0x80000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _megaman_roms[] =
{
  { "rcma_23b.rom", 0x80000, 0x61e4a397, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "rcma_22b.rom", 0x80000, 0x708268c4, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "rcma_21a.rom", 0x80000, 0x4376ea95, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "rcm_01.rom",  0x6ecdf13f, "rcm_02.rom",  0x944d4f0f,
            "rcm_03.rom",  0x36f3073c, "rcm_04.rom",  0x54e622ff),
  LOAD16_64(  REGION_GFX1,  0x200000,  0x80000,
            "rcm_05.rom",  0x5dd131fd, "rcm_06.rom",  0xf0faf813,
            "rcm_07.rom",  0x826de013, "rcm_08.rom",  0xfbff64cf),
  LOAD16_64(  REGION_GFX1,  0x400000,  0x80000,
            "rcm_10.rom",  0x4dc8ada9, "rcm_11.rom",  0xf2b9ee06,
            "rcm_12.rom",  0xfed5f203, "rcm_13.rom",  0x5069d4a9),
  LOAD16_64(  REGION_GFX1,  0x600000,  0x80000,
            "rcm_14.rom",  0x303be3bd, "rcm_15.rom",  0x4f2d372f,
            "rcm_16.rom",  0x93d97fde, "rcm_17.rom",  0x92371042),
  { "rcm_09.rom", 0x20000, 0x9632d6ef, REGION_ROM2, 0, LOAD_NORMAL },
  { "rcm_18.rom", 0x20000, 0x80f1f8aa, REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "rcm_19.rom", 0x20000, 0xf257dbe1, REGION_SMP1, 0x20000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _rockmanj_roms[] = // clone of _megaman
{
  { "rcm23a.bin", 0x80000, 0xefd96cb2, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "rcm22a.bin", 0x80000, 0x8729a689, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "rcm21a.bin", 0x80000, 0x517ccde2, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO _sfzch_roms[] =
{
  { "sfzch23", 0x80000, 0x1140743f , REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sfza22", 0x80000, 0x8d9b2480 , REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sfzch21", 0x80000, 0x5435225d , REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sfza20", 0x80000, 0x806e8f38 , REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x80000,
            "sfz01",  0x0dd53e62, "sfz02",  0x94c31e3f,
            "sfz03",  0x9584ac85, "sfz04",  0xb983624c),
  LOAD16_64(  REGION_GFX1,  0x200000,  0x80000,
            "sfz05",  0x2b47b645, "sfz06",  0x74fd9fb1,
            "sfz07",  0xbb2c734d, "sfz08",  0x454f7868),
  LOAD16_64(  REGION_GFX1,  0x400000,  0x80000,
            "sfz10",  0x2a7d675e, "sfz11",  0xe35546c8,
            "sfz12",  0xf122693a, "sfz13",  0x7cf942c8),
  LOAD16_64(  REGION_GFX1,  0x600000,  0x80000,
            "sfz14",  0x09038c81, "sfz15",  0x1aa17391,
            "sfz16",  0x19a5abd6, "sfz17",  0x248b3b73),
  { "sfz09", 0x10000, 0xc772628b , REGION_ROM2, 0, LOAD_NORMAL },
  { "sfz18", 0x20000, 0x61022b2d , REGION_SMP1, 0x00000, LOAD_NORMAL },
  { "sfz19", 0x20000, 0x3b5886d5 , REGION_SMP1, 0x20000, LOAD_NORMAL },
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
  { "Freeze", 0x80, 2 },
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

static struct DSW_INFO forgottn_dsw[] =
{
  { 0x2, 0xff, dsw_data_forgottn_1 },
  { 0x4, 0xff, dsw_data_forgottn_2 },
  { 0x6, 0xff, dsw_data_forgottn_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_ghouls_1[] =
{
	CPS1_COINAGE_1
  { MSG_CABINET, 0xc0, 2 },
  { "Upright 2 Players", 0x80 },
  { "Upright 1 Player", 0xc0 },
  { NULL, 0}
};

static struct DSW_DATA dsw_data_ghouls_2[] =
{
	CPS1_DIFFICULTY_2
  DSW_UNUSED( 0x00, 0x08),
  { MSG_EXTRA_LIFE, 0x30, 4 },
  { "10K30K and every 30K" , 0x20},
  { "20K50K and every 70K" , 0x10},
  { "30K60K and every 70K" , 0x30},
  { "40K70K and every 80K" , 0x00},
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
  { "Allow Continue", 0x40, 2 },
  { MSG_NO, 0x00},
  { MSG_YES, 0x40},
  { "Game Mode",    0x80, 2 },
  { "Game" , 0x80},
  { "Test" , 0x00},
  { NULL, 0}
};

static struct DSW_INFO ghouls_dsw[] =
{
  { 0x2, 0xff, dsw_data_ghouls_1 },
  { 0x4, 0xff, dsw_data_ghouls_2 },
  { 0x6, 0xff, dsw_data_ghouls_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_ghoulsu_2[] =
{
	CPS1_DIFFICULTY_2
  DSW_UNUSED( 0x00, 0x08),
  { MSG_EXTRA_LIFE, 0x30, 4 },
  { "10K30K and every 30K" , 0x20},
  { "20K50K and every 70K" , 0x10},
  { "30K60K and every 70K" , 0x30},
  { "40K70K and every 80K" , 0x00},
  DSW_UNUSED( 0x00, 0x40),
  { "Freeze", 0x80, 2 },
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
  { "Allow Continue", 0x40, 2 },
  { MSG_NO, 0x00},
  { MSG_YES, 0x40},
  { "Game Mode",    0x80, 2 },
  { "Game" , 0x80},
  { "Test" , 0x00},
  { NULL, 0}
};

static struct DSW_INFO ghoulsu_dsw[] =
{
  { 0x2, 0xff, dsw_data_ghouls_1 },
  { 0x4, 0xff, dsw_data_ghoulsu_2 },
  { 0x6, 0xff, dsw_data_ghoulsu_3 },
  { 0, 0, NULL }
};

static struct DSW_INFO daimakai_dsw[] =
{
  { 0x2, 0xff, dsw_data_ghouls_1 },
  { 0x4, 0xff, dsw_data_ghoulsu_2 },
  { 0x6, 0xff, dsw_data_ghouls_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_strider_2[] =
{
	CPS1_DIFFICULTY_2
  DSW_UNUSED( 0x00, 0x08),
  { MSG_EXTRA_LIFE, 0x30, 4 },
  { "20K40K then every 60K" , 0x30},
  { "30K50K then every 70K" , 0x20},
  { "20K & 60K only" , 0x10},
  { "30K & 60K only" , 0x00},
  { "Internal Diff. on Life Loss", 0xc0, 1 },
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
  { "Freeze", 0x04, 2 },
  { MSG_OFF, 0x04},
  { MSG_ON, 0x00},
  { MSG_FREE_PLAY, 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  DSW_SCREEN( 0x10, 0x00),
  DSW_DEMO_SOUND( 0x20, 0x00),
  { "Allow Continue", 0x40, 2 },
  { MSG_NO, 0x00},
  { MSG_YES, 0x40},
  { "Game Mode",    0x80, 2 },
  { "Game" , 0x80},
  { "Test" , 0x00},
  { NULL, 0}
};

static struct DSW_INFO strider_dsw[] =
{
  { 0x2, 0xff, dsw_data_ghouls_1 },
  { 0x4, 0x3f, dsw_data_strider_2 },
  { 0x6, 0xff, dsw_data_strider_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_stridrua_2[] =
{
	CPS1_DIFFICULTY_2
  { "2 Coins to Start,1 to Continue", 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  { MSG_EXTRA_LIFE, 0x30, 4 },
  { "20K40K then every 60K" , 0x30},
  { "30K50K then every 70K" , 0x20},
  { "20K & 60K only" , 0x10},
  { "30K & 60K only" , 0x00},
  { "Internal Diff. on Life Loss", 0xc0, 1 },
  { "-3" , 0xc0},
  { NULL, 0}
};

static struct DSW_INFO stridrua_dsw[] =
{
  { 0x2, 0xff, dsw_data_ghouls_1 },
  { 0x4, 0x3f, dsw_data_stridrua_2 },
  { 0x6, 0xff, dsw_data_strider_3 },
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
  { "Freeze", 0x01, 2 },
  { MSG_OFF, 0x01},
  { MSG_ON, 0x00},
  { "Turbo Mode", 0x02, 2 },
  { MSG_OFF, 0x02},
  { MSG_ON, 0x00},
  DSW_UNUSED( 0x00, 0x04),
  DSW_UNUSED( 0x00, 0x08),
  DSW_SCREEN( 0x10, 0x00),
  DSW_DEMO_SOUND( 0x20, 0x00),
  { "Allow Continue", 0x40, 2 },
  { MSG_NO, 0x00},
  { MSG_YES, 0x40},
  { "Game Mode",    0x80, 2 },
  { "Game" , 0x80},
  { "Test" , 0x00},
  { NULL, 0}
};

static struct DSW_INFO dynwar_dsw[] =
{
  { 0x2, 0xff, dsw_data_dynwar_1 },
  { 0x4, 0xff, dsw_data_dynwar_2 },
  { 0x6, 0xff, dsw_data_dynwar_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_willow_1[] =
{
	CPS1_COINAGE_3
  { MSG_CABINET, 0xc0, 2 },
  { "Upright 2 Players", 0x80 },
  { "Upright 1 Player", 0xc0 },
  { NULL, 0}
};

static struct DSW_DATA dsw_data_willow_2[] =
{
	CPS1_DIFFICULTY_2
  { "Nando Speed", 0x18, 4 },
  { "Slow" , 0x10},
  { "Normal" , 0x18},
  { "Fast" , 0x08},
  { "Very Fast" , 0x00},
  DSW_UNUSED( 0x00, 0x20),
  DSW_UNUSED( 0x00, 0x40),
  { "Stage Magic Continue", 0x80, 2 },
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
  { "Vitality", 0x0c, 4 },
  { "2" , 0x00},
  { "3" , 0x0c},
  { "4" , 0x08},
  { "5" , 0x04},
  DSW_SCREEN( 0x10, 0x00),
  DSW_DEMO_SOUND( 0x20, 0x00),
  { "Allow Continue", 0x40, 2 },
  { MSG_NO, 0x00},
  { MSG_YES, 0x40},
  { "Game Mode",    0x80, 2 },
  { "Game" , 0x80},
  { "Test" , 0x00},
  { NULL, 0}
};

static struct DSW_INFO willow_dsw[] =
{
  { 0x2, 0xff, dsw_data_willow_1 },
  { 0x4, 0xff, dsw_data_willow_2 },
  { 0x6, 0xff, dsw_data_willow_3 },
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
  { "Damage", 0x18, 4 },
  { "Small" , 0x10},
  { "Normal" , 0x18},
  { "Big" , 0x08},
  { "Biggest" , 0x00},
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
  { "Freeze", 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  DSW_SCREEN( 0x10, 0x00),
  DSW_DEMO_SOUND( 0x00, 0x20),
  { "Allow Continue", 0x40, 2 },
  { MSG_NO, 0x40},
  { MSG_YES, 0x00},
  { "Game Mode",    0x80, 2 },
  { "Game" , 0x80},
  { "Test" , 0x00},
  { NULL, 0}
};

static struct DSW_INFO unsquad_dsw[] =
{
  { 0x2, 0xff, dsw_data_unsquad_1 },
  { 0x4, 0xff, dsw_data_unsquad_2 },
  { 0x6, 0x9f, dsw_data_unsquad_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_ffight_1[] =
{
	CPS1_COINAGE_1
  { "2 Coins to Start,1 to Continue", 0x40, 2 },
  { MSG_OFF, 0x40},
  { MSG_ON, 0x00},
  DSW_UNUSED( 0x00, 0x80),
  { NULL, 0}
};

static struct DSW_DATA dsw_data_ffight_2[] =
{
  { "Difficulty Level 1", 0x07, 8 },
  { "Easiest" , 0x07},
  { "Easier" , 0x06},
  { "Easy" , 0x05},
  { "Normal" , 0x04},
  { "Medium" , 0x03},
  { "Hard" , 0x02},
  { "Harder" , 0x01},
  { "Hardest" , 0x00},
  { "Difficulty Level 2", 0x18, 4 },
  { "Easy" , 0x18},
  { "Normal" , 0x10},
  { "Hard" , 0x08},
  { "Hardest" , 0x00},
  { MSG_EXTRA_LIFE, 0x60, 4 },
  { "100k" , 0x60},
  { "200k" , 0x40},
  { "100k and every 200k" , 0x20},
  { "None" , 0x00},
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
  { "Freeze", 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  DSW_SCREEN( 0x10, 0x00),
  DSW_DEMO_SOUND( 0x00, 0x20),
  { "Allow Continue", 0x40, 2 },
  { MSG_NO, 0x40},
  { MSG_YES, 0x00},
  { "Game Mode",    0x80, 2 },
  { "Game" , 0x80},
  { "Test" , 0x00},
  { NULL, 0}
};

static struct DSW_INFO ffight_dsw[] =
{
  { 0x2, 0xff, dsw_data_ffight_1 },
  { 0x4, 0xf4, dsw_data_ffight_2 },
  { 0x6, 0x9f, dsw_data_ffight_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data__1941_2[] =
{
	CPS1_DIFFICULTY_1
  { "Life Bar", 0x18, 4 },
  { "More Slowly" , 0x18},
  { "Slowly" , 0x10},
  { "Quickly" , 0x08},
  { "More Quickly" , 0x00},
  { "Bullet's Speed", 0x60, 4 },
  { "Very Slow" , 0x60},
  { "Slow" , 0x40},
  { "Fast" , 0x20},
  { "Very Fast" , 0x00},
  { "Initial Vitality", 0x80, 2 },
  { "3 Bars" , 0x80},
  { "4 Bars" , 0x00},
  { NULL, 0}
};

static struct DSW_DATA dsw_data__1941_3[] =
{
  { "Throttle Game Speed", 0x01, 2 },
  { MSG_OFF, 0x00},
  { MSG_ON, 0x01},
  DSW_UNUSED( 0x00, 0x02),
  { MSG_FREE_PLAY, 0x04, 2 },
  { MSG_OFF, 0x04},
  { MSG_ON, 0x00},
  { "Freeze", 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  DSW_SCREEN( 0x10, 0x00),
  DSW_DEMO_SOUND( 0x00, 0x20),
  { "Allow Continue", 0x40, 2 },
  { MSG_NO, 0x40},
  { MSG_YES, 0x00},
  { "Game Mode",    0x80, 2 },
  { "Game" , 0x80},
  { "Test" , 0x00},
  { NULL, 0}
};

static struct DSW_INFO _1941_dsw[] =
{
  { 0x2, 0xff, dsw_data_ffight_1 },
  { 0x4, 0xff, dsw_data__1941_2 },
  { 0x6, 0x9f, dsw_data__1941_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_mercs_1[] =
{
	CPS1_COINAGE_2
  DSW_UNUSED( 0x00, 0x08),
  DSW_UNUSED( 0x00, 0x10),
  DSW_UNUSED( 0x00, 0x20),
  { "2 Coins to Start,1 to Continue", 0x40, 2 },
  { MSG_OFF, 0x40},
  { MSG_ON, 0x00},
  DSW_UNUSED( 0x00, 0x80),
  { NULL, 0}
};

static struct DSW_DATA dsw_data_mercs_2[] =
{
	CPS1_DIFFICULTY_1
  { "Coin Slots", 0x08, 2 },
  { "1" , 0x00},
  { "3" , 0x08},
  { "Max Players", 0x10, 2 },
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
  { "Freeze", 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  DSW_SCREEN( 0x10, 0x00),
  DSW_DEMO_SOUND( 0x00, 0x20),
  { "Allow Continue", 0x40, 2 },
  { MSG_NO, 0x40},
  { MSG_YES, 0x00},
  DSW_SERVICE( 0, 0x80),
  { NULL, 0}
};

static struct DSW_INFO mercs_dsw[] =
{
  { 0x2, 0xff, dsw_data_mercs_1 },
  { 0x4, 0xff, dsw_data_mercs_2 },
  { 0x6, 0x9f, dsw_data_mercs_3 },
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
  { MSG_LIVES, 0x38},
  DSW_UNUSED( 0x00, 0x40),
  DSW_UNUSED( 0x00, 0x80),
  { NULL, 0}
};

static struct DSW_INFO mtwins_dsw[] =
{
  { 0x2, 0xff, dsw_data_mtwins_1 },
  { 0x4, 0xff, dsw_data_mtwins_2 },
  { 0x6, 0x9f, dsw_data_unsquad_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_msword_2[] =
{
  { "Player's vitality consumption", 0x07, 8 },
  { "1 (Easiest)" , 0x07},
  { "2" , 0x06},
  { "3" , 0x05},
  { "4 (Normal)" , 0x04},
  { "5" , 0x03},
  { "6" , 0x02},
  { "7" , 0x01},
  { "8 (Hardest)" , 0x00},
  { "Enemy's vitality and attacking power", 0x38, 8 },
  { "1 (Easiest)" , 0x20},
  { "2" , 0x28},
  { "3" , 0x30},
  { "4 (Normal)" , 0x38},
  { "5" , 0x18},
  { "6" , 0x10},
  { "7" , 0x08},
  { "8 (Hardest)" , 0x00},
  { "Stage Select", 0x40, 2 },
  { MSG_OFF, 0x40},
  { MSG_ON, 0x00},
  DSW_UNUSED( 0x00, 0x80),
  { NULL, 0}
};

static struct DSW_DATA dsw_data_msword_3[] =
{
  { "Vitality Packs", 0x03, 4 },
  { "1" , 0x00},
  { "2" , 0x03},
  { "3 (2 when continue)" , 0x02},
  { "4 (3 when continue)" , 0x01},
  { MSG_FREE_PLAY, 0x04, 2 },
  { MSG_OFF, 0x04},
  { MSG_ON, 0x00},
  { "Freeze", 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  DSW_SCREEN( 0x10, 0x00),
  DSW_DEMO_SOUND( 0x00, 0x20),
  { "Allow Continue", 0x40, 2 },
  { MSG_NO, 0x40},
  { MSG_YES, 0x00},
  { "Game Mode",    0x80, 2 },
  { "Game" , 0x80},
  { "Test" , 0x00},
  { NULL, 0}
};

static struct DSW_INFO msword_dsw[] =
{
  { 0x2, 0xff, dsw_data_ffight_1 },
  { 0x4, 0xbc, dsw_data_msword_2 },
  { 0x6, 0x9f, dsw_data_msword_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_cawing_2[] =
{
  { "Difficulty Level (Enemy's Strength)", 0x07, 8 },
  { "1 (Easiest)" , 0x07},
  { "2" , 0x06},
  { "3" , 0x05},
  { "4 (Normal)" , 0x04},
  { "5" , 0x03},
  { "6" , 0x02},
  { "7" , 0x01},
  { "8 (Hardest)" , 0x00},
  { "Difficulty Level (Player's Strength)", 0x18, 4 },
  { "Easy" , 0x10},
  { "Normal" , 0x18},
  { "Hard" , 0x08},
  { "Hardest" , 0x00},
  DSW_UNUSED( 0x00, 0x20),
  DSW_UNUSED( 0x00, 0x40),
  DSW_UNUSED( 0x00, 0x80),
  { NULL, 0}
};

static struct DSW_INFO cawing_dsw[] =
{
  { 0x2, 0xff, dsw_data_ffight_1 },
  { 0x4, 0xfc, dsw_data_cawing_2 },
  { 0x6, 0x9f, dsw_data_unsquad_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_nemo_2[] =
{
	CPS1_DIFFICULTY_1
  { "Life Bar", 0x18, 2 },
  { "Minimun" , 0x00},
  { "Medium" , 0x18},
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
  { "Freeze", 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  DSW_SCREEN( 0x10, 0x00),
  DSW_DEMO_SOUND( 0x00, 0x20),
  { "Allow Continue", 0x40, 2 },
  { MSG_NO, 0x40},
  { MSG_YES, 0x00},
  { "Game Mode",    0x80, 2 },
  { "Game" , 0x80},
  { "Test" , 0x00},
  { NULL, 0}
};

static struct DSW_INFO nemo_dsw[] =
{
  { 0x2, 0xff, dsw_data_ffight_1 },
  { 0x4, 0xff, dsw_data_nemo_2 },
  { 0x6, 0x9f, dsw_data_nemo_3 },
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

static struct DSW_INFO sf2_dsw[] =
{
  { 0x2, 0xff, dsw_data_ffight_1 },
  { 0x4, 0xff, dsw_data_sf2_2 },
  { 0x6, 0x9f, dsw_data_unsquad_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_sf2j_2[] =
{
	CPS1_DIFFICULTY_1
  { "2 Players Game", 0x08, 2 },
  { "1 Credit/No Continue" , 0x08},
  { "2 Credits/Winner Continue" , 0x00},
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
  { "Freeze", 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  DSW_SCREEN( 0x10, 0x00),
  DSW_DEMO_SOUND( 0x00, 0x20),
  { "Allow Continue", 0x40, 2 },
  { MSG_NO, 0x40},
  { MSG_YES, 0x00},
  { "Game Mode",    0x80, 2 },
  { "Game" , 0x80},
  { "Test" , 0x00},
  { NULL, 0}
};

static struct DSW_INFO sf2j_dsw[] =
{
  { 0x2, 0xff, dsw_data_ffight_1 },
  { 0x4, 0xff, dsw_data_sf2j_2 },
  { 0x6, 0x9f, dsw_data_sf2j_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data__3wonders_1[] =
{
	CPS1_COINAGE_1
  { "2 Coins to Start,1 to Continue", 0x40, 2 },
  { MSG_OFF, 0x40},
  { MSG_ON, 0x00},
  { "Freeze", 0x80, 2 },
  { MSG_OFF, 0x80},
  { MSG_ON, 0x00},
  { NULL, 0}
};

static struct DSW_DATA dsw_data__3wonders_2[] =
{
  { "Lives (Midnight Wanderers)", 0x03, 4 },
  { "1" , 0x03},
  { "2" , 0x02},
  { "3" , 0x01},
  { "5" , 0x00},
  { "Difficulty (Midnight Wanderers)", 0x0c, 4 },
  { "Easy" , 0x0c},
  { "Normal" , 0x08},
  { "Hard" , 0x04},
  { "Hardest" , 0x00},
  { "Lives (Chariot)", 0x30, 4 },
  { "1" , 0x30},
  { "2" , 0x20},
  { "3" , 0x10},
  { "5" , 0x00},
  { "Difficulty (Chariot)", 0xc0, 4 },
  { "Easy" , 0xc0},
  { "Normal" , 0x80},
  { "Hard" , 0x40},
  { "Hardest" , 0x00},
  { NULL, 0}
};

static struct DSW_DATA dsw_data__3wonders_3[] =
{
  { "Lives (Don´t Pull)", 0x03, 4 },
  { "1" , 0x03},
  { "2" , 0x02},
  { "3" , 0x01},
  { "5" , 0x00},
  { "Difficulty (Don´t Pull)", 0x0c, 4 },
  { "Easy" , 0x0c},
  { "Normal" , 0x08},
  { "Hard" , 0x04},
  { "Hardest" , 0x00},
  DSW_SCREEN( 0x10, 0x00),
  DSW_DEMO_SOUND( 0x00, 0x20),
  { "Allow Continue", 0x40, 2 },
  { MSG_NO, 0x40},
  { MSG_YES, 0x00},
  { "Game Mode",    0x80, 2 },
  { "Game" , 0x80},
  { "Test" , 0x00},
  { NULL, 0}
};

static struct DSW_INFO _3wonders_dsw[] =
{
  { 0x2, 0xff, dsw_data__3wonders_1 },
  { 0x4, 0x9a, dsw_data__3wonders_2 },
  { 0x6, 0x99, dsw_data__3wonders_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_kod_1[] =
{
	CPS1_COINAGE_2
  { "Coin Slots", 0x08, 2 },
  { "1" , 0x00},
  { "3" , 0x08},
  { "Max Players", 0x10, 2 },
  { "2" , 0x00},
  { "3" , 0x10},
  DSW_UNUSED( 0x00, 0x20),
  { "2 Coins to Start,1 to Continue", 0x40, 2 },
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
  { "80k and every 400k" , 0x80},
  { "100k and every 450k" , 0xc0},
  { "160k and every 450k" , 0x40},
  { "None" , 0x00},
  { NULL, 0}
};

static struct DSW_INFO kod_dsw[] =
{
  { 0x2, 0xff, dsw_data_kod_1 },
  { 0x4, 0xff, dsw_data_kod_2 },
  { 0x6, 0x9f, dsw_data_unsquad_3 },
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
  { "80k and every 400k" , 0x80},
  { "200k and every 450k" , 0xc0},
  { "160k and every 450k" , 0x40},
  { "None" , 0x00},
  { NULL, 0}
};

static struct DSW_INFO kodj_dsw[] =
{
  { 0x2, 0xff, dsw_data_kod_1 },
  { 0x4, 0xff, dsw_data_kodj_2 },
  { 0x6, 0x9f, dsw_data_unsquad_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_captcomm_2[] =
{
  { "Difficulty 1", 0x07, 8 },
  { "1 (Easiest)" , 0x07},
  { "2" , 0x06},
  { "3" , 0x05},
  { "4 (Normal)" , 0x04},
  { "5" , 0x03},
  { "6" , 0x02},
  { "7" , 0x01},
  { "8 (Hardest)" , 0x00},
  { "Difficulty 2", 0x18, 4 },
  { "Easy" , 0x18},
  { "Normal" , 0x10},
  { "Hard" , 0x08},
  { "Hardest" , 0x00},
  DSW_UNUSED( 0x00, 0x20),
  { "Max Players", 0xc0, 4 },
  { "1" , 0x40},
  { "2" , 0xc0},
  { "3" , 0x80},
  { "4" , 0x00},
  { NULL, 0}
};

static struct DSW_INFO captcomm_dsw[] =
{
  { 0x2, 0xff, dsw_data_mercs_1 },
  { 0x4, 0xf4, dsw_data_captcomm_2 },
  { 0x6, 0x9f, dsw_data_ffight_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_knights_2[] =
{
  { "Enemy's attack frequency", 0x07, 8 },
  { "1 (Easiest)" , 0x07},
  { "2" , 0x06},
  { "3" , 0x05},
  { "4 (Normal)" , 0x04},
  { "5" , 0x03},
  { "6" , 0x02},
  { "7" , 0x01},
  { "8 (Hardest)" , 0x00},
  { "Enemy's attack power", 0x38, 8 },
  { "1 (Easiest)" , 0x10},
  { "2" , 0x08},
  { "3" , 0x00},
  { "4 (Normal)" , 0x38},
  { "5" , 0x30},
  { "6" , 0x28},
  { "7" , 0x20},
  { "8 (Hardest)" , 0x18},
  { "Coin Slots", 0x40, 2 },
  { "1" , 0x00},
  { "3" , 0x40},
  { "Max Players", 0x80, 2 },
  { "2" , 0x00},
  { "3" , 0x80},
  { NULL, 0}
};

static struct DSW_INFO knights_dsw[] =
{
  { 0x2, 0xff, dsw_data_mercs_1 },
  { 0x4, 0xfc, dsw_data_knights_2 },
  { 0x6, 0x9f, dsw_data_ffight_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_varth_2[] =
{
	CPS1_DIFFICULTY_1
  { MSG_EXTRA_LIFE, 0x18, 4 },
  { "600k and every 1.400k" , 0x18},
  { "600k 2.000k and 4500k" , 0x10},
  { "1.200k 3.500k" , 0x08},
  { "2000k only" , 0x00},
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
  { "Freeze", 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  DSW_SCREEN( 0x10, 0x00),
  DSW_DEMO_SOUND( 0x00, 0x20),
  { "Allow Continue", 0x40, 2 },
  { MSG_NO, 0x40},
  { MSG_YES, 0x00},
  { "Game Mode",    0x80, 2 },
  { "Game" , 0x80},
  { "Test" , 0x00},
  { NULL, 0}
};

static struct DSW_INFO varth_dsw[] =
{
  { 0x2, 0xff, dsw_data_ffight_1 },
  { 0x4, 0xff, dsw_data_varth_2 },
  { 0x6, 0x9f, dsw_data_varth_3 },
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
  { "2 Coins to Start,1 to Continue", 0x40, 2 },
  { MSG_OFF, 0x40},
  { MSG_ON, 0x00},
  { "Extended Test Mode", 0x80, 2 },
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
  { "Extend", 0x18, 3 },
  { "N" , 0x18},
  { "E" , 0x10},
  { "D" , 0x00},
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
  { "Freeze", 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  DSW_SCREEN( 0x10, 0x00),
  DSW_DEMO_SOUND( 0x00, 0x20),
  { "Allow Continue", 0x40, 2 },
  { MSG_NO, 0x00},
  { MSG_YES, 0x40},
  { "Game Mode",    0x80, 2 },
  { "Game" , 0x80},
  { "Test" , 0x00},
  { NULL, 0}
};

static struct DSW_INFO cworld2j_dsw[] =
{
  { 0x2, 0xff, dsw_data_cworld2j_1 },
  { 0x4, 0xfe, dsw_data_cworld2j_2 },
  { 0x6, 0xdf, dsw_data_cworld2j_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_wof_3[] =
{
  { "Freeze", 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  { NULL, 0}
};

static struct DSW_DATA dsw_data_dino_3[] =
{
  { "Freeze", 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  { "TEST MODE", 0x80, 2 },
  { MSG_OFF, 0x80},
  { MSG_ON, 0x00},
  { NULL, 0}
};

static struct DSW_INFO wof_dsw[] =
{
  { 0x6, 0x08, dsw_data_wof_3 },
  { 0, 0, NULL }
};

static struct DSW_INFO dino_dsw[] =
{
  { 0x6, 0x88, dsw_data_dino_3 },
  { 0, 0, NULL }
};

static struct DSW_INFO punisher_dsw[] =
{
  { 0x6, 0x08, dsw_data_wof_3 },
  { 0, 0, NULL }
};

static struct DSW_INFO slammast_dsw[] =
{
  { 0x6, 0x08, dsw_data_wof_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_pnickj_1[] =
{
	CPS1_COINAGE_2
  { "Coin Slots", 0x08, 2 },
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
  { "Vs Play Mode", 0xc0, 4 },
  { "1 Game Match" , 0xc0},
  { "3 Games Match" , 0x80},
  { "5 Games Match" , 0x40},
  { "7 Games Match" , 0x00},
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
  { "Freeze", 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  DSW_SCREEN( 0x10, 0x00),
  DSW_DEMO_SOUND( 0x00, 0x20),
  { MSG_UNKNOWN, 0x40, 2 },
  { MSG_OFF, 0x40},
  { MSG_ON, 0x00},
  { "Game Mode",    0x80, 2 },
  { "Game" , 0x80},
  { "Test" , 0x00},
  { NULL, 0}
};

static struct DSW_INFO pnickj_dsw[] =
{
  { 0x2, 0xff, dsw_data_pnickj_1 },
  { 0x4, 0xff, dsw_data_pnickj_2 },
  { 0x6, 0xdf, dsw_data_pnickj_3 },
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
  { "2 Coins to Start,1 to Continue", 0x40, 2 },
  { MSG_OFF, 0x40},
  { MSG_ON, 0x00},
  DSW_UNUSED( 0x00, 0x80),
  { NULL, 0}
};

static struct DSW_DATA dsw_data_qad_2[] =
{
  { MSG_DIFFICULTY, 0x07},
  { "Wisdom", 0x18, 4 },
  { "Easy" , 0x18},
  { "Normal" , 0x10},
  { "Hard" , 0x08},
  { "Hardest" , 0x00},
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
  { "Freeze", 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  DSW_SCREEN( 0x10, 0x00),
  DSW_DEMO_SOUND( 0x20, 0x00),
  { "Allow Continue", 0x40, 2 },
  { MSG_NO, 0x00},
  { MSG_YES, 0x40},
  { "Game Mode",    0x80, 2 },
  { "Game" , 0x80},
  { "Test" , 0x00},
  { NULL, 0}
};

static struct DSW_INFO qad_dsw[] =
{
  { 0x2, 0xff, dsw_data_qad_1 },
  { 0x4, 0xf4, dsw_data_qad_2 },
  { 0x6, 0xff, dsw_data_qad_3 },
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

static struct DSW_INFO qadj_dsw[] =
{
  { 0x2, 0xff, dsw_data_qad_1 },
  { 0x4, 0xff, dsw_data_qadj_2 },
  { 0x6, 0xdf, dsw_data_cworld2j_3 },
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
  { "2 Coins to Start,1 to Continue", 0x40, 2 },
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
  { "Infinite Lives", 0x02, 2 },
  { MSG_OFF, 0x02},
  { MSG_ON, 0x00},
  { MSG_FREE_PLAY, 0x04, 2 },
  { MSG_OFF, 0x04},
  { MSG_ON, 0x00},
  { "Freeze", 0x08, 2 },
  { MSG_OFF, 0x08},
  { MSG_ON, 0x00},
  DSW_SCREEN( 0x10, 0x00),
  DSW_DEMO_SOUND( 0x00, 0x20),
  { "Allow Continue", 0x40, 2 },
  { MSG_NO, 0x00},
  { MSG_YES, 0x40},
  { "Game Mode",    0x80, 2 },
  { "Game" , 0x80},
  { "Test" , 0x00},
  { NULL, 0}
};

static struct DSW_INFO qtono2_dsw[] =
{
  { 0x2, 0xff, dsw_data_qtono2_1 },
  { 0x4, 0xff, dsw_data_qtono2_2 },
  { 0x6, 0xdf, dsw_data_qtono2_3 },
  { 0, 0, NULL }
};

static struct DSW_INFO pang3_dsw[] =
{
  { 0x6, 0x08, dsw_data_wof_3 },
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
  { "2 Coins to Start1 to Continue" , 0x0e},
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
  { "Coin slots", 0x60},
  { MSG_UNKNOWN, 0x80, 2 },
  { MSG_OFF, 0x80},
  { MSG_ON, 0x00},
  { NULL, 0}
};

static struct DSW_DATA dsw_data_megaman_2[] =
{
  { MSG_DIFFICULTY, 0x03, 4 },
  { "Easy" , 0x03},
  { "Normal" , 0x02},
  { "Hard" , 0x01},
  { "Hardest" , 0x00},
  { "Time", 0x0c, 4 },
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
  { "Voice", 0x40, 2 },
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
  { "Allow Continue", 0x04, 2 },
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
  { "Game Mode",    0x80, 2 },
  { "Game" , 0x80},
  { "Test" , 0x00},
  { NULL, 0}
};

static struct DSW_INFO megaman_dsw[] =
{
  { 0x2, 0xff, dsw_data_megaman_1 },
  { 0x4, 0xfe, dsw_data_megaman_2 },
  { 0x6, 0xff, dsw_data_megaman_3 },
  { 0, 0, NULL }
};

static struct DSW_DATA dsw_data_rockmanj_2[] =
{
  { MSG_DIFFICULTY, 0x03, 4 },
  { "Easy" , 0x03},
  { "Normal" , 0x02},
  { "Hard" , 0x01},
  { "Hardest" , 0x00},
  { "Time", 0x0c, 4 },
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

static struct DSW_INFO rockmanj_dsw[] =
{
  { 0x2, 0xff, dsw_data_megaman_1 },
  { 0x4, 0xfe, dsw_data_rockmanj_2 },
  { 0x6, 0xff, dsw_data_megaman_3 },
  { 0, 0, NULL }
};


// And finally inputs : took the longest one...

static struct INPUT_INFO cps1_inputs[] = // 4 players, 3 buttons
{
  { KB_DEF_COIN1, MSG_COIN1, 0x00, 0x01, BIT_ACTIVE_0 },
  { KB_DEF_COIN2, MSG_COIN2, 0x00, 0x02, BIT_ACTIVE_0 },
  { KB_DEF_SERVICE, MSG_SERVICE, 0x00, 0x04, BIT_ACTIVE_0 },
  { KB_DEF_TEST, MSG_TEST, 0x00, 0x40, BIT_ACTIVE_0 },
  { KB_DEF_P1_START, MSG_P1_START, 0x00, 0x10, BIT_ACTIVE_0 },
  { KB_DEF_P2_START, MSG_P2_START, 0x00, 0x20, BIT_ACTIVE_0 },

  { KB_DEF_P1_RIGHT, MSG_P1_RIGHT, 0x08, 0x01, BIT_ACTIVE_0 },
  { KB_DEF_P1_LEFT, MSG_P1_LEFT, 0x08, 0x02, BIT_ACTIVE_0 },
  { KB_DEF_P1_DOWN, MSG_P1_DOWN, 0x08, 0x04, BIT_ACTIVE_0 },
  { KB_DEF_P1_UP, MSG_P1_UP, 0x08, 0x08, BIT_ACTIVE_0 },
  { KB_DEF_P1_B1, MSG_P1_B1, 0x08, 0x10, BIT_ACTIVE_0 },
  { KB_DEF_P1_B2, MSG_P1_B2, 0x08, 0x20, BIT_ACTIVE_0 },
  { KB_DEF_P1_B3, MSG_P1_B3, 0x08, 0x40, BIT_ACTIVE_0 },
  { KB_DEF_P3_B3, MSG_P3_B3, 0x08, 0x80, BIT_ACTIVE_0 },
  { KB_DEF_P2_RIGHT, MSG_P2_RIGHT, 0x09, 0x01, BIT_ACTIVE_0 },
  { KB_DEF_P2_LEFT, MSG_P2_LEFT, 0x09, 0x02, BIT_ACTIVE_0 },
  { KB_DEF_P2_DOWN, MSG_P2_DOWN, 0x09, 0x04, BIT_ACTIVE_0 },
  { KB_DEF_P2_UP, MSG_P2_UP, 0x09, 0x08, BIT_ACTIVE_0 },
  { KB_DEF_P2_B1, MSG_P2_B1, 0x09, 0x10, BIT_ACTIVE_0 },
  { KB_DEF_P2_B2, MSG_P2_B2, 0x09, 0x20, BIT_ACTIVE_0 },
  { KB_DEF_P2_B3, MSG_P2_B3, 0x09, 0x40, BIT_ACTIVE_0 },
  { KB_DEF_P4_B3, MSG_P3_B3, 0x09, 0x80, BIT_ACTIVE_0 },

  { KB_DEF_P3_RIGHT, MSG_P3_RIGHT, 0x0a, 0x01, BIT_ACTIVE_0 },
  { KB_DEF_P3_LEFT, MSG_P3_LEFT, 0x0a, 0x02, BIT_ACTIVE_0 },
  { KB_DEF_P3_DOWN, MSG_P3_DOWN, 0x0a, 0x04, BIT_ACTIVE_0 },
  { KB_DEF_P3_UP, MSG_P3_UP, 0x0a, 0x08, BIT_ACTIVE_0 },
  { KB_DEF_P3_B1, MSG_P3_B1, 0x0a, 0x10, BIT_ACTIVE_0 },
  { KB_DEF_P3_B2, MSG_P3_B2, 0x0a, 0x20, BIT_ACTIVE_0 },
  { KB_DEF_COIN3, MSG_COIN3, 0x0a, 0x40, BIT_ACTIVE_0 },
  { KB_DEF_P3_START, MSG_P3_START, 0x0a, 0x80, BIT_ACTIVE_0 },

  { KB_DEF_P4_RIGHT, MSG_P4_RIGHT, 0x0c, 0x01, BIT_ACTIVE_0 },
  { KB_DEF_P4_LEFT, MSG_P4_LEFT, 0x0c, 0x02, BIT_ACTIVE_0 },
  { KB_DEF_P4_DOWN, MSG_P4_DOWN, 0x0c, 0x04, BIT_ACTIVE_0 },
  { KB_DEF_P4_UP, MSG_P4_UP, 0x0c, 0x08, BIT_ACTIVE_0 },
  { KB_DEF_P4_B1, MSG_P4_B1, 0x0c, 0x10, BIT_ACTIVE_0 },
  { KB_DEF_P4_B2, MSG_P4_B2, 0x0c, 0x20, BIT_ACTIVE_0 },
  { KB_DEF_COIN4, MSG_COIN4, 0x0c, 0x40, BIT_ACTIVE_0 },
  { KB_DEF_P4_START, MSG_P4_START, 0x0c, 0x80, BIT_ACTIVE_0 },
   { 0, NULL,        0,        0,    0            },
};

static struct INPUT_INFO forgottn_inputs[] =
{
  { KB_DEF_COIN1, MSG_COIN1, 0x00, 0x01, BIT_ACTIVE_0 },
  { KB_DEF_COIN2, MSG_COIN2, 0x00, 0x02, BIT_ACTIVE_0 },
  { KB_DEF_SERVICE, MSG_SERVICE, 0x00, 0x04, BIT_ACTIVE_0 },
  { KB_DEF_TEST, MSG_TEST, 0x00, 0x40, BIT_ACTIVE_0 },
  { KB_DEF_P1_START, MSG_P1_START, 0x00, 0x10, BIT_ACTIVE_0 },
  { KB_DEF_P2_START, MSG_P2_START, 0x00, 0x20, BIT_ACTIVE_0 },

  { KB_DEF_P1_RIGHT, MSG_P1_RIGHT, 0x08, 0x01, BIT_ACTIVE_0 },
  { KB_DEF_P1_LEFT, MSG_P1_LEFT, 0x08, 0x02, BIT_ACTIVE_0 },
  { KB_DEF_P1_DOWN, MSG_P1_DOWN, 0x08, 0x04, BIT_ACTIVE_0 },
  { KB_DEF_P1_UP, MSG_P1_UP, 0x08, 0x08, BIT_ACTIVE_0 },
  { KB_DEF_P1_B1, MSG_P1_B1, 0x08, 0x10, BIT_ACTIVE_0 },

  { KB_DEF_B1_L, MSG_B1_L, 0x08, 0x20, BIT_ACTIVE_0 },
  { KB_DEF_B1_R, MSG_B1_R, 0x08, 0x40, BIT_ACTIVE_0 },

  { KB_DEF_P2_RIGHT, MSG_P2_RIGHT, 0x09, 0x01, BIT_ACTIVE_0 },
  { KB_DEF_P2_LEFT, MSG_P2_LEFT, 0x09, 0x02, BIT_ACTIVE_0 },
  { KB_DEF_P2_DOWN, MSG_P2_DOWN, 0x09, 0x04, BIT_ACTIVE_0 },
  { KB_DEF_P2_UP, MSG_P2_UP, 0x09, 0x08, BIT_ACTIVE_0 },
  { KB_DEF_P2_B1, MSG_P2_B1, 0x09, 0x10, BIT_ACTIVE_0 },
  { KB_DEF_P2_B2, MSG_P2_B2, 0x09, 0x20, BIT_ACTIVE_0 },
  { KB_DEF_P2_B3, MSG_P2_B3, 0x09, 0x40, BIT_ACTIVE_0 },
   { 0, NULL,        0,        0,    0            },
};

static struct INPUT_INFO sf2_inputs[] = // cps1, 6 buttons
{
  { KB_DEF_COIN1, MSG_COIN1, 0x00, 0x01, BIT_ACTIVE_0 },
  { KB_DEF_COIN2, MSG_COIN2, 0x00, 0x02, BIT_ACTIVE_0 },
  { KB_DEF_SERVICE, MSG_SERVICE, 0x00, 0x04, BIT_ACTIVE_0 },
  { KB_DEF_P1_START, MSG_P1_START, 0x00, 0x10, BIT_ACTIVE_0 },
  { KB_DEF_P2_START, MSG_P2_START, 0x00, 0x20, BIT_ACTIVE_0 },

  { KB_DEF_P1_RIGHT, MSG_P1_RIGHT, 0x08, 0x01, BIT_ACTIVE_0 },
  { KB_DEF_P1_LEFT, MSG_P1_LEFT, 0x08, 0x02, BIT_ACTIVE_0 },
  { KB_DEF_P1_DOWN, MSG_P1_DOWN, 0x08, 0x04, BIT_ACTIVE_0 },
  { KB_DEF_P1_UP, MSG_P1_UP, 0x08, 0x08, BIT_ACTIVE_0 },
  { KB_DEF_P1_B1, MSG_P1_B1, 0x08, 0x10, BIT_ACTIVE_0 },
  { KB_DEF_P1_B2, MSG_P1_B2, 0x08, 0x20, BIT_ACTIVE_0 },
  { KB_DEF_P1_B3, MSG_P1_B3, 0x08, 0x40, BIT_ACTIVE_0 },
  { KB_DEF_P2_RIGHT, MSG_P2_RIGHT, 0x09, 0x01, BIT_ACTIVE_0 },
  { KB_DEF_P2_LEFT, MSG_P2_LEFT, 0x09, 0x02, BIT_ACTIVE_0 },
  { KB_DEF_P2_DOWN, MSG_P2_DOWN, 0x09, 0x04, BIT_ACTIVE_0 },
  { KB_DEF_P2_UP, MSG_P2_UP, 0x09, 0x08, BIT_ACTIVE_0 },
  { KB_DEF_P2_B1, MSG_P2_B1, 0x09, 0x10, BIT_ACTIVE_0 },
  { KB_DEF_P2_B2, MSG_P2_B2, 0x09, 0x20, BIT_ACTIVE_0 },
  { KB_DEF_P2_B3, MSG_P2_B3, 0x09, 0x40, BIT_ACTIVE_0 },

  { KB_DEF_P1_B4, MSG_P1_B4, 0x0a, 0x01, BIT_ACTIVE_0 },
  { KB_DEF_P1_B5, MSG_P1_B5, 0x0a, 0x02, BIT_ACTIVE_0 },
  { KB_DEF_P1_B6, MSG_P1_B6, 0x0a, 0x04, BIT_ACTIVE_0 },
  { KB_DEF_P2_B4, MSG_P2_B4, 0x0a, 0x10, BIT_ACTIVE_0 },
  { KB_DEF_P2_B5, MSG_P2_B5, 0x0a, 0x20, BIT_ACTIVE_0 },
  { KB_DEF_P2_B6, MSG_P2_B6, 0x0a, 0x40, BIT_ACTIVE_0 },
   { 0, NULL,        0,        0,    0            },
};

static struct INPUT_INFO cps1b4_inputs[] = // cps1, 4 buttons
{
  { KB_DEF_COIN1, MSG_COIN1, 0x00, 0x01, BIT_ACTIVE_0 },
  { KB_DEF_COIN2, MSG_COIN2, 0x00, 0x02, BIT_ACTIVE_0 },
  { KB_DEF_COIN3, MSG_COIN3, 0x00, 0x04, BIT_ACTIVE_0 },
  { KB_DEF_COIN4, MSG_COIN4, 0x00, 0x08, BIT_ACTIVE_0 },
  { KB_DEF_P1_START, MSG_P1_START, 0x00, 0x10, BIT_ACTIVE_0 },
  { KB_DEF_P2_START, MSG_P2_START, 0x00, 0x20, BIT_ACTIVE_0 },
  { KB_DEF_P3_START, MSG_P3_START, 0x00, 0x40, BIT_ACTIVE_0 },
  { KB_DEF_P4_START, MSG_P4_START, 0x00, 0x80, BIT_ACTIVE_0 },

  { KB_DEF_P1_RIGHT, MSG_P1_RIGHT, 0x08, 0x01, BIT_ACTIVE_0 },
  { KB_DEF_P1_LEFT, MSG_P1_LEFT, 0x08, 0x02, BIT_ACTIVE_0 },
  { KB_DEF_P1_DOWN, MSG_P1_DOWN, 0x08, 0x04, BIT_ACTIVE_0 },
  { KB_DEF_P1_UP, MSG_P1_UP, 0x08, 0x08, BIT_ACTIVE_0 },
  { KB_DEF_P1_B1, MSG_P1_B1, 0x08, 0x10, BIT_ACTIVE_0 },
  { KB_DEF_P1_B2, MSG_P1_B2, 0x08, 0x20, BIT_ACTIVE_0 },
  { KB_DEF_P1_B3, MSG_P1_B3, 0x08, 0x40, BIT_ACTIVE_0 },
  { KB_DEF_P1_B4, MSG_P1_B4, 0x08, 0x80, BIT_ACTIVE_0 },
  { KB_DEF_P2_RIGHT, MSG_P2_RIGHT, 0x09, 0x01, BIT_ACTIVE_0 },
  { KB_DEF_P2_LEFT, MSG_P2_LEFT, 0x09, 0x02, BIT_ACTIVE_0 },
  { KB_DEF_P2_DOWN, MSG_P2_DOWN, 0x09, 0x04, BIT_ACTIVE_0 },
  { KB_DEF_P2_UP, MSG_P2_UP, 0x09, 0x08, BIT_ACTIVE_0 },
  { KB_DEF_P2_B1, MSG_P2_B1, 0x09, 0x10, BIT_ACTIVE_0 },
  { KB_DEF_P2_B2, MSG_P2_B2, 0x09, 0x20, BIT_ACTIVE_0 },
  { KB_DEF_P2_B3, MSG_P2_B3, 0x09, 0x40, BIT_ACTIVE_0 },
  { KB_DEF_P2_B4, MSG_P2_B4, 0x09, 0x80, BIT_ACTIVE_0 },
   { 0, NULL,        0,        0,    0            },
};

static struct INPUT_INFO sfzch_inputs[] = // alternate 6 buttons
// The changer version has no coins input. It's used for a button instead.
{
  { KB_DEF_P1_START, MSG_P1_START, 0x00, 0x10, BIT_ACTIVE_0 },
  { KB_DEF_P2_START, MSG_P2_START, 0x00, 0x20, BIT_ACTIVE_0 },

  { KB_DEF_P1_RIGHT, MSG_P1_RIGHT, 0x08, 0x01, BIT_ACTIVE_0 },
  { KB_DEF_P1_LEFT, MSG_P1_LEFT, 0x08, 0x02, BIT_ACTIVE_0 },
  { KB_DEF_P1_DOWN, MSG_P1_DOWN, 0x08, 0x04, BIT_ACTIVE_0 },
  { KB_DEF_P1_UP, MSG_P1_UP, 0x08, 0x08, BIT_ACTIVE_0 },
  { KB_DEF_P1_B1, MSG_P1_B1, 0x08, 0x10, BIT_ACTIVE_0 },
  { KB_DEF_P1_B2, MSG_P1_B2, 0x08, 0x20, BIT_ACTIVE_0 },
  { KB_DEF_P1_B3, MSG_P1_B3, 0x08, 0x40, BIT_ACTIVE_0 },
  { KB_DEF_P2_RIGHT, MSG_P2_RIGHT, 0x09, 0x01, BIT_ACTIVE_0 },
  { KB_DEF_P2_LEFT, MSG_P2_LEFT, 0x09, 0x02, BIT_ACTIVE_0 },
  { KB_DEF_P2_DOWN, MSG_P2_DOWN, 0x09, 0x04, BIT_ACTIVE_0 },
  { KB_DEF_P2_UP, MSG_P2_UP, 0x09, 0x08, BIT_ACTIVE_0 },
  { KB_DEF_P2_B1, MSG_P2_B1, 0x09, 0x10, BIT_ACTIVE_0 },
  { KB_DEF_P2_B2, MSG_P2_B2, 0x09, 0x20, BIT_ACTIVE_0 },
  { KB_DEF_P2_B3, MSG_P2_B3, 0x09, 0x40, BIT_ACTIVE_0 },

  { KB_DEF_P1_B4, MSG_P1_B4, 0x08, 0x80, BIT_ACTIVE_0 },
  { KB_DEF_P1_B5, MSG_P1_B5, 0x00, 0x01, BIT_ACTIVE_0 },
  { KB_DEF_P1_B6, MSG_P1_B6, 0x00, 0x40, BIT_ACTIVE_0 },
  { KB_DEF_P2_B4, MSG_P2_B4, 0x09, 0x80, BIT_ACTIVE_0 },
  { KB_DEF_P2_B5, MSG_P2_B5, 0x00, 0x02, BIT_ACTIVE_0 },
  { KB_DEF_P2_B6, MSG_P2_B6, 0x00, 0x80, BIT_ACTIVE_0 },
   { 0, NULL,        0,        0,    0            },
};

// Stuff...

static struct GFX_LIST cps1_gfx[] =
{
  { REGION_GFX1, NULL }, //&cps1_object, },
   { 0,           NULL,               },
};

// in fact the video is 512x256 with a border of 16 pixels in height
// and 64 pixels for the width !
struct VIDEO_INFO cps1_video =
{
   draw_cps1,
   384,
   224,
   32,
   VIDEO_ROTATE_NORMAL |
   VIDEO_ROTATABLE,
   cps1_gfx,
};

struct VIDEO_INFO cps1_video_270 =
{
   draw_cps1,
   384,
   224,
   32,
   VIDEO_ROTATE_270 |
   VIDEO_ROTATABLE,
   cps1_gfx,
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

static struct SOUND_INFO cps1_sound[] =
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

struct SOUND_INFO qsound_sound[2] =
{
   { SOUND_QSOUND, &qsound_interface     },
   { 0,              NULL,                 },
};

static struct SOUND_INFO forgottn_sound[] =
{
   { SOUND_YM2151J,  &ym2151_interface,    },
   { SOUND_M6295, &okim6295_interface_6061     },
   { 0,              NULL,                 },
};

#define cps1_game(NAME,LONGNAME,YEAR,DSW,VIDEO,COMPANY,TYPE)	\
							\
static struct DIR_INFO _##NAME##_dirs[] =		\
{							\
   { #NAME, },						\
   { NULL, },						\
};							\
							\
GAME( NAME,						\
   _##NAME##_dirs,					\
   _##NAME##_roms,						\
   cps1_inputs,						\
   DSW,							\
   NULL,						\
							\
   load_cps1,						\
   NULL,						\
   VIDEO,						\
   execute_cps1_frame,					\
   #NAME,						\
   LONGNAME,						\
   NULL,						\
   COMPANY,					\
   NULL,						\
   YEAR,						\
   cps1_sound,						\
   TYPE							\
)

#define cps1_clone(NAME,LONGNAME,YEAR,DSW,VIDEO,COMPANY,TYPE,MYCLONE)	\
								\
static struct DIR_INFO _##NAME##_dirs[] =			\
{								\
   { #NAME, },							\
   { ROMOF( MYCLONE ) },					\
   { CLONEOF( MYCLONE ) },					\
   { NULL, },							\
};								\
								\
GAME( NAME,							\
   _##NAME##_dirs,						\
   _##NAME##_roms,							\
   cps1_inputs,							\
   DSW,								\
   NULL,							\
								\
   load_cps1,							\
   NULL,							\
   VIDEO,							\
   execute_cps1_frame,						\
   #NAME,							\
   LONGNAME,							\
   NULL,							\
   COMPANY,						\
   NULL,							\
   YEAR,							\
   cps1_sound,							\
   TYPE								\
)

// same thing with 4 buttons

#define cps1b4_game(NAME,LONGNAME,YEAR,DSW,COMPANY,TYPE)	\
							\
static struct DIR_INFO _##NAME##_dirs[] =		\
{							\
   { #NAME, },						\
   { NULL, },						\
};							\
							\
GAME( NAME,						\
   _##NAME##_dirs,					\
   _##NAME##_roms,						\
   cps1b4_inputs,					\
   DSW,							\
   NULL,						\
							\
   load_cps1,						\
   NULL,						\
   &cps1_video,						\
   execute_cps1_frame,					\
   #NAME,						\
   LONGNAME,						\
   NULL,						\
   COMPANY,					\
   NULL,						\
   YEAR,						\
   cps1_sound,						\
   TYPE							\
)

#define cps1b4_clone(NAME,LONGNAME,YEAR,DSW,COMPANY,TYPE,MYCLONE)	\
								\
static struct DIR_INFO _##NAME##_dirs[] =			\
{								\
   { #NAME, },							\
   { ROMOF( MYCLONE ) },					\
   { CLONEOF( MYCLONE ) },					\
   { NULL, },							\
};								\
								\
GAME( NAME,							\
   _##NAME##_dirs,						\
   _##NAME##_roms,							\
   cps1b4_inputs,						\
   DSW,								\
   NULL,							\
								\
   load_cps1,							\
   NULL,							\
   &cps1_video,							\
   execute_cps1_frame,						\
   #NAME,							\
   LONGNAME,							\
   NULL,							\
   COMPANY,						\
   NULL,							\
   YEAR,							\
   cps1_sound,							\
   TYPE								\
)

// Forgottn : only 1 game and 1 clone, and we need all this because of sound

#define forgottn_game(NAME,LONGNAME,YEAR,COMPANY,TYPE)	\
							\
static struct DIR_INFO _##NAME##_dirs[] =		\
{							\
   { #NAME, },						\
   { NULL, },						\
};							\
							\
GAME( NAME,						\
   _##NAME##_dirs,					\
   _##NAME##_roms,					\
   forgottn_inputs,					\
   forgottn_dsw,					\
   NULL,						\
							\
   load_cps1,						\
   NULL,						\
   &cps1_video,						\
   execute_cps1_frame,					\
   #NAME,						\
   LONGNAME,						\
   NULL,						\
   COMPANY,					\
   NULL,						\
   YEAR,						\
   forgottn_sound,					\
   TYPE							\
)

#define forgottn_clone(NAME,LONGNAME,YEAR,COMPANY,TYPE,CLONE)	\
							\
static struct DIR_INFO _##NAME##_dirs[] =		\
{							\
   { #NAME, },						\
   { ROMOF( CLONE ) },					\
   { CLONEOF( CLONE ) },				\
   { NULL, },						\
};							\
							\
GAME( NAME,						\
   _##NAME##_dirs,					\
   _##NAME##_roms,						\
   cps1_inputs,						\
   forgottn_dsw,						\
   NULL,						\
							\
   load_cps1,						\
   NULL,						\
   &cps1_video,						\
   execute_cps1_frame,					\
   #NAME,						\
   LONGNAME,						\
   NULL,						\
   COMPANY,					\
   NULL,						\
   YEAR,						\
   forgottn_sound,					\
   TYPE							\
)

#define sf2_game(NAME,LONGNAME,YEAR,DSW,COMPANY,TYPE)	\
							\
static struct DIR_INFO _##NAME##_dirs[] =		\
{							\
   { #NAME, },						\
   { NULL, },						\
};							\
							\
GAME( NAME,						\
   _##NAME##_dirs,					\
   _##NAME##_roms,						\
   sf2_inputs,						\
   DSW,							\
   NULL,						\
							\
   load_cps1,						\
   NULL,						\
   &cps1_video,						\
   execute_sf2_frame,					\
   #NAME,						\
   LONGNAME,						\
   NULL,						\
   COMPANY,					\
   NULL,						\
   YEAR,						\
   cps1_sound,						\
   TYPE							\
)

#define sf2_clone(NAME,LONGNAME,YEAR,DSW,COMPANY,TYPE,MYCLONE)	\
								\
static struct DIR_INFO _##NAME##_dirs[] =			\
{								\
   { #NAME, },							\
   { ROMOF( MYCLONE ) },					\
   { CLONEOF( MYCLONE ) },					\
   { NULL, },							\
};								\
								\
GAME( NAME,							\
   _##NAME##_dirs,						\
   _##NAME##_roms,							\
   sf2_inputs,							\
   DSW,								\
   NULL,							\
								\
   load_cps1,							\
   NULL,							\
   &cps1_video,							\
   execute_sf2_frame,						\
   #NAME,							\
   LONGNAME,							\
   NULL,							\
   COMPANY,						\
   NULL,							\
   YEAR,							\
   cps1_sound,							\
   TYPE								\
)

// qsound

#define qsound_game(NAME,LONGNAME,YEAR,DSW,COMPANY,TYPE)	\
							\
static struct DIR_INFO _##NAME##_dirs[] =		\
{							\
   { #NAME, },						\
   { NULL, },						\
};							\
							\
GAME( NAME,						\
   _##NAME##_dirs,					\
   _##NAME##_roms,						\
   cps1_inputs,						\
   DSW,							\
   NULL,						\
							\
   load_qsound,						\
   NULL,					\
   &cps1_video,						\
   execute_qsound_frame,				\
   #NAME,						\
   LONGNAME,						\
   NULL,						\
   COMPANY,					\
   NULL,						\
   YEAR,						\
   qsound_sound,					\
   TYPE							\
)

#define qsound_clone(NAME,LONGNAME,YEAR,DSW,COMPANY,TYPE,MYCLONE)	\
								\
static struct DIR_INFO _##NAME##_dirs[] =			\
{								\
   { #NAME, },							\
   { ROMOF( MYCLONE ) },					\
   { CLONEOF( MYCLONE ) },					\
   { NULL, },							\
};								\
								\
GAME( NAME,							\
   _##NAME##_dirs,						\
   _##NAME##_roms,							\
   cps1_inputs,							\
   DSW,								\
   NULL,							\
								\
   load_qsound,							\
   NULL,							\
   &cps1_video,							\
   execute_qsound_frame,					\
   #NAME,							\
   LONGNAME,							\
   NULL,							\
   COMPANY,						\
   NULL,							\
   YEAR,							\
   qsound_sound,						\
   TYPE								\
)

static struct ROMSW_DATA romsw_sfzch[] =
{
  { "Japan", 0x0},
  { "USA",0x2},
  { "Hispanic", 0x4},
  { "Asia/Brazil",0x6},
  { NULL,                    0    },
};

static struct ROMSW_INFO sfzch_romsw[] =
{
   { 0xf41, 0x02, romsw_sfzch },
   { 0xf47, 0x02, romsw_sfzch },
   { 0,        0,    NULL },
};

#define sfzch_game(GAMENAME,NAME,LONGNAME,YEAR,TYPE)	\
							\
static struct DIR_INFO _##GAMENAME##_dirs[] =		\
{							\
   { NAME, },						\
   { NULL, },						\
};							\
							\
GAME( GAMENAME,						\
   _##GAMENAME##_dirs,					\
   _##GAMENAME##_roms,					\
   sfzch_inputs,					\
   NULL,						\
   sfzch_romsw,						\
							\
   load_cps1,						\
   NULL,						\
   &cps1_video,						\
   execute_cps1_frame,					\
   NAME,						\
   LONGNAME,						\
   NULL,						\
   COMPANY_ID_CAPCOM,					\
   NULL,						\
   YEAR,						\
   cps1_sound,						\
   TYPE							\
)

forgottn_game( forgottn,  "Forgotten Worlds (US)", 1988, COMPANY_ID_CAPCOM, GAME_SHOOT);
forgottn_clone( lostwrld,  "Lost Worlds (Japan)", 1988, COMPANY_ID_CAPCOM, GAME_SHOOT, "forgottn" );
cps1_game( ghouls,  "Ghouls'n Ghosts (World)" , 1988, ghouls_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_PLATFORM );
cps1_clone( ghoulsu,  "Ghouls'n Ghosts (US)" , 1988, ghoulsu_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_PLATFORM, "ghouls" );
cps1_clone( daimakai,  "Dai Makai-Mura (Japan)" , 1988, daimakai_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_PLATFORM, "ghouls" );
cps1_clone( daimakr2,  "Dai Makai-Mura (Japan hack?)" , 1988, daimakai_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_PLATFORM, "ghouls" );
cps1_game( strider,  "Strider (US set 1)", 1989, strider_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_PLATFORM );
cps1_clone( stridrua,  "Strider (US set 2)", 1989, stridrua_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_PLATFORM, "strider" );
cps1_clone( striderj,  "Strider Hiryu (Japan set 1)", 1989, strider_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_PLATFORM, "strider" );
cps1_clone( stridrja,  "Strider Hiryu (Japan set 2)", 1989, strider_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_PLATFORM, "strider" );
cps1_game( dynwar,  "Dynasty Wars (US set 1)", 1989, dynwar_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_BEAT );
cps1_clone( dynwaru,  "Dynasty Wars (US set 2)", 1989, dynwar_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_BEAT, "dynwar" );
cps1_clone( dynwarj,  "Tenchi wo Kurau (Japan)", 1989, dynwar_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_BEAT, "dynwar" );
cps1_game( willow,  "Willow (US)", 1989, willow_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_PLATFORM );
cps1_clone( willowj,  "Willow (Japan, Japanese)", 1989, willow_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_PLATFORM, "willow" );
cps1_clone( willowje,  "Willow (Japan, English)", 1989, willow_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_PLATFORM, "willow" );
cps1_game( unsquad,  "U.N. Squadron (US)", 1989, unsquad_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_SHOOT );
cps1_clone( area88,  "Area 88 (Japan)", 1989, unsquad_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_SHOOT, "unsquad" );
cps1_game( ffight,  "Final Fight (World)", 1989, ffight_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_BEAT );
cps1_clone( ffightu,  "Final Fight (US)", 1989, ffight_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_BEAT, "ffight" );
cps1_clone( ffightua,  "Final Fight (US 900112)", 1989, ffight_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_BEAT, "ffight" );
cps1_clone( ffightub,  "Final Fight (US 900613)", 1989, ffight_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_BEAT, "ffight" );
cps1_clone( ffightj,  "Final Fight (Japan)", 1989, ffight_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_BEAT, "ffight" );
cps1_clone( ffightj1,  "Final Fight (Japan 900305)", 1989, ffight_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_BEAT, "ffight" );
cps1_clone( ffightj2,  "Final Fight (Japan 900112)", 1989, ffight_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_BEAT, "ffight" );
cps1_game( 1941,  "1941 - Counter Attack (World)", 1990, _1941_dsw, &cps1_video_270, COMPANY_ID_CAPCOM, GAME_SHOOT );
cps1_clone( 1941j,  "1941 - Counter Attack (Japan)", 1990, _1941_dsw, &cps1_video_270, COMPANY_ID_CAPCOM, GAME_SHOOT, "1941" );
cps1_game( mercs,  "Mercs (World 900302)" , 1990, mercs_dsw, &cps1_video_270, COMPANY_ID_CAPCOM, GAME_SHOOT );
cps1_clone( mercsu,  "Mercs (US 900302)", 1990, mercs_dsw, &cps1_video_270, COMPANY_ID_CAPCOM, GAME_SHOOT, "mercs" );
cps1_clone( mercsua,  "Mercs (US 900608)", 1990, mercs_dsw, &cps1_video_270, COMPANY_ID_CAPCOM, GAME_SHOOT, "mercs" );
cps1_clone( mercsj,  "Senjou no Ookami II (Japan 900302)", 1990, mercs_dsw, &cps1_video_270, COMPANY_ID_CAPCOM, GAME_SHOOT, "mercs" );
cps1_game( mtwins,  "Mega Twins (World 900619)", 1990, mtwins_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_PLATFORM );
cps1_clone( chikij,  "Chiki Chiki Boys (Japan 900619)", 1990, mtwins_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_PLATFORM, "mtwins" );
cps1_game( msword,  "Magic Sword - Heroic Fantasy (World 900725)" , 1990, msword_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_PLATFORM );
cps1_clone( mswordr1,  "Magic Sword - Heroic Fantasy (World 900623)" , 1990, msword_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_PLATFORM, "msword" );
cps1_clone( mswordu,  "Magic Sword - Heroic Fantasy (US 900725)" , 1990, msword_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_PLATFORM, "msword" );
cps1_clone( mswordj,  "Magic Sword (Japan 900623)" , 1990, msword_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_PLATFORM, "msword" );
cps1_game( cawing,  "Carrier Air Wing (World 901012)" , 1990, cawing_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_SHOOT );
cps1_clone( cawingr1,  "Carrier Air Wing (World 901009)" , 1990, cawing_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_SHOOT, "cawing" );
cps1_clone( cawingu,  "Carrier Air Wing (US 901012)", 1990, cawing_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_SHOOT, "cawing" );
cps1_clone( cawingj,  "U.S. Navy (Japan 901012)", 1990, cawing_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_SHOOT, "cawing" );
cps1_game( nemo,  "Nemo (World 901130)" , 1990, nemo_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_PLATFORM );
cps1_clone( nemoj,  "Nemo (Japan 901120)", 1990, nemo_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_PLATFORM, "nemo" );
sf2_game( sf2,  "Street Fighter II - The World Warrior (World 910522)" , 1991, sf2_dsw, COMPANY_ID_CAPCOM, GAME_BEAT );
sf2_clone( sf2eb,  "Street Fighter II - The World Warrior (World 910214)" , 1991, sf2_dsw, COMPANY_ID_CAPCOM, GAME_BEAT, "sf2" );
sf2_clone( sf2ua,  "Street Fighter II - The World Warrior (US 910206)", 1991, sf2_dsw, COMPANY_ID_CAPCOM, GAME_BEAT, "sf2" );
sf2_clone( sf2ub,  "Street Fighter II - The World Warrior (US 910214)", 1991, sf2_dsw, COMPANY_ID_CAPCOM, GAME_BEAT, "sf2" );
sf2_clone( sf2ud,  "Street Fighter II - The World Warrior (US 910318)", 1991, sf2_dsw, COMPANY_ID_CAPCOM, GAME_BEAT, "sf2" );
sf2_clone( sf2ue,  "Street Fighter II - The World Warrior (US 910228)", 1991, sf2_dsw, COMPANY_ID_CAPCOM, GAME_BEAT, "sf2" );
sf2_clone( sf2uf,  "Street Fighter II - The World Warrior (US 910411)", 1991, sf2_dsw, COMPANY_ID_CAPCOM, GAME_BEAT, "sf2" );
sf2_clone( sf2ui,  "Street Fighter II - The World Warrior (US 910522)", 1991, sf2_dsw, COMPANY_ID_CAPCOM, GAME_BEAT, "sf2" );
sf2_clone( sf2uk,  "Street Fighter II - The World Warrior (US 911101)", 1991, sf2_dsw, COMPANY_ID_CAPCOM, GAME_BEAT, "sf2" );
sf2_clone( sf2j,  "Street Fighter II - The World Warrior (Japan 911210)", 1991, sf2j_dsw, COMPANY_ID_CAPCOM, GAME_BEAT, "sf2" );
sf2_clone( sf2ja,  "Street Fighter II - The World Warrior (Japan 910214)", 1991, sf2j_dsw, COMPANY_ID_CAPCOM, GAME_BEAT, "sf2" );
sf2_clone( sf2jc,  "Street Fighter II - The World Warrior (Japan 910306)", 1991, sf2j_dsw, COMPANY_ID_CAPCOM, GAME_BEAT, "sf2" );
cps1_game( 3wonders,  "Three Wonders (World 910520)" , 1991, _3wonders_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_MISC );
cps1_clone( 3wonderu,  "Three Wonders (US 910520)", 1991, _3wonders_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_MISC, "3wonders" );
cps1_clone( wonder3,  "Wonder 3 (Japan 910520)", 1991, _3wonders_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_MISC, "3wonders" );
cps1_clone( 3wonderh, "Three Wonders (hack?)", 1991, _3wonders_dsw, &cps1_video, COMPANY_ID_BOOTLEG, GAME_MISC, "3wonders" );
cps1_game( kod,  "The King of Dragons (World 910711)" , 1991, kod_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_BEAT );
cps1_clone( kodu,  "The King of Dragons (US 910910)", 1991, kodj_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_BEAT, "kod" );
cps1_clone( kodj,  "The King of Dragons (Japan 910805)", 1991, kodj_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_BEAT, "kod" );
cps1_game( captcomm,  "Captain Commando (World 911014)" , 1991, captcomm_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_BEAT );
cps1_clone( captcomu,  "Captain Commando (US 910928)", 1991, captcomm_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_BEAT, "captcomm" );
cps1_clone( captcomj,  "Captain Commando (Japan 911202)", 1991, captcomm_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_BEAT, "captcomm" );
cps1_game( knights,  "Knights of the Round (World 911127)" , 1991, knights_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_BEAT );
cps1_clone( knightsu,  "Knights of the Round (US 911127)", 1991, knights_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_BEAT, "knights" );
cps1_clone( knightsj,  "Knights of the Round (Japan 911127)", 1991, knights_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_BEAT, "knights" );
sf2_game( sf2ce,  "Street Fighter II' - Champion Edition (World 920313)" , 1992, sf2_dsw, COMPANY_ID_CAPCOM, GAME_BEAT );
sf2_clone( sf2ceua,  "Street Fighter II' - Champion Edition (US 920313)", 1992, sf2_dsw, COMPANY_ID_CAPCOM, GAME_BEAT, "sf2ce" );
sf2_clone( sf2ceub,  "Street Fighter II' - Champion Edition (US 920513)", 1992, sf2_dsw, COMPANY_ID_CAPCOM, GAME_BEAT, "sf2ce" );
sf2_clone( sf2ceuc,  "Street Fighter II' - Champion Edition (US 920803)", 1992, sf2_dsw, COMPANY_ID_CAPCOM, GAME_BEAT, "sf2ce" );
sf2_clone( sf2cej,  "Street Fighter II' - Champion Edition (Japan 920513)", 1992, sf2_dsw, COMPANY_ID_CAPCOM, GAME_BEAT, "sf2ce" );
sf2_clone( sf2rb,  "Street Fighter II' - Champion Edition (Rainbow set 1, bootleg)", 1992, sf2_dsw, COMPANY_ID_BOOTLEG, GAME_BEAT, "sf2ce" );
sf2_clone( sf2rb2,  "Street Fighter II' - Champion Edition (Rainbow set 2, bootleg)", 1992, sf2_dsw, COMPANY_ID_BOOTLEG, GAME_BEAT, "sf2ce" );
sf2_clone( sf2rb3,  "Street Fighter II' - Champion Edition (Rainbow set 3, bootleg)", 1992, sf2_dsw, COMPANY_ID_BOOTLEG, GAME_BEAT, "sf2ce" );
sf2_clone( sf2red,  "Street Fighter II' - Champion Edition (Red Wave, bootleg)", 1992, sf2_dsw, COMPANY_ID_BOOTLEG, GAME_BEAT, "sf2ce" );
sf2_clone( sf2v004,  "Street Fighter II! - Champion Edition (V004, bootleg)", 1992, sf2_dsw, COMPANY_ID_BOOTLEG, GAME_BEAT, "sf2ce" );
sf2_clone( sf2accp2,  "Street Fighter II' - Champion Edition (Accelerator Pt.II, bootleg)", 1992, sf2_dsw, COMPANY_ID_BOOTLEG, GAME_BEAT, "sf2ce" );
sf2_clone( sf2m2,  "Street Fighter II' - Champion Edition (M2, bootleg)", 1992, sf2_dsw, COMPANY_ID_BOOTLEG, GAME_BEAT, "sf2ce" );
sf2_clone( sf2m4,  "Street Fighter II' - Champion Edition (M4, bootleg)", 1992, sf2_dsw, COMPANY_ID_BOOTLEG, GAME_BEAT, "sf2ce" );
sf2_clone( sf2m5,  "Street Fighter II' - Champion Edition (M5, bootleg)", 1992, sf2_dsw, COMPANY_ID_BOOTLEG, GAME_BEAT, "sf2ce" );
sf2_clone( sf2m6,  "Street Fighter II' - Champion Edition (M6, bootleg)", 1992, sf2_dsw, COMPANY_ID_BOOTLEG, GAME_BEAT, "sf2ce" );
sf2_clone( sf2m7,  "Street Fighter II' - Champion Edition (M7, bootleg)", 1992, sf2_dsw, COMPANY_ID_BOOTLEG, GAME_BEAT, "sf2ce" );
sf2_clone( sf2yyc,  "Street Fighter II' - Champion Edition (YYC, bootleg)", 1992, sf2_dsw, COMPANY_ID_BOOTLEG, GAME_BEAT, "sf2ce" );
sf2_clone( sf2koryu,  "Street Fighter II' - Champion Edition (Xiang Long, Chinese bootleg)", 1992, sf2_dsw, COMPANY_ID_BOOTLEG, GAME_BEAT, "sf2ce" );
cps1_game( varth,  "Varth - Operation Thunderstorm (World 920714)" , 1992, varth_dsw, &cps1_video_270, COMPANY_ID_CAPCOM, GAME_SHOOT );
cps1_clone( varthr1,  "Varth - Operation Thunderstorm (World 920612)" , 1992, varth_dsw, &cps1_video_270, COMPANY_ID_CAPCOM, GAME_SHOOT, "varth" );
cps1_clone( varthu,  "Varth - Operation Thunderstorm (US 920612)", 1992, varth_dsw, &cps1_video_270, COMPANY_ID_CAPCOM, GAME_SHOOT, "varth" );
cps1_clone( varthj,  "Varth - Operation Thunderstorm (Japan 920714)", 1992, varth_dsw, &cps1_video_270, COMPANY_ID_CAPCOM, GAME_SHOOT, "varth" );
cps1_game( cworld2j,  "Capcom World 2 (Japan 920611)", 1992, cworld2j_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_QUIZZ );
sf2_clone( sf2hf,  "Street Fighter II' - Hyper Fighting (World 921209)", 1992, sf2_dsw, COMPANY_ID_CAPCOM, GAME_BEAT, "sf2ce" );
sf2_clone( sf2t,  "Street Fighter II' - Hyper Fighting (US 921209)", 1992, sf2_dsw, COMPANY_ID_CAPCOM, GAME_BEAT, "sf2ce" );
sf2_clone( sf2tj,  "Street Fighter II' Turbo - Hyper Fighting (Japan 921209)", 1992, sf2j_dsw, COMPANY_ID_CAPCOM, GAME_BEAT, "sf2ce" );
cps1b4_game( qad,  "Quiz & Dragons (US 920701)", 1992, qad_dsw, COMPANY_ID_CAPCOM, GAME_QUIZZ );
cps1b4_clone( qadj,  "Quiz & Dragons (Japan 940921)", 1994, qadj_dsw, COMPANY_ID_CAPCOM, GAME_QUIZZ, "qad" );
cps1b4_game( qtono2,  "Quiz Tonosama no Yabou 2 Zenkoku-ban (Japan 950123)", 1995, qtono2_dsw, COMPANY_ID_CAPCOM, GAME_QUIZZ );
cps1_game( megaman,  "Mega Man - The Power Battle (CPS1 Asia 951006)", 1995, megaman_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_BEAT );
cps1_clone( rockmanj,  "Rockman - The Power Battle (CPS1 Japan 950922)", 1995, rockmanj_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_BEAT, "megaman" );
qsound_game( wof,  "Warriors of Fate (World 921002)" , 1992, wof_dsw, COMPANY_ID_CAPCOM, GAME_BEAT );
qsound_clone( wofa,  "Sangokushi II (Asia 921005)" , 1992, wof_dsw, COMPANY_ID_CAPCOM, GAME_BEAT, "wof" );
qsound_clone( wofu,  "Warriors of Fate (US 921031)" , 1992, wof_dsw, COMPANY_ID_CAPCOM, GAME_BEAT, "wof" );
qsound_clone( wofj,  "Tenchi wo Kurau II - Sekiheki no Tatakai (Japan 921031)", 1992, wof_dsw, COMPANY_ID_CAPCOM, GAME_BEAT, "wof" );
cps1_clone( wofhfh, "Sangokushi II: Huo Fenghuang (Chinese bootleg)", 1999, wofhfh_dsw, &cps1_video, COMPANY_ID_BOOTLEG, GAME_BEAT, "wof" );
qsound_game( dino,  "Cadillacs and Dinosaurs (World 930201)" , 1993, dino_dsw, COMPANY_ID_CAPCOM, GAME_BEAT );
qsound_clone( dinou,  "Cadillacs and Dinosaurs (US 930201)", 1993, dino_dsw, COMPANY_ID_CAPCOM, GAME_BEAT, "dino" );
qsound_clone( dinoj,  "Cadillacs Kyouryuu-Shinseiki (Japan 930201)", 1993, dino_dsw, COMPANY_ID_CAPCOM, GAME_BEAT, "dino" );
qsound_game( punisher,  "The Punisher (World 930422)" , 1993, punisher_dsw, COMPANY_ID_CAPCOM, GAME_BEAT );
qsound_clone( punishru,  "The Punisher (US 930422)", 1993, punisher_dsw, COMPANY_ID_CAPCOM, GAME_BEAT, "punisher" );
qsound_clone( punishrj,  "The Punisher (Japan 930422)", 1993, punisher_dsw, COMPANY_ID_CAPCOM, GAME_BEAT, "punisher" );
qsound_game( slammast,  "Saturday Night Slam Masters (World 930713)" , 1993, slammast_dsw, COMPANY_ID_CAPCOM, GAME_SPORTS );
qsound_clone( slammasu,  "Saturday Night Slam Masters (US 930713)", 1993, slammast_dsw, COMPANY_ID_CAPCOM, GAME_SPORTS, "slammast" );
qsound_clone( mbomberj,  "Muscle Bomber - The Body Explosion (Japan 930713)", 1993, slammast_dsw, COMPANY_ID_CAPCOM, GAME_SPORTS, "slammast" );
qsound_game( mbombrd,  "Muscle Bomber Duo - Ultimate Team Battle (World 931206)" , 1993, slammast_dsw, COMPANY_ID_CAPCOM, GAME_SPORTS );
qsound_clone( mbombrdj,  "Muscle Bomber Duo - Heat Up Warriors (Japan 931206)", 1993, slammast_dsw, COMPANY_ID_CAPCOM, GAME_SPORTS, "mbombrd" );
cps1_game( pnickj,  "Pnickies (Japan 940608)", 1994, pnickj_dsw, &cps1_video, COMPANY_ID_CAPCOM, GAME_PUZZLE );
cps1_game( pang3,  "Pang! 3 (Euro 950511)", 1995, pang3_dsw, &cps1_video, COMPANY_ID_MITCHELL, GAME_SHOOT );
cps1_clone( pang3j,  "Pang! 3 (Japan 950511)", 1995, pang3_dsw, &cps1_video, COMPANY_ID_MITCHELL, GAME_SHOOT, "pang3" );
sfzch_game(sfzch,"sfzch","Street Fighter Zero (Japan CPS Changer)" ,1995,GAME_BEAT);
