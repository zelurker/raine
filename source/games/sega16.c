/*
 * sega16.c - Sega System 16B driver skeleton for Raine (from Claude)
 *
 * Target hardware: Sega System 16B
 * Representative games: Golden Axe, Altered Beast, Shinobi,
 *                       Tetris (Sega), Bullet (Sega)
 *
 * Main CPU  : Motorola 68000 @ 10 MHz
 * Sound CPU : Zilog Z80 @ 4 MHz
 * Sound     : YM2151 + UPD7759 (ADPCM samples)
 * Video     : Custom Sega IC (315-5195 memory mapper + 315-5197 scroll)
 *             - Two scrolling tilemap layers (FG/BG), 64x32 tiles each
 *             - One sprite layer, up to 128 sprites
 *             - Palette: 2048 colors (12-bit RGB)
 * I/O       : 8255 PPI for player inputs + coin inputs
 *
 * Known custom ICs:
 *   315-5195  : Memory mapper / bank switcher
 *   315-5197  : Tilemap scroll controller
 *   315-5248  : Sprite generator
 *
 * References:
 *   - MAME source: src/mame/sega/segas16b.cpp
 *   - Charles MacDonald's System 16 hardware notes
 *   - Raine source conventions (see cps1.c or neogeo.c for style)
 *
 * Status: SKELETON - does not run games yet.
 *         Memory map, video, and sound stubs are provided
 *         as a starting point for a full implementation.
 *
 * Author: only Claude for now (the AI)
 * Date  : 2026-03-19
 */

#include "gameinc.h"       /* Raine core game includes */
#include "sasound.h"       /* Raine sound system       */
#include "savegame.h"      /* Save-state support       */

/* =========================================================
 *  CONSTANTS
 * ========================================================= */

#define SYS16_MAIN_CPU_CLOCK   10000000   /* 68000 @ 10 MHz          */
#define SYS16_SOUND_CPU_CLOCK   4000000   /* Z80  @  4 MHz           */
#define SYS16_YM2151_CLOCK      4000000   /* YM2151 @ 4 MHz          */

#define SYS16_SCREEN_WIDTH      320
#define SYS16_SCREEN_HEIGHT     224
#define SYS16_SCREEN_REFRESH     60       /* ~60 Hz NTSC             */

/* Tilemap dimensions (in tiles) */
#define SYS16_MAP_COLS           64
#define SYS16_MAP_ROWS           32
#define SYS16_TILE_W              8
#define SYS16_TILE_H              8

/* Sprite limits */
#define SYS16_MAX_SPRITES       128

/* =========================================================
 *  MEMORY REGIONS
 *  These are filled by load_rom() at startup.
 * ========================================================= */

static Uint8 *sys16_rom       = NULL;  /* Main 68000 program ROMs (up to 512KB)  */
static Uint8 *sys16_soundrom  = NULL;  /* Z80 sound ROMs                         */
static Uint8 *sys16_gfxrom    = NULL;  /* Tile + sprite GFX ROMs                 */
static Uint8 *sys16_adpcmrom  = NULL;  /* UPD7759 ADPCM sample ROMs              */

/* =========================================================
 *  WORK RAM
 * ========================================================= */

static Uint8 sys16_workram[0x10000];   /* 64KB 68000 work RAM   */
static Uint8 sys16_soundram[0x0800];   /* 2KB  Z80  work RAM    */

/* =========================================================
 *  VIDEO RAM
 * ========================================================= */

/*
 * The System 16B video hardware has:
 *   - Tilemap RAM : two layers (FG and BG), each 0x1000 bytes
 *   - Sprite RAM  : 0x800 bytes (128 sprites x 8 bytes each)
 *   - Palette RAM : 0x1000 bytes (2048 entries x 2 bytes, RGB444)
 */
static Uint8 sys16_tileram[0x2000];    /* FG at [0x0000], BG at [0x1000] */
static Uint8 sys16_spriteram[0x0800];
static Uint8 sys16_paletteram[0x1000];

