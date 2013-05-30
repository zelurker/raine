#include "../gui/menu.h"
#include "../compat.h"
#include "control.h"
#include "sdl/control_internal.h"
#include "sdl/dialogs/messagebox.h"
#include "games.h" // current_game
#include "sdl/SDL_gfx/SDL_gfxPrimitives.h"
#include "ingame.h"

/* This is currently the bigest file in the dialogs directory.
 * But it's not because of long complicated code, it's just because it does
 * a lot of things, like translating inputs to text for keyboard, joystick
 * and mouse, mapping the hat to joystick events, setup the autofire, and
 * finally setup the analog inputs.
 * None of this is complicated, but together it produces a big file... ! */

static UINT32 inp_key,inp_joy,inp_mouse;

static char *get_joy_name(int code) {
  if (!code)
    return strdup("");
  else if ((code & 0xff)-1 >= SDL_NumJoysticks())
      return strdup("Not here");
  char name[80];
  // sprintf(name,"Joy %d ",code & 0xff);
  snprintf(name,30,"Joy %d (%s",code & 0xff,joy_name[(code & 0xff)-1]);
  name[30] = 0;
  if (strlen(name) == 29)
      strcat(name,"...");
  strcat(name,") ");

  int stick = (code >> 8) & 0xff;
  int btn = (code >> 16) & 0xff;
  if (!btn) {
    char *direction;
    if (stick & 2) {
      if (stick & 1) 
        direction = "DOWN";
      else
	direction = "UP";
    } else {
      if (stick & 1)
	direction = "RIGHT";
      else
	direction = "LEFT";
    }
    sprintf(&name[strlen(name)],"Stick %d %s",stick/4,direction);
    return strdup(name);
  }
  sprintf(&name[strlen(name)],"Btn %d",btn);
  return strdup(name);
}
  
static char *get_key_name(int key) {
  char keyname[80];
  keyname[0] = 0;
  int mod = key>>16;
  key &= 0xffff;
  if (mod & KMOD_LCTRL)
    strcat(keyname,"LCTRL ");
  if (mod & KMOD_RCTRL)
    strcat(keyname,"RCTRL ");
  if (mod & KMOD_LSHIFT)
    strcat(keyname,"LSHIFT ");
  if (mod & KMOD_RSHIFT)
    strcat(keyname,"RSHIFT ");
  if (mod & KMOD_LALT)
    strcat(keyname,"LALT ");
  if (mod & KMOD_RALT)
    strcat(keyname,"RALT ");
  strcat(keyname,SDL_GetKeyName((SDLKey)key));
  if (key & 0x200) { // special scancode encoding for unknown keys
    sprintf(&keyname[strlen(keyname)]," (%x)",key & 0x1ff);
  }
  return strdup(keyname);
}

static char *get_mouse_name(int btn) {
  if (!btn) return strdup("");
  char text[20];
  sprintf(text,"Mouse btn %d",btn);
  return strdup(text);
}

static TMenuMultiCol *controls;

class TInput : public TDialog {
  private:
    int must_handle_mouse,ignore_mod,must_handle_joy;
  public:
    TInput(char *my_title, menu_item_t *menu,int hmouse,int imod, int hjoy=1) : TDialog(my_title,menu)
    { must_handle_mouse = hmouse;
		  must_handle_joy = hjoy;
      ignore_mod = imod;
    }
    virtual void handle_key(SDL_Event *event);
    virtual void handle_mouse(SDL_Event *event);
    virtual void handle_joystick(SDL_Event *event);
};

