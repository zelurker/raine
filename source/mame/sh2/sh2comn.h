/*****************************************************************************
 *
 *   sh2common.h
 *
 *   SH-2 non-specific components
 *
 *****************************************************************************/

#pragma once

#ifndef __SH2COMN_H__
#define __SH2COMN_H__



// do we use a timer for the DMA, or have it in CPU_EXECUTE
// #define USE_TIMER_FOR_DMA

#define SH2_CODE_XOR(a)     ((a) ^ NATIVE_ENDIAN_VALUE_LE_BE(2,0))

struct irq_entry
{
	int irq_vector;
	int irq_priority;
};

typedef struct irq_entry irq_entry;

enum
{
	ICF  = 0x00800000,
	OCFA = 0x00080000,
	OCFB = 0x00040000,
	OVF  = 0x00020000
};

#define T   0x00000001
#define S   0x00000002
#define I   0x000000f0
#define Q   0x00000100
#define M   0x00000200

#define AM  0xc7ffffff

#define FLAGS   (M|Q|I|S|T)

#define Rn  ((opcode>>8)&15)
#define Rm  ((opcode>>4)&15)

#define CPU_TYPE_SH1    (0)
#define CPU_TYPE_SH2    (1)

#define CHECK_PENDING_IRQ(message)                                                                             \
{                                                                                                              \
        int irq = -1;                                                                                          \
        if (sh2->pending_irq & (1 <<  0)) irq = 0;                                                             \
        if (sh2->pending_irq & (1 <<  1)) irq = 1;                                                             \
        if (sh2->pending_irq & (1 <<  2)) irq = 2;                                                             \
        if (sh2->pending_irq & (1 <<  3)) irq = 3;                                                             \
        if (sh2->pending_irq & (1 <<  4)) irq = 4;                                                             \
        if (sh2->pending_irq & (1 <<  5)) irq = 5;                                                             \
        if (sh2->pending_irq & (1 <<  6)) irq = 6;                                                             \
        if (sh2->pending_irq & (1 <<  7)) irq = 7;                                                             \
        if (sh2->pending_irq & (1 <<  8)) irq = 8;                                                             \
        if (sh2->pending_irq & (1 <<  9)) irq = 9;                                                             \
        if (sh2->pending_irq & (1 << 10)) irq = 10;                                                            \
        if (sh2->pending_irq & (1 << 11)) irq = 11;                                                            \
        if (sh2->pending_irq & (1 << 12)) irq = 12;                                                            \
        if (sh2->pending_irq & (1 << 13)) irq = 13;                                                            \
        if (sh2->pending_irq & (1 << 14)) irq = 14;                                                            \
        if (sh2->pending_irq & (1 << 15)) irq = 15;                                                            \
        if ((sh2->internal_irq_level != -1) && (sh2->internal_irq_level > irq)) irq = sh2->internal_irq_level; \
        if (irq >= 0)                                                                                          \
                sh2_exception(sh2,message,irq);                                                                \
}

/* fast RAM info */
struct fast_ram_info
{
	u32              start;                      /* start of the RAM block */
	u32              end;                        /* end of the RAM block */
	UINT8               readonly;                   /* TRUE if read-only */
	void *              base;                       /* base in memory where the RAM lives */
};

typedef struct fast_ram_info fast_ram_info;

struct __fetch_region
{
	UINT32 Low_Adr;
	UINT32 High_Adr;
	UINT16 *Fetch_Reg;
};

typedef struct __fetch_region FETCHREG;
typedef UINT8 FASTCALL SH2_RB(UINT32 adr);
typedef UINT16 FASTCALL SH2_RW(UINT32 adr);
typedef UINT32 FASTCALL SH2_RL(UINT32 adr);
typedef void FASTCALL SH2_WB(UINT32 adr, UINT8 data);
typedef void FASTCALL SH2_WW(UINT32 adr, UINT16 data);
typedef void FASTCALL SH2_WL(UINT32 adr, UINT32 data);

struct sh2_state
{
	UINT32  ppc;
	UINT32  pc;
	UINT32  pr;
	UINT32  sr;
	UINT32  gbr, vbr;
	UINT32  mach, macl;
	UINT32  r[16];
	UINT32  ea;
	UINT32  delay;
	UINT32  cpu_off;
	UINT32  dvsr, dvdnth, dvdntl, dvcr;
	UINT32  pending_irq;
	UINT32  test_irq;
	UINT32  pending_nmi;
	INT32  irqline;
	UINT32  evec;               // exception vector for DRC
	UINT32  irqsr;              // IRQ-time old SR for DRC
	UINT32 target;              // target for jmp/jsr/etc so the delay slot can't kill it
	irq_entry     irq_queue[16];

	int pcfsel;                 // last pcflush entry set
	int maxpcfsel;              // highest valid pcflush entry
	UINT32 pcflushes[16];           // pcflush entries

	INT8    irq_line_state[17];
	UINT32 m[0x200/4];
	INT8  nmi_line_state;

	UINT16  frc;
	UINT16  ocra, ocrb, icr;
	UINT64  frc_base;

