#include <unistd.h>
#include "raine.h"
#include "console.h"
#include "parser.h"
#include "cpumain.h"
#include "exec.h"
#include "files.h"
#include "newmem.h"
#include "starhelp.h"
#include "gui.h"
#include "ingame.h"
#include "mz80help.h"
#include "emumain.h"
#include "savegame.h"
#include "dasm.h"

#define MAX_BREAK 10
typedef struct {
  UINT32 adr;
  UINT16 old;
  char *cond;
  int cpu;
} tbreak;

static int used_break,old_f3_init;
tbreak breakp[MAX_BREAK];
#if USE_MUSASHI < 2
static void (*resethandler)();
#endif

static void exec_break() {
  int n;
  for (n=0; n<used_break; n++) {
    if (breakp[n].adr == s68000_read_pc-2) {
      goto_debuger = n+1;
      Stop68000(0,0);
      /* Sadly, there is no way to correct the pc from here in starscream, so
       * we must update it later (in check_breakpoint), which is much more
       * awkward */
      /* And no way to test registers directly from here, so cond is evaluated
       * later */
    }
  }
#if USE_MUSASHI < 2
  if (resethandler && resethandler != quiet_reset_handler && resethandler != reset_game_hardware) {
    printf("calling prev reset handler\n");
    (*resethandler)();
  }
#endif
}

static void z80_port_break(UINT16 port, UINT16 data) {
  int n;
  for (n=0; n<used_break; n++) {
    if (breakp[n].adr == ((UINT32)z80pc-2)) {
      goto_debuger = n+1;
      StopZ80(0,0);
    } else
	printf("z80break: %x != %x\n",breakp[n].adr,z80pc);
  }
}

void (*old_f3)(UINT8 data);

void my_illg(UINT8 data)
{
    exec_break();
    if (data == 3)
	s68000_pc -= 2;
    if (old_f3)
	(*old_f3)(data);
}

void do_print_ingame(int argc, char **argv) {
    if (argc == 1) {
	cons->print("syntax : print_ingame nb_frame msg args...");
	return;
    }
    int nbf = parse(argv[1]);
    int nb = 0;
    int arg[3];
    char *s = argv[2]-1;
    while (nb < 3 && (s=strchr(s+1,'%'))) {
	if (s[1] == '%') { s++; continue; }
	arg[nb] = parse(argv[3+nb]);
	nb++;
    }
    if (argv[2][0] == '"') memmove(&argv[2][0],&argv[2][1],strlen(&argv[2][1]));
    if (argv[2][strlen(argv[2])-1] == '"') argv[2][strlen(argv[2])-1] = 0;
    if (nb == 0) print_ingame(nbf,argv[2]);
    else if (nb == 1) print_ingame(nbf,argv[2],arg[0]);
    else if (nb == 2) print_ingame(nbf,argv[2],arg[0],arg[1]);
    else if (nb == 3) print_ingame(nbf,argv[2],arg[0],arg[1],arg[2]);
}

