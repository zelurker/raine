// license:BSD-3-Clause
// copyright-holders:R. Belmont
/*****************************************************************************
 *
 *   sh2common.c
 *
 *   SH-2 non-specific components
 *
 *****************************************************************************/

#include "debug.h"
#include "sh2.h"
#include "sh2comn.h"
#include "handlers.h"
#include <stdio.h>
#include <stdlib.h> // exit
#include <string.h>
#include "savegame.h"

int sh2Engine;
sh2_context M_SH2;
sh2_context S_SH2;

#define VERBOSE 0

#define logerror print_debug
#define LOG(x)  do { if (VERBOSE) logerror x; } while (0)

// static const int div_tab[4] = { 3, 5, 7, 0 };

static inline UINT32 RL(sh2_context *sh2, u32 A)
{
#if 0
	if (A >= 0xe0000000) /* I/O */
		return sh2_internal_r(*sh2->internal, (A & 0x1fc)>>2, 0xffffffff);

	if (A >= 0xc0000000) /* Cache Data Array */
		return sh2->program->read_dword(A);

	/*  0x60000000 Cache Address Data Array */

	if (A >= 0x40000000) /* Cache Associative Purge Area */
		return 0xa5a5a5a5;

	/* 0x20000000 no Cache */
	/* 0x00000000 read thru Cache if CE bit is 1 */
	return sh2->program->read_dword(A & AM);
#else
	return sh2->Read_Long[A >> 24](A);
#endif
}

static inline void WL(sh2_context *sh2, u32 A, UINT32 V)
{
#if 0
	if (A >= 0xe0000000) /* I/O */
	{
		sh2_internal_w(*sh2->internal, (A & 0x1fc)>>2, V, 0xffffffff);
		return;
	}

	if (A >= 0xc0000000) /* Cache Data Array */
	{
		sh2->program->write_dword(A,V);
		return;
	}

	/*  0x60000000 Cache Address Data Array */

	if (A >= 0x40000000) /* Cache Associative Purge Area */
		return;

	/* 0x20000000 no Cache */
	/* 0x00000000 read thru Cache if CE bit is 1 */
	sh2->program->write_dword(A & AM,V);
#else
	sh2->Write_Long[A >> 24](A,V);
#endif
}

#if 0
static void sh2_timer_resync(sh2_context *sh2)
{
	int divider = div_tab[(sh2->m[5] >> 8) & 3];
	UINT64 cur_time = sh2->device->total_cycles();
	UINT64 add = (cur_time - sh2->frc_base) >> divider;

	if (add > 0)
	{
		if(divider)
			sh2->frc += add;

		sh2->frc_base = cur_time;
	}
}

static void sh2_timer_activate(sh2_context *sh2)
{
	int max_delta = 0xfffff;
	UINT16 frc;

	sh2->timer->adjust(attotime::never);

	frc = sh2->frc;
	if(!(sh2->m[4] & OCFA)) {
		UINT16 delta = sh2->ocra - frc;
		if(delta < max_delta)
			max_delta = delta;
	}

	if(!(sh2->m[4] & OCFB) && (sh2->ocra <= sh2->ocrb || !(sh2->m[4] & 0x010000))) {
		UINT16 delta = sh2->ocrb - frc;
		if(delta < max_delta)
			max_delta = delta;
	}

	if(!(sh2->m[4] & OVF) && !(sh2->m[4] & 0x010000)) {
		int delta = 0x10000 - frc;
		if(delta < max_delta)
			max_delta = delta;
	}

	if(max_delta != 0xfffff) {
		int divider = div_tab[(sh2->m[5] >> 8) & 3];
		if(divider) {
			max_delta <<= divider;
			sh2->frc_base = sh2->device->total_cycles();
			sh2->timer->adjust(sh2->device->cycles_to_attotime(max_delta));
		} else {
			logerror("SH2.%s: Timer event in %d cycles of external clock", "0", max_delta);
		}
	}
}


static TIMER_CALLBACK( sh2_timer_callback )
{
	sh2_context *sh2 = (sh2_context *)ptr;
	UINT16 frc;

	sh2_timer_resync(sh2);

	frc = sh2->frc;

	if(frc == sh2->ocrb)
		sh2->m[4] |= OCFB;

	if(frc == 0x0000)
		sh2->m[4] |= OVF;

	if(frc == sh2->ocra)
	{
		sh2->m[4] |= OCFA;

		if(sh2->m[4] & 0x010000)
			sh2->frc = 0;
	}

	sh2_recalc_irq(sh2);
	sh2_timer_activate(sh2);
}
#endif

/*
  We have to do DMA on a timer (or at least, in chunks) due to the way some systems use it.
  The 32x is a difficult case, they set the SOURCE of the DMA to a FIFO buffer, which at most
  can have 8 words in it.  Attempting to do an 'instant DMA' in this scenario is impossible
  because the game is expecting the 68k of the system to feed data into the FIFO at the same
  time as the SH2 is transfering it out via DMA

  There are two ways we can do this

  a) with a high frequency timer (more accurate, but a large performance hit)

  or

  b) in the CPU_EXECUTE loop


  we're currently doing a)

  b) causes problems with ST-V games

*/


#if 0
void sh2_notify_dma_data_available(device_t *device)
{
	sh2_context *sh2 = GET_SH2(device);
	//printf("call notify\n");

	for (int dma=0;dma<2;dma++)
	{
		//printf("sh2->dma_timer_active[dma] %04x\n",sh2->dma_timer_active[dma]);

		if (sh2->dma_timer_active[dma]==2) // 2 = stalled
		{
		//  printf("resuming stalled dma\n");
			sh2->dma_timer_active[dma]=1;
			sh2->dma_current_active_timer[dma]->adjust(attotime::zero, dma);
		}
	}

}

