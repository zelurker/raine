#define DRV_DEF_LOAD load_cps2
#define DRV_DEF_VIDEO &video_cps1
#define DRV_DEF_EXEC execute_cps2_frame
#define DRV_DEF_SOUND sound_qsound
#define DRV_DEF_INPUT input_p2b6
#define DRV_DEF_DSW NULL
/* cps2 drivers, see cps1.c for the code (VERY similar to cps1) */

#include "gameinc.h"
#include "cps1.h"

static struct ROM_INFO rom_1944[] =
{
  { "nffu.03", 0x80000, 0x9693cf8f, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "nff.04", 0x80000, 0xdba1c66e, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "nffu.05", 0x80000, 0xea813eb7, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x0000000,  0x400000,
            "nff.13m",  0xc9fca741, "nff.15m",  0xf809d898,
            "nff.17m",  0x15ba4507, "nff.19m",  0x3dd41b8c),
  LOAD16_64(  REGION_GFX1,  0x1000000,  0x100000,
            "nff.14m",  0x3fe3a54b, "nff.16m",  0x565cd231,
            "nff.18m",  0x63ca5988, "nff.20m",  0x21eb8f3b),
  { "nff.01", 0x20000, 0xd2e44318, REGION_ROM2, 0, LOAD_NORMAL },
  { "nff.11m", 0x400000, 0x243e4e05, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "nff.12m", 0x400000, 0x4fcf1600, REGION_SMP1, 0x400000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_1944j[] = // clone of _1944
{
  { "nffj.03", 0x80000, 0x247521ef, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "nff.04", 0x80000, 0xdba1c66e, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "nffj.05", 0x80000, 0x7f20c2ef, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_19xx[] =
{
  { "19xu.03", 0x80000, 0x05955268, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "19xu.04", 0x80000, 0x3111ab7f, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "19xu.05", 0x80000, 0x38df4a63, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "19xu.06", 0x80000, 0x5c7e60d3, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "19x.07", 0x80000, 0x61c0296c, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x0000000,  0x080000,
            "19x.13m",  0x427aeb18, "19x.15m",  0x63bdbf54,
            "19x.17m",  0x2dfe18b5, "19x.19m",  0xcbef9579),
  LOAD16_64(  REGION_GFX1,  0x0800000,  0x200000,
            "19x.14m",  0xe916967c, "19x.16m",  0x6e75f3db,
            "19x.18m",  0x2213e798, "19x.20m",  0xab9d5b96),
  { "19x.01", 0x20000, 0xef55195e, REGION_ROM2, 0, LOAD_NORMAL },
  { "19x.11m", 0x200000, 0xd38beef3, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "19x.12m", 0x200000, 0xd47c96e2, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_19xxj[] = // clone of _19xx
{
  { "19xj.03a", 0x80000, 0xed08bdd1, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "19xj.04a", 0x80000, 0xfb8e3f29, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "19xj.05a", 0x80000, 0xaa508ac4, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "19xj.06a", 0x80000, 0xff2d785b, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "19x.07", 0x80000, 0x61c0296c, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_19xxjr1[] = // clone of _19xx
{
  { "19xj.03", 0x80000, 0x26a381ed, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "19xj.04", 0x80000, 0x30100cca, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "19xj.05", 0x80000, 0xde67e938, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "19xj.06", 0x80000, 0x39f9a409, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "19x.07", 0x80000, 0x61c0296c, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_19xxh[] = // clone of _19xx
{
  { "19xh.03a", 0x80000, 0x357be2ac, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "19xh.04a", 0x80000, 0xbb13ea3b, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "19xh.05a", 0x80000, 0xcbd76601, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "19xh.06a", 0x80000, 0xb362de8b, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "19x.07", 0x80000, 0x61c0296c, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_19xxb[] = // clone of _19xx
{
  { "19xb.03a", 0x80000, 0x341bdf4a, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "19xb.04a", 0x80000, 0xdff8069e, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "19xb.05a", 0x80000, 0xa47a92a8, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "19xb.06a", 0x80000, 0xc52df10d, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "19x.07", 0x80000, 0x61c0296c, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_armwar[] =
{
  { "pwge.03c", 0x80000, 0x31f74931, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "pwge.04c", 0x80000, 0x16f34f5f, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "pwge.05b", 0x80000, 0x4403ed08, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "pwg.06", 0x80000, 0x87a60ce8, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "pwg.07", 0x80000, 0xf7b148df, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "pwg.08", 0x80000, 0xcc62823e, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "pwg.09a", 0x80000, 0x4c26baee, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "pwg.10", 0x80000, 0x07c4fb28, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x0000000,  0x400000,
            "pwg.13m",  0xae8fe08e, "pwg.15m",  0xdb560f58,
            "pwg.17m",  0xbc475b94, "pwg.19m",  0x07439ff7),
  LOAD16_64(  REGION_GFX1,  0x1000000,  0x100000,
            "pwg.14m",  0xc3f9ba63, "pwg.16m",  0x815b0e7b,
            "pwg.18m",  0x0109c71b, "pwg.20m",  0xeb75ffbe),
  { "pwg.01", 0x20000, 0x18a5c0e4, REGION_ROM2, 0, LOAD_NORMAL },
  { "pwg.02", 0x20000, 0xc9dfffa6, REGION_ROM2, 0x20000, LOAD_NORMAL },
  { "pwg.11m", 0x200000, 0xa78f7433, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "pwg.12m", 0x200000, 0x77438ed0, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_armwarr1[] = // clone of _armwar
{
  { "pwge.03b", 0x80000, 0xe822e3e9, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "pwge.04b", 0x80000, 0x4f89de39, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "pwge.05a", 0x80000, 0x83df24e5, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "pwg.06", 0x80000, 0x87a60ce8, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "pwg.07", 0x80000, 0xf7b148df, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "pwg.08", 0x80000, 0xcc62823e, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "pwg.09", 0x80000, 0xddc85ca6, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "pwg.10", 0x80000, 0x07c4fb28, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_armwaru[] = // clone of _armwar
{
  { "pwgu.03b", 0x80000, 0x8b95497a, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "pwgu.04b", 0x80000, 0x29eb5661, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "pwgu.05b", 0x80000, 0xa54e9e44, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "pwg.06", 0x80000, 0x87a60ce8, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "pwg.07", 0x80000, 0xf7b148df, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "pwg.08", 0x80000, 0xcc62823e, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "pwg.09a", 0x80000, 0x4c26baee, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "pwg.10", 0x80000, 0x07c4fb28, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_armwaru1[] = // clone of _armwar
{
  { "pwgu.03a", 0x80000, 0x73d397b1, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "pwgu.04a", 0x80000, 0x1f1de215, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "pwgu.05a", 0x80000, 0x835fbe73, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "pwg.06", 0x80000, 0x87a60ce8, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "pwg.07", 0x80000, 0xf7b148df, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "pwg.08", 0x80000, 0xcc62823e, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "pwg.09", 0x80000, 0xddc85ca6, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "pwg.10", 0x80000, 0x07c4fb28, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_pgear[] = // clone of _armwar
{
  { "pwgj.03a", 0x80000, 0xc79c0c02, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "pwgj.04a", 0x80000, 0x167c6ed8, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "pwgj.05a", 0x80000, 0xa63fb400, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "pwg.06", 0x80000, 0x87a60ce8, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "pwg.07", 0x80000, 0xf7b148df, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "pwg.08", 0x80000, 0xcc62823e, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "pwg.09a", 0x80000, 0x4c26baee, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "pwg.10", 0x80000, 0x07c4fb28, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_pgearr1[] = // clone of _armwar
{
  { "pwgj.03", 0x80000, 0xf264e74b, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "pwgj.04", 0x80000, 0x23a84983, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "pwgj.05", 0x80000, 0xbef58c62, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "pwg.06", 0x80000, 0x87a60ce8, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "pwg.07", 0x80000, 0xf7b148df, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "pwg.08", 0x80000, 0xcc62823e, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "pwg.09", 0x80000, 0xddc85ca6, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "pwg.10", 0x80000, 0x07c4fb28, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_armwara[] = // clone of _armwar
{
  { "pwga.03a", 0x80000, 0x8d474ab1, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "pwga.04a", 0x80000, 0x81b5aec7, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "pwga.05a", 0x80000, 0x2618e819, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "pwg.06", 0x80000, 0x87a60ce8, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "pwg.07", 0x80000, 0xf7b148df, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "pwg.08", 0x80000, 0xcc62823e, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "pwg.09", 0x80000, 0xddc85ca6, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "pwg.10", 0x80000, 0x07c4fb28, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_avsp[] =
{
  { "avpe.03d", 0x80000, 0x774334a9, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "avpe.04d", 0x80000, 0x7fa83769, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "avp.05d", 0x80000, 0xfbfb5d7a, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "avp.06", 0x80000, 0x190b817f, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x0000000,  0x200000,
            "avp.13m",  0x8f8b5ae4, "avp.15m",  0xb00280df,
            "avp.17m",  0x94403195, "avp.19m",  0xe1981245),
  LOAD16_64(  REGION_GFX1,  0x0800000,  0x200000,
            "avp.14m",  0xebba093e, "avp.16m",  0xfb228297,
            "avp.18m",  0x34fb7232, "avp.20m",  0xf90baa21),
  { "avp.01", 0x20000, 0x2d3b4220, REGION_ROM2, 0, LOAD_NORMAL },
  { "avp.11m", 0x200000, 0x83499817, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "avp.12m", 0x200000, 0xf4110d49, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_avspu[] = // clone of _avsp
{
  { "avpu.03d", 0x80000, 0x42757950, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "avpu.04d", 0x80000, 0x5abcdee6, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "avp.05d", 0x80000, 0xfbfb5d7a, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "avp.06", 0x80000, 0x190b817f, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_avspj[] = // clone of _avsp
{
  { "avpj.03d", 0x80000, 0x49799119, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "avpj.04d", 0x80000, 0x8cd2bba8, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "avp.05d", 0x80000, 0xfbfb5d7a, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "avp.06", 0x80000, 0x190b817f, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_avspa[] = // clone of _avsp
{
  { "avpa.03d", 0x80000, 0x6c1c1858, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "avpa.04d", 0x80000, 0x94f50b0c, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "avp.05d", 0x80000, 0xfbfb5d7a, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "avp.06", 0x80000, 0x190b817f, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_avsph[] = // clone of _avsp
{
  { "avph.03d", 0x80000, 0x3e440447, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "avph.04d", 0x80000, 0xaf6fc82f, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "avp.05d", 0x80000, 0xfbfb5d7a, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "avp.06", 0x80000, 0x190b817f, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_batcir[] =
{
  { "btce.03", 0x80000, 0xbc60484b, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "btce.04", 0x80000, 0x457d55f6, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "btce.05", 0x80000, 0xe86560d7, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "btce.06", 0x80000, 0xf778e61b, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "btc.07", 0x80000, 0x7322d5db, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "btc.08", 0x80000, 0x6aac85ab, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "btc.09", 0x80000, 0x1203db08, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x400000,
            "btc.13m",  0xdc705bad, "btc.15m",  0xe5779a3c,
            "btc.17m",  0xb33f4112, "btc.19m",  0xa6fcdb7e),
  { "btc.01", 0x20000, 0x1e194310, REGION_ROM2, 0, LOAD_NORMAL },
  { "btc.02", 0x20000, 0x01aeb8e6, REGION_ROM2, 0x20000, LOAD_NORMAL },
  { "btc.11m", 0x200000, 0xc27f2229, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "btc.12m", 0x200000, 0x418a2e33, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_batcirj[] = // clone of _batcir
{
  { "btcj.03", 0x80000, 0x6b7e168d, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "btcj.04", 0x80000, 0x46ba3467, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "btcj.05", 0x80000, 0x0e23a859, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "btcj.06", 0x80000, 0xa853b59c, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "btc.07", 0x80000, 0x7322d5db, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "btc.08", 0x80000, 0x6aac85ab, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "btc.09", 0x80000, 0x1203db08, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_batcira[] = // clone of _batcir
{
  { "btca.03", 0x80000, 0x1ad20d87, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "btca.04", 0x80000, 0x2b3f4dbe, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "btca.05", 0x80000, 0x8238a3d9, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "btca.06", 0x80000, 0x446c7c02, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "btc.07", 0x80000, 0x7322d5db, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "btc.08", 0x80000, 0x6aac85ab, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "btc.09", 0x80000, 0x1203db08, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_choko[] =
{
  { "tkoj.03", 0x80000, 0x11f5452f, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "tkoj.04", 0x80000, 0x68655378, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "tkoj1_d.simm1", 0x200000, 0x6933377d, REGION_GFX1, 0x0000000, LOAD_8_64 },
  { "tkoj1_c.simm1", 0x200000, 0x7f668950, REGION_GFX1, 0x0000001, LOAD_8_64 },
  { "tkoj1_b.simm1", 0x200000, 0xcfb68ca9, REGION_GFX1, 0x0000002, LOAD_8_64 },
  { "tkoj1_a.simm1", 0x200000, 0x437e21c5, REGION_GFX1, 0x0000003, LOAD_8_64 },
  { "tkoj3_d.simm3", 0x200000, 0xa9e32b57, REGION_GFX1, 0x0000004, LOAD_8_64 },
  { "tkoj3_c.simm3", 0x200000, 0xb7ab9338, REGION_GFX1, 0x0000005, LOAD_8_64 },
  { "tkoj3_b.simm3", 0x200000, 0x4d3f919a, REGION_GFX1, 0x0000006, LOAD_8_64 },
  { "tkoj3_a.simm3", 0x200000, 0xcfef17ab, REGION_GFX1, 0x0000007, LOAD_8_64 },
  { "tko.01", 0x20000, 0x6eda50c2, REGION_ROM2, 0, LOAD_NORMAL },
  LOAD8_16(  REGION_SMP1,  0x000000,  0x200000,
            "tkoj5_a.simm5",  0xab45d509, "tkoj5_b.simm5",  0xfa905c3d),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_csclubj[] = // clone of _csclub
{
  { "cscj.03", 0x80000, 0xec4ddaa2, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "cscj.04", 0x80000, 0x60c632bb, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "cscj.05", 0x80000, 0xad042003, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "cscj.06", 0x80000, 0x169e4d40, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "csc.07", 0x80000, 0x01b05caa, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL(              0x000000, 0x800000, 0 ) */
  LOAD16_64(  REGION_GFX1,  0x800000,  0x200000,
            "csc.14m",  0xe8904afa, "csc.16m",  0xc98c8079,
            "csc.18m",  0xc030df5a, "csc.20m",  0xb4e55863),
  { "csc.11m", 0x200000, 0xa027b827, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "csc.12m", 0x200000, 0xcb7f6e55, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_csclub[] =
{
  { "csce.03a", 0x80000, 0x824082be, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "csce.04a", 0x80000, 0x74e6a4fe, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "csce.05a", 0x80000, 0x8ae0df19, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "csce.06a", 0x80000, 0x51f2f0d3, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "csce.07a", 0x80000, 0x003968fd, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL(              0x000000, 0x800000, 0 ) */
  LOAD16_64(  REGION_GFX1,  0x800000,  0x080000,
            "csc.73",  0x335f07c3, "csc.74",  0xab215357,
            "csc.75",  0xa2367381, "csc.76",  0x728aac1f),
  LOAD16_64(  REGION_GFX1,  0x800002,  0x080000,
            "csc.63",  0x3711b8ca, "csc.64",  0x828a06d8,
            "csc.65",  0x86ee4569, "csc.66",  0xc24f577f),
  LOAD16_64(  REGION_GFX1,  0x800004,  0x080000,
            "csc.83",  0x0750d12a, "csc.84",  0x90a92f39,
            "csc.85",  0xd08ab012, "csc.86",  0x41652583),
  LOAD16_64(  REGION_GFX1,  0x800006,  0x080000,
            "csc.93",  0xa756c7f7, "csc.94",  0xfb7ccc73,
            "csc.95",  0x4d014297, "csc.96",  0x6754b1ef),
  { "csc.01", 0x20000, 0xee162111, REGION_ROM2, 0, LOAD_NORMAL },
  { "csc.51", 0x080000, 0x5a52afd5, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "csc.52", 0x080000, 0x1408a811, REGION_SMP1, 0x080000, LOAD_SWAP_16 },
  { "csc.53", 0x080000, 0x4fb9f57c, REGION_SMP1, 0x100000, LOAD_SWAP_16 },
  { "csc.54", 0x080000, 0x9a8f40ec, REGION_SMP1, 0x180000, LOAD_SWAP_16 },
  { "csc.55", 0x080000, 0x91529a91, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { "csc.56", 0x080000, 0x9a345334, REGION_SMP1, 0x280000, LOAD_SWAP_16 },
  { "csc.57", 0x080000, 0xaedc27f2, REGION_SMP1, 0x300000, LOAD_SWAP_16 },
  { "csc.58", 0x080000, 0x2300b7b3, REGION_SMP1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_csclub1[] = // clone of _csclub
{
  { "csce.03", 0x80000, 0xf2c852ef, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "csce.04", 0x80000, 0x1184530f, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "csce.05", 0x80000, 0x804e2b6b, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "csce.06", 0x80000, 0x09277cb9, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "csc.07", 0x80000, 0x01b05caa, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL(              0x000000, 0x800000, 0 ) */
  LOAD16_64(  REGION_GFX1,  0x800000,  0x200000,
            "csc.14m",  0xe8904afa, "csc.16m",  0xc98c8079,
            "csc.18m",  0xc030df5a, "csc.20m",  0xb4e55863),
  { "csc.11m", 0x200000, 0xa027b827, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "csc.12m", 0x200000, 0xcb7f6e55, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_cscluba[] = // clone of _csclub
{
  { "csca.03", 0x80000, 0xb6acd708, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "csca.04", 0x80000, 0xd44ae35f, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "csca.05", 0x80000, 0x8da76aec, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "csca.06", 0x80000, 0xa1b7b1ee, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "csc.07", 0x80000, 0x01b05caa, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL(              0x000000, 0x800000, 0 ) */
  LOAD16_64(  REGION_GFX1,  0x800000,  0x200000,
            "csc.14m",  0xe8904afa, "csc.16m",  0xc98c8079,
            "csc.18m",  0xc030df5a, "csc.20m",  0xb4e55863),
  { "csc.11m", 0x200000, 0xa027b827, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "csc.12m", 0x200000, 0xcb7f6e55, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_csclubh[] = // clone of _csclub
{
  { "csch.03", 0x80000, 0x0dd7e46d, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "csch.04", 0x80000, 0x486e8143, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "csch.05", 0x80000, 0x9e509dfb, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "csch.06", 0x80000, 0x817ba313, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "csc.07", 0x80000, 0x01b05caa, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL(              0x000000, 0x800000, 0 ) */
  LOAD16_64(  REGION_GFX1,  0x800000,  0x200000,
            "csc.14m",  0xe8904afa, "csc.16m",  0xc98c8079,
            "csc.18m",  0xc030df5a, "csc.20m",  0xb4e55863),
  { "csc.11m", 0x200000, 0xa027b827, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "csc.12m", 0x200000, 0xcb7f6e55, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_cybots[] =
{
  { "cybe.03", 0x80000, 0x234381cd, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "cybe.04", 0x80000, 0x80691061, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "cyb.05", 0x80000, 0xec40408e, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "cyb.06", 0x80000, 0x1ad0bed2, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "cyb.07", 0x80000, 0x6245a39a, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "cyb.08", 0x80000, 0x4b48e223, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "cyb.09", 0x80000, 0xe15238f6, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "cyb.10", 0x80000, 0x75f4003b, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x0000000,  0x400000,
            "cyb.13m",  0xf0dce192, "cyb.15m",  0x187aa39c,
            "cyb.17m",  0x8a0e4b12, "cyb.19m",  0x34b62612),
  LOAD16_64(  REGION_GFX1,  0x1000000,  0x400000,
            "cyb.14m",  0xc1537957, "cyb.16m",  0x15349e86,
            "cyb.18m",  0xd83e977d, "cyb.20m",  0x77cdad5c),
  { "cyb.01", 0x20000, 0x9c0fb079, REGION_ROM2, 0, LOAD_NORMAL },
  { "cyb.02", 0x20000, 0x51cb0c4e, REGION_ROM2, 0x20000, LOAD_NORMAL },
  { "cyb.11m", 0x200000, 0x362ccab2, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "cyb.12m", 0x200000, 0x7066e9cc, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_cybotsu[] = // clone of _cybots
{
  { "cybu.03", 0x80000, 0xdb4da8f4, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "cybu.04", 0x80000, 0x1eec68ac, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "cyb.05", 0x80000, 0xec40408e, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "cyb.06", 0x80000, 0x1ad0bed2, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "cyb.07", 0x80000, 0x6245a39a, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "cyb.08", 0x80000, 0x4b48e223, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "cyb.09", 0x80000, 0xe15238f6, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "cyb.10", 0x80000, 0x75f4003b, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_cybotsj[] = // clone of _cybots
{
  { "cybj.03", 0x80000, 0x6096eada, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "cybj.04", 0x80000, 0x7b0ffaa9, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "cyb.05", 0x80000, 0xec40408e, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "cyb.06", 0x80000, 0x1ad0bed2, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "cyb.07", 0x80000, 0x6245a39a, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "cyb.08", 0x80000, 0x4b48e223, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "cyb.09", 0x80000, 0xe15238f6, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "cyb.10", 0x80000, 0x75f4003b, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ddtod[] =
{
  { "dade.03c", 0x80000, 0x8e73533d, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "dade.04c", 0x80000, 0x00c2e82e, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "dade.05c", 0x80000, 0xea996008, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "dad.06a", 0x80000, 0x6225495a, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "dad.07a", 0x80000, 0xb3480ec3, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x200000,
            "dad.13m",  0xda3cb7d6, "dad.15m",  0x92b63172,
            "dad.17m",  0xb98757f5, "dad.19m",  0x8121ce46),
  LOAD16_64(  REGION_GFX1,  0x800000,  0x100000,
            "dad.14m",  0x837e6f3f, "dad.16m",  0xf0916bdb,
            "dad.18m",  0xcef393ef, "dad.20m",  0x8953fe9e),
  { "dad.01", 0x20000, 0x3f5e2424, REGION_ROM2, 0, LOAD_NORMAL },
  { "dad.11m", 0x200000, 0x0c499b67, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "dad.12m", 0x200000, 0x2f0b5a4e, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ddtodr1[] = // clone of _ddtod
{
  { "dade.03a", 0x80000, 0x665a035e, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "dade.04a", 0x80000, 0x02613207, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "dade.05a", 0x80000, 0x36845996, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "dad.06", 0x80000, 0x13aa3e56, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "dad.07", 0x80000, 0x431cb6dd, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ddtodu[] = // clone of _ddtod
{
  { "dadu.03b", 0x80000, 0xa519905f, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "dadu.04b", 0x80000, 0x52562d38, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "dadu.05b", 0x80000, 0xee1cfbfe, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "dad.06", 0x80000, 0x13aa3e56, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "dad.07", 0x80000, 0x431cb6dd, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ddtodur1[] = // clone of _ddtod
{
  { "dadu.03a", 0x80000, 0x4413f177, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "dadu.04a", 0x80000, 0x168de230, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "dadu.05a", 0x80000, 0x03d39e91, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "dad.06", 0x80000, 0x13aa3e56, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "dad.07", 0x80000, 0x431cb6dd, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ddtodj[] = // clone of _ddtod
{
  { "dadj.03c", 0x80000, 0x0b1b5798, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "dadj.04c", 0x80000, 0xc6a2fbc8, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "dadj.05c", 0x80000, 0x189b15fe, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "dad.06a", 0x80000, 0x6225495a, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "dad.07a", 0x80000, 0xb3480ec3, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ddtodjr1[] = // clone of _ddtod
{
  { "dadj.03b", 0x80000, 0x87606b85, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "dadj.04b", 0x80000, 0x24d49575, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "dadj.05b", 0x80000, 0x56ce51f7, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "dad.06", 0x80000, 0x13aa3e56, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "dad.07", 0x80000, 0x431cb6dd, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ddtodjr2[] = // clone of _ddtod
{
  { "dadj.03a", 0x80000, 0x711638dc, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "dadj.04a", 0x80000, 0x4869639c, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "dadj.05a", 0x80000, 0x484c0efa, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "dad.06", 0x80000, 0x13aa3e56, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "dad.07", 0x80000, 0x431cb6dd, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ddtoda[] = // clone of _ddtod
{
  { "dada.03a", 0x80000, 0xfc6f2dd7, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "dada.04a", 0x80000, 0xd4be4009, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "dada.05a", 0x80000, 0x6712d1cf, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "dad.06", 0x80000, 0x13aa3e56, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "dad.07", 0x80000, 0x431cb6dd, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ddtodh[] = // clone of _ddtod
{
  { "dadh.03b", 0x80000, 0xae0cb98e, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "dadh.04b", 0x80000, 0xb5774363, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "dadh.05b", 0x80000, 0x6ce2a485, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "dad.06", 0x80000, 0x13aa3e56, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "dad.07", 0x80000, 0x431cb6dd, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ddsom[] =
{
  { "dd2e.03e", 0x80000, 0x449361af, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "dd2e.04e", 0x80000, 0x5b7052b6, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "dd2e.05e", 0x80000, 0x788d5f60, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "dd2e.06e", 0x80000, 0xe0807e1e, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "dd2e.07", 0x80000, 0xbb777a02, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "dd2e.08", 0x80000, 0x30970890, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "dd2e.09", 0x80000, 0x99e2194d, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "dd2e.10", 0x80000, 0xe198805e, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x0000000,  0x400000,
            "dd2.13m",  0xa46b4e6e, "dd2.15m",  0xd5fc50fc,
            "dd2.17m",  0x837c0867, "dd2.19m",  0xbb0ec21c),
  LOAD16_64(  REGION_GFX1,  0x1000000,  0x200000,
            "dd2.14m",  0x6d824ce2, "dd2.16m",  0x79682ae5,
            "dd2.18m",  0xacddd149, "dd2.20m",  0x117fb0c0),
  { "dd2.01", 0x20000, 0x99d657e5, REGION_ROM2, 0, LOAD_NORMAL },
  { "dd2.02", 0x20000, 0x117a3824, REGION_ROM2, 0x20000, LOAD_NORMAL },
  { "dd2.11m", 0x200000, 0x98d0c325, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "dd2.12m", 0x200000, 0x5ea2e7fa, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ddsomr1[] = // clone of _ddsom
{
  { "dd2e.03d", 0x80000, 0x6c084ab5, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "dd2e.04d", 0x80000, 0x9b94a947, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "dd2e.05d", 0x80000, 0x5d6a63c6, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "dd2e.06d", 0x80000, 0x31bde8ee, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "dd2e.07", 0x80000, 0xbb777a02, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "dd2e.08", 0x80000, 0x30970890, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "dd2e.09", 0x80000, 0x99e2194d, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "dd2e.10", 0x80000, 0xe198805e, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ddsomr2[] = // clone of _ddsom
{
  { "dd2e.03b", 0x80000, 0xcd2deb66, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "dd2e.04b", 0x80000, 0xbfee43cc, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "dd2e.05b", 0x80000, 0x049ab19d, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "dd2e.06b", 0x80000, 0x3994fb8b, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "dd2e.07", 0x80000, 0xbb777a02, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "dd2e.08", 0x80000, 0x30970890, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "dd2e.09", 0x80000, 0x99e2194d, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "dd2e.10", 0x80000, 0xe198805e, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ddsomr3[] = // clone of _ddsom
{
  { "dd2e.03a", 0x80000, 0x6de67678, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "dd2e.04a", 0x80000, 0x0e45739a, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "dd2e.05a", 0x80000, 0x3dce8025, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "dd2e.06a", 0x80000, 0x51bafbef, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "dd2e.07", 0x80000, 0xbb777a02, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "dd2e.08", 0x80000, 0x30970890, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "dd2e.09", 0x80000, 0x99e2194d, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "dd2e.10", 0x80000, 0xe198805e, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ddsomu[] = // clone of _ddsom
{
  { "dd2u.03g", 0x80000, 0xfb089b39, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "dd2u.04g", 0x80000, 0xcd432b73, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "dd2.05g", 0x80000, 0x5eb1991c, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "dd2.06g", 0x80000, 0xc26b5e55, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "dd2.07", 0x80000, 0x909a0b8b, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "dd2.08", 0x80000, 0xe53c4d01, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "dd2.09", 0x80000, 0x5f86279f, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "dd2.10", 0x80000, 0xad954c26, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ddsomur1[] = // clone of _ddsom
{
  { "dd2u.03d", 0x80000, 0x0f700d84, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "dd2u.04d", 0x80000, 0xb99eb254, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "dd2.05d", 0x80000, 0xb23061f3, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "dd2.06d", 0x80000, 0x8bf1d8ce, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "dd2.07", 0x80000, 0x909a0b8b, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "dd2.08", 0x80000, 0xe53c4d01, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "dd2.09", 0x80000, 0x5f86279f, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "dd2.10", 0x80000, 0xad954c26, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ddsomj[] = // clone of _ddsom
{
  { "dd2j.03g", 0x80000, 0xe6c8c985, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "dd2j.04g", 0x80000, 0x8386c0bd, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "dd2.05g", 0x80000, 0x5eb1991c, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "dd2.06g", 0x80000, 0xc26b5e55, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "dd2.07", 0x80000, 0x909a0b8b, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "dd2.08", 0x80000, 0xe53c4d01, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "dd2.09", 0x80000, 0x5f86279f, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "dd2.10", 0x80000, 0xad954c26, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ddsomjr1[] = // clone of _ddsom
{
  { "dd2j.03b", 0x80000, 0x965d74e5, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "dd2j.04b", 0x80000, 0x958eb8f3, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "dd2.05b", 0x80000, 0xd38571ca, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "dd2.06b", 0x80000, 0x6d5a3bbb, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "dd2.07", 0x80000, 0x909a0b8b, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "dd2.08", 0x80000, 0xe53c4d01, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "dd2.09", 0x80000, 0x5f86279f, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "dd2.10", 0x80000, 0xad954c26, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ddsoma[] = // clone of _ddsom
{
  { "dd2a.03g", 0x80000, 0x0b4fec22, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "dd2a.04g", 0x80000, 0x055b7019, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "dd2.05g", 0x80000, 0x5eb1991c, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "dd2.06g", 0x80000, 0xc26b5e55, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "dd2.07", 0x80000, 0x909a0b8b, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "dd2.08", 0x80000, 0xe53c4d01, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "dd2.09", 0x80000, 0x5f86279f, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "dd2.10", 0x80000, 0xad954c26, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ddsomb[] = // clone of _ddsom
{
  { "dd2b.03a", 0x80000, 0xe8ce7fbb, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "dd2b.04a", 0x80000, 0x6b679664, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "dd2b.05a", 0x80000, 0x9b2534eb, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "dd2b.06a", 0x80000, 0x3b21ba59, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "dd2b.07", 0x80000, 0xfce2558d, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "dd2e.08", 0x80000, 0x30970890, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "dd2e.09", 0x80000, 0x99e2194d, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "dd2e.10", 0x80000, 0xe198805e, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_dimahoo[] =
{
  { "gmde.03", 0x80000, 0x968fcecd, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "gmd.04", 0x80000, 0x37485567, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "gmd.05", 0x80000, 0xda269ffb, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "gmd.06", 0x80000, 0x55b483c9, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x400000,
            "gmd.13m",  0x80dd19f0, "gmd.15m",  0xdfd93a78,
            "gmd.17m",  0x16356520, "gmd.19m",  0xdfc33031),
  { "gmd.01", 0x20000, 0x3f9bc985, REGION_ROM2, 0, LOAD_NORMAL },
  { "gmd.02", 0x20000, 0x3fd39dde, REGION_ROM2, 0x20000, LOAD_NORMAL },
  { "gmd.11m", 0x400000, 0x06a65542, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "gmd.12m", 0x400000, 0x50bc7a31, REGION_SMP1, 0x400000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_dimahoou[] = // clone of _dimahoo
{
  { "gmdu.03", 0x80000, 0x43bcb15f, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "gmd.04", 0x80000, 0x37485567, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "gmd.05", 0x80000, 0xda269ffb, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "gmd.06", 0x80000, 0x55b483c9, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_gmahou[] = // clone of _dimahoo
{
  { "gmdj.03", 0x80000, 0xcd6979e3, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "gmd.04", 0x80000, 0x37485567, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "gmd.05", 0x80000, 0xda269ffb, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "gmd.06", 0x80000, 0x55b483c9, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_dstlk[] =
{
  { "vame.03a", 0x80000, 0x004c9cff, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "vame.04a", 0x80000, 0xae413ff2, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "vame.05a", 0x80000, 0x60678756, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "vame.06a", 0x80000, 0x912870b3, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "vame.07a", 0x80000, 0xdabae3e8, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "vame.08a", 0x80000, 0x2c6e3077, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "vame.09a", 0x80000, 0xf16db74b, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "vame.10a", 0x80000, 0x701e2147, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x0000000,  0x400000,
            "vam.13m",  0xc51baf99, "vam.15m",  0x3ce83c77,
            "vam.17m",  0x4f2408e0, "vam.19m",  0x9ff60250),
  LOAD16_64(  REGION_GFX1,  0x1000000,  0x100000,
            "vam.14m",  0xbd87243c, "vam.16m",  0xafec855f,
            "vam.18m",  0x3a033625, "vam.20m",  0x2bff6a89),
  { "vam.01", 0x20000, 0x64b685d5, REGION_ROM2, 0, LOAD_NORMAL },
  { "vam.02", 0x20000, 0xcf7c97c7, REGION_ROM2, 0x20000, LOAD_NORMAL },
  { "vam.11m", 0x200000, 0x4a39deb2, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "vam.12m", 0x200000, 0x1a3e5c03, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_dstlku[] = // clone of _dstlk
{
  { "vamu.03b", 0x80000, 0x68a6343f, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "vamu.04b", 0x80000, 0x58161453, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "vamu.05b", 0x80000, 0xdfc038b8, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "vamu.06b", 0x80000, 0xc3842c89, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "vamu.07b", 0x80000, 0x25b60b6e, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "vamu.08b", 0x80000, 0x2113c596, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "vamu.09b", 0x80000, 0x2d1e9ae5, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "vamu.10b", 0x80000, 0x81145622, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_dstlkur1[] = // clone of _dstlk
{
  { "vamu.03a", 0x80000, 0x628899f9, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "vamu.04a", 0x80000, 0x696d9b25, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "vamu.05a", 0x80000, 0x673ed50a, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "vamu.06a", 0x80000, 0xf2377be7, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "vamu.07a", 0x80000, 0xd8f498c4, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "vamu.08a", 0x80000, 0xe6a8a1a0, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "vamu.09a", 0x80000, 0x8dd55b24, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "vamu.10a", 0x80000, 0xc1a3d9be, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_vampj[] = // clone of _dstlk
{
  { "vamj.03a", 0x80000, 0xf55d3722, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "vamj.04b", 0x80000, 0x4d9c43c4, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "vamj.05a", 0x80000, 0x6c497e92, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "vamj.06a", 0x80000, 0xf1bbecb6, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "vamj.07a", 0x80000, 0x1067ad84, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "vamj.08a", 0x80000, 0x4b89f41f, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "vamj.09a", 0x80000, 0xfc0a4aac, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "vamj.10a", 0x80000, 0x9270c26b, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_vampja[] = // clone of _dstlk
{
  { "vamj.03a", 0x80000, 0xf55d3722, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "vamj.04a", 0x80000, 0xfdcbdae3, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "vamj.05a", 0x80000, 0x6c497e92, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "vamj.06a", 0x80000, 0xf1bbecb6, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "vamj.07a", 0x80000, 0x1067ad84, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "vamj.08a", 0x80000, 0x4b89f41f, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "vamj.09a", 0x80000, 0xfc0a4aac, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "vamj.10a", 0x80000, 0x9270c26b, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_vampjr1[] = // clone of _dstlk
{
  { "vamj.03", 0x80000, 0x8895bf77, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "vamj.04", 0x80000, 0x5027db3d, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "vamj.05", 0x80000, 0x97c66fdb, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "vamj.06", 0x80000, 0x9b4c3426, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "vamj.07", 0x80000, 0x303bc4fd, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "vamj.08", 0x80000, 0x3dea3646, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "vamj.09", 0x80000, 0xc119a827, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "vamj.10", 0x80000, 0x46593b79, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_dstlka[] = // clone of _dstlk
{
  { "vama.03a", 0x80000, 0x294e0bec, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "vama.04a", 0x80000, 0xbc18e128, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "vama.05a", 0x80000, 0xe709fa59, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "vama.06a", 0x80000, 0x55e4d387, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "vama.07a", 0x80000, 0x24e8f981, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "vama.08a", 0x80000, 0x743f3a8e, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "vama.09a", 0x80000, 0x67fa5573, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "vama.10a", 0x80000, 0x5e03d747, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ecofghtr[] =
{
  { "uece.03", 0x80000, 0xec2c1137, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "uece.04", 0x80000, 0xb35f99db, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "uece.05", 0x80000, 0xd9d42d31, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "uece.06", 0x80000, 0x9d9771cf, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x200000,
            "uec.13m",  0xdcaf1436, "uec.15m",  0x2807df41,
            "uec.17m",  0x8a708d02, "uec.19m",  0xde7be0ef),
  LOAD16_64(  REGION_GFX1,  0x800000,  0x100000,
            "uec.14m",  0x1a003558, "uec.16m",  0x4ff8a6f9,
            "uec.18m",  0xb167ae12, "uec.20m",  0x1064bdc2),
  { "uec.01", 0x20000, 0xc235bd15, REGION_ROM2, 0, LOAD_NORMAL },
  { "uec.11m", 0x200000, 0x81b25d39, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "uec.12m", 0x200000, 0x27729e52, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ecofghtu[] = // clone of _ecofghtr
{
  { "uecu.03a", 0x80000, 0x22d88a4d, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "uecu.04a", 0x80000, 0x6436cfcd, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "uecu.05a", 0x80000, 0x336f121b, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "uecu.06a", 0x80000, 0x6f99d984, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ecofgtu1[] = // clone of _ecofghtr
{
  { "uecu.03", 0x80000, 0x6792480c, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "uecu.04", 0x80000, 0x95ce69d5, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "uecu.05", 0x80000, 0x3a1e78ad, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "uecu.06", 0x80000, 0xa3e2f3cc, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_uecology[] = // clone of _ecofghtr
{
  { "uecj.03", 0x80000, 0x94c40a4c, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "uecj.04", 0x80000, 0x8d6e3a09, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "uecj.05", 0x80000, 0x8604ecd7, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "uecj.06", 0x80000, 0xb7e1d31f, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ecofghta[] = // clone of _ecofghtr
{
  { "ueca.03", 0x80000, 0xbd4589b1, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "ueca.04", 0x80000, 0x1d134b7d, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "ueca.05", 0x80000, 0x9c581fc7, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "ueca.06", 0x80000, 0xc92a7c50, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_gigawing[] =
{
  { "ggwu.03", 0x80000, 0xac725eb2, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "ggwu.04", 0x80000, 0x392f4118, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "ggw.05", 0x80000, 0x3239d642, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x400000,
            "ggw.13m",  0x105530a4, "ggw.15m",  0x9e774ab9,
            "ggw.17m",  0x466e0ba4, "ggw.19m",  0x840c8dea),
  { "ggw.01", 0x20000, 0x4c6351d5, REGION_ROM2, 0, LOAD_NORMAL },
  { "ggw.11m", 0x400000, 0xe172acf5, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "ggw.12m", 0x400000, 0x4bee4e8f, REGION_SMP1, 0x400000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_gwingj[] = // clone of _gigawing
{
  { "ggwj.03a", 0x80000, 0xfdd23b91, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "ggwj.04a", 0x80000, 0x8c6e093c, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "ggwj.05a", 0x80000, 0x43811454, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_gwinga[] = // clone of _gigawing
{
  { "ggwa.03a", 0x80000, 0x116f8837, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "ggwa.04a", 0x80000, 0xe6e3f0c4, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "ggwa.05a", 0x80000, 0x465e8ac9, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_gwingb[] = // clone of _gigawing
{
  { "ggwb.03", 0x80000, 0xa1f8a448, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "ggwb.04", 0x80000, 0x6a423e76, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "ggw.05", 0x80000, 0x3239d642, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_hsf2[] =
{
  { "hs2a.03", 0x80000, 0xd50a17e0, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "hs2a.04", 0x80000, 0xa27f42de, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "hs2.05", 0x80000, 0xdde34a35, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "hs2.06", 0x80000, 0xf4e56dda, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "hs2a.07", 0x80000, 0xee4420fc, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "hs2.08", 0x80000, 0xc9441533, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "hs2.09", 0x80000, 0x3fc638a8, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "hs2.10", 0x80000, 0x20d0f9e4, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x0000000,  0x800000,
            "hs2.13m",  0xa6ecab17, "hs2.15m",  0x10a0ae4d,
            "hs2.17m",  0xadfa7726, "hs2.19m",  0xbb3ae322),
  { "hs2.01", 0x20000, 0xc1a13786, REGION_ROM2, 0, LOAD_NORMAL },
  { "hs2.02", 0x20000, 0x2d8794aa, REGION_ROM2, 0x20000, LOAD_NORMAL },
  { "hs2.11m", 0x800000, 0x0e15c359, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_hsf2j[] = // clone of _hsf2
{
  { "hs2j.03", 0x80000, 0x00738f73, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "hs2j.04", 0x80000, 0x40072c4a, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "hs2.05", 0x80000, 0xdde34a35, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "hs2.06", 0x80000, 0xf4e56dda, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "hs2j.07", 0x80000, 0x09fe85b4, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "hs2.08", 0x80000, 0xc9441533, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "hs2.09", 0x80000, 0x3fc638a8, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "hs2.10", 0x80000, 0x20d0f9e4, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_jyangoku[] =
{
  { "majj.03", 0x80000, 0x4614a3b2, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "maj1_d.simm1", 0x200000, 0xba0fe27b, REGION_GFX1, 0x0000000, LOAD_8_64 },
  { "maj1_c.simm1", 0x200000, 0x2cd141bf, REGION_GFX1, 0x0000001, LOAD_8_64 },
  { "maj1_b.simm1", 0x200000, 0xe29e4c26, REGION_GFX1, 0x0000002, LOAD_8_64 },
  { "maj1_a.simm1", 0x200000, 0x7f68b88a, REGION_GFX1, 0x0000003, LOAD_8_64 },
  { "maj3_d.simm3", 0x200000, 0x3aaeb90b, REGION_GFX1, 0x0000004, LOAD_8_64 },
  { "maj3_c.simm3", 0x200000, 0x97894cea, REGION_GFX1, 0x0000005, LOAD_8_64 },
  { "maj3_b.simm3", 0x200000, 0xec737d9d, REGION_GFX1, 0x0000006, LOAD_8_64 },
  { "maj3_a.simm3", 0x200000, 0xc23b6f22, REGION_GFX1, 0x0000007, LOAD_8_64 },
  { "maj.01", 0x20000, 0x1fe8c213, REGION_ROM2, 0, LOAD_NORMAL },
  LOAD8_16(  REGION_SMP1,  0x000000,  0x200000,
            "maj5_a.simm5",  0x5ad9ee53, "maj5_b.simm5",  0xefb3dbfb),
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_megaman2[] =
{
  { "rm2u.03", 0x80000, 0x8ffc2cd1, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "rm2u.04", 0x80000, 0xbb30083a, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "rm2.05", 0x80000, 0x02ee9efc, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL(              0x000000, 0x800000, 0 ) */
  LOAD16_64(  REGION_GFX1,  0x800000,  0x200000,
            "rm2.14m",  0x9b1f00b4, "rm2.16m",  0xc2bb0c24,
            "rm2.18m",  0x12257251, "rm2.20m",  0xf9b6e786),
  { "rm2.01a", 0x20000, 0xd18e7859, REGION_ROM2, 0, LOAD_NORMAL },
  { "rm2.02", 0x20000, 0xc463ece0, REGION_ROM2, 0x20000, LOAD_NORMAL },
  { "rm2.11m", 0x200000, 0x2106174d, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "rm2.12m", 0x200000, 0x546c1636, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_megamn2a[] = // clone of _megaman2
{
  { "rm2a.03", 0x80000, 0x2b330ca7, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "rm2a.04", 0x80000, 0x8b47942b, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "rm2.05", 0x80000, 0x02ee9efc, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL(              0x000000, 0x800000, 0 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_rckman2j[] = // clone of _megaman2
{
  { "rm2j.03", 0x80000, 0xdbaa1437, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "rm2j.04", 0x80000, 0xcf5ba612, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "rm2.05", 0x80000, 0x02ee9efc, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL(              0x000000, 0x800000, 0 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mmatrix[] =
{
  { "mmxu.03", 0x80000, 0xab65b599, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "mmxu.04", 0x80000, 0x0135fc6c, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "mmxu.05", 0x80000, 0xf1fd2b84, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x0000000,  0x400000,
            "mmx.13m",  0x04748718, "mmx.15m",  0x38074f44,
            "mmx.17m",  0xe4635e35, "mmx.19m",  0x4400a3f2),
  LOAD16_64(  REGION_GFX1,  0x1000000,  0x400000,
            "mmx.14m",  0xd52bf491, "mmx.16m",  0x23f70780,
            "mmx.18m",  0x2562c9d5, "mmx.20m",  0x583a9687),
  { "mmx.01", 0x20000, 0xc57e8171, REGION_ROM2, 0, LOAD_NORMAL },
  { "mmx.11m", 0x400000, 0x4180b39f, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "mmx.12m", 0x400000, 0x95e22a59, REGION_SMP1, 0x400000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mmatrixj[] = // clone of _mmatrix
{
  { "mmxj.03", 0x80000, 0x1d5de213, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "mmxj.04", 0x80000, 0xd943a339, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "mmxj.05", 0x80000, 0x0c8b4abb, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_msh[] =
{
  { "mshe.03e", 0x80000, 0xbd951414, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "mshe.04e", 0x80000, 0x19dd42f2, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "msh.05", 0x80000, 0x6a091b9e, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "msh.06b", 0x80000, 0x803e3fa4, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "msh.07a", 0x80000, 0xc45f8e27, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "msh.08a", 0x80000, 0x9ca6f12c, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "msh.09a", 0x80000, 0x82ec27af, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "msh.10b", 0x80000, 0x8d931196, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x0000000,  0x400000,
            "msh.13m",  0x09d14566, "msh.15m",  0xee962057,
            "msh.17m",  0x604ece14, "msh.19m",  0x94a731e8),
  LOAD16_64(  REGION_GFX1,  0x1000000,  0x400000,
            "msh.14m",  0x4197973e, "msh.16m",  0x438da4a0,
            "msh.18m",  0x4db92d94, "msh.20m",  0xa2b0c6c0),
  { "msh.01", 0x20000, 0xc976e6f9, REGION_ROM2, 0, LOAD_NORMAL },
  { "msh.02", 0x20000, 0xce67d0d9, REGION_ROM2, 0x20000, LOAD_NORMAL },
  { "msh.11m", 0x200000, 0x37ac6d30, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "msh.12m", 0x200000, 0xde092570, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mshu[] = // clone of _msh
{
  { "mshu.03", 0x80000, 0xd2805bdd, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "mshu.04", 0x80000, 0x743f96ff, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "msh.05", 0x80000, 0x6a091b9e, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "msh.06b", 0x80000, 0x803e3fa4, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "msh.07a", 0x80000, 0xc45f8e27, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "msh.08a", 0x80000, 0x9ca6f12c, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "msh.09a", 0x80000, 0x82ec27af, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "msh.10b", 0x80000, 0x8d931196, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mshj[] = // clone of _msh
{
  { "mshj.03g", 0x80000, 0x261f4091, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "mshj.04g", 0x80000, 0x61d791c6, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "msh.05a", 0x80000, 0xf37539e6, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "msh.06b", 0x80000, 0x803e3fa4, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "msh.07a", 0x80000, 0xc45f8e27, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "msh.08a", 0x80000, 0x9ca6f12c, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "msh.09a", 0x80000, 0x82ec27af, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "msh.10b", 0x80000, 0x8d931196, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mshjr1[] = // clone of _msh
{
  { "mshj.03f", 0x80000, 0xff172fd2, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "mshj.04f", 0x80000, 0xebbb205a, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "msh.05", 0x80000, 0x6a091b9e, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "msh.06b", 0x80000, 0x803e3fa4, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "msh.07a", 0x80000, 0xc45f8e27, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "msh.08a", 0x80000, 0x9ca6f12c, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "msh.09a", 0x80000, 0x82ec27af, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "msh.10b", 0x80000, 0x8d931196, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_msha[] = // clone of _msh
{
  { "msha.03e", 0x80000, 0xec84ec44, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "msha.04e", 0x80000, 0x098b8503, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "msh.05", 0x80000, 0x6a091b9e, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "msh.06b", 0x80000, 0x803e3fa4, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "msh.07a", 0x80000, 0xc45f8e27, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "msh.08a", 0x80000, 0x9ca6f12c, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "msh.09a", 0x80000, 0x82ec27af, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "msh.10b", 0x80000, 0x8d931196, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mshh[] = // clone of _msh
{
  { "mshh.03c", 0x80000, 0x8d84b0fa, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "mshh.04c", 0x80000, 0xd638f601, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "msh.05a", 0x80000, 0xf37539e6, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "msh.06b", 0x80000, 0x803e3fa4, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "msh.07a", 0x80000, 0xc45f8e27, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "msh.08a", 0x80000, 0x9ca6f12c, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "msh.09a", 0x80000, 0x82ec27af, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "msh.10b", 0x80000, 0x8d931196, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mshb[] = // clone of _msh
{
  { "mshb.03c", 0x80000, 0x19697f74, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "mshb.04c", 0x80000, 0x95317a6f, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "msh.05a", 0x80000, 0xf37539e6, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "msh.06b", 0x80000, 0x803e3fa4, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "msh.07a", 0x80000, 0xc45f8e27, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "msh.08a", 0x80000, 0x9ca6f12c, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "msh.09a", 0x80000, 0x82ec27af, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "msh.10b", 0x80000, 0x8d931196, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mshvsf[] =
{
  { "mvse.03f", 0x80000, 0xb72dc199, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "mvse.04f", 0x80000, 0x6ef799f9, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "mvs.05a", 0x80000, 0x1a5de0cb, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "mvs.06a", 0x80000, 0x959f3030, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "mvs.07b", 0x80000, 0x7f915bdb, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "mvs.08a", 0x80000, 0xc2813884, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "mvs.09b", 0x80000, 0x3ba08818, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "mvs.10b", 0x80000, 0xcf0dba98, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x0000000,  0x400000,
            "mvs.13m",  0x29b05fd9, "mvs.15m",  0xfaddccf1,
            "mvs.17m",  0x97aaf4c7, "mvs.19m",  0xcb70e915),
  LOAD16_64(  REGION_GFX1,  0x1000000,  0x400000,
            "mvs.14m",  0xb3b1972d, "mvs.16m",  0x08aadb5d,
            "mvs.18m",  0xc1228b35, "mvs.20m",  0x366cc6c2),
  { "mvs.01", 0x20000, 0x68252324, REGION_ROM2, 0, LOAD_NORMAL },
  { "mvs.02", 0x20000, 0xb34e773d, REGION_ROM2, 0x20000, LOAD_NORMAL },
  { "mvs.11m", 0x400000, 0x86219770, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "mvs.12m", 0x400000, 0xf2fd7f68, REGION_SMP1, 0x400000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mshvsfu[] = // clone of _mshvsf
{
  { "mvsu.03g", 0x80000, 0x0664ab15, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "mvsu.04g", 0x80000, 0x97e060ee, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "mvs.05d", 0x80000, 0x921fc542, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "mvs.06a", 0x80000, 0x959f3030, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "mvs.07b", 0x80000, 0x7f915bdb, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "mvs.08a", 0x80000, 0xc2813884, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "mvs.09b", 0x80000, 0x3ba08818, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "mvs.10b", 0x80000, 0xcf0dba98, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mshvsfu1[] = // clone of _mshvsf
{
  { "mvsu.03d", 0x80000, 0xae60a66a, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "mvsu.04d", 0x80000, 0x91f67d8a, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "mvs.05a", 0x80000, 0x1a5de0cb, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "mvs.06a", 0x80000, 0x959f3030, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "mvs.07b", 0x80000, 0x7f915bdb, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "mvs.08a", 0x80000, 0xc2813884, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "mvs.09b", 0x80000, 0x3ba08818, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "mvs.10b", 0x80000, 0xcf0dba98, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mshvsfj[] = // clone of _mshvsf
{
  { "mvsj.03i", 0x80000, 0xd8cbb691, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "mvsj.04i", 0x80000, 0x32741ace, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "mvs.05h", 0x80000, 0x77870dc3, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "mvs.06a", 0x80000, 0x959f3030, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "mvs.07b", 0x80000, 0x7f915bdb, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "mvs.08a", 0x80000, 0xc2813884, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "mvs.09b", 0x80000, 0x3ba08818, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "mvs.10b", 0x80000, 0xcf0dba98, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mshvsfj1[] = // clone of _mshvsf
{
  { "mvsj.03h", 0x80000, 0xfbe2115f, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "mvsj.04h", 0x80000, 0xb528a367, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "mvs.05g", 0x80000, 0x9515a245, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "mvs.06a", 0x80000, 0x959f3030, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "mvs.07b", 0x80000, 0x7f915bdb, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "mvs.08a", 0x80000, 0xc2813884, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "mvs.09b", 0x80000, 0x3ba08818, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "mvs.10b", 0x80000, 0xcf0dba98, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mshvsfj2[] = // clone of _mshvsf
{
  { "mvsj.03g", 0x80000, 0xfdfa7e26, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "mvsj.04g", 0x80000, 0xc921825f, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "mvs.05a", 0x80000, 0x1a5de0cb, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "mvs.06a", 0x80000, 0x959f3030, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "mvs.07b", 0x80000, 0x7f915bdb, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "mvs.08a", 0x80000, 0xc2813884, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "mvs.09b", 0x80000, 0x3ba08818, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "mvs.10b", 0x80000, 0xcf0dba98, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mshvsfh[] = // clone of _mshvsf
{
  { "mvsh.03f", 0x80000, 0x4f60f41e, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "mvsh.04f", 0x80000, 0xdc08ec12, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "mvs.05a", 0x80000, 0x1a5de0cb, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "mvs.06a", 0x80000, 0x959f3030, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "mvs.07b", 0x80000, 0x7f915bdb, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "mvs.08a", 0x80000, 0xc2813884, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "mvs.09b", 0x80000, 0x3ba08818, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "mvs.10b", 0x80000, 0xcf0dba98, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mshvsfa[] = // clone of _mshvsf
{
  { "mvsa.03f", 0x80000, 0x5b863716, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "mvsa.04f", 0x80000, 0x4886e65f, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "mvs.05a", 0x80000, 0x1a5de0cb, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "mvs.06a", 0x80000, 0x959f3030, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "mvs.07b", 0x80000, 0x7f915bdb, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "mvs.08a", 0x80000, 0xc2813884, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "mvs.09b", 0x80000, 0x3ba08818, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "mvs.10b", 0x80000, 0xcf0dba98, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mshvsfa1[] = // clone of _mshvsf
{
  { "mvsa.03", 0x80000, 0x92ef1933, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "mvsa.04", 0x80000, 0x4b24373c, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "mvs.05", 0x80000, 0xac180c1c, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "mvs.06a", 0x80000, 0x959f3030, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "mvs.07b", 0x80000, 0x7f915bdb, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "mvs.08a", 0x80000, 0xc2813884, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "mvs.09b", 0x80000, 0x3ba08818, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "mvs.10b", 0x80000, 0xcf0dba98, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mshvsfb[] = // clone of _mshvsf
{
  { "mvsb.03g", 0x80000, 0x143895ef, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "mvsb.04g", 0x80000, 0xdd8a886c, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "mvs.05d", 0x80000, 0x921fc542, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "mvs.06a", 0x80000, 0x959f3030, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "mvs.07b", 0x80000, 0x7f915bdb, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "mvs.08a", 0x80000, 0xc2813884, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "mvs.09b", 0x80000, 0x3ba08818, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "mvs.10b", 0x80000, 0xcf0dba98, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mshvsfb1[] = // clone of _mshvsf
{
  { "mvsb.03f", 0x80000, 0x9c4bb950, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "mvsb.04f", 0x80000, 0xd3320d13, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "mvs.05a", 0x80000, 0x1a5de0cb, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "mvs.06a", 0x80000, 0x959f3030, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "mvs.07b", 0x80000, 0x7f915bdb, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "mvs.08a", 0x80000, 0xc2813884, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "mvs.09b", 0x80000, 0x3ba08818, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "mvs.10b", 0x80000, 0xcf0dba98, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mvsc[] =
{
  { "mvce.03a", 0x80000, 0x824e4a90, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "mvce.04a", 0x80000, 0xcac02153, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "mvc.05a", 0x80000, 0x2d8c8e86, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "mvc.06a", 0x80000, 0x8528e1f5, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "mvc.07", 0x80000, 0xc3baa32b, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "mvc.08", 0x80000, 0xbc002fcd, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "mvc.09", 0x80000, 0xc67b26df, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "mvc.10", 0x80000, 0x0fdd1e26, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x0000000,  0x400000,
            "mvc.13m",  0xfa5f74bc, "mvc.15m",  0x71938a8f,
            "mvc.17m",  0x92741d07, "mvc.19m",  0xbcb72fc6),
  LOAD16_64(  REGION_GFX1,  0x1000000,  0x400000,
            "mvc.14m",  0x7f1df4e4, "mvc.16m",  0x90bd3203,
            "mvc.18m",  0x67aaf727, "mvc.20m",  0x8b0bade8),
  { "mvc.01", 0x20000, 0x41629e95, REGION_ROM2, 0, LOAD_NORMAL },
  { "mvc.02", 0x20000, 0x963abf6b, REGION_ROM2, 0x20000, LOAD_NORMAL },
  { "mvc.11m", 0x400000, 0x850fe663, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "mvc.12m", 0x400000, 0x7ccb1896, REGION_SMP1, 0x400000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mvscr1[] = // clone of _mvsc
{
  { "mvce.03", 0x80000, 0xe0633fc0, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "mvce.04", 0x80000, 0xa450a251, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "mvc.05", 0x80000, 0x7db71ce9, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "mvc.06", 0x80000, 0x4b0b6d3e, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "mvc.07", 0x80000, 0xc3baa32b, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "mvc.08", 0x80000, 0xbc002fcd, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "mvc.09", 0x80000, 0xc67b26df, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "mvc.10", 0x80000, 0x0fdd1e26, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mvscu[] = // clone of _mvsc
{
  { "mvcu.03d", 0x80000, 0xc6007557, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "mvcu.04d", 0x80000, 0x724b2b20, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "mvc.05a", 0x80000, 0x2d8c8e86, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "mvc.06a", 0x80000, 0x8528e1f5, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "mvc.07", 0x80000, 0xc3baa32b, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "mvc.08", 0x80000, 0xbc002fcd, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "mvc.09", 0x80000, 0xc67b26df, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "mvc.10", 0x80000, 0x0fdd1e26, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mvscj[] = // clone of _mvsc
{
  { "mvcj.03a", 0x80000, 0x3df18879, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "mvcj.04a", 0x80000, 0x07d212e8, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "mvc.05a", 0x80000, 0x2d8c8e86, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "mvc.06a", 0x80000, 0x8528e1f5, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "mvc.07", 0x80000, 0xc3baa32b, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "mvc.08", 0x80000, 0xbc002fcd, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "mvc.09", 0x80000, 0xc67b26df, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "mvc.10", 0x80000, 0x0fdd1e26, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mvscjr1[] = // clone of _mvsc
{
  { "mvcj.03", 0x80000, 0x2164213f, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "mvcj.04", 0x80000, 0xc905c86f, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "mvc.05", 0x80000, 0x7db71ce9, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "mvc.06", 0x80000, 0x4b0b6d3e, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "mvc.07", 0x80000, 0xc3baa32b, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "mvc.08", 0x80000, 0xbc002fcd, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "mvc.09", 0x80000, 0xc67b26df, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "mvc.10", 0x80000, 0x0fdd1e26, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mvsca[] = // clone of _mvsc
{
  { "mvca.03a", 0x80000, 0x2ff4ae25, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "mvca.04a", 0x80000, 0xf28427ef, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "mvc.05a", 0x80000, 0x2d8c8e86, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "mvc.06a", 0x80000, 0x8528e1f5, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "mvc.07", 0x80000, 0xc3baa32b, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "mvc.08", 0x80000, 0xbc002fcd, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "mvc.09", 0x80000, 0xc67b26df, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "mvc.10", 0x80000, 0x0fdd1e26, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mvscar1[] = // clone of _mvsc
{
  { "mvca.03", 0x80000, 0xfe5fa7b9, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "mvca.04", 0x80000, 0x082b701c, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "mvc.05", 0x80000, 0x7db71ce9, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "mvc.06", 0x80000, 0x4b0b6d3e, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "mvc.07", 0x80000, 0xc3baa32b, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "mvc.08", 0x80000, 0xbc002fcd, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "mvc.09", 0x80000, 0xc67b26df, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "mvc.10", 0x80000, 0x0fdd1e26, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mvsch[] = // clone of _mvsc
{
  { "mvch.03", 0x80000, 0x6a0ec9f7, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "mvch.04", 0x80000, 0x00f03fa4, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "mvc.05a", 0x80000, 0x2d8c8e86, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "mvc.06a", 0x80000, 0x8528e1f5, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "mvc.07", 0x80000, 0xc3baa32b, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "mvc.08", 0x80000, 0xbc002fcd, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "mvc.09", 0x80000, 0xc67b26df, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "mvc.10", 0x80000, 0x0fdd1e26, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mvscb[] = // clone of _mvsc
{
  { "mvcb.03a", 0x80000, 0x7155953b, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "mvcb.04a", 0x80000, 0xfb117d0e, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "mvc.05a", 0x80000, 0x2d8c8e86, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "mvc.06a", 0x80000, 0x8528e1f5, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "mvc.07", 0x80000, 0xc3baa32b, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "mvc.08", 0x80000, 0xbc002fcd, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "mvc.09", 0x80000, 0xc67b26df, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "mvc.10", 0x80000, 0x0fdd1e26, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mpang[] =
{
  { "mpne.03c", 0x80000, 0xfe16fc9f, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "mpne.04c", 0x80000, 0x2cc5ec22, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "mpn-simm.01c", 0x200000, 0x388db66b, REGION_GFX1, 0x0000000, LOAD_8_64 },
  { "mpn-simm.01d", 0x200000, 0xaff1b494, REGION_GFX1, 0x0000001, LOAD_8_64 },
  { "mpn-simm.01a", 0x200000, 0xa9c4857b, REGION_GFX1, 0x0000002, LOAD_8_64 },
  { "mpn-simm.01b", 0x200000, 0xf759df22, REGION_GFX1, 0x0000003, LOAD_8_64 },
  { "mpn-simm.03c", 0x200000, 0xdec6b720, REGION_GFX1, 0x0000004, LOAD_8_64 },
  { "mpn-simm.03d", 0x200000, 0xf8774c18, REGION_GFX1, 0x0000005, LOAD_8_64 },
  { "mpn-simm.03a", 0x200000, 0xc2aea4ec, REGION_GFX1, 0x0000006, LOAD_8_64 },
  { "mpn-simm.03b", 0x200000, 0x84d6dc33, REGION_GFX1, 0x0000007, LOAD_8_64 },
  { "mpn.01", 0x20000, 0x90c7adb6, REGION_ROM2, 0, LOAD_NORMAL },
  { "mpn-simm.05a", 0x200000, 0x318a2e21, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "mpn-simm.05b", 0x200000, 0x5462f4e8, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mpangu[] = // clone of _mpang
{
  { "mpnu.03", 0x80000, 0x6e7ed03c, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "mpnu.04", 0x80000, 0xde079131, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL(              0x000000, 0x800000, 0 ) */
  LOAD16_64(  REGION_GFX1,  0x800000,  0x200000,
            "mpn.13m",  0xc5f123dc, "mpn.15m",  0x8e033265,
            "mpn.17m",  0xcfcd73d2, "mpn.19m",  0x2db1ffbc),
  { "mpn.q1", 0x100000, 0xd21c1f5a, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "mpn.q2", 0x100000, 0xd22090b1, REGION_SMP1, 0x100000, LOAD_SWAP_16 },
  { "mpn.q3", 0x100000, 0x60aa5ef2, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { "mpn.q4", 0x100000, 0x3a67d203, REGION_SMP1, 0x300000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mpangj[] = // clone of _mpang
{
  { "mpnj.03a", 0x80000, 0xbf597b1c, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "mpnj.04a", 0x80000, 0xf4a3ab0f, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_nwarr[] =
{
  { "vphe.03f", 0x80000, 0xa922c44f, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "vphe.04c", 0x80000, 0x7312d890, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "vphe.05d", 0x80000, 0xcde8b506, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "vphe.06c", 0x80000, 0xbe99e7d0, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "vphe.07b", 0x80000, 0x69e0e60c, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "vphe.08b", 0x80000, 0xd95a3849, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "vphe.09b", 0x80000, 0x9882561c, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "vphe.10b", 0x80000, 0x976fa62f, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x0000000,  0x400000,
            "vph.13m",  0xc51baf99, "vph.15m",  0x3ce83c77,
            "vph.17m",  0x4f2408e0, "vph.19m",  0x9ff60250),
  LOAD16_64(  REGION_GFX1,  0x1000000,  0x400000,
            "vph.14m",  0x7a0e1add, "vph.16m",  0x2f41ca75,
            "vph.18m",  0x64498eed, "vph.20m",  0x17f2433f),
  { "vph.01", 0x20000, 0x5045dcac, REGION_ROM2, 0, LOAD_NORMAL },
  { "vph.02", 0x20000, 0x86b60e59, REGION_ROM2, 0x20000, LOAD_NORMAL },
  { "vph.11m", 0x200000, 0xe1837d33, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "vph.12m", 0x200000, 0xfbd3cd90, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_nwarru[] = // clone of _nwarr
{
  { "vphu.03f", 0x80000, 0x85d6a359, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "vphu.04c", 0x80000, 0xcb7fce77, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "vphu.05e", 0x80000, 0xe08f2bba, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "vphu.06c", 0x80000, 0x08c04cdb, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "vphu.07b", 0x80000, 0xb5a5ab19, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "vphu.08b", 0x80000, 0x51bb20fb, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "vphu.09b", 0x80000, 0x41a64205, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "vphu.10b", 0x80000, 0x2b1d43ae, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_nwarrh[] = // clone of _nwarr
{
  { "vphh.03d", 0x80000, 0x6029c7be, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "vphh.04a", 0x80000, 0xd26625ee, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "vphh.05c", 0x80000, 0x73ee0b8a, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "vphh.06a", 0x80000, 0xa5b3a50a, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "vphh.07", 0x80000, 0x5fc2bdc1, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "vphh.08", 0x80000, 0xe65588d9, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "vphh.09", 0x80000, 0xa2ce6d63, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "vphh.10", 0x80000, 0xe2f4f4b9, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_nwarrb[] = // clone of _nwarr
{
  { "vphb.03d", 0x80000, 0x3a426d3f, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "vphb.04a", 0x80000, 0x51c4bb2f, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "vphb.05c", 0x80000, 0xac44d997, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "vphb.06a", 0x80000, 0x5072a5fe, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "vphb.07", 0x80000, 0x9b355192, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "vphb.08", 0x80000, 0x42220f84, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "vphb.09", 0x80000, 0x029e015d, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "vphb.10", 0x80000, 0x37b3ce37, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_vhuntj[] = // clone of _nwarr
{
  { "vphj.03f", 0x80000, 0x3de2e333, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "vphj.04c", 0x80000, 0xc95cf304, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "vphj.05d", 0x80000, 0x50de5ddd, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "vphj.06c", 0x80000, 0xac3bd3d5, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "vphj.07b", 0x80000, 0x0761309f, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "vphj.08b", 0x80000, 0x5a5c2bf5, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "vphj.09b", 0x80000, 0x823d6d99, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "vphj.10b", 0x80000, 0x32c7d8f0, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_vhuntjr1[] = // clone of _nwarr
{
  { "vphj.03c", 0x80000, 0x606b682a, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "vphj.04b", 0x80000, 0xa3b40393, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "vphj.05b", 0x80000, 0xfccd5558, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "vphj.06b", 0x80000, 0x07e10a73, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "vphj.07b", 0x80000, 0x0761309f, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "vphj.08b", 0x80000, 0x5a5c2bf5, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "vphj.09b", 0x80000, 0x823d6d99, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "vphj.10b", 0x80000, 0x32c7d8f0, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_vhuntjr2[] = // clone of _nwarr
{
  { "vphj.03b", 0x80000, 0x679c3fa9, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "vphj.04a", 0x80000, 0xeb6e71e4, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "vphj.05a", 0x80000, 0xeaf634ea, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "vphj.06a", 0x80000, 0xb70cc6be, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "vphj.07a", 0x80000, 0x46ab907d, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "vphj.08a", 0x80000, 0x1c00355e, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "vphj.09a", 0x80000, 0x026e6f82, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "vphj.10a", 0x80000, 0xaadfb3ea, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_progear[] =
{
  { "pgau.03", 0x80000, 0x343a783e, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "pgau.04", 0x80000, 0x16208d79, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "pga-simm.01c", 0x200000, 0x452f98b0, REGION_GFX1, 0x0000000, LOAD_8_64 },
  { "pga-simm.01d", 0x200000, 0x9e672092, REGION_GFX1, 0x0000001, LOAD_8_64 },
  { "pga-simm.01a", 0x200000, 0xae9ddafe, REGION_GFX1, 0x0000002, LOAD_8_64 },
  { "pga-simm.01b", 0x200000, 0x94d72d94, REGION_GFX1, 0x0000003, LOAD_8_64 },
  { "pga-simm.03c", 0x200000, 0x48a1886d, REGION_GFX1, 0x0000004, LOAD_8_64 },
  { "pga-simm.03d", 0x200000, 0x172d7e37, REGION_GFX1, 0x0000005, LOAD_8_64 },
  { "pga-simm.03a", 0x200000, 0x9ee33d98, REGION_GFX1, 0x0000006, LOAD_8_64 },
  { "pga-simm.03b", 0x200000, 0x848dee32, REGION_GFX1, 0x0000007, LOAD_8_64 },
  { "pga.01", 0x20000, 0xbdbfa992, REGION_ROM2, 0, LOAD_NORMAL },
  { "pga-simm.05a", 0x200000, 0xc0aac80c, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "pga-simm.05b", 0x200000, 0x37a65d86, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { "pga-simm.06a", 0x200000, 0xd3f1e934, REGION_SMP1, 0x400000, LOAD_SWAP_16 },
  { "pga-simm.06b", 0x200000, 0x8b39489a, REGION_SMP1, 0x600000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_progearj[] = // clone of _progear
{
  { "pgaj.03", 0x80000, 0x06dbba54, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "pgaj.04", 0x80000, 0xa1f1f1bc, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_progeara[] = // clone of _progear
{
  { "pgaa.03", 0x80000, 0x25e6e2ce, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "pgaa.04", 0x80000, 0x8104307e, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_pzloop2[] =
{
  { "pl2e.03", 0x80000, 0x3b1285b2, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "pl2e.04", 0x80000, 0x40a2d647, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "pl2e.05", 0x80000, 0x0f11d818, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "pl2e.06", 0x80000, 0x86fbbdf4, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "pl2-simm.01c", 0x200000, 0x137b13a7, REGION_GFX1, 0x0000000, LOAD_8_64 },
  { "pl2-simm.01d", 0x200000, 0xa2db1507, REGION_GFX1, 0x0000001, LOAD_8_64 },
  { "pl2-simm.01a", 0x200000, 0x7e80ff8e, REGION_GFX1, 0x0000002, LOAD_8_64 },
  { "pl2-simm.01b", 0x200000, 0xcd93e6ed, REGION_GFX1, 0x0000003, LOAD_8_64 },
  { "pl2-simm.03c", 0x200000, 0x0f52bbca, REGION_GFX1, 0x0000004, LOAD_8_64 },
  { "pl2-simm.03d", 0x200000, 0xa62712c3, REGION_GFX1, 0x0000005, LOAD_8_64 },
  { "pl2-simm.03a", 0x200000, 0xb60c9f8e, REGION_GFX1, 0x0000006, LOAD_8_64 },
  { "pl2-simm.03b", 0x200000, 0x83fef284, REGION_GFX1, 0x0000007, LOAD_8_64 },
  { "pl2.01", 0x20000, 0x35697569, REGION_ROM2, 0, LOAD_NORMAL },
  { "pl2-simm.05a", 0x200000, 0x85d8fbe8, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "pl2-simm.05b", 0x200000, 0x1ed62584, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_pzloop2j[] = // clone of _pzloop2
{
  { "pl2j.03a", 0x80000, 0x0a751bd0, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "pl2j.04a", 0x80000, 0xc3f72afe, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "pl2j.05a", 0x80000, 0x6ea9dbfc, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "pl2j.06a", 0x80000, 0x0f14848d, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_qndream[] =
{
  { "tqzj.03a", 0x80000, 0x7acf3e30, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "tqzj.04", 0x80000, 0xf1044a87, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "tqzj.05", 0x80000, 0x4105ba0e, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "tqzj.06", 0x80000, 0xc371e8a5, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL(              0x000000, 0x800000, 0 ) */
  LOAD16_64(  REGION_GFX1,  0x800000,  0x200000,
            "tqz.14m",  0x98af88a2, "tqz.16m",  0xdf82d491,
            "tqz.18m",  0x42f132ff, "tqz.20m",  0xb2e128a3),
  { "tqz.01", 0x20000, 0xe9ce9d0a, REGION_ROM2, 0, LOAD_NORMAL },
  { "tqz.11m", 0x200000, 0x78e7884f, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "tqz.12m", 0x200000, 0x2e049b13, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ringdest[] =
{
  { "smbe.03b", 0x80000, 0xb8016278, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "smbe.04b", 0x80000, 0x18c4c447, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "smbe.05b", 0x80000, 0x18ebda7f, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "smbe.06b", 0x80000, 0x89c80007, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "smb.07", 0x80000, 0xb9a11577, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "smb.08", 0x80000, 0xf931b76b, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x0000000,  0x200000,
            "smb.13m",  0xd9b2d1de, "smb.15m",  0x9a766d92,
            "smb.17m",  0x51800f0f, "smb.19m",  0x35757e96),
  LOAD16_64(  REGION_GFX1,  0x0800000,  0x200000,
            "smb.14m",  0xe5bfd0e7, "smb.16m",  0xc56c0866,
            "smb.18m",  0x4ded3910, "smb.20m",  0x26ea1ec5),
  LOAD16_64(  REGION_GFX1,  0x1000000,  0x080000,
            "smb.21m",  0x0a08c5fc, "smb.23m",  0x0911b6c4,
            "smb.25m",  0x82d6c4ec, "smb.27m",  0x9b48678b),
  { "smb.01", 0x20000, 0x0abc229a, REGION_ROM2, 0, LOAD_NORMAL },
  { "smb.02", 0x20000, 0xd051679a, REGION_ROM2, 0x20000, LOAD_NORMAL },
  { "smb.11m", 0x200000, 0xc56935f9, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "smb.12m", 0x200000, 0x955b0782, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_smbomb[] = // clone of _ringdest
{
  { "smbj.03a", 0x80000, 0x1c5613de, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "smbj.04a", 0x80000, 0x29071ed7, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "smbj.05a", 0x80000, 0xeb20bce4, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "smbj.06a", 0x80000, 0x94b420cd, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "smb.07", 0x80000, 0xb9a11577, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "smb.08", 0x80000, 0xf931b76b, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_smbombr1[] = // clone of _ringdest
{
  { "smbj.03", 0x80000, 0x52eafb10, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "smbj.04", 0x80000, 0xaa6e8078, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "smbj.05", 0x80000, 0xb69e7d5f, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "smbj.06", 0x80000, 0x8d857b56, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "smb.07", 0x80000, 0xb9a11577, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "smb.08", 0x80000, 0xf931b76b, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mmancp2u[] =
{
  { "rcmu.03b", 0x80000, 0xc39f037f, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "rcmu.04b", 0x80000, 0xcd6f5e99, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "rcm.05b", 0x80000, 0x4376ea95, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL(              0x000000, 0x800000, 0 ) */
  LOAD16_64(  REGION_GFX1,  0x800000,  0x080000,
            "rcm.73",  0x774c6e04, "rcm.63",  0xacad7c62,
            "rcm.83",  0x6af30499, "rcm.93",  0x7a5a5166),
  LOAD16_64(  REGION_GFX1,  0xa00000,  0x080000,
            "rcm.74",  0x004ec725, "rcm.64",  0x65c0464e,
            "rcm.84",  0xfb3097cc, "rcm.94",  0x2e16557a),
  LOAD16_64(  REGION_GFX1,  0xc00000,  0x080000,
            "rcm.75",  0x70a73f99, "rcm.65",  0xecedad3d,
            "rcm.85",  0x3d6186d8, "rcm.95",  0x8c7700f1),
  LOAD16_64(  REGION_GFX1,  0xe00000,  0x080000,
            "rcm.76",  0x89a889ad, "rcm.66",  0x1300eb7b,
            "rcm.86",  0x6d974ebd, "rcm.96",  0x7da4cd24),
  { "rcm.01", 0x20000, 0xd60cf8a3, REGION_ROM2, 0, LOAD_NORMAL },
  { "rcm.51", 0x80000, 0xb6d07080, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "rcm.52", 0x80000, 0xdfddc493, REGION_SMP1, 0x080000, LOAD_SWAP_16 },
  { "rcm.53", 0x80000, 0x6062ae3a, REGION_SMP1, 0x100000, LOAD_SWAP_16 },
  { "rcm.54", 0x80000, 0x08c6f3bf, REGION_SMP1, 0x180000, LOAD_SWAP_16 },
  { "rcm.55", 0x80000, 0xf97dfccc, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { "rcm.56", 0x80000, 0xade475bc, REGION_SMP1, 0x280000, LOAD_SWAP_16 },
  { "rcm.57", 0x80000, 0x075effb3, REGION_SMP1, 0x300000, LOAD_SWAP_16 },
  { "rcm.58", 0x80000, 0xf6c1f87b, REGION_SMP1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_rmancp2j[] =
{
  { "rcmj.03a", 0x80000, 0x30559f60, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "rcmj.04a", 0x80000, 0x5efc9366, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "rcm.05a", 0x80000, 0x517ccde2, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL(              0x000000, 0x800000, 0 ) */
  LOAD16_64(  REGION_GFX1,  0x800000,  0x080000,
            "rcm.73",  0x774c6e04, "rcm.63",  0xacad7c62,
            "rcm.83",  0x6af30499, "rcm.93",  0x7a5a5166),
  LOAD16_64(  REGION_GFX1,  0xa00000,  0x080000,
            "rcm.74",  0x004ec725, "rcm.64",  0x65c0464e,
            "rcm.84",  0xfb3097cc, "rcm.94",  0x2e16557a),
  LOAD16_64(  REGION_GFX1,  0xc00000,  0x080000,
            "rcm.75",  0x70a73f99, "rcm.65",  0xecedad3d,
            "rcm.85",  0x3d6186d8, "rcm.95",  0x8c7700f1),
  LOAD16_64(  REGION_GFX1,  0xe00000,  0x080000,
            "rcm.76",  0x89a889ad, "rcm.66",  0x1300eb7b,
            "rcm.86",  0x6d974ebd, "rcm.96",  0x7da4cd24),
  { "rcm.01", 0x20000, 0xd60cf8a3, REGION_ROM2, 0, LOAD_NORMAL },
  { "rcm.51", 0x80000, 0xb6d07080, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "rcm.52", 0x80000, 0xdfddc493, REGION_SMP1, 0x080000, LOAD_SWAP_16 },
  { "rcm.53", 0x80000, 0x6062ae3a, REGION_SMP1, 0x100000, LOAD_SWAP_16 },
  { "rcm.54", 0x80000, 0x08c6f3bf, REGION_SMP1, 0x180000, LOAD_SWAP_16 },
  { "rcm.55", 0x80000, 0xf97dfccc, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { "rcm.56", 0x80000, 0xade475bc, REGION_SMP1, 0x280000, LOAD_SWAP_16 },
  { "rcm.57", 0x80000, 0x075effb3, REGION_SMP1, 0x300000, LOAD_SWAP_16 },
  { "rcm.58", 0x80000, 0xf6c1f87b, REGION_SMP1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfa[] =
{
  { "sfze.03d", 0x80000, 0xebf2054d, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sfz.04b", 0x80000, 0x8b73b0e5, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sfz.05a", 0x80000, 0x0810544d, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sfz.06", 0x80000, 0x806e8f38, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL(              0x000000, 0x800000, 0 ) */
  LOAD16_64(  REGION_GFX1,  0x800000,  0x200000,
            "sfz.14m",  0x90fefdb3, "sfz.16m",  0x5354c948,
            "sfz.18m",  0x41a1e790, "sfz.20m",  0xa549df98),
  { "sfz.01", 0x20000, 0xffffec7d, REGION_ROM2, 0, LOAD_NORMAL },
  { "sfz.02", 0x20000, 0x45f46a08, REGION_ROM2, 0x20000, LOAD_NORMAL },
  { "sfz.11m", 0x200000, 0xc4b093cd, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "sfz.12m", 0x200000, 0x8bdbc4b4, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfar1[] = // clone of _sfa
{
  { "sfze.03c", 0x80000, 0xa1b69dd7, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sfze.04b", 0x80000, 0xbb90acd5, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sfz.05a", 0x80000, 0x0810544d, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sfz.06", 0x80000, 0x806e8f38, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL(              0x000000, 0x800000, 0 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfar2[] = // clone of _sfa
{
  { "sfze.03b", 0x80000, 0x2bf5708e, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sfz.04a", 0x80000, 0x5f99e9a5, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sfz.05a", 0x80000, 0x0810544d, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sfz.06", 0x80000, 0x806e8f38, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL(              0x000000, 0x800000, 0 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfar3[] = // clone of _sfa
{
  { "sfze.03a", 0x80000, 0xfdbcd434, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sfz.04", 0x80000, 0x0c436d30, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sfz.05", 0x80000, 0x1f363612, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sfz.06", 0x80000, 0x806e8f38, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL(              0x000000, 0x800000, 0 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfau[] = // clone of _sfa
{
  { "sfzu.03a", 0x80000, 0x49fc7db9, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sfz.04a", 0x80000, 0x5f99e9a5, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sfz.05a", 0x80000, 0x0810544d, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sfz.06", 0x80000, 0x806e8f38, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL(              0x000000, 0x800000, 0 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfza[] = // clone of _sfa
{
  { "sfza.03a", 0x80000, 0xca91bed9, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sfz.04a", 0x80000, 0x5f99e9a5, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sfz.05a", 0x80000, 0x0810544d, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sfz.06", 0x80000, 0x806e8f38, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL(              0x000000, 0x800000, 0 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfzj[] = // clone of _sfa
{
  { "sfzj.03c", 0x80000, 0xf5444120, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sfz.04b", 0x80000, 0x8b73b0e5, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sfz.05a", 0x80000, 0x0810544d, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sfz.06", 0x80000, 0x806e8f38, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL(              0x000000, 0x800000, 0 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfzjr1[] = // clone of _sfa
{
  { "sfzj.03b", 0x80000, 0x844220c2, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sfz.04a", 0x80000, 0x5f99e9a5, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sfz.05a", 0x80000, 0x0810544d, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sfz.06", 0x80000, 0x806e8f38, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL(              0x000000, 0x800000, 0 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfzjr2[] = // clone of _sfa
{
  { "sfzj.03a", 0x80000, 0x3cfce93c, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sfz.04", 0x80000, 0x0c436d30, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sfz.05", 0x80000, 0x1f363612, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sfz.06", 0x80000, 0x806e8f38, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL(              0x000000, 0x800000, 0 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfzh[] = // clone of _sfa
{
  { "sfzh.03c", 0x80000, 0xbce635aa, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sfz.04a", 0x80000, 0x5f99e9a5, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sfz.05a", 0x80000, 0x0810544d, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sfz.06", 0x80000, 0x806e8f38, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL(              0x000000, 0x800000, 0 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfzb[] = // clone of _sfa
{
  { "sfzb.03g", 0x80000, 0x348862d4, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sfzb.04e", 0x80000, 0x8d9b2480, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sfz.05a", 0x80000, 0x0810544d, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sfz.06", 0x80000, 0x806e8f38, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL(              0x000000, 0x800000, 0 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfzbr1[] = // clone of _sfa
{
  { "sfzb.03e", 0x80000, 0xecba89a3, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sfz.04b", 0x80000, 0x8b73b0e5, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sfz.05a", 0x80000, 0x0810544d, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sfz.06", 0x80000, 0x806e8f38, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL(              0x000000, 0x800000, 0 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfa2[] =
{
  { "sz2e.03", 0x80000, 0x1061e6bb, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sz2e.04", 0x80000, 0x22d17b26, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sz2.05", 0x80000, 0x4b442a7c, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sz2.06", 0x80000, 0x5b1d49c0, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "sz2.07", 0x80000, 0x8e184246, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "sz2.08", 0x80000, 0x0fe8585d, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x0000000,  0x400000,
            "sz2.13m",  0x4d1f1f22, "sz2.15m",  0x19cea680,
            "sz2.17m",  0xe01b4588, "sz2.19m",  0x0feeda64),
  LOAD16_64(  REGION_GFX1,  0x1000000,  0x100000,
            "sz2.14m",  0x0560c6aa, "sz2.16m",  0xae940f87,
            "sz2.18m",  0x4bc3c8bc, "sz2.20m",  0x39e674c0),
  { "sz2.01a", 0x20000, 0x1bc323cf, REGION_ROM2, 0, LOAD_NORMAL },
  { "sz2.02a", 0x20000, 0xba6a5013, REGION_ROM2, 0x20000, LOAD_NORMAL },
  { "sz2.11m", 0x200000, 0xaa47a601, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "sz2.12m", 0x200000, 0x2237bc53, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfa2u[] = // clone of _sfa2
{
  { "sz2u.03", 0x80000, 0x84a09006, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sz2u.04", 0x80000, 0xac46e5ed, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sz2u.05", 0x80000, 0x6c0c79d3, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sz2u.06", 0x80000, 0xc5c8eb63, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "sz2u.07", 0x80000, 0x5de01cc5, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "sz2u.08", 0x80000, 0xbea11d56, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfz2j[] = // clone of _sfa2
{
  { "sz2j.03a", 0x80000, 0x97461e28, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sz2j.04a", 0x80000, 0xae4851a9, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sz2.05a", 0x80000, 0x98e8e992, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sz2.06", 0x80000, 0x5b1d49c0, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "sz2j.07a", 0x80000, 0xd910b2a2, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "sz2.08", 0x80000, 0x0fe8585d, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfz2a[] = // clone of _sfa2
{
  { "sz2a.03a", 0x80000, 0x30d2099f, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sz2a.04a", 0x80000, 0x1cc94db1, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sz2.05a", 0x80000, 0x98e8e992, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sz2.06", 0x80000, 0x5b1d49c0, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "sz2a.07a", 0x80000, 0x0aed2494, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "sz2.08", 0x80000, 0x0fe8585d, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfz2b[] = // clone of _sfa2
{
  { "sz2b.03b", 0x80000, 0x1ac12812, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sz2b.04b", 0x80000, 0xe4ffaf68, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sz2b.05a", 0x80000, 0xdd224156, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sz2b.06a", 0x80000, 0xa45a75a6, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "sz2b.07a", 0x80000, 0x7d19d5ec, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "sz2b.08", 0x80000, 0x92b66e01, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfz2br1[] = // clone of _sfa2
{
  { "sz2b.03", 0x80000, 0xe6ce530b, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sz2b.04", 0x80000, 0x1605a0cb, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sz2.05", 0x80000, 0x4b442a7c, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sz2.06", 0x80000, 0x5b1d49c0, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "sz2b.07", 0x80000, 0x947e8ac6, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "sz2b.08", 0x80000, 0x92b66e01, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfz2h[] = // clone of _sfa2
{
  { "sz2h.03", 0x80000, 0xbfeddf5b, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sz2h.04", 0x80000, 0xea5009fb, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sz2.05", 0x80000, 0x4b442a7c, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sz2.06", 0x80000, 0x5b1d49c0, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "sz2b.07", 0x80000, 0x947e8ac6, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "sz2b.08", 0x80000, 0x92b66e01, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfz2n[] = // clone of _sfa2
{
  { "sz2n.03", 0x80000, 0x58924741, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sz2n.04", 0x80000, 0x592a17c5, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sz2.05", 0x80000, 0x4b442a7c, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sz2.06", 0x80000, 0x5b1d49c0, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "sz2.07", 0x80000, 0x8e184246, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "sz2.08", 0x80000, 0x0fe8585d, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfz2alj[] = // clone of _sfz2al
{
  { "szaj.03a", 0x80000, 0xa3802fe3, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "szaj.04a", 0x80000, 0xe7ca87c7, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "szaj.05a", 0x80000, 0xc88ebf88, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "szaj.06a", 0x80000, 0x35ed5b7a, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "szaj.07a", 0x80000, 0x975dcb3e, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "szaj.08a", 0x80000, 0xdc73f2d7, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfz2al[] =
{
  { "szaa.03", 0x80000, 0x88e7023e, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "szaa.04", 0x80000, 0xae8ec36e, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "szaa.05", 0x80000, 0xf053a55e, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "szaa.06", 0x80000, 0xcfc0e7a8, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "szaa.07", 0x80000, 0x5feb8b20, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "szaa.08", 0x80000, 0x6eb6d412, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x0000000,  0x400000,
            "sza.13m",  0x4d1f1f22, "sza.15m",  0x19cea680,
            "sza.17m",  0xe01b4588, "sza.19m",  0x0feeda64),
  LOAD16_64(  REGION_GFX1,  0x1000000,  0x100000,
            "sza.14m",  0x0560c6aa, "sza.16m",  0xae940f87,
            "sza.18m",  0x4bc3c8bc, "sza.20m",  0x39e674c0),
  { "sza.01", 0x20000, 0x1bc323cf, REGION_ROM2, 0, LOAD_NORMAL },
  { "sza.02", 0x20000, 0xba6a5013, REGION_ROM2, 0x20000, LOAD_NORMAL },
  { "sza.11m", 0x200000, 0xaa47a601, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "sza.12m", 0x200000, 0x2237bc53, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfz2alh[] = // clone of _sfz2al
{
  { "szah.03", 0x80000, 0x06f93d1d, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "szah.04", 0x80000, 0xe62ee914, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "szah.05", 0x80000, 0x2b7f4b20, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sza.06", 0x80000, 0x0abda2fc, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "sza.07", 0x80000, 0xe9430762, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "sza.08", 0x80000, 0xb65711a9, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfz2alb[] = // clone of _sfz2al
{
  { "szab.03", 0x80000, 0xcb436eca, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "szab.04", 0x80000, 0x14534bea, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "szab.05", 0x80000, 0x7fb10658, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sza.06", 0x80000, 0x0abda2fc, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "sza.07", 0x80000, 0xe9430762, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "sza.08", 0x80000, 0xb65711a9, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfa3[] =
{
  { "sz3e.03c", 0x80000, 0x9762b206, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sz3e.04c", 0x80000, 0x5ad3f721, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sz3.05c", 0x80000, 0x57fd0a40, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sz3.06c", 0x80000, 0xf6305f8b, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "sz3.07c", 0x80000, 0x6eab0f6f, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "sz3.08c", 0x80000, 0x910c4a3b, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "sz3.09c", 0x80000, 0xb29e5199, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "sz3.10b", 0x80000, 0xdeb2ff52, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x0000000,  0x400000,
            "sz3.13m",  0x0f7a60d9, "sz3.15m",  0x8e933741,
            "sz3.17m",  0xd6e98147, "sz3.19m",  0xf31a728a),
  LOAD16_64(  REGION_GFX1,  0x1000000,  0x400000,
            "sz3.14m",  0x5ff98297, "sz3.16m",  0x52b5bdee,
            "sz3.18m",  0x40631ed5, "sz3.20m",  0x763409b4),
  { "sz3.01", 0x20000, 0xde810084, REGION_ROM2, 0, LOAD_NORMAL },
  { "sz3.02", 0x20000, 0x72445dc4, REGION_ROM2, 0x20000, LOAD_NORMAL },
  { "sz3.11m", 0x400000, 0x1c89eed1, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "sz3.12m", 0x400000, 0xf392b13a, REGION_SMP1, 0x400000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfa3u[] = // clone of _sfa3
{
  { "sz3u.03c", 0x80000, 0xe007da2e, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sz3u.04c", 0x80000, 0x5f78f0e7, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sz3.05c", 0x80000, 0x57fd0a40, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sz3.06c", 0x80000, 0xf6305f8b, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "sz3.07c", 0x80000, 0x6eab0f6f, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "sz3.08c", 0x80000, 0x910c4a3b, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "sz3.09c", 0x80000, 0xb29e5199, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "sz3.10b", 0x80000, 0xdeb2ff52, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfa3ur1[] = // clone of _sfa3
{
  { "sz3u.03", 0x80000, 0xb5984a19, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sz3u.04", 0x80000, 0x7e8158ba, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sz3.05", 0x80000, 0x9b21518a, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sz3.06", 0x80000, 0xe7a6c3a7, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "sz3.07", 0x80000, 0xec4c0cfd, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "sz3.08", 0x80000, 0x5c7e7240, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "sz3.09", 0x80000, 0xc5589553, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "sz3.10", 0x80000, 0xa9717252, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfz3j[] = // clone of _sfa3
{
  { "sz3j.03c", 0x80000, 0xcadf4a51, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sz3j.04c", 0x80000, 0xfcb31228, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sz3.05c", 0x80000, 0x57fd0a40, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sz3.06c", 0x80000, 0xf6305f8b, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "sz3.07c", 0x80000, 0x6eab0f6f, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "sz3.08c", 0x80000, 0x910c4a3b, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "sz3.09c", 0x80000, 0xb29e5199, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "sz3.10b", 0x80000, 0xdeb2ff52, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfz3jr1[] = // clone of _sfa3
{
  { "sz3j.03a", 0x80000, 0x6ee0beae, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sz3j.04a", 0x80000, 0xa6e2978d, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sz3.05a", 0x80000, 0x05964b7d, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sz3.06a", 0x80000, 0x78ce2179, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "sz3.07a", 0x80000, 0x398bf52f, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "sz3.08a", 0x80000, 0x866d0588, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "sz3.09a", 0x80000, 0x2180892c, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "sz3.10", 0x80000, 0xa9717252, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfz3jr2[] = // clone of _sfa3
{
  { "sz3j.03", 0x80000, 0xf7cb4b13, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sz3j.04", 0x80000, 0x0846c29d, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sz3.05", 0x80000, 0x9b21518a, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sz3.06", 0x80000, 0xe7a6c3a7, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "sz3.07", 0x80000, 0xec4c0cfd, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "sz3.08", 0x80000, 0x5c7e7240, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "sz3.09", 0x80000, 0xc5589553, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "sz3.10", 0x80000, 0xa9717252, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfz3a[] = // clone of _sfa3
{
  { "sz3a.03d", 0x80000, 0xd7e140d6, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sz3a.04d", 0x80000, 0xe06869a2, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sz3.05c", 0x80000, 0x57fd0a40, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sz3.06c", 0x80000, 0xf6305f8b, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "sz3.07c", 0x80000, 0x6eab0f6f, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "sz3.08c", 0x80000, 0x910c4a3b, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "sz3.09c", 0x80000, 0xb29e5199, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "sz3.10b", 0x80000, 0xdeb2ff52, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfz3ar1[] = // clone of _sfa3
{
  { "sz3a.03a", 0x80000, 0x29c681fd, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sz3a.04", 0x80000, 0x9ddd1484, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sz3.05", 0x80000, 0x9b21518a, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sz3.06", 0x80000, 0xe7a6c3a7, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "sz3.07", 0x80000, 0xec4c0cfd, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "sz3.08", 0x80000, 0x5c7e7240, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "sz3.09", 0x80000, 0xc5589553, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "sz3.10", 0x80000, 0xa9717252, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfa3b[] = // clone of _sfa3
{
  { "sz3b.03", 0x80000, 0x046c9b4d, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sz3b.04", 0x80000, 0xda211919, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sz3.05", 0x80000, 0x9b21518a, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sz3.06", 0x80000, 0xe7a6c3a7, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "sz3.07", 0x80000, 0xec4c0cfd, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "sz3.08", 0x80000, 0x5c7e7240, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "sz3.09", 0x80000, 0xc5589553, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "sz3.10", 0x80000, 0xa9717252, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sgemf[] =
{
  { "pcfu.03", 0x80000, 0xac2e8566, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "pcf.04", 0x80000, 0xf4314c96, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "pcf.05", 0x80000, 0x215655f6, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "pcf.06", 0x80000, 0xea6f13ea, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "pcf.07", 0x80000, 0x5ac6d5ea, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x0000000,  0x400000,
            "pcf.13m",  0x22d72ab9, "pcf.15m",  0x16a4813c,
            "pcf.17m",  0x1097e035, "pcf.19m",  0xd362d874),
  LOAD16_64(  REGION_GFX1,  0x1000000,  0x100000,
            "pcf.14m",  0x0383897c, "pcf.16m",  0x76f91084,
            "pcf.18m",  0x756c3754, "pcf.20m",  0x9ec9277d),
  { "pcf.01", 0x20000, 0x254e5f33, REGION_ROM2, 0, LOAD_NORMAL },
  { "pcf.02", 0x20000, 0x6902f4f9, REGION_ROM2, 0x20000, LOAD_NORMAL },
  { "pcf.11m", 0x400000, 0xa5dea005, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "pcf.12m", 0x400000, 0x4ce235fe, REGION_SMP1, 0x400000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_pfghtj[] = // clone of _sgemf
{
  { "pcfj.03", 0x80000, 0x681da43e, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "pcf.04", 0x80000, 0xf4314c96, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "pcf.05", 0x80000, 0x215655f6, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "pcf.06", 0x80000, 0xea6f13ea, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "pcf.07", 0x80000, 0x5ac6d5ea, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sgemfa[] = // clone of _sgemf
{
  { "pcfa.03", 0x80000, 0xe17c089a, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "pcf.04", 0x80000, 0xf4314c96, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "pcf.05", 0x80000, 0x215655f6, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "pcf.06", 0x80000, 0xea6f13ea, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "pcf.07", 0x80000, 0x5ac6d5ea, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sgemfh[] = // clone of _sgemf
{
  { "pcfh.03", 0x80000, 0xe9103347, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "pcf.04", 0x80000, 0xf4314c96, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "pcf.05", 0x80000, 0x215655f6, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "pcf.06", 0x80000, 0xea6f13ea, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "pcf.07", 0x80000, 0x5ac6d5ea, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_spf2t[] =
{
  { "pzfu.03a", 0x80000, 0x346e62ef, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "pzf.04", 0x80000, 0xb80649e2, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL(             0x000000, 0x800000, 0 ) */
  LOAD16_64(  REGION_GFX1,  0x800000,  0x100000,
            "pzf.14m",  0x2d4881cb, "pzf.16m",  0x4b0fd1be,
            "pzf.18m",  0xe43aac33, "pzf.20m",  0x7f536ff1),
  { "pzf.01", 0x20000, 0x600fb2a3, REGION_ROM2, 0, LOAD_NORMAL },
  { "pzf.02", 0x20000, 0x496076e0, REGION_ROM2, 0x20000, LOAD_NORMAL },
  { "pzf.11m", 0x200000, 0x78442743, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "pzf.12m", 0x200000, 0x399d2c7b, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_spf2xj[] = // clone of _spf2t
{
  { "pzfj.03a", 0x80000, 0x2070554a, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "pzf.04", 0x80000, 0xb80649e2, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL(             0x000000, 0x800000, 0 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_spf2ta[] = // clone of _spf2t
{
  { "pzfa.03", 0x80000, 0x3cecfa78, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "pzf.04", 0x80000, 0xb80649e2, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL(             0x000000, 0x800000, 0 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ssf2[] =
{
  { "ssfe.03", 0x80000, 0xa597745d, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "ssfe.04", 0x80000, 0xb082aa67, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "ssfe.05", 0x80000, 0x02b9c137, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "ssfe.06", 0x80000, 0x70d470c5, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "ssfe.07", 0x80000, 0x2409001d, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x000000,  0x200000,
            "ssf.13m",  0xcf94d275, "ssf.15m",  0x5eb703af,
            "ssf.17m",  0xffa60e0f, "ssf.19m",  0x34e825c5),
  LOAD16_64(  REGION_GFX1,  0x800000,  0x100000,
            "ssf.14m",  0xb7cc32e7, "ssf.16m",  0x8376ad18,
            "ssf.18m",  0xf5b1b336, "ssf.20m",  0x459d5c6b),
  { "ssf.01", 0x20000, 0xeb247e8c, REGION_ROM2, 0, LOAD_NORMAL },
  { "ssf.q01", 0x080000, 0xa6f9da5c, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "ssf.q02", 0x080000, 0x8c66ae26, REGION_SMP1, 0x080000, LOAD_NORMAL },
  { "ssf.q03", 0x080000, 0x695cc2ca, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "ssf.q04", 0x080000, 0x9d9ebe32, REGION_SMP1, 0x180000, LOAD_NORMAL },
  { "ssf.q05", 0x080000, 0x4770e7b7, REGION_SMP1, 0x200000, LOAD_NORMAL },
  { "ssf.q06", 0x080000, 0x4e79c951, REGION_SMP1, 0x280000, LOAD_NORMAL },
  { "ssf.q07", 0x080000, 0xcdd14313, REGION_SMP1, 0x300000, LOAD_NORMAL },
  { "ssf.q08", 0x080000, 0x6f5a088c, REGION_SMP1, 0x380000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ssf2u[] = // clone of _ssf2
{
  { "ssfu.03a", 0x80000, 0x72f29c33, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "ssfu.04a", 0x80000, 0x935cea44, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "ssfu.05", 0x80000, 0xa0acb28a, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "ssfu.06", 0x80000, 0x47413dcf, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "ssfu.07", 0x80000, 0xe6066077, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ssf2a[] = // clone of _ssf2
{
  { "ssfa.03b", 0x80000, 0x83a059bf, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "ssfa.04a", 0x80000, 0x5d873642, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "ssfa.05", 0x80000, 0xf8fb4de2, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "ssfa.06b", 0x80000, 0x3185d19d, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "ssfa.07", 0x80000, 0x36e29217, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ssf2ar1[] = // clone of _ssf2
{
  { "ssfa.03a", 0x80000, 0xd2a3c520, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "ssfa.04a", 0x80000, 0x5d873642, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "ssfa.05", 0x80000, 0xf8fb4de2, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "ssfa.06", 0x80000, 0xaa8acee7, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "ssfa.07", 0x80000, 0x36e29217, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ssf2j[] = // clone of _ssf2
{
  { "ssfj.03b", 0x80000, 0x5c2e356d, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "ssfj.04a", 0x80000, 0x013bd55c, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "ssfj.05", 0x80000, 0x0918d19a, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "ssfj.06b", 0x80000, 0x014e0c6d, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "ssfj.07", 0x80000, 0xeb6a9b1b, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ssf2jr1[] = // clone of _ssf2
{
  { "ssfj.03a", 0x80000, 0x0bbf1304, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "ssfj.04a", 0x80000, 0x013bd55c, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "ssfj.05", 0x80000, 0x0918d19a, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "ssfj.06", 0x80000, 0xd808a6cd, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "ssfj.07", 0x80000, 0xeb6a9b1b, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ssf2jr2[] = // clone of _ssf2
{
  { "ssfj.03", 0x80000, 0x7eb0efed, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "ssfj.04", 0x80000, 0xd7322164, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "ssfj.05", 0x80000, 0x0918d19a, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "ssfj.06", 0x80000, 0xd808a6cd, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "ssfj.07", 0x80000, 0xeb6a9b1b, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ssf2tb[] = // clone of _ssf2
{
  { "ssfe.3tc", 0x80000, 0x496a8409, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "ssfe.4tc", 0x80000, 0x4b45c18b, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "ssfe.5t", 0x80000, 0x6a9c6444, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "ssfe.6tb", 0x80000, 0xe4944fc3, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "ssfe.7t", 0x80000, 0x2c9f4782, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ssf2tbr1[] = // clone of _ssf2
{
  { "ssfe.3t", 0x80000, 0x1e018e34, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "ssfe.4t", 0x80000, 0xac92efaf, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "ssfe.5t", 0x80000, 0x6a9c6444, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "ssfe.6t", 0x80000, 0xf442562b, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "ssfe.7t", 0x80000, 0x2c9f4782, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ssf2tbj[] = // clone of _ssf2
{
  { "ssfj.3t", 0x80000, 0x980d4450, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "ssfj.4t", 0x80000, 0xb4dc1906, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "ssfj.5t", 0x80000, 0xa7e35fbc, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "ssfj.6t", 0x80000, 0xcb592b30, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "ssfj.7t", 0x80000, 0x1f239515, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ssf2t[] = // clone of _ssf2
{
  { "sfxe.03c", 0x80000, 0x2fa1f396, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sfxe.04a", 0x80000, 0xd0bc29c6, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sfxe.05", 0x80000, 0x65222964, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sfxe.06a", 0x80000, 0x8fe9f531, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "sfxe.07", 0x80000, 0x8a7d0cb6, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "sfxe.08", 0x80000, 0x74c24062, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "sfx.09", 0x80000, 0x642fae3f, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "sfx.01", 0x20000, 0xb47b8835, REGION_ROM2, 0, LOAD_NORMAL },
  { "sfx.02", 0x20000, 0x0022633f, REGION_ROM2, 0x20000, LOAD_NORMAL },
  { "sfx.11m", 0x200000, 0x9bdbd476, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "sfx.12m", 0x200000, 0xa05e3aab, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ssf2tu[] = // clone of _ssf2
{
  { "sfxu.03e", 0x80000, 0xd6ff689e, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sfxu.04a", 0x80000, 0x532b5ffd, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sfxu.05", 0x80000, 0xffa3c6de, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sfxu.06b", 0x80000, 0x83f9382b, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "sfxu.07a", 0x80000, 0x6ab673e8, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "sfxu.08", 0x80000, 0xb3c71810, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "sfx.09", 0x80000, 0x642fae3f, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "sfx.01", 0x20000, 0xb47b8835, REGION_ROM2, 0, LOAD_NORMAL },
  { "sfx.02", 0x20000, 0x0022633f, REGION_ROM2, 0x20000, LOAD_NORMAL },
  { "sfx.11m", 0x200000, 0x9bdbd476, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "sfx.12m", 0x200000, 0xa05e3aab, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ssf2tur1[] = // clone of _ssf2
{
  { "sfxu.03c", 0x80000, 0x86e4a335, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sfxu.04a", 0x80000, 0x532b5ffd, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sfxu.05", 0x80000, 0xffa3c6de, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sfxu.06a", 0x80000, 0xe4c04c99, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "sfxu.07", 0x80000, 0xd8199e41, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "sfxu.08", 0x80000, 0xb3c71810, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "sfx.09", 0x80000, 0x642fae3f, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "sfx.01", 0x20000, 0xb47b8835, REGION_ROM2, 0, LOAD_NORMAL },
  { "sfx.02", 0x20000, 0x0022633f, REGION_ROM2, 0x20000, LOAD_NORMAL },
  { "sfx.11m", 0x200000, 0x9bdbd476, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "sfx.12m", 0x200000, 0xa05e3aab, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ssf2ta[] = // clone of _ssf2
{
  { "sfxa.03c", 0x80000, 0x04b9ff34, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sfxa.04a", 0x80000, 0x16ea5f7a, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sfxa.05", 0x80000, 0x53d61f0c, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sfxa.06a", 0x80000, 0x066d09b5, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "sfxa.07", 0x80000, 0xa428257b, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "sfxa.08", 0x80000, 0x39be596c, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "sfx.09", 0x80000, 0x642fae3f, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "sfx.01", 0x20000, 0xb47b8835, REGION_ROM2, 0, LOAD_NORMAL },
  { "sfx.02", 0x20000, 0x0022633f, REGION_ROM2, 0x20000, LOAD_NORMAL },
  { "sfx.11m", 0x200000, 0x9bdbd476, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "sfx.12m", 0x200000, 0xa05e3aab, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ssf2xj[] = // clone of _ssf2
{
  { "sfxj.03c", 0x80000, 0xa7417b79, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sfxj.04a", 0x80000, 0xaf7767b4, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sfxj.05", 0x80000, 0xf4ff18f5, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sfxj.06a", 0x80000, 0x260d0370, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "sfxj.07", 0x80000, 0x1324d02a, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "sfxj.08", 0x80000, 0x2de76f10, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "sfx.09", 0x80000, 0x642fae3f, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "sfx.01", 0x20000, 0xb47b8835, REGION_ROM2, 0, LOAD_NORMAL },
  { "sfx.02", 0x20000, 0x0022633f, REGION_ROM2, 0x20000, LOAD_NORMAL },
  { "sfx.11m", 0x200000, 0x9bdbd476, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "sfx.12m", 0x200000, 0xa05e3aab, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_vhunt2[] =
{
  { "vh2j.03a", 0x80000, 0x9ae8f186, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "vh2j.04a", 0x80000, 0xe2fabf53, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "vh2j.05", 0x80000, 0xde34f624, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "vh2j.06", 0x80000, 0x6a3b9897, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "vh2j.07", 0x80000, 0xb021c029, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "vh2j.08", 0x80000, 0xac873dff, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "vh2j.09", 0x80000, 0xeaefce9c, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "vh2j.10", 0x80000, 0x11730952, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x0000000,  0x400000,
            "vh2.13m",  0x3b02ddaa, "vh2.15m",  0x4e40de66,
            "vh2.17m",  0xb31d00c9, "vh2.19m",  0x149be3ab),
  LOAD16_64(  REGION_GFX1,  0x1000000,  0x400000,
            "vh2.14m",  0xcd09bd63, "vh2.16m",  0xe0182c15,
            "vh2.18m",  0x778dc4f6, "vh2.20m",  0x605d9d1d),
  { "vh2.01", 0x20000, 0x67b9f779, REGION_ROM2, 0, LOAD_NORMAL },
  { "vh2.02", 0x20000, 0xaaf15fcb, REGION_ROM2, 0x20000, LOAD_NORMAL },
  { "vh2.11m", 0x400000, 0x38922efd, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "vh2.12m", 0x400000, 0x6e2430af, REGION_SMP1, 0x400000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_vhunt2r1[] = // clone of _vhunt2
{
  { "vh2j.03", 0x80000, 0x1a5feb13, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "vh2j.04", 0x80000, 0x434611a5, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "vh2j.05", 0x80000, 0xde34f624, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "vh2j.06", 0x80000, 0x6a3b9897, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "vh2j.07", 0x80000, 0xb021c029, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "vh2j.08", 0x80000, 0xac873dff, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "vh2j.09", 0x80000, 0xeaefce9c, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "vh2j.10", 0x80000, 0x11730952, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_vsav[] =
{
  { "vm3e.03d", 0x80000, 0xf5962a8c, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "vm3e.04d", 0x80000, 0x21b40ea2, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "vm3.05a", 0x80000, 0x4118e00f, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "vm3.06a", 0x80000, 0x2f4fd3a9, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "vm3.07b", 0x80000, 0xcbda91b8, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "vm3.08a", 0x80000, 0x6ca47259, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "vm3.09b", 0x80000, 0xf4a339e3, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "vm3.10b", 0x80000, 0xfffbb5b8, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x0000000,  0x400000,
            "vm3.13m",  0xfd8a11eb, "vm3.15m",  0xdd1e7d4e,
            "vm3.17m",  0x6b89445e, "vm3.19m",  0x3830fdc7),
  LOAD16_64(  REGION_GFX1,  0x1000000,  0x400000,
            "vm3.14m",  0xc1a28e6c, "vm3.16m",  0x194a7304,
            "vm3.18m",  0xdf9a9f47, "vm3.20m",  0xc22fc3d9),
  { "vm3.01", 0x20000, 0xf778769b, REGION_ROM2, 0, LOAD_NORMAL },
  { "vm3.02", 0x20000, 0xcc09faa1, REGION_ROM2, 0x20000, LOAD_NORMAL },
  { "vm3.11m", 0x400000, 0xe80e956e, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "vm3.12m", 0x400000, 0x9cd71557, REGION_SMP1, 0x400000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_vsavu[] = // clone of _vsav
{
  { "vm3u.03d", 0x80000, 0x1f295274, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "vm3u.04d", 0x80000, 0xc46adf81, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "vm3.05a", 0x80000, 0x4118e00f, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "vm3.06a", 0x80000, 0x2f4fd3a9, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "vm3.07b", 0x80000, 0xcbda91b8, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "vm3.08a", 0x80000, 0x6ca47259, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "vm3.09b", 0x80000, 0xf4a339e3, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "vm3.10b", 0x80000, 0xfffbb5b8, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_vsavj[] = // clone of _vsav
{
  { "vm3j.03d", 0x80000, 0x2a2e74a4, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "vm3j.04d", 0x80000, 0x1c2427bc, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "vm3j.05a", 0x80000, 0x95ce88d5, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "vm3j.06b", 0x80000, 0x2c4297e0, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "vm3j.07b", 0x80000, 0xa38aaae7, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "vm3j.08a", 0x80000, 0x5773e5c9, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "vm3j.09b", 0x80000, 0xd064f8b9, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "vm3j.10b", 0x80000, 0x434518e9, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_vsava[] = // clone of _vsav
{
  { "vm3a.03d", 0x80000, 0x44c1198f, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "vm3a.04d", 0x80000, 0x2218b781, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "vm3.05a", 0x80000, 0x4118e00f, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "vm3.06a", 0x80000, 0x2f4fd3a9, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "vm3.07b", 0x80000, 0xcbda91b8, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "vm3.08a", 0x80000, 0x6ca47259, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "vm3.09b", 0x80000, 0xf4a339e3, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "vm3.10b", 0x80000, 0xfffbb5b8, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_vsavh[] = // clone of _vsav
{
  { "vm3h.03a", 0x80000, 0x7cc62df8, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "vm3h.04d", 0x80000, 0xd716f3b5, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "vm3.05a", 0x80000, 0x4118e00f, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "vm3.06a", 0x80000, 0x2f4fd3a9, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "vm3.07b", 0x80000, 0xcbda91b8, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "vm3.08a", 0x80000, 0x6ca47259, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "vm3.09b", 0x80000, 0xf4a339e3, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "vm3.10b", 0x80000, 0xfffbb5b8, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_vsav2[] =
{
  { "vs2j.03", 0x80000, 0x89fd86b4, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "vs2j.04", 0x80000, 0x107c091b, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "vs2j.05", 0x80000, 0x61979638, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "vs2j.06", 0x80000, 0xf37c5bc2, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "vs2j.07", 0x80000, 0x8f885809, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "vs2j.08", 0x80000, 0x2018c120, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "vs2j.09", 0x80000, 0xfac3c217, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "vs2j.10", 0x80000, 0xeb490213, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x0000000,  0x400000,
            "vs2.13m",  0x5c852f52, "vs2.15m",  0xa20f58af,
            "vs2.17m",  0x39db59ad, "vs2.19m",  0x00c763a7),
  LOAD16_64(  REGION_GFX1,  0x1000000,  0x400000,
            "vs2.14m",  0xcd09bd63, "vs2.16m",  0xe0182c15,
            "vs2.18m",  0x778dc4f6, "vs2.20m",  0x605d9d1d),
  { "vs2.01", 0x20000, 0x35190139, REGION_ROM2, 0, LOAD_NORMAL },
  { "vs2.02", 0x20000, 0xc32dba09, REGION_ROM2, 0x20000, LOAD_NORMAL },
  { "vs2.11m", 0x400000, 0xd67e47b7, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "vs2.12m", 0x400000, 0x6d020a14, REGION_SMP1, 0x400000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_xmcota[] =
{
  { "xmne.03e", 0x80000, 0xa9a09b09, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "xmne.04e", 0x80000, 0x52fa2106, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "xmn.05a", 0x80000, 0xac0d7759, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "xmn.06a", 0x80000, 0x1b86a328, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "xmn.07a", 0x80000, 0x2c142a44, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "xmn.08a", 0x80000, 0xf712d44f, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "xmn.09a", 0x80000, 0x9241cae8, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "xmn.10a", 0x80000, 0x53c0eab9, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x0000000,  0x400000,
            "xmn.13m",  0xbf4df073, "xmn.15m",  0x4d7e4cef,
            "xmn.17m",  0x513eea17, "xmn.19m",  0xd23897fc),
  LOAD16_64(  REGION_GFX1,  0x1000000,  0x400000,
            "xmn.14m",  0x778237b7, "xmn.16m",  0x67b36948,
            "xmn.18m",  0x015a7c4c, "xmn.20m",  0x9dde2758),
  { "xmn.01a", 0x20000, 0x40f479ea, REGION_ROM2, 0, LOAD_NORMAL },
  { "xmn.02a", 0x20000, 0x39d9b5ad, REGION_ROM2, 0x20000, LOAD_NORMAL },
  { "xmn.11m", 0x200000, 0xc848a6bc, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "xmn.12m", 0x200000, 0x729c188f, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_xmcotau[] = // clone of _xmcota
{
  { "xmnu.03e", 0x80000, 0x0bafeb0e, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "xmnu.04e", 0x80000, 0xc29bdae3, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "xmn.05a", 0x80000, 0xac0d7759, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "xmn.06a", 0x80000, 0x1b86a328, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "xmn.07a", 0x80000, 0x2c142a44, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "xmn.08a", 0x80000, 0xf712d44f, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "xmn.09a", 0x80000, 0x9241cae8, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "xmn.10a", 0x80000, 0x53c0eab9, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_xmcotah[] = // clone of _xmcota
{
  { "xmnh.03", 0x80000, 0xe4b85a90, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "xmnh.04", 0x80000, 0x7dfe1406, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "xmnh.05", 0x80000, 0x87b0ed0f, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "xmn.06a", 0x80000, 0x1b86a328, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "xmn.07a", 0x80000, 0x2c142a44, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "xmn.08a", 0x80000, 0xf712d44f, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "xmn.09a", 0x80000, 0x9241cae8, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "xmnh.10", 0x80000, 0xcb36b0a4, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_xmcotaj[] = // clone of _xmcota
{
  { "xmnj.03d", 0x80000, 0x79086d62, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "xmnj.04d", 0x80000, 0x38eed613, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "xmn.05", 0x80000, 0xc3ed62a2, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "xmn.06", 0x80000, 0xf03c52e1, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "xmn.07", 0x80000, 0x325626b1, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "xmn.08", 0x80000, 0x7194ea10, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "xmn.09", 0x80000, 0xae946df3, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "xmn.10", 0x80000, 0x32a6be1d, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_xmcotaj1[] = // clone of _xmcota
{
  { "xmnj.03b", 0x80000, 0xc8175fb3, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "xmnj.04b", 0x80000, 0x54b3fba3, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "xmn.05", 0x80000, 0xc3ed62a2, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "xmn.06", 0x80000, 0xf03c52e1, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "xmn.07", 0x80000, 0x325626b1, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "xmn.08", 0x80000, 0x7194ea10, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "xmn.09", 0x80000, 0xae946df3, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "xmn.10", 0x80000, 0x32a6be1d, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_xmcotaj2[] = // clone of _xmcota
{
  { "xmnj.03a", 0x80000, 0x00761611, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "xmnj.04a", 0x80000, 0x614d3f60, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "xmn.05", 0x80000, 0xc3ed62a2, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "xmn.06", 0x80000, 0xf03c52e1, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "xmn.07", 0x80000, 0x325626b1, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "xmn.08", 0x80000, 0x7194ea10, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "xmn.09", 0x80000, 0xae946df3, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "xmn.10", 0x80000, 0x32a6be1d, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_xmcotajr[] = // clone of _xmcota
{
  { "xmno.03a", 0x80000, 0x7ab19acf, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "xmno.04a", 0x80000, 0x7615dd21, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "xmno.05a", 0x80000, 0x0303d672, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "xmno.06a", 0x80000, 0x332839a5, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "xmno.07", 0x80000, 0x6255e8d5, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "xmno.08", 0x80000, 0xb8ebe77c, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "xmno.09", 0x80000, 0x5440d950, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "xmno.10a", 0x80000, 0xb8296966, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { "xmn.01", 0x20000, 0x7178336e, REGION_ROM2, 0, LOAD_NORMAL },
  { "xmn.02", 0x20000, 0x0ec58501, REGION_ROM2, 0x20000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_xmcotaa[] = // clone of _xmcota
{
  { "xmna.03a", 0x80000, 0x7df8b27e, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "xmna.04a", 0x80000, 0xb44e30a7, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "xmn.05", 0x80000, 0xc3ed62a2, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "xmn.06", 0x80000, 0xf03c52e1, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "xmn.07", 0x80000, 0x325626b1, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "xmn.08", 0x80000, 0x7194ea10, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "xmn.09", 0x80000, 0xae946df3, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "xmn.10", 0x80000, 0x32a6be1d, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_xmvsf[] =
{
  { "xvse.03f", 0x80000, 0xdb06413f, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "xvse.04f", 0x80000, 0xef015aef, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "xvs.05a", 0x80000, 0x7db6025d, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "xvs.06a", 0x80000, 0xe8e2c75c, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "xvs.07", 0x80000, 0x08f0abed, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "xvs.08", 0x80000, 0x81929675, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "xvs.09", 0x80000, 0x9641f36b, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  LOAD16_64(  REGION_GFX1,  0x0000000,  0x400000,
            "xvs.13m",  0xf6684efd, "xvs.15m",  0x29109221,
            "xvs.17m",  0x92db3474, "xvs.19m",  0x3733473c),
  LOAD16_64(  REGION_GFX1,  0x1000000,  0x400000,
            "xvs.14m",  0xbcac2e41, "xvs.16m",  0xea04a272,
            "xvs.18m",  0xb0def86a, "xvs.20m",  0x4b40ff9f),
  { "xvs.01", 0x20000, 0x3999e93a, REGION_ROM2, 0, LOAD_NORMAL },
  { "xvs.02", 0x20000, 0x101bdee9, REGION_ROM2, 0x20000, LOAD_NORMAL },
  { "xvs.11m", 0x200000, 0x9cadcdbc, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "xvs.12m", 0x200000, 0x7b11e460, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_xmvsfr1[] = // clone of _xmvsf
{
  { "xvse.03d", 0x80000, 0x5ae5bd3b, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "xvse.04d", 0x80000, 0x5eb9c02e, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "xvs.05a", 0x80000, 0x7db6025d, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "xvs.06a", 0x80000, 0xe8e2c75c, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "xvs.07", 0x80000, 0x08f0abed, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "xvs.08", 0x80000, 0x81929675, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "xvs.09", 0x80000, 0x9641f36b, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_xmvsfu[] = // clone of _xmvsf
{
  { "xvsu.03k", 0x80000, 0x8739ef61, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "xvsu.04k", 0x80000, 0xe11d35c1, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "xvs.05a", 0x80000, 0x7db6025d, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "xvs.06a", 0x80000, 0xe8e2c75c, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "xvs.07", 0x80000, 0x08f0abed, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "xvs.08", 0x80000, 0x81929675, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "xvs.09", 0x80000, 0x9641f36b, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_xmvsfur1[] = // clone of _xmvsf
{
  { "xvsu.03h", 0x80000, 0x5481155a, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "xvsu.04h", 0x80000, 0x1e236388, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "xvs.05a", 0x80000, 0x7db6025d, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "xvs.06a", 0x80000, 0xe8e2c75c, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "xvs.07", 0x80000, 0x08f0abed, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "xvs.08", 0x80000, 0x81929675, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "xvs.09", 0x80000, 0x9641f36b, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_xmvsfj[] = // clone of _xmvsf
{
  { "xvsj.03i", 0x80000, 0xef24da96, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "xvsj.04i", 0x80000, 0x70a59b35, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "xvs.05a", 0x80000, 0x7db6025d, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "xvs.06a", 0x80000, 0xe8e2c75c, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "xvs.07", 0x80000, 0x08f0abed, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "xvs.08", 0x80000, 0x81929675, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "xvs.09", 0x80000, 0x9641f36b, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_xmvsfjr1[] = // clone of _xmvsf
{
  { "xvsj.03d", 0x80000, 0xbeb81de9, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "xvsj.04d", 0x80000, 0x23d11271, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "xvs.05a", 0x80000, 0x7db6025d, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "xvs.06a", 0x80000, 0xe8e2c75c, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "xvs.07", 0x80000, 0x08f0abed, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "xvs.08", 0x80000, 0x81929675, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "xvs.09", 0x80000, 0x9641f36b, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_xmvsfjr2[] = // clone of _xmvsf
{
  { "xvsj.03c", 0x80000, 0x180656a1, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "xvsj.04c", 0x80000, 0x5832811c, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "xvs.05", 0x80000, 0x030e0e1e, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "xvs.06", 0x80000, 0x5d04a8ff, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "xvs.07", 0x80000, 0x08f0abed, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "xvs.08", 0x80000, 0x81929675, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "xvs.09", 0x80000, 0x9641f36b, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_xmvsfa[] = // clone of _xmvsf
{
  { "xvsa.03k", 0x80000, 0xd0cca7a8, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "xvsa.04k", 0x80000, 0x8c8e76fd, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "xvs.05a", 0x80000, 0x7db6025d, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "xvs.06a", 0x80000, 0xe8e2c75c, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "xvs.07", 0x80000, 0x08f0abed, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "xvs.08", 0x80000, 0x81929675, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "xvs.09", 0x80000, 0x9641f36b, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_xmvsfar1[] = // clone of _xmvsf
{
  { "xvsa.03e", 0x80000, 0x9bdde21c, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "xvsa.04e", 0x80000, 0x33300edf, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "xvs.05a", 0x80000, 0x7db6025d, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "xvs.06a", 0x80000, 0xe8e2c75c, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "xvs.07", 0x80000, 0x08f0abed, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "xvs.08", 0x80000, 0x81929675, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "xvs.09", 0x80000, 0x9641f36b, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_xmvsfh[] = // clone of _xmvsf
{
  { "xvsh.03a", 0x80000, 0xd4fffb04, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "xvsh.04a", 0x80000, 0x1b4ea638, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "xvs.05a", 0x80000, 0x7db6025d, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "xvs.06a", 0x80000, 0xe8e2c75c, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "xvs.07", 0x80000, 0x08f0abed, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "xvs.08", 0x80000, 0x81929675, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "xvs.09", 0x80000, 0x9641f36b, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_xmvsfb[] = // clone of _xmvsf
{
  { "xvsb.03h", 0x80000, 0x05baccca, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "xvsb.04h", 0x80000, 0xe350c755, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "xvs.05a", 0x80000, 0x7db6025d, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "xvs.06a", 0x80000, 0xe8e2c75c, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "xvs.07", 0x80000, 0x08f0abed, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "xvs.08", 0x80000, 0x81929675, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "xvs.09", 0x80000, 0x9641f36b, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_1944d[] = // clone of _1944
{
  { "nffud.03", 0x80000, 0x28e8aae4, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "nff.04", 0x80000, 0xdba1c66e, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "nffu.05", 0x80000, 0xea813eb7, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL( 0x000000, 0x100000, 0 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_19xxd[] = // clone of _19xx
{
  { "19xud.03", 0x80000, 0xf81b60e5, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "19xud.04", 0x80000, 0xcc44638c, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "19xud.05", 0x80000, 0x33a168de, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "19xud.06", 0x80000, 0xe0111282, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "19x.07", 0x80000, 0x61c0296c, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL( 0x000000, 0x100000, 0 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfz2ad[] = // clone of _sfa2
{
  { "sz2ad.03a", 0x80000, 0x017f8fab, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sz2ad.04a", 0x80000, 0xf50e5ea2, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sz2.05a", 0x80000, 0x98e8e992, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sz2.06", 0x80000, 0x5b1d49c0, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "sz2a.07a", 0x80000, 0x0aed2494, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "sz2.08", 0x80000, 0x0fe8585d, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_avspd[] = // clone of _avsp
{
  { "avped.03d", 0x80000, 0x66aa8aad, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "avped.04d", 0x80000, 0x579306c2, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "avp.05d", 0x80000, 0xfbfb5d7a, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "avpd.06", 0x80000, 0x63094539, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL( 0x000000, 0x100000, 0 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_batcird[] = // clone of _batcir
{
  { "btced.03", 0x80000, 0x0737db6d, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "btced.04", 0x80000, 0xef1a8823, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "btced.05", 0x80000, 0x20bdbb14, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "btced.06", 0x80000, 0xb4d8f5bc, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "btc.07", 0x80000, 0x7322d5db, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "btc.08", 0x80000, 0x6aac85ab, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "btc.09", 0x80000, 0x1203db08, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL( 0x000000, 0x100000, 0 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ddtodd[] = // clone of _ddtod
{
  { "daded.03c", 0x80000, 0x843330f4, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "daded.04c", 0x80000, 0x306f14fc, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "daded.05c", 0x80000, 0x8c6b8328, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "dad.06a", 0x80000, 0x6225495a, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "dadd.07a", 0x80000, 0x0f0df6cc, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL( 0x000000, 0x100000, 0 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ddsomud[] = // clone of _ddsom
{
  { "dd2ud.03g", 0x80000, 0x816f695a, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "dd2ud.04g", 0x80000, 0x7cc81c6b, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "dd2.05g", 0x80000, 0x5eb1991c, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "dd2.06g", 0x80000, 0xc26b5e55, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "dd2.07", 0x80000, 0x909a0b8b, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "dd2.08", 0x80000, 0xe53c4d01, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "dd2.09", 0x80000, 0x5f86279f, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "dd2d.10", 0x80000, 0x0c172f8f, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL( 0x000000, 0x100000, 0 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_gwingjd[] = // clone of _gigawing
{
  { "ggwjd.03a", 0x80000, 0xcb1c756e, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "ggwjd.04a", 0x80000, 0xfa158e04, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "ggwjd.05a", 0x80000, 0x1c5bc4e7, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL( 0x000000, 0x100000, 0 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_hsf2d[] = // clone of _hsf2
{
  { "hs2ad.03", 0x80000, 0x0153d371, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "hs2ad.04", 0x80000, 0x0276b78a, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "hs2.05", 0x80000, 0xdde34a35, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "hs2.06", 0x80000, 0xf4e56dda, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "hs2a.07", 0x80000, 0xee4420fc, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "hs2.08", 0x80000, 0xc9441533, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "hs2.09", 0x80000, 0x3fc638a8, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "hs2.10", 0x80000, 0x20d0f9e4, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL( 0x000000, 0x100000, 0 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_megamn2d[] = // clone of _megaman2
{
  { "rm2ud.03", 0x80000, 0xd3635f25, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "rm2ud.04", 0x80000, 0x768a1705, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "rm2.05", 0x80000, 0x02ee9efc, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL( 0x000000, 0x100000, 0 ) */
/* Ignored : 	ROM_FILL(               0x000000, 0x800000, 0 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mvscud[] = // clone of _mvsc
{
  { "mvcud.03d", 0x80000, 0x75cde3e5, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "mvcud.04d", 0x80000, 0xb32ea484, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "mvc.05a", 0x80000, 0x2d8c8e86, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "mvc.06a", 0x80000, 0x8528e1f5, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "mvcd.07", 0x80000, 0x205293e9, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "mvc.08", 0x80000, 0xbc002fcd, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "mvc.09", 0x80000, 0xc67b26df, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "mvc.10", 0x80000, 0x0fdd1e26, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL( 0x000000, 0x100000, 0 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_nwarrud[] = // clone of _nwarr
{
  { "vphud.03f", 0x80000, 0x20d4d5a8, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "vphud.04c", 0x80000, 0x61be9b42, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "vphud.05e", 0x80000, 0x1ba906d8, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "vphu.06c", 0x80000, 0x08c04cdb, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "vphu.07b", 0x80000, 0xb5a5ab19, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "vphu.08b", 0x80000, 0x51bb20fb, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "vphu.09b", 0x80000, 0x41a64205, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "vphud.10b", 0x80000, 0x9619adad, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL( 0x000000, 0x100000, 0 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ringdstd[] = // clone of _ringdest
{
  { "smbed.03b", 0x80000, 0xf6fba4cd, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "smbed.04b", 0x80000, 0x193bc493, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "smbed.05b", 0x80000, 0x168cccbb, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "smbed.06b", 0x80000, 0x04673262, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "smb.07", 0x80000, 0xb9a11577, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "smb.08", 0x80000, 0xf931b76b, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL( 0x000000, 0x100000, 0 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfad[] = // clone of _sfa
{
  { "sfzed.03d", 0x80000, 0xa1a54827, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sfz.04b", 0x80000, 0x8b73b0e5, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sfz.05a", 0x80000, 0x0810544d, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sfz.06", 0x80000, 0x806e8f38, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL( 0x000000, 0x100000, 0 ) */
/* Ignored : 	ROM_FILL(               0x000000, 0x800000, 0 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_mshud[] = // clone of _msh
{
  { "mshud.03", 0x80000, 0xc1d8c4c6, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "mshud.04", 0x80000, 0xe73dda16, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "mshud.05", 0x80000, 0x3b493e84, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "msh.06b", 0x80000, 0x803e3fa4, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "msh.07a", 0x80000, 0xc45f8e27, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "msh.08a", 0x80000, 0x9ca6f12c, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "msh.09a", 0x80000, 0x82ec27af, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "msh.10b", 0x80000, 0x8d931196, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfz2ald[] = // clone of _sfz2al
{
  { "szaad.03", 0x80000, 0x89f9483b, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "szaad.04", 0x80000, 0xaef27ae5, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "szaa.05", 0x80000, 0xf053a55e, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "szaa.06", 0x80000, 0xcfc0e7a8, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "szaa.07", 0x80000, 0x5feb8b20, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "szaa.08", 0x80000, 0x6eb6d412, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL( 0x000000, 0x100000, 0 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sfa3ud[] = // clone of _sfa3
{
  { "sz3ud.03c", 0x80000, 0x6db8add7, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sz3ud.04c", 0x80000, 0xd9c65a26, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sz3.05c", 0x80000, 0x57fd0a40, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sz3.06c", 0x80000, 0xf6305f8b, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "sz3.07c", 0x80000, 0x6eab0f6f, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "sz3.08c", 0x80000, 0x910c4a3b, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "sz3.09c", 0x80000, 0xb29e5199, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "sz3.10b", 0x80000, 0xdeb2ff52, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL( 0x000000, 0x100000, 0 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_spf2xjd[] = // clone of _spf2t
{
  { "pzfjd.03a", 0x80000, 0x5e85ed08, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "pzf.04", 0x80000, 0xb80649e2, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL( 0x000000, 0x100000, 0 ) */
/* Ignored : 	ROM_FILL(              0x000000, 0x800000, 0 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_vsavd[] = // clone of _vsav
{
  { "vm3ed.03d", 0x80000, 0x97d805e3, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "vm3ed.04d", 0x80000, 0x5e07fdce, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "vm3.05a", 0x80000, 0x4118e00f, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "vm3.06a", 0x80000, 0x2f4fd3a9, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "vm3.07b", 0x80000, 0xcbda91b8, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "vm3.08a", 0x80000, 0x6ca47259, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "vm3.09b", 0x80000, 0xf4a339e3, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "vm3.10b", 0x80000, 0xfffbb5b8, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL( 0x000000, 0x100000, 0 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_xmcotad[] = // clone of _xmcota
{
  { "xmned.03e", 0x80000, 0xbef56003, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "xmned.04e", 0x80000, 0xb1a21fa6, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "xmn.05a", 0x80000, 0xac0d7759, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "xmn.06a", 0x80000, 0x1b86a328, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "xmn.07a", 0x80000, 0x2c142a44, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "xmn.08a", 0x80000, 0xf712d44f, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "xmn.09a", 0x80000, 0x9241cae8, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "xmn.10a", 0x80000, 0x53c0eab9, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL( 0x000000, 0x100000, 0 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_xmvsfu1d[] = // clone of _xmvsf
{
  { "xvsud.03h", 0x80000, 0x4e2e76b7, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "xvsud.04h", 0x80000, 0x290c61a7, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "xvsd.05a", 0x80000, 0xde347b11, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "xvs.06a", 0x80000, 0xe8e2c75c, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "xvsd.07", 0x80000, 0xf761ded7, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "xvs.08", 0x80000, 0x81929675, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "xvs.09", 0x80000, 0x9641f36b, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
/* Ignored : 	ROM_FILL( 0x000000, 0x100000, 0 ) */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_dstlku1d[] = // clone of _dstlk
{
  { "vamud.03a", 0x80000, 0x47b7a680, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "vamud.04a", 0x80000, 0x3b7a4939, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "vamu.05a", 0x80000, 0x673ed50a, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "vamu.06a", 0x80000, 0xf2377be7, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "vamu.07a", 0x80000, 0xd8f498c4, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "vamu.08a", 0x80000, 0xe6a8a1a0, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "vamud.09a", 0x80000, 0x8b333a19, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "vamu.10a", 0x80000, 0xc1a3d9be, REGION_ROM1, 0x380000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_progerjd[] = // clone of _progear
{
  { "pgajd.03", 0x80000, 0x4fef676c, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "pgajd.04", 0x80000, 0xa069bd3b, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ssf2ud[] = // clone of _ssf2
{
  { "ssfud.03a", 0x80000, 0xfad5daf8, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "ssfud.04a", 0x80000, 0x0d31af65, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "ssfud.05", 0x80000, 0x75c651ef, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "ssfud.06", 0x80000, 0x85c3ec00, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "ssfud.07", 0x80000, 0x247e2504, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ssf2tbd[] = // clone of _ssf2
{
  { "ssfed.3tc", 0x80000, 0x5d86caf8, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "ssfed.4tc", 0x80000, 0xf6e1f98d, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "ssfed.5t", 0x80000, 0x75c651ef, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "ssfed.6tb", 0x80000, 0x9adac7d7, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "ssfed.7t", 0x80000, 0x84f54db3, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ssf2xjd[] = // clone of _ssf2
{
  { "sfxjd.03c", 0x80000, 0x316de996, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "sfxjd.04a", 0x80000, 0x9bf3bb2e, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "sfxjd.05", 0x80000, 0xc63358d0, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "sfxjd.06a", 0x80000, 0xccb29808, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "sfxjd.07", 0x80000, 0x61f94982, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { "sfxjd.08", 0x80000, 0xd399c36c, REGION_ROM1, 0x280000, LOAD_SWAP_16 },
  { "sfxd.09", 0x80000, 0x0b3a6196, REGION_ROM1, 0x300000, LOAD_SWAP_16 },
  { "sfx.01", 0x20000, 0xb47b8835, REGION_ROM2, 0, LOAD_NORMAL },
  { "sfx.02", 0x20000, 0x0022633f, REGION_ROM2, 0x20000, LOAD_NORMAL },
  { "sfx.11m", 0x200000, 0x9bdbd476, REGION_SMP1, 0x000000, LOAD_SWAP_16 },
  { "sfx.12m", 0x200000, 0xa05e3aab, REGION_SMP1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sgemfd[] = // clone of _sgemf
{
  { "pcfud.03", 0x80000, 0x8b83674a, REGION_ROM1, 0x000000, LOAD_SWAP_16 },
  { "pcfd.04", 0x80000, 0xb58f1d03, REGION_ROM1, 0x080000, LOAD_SWAP_16 },
  { "pcf.05", 0x80000, 0x215655f6, REGION_ROM1, 0x100000, LOAD_SWAP_16 },
  { "pcf.06", 0x80000, 0xea6f13ea, REGION_ROM1, 0x180000, LOAD_SWAP_16 },
  { "pcf.07", 0x80000, 0x5ac6d5ea, REGION_ROM1, 0x200000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct INPUT_INFO input_p4b4[] =
{
  INP0( P1_RIGHT, 0x00, 0x01 ),
  INP0( P1_LEFT, 0x00, 0x02 ),
  INP0( P1_DOWN, 0x00, 0x04 ),
  INP0( P1_UP, 0x00, 0x08 ),
  INP0( P1_B1, 0x00, 0x10 ),
  INP0( P1_B2, 0x00, 0x20 ),
  INP0( P1_B3, 0x00, 0x40 ),
  INP0( P1_B4, 0x00, 0x80 ),
  INP0( P2_RIGHT, 0x01, 0x01 ),
  INP0( P2_LEFT, 0x01, 0x02 ),
  INP0( P2_DOWN, 0x01, 0x04 ),
  INP0( P2_UP, 0x01, 0x08 ),
  INP0( P2_B1, 0x01, 0x10 ),
  INP0( P2_B2, 0x01, 0x20 ),
  INP0( P2_B3, 0x01, 0x40 ),
  INP0( P2_B4, 0x01, 0x80 ),

  INP0( P3_RIGHT, 0x02, 0x01 ),
  INP0( P3_LEFT, 0x02, 0x02 ),
  INP0( P3_DOWN, 0x02, 0x04 ),
  INP0( P3_UP, 0x02, 0x08 ),
  INP0( P3_B1, 0x02, 0x10 ),
  INP0( P3_B2, 0x02, 0x20 ),
  INP0( P3_B3, 0x02, 0x40 ),
  INP0( P3_B4, 0x02, 0x80 ),
  INP0( P4_RIGHT, 0x03, 0x01 ),
  INP0( P4_LEFT, 0x03, 0x02 ),
  INP0( P4_DOWN, 0x03, 0x04 ),
  INP0( P4_UP, 0x03, 0x08 ),
  INP0( P4_B1, 0x03, 0x10 ),
  INP0( P4_B2, 0x03, 0x20 ),
  INP0( P4_B3, 0x03, 0x40 ),
  INP0( P4_B4, 0x03, 0x80 ),

  INP0( TEST, 0x04, 0x02 ),
  INP0( SERVICE, 0x04, 0x04 ),
  INP0( P1_START, 0x05, 0x01 ),
  INP0( P2_START, 0x05, 0x02 ),
  INP0( P3_START, 0x05, 0x04 ),
  INP0( P4_START, 0x05, 0x08 ),
  INP0( COIN1, 0x05, 0x10 ),
  INP0( COIN2, 0x05, 0x20 ),
  INP0( COIN3, 0x05, 0x40 ),
  INP0( COIN4, 0x05, 0x80 ),
   END_INPUT
};

static struct INPUT_INFO input_p4b2[] =
{
  INP0( P1_RIGHT, 0x00, 0x01 ),
  INP0( P1_LEFT, 0x00, 0x02 ),
  INP0( P1_DOWN, 0x00, 0x04 ),
  INP0( P1_UP, 0x00, 0x08 ),
  INP0( P1_B1, 0x00, 0x10 ),
  INP0( P1_B2, 0x00, 0x20 ),
  INP0( P2_RIGHT, 0x01, 0x01 ),
  INP0( P2_LEFT, 0x01, 0x02 ),
  INP0( P2_DOWN, 0x01, 0x04 ),
  INP0( P2_UP, 0x01, 0x08 ),
  INP0( P2_B1, 0x01, 0x10 ),
  INP0( P2_B2, 0x01, 0x20 ),

  INP0( P3_RIGHT, 0x02, 0x01 ),
  INP0( P3_LEFT, 0x02, 0x02 ),
  INP0( P3_DOWN, 0x02, 0x04 ),
  INP0( P3_UP, 0x02, 0x08 ),
  INP0( P3_B1, 0x02, 0x10 ),
  INP0( P3_B2, 0x02, 0x20 ),
  INP0( P4_RIGHT, 0x03, 0x01 ),
  INP0( P4_LEFT, 0x03, 0x02 ),
  INP0( P4_DOWN, 0x03, 0x04 ),
  INP0( P4_UP, 0x03, 0x08 ),
  INP0( P4_B1, 0x03, 0x10 ),
  INP0( P4_B2, 0x03, 0x20 ),

  INP0( TEST, 0x04, 0x02 ),
  INP0( SERVICE, 0x04, 0x04 ),
  INP0( P1_START, 0x05, 0x01 ),
  INP0( P2_START, 0x05, 0x02 ),
  INP0( P3_START, 0x05, 0x04 ),
  INP0( P4_START, 0x05, 0x08 ),
  INP0( COIN1, 0x05, 0x10 ),
  INP0( COIN2, 0x05, 0x20 ),
  INP0( COIN3, 0x05, 0x40 ),
  INP0( COIN4, 0x05, 0x80 ),
   END_INPUT
};

static struct INPUT_INFO input_p3b3[] =
{
  INP0( P1_RIGHT, 0x00, 0x01 ),
  INP0( P1_LEFT, 0x00, 0x02 ),
  INP0( P1_DOWN, 0x00, 0x04 ),
  INP0( P1_UP, 0x00, 0x08 ),
  INP0( P1_B1, 0x00, 0x10 ),
  INP0( P1_B2, 0x00, 0x20 ),
  INP0( P1_B3, 0x00, 0x40 ),
  INP0( P2_RIGHT, 0x01, 0x01 ),
  INP0( P2_LEFT, 0x01, 0x02 ),
  INP0( P2_DOWN, 0x01, 0x04 ),
  INP0( P2_UP, 0x01, 0x08 ),
  INP0( P2_B1, 0x01, 0x10 ),
  INP0( P2_B2, 0x01, 0x20 ),
  INP0( P2_B3, 0x01, 0x40 ),

  INP0( P3_RIGHT, 0x02, 0x01 ),
  INP0( P3_LEFT, 0x02, 0x02 ),
  INP0( P3_DOWN, 0x02, 0x04 ),
  INP0( P3_UP, 0x02, 0x08 ),
  INP0( P3_B1, 0x02, 0x10 ),
  INP0( P3_B2, 0x02, 0x20 ),
  INP0( P3_B3, 0x02, 0x40 ),

  INP0( TEST, 0x04, 0x02 ),
  INP0( SERVICE, 0x04, 0x04 ),
  INP0( P1_START, 0x05, 0x01 ),
  INP0( P2_START, 0x05, 0x02 ),
  INP0( P3_START, 0x05, 0x04 ),
  INP0( COIN1, 0x05, 0x10 ),
  INP0( COIN2, 0x05, 0x20 ),
  INP0( COIN3, 0x05, 0x40 ),
   END_INPUT
};

static struct INPUT_INFO input_p2b6[] =
{
  INP0( P1_RIGHT, 0x00, 0x01 ),
  INP0( P1_LEFT, 0x00, 0x02 ),
  INP0( P1_DOWN, 0x00, 0x04 ),
  INP0( P1_UP, 0x00, 0x08 ),
  INP0( P1_B1, 0x00, 0x10 ),
  INP0( P1_B2, 0x00, 0x20 ),
  INP0( P1_B3, 0x00, 0x40 ),
  INP0( P2_RIGHT, 0x01, 0x01 ),
  INP0( P2_LEFT, 0x01, 0x02 ),
  INP0( P2_DOWN, 0x01, 0x04 ),
  INP0( P2_UP, 0x01, 0x08 ),
  INP0( P2_B1, 0x01, 0x10 ),
  INP0( P2_B2, 0x01, 0x20 ),
  INP0( P2_B3, 0x01, 0x40 ),

  INP0( P1_B4, 0x02, 0x01 ),
  INP0( P1_B5, 0x02, 0x02 ),
  INP0( P1_B6, 0x02, 0x04 ),
  INP0( P2_B4, 0x02, 0x10 ),
  INP0( P2_B5, 0x02, 0x20 ),

  INP0( TEST, 0x04, 0x02 ),
  INP0( SERVICE, 0x04, 0x04 ),
  INP0( P1_START, 0x05, 0x01 ),
  INP0( P2_START, 0x05, 0x02 ),
  INP0( COIN1, 0x05, 0x10 ),
  INP0( COIN2, 0x05, 0x20 ),
  INP0( P2_B6, 0x05, 0x40 ),
   END_INPUT
};

static struct INPUT_INFO input_p2b4[] =
{
  INP0( P1_RIGHT, 0x00, 0x01 ),
  INP0( P1_LEFT, 0x00, 0x02 ),
  INP0( P1_DOWN, 0x00, 0x04 ),
  INP0( P1_UP, 0x00, 0x08 ),
  INP0( P1_B1, 0x00, 0x10 ),
  INP0( P1_B2, 0x00, 0x20 ),
  INP0( P1_B3, 0x00, 0x40 ),
  INP0( P2_RIGHT, 0x01, 0x01 ),
  INP0( P2_LEFT, 0x01, 0x02 ),
  INP0( P2_DOWN, 0x01, 0x04 ),
  INP0( P2_UP, 0x01, 0x08 ),
  INP0( P2_B1, 0x01, 0x10 ),
  INP0( P2_B2, 0x01, 0x20 ),
  INP0( P2_B3, 0x01, 0x40 ),

  INP0( P1_B4, 0x02, 0x01 ),
  INP0( P2_B4, 0x02, 0x10 ),

  INP0( TEST, 0x04, 0x02 ),
  INP0( SERVICE, 0x04, 0x04 ),
  INP0( P1_START, 0x05, 0x01 ),
  INP0( P2_START, 0x05, 0x02 ),
  INP0( COIN1, 0x05, 0x10 ),
  INP0( COIN2, 0x05, 0x20 ),
   END_INPUT
};

static struct INPUT_INFO input_p2b3[] =
{
  INP0( P1_RIGHT, 0x00, 0x01 ),
  INP0( P1_LEFT, 0x00, 0x02 ),
  INP0( P1_DOWN, 0x00, 0x04 ),
  INP0( P1_UP, 0x00, 0x08 ),
  INP0( P1_B1, 0x00, 0x10 ),
  INP0( P1_B2, 0x00, 0x20 ),
  INP0( P1_B3, 0x00, 0x40 ),
  INP0( P2_RIGHT, 0x01, 0x01 ),
  INP0( P2_LEFT, 0x01, 0x02 ),
  INP0( P2_DOWN, 0x01, 0x04 ),
  INP0( P2_UP, 0x01, 0x08 ),
  INP0( P2_B1, 0x01, 0x10 ),
  INP0( P2_B2, 0x01, 0x20 ),
  INP0( P2_B3, 0x01, 0x40 ),

  INP0( TEST, 0x04, 0x02 ),
  INP0( SERVICE, 0x04, 0x04 ),
  INP0( P1_START, 0x05, 0x01 ),
  INP0( P2_START, 0x05, 0x02 ),
  INP0( COIN1, 0x05, 0x10 ),
  INP0( COIN2, 0x05, 0x20 ),
   END_INPUT
};

static struct INPUT_INFO input_p2b2[] =
{
  INP0( P1_RIGHT, 0x00, 0x01 ),
  INP0( P1_LEFT, 0x00, 0x02 ),
  INP0( P1_DOWN, 0x00, 0x04 ),
  INP0( P1_UP, 0x00, 0x08 ),
  INP0( P1_B1, 0x00, 0x10 ),
  INP0( P1_B2, 0x00, 0x20 ),
  INP0( P2_RIGHT, 0x01, 0x01 ),
  INP0( P2_LEFT, 0x01, 0x02 ),
  INP0( P2_DOWN, 0x01, 0x04 ),
  INP0( P2_UP, 0x01, 0x08 ),
  INP0( P2_B1, 0x01, 0x10 ),
  INP0( P2_B2, 0x01, 0x20 ),

  INP0( TEST, 0x04, 0x02 ),
  INP0( SERVICE, 0x04, 0x04 ),
  INP0( P1_START, 0x05, 0x01 ),
  INP0( P2_START, 0x05, 0x02 ),
  INP0( COIN1, 0x05, 0x10 ),
  INP0( COIN2, 0x05, 0x20 ),
   END_INPUT
};

static struct INPUT_INFO input_p2b1[] =
{
  INP0( P1_RIGHT, 0x00, 0x01 ),
  INP0( P1_LEFT, 0x00, 0x02 ),
  INP0( P1_DOWN, 0x00, 0x04 ),
  INP0( P1_UP, 0x00, 0x08 ),
  INP0( P1_B1, 0x00, 0x10 ),
  INP0( P2_RIGHT, 0x01, 0x01 ),
  INP0( P2_LEFT, 0x01, 0x02 ),
  INP0( P2_DOWN, 0x01, 0x04 ),
  INP0( P2_UP, 0x01, 0x08 ),
  INP0( P2_B1, 0x01, 0x10 ),

  INP0( TEST, 0x04, 0x02 ),
  INP0( SERVICE, 0x04, 0x04 ),
  INP0( P1_START, 0x05, 0x01 ),
  INP0( P2_START, 0x05, 0x02 ),
  INP0( COIN1, 0x05, 0x10 ),
  INP0( COIN2, 0x05, 0x20 ),
   END_INPUT
};

static struct INPUT_INFO input_p1b3[] =
{
  INP0( P1_RIGHT, 0x00, 0x01 ),
  INP0( P1_LEFT, 0x00, 0x02 ),
  INP0( P1_DOWN, 0x00, 0x04 ),
  INP0( P1_UP, 0x00, 0x08 ),
  INP0( P1_B1, 0x00, 0x10 ),
  INP0( P1_B2, 0x00, 0x20 ),
  INP0( P1_B3, 0x00, 0x40 ),

  INP0( TEST, 0x04, 0x02 ),
  INP0( SERVICE, 0x04, 0x04 ),
  INP0( P1_START, 0x05, 0x01 ),
  INP0( COIN1, 0x05, 0x10 ),
   END_INPUT
};

static struct INPUT_INFO input_p1b2[] =
{
  INP0( P1_RIGHT, 0x00, 0x01 ),
  INP0( P1_LEFT, 0x00, 0x02 ),
  INP0( P1_DOWN, 0x00, 0x04 ),
  INP0( P1_UP, 0x00, 0x08 ),
  INP0( P1_B1, 0x00, 0x10 ),
  INP0( P1_B2, 0x00, 0x20 ),

  INP0( TEST, 0x04, 0x02 ),
  INP0( SERVICE, 0x04, 0x04 ),
  INP0( P1_START, 0x05, 0x01 ),
  INP0( COIN1, 0x05, 0x10 ),
   END_INPUT
};

static struct INPUT_INFO input_qndream[] =
// super bizarre inputs : 4 buttons instead of joystick !
{
  INP0( P1_B4, 0x00, 0x01 ),
  INP0( P1_B3, 0x00, 0x02 ),
  INP0( P1_B2, 0x00, 0x04 ),
  INP0( P1_B1, 0x00, 0x08 ),

  INP0( P2_B4, 0x01, 0x01 ),
  INP0( P2_B3, 0x01, 0x02 ),
  INP0( P2_B2, 0x01, 0x04 ),
  INP0( P2_B1, 0x01, 0x08 ),

  INP0( TEST, 0x04, 0x02 ),
  INP0( SERVICE, 0x04, 0x04 ),
  INP0( P1_START, 0x05, 0x01 ),
  INP0( P2_START, 0x05, 0x02 ),
  INP0( COIN1, 0x05, 0x10 ),
  INP0( COIN2, 0x05, 0x20 ),
   END_INPUT
};

static struct ROMSW_DATA romswd_msh[] =
{
  { "Japan", 0x0},
  { "USA",0x2},
  { "Hispanic", 0x4},
  { "Asia",0x6},
  { "Euro",0x8},
  { "Brazil",0xa},
  { NULL,                    0    },
};

static struct ROMSW_DATA romswd_1944[] =
{
  { "Japan", 0x0},
  { "USA",0x2},
  { "Euro", 0x4},
  { "Asia",0x6},
  { "Western",0x8},
  { "Brazil",0xa},
  { NULL,                    0    },
};

static struct ROMSW_DATA romswd_vhunt2[] =
{
  { "Japan", 0x0},
  { "USA",0x2},
  { "Hispanic", 0x4},
  { "Oceania",0x6},
  { "Asia",0x8},
  { "Euro",0xa},
  { "Brazil",0xc},
  { NULL,                    0    },
};

static struct ROMSW_DATA romswd_19xx[] =
{
  { "Japan", 0x0},
  { "USA", 1 },
  { "Asia",0x2},
  { "Hispanic", 3 },
  { "Euro", 0x4},
  { "Brazil", 5 },
  { NULL,                    0    },
};

static struct ROMSW_INFO romsw_msh[] =
{
   { 0xc3, 0x08, romswd_msh },
   { 0,        0,    NULL },
};

static struct ROMSW_INFO romsw_1944[] =
{
   { 0xc3, 0x02, romswd_1944 },
   { 0,        0,    NULL },
};

static struct ROMSW_INFO romsw_vhunt2[] =
{
   { 0x10ce, 0x00, romswd_vhunt2 },
   { 0,        0,    NULL },
};

static struct ROMSW_INFO romsw_19xx[] =
{
   { 0xc0, 0x00, romswd_19xx },
   { 0,        0,    NULL },
};

static struct ROMSW_INFO romsw_mpangj[] =
{
   { 0x7a0, 0x02, romswd_vhunt2 },
   { 0,        0,    NULL },
};

GMEI(ssf2,"Super Street Fighter II: The New Challengers (World 930911)",CAPCOM,1993,GAME_BEAT);
CLNEI(ssf2u,ssf2,"Super Street Fighter II: The New Challengers (USA 930911)",CAPCOM,1993,GAME_BEAT);
CLNEI(ssf2a,ssf2,"Super Street Fighter II: The New Challengers (Asia 931005)",CAPCOM,1993,GAME_BEAT);
CLNEI(ssf2ar1,ssf2,"Super Street Fighter II: The New Challengers (Asia 930914)",CAPCOM,1993,GAME_BEAT);
CLNEI(ssf2j,ssf2,"Super Street Fighter II: The New Challengers (Japan 931005)",CAPCOM,1993,GAME_BEAT);
CLNEI(ssf2jr1,ssf2,"Super Street Fighter II: The New Challengers (Japan 930911)",CAPCOM,1993,GAME_BEAT);
CLNEI(ssf2jr2,ssf2,"Super Street Fighter II: The New Challengers (Japan 930910)",CAPCOM,1993,GAME_BEAT);
CLNEI(ssf2tb,ssf2,"Super Street Fighter II: The Tournament Battle (World 931119)",CAPCOM,1993,GAME_BEAT);
CLNEI(ssf2tbr1,ssf2,"Super Street Fighter II: The Tournament Battle (World 930911)",CAPCOM,1993,GAME_BEAT);
CLNEI(ssf2tbj,ssf2,"Super Street Fighter II: The Tournament Battle (Japan 930911)",CAPCOM,1993,GAME_BEAT);
GMEI(ecofghtr,"Eco Fighters (World 931203)",CAPCOM,1993,GAME_SHOOT, .input = input_p2b3);
CLNEI(ecofghtu,ecofghtr,"Eco Fighters (USA 940215)",CAPCOM,1993,GAME_SHOOT, .input = input_p2b3);
CLNEI(ecofgtu1,ecofghtr,"Eco Fighters (USA 931203)",CAPCOM,1993,GAME_SHOOT, .input = input_p2b3);
CLNEI(uecology,ecofghtr,"Ultimate Ecology (Japan 931203)",CAPCOM,1993,GAME_SHOOT, .input = input_p2b3);
CLNEI(ecofghta,ecofghtr,"Eco Fighters (Asia 931203)",CAPCOM,1993,GAME_SHOOT, .input = input_p2b3);
GMEI(ddtod,"Dungeons & Dragons: Tower of Doom (Euro 940412)",CAPCOM,1993,GAME_BEAT, .input = input_p4b4);
CLNEI(ddtodr1,ddtod,"Dungeons & Dragons: Tower of Doom (Euro 940113)",CAPCOM,1993,GAME_BEAT, .input = input_p4b4);
CLNEI(ddtodu,ddtod,"Dungeons & Dragons: Tower of Doom (USA 940125)",CAPCOM,1993,GAME_BEAT, .input = input_p4b4);
CLNEI(ddtodur1,ddtod,"Dungeons & Dragons: Tower of Doom (USA 940113)",CAPCOM,1993,GAME_BEAT, .input = input_p4b4);
CLNEI(ddtodj,ddtod,"Dungeons & Dragons: Tower of Doom (Japan 940412)",CAPCOM,1993,GAME_BEAT, .input = input_p4b4);
CLNEI(ddtodjr1,ddtod,"Dungeons & Dragons: Tower of Doom (Japan 940125)",CAPCOM,1993,GAME_BEAT, .input = input_p4b4);
CLNEI(ddtodjr2,ddtod,"Dungeons & Dragons: Tower of Doom (Japan 940113)",CAPCOM,1993,GAME_BEAT, .input = input_p4b4);
CLNEI(ddtoda,ddtod,"Dungeons & Dragons: Tower of Doom (Asia 940113)",CAPCOM,1993,GAME_BEAT, .input = input_p4b4);
CLNEI(ddtodh,ddtod,"Dungeons & Dragons: Tower of Doom (Hispanic 940125)",CAPCOM,1993,GAME_BEAT, .input = input_p4b4);
CLNEI(ssf2t,ssf2,"Super Street Fighter II Turbo (World 940223)",CAPCOM,1994,GAME_BEAT);
CLNEI(ssf2ta,ssf2,"Super Street Fighter II Turbo (Asia 940223)",CAPCOM,1994,GAME_BEAT);
CLNEI(ssf2tu,ssf2,"Super Street Fighter II Turbo (USA 940323)",CAPCOM,1994,GAME_BEAT);
CLNEI(ssf2tur1,ssf2,"Super Street Fighter II Turbo (USA 940223)",CAPCOM,1994,GAME_BEAT);
CLNEI(ssf2xj,ssf2,"Super Street Fighter II X: Grand Master Challenge (Japan 940223)",CAPCOM,1994,GAME_BEAT);
GMEI(avsp,"Alien vs. Predator (Euro 940520)",CAPCOM,1994,GAME_BEAT, .input = input_p3b3);
CLNEI(avspu,avsp,"Alien vs. Predator (USA 940520)",CAPCOM,1994,GAME_BEAT, .input = input_p3b3);
CLNEI(avspj,avsp,"Alien vs. Predator (Japan 940520)",CAPCOM,1994,GAME_BEAT, .input = input_p3b3);
CLNEI(avspa,avsp,"Alien vs. Predator (Asia 940520)",CAPCOM,1994,GAME_BEAT, .input = input_p3b3);
CLNEI(avsph,avsp,"Alien vs. Predator (Hispanic 940520)",CAPCOM,1994,GAME_BEAT, .input = input_p3b3);
GMEI(dstlk,"Darkstalkers: The Night Warriors (Euro 940705)",CAPCOM,1994,GAME_BEAT);
CLNEI(dstlku,dstlk,"Darkstalkers: The Night Warriors (USA 940818)",CAPCOM,1994,GAME_BEAT);
CLNEI(dstlkur1,dstlk,"Darkstalkers: The Night Warriors (USA 940705)",CAPCOM,1994,GAME_BEAT);
CLNEI(dstlka,dstlk,"Darkstalkers: The Night Warriors (Asia 940705)",CAPCOM,1994,GAME_BEAT);
CLNEI(vampj,dstlk,"Vampire: The Night Warriors (Japan 940705)",CAPCOM,1994,GAME_BEAT);
CLNEI(vampja,dstlk,"Vampire: The Night Warriors (Japan 940705 alt)",CAPCOM,1994,GAME_BEAT);
CLNEI(vampjr1,dstlk,"Vampire: The Night Warriors (Japan 940630)",CAPCOM,1994,GAME_BEAT);
GMEI(ringdest,"Ring of Destruction: Slammasters II (Euro 940902)",CAPCOM,1994,GAME_SPORTS);
CLNEI(smbomb,ringdest,"Super Muscle Bomber: The International Blowout (Japan 940831)",CAPCOM,1994,GAME_SPORTS);
CLNEI(smbombr1,ringdest,"Super Muscle Bomber: The International Blowout (Japan 940808)",CAPCOM,1994,GAME_SPORTS);
GMEI(armwar,"Armored Warriors (Euro 941024)",CAPCOM,1994,GAME_PLATFORM, .input = input_p3b3);
CLNEI(armwarr1,armwar,"Armored Warriors (Euro 941011)",CAPCOM,1994,GAME_PLATFORM, .input = input_p3b3);
CLNEI(armwaru,armwar,"Armored Warriors (USA 941024)",CAPCOM,1994,GAME_PLATFORM, .input = input_p3b3);
CLNEI(armwaru1,armwar,"Armored Warriors (USA 940920)",CAPCOM,1994,GAME_PLATFORM, .input = input_p3b3);
CLNEI(pgear,armwar,"Powered Gear: Strategic Variant Armor Equipment (Japan 941024)",CAPCOM,1994,GAME_PLATFORM, .input = input_p3b3);
CLNEI(pgearr1,armwar,"Powered Gear: Strategic Variant Armor Equipment (Japan 940916)",CAPCOM,1994,GAME_PLATFORM, .input = input_p3b3);
CLNEI(armwara,armwar,"Armored Warriors (Asia 940920)",CAPCOM,1994,GAME_PLATFORM, .input = input_p3b3);
GMEI(xmcota,"X-Men: Children of the Atom (Euro 950105)",CAPCOM,1994,GAME_BEAT);
CLNEI(xmcotau,xmcota,"X-Men: Children of the Atom (USA 950105)",CAPCOM,1994,GAME_BEAT);
CLNEI(xmcotah,xmcota,"X-Men: Children of the Atom (Hispanic 950331)",CAPCOM,1994,GAME_BEAT);
CLNEI(xmcotaj,xmcota,"X-Men: Children of the Atom (Japan 941222)",CAPCOM,1994,GAME_BEAT);
CLNEI(xmcotaj1,xmcota,"X-Men: Children of the Atom (Japan 941219)",CAPCOM,1994,GAME_BEAT);
CLNEI(xmcotaj2,xmcota,"X-Men: Children of the Atom (Japan 941217)",CAPCOM,1994,GAME_BEAT);
CLNEI(xmcotajr,xmcota,"X-Men: Children of the Atom (Japan 941208 rent version)",CAPCOM,1994,GAME_BEAT);
CLNEI(xmcotaa,xmcota,"X-Men: Children of the Atom (Asia 941217)",CAPCOM,1994,GAME_BEAT);
GMEI(nwarr,"Night Warriors: Darkstalkers' Revenge (Euro 950316)",CAPCOM,1995,GAME_BEAT);
CLNEI(nwarru,nwarr,"Night Warriors: Darkstalkers' Revenge (USA 950406)",CAPCOM,1995,GAME_BEAT);
CLNEI(nwarrh,nwarr,"Night Warriors: Darkstalkers' Revenge (Hispanic 950403)",CAPCOM,1995,GAME_BEAT);
CLNEI(nwarrb,nwarr,"Night Warriors: Darkstalkers' Revenge (Brazil 950403)",CAPCOM,1995,GAME_BEAT);
CLNEI(vhuntj,nwarr,"Vampire Hunter: Darkstalkers' Revenge (Japan 950316)",CAPCOM,1995,GAME_BEAT);
CLNEI(vhuntjr1,nwarr,"Vampire Hunter: Darkstalkers' Revenge (Japan 950307)",CAPCOM,1995,GAME_BEAT);
CLNEI(vhuntjr2,nwarr,"Vampire Hunter: Darkstalkers' Revenge (Japan 950302)",CAPCOM,1995,GAME_BEAT);
GMEI(cybots,"Cyberbots: Fullmetal Madness (Euro 950424)",CAPCOM,1995,GAME_BEAT, .input = input_p2b4);
CLNEI(cybotsu,cybots,"Cyberbots: Fullmetal Madness (USA 950424)",CAPCOM,1995,GAME_BEAT, .input = input_p2b4);
CLNEI(cybotsj,cybots,"Cyberbots: Fullmetal Madness (Japan 950420)",CAPCOM,1995,GAME_BEAT, .input = input_p2b4);
GMEI(sfa,"Street Fighter Alpha: Warriors' Dreams (Euro 950727)",CAPCOM,1995,GAME_BEAT);
CLNEI(sfar1,sfa,"Street Fighter Alpha: Warriors' Dreams (Euro 950718)",CAPCOM,1995,GAME_BEAT);
CLNEI(sfar2,sfa,"Street Fighter Alpha: Warriors' Dreams (Euro 950627)",CAPCOM,1995,GAME_BEAT);
CLNEI(sfar3,sfa,"Street Fighter Alpha: Warriors' Dreams (Euro 950605)",CAPCOM,1995,GAME_BEAT);
CLNEI(sfau,sfa,"Street Fighter Alpha: Warriors' Dreams (USA 950627)",CAPCOM,1995,GAME_BEAT);
CLNEI(sfza,sfa,"Street Fighter Zero (Asia 950627)",CAPCOM,1995,GAME_BEAT);
CLNEI(sfzj,sfa,"Street Fighter Zero (Japan 950727)",CAPCOM,1995,GAME_BEAT);
CLNEI(sfzjr1,sfa,"Street Fighter Zero (Japan 950627)",CAPCOM,1995,GAME_BEAT);
CLNEI(sfzjr2,sfa,"Street Fighter Zero (Japan 950605)",CAPCOM,1995,GAME_BEAT);
CLNEI(sfzh,sfa,"Street Fighter Zero (Hispanic 950627)",CAPCOM,1995,GAME_BEAT);
CLNEI(sfzb,sfa,"Street Fighter Zero (Brazil 951109)",CAPCOM,1995,GAME_BEAT);
CLNEI(sfzbr1,sfa,"Street Fighter Zero (Brazil 950727)",CAPCOM,1995,GAME_BEAT);
CLNEI(mmancp2u,megaman,"Mega Man - The Power Battle (CPS2,USA 951006,SAMPLE Version)",CAPCOM,1995,GAME_BEAT, .input = input_p2b3);
CLNEI(rmancp2j,megaman,"Rockman: The Power Battle (CPS2,Japan 950922)",CAPCOM,1995,GAME_BEAT, .input = input_p2b3);
GMEI(msh,"Marvel Super Heroes (Euro 951024)",CAPCOM,1995,GAME_BEAT, .romsw = romsw_msh);
CLNEI(mshu,msh,"Marvel Super Heroes (USA 951024)",CAPCOM,1995,GAME_BEAT);
CLNEI(mshj,msh,"Marvel Super Heroes (Japan 951117)",CAPCOM,1995,GAME_BEAT);
CLNEI(mshjr1,msh,"Marvel Super Heroes (Japan 951024)",CAPCOM,1995,GAME_BEAT);
CLNEI(msha,msh,"Marvel Super Heroes (Asia 951024)",CAPCOM,1995,GAME_BEAT);
CLNEI(mshh,msh,"Marvel Super Heroes (Hispanic 951117)",CAPCOM,1995,GAME_BEAT);
CLNEI(mshb,msh,"Marvel Super Heroes (Brazil 951117)",CAPCOM,1995,GAME_BEAT);
GMEI(19xx,"19XX: The War Against Destiny (USA 951207)",CAPCOM,1996,GAME_SHOOT, .video = &video_cps1_270, .input = input_p2b2, .romsw = romsw_19xx);
CLNEI(19xxj,19xx,"19XX: The War Against Destiny (Japan 951225)",CAPCOM,1996,GAME_SHOOT, .video = &video_cps1_270, .input = input_p2b2, .romsw = romsw_19xx);
CLNEI(19xxjr1,19xx,"19XX: The War Against Destiny (Japan 951207)",CAPCOM,1996,GAME_SHOOT, .video = &video_cps1_270, .input = input_p2b2, .romsw = romsw_19xx);
CLNEI(19xxh,19xx,"19XX: The War Against Destiny (Hispanic 951218)",CAPCOM,1996,GAME_SHOOT, .video = &video_cps1_270, .input = input_p2b2, .romsw = romsw_19xx);
CLNEI(19xxb,19xx,"19XX: The War Against Destiny (Brazil 951218)",CAPCOM,1996,GAME_SHOOT, .video = &video_cps1_270, .input = input_p2b2);
GMEI(ddsom,"Dungeons & Dragons: Shadow over Mystara (Euro 960619)",CAPCOM,1996,GAME_BEAT, .input = input_p4b4);
CLNEI(ddsomr1,ddsom,"Dungeons & Dragons: Shadow over Mystara (Euro 960223)",CAPCOM,1996,GAME_BEAT, .input = input_p4b4);
CLNEI(ddsomr2,ddsom,"Dungeons & Dragons: Shadow over Mystara (Euro 960209)",CAPCOM,1996,GAME_BEAT, .input = input_p4b4);
CLNEI(ddsomr3,ddsom,"Dungeons & Dragons: Shadow over Mystara (Euro 960208)",CAPCOM,1996,GAME_BEAT, .input = input_p4b4);
CLNEI(ddsomu,ddsom,"Dungeons & Dragons: Shadow over Mystara (USA 960619)",CAPCOM,1996,GAME_BEAT, .input = input_p4b4);
CLNEI(ddsomur1,ddsom,"Dungeons & Dragons: Shadow over Mystara (USA 960209)",CAPCOM,1996,GAME_BEAT, .input = input_p4b4);
CLNEI(ddsomj,ddsom,"Dungeons & Dragons: Shadow over Mystara (Japan 960619)",CAPCOM,1996,GAME_BEAT, .input = input_p4b4);
CLNEI(ddsomjr1,ddsom,"Dungeons & Dragons: Shadow over Mystara (Japan 960206)",CAPCOM,1996,GAME_BEAT, .input = input_p4b4);
CLNEI(ddsoma,ddsom,"Dungeons & Dragons: Shadow over Mystara (Asia 960619)",CAPCOM,1996,GAME_BEAT, .input = input_p4b4);
CLNEI(ddsomb,ddsom,"Dungeons & Dragons: Shadow over Mystara (Brazil 960223)",CAPCOM,1996,GAME_BEAT, .input = input_p4b4);
GMEI(sfa2,"Street Fighter Alpha 2 (Euro 960229)",CAPCOM,1996,GAME_BEAT);
CLNEI(sfa2u,sfa2,"Street Fighter Alpha 2 (USA 960306)",CAPCOM,1996,GAME_BEAT);
CLNEI(sfz2j,sfa2,"Street Fighter Zero 2 (Japan 960227)",CAPCOM,1996,GAME_BEAT);
CLNEI(sfz2a,sfa2,"Street Fighter Zero 2 (Asia 960227)",CAPCOM,1996,GAME_BEAT);
CLNEI(sfz2b,sfa2,"Street Fighter Zero 2 (Brazil 960531)",CAPCOM,1996,GAME_BEAT);
CLNEI(sfz2br1,sfa2,"Street Fighter Zero 2 (Brazil 960304)",CAPCOM,1996,GAME_BEAT);
CLNEI(sfz2h,sfa2,"Street Fighter Zero 2 (Hispanic 960304)",CAPCOM,1996,GAME_BEAT);
CLNEI(sfz2n,sfa2,"Street Fighter Zero 2 (Oceania 960229)",CAPCOM,1996,GAME_BEAT);
GMEI(sfz2al,"Street Fighter Zero 2 Alpha (Asia 960826)",CAPCOM,1996,GAME_BEAT);
CLNEI(sfz2alj,sfz2al,"Street Fighter Zero 2 Alpha (Japan 960805)",CAPCOM,1996,GAME_BEAT);
CLNEI(sfz2alh,sfz2al,"Street Fighter Zero 2 Alpha (Hispanic 960813)",CAPCOM,1996,GAME_BEAT);
CLNEI(sfz2alb,sfz2al,"Street Fighter Zero 2 Alpha (Brazil 960813)",CAPCOM,1996,GAME_BEAT);
GMEI(spf2t,"Super Puzzle Fighter II Turbo (USA 960620)",CAPCOM,1996,GAME_PUZZLE, .input = input_p2b2);
CLNEI(spf2xj,spf2t,"Super Puzzle Fighter II X (Japan 960531)",CAPCOM,1996,GAME_PUZZLE, .input = input_p2b2);
CLNEI(spf2ta,spf2t,"Super Puzzle Fighter II Turbo (Asia 960529)",CAPCOM,1996,GAME_PUZZLE, .input = input_p2b2);
GMEI(megaman2,"Mega Man 2: The Power Fighters (USA 960708)",CAPCOM,1996,GAME_BEAT, .input = input_p2b3);
CLNEI(megamn2a,megaman2,"Mega Man 2: The Power Fighters (Asia 960708)",CAPCOM,1996,GAME_BEAT, .input = input_p2b3);
CLNEI(rckman2j,megaman2,"Rockman 2: The Power Fighters (Japan 960708)",CAPCOM,1996,GAME_BEAT, .input = input_p2b3);
GMEI(qndream,"Quiz Nanairo Dreams: Nijiirochou no Kiseki (Japan 960826)",CAPCOM,1996,GAME_QUIZZ, .input = input_qndream);
GMEI(xmvsf,"X-Men Vs. Street Fighter (Euro 961004)",CAPCOM,1996,GAME_BEAT);
CLNEI(xmvsfr1,xmvsf,"X-Men Vs. Street Fighter (Euro 960910)",CAPCOM,1996,GAME_BEAT);
CLNEI(xmvsfu,xmvsf,"X-Men Vs. Street Fighter (USA 961023)",CAPCOM,1996,GAME_BEAT);
CLNEI(xmvsfur1,xmvsf,"X-Men Vs. Street Fighter (USA 961004)",CAPCOM,1996,GAME_BEAT);
CLNEI(xmvsfj,xmvsf,"X-Men Vs. Street Fighter (Japan 961004)",CAPCOM,1996,GAME_BEAT);
CLNEI(xmvsfjr1,xmvsf,"X-Men Vs. Street Fighter (Japan 960910)",CAPCOM,1996,GAME_BEAT);
CLNEI(xmvsfjr2,xmvsf,"X-Men Vs. Street Fighter (Japan 960909)",CAPCOM,1996,GAME_BEAT);
CLNEI(xmvsfa,xmvsf,"X-Men Vs. Street Fighter (Asia 961023)",CAPCOM,1996,GAME_BEAT);
CLNEI(xmvsfar1,xmvsf,"X-Men Vs. Street Fighter (Asia 960919)",CAPCOM,1996,GAME_BEAT);
CLNEI(xmvsfh,xmvsf,"X-Men Vs. Street Fighter (Hispanic 961004)",CAPCOM,1996,GAME_BEAT);
CLNEI(xmvsfb,xmvsf,"X-Men Vs. Street Fighter (Brazil 961023)",CAPCOM,1996,GAME_BEAT);
GMEI(batcir,"Battle Circuit (Euro 970319)",CAPCOM,1997,GAME_BEAT, .input = input_p4b2);
CLNEI(batcira,batcir,"Battle Circuit (Asia 970319)",CAPCOM,1997,GAME_BEAT, .input = input_p4b2);
CLNEI(batcirj,batcir,"Battle Circuit (Japan 970319)",CAPCOM,1997,GAME_BEAT, .input = input_p4b2);
GMEI(vsav,"Vampire Savior: The Lord of Vampire (Euro 970519)",CAPCOM,1997,GAME_BEAT);
CLNEI(vsavu,vsav,"Vampire Savior: The Lord of Vampire (USA 970519)",CAPCOM,1997,GAME_BEAT);
CLNEI(vsavj,vsav,"Vampire Savior: The Lord of Vampire (Japan 970519)",CAPCOM,1997,GAME_BEAT);
CLNEI(vsava,vsav,"Vampire Savior: The Lord of Vampire (Asia 970519)",CAPCOM,1997,GAME_BEAT);
CLNEI(vsavh,vsav,"Vampire Savior: The Lord of Vampire (Hispanic 970519)",CAPCOM,1997,GAME_BEAT);
GMEI(mshvsf,"Marvel Super Heroes Vs. Street Fighter (Euro 970625)",CAPCOM,1997,GAME_BEAT);
CLNEI(mshvsfu,mshvsf,"Marvel Super Heroes Vs. Street Fighter (USA 970827)",CAPCOM,1997,GAME_BEAT);
CLNEI(mshvsfu1,mshvsf,"Marvel Super Heroes Vs. Street Fighter (USA 970625)",CAPCOM,1997,GAME_BEAT);
CLNEI(mshvsfj,mshvsf,"Marvel Super Heroes Vs. Street Fighter (Japan 970707)",CAPCOM,1997,GAME_BEAT);
CLNEI(mshvsfj1,mshvsf,"Marvel Super Heroes Vs. Street Fighter (Japan 970702)",CAPCOM,1997,GAME_BEAT);
CLNEI(mshvsfj2,mshvsf,"Marvel Super Heroes Vs. Street Fighter (Japan 970625)",CAPCOM,1997,GAME_BEAT);
CLNEI(mshvsfh,mshvsf,"Marvel Super Heroes Vs. Street Fighter (Hispanic 970625)",CAPCOM,1997,GAME_BEAT);
CLNEI(mshvsfa,mshvsf,"Marvel Super Heroes Vs. Street Fighter (Asia 970625)",CAPCOM,1997,GAME_BEAT);
CLNEI(mshvsfa1,mshvsf,"Marvel Super Heroes Vs. Street Fighter (Asia 970620)",CAPCOM,1997,GAME_BEAT);
CLNEI(mshvsfb,mshvsf,"Marvel Super Heroes Vs. Street Fighter (Brazil 970827)",CAPCOM,1997,GAME_BEAT);
CLNEI(mshvsfb1,mshvsf,"Marvel Super Heroes Vs. Street Fighter (Brazil 970625)",CAPCOM,1997,GAME_BEAT);
GMEI(csclub,"Capcom Sports Club (Euro 971017)",CAPCOM,1997,GAME_SPORTS, .input = input_p2b3);
CLNEI(csclub1,csclub,"Capcom Sports Club (Euro 970722)",CAPCOM,1997,GAME_SPORTS, .input = input_p2b3);
CLNEI(cscluba,csclub,"Capcom Sports Club (Asia 970722)",CAPCOM,1997,GAME_SPORTS, .input = input_p2b3);
CLNEI(csclubj,csclub,"Capcom Sports Club (Japan 970722)",CAPCOM,1997,GAME_SPORTS, .input = input_p2b3);
CLNEI(csclubh,csclub,"Capcom Sports Club (Hispanic 970722)",CAPCOM,1997,GAME_SPORTS, .input = input_p2b3);
GMEI(sgemf,"Super Gem Fighter Mini Mix (USA 970904)",CAPCOM,1997,GAME_BEAT, .input = input_p2b3);
CLNEI(pfghtj,sgemf,"Pocket Fighter (Japan 970904)",CAPCOM,1997,GAME_BEAT, .input = input_p2b3);
CLNEI(sgemfa,sgemf,"Super Gem Fighter: Mini Mix (Asia 970904)",CAPCOM,1997,GAME_BEAT, .input = input_p2b3);
CLNEI(sgemfh,sgemf,"Super Gem Fighter: Mini Mix (Hispanic 970904)",CAPCOM,1997,GAME_BEAT, .input = input_p2b3);
GMEI(vhunt2,"Vampire Hunter 2: Darkstalkers Revenge (Japan 970929)",CAPCOM,1997,GAME_BEAT, .romsw = romsw_vhunt2);
CLNEI(vhunt2r1,vhunt2,"Vampire Hunter 2: Darkstalkers Revenge (Japan 970913)",CAPCOM,1997,GAME_BEAT);
GMEI(vsav2,"Vampire Savior 2: The Lord of Vampire (Japan 970913)",CAPCOM,1997,GAME_BEAT);
GMEI(mvsc,"Marvel Vs. Capcom: Clash of Super Heroes (Euro 980123)",CAPCOM,1998,GAME_BEAT);
CLNEI(mvscr1,mvsc,"Marvel Vs. Capcom: Clash of Super Heroes (Euro 980112)",CAPCOM,1998,GAME_BEAT);
CLNEI(mvscu,mvsc,"Marvel Vs. Capcom: Clash of Super Heroes (USA 980123)",CAPCOM,1998,GAME_BEAT);
CLNEI(mvscj,mvsc,"Marvel Vs. Capcom: Clash of Super Heroes (Japan 980123)",CAPCOM,1998,GAME_BEAT);
CLNEI(mvscjr1,mvsc,"Marvel Vs. Capcom: Clash of Super Heroes (Japan 980112)",CAPCOM,1998,GAME_BEAT);
CLNEI(mvsca,mvsc,"Marvel Vs. Capcom: Clash of Super Heroes (Asia 980123)",CAPCOM,1998,GAME_BEAT);
CLNEI(mvscar1,mvsc,"Marvel Vs. Capcom: Clash of Super Heroes (Asia 980112)",CAPCOM,1998,GAME_BEAT);
CLNEI(mvsch,mvsc,"Marvel Vs. Capcom: Clash of Super Heroes (Hispanic 980123)",CAPCOM,1998,GAME_BEAT);
CLNEI(mvscb,mvsc,"Marvel Vs. Capcom: Clash of Super Heroes (Brazil 980123)",CAPCOM,1998,GAME_BEAT);
GMEI(sfa3,"Street Fighter Alpha 3 (Euro 980904)",CAPCOM,1998,GAME_BEAT);
CLNEI(sfa3u,sfa3,"Street Fighter Alpha 3 (USA 980904)",CAPCOM,1998,GAME_BEAT);
CLNEI(sfa3ur1,sfa3,"Street Fighter Alpha 3 (USA 980629)",CAPCOM,1998,GAME_BEAT);
CLNEI(sfa3b,sfa3,"Street Fighter Alpha 3 (Brazil 980629)",CAPCOM,1998,GAME_BEAT);
CLNEI(sfz3j,sfa3,"Street Fighter Zero 3 (Japan 980904)",CAPCOM,1998,GAME_BEAT);
CLNEI(sfz3jr1,sfa3,"Street Fighter Zero 3 (Japan 980727)",CAPCOM,1998,GAME_BEAT);
CLNEI(sfz3jr2,sfa3,"Street Fighter Zero 3 (Japan 980629)",CAPCOM,1998,GAME_BEAT);
CLNEI(sfz3a,sfa3,"Street Fighter Zero 3 (Asia 980904)",CAPCOM,1998,GAME_BEAT);
CLNEI(sfz3ar1,sfa3,"Street Fighter Zero 3 (Asia 980701)",CAPCOM,1998,GAME_BEAT);
GMEI(jyangoku,"Jyangokushi: Haoh no Saihai (Japan 990527)",CAPCOM,1999,GAME_PUZZLE, .input = input_p1b2);
GMEI(hsf2,"Hyper Street Fighter 2: The Anniversary Edition (Asia 040202)",CAPCOM,2004,GAME_BEAT);
CLNEI(hsf2j,hsf2,"Hyper Street Fighter 2: The Anniversary Edition (Japan 031222)",CAPCOM,2004,GAME_BEAT);
GMEI(gigawing,"Giga Wing (USA 990222)",CAPCOM,1999,GAME_SHOOT, .input = input_p2b2);
CLNEI(gwingj,gigawing,"Giga Wing (Japan 990223)",CAPCOM,1999,GAME_SHOOT, .input = input_p2b2);
CLNEI(gwinga,gigawing,"Giga Wing (Asia 990222)",CAPCOM,1999,GAME_SHOOT, .input = input_p2b2);
CLNEI(gwingb,gigawing,"Giga Wing (Brazil 990222)",CAPCOM,1999,GAME_SHOOT, .input = input_p2b2);
GMEI(mmatrix,"Mars Matrix: Hyper Solid Shooting (USA 000412)",CAPCOM,2000,GAME_SHOOT, .input = input_p2b1);
CLNEI(mmatrixj,mmatrix,"Mars Matrix: Hyper Solid Shooting (Japan 000412)",CAPCOM,2000,GAME_SHOOT, .input = input_p2b1);
GMEI(mpang,"Mighty! Pang (Euro 001010)",MITCHELL,2000,GAME_SHOOT, .input = input_p2b1, .romsw = romsw_mpangj);
CLNEI(mpangu,mpang,"Mighty! Pang (USA 001010)",MITCHELL,2000,GAME_SHOOT, .input = input_p2b1);
CLNEI(mpangj,mpang,"Mighty! Pang (Japan 001011)",MITCHELL,2000,GAME_SHOOT, .input = input_p2b1, .romsw = romsw_mpangj);
GMEI(pzloop2,"Puzz Loop 2 (Euro 010302)",MITCHELL,2001,GAME_PUZZLE, .input = input_p2b1);
CLNEI(pzloop2j,pzloop2,"Puzz Loop 2 (Japan 010205)",MITCHELL,2001,GAME_PUZZLE, .input = input_p2b1);
GMEI(choko,"Janpai Puzzle Choukou (Japan 010820)",MITCHELL,2001,GAME_PUZZLE, .input = input_p1b3);
GMEI(dimahoo,"Dimahoo (Euro 000121)",RAIZING,2000,GAME_SHOOT, .video = &video_cps1_270, .input = input_p2b3);
CLNEI(dimahoou,dimahoo,"Dimahoo (USA 000121)",RAIZING,2000,GAME_SHOOT, .video = &video_cps1_270, .input = input_p2b3);
CLNEI(gmahou,dimahoo,"Great Mahou Daisakusen (Japan 000121)",RAIZING,2000,GAME_SHOOT, .video = &video_cps1_270, .input = input_p2b3);
GMEI(1944,"1944: The Loop Master (USA 000620)",RAIZING,2000,GAME_SHOOT, .input = input_p2b2, .romsw = romsw_1944);
CLNEI(1944j,1944,"1944: The Loop Master (Japan 000620)",RAIZING,2000,GAME_SHOOT, .input = input_p2b2);
GMEI(progear,"Progear (USA 010117)",CAVE,2001,GAME_SHOOT, .input = input_p2b3);
CLNEI(progearj,progear,"Progear no Arashi (Japan 010117)",CAVE,2001,GAME_SHOOT, .input = input_p2b3);
CLNEI(progeara,progear,"Progear (Asia 010117)",CAVE,2001,GAME_SHOOT, .input = input_p2b3);
CLNEI(ssf2ud,ssf2,"Super Street Fighter II: The New Challengers (USA 930911 Phoenix Edition) (bootleg)",BOOTLEG,1993,GAME_BEAT);
CLNEI(ddtodd,ddtod,"Dungeons & Dragons: Tower of Doom (Euro 940412 Phoenix Edition) (bootleg)",BOOTLEG,1993,GAME_BEAT, .input = input_p4b4);
CLNEI(avspd,avsp,"Alien vs. Predator (Euro 940520 Phoenix Edition) (bootleg)",BOOTLEG,1994,GAME_BEAT, .input = input_p3b3);
CLNEI(dstlku1d,dstlk,"Darkstalkers: The Night Warriors (USA 940705 Phoenix Edition) (bootleg)",BOOTLEG,1994,GAME_BEAT);
CLNEI(ringdstd,ringdest,"Ring of Destruction: Slammasters II (Euro 940902 Phoenix Edition) (bootleg)",BOOTLEG,1994,GAME_SPORTS);
CLNEI(xmcotad,xmcota,"X-Men: Children of the Atom (Euro 950105 Phoenix Edition) (bootleg)",BOOTLEG,1994,GAME_BEAT);
CLNEI(nwarrud,nwarr,"Night Warriors: Darkstalkers' Revenge (USA 950406 Phoenix Edition) (bootleg)",BOOTLEG,1995,GAME_BEAT);
CLNEI(sfad,sfa,"Street Fighter Alpha: Warriors' Dreams (Euro 950727 Phoenix Edition) (bootleg)",BOOTLEG,1995,GAME_BEAT);
CLNEI(mshud,msh,"Marvel Super Heroes (US 951024 Phoenix Edition) (bootleg)",BOOTLEG,1995,GAME_BEAT);
CLNEI(19xxd,19xx,"19XX: The War Against Destiny (USA 951207 Phoenix Edition) (bootleg)",BOOTLEG,1996,GAME_SHOOT, .video = &video_cps1_270, .input = input_p2b2);
CLNEI(sfz2ad,sfa2,"Street Fighter Zero 2 (Asia 960227 Phoenix Edition) (bootleg)",BOOTLEG,1996,GAME_BEAT);
CLNEI(spf2xjd,spf2t,"Super Puzzle Fighter II X (Japan 960531 Phoenix Edition) (bootleg)",BOOTLEG,1996,GAME_PUZZLE, .input = input_p2b2);
CLNEI(ddsomud,ddsom,"Dungeons & Dragons: Shadow over Mystara (USA 960619 Phoenix Edition) (bootleg)",BOOTLEG,1996,GAME_BEAT, .input = input_p4b4);
CLNEI(megamn2d,megaman2,"Mega Man 2: The Power Fighters (USA 960708 Phoenix Edition) (bootleg)",BOOTLEG,1996,GAME_BEAT, .input = input_p2b3);
CLNEI(sfz2ald,sfz2al,"Street Fighter Zero 2 Alpha (Asia 960826 Phoenix Edition) (bootleg)",BOOTLEG,1996,GAME_BEAT);
CLNEI(xmvsfu1d,xmvsf,"X-Men Vs. Street Fighter (USA 961004 Phoenix Edition) (bootleg)",BOOTLEG,1996,GAME_BEAT);
CLNEI(batcird,batcir,"Battle Circuit (Euro 970319 Phoenix Edition) (bootleg)",BOOTLEG,1997,GAME_BEAT, .input = input_p4b2);
CLNEI(vsavd,vsav,"Vampire Savior: The Lord of Vampire (Euro 970519 Phoenix Edition) (bootleg)",BOOTLEG,1997,GAME_BEAT);
CLNEI(mvscud,mvsc,"Marvel Vs. Capcom: Clash of Super Heroes (USA 980123 Phoenix Edition) (bootleg)",BOOTLEG,1998,GAME_BEAT);
CLNEI(sfa3ud,sfa3,"Street Fighter Alpha 3 (USA 980904 Phoenix Edition) (bootleg)",BOOTLEG,1998,GAME_BEAT);
CLNEI(gwingjd,gigawing,"Giga Wing (Japan 990223 Phoenix Edition) (bootleg)",BOOTLEG,1999,GAME_SHOOT, .input = input_p2b2);
CLNEI(1944d,1944,"1944: The Loop Master (USA 000620 Phoenix Edition) (bootleg)",BOOTLEG,2000,GAME_SHOOT, .input = input_p2b2);
CLNEI(progerjd,progear,"Progear no Arashi (Japan 010117 Phoenix Edition) (bootleg)",BOOTLEG,2001,GAME_SHOOT, .input = input_p2b3);
CLNEI(hsf2d,hsf2,"Hyper Street Fighter II: The Anniversary Edition (Asia 040202 Phoenix Edition) (bootleg)",BOOTLEG,2004,GAME_BEAT);
CLNEI(ssf2tbd,ssf2,"Super Street Fighter II: The Tournament Battle (World 931119 Phoenix Edition) (bootleg)",BOOTLEG,1993,GAME_BEAT);
CLNEI(ssf2xjd,ssf2,"Super Street Fighter II X: Grand Master Challenge (Japan 940223 Phoenix Edition) (bootleg)",BOOTLEG,1994,GAME_BEAT);
CLNEI(sgemfd,sgemf,"Super Gem Fighter Mini Mix (USA 970904 Phoenix Edition) (bootleg)",BOOTLEG,1997,GAME_BEAT, .input = input_p2b3);
