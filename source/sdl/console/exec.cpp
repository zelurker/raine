#include <unistd.h>
#include "raine.h"
#include "console.h"
#include "parser.h"
#include "cpumain.h"
#include "exec.h"
#include "files.h"
#include "starhelp.h"
#include "sdl/gui.h"
#include "neocd/neocd.h"

#define MAX_BREAK 10
typedef struct {
  UINT32 adr;
  UINT16 old;
  char *cond;
} tbreak;

static int used_break;
tbreak breakp[MAX_BREAK];
static void (*resethandler)();

static void exec_break() {
  int n;
  for (n=0; n<used_break; n++) {
    if (breakp[n].adr == s68000readPC()-2) {
      goto_debuger = n+1;
      Stop68000(0,0);
      /* Sadly, there is no way to correct the pc from here in starscream, so
       * we must update it later (in check_breakpoint), which is much more
       * awkward */
      /* And no way to test registers directly from here, so cond is evaluated
       * later */
    }
  }
  if (resethandler && resethandler != quiet_reset_handler) {
    printf("calling prev reset handler\n");
    (*resethandler)();
  }
}

void do_break(int argc, char **argv) {
  if (argc == 1) {
    if (used_break == 0) {
      cons->print("no breakpoint");
    } else {
      int n;
      for (n=0; n<used_break; n++) {
	cons->print("break #%d at %x",n,breakp[n].adr);
      }
    }
  } else if (argc == 3) {
    if (strcmp(argv[1],"del"))
      throw "break del <number> or break adr";
    int nb = parse(argv[2]);
    if (nb >= used_break) {
      cons->print("no breakpoint #%d",nb);
      return;
    }
    int adr = breakp[nb].adr;
    UINT8 *ptr = get_userdata(0,adr);
    WriteWord(&ptr[adr],breakp[nb].old); // restore
    if (breakp[nb].cond)
      free(breakp[nb].cond);
    if (nb < used_break-1)
      memmove(&breakp[nb],&breakp[nb+1],(used_break-1-nb)*sizeof(tbreak));
    used_break--;
    cons->print("breakpoint #%d deleted",nb);
    return;
  } else {
    int adr = parse(argv[1]),n;
    for (n=0; n<used_break; n++) {
      if (abs(adr-breakp[n].adr)<6) {
	cons->print("already have a breakpoint at %x",breakp[n].adr);
	return;
      }
    }
    UINT8 *ptr = get_userdata(0,adr);
    if (!ptr) {
      cons->print("no data known for this adr");
      return;
    }
    if (used_break == MAX_BREAK) {
      cons->print("maximum number of breakpoints reached");
      return;
    }
    breakp[used_break].old = ReadWord(&ptr[adr]);
    breakp[used_break].cond = NULL;
    WriteWord(&ptr[adr],0x4e70); // reset
    breakp[used_break++].adr = adr;
    if (s68000context.resethandler != &exec_break) {
      resethandler = s68000context.resethandler;
      M68000_context[0].resethandler = s68000context.resethandler = &exec_break;
    }
    cons->print("breakpoint #%d inserted at %x",used_break-1,adr);
  }
}

// check if we are current in an irq and the return adress is just after
// adr. In this case execute the irq and adjust the return address to be
// adr (for watch points and breakpoints). Return the irq number or 0
int check_irq(uint adr) {
    int irq = 0;
    if (s68000context.sr >= 0x2100) {
	UINT8 *ptr = get_userdata(0,s68000context.areg[7]);
	UINT32 ret = ((UINT32)ReadLongSc(&ptr[s68000context.areg[7]+2]));
	if (ret == adr+2 || ret == adr) {
	    WriteLongSc(&ptr[s68000context.areg[7]+2],adr);
	    irq = (s68000context.sr & 0x700) >> 8;
	    get_regs(0);
	    do_irq(0,NULL); // get out of the irq...
	} else
	    printf("irq detected but address does not match : %x, passed %x\n",
		    ReadLongSc(&ptr[s68000context.areg[7]+2]),adr);
    }
    return irq;
}

