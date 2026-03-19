/*
 * sega16.c - Sega System 16B driver skeleton for Raine
 *
 * Target hardware: Sega System 16B
 * Representative games: Golden Axe, Altered Beast, Shinobi,
 *                       Tetris (Sega)
 *
 * Main CPU  : Motorola 68000 @ 10 MHz
 * Sound CPU : Zilog Z80 @ 4 MHz
 * Sound     : YM2151 + UPD7759 (ADPCM samples)
 * Video     : Custom Sega ICs:
 *               315-5195  memory mapper / bank switcher
 *               315-5197  tilemap scroll controller
 *               315-5248  sprite generator
 *             - Two scrolling tilemap layers (FG/BG), 64x32 tiles each
 *             - One sprite layer, up to 128 sprites
 *             - Palette: 2048 colors (RGB444)
 *
 * ROM regions used:
 *   REGION_ROM1  : 68000 program ROMs (up to 512KB)
 *   REGION_ROM2  : Z80 sound ROMs (up to 128KB)
 *   REGION_GFX1  : Tile + sprite GFX ROMs
 *   REGION_SMP1  : UPD7759 ADPCM sample ROMs
 *
 * References:
 *   MAME source  : src/mame/sega/segas16b.cpp
 *   Charles MacDonald's System 16 hardware notes
 *
 * Status: SKELETON - does not run games yet.
 *         Memory map, video stubs, ROM loading and driver
 *         declarations are provided as a starting point.
 *
 * Note on protection:
 *   Most System 16B PCBs use the FD1094 custom encrypted CPU.
 *   For initial development, use decrypted / bootleg ROM sets
 *   to avoid implementing the FD1094 decryption engine early on.
 */

#include "gameinc.h"    /* Raine core: UINT8/16/32, AddReadWord, VIDEO_INFO... */
#include "sasound.h"    /* Raine sound system                                  */
#include "savegame.h"   /* Save-state support                                  */
#include "2151intf.h"   /* YM2151 interface                                    */
#include "timer.h"      /* execute_z80_audio_frame                             */
#include "emumain.h"    /* set_reset_function                                  */

/* =========================================================
 *  CONSTANTS
 * ========================================================= */

#define SYS16_MAIN_CPU_CLOCK   10000000   /* 68000 @ 10 MHz         */
#define SYS16_SOUND_CPU_CLOCK   4000000   /* Z80  @  4 MHz          */
#define SYS16_YM2151_CLOCK      4000000   /* YM2151 @ 4 MHz         */

#define SYS16_SCREEN_W          320
#define SYS16_SCREEN_H          224
#define SYS16_FPS                60.0f

/* Tilemap geometry */
#define SYS16_MAP_COLS           64
#define SYS16_MAP_ROWS           32
#define SYS16_TILE_W              8
#define SYS16_TILE_H              8

/* Maximum sprites per frame */
#define SYS16_MAX_SPRITES       128

/* =========================================================
 *  VIDEO RAM  (laid out as it appears on the real PCB)
 *
 *  Tilemap RAM  : two layers of 64x32 word entries
 *                 FG layer at offset 0x0000
 *                 BG layer at offset 0x1000
 *  Sprite RAM   : 128 sprites x 8 bytes = 0x400 words
 *  Palette RAM  : 2048 entries x 2 bytes (RGB444)
 * ========================================================= */

static UINT8 sys16_tileram  [0x2000];   /* FG=[0x0000..0x0FFF] BG=[0x1000..0x1FFF] */
static UINT8 sys16_spriteram[0x0800];
static UINT8 sys16_paletteram[0x1000];

/* Scroll registers written by the 315-5197 */
static UINT16 sys16_fg_scrollx, sys16_fg_scrolly;
static UINT16 sys16_bg_scrollx, sys16_bg_scrolly;

/* =========================================================
 *  WORK RAM
 * ========================================================= */

static UINT8 sys16_workram [0x10000];   /* 64KB 68000 work RAM */
static UINT8 sys16_soundram[0x0800];    /*  2KB Z80  work RAM  */

/* =========================================================
 *  I/O REGISTERS
 * ========================================================= */

static UINT8  sys16_soundlatch = 0;     /* 68000 -> Z80 command byte */
static UINT8  sys16_bankselect = 0;     /* 315-5195 bank register    */

/* =========================================================
 *  68000 READ HANDLERS
 *
 *  Raine's starscream interface passes the full bus address.
 *  All word handlers return UINT16.
 * ========================================================= */

static UINT16 sys16_workram_rw(UINT32 addr)
{
    return *(UINT16 *)(sys16_workram + (addr & 0x0FFFE));
}

static UINT16 sys16_spriteram_rw(UINT32 addr)
{
    return *(UINT16 *)(sys16_spriteram + (addr & 0x07FE));
}

