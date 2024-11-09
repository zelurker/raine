// Work in progress, taken from mame 153 with obvious changes to adapt to raine...
/*****************************************************************************
 *
 *   sh2.c
 *   Portable Hitachi SH-2 (SH7600 family) emulator
 *
 *   Copyright Juergen Buchmueller <pullmoll@t-online.de>,
 *   all rights reserved.
 *
 *   - This source code is released as freeware for non-commercial purposes.
 *   - You are free to use and redistribute this code in modified or
 *     unmodified form, provided you list me in the credits.
 *   - If you modify this source code, you must add a notice to each modified
 *     source file that it has been changed.  If you're a nice person, you
 *     will clearly mark each change too.  :)
 *   - If you wish to use this for commercial purposes, please contact me at
 *     pullmoll@t-online.de
 *   - The author of this copywritten work reserves the right to change the
 *     terms of its usage and license at any time, including retroactively
 *   - This entire notice must remain in the source code.
 *
 *  This work is based on <tiraniddo@hotmail.com> C/C++ implementation of
 *  the SH-2 CPU core and was adapted to the MAME CPU core requirements.
 *  Thanks also go to Chuck Mason <chukjr@sundail.net> and Olivier Galibert
 *  <galibert@pobox.com> for letting me peek into their SEMU code :-)
 *
 *****************************************************************************/

/*****************************************************************************
    Changes
    20130129 Angelo Salese
    - added illegal opcode exception handling, side effect of some Saturn games
      on loading like Feda or Falcom Classics Vol. 1
      (i.e. Master CPU Incautiously transfers memory from CD to work RAM H, and
            wipes out Slave CPU program code too while at it).

    20051129 Mariusz Wojcieszek
    - introduced memory_decrypted_read_word() for opcode fetching

    20050813 Mariusz Wojcieszek
    - fixed 64 bit / 32 bit division in division unit

    20031015 O. Galibert
    - dma fixes, thanks to sthief

    20031013 O. Galibert, A. Giles
    - timer fixes
    - multi-cpu simplifications

    20030915 O. Galibert
    - fix DMA1 irq vector
    - ignore writes to DRCRx
    - fix cpu number issues
    - fix slave/master recognition
    - fix wrong-cpu-in-context problem with the timers

    20021020 O. Galibert
    - DMA implementation, lightly tested
    - delay slot in debugger fixed
    - add divide box mirrors
    - Nicola-ify the indentation
    - Uncrapify sh2_internal_*
    - Put back nmi support that had been lost somehow

    20020914 R. Belmont
    - Initial SH2 internal timers implementation, based on code by O. Galibert.
      Makes music work in galspanic4/s/s2, panic street, cyvern, other SKNS games.
    - Fix to external division, thanks to "spice" on the E2J board.
      Corrects behavior of s1945ii turret boss.

    20020302 Olivier Galibert (galibert@mame.net)
    - Fixed interrupt in delay slot
    - Fixed rotcr
    - Fixed div1
    - Fixed mulu
    - Fixed negc

    20020301 R. Belmont
    - Fixed external division

    20020225 Olivier Galibert (galibert@mame.net)
    - Fixed interrupt handling

    20010207 Sylvain Glaize (mokona@puupuu.org)

    - Bug fix in static inline void MOVBM(UINT32 m, UINT32 n) (see comment)
    - Support of full 32 bit addressing (RB, RW, RL and WB, WW, WL functions)
        reason : when the two high bits of the address are set, access is
        done directly in the cache data array. The SUPER KANEKO NOVA SYSTEM
        sets the stack pointer here, using these addresses as usual RAM access.

        No real cache support has been added.
    - Read/Write memory format correction (_bew to _bedw) (see also SH2
        definition in cpuintrf.c and DasmSH2(..) in sh2dasm.c )

    20010623 James Forshaw (TyRaNiD@totalise.net)

    - Modified operation of sh2_exception. Done cause mame irq system is stupid, and
      doesnt really seem designed for any more than 8 interrupt lines.

    20010701 James Forshaw (TyRaNiD@totalise.net)

    - Fixed DIV1 operation. Q bit now correctly generated

    20020218 Added save states (mokona@puupuu.org)

 *****************************************************************************/

#include "sh2.h"
#include "sh2comn.h"
#include "debug.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "raine.h"

/* speed up delay loops, bail out of tight loops */
// #define BUSY_LOOP_HACKS     1
// For now in psikyosh there is no need

#define VERBOSE 0

#define logerror print_debug
#define LOG(x)  do { if (VERBOSE) logerror x; } while (0)

static inline UINT8 RB(sh2_context *sh2, u32 A)
{
#if 0
	if (A >= 0xe0000000)
		return sh2_internal_r(*sh2->internal, (A & 0x1fc)>>2, 0xff << (((~A) & 3)*8)) >> (((~A) & 3)*8);

	if (A >= 0xc0000000)
		return sh2->read_byte(A);

	if (A >= 0x40000000)
		return 0xa5;

	return sh2->read_byte(A & AM);
#else
    return sh2->Read_Byte[A >> 24](A);
#endif
}

static inline UINT16 RW(sh2_context *sh2, u32 A)
{
#if 0
	if (A >= 0xe0000000)
		return sh2_internal_r(*sh2->internal, (A & 0x1fc)>>2, 0xffff << (((~A) & 2)*8)) >> (((~A) & 2)*8);

	if (A >= 0xc0000000)
		return sh2->read_word(A);

	if (A >= 0x40000000)
		return 0xa5a5;

	return sh2->read_word(A & AM);
#else
	return sh2->Read_Word[A >> 24](A);
#endif
}

static inline UINT32 RL(sh2_context *sh2, u32 A)
{
#if 0
	if (A >= 0xe0000000)
		return sh2_internal_r(*sh2->internal, (A & 0x1fc)>>2, 0xffffffff);

	if (A >= 0xc0000000)
		return sh2->read_dword(A);

	if (A >= 0x40000000)
		return 0xa5a5a5a5;

	return sh2->read_dword(A & AM);
#else
	return sh2->Read_Long[A >> 24](A);
#endif
}

static inline void WB(sh2_context *sh2, u32 A, UINT8 V)
{
#if 0
	if (A >= 0xe0000000)
	{
		sh2_internal_w(*sh2->internal, (A & 0x1fc)>>2, V << (((~A) & 3)*8), 0xff << (((~A) & 3)*8));
		return;
	}

	if (A >= 0xc0000000)
	{
		sh2->write_byte(A,V);
		return;
	}

	if (A >= 0x40000000)
		return;

	sh2->write_byte(A & AM,V);
#else
	sh2->Write_Byte[A >> 24](A,V);
#endif
}

static inline void WW(sh2_context *sh2, u32 A, UINT16 V)
{
#if 0
	if (A >= 0xe0000000)
	{
		sh2_internal_w(*sh2->internal, (A & 0x1fc)>>2, V << (((~A) & 2)*8), 0xffff << (((~A) & 2)*8));
		return;
	}

	if (A >= 0xc0000000)
	{
		sh2->write_word(A,V);
		return;
	}

	if (A >= 0x40000000)
		return;

	sh2->write_word(A & AM,V);
#else
	sh2->Write_Word[A >> 24](A,V);
#endif
}

static inline void WL(sh2_context *sh2, u32 A, UINT32 V)
{
#if 0
	if (A >= 0xe0000000)
	{
		sh2_internal_w(*sh2->internal, (A & 0x1fc)>>2, V, 0xffffffff);
		return;
	}

	if (A >= 0xc0000000)
	{
		sh2->write_dword(A,V);
		return;
	}

	if (A >= 0x40000000)
		return;

	sh2->write_dword(A & AM,V);
#else
	sh2->Write_Long[A >> 24](A,V);
#endif
}

/*  code                 cycles  t-bit
 *  0011 nnnn mmmm 1100  1       -
 *  ADD     Rm,Rn
 */
static inline void ADD(sh2_context *sh2, UINT32 m, UINT32 n)
{
	sh2->r[n] += sh2->r[m];
}

/*  code                 cycles  t-bit
 *  0111 nnnn iiii iiii  1       -
 *  ADD     #imm,Rn
 */
static inline void ADDI(sh2_context *sh2, UINT32 i, UINT32 n)
{
	sh2->r[n] += (INT32)(INT16)(INT8)i;
}

/*  code                 cycles  t-bit
 *  0011 nnnn mmmm 1110  1       carry
 *  ADDC    Rm,Rn
 */
static inline void ADDC(sh2_context *sh2, UINT32 m, UINT32 n)
{
	UINT32 tmp0, tmp1;

	tmp1 = sh2->r[n] + sh2->r[m];
	tmp0 = sh2->r[n];
	sh2->r[n] = tmp1 + (sh2->sr & T);
	if (tmp0 > tmp1)
		sh2->sr |= T;
	else
		sh2->sr &= ~T;
	if (tmp1 > sh2->r[n])
		sh2->sr |= T;
}

/*  code                 cycles  t-bit
 *  0011 nnnn mmmm 1111  1       overflow
 *  ADDV    Rm,Rn
 */
static inline void ADDV(sh2_context *sh2, UINT32 m, UINT32 n)
{
	INT32 dest, src, ans;

	if ((INT32) sh2->r[n] >= 0)
		dest = 0;
	else
		dest = 1;
	if ((INT32) sh2->r[m] >= 0)
		src = 0;
	else
		src = 1;
	src += dest;
	sh2->r[n] += sh2->r[m];
	if ((INT32) sh2->r[n] >= 0)
		ans = 0;
	else
		ans = 1;
	ans += dest;
	if (src == 0 || src == 2)
	{
		if (ans == 1)
			sh2->sr |= T;
		else
			sh2->sr &= ~T;
	}
	else
		sh2->sr &= ~T;
}

/*  code                 cycles  t-bit
 *  0010 nnnn mmmm 1001  1       -
 *  AND     Rm,Rn
 */
static inline void AND(sh2_context *sh2, UINT32 m, UINT32 n)
{
	sh2->r[n] &= sh2->r[m];
}


/*  code                 cycles  t-bit
 *  1100 1001 iiii iiii  1       -
 *  AND     #imm,R0
 */
