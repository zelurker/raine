/*

   YMF278B  FM + Wave table Synthesizer (OPL4)

   Timer and PCM YMF278B.  The FM will be shared with the ymf262, eventually.

   This chip roughly splits the difference between the Sega 315-5560 MultiPCM
   (Multi32, Model 1/2) and YMF 292-F SCSP (later Model 2, STV, Saturn, Model 3).

   Features as listed in LSI-4MF2782 data sheet:
    FM Synthesis (same as YMF262)
     1. Sound generation mode
         Two-operater mode
          Generates eighteen voices or fifteen voices plus five rhythm sounds simultaneously
         Four-operator mode
          Generates six voices in four-operator mode plus six voices in two-operator mode simultaneously,
          or generates six voices in four-operator mode plus three voices in two-operator mode plus five
          rhythm sounds simultaneously
     2. Eight selectable waveforms
     3. Stereo output
    Wave Table Synthesis
     1. Generates twenty-four voices simultaneously
     2. 44.1kHz sampling rate for output sound data
     3. Selectable from 8-bit, 12-bit and 16-bit word lengths for wave data
     4. Stereo output (16-stage panpot for each voice)
    Wave Data
     1. Accepts 32M bit external memory at maximum
     2. Up to 512 wave tables
     3. External ROM or SRAM can be connected. With SRAM connected, the CPU can download wave data
     4. Outputs chip select signals for 1Mbit, 4Mbit, 8Mbit or 16Mbit memory
     5. Can be directly connected to the Yamaha YRW801 (Wave data ROM)
        Features of YRW801 as listed in LSI 4RW801A2
          Built-in wave data of tones which comply with GM system Level 1
           Melody tone ....... 128 tones
           Percussion tone ...  47 tones
          16Mbit capacity (2,097,152word x 8)

   By R. Belmont and O. Galibert.

   Copyright R. Belmont and O. Galibert.

   This software is dual-licensed: it may be used in MAME and properly licensed
   MAME derivatives under the terms of the MAME license.  For use outside of
   MAME and properly licensed derivatives, it is available under the
   terms of the GNU Lesser General Public License (LGPL), version 2.1.
   You may read the LGPL at http://www.gnu.org/licenses/lgpl.html

   Changelog:
   Sep. 8, 2002 - fixed ymf278b_compute_rate when octave is negative (RB)
   Dec. 11, 2002 - added ability to set non-standard clock rates (RB)
                   fixed envelope target for release (fixes missing
		   instruments in hotdebut).
                   Thanks to Team Japump! for MP3s from a real PCB.
		   fixed crash if MAME is run with no sound.
   June 4, 2003 -  Changed to dual-license with LGPL for use in openMSX.
                   openMSX contributed a bugfix where looped samples were
 		    not being addressed properly, causing pitch fluctuation.

   With further improvements over the years by MAME team.

   TODO:
   - accurate timing of envelopes
   - LFO (vibrato, tremolo)
   - integrate YMF262 (used by Fuuki games, not used by Psikyo and Metro games)
   - able to hook up "Moonsound", supporting mixed ROM+RAM (for MSX driver in MESS)
     (this should be possible now by using a custom address map?)
*/

#include <math.h>
#include <string.h>
#include "driver.h"
#include "ymf278b.h"
#include "sasound.h"
#include "timer.h"
#include "loadroms.h"
#include "streams.h"
#include "savegame.h"
#include "compat.h"

#undef VERBOSE
#define YMF278B_STD_CLOCK (33868800)            /* standard clock for OPL4 */

typedef struct
{
	INT16 wave;		/* wavetable number */
	INT16 F_NUMBER;		/* f-number */
	INT8 octave;		/* octave */
	INT8 preverb;		/* pseudo-reverb */
	INT8 DAMP;	        /* damping */
	INT8 CH;	        /* output channel */
	INT8 LD;		/* level direct */
	INT8 TL;		/* total level */
	INT8 pan;		/* panpot */
	INT8 LFO;		/* LFO */
	INT8 VIB;		/* vibrato */
	INT8 AM;		/* AM level */

	INT8 AR;
	INT8 D1R;
	INT8 DL;
	INT8 D2R;
	INT8 RC;   		/* rate correction */
	INT8 RR;

	UINT32 step;	/* fixed-point frequency step */
	UINT32 stepptr;	/* fixed-point pointer into the sample */

	INT8 active;    /* channel is playing */
	INT8 KEY_ON;    /* slot keyed on */
	INT8 bits;		/* width of the samples */
	UINT32 startaddr;
	UINT32 loopaddr;
	UINT32 endaddr;

	int env_step;
	UINT32 env_vol;
	UINT32 env_vol_step;
	UINT32 env_vol_lim;
	INT8 env_preverb;

	int num;        /* slot number (for debug only) */
} YMF278BSlot;

