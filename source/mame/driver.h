
#ifdef __cplusplus
extern "C" {
#endif
/* Learn Raine how to speak the Mame language... */
#ifndef DRIVER_H
// Mame include files have the bad habit to include each other...

#define DRIVER_H

#include "memory.h"

/* driver.h */

/* set this if the CPU is used as a slave for audio. It will not be emulated if */
/* sound is disabled, therefore speeding up a lot the emulation. */
#define CPU_AUDIO_CPU 0x8000

/* the Z80 can be wired to use 16 bit addressing for I/O ports */
#define CPU_16BIT_PORT 0x4000

#define CPU_FLAGS_MASK 0xff00


#define MAX_CPU 8	/* MAX_CPU is the maximum number of CPUs which cpuintrf.c */
					/* can run at the same time. Currently, 8 is enough. */


#define MAX_SOUND 5	/* MAX_SOUND is the maximum number of sound subsystems */
					/* which can run at the same time. Currently, 5 is enough. */


#endif

#ifdef __cplusplus
}
#endif