/* Scroll registers (set by 315-5197) */
static Uint16 sys16_fg_scrollx = 0;
static Uint16 sys16_fg_scrolly = 0;
static Uint16 sys16_bg_scrollx = 0;
static Uint16 sys16_bg_scrolly = 0;

/* =========================================================
 *  I/O & MISC REGISTERS
 * ========================================================= */

static Uint8 sys16_soundlatch = 0;     /* 68000 -> Z80 command byte  */
static Uint8 sys16_bankselect = 0;     /* ROM bank register          */

/* =========================================================
 *  MEMORY MAP - 68000
 *
 *  $000000 - $0FFFFF  Program ROM (banked via 315-5195)
 *  $100000 - $1FFFFF  Banked ROM area
 *  $200000 - $20FFFF  Work RAM
 *  $400000 - $401FFF  Sprite RAM
 *  $410000 - $417FFF  Tilemap RAM (FG + BG)
 *  $840000 - $840FFF  Palette RAM
 *  $C00000 - $C0001F  Video registers (scroll, etc.)
 *  $C40000            Sound latch (write)
 *  $C80000            I/O ports (8255 PPI equivalent)
 * ========================================================= */

/* --- Read handlers --- */

static Uint16 sys16_rom_r(Uint32 addr)
{
    /* Direct ROM read, no banking in this stub */
    return read16(sys16_rom + (addr & 0x0FFFFE));
}

static Uint16 sys16_workram_r(Uint32 addr)
{
    return read16(sys16_workram + (addr & 0x0FFFE));
}

static Uint16 sys16_spriteram_r(Uint32 addr)
{
    return read16(sys16_spriteram + (addr & 0x7FE));
}

static Uint16 sys16_tileram_r(Uint32 addr)
{
    return read16(sys16_tileram + (addr & 0x1FFE));
}

static Uint16 sys16_paletteram_r(Uint32 addr)
{
    return read16(sys16_paletteram + (addr & 0xFFE));
}

static Uint16 sys16_input_r(Uint32 addr)
{
    /*
     * TODO: Route through Raine input system.
     * Return active-low bitmask for player 1, player 2, coins.
     *
     *  Bit layout (typical System 16):
     *    D7 : P1 Start
     *    D6 : P2 Start
     *    D5 : Coin 1
     *    D4 : Coin 2
     *    D3 : P1 Up    (or P2 Up on second port)
     *    D2 : P1 Down
     *    D1 : P1 Left
     *    D0 : P1 Right / attack buttons vary per game
     */
    return 0xFFFF;  /* All inputs idle (active low) */
}

/* --- Write handlers --- */

static void sys16_workram_w(Uint32 addr, Uint16 data)
{
    write16(sys16_workram + (addr & 0x0FFFE), data);
}

static void sys16_spriteram_w(Uint32 addr, Uint16 data)
{
    write16(sys16_spriteram + (addr & 0x7FE), data);
}

static void sys16_tileram_w(Uint32 addr, Uint16 data)
{
    write16(sys16_tileram + (addr & 0x1FFE), data);
}

static void sys16_paletteram_w(Uint32 addr, Uint16 data)
{
    /*
     * Palette format: RGB444 packed in 16 bits
     *   Bits [11:8] = Red
     *   Bits  [7:4] = Green
     *   Bits  [3:0] = Blue
     * Needs expansion to 24-bit for Raine's renderer.
     */
    write16(sys16_paletteram + (addr & 0xFFE), data);
    /* TODO: call Raine palette update here */
}

static void sys16_soundlatch_w(Uint32 addr, Uint16 data)
{
    sys16_soundlatch = data & 0xFF;
    /* TODO: trigger Z80 NMI so it reads the latch */
}