static inline void ANDI(sh2_context *sh2, UINT32 i)
{
	sh2->r[0] &= i;
}

/*  code                 cycles  t-bit
 *  1100 1101 iiii iiii  1       -
 *  AND.B   #imm,@(R0,GBR)
 */
static inline void ANDM(sh2_context *sh2, UINT32 i)
{
	UINT32 temp;

	sh2->ea = sh2->gbr + sh2->r[0];
	temp = i & RB( sh2, sh2->ea );
	WB( sh2, sh2->ea, temp );
	sh2->icount -= 2;
}

/*  code                 cycles  t-bit
 *  1000 1011 dddd dddd  3/1     -
 *  BF      disp8
 */
static inline void BF(sh2_context *sh2, UINT32 d)
{
	if ((sh2->sr & T) == 0)
	{
		INT32 disp = ((INT32)d << 24) >> 24;
		sh2->pc = sh2->ea = sh2->pc + disp * 2 + 2;
		sh2->icount -= 2;
	}
}

/*  code                 cycles  t-bit
 *  1000 1111 dddd dddd  3/1     -
 *  BFS     disp8
 */
static inline void BFS(sh2_context *sh2, UINT32 d)
{
	if ((sh2->sr & T) == 0)
	{
		INT32 disp = ((INT32)d << 24) >> 24;
		sh2->delay = sh2->pc;
		sh2->pc = sh2->ea = sh2->pc + disp * 2 + 2;
		sh2->icount--;
	}
}

/*  code                 cycles  t-bit
 *  1010 dddd dddd dddd  2       -
 *  BRA     disp12
 */
static inline void BRA(sh2_context *sh2, UINT32 d)
{
	INT32 disp = ((INT32)d << 20) >> 20;

#if BUSY_LOOP_HACKS
	if (disp == -2)
	{
		UINT32 next_opcode = RW( sh2, sh2->ppc & AM );
		/* BRA  $
		 * NOP
		 */
		if (next_opcode == 0x0009)
			sh2->icount %= 3;   /* cycles for BRA $ and NOP taken (3) */
	}
#endif
	sh2->delay = sh2->pc;
	sh2->pc = sh2->ea = sh2->pc + disp * 2 + 2;
	sh2->icount--;
}

/*  code                 cycles  t-bit
 *  0000 mmmm 0010 0011  2       -
 *  BRAF    Rm
 */
static inline void BRAF(sh2_context *sh2, UINT32 m)
{
	sh2->delay = sh2->pc;
	sh2->pc += sh2->r[m] + 2;
	sh2->icount--;
}

/*  code                 cycles  t-bit
 *  1011 dddd dddd dddd  2       -
 *  BSR     disp12
 */
static inline void BSR(sh2_context *sh2, UINT32 d)
{
	INT32 disp = ((INT32)d << 20) >> 20;

	sh2->pr = sh2->pc + 2;
	sh2->delay = sh2->pc;
	sh2->pc = sh2->ea = sh2->pc + disp * 2 + 2;
	sh2->icount--;
}

/*  code                 cycles  t-bit
 *  0000 mmmm 0000 0011  2       -
 *  BSRF    Rm
 */
static inline void BSRF(sh2_context *sh2, UINT32 m)
{
	sh2->pr = sh2->pc + 2;
	sh2->delay = sh2->pc;
	sh2->pc += sh2->r[m] + 2;
	sh2->icount--;
}

/*  code                 cycles  t-bit
 *  1000 1001 dddd dddd  3/1     -
 *  BT      disp8
 */
static inline void BT(sh2_context *sh2, UINT32 d)
{
	if ((sh2->sr & T) != 0)
	{
		INT32 disp = ((INT32)d << 24) >> 24;
		sh2->pc = sh2->ea = sh2->pc + disp * 2 + 2;
		sh2->icount -= 2;
	}
}

/*  code                 cycles  t-bit
 *  1000 1101 dddd dddd  2/1     -
 *  BTS     disp8
 */
static inline void BTS(sh2_context *sh2, UINT32 d)
{
	if ((sh2->sr & T) != 0)
	{
		INT32 disp = ((INT32)d << 24) >> 24;
		sh2->delay = sh2->pc;
		sh2->pc = sh2->ea = sh2->pc + disp * 2 + 2;
		sh2->icount--;
	}
}

/*  code                 cycles  t-bit
 *  0000 0000 0010 1000  1       -
 *  CLRMAC
 */
static inline void CLRMAC(sh2_context *sh2)
{
	sh2->mach = 0;
	sh2->macl = 0;
}

/*  code                 cycles  t-bit
 *  0000 0000 0000 1000  1       -
 *  CLRT
 */
static inline void CLRT(sh2_context *sh2)
{
	sh2->sr &= ~T;
}

/*  code                 cycles  t-bit
 *  0011 nnnn mmmm 0000  1       comparison result
 *  CMP_EQ  Rm,Rn
 */
static inline void CMPEQ(sh2_context *sh2, UINT32 m, UINT32 n)
{
	if (sh2->r[n] == sh2->r[m])
		sh2->sr |= T;
	else
		sh2->sr &= ~T;
}

/*  code                 cycles  t-bit
 *  0011 nnnn mmmm 0011  1       comparison result
 *  CMP_GE  Rm,Rn
 */
static inline void CMPGE(sh2_context *sh2, UINT32 m, UINT32 n)
{
	if ((INT32) sh2->r[n] >= (INT32) sh2->r[m])
		sh2->sr |= T;
	else
		sh2->sr &= ~T;
}

/*  code                 cycles  t-bit
 *  0011 nnnn mmmm 0111  1       comparison result
 *  CMP_GT  Rm,Rn
 */
static inline void CMPGT(sh2_context *sh2, UINT32 m, UINT32 n)
{
	if ((INT32) sh2->r[n] > (INT32) sh2->r[m])
		sh2->sr |= T;
	else
		sh2->sr &= ~T;
}

/*  code                 cycles  t-bit
 *  0011 nnnn mmmm 0110  1       comparison result
 *  CMP_HI  Rm,Rn
 */
static inline void CMPHI(sh2_context *sh2, UINT32 m, UINT32 n)
{
	if ((UINT32) sh2->r[n] > (UINT32) sh2->r[m])
		sh2->sr |= T;
	else
		sh2->sr &= ~T;
}

/*  code                 cycles  t-bit
 *  0011 nnnn mmmm 0010  1       comparison result
 *  CMP_HS  Rm,Rn
 */
static inline void CMPHS(sh2_context *sh2, UINT32 m, UINT32 n)
{
	if ((UINT32) sh2->r[n] >= (UINT32) sh2->r[m])
		sh2->sr |= T;
	else
		sh2->sr &= ~T;
}


/*  code                 cycles  t-bit
 *  0100 nnnn 0001 0101  1       comparison result
 *  CMP_PL  Rn
 */
static inline void CMPPL(sh2_context *sh2, UINT32 n)
{
	if ((INT32) sh2->r[n] > 0)
		sh2->sr |= T;
	else
		sh2->sr &= ~T;
}

/*  code                 cycles  t-bit
 *  0100 nnnn 0001 0001  1       comparison result
 *  CMP_PZ  Rn
 */
static inline void CMPPZ(sh2_context *sh2, UINT32 n)
{
	if ((INT32) sh2->r[n] >= 0)
		sh2->sr |= T;
	else
		sh2->sr &= ~T;
}

/*  code                 cycles  t-bit
 *  0010 nnnn mmmm 1100  1       comparison result
 * CMP_STR  Rm,Rn
 */
static inline void CMPSTR(sh2_context *sh2, UINT32 m, UINT32 n)
	{
	UINT32 temp;
	INT32 HH, HL, LH, LL;
	temp = sh2->r[n] ^ sh2->r[m];
	HH = (temp >> 24) & 0xff;
	HL = (temp >> 16) & 0xff;
	LH = (temp >> 8) & 0xff;
	LL = temp & 0xff;
	if (HH && HL && LH && LL)
	sh2->sr &= ~T;
	else
	sh2->sr |= T;
	}


/*  code                 cycles  t-bit
 *  1000 1000 iiii iiii  1       comparison result
 *  CMP/EQ #imm,R0
 */
static inline void CMPIM(sh2_context *sh2, UINT32 i)
{
	UINT32 imm = (UINT32)(INT32)(INT16)(INT8)i;

	if (sh2->r[0] == imm)
		sh2->sr |= T;
	else
		sh2->sr &= ~T;
}

/*  code                 cycles  t-bit
 *  0010 nnnn mmmm 0111  1       calculation result
 *  DIV0S   Rm,Rn
 */
static inline void DIV0S(sh2_context *sh2, UINT32 m, UINT32 n)
{
	if ((sh2->r[n] & 0x80000000) == 0)
		sh2->sr &= ~Q;
	else
		sh2->sr |= Q;
	if ((sh2->r[m] & 0x80000000) == 0)
		sh2->sr &= ~M;
	else
		sh2->sr |= M;
	if ((sh2->r[m] ^ sh2->r[n]) & 0x80000000)
		sh2->sr |= T;
	else
		sh2->sr &= ~T;
}

/*  code                 cycles  t-bit
 *  0000 0000 0001 1001  1       0
 *  DIV0U
 */
static inline void DIV0U(sh2_context *sh2)
{
	sh2->sr &= ~(M | Q | T);
}

/*  code                 cycles  t-bit
 *  0011 nnnn mmmm 0100  1       calculation result
 *  DIV1 Rm,Rn
 */