typedef struct
{
	UINT8 pcmregs[256];
	YMF278BSlot slots[24];
	INT8 lsitest0;
	INT8 lsitest1;
	INT8 wavetblhdr;
	INT8 memmode;
	INT32 memadr;

	UINT8 status_busy, status_ld;
	void *timer_busy;
	void *timer_ld;
	UINT8 exp;

	INT32 fm_l, fm_r;
	INT32 pcm_l, pcm_r;

	double timer_base;
	UINT8 timer_a_count, timer_b_count, enable, current_irq;
	void *timer_a, *timer_b;
	int clock;
	int irq_line;

	UINT8 port_AB, port_C,lastport;
	void (*irq_callback)(int);

	const UINT8 *rom;

	// precomputed tables
	UINT32 lut_ar[64];              // attack rate
	UINT32 lut_dr[64];              // decay rate
	INT32 volume[256*4];			// precalculated attenuation values with some marging for enveloppe and pan levels
	int pan_left[16], pan_right[16];	// pan volume offsets
	INT32 mix_level[8];

	int index;
} YMF278BChip;

static YMF278BChip YMF278B[MAX_YMF278B];

static int compute_rate(YMF278BSlot *slot, int val)
{
	int res, oct;

	if(val == 0)
		return 0;
	if(val == 15)
		return 63;
	if(slot->RC != 15)
	{
		oct = slot->octave;
		if (oct & 8)
			oct |= -8;

		res = (oct+slot->RC)*2 + (slot->F_NUMBER & 0x200 ? 1 : 0) + val*4;
	}
	else
		res = val * 4;
	if(res < 0)
		res = 0;
	else if(res > 63)
		res = 63;

	return res;
}

static UINT32 compute_decay_env_vol_step(YMF278BChip *chip,YMF278BSlot *slot, int val)
{
	int rate;
	UINT32 res;

	// rate override with damping/pseudo reverb
	if (slot->DAMP)
		rate = 56; // approximate, datasheet says it's slightly curved though
	else if (slot->preverb && slot->env_vol > ((6*8)<<23))
	{
		// pseudo reverb starts at -18dB (6 in voltab)
		slot->env_preverb = 1;
		rate = 5;
	}
	else
		rate = compute_rate(slot, val);

	if (rate < 4)
		res = 0;
	else
		res = (256U<<23) / chip->lut_dr[rate];

	return res;
}

static void compute_freq_step(YMF278BSlot *slot)
{
	UINT32 step;
	int oct;

	oct = slot->octave;
	if(oct & 8)
		oct |= -8;

	step = (slot->F_NUMBER | 1024) << (oct + 8);
	slot->step = step >> 3;
}

