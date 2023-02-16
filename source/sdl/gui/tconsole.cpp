#include <stdio.h>
#include <stdarg.h>
#include <muParser.h>
#include "raine.h"
#include "version.h"
#include "tfont.h"
#include "control.h"
#include "control_internal.h"
#include "tconsole.h"
#include "dialogs/messagebox.h"
#include "games.h"
#include "gui.h"
#include "SDL_gfx/SDL_gfxPrimitives.h"
#include "console/scripts.h"

void split_command(char *field, char **argv, int *argc, int max, int strip_quotes) {
    // strip_quotes is now a parameter because quotes are a handy way to pass strings
    // and if they are stripped then there is no way to make the difference between a string and a variable name if there is no space
    // the only place where stripping quotes is really useful is in init_scripts, so it uses this parameter for that.
  char *quote = strrchr(field,'"'); // Ensure that comments are not inside a string !!!
  if (quote)
      quote++;
  else
      quote = field;
  char *s = strchr(quote,';'); // strip comments starting by ; or #. # might be used some day for something else...
  if (s) *s = 0;
  s = strchr(quote,'#');
  if (s) *s = 0;
  s = field;
  char *copie = strdup(field);
  *argc = 0;
  for (int n=0; n<max; n++)
      argv[n] = NULL;
  while (*s && (*s == ' ' || *s==9)) // skip the leading spaces
    s++;
  if (*s) argv[(*argc)++] = s;
  while (*s) {
      if (*s == '#')
	  return; // skip comment
      while (*s != ' ' && *s)  {
	  if (*s == '"') {
	      s++;
	      while (*s != '"' && *s)
		  s++;
	  } else if (*s == 0x27 /* ' */) {
	      s++;
	      while (*s != 0x27 /* ' */ && *s)
		  s++;
	  }
	  s++;
      }
      if (*s == ' ' || *s==9) {
	  *s++ = 0;
	  while (*s == ' ' || *s==9)
	      s++;
	  if (*s) {
	      if (strip_quotes && *s == '"') {
		  s++;
		  char *e = strchr(s,'"');
		  if (e) {
		      *e = 0;
		      argv[(*argc)++] = s;
		      s = e+1;
		  } else {
		      // error case anyway
		      s--;
		      *s = '"';
		      argv[(*argc)++] = s;
		  }
	      } else
		  argv[(*argc)++] = s;
	      if (*argc == max) {
		  fatal_error("split_command: max arguments : %d for command %s !",max,copie);
	      }
	  }
      }
  }
  free(copie);
}

static int dummy_handler(int cause) { return 0; }
int key_console;

#define MAX_FIELD 8192

TConsole::TConsole(char *my_title, char *init_label, int maxlen, int maxlines, commands_t *mycmd,int is_visible) :
  TDialog(my_title,NULL)
{
  int dummy_int;
  commands = mycmd;
  nb_alloc = maxlines;
  nb_disp_items = 0;
  menu = (menu_item_t*)malloc(sizeof(menu_item_t) * (nb_alloc+1));
  memset(&menu[0],0,sizeof(menu_item_t) * (nb_alloc+1));
  nb_items = 0;
  font_name = "VeraMono.ttf";
  len_max = maxlen;
  field = new char[MAX_FIELD];
  *field = 0;
  // expansions (base conversion...)
  edit_menu.label = init_label; // not used, but must not be NULL to show this field
  edit_menu.menu_func = dummy_handler;// must contain something to gain focus
  edit_menu.value_int = &dummy_int; // must also be != NULL for the extension
  // to be detected...
  edit_menu.values_list_size = ITEM_EDIT;
  edit_menu.values_list_label[0] = field;
  edit_menu.values_list[0] = maxlen;
  edit_menu.values_list[1] = 1; // use history
  edit_menu.values_list[2] = work_area.w*3/4; // width
  edit_child = new TEdit(&edit_menu);
  visible = is_visible;
  interactive = NULL;
  // For some unknown reason, this simple line to get muparser version creates a stack smash when leaving this constructor
  // but only on 1 computer ! No obvious reason for that, the simplest workaround seems to use the gcc -fno-stack-protector option
  print(pretty_emu_name " " VERSION " console based on muParser v%s", mu::Parser().GetVersion().c_str());
}