static inline void DIV1(sh2_context *sh2, UINT32 m, UINT32 n)
{
	UINT32 tmp0;
	UINT32 old_q;

	old_q = sh2->sr & Q;
	if (0x80000000 & sh2->r[n])
		sh2->sr |= Q;
	else
		sh2->sr &= ~Q;

	sh2->r[n] = (sh2->r[n] << 1) | (sh2->sr & T);

	if (!old_q)
	{
		if (!(sh2->sr & M))
		{
			tmp0 = sh2->r[n];
			sh2->r[n] -= sh2->r[m];
			if(!(sh2->sr & Q))
				if(sh2->r[n] > tmp0)
					sh2->sr |= Q;
				else
					sh2->sr &= ~Q;
			else
				if(sh2->r[n] > tmp0)
					sh2->sr &= ~Q;
				else
					sh2->sr |= Q;
		}
		else
		{
			tmp0 = sh2->r[n];
			sh2->r[n] += sh2->r[m];
			if(!(sh2->sr & Q))
			{
				if(sh2->r[n] < tmp0)
					sh2->sr &= ~Q;
				else
					sh2->sr |= Q;
			}
			else
			{
				if(sh2->r[n] < tmp0)
					sh2->sr |= Q;
				else
					sh2->sr &= ~Q;
			}
		}
	}
	else
	{
		if (!(sh2->sr & M))
		{
			tmp0 = sh2->r[n];
			sh2->r[n] += sh2->r[m];
			if(!(sh2->sr & Q))
				if(sh2->r[n] < tmp0)
					sh2->sr |= Q;
				else
					sh2->sr &= ~Q;
			else
				if(sh2->r[n] < tmp0)
					sh2->sr &= ~Q;
				else
					sh2->sr |= Q;
		}
		else
		{
			tmp0 = sh2->r[n];
			sh2->r[n] -= sh2->r[m];
			if(!(sh2->sr & Q))
				if(sh2->r[n] > tmp0)
					sh2->sr &= ~Q;
				else
					sh2->sr |= Q;
			else
				if(sh2->r[n] > tmp0)
					sh2->sr |= Q;
				else
					sh2->sr &= ~Q;
		}
	}

	tmp0 = (sh2->sr & (Q | M));
	if((!tmp0) || (tmp0 == 0x300)) /* if Q == M set T else clear T */
		sh2->sr |= T;
	else
		sh2->sr &= ~T;
}

/*  DMULS.L Rm,Rn */
static inline void DMULS(sh2_context *sh2, UINT32 m, UINT32 n)
{
	UINT32 RnL, RnH, RmL, RmH, Res0, Res1, Res2;
	UINT32 temp0, temp1, temp2, temp3;
	INT32 tempm, tempn, fnLmL;

	tempn = (INT32) sh2->r[n];
	tempm = (INT32) sh2->r[m];
	if (tempn < 0)
		tempn = 0 - tempn;
	if (tempm < 0)
		tempm = 0 - tempm;
	if ((INT32) (sh2->r[n] ^ sh2->r[m]) < 0)
		fnLmL = -1;
	else
		fnLmL = 0;
	temp1 = (UINT32) tempn;
	temp2 = (UINT32) tempm;
	RnL = temp1 & 0x0000ffff;
	RnH = (temp1 >> 16) & 0x0000ffff;
	RmL = temp2 & 0x0000ffff;
	RmH = (temp2 >> 16) & 0x0000ffff;
	temp0 = RmL * RnL;
	temp1 = RmH * RnL;
	temp2 = RmL * RnH;
	temp3 = RmH * RnH;
	Res2 = 0;
	Res1 = temp1 + temp2;
	if (Res1 < temp1)
		Res2 += 0x00010000;
	temp1 = (Res1 << 16) & 0xffff0000;
	Res0 = temp0 + temp1;
	if (Res0 < temp0)
		Res2++;
	Res2 = Res2 + ((Res1 >> 16) & 0x0000ffff) + temp3;
	if (fnLmL < 0)
	{
		Res2 = ~Res2;
		if (Res0 == 0)
			Res2++;
		else
			Res0 = (~Res0) + 1;
	}
	sh2->mach = Res2;
	sh2->macl = Res0;
	sh2->icount--;
}

/*  DMULU.L Rm,Rn */
static inline void DMULU(sh2_context *sh2, UINT32 m, UINT32 n)
{
	UINT32 RnL, RnH, RmL, RmH, Res0, Res1, Res2;
	UINT32 temp0, temp1, temp2, temp3;

	RnL = sh2->r[n] & 0x0000ffff;
	RnH = (sh2->r[n] >> 16) & 0x0000ffff;
	RmL = sh2->r[m] & 0x0000ffff;
	RmH = (sh2->r[m] >> 16) & 0x0000ffff;
	temp0 = RmL * RnL;
	temp1 = RmH * RnL;
	temp2 = RmL * RnH;
	temp3 = RmH * RnH;
	Res2 = 0;
	Res1 = temp1 + temp2;
	if (Res1 < temp1)
		Res2 += 0x00010000;
	temp1 = (Res1 << 16) & 0xffff0000;
	Res0 = temp0 + temp1;
	if (Res0 < temp0)
		Res2++;
	Res2 = Res2 + ((Res1 >> 16) & 0x0000ffff) + temp3;
	sh2->mach = Res2;
	sh2->macl = Res0;
	sh2->icount--;
}

/*  DT      Rn */
static inline void DT(sh2_context *sh2, UINT32 n)
{
	sh2->r[n]--;
	if (sh2->r[n] == 0)
		sh2->sr |= T;
	else
		sh2->sr &= ~T;
#if BUSY_LOOP_HACKS
	{
		UINT32 next_opcode = RW( sh2, sh2->ppc & AM );
		/* DT   Rn
		 * BF   $-2
		 */
		if (next_opcode == 0x8bfd)
		{
			while (sh2->r[n] > 1 && sh2->icount > 4)
			{
				sh2->r[n]--;
				sh2->icount -= 4;   /* cycles for DT (1) and BF taken (3) */
			}
		}
	}
#endif
}

/*  EXTS.B  Rm,Rn */
static inline void EXTSB(sh2_context *sh2, UINT32 m, UINT32 n)
{
	sh2->r[n] = ((INT32)sh2->r[m] << 24) >> 24;
}

/*  EXTS.W  Rm,Rn */
static inline void EXTSW(sh2_context *sh2, UINT32 m, UINT32 n)
{
	sh2->r[n] = ((INT32)sh2->r[m] << 16) >> 16;
}

/*  EXTU.B  Rm,Rn */
static inline void EXTUB(sh2_context *sh2, UINT32 m, UINT32 n)
{
	sh2->r[n] = sh2->r[m] & 0x000000ff;
}

/*  EXTU.W  Rm,Rn */
static inline void EXTUW(sh2_context *sh2, UINT32 m, UINT32 n)
{
	sh2->r[n] = sh2->r[m] & 0x0000ffff;
}

/*  ILLEGAL */
static inline void ILLEGAL(sh2_context *sh2)
{
	logerror("SH2.: Illegal opcode at %08x\n", sh2->pc - 2);
	sh2->r[15] -= 4;
	WL( sh2, sh2->r[15], sh2->sr );     /* push SR onto stack */
	sh2->r[15] -= 4;
	WL( sh2, sh2->r[15], sh2->pc - 2 ); /* push PC onto stack */

	/* fetch PC */
	sh2->pc = RL( sh2, sh2->vbr + 4 * 4 );

	/* TODO: timing is a guess */
	sh2->icount -= 5;
}


/*  JMP     @Rm */
static inline void JMP(sh2_context *sh2, UINT32 m)
{
	sh2->delay = sh2->pc;
	sh2->pc = sh2->ea = sh2->r[m];
	sh2->icount--;
}

/*  JSR     @Rm */
static inline void JSR(sh2_context *sh2, UINT32 m)
{
	sh2->delay = sh2->pc;
	sh2->pr = sh2->pc + 2;
	sh2->pc = sh2->ea = sh2->r[m];
	sh2->icount--;
}


/*  LDC     Rm,SR */
static inline void LDCSR(sh2_context *sh2, UINT32 m)
{
	sh2->sr = sh2->r[m] & FLAGS;
	sh2->test_irq = 1;
}

/*  LDC     Rm,GBR */
static inline void LDCGBR(sh2_context *sh2, UINT32 m)
{
	sh2->gbr = sh2->r[m];
}

/*  LDC     Rm,VBR */
static inline void LDCVBR(sh2_context *sh2, UINT32 m)
{
	sh2->vbr = sh2->r[m];
}

/*  LDC.L   @Rm+,SR */
static inline void LDCMSR(sh2_context *sh2, UINT32 m)
{
	sh2->ea = sh2->r[m];
	sh2->sr = RL( sh2, sh2->ea ) & FLAGS;
	sh2->r[m] += 4;
	sh2->icount -= 2;
	sh2->test_irq = 1;
}

/*  LDC.L   @Rm+,GBR */
static inline void LDCMGBR(sh2_context *sh2, UINT32 m)
{
	sh2->ea = sh2->r[m];
	sh2->gbr = RL( sh2, sh2->ea );
	sh2->r[m] += 4;
	sh2->icount -= 2;
}

/*  LDC.L   @Rm+,VBR */
static inline void LDCMVBR(sh2_context *sh2, UINT32 m)
{
	sh2->ea = sh2->r[m];
	sh2->vbr = RL( sh2, sh2->ea );
	sh2->r[m] += 4;
	sh2->icount -= 2;
}

/*  LDS     Rm,MACH */
static inline void LDSMACH(sh2_context *sh2, UINT32 m)
{
	sh2->mach = sh2->r[m];
}

/*  LDS     Rm,MACL */
static inline void LDSMACL(sh2_context *sh2, UINT32 m)
{
	sh2->macl = sh2->r[m];
}

/*  LDS     Rm,PR */
static inline void LDSPR(sh2_context *sh2, UINT32 m)
{
	sh2->pr = sh2->r[m];
}

/*  LDS.L   @Rm+,MACH */
static inline void LDSMMACH(sh2_context *sh2, UINT32 m)
{
	sh2->ea = sh2->r[m];
	sh2->mach = RL( sh2, sh2->ea );
	sh2->r[m] += 4;
}

/*  LDS.L   @Rm+,MACL */
static inline void LDSMMACL(sh2_context *sh2, UINT32 m)
{
	sh2->ea = sh2->r[m];
	sh2->macl = RL( sh2, sh2->ea );
	sh2->r[m] += 4;
}

/*  LDS.L   @Rm+,PR */
static inline void LDSMPR(sh2_context *sh2, UINT32 m)
{
	sh2->ea = sh2->r[m];
	sh2->pr = RL( sh2, sh2->ea );
	sh2->r[m] += 4;
}