static void compute_envelope(YMF278BChip *chip,YMF278BSlot *slot)
{
	switch (slot->env_step)
	{
		// Attack
		case 0:
		{
			// Attack
			int rate = compute_rate(slot, slot->AR);
			slot->env_vol = 256U<<23;
			slot->env_vol_lim = (256U<<23) - 1;

			if (rate==63)
			{
				// immediate
				print_debug("YMF278B: Attack skipped - ");
				slot->env_vol = 0;
				slot->env_step++;
				compute_envelope(chip,slot);
			}
			else if (rate<4)
			{
				slot->env_vol_step = 0;
			}
			else
			{
				// NOTE: attack rate is linear here, but datasheet shows a smooth curve
				print_debug("YMF278B: Attack, val = %d, rate = %d, delay = %g\n", slot->AR, rate, chip->lut_ar[rate]*1000.0);
				slot->env_vol_step = ~((256U<<23) / chip->lut_ar[rate]);
			}

			break;
		}

		// Decay 1
		case 1:
			if(slot->DL)
			{
				print_debug("YMF278B: Decay step 1, dl=%d, val = %d rate = %d, delay = %g, preverb = %d, DAMP = %d\n", slot->DL, slot->D1R, compute_rate(slot, slot->D1R), chip->lut_dr[compute_rate(slot, slot->D1R)]*1000.0, slot->preverb, slot->DAMP);
				slot->env_vol_step = compute_decay_env_vol_step(chip,slot, slot->D1R);
				slot->env_vol_lim = (slot->DL*8)<<23;
			}
			else
			{
				print_debug("YMF278B: Decay 1 skipped - ");
				slot->env_step++;
				compute_envelope(chip,slot);
			}

			break;

		// Decay 2
		case 2:
			print_debug("YMF278B: Decay step 2, val = %d, rate = %d, delay = %g, , preverb = %d, DAMP = %d, current vol = %d\n", slot->D2R, compute_rate(slot, slot->D2R), chip->lut_dr[compute_rate(slot, slot->D2R)]*1000.0, slot->preverb, slot->DAMP, slot->env_vol >> 23);
			slot->env_vol_step = compute_decay_env_vol_step(chip,slot, slot->D2R);
			slot->env_vol_lim = 256U<<23;
			break;

		// Decay 2 reached -96dB
		case 3:
			print_debug("YMF278B: Voice cleared because of decay 2\n");
			slot->env_vol = 256U<<23;
			slot->env_vol_step = 0;
			slot->env_vol_lim = 0;
			slot->active = 0;
			break;

		// Release
		case 4:
			print_debug("YMF278B: Release, val = %d, rate = %d, delay = %g, preverb = %d, DAMP = %d\n", slot->RR, compute_rate(slot, slot->RR), chip->lut_dr[compute_rate(slot, slot->RR)]*1000.0, slot->preverb, slot->DAMP);
			slot->env_vol_step = compute_decay_env_vol_step(chip,slot, slot->RR);
			slot->env_vol_lim = 256U<<23;
			break;

		// Release reached -96dB
		case 5:
			print_debug("YMF278B: Release ends\n");
			slot->env_vol = 256U<<23;
			slot->env_vol_step = 0;
			slot->env_vol_lim = 0;
			slot->active = 0;
			break;

		default: break;
	}
}

static void ymf278b_pcm_update(int num, INT16 **outputs, int samples)
{
	YMF278BChip *chip = &YMF278B[num];
	int i, j;
	YMF278BSlot *slot = NULL;
	INT16 sample = 0;
	const UINT8 *rombase;
	INT32 mix[44100*2];
	INT32 *mixp;
	INT32 vl, vr;

	memset(mix, 0, sizeof(mix[0])*samples*2);

	rombase = chip->rom;

	for (i = 0; i < 24; i++)
	{
		slot = &chip->slots[i];

		if (slot->active)
		{
			mixp = mix;

			for (j = 0; j < samples; j++)
			{
				if (slot->stepptr >= slot->endaddr)
				{
					slot->stepptr = slot->stepptr - slot->endaddr + slot->loopaddr;

					// NOTE: loop overflow is still possible here if (slot->stepptr >= slot->endaddr)
					// This glitch may be (ab)used to your advantage to create pseudorandom noise.
				}

			  switch (slot->bits)
			    {
					// 8 bit
					case 0:
					    sample = rombase[slot->startaddr + (slot->stepptr>>16)]<<8;
			      break;

					// 12 bit
					case 1:
						if (slot->stepptr & 0x10000)
							sample = rombase[slot->startaddr + (slot->stepptr>>17)*3+2]<<8 |
								(rombase[slot->startaddr + (slot->stepptr>>17)*3+1] << 4 & 0xf0);
						else
							sample = rombase[slot->startaddr + (slot->stepptr>>17)*3]<<8 |
								(rombase[slot->startaddr + (slot->stepptr>>17)*3+1] & 0xf0);
						break;

					// 16 bit
					case 2:
						sample = rombase[slot->startaddr + ((slot->stepptr>>16)*2)]<<8 |
							rombase[slot->startaddr + ((slot->stepptr>>16)*2)+1];
						break;

					// ?? bit, effect is unknown, datasheet says it's prohibited
					case 3:
						sample = 0;
						break;
			    }

			  *mixp++ += (sample * chip->volume[slot->TL+chip->pan_left [slot->pan]+(slot->env_vol>>23)])>>17;
			  *mixp++ += (sample * chip->volume[slot->TL+chip->pan_right[slot->pan]+(slot->env_vol>>23)])>>17;

			  // update frequency
			  slot->stepptr += slot->step;

			  // update envelope
			  slot->env_vol += slot->env_vol_step;
			  if (((INT32)(slot->env_vol - slot->env_vol_lim)) >= 0)
				{
					slot->env_step++;
					compute_envelope(chip,slot);
				}
				else if (slot->preverb && !slot->env_preverb && slot->env_step && slot->env_vol > ((6*8)<<23))
					compute_envelope(chip,slot);
			}
		}
	}

	mixp = mix;
	vl = chip->mix_level[chip->pcm_l];
	vr = chip->mix_level[chip->pcm_r];
	for (i = 0; i < samples; i++)
	  {
	    outputs[0][i] = (*mixp++ * vl) >> 16;
	    outputs[1][i] = (*mixp++ * vr) >> 16;
	  }
}