static void sys16_scrollregs_w(Uint32 addr, Uint16 data)
{
    /*
     * Scroll register layout (315-5197, offsets from $C00000):
     *   $00 : BG scroll X
     *   $02 : BG scroll Y
     *   $04 : FG scroll X
     *   $06 : FG scroll Y
     */
    switch (addr & 0x1E)
    {
        case 0x00: sys16_bg_scrollx = data; break;
        case 0x02: sys16_bg_scrolly = data; break;
        case 0x04: sys16_fg_scrollx = data; break;
        case 0x06: sys16_fg_scrolly = data; break;
        default: break;
    }
}

static void sys16_bankselect_w(Uint32 addr, Uint16 data)
{
    /*
     * The 315-5195 mapper latches the bank selection here.
     * The exact bit layout differs per game; Golden Axe uses
     * bits [2:0] to select one of 8 ROM banks.
     * TODO: implement proper banking once ROM layout is confirmed.
     */
    sys16_bankselect = data & 0x07;
}

/* =========================================================
 *  MEMORY MAP - Z80 (Sound CPU)
 *
 *  $0000 - $7FFF  Sound ROM
 *  $8000 - $87FF  Sound RAM
 *  $A000          Read sound latch from 68000
 *  $B000          YM2151 address port
 *  $B001          YM2151 data port
 *  $C000          UPD7759 data write
 *  $C001          UPD7759 control
 * ========================================================= */

static Uint8 sys16_z80_rom_r(Uint16 addr)
{
    return sys16_soundrom[addr & 0x7FFF];
}

static Uint8 sys16_z80_ram_r(Uint16 addr)
{
    return sys16_soundram[addr & 0x7FF];
}

static void sys16_z80_ram_w(Uint16 addr, Uint8 data)
{
    sys16_soundram[addr & 0x7FF] = data;
}

static Uint8 sys16_soundlatch_r(Uint16 addr)
{
    return sys16_soundlatch;
}

/* YM2151 stubs - hook into Raine's YM2151 emulation */
static void sys16_ym2151_addr_w(Uint16 addr, Uint8 data)
{
    /* TODO: YM2151_write_port(0, data); */
}

static void sys16_ym2151_data_w(Uint16 addr, Uint8 data)
{
    /* TODO: YM2151_write_port(1, data); */
}

/* =========================================================
 *  VIDEO RENDERING
 *
 *  Render order (back to front):
 *    1. Background tilemap (BG layer)
 *    2. Low-priority sprites
 *    3. Foreground tilemap (FG layer)
 *    4. High-priority sprites
 * ========================================================= */

/*
 * sys16_draw_tilemap()
 * Draw one tilemap layer to the Raine framebuffer.
 *
 * @param ram       Pointer to the tilemap RAM for this layer
 * @param scrollx   Horizontal scroll value
 * @param scrolly   Vertical scroll value
 * @param priority  0 = background layer, 1 = foreground layer
 */
static void sys16_draw_tilemap(Uint8 *ram, int scrollx, int scrolly, int priority)
{
    int tx, ty;

    for (ty = 0; ty < SYS16_SCREEN_HEIGHT / SYS16_TILE_H + 1; ty++)
    {
        for (tx = 0; tx < SYS16_SCREEN_WIDTH / SYS16_TILE_W + 1; tx++)
        {
            /*
             * Each tilemap entry is 2 bytes (16-bit word):
             *   Bits [12:0] : Tile index into GFX ROM
             *   Bits [15:13]: Color palette select (0-7)
             *
             * The tile index is combined with the current
             * bank bits from the memory mapper for games
             * with large tile sets (e.g. Golden Axe).
             */
            int map_x = (tx + (scrollx / SYS16_TILE_W)) & (SYS16_MAP_COLS - 1);
            int map_y = (ty + (scrolly / SYS16_TILE_H)) & (SYS16_MAP_ROWS - 1);
            int map_offset = (map_y * SYS16_MAP_COLS + map_x) * 2;

            Uint16 entry     = read16(ram + map_offset);
            int    tile_idx  = entry & 0x1FFF;
            int    color     = (entry >> 13) & 0x07;

            int screen_x = tx * SYS16_TILE_W - (scrollx & (SYS16_TILE_W - 1));
            int screen_y = ty * SYS16_TILE_H - (scrolly & (SYS16_TILE_H - 1));

            /*
             * TODO: blit 8x8 tile from sys16_gfxrom using
             * tile_idx and color palette.
             * Raine provides draw_tile() or equivalent helpers.
             */
            (void)tile_idx; (void)color;
            (void)screen_x; (void)screen_y;
        }
    }
}

