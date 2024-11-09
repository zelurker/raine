/*****************************************************************************
 *
 *   sh2.h
 *   Portable Hitachi SH-2 (SH7600 family) emulator interface
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
 *  the SH-2 CPU core and was heavily changed to the MAME CPU requirements.
 *  Thanks also go to Chuck Mason <chukjr@sundail.net> and Olivier Galibert
 *  <galibert@pobox.com> for letting me peek into their SEMU code :-)
 *
 *****************************************************************************/

#pragma once

#ifndef __SH2_H__
#define __SH2_H__

#include "deftypes.h"

#ifndef __GNUC__

#define FASTCALL				__fastcall
#define DECL_FASTCALL(type, name)	type FASTCALL name

#else //__GNUC__

#define FASTCALL __attribute__ ((regparm(2)))
#define DECL_FASTCALL(type, name)	type name __attribute__ ((regparm(2)))

#endif //!__GNUC__

#define SH2_INT_NONE    -1
#define SH2_INT_VBLIN   0
#define SH2_INT_VBLOUT  1
#define SH2_INT_HBLIN   2
#define SH2_INT_TIMER0  3
#define SH2_INT_TIMER1  4
#define SH2_INT_DSP     5
#define SH2_INT_SOUND   6
#define SH2_INT_SMPC    7
#define SH2_INT_PAD     8
#define SH2_INT_DMA2    9
#define SH2_INT_DMA1    10
#define SH2_INT_DMA0    11
#define SH2_INT_DMAILL  12
#define SH2_INT_SPRITE  13
#define SH2_INT_14      14
#define SH2_INT_15      15
#define SH2_INT_ABUS    16

enum
{
	SH2_PC=1, SH2_SR, SH2_PR, SH2_GBR, SH2_VBR, SH2_MACH, SH2_MACL,
	SH2_R0, SH2_R1, SH2_R2, SH2_R3, SH2_R4, SH2_R5, SH2_R6, SH2_R7,
	SH2_R8, SH2_R9, SH2_R10, SH2_R11, SH2_R12, SH2_R13, SH2_R14, SH2_R15, SH2_EA
};

/***************************************************************************
    COMPILER-SPECIFIC OPTIONS
***************************************************************************/

#define SH2_MAX_FASTRAM       4

#include "sh2comn.h"

void SH2_Reset( sh2_context *sh2, int manual );
void SH1_Reset( sh2_context *sh2 );
int SH2_Exec( sh2_context *sh2, u32 cycles );
void SH2_Clear_Odo(sh2_context *sh2);
UINT64 SH2_Read_Odo(sh2_context *sh2);

#endif /* __SH2_H__ */