static UINT16 sys16_tileram_rw(UINT32 addr)
{
    return *(UINT16 *)(sys16_tileram + (addr & 0x1FFE));
}

static UINT16 sys16_paletteram_rw(UINT32 addr)
{
    return *(UINT16 *)(sys16_paletteram + (addr & 0x0FFE));
}

static UINT16 sys16_input_rw(UINT32 addr)
{
    /*
     * Typical System 16B port layout (active low):
     *   Bits [15:8] : Player 2 inputs
     *   Bits  [7:0] : Player 1 inputs
     *
     * Bit meaning (each nibble):
     *   7=Start  6=unused  5=B3  4=B2  3=B1  2=Right  1=Left  0=Down
     *   (varies per game)
     *
     * TODO: wire to Raine input system.
     */
    return 0xFFFF;
}

static UINT16 sys16_coin_rw(UINT32 addr)
{
    /*
     * Coin / service inputs, also active low.
     * Bit 0 = Coin 1, Bit 1 = Coin 2, Bit 2 = Service
     * TODO: wire to Raine input system.
     */
    return 0xFFFF;
}

/* =========================================================
 *  68000 WRITE HANDLERS
 * ========================================================= */

static void sys16_workram_ww(UINT32 addr, UINT16 data)
{
    *(UINT16 *)(sys16_workram + (addr & 0x0FFFE)) = data;
}

static void sys16_spriteram_ww(UINT32 addr, UINT16 data)
{
    *(UINT16 *)(sys16_spriteram + (addr & 0x07FE)) = data;
}

static void sys16_tileram_ww(UINT32 addr, UINT16 data)
{
    *(UINT16 *)(sys16_tileram + (addr & 0x1FFE)) = data;
}

static void sys16_paletteram_ww(UINT32 addr, UINT16 data)
{
    /*
     * Palette format: RGB444 packed in 16 bits
     *   Bits [11:8] = Red (4 bits)
     *   Bits  [7:4] = Green (4 bits)
     *   Bits  [3:0] = Blue (4 bits)
     * Expand to 8-bit per channel: c8 = (c4 << 4) | c4
     * TODO: call Raine palette update helper.
     */
    *(UINT16 *)(sys16_paletteram + (addr & 0x0FFE)) = data;
}

static void sys16_soundlatch_ww(UINT32 addr, UINT16 data)
{
    /*
     * 68000 writes a command byte here; the Z80 reads it back
     * via its I/O port. A NMI is usually triggered on the Z80.
     * TODO: trigger Z80 NMI after write.
     */
    sys16_soundlatch = (UINT8)(data & 0xFF);
}

static void sys16_scrollregs_ww(UINT32 addr, UINT16 data)
{
    /*
     * 315-5197 scroll registers, offsets from base $C00000:
     *   +0x00 : BG scroll X
     *   +0x02 : BG scroll Y
     *   +0x04 : FG scroll X
     *   +0x06 : FG scroll Y
     */
    switch (addr & 0x0E)
    {
        case 0x00: sys16_bg_scrollx = data; break;
        case 0x02: sys16_bg_scrolly = data; break;
        case 0x04: sys16_fg_scrollx = data; break;
        case 0x06: sys16_fg_scrolly = data; break;
    }
}

static void sys16_bankselect_ww(UINT32 addr, UINT16 data)
{
    /*
     * The 315-5195 mapper latches ROM bank selection here.
     * Bits [2:0] select one of 8 banks for the banked ROM area.
     * The exact bit layout is game-specific.
     * TODO: implement per-game bank mapping table.
     */
    sys16_bankselect = (UINT8)(data & 0x07);
}

/* =========================================================
 *  Z80 READ/WRITE HANDLERS
 *
 *  The Z80 sound CPU has a simple 64KB address space.
 *  Raine's Z80 interface uses UINT32 addresses and UINT8 data.
 * ========================================================= */

static UINT8 sys16_z80_soundlatch_r(UINT32 addr)
{
    return sys16_soundlatch;
}

static void sys16_ym2151_addr_w(UINT32 addr, UINT8 data)
{
    /* TODO: YM2151_write_port(0, data); */
}

static void sys16_ym2151_data_w(UINT32 addr, UINT8 data)
{
    /* TODO: YM2151_write_port(1, data); */
}

static void sys16_upd7759_w(UINT32 addr, UINT8 data)
{
    /* TODO: upd7759_write(data); */
}

/* =========================================================
 *  VIDEO RENDERING
 *
 *  Render order (back to front):
 *    1. BG tilemap
 *    2. Low-priority sprites
 *    3. FG tilemap
 *    4. High-priority sprites
 * ========================================================= */

