#include "gameinc.h"
#include "streams.h"
#include "m6502hlp.h"
#include "5220intf.h"
#include "pokey.h"
#include "2151intf.h"
#include "sasound.h"
#include "timer.h"
#include "profile.h"
#include "files.h"
#include "slapstic.h"
#include "emumain.h"

static struct ROM_INFO rom_gauntlet[] =
{
  LOAD_16_8( CPU1, "136037-1307.9a", 0x000000, 0x008000, 0x46fe8743),
  LOAD_16_8( CPU1, "136037-1308.9b", 0x000001, 0x008000, 0x276e15c4),
  LOAD_16_8( CPU1, "136037-205.10a", 0x038000, 0x004000, 0x6d99ed51),
  LOAD_16_8( CPU1, "136037-206.10b", 0x038001, 0x004000, 0x545ead91),
  LOAD_16_8( CPU1, "136037-1409.7a", 0x040000, 0x008000, 0x6fb8419c),
  LOAD_16_8( CPU1, "136037-1410.7b", 0x040001, 0x008000, 0x931bd2a0),
  LOAD( ROM2, "136037-120.16r", 0x004000, 0x004000, 0x6ee7f3cc),
  LOAD( ROM2, "136037-119.16s", 0x008000, 0x008000, 0xfa19861f),

  LOAD( GFX1, "136037-104.6p", 0x000000, 0x004000, 0x6c276a1d),

  LOAD( GFX2, "136037-111.1a", 0x000000, 0x008000, 0x91700f33),
  LOAD( GFX2, "136037-112.1b", 0x008000, 0x008000, 0x869330be),
  LOAD( GFX2, "136037-113.1l", 0x010000, 0x008000, 0xd497d0a8),
  LOAD( GFX2, "136037-114.1mn", 0x018000, 0x008000, 0x29ef9882),
  LOAD( GFX2, "136037-115.2a", 0x020000, 0x008000, 0x9510b898),
  LOAD( GFX2, "136037-116.2b", 0x028000, 0x008000, 0x11e0ac5b),
  LOAD( GFX2, "136037-117.2l", 0x030000, 0x008000, 0x29a5db41),
  LOAD( GFX2, "136037-118.2mn", 0x038000, 0x008000, 0x8bf3b263),

  // FILL( GFX3, 0, 0x40000, 0 ),

  LOAD( PROMS, "74s472-136037-101.7u", 0x000, 0x200, 0x2964f76f), /* MO timing */
  LOAD( PROMS, "74s472-136037-102.5l", 0x200, 0x200, 0x4d4fec6c), /* MO flip control */
  LOAD( PROMS, "74s287-136037-103.4r", 0x400, 0x100, 0x6c5ccf08), /* MO position/size */
  { NULL, 0, 0, 0, 0, 0 }
};

static UINT8 *eeprom,*playfield, *sprites, *alpha,*ram6502,last_speech_write,speech_val,*atarigen_slapstic_bank0;
static int eeprom_unlocked,atarigen_sound_to_cpu_ready,atarigen_sound_int_state,atarigen_video_int_state,atarigen_sound_to_cpu,sound_reset_val,
	   atarigen_cpu_to_sound,atarigen_cpu_to_sound_ready,atarigen_slapstic_bank,watchdog_counter;
static int layer_id_data[3];

#define ATARI_CLOCK_14MHz       14318180

static struct YM2151interface ym2151_interface =
{
        1,
        ATARI_CLOCK_14MHz/4,
        { YM3012_VOL(120,MIXER_PAN_LEFT,120,MIXER_PAN_RIGHT) },
        { 0 }
};

static struct POKEYinterface pokey_interface =
{
        1,
        ATARI_CLOCK_14MHz/8,
        { 85 },
};

static struct TMS5220interface tms5220_interface =
{
        ATARI_CLOCK_14MHz/2/11, /* potentially ATARI_CLOCK_14MHz/2/9 as well */
        255,
        0
};

static struct SOUND_INFO sound_gauntlet[] =
  {
    { SOUND_YM2151J, &ym2151_interface },
    { SOUND_POKEY, &pokey_interface },
    { SOUND_5220, &tms5220_interface },
    { 0,             NULL,                 },
  };