/*  MAC.L   @Rm+,@Rn+ */
static inline void MAC_L(sh2_context *sh2, UINT32 m, UINT32 n)
{
	UINT32 RnL, RnH, RmL, RmH, Res0, Res1, Res2;
	UINT32 temp0, temp1, temp2, temp3;
	INT32 tempm, tempn, fnLmL;

	tempn = (INT32) RL( sh2, sh2->r[n] );
	sh2->r[n] += 4;
	tempm = (INT32) RL( sh2, sh2->r[m] );
	sh2->r[m] += 4;
	if ((INT32) (tempn ^ tempm) < 0)
		fnLmL = -1;
	else
		fnLmL = 0;
	if (tempn < 0)
		tempn = 0 - tempn;
	if (tempm < 0)
		tempm = 0 - tempm;
	temp1 = (UINT32) tempn;
	temp2 = (UINT32) tempm;
	RnL = temp1 & 0x0000ffff;
	RnH = (temp1 >> 16) & 0x0000ffff;
	RmL = temp2 & 0x0000ffff;
	RmH = (temp2 >> 16) & 0x0000ffff;
	temp0 = RmL * RnL;
	temp1 = RmH * RnL;
	temp2 = RmL * RnH;
	temp3 = RmH * RnH;
	Res2 = 0;
	Res1 = temp1 + temp2;
	if (Res1 < temp1)
		Res2 += 0x00010000;
	temp1 = (Res1 << 16) & 0xffff0000;
	Res0 = temp0 + temp1;
	if (Res0 < temp0)
		Res2++;
	Res2 = Res2 + ((Res1 >> 16) & 0x0000ffff) + temp3;
	if (fnLmL < 0)
	{
		Res2 = ~Res2;
		if (Res0 == 0)
			Res2++;
		else
			Res0 = (~Res0) + 1;
	}
	if (sh2->sr & S)
	{
		Res0 = sh2->macl + Res0;
		if (sh2->macl > Res0)
			Res2++;
		Res2 += (sh2->mach & 0x0000ffff);
		if (((INT32) Res2 < 0) && (Res2 < 0xffff8000))
		{
			Res2 = 0x00008000;
			Res0 = 0x00000000;
		}
		else if (((INT32) Res2 > 0) && (Res2 > 0x00007fff))
		{
			Res2 = 0x00007fff;
			Res0 = 0xffffffff;
		}
		sh2->mach = Res2;
		sh2->macl = Res0;
	}
	else
	{
		Res0 = sh2->macl + Res0;
		if (sh2->macl > Res0)
			Res2++;
		Res2 += sh2->mach;
		sh2->mach = Res2;
		sh2->macl = Res0;
	}
	sh2->icount -= 2;
}

/*  MAC.W   @Rm+,@Rn+ */
static inline void MAC_W(sh2_context *sh2, UINT32 m, UINT32 n)
{
	INT32 tempm, tempn, dest, src, ans;
	UINT32 templ;

	tempn = (INT32) RW( sh2, sh2->r[n] );
	sh2->r[n] += 2;
	tempm = (INT32) RW( sh2, sh2->r[m] );
	sh2->r[m] += 2;
	templ = sh2->macl;
	tempm = ((INT32) (short) tempn * (INT32) (short) tempm);
	if ((INT32) sh2->macl >= 0)
		dest = 0;
	else
		dest = 1;
	if ((INT32) tempm >= 0)
	{
		src = 0;
		tempn = 0;
	}
	else
	{
		src = 1;
		tempn = 0xffffffff;
	}
	src += dest;
	sh2->macl += tempm;
	if ((INT32) sh2->macl >= 0)
		ans = 0;
	else
		ans = 1;
	ans += dest;
	if (sh2->sr & S)
	{
		if (ans == 1)
			{
				if (src == 0)
					sh2->macl = 0x7fffffff;
				if (src == 2)
					sh2->macl = 0x80000000;
			}
	}
	else
	{
		sh2->mach += tempn;
		if (templ > sh2->macl)
			sh2->mach += 1;
	}
	sh2->icount -= 2;
}

/*  MOV     Rm,Rn */
static inline void MOV(sh2_context *sh2, UINT32 m, UINT32 n)
{
	sh2->r[n] = sh2->r[m];
}

/*  MOV.B   Rm,@Rn */
static inline void MOVBS(sh2_context *sh2, UINT32 m, UINT32 n)
{
	sh2->ea = sh2->r[n];
	WB( sh2, sh2->ea, sh2->r[m] & 0x000000ff);
}

/*  MOV.W   Rm,@Rn */
static inline void MOVWS(sh2_context *sh2, UINT32 m, UINT32 n)
{
	sh2->ea = sh2->r[n];
	WW( sh2, sh2->ea, sh2->r[m] & 0x0000ffff);
}

/*  MOV.L   Rm,@Rn */
static inline void MOVLS(sh2_context *sh2, UINT32 m, UINT32 n)
{
	sh2->ea = sh2->r[n];
	WL( sh2, sh2->ea, sh2->r[m] );
}

/*  MOV.B   @Rm,Rn */
static inline void MOVBL(sh2_context *sh2, UINT32 m, UINT32 n)
{
	sh2->ea = sh2->r[m];
	sh2->r[n] = (UINT32)(INT32)(INT16)(INT8) RB( sh2, sh2->ea );
}

/*  MOV.W   @Rm,Rn */
static inline void MOVWL(sh2_context *sh2, UINT32 m, UINT32 n)
{
	sh2->ea = sh2->r[m];
	sh2->r[n] = (UINT32)(INT32)(INT16) RW( sh2, sh2->ea );
}

/*  MOV.L   @Rm,Rn */
static inline void MOVLL(sh2_context *sh2, UINT32 m, UINT32 n)
{
	sh2->ea = sh2->r[m];
	sh2->r[n] = RL( sh2, sh2->ea );
}

/*  MOV.B   Rm,@-Rn */
static inline void MOVBM(sh2_context *sh2, UINT32 m, UINT32 n)
{
	/* SMG : bug fix, was reading sh2->r[n] */
	UINT32 data = sh2->r[m] & 0x000000ff;

	sh2->r[n] -= 1;
	WB( sh2, sh2->r[n], data );
}

/*  MOV.W   Rm,@-Rn */
static inline void MOVWM(sh2_context *sh2, UINT32 m, UINT32 n)
{
	UINT32 data = sh2->r[m] & 0x0000ffff;

	sh2->r[n] -= 2;
	WW( sh2, sh2->r[n], data );
}

/*  MOV.L   Rm,@-Rn */
static inline void MOVLM(sh2_context *sh2, UINT32 m, UINT32 n)
{
	UINT32 data = sh2->r[m];

	sh2->r[n] -= 4;
	WL( sh2, sh2->r[n], data );
}

/*  MOV.B   @Rm+,Rn */
static inline void MOVBP(sh2_context *sh2, UINT32 m, UINT32 n)
{
	sh2->r[n] = (UINT32)(INT32)(INT16)(INT8) RB( sh2, sh2->r[m] );
	if (n != m)
		sh2->r[m] += 1;
}

/*  MOV.W   @Rm+,Rn */
static inline void MOVWP(sh2_context *sh2, UINT32 m, UINT32 n)
{
	sh2->r[n] = (UINT32)(INT32)(INT16) RW( sh2, sh2->r[m] );
	if (n != m)
		sh2->r[m] += 2;
}

/*  MOV.L   @Rm+,Rn */
static inline void MOVLP(sh2_context *sh2, UINT32 m, UINT32 n)
{
	sh2->r[n] = RL( sh2, sh2->r[m] );
	if (n != m)
		sh2->r[m] += 4;
}

/*  MOV.B   Rm,@(R0,Rn) */
static inline void MOVBS0(sh2_context *sh2, UINT32 m, UINT32 n)
{
	sh2->ea = sh2->r[n] + sh2->r[0];
	WB( sh2, sh2->ea, sh2->r[m] & 0x000000ff );
}

/*  MOV.W   Rm,@(R0,Rn) */
static inline void MOVWS0(sh2_context *sh2, UINT32 m, UINT32 n)
{
	sh2->ea = sh2->r[n] + sh2->r[0];
	WW( sh2, sh2->ea, sh2->r[m] & 0x0000ffff );
}

/*  MOV.L   Rm,@(R0,Rn) */
static inline void MOVLS0(sh2_context *sh2, UINT32 m, UINT32 n)
{
	sh2->ea = sh2->r[n] + sh2->r[0];
	WL( sh2, sh2->ea, sh2->r[m] );
}

/*  MOV.B   @(R0,Rm),Rn */
static inline void MOVBL0(sh2_context *sh2, UINT32 m, UINT32 n)
{
	sh2->ea = sh2->r[m] + sh2->r[0];
	sh2->r[n] = (UINT32)(INT32)(INT16)(INT8) RB( sh2, sh2->ea );
}

/*  MOV.W   @(R0,Rm),Rn */
static inline void MOVWL0(sh2_context *sh2, UINT32 m, UINT32 n)
{
	sh2->ea = sh2->r[m] + sh2->r[0];
	sh2->r[n] = (UINT32)(INT32)(INT16) RW( sh2, sh2->ea );
}

/*  MOV.L   @(R0,Rm),Rn */
static inline void MOVLL0(sh2_context *sh2, UINT32 m, UINT32 n)
{
	sh2->ea = sh2->r[m] + sh2->r[0];
	sh2->r[n] = RL( sh2, sh2->ea );
}

/*  MOV     #imm,Rn */
static inline void MOVI(sh2_context *sh2, UINT32 i, UINT32 n)
{
	sh2->r[n] = (UINT32)(INT32)(INT16)(INT8) i;
}

/*  MOV.W   @(disp8,PC),Rn */
static inline void MOVWI(sh2_context *sh2, UINT32 d, UINT32 n)
{
	UINT32 disp = d & 0xff;
	sh2->ea = sh2->pc + disp * 2 + 2;
	sh2->r[n] = (UINT32)(INT32)(INT16) RW( sh2, sh2->ea );
}

/*  MOV.L   @(disp8,PC),Rn */
static inline void MOVLI(sh2_context *sh2, UINT32 d, UINT32 n)
{
	UINT32 disp = d & 0xff;
	sh2->ea = ((sh2->pc + 2) & ~3) + disp * 4;
	sh2->r[n] = RL( sh2, sh2->ea );
}