void sh2_do_dma(sh2_context *sh2, int dma)
{
	UINT32 dmadata;

	UINT32 tempsrc, tempdst;

	if (sh2->active_dma_count[dma] > 0)
	{
		// process current DMA
		switch(sh2->active_dma_size[dma])
		{
		case 0:
			{
				// we need to know the src / dest ahead of time without changing them
				// to allow for the callback to check if we can process the DMA at this
				// time (we need to know where we're reading / writing to/from)

				if(sh2->active_dma_incs[dma] == 2)
					tempsrc = sh2->active_dma_src[dma] - 1;
				else
					tempsrc = sh2->active_dma_src[dma];

				if(sh2->active_dma_incd[dma] == 2)
					tempdst = sh2->active_dma_dst[dma] - 1;
				else
					tempdst = sh2->active_dma_dst[dma];

#if 0
				if (sh2->dma_callback_fifo_data_available)
				{
					int available = sh2->dma_callback_fifo_data_available(sh2->device, tempsrc, tempdst, 0, sh2->active_dma_size[dma]);

					if (!available)
					{
						//printf("dma stalled\n");
						sh2->dma_timer_active[dma]=2;// mark as stalled
						return;
					}
				}
#endif

#ifdef USE_TIMER_FOR_DMA
					//schedule next DMA callback
				sh2->dma_current_active_timer[dma]->adjust(sh2->device->cycles_to_attotime(2), dma);
#endif


				dmadata = sh2->Read_Byte(tempsrc);
				if (sh2->dma_callback_kludge) dmadata = sh2->dma_callback_kludge(sh2->device, tempsrc, tempdst, dmadata, sh2->active_dma_size[dma]);
				sh2->program->write_byte(tempdst, dmadata);

				if(sh2->active_dma_incs[dma] == 2)
					sh2->active_dma_src[dma] --;
				if(sh2->active_dma_incd[dma] == 2)
					sh2->active_dma_dst[dma] --;


				if(sh2->active_dma_incs[dma] == 1)
					sh2->active_dma_src[dma] ++;
				if(sh2->active_dma_incd[dma] == 1)
					sh2->active_dma_dst[dma] ++;

				sh2->active_dma_count[dma] --;
			}
			break;
		case 1:
			{
				if(sh2->active_dma_incs[dma] == 2)
					tempsrc = sh2->active_dma_src[dma] - 2;
				else
					tempsrc = sh2->active_dma_src[dma];

				if(sh2->active_dma_incd[dma] == 2)
					tempdst = sh2->active_dma_dst[dma] - 2;
				else
					tempdst = sh2->active_dma_dst[dma];

				if (sh2->dma_callback_fifo_data_available)
				{
					int available = sh2->dma_callback_fifo_data_available(sh2->device, tempsrc, tempdst, 0, sh2->active_dma_size[dma]);

					if (!available)
					{
						//printf("dma stalled\n");
						sh2->dma_timer_active[dma]=2;// mark as stalled
						return;
					}
				}

				#ifdef USE_TIMER_FOR_DMA
					//schedule next DMA callback
				sh2->dma_current_active_timer[dma]->adjust(sh2->device->cycles_to_attotime(2), dma);
				#endif

				// check: should this really be using read_word_32 / write_word_32?
				dmadata = sh2->program->read_word(tempsrc);
				if (sh2->dma_callback_kludge) dmadata = sh2->dma_callback_kludge(sh2->device, tempsrc, tempdst, dmadata, sh2->active_dma_size[dma]);
				sh2->program->write_word(tempdst, dmadata);

				if(sh2->active_dma_incs[dma] == 2)
					sh2->active_dma_src[dma] -= 2;
				if(sh2->active_dma_incd[dma] == 2)
					sh2->active_dma_dst[dma] -= 2;

				if(sh2->active_dma_incs[dma] == 1)
					sh2->active_dma_src[dma] += 2;
				if(sh2->active_dma_incd[dma] == 1)
					sh2->active_dma_dst[dma] += 2;

				sh2->active_dma_count[dma] --;
			}
			break;
		case 2:
			{
				if(sh2->active_dma_incs[dma] == 2)
					tempsrc = sh2->active_dma_src[dma] - 4;
				else
					tempsrc = sh2->active_dma_src[dma];

				if(sh2->active_dma_incd[dma] == 2)
					tempdst = sh2->active_dma_dst[dma] - 4;
				else
					tempdst = sh2->active_dma_dst[dma];

				if (sh2->dma_callback_fifo_data_available)
				{
					int available = sh2->dma_callback_fifo_data_available(sh2->device, tempsrc, tempdst, 0, sh2->active_dma_size[dma]);

					if (!available)
					{
						//printf("dma stalled\n");
						sh2->dma_timer_active[dma]=2;// mark as stalled
						return;
					}
				}

				#ifdef USE_TIMER_FOR_DMA
					//schedule next DMA callback
				sh2->dma_current_active_timer[dma]->adjust(sh2->device->cycles_to_attotime(2), dma);
				#endif

				dmadata = sh2->program->read_dword(tempsrc);
				if (sh2->dma_callback_kludge) dmadata = sh2->dma_callback_kludge(sh2->device, tempsrc, tempdst, dmadata, sh2->active_dma_size[dma]);
				sh2->program->write_dword(tempdst, dmadata);

				if(sh2->active_dma_incs[dma] == 2)
					sh2->active_dma_src[dma] -= 4;
				if(sh2->active_dma_incd[dma] == 2)
					sh2->active_dma_dst[dma] -= 4;

				if(sh2->active_dma_incs[dma] == 1)
					sh2->active_dma_src[dma] += 4;
				if(sh2->active_dma_incd[dma] == 1)
					sh2->active_dma_dst[dma] += 4;

				sh2->active_dma_count[dma] --;
			}
			break;
		case 3:
			{
				// shouldn't this really be 4 calls here instead?

				tempsrc = sh2->active_dma_src[dma];

				if(sh2->active_dma_incd[dma] == 2)
					tempdst = sh2->active_dma_dst[dma] - 16;
				else
					tempdst = sh2->active_dma_dst[dma];

				if (sh2->dma_callback_fifo_data_available)
				{
					int available = sh2->dma_callback_fifo_data_available(sh2->device, tempsrc, tempdst, 0, sh2->active_dma_size[dma]);

					if (!available)
					{
						//printf("dma stalled\n");
						sh2->dma_timer_active[dma]=2;// mark as stalled
						fatalerror("SH2 dma_callback_fifo_data_available == 0 in unsupported mode\n");
					}
				}

				#ifdef USE_TIMER_FOR_DMA
					//schedule next DMA callback
				sh2->dma_current_active_timer[dma]->adjust(sh2->device->cycles_to_attotime(2), dma);
				#endif

				dmadata = sh2->program->read_dword(tempsrc);
				if (sh2->dma_callback_kludge) dmadata = sh2->dma_callback_kludge(sh2->device, tempsrc, tempdst, dmadata, sh2->active_dma_size[dma]);
				sh2->program->write_dword(tempdst, dmadata);

				dmadata = sh2->program->read_dword(tempsrc+4);
				if (sh2->dma_callback_kludge) dmadata = sh2->dma_callback_kludge(sh2->device, tempsrc, tempdst, dmadata, sh2->active_dma_size[dma]);
				sh2->program->write_dword(tempdst+4, dmadata);

				dmadata = sh2->program->read_dword(tempsrc+8);
				if (sh2->dma_callback_kludge) dmadata = sh2->dma_callback_kludge(sh2->device, tempsrc, tempdst, dmadata, sh2->active_dma_size[dma]);
				sh2->program->write_dword(tempdst+8, dmadata);

				dmadata = sh2->program->read_dword(tempsrc+12);
				if (sh2->dma_callback_kludge) dmadata = sh2->dma_callback_kludge(sh2->device, tempsrc, tempdst, dmadata, sh2->active_dma_size[dma]);
				sh2->program->write_dword(tempdst+12, dmadata);

				if(sh2->active_dma_incd[dma] == 2)
					sh2->active_dma_dst[dma] -= 16;

				sh2->active_dma_src[dma] += 16;
				if(sh2->active_dma_incd[dma] == 1)
					sh2->active_dma_dst[dma] += 16;

				sh2->active_dma_count[dma]-=4;
			}
			break;
		}
	}
	else // the dma is complete
	{
	//  int dma = param & 1;
	//  sh2_context *sh2 = (sh2_context *)ptr;

		// fever soccer uses cycle-stealing mode, resume the CPU now DMA has finished
		if (sh2->active_dma_steal[dma])
		{
			sh2->device->resume(SUSPEND_REASON_HALT );
		}


		LOG(("SH2.%s: DMA %d complete\n", "0", dma));
		sh2->m[0x63+4*dma] |= 2;
		sh2->dma_timer_active[dma] = 0;
		sh2->dma_irq[dma] |= 1;
		sh2_recalc_irq(sh2);

	}
}