// The eeprom is always accessed by byte, every 2 bytes, at least for gauntlet
// weird thing : it reports 16 errors (I guess it's eeprom errors) in the service mode the 1st time the eeprom is initialized
// I couldn't detect anything wrong and after this the error can be cleared in service mode and never seem to change after that... !
static UINT8 read_eeprom(UINT32 offset) {
    offset = (offset & 0xfff) >> 1;
    return eeprom[offset];
}

static void write_eeprom(UINT32 offset, UINT8 data) {
    if (!eeprom_unlocked) return;
    offset = (offset & 0xfff) >> 1;
    eeprom[offset] = data;
    eeprom_unlocked = 0;
}

static void update_interrupts() {
    if (atarigen_video_int_state) {
	cpu_interrupt(CPU_68K_0, 4);
	atarigen_video_int_state = 0;
	if (atarigen_sound_int_state) cpu_execute_cycles(CPU_68K_0,100);
    }
    if (atarigen_sound_int_state) {
	    cpu_interrupt(CPU_68K_0, 6);
	    atarigen_sound_int_state = 0;
    }
}

static UINT16 atarigen_sound_r(UINT32 offset) {
    atarigen_sound_to_cpu_ready = 0;
    atarigen_sound_int_state = 0;
    printf("atarigen_sound_r read %x from %x\n",atarigen_sound_to_cpu,s68000_read_pc);
    input_buffer[8] &= ~0x10;
    return atarigen_sound_to_cpu;
}

static int made_reset;

static void sound_reset_w(UINT32 offset, UINT16 data) {
    int oldword = sound_reset_val;
    sound_reset_val = data;
    if ((sound_reset_val ^ oldword) & 1) {
	print_debug("reset 6502 from 68k val,old %d %d pc %x sr %x\n",sound_reset_val,oldword,s68000readPC(),s68000_sr);
	printf("reset 6502 from 68k val,old %d %d pc %x sr %x\n",sound_reset_val,oldword,s68000readPC(),s68000_sr);
	atarigen_sound_to_cpu_ready = 0;
	atarigen_sound_int_state = 0;
	input_buffer[8] &= ~0x10;
	if (!(sound_reset_val & 1)) {
	    // cpu_reset(CPU_M6502_0);
	    made_reset = 1;
	}
    }
}

static void atarigen_video_int_ack_w(UINT32 offset, UINT16 data) {
    atarigen_video_int_state = 0;
}

static void eeprom_enable_w(UINT32 offset, UINT16 data) {
    eeprom_unlocked = 1;
}

static void atarigen_sound_w_byte(UINT32 offset, UINT16 data) {
    atarigen_cpu_to_sound = data;
    atarigen_cpu_to_sound_ready = 1;
    print_debug("sound_w %x\n",data);
    printf("sound_w %x\n",data);
    if (made_reset) {
	// Crazy sync code for the 6502, the reset code expects that a command byte is ready to read at the very beginning
	// so the best way to be sure it happens is to reset when the command is sent !
	cpu_reset(CPU_M6502_0);
	cpu_execute_cycles(CPU_M6502_0,10);
	made_reset = 0;
    }
    cpu_int_nmi(CPU_M6502_0);
    cpu_execute_cycles(CPU_M6502_0,1000);
}

static void swap_mem(void *ptr1, void *ptr2, int bytes)
{
    UINT8 *p1 = ptr1;
    UINT8 *p2 = ptr2;
    while (bytes--)
    {
	int temp = *p1;
	*p1++ = *p2;
	*p2++ = temp;
    }
}

static void m6502_sound_w(UINT32 offset, UINT8 data) {
    print_debug("*** m6502_sound_w %x,%x\n",offset,data);
    printf("*** m6502_sound_w %x,%x from %x\n",offset,data,cpu_get_pc(CPU_M6502_0));
    if (atarigen_sound_to_cpu_ready) {
	print_debug("Missed result from 6502\n");
    }

    /* set up the states and signal the sound interrupt to the main CPU */
    atarigen_sound_to_cpu = data;
    atarigen_sound_to_cpu_ready = 1;
    input_buffer[8] |= 0x10;
    atarigen_sound_int_state = 1;
    update_interrupts();
}

