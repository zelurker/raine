#include "raine.h"
#include "console.h"
#include "parser.h"
#include "scripts.h"

static void exec_if(char *field);
static int level,inner;
/* I am not 100% certain this code is correct for unlimited imbrication of ifs
 * there are 2 variables, inner and level, which count respectively the number
 * of imbricated ifs in a true test and in a false test.
 * The variables have priorities, and if we are executing there can't be any
 * level of false ifs, so it should work in all the cases... */

static void restore_field(char *field, int argc) {
  while (argc > 1) {
    field[strlen(field)] = ' ';
    argc--;
  }
}

static void skip_if(char *field) {
    int nb,line; char *section;
    int argc; char **argv; void (*cmd)(int, char **);
    int alloc=0;
    if (get_running_script_info(&nb,&line,&section) && !strcmp(section,"run")) {
	if (is_script_parsing()) {
	    cons->parse_cmd(field);
	    cons->get_parsed_info(&argc,&argv,&cmd);
	} else {
	    get_script_parsed(nb,line,&argc,&argv,&cmd);
	    if (argc == 0 || argv == NULL) {
		// an if block which was not executed in the previous parsing
		cons->parse_cmd(field);
		cons->get_parsed_info(&argc,&argv,&cmd);
		script_set_parsed(nb,line,argc,argv,cmd);
	    }
	}
    } else {
	alloc = 1;
	argv = (char**)calloc(10,sizeof(char*));
	split_command(field,argv,&argc,10);
    }

  if (!strcmp(argv[0],"else")) {
    if (!level)
      cons->set_interactive(&exec_if);
  } else if (!strcmp(argv[0],"elsif") && !level) {
    if (argc != 2) throw "elsif takes exactly 1 argument !";
    int cond = parse(argv[1]);
    if (cond)
      cons->set_interactive(&exec_if);
  } else if (!strcmp(argv[0],"endif")) {
    if (level) level--;
    else {
      if (inner) {
	inner--;
	cons->set_interactive(&exec_if);
      } else
	cons->end_interactive();
    }
  } else if (!strcmp(argv[0],"if"))
    level++;
  if (alloc) {
      restore_field(field,argc);
      free(argv);
  } else
      cons->finish_parsed_info();
}

static void exec_if(char *field) {
    int nb,line; char *section;
    int argc; char **argv; void (*cmd)(int, char **);
    int alloc=0;
    if (get_running_script_info(&nb,&line,&section) && !strcmp(section,"run")) {
	if (is_script_parsing()) {
	    cons->parse_cmd(field);
	    cons->get_parsed_info(&argc,&argv,&cmd);
	} else {
	    get_script_parsed(nb,line,&argc,&argv,&cmd);
	    if (argc == 0 || argv == NULL) {
		// an if block which was not executed in the previous parsing
		cons->parse_cmd(field);
		cons->get_parsed_info(&argc,&argv,&cmd);
		script_set_parsed(nb,line,argc,argv,cmd);
	    }
	}
    } else {
	alloc = 1;
	argv = (char**)calloc(10,sizeof(char*));
	split_command(field,argv,&argc,10);
    }

  if (!strcmp(argv[0],"else") || !strcmp(argv[0],"elsif")) {
    cons->set_interactive(&skip_if);
  } else if (!strcmp(argv[0],"endif")) {
    if (inner) {
      /* Apparently the only consequence of having another if executing
       * is only the handling of endif... */
      inner--;
    } else
      cons->end_interactive();
  } else {
    // just execute the instruction, then come back here
    // 1st restore field...
    int other_if = 0;
    if (!strcmp(argv[0],"if")) other_if=1;
    cons->end_interactive();
    if (alloc)
	cons->run_cmd(field);
    else if (!cmd) {
	if (argc == 1) parse(argv[0]);
	else {
	    printf("exec_if: no cmd for line %s 1st command %s\n",field,argv[0]);
	    exit(1);
	}
    } else {
	(*cmd)(argc,argv);
    }

    if (!other_if)
      cons->set_interactive(&exec_if);
    else
      inner++;
  }
  if (alloc) {
      restore_field(field,argc);
      free(argv);
  } else
      cons->finish_parsed_info();
}

void do_if(int argc, char **argv) {
  if (argc != 2) throw "if takes exactly 1 argument !";
  int cond = parse(argv[1]);
  if (cond) {
    cons->set_interactive(&exec_if);
  } else {
    cons->set_interactive(&skip_if);
  }
}