/*
 * sys16_draw_tilemap()
 * Blit one tilemap layer to the Raine framebuffer.
 *
 * @param vram    Pointer to the 64x32 tilemap word array for this layer
 * @param scrollx Horizontal scroll in pixels
 * @param scrolly Vertical scroll in pixels
 *
 * Each 16-bit tilemap word:
 *   Bits [12:0] : Tile index into REGION_GFX1
 *   Bits [15:13]: Color palette bank (0-7)
 */
static void sys16_draw_tilemap(UINT8 *vram, int scrollx, int scrolly)
{
    int tx, ty;
    UINT8 *gfxbase = REG(GFX1);   /* tile pixel data from REGION_GFX1 */

    if (!gfxbase) return;          /* GFX not loaded yet */

    for (ty = 0; ty < SYS16_SCREEN_H / SYS16_TILE_H + 1; ty++)
    {
        for (tx = 0; tx < SYS16_SCREEN_W / SYS16_TILE_W + 1; tx++)
        {
            int map_x = (tx + (scrollx >> 3)) & (SYS16_MAP_COLS - 1);
            int map_y = (ty + (scrolly >> 3)) & (SYS16_MAP_ROWS - 1);
            int map_off = (map_y * SYS16_MAP_COLS + map_x) * 2;

            UINT16 entry    = *(UINT16 *)(vram + map_off);
            int    tile_idx = entry & 0x1FFF;
            int    color    = (entry >> 13) & 0x07;

            int screen_x = tx * SYS16_TILE_W - (scrollx & (SYS16_TILE_W - 1));
            int screen_y = ty * SYS16_TILE_H - (scrolly & (SYS16_TILE_H - 1));

            /*
             * TODO: blit 8x8 tile from gfxbase at index tile_idx
             * using color palette bank 'color'.
             * Use Raine's tile blitting helpers from newspr.h / scroll.h.
             */
            (void)tile_idx; (void)color;
            (void)screen_x; (void)screen_y;
        }
    }
}

/*
 * sys16_draw_sprites()
 * Blit all active sprites from sys16_spriteram.
 *
 * Sprite RAM layout: 8 bytes (4 words) per sprite entry
 *
 *   Word 0 [15:9] : Height in tiles minus 1 (0=1 tile, 15=16 tiles)
 *   Word 0  [8:0] : Y position (9-bit, hardware offset applies)
 *   Word 1  [8:0] : X position (9-bit, hardware offset applies)
 *   Word 2 [12:0] : Starting tile index in GFX ROM
 *   Word 3 [15]   : Flip Y
 *   Word 3 [14]   : Flip X
 *   Word 3 [13:11]: Color palette bank
 *   Word 3  [0]   : Priority (0=behind FG layer, 1=above FG layer)
 *
 * Sprites are 1 tile wide, 1 to 16 tiles tall.
 * Wide characters are built from multiple adjacent sprite columns.
 */
static void sys16_draw_sprites(void)
{
    int i;
    UINT8 *gfxbase = REG(GFX1);

    if (!gfxbase) return;

    for (i = 0; i < SYS16_MAX_SPRITES; i++)
    {
        UINT16 *sp   = (UINT16 *)(sys16_spriteram + i * 8);
        int height   = ((sp[0] >> 9) & 0x0F) + 1;  /* tiles tall        */
        int y        =   sp[0] & 0x01FF;
        int x        =   sp[1] & 0x01FF;
        int tile     =   sp[2] & 0x1FFF;
        int flipy    =  (sp[3] >> 15) & 1;
        int flipx    =  (sp[3] >> 14) & 1;
        int color    =  (sp[3] >> 11) & 0x07;
        int priority =   sp[3] & 0x01;

        /* Skip unused sprite slots */
        if (!x && !y) continue;

        /* Apply System 16B coordinate offsets */
        x -= 0x60;
        y  = 0x100 - y;

        /*
         * TODO: blit sprite column of 'height' tiles from gfxbase
         * starting at tile index 'tile', at screen position (x, y),
         * with flip flags and color palette 'color'.
         * Use Raine's sprite blitting routines from newspr.h.
         */
        (void)height; (void)tile;
        (void)flipx;  (void)flipy;
        (void)color;  (void)priority;
    }
}

/*
 * draw_game_sys16()
 * Main video refresh callback — called once per frame by Raine.
 * This is the function pointed to by video_goldnaxe.draw_game.
 */
static void draw_game_sys16(void)
{
    /* 1. BG layer */
    sys16_draw_tilemap(sys16_tileram + 0x1000, sys16_bg_scrollx, sys16_bg_scrolly);

    /* 2. Low-priority sprites */
    /* TODO: split into two passes based on priority bit */
    sys16_draw_sprites();

    /* 3. FG layer */
    sys16_draw_tilemap(sys16_tileram + 0x0000, sys16_fg_scrollx, sys16_fg_scrolly);

    /* 4. High-priority sprites — TODO: second pass here */
}

