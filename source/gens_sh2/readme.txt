*****************************************
*      SSH2 emulator version 1.60       *
*  Emulator for x86 CPU based platform  *
* Copyright 2002 Stéphane Dallongeville *
*****************************************

This emulator is used in Gens for the SH-2 (Hitachi SH7604) CPU
emulation required by the 32X system, it's still incomplete because
some features aren't needed to achieve 32X emulation but here's what
is done :
- 100% SH-2 instruction set implemented
- Fetch/Read/Write from/to cache area supported
- External division unit
- External DMAC unit
- External watchdog timer
- External free run timer

Hopefully that is enough for almost case.

The emulator can be freely distribued and used for any non commercial
project as long you don't forget to credit me somewhere :)
If you want some support about the SH-2 emulator, you can contact me on
the Gens forum (http://gens.consolemul.com then go to the forum).


You should find the following files in the emulation pack :
- sh2.c        -> contains initialisation stuff
- sh2a.asm     -> contains the emulation core itself
- sh2_IO.inc   -> contains the external IO emulation part
- sh2.h        -> header file
- readme.txt   -> the current file you're reading ;)


* How compile the emulator ?
****************************

The emulator has been developped with Visual C++ and NASM.
Maybe you will need to modify the UINTXX, INTXX and FASTCALL
definitions (sh2.h) according to your C compiler, except that, sh2.c
should not give any problems for compilation.

For the ASM file you need the NASM assembler :
nasm -f win32 -o sh2a.obj sh2a.asm

Normally you now have the sh2.obj and sh2a.obj files and you're
ready to use the emulator just by linking them in your project.


* How to use the emulator ?
***************************

1) Include the header file in your source :
------------------------------------------

#include "sh2.h"


2) Init the SH-2 core :
-----------------------

If you want to use the internal SH2 context offered :

SH2_Init(&M_SH2,UINT32 slave);

but you can also define your own SH2 context :

SH2_CONTEXT My_Sh2;

....

SH2_Init(&My_Sh2,UINT32 slave);


3) Set up your fetch region (where the SH-2 will run code from) :
-----------------------------------------------------------------

SH2_Add_Fetch(&M_SH2, 0x00000000, 0x0003FFFF, (UINT16 *) ROM);
SH2_Add_Fetch(&M_SH2, 0x02000000, 0x0201FFFF, (UINT16 *) RAM);

// Cache trough
SH2_Add_Fetch(&M_SH2, 0x20000000, 0x2003FFFF, (UINT16 *) ROM);
SH2_Add_Fetch(&M_SH2, 0x22000000, 0x2201FFFF, (UINT16 *) RAM);

// End definition
SH2_Add_Fetch(&M_SH2, 0x00000000, 0x00000000, (UINT16 *) -1);

The last line is used to end the fetch region definition.
You can also use :

SH2_Set_Fetch_Reg(&M_SH2, 0, 0x00000000, 0x0003FFFF, (UINT16 *) ROM);
SH2_Set_Fetch_Reg(&M_SH2, 1, 0x02000000, 0x0201FFFF, (UINT16 *) RAM);

// Cache trough
SH2_Set_Fetch_Reg(&M_SH2, 2, 0x20000000, 0x2003FFFF, (UINT16 *) ROM);
SH2_Set_Fetch_Reg(&M_SH2, 3, 0x22000000, 0x2201FFFF, (UINT16 *) RAM);

// End definition
SH2_Set_Fetch_Reg(&M_SH2, 4, 0x00000000, 0x00000000, (UINT16 *) -1);

This give you more control on the fetch region structure.
Always place the most fetched regions first to increase emulation speed.


4) Set up your memory (where the SH-2 will read and write data) :
-----------------------------------------------------------------

SH2_Add_ReadB(&M_SH2, 0x00, 0x0F, Read_Byte);
SH2_Add_ReadW(&M_SH2, 0x00, 0x0F, Read_Word);
SH2_Add_ReadL(&M_SH2, 0x00, 0x0F, Read_Long);

SH2_Add_WriteB(&M_SH2, 0x00, 0x0F, Write_Byte);
SH2_Add_WriteW(&M_SH2, 0x00, 0x0F, Write_Word);
SH2_Add_WriteL(&M_SH2, 0x00, 0x0F, Write_Long);

You can then add :

SH2_Map_Cache_Trough(&M_SH2);

to automatically add your memory definition in the cache through area.


5) Reset the SH-2 core before fisrt use :
-----------------------------------------

SH2_Reset(&M_SH2, 0);		// Hard reset


6) Do some cycles :
-------------------

Clear the internal cycle counter :

SH2_Clear_Odo(&M_SH2);

Then execute 100000 cyles :

SH2_Exec(&M_SH2, 100000);

If you want to execute 1000 extras cycles :

SH2_Exec(&M_SH2, 100000 + 1000);

or you can re-clear the internal counter first :

SH2_Clear_Odo(&M_SH2);
SH2_Exec(&M_SH2, 1000);


6) Do an interrupt :
--------------------