static UINT8 m6502_sound_r(UINT32 offset) {
    atarigen_cpu_to_sound_ready = 0;
    print_debug("6502 read %x pc %x\n",atarigen_cpu_to_sound,cpu_get_pc(CPU_M6502_0));
    printf("6502 read %x pc %x\n",atarigen_cpu_to_sound,cpu_get_pc(CPU_M6502_0));
    return atarigen_cpu_to_sound;
}

void atarigen_set_vol(int volume, const char *string)
{
        int ch;

        for (ch = 0; ch < MAX_STREAM_CHANNELS; ch++)
        {
                const char *name = stream_get_name(ch);
                if (name && strstr(name, string))
                        stream_set_volume(ch, volume);
        }
}

void atarigen_set_ym2151_vol(int volume)
{
        atarigen_set_vol(volume, "2151");
}

void atarigen_set_pokey_vol(int volume)
{
        atarigen_set_vol(volume, "POKEY");
}

void atarigen_set_tms5220_vol(int volume)
{
        atarigen_set_vol(volume, "5220");
}

static UINT8 input_port_5_r(UINT32 offset) {
    return input_buffer[10];
}

static void mixer_w(UINT32 offset, UINT8 data) {
    // Since the volumes are applied directly to the streams, it overwrites the volumes in the sound interface structs
    // so I try to keep the ratio to max volume there was in the initial structs...
    atarigen_set_ym2151_vol((data & 7) * 128 / 7); // 50%
    atarigen_set_pokey_vol(((data >> 3) & 3) * 255/3 / 3); // 1/3
    atarigen_set_tms5220_vol(((data >> 5) & 7) * 204 / 7); // 80%
}

static UINT8 switch_6502_r(UINT32 offset)
{
        int temp = 0x30;

        if (atarigen_cpu_to_sound_ready) temp ^= 0x80;
        if (atarigen_sound_to_cpu_ready) temp ^= 0x40;
        if (tms5220_ready_r()) temp ^= 0x20;
        if (!(input_buffer[8] & 8)) temp ^= 0x10;
	// print_debug("*** switch_6502_r %x from %x\n",temp,cpu_get_pc(CPU_M6502_0));
	// printf("*** switch_6502_r %x from %x\n",temp,cpu_get_pc(CPU_M6502_0));

        return temp;
}

static void sound_ctl_w(UINT32 offset, UINT8 data)
{
    switch (offset & 7)
    {
    case 0: /* music reset, bit D7, low reset */
	break;

    case 1: /* speech write, bit D7, active low */
	if (((data ^ last_speech_write) & 0x80) && (data & 0x80))
	    tms5220_data_w(0, speech_val);
	last_speech_write = data;
	break;

    case 2: /* speech reset, bit D7, active low */
	if (((data ^ last_speech_write) & 0x80) && (data & 0x80))
	    tms5220_reset();
	break;

    case 3: /* speech squeak, bit D7 */
	data = 5 | ((data >> 6) & 2);
	tms5220_set_frequency(ATARI_CLOCK_14MHz/2 / (16 - data));
	break;
    }
}

static void tms5220_w(UINT32 offset, UINT8 data) {
    speech_val = data;
}

static INLINE void update_bank(int bank)
{
        /* if the bank has changed, copy the memory; Pit Fighter needs this */
        if (bank != atarigen_slapstic_bank)
        {
                /* bank 0 comes from the copy we made earlier */
                if (bank == 0)
                        memcpy(&ROM[0x38000], atarigen_slapstic_bank0, 0x2000);
                else
                        memcpy(&ROM[0x38000], &ROM[0x38000 + bank * 0x2000], 0x2000);

                /* remember the current bank */
                atarigen_slapstic_bank = bank;
        }
}

static void slapstic_ww(UINT32 offset,UINT16 data)
{
        update_bank(slapstic_tweak((offset-0x38000)/2));
}

static void slapstic_wb(UINT32 offset,UINT16 data)
{
    printf("slapstic_wb ?\n");
    exit(1);
}