// return the irq were were in or 0 ir no irq
int check_breakpoint() {
    int irq = 0;
    if (goto_debuger > 0 && goto_debuger <= MAX_BREAK) {
	int n = goto_debuger-1;
	UINT8 *ptr = get_userdata(0,breakp[n].adr);
	WriteWord(&ptr[breakp[n].adr],breakp[n].old);
	irq = check_irq(breakp[n].adr);
	if (s68000context.pc == breakp[n].adr+2) {
	    s68000context.pc = breakp[n].adr;
	}
	if (breakp[n].cond) {
	    get_regs(0);
	    if (!parse(breakp[n].cond)) {
		goto_debuger = -1; // breakpoint failed, avoid console
		return irq;
	    }
	}
	cons->set_visible();
	goto_debuger = 0;
	cons->print("breakpoint #%d at %x",n,breakp[n].adr);
    }
    return irq;
}

static int do_cycles(int cpu, int can_be_stopped = 1) {
  set_regs(0);
  UINT32 cycles = 1, oldpc = cpu_get_pc(cpu);
  do {
    cpu_execute_cycles(cpu,cycles);
    cycles++;
  } while (cpu_get_pc(cpu) == oldpc);
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
  get_regs(cpu);
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
    UINT8 *ptr = get_userdata(0,breakp[n].adr);
    if (ReadWord(&ptr[breakp[n].adr]) == breakp[n].old) {
      printf("found breakpoint to restore : %d\n",n);
      // 1 : get the pc out of the breakpoint
      while (s68000context.pc >= breakp[n].adr && s68000context.pc < breakp[n].adr+2) {
	do_cycles(CPU_68K_0,0);
      }
      // 2 : restore it
      UINT32 adr = breakp[n].adr;
      WriteWord(&ptr[adr],0x4e70); // reset
      printf("breakpoint restored, pc = %x\n",s68000context.pc);
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
      total -= do_cycles(CPU_68K_0);
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
}

static char buff[256];
static char instruction[20],args[40];
static UINT32 pc, opcode,next_pc,next_opcode;

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
   char *header)
{
  char name[1024];
  strcpy(name,name2);
  name[strlen(name)-2] = 0; // remove extension .s
  char cmd[1024];
  // add the -all option because m68kdis sometimes tries to be too clever
  // and finds data where there are only instructions (maybe there is another
  // way to do it, but it's the fastest one).
  sprintf(cmd,"m68kdis -pc %d -o \"%s\" \"%s\"",start,name2,name);
  ByteSwap(&ptr[start],end-start);
  save_file(name,&ptr[start],end-start);
  ByteSwap(&ptr[start],end-start);
  printf("cmd: %s\n",cmd);
  if (system(cmd) < 0) 
    throw "can't execute m68kdis !";
  int found_dcw = 0;
  FILE *f = fopen(name2,"r");
  while (!feof(f) && !found_dcw) {
      char buff[1024];
      fgets(buff,1024,f);
      if (strstr(buff,"DC.W")) {
	  found_dcw = 1;
	  break;
      }
  }
  fclose(f);
  if (found_dcw) {
      sprintf(cmd,"m68kdis -all -pc %d -o \"%s\" \"%s\"",start,name2,name);
      system(cmd);
  }
  name[strlen(name)] = '.'; // extension back
  name[strlen(name)-1] = 't'; // extension back
  f = fopen(name,"w");
  if (!f) 
    throw "can't create asm temporary file";
  fprintf(f,"%s\n",header);
  FILE *g = fopen(name2,"r");
  if (!g) {
    fclose(f);
    throw "m68kdis didn't work as expected !";
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
}

static FILE *open_asm(UINT32 target) {
  char str[1024];
  char buf[256];
  sprintf(str,"%s/prg_%02x.s",get_shared("debug"),target/0x10000);
  UINT32 start,end;
  UINT8 *ptr = get_code_range(0,target,&start,&end);
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
  if (!used_offs[target/0x10000]) {
    // didn't open this range yet, find the crc...
    for (n=start; n<end; n+=4)
      crc += ReadLong(&ptr[n]);
    sprintf(checksum,"; crc %x",crc);
  }

  FILE *f = fopen(str,"rb");
  if (!f) {
    generate_asm(str,start,end,ptr,checksum);
    f = fopen(str,"rb");
    myfgets(buf,256,f);
  } else if (!used_offs[target/0x10000]) {
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

static void get_instruction(UINT32 target = s68000context.pc) {
  FILE *f = open_asm(target);
  int line;
  fseek(f,get_offs(target,&line),SEEK_SET);
  int offset;
  while (!feof(f)) {
    offset = ftell(f);
    myfgets(buff,256,f);
    if (buff[0] == ';')
      continue;
    sscanf(buff,"%x %x %s %s",&pc,&opcode,instruction,args);
    if (pc >= target) break;
    if (!(pc & 0xffff)) set_offs(pc,offset,line);
    line++;
  }
  set_offs(pc,offset,line);
  if (pc != target)
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
  if (argc == 1 && last_list_adr && last_list_pc == s68000context.pc) {
    argc = 2;
    sprintf(buffadr,"$%x",last_list_adr);
    argv[1] = buffadr;
  }
    
  if (argc == 1) {
    if (pc != s68000context.pc) {
      get_instruction();
      if (pc != s68000context.pc) {
	cons->print("pc=%x not found in asm file",s68000context.pc);
	return;
      }
    }
    offset = get_offs(s68000context.pc,&line);
    int prev_line;
    if (line > 3) {
      prev_line = line-3;
      int actual_line = prev_line;
      get_line_offset(&actual_line,&offset,s68000context.pc/0x10000);
      if (actual_line < prev_line) {
	FILE *f = open_asm(s68000context.pc);
	fseek(f,offset,SEEK_SET);
	myfgets(buff,256,f);
	while (actual_line < prev_line) {
	  offset = ftell(f);
	  actual_line++;
	  myfgets(buff,256,f);
	  sscanf(buff,"%x %x %s %s",&pc,&opcode,instruction,args);
	}
	set_offs(pc,offset,actual_line);
	fclose(f);
      }
    } else {
      prev_line = 1;
      offset = 0;
    }
  } else if (argc == 2) {
    int target = parse(argv[1]);
    get_instruction(target);
    offset = get_offs(pc,&line);
  }
  FILE *f = open_asm(pc);
  fseek(f,offset,SEEK_SET);
  int n;
  for (n=1; n<=10 && !feof(f); n++) {
    offset = ftell(f);
    myfgets(buff,256,f);
    line++;
    sscanf(buff,"%x %x %s %s",&pc,&opcode,instruction,args);
    if (pc == s68000context.pc)
      cons->print("\E[32m%s\E[0m",buff);
    else
      cons->print("%s",buff);
  }
  offset = ftell(f);
  myfgets(buff,256,f);
  line++;
  sscanf(buff,"%x %x %s %s",&pc,&opcode,instruction,args);
  set_offs(pc,offset,line);
  last_list_adr = pc;
  last_list_pc = s68000context.pc;
  fclose(f);
}

static void disp_instruction() {
  get_instruction();
  if (pc != s68000context.pc)
    cons->print("pc=%x not found in asm file",s68000context.pc);
  else 
    cons->print(buff);
}

void do_step(int argc, char **argv) {
  if (argc > 1) throw("syntax : step");
  do_cycles(CPU_68K_0);
  disp_instruction();
}

void do_next(int argc, char **argv) {
  if (pc != s68000context.pc)
    get_instruction();
  do_cycles(CPU_68K_0);
  if (!strcasecmp(instruction,"JSR") || !strcasecmp(instruction,"BSR") ||
      !strcasecmp(instruction,"TRAP")) {
    while (s68000context.pc != next_pc) {
      do_cycles(CPU_68K_0);
    }
  }
  disp_instruction();
}

void do_irq(int argc, char **argv) {
  if (argc == 2) {
    int irq = parse(argv[1]);
    cpu_interrupt(CPU_68K_0,irq);
    get_regs(0);
    return;
  } 
  if ((s68000context.sr & 0x2700) <= 0x2000)
    throw "not in an irq !";
  int current = s68000context.sr & 0x2700;
  while ((s68000context.sr & 0x2700) == current)
    do_cycles(CPU_68K_0);
  disp_instruction();
}

void do_until(int argc, char **argv) {
  if (argc < 2) {
    throw "syntax : until pc";
  } 
  UINT32 pc = parse(argv[1]);
  while (s68000context.pc != pc)
    do_cycles(CPU_68K_0);
  disp_instruction();
}