void TInput::handle_key(SDL_Event *event) {
  switch (event->type) {
    case SDL_KEYDOWN:
      if (ignore_mod) {
	switch(event->key.keysym.sym) {
        case SDLK_LCTRL:
	case SDLK_RCTRL:
	case SDLK_LSHIFT:
	case SDLK_RSHIFT:
	case SDLK_LALT:
	case SDLK_RALT:
	  return;
	default:
	  break;
	}
      }
      if (event->key.keysym.sym) {
	inp_key = event->key.keysym.sym | ((event->key.keysym.mod & 0x4fc3) << 16);
      } else {
	inp_key = (event->key.keysym.scancode|0x200) | ((event->key.keysym.mod & 0x4fc3) << 16);
      }
      exit_menu = 1;
  }
}

void TInput::handle_joystick(SDL_Event *event) {
	if (!must_handle_joy)
		return;
  int which, axis, value,hat;
  switch (event->type) {
    case SDL_JOYAXISMOTION:
      which = event->jaxis.which+1;
      axis = event->jaxis.axis;
      value = event->jaxis.value;
      if (which >= MAX_JOY || axis >= MAX_AXIS) {
	return;
      }
      if (bad_axes[MAX_AXIS*(which-1)+axis]) {
	  printf("skipping bad axis\n");
	  return;
      }
      if (axis > SDL_JoystickNumAxes(joy[which-1]))
	  return;
      if (value < -20000) {
	inp_joy = JOY(which,AXIS_LEFT(axis),0,0);
	exit_menu = 1;
      } else if (value > 20000) {
	inp_joy = JOY(which,AXIS_RIGHT(axis),0,0);
	exit_menu = 1;
      }
      break;
    case SDL_JOYBUTTONUP:
      inp_joy = JOY(event->jbutton.which+1,0,event->jbutton.button+1,0);
      exit_menu = 1;
      break;
    case SDL_JOYHATMOTION:
      which = event->jhat.which;
      hat = event->jhat.hat;
      axis = get_axis_from_hat(which,hat);
      which++;
      switch(event->jhat.value) { // take only 1 of the 4 positions
	case SDL_HAT_LEFT:
	  inp_joy = JOY(which,AXIS_LEFT(axis),0,0);
	  exit_menu = 1;
	  break;
	case SDL_HAT_UP:
	  inp_joy = JOY(which,AXIS_LEFT(axis+1),0,0);
	  exit_menu = 1;
	  break;
	case SDL_HAT_RIGHT:
	  inp_joy = JOY(which,AXIS_RIGHT(axis),0,0);
	  exit_menu = 1;
	  break;
	case SDL_HAT_DOWN:
	  inp_joy = JOY(which,AXIS_RIGHT(axis+1),0,0);
	  exit_menu = 1;
	  break;
      }
      break;
  }
}

void TInput::handle_mouse(SDL_Event *event) {
  if (event->type == SDL_MOUSEBUTTONUP && must_handle_mouse) {
    inp_mouse = event->button.button;
    exit_menu = 1;
  }
}

static menu_item_t menu_input[] =
{
  { "Press a key, a button, or move a joystick..." },
  { "or escape to delete this control" },
  { NULL },
};

static menu_item_t kb_only[] =
{
  { "Press a key..." },
  { NULL },
};

static char **cols;
static int base_input;

static int do_input(int sel) {
  inp_key = inp_joy = inp_mouse = 0;
  TInput *input = new TInput("Input",menu_input,0,1);
  input->execute();
  delete input;
  if (inp_key) {
    if (inp_key == SDLK_ESCAPE) {
      int ret = MessageBox("Question","What do you want to do ?",
        "Delete the key|"
	"Delete the joystick ctrl|"
	"Really use ESC here");
      switch(ret) {
	case 1: def_input_emu[sel].scancode = 0; 
		free(cols[sel*2+0]);
		cols[sel*2+0] = strdup("no key");
		break;
	case 2: def_input_emu[sel].joycode = 0;
		free(cols[sel*2+1]);
		cols[sel*2+1] = strdup("");
		break;
	case 3:
		def_input_emu[sel].scancode = inp_key;
		free(cols[sel*2+0]);
		cols[sel*2+0] = get_key_name(inp_key);
		break;
      }
    } else {
      def_input_emu[sel].scancode = inp_key;
      free(cols[sel*2+0]);
      cols[sel*2+0] = get_key_name(inp_key);
    }
  } else {
    def_input_emu[sel].joycode = inp_joy;
    free(cols[sel*2+1]);
    cols[sel*2+1] = get_joy_name(def_input_emu[sel].joycode);
  }
  controls->setup_font(40);
  controls->draw();
  return 0;
}