static UINT8 slapstic_rb(UINT32 offset) {
    // Adapted from the word version
    UINT8 result = ROM[0x38000 + ((offset & 0x1fff) ^ 1)];
    update_bank(slapstic_tweak((offset-0x38000)/2));
    return result;
}

static UINT16 slapstic_rw(UINT32 offset)
{
        /* fetch the result from the current bank first */
        UINT16 result = ReadWord(&ROM[0x38000 + (offset & 0x1fff)]);

        /* then determine the new one */
        update_bank(slapstic_tweak((offset-0x38000)/2));
        return result;
}

static void watchdog_w(UINT32 offset, UINT16 data) {
    watchdog_counter = 180;
}

static UINT16 read_port_4(UINT32 offset) {
    return ReadWord(&input_buffer[8]);
}

static UINT16 read_port_4b(UINT32 offset) {
    return input_buffer[8];
}

static void load_gauntlet() {
    // memcpy(load_region[REGION_GFX3],load_region[REGION_GFX2],get_region_size(REGION_GFX2));

    RAMSize = 0x2000 + // main ram
	0x1000 + // 2nd ram
	0x2000 + // playfield
	0x2000 + // sprites
	0x1000 + // alpha
	0x800 + // palette
	0x10000 + // ram 6502
	2; // xscroll
    if(!(RAM=AllocateMem(RAMSize))) return;
    memset(RAM,0,RAMSize);
    setup_z80_frame(CPU_M6502_0,ATARI_CLOCK_14MHz/8/fps);
    eeprom = AllocateMem(0x800);
    if (!eeprom) return;
    memset(eeprom,0xff,0x800);
    load_file(get_shared("savedata/gauntlet.epr"),eeprom,0x800);
    playfield = RAM + 0x3000;
    sprites = playfield + 0x2000;
    alpha = sprites + 0x2000;
    RAM_PAL = alpha + 0x1000;
    ram6502 = RAM_PAL + 0x800;
    InitPaletteMap(RAM_PAL, 0x40*4, 0x4, 0x8000); // banks of 4 colors only because of alpha !
    set_colour_mapper(&col_map_nnnn_rrrr_gggg_bbbb_atari);
    // xor the sprites of this region...
    UINT16 *p = (UINT16*)load_region[REGION_GFX2];
    int s = get_region_size(REGION_GFX2)/2-1;
    do {
	p[s--] ^= 0xffff;
    } while (s >= 0);


    ByteSwap(ROM, get_region_size(REGION_ROM1) );
    swap_mem(ROM + 0x000000, ROM + 0x008000, 0x8000);
    swap_mem(ROM + 0x040000, ROM + 0x048000, 0x8000);
    // swap_mem(ROM + 0x050000, ROM + 0x058000, 0x8000);
    // swap_mem(ROM + 0x060000, ROM + 0x068000, 0x8000);
    // swap_mem(ROM + 0x070000, ROM + 0x078000, 0x8000);

    atarigen_slapstic_bank0 = AllocateMem(0x2000);
    save_debug("rom",ROM,0x50000,0);
    if (atarigen_slapstic_bank0)
	memcpy(atarigen_slapstic_bank0, &ROM[0x38000], 0x2000);
    slapstic_init(104);
    slapstic_reset();
    update_bank(slapstic_bank());

    AddReadByte(0x38000, 0x38000 + 0x7fff, slapstic_rb, NULL);
    AddReadWord(0x38000, 0x38000 + 0x7fff, slapstic_rw, NULL);
    AddWriteWord(0x38000, 0x38000 + 0x7fff, slapstic_ww, NULL);
    AddWriteByte(0x38000, 0x38000 + 0x7fff, slapstic_wb, NULL);
    add_68000_rom(0, 0, get_region_size(REGION_ROM1)-1, ROM);
    add_68000_ram(0,0x800000,0x801fff, RAM);
    add_68000_ram(0,0x900000,0x901fff, playfield);
    add_68000_ram(0,0x902000,0x903fff, sprites);
    add_68000_ram(0,0x904000,0x904fff, RAM+0x2000);
    add_68000_ram(0,0x905000,0x905fff, alpha);
    add_68000_ram(0,0x910000,0x9107ff, RAM_PAL);
    add_68000_ram(0,0x930000,0x930001, ram6502+0x10000); // xscroll
    AddReadByte(0x802000,0x802fff, read_eeprom, NULL);
    AddWriteByte(0x802000, 0x802fff, write_eeprom, NULL);
    AddReadWord(0x803008,0x803009,read_port_4,NULL);
    AddReadByte(0x803008,0x803009,read_port_4b,NULL);
    AddReadBW(0x803000, 0x803009, NULL, input_buffer);
    AddReadBW(0x80300e, 0x80300f, atarigen_sound_r, NULL);
    AddWriteWord(0x803100,0x803101,watchdog_w, NULL);
    AddWriteBW(0x80312e, 0x80312f, sound_reset_w, NULL);
    AddWriteBW(0x803140, 0x803141, atarigen_video_int_ack_w, NULL);
    AddWriteBW(0x803150, 0x803151, eeprom_enable_w, NULL);
    AddWriteWord(0x803170, 0x803171, atarigen_sound_w_byte, NULL);

    finish_conf_68000(0);
    // m68k_set_cpu_type(M68K_CPU_TYPE_68010);
    // m68k_get_context(&M68000_context[0]);

#ifdef MAME_6502
    m6502_init();
#endif
    AddM6502AROMBase(ram6502);
    memcpy(ram6502+0x4000, Z80ROM+0x4000, 0xc000);
    AddM6502AWrite(0,0xfff,NULL,ram6502);
    AddM6502AWrite(0x1000,0x100f,m6502_sound_w,NULL);
    AddM6502ARead(0x1010,0x101f,m6502_sound_r,NULL);
    AddM6502ARead(0x1020,0x102f,input_port_5_r,NULL); // really ?
    AddM6502AWrite(0x1020,0x102f,mixer_w,NULL);
    AddM6502ARead(0x1030,0x103f,switch_6502_r,NULL);
    AddM6502AWrite(0x1030,0x103f,sound_ctl_w,NULL);
    AddM6502ARead(0x1800,0x180f, pokey1_r, NULL);
    AddM6502AWrite(0x1800,0x180f, pokey1_w, NULL);
    AddM6502AWrite(0x1810,0x1811, YM2151_word_0_w, NULL);
    AddM6502AWrite(0x1820,0x182f, tms5220_w, NULL);
    // 1830-183f 6502_irq_ack_w / irq_ack_r
    AddM6502ARead(0x1811,0x1811,  YM2151_status_port_0_r, NULL);
    AddM6502AWrite(0,0xffff,DefBadWriteM6502,NULL);
    AddM6502ARead( 0x1040,0x3fff,DefBadReadM6502, NULL);
    AddM6502ARead(     -1,    -1, NULL, NULL);
    AddM6502AWrite(    -1,    -1, NULL, NULL);
    AddM6502AInit();

    last_speech_write = 0x80;
    sound_reset_val = 1;
    eeprom_unlocked = 0;
    atarigen_video_int_state = atarigen_sound_int_state = 0;

    layer_id_data[0] = add_layer_info("playfield");
    layer_id_data[1] = add_layer_info("alpha");
    layer_id_data[2] = add_layer_info("sprites");

    // WriteWord(&ROM[0x47ee],1); // huge acceleration of the boot, just remove some useless loop...
    // WriteWord(&ROM[0x78a],0x6062); // disable rom check, produces annoying error message, particularly in service mode where it waits for a vbl without irq... !
    // Wondering if this huge loop is not for the 6502 to finish some long init ? But it receives 0 command before this loop, and so it's stuck in its infinite loop waiting...
    watchdog_counter = 180;
    atarigen_cpu_to_sound = atarigen_sound_to_cpu = 0;
    atarigen_cpu_to_sound_ready = atarigen_sound_to_cpu_ready = 0;
    // ram6502[0x59a5] = 0xea;
    // ram6502[0x59a6] = 0xf7;
    // 40184 : increment a counter inside the vbl, if the counter reaches $40, the program makes suicide !
    // looks like an alternate watchdog. It's related to the slapstic (verified).
    // The counter is at $904002
}