	int     frt_input;
	int     internal_irq_level;
	int     internal_irq_vector;
	int             icount;

	// emu_timer *timer;
	// emu_timer *dma_current_active_timer[2];
	int     dma_timer_active[2];
	UINT8  dma_irq[2];

	int active_dma_incs[2];
	int active_dma_incd[2];
	int active_dma_size[2];
	int active_dma_steal[2];
	UINT32 active_dma_src[2];
	UINT32 active_dma_dst[2];
	UINT32 active_dma_count[2];
	UINT16 wtcnt;
	UINT8 wtcsr;

	UINT8 sleep_mode;

	int     is_slave, cpu_type;
	// int  (*dma_callback_kludge)(device_t *device, UINT32 src, UINT32 dst, UINT32 data, int size);
	// int  (*dma_callback_fifo_data_available)(device_t *device, UINT32 src, UINT32 dst, UINT32 data, int size);

	// void    (*ftcsr_read_callback)(UINT32 data);

	/* internal stuff */
	UINT8               cache_dirty;                /* true if we need to flush the cache */

	/* parameters for subroutines */
	UINT64              numcycles;              /* return value from gettotalcycles */
	UINT32              irq;                /* irq we're taking */

	/* fast RAM (untested in raine) */
	UINT32              fastram_select;
	fast_ram_info       fastram[SH2_MAX_FASTRAM];
	SH2_RB     *Read_Byte[0x100];
	SH2_RW     *Read_Word[0x100];
	SH2_RL     *Read_Long[0x100];

	SH2_WB     *Write_Byte[0x100];
	SH2_WW     *Write_Word[0x100];
	SH2_WL     *Write_Long[0x100];

	// The fetch region is here only for the console actually
	// after testing in Exec, there is no noticeable acceleration if using a fetch_word for the opcodes
	// I guess it would require more optimizations with a Base_PC as gens did, but I won't do that for now
	// so for now this is only for the console !
	FETCHREG   Fetch_Region[0x100];
};

typedef struct sh2_state sh2_context;

extern int sh2Engine;
extern sh2_context M_SH2;
extern sh2_context S_SH2;

extern int sh2Engine;
void SH2_Map_Cache_Trough (sh2_context * SH2);
void SH2_Init(sh2_context *sh2, int is_slave);
u32 SH2_Get_PC(sh2_context *sh2);
u32 SH2_Get_SR(sh2_context *sh2);
void sh2_recalc_irq(sh2_context *sh2);
void sh2_set_irq_line(sh2_context *sh2, int irqline, int state);
void sh2_exception(sh2_context *sh2, const char *message, int irqline);
void sh2_do_dma(sh2_context *sh2, int dma);
void SH2_ReleaseTimeSlice(sh2_context *M_SH2);
void SH2_Interrupt(sh2_context *sh2, int irq);
// For now no fetch, will test if necessary or not... (actually used at least by the console, get_code_range)
void SH2_Add_Fetch (sh2_context * SH2, UINT32 low_adr, UINT32 high_adr, UINT16 * Fetch);

// void sh2_notify_dma_data_available(device_t *device);

void SH2_Add_ReadB (sh2_context * SH2, UINT32 low_adr, UINT32 high_adr, SH2_RB * Func);
void SH2_Add_ReadW (sh2_context * SH2, UINT32 low_adr, UINT32 high_adr, SH2_RW * Func);
void SH2_Add_ReadL (sh2_context * SH2, UINT32 low_adr, UINT32 high_adr, SH2_RL * Func);

void SH2_Add_WriteB (sh2_context * SH2, UINT32 low_adr, UINT32 high_adr, SH2_WB * Func);
void SH2_Add_WriteW (sh2_context * SH2, UINT32 low_adr, UINT32 high_adr, SH2_WW * Func);
void SH2_Add_WriteL (sh2_context * SH2, UINT32 low_adr, UINT32 high_adr, SH2_WL * Func);
UINT8 *get_sh2_code_range(sh2_context *SH2,UINT32 adr,UINT32 *start,UINT32 *end);
UINT8 *get_sh2_userdata(sh2_context *SH2, UINT32 ptr);
UINT8 SH2_Read_Byte(sh2_context *sh2, UINT32 adr);
UINT16 SH2_Read_Word(sh2_context *sh2, UINT32 adr);
UINT32 SH2_Read_Long(sh2_context *sh2, UINT32 adr);
void SH2_Write_Byte(sh2_context *sh2, UINT32 adr, UINT8 data);
void SH2_Write_Word(sh2_context *sh2, UINT32 adr, UINT16 data);
void SH2_Write_Long(sh2_context *sh2, UINT32 adr, UINT32 data);
UINT32 FASTCALL SH2_Get_R(sh2_context *sh2, UINT32 n);
void FASTCALL SH2_Set_SR(sh2_context *sh2, UINT32 sr);
void FASTCALL SH2_Set_PC(sh2_context *sh2, UINT32 pc);

#endif /* __SH2COMN_H__ */