static int do_kb_input(int sel) {
  // same thing as do_input, but keyboard only (for layers)
  inp_key = inp_joy = inp_mouse = 0;
  TInput *input = new TInput("Input",kb_only,0,1,0);
  input->execute();
  delete input;
  if (inp_key) {
    if (inp_key == SDLK_ESCAPE) {
      int ret = MessageBox("Question","What do you want to do ?",
        "Delete the key");
      switch(ret) {
	case 1: 
		layer_info_list[sel].keycode = 0;
		free(cols[sel]);
		cols[sel] = strdup("no key");
		break;
      }
    } else {
      layer_info_list[sel].keycode = inp_key;
      free(cols[sel]);
      cols[sel] = get_key_name(inp_key);
    }
  }
  controls->setup_font(40);
  controls->draw();
  return 0;
}

static int do_input_ingame(int sel) {
  inp_key = inp_joy = inp_mouse = 0;
  TInput *input = new TInput("Input",menu_input,1,0);
  input->execute();
  delete input;
  if (inp_key) {
    if (inp_key == SDLK_ESCAPE) {
      int ret = MessageBox("Question","What do you want to do ?",
        "Delete the key|"
	"Delete the joystick ctrl|"
	"Del the mouse button");
      switch(ret) {
	case 1: 
		InputList[sel+base_input].Key = 0;
		if (!use_custom_keys) 
		    def_input[InputList[sel+base_input].default_key & 0xFF].scancode = 0;
		free(cols[sel*3+0]);
		cols[sel*3+0] = strdup("no key");
		break;
	case 2: 
		InputList[sel+base_input].Joy = 0;
		if (!use_custom_keys) 
		    def_input[InputList[sel+base_input].default_key & 0xFF].joycode = 0;
		free(cols[sel*3+1]);
		cols[sel*3+1] = strdup("");
		break;
	case 3:
		InputList[sel+base_input].mousebtn = 0;
		if (!use_custom_keys) 
		    def_input[InputList[sel+base_input].default_key & 0xFF].mousebtn = 0;
		free(cols[sel*3+2]);
		cols[sel*3+2] = strdup("");
		break;
      }
    } else {
      InputList[sel+base_input].Key = inp_key;
      if (!use_custom_keys && !base_input) {
	def_input[InputList[sel].default_key & 0xff].scancode = inp_key;
	printf("defaults updated\n");
      } else {
	printf("defaults not updated use_custom %d base_input %d\n",use_custom_keys,base_input);
      }
      free(cols[sel*3+0]);
      cols[sel*3+0] = get_key_name(inp_key);
    }
  } else if (inp_joy) {
    InputList[sel+base_input].Joy = inp_joy;
    if (!use_custom_keys && !base_input)
      def_input[InputList[sel].default_key & 0xff].joycode = inp_joy;
    free(cols[sel*3+1]);
    cols[sel*3+1] = get_joy_name(inp_joy);
  } else if (inp_mouse) {
    InputList[sel+base_input].mousebtn = inp_mouse;
    if (!use_custom_keys && !base_input)
      def_input[InputList[sel].default_key & 0xff].mousebtn = inp_mouse;
    free(cols[sel*3+2]);
    cols[sel*3+2] = get_mouse_name(inp_mouse);
  }

  controls->setup_font(40);
  controls->draw();
  return 0;
}