static void clear_gauntlet() {
    save_file(get_shared("savedata/gauntlet.epr"),eeprom,0x800);
}

extern int goto_debuger;

static void execute_gauntlet()
{
    input_buffer[8] |= 0x40;
#if 0
    if (((s68000_sr >> 8) & 7) == 4) {
	printf("level still 4 at frame %d pc %x\n",cpu_frame_count,s68000_pc);
    }
#endif
    // cpu_execute_cycles(CPU_68K_0, ATARI_CLOCK_14MHz/8/fps); // 68010
    if (goto_debuger) return;
    for (int n=0; n<4; n++) {
	int frame = ATARI_CLOCK_14MHz/8/fps/4,diff;
	if (n == 3) {
	    input_buffer[8] &= ~0x40;
	    atarigen_video_int_state = 1;
	    update_interrupts();
	}
	while (frame > 16) {
	    diff = execute_one_z80_audio_frame(frame);
	    if (s68000_read_pc == 0x46c4 || s68000_read_pc == 0x46ca || s68000_read_pc == 0x4694 || s68000_read_pc == 0x148 || s68000_read_pc == 0x46b8 || s68000_read_pc == 0x468c || s68000_pc == 0x4244) {
		int n=0;
		int cycles = diff*4;
		while (s68000_read_pc != 0x10000 && cycles > 0) {
		    int pc = s68000_read_pc;
		    int cyc = 10;
		    do {
			cpu_execute_cycles(CPU_68K_0,cyc);
			cyc *= 2;
		    } while (s68000_read_pc == pc && cyc < 1000);
		    printf("%d: %x (cycles %d, sr=%x)\n",n++,s68000_read_pc,cyc,s68000_sr);
		    cycles -= 128;
		}
	    } else
		cpu_execute_cycles(CPU_68K_0, diff*4); // 68010
	    printf("68k pc %x\n",s68000_read_pc);
	    frame -= diff;
	    if (diff == 0) break;
	}
	cpu_interrupt(CPU_M6502_0,1);
    }
}