/*  MOV.B   @(disp8,GBR),R0 */
static inline void MOVBLG(sh2_context *sh2, UINT32 d)
{
	UINT32 disp = d & 0xff;
	sh2->ea = sh2->gbr + disp;
	sh2->r[0] = (UINT32)(INT32)(INT16)(INT8) RB( sh2, sh2->ea );
}

/*  MOV.W   @(disp8,GBR),R0 */
static inline void MOVWLG(sh2_context *sh2, UINT32 d)
{
	UINT32 disp = d & 0xff;
	sh2->ea = sh2->gbr + disp * 2;
	sh2->r[0] = (INT32)(INT16) RW( sh2, sh2->ea );
}

/*  MOV.L   @(disp8,GBR),R0 */
static inline void MOVLLG(sh2_context *sh2, UINT32 d)
{
	UINT32 disp = d & 0xff;
	sh2->ea = sh2->gbr + disp * 4;
	sh2->r[0] = RL( sh2, sh2->ea );
}

/*  MOV.B   R0,@(disp8,GBR) */
static inline void MOVBSG(sh2_context *sh2, UINT32 d)
{
	UINT32 disp = d & 0xff;
	sh2->ea = sh2->gbr + disp;
	WB( sh2, sh2->ea, sh2->r[0] & 0x000000ff );
}

/*  MOV.W   R0,@(disp8,GBR) */
static inline void MOVWSG(sh2_context *sh2, UINT32 d)
{
	UINT32 disp = d & 0xff;
	sh2->ea = sh2->gbr + disp * 2;
	WW( sh2, sh2->ea, sh2->r[0] & 0x0000ffff );
}

/*  MOV.L   R0,@(disp8,GBR) */
static inline void MOVLSG(sh2_context *sh2, UINT32 d)
{
	UINT32 disp = d & 0xff;
	sh2->ea = sh2->gbr + disp * 4;
	WL( sh2, sh2->ea, sh2->r[0] );
}

/*  MOV.B   R0,@(disp4,Rn) */
static inline void MOVBS4(sh2_context *sh2, UINT32 d, UINT32 n)
{
	UINT32 disp = d & 0x0f;
	sh2->ea = sh2->r[n] + disp;
	WB( sh2, sh2->ea, sh2->r[0] & 0x000000ff );
}

/*  MOV.W   R0,@(disp4,Rn) */
static inline void MOVWS4(sh2_context *sh2, UINT32 d, UINT32 n)
{
	UINT32 disp = d & 0x0f;
	sh2->ea = sh2->r[n] + disp * 2;
	WW( sh2, sh2->ea, sh2->r[0] & 0x0000ffff );
}

/* MOV.L Rm,@(disp4,Rn) */
static inline void MOVLS4(sh2_context *sh2, UINT32 m, UINT32 d, UINT32 n)
{
	UINT32 disp = d & 0x0f;
	sh2->ea = sh2->r[n] + disp * 4;
	WL( sh2, sh2->ea, sh2->r[m] );
}

/*  MOV.B   @(disp4,Rm),R0 */
static inline void MOVBL4(sh2_context *sh2, UINT32 m, UINT32 d)
{
	UINT32 disp = d & 0x0f;
	sh2->ea = sh2->r[m] + disp;
	sh2->r[0] = (UINT32)(INT32)(INT16)(INT8) RB( sh2, sh2->ea );
}

/*  MOV.W   @(disp4,Rm),R0 */
static inline void MOVWL4(sh2_context *sh2, UINT32 m, UINT32 d)
{
	UINT32 disp = d & 0x0f;
	sh2->ea = sh2->r[m] + disp * 2;
	sh2->r[0] = (UINT32)(INT32)(INT16) RW( sh2, sh2->ea );
}

/*  MOV.L   @(disp4,Rm),Rn */
static inline void MOVLL4(sh2_context *sh2, UINT32 m, UINT32 d, UINT32 n)
{
	UINT32 disp = d & 0x0f;
	sh2->ea = sh2->r[m] + disp * 4;
	sh2->r[n] = RL( sh2, sh2->ea );
}

/*  MOVA    @(disp8,PC),R0 */
static inline void MOVA(sh2_context *sh2, UINT32 d)
{
	UINT32 disp = d & 0xff;
	sh2->ea = ((sh2->pc + 2) & ~3) + disp * 4;
	sh2->r[0] = sh2->ea;
}

/*  MOVT    Rn */
static inline void MOVT(sh2_context *sh2, UINT32 n)
{
	sh2->r[n] = sh2->sr & T;
}

/*  MUL.L   Rm,Rn */
static inline void MULL(sh2_context *sh2, UINT32 m, UINT32 n)
{
	sh2->macl = sh2->r[n] * sh2->r[m];
	sh2->icount--;
}

/*  MULS    Rm,Rn */
static inline void MULS(sh2_context *sh2, UINT32 m, UINT32 n)
{
	sh2->macl = (INT16) sh2->r[n] * (INT16) sh2->r[m];
}

/*  MULU    Rm,Rn */
static inline void MULU(sh2_context *sh2, UINT32 m, UINT32 n)
{
	sh2->macl = (UINT16) sh2->r[n] * (UINT16) sh2->r[m];
}

/*  NEG     Rm,Rn */
static inline void NEG(sh2_context *sh2, UINT32 m, UINT32 n)
{
	sh2->r[n] = 0 - sh2->r[m];
}

/*  NEGC    Rm,Rn */
static inline void NEGC(sh2_context *sh2, UINT32 m, UINT32 n)
{
	UINT32 temp;

	temp = sh2->r[m];
	sh2->r[n] = -temp - (sh2->sr & T);
	if (temp || (sh2->sr & T))
		sh2->sr |= T;
	else
		sh2->sr &= ~T;
}

/*  NOP */
static inline void NOP(void)
{
}

/*  NOT     Rm,Rn */
static inline void NOT(sh2_context *sh2, UINT32 m, UINT32 n)
{
	sh2->r[n] = ~sh2->r[m];
}

/*  OR      Rm,Rn */
static inline void OR(sh2_context *sh2, UINT32 m, UINT32 n)
{
	sh2->r[n] |= sh2->r[m];
}

/*  OR      #imm,R0 */
static inline void ORI(sh2_context *sh2, UINT32 i)
{
	sh2->r[0] |= i;
}

/*  OR.B    #imm,@(R0,GBR) */
static inline void ORM(sh2_context *sh2, UINT32 i)
{
	UINT32 temp;

	sh2->ea = sh2->gbr + sh2->r[0];
	temp = RB( sh2, sh2->ea );
	temp |= i;
	WB( sh2, sh2->ea, temp );
	sh2->icount -= 2;
}

/*  ROTCL   Rn */
static inline void ROTCL(sh2_context *sh2, UINT32 n)
{
	UINT32 temp;

	temp = (sh2->r[n] >> 31) & T;
	sh2->r[n] = (sh2->r[n] << 1) | (sh2->sr & T);
	sh2->sr = (sh2->sr & ~T) | temp;
}

/*  ROTCR   Rn */
static inline void ROTCR(sh2_context *sh2, UINT32 n)
{
	UINT32 temp;
	temp = (sh2->sr & T) << 31;
	if (sh2->r[n] & T)
		sh2->sr |= T;
	else
		sh2->sr &= ~T;
	sh2->r[n] = (sh2->r[n] >> 1) | temp;
}

/*  ROTL    Rn */
static inline void ROTL(sh2_context *sh2, UINT32 n)
{
	sh2->sr = (sh2->sr & ~T) | ((sh2->r[n] >> 31) & T);
	sh2->r[n] = (sh2->r[n] << 1) | (sh2->r[n] >> 31);
}

/*  ROTR    Rn */
static inline void ROTR(sh2_context *sh2, UINT32 n)
{
	sh2->sr = (sh2->sr & ~T) | (sh2->r[n] & T);
	sh2->r[n] = (sh2->r[n] >> 1) | (sh2->r[n] << 31);
}

/*  RTE */
static inline void RTE(sh2_context *sh2)
{
	sh2->ea = sh2->r[15];
	sh2->delay = sh2->pc;
	sh2->pc = RL( sh2, sh2->ea );
	sh2->r[15] += 4;
	sh2->ea = sh2->r[15];
	sh2->sr = RL( sh2, sh2->ea ) & FLAGS;
	sh2->r[15] += 4;
	sh2->icount -= 3;
	sh2->test_irq = 1;
}

/*  RTS */
static inline void RTS(sh2_context *sh2)
{
	sh2->delay = sh2->pc;
	sh2->pc = sh2->ea = sh2->pr;
	sh2->icount--;
}

/*  SETT */
static inline void SETT(sh2_context *sh2)
{
	sh2->sr |= T;
}

/*  SHAL    Rn      (same as SHLL) */
static inline void SHAL(sh2_context *sh2, UINT32 n)
{
	sh2->sr = (sh2->sr & ~T) | ((sh2->r[n] >> 31) & T);
	sh2->r[n] <<= 1;
}

/*  SHAR    Rn */
static inline void SHAR(sh2_context *sh2, UINT32 n)
{
	sh2->sr = (sh2->sr & ~T) | (sh2->r[n] & T);
	sh2->r[n] = (UINT32)((INT32)sh2->r[n] >> 1);
}

/*  SHLL    Rn      (same as SHAL) */
static inline void SHLL(sh2_context *sh2, UINT32 n)
{
	sh2->sr = (sh2->sr & ~T) | ((sh2->r[n] >> 31) & T);
	sh2->r[n] <<= 1;
}

/*  SHLL2   Rn */
static inline void SHLL2(sh2_context *sh2, UINT32 n)
{
	sh2->r[n] <<= 2;
}

/*  SHLL8   Rn */
static inline void SHLL8(sh2_context *sh2, UINT32 n)
{
	sh2->r[n] <<= 8;
}

/*  SHLL16  Rn */
static inline void SHLL16(sh2_context *sh2, UINT32 n)
{
	sh2->r[n] <<= 16;
}