SH2_Interrupt(&M_SH2, 10);


* SH-2 function description :
*****************************

void SH2_Init(SH2_CONTEXT *SH2, UINT32 Slave);

  Initialise the sh2 core and the sh2 context.
  Have to be called before using the emulator.
  Slave is used to determine if you're initialising a slave CPU.
  Always use '0' for mono CPU system.

UINT32 FASTCALL SH2_Reset(SH2_CONTEXT *SH2, UINT32 manual);

  Reset the sh2 context (memory and fetch regions not reseted)
  The manual flag determine if we want a hard (0) or manual (1) reset.
  As SH2_Init, it has to be called before execute cycles.
  Return 0 if ok and -1 for error.

void SH2_Set_Fetch_Reg(SH2_CONTEXT *SH2, UINT32 reg, UINT32 low_adr, UINT32 high_adr, UINT16 *Fetch);
void SH2_Add_Fetch(SH2_CONTEXT *SH2, UINT32 low_adr, UINT32 high_adr, UINT16 *Fetch);

  Both functions are used to define the fetch area.
  The 'Fetch' pointer define the location where instructions will
  be read when PC is comprise between low_adr and high_adr.

! VERY IMPORTANT ! 'Fetch' must be a DWORD aligned pointer.

void SH2_Add_ReadB(SH2_CONTEXT *SH2, UINT32 low_adr, UINT32 high_adr, SH2_RB *Func);
void SH2_Add_ReadW(SH2_CONTEXT *SH2, UINT32 low_adr, UINT32 high_adr, SH2_RW *Func);
void SH2_Add_ReadL(SH2_CONTEXT *SH2, UINT32 low_adr, UINT32 high_adr, SH2_RL *Func);
void SH2_Add_WriteB(SH2_CONTEXT *SH2, UINT32 low_adr, UINT32 high_adr, SH2_WB *Func);
void SH2_Add_WriteW(SH2_CONTEXT *SH2, UINT32 low_adr, UINT32 high_adr, SH2_WW *Func);
void SH2_Add_WriteL(SH2_CONTEXT *SH2, UINT32 low_adr, UINT32 high_adr, SH2_WL *Func);

  Install a memory callback for a range of memory.
  Range of memory is defined by (low_adr << 24) and (high_adr << 24).
  For instance if you want to call Read_Byte_RAM for the
  0x02000000-0x03FFFFFF range you have to do :
  SH2_Add_ReadB(&M_SH2, 0x02, 0x03, UINT32 low_adr, Read_Byte_RAM);

  Here's the diferent callback function type :

  typedef UINT8 FASTCALL SH2_RB(UINT32 adr);
  typedef UINT16 FASTCALL SH2_RW(UINT32 adr);
  typedef UINT32 FASTCALL SH2_RL(UINT32 adr);
  typedef void FASTCALL SH2_WB(UINT32 adr, UINT8 data);
  typedef void FASTCALL SH2_WW(UINT32 adr, UINT16 data);
  typedef void FASTCALL SH2_WL(UINT32 adr, UINT32 data);

void SH2_Map_Cache_Trough(SH2_CONTEXT *SH2);

  Fast way to map you currents memory definitions in the
  cache through area.

void FASTCALL SH2_Clear_Odo(SH2_CONTEXT *sh2);

  Clear the internal cycle counter.

UINT32 FASTCALL SH2_Read_Odo(SH2_CONTEXT *sh2);

  Return the value of the internal cycle counter.

void FASTCALL SH2_Enable(SH2_CONTEXT *sh2);

  Enable the SH2 (default).

void FASTCALL SH2_Enable(SH2_CONTEXT *sh2);

  Disable the SH2.
  This function is usefull for multi CPU systems where
  sub CPU can be halted.

UINT32 FASTCALL SH2_Exec(SH2_CONTEXT *SH2, UINT32 odo);

  Execute cyles until the internal cycle counter = odo.
  Return value :
  0     = Success
  -1    = No cycle to do
  other = SH-2 state

void FASTCALL SH2_Add_Cycles(SH2_CONTEXT *sh2, UINT32 cycles);

  Add some cycles to the internal cycle counter without executing
  them (can be used for simulate wait state).

void FASTCALL SH2_Write_Odo(SH2_CONTEXT *sh2, UINT32 odo);

  Set the odometer (can be used for short halt state).

void FASTCALL SH2_Interrupt(SH2_CONTEXT *sh2, UINT32 level);

  Generate an interrupt.

void FASTCALL SH2_Interrupt_Internal(SH2_CONTEXT *sh2, UINT32 level_vector);

  Generate an internal interrupt (exception), you normally don't need
  to use this function.
  level_vector = (level << 8) + vector

void FASTCALL SH2_NMI(SH2_CONTEXT *sh2);

  Generate an non maskable interrupt.