/* =========================================================
 *  VIDEO_INFO STRUCTURES
 *  One per game (or shared when hardware is identical).
 * ========================================================= */

static VIDEO_INFO video_goldnaxe =
{
    draw_game_sys16,     /* draw_game  : called each frame     */
    SYS16_SCREEN_W,      /* screen_x                           */
    SYS16_SCREEN_H,      /* screen_y                           */
    0,                   /* border_size: no overscan border    */
    VIDEO_ROTATE_NORMAL, /* flags                              */
    NULL,                /* gfx_list   : decoded in load_game  */
    SYS16_FPS            /* fps                                */
};

/* Altered Beast and Shinobi share the same screen geometry */
#define video_altbeast  video_goldnaxe
#define video_shinobi   video_goldnaxe

/* =========================================================
 *  68000 MEMORY MAP INITIALISATION
 *
 *  Called from load_goldnaxe() (and shared by other games).
 *  Uses Raine's starhelp.h API:
 *
 *    add_68000_rom(cpu, start, end, ptr)
 *      Map a read-only ROM region (opcode fetch + data read).
 *
 *    add_68000_ram(cpu, start, end, ptr)
 *      Map a read/write RAM region (byte + word handlers).
 *
 *    AddReadWord (start, end, handler_fn, NULL)
 *    AddWriteWord(start, end, handler_fn, NULL)
 *      Map custom word read/write handlers.
 *
 *    AddInitMemory()
 *      Commit the map to the 68000 core (call once after setup).
 *
 *  System 16B address map:
 *   $000000-$0FFFFF  Program ROM (512KB, direct)
 *   $100000-$1FFFFF  Banked ROM area (via 315-5195 mapper)
 *   $200000-$20FFFF  Work RAM (64KB)
 *   $400000-$4007FF  Sprite RAM
 *   $410000-$411FFF  Tilemap RAM (FG + BG)
 *   $840000-$840FFF  Palette RAM
 *   $C00000-$C0000F  Scroll registers (315-5197)
 *   $C40000          Sound latch write
 *   $C80000-$C80003  I/O ports (inputs, coins)
 *   $FE0000          Bank select write (315-5195)
 * ========================================================= */

static void sys16_init_68000_map(void)
{
    Clear68000List();

    /* Program ROM — direct opcode fetch + data read, no handler overhead */
    add_68000_rom(0, 0x000000, 0x0FFFFF, REG(ROM1));

    /* Banked ROM area — mirror lower ROM until 315-5195 mapper implemented */
    add_68000_rom(0, 0x100000, 0x1FFFFF, REG(ROM1));

    /* Work RAM — byte + word read/write via Raine's direct RAM helper */
    add_68000_ram(0, 0x200000, 0x20FFFF, sys16_workram);

    /* Sprite RAM */
    AddReadWord (0x400000, 0x4007FF, sys16_spriteram_rw, NULL);
    AddWriteWord(0x400000, 0x4007FF, sys16_spriteram_ww, NULL);

    /* Tilemap RAM */
    AddReadWord (0x410000, 0x411FFF, sys16_tileram_rw,   NULL);
    AddWriteWord(0x410000, 0x411FFF, sys16_tileram_ww,   NULL);

    /* Palette RAM */
    AddReadWord (0x840000, 0x840FFF, sys16_paletteram_rw, NULL);
    AddWriteWord(0x840000, 0x840FFF, sys16_paletteram_ww, NULL);

    /* Scroll registers (315-5197) — write only */
    AddWriteWord(0xC00000, 0xC0000F, sys16_scrollregs_ww, NULL);

    /* Sound latch — write only */
    AddWriteWord(0xC40000, 0xC40001, sys16_soundlatch_ww, NULL);

    /* I/O ports — read only */
    AddReadWord(0xC80000, 0xC80001, sys16_input_rw, NULL);
    AddReadWord(0xC80002, 0xC80003, sys16_coin_rw,  NULL);

    /* Bank select (315-5195) — write only */
    AddWriteWord(0xFE0000, 0xFE0001, sys16_bankselect_ww, NULL);

    /* Commit the memory map to the 68000 emulation core */
    AddInitMemory();
}

/* =========================================================
 *  GAME LOAD FUNCTIONS
 *
 *  load_xxx() is called by Raine after the ROMs have been
 *  loaded into load_region[] by load_game_rom_info().
 *  It must:
 *    1. Optionally decode / rearrange GFX regions
 *    2. Set up the 68000 and Z80 memory maps
 *    3. Initialise sound chips
 *    4. Reset all state
 * ========================================================= */