void do_break(int argc, char **argv) {
    if ((old_f3 != F3SystemEEPROMAccess || !old_f3) && !old_f3_init) {
	old_f3_init = 1;
	old_f3 = F3SystemEEPROMAccess;
	F3SystemEEPROMAccess=&my_illg;
    }
    int cpu_id = get_cpu_id();
#ifdef USE_MUSASHI
    if (cpu_id >> 4 != 1 && cpu_id >> 4 != 2 && cpu_id >> 4 != 3)
	throw "breakpoints only for 68000 & 68020 & z80 for now";
#else
    if (cpu_id >> 4 != 1 && cpu_id >> 4 != 2)
	throw "breakpoints only for 68000 & z80 for now (use musashi for the 68020)";
#endif
  if (argc == 1) {
    if (used_break == 0) {
      if (cons) cons->print("no breakpoint");
    } else if (cons) {
      int n;
      for (n=0; n<used_break; n++) {
	cons->print("break #%d at %x cond:%s cpu:%s",n,breakp[n].adr,(breakp[n].cond ? breakp[n].cond : "none"),get_cpu_name_from_cpu_id(breakp[n].cpu));
      }
    }
  } else if (argc == 3) {
    if (strcmp(argv[1],"del"))
      throw "break del <number> or break adr";
    int nb = parse(argv[2]);
    if (nb >= used_break) {
      if (cons) cons->print("no breakpoint #%d",nb);
      return;
    }
    int adr = breakp[nb].adr;
    UINT8 *ptr = get_userdata(breakp[nb].cpu,adr);
    WriteWord(&ptr[adr],breakp[nb].old); // restore
    if (breakp[nb].cond)
      free(breakp[nb].cond);
    if (nb < used_break-1)
      memmove(&breakp[nb],&breakp[nb+1],(used_break-1-nb)*sizeof(tbreak));
    used_break--;
    if (cons) cons->print("breakpoint #%d deleted",nb);
    return;
  } else {
      if (used_break == MAX_BREAK) {
	  cons->print("maximum nb of breakpoints reached !");
	  return;
      }
    int adr = parse(argv[1]),n;
    for (n=0; n<used_break; n++) {
      if (abs(adr-(int)breakp[n].adr)<2) {
	if (cons) cons->print("already have a breakpoint at %x",breakp[n].adr);
	return;
      }
    }
// #ifdef MAME_Z80
//     UINT8 *ptr = get_userdata(cpu_id,adr);
// #else
    // The hard rule of mz80 : rombase takes priority on the memory map for code execution, so that's rombase that we want here
    // actually it's done for mame_Z80 too for now, which might get incompatible if passing a memory map in conflict with this...
    UINT8 *ptr = (cpu_id >> 4 == CPU_Z80 ? Z80_context[cpu_id & 0xf].z80Base : get_userdata(cpu_id,adr));
// #endif
    if (!ptr) {
      if (cons) cons->print("no data known for this adr");
      return;
    }
    if (used_break == MAX_BREAK) {
      if (cons) cons->print("maximum number of breakpoints reached");
      return;
    }
    breakp[used_break].old = ReadWord(&ptr[adr]);
    breakp[used_break].cpu = cpu_id;
    breakp[used_break].cond = NULL;
#ifdef USE_MUSASHI
    if (cpu_id >> 4 == 1 || cpu_id >> 4 == 3)
	WriteWord(&ptr[adr],0x7f03); // illegal instruction : raine #3
#else
    if (cpu_id >> 4 == 1)
	WriteWord(&ptr[adr],0x4e70); // reset
#endif
    if (cpu_id >> 4 == 2) { // z80
	insert_z80_port_wb(cpu_id & 0xf,0xab,0xab,(void*)&z80_port_break);
	WriteWord68k(&ptr[adr],0xd3ab);
    }
    breakp[used_break++].adr = adr;
#if USE_MUSASHI < 2
    if (s68000context.resethandler != &exec_break) {
      resethandler = s68000context.resethandler;
      M68000_context[0].resethandler = s68000context.resethandler = &exec_break;
    }
#endif
    if (cons) cons->print("breakpoint #%d inserted at %x",used_break-1,adr);
  }
}

// check if we are current in an irq and the return adress is just after
// adr. In this case execute the irq and adjust the return address to be
// adr (for watch points and breakpoints). Return the irq number or 0
int check_irq(UINT32 adr) {
    int irq = 0;
    int cpu_id = get_cpu_id();
    if (cpu_id >> 4 != 1 && cpu_id != 3)
	// 68000 & 68020 only
	return 0;
    if (s68000_sr >= 0x2100) {
	UINT8 *ptr = get_userdata(cpu_id,s68000_areg[7]);
	if (!ptr) {
	    do_irq(0,NULL);
	    printf("didn't get memory pointer for a7, you are on your own...");
	    return 0;
	}
	UINT32 ret = ((UINT32)ReadLongSc(&ptr[s68000_areg[7]+2]));
	if (ret == adr+2 || ret == adr) {
	    WriteLongSc(&ptr[s68000_areg[7]+2],adr);
	    irq = (s68000_sr & 0x700) >> 8;
	    get_regs();
	    do_irq(0,NULL); // get out of the irq...
	} else
	    printf("irq detected but address does not match : %x, passed %x\n",
		    ReadLongSc(&ptr[s68000_areg[7]+2]),adr);
    }
    return irq;
}