static int do_emu_controls(int sel) {
  int nb = raine_get_emu_nb_ctrl();
  menu_item_t *menu = (menu_item_t*)malloc(sizeof(menu_item_t)*(nb+1));
  memset(menu,0,sizeof(menu_item_t)*(nb+1));
  cols = (char**)malloc(sizeof(char*)*nb*2);
  for (int n=0; n<nb; n++) {
    menu[n].label = def_input_emu[n].name;
    menu[n].menu_func = &do_input;
    cols[n*2+0] = get_key_name(def_input_emu[n].scancode);
    cols[n*2+1] = get_joy_name(def_input_emu[n].joycode);
  }
  controls = new TMenuMultiCol("Raine Controls",menu,2,cols);
  controls->execute();
  delete controls;
  for (int n=0; n<nb*2; n++) {
    free(cols[n]);
  }
  free(cols);
  free(menu);
  return 0;
}

static int do_layers_controls(int sel) {
  int nb = layer_info_count;
  menu_item_t *menu = (menu_item_t*)malloc(sizeof(menu_item_t)*(nb+1));
  memset(menu,0,sizeof(menu_item_t)*(nb+1));
  cols = (char**)malloc(sizeof(char*)*nb);
  for (int n=0; n<nb; n++) {
    menu[n].label = layer_info_list[n].name;
    menu[n].menu_func = &do_kb_input;
    cols[n] = get_key_name(layer_info_list[n].keycode);
  }
  // A multi column dialog of only 1 column... oh well it is to copy the way
  // the other inputs dialogs work, and it might be easier this way anyway...
  controls = new TMenuMultiCol("Raine Controls",menu,1,cols);
  controls->execute();
  delete controls;
  for (int n=0; n<nb; n++) {
    free(cols[n]);
  }
  free(cols);
  free(menu);
  return 0;
}

static int do_ingame_controls(int sel) {
  int nb = InputCount,mynb;
  menu_item_t *menu = (menu_item_t*)malloc(sizeof(menu_item_t)*(nb+1));
  memset(menu,0,sizeof(menu_item_t)*(nb+1));
  cols = (char**)malloc(sizeof(char*)*nb*3);
  mynb = 0;
  base_input = 0;
  for (int n=0; n<nb; n++) {
    if ((sel >= 0 && (InputList[n].link == 0 || InputList[n].link > n)) ||
        (sel < 0 && InputList[n].link > 0 && InputList[n].link < n)) {
      if (sel < 0 && !base_input)
	base_input = n;
      menu[mynb].label = InputList[n].InputName;
      menu[mynb].menu_func = &do_input_ingame;
      cols[mynb*3+0] = get_key_name(InputList[n].Key);
      cols[mynb*3+1] = get_joy_name(InputList[n].Joy);
      cols[mynb*3+2] = get_mouse_name(InputList[n].mousebtn);
      mynb++;
    }
  }
  controls = new TMenuMultiCol("Ingame Controls",menu,3,cols);
  controls->execute();
  delete controls;
  for (int n=0; n<mynb*3; n++) {
    free(cols[n]);
  }
  free(cols);
  free(menu);
  return 0;
}

class TControl : public TMenu {
  public:
    TControl(char *myname, menu_item_t *menu) : TMenu(myname,menu)
    {}
    int can_be_displayed(int n) {
      if (n == 1 && !current_game)
	return 0;
      if (n == 5) { // autofire controls
	int n,nb = InputCount,nb_autofire;
	nb_autofire = 0;
	for (n=0; n<nb; n++) {
	  if (InputList[n].auto_rate) {
	    nb_autofire++;
	    break;
	  }
	}
	return nb_autofire; // works only if some controls have autofire
      }
      if (n == 3) // layers
	return layer_info_count;
      return 1;
    }
};

static int revert_to_default(int);
static int switch_to_custom(int);
static TControl *ctrl;