/*
 * sys16_draw_sprites()
 * Draw all sprites from sprite RAM.
 *
 * Sprite RAM layout (8 bytes per sprite):
 *   Word 0 : Y position + sprite height (number of tiles - 1)
 *   Word 1 : X position
 *   Word 2 : Starting tile index
 *   Word 3 : Attributes (color, flip X/Y, priority)
 */
static void sys16_draw_sprites(void)
{
    int i;

    for (i = 0; i < SYS16_MAX_SPRITES; i++)
    {
        int base = i * 8;

        Uint16 word0 = read16(sys16_spriteram + base + 0);
        Uint16 word1 = read16(sys16_spriteram + base + 2);
        Uint16 word2 = read16(sys16_spriteram + base + 4);
        Uint16 word3 = read16(sys16_spriteram + base + 6);

        int y         = (word0 & 0x01FF);        /* 9-bit Y position    */
        int height    = ((word0 >> 9) & 0x0F);   /* Height in tiles - 1 */
        int x         = (word1 & 0x01FF);        /* 9-bit X position    */
        int tile      =  word2 & 0x1FFF;         /* Tile index          */
        int color     = (word3 >> 11) & 0x07;    /* Palette select      */
        int flipx     = (word3 >> 14) & 1;
        int flipy     = (word3 >> 15) & 1;

        /* Skip sprite if Y == 0 or X == 0 (not visible / unused slot) */
        if (!y || !x)
            continue;

        /* Adjust for System 16 coordinate offset */
        x -= 0x60;
        y  = 0x100 - y;

        /*
         * TODO: blit sprite column (1 tile wide, 1..16 tiles tall)
         * from sys16_gfxrom starting at 'tile', using 'color' palette.
         * Respect flipx / flipy flags.
         */
        (void)tile; (void)color; (void)flipx; (void)flipy;
        (void)height;
    }
}

/*
 * sys16_refresh()
 * Called once per frame by Raine's main loop.
 */
static void sys16_refresh(void)
{
    /* 1. Background layer */
    sys16_draw_tilemap(sys16_tileram + 0x1000, sys16_bg_scrollx, sys16_bg_scrolly, 0);

    /* 2. Low-priority sprites (drawn before FG layer) */
    sys16_draw_sprites();

    /* 3. Foreground layer */
    sys16_draw_tilemap(sys16_tileram + 0x0000, sys16_fg_scrollx, sys16_fg_scrolly, 1);

    /* 4. High-priority sprites (drawn on top of everything) */
    /*    TODO: separate low/high priority sprite passes      */
}

/* =========================================================
 *  INITIALISATION & CLEANUP
 * ========================================================= */

/*
 * sys16_init()
 * Allocate memory, load ROMs, initialise CPU cores and sound.
 * Returns 0 on success, -1 on failure.
 */