TConsole::~TConsole() {
  for (int n=0; n<nb_items; n++) {
    free((void*)menu[n].label);
  }
  free(menu);
  delete edit_child;
  delete field;
}

/* void TConsole::compute_nb_items() {
  // We must init nb_disp_itmes / menu_disp here
  nb_disp_items = nb_items;
  menu_disp = menu;
} */

void TConsole::draw() {
  ((TRaineDesktop*)desktop)->end_preinit();
  TDialog::draw();
  visible = 1;
}

UINT32 ansilen(const char *s) {
  // strlen which skips ansi esc sequences
  int len = 0;
  while (*s) {
    if (*s == 27) {
      while (*s != 'm')
	s++;
      s++;
      continue;
    }
    len++;
    s++;
  }
  return len;
}

int ansi_pos(char *s,int pos) {
    // skip ansi sequences to reach pos in string s
    char *orig = s;
    do {
	while (*s == 27) {
	    while (*s != 'm')
		s++;
	    s++;
	}
	if (pos) {
	    s++;
	    pos--;
	}
    } while (pos);
    while (*s == 27) {
	while (*s != 'm')
	    s++;
	s++;
    }
    return s-orig;
}

char* ansistr(char *s,int n) {
  // convert a len on a string without esc sequences to a len with esc
  while (*s && n>0) {
    if (*s == 27) {
      while (*s != 'm')
	s++;
      continue;
    }
    n--;
    s++;
  }
  return s;
}

#define mystrcpy(a,b) memmove(a,b,strlen(b)+1)

void TConsole::print(const char *format, ...)
{
  if (!visible) {
    return;
  }
  char string[2048],*s;
  va_list ap;
  va_start(ap,format);
  vsprintf(string,format,ap);
  va_end(ap);

  // 1) split the string at \n if there are some...
  while ((s = strstr(string,"\n"))) {
    *s = 0;
    print(string);
    mystrcpy(string,&s[1]);
  }

  // 2 extend tabs
  while ((s = strchr(string,9))) {
    *s = 0;
    int len_tab = 8-ansilen(string)%8;
    *s = 9;
    if (len_tab > 1)
      memmove(s+len_tab-1,s,strlen(s)+1);
    int n;
    for (n=0; n<len_tab; n++)
      s[n] = ' ';
  }

  // limit the prints to len_max
  while (ansilen(string) > len_max) {
    char *pos = ansistr(string,len_max);
    char old = *pos;
    *pos = 0;
    char *s = strrchr(string,' ');
    if (s) {
      *s = 0;
      print(string);
      *pos = old;
      mystrcpy(string,s+1);
    } else {
      print(string);
      *pos = old;
      mystrcpy(string,pos);
    }
  }

  if (nb_items == nb_alloc) {
    free((void*)menu[0].label);
    memcpy(&menu[0],&menu[1],(nb_items-1)*sizeof(menu_item_t));
    /* The beauty of this thing is that the child array doesn't have to be
     * changed : its elements point to menu entries which don't move, it's the
     * contents of the menu entries which move, not the menu entries themselves
     */
    nb_items--;
  } else {
    nb_disp_items++;
    menu_disp = (int*)realloc(menu_disp,sizeof(int)*(nb_disp_items));
    menu_disp[nb_disp_items-1] = nb_disp_items-1;
    if (nb_items == 1 || child != last_child) { // 1st allocation after compute_nb_items
	int is_zero = (child == NULL);
      child = (TStatic **)realloc(child,sizeof(TStatic*)*nb_alloc);
      if (is_zero) {
	  memset(child,0,sizeof(TStatic*)*nb_alloc);
	  nb_items = 0;
      }
      last_child = child;
    }
  }
  // memcpy(&menu[nb_items],&menu[nb_items-1],sizeof(menu_item_t));
  menu[nb_items].label = strdup(string);
  if (child)
    child[nb_items] = new TStatic(&menu[nb_items]);
  nb_items++;

  if (lift) {
      // make a super fast update !
      lift->next_page();
  } else {
      // If there is no lift, then the window will probably grow which means fg_layer will have to be re-created
#if SDL == 2
      SDL_DestroyTexture(fg_layer);
      fg_layer = NULL;
#endif
      draw();
  }
  post_print();
}

void TConsole::post_print() {
  // this function is here only to be overloaded
}