static TIMER_CALLBACK( sh2_dma_current_active_callback )
{
	int dma = param & 1;
	sh2_context *sh2 = (sh2_context *)ptr;

	sh2_do_dma(sh2, dma);
}


static void sh2_dmac_check(sh2_context *sh2, int dma)
{
	if(sh2->m[0x63+4*dma] & sh2->m[0x6c] & 1)
	{
		if(!sh2->dma_timer_active[dma] && !(sh2->m[0x63+4*dma] & 2))
		{
			sh2->active_dma_incd[dma] = (sh2->m[0x63+4*dma] >> 14) & 3;
			sh2->active_dma_incs[dma] = (sh2->m[0x63+4*dma] >> 12) & 3;
			sh2->active_dma_size[dma] = (sh2->m[0x63+4*dma] >> 10) & 3;
			sh2->active_dma_steal[dma] = (sh2->m[0x63+4*dma] &0x10);

			if(sh2->active_dma_incd[dma] == 3 || sh2->active_dma_incs[dma] == 3)
			{
				logerror("SH2: DMA: bad increment values (%d, %d, %d, %04x)\n", sh2->active_dma_incd[dma], sh2->active_dma_incs[dma], sh2->active_dma_size[dma], sh2->m[0x63+4*dma]);
				return;
			}
			sh2->active_dma_src[dma]   = sh2->m[0x60+4*dma];
			sh2->active_dma_dst[dma]   = sh2->m[0x61+4*dma];
			sh2->active_dma_count[dma] = sh2->m[0x62+4*dma];
			if(!sh2->active_dma_count[dma])
				sh2->active_dma_count[dma] = 0x1000000;

			LOG(("SH2: DMA %d start %x, %x, %x, %04x, %d, %d, %d\n", dma, sh2->active_dma_src[dma], sh2->active_dma_dst[dma], sh2->active_dma_count[dma], sh2->m[0x63+4*dma], sh2->active_dma_incs[dma], sh2->active_dma_incd[dma], sh2->active_dma_size[dma]));

			sh2->dma_timer_active[dma] = 1;

			sh2->active_dma_src[dma] &= AM;
			sh2->active_dma_dst[dma] &= AM;

			switch(sh2->active_dma_size[dma])
			{
			case 0:
				break;
			case 1:
				sh2->active_dma_src[dma] &= ~1;
				sh2->active_dma_dst[dma] &= ~1;
				break;
			case 2:
				sh2->active_dma_src[dma] &= ~3;
				sh2->active_dma_dst[dma] &= ~3;
				break;
			case 3:
				sh2->active_dma_src[dma] &= ~3;
				sh2->active_dma_dst[dma] &= ~3;
				sh2->active_dma_count[dma] &= ~3;
				break;
			}




#ifdef USE_TIMER_FOR_DMA
			// start DMA timer

			// fever soccer uses cycle-stealing mode, requiring the CPU to be halted
			if (sh2->active_dma_steal[dma])
			{
				//printf("cycle stealing DMA\n");
				sh2->device->suspend(SUSPEND_REASON_HALT, 1 );
			}

			sh2->dma_current_active_timer[dma]->adjust(sh2->device->cycles_to_attotime(2), dma);
#endif

		}
	}
	else
	{
		if(sh2->dma_timer_active[dma])
		{
			logerror("SH2: DMA %d cancelled in-flight\n", dma);
			//sh2->dma_complete_timer[dma]->adjust(attotime::never);
			sh2->dma_current_active_timer[dma]->adjust(attotime::never);

			sh2->dma_timer_active[dma] = 0;
		}
	}
}