static void irq_check(YMF278BChip *chip)
{
	int prev_line = chip->irq_line;
	chip->irq_line = chip->current_irq ? ASSERT_LINE : CLEAR_LINE;
	if(chip->irq_line != prev_line && chip->irq_callback)
		chip->irq_callback(chip->irq_line);
}

void ymf278b_timer_ld_clear(int i)
{
    YMF278BChip *chip = &YMF278B[i];
    chip->status_ld = 0;
}

void ymf278b_timer_busy_clear(int i)
{
    YMF278BChip *chip = &YMF278B[i];
    chip->status_busy = 0;
}

void ymf278b_timer_a_tick(int i)
{
    YMF278BChip *chip = &YMF278B[i];
	if(!(chip->enable & 0x40))
	{
		chip->current_irq |= 0x40;
		irq_check(chip);
	}
}

void ymf278b_timer_b_tick(int i)
{
    YMF278BChip *chip = &YMF278B[i];
	if(!(chip->enable & 0x20))
	{
		chip->current_irq |= 0x20;
		irq_check(chip);
	}
}

static void timer_a_reset(YMF278BChip *chip)
{
  if(chip->enable & 1)
    {
      double period = TIME_IN_NSEC((256-chip->timer_a_count) * 80800);
      timer_readjust(chip->timer_a, period, ymf278b_timer_a_tick);
    }
  else if (chip->timer_a) {
    timer_remove(chip->timer_a);
    chip->timer_a = NULL;
  }
}

static void timer_b_reset(YMF278BChip *chip)
{
  if(chip->enable & 2)
    {
      double period = TIME_IN_NSEC((256-chip->timer_b_count) * 323100);
      timer_readjust(chip->timer_b, period, ymf278b_timer_b_tick);
    }
  else if (chip->timer_b) {
    timer_remove(chip->timer_b);
    chip->timer_b = NULL;
  }
}

static void A_w(YMF278BChip *chip, UINT8 reg, UINT8 data)
{
	if (!audio_sample_rate) return;

	// FM register array 0 (compatible with YMF262)
	switch(reg)
	{
		// LSI TEST
		case 0x00:
		case 0x01:
			break;

		// timer a count
		case 0x02:
			if (data != chip->timer_a_count)
			{
				chip->timer_a_count = data;

				// change period, ~80.8us * t
				if (chip->enable & 1) {
				    double period = chip->timer_base * (256-data) * 4;
				    timer_readjust(chip->timer_a, period, ymf278b_timer_a_tick);
				}
			}
			break;

		// timer b count
		case 0x03:
			if (data != chip->timer_b_count)
			{
				chip->timer_b_count = data;

				// change period, ~323.1us * t
				if (chip->enable & 2) {
				    double period = chip->timer_base * (256-data) * 16;
				    timer_readjust(chip->timer_b, period, ymf278b_timer_b_tick);
				}
			}
			break;

		// timer control
		case 0x04:
			if(data & 0x80)
				chip->current_irq = 0;
			else
			{
				// reset timers
				if((chip->enable ^ data) & 1)
				{
					double period = (data & 1) ? chip->timer_base * (256-chip->timer_a_count) * 4 : -1;
					timer_readjust(chip->timer_a, period, ymf278b_timer_a_tick);
				}
				if((chip->enable ^ data) & 2)
				{
					double period = (data & 2) ? chip->timer_base * (256-chip->timer_b_count) * 16 : -1;
					timer_readjust(chip->timer_b, period, ymf278b_timer_b_tick);
				}

				chip->enable = data;
				chip->current_irq &= ~data;
			}
			irq_check(chip);
			break;
#ifdef VERBOSE
		default:
			logerror("YMF278B:  Port A write %02x, %02x\n", reg, data);
#endif
	}
}