static void load_goldnaxe(void)
{
    /* Clear all RAM and registers */
    memset(sys16_workram,    0, sizeof(sys16_workram));
    memset(sys16_soundram,   0, sizeof(sys16_soundram));
    memset(sys16_tileram,    0, sizeof(sys16_tileram));
    memset(sys16_spriteram,  0, sizeof(sys16_spriteram));
    memset(sys16_paletteram, 0, sizeof(sys16_paletteram));
    sys16_soundlatch = 0;
    sys16_bankselect = 0;
    sys16_fg_scrollx = sys16_fg_scrolly = 0;
    sys16_bg_scrollx = sys16_bg_scrolly = 0;

    /* TODO: GFX decoding if required
     * e.g. decode_region(REGION_GFX1, &sys16_gfx_layout); */

    /* Set up 68000 address space */
    sys16_init_68000_map();

    /* TODO: initialise Z80 + sound chips
     * YM2151_init(SYS16_YM2151_CLOCK);
     * upd7759_init(REG(SMP1), get_region_size(REGION_SMP1)); */
}

static void load_altbeast(void) { load_goldnaxe(); }
static void load_shinobi (void) { load_goldnaxe(); }

/* =========================================================
 *  EXECUTE FUNCTIONS
 *
 *  execute_xxx() is the per-frame main loop.
 *  It runs the CPUs for one video frame worth of cycles,
 *  handles interrupts, and triggers the video refresh.
 *
 *  System 16B interrupt scheme:
 *    IRQ 4 : fired at vblank  (main game logic)
 *    IRQ 2 : fired at a mid-screen scanline (raster effects)
 * ========================================================= */

static void execute_goldnaxe(void)
{
    int cycles = SYS16_MAIN_CPU_CLOCK / (int)SYS16_FPS;

    cpu_execute_cycles(CPU_68K_0, cycles);
    cpu_interrupt(CPU_68K_0, 4);   /* vblank IRQ 4 */

    /* TODO: run Z80 for one audio frame
     * execute_z80_audio_frame(SYS16_SOUND_CPU_CLOCK, (int)SYS16_FPS); */
}

static void execute_altbeast(void) { execute_goldnaxe(); }
static void execute_shinobi (void) { execute_goldnaxe(); }

/* =========================================================
 *  SOUND FUNCTIONS
 *
 *  sound_xxx() is called by Raine's sound system each frame.
 *  TODO: mix YM2151 and UPD7759 output into Raine's audio buffer.
 * ========================================================= */

static void sound_goldnaxe(void)
{
    /* TODO */
}

static void sound_altbeast(void) { sound_goldnaxe(); }
static void sound_shinobi (void) { sound_goldnaxe(); }

/* =========================================================
 *  INPUT TABLES
 *
 *  Uses macros from control.h:
 *    INP0(KB_DEF_xxx, offset, bitmask)  active-low  (0 = pressed)
 *    INP1(KB_DEF_xxx, offset, bitmask)  active-high (1 = pressed)
 *    END_INPUT                          terminator
 *
 *  The KB_DEF_xxx constants (P1_START, P1_UP, P1_B1...) are
 *  defined in control.h and map to default host keys.
 *  Offsets refer to input_buffer[] positions set by update_inputs().
 *
 *  System 16B uses active-low inputs (0 = button pressed),
 *  so all entries use INP0.
 *
 *  Bit layout for Golden Axe (input_buffer offsets):
 *    0x00 : P1  bits — 7=Start 5=B2(Magic) 4=B1(Attack)
 *                       3=Up   2=Down  1=Right  0=Left
 *    0x01 : P2  bits — same layout
 *    0x02 : System   — 0=Coin1  1=Coin2  2=Service
 *
 *  TODO: verify offsets against real PCB once 68000 boots.
 * ========================================================= */

static INPUT_INFO input_goldnaxe[] =
{
    INP0( P1_START,  0x00, 0x80 ),
    INP0( P1_UP,     0x00, 0x08 ),
    INP0( P1_DOWN,   0x00, 0x04 ),
    INP0( P1_RIGHT,  0x00, 0x02 ),
    INP0( P1_LEFT,   0x00, 0x01 ),
    INP0( P1_B1,     0x00, 0x10 ),   /* Attack */
    INP0( P1_B2,     0x00, 0x20 ),   /* Magic  */
    INP0( P2_START,  0x01, 0x80 ),
    INP0( P2_UP,     0x01, 0x08 ),
    INP0( P2_DOWN,   0x01, 0x04 ),
    INP0( P2_RIGHT,  0x01, 0x02 ),
    INP0( P2_LEFT,   0x01, 0x01 ),
    INP0( P2_B1,     0x01, 0x10 ),
    INP0( P2_B2,     0x01, 0x20 ),
    INP0( COIN1,     0x02, 0x01 ),
    INP0( COIN2,     0x02, 0x02 ),
    END_INPUT
};

