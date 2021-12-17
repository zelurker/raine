#include "raine.h"
#include "ingame.h"
#include "cpumain.h"
#include "savegame.h"

#define NB_ADR 32
static UINT32 adr[NB_ADR];
static UINT16 wordval[NB_ADR];
static UINT16 hackval[NB_ADR];
// Actually this speed_hack variable must be saved with the others (AddSave...) so it must be static to be in the same zone
// so we need 2 functions to access it from outside... !
static int speed_hack = 0; // speed hack detection (see execute_xxx_frame)
static int hack_length;

void set_speed_hack() {
    // Used by drivers which use this as part of their test
    speed_hack = 1;
}

int get_speed_hack() {
    return speed_hack;
}

// No cpu handling yet, it's always CPU_68K_0
static void myWriteWord(UINT32 adr, UINT16 val) {
    UINT8 *ptr = get_userdata(CPU_68K_0,adr);
    UINT32 start,end;
    UINT8 *ptr2 = get_code_range(CPU_68K_0,adr,&start,&end);
    // get_code_range is used mainly for cps2 encrypted roms, afaik it works always by using this pointer instead of the 1st one !
    if (ptr2)
	ptr = ptr2;
    if (!ptr)
	fatal_error("speed hack: can't get ptr for %x !",adr);
    WriteWord(&ptr[adr],val);
}

void undo_hack() {
  // Undo the speed hack when a game is reseted, or the rom check will fail
  // Sigh ! This is to avoid to have to find 80 rom check functions or so.
  if (speed_hack) {
    int n;
    for (n=0; n<hack_length; n++)
      myWriteWord(adr[n],wordval[n]);
    hack_length = 0;
    speed_hack = 0; // Not found yet...
  }
}

void pWriteWord(UINT32 myadr, UINT16 val) {
    adr[hack_length] = myadr;
    UINT8 *ptr = get_userdata(CPU_68K_0,myadr);
    UINT32 start,end;
    UINT8 *ptr2 = get_code_range(CPU_68K_0,myadr,&start,&end);
    if (ptr2)
	ptr = ptr2;
    if (!ptr)
	fatal_error("speed hack: can't get ptr for %x !",myadr);
    wordval[hack_length] = ReadWord(&ptr[myadr]);
    hackval[hack_length] = val;
    hack_length++;
    if (hack_length == NB_ADR) {
	printf("speed hack overloads NB_ADR\n");
	hack_length--;
    }
    WriteWord(&ptr[myadr],val);
}

static void restore_hack() {
    if (speed_hack) {
	// This is for some extreme case where the speed hack is removed after being installed (cps2 rasters !)
	// in this case when loading a savegame where it was installed, it needs to be reinstalled on load, that's what this function does
	for (int n=0; n<hack_length; n++) {
	    myWriteWord(adr[n],hackval[n]);
	}
	printf("restoration speed hack length %d\n",hack_length);
    }
}

void apply_hack(UINT32 pc, int kind) {
  print_ingame(180,gettext("Applied speed hack %d at %x"),kind,pc);
  print_debug("Applied speed hack %d at %x\n",kind,pc);
  speed_hack = 1;

  pWriteWord(pc,0x4239);
  pWriteWord(pc+2,0xaa);
  pWriteWord(pc+4,0);
  if (kind == 1) {
    pWriteWord(pc+6,0x4e71);
  } else if (kind == 2) {
    // qsound (punisher)
    pWriteWord(pc+6,0x6000);
  } else if (kind == 3) {
    // qsound (slammast)
    pWriteWord(pc+6,0x6008);
  } else if (kind == 4) {
      // tetrist, syvalion : just a loop
    pWriteWord(pc+8,0x6100-8);
  } else if (kind == 10) {
    // cps2 beq long
    pWriteWord(pc+6,0x4e71);
    pWriteWord(pc+8,0x4e71);
  } else if (kind == 11) {
    // cps2 most stupid (19xx)
    pWriteWord(pc+6,0x4e71);
    pWriteWord(pc+8,0x4e71);
    pWriteWord(pc+10,0x4e71);
  }
  AddSaveData_ext("speedhack",(UINT8*)adr,((UINT8*)&hack_length)-((UINT8*)&adr[0])+sizeof(int));
  AddLoadCallback(&restore_hack);
}

UINT32 get_speed_hack_adr(int n) {
  return adr[n];
}

