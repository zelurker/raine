/* Comparable to pengo/pacman hardware, quite a lot of tiny hacks everywhere !
 * The most incredible one would certainly be the sound chip which is a kind of 4 bit dsp and which runs its own code, and whose only purpose is to make the explosion sound !
 * Just after that we have the namcoio chip, responsible for input/output, and accessing the namco54 sound chip too, a crazy chip which is supposed to generate a nmi to the main
 * cpu every 200 us !
 * And then we have the 3 z80 with exactly the same memory map which all share some ram and which are supposed to run 100 slices / frame to remain in sync, they finally work here with only
 * 35 slices / frame, which is already quite a lot, see comments in execute_galaga.
 * Plus of course some weird setup for the video hardware (36*8 x 28*8 resolution, not a power of 2 and so it creates havoc for the usual layer handling functions), a weird palette too
 * only 32 base colors, but then 256 color entries for text and 256 color entries for sprites ! Why would it need so many entries for such a small palette ? (and half these color entries are unused).
 * And then the stars, whose emulation comes from mame, but are drawn 1st here instead of last ! */

#include "gameinc.h"
#include "sound/namco.h"
#include "mame/handlers.h"
#include "savegame.h"
#include "emumain.h"
#include "blit.h"
#include "sasound.h"
#include "dxsmp.h"
#include "emudx.h"

extern UINT32 cpu_frame_count;

// SHARE_HANDLERS : if defined use handlers which to access shared memory which try to be clever and to pass control to the next z80 if they know it's currently stuck waiting for something
// The handlers approach is probably better, and it's certainly more handy to debug, but on modern hardware it probably doesn't make a big difference and it makes things harder for the console
// (cheat scripts) and the hiscores. I changed the hiscore code to work around that for now, I keep this define here, will need more testing...
// comment it out to revert to a more direct and more basic approach (they are just trying to optimize things, it doesn't change anything to the logic of the driver)
#ifdef MAME_Z80
// This thing can't work with mz80, at least for now, because of the rombase :
// the stack is installed in the middle of the shared ram, and mz80 uses shortcuts to execute its code, it expects to be able to push on the stack directly using the rombase as reference.
// So if the stack can be accessed only through handlers, then it crashes miserably, just after the hardware test (frozen screen with some weird graphics).
// There is a boolean in mz80 which is supposed to allow it to work in such a configuration, bThroughCallHandler. I tested it, and it failed exactly the same way, so there must be bugs in
// the implementation. It would take too much time to fix that, the easiest solution is to allow these handlers only with the mame z80 cpu core !
// #define SHARE_HANDLERS
// Actually an optimized build seems slightly faster without the handlers so I'll leave it commented from now on, I'll just keep the code for reference... !
#endif

// DEBUG: print any rw access to the 3 shared ram areas
// #define DEBUG

#ifdef DEBUG
#define LOG printf
#else
#define LOG(...)
#endif

extern struct dxsmpinterface galax_emudx_interface;

static struct namco_interface namco_interface =
{
	3072000/32, // ?!!
	3,			/* number of voices */
	255,		/* playback volume */
	REGION_SOUND1	/* memory region */
};

static struct SOUND_INFO sound_galaga_dx[] =
  {
  { SOUND_DXSMP, &galax_emudx_interface, },
   { SOUND_NAMCO, &namco_interface },
   { 0,             NULL,                 },
  };

static struct SOUND_INFO sound_galaga[] =
  {
   { SOUND_NAMCO, &namco_interface },
   { 0,             NULL,                 },
  };

enum sample_method {
    none,
    emudx,
    wav,
    mp3,
    ogg
};

static int sample = none;

static int nmi_enable;

static struct ROM_INFO rom_galaga[] =
{
  LOAD( ROM1, "gg1-1b.3p", 0x0000, 0x1000, 0xab036c9f),
  LOAD( ROM1, "gg1-2b.3m", 0x1000, 0x1000, 0xd9232240),
  LOAD( ROM1, "gg1-3.2m", 0x2000, 0x1000, 0x753ce503),
  LOAD( ROM1, "gg1-4b.2l", 0x3000, 0x1000, 0x499fcc76),
  LOAD( ROM2, "gg1-5b.3f", 0x0000, 0x1000, 0xbb5caae3),
  LOAD( CPU3, "gg1-7b.2c", 0x0000, 0x1000, 0xd016686b),
  LOAD( GFX1, "gg1-9.4l", 0x0000, 0x1000, 0x58b2f47c),
  LOAD( GFX2, "gg1-11.4d", 0x0000, 0x1000, 0xad447c80),
  LOAD( GFX2, "gg1-10.4f", 0x1000, 0x1000, 0xdd6f1afc),
  LOAD( PROMS, "prom-5.5n", 0x0000, 0x0020, 0x54603c6b), /* palette */
  LOAD( PROMS, "prom-4.2n", 0x0020, 0x0100, 0x59b6edab), /* char lookup table */
  LOAD( PROMS, "prom-3.1c", 0x0120, 0x0100, 0x4a04bb6b), /* sprite lookup table */
  LOAD( SMP1, "prom-1.1d", 0x0000, 0x0100, 0x7a2815b4),
  LOAD( SMP1, "prom-2.5c", 0x0100, 0x0100, 0x77245b66), /* timing - not used */
  { NULL, 0, 0, 0, 0, 0 }
};

static struct INPUT_INFO input_galaga[] =
{
  INP0( P1_B1, 0x00, 0x01),
  INP0( P1_B1, 0x00, 0x02),
  INP0( P1_START, 0x00, 0x04),
  INP0( P2_START, 0x00, 0x08),
  INP0( COIN1, 0x00, 0x10),
  INP0( COIN2, 0x00, 0x20),
  INP0( SERVICE, 0x00, 0x40),
  INP0( TEST, 0x00, 0x80),

  INP0( P1_RIGHT, 0x02, 0x02),
  INP0( P1_LEFT, 0x02, 0x08),
  INP0( P1_RIGHT, 0x02, 0x20), // cocktail
  INP0( P1_LEFT, 0x02, 0x80), // cocktail
  END_INPUT
};