#define input_altbeast input_goldnaxe
#define input_shinobi  input_goldnaxe

/* =========================================================
 *  DIP SWITCH TABLES
 *
 *  Generated by the Raine perl DSW converter from MAME's
 *  segas16b.cpp, then cleaned up manually.
 *
 *  Structure:
 *    DSW_DATA : describes one SW byte — list of named bit
 *               field settings. Format per entry:
 *               { label, bitmask, count_or_0 }
 *               where count > 0 marks a group header.
 *               Terminated by { NULL, 0, 0 }.
 *
 *    DSW_INFO : binds a DSW_DATA table to a memory offset
 *               and an active-bits mask.
 *               Format: { offset, active_mask, dsw_data_ptr }
 *               Terminated by { 0, 0, NULL }.
 *
 *  The common coinage table (SW1) is shared by most games
 *  via dsw_sys16_common[].
 *
 *  NOTE: the perl converter is not perfect — bootleg entries
 *  (dsw_ddcrewbl, dsw_mwalkbl, dsw_shdancbl) had corrupt
 *  offsets (0xfffffffffffffffe) and anonymous DSW_DATA structs;
 *  those are omitted here.
 * ========================================================= */

/* --- Shared coinage table (SW1, used by most System 16B games) --- */
static struct DSW_DATA dsw_data_sys16_common_3[] =
{
    /* SW1:1,2,3,4 — Coin A */
    { MSG_COIN1,        0x0f, 16 },
    { MSG_4COIN_1PLAY,  0x07, 0x00 },
    { MSG_3COIN_1PLAY,  0x08, 0x00 },
    { MSG_2COIN_1PLAY,  0x09, 0x00 },
    { "2 Coins/1 Credit 5/3 6/4", 0x05, 0x00 },
    { "2 Coins/1 Credit 4/3",     0x04, 0x00 },
    { MSG_1COIN_1PLAY,  0x0f, 0x00 },
    { "1 Coin/1 Credit 2/3",      0x01, 0x00 },
    { "1 Coin/1 Credit 4/5",      0x02, 0x00 },
    { "1 Coin/1 Credit 5/6",      0x03, 0x00 },
    { MSG_2COIN_3PLAY,  0x06, 0x00 },
    { MSG_1COIN_2PLAY,  0x0e, 0x00 },
    { MSG_1COIN_3PLAY,  0x0d, 0x00 },
    { MSG_1COIN_4PLAY,  0x0c, 0x00 },
    { MSG_1COIN_5PLAY,  0x0b, 0x00 },
    { MSG_1COIN_6PLAY,  0x0a, 0x00 },
    { "Free Play (if Coin B too) or 1/1", 0x00, 0x00 },
    /* SW1:5,6,7,8 — Coin B */
    { MSG_COIN2,        0xf0, 16 },
    { MSG_4COIN_1PLAY,  0x70, 0x00 },
    { MSG_3COIN_1PLAY,  0x80, 0x00 },
    { MSG_2COIN_1PLAY,  0x90, 0x00 },
    { "2 Coins/1 Credit 5/3 6/4", 0x50, 0x00 },
    { "2 Coins/1 Credit 4/3",     0x40, 0x00 },
    { MSG_1COIN_1PLAY,  0xf0, 0x00 },
    { "1 Coin/1 Credit 2/3",      0x10, 0x00 },
    { "1 Coin/1 Credit 4/5",      0x20, 0x00 },
    { "1 Coin/1 Credit 5/6",      0x30, 0x00 },
    { MSG_2COIN_3PLAY,  0x60, 0x00 },
    { MSG_1COIN_2PLAY,  0xe0, 0x00 },
    { MSG_1COIN_3PLAY,  0xd0, 0x00 },
    { MSG_1COIN_4PLAY,  0xc0, 0x00 },
    { MSG_1COIN_5PLAY,  0xb0, 0x00 },
    { MSG_1COIN_6PLAY,  0xa0, 0x00 },
    { "Free Play (if Coin A too) or 1/1", 0x00, 0x00 },
    { NULL, 0, 0 }
};

static struct DSW_INFO dsw_sys16_common[] =
{
    { 0x6, 0xff, dsw_data_sys16_common_3 },
    { 0, 0, NULL }
};

/* --- Golden Axe --- */
static struct DSW_DATA dsw_data_goldnaxe_0[] =
{
    /* SW2:1 */
    { "Credits Needed",       0x01, 2 },
    { "1 Credit To Start",    0x01, 0x00 },
    { "2 Credits To Start",   0x00, 0x00 },
    /* SW2:2 */
    DSW_DEMO_SOUND( 0x00, 0x02 ),
    /* SW2:3,4,5,6 */
    { MSG_DIFFICULTY,         0x3c, 8 },
    { "Special",              0x00, 0x00 },
    { "Easiest",            0x14, 0x00 },
    { "Easier",             0x34, 0x00 },
    { MSG_EASY,               0x1c, 0x00 },
    { MSG_NORMAL,             0x3c, 0x00 },
    { MSG_HARD,               0x38, 0x00 },
    { "Harder",             0x2c, 0x00 },
    { MSG_HARDEST,            0x28, 0x00 },
    { NULL, 0, 0 }
};