// return the irq were were in or 0 ir no irq
int check_breakpoint() {
    int irq = 0;
    if (goto_debuger > 0 && goto_debuger <= MAX_BREAK) {
	int n = goto_debuger-1;
	goto_debuger = 0;
	if (n >= used_break) return irq;
	int cpu_id = breakp[n].cpu;
	set_cpu_id(cpu_id);
	switch_cpu(cpu_id);
	get_regs();
	UINT8 *ptr;
	if ((cpu_id >> 4) != CPU_Z80)
	    ptr = get_userdata(cpu_id,breakp[n].adr);
	else
	    ptr = Z80_context[cpu_id & 0xf].z80Base;
	// restore original code here, without checking the pc
	WriteWord(&ptr[breakp[n].adr],breakp[n].old);
#ifdef MAME_Z80
	if ((cpu_id >> 4) == CPU_Z80) {
	    // Since the memory map can be different, modify this too
	    UINT8 *ptr2 = get_userdata(cpu_id,breakp[n].adr);
	    WriteWord(&ptr2[breakp[n].adr],breakp[n].old);
	}
#endif
	if (pc != breakp[n].adr) {
	    pc = breakp[n].adr;
	    set_regs(cpu_id);
	}
	irq = check_irq(breakp[n].adr);
	if (breakp[n].cond) {
	    get_regs();
	    if (!parse(breakp[n].cond)) {
		goto_debuger = -1; // breakpoint failed, avoid console
		return irq;
	    }
	}
	cons->set_visible();
	goto_debuger = 0;
	cons->print("breakpoint #%d at %x",n,breakp[n].adr);
	disp_instruction();
    }
    return irq;
}

static int do_cycles(int cpu = get_cpu_id(), int can_be_stopped = 1) {
  set_regs();
  UINT32 cycles = 1, oldpc = cpu_get_pc(cpu);
  do {
    cpu_execute_cycles(cpu,cycles);
    cycles++;
  } while (cpu_get_pc(cpu) == oldpc && cycles < 1000);
  if (cycles >= 1000) cons->print("%d cycles executed !",cycles);
  get_regs();
  if (can_be_stopped) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) { // we must allow a way to stop
      if (event.type == SDL_KEYDOWN) {
	int input = event.key.keysym.sym; // | ((event.key.keysym.mod & 0x4fc0)<<16);
	if (!input) { // special encoding for scancodes (unknown keys)
	  input = event.key.keysym.scancode | 0x200;
	}
	if (input == SDLK_ESCAPE)
	  throw "Interrupted";
	int modifier = (event.key.keysym.mod & 0x4fc3);
	if ((modifier & KMOD_CTRL) && input == SDLK_c) // ctrl-c
	  throw "Interrupted";
      }
    }
  }
  return cycles-1;
}

void restore_breakpoints() {
  int n;
  for (n=0; n<used_break; n++) {
      int cpu_id = breakp[n].cpu;
      UINT8 *ptr = get_userdata(cpu_id,breakp[n].adr);
      // 1 : get the pc out of the breakpoint
      while (pc >= breakp[n].adr && pc < breakp[n].adr+2) {
	  do_cycles(get_cpu_id(),0);
      }
      // 2 : restore it
      UINT32 adr = breakp[n].adr;
#ifdef USE_MUSASHI
      if (cpu_id >> 4 == 1 || cpu_id >> 4 == 3)
	  WriteWord(&ptr[adr],0x7f03); // illegal instruction : raine #3
#else
      if (cpu_id >> 4 == 1)
	  WriteWord(&ptr[adr],0x4e70); // reset
#endif
      if (cpu_id >> 4 == 2) { // z80
	  WriteWord68k(&ptr[adr],0xd3ab);
	  ptr = Z80_context[cpu_id & 0xf].z80Base;
	  WriteWord68k(&ptr[adr],0xd3ab);
      }
  }
}

void do_cond(int argc, char **argv) {
  if (argc == 2) {
    int cond = parse(argv[1]);
    if (cond) {
      cons->print("the condition is already true !");
      return;
    }
    int total = 16000000/60;
    while (total > 0 && !cond) {
      total -= do_cycles();
      cond = parse(argv[1]);
    }
    if (cond) {
      cons->print("the test just became true.");
      do_regs(argc,argv);
    } else {
      cons->print("test still false after %d cycles",16000000/60);
    }
  } else if (argc == 3) {
    // conditional breakpoints
    int n = parse(argv[1]);
    if (n >= used_break) {
      cons->print("breakpoint %d isn't defined",n);
      return;
    }
    if (breakp[n].cond)
      free(breakp[n].cond);
    breakp[n].cond = strdup(argv[2]);
    cons->print("condition set for breakpoint %d",n);
  } else
    cons->print("syntax : cond [breakpoint number] condition");
}

void done_breakpoints() {
  int n;
  for (n=0; n<used_break; n++) {
    if (breakp[n].cond)
      free(breakp[n].cond);
  }
  old_f3_init = 0;
}

static char buff[256];
static char instruction[20];
static UINT32 cur_pc, next_pc;