static struct INPUT_INFO input_gauntlet[] =
{
  INP0( P1_B2, 0x00, 0x01),
  INP0( P1_B1, 0x00, 0x02),
  INP0( P1_B3, 0x00, 0x04),
  INP0( P1_B4, 0x00, 0x08),
  INP0( P1_RIGHT, 0x00, 0x10),
  INP0( P1_LEFT, 0x00, 0x20),
  INP0( P1_DOWN, 0x00, 0x40),
  INP0( P1_UP, 0x00, 0x80),

  INP0( P2_B2, 0x02, 0x01),
  INP0( P2_B1, 0x02, 0x02),
  INP0( UNUSED, 2, 0xc),
  INP0( P2_RIGHT, 0x02, 0x10),
  INP0( P2_LEFT, 0x02, 0x20),
  INP0( P2_DOWN, 0x02, 0x40),
  INP0( P2_UP, 0x02, 0x80),

  INP0( P3_B2, 0x04, 0x01),
  INP0( P3_B1, 0x04, 0x02),
  INP0( UNUSED, 4, 0xc),
  INP0( P3_RIGHT, 0x04, 0x10),
  INP0( P3_LEFT, 0x04, 0x20),
  INP0( P3_DOWN, 0x04, 0x40),
  INP0( P3_UP, 0x04, 0x80),

  INP0( P4_B2, 0x06, 0x01),
  INP0( P4_B1, 0x06, 0x02),
  INP0( UNUSED, 6, 0xc),
  INP0( P4_RIGHT, 0x06, 0x10),
  INP0( P4_LEFT, 0x06, 0x20),
  INP0( P4_DOWN, 0x06, 0x40),
  INP0( P4_UP, 0x06, 0x80),

  INP1( UNUSED,  0x08, 0x07),
  INP1( UNUSED,  0x08, 0x30), // ?
  INP0( UNUSED,  0x08, 0x40), // vblank
  INP1( UNUSED,  0x08, 0x80),