static struct DSW_INFO dsw_goldnaxe[] =
{
    { 0x0, 0x3d, dsw_data_goldnaxe_0 },
    { 0, 0, NULL }
};

/* --- Shinobi --- */
static struct DSW_DATA dsw_data_shinobi_0[] =
{
    /* SW2:1 */
    { MSG_CABINET,            0x01, 2 },
    { MSG_UPRIGHT,            0x00, 0x00 },
    { MSG_TABLE,              0x01, 0x00 },
    /* SW2:2 */
    DSW_DEMO_SOUND( 0x00, 0x02 ),
    /* SW2:3,4 */
    { MSG_LIVES,              0x0c, 4 },
    { "2",                    0x08, 0x00 },
    { "3",                    0x0c, 0x00 },
    { "5",                    0x04, 0x00 },
    { MSG_FREE_PLAY,          0x00, 0x00 },
    /* SW2:5,6 */
    { MSG_DIFFICULTY,         0x30, 4 },
    { MSG_EASY,               0x20, 0x00 },
    { MSG_NORMAL,             0x30, 0x00 },
    { MSG_HARD,               0x10, 0x00 },
    { MSG_HARDEST,            0x00, 0x00 },
    /* SW2:7 */
    { "Enemy's Bullet Speed", 0x40, 2 },
    { "Slow",                 0x40, 0x00 },
    { "Fast",                 0x00, 0x00 },
    /* SW2:8 */
    { "Language",           0x80, 2 },
    { "Japenese",           0x80, 0x00 },
    { "English",            0x00, 0x00 },
    { NULL, 0, 0 }
};

static struct DSW_INFO dsw_shinobi[] =
{
    { 0x0, 0xfc, dsw_data_shinobi_0 },
    { 0, 0, NULL }
};

/* --- Altered Beast ---
 * TODO: add dsw_data_altbeast_0 once the MAME source entry
 * for altbeast is confirmed. Using goldnaxe as placeholder. */
#define dsw_altbeast dsw_goldnaxe

/* =========================================================
 *  ROM DEFINITIONS
 *
 *  Macros from loadroms.h:
 *    LOAD(region, filename, offset, size, crc32)
 *      Load a ROM file into a region at a given offset.
 *
 *    LOAD8_16(region, filename, offset, size, crc32)
 *      Interleaved byte load (every other byte).
 *      Use pairs with offset 0 (even) and offset 1 (odd)
 *      to reconstruct a 16-bit wide ROM from two 8-bit chips.
 *
 *    ROM_END
 *      Terminates the ROM_INFO array.
 *
 *  Region names (from loadroms.h enum, stripped of REGION_):
 *    ROM1 = 68000 program
 *    ROM2 = Z80 sound
 *    GFX1 = tiles + sprites
 *    SMP1 = ADPCM samples
 *
 *  CRC values are placeholders (0x00000000).
 *  Replace with real values from the MAME romset:
 *    src/mame/sega/segas16b.cpp
 *
 *  The decrypted / bootleg sets are used here to avoid
 *  FD1094 decryption during initial development.
 * ========================================================= */

/* --- Golden Axe (World, bootleg / decrypted) --- */
static ROM_INFO rom_goldnaxe[] =
{
    /* 68000 program — two 27C020s, byte-interleaved (even/odd) */
    LOAD8_16(ROM1, "ga_pr_e.bin",  0x000000, 0x040000, 0x00000000),
    LOAD8_16(ROM1, "ga_pr_o.bin",  0x000001, 0x040000, 0x00000000),

    /* Z80 sound ROM */
    LOAD(ROM2,     "ga_sound.bin", 0x000000, 0x020000, 0x00000000),

    /* GFX — tiles + sprites, two interleaved pairs */
    LOAD8_16(GFX1, "ga_gfx0e.bin", 0x000000, 0x080000, 0x00000000),
    LOAD8_16(GFX1, "ga_gfx0o.bin", 0x000001, 0x080000, 0x00000000),
    LOAD8_16(GFX1, "ga_gfx1e.bin", 0x100000, 0x080000, 0x00000000),
    LOAD8_16(GFX1, "ga_gfx1o.bin", 0x100001, 0x080000, 0x00000000),

    /* UPD7759 ADPCM samples */
    LOAD(SMP1,     "ga_pcm.bin",   0x000000, 0x020000, 0x00000000),

    ROM_END
};