/*  SHLR    Rn */
static inline void SHLR(sh2_context *sh2, UINT32 n)
{
	sh2->sr = (sh2->sr & ~T) | (sh2->r[n] & T);
	sh2->r[n] >>= 1;
}

/*  SHLR2   Rn */
static inline void SHLR2(sh2_context *sh2, UINT32 n)
{
	sh2->r[n] >>= 2;
}

/*  SHLR8   Rn */
static inline void SHLR8(sh2_context *sh2, UINT32 n)
{
	sh2->r[n] >>= 8;
}

/*  SHLR16  Rn */
static inline void SHLR16(sh2_context *sh2, UINT32 n)
{
	sh2->r[n] >>= 16;
}

/*  SLEEP */
static inline void SLEEP(sh2_context *sh2)
{
	if(sh2->sleep_mode != 2)
		sh2->pc -= 2;
	sh2->icount -= 2;
	/* Wait_for_exception; */
	if(sh2->sleep_mode == 0)
		sh2->sleep_mode = 1;
	else if(sh2->sleep_mode == 2)
		sh2->sleep_mode = 0;
}

/*  STC     SR,Rn */
static inline void STCSR(sh2_context *sh2, UINT32 n)
{
	sh2->r[n] = sh2->sr;
}

/*  STC     GBR,Rn */
static inline void STCGBR(sh2_context *sh2, UINT32 n)
{
	sh2->r[n] = sh2->gbr;
}

/*  STC     VBR,Rn */
static inline void STCVBR(sh2_context *sh2, UINT32 n)
{
	sh2->r[n] = sh2->vbr;
}

/*  STC.L   SR,@-Rn */
static inline void STCMSR(sh2_context *sh2, UINT32 n)
{
	sh2->r[n] -= 4;
	sh2->ea = sh2->r[n];
	WL( sh2, sh2->ea, sh2->sr );
	sh2->icount--;
}

/*  STC.L   GBR,@-Rn */
static inline void STCMGBR(sh2_context *sh2, UINT32 n)
{
	sh2->r[n] -= 4;
	sh2->ea = sh2->r[n];
	WL( sh2, sh2->ea, sh2->gbr );
	sh2->icount--;
}

/*  STC.L   VBR,@-Rn */
static inline void STCMVBR(sh2_context *sh2, UINT32 n)
{
	sh2->r[n] -= 4;
	sh2->ea = sh2->r[n];
	WL( sh2, sh2->ea, sh2->vbr );
	sh2->icount--;
}

/*  STS     MACH,Rn */
static inline void STSMACH(sh2_context *sh2, UINT32 n)
{
	sh2->r[n] = sh2->mach;
}

/*  STS     MACL,Rn */
static inline void STSMACL(sh2_context *sh2, UINT32 n)
{
	sh2->r[n] = sh2->macl;
}

/*  STS     PR,Rn */
static inline void STSPR(sh2_context *sh2, UINT32 n)
{
	sh2->r[n] = sh2->pr;
}

/*  STS.L   MACH,@-Rn */
static inline void STSMMACH(sh2_context *sh2, UINT32 n)
{
	sh2->r[n] -= 4;
	sh2->ea = sh2->r[n];
	WL( sh2, sh2->ea, sh2->mach );
}

/*  STS.L   MACL,@-Rn */
static inline void STSMMACL(sh2_context *sh2, UINT32 n)
{
	sh2->r[n] -= 4;
	sh2->ea = sh2->r[n];
	WL( sh2, sh2->ea, sh2->macl );
}

/*  STS.L   PR,@-Rn */
static inline void STSMPR(sh2_context *sh2, UINT32 n)
{
	sh2->r[n] -= 4;
	sh2->ea = sh2->r[n];
	WL( sh2, sh2->ea, sh2->pr );
}

/*  SUB     Rm,Rn */
static inline void SUB(sh2_context *sh2, UINT32 m, UINT32 n)
{
	sh2->r[n] -= sh2->r[m];
}

/*  SUBC    Rm,Rn */
static inline void SUBC(sh2_context *sh2, UINT32 m, UINT32 n)
{
	UINT32 tmp0, tmp1;

	tmp1 = sh2->r[n] - sh2->r[m];
	tmp0 = sh2->r[n];
	sh2->r[n] = tmp1 - (sh2->sr & T);
	if (tmp0 < tmp1)
		sh2->sr |= T;
	else
		sh2->sr &= ~T;
	if (tmp1 < sh2->r[n])
		sh2->sr |= T;
}

/*  SUBV    Rm,Rn */
static inline void SUBV(sh2_context *sh2, UINT32 m, UINT32 n)
{
	INT32 dest, src, ans;

	if ((INT32) sh2->r[n] >= 0)
		dest = 0;
	else
		dest = 1;
	if ((INT32) sh2->r[m] >= 0)
		src = 0;
	else
		src = 1;
	src += dest;
	sh2->r[n] -= sh2->r[m];
	if ((INT32) sh2->r[n] >= 0)
		ans = 0;
	else
		ans = 1;
	ans += dest;
	if (src == 1)
	{
		if (ans == 1)
			sh2->sr |= T;
		else
			sh2->sr &= ~T;
	}
	else
		sh2->sr &= ~T;
}

/*  SWAP.B  Rm,Rn */
static inline void SWAPB(sh2_context *sh2, UINT32 m, UINT32 n)
{
	UINT32 temp0, temp1;

	temp0 = sh2->r[m] & 0xffff0000;
	temp1 = (sh2->r[m] & 0x000000ff) << 8;
	sh2->r[n] = (sh2->r[m] >> 8) & 0x000000ff;
	sh2->r[n] = sh2->r[n] | temp1 | temp0;
}

/*  SWAP.W  Rm,Rn */
static inline void SWAPW(sh2_context *sh2, UINT32 m, UINT32 n)
{
	UINT32 temp;

	temp = (sh2->r[m] >> 16) & 0x0000ffff;
	sh2->r[n] = (sh2->r[m] << 16) | temp;
}

/*  TAS.B   @Rn */
static inline void TAS(sh2_context *sh2, UINT32 n)
{
	UINT32 temp;
	sh2->ea = sh2->r[n];
	/* Bus Lock enable */
	temp = RB( sh2, sh2->ea );
	if (temp == 0)
		sh2->sr |= T;
	else
		sh2->sr &= ~T;
	temp |= 0x80;
	/* Bus Lock disable */
	WB( sh2, sh2->ea, temp );
	sh2->icount -= 3;
}

/*  TRAPA   #imm */
static inline void TRAPA(sh2_context *sh2, UINT32 i)
{
	UINT32 imm = i & 0xff;

	sh2->ea = sh2->vbr + imm * 4;

	sh2->r[15] -= 4;
	WL( sh2, sh2->r[15], sh2->sr );
	sh2->r[15] -= 4;
	WL( sh2, sh2->r[15], sh2->pc );

	sh2->pc = RL( sh2, sh2->ea );

	sh2->icount -= 7;
}

/*  TST     Rm,Rn */
static inline void TST(sh2_context *sh2, UINT32 m, UINT32 n)
{
	if ((sh2->r[n] & sh2->r[m]) == 0)
		sh2->sr |= T;
	else
		sh2->sr &= ~T;
}

/*  TST     #imm,R0 */
static inline void TSTI(sh2_context *sh2, UINT32 i)
{
	UINT32 imm = i & 0xff;

	if ((imm & sh2->r[0]) == 0)
		sh2->sr |= T;
	else
		sh2->sr &= ~T;
}

/*  TST.B   #imm,@(R0,GBR) */
static inline void TSTM(sh2_context *sh2, UINT32 i)
{
	UINT32 imm = i & 0xff;

	sh2->ea = sh2->gbr + sh2->r[0];
	if ((imm & RB( sh2, sh2->ea )) == 0)
		sh2->sr |= T;
	else
		sh2->sr &= ~T;
	sh2->icount -= 2;
}

/*  XOR     Rm,Rn */
static inline void XOR(sh2_context *sh2, UINT32 m, UINT32 n)
{
	sh2->r[n] ^= sh2->r[m];
}

/*  XOR     #imm,R0 */
static inline void XORI(sh2_context *sh2, UINT32 i)
{
	UINT32 imm = i & 0xff;
	sh2->r[0] ^= imm;
}

/*  XOR.B   #imm,@(R0,GBR) */
static inline void XORM(sh2_context *sh2, UINT32 i)
{
	UINT32 imm = i & 0xff;
	UINT32 temp;

	sh2->ea = sh2->gbr + sh2->r[0];
	temp = RB( sh2, sh2->ea );
	temp ^= imm;
	WB( sh2, sh2->ea, temp );
	sh2->icount -= 2;
}

/*  XTRCT   Rm,Rn */
static inline void XTRCT(sh2_context *sh2, UINT32 m, UINT32 n)
{
	UINT32 temp;

	temp = (sh2->r[m] << 16) & 0xffff0000;
	sh2->r[n] = (sh2->r[n] >> 16) & 0x0000ffff;
	sh2->r[n] |= temp;
}

/*****************************************************************************
 *  OPCODE DISPATCHERS
 *****************************************************************************/