static void B_w(YMF278BChip *chip, UINT8 reg, UINT8 data)
{
	// FM register array 1 (compatible with YMF262)
	switch(reg)
	{
		// LSI TEST
		case 0x00:
		case 0x01:
			break;

		// expansion register (NEW2/NEW)
		case 0x05:
			chip->exp = data;
			break;

		default:
			logerror("YMF278B:  Port B write %02x, %02x\n", reg, data);
			break;
	}
}

static void retrigger_note(YMF278BChip *chip,YMF278BSlot *slot)
{
	// activate channel
	if (slot->octave != 8)
		slot->active = 1;

	// reset sample pos and go to attack stage
	slot->stepptr = 0;
	slot->env_step = 0;
	slot->env_preverb = 0;

	compute_freq_step(slot);
	compute_envelope(chip,slot);
}

static void C_w(YMF278BChip *chip, UINT8 reg, UINT8 data)
{
	if (!audio_sample_rate) return;

	// Handle slot registers specifically
	if (reg >= 0x08 && reg <= 0xf7)
	{
		YMF278BSlot *slot = NULL;
		int snum;
		snum = (reg-8) % 24;
		slot = &chip->slots[snum];
		switch((reg-8) / 24)
		{
			case 0:
			{
				double period;
				UINT32 offset;
				UINT8 p[12];
				int i;

				slot->wave &= 0x100;
				slot->wave |= data;

				// load wavetable header
				if(slot->wave < 384 || !chip->wavetblhdr)
					offset = slot->wave * 12;
				else
					offset = chip->wavetblhdr*0x80000 + (slot->wave - 384) * 12;
				for (i = 0; i < 12; i++)
					p[i] = chip->rom[offset+i];

				slot->bits = (p[0]&0xc0)>>6;
				slot->startaddr = (p[2] | (p[1]<<8) | ((p[0]&0x3f)<<16));
				slot->loopaddr = (p[4]<<16) | (p[3]<<24);
				slot->endaddr = (p[6]<<16) | (p[5]<<24);
				slot->endaddr -= 0x00010000U;
				slot->endaddr ^= 0xffff0000U;

				// copy internal registers data
				for (i = 7; i < 12; i++)
					C_w(chip,8 + snum + (i-2) * 24, p[i]);

				// status register LD bit is on for approx 300us
				chip->status_ld = 1;
				period = TIME_IN_USEC(300);
				if (chip->clock != YMF278B_STD_CLOCK)
					period = (period * chip->clock) / YMF278B_STD_CLOCK;
				timer_readjust(chip->timer_ld,period,ymf278b_timer_ld_clear);

				// retrigger if key is on
				if (slot->KEY_ON)
					retrigger_note(chip,slot);
				else if (slot->active)
				{
					// deactivate channel
					slot->env_step = 5;
					compute_envelope(chip,slot);
				}

				break;
			}

			case 1:
				slot->wave &= 0xff;
				slot->wave |= ((data&0x1)<<8);
				slot->F_NUMBER &= 0x380;
				slot->F_NUMBER |= (data>>1);
				if (slot->active && (data ^ chip->pcmregs[reg]) & 0xfe)
				{
					compute_freq_step(slot);
					compute_envelope(chip,slot);
				}
				break;
			case 2:
				slot->F_NUMBER &= 0x07f;
				slot->F_NUMBER |= ((data&0x07)<<7);
				slot->preverb = (data&0x8)>>3;
				slot->octave = (data&0xf0)>>4;
				if (data != chip->pcmregs[reg])
				{
					// channel goes off if octave is set to -8 (datasheet says it's prohibited)
					// (it is ok if this activates the channel while it was off: compute_envelope will reset it again if needed)
					slot->active = (slot->octave != 8);

					if (slot->active)
					{
						slot->env_preverb = 0;
						compute_freq_step(slot);
						compute_envelope(chip,slot);
					}
				}
				break;
			case 3:
				slot->TL = data>>1;
				slot->LD = data&0x1;
				break;
			case 4:
				slot->CH = (data&0x10)>>4;
				// CH bit note: output to DO1 pin (1) or DO2 pin (0), this may
				// silence the channel depending on how it's wired up on the PCB.
				// For now, it's always enabled.
				// (bit 5 (LFO reset) is also not hooked up yet)

				slot->pan = data&0xf;
				slot->DAMP = (data&0x40)>>6;
				if (data & 0x80)
				{
					// don't retrigger if key was already on
					if (slot->KEY_ON)
					{
						if ((data ^ chip->pcmregs[reg]) & 0x40)
							compute_envelope(chip,slot);

						break;
					}

					retrigger_note(chip,slot);
				}
				else if (slot->active)
				{
					// release
					slot->env_step = 4;
					compute_envelope(chip,slot);
				}
				slot->KEY_ON = (data&0x80)>>7;
				break;
			case 5:
				// LFO and vibrato level, not hooked up yet
				slot->LFO = (data>>3)&0x7;
				slot->VIB = data&0x7;
				break;
			case 6:
				slot->AR = data>>4;
				slot->D1R = data&0xf;
				if (slot->active && data != chip->pcmregs[reg])
					compute_envelope(chip,slot);
				break;
			case 7:
				slot->DL = data>>4;
				slot->D2R = data&0xf;
				if (slot->active && data != chip->pcmregs[reg])
					compute_envelope(chip,slot);
				break;
			case 8:
				slot->RC = data>>4;
				slot->RR = data&0xf;
				if (slot->active && data != chip->pcmregs[reg])
					compute_envelope(chip,slot);
				break;
			case 9:
				// tremolo level, not hooked up yet
				slot->AM = data & 0x7;
				break;
		}
	}
	else
	{
		// All non-slot registers
		switch (reg)
		{
			// LSI TEST
			case 0x00:
			case 0x01:
				break;

			case 0x02:
				chip->wavetblhdr = (data>>2)&0x7;
				chip->memmode = data&3;
				break;

			case 0x03:
				data &= 0x3f; // !
				break;
			case 0x04:
				break;
			case 0x05:
				// set memory address
				chip->memadr = chip->pcmregs[3] << 16 | chip->pcmregs[4] << 8 | data;
				break;

			case 0x06:
				// memory data
				// chip->addrspace[0]->write_byte(chip->memadr, data);
				printf("would write to ymf278b ram addr %x, investigate !\n",chip->memadr);
				chip->memadr = (chip->memadr + 1) & 0x3fffff;
				break;

			case 0x07:
				break; // unused
			case 0xf8:
				chip->fm_l = data & 0x7;
				chip->fm_r = (data>>3)&0x7;
				break;

			case 0xf9:
				chip->pcm_l = data & 0x7;
				chip->pcm_r = (data>>3)&0x7;
				break;

			default:
				logerror("YMF278B:  Port C write %02x, %02x\n", reg, data);
				break;
		}
	}

	chip->pcmregs[reg] = data;
}