WRITE32_HANDLER( sh2_internal_w )
{
	sh2_context *sh2 = GET_SH2(&space.device());
	UINT32 old;

	old = sh2->m[offset];
	COMBINE_DATA(sh2->m+offset);

	//  if(offset != 0x20)
	//      logerror("sh2_internal_w:  Write %08x (%x), %08x @ %08x\n", 0xfffffe00+offset*4, offset, data, mem_mask);

//    if(offset != 0x20)
//        printf("sh2_internal_w:  Write %08x (%x), %08x @ %08x (PC %x)\n", 0xfffffe00+offset*4, offset, data, mem_mask, space.device().safe_pc());

	switch( offset )
	{
	case 0x00:
		//if(mem_mask == 0xff)
		//  printf("%c",data & 0xff);
		break;
	case 0x01:
		//printf("%08x %02x %02x\n",mem_mask,offset,data);
		break;
		// Timers
	case 0x04: // TIER, FTCSR, FRC
		if((mem_mask & 0x00ffffff) != 0)
		{
			sh2_timer_resync(sh2);
		}
//      printf("SH2.%s: TIER write %04x @ %04x\n", "0", data >> 16, mem_mask>>16);
		sh2->m[4] = (sh2->m[4] & ~(ICF|OCFA|OCFB|OVF)) | (old & sh2->m[4] & (ICF|OCFA|OCFB|OVF));
		COMBINE_DATA(&sh2->frc);
		if((mem_mask & 0x00ffffff) != 0)
			sh2_timer_activate(sh2);
		sh2_recalc_irq(sh2);
		break;
	case 0x05: // OCRx, TCR, TOCR
//      printf("SH2.%s: TCR write %08x @ %08x\n", "0", data, mem_mask);
		sh2_timer_resync(sh2);
		if(sh2->m[5] & 0x10)
			sh2->ocrb = (sh2->ocrb & (~mem_mask >> 16)) | ((data & mem_mask) >> 16);
		else
			sh2->ocra = (sh2->ocra & (~mem_mask >> 16)) | ((data & mem_mask) >> 16);
		sh2_timer_activate(sh2);
		break;

	case 0x06: // ICR
		break;

		// Interrupt vectors
	case 0x18: // IPRB, VCRA
	case 0x19: // VCRB, VCRC
	case 0x1a: // VCRD
		sh2_recalc_irq(sh2);
		break;

		// DMA
	case 0x1c: // DRCR0, DRCR1
		break;

		// Watchdog
	case 0x20: // WTCNT, RSTCSR
		if((sh2->m[0x20] & 0xff000000) == 0x5a000000)
			sh2->wtcnt = (sh2->m[0x20] >> 16) & 0xff;

		if((sh2->m[0x20] & 0xff000000) == 0xa5000000)
		{
			/*
			WTCSR
			x--- ---- Overflow in IT mode
			-x-- ---- Timer mode (0: IT 1: watchdog)
			--x- ---- Timer enable
			---1 1---
			---- -xxx Clock select
			*/

			sh2->wtcsr = (sh2->m[0x20] >> 16) & 0xff;
		}

		if((sh2->m[0x20] & 0x0000ff00) == 0x00005a00)
		{
			// -x-- ---- RSTE (1: resets wtcnt when overflows 0: no reset)
			// --x- ---- RSTS (0: power-on reset 1: Manual reset)
			// ...
		}

		if((sh2->m[0x20] & 0x0000ff00) == 0x0000a500)
		{
			// clear WOVF
			// ...
		}



		break;

		// Standby and cache
	case 0x24: // SBYCR, CCR
		/*
		    CCR
		    xx-- ---- ---- ---- Way 0/1
		    ---x ---- ---- ---- Cache Purge (CP)
		    ---- x--- ---- ---- Two-Way Mode (TW)
		    ---- -x-- ---- ---- Data Replacement Disable (OD)
		    ---- --x- ---- ---- Instruction Replacement Disable (ID)
		    ---- ---x ---- ---- Cache Enable (CE)
		*/
		break;

		// Interrupt vectors cont.
	case 0x38: // ICR, IRPA
		break;
	case 0x39: // VCRWDT
		break;

		// Division box
	case 0x40: // DVSR
		break;
	case 0x41: // DVDNT
		{
			INT32 a = sh2->m[0x41];
			INT32 b = sh2->m[0x40];
			LOG(("SH2 '%s' div+mod %d/%d\n", "0", a, b));
			if (b)
			{
				sh2->m[0x45] = a / b;
				sh2->m[0x44] = a % b;
			}
			else
			{
				sh2->m[0x42] |= 0x00010000;
				sh2->m[0x45] = 0x7fffffff;
				sh2->m[0x44] = 0x7fffffff;
				sh2_recalc_irq(sh2);
			}
			break;
		}
	case 0x42: // DVCR
		sh2->m[0x42] = (sh2->m[0x42] & ~0x00001000) | (old & sh2->m[0x42] & 0x00010000);
		sh2_recalc_irq(sh2);
		break;
	case 0x43: // VCRDIV
		sh2_recalc_irq(sh2);
		break;
	case 0x44: // DVDNTH
		break;
	case 0x45: // DVDNTL
		{
			INT64 a = sh2->m[0x45] | ((UINT64)(sh2->m[0x44]) << 32);
			INT64 b = (INT32)sh2->m[0x40];
			LOG(("SH2 '%s' div+mod %" I64FMT "d/%" I64FMT "d\n", "0", a, b));
			if (b)
			{
				INT64 q = a / b;
				if (q != (INT32)q)
				{
					sh2->m[0x42] |= 0x00010000;
					sh2->m[0x45] = 0x7fffffff;
					sh2->m[0x44] = 0x7fffffff;
					sh2_recalc_irq(sh2);
				}
				else
				{
					sh2->m[0x45] = q;
					sh2->m[0x44] = a % b;
				}
			}
			else
			{
				sh2->m[0x42] |= 0x00010000;
				sh2->m[0x45] = 0x7fffffff;
				sh2->m[0x44] = 0x7fffffff;
				sh2_recalc_irq(sh2);
			}
			break;
		}

		// DMA controller
	case 0x60: // SAR0
	case 0x61: // DAR0
		break;
	case 0x62: // DTCR0
		sh2->m[0x62] &= 0xffffff;
		break;
	case 0x63: // CHCR0
		sh2->m[0x63] = (sh2->m[0x63] & ~2) | (old & sh2->m[0x63] & 2);
		sh2_dmac_check(sh2, 0);
		break;
	case 0x64: // SAR1
	case 0x65: // DAR1
		break;
	case 0x66: // DTCR1
		sh2->m[0x66] &= 0xffffff;
		break;
	case 0x67: // CHCR1
		sh2->m[0x67] = (sh2->m[0x67] & ~2) | (old & sh2->m[0x67] & 2);
		sh2_dmac_check(sh2, 1);
		break;
	case 0x68: // VCRDMA0
	case 0x6a: // VCRDMA1
		sh2_recalc_irq(sh2);
		break;
	case 0x6c: // DMAOR
		sh2->m[0x6c] = (sh2->m[0x6c] & ~6) | (old & sh2->m[0x6c] & 6);
		sh2_dmac_check(sh2, 0);
		sh2_dmac_check(sh2, 1);
		break;

		// Bus controller
	case 0x78: // BCR1
	case 0x79: // BCR2
	case 0x7a: // WCR
	case 0x7b: // MCR
	case 0x7c: // RTCSR
	case 0x7d: // RTCNT
	case 0x7e: // RTCOR
		break;

	default:
		logerror("sh2_internal_w:  Unmapped write %08x, %08x @ %08x\n", 0xfffffe00+offset*4, data, mem_mask);
		break;
	}
}