static int setup_autofire(int sel) {
  int nb = InputCount,mynb,x,n;
  menu_item_t *menu = (menu_item_t*)malloc(sizeof(menu_item_t)*(nb+1));
  memset(menu,0,sizeof(menu_item_t)*(nb+1));
  mynb=0;
  for (n=0; n<nb; n++) {
    if ((strstr(InputList[n].InputName,"utton") ||
	  (InputList[n].default_key >= KB_DEF_P1_B1 &&
	   InputList[n].default_key <= KB_DEF_P1_B8) || 
	  (InputList[n].default_key >= KB_DEF_P2_B1 &&
	   InputList[n].default_key <= KB_DEF_P2_B8) || 
	  (InputList[n].default_key >= KB_DEF_P3_B1 &&
	   InputList[n].default_key <= KB_DEF_P3_B8) || 
	  (InputList[n].default_key >= KB_DEF_P4_B1 &&
	   InputList[n].default_key <= KB_DEF_P4_B8)) &&
	  InputList[n].link < n) {
      menu[mynb].label = InputList[n].InputName;
      menu[mynb].value_int = &InputList[n].auto_rate;
      menu[mynb].values_list_size = 6;
      for (x=0; x<6; x++)
	menu[mynb].values_list[x] = x;
      menu[mynb].values_list_label[0] = "Off";
      menu[mynb].values_list_label[1] = "30 fps";
      menu[mynb].values_list_label[2] = "15 fps";
      menu[mynb].values_list_label[3] = "10 fps";
      menu[mynb].values_list_label[4] = "7 fps";
      menu[mynb].values_list_label[5] = "6 fps";
      mynb++;
    }
  }
  if (mynb == 0) {
    MessageBox("Error","No buttons mapped by this game for autofire, sorry !");
    free(menu);
    return 0;
  }
  TMenu *controls = new TMenu("Autofire...",menu);
  controls->execute();
  delete controls;
  // Now we must check if autofire was disabled for a newly created control
  for (n=0; n<nb; n++) {
    if (InputList[n].link && InputList[n].link < n && !InputList[n].auto_rate){
      InputList[InputList[n].link].link = 0;
      if (n < InputCount-1) { // This isn't the last input, bad luck !
	for (x=0; x<n; x++) {
	  if (InputList[x].link > n)
	    InputList[x].link--;
	}
	memcpy(&InputList[n],&InputList[n+1],(InputCount-1-n)*sizeof(INPUT));
      }
      InputCount--;
    }
  }
  if (!use_custom_keys) {
    // check if auto fire was enabled, in this case switch to custom controls
    for (n=0; n<nb; n++) {
      if (InputList[n].auto_rate) {
	use_custom_keys = 1;
	break;
      }
    }
  }
  free(menu);
  return 0;
}

static int autofire_controls(int sel) {
  /* When this function is called, there are some controls with autofire
   * enabled. We'll just create new entries in InputList for them and allow
   * the user to choose new controls for these */
  int n;
  for (n=0; n<InputCount; n++) {
    if (InputList[n].auto_rate && InputList[n].link == 0) {
      if (InputCount >= MAX_INPUTS-1) {
	MessageBox("Error","Too many controls have been defined for this game, sorry !");
	return 0;
      }
      InputList[InputCount] = InputList[n];
      InputList[n].link = InputCount;
      InputList[InputCount].link = n;
      InputList[n].auto_rate = 0;
      InputList[InputCount].Key = 0;
      InputList[InputCount].Joy = 0;
      InputList[InputCount].mousebtn = 0;
      InputCount++;
    }
  }
  return do_ingame_controls(-1);
}

class TCalibrate : public TBitmap_menu {
  protected:
    int cx,cy; // coordinates of the stick (real time)
    int myy; // remember y start of bitmap
  public:
    TCalibrate(char *my_title, menu_item_t *mymenu) :
      TBitmap_menu(my_title,mymenu," ") { cx = cy = 0; }
    virtual void setup_font(unsigned int len_frame);
    virtual int compute_fglayer_height();
    virtual void display_fglayer_header(int &y);
    virtual void skip_fglayer_header(int &y);
    virtual void handle_joystick(SDL_Event *event);
};