void timer_busy_start(YMF278BChip *chip, int is_pcm)
{
	// status register BUSY bit is on for 56(FM) or 88(PCM) cycles
	chip->status_busy = 1;
	timer_readjust(chip->timer_busy,TIME_IN_HZ(chip->clock / (is_pcm ? 88 : 56)),ymf278b_timer_busy_clear);
}

void ymf278b_w(int num, UINT32 offset, UINT8 data)
{
	YMF278BChip *chip = &YMF278B[num];

	switch (offset&7)
	{
		case 0:
		case 2:
			timer_busy_start(chip,0);
			chip->port_AB = data;
			chip->lastport = offset>>1 & 1;
			break;

		case 1:
		case 3:
			timer_busy_start(chip,0);
			if (chip->lastport) B_w(chip,chip->port_AB, data);
			else A_w(chip,chip->port_AB, data);
			break;

		case 4:
			timer_busy_start(chip,1);
			chip->port_C = data;
			break;

		case 5:
			// PCM regs are only accessible if NEW2 is set
			if (~chip->exp & 2)
				break;

			timer_busy_start(chip,1);
			C_w(chip,chip->port_C, data);
			break;

		default:
			logerror("unexpected write at offset %X to ymf278b = %02X\n", offset, data);
			break;
	}
}