READ32_HANDLER( sh2_internal_r )
{
	sh2_context *sh2 = GET_SH2(&space.device());

//  logerror("sh2_internal_r:  Read %08x (%x) @ %08x\n", 0xfffffe00+offset*4, offset, mem_mask);
	switch( offset )
	{
	case 0x00:
		break;
	case 0x01:
		return sh2->m[1] | 0x80000000; // TDRE: Trasmit Data Register Empty. Force it to be '1' for the time being.

	case 0x04: // TIER, FTCSR, FRC
		if ( mem_mask == 0x00ff0000 )
		{
			if ( sh2->ftcsr_read_callback != NULL )
			{
				sh2->ftcsr_read_callback( (sh2->m[4] & 0xffff0000) | sh2->frc );
			}
		}
		sh2_timer_resync(sh2);
		return (sh2->m[4] & 0xffff0000) | sh2->frc;
	case 0x05: // OCRx, TCR, TOCR
		if(sh2->m[5] & 0x10)
			return (sh2->ocrb << 16) | (sh2->m[5] & 0xffff);
		else
			return (sh2->ocra << 16) | (sh2->m[5] & 0xffff);
	case 0x06: // ICR
		return sh2->icr << 16;

	case 0x20:
		return (((sh2->wtcsr | 0x18) & 0xff) << 24)  | ((sh2->wtcnt & 0xff) << 16);

	case 0x24: // SBYCR, CCR
		return sh2->m[0x24] & ~0x3000; /* bit 4-5 of CCR are always zero */

	case 0x38: // ICR, IPRA
		return (sh2->m[0x38] & 0x7fffffff) | (sh2->nmi_line_state == ASSERT_LINE ? 0 : 0x80000000);

	case 0x78: // BCR1
		return sh2->is_slave ? 0x00008000 : 0;

	case 0x41: // dvdntl mirrors
	case 0x47:
		return sh2->m[0x45];

	case 0x46: // dvdnth mirror
		return sh2->m[0x44];
	}
	return sh2->m[offset];
}

void sh2_set_ftcsr_read_callback(device_t *device, void (*callback)(UINT32))
{
	sh2_context *sh2 = GET_SH2(device);
	sh2->ftcsr_read_callback = callback;
}