static int get_instruction(UINT32 target = cpu_get_pc(get_cpu_id())) {
  int cpu_id = get_cpu_id();
  u32 start,end;
  char buf2[256];
  int ret;
  int type;
  UINT8 *ptr = get_code_range(cpu_id,target,&start,&end);
  if (!ptr) {
      sprintf(buff,"sh2: can't get pointer for instruction");
      return 0;
  }
  switch(cpu_id >> 4) {
  case CPU_6502:
      // 65c02 should be good in all cases, games with arbalest as parent really use one, otherwise matmania uses a real 6502 but it should be ok here.
      ret =  dasm_65c02(buff, target, &ptr[target], &ptr[target], 0);
      break;
  case CPU_Z80:
      ret = z80_dasm(buff, target, &ptr[target], &ptr[target + z80_offdata]); // last argument is to be able to disassemble encrypted roms with separated opcodes and data
      break;
  case CPU_68000:
  case CPU_68020:
#if USE_MUSASHI == 2
  switch (m68ki_cpu.cpu_type) {
  case CPU_TYPE_000: type=M68K_CPU_TYPE_68000; break;
  case CPU_TYPE_010: type=M68K_CPU_TYPE_68010; break;
  case CPU_TYPE_EC020: type=M68K_CPU_TYPE_68EC020; break;
  case CPU_TYPE_020: type=M68K_CPU_TYPE_68020; break;
  }
#else
  enum
  {
      M68K_CPU_TYPE_INVALID,
      M68K_CPU_TYPE_68000,
      M68K_CPU_TYPE_68010,
      M68K_CPU_TYPE_68EC020,
      M68K_CPU_TYPE_68020,
      M68K_CPU_TYPE_68030,	/* Supported by disassembler ONLY */
      M68K_CPU_TYPE_68040		/* Supported by disassembler ONLY */
  };
  if ((cpu_id >> 4) == CPU_68000)
      type = M68K_CPU_TYPE_68000;
  else
      type = M68K_CPU_TYPE_68020;
#endif
  ret = m68k_disassemble(buff, target, type);
     break;
  case CPU_SH2:
     ret = DasmSH2(buff, target, ReadWord68k(&ptr[target]));
      break;
  default:
      sprintf(buff,"cpu unknown %x",cpu_id);
      return 0;
  }
  strncpy(instruction,buff,20);
  char *space = strchr(instruction,' ');
  if (space)
      *space = 0;
  printf("dasm returns %x\n",ret);
  switch (cpu_id >> 4) {
  case CPU_Z80:
  case CPU_6502:
      switch(ret & 0xff) {
      case 1: snprintf(buf2,256,"%04x: %02x       %s",target,ptr[target],buff); break;
      case 2: snprintf(buf2,256,"%04x: %04x     %s",target,ReadWord68k(&ptr[target]),buff); break;
      case 3: snprintf(buf2,256,"%04x: %04x%02x   %s",target,ReadWord68k(&ptr[target]),ptr[target+2],buff); break;
      case 4: snprintf(buf2,256,"%04x: %04x%04x %s",target,ReadWord68k(&ptr[target]),ReadWord68k(&ptr[target+2]),buff); break;
      }
      break;
  case CPU_SH2:
      snprintf(buf2,256,"%08x: %04x %s",target,ReadWord68k(&ptr[target]),buff); // All instructions on 2 bytes !!!
      break;
  case CPU_68020:
      switch(ret & 0xff) {
      case 2: snprintf(buf2,256,"%08x: %04x             %s",target,ReadWord68k(&ptr[target]),buff); break;
      case 4: snprintf(buf2,256,"%08x: %04x%04x         %s",target,ReadWord68k(&ptr[target]),ReadWord68k(&ptr[target+2]),buff); break;
      case 6: snprintf(buf2,256,"%08x: %04x%04x%04x     %s",target,ReadWord68k(&ptr[target]),ReadWord68k(&ptr[target+2]),ReadWord68k(&ptr[target+4]),buff); break;
      case 8: snprintf(buf2,256,"%08x: %04x%04x%04x%04x %s",target,ReadWord68k(&ptr[target]),ReadWord68k(&ptr[target+2]),ReadWord68k(&ptr[target+4]),ReadWord68k(&ptr[target+6]),buff); break;
      default: snprintf(buf2,256,"%08x: %s (len %d)",target,buff,ret & 0xff);;
      }
      break;
  case CPU_68000:
      switch(ret & 0xff) {
      case 2: snprintf(buf2,256,"%08x: %04x             %s",target,ReadWord(&ptr[target]),buff); break;
      case 4: snprintf(buf2,256,"%08x: %04x%04x         %s",target,ReadWord(&ptr[target]),ReadWord(&ptr[target+2]),buff); break;
      case 6: snprintf(buf2,256,"%08x: %04x%04x%04x     %s",target,ReadWord(&ptr[target]),ReadWord(&ptr[target+2]),ReadWord(&ptr[target+4]),buff); break;
      case 8: snprintf(buf2,256,"%08x: %04x%04x%04x%04x %s",target,ReadWord(&ptr[target]),ReadWord(&ptr[target+2]),ReadWord(&ptr[target+4]),ReadWord(&ptr[target+6]),buff); break;
      default: snprintf(buf2,256,"%08x: %s (len %d)",target,buff,ret & 0xff);;
      }
      break;
  }
  strcpy(buff,buf2);
  cur_pc = target;
  next_pc = target + (ret & 0xff);
  return ret;
}