int TConsole::get_fglayer_footer_height() {
  if (font)
    return edit_child->get_height(font);
  return 0;
}

int TConsole::get_fglayer_footer_len() {
  return edit_child->get_len();
}

static int myx, myy, myw, myh, myxoptions;
void TConsole::display_fglayer_footer(int x,int &y,int w, int xoptions) {
  myx = x; myy=y; myw =w; myh=edit_child->get_height(font);
  myxoptions=xoptions;
#if SDL==2
  edit_child->disp(NULL,font,x,y,w,myh,fg,bg,
#else
  edit_child->disp(fg_layer,font,x,y,w,myh,fg,bg,
#endif
    xoptions);
}

void TConsole::setup_fg_layer() {
  TDialog::setup_fg_layer();
  // keep the lift down when printing
  if (lift)
    top = nb_disp_items - rows;
}

void TConsole::fglayer_footer_update() {
  edit_child->update();
  SDL_Rect dst;
  dst.x = myx; dst.y = myy; dst.w = myw; dst.h = myh;
#if SDL==2
  int ret = SDL_SetRenderTarget(rend,fg_layer);
  if (ret < 0) {
      fatal_error("rendertarget error");
  }
  // boxColor(rend,dst.x,dst.y,dst.x+dst.w-1,dst.h+dst.y-1,0xffff0000 /* bgsdl */);
  setcolor(bgsdl);
  SDL_RenderFillRect(rend,&dst);
  edit_child->disp(NULL,font,myx,myy,myw,myh,fg,bg,
#else
  SDL_FillRect(fg_layer,&dst,bgsdl);
  edit_child->disp(fg_layer,font,myx,myy,myw,myh,fg,bg,
#endif
	  myxoptions);
#if SDL==1
  SDL_Rect to;
  to.x = dst.x + fgdst.x; to.y = dst.y+fgdst.y;
  SDL_BlitSurface(fg_layer,&dst,sdl_screen,&to);
  do_update(&to);
#else
  SDL_SetRenderTarget(rend,NULL);
  redraw(NULL);
#endif
}

int TConsole::fglayer_footer_handle_key(SDL_Event *event) {
  if (event->type == SDL_KEYDOWN) {
    if (event->key.keysym.sym == SDLK_TAB) {
      // completion
      commands_t *cmd = commands;
      while (cmd->name) {
	if (!strncasecmp(field,cmd->name,strlen(field))) {
	  sprintf(field,"%s ",cmd->name);
	  event->key.keysym.sym = SDLK_END;
	  break;
	}
	cmd++;
      }
      if (!cmd->name) { // not found
	if (!strncasecmp(field,"help",strlen(field))) { // help is not a standard command
	  strcpy(field,"help ");
	  event->key.keysym.sym = SDLK_END;
	}
      }
    } else if (event->key.keysym.scancode == key_console) {
      exit_menu = 1;
      return 0;
    }
  }

  return edit_child->handle_key(event);
}

void TConsole::fglayer_footer_call_handler() {
  exit_menu = handler(1);
}

void TConsole::unknown_command(int argc, char **argv) {
  print("command unknown");
}

void TConsole::do_help(int argc, char **argv) {
  if (argc > 2) {
    print("syntax: help [command]");
    return;
  }

  commands_t *cmd = commands;
  while (cmd->name) {
    if (argc == 1) { // general help
      if (cmd->help) {
	if (cmd->long_help) {
	  print("*%s",cmd->help);
	} else
	  print(" %s",cmd->help);
      }
    } else if (!strcasecmp(argv[1],cmd->name)) {
      print(cmd->help);
      if (cmd->long_help) {
	print(cmd->long_help);
      } else {
	print("no more help");
      }
      break;
    }
    cmd++;
  }
  if (argc == 1) {
    print(" exit : exit (or use the ESC key !!!)");
    print("\nCommands preceded by * have an extended help, type\nhelp command for the details");
  }
  *field = 0;
}

static char temp[MAX_FIELD+1];

int TConsole::parse_cmd(char *string) {
  int len = strlen(string);
  cmd = NULL;
  if (string[len-1] == '\\') {
      string[len-1] = 0;
      strncat(temp,string,MAX_FIELD);
      temp[MAX_FIELD-1] = 0;
      return 0;
  }
  if (*temp) {
      strncat(temp,string,MAX_FIELD);
      temp[MAX_FIELD-1] = 0;
      string = temp;
  }

  while (len && string[len-1] < 32)
    string[--len] = 0;
  if (field != string) {
    strncpy(field,string,MAX_FIELD);
    field[MAX_FIELD-1] = 0;
  }
  if (*field) {
      if (!strchr(field,'#') && visible)
	  print("\E[31m>\E[0m %s",field);
      if (!strcasecmp(field,"exit")) {
	  *field = 0;
	  *temp = 0;
	  return 1;
      }
      split_command(field,argv,&argc,50);
      if (argc < 1) { *field = 0; *temp = 0; return 0; } // empty line (or spaces)
      if (argv[0][0] == '#') {
	  *field = 0;
	  *temp = 0;
	  return 0; // comment
      }
      if (!strcasecmp(argv[0],"help") || !strcmp(argv[0],"?")) {
	  do_help(argc, argv);
	  *field = 0;
	  *temp = 0;
	  return 0;
      }
      cmd = commands;
      oldfield = *field;
      while (cmd->name) {
	  if (!strcmp(cmd->name,argv[0])) {
	      return 1;
	      break;
	  }
	  cmd++;
      }
  }
  return 0;
}

int TConsole::run_cmd(char *string) {
    if (string[0] == '#' || string[0] == ';')
	return 0; // comment
    if (interactive) {
	interactive(string);
	temp[0] = 0;
	return 0;
    }
    int ret = parse_cmd(string);
    try {
	if (ret)
	    if (cmd->handler) (*cmd->handler)(argc,argv);
	if (!cmd->name) {
	    unknown_command(argc,argv);
	}
    }
    /* Exception handling :
     * C++ obliges to use classes if you want to do something as simple as
     * passing more than 1 parameter to the catch (message + value).
     * Anyway, the easiest way to do it I found until now is to have 2 handlers
     * 1 which takes a ConsExcept class as argument, and which can handle
     * any printf message
     * and the other one which handles a single string argument for simple
     * error messages without arguments.
     * Notice that if you pass a temporary string in which you use sprintf for
     * example by throw, then the temporary string is never freed (or so I have
     * read), so it's better to use a class by reference instead of this. */
    catch(ConsExcept &e) {
	if (visible) print(e.what());
	else {
	    char msg[240];
	    int nb,line;
	    char *section;
	    if (get_running_script_info(&nb,&line,&section)) {
		// Line numbers are unprecise because lines with only a comment are jumped
		sprintf(msg,"script: %s\nsection: %s\nline: %d\n\n",
			get_script_title(nb),
			section,
			line+1);
		strncat(msg, e.what(),240-strlen(msg));
		stop_script(nb);
		strncat(msg,"\n(script stopped)",240-strlen(msg));
		MessageBox("script error",msg,"ok");
	    } else
		MessageBox("script error",e.what(),"ok");
	}
    }
    catch(const char *msg) {
	if (visible) print(msg);
	else {
	    MessageBox("script error",(char *)msg,"ok");
	}
    }
    oldfield = *field;
    *field = 0;
    *temp = 0;
    return 0;
}

int TConsole::handler(int cause) {
  return run_cmd(field);
}

void TConsole::execute()
{
    TDialog::execute();
    visible = 0;
}

void TConsole::get_parsed_info(int *myargc, char ***myargv,void (**mycmd)(int, char **) ) {
    // to be called after a call to run_cmd
    *field = oldfield;
    *myargc = argc;
    *myargv = argv;
    if (cmd)
	*mycmd = cmd->handler;
    else if (argc) {
	// When the console is interactive mode for commands such as if, the line is passed directly to interactive and the handler is not even searched
	// in this case we search it here, it can be useful later
	cmd = commands;
	while (cmd->name) {
	    if (!strcmp(cmd->name,argv[0])) {
		*mycmd = cmd->handler;
		return;
	    }
	    cmd++;
	}
	*mycmd = NULL;
    } else
	*mycmd = NULL;
    argc = 0;
}

void TConsole::finish_parsed_info() {
    // the console internally uses field to parse its commands so it's better to set it to 0 in the end otherwise the last processed command appears when you open the console !
    *field = 0;
}
