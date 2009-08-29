#include <muParser.h>
#include <stdio.h>
#include "sdl/compat.h"
#include "sdl/dialogs/messagebox.h"
#include "parser.h"
#include "starhelp.h"
#include "console.h"
#include "raine.h" // ReadWord !
#include "neocd/neocd.h" // ReadLongSc

/* muParser is specialised in double numbers, so it lacks some basic integer
 * operations, but can be easily extended, so let's go... */

using namespace mu;
value_type rol(value_type v1, value_type v2) { return int(rint(v1))<<int(rint(v2)); }
value_type ror(value_type v1, value_type v2) { return int(rint(v1))>>int(rint(v2)); }
value_type Or(value_type v1, value_type v2) { return int(v1) | int(v2); }
value_type And(value_type v1, value_type v2) { return int(v1) & int(v2); }
value_type Not(value_type v1) { return ~int(rint(v1)); }
value_type LogOr(value_type v1, value_type v2) { return int(v1) || int(v2); }
value_type LogAnd(value_type v1, value_type v2) { return int(v1) && int(v2); }
value_type LogNot(value_type v1) { return !int(rint(v1)); }

static double sr, pc, a[8], d[8];

value_type peek(value_type fadr) {
  UINT32 adr = fadr;
  UINT8 *ptr = get_ptr(adr);
  if (!ptr) throw "this adr isn't in ram !";

  return ptr[adr ^ 1];
}

value_type dpeek(value_type fadr) {
  UINT32 adr = fadr;
  UINT8 *ptr = get_ptr(adr);
  if (!ptr) throw "this adr isn't in ram !";

  return ReadWord(&ptr[adr]);
}

value_type lpeek(value_type fadr) {
  UINT32 adr = fadr;
  UINT8 *ptr = get_ptr(adr);
  if (!ptr) throw "this adr isn't in ram !";

  return ReadLongSc(&ptr[adr]);
}

void get_regs(int cpu) {
  // for now cpu is just ignored, we'll see if I decide to handle other cpus...
  for (int n=0; n<8; n++) {
    a[n] = s68000context.areg[n];
    d[n] = s68000context.dreg[n];
  }
  sr = s68000context.sr;
  pc = s68000context.pc;
}

void set_regs(int cpu) {
  // for now cpu is just ignored, we'll see if I decide to handle other cpus...
  for (int n=0; n<8; n++) {
    M68000_context[0].areg[n] = s68000context.areg[n] = a[n];
    M68000_context[0].dreg[n] = s68000context.dreg[n] = d[n];
  }
  M68000_context[0].sr = s68000context.sr = sr;
  M68000_context[0].pc = s68000context.pc = pc;
}

static value_type alert(const char_type *msg_and_btns) {
  // muparser doesn't support more than 1 string for its arguments...
  // so we'll pass the message + the buttons in the same string, separated by
  // the char |
  string_type mystr = msg_and_btns;
  char *cmsg = (char*)mystr.c_str();
  char *btn = strchr(cmsg,'|');
  if (btn) {
    *btn++ = 0;
    return MessageBox("alert",cmsg,btn);
  }
  // otherwise it's just a message
  return MessageBox("alert",cmsg);
}

static int initialised = 0;
static mu::Parser p;
int parser_error;
static double afValBuf[100];  
static int iVal = 0;          

double* AddVariable(const char *a_szName, void *pUserData)
{

  afValBuf[iVal++] = 0;
  if (iVal>=99)
    throw mu::Parser::exception_type("Variable buffer overflow.");

  return &afValBuf[iVal-1];
}

int parse(char *orig) 
{
//  using namespace mu;
  int res;
  char expr[1024];
  strcpy(expr,orig);
  parser_error = 0;

  /* Handling of bases is stupid in muparser, you can't make it to understand
   * hex numbers begining by 0x, so it's easier to convert everything before
   * calling it... */
  strlwr(expr);
  char *s = expr;
  while ((s = strstr(s,"0x"))) {
    s[1] = '$';
    strcpy(s,s+1);
  } // 0x -> $
  s = expr;
  while ((s = strchr(s,'$'))) {
    *s = 0;
    s++;
    int res = 0;
    while ((*s >= '0' && *s <= '9') || (*s >= 'a' && *s <= 'f')) {
      int digit;
      if (*s >= 'a') digit = *s - 'a' + 10;
      else
	digit = *s - '0';
      res = res*16 + digit;
      s++;
    }
    char buff2[1024];
    sprintf(buff2,"%s%d%s",expr,res,s);
    strcpy(expr,buff2); // obliged to use another string for the sprintf
  }

    if (!initialised) {
      p.DefineOprt( _T("<<"), rol, 0);
      p.DefineOprt( _T(">>"), ror, 0);
      p.DefineOprt( _T("|"), Or, 0);
      p.DefineOprt( _T("&"), And, 0);
      p.DefineInfixOprt( _T("~"), Not);
      p.DefineOprt( _T("||"), LogOr, 0);
      p.DefineOprt( _T("&&"), LogAnd, 0);
      p.DefineInfixOprt( _T("!"), LogNot);
      p.DefineFun( _T("alert"), alert, false);
      p.DefineFun( _T("dpeek"), dpeek);
      p.DefineFun( _T("peek"), peek, false);
      p.DefineFun( _T("lpeek"), lpeek, false);
      p.SetVarFactory(AddVariable,&p);
      // p.DefineFun( _T("r"), reg, false);
      for (int n=0; n<8; n++) {
	char name[3];
	sprintf(name,"d%d",n);
	p.DefineVar(name,&d[n]);
	sprintf(name,"a%d",n);
	p.DefineVar(name,&a[n]);
      }
      p.DefineVar("sr",&sr);
      p.DefineVar("pc",&pc);
      initialised = 1;
    }
//    p.DefineVar("a", &fVal); 
//    p.DefineFun("MyFunc", MyFunction); 
  try
  { 
    p.SetExpr(expr);
    res = p.Eval();

    // STd::cout << p.Eval() << endl;
  }
  catch (mu::Parser::exception_type &e)
  {
    parser_error = 1;
    const char *msg = e.GetMsg().c_str();
    // apparently vsprintf needs a char* which can be writen to, so we convert
#ifdef RAINE_DEBUG
    printf("console: %s\n",msg);
#endif
    throw msg;
  }
  return res;
}