UINT8 ymf278b_r(int num, UINT32 offset)
{
	YMF278BChip *chip = &YMF278B[num];
	UINT8 ret = 0;

	switch (offset & 7)
	{
		// status register
		case 0:
		{
			// bits 0 and 1 are only valid if NEW2 is set
			UINT8 newbits = 0;
			if (chip->exp & 2)
				newbits = (chip->status_ld << 1) | chip->status_busy;

			ret = newbits | chip->current_irq | (chip->irq_line ? 0x80 : 0x00);
			break;
		}

		// FM regs can be read too (on contrary to what the datasheet says)
		case 1:
		case 3:
			// but they're not implemented here yet
			break;

		// PCM regs
		case 5:
			// only accessible if NEW2 is set
			if (~chip->exp & 2)
				break;

			switch (chip->port_C)
			{
				// special cases
				case 2:
					ret = (chip->pcmregs[chip->port_C] & 0x1f) | 0x20; // device ID in upper bits
					break;
				case 6:
					ret = chip->rom[chip->memadr];
					chip->memadr = (chip->memadr + 1) & 0x3fffff;
					break;

				default:
					ret = chip->pcmregs[chip->port_C];
					break;
			}
			break;

		default:
			logerror("unexpected read at offset %X from ymf278b\n", offset);
			break;
	}

	return ret;
}

static UINT8 ymf278b_status_port_r(int num)
{
	return YMF278B[num].current_irq | (YMF278B[num].irq_line == ASSERT_LINE ? 0x80 : 0x00);
}

// Not implemented yet
static UINT8 ymf278b_data_port_r(int num)
{
	return 0;
}

static void ymf278b_control_port_A_w(int num, UINT8 data)
{
	YMF278B[num].port_AB = data;
}

static void ymf278b_data_port_A_w(int num, UINT8 data)
{
	A_w(&YMF278B[num], YMF278B[num].port_AB, data);
}

static void ymf278b_control_port_B_w(int num, UINT8 data)
{
	YMF278B[num].port_AB = data;
}

static void ymf278b_data_port_B_w(int num, UINT8 data)
{
	B_w(&YMF278B[num], YMF278B[num].port_AB, data);
}

static void ymf278b_control_port_C_w(int num, UINT8 data)
{
	YMF278B[num].port_C = data;
}

static void ymf278b_data_port_C_w(int num, UINT8 data)
{
	C_w(&YMF278B[num], YMF278B[num].port_C, data);
}

static void ymf278b_init(INT8 num, UINT8 *rom, void (*cb)(int), int clock)
{
    YMF278BChip *chip = &YMF278B[num];
    int i;

    chip->rom = rom;
    chip->irq_callback = cb;
    // clear registers
    for (i = 0; i <= 4; i++)
	A_w(chip,i, 0);
    B_w(chip,5, 0);
    for (i = 0; i < 8; i++)
	C_w(chip,i, 0);
    for (i = 0xff; i >= 8; i--)
	C_w(chip,i, 0);
    C_w(chip,0xf8, 0x1b);

    chip->port_AB = chip->port_C = 0;
    chip->lastport = 0;
    chip->memadr = 0;
    chip->clock = clock; // ?
    chip->timer_base = TIME_IN_HZ(chip->clock) * (19*36);

    // init/silence channels
    for (i = 0; i < 24 ; i++)
    {
	YMF278BSlot *slot = &chip->slots[i];

	slot->LFO = 0;
	slot->VIB = 0;
	slot->AR = 0;
	slot->D1R = 0;
	slot->DL = 0;
	slot->D2R = 0;
	slot->RC = 0;
	slot->RR = 0;
	slot->AM = 0;

	slot->startaddr = 0;
	slot->loopaddr = 0;
	slot->endaddr = 0;

	slot->env_step = 5;
	compute_envelope(chip,slot);
    }

    timer_a_reset(chip);
    timer_b_reset(chip);
    chip->timer_busy = NULL;  chip->status_busy = 0;
    chip->timer_ld = NULL;    chip->status_ld = 0;

    chip->irq_line = 0;
    chip->current_irq = 0;
    if (chip->irq_callback)
	chip->irq_callback(0);
}

static void precompute_rate_tables(YMF278BChip *chip)
{
	int i;

	// decay rate
	for (i = 0; i < 64; i++)
	{
		if (i <= 3)
			chip->lut_dr[i] = 0;
		else if (i >= 60)
			chip->lut_dr[i] = 15 << 4;
		else
			chip->lut_dr[i] = (15 << (21 - i / 4)) / (4 + i % 4);
	}

	// attack rate (manual shows curve instead of linear though, so this is not entirely accurate)
	for (i = 0; i < 64; i++)
	{
		if (i <= 3 || i == 63)
			chip->lut_ar[i] = 0;
		else if (i >= 60)
			chip->lut_ar[i] = 17;
		else
			chip->lut_ar[i] = (67 << (15 - i / 4)) / (4 + i % 4);
	}
}