  INP0( COIN4, 0x0a, 0x01),
  INP0( COIN3, 0x0a, 0x02),
  INP0( COIN2, 0x0a, 0x04),
  INP0( COIN1, 0x0a, 0x08),
  INP0( UNUSED,0x0a, 0xf0),
  END_INPUT
};

static struct DSW_DATA dsw_data_gauntlet[] =
{
    DSW_SERVICE(0,8),
  { NULL, 0}
};

static struct DSW_INFO dsw_gauntlet[] =
{
  { 8, 0x8, dsw_data_gauntlet },
  { 0, 0, NULL }
};

static gfx_layout anlayout =
{
	8,8,
	RGN_FRAC(1,1),
	2,
	{ 0, 4 },
	{ 0, 1, 2, 3, 8, 9, 10, 11 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16 },
	8*16
};

static gfx_layout pfmolayout =
{
	8,8,
	RGN_FRAC(1,4),
	4,
	{ RGN_FRAC(3,4), RGN_FRAC(2,4), RGN_FRAC(1,4), RGN_FRAC(0,4) },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};

static struct GFX_LIST gauntlet_gfx[] =
{
    {	REGION_GFX1, &anlayout, }, // 0, 64
    {	REGION_GFX2, &pfmolayout, }, // 256, 32
   { 0,           NULL,             },
};

static char visited[1024];

static void render_sprites(int n,int xscroll,int yscroll,int miny, int maxy) {
    /* The sprites are a bizarre bunch, as shown by the memory map in mame :
     * 902000-903FFF   R/W   xxxxxxxx xxxxxxxx   Motion object RAM (1024 entries x 4 words)
     R/W   -xxxxxxx xxxxxxxx      (0: Tile index)
     R/W   xxxxxxxx x-------      (1024: X position)
     R/W   -------- ----xxxx      (1024: Palette select)
     R/W   xxxxxxxx x-------      (2048: Y position)
     R/W   -------- -x------      (2048: Horizontal flip)
     R/W   -------- --xxx---      (2048: Number of X tiles - 1)
     R/W   -------- -----xxx      (2048: Number of Y tiles - 1)
     R/W   ------xx xxxxxxxx      (3072: Link to next object)
     */
    UINT8 *map;
    int nb = 0;
    while (!visited[n]) {
	visited[n] = 1;
	nb++;
	UINT16 code = (ReadWord(&sprites[n*2]) ^ 0x800);
	UINT16 link = ReadWord(&sprites[(n*2)+3072*2]) & 0x3ff;
	code &= 0x1fff;
	int x = ReadWord(&sprites[(n*2)+1024*2]);
	UINT16 color = 0x40 + (x & 0xf)*4;
	x >>= 7;
	int y = ReadWord(&sprites[(n*2)+2048*2]);
	int flipx = (y & 0x40);
	int nx = (y >> 3) & 7;
	int ny = (y & 7);
	y >>= 7;
	// The y is coded inverted !!! Crazy old hardware... !!!
	y = -y;
	x -= xscroll; y -= yscroll;
	y -= (ny+1)*8;
	x &= 0x1ff; y &= 0x1ff;
	if (x > 335) x -= 512;
	if (y > 239) y -= 512;
	MAP_PALETTE_MAPPED_NEW(color+3,4,map);
	MAP_PALETTE_MAPPED_NEW(color+2,4,map);
	MAP_PALETTE_MAPPED_NEW(color+1,4,map);
	MAP_PALETTE_MAPPED_NEW(color,4,map);
	for (int cy=0; cy <= ny; cy++) {
	    int px,py;
	    py = y+16+cy*8;
	    if (py < miny-7) continue;
	    if (py > maxy) break;
	    for (int cx=0; cx <= nx; cx++) {
		px = (x+16+cx*8);
		if (px > 8 && px < 336+16 && gfx_solid[1]) {
		    if (flipx)
			Draw8x8_Trans_Mapped_FlipY_Rot(&gfx[1][(code+cx+cy*(nx+1))<<6],px,py,map);
		    else
			Draw8x8_Trans_Mapped_Rot(&gfx[1][(code+cx+cy*(nx+1))<<6],px,py,map);
		}
	    }
	}
	n = link;
    }
}