static UINT32 last_list_adr = 0xffffffff, last_list_pc;
void do_list(int argc, char **argv) {
    int cpu_id = get_cpu_id();
    static char buffadr[10]; // required static otherwise asan makes a nervous breakdown on parse !
    UINT32 pc0 = cpu_get_pc(cpu_id);

    if (argc == 1 && last_list_adr != 0xffffffff && last_list_pc == pc0) {
	// use last_list_adr to continue the list...
	argc = 2;
	sprintf(buffadr,"$%x",last_list_adr);
	argv[1] = buffadr;
    }
    if (argc == 2)
	pc0 = parse(argv[1]);
    for (int n=0; n<6; n++) {
	int ret = get_instruction(pc0);
	if (pc0 == cpu_get_pc(cpu_id))
	    cons->print("\E[32m%s\E[0m",buff);
	else
	    cons->print(buff);
	pc0 += (ret & 0xff);
    }
    last_list_adr = next_pc;
    last_list_pc = cpu_get_pc(cpu_id);
}

void disp_instruction() {
    if (!cons->is_visible()) return;
    get_instruction();
    cons->print(buff);
}

void do_step(int argc, char **argv) {
  if (argc > 1) throw("syntax : step");
  do_cycles();
  disp_instruction();
}

void do_next(int argc, char **argv) {
    int cpu = get_cpu_id();
  UINT32 pc1 = cpu_get_pc(cpu);
  if (cur_pc != pc1)
    get_instruction();
  do_cycles();
  switch(cpu>>4) {
  case CPU_68000:
  case CPU_68020:
      if (!strcasecmp(instruction,"JSR") || !strcasecmp(instruction,"BSR") ||
	      !strcasecmp(instruction,"TRAP") || !strcasecmp(instruction,"DBRA")) {
	  while (cpu_get_pc(cpu) != next_pc) {
	      do_cycles();
	  }
      }
      break;
  case CPU_Z80:
      if (!strcasecmp(instruction,"CALL") || strcasestr(buff,"jr   nz,")) { // heavily dependant on the disassembler output !
	  while (cpu_get_pc(cpu) != next_pc) {
	      do_cycles();
	  }
      }
      break;
  case CPU_6502:
      while (cpu_get_pc(cpu) != next_pc) {
	  do_cycles();
      }
      break;
  default:
      throw "this cpu is not supported";
  }
  disp_instruction();
}

void do_irq(int argc, char **argv) {
    int cpu_id = get_cpu_id()>>4;
  if (argc == 2) {
    int irq = parse(argv[1]);
    cpu_interrupt(cpu_id,irq);
    cpu_execute_cycles(cpu_id,1);
    get_regs(cpu_id);
    return;
  }
  if (cpu_id == 2) {
      if ((int(iff)&1))
	  throw "z80: not in an irq !";
      while (!(int (iff) & 1))
	  do_cycles();
  }
  if (cpu_id != 2) {
      if ((int(sr) & 0x2700) <= 0x2000)
	  throw "not in an irq !";
      int current = int(sr) & 0x2700;
      while ((int(sr) & 0x2700) == current && !goto_debuger)
	  do_cycles();
  }
  disp_instruction();
}

void do_until(int argc, char **argv) {
  if (argc < 2) {
    throw "syntax : until <expression>";
  }
  while (!parse(argv[1]) && !goto_debuger)
    do_cycles();
  disp_instruction();
}

void do_set_save_slot(int argc, char **argv) {
    if (argc != 2)
	throw "syntax : set_save_slot number, with 0 <= number <= 9";
    int nb = atoi(argv[1]);
    if (nb < 0 || nb > 9)
	throw "you must pass a number >= 0 and <= 9";
    SaveSlot = nb;
    cons->print("Save slot set to %d",nb);
}

void cons_save_state(int argc, char **argv) {
    if (argc != 1)
	throw "syntax : save_state";
    GameSave();
}

void cons_load_state(int argc, char **argv) {
    if (argc != 1)
	throw "syntax load_state";
    GameLoad();
}