void sh2_set_frt_input(device_t *device, int state)
{
	sh2_context *sh2 = GET_SH2(device);

	if(state == PULSE_LINE)
	{
		sh2_set_frt_input(device, ASSERT_LINE);
		sh2_set_frt_input(device, CLEAR_LINE);
		return;
	}

	if(sh2->frt_input == state) {
		return;
	}

	sh2->frt_input = state;

	if(sh2->m[5] & 0x8000) {
		if(state == CLEAR_LINE) {
			return;
		}
	} else {
		if(state == ASSERT_LINE) {
			return;
		}
	}

	sh2_timer_resync(sh2);
	sh2->icr = sh2->frc;
	sh2->m[4] |= ICF;
	//logerror("SH2.%s: ICF activated (%x)\n", "0", sh2->pc & AM);
	sh2_recalc_irq(sh2);
}
#endif

void sh2_set_irq_line(sh2_context *sh2, int irqline, int state)
{
	if (irqline == INPUT_LINE_NMI)
	{
		if (sh2->nmi_line_state == state)
			return;
		sh2->nmi_line_state = state;

		if( state == CLEAR_LINE )
		{
			LOG(("SH-2 '' cleared nmi\n"));
		}
		else
		{
			LOG(("SH-2 '%s' assert nmi\n", "0"));

			sh2_exception(sh2, "Set IRQ line", 16);

		}
	}
	else
	{
		if (sh2->irq_line_state[irqline] == state)
			return;
		sh2->irq_line_state[irqline] = state;

		if( state == CLEAR_LINE )
		{
			LOG(("SH-2 '%s' cleared irq #%d\n", "0", irqline));
			sh2->pending_irq &= ~(1 << irqline);
		}
		else
		{
			LOG(("SH-2 '%s' assert irq #%d\n", "0", irqline));
			sh2->pending_irq |= 1 << irqline;
			if(sh2->delay)
			    sh2->test_irq = 1;
			else
			    CHECK_PENDING_IRQ("sh2_set_irq_line");
		}
	}
}

void SH2_Interrupt(sh2_context *sh2, int irq) {
    sh2_set_irq_line(sh2,irq,HOLD_LINE);
    SH2_Exec(sh2,2);
    sh2_set_irq_line(sh2,irq,CLEAR_LINE);
}

void sh2_recalc_irq(sh2_context *sh2)
{
	int irq = 0, vector = -1;
	int  level;

	// Timer irqs
	if((sh2->m[4]>>8) & sh2->m[4] & (ICF|OCFA|OCFB|OVF))
	{
		level = (sh2->m[0x18] >> 24) & 15;
		if(level > irq)
		{
			int mask = (sh2->m[4]>>8) & sh2->m[4];
			irq = level;
			if(mask & ICF)
				vector = (sh2->m[0x19] >> 8) & 0x7f;
			else if(mask & (OCFA|OCFB))
				vector = sh2->m[0x19] & 0x7f;
			else
				vector = (sh2->m[0x1a] >> 24) & 0x7f;
		}
	}

	// DMA irqs
	if((sh2->m[0x63] & 6) == 6 && sh2->dma_irq[0]) {
		level = (sh2->m[0x38] >> 8) & 15;
		if(level > irq) {
			irq = level;
			sh2->dma_irq[0] &= ~1;
			vector = (sh2->m[0x68]) & 0x7f;
		}
	}
	else if((sh2->m[0x67] & 6) == 6 && sh2->dma_irq[1]) {
		level = (sh2->m[0x38] >> 8) & 15;
		if(level > irq) {
			irq = level;
			sh2->dma_irq[1] &= ~1;
			vector = (sh2->m[0x6a]) & 0x7f;
		}
	}

	sh2->internal_irq_level = irq;
	sh2->internal_irq_vector = vector;
	sh2->test_irq = 1;
}

void sh2_exception(sh2_context *sh2, const char *message, int irqline)
{
	int vector;

	if (irqline != 16)
	{
		if (irqline <= ((sh2->sr >> 4) & 15)) /* If the cpu forbids this interrupt */
			return;

		// if this is an sh2 internal irq, use its vector
		if (sh2->internal_irq_level == irqline)
		{
			vector = sh2->internal_irq_vector;
			/* avoid spurious irqs with this (TODO: needs a better fix) */
			sh2->internal_irq_level = -1;
			LOG(("SH-2 '%s' exception #%d (internal vector: $%x) after [%s]\n", "0", irqline, vector, message));
		}
		else
		{
			if(sh2->m[0x38] & 0x00010000)
			{
				// vector = sh2->irq_callback(sh2->device, irqline);
				printf("would need an irqcallback here 1\n");
				exit(1);
				// LOG(("SH-2 '%s' exception #%d (external vector: $%x) after [%s]\n", "0", irqline, vector, message));
			}
			else
			{
				// sh2->irq_callback(sh2->device, irqline);
				vector = 64 + irqline/2;
				LOG(("SH-2 '%s' exception #%d (autovector: $%x) after [%s]\n", "0", irqline, vector, message));
			}
		}
	}
	else
	{
		vector = 11;
		LOG(("SH-2 '%s' nmi exception (autovector: $%x) after [%s]\n", "0", vector, message));
	}

	sh2->r[15] -= 4;
	WL( sh2, sh2->r[15], sh2->sr );     /* push SR onto stack */
	sh2->r[15] -= 4;
	WL( sh2, sh2->r[15], sh2->pc );     /* push PC onto stack */

	/* set I flags in SR */
	if (irqline > SH2_INT_15)
		sh2->sr = sh2->sr | I;
	else
		sh2->sr = (sh2->sr & ~I) | (irqline << 4);

	/* fetch PC */
	sh2->pc = RL( sh2, sh2->vbr + vector * 4 );

	if(sh2->sleep_mode == 1) { sh2->sleep_mode = 2; }
}

UINT8 FASTCALL
Def_READB (UINT32 adr)
{
  print_debug ("SH2 read byte at %.8lx\n", adr); return 0;
}

UINT8 FASTCALL
SH2_Read_Byte_C0 (UINT32 adr)
{
    printf("read_byte_c0 %x\n",adr);
    exit(1);
    return 0;
}