void FASTCALL SH2_DMA0_Request(SH2_CONTEXT *sh2, UINT8 state);
void FASTCALL SH2_DMA1_Request(SH2_CONTEXT *sh2, UINT8 state);

  These functions are used for DMA requests (channel 0 or 1).
  When some datas are available on the DMA port,
  call SH2_DMAX_Request() with state != 0.
  When data are read from the DMA port and queue becomes empty,
  call SH2_DMAX_Request() with state = 0.

void FASTCALL SH2_FRT_Signal(SH2_CONTEXT *sh2);

  Send a signal to the FRT unit to catch the current FRT counter.

UINT32 FASTCALL SH2_Get_R(SH2_CONTEXT *sh2, UINT32 num);
UINT32 FASTCALL SH2_Get_PC(SH2_CONTEXT *sh2);
UINT32 FASTCALL SH2_Get_SR(SH2_CONTEXT *sh2);
UINT32 FASTCALL SH2_Get_GBR(SH2_CONTEXT *sh2);
UINT32 FASTCALL SH2_Get_VBR(SH2_CONTEXT *sh2);
UINT32 FASTCALL SH2_Get_PR(SH2_CONTEXT *sh2);
UINT32 FASTCALL SH2_Get_MACH(SH2_CONTEXT *sh2);
UINT32 FASTCALL SH2_Get_MACL(SH2_CONTEXT *sh2);

  Use these functions to get SH-2 registers values.

void FASTCALL SH2_Set_PC(SH2_CONTEXT *sh2, UINT32 val);
void FASTCALL SH2_Set_SR(SH2_CONTEXT *sh2, UINT32 val);
void FASTCALL SH2_Set_GBR(SH2_CONTEXT *sh2, UINT32 val);
void FASTCALL SH2_Set_VBR(SH2_CONTEXT *sh2, UINT32 val);
void FASTCALL SH2_Set_PR(SH2_CONTEXT *sh2, UINT32 val);
void FASTCALL SH2_Set_MACH(SH2_CONTEXT *sh2, UINT32 val);
void FASTCALL SH2_Set_MACL(SH2_CONTEXT *sh2, UINT32 val);

  Use these functions to set SH-2 registers.

UINT8 SH2_Read_Byte(SH2_CONTEXT *SH2, UINT32 adr);
UINT16 SH2_Read_Word(SH2_CONTEXT *SH2, UINT32 adr);
UINT32 SH2_Read_Long(SH2_CONTEXT *SH2, UINT32 adr);

  Read data from the SH-2 memory.

void SH2_Write_Byte(SH2_CONTEXT *SH2, UINT32 adr, UINT8 data);
void SH2_Write_Word(SH2_CONTEXT *SH2, UINT32 adr, UINT16 data);
void SH2_Write_Long(SH2_CONTEXT *SH2, UINT32 adr, UINT32 data);

  Write data to the SH-2 memory.


* Usefull infos
***************

You can configure SSH2 depending your need, for that just
comment or un-comment some switch in the 'constantes definitions'
part of sh2a.asm file.


1) GCC switch :
---------------

By default SSH2 uses the MSVC fastcall convention :
If you want to compile SSH2 for GCC, you have to uncomment the
__GCC definition.

By default :

;%define __GCC

If you compile for GCC :

%define __GCC


2) Speed switch :
-----------------

You can configure the SH-2 emulator to run in 3 modes :
- optimist
- realist
- pessimist

* Optimist mode uses the fastest timings possible, means that no wait
state occurs so you will emulate a fast (too fast) SH2 CPU but of
course it will take more CPU time for emulation.

* Realist mode add few wait states somewhere, it appears to
be less or more realist depending the system emulated.
Actually almost time it's still faster than the real thing but i think
it can be a bit slower in some case (intensive cache use).

* Realist mode add many wait states for read/write memory stuff,
it can be ok when SH-2 is operating with slow memory but almost case
the SH-2 will appears a bit slow compared to the real thing.
Use this mode permit to speed up emulation.

By default :

;%define SH2_SPEED       OPTIMIST
%define SH2_SPEED       REALIST
;%define SH2_SPEED       PESSIMIST

If you want to use 'optimist' timings :

%define SH2_SPEED       OPTIMIST
;%define SH2_SPEED       REALIST
;%define SH2_SPEED       PESSIMIST


3) FRT switch :
---------------

The Free Run Timer take some extra time for emulation, so you can
enable/disable FRT emulation.
By default, FRT is enable :

%define FRT_SUPPORT

If you don't want FRT emulation :

;%define FRT_SUPPORT


* H-Ints
********

* Emulation is faster when you use longer execution loop :

SH2_Exec(&M_SH2, 100000);

* WDT and FRT are more accurate when you use shorter execution call :

SH2_Exec(&M_SH2, 1000);

Just choose correctly depending your need :)


* History
*********

Version 1.60 :
--------------

* 10% speed increasement.
* CMP/STR instruction fixed.
* RESET instruction fixed.
* Some fix in the interface.
* NMI added.
* FRT added.
* Master/Slave CPU setting added.
* Some interface improvement.
* Many others minors add/tweaks/fixes...

Version 1.00 :
--------------

* Initial release !