static int sys16_init(void)
{
    /* --- Allocate ROM buffers --- */
    sys16_rom      = (Uint8 *)malloc(0x80000);  /* 512KB main ROMs  */
    sys16_soundrom = (Uint8 *)malloc(0x20000);  /* 128KB sound ROMs */
    sys16_gfxrom   = (Uint8 *)malloc(0x200000); /* 2MB  GFX ROMs    */
    sys16_adpcmrom = (Uint8 *)malloc(0x20000);  /* 128KB ADPCM      */

    if (!sys16_rom || !sys16_soundrom || !sys16_gfxrom || !sys16_adpcmrom)
    {
        printf("sega16: ROM allocation failed\n");
        return -1;
    }

    /* --- Load ROMs (Raine helper) --- */
    /*
     * TODO: call load_rom() for each ROM file listed in the
     * game descriptor (see struct GameDriver below).
     * Example for Golden Axe:
     *   load_rom("epr-12386.bin", sys16_rom,       0x00000, 0x40000);
     *   load_rom("epr-12387.bin", sys16_rom,       0x40000, 0x40000);
     *   load_rom("epr-12390.bin", sys16_gfxrom,    0x00000, 0x80000);
     *   load_rom("epr-12391.bin", sys16_gfxrom,    0x80000, 0x80000);
     *   load_rom("epr-12389.bin", sys16_soundrom,  0x00000, 0x20000);
     *   load_rom("epr-12388.bin", sys16_adpcmrom,  0x00000, 0x20000);
     */

    /* --- Initialise 68000 memory map --- */
    /*
     * TODO: use Raine's memory map API.
     * Typical calls look like:
     *   cpu_setOPbase16(sys16_rom_r);
     *   A68K_map_rb(0x000000, 0x0FFFFF, sys16_rom_r);
     *   A68K_map_wb(0x200000, 0x20FFFF, sys16_workram_w);
     *   ...
     */

    /* --- Initialise Z80 --- */
    /*
     * TODO: init Z80 core and map its address space.
     */

    /* --- Initialise sound chips --- */
    /*
     * TODO:
     *   YM2151_init(SYS16_YM2151_CLOCK);
     *   UPD7759_init(sys16_adpcmrom, 0x20000);
     */

    memset(sys16_workram,   0, sizeof(sys16_workram));
    memset(sys16_soundram,  0, sizeof(sys16_soundram));
    memset(sys16_tileram,   0, sizeof(sys16_tileram));
    memset(sys16_spriteram, 0, sizeof(sys16_spriteram));
    memset(sys16_paletteram,0, sizeof(sys16_paletteram));

    printf("sega16: init OK\n");
    return 0;
}

/*
 * sys16_reset()
 * Hardware reset (called on emulator reset or game restart).
 */
static void sys16_reset(void)
{
    sys16_soundlatch = 0;
    sys16_bankselect = 0;
    sys16_fg_scrollx = sys16_fg_scrolly = 0;
    sys16_bg_scrollx = sys16_bg_scrolly = 0;
    /* TODO: reset 68000 and Z80 cores */
}

/*
 * sys16_free()
 * Free all allocated memory on emulator exit.
 */
static void sys16_free(void)
{
    free(sys16_rom);       sys16_rom       = NULL;
    free(sys16_soundrom);  sys16_soundrom  = NULL;
    free(sys16_gfxrom);    sys16_gfxrom    = NULL;
    free(sys16_adpcmrom);  sys16_adpcmrom  = NULL;
}

/* =========================================================
 *  GAME DESCRIPTORS
 *
 *  Each game on the System 16B needs its own entry in the
 *  GameDriver table, listing its ROMs and any game-specific
 *  quirks (different ROM banking, custom protection IC, etc.).
 *
 *  The 315-5195 mapper configuration differs per game;
 *  that mapping table will need to be loaded alongside
 *  the standard ROMs.
 * ========================================================= */

/*
 * Golden Axe (World, Rev A)
 * Board: 171-5704 (System 16B)
 * Protection: 317-0110 (FD1094 encrypted CPU)
 *   Note: FD1094 decryption is a significant extra step.
 *         A decrypted ROM set can be used for initial work.
 */