UINT16 FASTCALL
SH2_Read_Word_C0 (UINT32 adr)
{
    printf("read_word_c0 %x\n",adr);
    exit(1);
    return 0;
}

UINT32 FASTCALL
SH2_Read_Long_C0 (UINT32 adr)
{
    printf("read_long_c0 %x\n",adr);
    exit(1);
    return 0;
}

UINT8 FASTCALL
SH2_Read_Byte_FF (UINT32 adr)
{
    printf("read_byte_ff %x\n",adr);
    exit(1);
    return 0;
}

UINT16 FASTCALL
SH2_Read_Word_FF (UINT32 adr)
{
    printf("read_word_ff %x\n",adr);
    exit(1);
    return 0;
}

UINT32 FASTCALL
SH2_Read_Long_FF (UINT32 adr)
{
    printf("read_long_ff %x\n",adr);
    exit(1);
    return 0;
}
UINT16 FASTCALL
Def_READW (UINT32 adr)
{
  print_debug ("SH2 read word at %.8lX\n", adr); return 0;
}

UINT32 FASTCALL
Def_READL (UINT32 adr)
{
  print_debug ("SH2 read long at %.8lX\n", adr); return 0;
}

void FASTCALL
Def_WRITEB (UINT32 adr, UINT8 data)
{
    print_debug ("SH2 write byte %.2X at %.8lX\n", data, adr);
}

void FASTCALL
Def_WRITEW (UINT32 adr, UINT16 data)
{
    print_debug ("SH2 write word %.4X at %.8lX\n", data, adr);
}

void FASTCALL
Def_WRITEL (UINT32 adr, UINT32 data)
{
#ifdef __i386__
    print_debug ("SH2 write long %.8lX at %.8lX\n", data, adr);
#else
    print_debug ("SH2 write long %.8X at %.8X\n", data, adr);
#endif
}

static sh2_context mcopy, scopy;

static void postload_sh2(void *aSH2) {
    sh2_context *SH2 = (sh2_context *)aSH2;
    int slave = (SH2 != &M_SH2);
    if (!slave) {
	memcpy(SH2->Read_Byte, mcopy.Read_Byte, sizeof(SH2_RB*)*0x100);
	memcpy(SH2->Read_Word, mcopy.Read_Word, sizeof(SH2_RB*)*0x100);
	memcpy(SH2->Read_Long, mcopy.Read_Long, sizeof(SH2_RB*)*0x100);

	memcpy(SH2->Write_Byte, mcopy.Write_Byte, sizeof(SH2_RB*)*0x100);
	memcpy(SH2->Write_Word, mcopy.Write_Word, sizeof(SH2_RB*)*0x100);
	memcpy(SH2->Write_Long, mcopy.Write_Long, sizeof(SH2_RB*)*0x100);

	memcpy(SH2->Fetch_Region, mcopy.Fetch_Region, sizeof(FETCHREG)*0x100);
    }
}

void SH2_Map_Cache_Trough (sh2_context * SH2)
{
    UINT32 i;

    for (i = 0; i < 0x10; i++)
    {
	SH2->Read_Byte[i + 0x20] = SH2->Read_Byte[i];
	SH2->Read_Word[i + 0x20] = SH2->Read_Word[i];
	SH2->Read_Long[i + 0x20] = SH2->Read_Long[i];
	SH2->Write_Byte[i + 0x20] = SH2->Write_Byte[i];
	SH2->Write_Word[i + 0x20] = SH2->Write_Word[i];
	SH2->Write_Long[i + 0x20] = SH2->Write_Long[i];
    }
    int slave = (SH2 != &M_SH2);
    AddSaveData(ASCII_ID('s','h','2',slave),(UINT8*)SH2, sizeof(sh2_context)); // SH2 for asm, sh2 for C version !
    if (!slave)
	memcpy(&mcopy,&M_SH2,sizeof(sh2_context));
    else
	memcpy(&scopy,&S_SH2,sizeof(sh2_context));
    AddLoadCallback_ptr(postload_sh2,SH2);
}

int sh2Engine;

void SH2_Init(sh2_context *sh2, int is_slave)
{
    // Most of the init is done in SH2_Reset, see original mame code in 153 for the rest (mainly save/restore callbacks).
	sh2->is_slave = is_slave;
	sh2Engine = 1;

  // Default initialisation memory...

  SH2_Add_ReadB (sh2, 0x00, 0xFF, Def_READB);
  SH2_Add_ReadW (sh2, 0x00, 0xFF, Def_READW);
  SH2_Add_ReadL (sh2, 0x00, 0xFF, Def_READL);
  SH2_Add_WriteB (sh2, 0x00, 0xFF, Def_WRITEB);
  SH2_Add_WriteW (sh2, 0x00, 0xFF, Def_WRITEW);
  SH2_Add_WriteL (sh2, 0x00, 0xFF, Def_WRITEL);

  SH2_Add_ReadB (sh2, 0xC0, 0xC0, SH2_Read_Byte_C0);
  SH2_Add_ReadW (sh2, 0xC0, 0xC0, SH2_Read_Word_C0);
  SH2_Add_ReadL (sh2, 0xC0, 0xC0, SH2_Read_Long_C0);
  //SH2_Add_WriteB (sh2, 0xC0, 0xC0, SH2_Write_Byte_C0);
  //SH2_Add_WriteW (sh2, 0xC0, 0xC0, SH2_Write_Word_C0);
  //SH2_Add_WriteL (sh2, 0xC0, 0xC0, SH2_Write_Long_C0);

  SH2_Add_ReadB (sh2, 0xFF, 0xFF, SH2_Read_Byte_FF);
  SH2_Add_ReadW (sh2, 0xFF, 0xFF, SH2_Read_Word_FF);
  SH2_Add_ReadL (sh2, 0xFF, 0xFF, SH2_Read_Long_FF);
  //SH2_Add_WriteB (sh2, 0xFF, 0xFF, SH2_Write_Byte_FF);
  //SH2_Add_WriteW (sh2, 0xFF, 0xFF, SH2_Write_Word_FF);
  //SH2_Add_WriteL (sh2, 0xFF, 0xFF, SH2_Write_Long_FF);

}

