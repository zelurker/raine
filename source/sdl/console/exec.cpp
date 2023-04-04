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
    if (argv[2][0] == '"') argv[2]++;
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

typedef struct {
  int adr, offset, line;
} toffset;

#define MAX_OFFS 100
static toffset *offs[0x100];
static int used_offs[0x100];

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

  // Also frees the offsets when leaving the console...
  for (n=0; n<0x100; n++)
      if (used_offs[n]) {
	  used_offs[n] = 0;
	  free(offs[n]);
	  offs[n] = NULL;
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
static char instruction[20],args[40];
static UINT32 cur_pc, opcode,next_pc,next_opcode;

static int get_offs(int adr, int *line) {
  int n;
  int index = adr/0x10000;
  toffset *pofs = offs[index];
  for (n=0; n<used_offs[index]; n++) {
    if (pofs[n].adr >= adr) break;
  }
  if (n < used_offs[index] && pofs[n].adr == adr) {
    *line = pofs[n].line;
    return pofs[n].offset;
  }
  if (n > 0) {
    *line = pofs[n-1].line;
    return pofs[n-1].offset;
  }
  *line = 1;
  return 0;
}

static void get_line_offset(int *line, int *offset,int index) {
  int n;
  toffset *pofs = offs[index];
  for (n=0; n<used_offs[index]; n++) {
    if (pofs[n].line >= *line) break;
  }
  if (n < used_offs[index] && pofs[n].line == *line) {
    *offset = pofs[n].offset;
    return;
  }
  if (n > 0) {
    *line = pofs[n-1].line;
    *offset = pofs[n-1].offset;
    return;
  }
  *line = 1;
  *offset = 0;
}

static void set_offs(int adr, int offset, int line) {
  /* Store an offset and line number for a given adr */
  int n;
  int index = adr/0x10000;
  int used_of = used_offs[index];
  toffset *pofs = offs[index];
  for (n=0; n<used_of && offs[index][n].adr < adr; n++);
  if (n == used_of) {
    if (used_of == 0) {
      offs[index] = (toffset*)malloc(sizeof(toffset)*(MAX_OFFS+1));
    }
    if (used_of < MAX_OFFS) {
      used_offs[index]++;
    } else {
      memmove(&pofs[0],&pofs[1],sizeof(toffset)*(used_of-1));
      n--;
    }
  } else {
    if (offs[index][n].adr == adr) return; // already have it
    if (n < used_of)
      memmove(&pofs[n+1],&pofs[n],sizeof(toffset)*(used_of-n));
    if (used_of < MAX_OFFS) {
      used_offs[index]++;
    }
  }
  offs[index][n].adr = adr;
  offs[index][n].offset = offset;
  offs[index][n].line = line;
}

static void generate_asm(char *name2,UINT32 start, UINT32 end,UINT8 *ptr,
   char *header, int target=0)
{
  char name[1024];
  char dir[FILENAME_MAX];
  getcwd(dir,FILENAME_MAX);
  // Strip dir: dz80 is totally buggy, if you pass a long filename with dirs
  // it just can't find it !!!
  char *pdir = strrchr(name2,'/');
  strcpy(name,pdir+1);
  *pdir = 0;
  chdir(name2);
  *pdir = '/';
  sprintf(strrchr(name,'.')+1,"bin"); // new extension, required by dz80
  char cmd[1024];
  int cpu_id = get_cpu_id()>>4;
  int has_pc = 0;
  if ((pc >= start && pc < end) || target) {
      FILE *f = fopen("pc","w");
      if (f) {
	  has_pc = 1;
	  if (pc >= start && pc < end) fprintf(f,"%d\n",int(pc));
	  if (target) fprintf(f,"%d\n",target);
	  fclose(f);
      }
  }
  switch(cpu_id) {
  case CPU_68000:
      snprintf(cmd,1024,"%s ",dir_cfg.m68kdis);
      if (has_pc) strcat(cmd," -i pc ");
      snprintf(cmd+strlen(cmd),1024-strlen(cmd),"-pc %d -o \"%s\" \"%s\"",start,name2,name);
      ByteSwap(&ptr[start],end-start);
      break;
  case CPU_68020:
      snprintf(cmd,1024,"%s ",dir_cfg.m68kdis);
      if (has_pc) strcat(cmd," -i pc ");
      snprintf(cmd+strlen(cmd),1024-strlen(cmd)," -020 -pc %d -o \"%s\" \"%s\"",start,name2,name);
      break;
  case CPU_Z80:
      snprintf(cmd,1024,"%s \"%s\" prg.z80",dir_cfg.dz80,name);
      break;
  case CPU_6502:
      snprintf(cmd,1024,"%s \"%s\" > prg.6502",dir_cfg.d6502,name);
      break;
  }
  save_file(name,&ptr[start],end-start);
  if (cpu_id == 1)
	  ByteSwap(&ptr[start],end-start);
  if (used_offs[start/0x10000]) {
      used_offs[start/0x10000] = 0;
      free(offs[start/0x10000]);
      offs[start/0x10000] = NULL;
  }
  printf("cmd: %s\n",cmd);
  int ret;
  if ((ret = system(cmd)) < 0) {
      chdir(dir);
      throw "can't execute disassembler !";
  }
  if ((ret >> 8) == 127) { // 127 is command not found from shell...
      chdir(dir);
      throw "Can't find disassembler - see Options / Directories for m68kdis or dz80";
  }
  sprintf(strrchr(name,'.'),".t");
  FILE *f = fopen(name,"w");
  if (!f) {
      chdir(dir);
      throw "can't create asm temporary file";
  }
  fprintf(f,"%s\n",header);
  FILE *g = fopen(name2,"r");
  if (!g) {
    fclose(f);
    chdir(dir);
    throw "couldn't open assembler output !";
  }
  while (!feof(g)) {
    char buf[256];
    myfgets(buf,256,g);
    fprintf(f,"%s\n",buf);
  }
  fclose(f);
  fclose(g);
  unlink(name2);
  rename(name,name2);
  chdir(dir);
}

static void get_asm_file(char *str, UINT32 target = cpu_get_pc(get_cpu_id())) {
  int cpu_id = get_cpu_id();
  switch(cpu_id>>4) {
  case CPU_68000:
      sprintf(str,"%s/prg_%02x.s",get_shared("debug"),target/0x10000);
      break;
  case CPU_Z80:
      sprintf(str,"%s/prg.z80",get_shared("debug"));
      break;
  case CPU_6502:
      sprintf(str,"%s/prg.6502",get_shared("debug"));
      break;
  case CPU_68020:
      sprintf(str,"%s/prg020_%02x.s",get_shared("debug"),target/0x10000);
      break;
  }
}

static FILE *open_asm(UINT32 target) {
  char str[1024];
  char buf[256];
  int cpu_id = get_cpu_id();
  get_asm_file(str,target);
  UINT32 start,end;
  UINT8 *ptr = get_code_range(cpu_id,target,&start,&end);
  if (!ptr)
    throw "no code for this address";
  int min = target/0x10000*0x10000-0x100;
  if (min < 0) min = 0;
  UINT32 max = target/0x10000*0x10000+0x10000+0x100;
  if (max > 0xffffff) max = 0xffffff;
  if (start < (UINT32)min)
    start = min;
  if (end > max)
    end = max;
  int crc = 0;
  UINT32 n;
  char checksum[80];
  for (n=start; n<end; n+=4)
      crc += ReadLong(&ptr[n]);
  sprintf(checksum,"; crc %x",crc);

  FILE *f = fopen(str,"rb");
  if (!f) {
    generate_asm(str,start,end,ptr,checksum,target);
    f = fopen(str,"rb");
    myfgets(buf,256,f);
  } else {
    myfgets(buf,256,f);
    if (strcmp(buf,checksum)) {
      // didn't find the right crc at the top, refresh the file
      fclose(f);
      generate_asm(str,start,end,ptr,checksum);
      f = fopen(str,"rb");
      myfgets(buf,256,f);
    }
  }

  if (!f) {
    throw "could not generate asm code - no asm support";
  }
  return f;
}

static void get_instruction(UINT32 target = cpu_get_pc(get_cpu_id())) {
  FILE *f = open_asm(target);
  int line;
  fseek(f,get_offs(target,&line),SEEK_SET);
  int offset;
  while (!feof(f)) {
    offset = ftell(f);
    myfgets(buff,256,f);
    if (buff[0] == ';')
      continue;
    sscanf(buff,"%x %x %s %s",&cur_pc,&opcode,instruction,args);
    if ((target >> 4) == CPU_6502) {
	char *s = buff;
	while (*s == ' ' || (*s >= '0' && *s<='9') || (*s >= 'A' && *s <= 'F') || *s == '-' || *s == ' ')
	    s++;
	sscanf(s,"%s %s",instruction,args);
    }
    if (cur_pc >= target) break;
    if (!(cur_pc & 0xffff)) set_offs(cur_pc,offset,line);
    line++;
  }
  set_offs(cur_pc,offset,line);
  if (cur_pc != target)
    *buff = 0;
  char buff2[256];
  offset = ftell(f);
  myfgets(buff2,256,f);
  sscanf(buff2,"%x %x",&next_pc,&next_opcode);
  set_offs(next_pc,offset,++line);
  fclose(f);
}

void do_list(int argc, char **argv) {
  int offset, line;
  static UINT32 last_list_adr, last_list_pc;
  static char buffadr[10];
  int cpu_id = get_cpu_id();
  UINT32 pc0 = cpu_get_pc(cpu_id);
  if (argc == 1 && last_list_adr && last_list_pc == pc0) {
    argc = 2;
    sprintf(buffadr,"$%x",last_list_adr);
    argv[1] = buffadr;
  }

  if (argc == 1) {
    if (cur_pc != pc0) {
      get_instruction();
      if (cur_pc != pc0) {
	  char str[1024];
	  get_asm_file(str);
	  unlink(str);
	  get_instruction();
      }
      if (cur_pc != pc0) {
	cons->print("pc=%x not found in asm file",pc0);
	return;
      }
    }
    offset = get_offs(pc0,&line);
    int prev_line;
    if (line > 3) {
      prev_line = line-3;
      int actual_line = prev_line;
      get_line_offset(&actual_line,&offset,pc0/0x10000);
      if (actual_line < prev_line) {
	FILE *f = open_asm(pc0);
	fseek(f,offset,SEEK_SET);
	myfgets(buff,256,f);
	if (offset == 0)  myfgets(buff,256,f);
	while (actual_line < prev_line) {
	  offset = ftell(f);
	  actual_line++;
	  myfgets(buff,256,f);
	  sscanf(buff,"%x %x %s %s",&cur_pc,&opcode,instruction,args);
	}
	set_offs(cur_pc,offset,actual_line);
	fclose(f);
      }
    } else {
      prev_line = 1;
      offset = 0;
    }
  } else if (argc == 2) {
    int target = parse(argv[1]);
    get_instruction(target);
    offset = get_offs(cur_pc,&line);
  }
  FILE *f = open_asm(cur_pc);
  fseek(f,offset,SEEK_SET);
  if (offset == 0)  myfgets(buff,256,f);
  int n;
  for (n=1; n<=10 && !feof(f); n++) {
    offset = ftell(f);
    myfgets(buff,256,f);
    line++;
    sscanf(buff,"%x %x %s %s",&cur_pc,&opcode,instruction,args);
    if (cur_pc == pc0)
      cons->print("\E[32m%s\E[0m",buff);
    else
      cons->print("%s",buff);
  }
  offset = ftell(f);
  myfgets(buff,256,f);
  line++;
  sscanf(buff,"%x %x %s %s",&cur_pc,&opcode,instruction,args);
  set_offs(cur_pc,offset,line);
  last_list_adr = cur_pc;
  last_list_pc = pc0;
  fclose(f);
}

void disp_instruction() {
    if (!cons->is_visible()) return;
  get_instruction();
  UINT32 pc1 = cpu_get_pc(get_cpu_id());
  if (cur_pc != pc1) {
      char str[1024];
      get_asm_file(str);
      unlink(str);
      get_instruction();
  }
  if (cur_pc != pc1)
    cons->print("pc=%x not found in asm file",pc1);
  else
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
      if (!strcasecmp(instruction,"CALL")) {
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
      while (!((int) iff) & 1)
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