static inline void op0000(sh2_context *sh2, UINT16 opcode)
{
	switch (opcode & 0x3F)
	{
	case 0x00: ILLEGAL(sh2);                       break;
	case 0x01: ILLEGAL(sh2);                       break;
	case 0x02: STCSR(sh2, Rn);                  break;
	case 0x03: BSRF(sh2, Rn);                   break;
	case 0x04: MOVBS0(sh2, Rm, Rn);             break;
	case 0x05: MOVWS0(sh2, Rm, Rn);             break;
	case 0x06: MOVLS0(sh2, Rm, Rn);             break;
	case 0x07: MULL(sh2, Rm, Rn);               break;
	case 0x08: CLRT(sh2);                       break;
	case 0x09: NOP();                           break;
	case 0x0a: STSMACH(sh2, Rn);                break;
	case 0x0b: RTS(sh2);                        break;
	case 0x0c: MOVBL0(sh2, Rm, Rn);             break;
	case 0x0d: MOVWL0(sh2, Rm, Rn);             break;
	case 0x0e: MOVLL0(sh2, Rm, Rn);             break;
	case 0x0f: MAC_L(sh2, Rm, Rn);              break;

	case 0x10: ILLEGAL(sh2);                       break;
	case 0x11: ILLEGAL(sh2);                       break;
	case 0x12: STCGBR(sh2, Rn);                 break;
	case 0x13: ILLEGAL(sh2);                       break;
	case 0x14: MOVBS0(sh2, Rm, Rn);             break;
	case 0x15: MOVWS0(sh2, Rm, Rn);             break;
	case 0x16: MOVLS0(sh2, Rm, Rn);             break;
	case 0x17: MULL(sh2, Rm, Rn);               break;
	case 0x18: SETT(sh2);                       break;
	case 0x19: DIV0U(sh2);                  break;
	case 0x1a: STSMACL(sh2, Rn);                break;
	case 0x1b: SLEEP(sh2);                  break;
	case 0x1c: MOVBL0(sh2, Rm, Rn);             break;
	case 0x1d: MOVWL0(sh2, Rm, Rn);             break;
	case 0x1e: MOVLL0(sh2, Rm, Rn);             break;
	case 0x1f: MAC_L(sh2, Rm, Rn);              break;

	case 0x20: ILLEGAL(sh2);                       break;
	case 0x21: ILLEGAL(sh2);                       break;
	case 0x22: STCVBR(sh2, Rn);                 break;
	case 0x23: BRAF(sh2, Rn);                   break;
	case 0x24: MOVBS0(sh2, Rm, Rn);             break;
	case 0x25: MOVWS0(sh2, Rm, Rn);             break;
	case 0x26: MOVLS0(sh2, Rm, Rn);             break;
	case 0x27: MULL(sh2, Rm, Rn);               break;
	case 0x28: CLRMAC(sh2);                 break;
	case 0x29: MOVT(sh2, Rn);                   break;
	case 0x2a: STSPR(sh2, Rn);                  break;
	case 0x2b: RTE(sh2);                        break;
	case 0x2c: MOVBL0(sh2, Rm, Rn);             break;
	case 0x2d: MOVWL0(sh2, Rm, Rn);             break;
	case 0x2e: MOVLL0(sh2, Rm, Rn);             break;
	case 0x2f: MAC_L(sh2, Rm, Rn);              break;

	case 0x30: ILLEGAL(sh2);                       break;
	case 0x31: ILLEGAL(sh2);                       break;
	case 0x32: ILLEGAL(sh2);                       break;
	case 0x33: ILLEGAL(sh2);                       break;
	case 0x34: MOVBS0(sh2, Rm, Rn);             break;
	case 0x35: MOVWS0(sh2, Rm, Rn);             break;
	case 0x36: MOVLS0(sh2, Rm, Rn);             break;
	case 0x37: MULL(sh2, Rm, Rn);               break;
	case 0x38: ILLEGAL(sh2);                       break;
	case 0x39: ILLEGAL(sh2);                       break;
	case 0x3c: MOVBL0(sh2, Rm, Rn);             break;
	case 0x3d: MOVWL0(sh2, Rm, Rn);             break;
	case 0x3e: MOVLL0(sh2, Rm, Rn);             break;
	case 0x3f: MAC_L(sh2, Rm, Rn);              break;
	case 0x3a: ILLEGAL(sh2);                       break;
	case 0x3b: ILLEGAL(sh2);                       break;



	}
}

static inline void op0001(sh2_context *sh2, UINT16 opcode)
{
	MOVLS4(sh2, Rm, opcode & 0x0f, Rn);
}

static inline void op0010(sh2_context *sh2, UINT16 opcode)
{
	switch (opcode & 15)
	{
	case  0: MOVBS(sh2, Rm, Rn);                break;
	case  1: MOVWS(sh2, Rm, Rn);                break;
	case  2: MOVLS(sh2, Rm, Rn);                break;
	case  3: ILLEGAL(sh2);                         break;
	case  4: MOVBM(sh2, Rm, Rn);                break;
	case  5: MOVWM(sh2, Rm, Rn);                break;
	case  6: MOVLM(sh2, Rm, Rn);                break;
	case  7: DIV0S(sh2, Rm, Rn);                break;
	case  8: TST(sh2, Rm, Rn);                  break;
	case  9: AND(sh2, Rm, Rn);                  break;
	case 10: XOR(sh2, Rm, Rn);                  break;
	case 11: OR(sh2, Rm, Rn);                   break;
	case 12: CMPSTR(sh2, Rm, Rn);               break;
	case 13: XTRCT(sh2, Rm, Rn);                break;
	case 14: MULU(sh2, Rm, Rn);                 break;
	case 15: MULS(sh2, Rm, Rn);                 break;
	}
}

static inline void op0011(sh2_context *sh2, UINT16 opcode)
{
	switch (opcode & 15)
	{
	case  0: CMPEQ(sh2, Rm, Rn);                break;
	case  1: ILLEGAL(sh2);                         break;
	case  2: CMPHS(sh2, Rm, Rn);                break;
	case  3: CMPGE(sh2, Rm, Rn);                break;
	case  4: DIV1(sh2, Rm, Rn);                 break;
	case  5: DMULU(sh2, Rm, Rn);                break;
	case  6: CMPHI(sh2, Rm, Rn);                break;
	case  7: CMPGT(sh2, Rm, Rn);                break;
	case  8: SUB(sh2, Rm, Rn);                  break;
	case  9: ILLEGAL(sh2);                         break;
	case 10: SUBC(sh2, Rm, Rn);                 break;
	case 11: SUBV(sh2, Rm, Rn);                 break;
	case 12: ADD(sh2, Rm, Rn);                  break;
	case 13: DMULS(sh2, Rm, Rn);                break;
	case 14: ADDC(sh2, Rm, Rn);                 break;
	case 15: ADDV(sh2, Rm, Rn);                 break;
	}
}

static inline void op0100(sh2_context *sh2, UINT16 opcode)
{
	switch (opcode & 0x3F)
	{
	case 0x00: SHLL(sh2, Rn);                   break;
	case 0x01: SHLR(sh2, Rn);                   break;
	case 0x02: STSMMACH(sh2, Rn);               break;
	case 0x03: STCMSR(sh2, Rn);                 break;
	case 0x04: ROTL(sh2, Rn);                   break;
	case 0x05: ROTR(sh2, Rn);                   break;
	case 0x06: LDSMMACH(sh2, Rn);               break;
	case 0x07: LDCMSR(sh2, Rn);                 break;
	case 0x08: SHLL2(sh2, Rn);                  break;
	case 0x09: SHLR2(sh2, Rn);                  break;
	case 0x0a: LDSMACH(sh2, Rn);                break;
	case 0x0b: JSR(sh2, Rn);                    break;
	case 0x0c: ILLEGAL(sh2);                       break;
	case 0x0d: ILLEGAL(sh2);                       break;
	case 0x0e: LDCSR(sh2, Rn);                  break;
	case 0x0f: MAC_W(sh2, Rm, Rn);              break;

	case 0x10: DT(sh2, Rn);                     break;
	case 0x11: CMPPZ(sh2, Rn);                  break;
	case 0x12: STSMMACL(sh2, Rn);               break;
	case 0x13: STCMGBR(sh2, Rn);                break;
	case 0x14: ILLEGAL(sh2);                       break;
	case 0x15: CMPPL(sh2, Rn);                  break;
	case 0x16: LDSMMACL(sh2, Rn);               break;
	case 0x17: LDCMGBR(sh2, Rn);                break;
	case 0x18: SHLL8(sh2, Rn);                  break;
	case 0x19: SHLR8(sh2, Rn);                  break;
	case 0x1a: LDSMACL(sh2, Rn);                break;
	case 0x1b: TAS(sh2, Rn);                    break;
	case 0x1c: ILLEGAL(sh2);                       break;
	case 0x1d: ILLEGAL(sh2);                       break;
	case 0x1e: LDCGBR(sh2, Rn);                 break;
	case 0x1f: MAC_W(sh2, Rm, Rn);              break;

	case 0x20: SHAL(sh2, Rn);                   break;
	case 0x21: SHAR(sh2, Rn);                   break;
	case 0x22: STSMPR(sh2, Rn);                 break;
	case 0x23: STCMVBR(sh2, Rn);                break;
	case 0x24: ROTCL(sh2, Rn);                  break;
	case 0x25: ROTCR(sh2, Rn);                  break;
	case 0x26: LDSMPR(sh2, Rn);                 break;
	case 0x27: LDCMVBR(sh2, Rn);                break;
	case 0x28: SHLL16(sh2, Rn);                 break;
	case 0x29: SHLR16(sh2, Rn);                 break;
	case 0x2a: LDSPR(sh2, Rn);                  break;
	case 0x2b: JMP(sh2, Rn);                    break;
	case 0x2c: ILLEGAL(sh2);                       break;
	case 0x2d: ILLEGAL(sh2);                       break;
	case 0x2e: LDCVBR(sh2, Rn);                 break;
	case 0x2f: MAC_W(sh2, Rm, Rn);              break;

	case 0x30: ILLEGAL(sh2);                       break;
	case 0x31: ILLEGAL(sh2);                       break;
	case 0x32: ILLEGAL(sh2);                       break;
	case 0x33: ILLEGAL(sh2);                       break;
	case 0x34: ILLEGAL(sh2);                       break;
	case 0x35: ILLEGAL(sh2);                       break;
	case 0x36: ILLEGAL(sh2);                       break;
	case 0x37: ILLEGAL(sh2);                       break;
	case 0x38: ILLEGAL(sh2);                       break;
	case 0x39: ILLEGAL(sh2);                       break;
	case 0x3a: ILLEGAL(sh2);                       break;
	case 0x3b: ILLEGAL(sh2);                       break;
	case 0x3c: ILLEGAL(sh2);                       break;
	case 0x3d: ILLEGAL(sh2);                       break;
	case 0x3e: ILLEGAL(sh2);                       break;
	case 0x3f: MAC_W(sh2, Rm, Rn);              break;

	}
}

static inline void op0101(sh2_context *sh2, UINT16 opcode)
{
	MOVLL4(sh2, Rm, opcode & 0x0f, Rn);
}

