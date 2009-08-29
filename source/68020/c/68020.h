
#ifdef __cplusplus
extern "C" {
#endif
/*
 *  RAINE 68020 Interface with UAE engine
 */

#include "raine.h"


// Reset 68020():
//
// Reset 68020 regs and read Stack/PC from Vector table

void Reset68020(void);

// Execute68020(int c):
//
// Execute 68020 for c cycles

void Execute68020(int c);

// Interrupt68020(int level):
//
// Attempt to call Interrupt level (if Interrupt level is in enabled in SR)

void Interrupt68020(int level);

// Stop68020():
//
// Stop Execute68020() loop (for speed hacks)

void Stop68020(void);

#ifdef __cplusplus
}
#endif
