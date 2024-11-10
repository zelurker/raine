#ifndef DASM_H
#define DASM_H

#define DASMFLAG_SUPPORTED     0x80000000   // are disassembly flags supported?
#define DASMFLAG_STEP_OUT      0x40000000   // this instruction should be the end of a step out sequence
#define DASMFLAG_STEP_OVER     0x20000000   // this instruction should be stepped over by setting a breakpoint afterwards
#define DASMFLAG_OVERINSTMASK  0x18000000   // number of extra instructions to skip when stepping over
#define DASMFLAG_OVERINSTSHIFT 27           // bits to shift after masking to get the value
#define DASMFLAG_LENGTHMASK    0x0000ffff   // the low 16-bits contain the actual length

#define DASMFLAG_STEP_OVER_EXTRA(x)         ((x) << DASMFLAG_OVERINSTSHIFT)

#ifdef __cplusplus
extern "C" {
#endif

unsigned DasmSH2(char *buffer, unsigned pc, UINT16 opcode);
int dasm_6502(char *buffer, unsigned pc, const UINT8 *oprom, const UINT8 *opram, int options);
int dasm_65c02(char *buffer, unsigned pc, const UINT8 *oprom, const UINT8 *opram, int options);
unsigned z80_dasm(char *buffer, int pc, const UINT8 *oprom, const UINT8 *opram);
unsigned int m68k_disassemble(int cpu_id, char* str_buff, unsigned int pc, unsigned int cpu_type);

#ifdef __cplusplus
}
#endif

#endif