static const struct YMF278B_interface *my_intf;

static void restore_ymf278b() {
    int i;
    for(i=0; i<my_intf->num; i++) {
	YMF278B[i].rom = load_region[my_intf->region[0]];
	YMF278B[i].irq_callback = my_intf->irq_callback[i];
    }
}

int YMF278B_sh_start( const struct YMF278B_interface *intf )
{
	char buf[2][40];
	const char *name[2];
	int  vol[2];
	int i;
	my_intf = intf;

	for(i=0; i<intf->num; i++)
	{
		sprintf(buf[0], "YMF278B %d L", i);
		sprintf(buf[1], "YMF278B %d R", i);
		name[0] = buf[0];
		name[1] = buf[1];
		vol[0]=intf->mixing_level[i] >> 16;
		vol[1]=intf->mixing_level[i] & 0xffff;
		ymf278b_init(i, load_region[intf->region[0]], intf->irq_callback[i], intf->clock[i]);
		YMF278BChip *chip = &YMF278B[i];
		chip->index = i;
		stream_init_multim(2, name, vol, audio_sample_rate, i, ymf278b_pcm_update);

		// rate tables
		precompute_rate_tables(chip);

		// Volume table, 1 = -0.375dB, 8 = -3dB, 256 = -96dB
		for(i = 0; i < 256; i++)
		    chip->volume[i] = 65536*pow(2.0, (-0.375/6)*i);
		for(i = 256; i < 256*4; i++)
		    chip->volume[i] = 0;

		// Pan values, units are -3dB, i.e. 8.
		for(i = 0; i < 16; i++)
		{
		    chip->pan_left[i] = i < 7 ? i*8 : i < 9 ? 256 : 0;
		    chip->pan_right[i] = i < 8 ? 0 : i < 10 ? 256 : (16-i)*8;
		}

		// Mixing levels, units are -3dB, and add some marging to avoid clipping
		for(i=0; i<7; i++)
		    chip->mix_level[i] = chip->volume[8*i+8];
		chip->mix_level[7] = 0;
	}


	save_timers();
	AddSaveData_ext("ymf278b timers", (UINT8*)YMF278B, sizeof(YMF278B));
	AddLoadCallback(restore_ymf278b);
	return 0;
}

void YMF278B_sh_stop( void )
{
}


READ8_HANDLER( YMF278B_status_port_0_r )
{
	return ymf278b_status_port_r(0);
}

READ8_HANDLER( YMF278B_data_port_0_r )
{
	return ymf278b_data_port_r(0);
}

WRITE8_HANDLER( YMF278B_control_port_0_A_w )
{
	ymf278b_control_port_A_w(0, data);
}

WRITE8_HANDLER( YMF278B_data_port_0_A_w )
{
	ymf278b_data_port_A_w(0, data);
}

WRITE8_HANDLER( YMF278B_control_port_0_B_w )
{
	ymf278b_control_port_B_w(0, data);
}

WRITE8_HANDLER( YMF278B_data_port_0_B_w )
{
	ymf278b_data_port_B_w(0, data);
}

WRITE8_HANDLER( YMF278B_control_port_0_C_w )
{
	ymf278b_control_port_C_w(0, data);
}

WRITE8_HANDLER( YMF278B_data_port_0_C_w )
{
	ymf278b_data_port_C_w(0, data);
}


READ8_HANDLER( YMF278B_status_port_1_r )
{
	return ymf278b_status_port_r(1);
}

READ8_HANDLER( YMF278B_data_port_1_r )
{
	return ymf278b_data_port_r(1);
}

WRITE8_HANDLER( YMF278B_control_port_1_A_w )
{
	ymf278b_control_port_A_w(1, data);
}

WRITE8_HANDLER( YMF278B_data_port_1_A_w )
{
	ymf278b_data_port_A_w(1, data);
}

WRITE8_HANDLER( YMF278B_control_port_1_B_w )
{
	ymf278b_control_port_B_w(1, data);
}

WRITE8_HANDLER( YMF278B_data_port_1_B_w )
{
	ymf278b_data_port_B_w(1, data);
}

WRITE8_HANDLER( YMF278B_control_port_1_C_w )
{
	ymf278b_control_port_C_w(1, data);
}

WRITE8_HANDLER( YMF278B_data_port_1_C_w )
{
	ymf278b_data_port_C_w(1, data);
}