void TCalibrate::setup_font(unsigned int len_frame) {
  TMenu::setup_font(len_frame);
  int h;
  h = (sdl_screen->h-width_max-2)/(nb_items + 4 + 6); // margin
  if (h < font->get_font_height() && h < font->get_font_width()) {
    delete font;
    font = new TFont_ttf(h);
    compute_width_from_font();
  }
}

int TCalibrate::compute_fglayer_height() {
  int h = TMenu::compute_fglayer_height();
  h+=width_max;
  return h;
}

#define analog2screen(x) ((w-20)/2+(x)*(w-20)/(2*32767))

void TCalibrate::display_fglayer_header(int &y) {
  int w = fg_layer->w;
  int by = y+(w-20)/2; // center
  int bx = w/2;
  myy = y;

  boxColor(fg_layer,10,by-10,w-20,by+10,bg_frame);
  boxColor(fg_layer,bx-10,y,bx+10,y+w-20,bg_frame);

  // Horizontal
  if (analog_minx || analog_maxx)
    boxColor(fg_layer,10+analog2screen(analog_minx),by-10,
     10+analog2screen(analog_maxx),by+10,mymakecol(255,0,0));
  if (cx)
    vlineColor(fg_layer,10+analog2screen(cx),by-10,by+10,mymakecol(255,255,255));

  // vertical
  if (analog_miny || analog_maxy)
    boxColor(fg_layer,bx-10,y+analog2screen(analog_miny),
      bx+10,y+analog2screen(analog_maxy),mymakecol(255,0,0));
  if (cy)
    hlineColor(fg_layer,bx-10,bx+10,y+analog2screen(cy),mymakecol(255,255,255));
  y += fg_layer->w-18;
}

void TCalibrate::skip_fglayer_header(int &y) {
  if (fg_layer)
    y += fg_layer->w-18;
}

void TCalibrate::handle_joystick(SDL_Event *event) {
  switch (event->type) {
  case SDL_JOYAXISMOTION:
    if (event->jaxis.which == analog_num) {
      if (event->jaxis.axis == 2*analog_stick) {
	cx = event->jaxis.value;
	if (cx < analog_minx)
	  analog_minx = cx;
	else if (cx > analog_maxx)
	  analog_maxx = cx;
      } else if (event->jaxis.axis == 2*analog_stick+1) { // vertical
        cy = event->jaxis.value;
	if (cy < analog_miny)
	  analog_miny = cy;
	else if (cy > analog_maxy)
	  analog_maxy = cy;
      } else
        break;
      int y = myy;
      display_fglayer_header(y);
      SDL_Rect dest,from;
      dest.x = fgdst.x+10;
      dest.y = fgdst.y+10;
      dest.w = fg_layer->w-20;
      dest.h = fg_layer->w-20;
      from.x = 10; from.y = 10;
      from.w = from.h = fg_layer->w-20;
      SDL_FillRect(sdl_screen,&dest,0);
      SDL_BlitSurface(fg_layer,&from,sdl_screen,&dest);
      do_update(&dest);
      // printf("redraw %d %d %d %d cx %d cy %d\n",analog_minx,analog_maxx,analog_miny,analog_maxy,cx,cy);
    }
    break;
  case SDL_JOYBUTTONUP:
    exit_menu = 1;
    break;
  }
}

static int selected;

static int select_joy(int sel) {
  selected = sel;
  return 1;
}