static struct DSW_DATA dsw_data_galaga_2[] = // dswa
{
  { MSG_DIFFICULTY, 0x03, 4 },
  { MSG_EASY, 0x03, 0x00 },
  { MSG_MEDIUM, 0x00, 0x00 },
  { MSG_HARD, 0x01, 0x00 },
  { MSG_HARDEST, 0x02, 0x00 },
  { MSG_UNKNOWN, 0x04, 2 },
  { MSG_OFF, 0x04, 0x00 },
  { MSG_ON, 0x00, 0x00 },
  DSW_DEMO_SOUND( 0x00, 0x08 ),
  { "Freeze", 0x10, 2 },
  { MSG_OFF, 0x10, 0x00 },
  { MSG_ON, 0x00, 0x00 },
  DSW_TEST_MODE( 0x00, 0x20 ),
  { MSG_UNKNOWN, 0x40, 2 },
  { MSG_OFF, 0x40, 0x00 },
  { MSG_ON, 0x00, 0x00 },
  { MSG_CABINET, 0x80, 2 },
  { MSG_UPRIGHT, 0x80, 0x00 },
  { MSG_TABLE, 0x00, 0x00 },
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_data_galaga_3[] = // dswb
{
  { MSG_COINAGE, 0x07, 8 },
  { MSG_4COIN_1PLAY, 0x04, 0x00 },
  { MSG_3COIN_1PLAY, 0x02, 0x00 },
  { MSG_2COIN_1PLAY, 0x06, 0x00 },
  { MSG_1COIN_1PLAY, 0x07, 0x00 },
  { MSG_2COIN_3PLAY, 0x01, 0x00 },
  { MSG_1COIN_2PLAY, 0x03, 0x00 },
  { MSG_1COIN_3PLAY, 0x05, 0x00 },
  { MSG_FREE_PLAY, 0x00, 0x00 },
  { MSG_EXTRA_LIFE, 0x38, 8 },
  { "20K 60K 60K", 0x20, 0x00 },
  { "20K 60K", 0x18, 0x00 },
  { "20K 70K 70K", 0x10, 0x00 },
  { "20K 80K 80K", 0x30, 0x00 },
  { "30K 80K", 0x38, 0x00 },
  { "30K 100K 100K", 0x08, 0x00 },
  { "30K 120K 120K", 0x28, 0x00 },
  { "None", 0x00, 0x00 },
  { MSG_LIVES, 0xc0, 4 },
  { "2", 0x00, 0x00 },
  { "3", 0x80, 0x00 },
  { "4", 0x40, 0x00 },
  { "5", 0xc0, 0x00 },
  { NULL, 0, 0}
};

static struct DSW_INFO dsw_galaga[] =
{
  { 0x4, 0xf7, dsw_data_galaga_2 },
  { 0x6, 0x97, dsw_data_galaga_3 },
  { 0, 0, NULL }
};

static UINT8 bosco_dsw_r(UINT32 offset)
{
        int bit0,bit1;
	offset &= 7;
	LOG("bosco_dsw_r %x\n",offset);

        bit0 = (input_buffer[2*3] >> offset) & 1;
        bit1 = (input_buffer[2*2] >> offset) & 1;

        return bit0 | (bit1 << 1);
}

static int irq_enabled[3];

static void bosco_latch_w(UINT32 offset, UINT8 data)
{
    offset &= 7;
	int bit = data & 1;

	switch (offset)
	{
		case 0x00:	/* IRQ1 */
		case 0x01:
		    LOG("irq_enabled %d = %d from %x\n",offset,bit,z80pc);
			irq_enabled[offset] = bit;
			break;

		case 0x02:	/* NMION */ // to be confirmed
		    LOG("irq_enabled %d = %d from %x\n",offset,!bit,z80pc);
			irq_enabled[offset] = !bit; // it's actually an nmi here anyway...
			break;

		case 0x03:	/* RESET */
			if (bit) {
			    LOG("reset !!! from %x\n",z80pc);
			    cpu_reset(CPU_Z80_1);
			    cpu_reset(CPU_Z80_2);
			}
			break;

		case 0x04:	/* n.c. */
			break;

		case 0x05:	/* MOD 0 (xevious: n.c.) */
		case 0x06:	/* MOD 1 (xevious: n.c.) */
		case 0x07:	/* MOD 2 (xevious: n.c.) */
			break;
	}
}

static UINT8 *share1, *share2, *share3;

// All the namcoio stuff is from machine/namcoio.c in mame, but I don't plan to reuse this anywhere else, so I merge it here...

typedef UINT8   (*read8_space_func)  (UINT32 offset);
typedef void    (*write8_space_func) (UINT32 offset, UINT8 data);

enum
{
        NAMCOIO_NONE = 0,
        NAMCOIO_50XX,
        NAMCOIO_50XX_2,
        NAMCOIO_51XX,
        NAMCOIO_52XX,
        NAMCOIO_53XX_DIGDUG,
        NAMCOIO_53XX_POLEPOS,
        NAMCOIO_54XX,
        NAMCOIO_56XX,
        NAMCOIO_58XX,
        NAMCOIO_59XX,
        NAMCOIO_62XX
};

struct namcoio
{
	read8_space_func in[4];
	write8_space_func out[2];
	INT32 type;
	INT32 reset;
	INT32 lastcoins,lastbuttons;
	INT32 credits;
	INT32 coins[2];
	INT32 coins_per_cred[2];
	INT32 creds_per_coin[2];
	INT32 in_count;
	INT32 mode,coincred_mode,remap_joy;	// for 51XX
};

struct namcoio_interface
{
	read8_space_func in[4];	/* read handlers for ports A-D */
	write8_space_func out[2];	/* write handlers for ports A-B */
};

static UINT8 in0_l(UINT32 offset)	{ return input_buffer[0]; }		// fire and start buttons
static UINT8 in0_h(UINT32 offset)	{ return input_buffer[0] >> 4; }	// coins
static UINT8 in1_l(UINT32 offset)	{ return input_buffer[2]; }		// P1 joystick
static UINT8 in1_h(UINT32 offset)	{ return input_buffer[2] >> 4; }	// P2 joystick

static void out_0(UINT32 offset, UINT8 data)
{
	switch_led(1,data & 1);
	switch_led(0,data & 2);
	// coin_counter_w(1,~data & 4);
	// coin_counter_w(0,~data & 8);
}
static void out_1(UINT32 offset, UINT8 data)
{
	// coin_lockout_global_w(data & 1);
}

static const struct namcoio_interface intf0 =
{
	{ in0_l, in0_h, in1_l, in1_h },	/* port read handlers */
	{ out_0, out_1 }				/* port write handlers */
};

#define MAX_NAMCOIO 8
#define MAX_06XX 2

static struct namcoio io[MAX_NAMCOIO],back_io[MAX_NAMCOIO];
static INT32 nmi_cpu[MAX_06XX];

#define READ_PORT(n)	(io[chip].in[n](0) & 0x0f)
#define WRITE_PORT(n,d)	io[chip].out[n](0,(d) & 0x0f)

/* joystick input mapping

  The joystick is parsed and a number corresponding to the direction is returned,
  according to the following table:

          0
        7   1
      6   8   2
        5   3
          4

  The values for directions impossible to obtain on a joystick have not been
  verified on Namco original hardware, but they are the same in all the bootlegs,
  so we can assume they are right.
*/
static const int joy_map[16] =
/*  LDRU, LDR, LDU,  LD, LRU, LR,  LU,    L, DRU,  DR,  DU,   D,  RU,   R,   U, center */
{	 0xf, 0xe, 0xd, 0x5, 0xc, 0x9, 0x7, 0x6, 0xb, 0x3, 0xa, 0x4, 0x1, 0x2, 0x0, 0x8 };

static UINT8 customio_command[MAX_06XX];

static UINT8 namcoio_51XX_read(int chip)
{
	if (io[chip].mode == 0)	/* switch mode */
	{
		switch ((io[chip].in_count++) % 3)
		{
			default:
			case 0: return READ_PORT(0) | (READ_PORT(1) << 4);
			case 1: return READ_PORT(2) | (READ_PORT(3) << 4);
			case 2: return 0;	// nothing?
		}
	}
	else	/* credits mode */
	{
		switch ((io[chip].in_count++) % 3)
		{
			default:
			case 0:	// number of credits in BCD format
				{
					int in,toggle;

					in = ~(READ_PORT(0) | (READ_PORT(1) << 4));
					toggle = in ^ io[chip].lastcoins;
					io[chip].lastcoins = in;

					if (io[chip].coins_per_cred[0] > 0)
					{
						if (io[chip].credits >= 99)
						{
							WRITE_PORT(1,1);	// coin lockout
						}
						else
						{
							WRITE_PORT(1,0);	// coin lockout
							/* check if the user inserted a coin */
							if (toggle & in & 0x10)
							{
								io[chip].coins[0]++;
								WRITE_PORT(0,0x04);	// coin counter
								WRITE_PORT(0,0x0c);
								if (io[chip].coins[0] >= io[chip].coins_per_cred[0])
								{
									io[chip].credits += io[chip].creds_per_coin[0];
									io[chip].coins[0] -= io[chip].coins_per_cred[0];
								}
							}
							if (toggle & in & 0x20)
							{
								io[chip].coins[1]++;
								WRITE_PORT(0,0x08);	// coin counter
								WRITE_PORT(0,0x0c);
								if (io[chip].coins[1] >= io[chip].coins_per_cred[1])
								{
									io[chip].credits += io[chip].creds_per_coin[1];
									io[chip].coins[1] -= io[chip].coins_per_cred[1];
								}
							}
							if (toggle & in & 0x40)
							{
								io[chip].credits++;
							}
						}
					}
					else io[chip].credits = 100;	// free play

					if (io[chip].mode == 1)
					{
						int on = (cpu_frame_count & 0x10) >> 4;

						if (io[chip].credits >= 2)
							WRITE_PORT(0,0x0c | 3*on);	// lamps
						else if (io[chip].credits >= 1)
							WRITE_PORT(0,0x0c | 2*on);	// lamps
						else
							WRITE_PORT(0,0x0c);	// lamps off

						/* check for 1 player start button */
						if (toggle & in & 0x04)
						{
							if (io[chip].credits >= 1)
							{
								io[chip].credits--;
								io[chip].mode = 2;
								WRITE_PORT(0,0x0c);	// lamps off
							}
						}
						/* check for 2 players start button */
						else if (toggle & in & 0x08)
						{
							if (io[chip].credits >= 2)
							{
								io[chip].credits -= 2;
								io[chip].mode = 2;
								WRITE_PORT( 0,0x0c);	// lamps off
							}
						}
					}
				}

				if (~input_buffer[0] & 0x80)	/* check test mode switch */
					return 0xbb;

				return (io[chip].credits / 10) * 16 + io[chip].credits % 10;

			case 1:
				{
					int joy = READ_PORT(2) & 0x0f;
					int in,toggle;

					in = ~READ_PORT(0);
					toggle = in ^ io[chip].lastbuttons;
					io[chip].lastbuttons = (io[chip].lastbuttons & 2) | (in & 1);

					/* remap joystick */
					if (io[chip].remap_joy) joy = joy_map[joy];

					/* fire */
					joy |= ((toggle & in & 0x01)^1) << 4;
					joy |= ((in & 0x01)^1) << 5;

					return joy;
				}

			case 2:
				{
					int joy = READ_PORT(3) & 0x0f;
					int in,toggle;

					in = ~READ_PORT(0);
					toggle = in ^ io[chip].lastbuttons;
					io[chip].lastbuttons = (io[chip].lastbuttons & 1) | (in & 2);

					/* remap joystick */
					if (io[chip].remap_joy) joy = joy_map[joy];

					/* fire */
					joy |= ((toggle & in & 0x02)^2) << 3;
					joy |= ((in & 0x02)^2) << 4;

					return joy;
				}
		}
	}
}

static UINT8 namco_06xx_data_read(int chipnum)
{
	print_debug("forwarding read to chip %d\n",chipnum%3);

	switch (io[chipnum].type)
	{
		case NAMCOIO_51XX: return namcoio_51XX_read(chipnum);
		//case NAMCOIO_53XX_DIGDUG:  return namcoio_53XX_digdug_read(machine, chipnum);
		//case NAMCOIO_53XX_POLEPOS: return namcoio_53XX_polepos_read(machine, chipnum);
		default:
			print_debug("custom IO type %d unsupported read\n",io[chipnum].type);
			return 0xff;
	}
}

static UINT8 namco_06xx_data_r(int chip,int offset)
{
	if (!(customio_command[chip] & 0x10))
	{
		print_debug("06XX #%d read in write mode %02x\n",chip,customio_command[chip]);
		return 0;
	}

	switch (customio_command[chip] & 0xf)
	{
		case 0x1: return namco_06xx_data_read(4*chip + 0);
		case 0x2: return namco_06xx_data_read(4*chip + 1);
		case 0x4: return namco_06xx_data_read(4*chip + 2);
		case 0x8: return namco_06xx_data_read(4*chip + 3);
		default:
			print_debug("06XX #%d read in unsupported mode %02x\n",chip,customio_command[chip]);
			return 0xff;
	}
}

static void namcoio_51XX_write(int chip,int data)
{
	data &= 0x07;

	if (io[chip].coincred_mode)
	{
		switch (io[chip].coincred_mode--)
		{
			case 4: io[chip].coins_per_cred[0] = data; break;
			case 3: io[chip].creds_per_coin[0] = data; break;
			case 2: io[chip].coins_per_cred[1] = data; break;
			case 1: io[chip].creds_per_coin[1] = data; break;
		}
	}
	else
	{
		switch (data)
		{
			case 0:	// nop
				break;

			case 1:	// set coinage
				io[chip].coincred_mode = 4;
				/* this is a good time to reset the credits counter */
				io[chip].credits = 0;

				{
					if (is_current_game("xevious"))
					{
						io[chip].coincred_mode = 6;
						io[chip].remap_joy = 1;
					}
				}
				break;

			case 2:	// go in "credits" mode and enable start buttons
				io[chip].mode = 1;
				io[chip].in_count = 0;
				break;

			case 3:	// disable joystick remapping
				io[chip].remap_joy = 0;
				break;

			case 4:	// enable joystick remapping
				io[chip].remap_joy = 1;
				break;

			case 5:	// go in "switch" mode
				io[chip].mode = 0;
				io[chip].in_count = 0;
				break;

			default:
				print_debug("unknown 51XX command %02x\n",data);
				break;
		}
	}
}

static void namco_06xx_data_write(int chipnum,UINT8 data)
{
    static UINT32 last_frame;
	switch (io[chipnum].type)
	{
		//case NAMCOIO_50XX:   namco_50xx_write(machine, data); break;
		//case NAMCOIO_50XX_2: namco_50xx_2_write(machine, data); break;
		case NAMCOIO_51XX:   namcoio_51XX_write(chipnum,data); break;
		//case NAMCOIO_52XX:   namcoio_52xx_write(devtag_get_device(machine, "namco52"), data); break;
		case NAMCOIO_54XX:
				     if (cpu_frame_count - last_frame > 1 && sample) {
					 last_frame = cpu_frame_count;
					 if (sample == ogg)
					     load_sample("galaga_explode.ogg");
					 else if (sample == mp3)
					     load_sample("galaga_explode.mp3");
					 else if (sample == wav)
					     load_sample("galaga_explode.wav");
					 else if (sample == emudx)
					     raine_play_sample(1,100);
				     }
				     break; // namco_54xx_write( data); break;
		default:
			print_debug("custom IO type %d unsupported write\n",io[chipnum].type);
			break;
	}
}

static void namco_06xx_data_w(int chip,int offset,int data)
{
	if (customio_command[chip] & 0x10)
	{
		print_debug("06XX #%d write in read mode %02x\n",chip,customio_command[chip]);
		return;
	}

	switch (customio_command[chip] & 0xf)
	{
		case 0x1: namco_06xx_data_write(4*chip + 0,data); break;
		case 0x2: namco_06xx_data_write(4*chip + 1,data); break;
		case 0x4: namco_06xx_data_write(4*chip + 2,data); break;
		case 0x8: namco_06xx_data_write(4*chip + 3,data); break;
		default:
			print_debug("06XX #%d write in unsupported mode %02x\n",chip,customio_command[chip]);
			break;
	}
}

static UINT8 namco_06xx_0_data_r(UINT32 offset) {
    return namco_06xx_data_r(0,offset);
}

static void namco_06xx_0_data_w(UINT32 offset, UINT8 data) {
    namco_06xx_data_w(0,offset,data);
}

static UINT8 namco_06xx_ctrl_r(int chip)
{
    if (z80pc == 0x37f9 && customio_command[chip] != 0x10) {
	// At this point, it just enabled the nmis and is waiting for an nmi to occur
	mz80ReleaseTimeslice();
    }

    return customio_command[chip];
}

static void namco_06xx_read_request(int chipnum)
{
	switch (io[chipnum].type)
	{
		// case NAMCOIO_50XX:   namco_50xx_read_request(machine); break;
		// case NAMCOIO_50XX_2: namco_50xx_2_read_request(machine); break;
		default:
			print_debug("custom IO type %d read_request unsupported\n",io[chipnum].type);
			break;
	}
}

static void namco_06xx_ctrl_w(int chip,int data)
{
	customio_command[chip] = data;

	LOG("customio_command %x,%x\n",chip,data);
	if ((customio_command[chip] & 0x0f) == 0)
	{
		nmi_enable = 0;
	}
	else
	{
		// this timing is critical. Due to a bug, Bosconian will stop responding to
		// inputs if a transfer terminates at the wrong time.
		// On the other hand, the time cannot be too short otherwise the 54XX will
		// not have enough time to process the incoming commands.
		nmi_enable = 1;
		LOG("nmi enable cmd %x\n",data);

		if (customio_command[chip] & 0x10)
		{
			switch (customio_command[chip] & 0xf)
			{
				case 0x1: namco_06xx_read_request(4*chip + 0); break;
				case 0x2: namco_06xx_read_request(4*chip + 1); break;
				case 0x4: namco_06xx_read_request(4*chip + 2); break;
				case 0x8: namco_06xx_read_request(4*chip + 3); break;
				default:
					print_debug("06XX #%d read in unsupported mode %02x\n",chip,customio_command[chip]);
			}
		}
	}
}

static UINT8 namco_06xx_0_ctrl_r(UINT32 offset)	{
    return namco_06xx_ctrl_r(0);
}

static void namco_06xx_0_ctrl_w(UINT32 offset, UINT8 data) {
    namco_06xx_ctrl_w(0,data);
}

static void bosco_latch_reset()
{
	int i;

	/* Reset all latches */
	for (i = 0;i < 8;i++)
		bosco_latch_w(i,0);
}

static UINT8 nop_r(UINT32 offset) { return 0x0f; }
static void nop_w(UINT32 offset, UINT8 data) { }

static void restore_io() {
    for (int n=0; n<4; n++)
	io[0].in[n] = back_io[0].in[n];
    for (int n=0; n<2; n++)
	io[0].out[n] = back_io[0].out[n];
}

static void namcoio_state_save(int chipnum)
{
    AddSaveData(ASCII_ID('N','M','I','O'),(UINT8*)&io[chipnum],sizeof(struct namcoio));
    AddLoadCallback(&restore_io);
}

void namcoio_set_reset_line(int chipnum, int state)
{
	io[chipnum].reset = (state == ASSERT_LINE) ? 1 : 0;
	if (state != CLEAR_LINE)
	{
		/* reset internal registers */
		io[chipnum].credits = 0;
		io[chipnum].coins[0] = 0;
		io[chipnum].coins_per_cred[0] = 1;
		io[chipnum].creds_per_coin[0] = 1;
		io[chipnum].coins[1] = 0;
		io[chipnum].coins_per_cred[1] = 1;
		io[chipnum].creds_per_coin[1] = 1;
		io[chipnum].in_count = 0;
	}
}

void namcoio_init(int chipnum, int type, const struct namcoio_interface *intf)
{
	if (chipnum < MAX_NAMCOIO)
	{
		io[chipnum].type = type;
		io[chipnum].in[0] = (intf && intf->in[0]) ? intf->in[0] : nop_r;
		io[chipnum].in[1] = (intf && intf->in[1]) ? intf->in[1] : nop_r;
		io[chipnum].in[2] = (intf && intf->in[2]) ? intf->in[2] : nop_r;
		io[chipnum].in[3] = (intf && intf->in[3]) ? intf->in[3] : nop_r;
		io[chipnum].out[0] = (intf && intf->out[0]) ? intf->out[0] : nop_w;
		io[chipnum].out[1] = (intf && intf->out[1]) ? intf->out[1] : nop_w;
		memcpy(back_io,io,sizeof(io));
		namcoio_state_save(chipnum);
		namcoio_set_reset_line(chipnum,PULSE_LINE);
	}
}

void namco_06xx_init(int chipnum, int cpu,
	int type0, const struct namcoio_interface *intf0,
	int type1, const struct namcoio_interface *intf1,
	int type2, const struct namcoio_interface *intf2,
	int type3, const struct namcoio_interface *intf3)
{
	if (chipnum < MAX_06XX)
	{
		namcoio_init(4*chipnum + 0, type0, intf0);
		namcoio_init(4*chipnum + 1, type1, intf1);
		namcoio_init(4*chipnum + 2, type2, intf2);
		namcoio_init(4*chipnum + 3, type3, intf3);
		nmi_cpu[chipnum] = cpu;
		// nmi_timer[chipnum] = timer_alloc(machine, nmi_generate, NULL);
		namcoio_state_save(chipnum);
	}
}

static int watchdog;

static void watchdog_reset_w(UINT32, UINT8 offset) {
    watchdog = 8;
}

static void reset_galaga() {
	/* Reset all latches */
	bosco_latch_reset();
	watchdog = 8;

	namco_06xx_init(0, 0,
		NAMCOIO_51XX, &intf0,
		NAMCOIO_NONE, NULL,
		NAMCOIO_NONE, NULL,
		NAMCOIO_54XX, NULL);

	// timer_adjust_oneshot(cpu3_interrupt_timer, video_screen_get_time_until_pos(machine->primary_screen, 64, 0), 64);
}

static UINT8 galaga_starcontrol[6];

static void galaga_starcontrol_w(UINT32 offset, UINT8 data)
{
    offset &= 7;
    LOG("starcontrol_w %x,%x from %x\n",offset,data,z80pc);
    galaga_starcontrol[offset] = data & 1;
}

static void galaga_flip_screen_w(UINT32 offset, UINT8 data) {
    LOG("flipswreen %d\n",data & 1);
}

#ifdef SHARE_HANDLERS
static UINT8 share1_ra(UINT32 offset) {
    offset &= 0x3ff;
    UINT8 ret = share1[offset];
    LOG("z80a: share1 read %x -> %x from %x\n",offset,ret,z80pc);
    return ret;
}

static UINT8 share2_ra(UINT32 offset) {
    offset &= 0x3ff;
    UINT8 ret = share2[offset];
    if (offset == 0x100 && z80pc == 0x358c && !ret) {
	// it waits for a value != 0 here, can stop here
	mz80ReleaseTimeslice();
    } else if (offset == 0x2ae && z80pc == 0x1c2 && ret) {
	// the opposite : waiting for 0 here
	mz80ReleaseTimeslice();
    } else if (offset == 0x201 && z80pc == 0x390 && ret == 1)
	// yeah looping on value 1 here !
	mz80ReleaseTimeslice();
    else if (offset == 0x201 && z80pc == 0x3db && ret == 2)
	// yeah looping on value 2 here !
	mz80ReleaseTimeslice();
    else if (offset == 0x2a0 && z80pc == 0x371c && ret < 0x80)
	// rather special
	mz80ReleaseTimeslice();
    else if (offset == 0x2af && z80pc == 0x415 && ret)
	mz80ReleaseTimeslice();
    else if (offset == 0x2d7 && z80pc == 0x853 && ret == 1)
	mz80ReleaseTimeslice();
    LOG("z80a: share2 read %x -> %x from %x\n",offset,ret,z80pc);
    return ret;
}

static UINT8 share3_ra(UINT32 offset) {
    offset &= 0x3ff;
    UINT8 ret = share3[offset];
    LOG("z80a: share3 read %x -> %x from %x\n",offset,ret,z80pc);
    return ret;
}

static void share1_wa(UINT32 offset, UINT8 data) {
    offset &= 0x3ff;
    LOG("z80a: share1 write %x,%x from %x\n",offset,data,z80pc);
    share1[offset] = data;
}

static void share2_wa(UINT32 offset, UINT8 data) {
    offset &= 0x3ff;
    LOG("z80a: share2 write %x,%x from %x\n",offset,data,z80pc);
    share2[offset] = data;
}

static void share3_wa(UINT32 offset, UINT8 data) {
    offset &= 0x3ff;
    LOG("z80a: share3 write %x,%x from %x\n",offset,data,z80pc);
    share3[offset] = data;
}

static UINT8 share1_rb(UINT32 offset) {
    offset &= 0x3ff;
    UINT8 ret = share1[offset];
    LOG("z80b: share1 read %x -> %x from %x\n",offset,ret,z80pc);
    return ret;
}

static UINT8 share2_rb(UINT32 offset) {
    offset &= 0x3ff;
    UINT8 ret = share2[offset];
    LOG("z80b: share2 read %x -> %x from %x\n",offset,ret,z80pc);
    if (offset == 0x100 && z80pc == 0x597 && ret) {
	// it waits for a value = 0 here, can stop here
	mz80ReleaseTimeslice();
    } else if (offset == 0x2d6 && z80pc == 0x5ea && ret == 1)
	mz80ReleaseTimeslice();
    return ret;
}

static UINT8 share3_rb(UINT32 offset) {
    offset &= 0x3ff;
    UINT8 ret = share3[offset];
    LOG("z80b: share3 read %x -> %x from %x\n",offset,ret,z80pc);
    return ret;
}

static void share1_wb(UINT32 offset, UINT8 data) {
    offset &= 0x3ff;
    LOG("z80b: share1 write %x,%x from %x\n",offset,data,z80pc);
    share1[offset] = data;
}

static void share2_wb(UINT32 offset, UINT8 data) {
    offset &= 0x3ff;
    LOG("z80b: share2 write %x,%x from %x\n",offset,data,z80pc);
    share2[offset] = data;
}

static void share3_wb(UINT32 offset, UINT8 data) {
    offset &= 0x3ff;
    LOG("z80b: share3 write %x,%x from %x\n",offset,data,z80pc);
    share3[offset] = data;
}

static UINT8 share1_rc(UINT32 offset) {
    offset &= 0x3ff;
    UINT8 ret = share1[offset];
    LOG("z80c: share1 read %x -> %x from %x\n",offset,ret,z80pc);
    return ret;
}

static UINT8 share2_rc(UINT32 offset) {
    offset &= 0x3ff;
    UINT8 ret = share2[offset];
    if (offset == 0x101 && z80pc == 0xa5 && ret) {
	// it waits for a value = 0 here, can stop here
	mz80ReleaseTimeslice();
    }
    LOG("z80c: share2 read %x -> %x from %x\n",offset,ret,z80pc);
    return ret;
}

static UINT8 share3_rc(UINT32 offset) {
    offset &= 0x3ff;
    UINT8 ret = share3[offset];
    LOG("z80c: share3 read %x -> %x from %x\n",offset,ret,z80pc);
    return ret;
}

static void share1_wc(UINT32 offset, UINT8 data) {
    offset &= 0x3ff;
    LOG("z80c: share1 write %x,%x from %x\n",offset,data,z80pc);
    share1[offset] = data;
}

static void share2_wc(UINT32 offset, UINT8 data) {
    offset &= 0x3ff;
    LOG("z80c: share2 write %x,%x from %x\n",offset,data,z80pc);
    share2[offset] = data;
}

static void share3_wc(UINT32 offset, UINT8 data) {
    offset &= 0x3ff;
    LOG("z80c: share3 write %x,%x from %x\n",offset,data,z80pc);
    share3[offset] = data;
}
#endif

static UINT8 *mypal,*spriteram, *spriteram_2,*spriteram_3,*videoram;
static int layer_id_data[2];
static void load_galaga() {
    set_reset_function(&reset_galaga);
    // Taking 64kb (0x10000) is safer here for mz80
    // it tends to use its stack directly from the rombase, and the stack is installed in the shared ram by these z80
    // so if only using the roms as a rombase, it's going to write outside the bounds when executing a call !
    // It will make the saves slightly bigger.
    // Here I arranged to copy the 3 roms to the same 64 Kb of ram for more simplicity. It fits...
    AddSaveData(ASCII_ID('S','T','A','R'),galaga_starcontrol,sizeof(galaga_starcontrol));
    AddSaveData(ASCII_ID('I','R','Q','E'),(UINT8*)irq_enabled,sizeof(irq_enabled));
    AddSaveData(ASCII_ID('N','M','I','E'),(UINT8*)&nmi_enable,sizeof(nmi_enable));
    // The ramsize was set at 64 kb to work around an mz80 bug related to stack writes, now it's fixed, but I'll keep 64 kb because it's not so big and this way there will be no
    // incompatible save !
    RAMSize = 0x10000;
    if(!(RAM=AllocateMem(RAMSize))) return;
    memset(RAM,0,RAMSize);
    videoram = RAM + 0x8000;
    share1 = RAM + 0x8800;
    share2 = RAM + 0x9000;
    share3 = RAM + 0x9800;
    pengo_soundregs = RAM + 0x6000;
    spriteram   = share1 + 0x380;
    spriteram_2 = share2 + 0x380;
    spriteram_3 = share3 + 0x380;

    AddZ80AROMBase(ROM, 0x38, 0x66);
    AddZ80BROMBase(Z80ROM, 0x38, 0x66);
    AddZ80CROMBase(load_region[REGION_CPU3], 0x38, 0x66);
    AddZ80ARead(0x0000, 0x3fff, NULL, ROM);  /* the only area different for each CPU */
    AddZ80BRead(0x0000, 0x0fff, NULL, Z80ROM);  /* the only area different for each CPU */
    AddZ80CRead(0x0000, 0x0fff, NULL, load_region[REGION_CPU3]);  /* the only area different for each CPU */
    // The very same memory map for the 3 z80 except for the rom !
    for (int n=0; n<=2; n++) {
	add_z80_r(n,0x6800, 0x6807, bosco_dsw_r, NULL);
	add_z80_w(n,0x6800, 0x681f, pengo_sound_w, NULL);
	// add_z80_r(n,0x6000, 0x601f, NULL, pengo_soundregs);
	add_z80_w(n,0x6820, 0x6827, bosco_latch_w, NULL);						/* misc latches */
	add_z80_w(n,0x6830, 0x6830, watchdog_reset_w, NULL); // Nope, just resets the machine if it hangs, but otherwise useless
	add_z80_r(n,0x7000, 0x70ff, namco_06xx_0_data_r, NULL);
	add_z80_w(n,0x7000, 0x70ff, namco_06xx_0_data_w, NULL);
	add_z80_r(n,0x7100, 0x7100, namco_06xx_0_ctrl_r, NULL);
	add_z80_w(n,0x7100, 0x7100, namco_06xx_0_ctrl_w, NULL);
	add_z80_rw(n,0x8000, 0x87ff, videoram); // video ram
	// These shared ram are trouble, 100 slices / frame to make them work in original mame...
	// I'll use the same principle for now, maybe improve later...
	add_z80_w(n,0xa000, 0xa005, galaga_starcontrol_w, NULL);
	add_z80_w(n,0xa007, 0xa007, galaga_flip_screen_w, NULL);
#ifndef SHARE_HANDLERS
	add_z80_rw(n,0x8800, 0x8bff, share1);
	add_z80_rw(n,0x9000, 0x93ff, share2);
	add_z80_rw(n,0x9800, 0x9bff, share3);
	finish_conf_z80(n);
#endif
    }
#ifdef SHARE_HANDLERS
    AddZ80ARead(0x8800, 0x8bff, share1_ra,NULL);
    AddZ80ARead(0x9000, 0x93ff, share2_ra,NULL);
    AddZ80ARead(0x9800, 0x9bff, share3_ra, NULL);
    AddZ80AWrite(0x8800, 0x8bff, share1_wa,NULL);
    AddZ80AWrite(0x9000, 0x93ff, share2_wa,NULL);
    AddZ80AWrite(0x9800, 0x9bff, share3_wa, NULL);

    AddZ80BRead(0x8800, 0x8bff, share1_rb,NULL);
    AddZ80BRead(0x9000, 0x93ff, share2_rb,NULL);
    AddZ80BRead(0x9800, 0x9bff, share3_rb, NULL);
    AddZ80BWrite(0x8800, 0x8bff, share1_wb,NULL);
    AddZ80BWrite(0x9000, 0x93ff, share2_wb,NULL);
    AddZ80BWrite(0x9800, 0x9bff, share3_wb, NULL);

    AddZ80CRead(0x8800, 0x8bff, share1_rc,NULL);
    AddZ80CRead(0x9000, 0x93ff, share2_rc,NULL);
    AddZ80CRead(0x9800, 0x9bff, share3_rc, NULL);
    AddZ80CWrite(0x8800, 0x8bff, share1_wc,NULL);
    AddZ80CWrite(0x9000, 0x93ff, share2_wc,NULL);
    AddZ80CWrite(0x9800, 0x9bff, share3_wc, NULL);

    finish_conf_z80(0);
    finish_conf_z80(1);
    finish_conf_z80(2);
#endif
    UINT8 *color_prom = load_region[REGION_PROMS];
    int i;
    UINT8 pal[32*2];
    mypal = AllocateMem(0x100*2*2 + 64*2); // 256 colors for text and then for sprites, on 2 bytes, and 64 more for the stars
    for (i = 0;i < 32;i++)
    {
	int bit0,bit1,bit2,r,g,b;


	bit0 = ((*color_prom) >> 0) & 0x01;
	bit1 = ((*color_prom) >> 1) & 0x01;
	bit2 = ((*color_prom) >> 2) & 0x01;
	// r = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
	r = (bit2 << 2) | (bit1 << 1) | bit0;
	bit0 = ((*color_prom) >> 3) & 0x01;
	bit1 = ((*color_prom) >> 4) & 0x01;
	bit2 = ((*color_prom) >> 5) & 0x01;
	// g = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
	g = (bit2 << 2) | (bit1 << 1) | bit0;
	bit0 = 0;
	bit1 = ((*color_prom) >> 6) & 0x01;
	bit2 = ((*color_prom) >> 7) & 0x01;
	// b = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
	b = (bit2 << 2) | (bit1 << 1) | bit0;

	WriteWord(&pal[i*2],b + (g<<3) + (r<<6));
	color_prom++;
    }

    // Text
    for (i = 0;i < 64*4;i++)
	WriteWord(&mypal[i*2], ReadWord(&pal[((*(color_prom++) & 0x0f) + 0x10)*2]));	/* chars */

    /* sprites */
    for (i = 0;i < 64*4;i++)
	WriteWord(&mypal[i*2 + 0x200], ReadWord(&pal[(*(color_prom++) & 0x0f)*2]));	/* sprites */

    /* now the stars */
    // Which are a problem since it seems I will need a more precise color mapper for that !!!
    for (i = 0;i < 64;i++)
    {
	int bits,r,g,b;
	// int map[4] = { 0x00, 0x47, 0x97 ,0xde };
	int map[4] = { 0x00, 0x1, 0x3 ,0x7 }; // I am going to try to keep my 3 bits color mapper... !

	bits = (i >> 0) & 0x03;
	r = map[bits];
	bits = (i >> 2) & 0x03;
	g = map[bits];
	bits = (i >> 4) & 0x03;
	b = map[bits];

	WriteWord(&mypal[i*2 + 0x400],b + (g<<3) + (r<<6));
    }
    InitPaletteMap(mypal, 0x24*4, 4, 0x4000); // ???
    set_colour_mapper(&col_map_xxxxRrrgggbbb);
    layer_id_data[0] = add_layer_info(gettext("text"));
    layer_id_data[1] = add_layer_info(gettext("sprites"));
    current_game->sound = sound_galaga;
    if (exists("galaga_explode.wav")) {
	sample = wav;
    } else if (exists("galaga_explode.mp3"))
	sample = mp3;
    else if (exists("galaga_explode.ogg"))
	sample = ogg;
    else if (exists_emudx_file("galdxm.dx2")) {
	sample = emudx;
	current_game->sound = sound_galaga_dx;
    } else
	sample = none;
}

static gfx_layout spritelayout_galaga =
{
	16,16,
	RGN_FRAC(1,1),
	2,
	{ 0, 4 },
	{ 0, 1, 2, 3, 8*8, 8*8+1, 8*8+2, 8*8+3, 16*8+0, 16*8+1, 16*8+2, 16*8+3,
			24*8+0, 24*8+1, 24*8+2, 24*8+3 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
			32*8, 33*8, 34*8, 35*8, 36*8, 37*8, 38*8, 39*8 },
	64*8
};

static gfx_layout charlayout_2bpp =
{
	8,8,
	RGN_FRAC(1,1),
	2,
	{ 0, 4 },
	{ 8*8+0, 8*8+1, 8*8+2, 8*8+3, 0, 1, 2, 3 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	16*8
};

static struct GFX_LIST gfx_galaga[] =
{
	{ REGION_GFX1, &charlayout_2bpp }, // ,        0, 64 },
	{ REGION_GFX2, &spritelayout_galaga }, // , 64*4, 64 },
	{ 0, NULL } /* end of array */
};

/*
Galaga star line and pixel locations pulled directly from
a clocked stepping of the 05 starfield. The chip was clocked
on a test rig with hblank and vblank simulated, each X & Y
location of a star being recorded along with it's color value.

Because the starfield begins generating stars at the point
in time it's enabled the exact horiz location of the stars
on Galaga depends on the length of time of the POST for the
original board.

Two control bits determine which of two sets are displayed
set 0 or 1 and simultaneously 2 or 3.

There are 63 stars in each set, 126 displayed at any one time

*/

struct star
{
	int x,y,col,set;
};

struct star star_seed_tab[252]=
{

/* also shared by Bosconian */

/* star set 0 */

{0x0085,0x0006,0x35, 0x00},
{0x008F,0x0008,0x30, 0x00},
{0x00E5,0x001B,0x07, 0x00},
{0x0022,0x001C,0x31, 0x00},
{0x00E5,0x0025,0x1D, 0x00},
{0x0015,0x0026,0x29, 0x00},
{0x0080,0x002D,0x3B, 0x00},
{0x0097,0x002E,0x1C, 0x00},
{0x00BA,0x003B,0x05, 0x00},
{0x0036,0x003D,0x36, 0x00},
{0x0057,0x0044,0x09, 0x00},
{0x00CF,0x0044,0x3D, 0x00},
{0x0061,0x004E,0x27, 0x00},
{0x0087,0x0064,0x1A, 0x00},
{0x00D6,0x0064,0x17, 0x00},
{0x000B,0x006C,0x3C, 0x00},
{0x0006,0x006D,0x24, 0x00},
{0x0018,0x006E,0x3A, 0x00},
{0x00A9,0x0079,0x23, 0x00},
{0x008A,0x007B,0x11, 0x00},
{0x00D6,0x0080,0x0C, 0x00},
{0x0067,0x0082,0x3F, 0x00},
{0x0039,0x0083,0x38, 0x00},
{0x0072,0x0083,0x14, 0x00},
{0x00EC,0x0084,0x16, 0x00},
{0x008E,0x0085,0x10, 0x00},
{0x0020,0x0088,0x25, 0x00},
{0x0095,0x008A,0x0F, 0x00},
{0x000E,0x008D,0x00, 0x00},
{0x0006,0x0091,0x2E, 0x00},
{0x0007,0x0094,0x0D, 0x00},
{0x00AE,0x0097,0x0B, 0x00},
{0x0000,0x0098,0x2D, 0x00},
{0x0086,0x009B,0x01, 0x00},
{0x0058,0x00A1,0x34, 0x00},
{0x00FE,0x00A1,0x3E, 0x00},
{0x00A2,0x00A8,0x1F, 0x00},
{0x0041,0x00AA,0x0A, 0x00},
{0x003F,0x00AC,0x32, 0x00},
{0x00DE,0x00AC,0x03, 0x00},
{0x00D4,0x00B9,0x26, 0x00},
{0x006D,0x00BB,0x1B, 0x00},
{0x0062,0x00BD,0x39, 0x00},
{0x00C9,0x00BE,0x18, 0x00},
{0x006C,0x00C1,0x04, 0x00},
{0x0059,0x00C3,0x21, 0x00},
{0x0060,0x00CC,0x0E, 0x00},
{0x0091,0x00CC,0x12, 0x00},
{0x003F,0x00CF,0x06, 0x00},
{0x00F7,0x00CF,0x22, 0x00},
{0x0044,0x00D0,0x33, 0x00},
{0x0034,0x00D2,0x08, 0x00},
{0x00D3,0x00D9,0x20, 0x00},
{0x0071,0x00DD,0x37, 0x00},
{0x0073,0x00E1,0x2C, 0x00},
{0x00B9,0x00E3,0x2F, 0x00},
{0x00A9,0x00E4,0x13, 0x00},
{0x00D3,0x00E7,0x19, 0x00},
{0x0037,0x00ED,0x02, 0x00},
{0x00BD,0x00F4,0x15, 0x00},
{0x000F,0x00F6,0x28, 0x00},
{0x004F,0x00F7,0x2B, 0x00},
{0x00FB,0x00FF,0x2A, 0x00},

/* star set 1 */

{0x00FE,0x0004,0x3D, 0x01},
{0x00C4,0x0006,0x10, 0x01},
{0x001E,0x0007,0x2D, 0x01},
{0x0083,0x000B,0x1F, 0x01},
{0x002E,0x000D,0x3C, 0x01},
{0x001F,0x000E,0x00, 0x01},
{0x00D8,0x000E,0x2C, 0x01},
{0x0003,0x000F,0x17, 0x01},
{0x0095,0x0011,0x3F, 0x01},
{0x006A,0x0017,0x35, 0x01},
{0x00CC,0x0017,0x02, 0x01},
{0x0000,0x0018,0x32, 0x01},
{0x0092,0x001D,0x36, 0x01},
{0x00E3,0x0021,0x04, 0x01},
{0x002F,0x002D,0x37, 0x01},
{0x00F0,0x002F,0x0C, 0x01},
{0x009B,0x003E,0x06, 0x01},
{0x00A4,0x004C,0x07, 0x01},
{0x00EA,0x004D,0x13, 0x01},
{0x0084,0x004E,0x21, 0x01},
{0x0033,0x0052,0x0F, 0x01},
{0x0070,0x0053,0x0E, 0x01},
{0x0006,0x0059,0x08, 0x01},
{0x0081,0x0060,0x28, 0x01},
{0x0037,0x0061,0x29, 0x01},
{0x008F,0x0067,0x2F, 0x01},
{0x001B,0x006A,0x1D, 0x01},
{0x00BF,0x007C,0x12, 0x01},
{0x0051,0x007F,0x31, 0x01},
{0x0061,0x0086,0x25, 0x01},
{0x006A,0x008F,0x0D, 0x01},
{0x006A,0x0091,0x19, 0x01},
{0x0090,0x0092,0x05, 0x01},
{0x003B,0x0096,0x24, 0x01},
{0x008C,0x0097,0x0A, 0x01},
{0x0006,0x0099,0x03, 0x01},
{0x0038,0x0099,0x38, 0x01},
{0x00A8,0x0099,0x18, 0x01},
{0x0076,0x00A6,0x20, 0x01},
{0x00AD,0x00A6,0x1C, 0x01},
{0x00EC,0x00A6,0x1E, 0x01},
{0x0086,0x00AC,0x15, 0x01},
{0x0078,0x00AF,0x3E, 0x01},
{0x007B,0x00B3,0x09, 0x01},
{0x0027,0x00B8,0x39, 0x01},
{0x0088,0x00C2,0x23, 0x01},
{0x0044,0x00C3,0x3A, 0x01},
{0x00CF,0x00C5,0x34, 0x01},
{0x0035,0x00C9,0x30, 0x01},
{0x006E,0x00D1,0x3B, 0x01},
{0x00D6,0x00D7,0x16, 0x01},
{0x003A,0x00D9,0x2B, 0x01},
{0x00AB,0x00E0,0x11, 0x01},
{0x00E0,0x00E2,0x1B, 0x01},
{0x006F,0x00E6,0x0B, 0x01},
{0x00B8,0x00E8,0x14, 0x01},
{0x00D9,0x00E8,0x1A, 0x01},
{0x00F9,0x00E8,0x22, 0x01},
{0x0004,0x00F1,0x2E, 0x01},
{0x0049,0x00F8,0x26, 0x01},
{0x0010,0x00F9,0x01, 0x01},
{0x0039,0x00FB,0x33, 0x01},
{0x0028,0x00FC,0x27, 0x01},

/* star set 2 */

{0x00FA,0x0006,0x19, 0x02},
{0x00E4,0x0007,0x2D, 0x02},
{0x0072,0x000A,0x03, 0x02},
{0x0084,0x001B,0x00, 0x02},
{0x00BA,0x001D,0x29, 0x02},
{0x00E3,0x0022,0x04, 0x02},
{0x00D1,0x0026,0x2A, 0x02},
{0x0089,0x0032,0x30, 0x02},
{0x005B,0x0036,0x27, 0x02},
{0x0084,0x003A,0x36, 0x02},
{0x0053,0x003F,0x0D, 0x02},
{0x0008,0x0040,0x1D, 0x02},
{0x0055,0x0040,0x1A, 0x02},
{0x00AA,0x0041,0x31, 0x02},
{0x00FB,0x0041,0x2B, 0x02},
{0x00BC,0x0046,0x16, 0x02},
{0x0093,0x0052,0x39, 0x02},
{0x00B9,0x0057,0x10, 0x02},
{0x0054,0x0059,0x28, 0x02},
{0x00E6,0x005A,0x01, 0x02},
{0x00A7,0x005D,0x1B, 0x02},
{0x002D,0x005E,0x35, 0x02},
{0x0014,0x0062,0x21, 0x02},
{0x0069,0x006D,0x1F, 0x02},
{0x00CE,0x006F,0x0B, 0x02},
{0x00DF,0x0075,0x2F, 0x02},
{0x00CB,0x0077,0x12, 0x02},
{0x004E,0x007C,0x23, 0x02},
{0x004A,0x0084,0x0F, 0x02},
{0x0012,0x0086,0x25, 0x02},
{0x0068,0x008C,0x32, 0x02},
{0x0003,0x0095,0x20, 0x02},
{0x000A,0x009C,0x17, 0x02},
{0x005B,0x00A3,0x08, 0x02},
{0x005F,0x00A4,0x3E, 0x02},
{0x0072,0x00A4,0x2E, 0x02},
{0x00CC,0x00A6,0x06, 0x02},
{0x008A,0x00AB,0x0C, 0x02},
{0x00E0,0x00AD,0x26, 0x02},
{0x00F3,0x00AF,0x0A, 0x02},
{0x0075,0x00B4,0x13, 0x02},
{0x0068,0x00B7,0x11, 0x02},
{0x006D,0x00C2,0x2C, 0x02},
{0x0076,0x00C3,0x14, 0x02},
{0x00CF,0x00C4,0x1E, 0x02},
{0x0004,0x00C5,0x1C, 0x02},
{0x0013,0x00C6,0x3F, 0x02},
{0x00B9,0x00C7,0x3C, 0x02},
{0x0005,0x00D7,0x34, 0x02},
{0x0095,0x00D7,0x3A, 0x02},
{0x00FC,0x00D8,0x02, 0x02},
{0x00E7,0x00DC,0x09, 0x02},
{0x001D,0x00E1,0x05, 0x02},
{0x0005,0x00E6,0x33, 0x02},
{0x001C,0x00E9,0x3B, 0x02},
{0x00A2,0x00ED,0x37, 0x02},
{0x0028,0x00EE,0x07, 0x02},
{0x00DD,0x00EF,0x18, 0x02},
{0x006D,0x00F0,0x38, 0x02},
{0x00A1,0x00F2,0x0E, 0x02},
{0x0074,0x00F7,0x3D, 0x02},
{0x0069,0x00F9,0x22, 0x02},
{0x003F,0x00FF,0x24, 0x02},

/* star set 3 */

{0x0071,0x0010,0x34, 0x03},
{0x00AF,0x0011,0x23, 0x03},
{0x00A0,0x0014,0x26, 0x03},
{0x0002,0x0017,0x02, 0x03},
{0x004B,0x0019,0x31, 0x03},
{0x0093,0x001C,0x0E, 0x03},
{0x001B,0x001E,0x25, 0x03},
{0x0032,0x0020,0x2E, 0x03},
{0x00EE,0x0020,0x3A, 0x03},
{0x0079,0x0022,0x2F, 0x03},
{0x006C,0x0023,0x17, 0x03},
{0x00BC,0x0025,0x11, 0x03},
{0x0041,0x0029,0x30, 0x03},
{0x001C,0x002E,0x32, 0x03},
{0x00B9,0x0031,0x01, 0x03},
{0x0083,0x0032,0x05, 0x03},
{0x0095,0x003A,0x12, 0x03},
{0x000D,0x003F,0x07, 0x03},
{0x0020,0x0041,0x33, 0x03},
{0x0092,0x0045,0x2C, 0x03},
{0x00D4,0x0047,0x08, 0x03},
{0x00A1,0x004B,0x2D, 0x03},
{0x00D2,0x004B,0x3B, 0x03},
{0x00D6,0x0052,0x24, 0x03},
{0x009A,0x005F,0x1C, 0x03},
{0x0016,0x0060,0x3D, 0x03},
{0x001A,0x0063,0x1F, 0x03},
{0x00CD,0x0066,0x28, 0x03},
{0x00FF,0x0067,0x10, 0x03},
{0x0035,0x0069,0x20, 0x03},
{0x008F,0x006C,0x04, 0x03},
{0x00CA,0x006C,0x2A, 0x03},
{0x005A,0x0074,0x09, 0x03},
{0x0060,0x0078,0x38, 0x03},
{0x0072,0x0079,0x1E, 0x03},
{0x0037,0x007F,0x29, 0x03},
{0x0012,0x0080,0x14, 0x03},
{0x0029,0x0082,0x2B, 0x03},
{0x0084,0x0098,0x36, 0x03},
{0x0032,0x0099,0x37, 0x03},
{0x00BB,0x00A0,0x19, 0x03},
{0x003E,0x00A3,0x3E, 0x03},
{0x004A,0x00A6,0x1A, 0x03},
{0x0029,0x00A7,0x21, 0x03},
{0x009D,0x00B7,0x22, 0x03},
{0x006C,0x00B9,0x15, 0x03},
{0x000C,0x00C0,0x0A, 0x03},
{0x00C2,0x00C3,0x0F, 0x03},
{0x002F,0x00C9,0x0D, 0x03},
{0x00D2,0x00CE,0x16, 0x03},
{0x00F3,0x00CE,0x0B, 0x03},
{0x0075,0x00CF,0x27, 0x03},
{0x001A,0x00D5,0x35, 0x03},
{0x0026,0x00D6,0x39, 0x03},
{0x0080,0x00DA,0x3C, 0x03},
{0x00A9,0x00DD,0x00, 0x03},
{0x00BC,0x00EB,0x03, 0x03},
{0x0032,0x00EF,0x1B, 0x03},
{0x0067,0x00F0,0x3F, 0x03},
{0x00EF,0x00F1,0x18, 0x03},
{0x00A8,0x00F3,0x0C, 0x03},
{0x00DE,0x00F9,0x1D, 0x03},
{0x002C,0x00FA,0x13, 0x03}
};

#define MAX_STARS 252
#define STARS_COLOR_BASE 32
static int stars_scrollx;
static UINT8 *map_stars;

static void draw_stars(  )
{
    /* draw the stars */

    /* $a005 controls the stars ON/OFF */

    int star_cntr;
    int set_a, set_b;

    /* two sets of stars controlled by these bits */

    set_a = galaga_starcontrol[3];
    set_b = galaga_starcontrol[4] | 0x2;


    for (star_cntr = 0;star_cntr < MAX_STARS ;star_cntr++)
    {
	int x,y;

	if   ( (set_a == star_seed_tab[star_cntr].set) ||  ( set_b == star_seed_tab[star_cntr].set) )
	{

	    x = (star_seed_tab[star_cntr].x + stars_scrollx) % 256;
	    y = (112 + star_seed_tab[star_cntr].y ) % 256 + 16;
	    if (x < 0) x+= 256;
	    x += 32;
	    /* 112 is a tweak to get alignment about perfect */



	    if (x >= 0 && x <= 36*8)
	    {
		switch(display_cfg.bpp) {
		case 8:
		    *(GameBitmap->line[x]+y) = map_stars[star_seed_tab[ star_cntr ].col]; break;
		case 15:
		case 16:
		    WriteWord(GameBitmap->line[x]+y*2,ReadWord(&map_stars[star_seed_tab[ star_cntr ].col*2])); break;
		case 32:
		    WriteLong(GameBitmap->line[x]+y*4,ReadLong(&map_stars[star_seed_tab[ star_cntr ].col*4])); break;
		}
	    }
	}

    }
    int s0,s1,s2;
    int speeds[8] = { -1, -2, -3, 0, 3, 2, 1, 0 };


    s0 = galaga_starcontrol[0];
    s1 = galaga_starcontrol[1];
    s2 = galaga_starcontrol[2];
    LOG("starcontrol: s0 %d s1 %d s2 %d\n",s0,s1,s2);

    stars_scrollx -= speeds[s0 + s1*2 + s2*4];
}

static int get_offset(UINT32 row, UINT32 col) {
    int offs;

    row += 2;
    col -= 2;
    if (col & 0x20)
	offs = row + ((col & 0x1f) << 5);
    else
	offs = col + (row << 5);

    return offs;
}

static void draw_galaga() {
    UINT8 *map;

    if (RefreshBuffers) {
	MAP_PALETTE_MAPPED_NEW(0x20*4, 64, map_stars);
    }
    if (galaga_starcontrol[5]) { // stars enabled
	clear_game_screen(0); // in this case clear the screen 1st, stars must be drawn on a cleared screen
	draw_stars();
    }

    if (check_layer_enabled(layer_id_data[0])) { // text layer
	for (int x=0; x < 36; x++) {
	    for (int y=0; y<28; y++) {
		// Forget any macro to convert these x,y to offset since it doesn't use a power of 2 for resolution... !
		// thanks to mame for the conversion... !
		int offset = get_offset(y,x);
		int code = videoram[offset] & 0x7f;
		int color = videoram[offset + 0x400] & 0x3f;
		MAP_PALETTE_MAPPED_NEW(color,4,map);
		if (galaga_starcontrol[5]) {
		    // if stars are drawn, then draw the text layer as transparant sprites on top of it
		    if (gfx1_solid[code]) {
			if (gfx1_solid[code] == 2)
			    Draw8x8_Mapped_Rot(&GFX[code << 6],x*8+16,y*8+16,map);
			else
			    Draw8x8_Trans_Mapped_Rot(&GFX[code << 6],x*8+16,y*8+16,map);
		    }
		} else
		    Draw8x8_Mapped_Rot(&GFX[code << 6],x*8+16,y*8+16,map); // otherwise draw it opaque
	    }
	}
    }

    if (check_layer_enabled(layer_id_data[1])) {
	// Who would have believed galaga had some 16x16 sprites, and in blocks
	// it's almost comparable to the cave system, minus the zoom !
	int offs;

	for (offs = 0;offs < 0x80;offs += 2)
	{
		static const int gfx_offs[2][2] =
		{
			{ 0, 1 },
			{ 2, 3 }
		};
		int sprite = spriteram[offs] & 0x7f;
		int color = spriteram[offs+1] & 0x3f;
		int sx = spriteram_2[offs+1] - 40 + 0x100*(spriteram_3[offs+1] & 3);
		int sy = 256 - spriteram_2[offs] + 1;	// sprites are buffered and delayed by one scanline
		int flipx = (spriteram_3[offs] & 0x01);
		int flipy = (spriteram_3[offs] & 0x02) >> 1;
		int sizex = (spriteram_3[offs] & 0x04) >> 2;
		int sizey = (spriteram_3[offs] & 0x08) >> 3;
		int x,y;

		sy -= 16 * sizey;
		sy = (sy & 0xff) - 32;	// fix wraparound

		/* if (flip_screen_get(machine))
		{
			flipx ^= 1;
			flipy ^= 1;
			sy += 48;
		} */

		for (y = 0;y <= sizey;y++)
		{
			for (x = 0;x <= sizex;x++)
			{
			    int code = sprite + gfx_offs[y ^ (sizey * flipy)][x ^ (sizex * flipx)];
			    if (gfx_solid[1][code]) {
				MAP_PALETTE_MAPPED_NEW(0x40|color, 4, map);

				int mx = sx +16*x + 16, my = sy + 16*y + 16;
				if (mx >= 0 && mx < 36*8+16 && my >= 0 && my < 28*8+16) {
				    if (gfx_solid[1][code] == 2) { // opaque
					Draw16x16_Mapped_flip_Rot(&gfx[1][code<<8],mx, my, map, flipx + flipy*2);
				    } else {
					Draw16x16_Trans_Mapped_flip_Rot(&gfx[1][code<<8],mx, my, map, flipx + flipy*2);
				    }
				}
			    }
			}
		}
	}
    }
}

static void execute_galaga() {
    int freq = 18432000/6;
    /* Super mess ! What could be found :
     * with SLICES too low the game hangs at the end of the hardware test, 35 seems to be the minimum to pass (well it hangs at 30, didn't test between 30 & 35),
     * there doesn't seem to be any difference for any value > that, even 129 !
     * The 2 nmis generated on Z80_2 determine music speed.
     * The nmi generated on cpu1 is very special, it must be more than 1 / frame, but after that it doesn't seem to matter ? it seems related to controls anyway.
     * Anyway the game is totally broken for now, it detects the credits, but the game never starts, stages advance automatically without doing anything, no ship on screen.
     * It looks like some communication problem between the 3 cpus, but it's a hard one !!! */
#define NB_SLICES 35
    for (int slice = 0; slice < NB_SLICES; slice++) {
	cpu_execute_cycles(CPU_Z80_0, freq / 60 / NB_SLICES);
	cpu_execute_cycles(CPU_Z80_1, freq / 60 / NB_SLICES);
	if (irq_enabled[2] && (slice == NB_SLICES/2 || slice == 0)) {
	    // 2 nmis / frame, and make sure the cpu has passed the ldir in b7 (inifinite loop in b9)
	    if (cpu_get_pc(CPU_Z80_2)==0xb9) {
		LOG("nmi cpu2\n");
		cpu_int_nmi(CPU_Z80_2);
	    } else {
		LOG("cpu2: pc = %x (no nmi)\n",cpu_get_pc(CPU_Z80_2));
	    }
	}
	cpu_execute_cycles(CPU_Z80_2, freq / 60 / NB_SLICES);
	if (slice == NB_SLICES-2) {
	    if (irq_enabled[0]) {
		LOG("irq0\n");
		cpu_interrupt(CPU_Z80_0,0x38);
	    }
	    if (irq_enabled[1]) {
		LOG("irq1\n");
		cpu_interrupt(CPU_Z80_1,0x38);
	    }
	}
	if (nmi_enable) { //  && slice % 2 == 0)
	    // if slice % 10 or higher -> starts a game without inserting a credit with 35 slices, but controls are broken
	    // slice % 8 or no test at all (nmi on all the slices) -> no change at all !
	    // After checking the z80 code : this weird nmi transfers 3 bytes / frame, 1 byte / nmi. After the 3 bytes have been transfered the nmis are stopped
	    // then the next irq restores the original values so that the 3 bytes can be transfered again. It was probably some way to check the namcoio chip stayed in sync
	    // because if the irq doesn't find the expected values, it just triggers a reset !
	    // So well leaving this for every slice is ok, it will execute only 3 times / frame anyway
	    LOG("nmi from nmi_enable (cpu0)\n");
	    cpu_int_nmi(CPU_Z80_0);
	}
    }
    // if (irq_enabled[2]) cpu_int_nmi(CPU_Z80_2);
#if 0
    watchdog--;
    if (watchdog == 0) {
	LOG("reset z80\n");
	cpu_reset(CPU_Z80_0);
	cpu_reset(CPU_Z80_1);
	cpu_reset(CPU_Z80_2);
	watchdog = 8;
    }
#endif
}

static struct VIDEO_INFO video_galaga =
{
   draw_galaga,
   36*8,
   28*8,
   16,
   VIDEO_ROTATE_90 |
   VIDEO_ROTATABLE,
   gfx_galaga,
   60.606060
};

#if 0
static struct namco_54xx_interface namco_54xx_interface =
{
	{ RES_K(150),	RES_K(47),		RES_K(100) },	/* R42, R33, R24 */
	{ RES_K(22),	RES_K(10),		RES_K(22) },	/* R41, R34, R23 */
	{ RES_K(470),	RES_K(150),		RES_K(220) },	/* R40, R35, R22 */
	{ RES_K(10),	RES_K(33),		RES_K(33)},		/* R37, R36, R21 */
	{ CAP_U(.01),	CAP_U(.01),		CAP_U(.001) },	/* C27, C29, C31 */
	{ CAP_U(.01),	CAP_U(.01),		CAP_U(.001) },	/* C26, C28, C30 */
};
#endif

GMEI( galaga,
     "Galaga",
     NAMCO,
     1981,
     GAME_SHOOT);