static struct GameDriver driver_goldnaxe =
{
    "goldnaxe",                 /* ROM set name (must match zip name) */
    "Golden Axe",               /* Full display name                  */
    "1989",                     /* Year                               */
    "Sega",                     /* Manufacturer                       */
    sys16_init,                 /* init()    callback                 */
    sys16_reset,                /* reset()   callback                 */
    sys16_free,                 /* free()    callback                 */
    sys16_refresh,              /* refresh() callback (once per frame)*/
    SYS16_SCREEN_WIDTH,
    SYS16_SCREEN_HEIGHT,
    SYS16_SCREEN_REFRESH,
    /*
     * ROM list would be declared here following Raine conventions.
     * See existing drivers (e.g. cps1.c) for the exact macro syntax.
     */
};

/*
 * Altered Beast (World)
 * Very similar to Golden Axe; also uses FD1094 on most board revisions.
 * The "Datsu" bootleg uses a standard 68000 and is easier to start with.
 */
static struct GameDriver driver_altbeast =
{
    "altbeast",
    "Altered Beast",
    "1988",
    "Sega",
    sys16_init,
    sys16_reset,
    sys16_free,
    sys16_refresh,
    SYS16_SCREEN_WIDTH,
    SYS16_SCREEN_HEIGHT,
    SYS16_SCREEN_REFRESH,
};

/*
 * Shinobi (World)
 * Uses the 317-0049 protection IC on the original board,
 * but Datsu bootlegs are unprotected.
 */
static struct GameDriver driver_shinobi =
{
    "shinobi",
    "Shinobi",
    "1987",
    "Sega",
    sys16_init,
    sys16_reset,
    sys16_free,
    sys16_refresh,
    SYS16_SCREEN_WIDTH,
    SYS16_SCREEN_HEIGHT,
    SYS16_SCREEN_REFRESH,
};

/* =========================================================
 *  NOTES FOR THE IMPLEMENTER
 *  -------------------------------------------------------
 *  1. FD1094 PROTECTION
 *     Many System 16B games use the FD1094 custom CPU,
 *     which is a 68000 with encrypted opcodes. The decryption
 *     key is stored in an internal battery-backed RAM inside
 *     the chip. MAME has a full FD1094 decryption engine
 *     (src/mame/machine/fd1094.cpp) that could be ported.
 *     Starting with bootleg / decrypted sets avoids this
 *     problem entirely during early development.
 *
 *  2. 315-5195 MEMORY MAPPER
 *     This IC maps 23-bit addresses from the 68000 bus to
 *     physical ROM/RAM locations. Each game has a different
 *     mapping table burned into a small PROM. The table
 *     needs to be read from ROM and applied at startup.
 *     MAME's segas16b.cpp has a full implementation.
 *
 *  3. SPRITE SYSTEM
 *     The 315-5248 sprite chip draws sprites in a single
 *     column (1 tile wide, 1-16 tiles tall). Multiple
 *     columns are positioned side by side to form wide
 *     characters. Sprites are stored sorted by priority;
 *     the hardware draws them in reverse order.
 *
 *  4. PALETTE
 *     12-bit RGB444 values are stored in palette RAM.
 *     Expand to 24-bit: R = (r << 4) | r, etc.
 *     There are 8 banks of 32 colors for tiles and
 *     8 banks of 16 colors for sprites.
 *
 *  5. SOUND
 *     The Z80 polls the sound latch written by the 68000.
 *     A NMI is usually triggered after the write.
 *     The UPD7759 plays back PCM samples for voice/SFX.
 *
 *  6. SUGGESTED DEVELOPMENT ORDER
 *     a) Get 68000 executing from ROM (check PC after reset)
 *     b) Implement palette RAM writes and a basic tile blit
 *     c) Draw the background tilemap (static, no scroll)
 *     d) Add scroll register support
 *     e) Draw sprites
 *     f) Hook up inputs (coins, P1, P2)
 *     g) Add Z80 / YM2151 sound
 *     h) Handle FD1094 decryption (if using original ROMs)
 * ========================================================= */