static inline void op0110(sh2_context *sh2, UINT16 opcode)
{
	switch (opcode & 15)
	{
	case  0: MOVBL(sh2, Rm, Rn);                break;
	case  1: MOVWL(sh2, Rm, Rn);                break;
	case  2: MOVLL(sh2, Rm, Rn);                break;
	case  3: MOV(sh2, Rm, Rn);                  break;
	case  4: MOVBP(sh2, Rm, Rn);                break;
	case  5: MOVWP(sh2, Rm, Rn);                break;
	case  6: MOVLP(sh2, Rm, Rn);                break;
	case  7: NOT(sh2, Rm, Rn);                  break;
	case  8: SWAPB(sh2, Rm, Rn);                break;
	case  9: SWAPW(sh2, Rm, Rn);                break;
	case 10: NEGC(sh2, Rm, Rn);                 break;
	case 11: NEG(sh2, Rm, Rn);                  break;
	case 12: EXTUB(sh2, Rm, Rn);                break;
	case 13: EXTUW(sh2, Rm, Rn);                break;
	case 14: EXTSB(sh2, Rm, Rn);                break;
	case 15: EXTSW(sh2, Rm, Rn);                break;
	}
}

static inline void op0111(sh2_context *sh2, UINT16 opcode)
{
	ADDI(sh2, opcode & 0xff, Rn);
}

static inline void op1000(sh2_context *sh2, UINT16 opcode)
{
	switch ( opcode  & (15<<8) )
	{
	case  0 << 8: MOVBS4(sh2, opcode & 0x0f, Rm);   break;
	case  1 << 8: MOVWS4(sh2, opcode & 0x0f, Rm);   break;
	case  2<< 8: ILLEGAL(sh2);                 break;
	case  3<< 8: ILLEGAL(sh2);                 break;
	case  4<< 8: MOVBL4(sh2, Rm, opcode & 0x0f);    break;
	case  5<< 8: MOVWL4(sh2, Rm, opcode & 0x0f);    break;
	case  6<< 8: ILLEGAL(sh2);                 break;
	case  7<< 8: ILLEGAL(sh2);                 break;
	case  8<< 8: CMPIM(sh2, opcode & 0xff);     break;
	case  9<< 8: BT(sh2, opcode & 0xff);        break;
	case 10<< 8: ILLEGAL(sh2);                 break;
	case 11<< 8: BF(sh2, opcode & 0xff);        break;
	case 12<< 8: ILLEGAL(sh2);                 break;
	case 13<< 8: BTS(sh2, opcode & 0xff);       break;
	case 14<< 8: ILLEGAL(sh2);                 break;
	case 15<< 8: BFS(sh2, opcode & 0xff);       break;
	}
}


static inline void op1001(sh2_context *sh2, UINT16 opcode)
{
	MOVWI(sh2, opcode & 0xff, Rn);
}

static inline void op1010(sh2_context *sh2, UINT16 opcode)
{
	BRA(sh2, opcode & 0xfff);
}

static inline void op1011(sh2_context *sh2, UINT16 opcode)
{
	BSR(sh2, opcode & 0xfff);
}

static inline void op1100(sh2_context *sh2, UINT16 opcode)
{
	switch (opcode & (15<<8))
	{
	case  0<<8: MOVBSG(sh2, opcode & 0xff);     break;
	case  1<<8: MOVWSG(sh2, opcode & 0xff);     break;
	case  2<<8: MOVLSG(sh2, opcode & 0xff);     break;
	case  3<<8: TRAPA(sh2, opcode & 0xff);      break;
	case  4<<8: MOVBLG(sh2, opcode & 0xff);     break;
	case  5<<8: MOVWLG(sh2, opcode & 0xff);     break;
	case  6<<8: MOVLLG(sh2, opcode & 0xff);     break;
	case  7<<8: MOVA(sh2, opcode & 0xff);       break;
	case  8<<8: TSTI(sh2, opcode & 0xff);       break;
	case  9<<8: ANDI(sh2, opcode & 0xff);       break;
	case 10<<8: XORI(sh2, opcode & 0xff);       break;
	case 11<<8: ORI(sh2, opcode & 0xff);            break;
	case 12<<8: TSTM(sh2, opcode & 0xff);       break;
	case 13<<8: ANDM(sh2, opcode & 0xff);       break;
	case 14<<8: XORM(sh2, opcode & 0xff);       break;
	case 15<<8: ORM(sh2, opcode & 0xff);            break;
	}
}

static inline void op1101(sh2_context *sh2, UINT16 opcode)
{
	MOVLI(sh2, opcode & 0xff, Rn);
}

static inline void op1110(sh2_context *sh2, UINT16 opcode)
{
	MOVI(sh2, opcode & 0xff, Rn);
}

static inline void op1111(sh2_context *sh2, UINT16 opcode)
{
	ILLEGAL(sh2);
}

/*****************************************************************************
 *  MAME CPU INTERFACE
 *****************************************************************************/

void SH2_Reset( sh2_context *sh2, int manual )
{
    // manual parameter is 1 for a warm reset from a switch... !
    // todo : investigate what gens does with that, for now the parameter is just ignored
	// int  (*dma_callback_kludge)(device_t *device, UINT32 src, UINT32 dst, UINT32 data, int size);
	// int  (*dma_callback_fifo_data_available)(device_t *device, UINT32 src, UINT32 dst, UINT32 data, int size);
	int save_is_slave;

	save_is_slave = sh2->is_slave;
	// dma_callback_kludge = sh2->dma_callback_kludge;
	// dma_callback_fifo_data_available = sh2->dma_callback_fifo_data_available;

	sh2->ppc = sh2->pc = sh2->pr = sh2->sr = sh2->gbr = sh2->vbr = sh2->mach = sh2->macl = 0;
	sh2->numcycles = 0;
	sh2->evec = sh2->irqsr = 0;
	memset(&sh2->r[0], 0, sizeof(sh2->r[0])*16);
	sh2->ea = sh2->delay = sh2->cpu_off = sh2->dvsr = sh2->dvdnth = sh2->dvdntl = sh2->dvcr = 0;
	sh2->pending_irq = sh2->test_irq = 0;
	memset(&sh2->irq_queue[0], 0, sizeof(sh2->irq_queue[0])*16);
	memset(&sh2->irq_line_state[0], 0, sizeof(sh2->irq_line_state[0])*17);
	sh2->frc = sh2->ocra = sh2->ocrb = sh2->icr = 0;
	sh2->frc_base = 0;
	sh2->frt_input = sh2->internal_irq_level = sh2->internal_irq_vector = 0;
	sh2->dma_timer_active[0] = sh2->dma_timer_active[1] = 0;
	sh2->dma_irq[0] = sh2->dma_irq[1] = 0;

	// sh2->dma_callback_kludge = dma_callback_kludge;
	// sh2->dma_callback_fifo_data_available = dma_callback_fifo_data_available;
	sh2->is_slave = save_is_slave;

	memset(sh2->m, 0, 0x200);

	sh2->pc = RL(sh2, 0);
	sh2->r[15] = RL(sh2, 4);
	sh2->sr = I;
	sh2->sleep_mode = 0;

	sh2->internal_irq_level = -1;
}

/*-------------------------------------------------
    sh1_reset - reset the processor
-------------------------------------------------*/

void SH1_Reset( sh2_context *sh2 )
{
	SH2_Reset( sh2,0 );
	sh2->cpu_type = CPU_TYPE_SH1;
}

/* Execute cycles - returns number of cycles actually run */
int SH2_Exec( sh2_context *sh2, u32 cycles )
{
	if (sh2->cpu_off)
	{
		sh2->icount = 0;
		return -1;
	}

	sh2->icount = cycles;

	// run any active DMAs now
#ifndef USE_TIMER_FOR_DMA
	if (sh2->dma_timer_active[0]) {
	    for ( int i = 0; i < sh2->icount ; i++)
	    {
		for( int dma=0;dma<1;dma++)
		{
		    if (sh2->dma_timer_active[dma]) {
			printf("sh2_do_dma ?!!\n"); // obviously shouldn't happen for now !
			exit(1);
			// sh2_do_dma(sh2, dma);
		    }
		}
	    }
	}
#endif

	do
	{
		UINT32 opcode;

		if (sh2->delay)
		{
			opcode = sh2->Read_Word[(sh2->delay ) >> 24](((UINT32)(sh2->delay)));
			sh2->pc -= 2;
		}
		else
			opcode = sh2->Read_Word[sh2->pc >> 24](((UINT32)(sh2->pc)));

		sh2->delay = 0;
		sh2->pc += 2;
		sh2->ppc = sh2->pc;

		switch (opcode & ( 15 << 12))
		{
		case  0<<12: op0000(sh2, opcode); break;
		case  1<<12: op0001(sh2, opcode); break;
		case  2<<12: op0010(sh2, opcode); break;
		case  3<<12: op0011(sh2, opcode); break;
		case  4<<12: op0100(sh2, opcode); break;
		case  5<<12: op0101(sh2, opcode); break;
		case  6<<12: op0110(sh2, opcode); break;
		case  7<<12: op0111(sh2, opcode); break;
		case  8<<12: op1000(sh2, opcode); break;
		case  9<<12: op1001(sh2, opcode); break;
		case 10<<12: op1010(sh2, opcode); break;
		case 11<<12: op1011(sh2, opcode); break;
		case 12<<12: op1100(sh2, opcode); break;
		case 13<<12: op1101(sh2, opcode); break;
		case 14<<12: op1110(sh2, opcode); break;
		default: op1111(sh2, opcode); break;
		}

		if(sh2->test_irq && !sh2->delay)
		{
			CHECK_PENDING_IRQ("mame_sh2_execute");
			sh2->test_irq = 0;
		}
		sh2->icount--;
	} while( sh2->icount > 0 );
	sh2->numcycles += cycles + sh2->icount;
	return cycles + sh2->icount;
}

void SH2_Clear_Odo(sh2_context *sh2) {
    sh2->numcycles = 0;
}

UINT64 SH2_Read_Odo(sh2_context *sh2) {
    return sh2->numcycles;
}

/**************************************************************************
 * Generic set_info
 **************************************************************************/