static int setup_analog(int sel) {
  int nb = SDL_NumJoysticks(),n;
  menu_item_t *menu = (menu_item_t*)malloc(sizeof(menu_item_t)*(nb+2));
  memset(menu,0,sizeof(menu_item_t)*(nb+1));
  selected = nb;
  for (n=0; n<nb; n++) {
    menu[n].label = strdup(joy_name[n]);
    menu[n].menu_func = &select_joy;
  }
  menu[n].label = "No analog device";
  menu[n].menu_func = &select_joy;
  menu[n+1].label = NULL;
  TMenu *dlg = new TMenu("Select analog device",menu);
  dlg->execute();
  delete dlg;
  if (analog_num != selected) {
    // reset calibration info if the device changes
    analog_minx = analog_maxx = analog_miny = analog_maxy = 0;
  }
  analog_num = selected;
  if (analog_num < nb)
    strcpy(analog_name,menu[analog_num].label);
  for (n=0; n<nb; n++)
    free((void*)menu[n].label);
  free(menu);

  if (analog_num >= nb) {
    analog_num = -1;
    analog_name[0] = 0;
    return 0;
  }
  nb = SDL_JoystickNumAxes(joy[analog_num])/2;
  if (nb == 1) {
    analog_stick = 0;
  } else {
    menu_item_t *menu = (menu_item_t*)malloc(sizeof(menu_item_t)*(nb+1));
    memset(menu,0,sizeof(menu_item_t)*(nb+1));
    for (n=0; n<nb; n++) {
      char buff[20];
      sprintf(buff,"Stick %d",n);
      menu[n].label = strdup(buff);
      menu[n].menu_func = &select_joy;
    }
    selected = 999;
    TMenu *dlg = new TMenu("Select the stick to use...",menu);
    dlg->execute();
    delete dlg;
    for (n=0; n<nb; n++) 
      free((void*)menu[n].label);
    free(menu);
    if (selected == 999) {
      // canceled !
      analog_num = -1;
      analog_name[0] = 0;
      return 0;
    } 
    analog_stick = selected*2;
  }
  nb = 4;
  menu = (menu_item_t*)malloc(sizeof(menu_item_t)*(nb+1));
  memset(menu,0,sizeof(menu_item_t)*(nb+1));
  menu[0].label = "Move the stick to all the extreme positions";
  menu[1].label = "If it's a steering wheel, then move it to the max";
  menu[2].label = "left and right positions, and press then release";
  menu[3].label = "each pedal separately";
  TCalibrate *cal = new TCalibrate("Calibration",menu);
  cal->execute();
  delete cal;
  free(menu);
  if (analog_minx >= 0 || analog_miny >= 0 || analog_maxx <= 0 || 
      analog_maxy <= 0) {
    // calibration not finished -> give up
    analog_num = -1;
    analog_name[0] = 0;
  }
  return 0;
}

static menu_item_t controls_menu[] =
{
  { "Raine controls", &do_emu_controls },
  { "", &do_ingame_controls },
  { "Revert to default game controls", &revert_to_default },
  { "Video layers", &do_layers_controls },
  { "Autofire...", &setup_autofire },
  { "Autofire controls", &autofire_controls },
  { "Analog controls...", &setup_analog },
  { NULL },
};

static int revert_to_default(int sel) {
  reset_game_keys();
  use_custom_keys = 0;
  controls_menu[1].label = "Edit default game inputs";
  controls_menu[2].label = "Switch to custom game controls";
  controls_menu[2].menu_func = &switch_to_custom;
  if (ctrl) ctrl->draw();
  return 0;
}

static int switch_to_custom(int sel) {
  use_custom_keys = 1;
  controls_menu[1].label = "Edit custom game inputs";
  controls_menu[2].label = "Revert to default game controls";
  controls_menu[2].menu_func = &revert_to_default;
  if (ctrl) ctrl->draw();
  return 0;
}

int do_controls(int sel) {
  if (use_custom_keys) {
    switch_to_custom(0);
  } else if (current_game) {
    revert_to_default(0);
  } else
    controls_menu[2].label = NULL;

  ctrl = new TControl("Controls", controls_menu);
  ctrl->execute();
  delete ctrl;
  ctrl = NULL;
  return 0;
}