u32 SH2_Get_PC(sh2_context *sh2) {
    return sh2->pc;
}

u32 SH2_Get_SR(sh2_context *sh2) {
    return sh2->sr;
}

void SH2_ReleaseTimeSlice(sh2_context *M_SH2) {
    M_SH2->icount = 0;
}

void
SH2_Add_ReadB (sh2_context * SH2, UINT32 low_adr, UINT32 high_adr,
	       SH2_RB * Func)
{
  UINT32 i;

  for (i = (low_adr & 0xFF); i <= (high_adr & 0xFF); i++)
    SH2->Read_Byte[i] = Func;
}


void SH2_Add_ReadW (sh2_context * SH2, UINT32 low_adr, UINT32 high_adr, SH2_RW * Func)
{
  UINT32 i;

  for (i = (low_adr & 0xFF); i <= (high_adr & 0xFF); i++)
    SH2->Read_Word[i] = Func;
}


void SH2_Add_ReadL (sh2_context * SH2, UINT32 low_adr, UINT32 high_adr, SH2_RL * Func)
{
  UINT32 i;

  for (i = (low_adr & 0xFF); i <= (high_adr & 0xFF); i++)
    SH2->Read_Long[i] = Func;
}


void SH2_Add_WriteB (sh2_context * SH2, UINT32 low_adr, UINT32 high_adr, SH2_WB * Func)
{
  UINT32 i;

  for (i = (low_adr & 0xFF); i <= (high_adr & 0xFF); i++)
    SH2->Write_Byte[i] = Func;
}


void SH2_Add_WriteW (sh2_context * SH2, UINT32 low_adr, UINT32 high_adr, SH2_WW * Func)
{
  UINT32 i;

  for (i = (low_adr & 0xFF); i <= (high_adr & 0xFF); i++)
    SH2->Write_Word[i] = Func;
}


void SH2_Add_WriteL (sh2_context * SH2, UINT32 low_adr, UINT32 high_adr, SH2_WL * Func)
{
  UINT32 i;

  for (i = (low_adr & 0xFF); i <= (high_adr & 0xFF); i++)
    SH2->Write_Long[i] = Func;
}

void SH2_Add_Fetch (sh2_context * SH2, UINT32 low_adr, UINT32 high_adr, UINT16 * Fetch)
{
  UINT32 i;

  for (i = 0; i < 0x100; i++)
    {
      if (SH2->Fetch_Region[i].Fetch_Reg == NULL)
	{
	  SH2->Fetch_Region[i].Low_Adr = low_adr;
	  SH2->Fetch_Region[i].High_Adr = high_adr;
	  SH2->Fetch_Region[i].Fetch_Reg = (UINT16*)(((UINT8*)Fetch) - low_adr);
	  break;
	}
    }
}

UINT8 *get_sh2_code_range(sh2_context *SH2,UINT32 adr,UINT32 *start,UINT32 *end) {
    for (int i=0; i<0x100 && SH2->Fetch_Region[i].Fetch_Reg; i++) {
	if (SH2->Fetch_Region[i].Low_Adr <= adr &&
		SH2->Fetch_Region[i].High_Adr >= adr) {
	    *start = SH2->Fetch_Region[i].Low_Adr;
	    *end = SH2->Fetch_Region[i].High_Adr;
	    return (UINT8*)SH2->Fetch_Region[i].Fetch_Reg;
	}
    }
    *start = *end = 0;
    return NULL;
}

UINT8 *get_sh2_userdata(sh2_context *SH2, UINT32 ptr) {
    int i;
    for (i=0; i<0x100 && SH2->Fetch_Region[i].Fetch_Reg; i++) {
	if (SH2->Fetch_Region[i].Low_Adr <= ptr &&
		SH2->Fetch_Region[i].High_Adr >= ptr)
	    return (UINT8*)SH2->Fetch_Region[i].Fetch_Reg;
    }
    return NULL;
}

UINT8 SH2_Read_Byte(sh2_context *sh2, UINT32 adr) {
    return sh2->Read_Byte[adr >> 24](adr);
}

UINT16 SH2_Read_Word(sh2_context *sh2, UINT32 adr) {
    return sh2->Read_Word[adr >> 24](adr);
}

UINT32 SH2_Read_Long(sh2_context *sh2, UINT32 adr) {
    return sh2->Read_Long[adr >> 24](adr);
}

void SH2_Write_Byte(sh2_context *sh2, UINT32 adr, UINT8 data) {
    sh2->Write_Byte[adr >> 24](adr,data);
}

void SH2_Write_Word(sh2_context *sh2, UINT32 adr, UINT16 data) {
    sh2->Write_Word[adr >> 24](adr,data);
}

void SH2_Write_Long(sh2_context *sh2, UINT32 adr, UINT32 data) {
    sh2->Write_Long[adr>>24](adr,data);
}

UINT32 FASTCALL SH2_Get_R(sh2_context *sh2, UINT32 n) {
    return sh2->r[n];
}

void FASTCALL SH2_Set_SR(sh2_context *sh2, UINT32 sr) {
    sh2->sr = sr;
}

void FASTCALL SH2_Set_PC(sh2_context *sh2, UINT32 pc) {
    sh2->pc = pc;
}