/* --- Altered Beast (World, bootleg) --- */
static ROM_INFO rom_altbeast[] =
{
    LOAD8_16(ROM1, "ab_pr_e.bin",  0x000000, 0x040000, 0x00000000),
    LOAD8_16(ROM1, "ab_pr_o.bin",  0x000001, 0x040000, 0x00000000),
    LOAD(ROM2,     "ab_sound.bin", 0x000000, 0x008000, 0x00000000),
    LOAD8_16(GFX1, "ab_gfx0e.bin", 0x000000, 0x080000, 0x00000000),
    LOAD8_16(GFX1, "ab_gfx0o.bin", 0x000001, 0x080000, 0x00000000),
    LOAD(SMP1,     "ab_pcm.bin",   0x000000, 0x020000, 0x00000000),
    ROM_END
};

/* --- Shinobi (World) --- */
static ROM_INFO rom_shinobi[] =
{
    LOAD8_16(ROM1, "sh_pr_e.bin",  0x000000, 0x020000, 0x00000000),
    LOAD8_16(ROM1, "sh_pr_o.bin",  0x000001, 0x020000, 0x00000000),
    LOAD(ROM2,     "sh_sound.bin", 0x000000, 0x008000, 0x00000000),
    LOAD8_16(GFX1, "sh_gfx0e.bin", 0x000000, 0x040000, 0x00000000),
    LOAD8_16(GFX1, "sh_gfx0o.bin", 0x000001, 0x040000, 0x00000000),
    ROM_END
};

/* =========================================================
 *  GAME DRIVER DECLARATIONS
 *
 *  GMEI(short_name, long_name, COMPANY_ID, year, flags, ...)
 *    Expands to:
 *      static DIR_INFO dir_xxx[] = { { "xxx" }, { NULL } };
 *      struct GAME_MAIN game_xxx = {
 *          .load_game = load_xxx,
 *          .exec      = execute_xxx,
 *          .sound     = sound_xxx,
 *          .video     = &video_xxx,
 *          .input     = input_xxx,
 *          .dsw       = dsw_xxx,
 *          .rom_list  = rom_xxx,
 *          ... };
 *
 *  CLNEI(short_name, parent, long_name, COMPANY_ID, year, flags)
 *    Same as GMEI but creates a clone dir entry pointing to
 *    the parent's directory.
 *
 *  All six function/struct pointers above must be defined
 *  before the GMEI macro is expanded.
 * ========================================================= */

GMEI( goldnaxe, "Golden Axe",    SEGA, 1989, GAME_BEAT );
GMEI( altbeast, "Altered Beast", SEGA, 1988, GAME_BEAT );
GMEI( shinobi,  "Shinobi",       SEGA, 1987, GAME_BEAT );

/*
 * Clone example — same hardware, different ROM revision:
 *
 * CLNEI( goldnaxea, goldnaxe,
 *        "Golden Axe (World, Rev A)", SEGA, 1989, GAME_BEAT );
 */

/* =========================================================
 *  NOTES FOR THE IMPLEMENTER
 *  -------------------------------------------------------
 *
 *  SUGGESTED DEVELOPMENT ORDER
 *  ----------------------------
 *  1. Fill in real CRC32 values in the ROM tables above.
 *     The MAME romset names and CRCs are the reference:
 *     src/mame/sega/segas16b.cpp
 *
 *  2. Verify the 68000 boots: add a debug printf in
 *     execute_goldnaxe() to print the PC after the first frame.
 *
 *  3. Implement palette: expand RGB444 -> RGB888 in
 *     sys16_paletteram_ww() and call Raine's palette helper
 *     (see palette.h).
 *
 *  4. Draw tiles: implement sys16_draw_tilemap() using Raine's
 *     tile blitter (scroll.h). Start with BG layer, no scroll.
 *
 *  5. Add scroll register support (already stubbed above).
 *
 *  6. Draw sprites: implement sys16_draw_sprites() using
 *     Raine's sprite routines (newspr.h).
 *
 *  7. Wire inputs: replace the 0xFFFF stubs in sys16_input_rw()
 *     and sys16_coin_rw() with real Raine input reads
 *     (control.h).
 *
 *  8. Add Z80 + YM2151 + UPD7759 sound.
 *
 *  9. Implement the 315-5195 banked ROM area ($100000-$1FFFFF).
 *     Each game has a different mapping table; refer to MAME's
 *     segas16b_state::memory_mapper_r/w for the logic.
 *
 * 10. For original (non-bootleg) PCBs: implement FD1094
 *     decryption. MAME's fd1094.cpp is the reference.
 *     The decryption key is stored per-game in a separate
 *     ROM file (usually named "317-xxxx.key").
 * ========================================================= */