static void draw_gauntlet()
{
    ClearPaletteMap();

    printf("sizes %x,%x\n",get_region_size(REGION_GFX1),get_region_size(REGION_GFX2));
    int xscroll, yscroll, ta, code, color;
    SCROLL_REGS;
    UINT8 *map;
    int bank,size;
    static int bank_mask;
    if (!bank_mask) {
	size = get_region_size(REGION_GFX2);
	bank_mask = (size>>6)-1;
    }

    yscroll = ReadWord(&alpha[0xf6e]);
    xscroll = ReadWord(ram6502+0x10000) & 0x1ff;
    int tilebank = yscroll & 3;
    yscroll >>= 7;
    yscroll &= 0x1ff;
    // if (bank < 0x40 || bank > 0x80) bank = 0x40;
    bank = 0x80+0x20;

    if( check_layer_enabled(layer_id_data[0])) {
	MAKE_SCROLL_512x512_2_8_YX(yscroll,xscroll);
	START_SCROLL_8_YX(16,16,336,240) {
	    ta = ReadWord(&playfield[zz]);
	    code = (((tilebank*0x1000) & bank_mask) + (ta & 0xfff)) ^ 0x800;
	    color = (bank)+ (((ta >> 12) & 7)*4);
	    // Oh well, this layer has 16 colors when the other one has 4 only, I could maybe use the multi_mapped_new macro
	    // but it obliges to count in banks of 16 colors which could create problems, I'll just use 4 map calls, anyway the palette is mapped only once
	    MAP_PALETTE_MAPPED_NEW(
		    color+3,
		    4,
		    map);
	    MAP_PALETTE_MAPPED_NEW(
		    color+2,
		    4,
		    map);
	    MAP_PALETTE_MAPPED_NEW(
		    color+1,
		    4,
		    map);
	    MAP_PALETTE_MAPPED_NEW(
		    color,
		    4,
		    map);
	    if (ta >> 15)
		Draw8x8_Mapped_FlipY_Rot(&gfx[1][code<<6],x,y,map);
	    else
		Draw8x8_Mapped_Rot(&gfx[1][code<<6],x,y,map);
	}
	END_SCROLL_512x512_2_8();
    }

    if( check_layer_enabled(layer_id_data[2])) {
	for (int ta=0xf80; ta<=0xfff; ta+=2) {
	    // Not only does these addresses give the start index for the sprites, they also indicate miny and maxy !
	    // each word is for 8 pixels high, so we stop when we reach the max screen coordinate (240)
	    int miny = (ta-0xf80)/2*8+8;
	    if (miny >= 240) break;
	    int maxy = miny+7;
	    memset(visited,0,1024);
	    int link = ReadWord(&alpha[ta]) & 0x3ff;
	    render_sprites(link,xscroll,yscroll,miny+16,maxy+16);
	}
    }

    if( check_layer_enabled(layer_id_data[1])) {
	MAKE_SCROLL_512x256_2_8(0,0);
	START_SCROLL_512x256_2_8(16,16,336,240) {
	    ta = ReadWord(&alpha[zz]);
	    code = ta & 0x3ff;
	    MAP_PALETTE_MULTI_MAPPED_NEW(
		    (((ta >> 10) & 0xf) | ((ta >> 9) & 0x20)),
		    4,
		    map);
	    if (ta & 0x8000) // opaque
		Draw8x8_Mapped_Rot(&gfx[0][code<<6],x,y,map);
	    else
		Draw8x8_Trans_Mapped_Rot(&gfx[0][code<<6],x,y,map);
	}
	END_SCROLL_512x256_2_8();
    }
}

static struct VIDEO_INFO video_gauntlet =
{
   draw_gauntlet,
   336,
   240,
   16,
   VIDEO_ROTATE_NORMAL |
   VIDEO_ROTATABLE,
   gauntlet_gfx,
   60
};

GMEI( gauntlet,"Gauntlet (rev 14)",ATARI,1985, GAME_MISC,
	.clear= clear_gauntlet);
