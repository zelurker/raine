/******************************************************************************/ /*                                                                            */
/*                 CONTROL SUPPORT [KEYBOARD/JOYSTICK/LEDS]                   */
/*                                                                            */
/******************************************************************************/
/*
  How the controls work :

  the game drivers expect a bit to be cleared or set when a specific control
  changes (like a button pressed/released or a movement of the stick).
  So for each bit of the controls to test, we have the index in our array of
  controls to test.

  For example :
   { KB_DEF_P1_B1,        MSG_P1_B1,               0x000001, 0x10, BIT_ACTIVE_0 },

   means : the key for p1_b1, which is called MSG_P1_B1 in the gui should change
   bitmask 0x10 at offset 1 in the controls so that the bit is 0 if the key is
   pressed.

   For the keyboard, KB_DEF_P1_B1 just gives an index in the allegro key array
   to check.
   For the joysticks, we reproduce the same behaviour by updating our own
   rjoy array based on the joystick changes (see update_rjoy_lists)
*/

#include <SDL.h>
#include "deftypes.h"
#include "games.h"
#include "control.h"
#include "demos.h"
#include "ingame.h"
#include "savegame.h"           // Save/Load game stuff
#include "profile.h" // switch_fps_mode
#include "display_sdl.h"
#include "blit.h" // SetupScreenBitmap
#include "video/newspr.h" // init_video_core
#include "control_internal.h"
#include "display.h"
#ifdef USE_BEZELS
#include "bezel.h"
#endif
#ifdef HAS_CONSOLE
#include "console/console.h"
#endif
#include "dialogs/cheats.h"
#include "sound/sasound.h"
#include "hiscore.h"
#include "neocd/cdda.h"
#include "newmem.h"
#include "emumain.h"
#include "opengl.h"
#include "files.h"

/* The difference in the sdl version :
 * instead of looping for every frame in all the available inputs to the game
 * (which is not so long after all), I tried to do the opposite : when we
 * receive a key up/down event, then find the corresponding input of the game,
 * and update it accordingly. It should be slightly faster, but in fact it's
 * probably impossible to measure. And the key array is still necessary to be
 * able to make an input which was invalid to become valid when another key
 * goes up (see the comments about that in the handling of the KEYUP event.
 *
 * Anyway this method to handle the inputs based on events is much more
 * convinient for the gui inputs at least, so we won something from it for
 * sure ! :) */

typedef struct joystick_state {
  int pos_axis[MAX_AXIS];
  UINT8 hat[MAX_HAT];
} joystick_state;

typedef struct {
    SDL_Joystick *joy;
#if SDL == 2
    SDL_GameController *controller;
#endif
    char *name;
    // The index is the index used in raine to know which joystick it is for the inputs
    // the instance is what is returned in the which field for its events... !
    int index,instance;
    joystick_state jstate;
    int cancel_sticks;
} tjoy;

static int nb_joy;
tjoy joy[MAX_JOY];

char analog_name[80]; // analog device saved by name because its index
// can change if it's pluged differently

int analog_num,analog_stick,analog_minx,analog_maxx,analog_miny,
  analog_maxy,analog_normx,analog_normy,
   pause_on_focus;
// analog_normx & normy are the normalized position of the stick after
// calibration (between -16384 and +16384 inclusive).
Uint8 key[0x300];
UINT8 input_buffer[0x100];
int GameMouse,use_leds;
int use_custom_keys;
int joy_use_custom_keys;

char *get_joy_name(int n) {
    for (int ta=0; ta<nb_joy; ta++)
	if (joy[ta].index == n)
	    return joy[ta].name;
    return "???";
}

int is_game_controller(int n) {
#if SDL < 2
    return 0;
#else
    return joy[n].controller != NULL;
#endif
}

int get_axis(int n,int axis) {
#if SDL == 2
  if (is_game_controller(n))
      return SDL_GameControllerGetAxis(joy[n].controller,axis);
#endif
  return SDL_JoystickGetAxis(joy[n].joy,axis);
}

int get_def_input(int inp) { // return index from InputList inputs having this default input or -1
    for (int n=0; n<InputCount; n++)
	if (InputList[n].default_key == inp)
	    return(n);
    return -1;
}

int get_axis_from_InputList(int inp) {
    int accel = 0;
    if (inp >= 0) {
       if (InputList[inp].Joy) {
	   int code = InputList[inp].Joy;
	   int which = get_joy_index_from_playerindex((code & 0xff)-1);
	   int axis = (InputList[inp].Joy >> 8) & 0xff;
	   if (axis) {
	       axis -= 2;
	       axis /= 2;
	       accel = get_axis(which, axis);
	   }
       }
    }
    return accel;
}

int get_joy_input(int num, int axe, int button, int hat) {
    // It's just some converssion from the JOY macro using the new player index
    // but since JOY must return constants because it's used for the default inputs
    // we use this function for everything which doesn't need to be constant
    num = joy[num].index+1; // the JOY macro takes num from 1
    return JOY(num, axe, button, hat);
}

int get_joy_naxes(int n) {
    return SDL_JoystickNumAxes(joy[n].joy);
}

/* Removed led handling for now. In SDL there is no direct support for that
 * and it's obvious that having direct access to the keyboard to be able
 * to manipulate the leds will be a problem in a multitasking OS */

#ifndef RAINE_UNIX
void switch_led(int a, int b) {
}
#endif

volatile int *MouseB;
UINT32 p1_trackball_x;
UINT32 p1_trackball_y;
int mouse_x,mouse_y,mouse_b;
// min/max coords for the mouse
static int min_x,max_x,min_y,max_y;

static int mickey_x, mickey_y;
float mouse_scale = 0.33;
float mickey_x_scaled, mickey_y_scaled;

void set_mouse_range(int x1, int y1, int x2, int y2) {
  min_x = x1;
  min_y = y1;
  max_x = x2;
  max_y = y2;
  if (mouse_x > max_x || mouse_x < min_x) mouse_x = min_x;
  if (mouse_y > max_y || mouse_y < min_y) mouse_y = min_y;
}

void my_get_mouse_mickeys(int *mx, int *my) {
  *mx = trunc(mickey_x_scaled);
  *my = trunc(mickey_y_scaled);
}

void (*GetMouseMickeys)(int *mx,int *my) = &my_get_mouse_mickeys;

#if SDL < 2
typedef enum
{
    SDL_CONTROLLER_BUTTON_INVALID = -1,
    SDL_CONTROLLER_BUTTON_A,
    SDL_CONTROLLER_BUTTON_B,
    SDL_CONTROLLER_BUTTON_X,
    SDL_CONTROLLER_BUTTON_Y,
    SDL_CONTROLLER_BUTTON_BACK,
    SDL_CONTROLLER_BUTTON_GUIDE,
    SDL_CONTROLLER_BUTTON_START,
    SDL_CONTROLLER_BUTTON_LEFTSTICK,
    SDL_CONTROLLER_BUTTON_RIGHTSTICK,
    SDL_CONTROLLER_BUTTON_LEFTSHOULDER,
    SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,
    SDL_CONTROLLER_BUTTON_DPAD_UP,
    SDL_CONTROLLER_BUTTON_DPAD_DOWN,
    SDL_CONTROLLER_BUTTON_DPAD_LEFT,
    SDL_CONTROLLER_BUTTON_DPAD_RIGHT,
    SDL_CONTROLLER_BUTTON_MISC1,    /* Xbox Series X share button, PS5 microphone button, Nintendo Switch Pro capture button, Amazon Luna microphone button */
    SDL_CONTROLLER_BUTTON_PADDLE1,  /* Xbox Elite paddle P1 */
    SDL_CONTROLLER_BUTTON_PADDLE2,  /* Xbox Elite paddle P3 */
    SDL_CONTROLLER_BUTTON_PADDLE3,  /* Xbox Elite paddle P2 */
    SDL_CONTROLLER_BUTTON_PADDLE4,  /* Xbox Elite paddle P4 */
    SDL_CONTROLLER_BUTTON_TOUCHPAD, /* PS4/PS5 touchpad button */
    SDL_CONTROLLER_BUTTON_MAX
} SDL_GameControllerButton;
#endif
/******************************************************************************/
/*                                                                            */
/*                        DEFAULT GAME KEY SETTINGS                           */
/*                                                                            */
/******************************************************************************/

// must be global for the controls dialog
// The categ field has the only purpose to sort the inputs in the control dialog !
struct DEF_INPUT def_input[KB_DEF_COUNT] =
{
 { SDL_SCANCODE_3,       JOY(1,0,SDL_CONTROLLER_BUTTON_BACK+1,0), 0, "Def Coin A",P1S        },      // KB_DEF_COIN1,
 { SDL_SCANCODE_4,       JOY(2,0,SDL_CONTROLLER_BUTTON_BACK+1,0), 0, "Def Coin B",P2S        },      // KB_DEF_COIN2,
 { SDL_SCANCODE_7,       JOY(3,0,SDL_CONTROLLER_BUTTON_BACK+1,0), 0, "Def Coin C",P3S        },      // KB_DEF_COIN3,
 { SDL_SCANCODE_8,       JOY(4,0,SDL_CONTROLLER_BUTTON_BACK+1,0), 0, "Def Coin D",P4S        },      // KB_DEF_COIN4,

 { SDL_SCANCODE_T,       0x00, 0, "Def Tilt",    SYS      },      // KB_DEF_TILT,
 { SDL_SCANCODE_Y,       0x00, 0, "Def Service", SYS      },      // KB_DEF_SERVICE,
 { SDL_SCANCODE_U,       0x00, 0, "Def Test",    SYS      },      // KB_DEF_TEST,

 { SDL_SCANCODE_1,       JOY(1,0,SDL_CONTROLLER_BUTTON_START+1,0), 0, "Def P1 Start",P1S      },      // KB_DEF_P1_START,

 { SDL_SCANCODE_UP,      JOY(1,AXIS_LEFT(1),0,0), 0, "Def P1 Up",P1D         },      // KB_DEF_P1_UP,
 { SDL_SCANCODE_DOWN,    JOY(1,AXIS_RIGHT(1),0,0), 0, "Def P1 Down",P1D       },      // KB_DEF_P1_DOWN,
 { SDL_SCANCODE_LEFT,    JOY(1,AXIS_LEFT(0),0,0), 0, "Def P1 Left",P1D       },      // KB_DEF_P1_LEFT,
 { SDL_SCANCODE_RIGHT,   JOY(1,AXIS_RIGHT(0),0,0), 0, "Def P1 Right",P1D      },      // KB_DEF_P1_RIGHT,

 { SDL_SCANCODE_A,       JOY(1,0,SDL_CONTROLLER_BUTTON_A+1,0), 1, "Def P1 Button 1", P1B      },      // KB_DEF_P1_B1,
 { SDL_SCANCODE_S,       JOY(1,0,SDL_CONTROLLER_BUTTON_B+1,0), 3, "Def P1 Button 2", P1B      },      // KB_DEF_P1_B2,
 { SDL_SCANCODE_D,       JOY(1,0,SDL_CONTROLLER_BUTTON_X+1,0), 2, "Def P1 Button 3", P1B      },      // KB_DEF_P1_B3,
 { SDL_SCANCODE_Z,       JOY(1,0,SDL_CONTROLLER_BUTTON_Y+1,0), 0, "Def P1 Button 4", P1B      },      // KB_DEF_P1_B4,
 { SDL_SCANCODE_X,       JOY(1,0,SDL_CONTROLLER_BUTTON_LEFTSHOULDER+1,0), 0, "Def P1 Button 5", P1B      },      // KB_DEF_P1_B5,
 { SDL_SCANCODE_C,       JOY(1,0,SDL_CONTROLLER_BUTTON_RIGHTSHOULDER+1,0), 0, "Def P1 Button 6", P1B      },      // KB_DEF_P1_B6,
 { SDL_SCANCODE_Q,       JOY(1,0,SDL_CONTROLLER_BUTTON_DPAD_LEFT+1,0), 0, "Def P1 Button 7", P1B      },      // KB_DEF_P1_B7,
 { SDL_SCANCODE_W,       JOY(1,0,SDL_CONTROLLER_BUTTON_DPAD_RIGHT+1,0), 0, "Def P1 Button 8", P1B      },      // KB_DEF_P1_B8,

 { SDL_SCANCODE_2,       JOY(2,0,SDL_CONTROLLER_BUTTON_START+1,0), 0, "Def P2 Start", P2S        },      // KB_DEF_P2_START,

 { 0,       JOY(2,AXIS_LEFT(1),0,0), 0, "Def P2 Up", P2D            },      // KB_DEF_P2_UP,
 { 0,       JOY(2,AXIS_RIGHT(1),0,0), 0, "Def P2 Down", P2D          },      // KB_DEF_P2_DOWN,
 { 0,       JOY(2,AXIS_LEFT(0),0,0), 0, "Def P2 Left", P2D          },      // KB_DEF_P2_LEFT,
 { 0,       JOY(2,AXIS_RIGHT(0),0,0), 0, "Def P2 Right", P2D         },      // KB_DEF_P2_RIGHT,

 { SDL_SCANCODE_KP_4,       JOY(2,0,SDL_CONTROLLER_BUTTON_A+1,0), 0, "Def P2 Button 1", P2B      },      // KB_DEF_P2_B1,
 { SDL_SCANCODE_KP_5,       JOY(2,0,SDL_CONTROLLER_BUTTON_B+1,0), 0, "Def P2 Button 2", P2B      },      // KB_DEF_P2_B2,
 { SDL_SCANCODE_KP_6,       JOY(2,0,SDL_CONTROLLER_BUTTON_X+1,0), 0, "Def P2 Button 3", P2B      },      // KB_DEF_P2_B3,
 { SDL_SCANCODE_KP_1,       JOY(2,0,SDL_CONTROLLER_BUTTON_Y+1,0), 0, "Def P2 Button 4", P2B      },      // KB_DEF_P2_B4,
 { SDL_SCANCODE_KP_2,       JOY(2,0,SDL_CONTROLLER_BUTTON_LEFTSHOULDER+1,0), 0, "Def P2 Button 5", P2B      },      // KB_DEF_P2_B5,
 { SDL_SCANCODE_KP_3,       JOY(2,0,SDL_CONTROLLER_BUTTON_RIGHTSHOULDER+1,0), 0, "Def P2 Button 6", P2B      },      // KB_DEF_P2_B6,
 { SDL_SCANCODE_KP_7,       JOY(2,0,SDL_CONTROLLER_BUTTON_DPAD_LEFT+1,0), 0, "Def P2 Button 7", P2B      },      // KB_DEF_P2_B7,
 { SDL_SCANCODE_KP_8,       JOY(2,0,SDL_CONTROLLER_BUTTON_DPAD_RIGHT+1,0), 0, "Def P2 Button 8", P2B      },      // KB_DEF_P2_B8,

 { SDL_SCANCODE_5,       JOY(3,0,SDL_CONTROLLER_BUTTON_START+1,0), 0, "Def P3 Start",P3S         },      // KB_DEF_P3_START,

 { 0,       JOY(3,AXIS_LEFT(1),0,0), 0, "Def P3 Up", P3D            },      // KB_DEF_P3_UP,
 { 0,       JOY(3,AXIS_RIGHT(1),0,0), 0, "Def P3 Down", P3D          },      // KB_DEF_P3_DOWN,
 { 0,       JOY(3,AXIS_LEFT(0),0,0), 0, "Def P3 Left", P3D          },      // KB_DEF_P3_LEFT,
 { 0,       JOY(3,AXIS_RIGHT(0),0,0), 0, "Def P3 Right", P3D         },      // KB_DEF_P3_RIGHT,

 { 0x00,        JOY(3,0,SDL_CONTROLLER_BUTTON_A+1,0), 0, "Def P3 Button 1", P3B      },      // KB_DEF_P3_B1,
 { 0x00,        JOY(3,0,SDL_CONTROLLER_BUTTON_B+1,0), 0, "Def P3 Button 2", P3B      },      // KB_DEF_P3_B2,
 { 0x00,        JOY(3,0,SDL_CONTROLLER_BUTTON_X+1,0), 0, "Def P3 Button 3", P3B      },      // KB_DEF_P3_B3,
 { 0x00,        JOY(3,0,SDL_CONTROLLER_BUTTON_Y+1,0), 0, "Def P3 Button 4", P3B      },      // KB_DEF_P3_B4,
 { 0x00,        JOY(3,0,SDL_CONTROLLER_BUTTON_LEFTSHOULDER+1,0), 0, "Def P3 Button 5", P3B      },      // KB_DEF_P3_B5,
 { 0x00,        JOY(3,0,SDL_CONTROLLER_BUTTON_RIGHTSHOULDER+1,0), 0, "Def P3 Button 6", P3B      },      // KB_DEF_P3_B6,
 { 0x00,        JOY(3,0,SDL_CONTROLLER_BUTTON_DPAD_LEFT+1,0), 0, "Def P3 Button 7", P3B      },      // KB_DEF_P3_B7,
 { 0x00,        JOY(3,0,SDL_CONTROLLER_BUTTON_DPAD_RIGHT+1,0), 0, "Def P3 Button 8", P3B      },      // KB_DEF_P3_B8,

 { SDL_SCANCODE_6,       JOY(4,0,SDL_CONTROLLER_BUTTON_START+1,0), 0, "Def P4 Start", P4S         },      // KB_DEF_P4_START,

 { 0,       JOY(4,AXIS_LEFT(1),0,0), 0, "Def P4 Up", P4D            },      // KB_DEF_P4_UP,
 { 0,       JOY(4,AXIS_RIGHT(1),0,0), 0, "Def P4 Down", P4D          },      // KB_DEF_P4_DOWN,
 { 0,       JOY(4,AXIS_LEFT(0),0,0), 0, "Def P4 Left", P4D          },      // KB_DEF_P4_LEFT,
 { 0,       JOY(4,AXIS_RIGHT(0),0,0), 0, "Def P4 Right", P4D         },      // KB_DEF_P4_RIGHT,

 { 0x00,        JOY(4,0,SDL_CONTROLLER_BUTTON_A+1,0), 0, "Def P4 Button 1", P4B      },      // KB_DEF_P4_B1,
 { 0x00,        JOY(4,0,SDL_CONTROLLER_BUTTON_B+1,0), 0, "Def P4 Button 2", P4B      },      // KB_DEF_P4_B2,
 { 0x00,        JOY(4,0,SDL_CONTROLLER_BUTTON_X+1,0), 0, "Def P4 Button 3", P4B      },      // KB_DEF_P4_B3,
 { 0x00,        JOY(4,0,SDL_CONTROLLER_BUTTON_Y+1,0), 0, "Def P4 Button 4", P4B      },      // KB_DEF_P4_B4,
 { 0x00,        JOY(4,0,SDL_CONTROLLER_BUTTON_LEFTSHOULDER+1,0), 0, "Def P4 Button 5", P4B      },      // KB_DEF_P4_B5,
 { 0x00,        JOY(4,0,SDL_CONTROLLER_BUTTON_RIGHTSHOULDER+1,0), 0, "Def P4 Button 6", P4B      },      // KB_DEF_P4_B6,
 { 0x00,        JOY(4,0,SDL_CONTROLLER_BUTTON_DPAD_LEFT+1,0), 0, "Def P4 Button 7", P4B      },      // KB_DEF_P4_B7,
 { 0x00,        JOY(4,0,SDL_CONTROLLER_BUTTON_DPAD_RIGHT+1,0), 0, "Def P4 Button 8", P4B      },      // KB_DEF_P4_B8,

 { SDL_SCANCODE_LCTRL,0x00, 0, "Def Flipper 1 Left", P1B   },      // KB_DEF_FLIPPER_1_L,
 { SDL_SCANCODE_RCTRL,0x00, 0, "Def Flipper 1 Right", P1B  },      // KB_DEF_FLIPPER_1_R,
 { SDL_SCANCODE_LSHIFT,  0x00, 0, "Def Flipper 2 Left", P2B   },      // KB_DEF_FLIPPER_2_L,
 { SDL_SCANCODE_RSHIFT,  0x00, 0, "Def Flipper 2 Right", P2B  },      // KB_DEF_FLIPPER_2_R,
 { SDL_SCANCODE_BACKSLASH,0x00, 0, "Def Tilt Left", SYS        },      // KB_DEF_TILT_L,
 { SDL_SCANCODE_SLASH,   0x00, 0, "Def Tilt Right", SYS       },      // KB_DEF_TILT_R,
 { SDL_SCANCODE_Z,       0x00, 0, "Def Button 1 Left", P1B    },      // KB_DEF_B1_L,
 { SDL_SCANCODE_E,    0x00, 0, "Def Button 1 Right", P1B   },      // KB_DEF_B1_R,

 // Mahjong controls, at least in mahjong quest...
 { SDL_SCANCODE_A,       0x00, 0, "Def P1 A", P1B             }, // KB_DEF_P1_A
 { SDL_SCANCODE_E,       0x00, 0, "Def P1 E", P1B             }, // KB_DEF_P1_E
 { SDL_SCANCODE_I,       0x00, 0, "Def P1 I", P1B             }, // KB_DEF_P1_I
 { SDL_SCANCODE_M,       0x00, 0, "Def P1 M", P1B             }, // KB_DEF_P1_M
 { SDL_SCANCODE_LCTRL,0x00, 0, "Def P1 Kan", P1B           }, // KB_DEF_P1_KAN
 { SDL_SCANCODE_B,       0x00, 0, "Def P1 B", P1B             }, // KB_DEF_P1_B
 { SDL_SCANCODE_F,       0x00, 0, "Def P1 F", P1B             }, // KB_DEF_P1_F
 { SDL_SCANCODE_J,       0x00, 0, "Def P1 J", P1B             }, // KB_DEF_P1_J
 { SDL_SCANCODE_N,       0x00, 0, "Def P1 N", P1B             }, // KB_DEF_P1_N
 { SDL_SCANCODE_LSHIFT,  0x00, 0, "Def P1 Reach", P1B         }, // KB_DEF_P1_REACH
 { SDL_SCANCODE_C,       0x00, 0, "Def P1 C", P1B             }, // KB_DEF_P1_C
 { SDL_SCANCODE_G,       0x00, 0, "Def P1 G", P1B             }, // KB_DEF_P1_G
 { SDL_SCANCODE_K,       0x00, 0, "Def P1 K", P1B             }, // KB_DEF_P1_K
 { SDL_SCANCODE_SPACE,   0x00, 0, "Def P1 Chi", P1B           }, // KB_DEF_P1_CHI
 { SDL_SCANCODE_Z,       0x00, 0, "Def P1 Ron", P1B           }, // KB_DEF_P1_RON,
 { SDL_SCANCODE_D,       0x00, 0, "Def P1 D", P1B             }, // KB_DEF_P1_D
 { SDL_SCANCODE_H,       0x00, 0, "Def P1 H", P1B             }, // KB_DEF_P1_H
 { SDL_SCANCODE_L,       0x00, 0, "Def P1 L", P1B             }, // KB_DEF_P1_L
 { SDL_SCANCODE_LALT,     0x00, 0, "Def P1 Pon", P1B           }, // KB_DEF_P1_PON

 { 0,           0, 0, "Def Service A", SYS }, // KB_DEF_SERVICE_A
 { 0,           0, 0, "Def Service B", SYS }, // KB_DEF_SERVICE_B
 { 0,           0, 0, "Def Service C", SYS }, // KB_DEF_SERVICE_C

 { SDL_SCANCODE_R,       0x00, 0, "Def Button 2 Left", P2B    },      // KB_DEF_B2_L,
 { SDL_SCANCODE_T,    0x00, 0, "Def Button 2 Right", P2B   },      // KB_DEF_B2_R,

 { 0,           0, 0, "Player1 B1+B2", P1C }, // p1_b1B2
 { 0,           0, 0, "Player1 B3+B4", P1C },
 { 0,           0, 0, "Player1 B2+B3", P1C },
 { 0,           0, 0, "Player1 B1+B2+B3", P1C },
 { 0,           0, 0, "Player1 B2+B3+B4", P1C },

 { 0,           0, 0, "Player2 B1+B2", P2C }, // p2_b1B2
 { 0,           0, 0, "Player2 B3+B4", P2C },
 { 0,           0, 0, "Player2 B2+B3", P2C },
 { 0,           0, 0, "Player2 B1+B2+B3", P2C },
 { 0,           0, 0, "Player2 B2+B3+B4", P2C },
 { 0,           0, 0, "Player1 B4+B5+B6", P1C },
 { 0,           0, 0, "Player3 B1+B2", P3C },
 { 0,           0, 0, "Player4 B1+B2", P4C },

 { 0,           0, 0, "Next Game", SYS },
 { 0,           0, 0, "Prev Game", SYS },

 { 0,           0, 0, "", }, // unknown, should be hidden
 { 0,           0, 0, "", }, // special, should be hidden
 { 0,           0, 0, "", }, // unused, should be hidden
};

/******************************************************************************/
/*                                                                            */
/*                       DEFAULT EMULATOR KEY SETTINGS                        */
/*                                                                            */
/******************************************************************************/

static void key_save_screen(void)
{
   raine_cfg.req_save_screen = 1;
}

static void key_quit() {
    // Violent quit, equivalent of closing the window
    if (recording)
	end_recording();
    exit(1);
}

static void frame_skip_up(void)
{
   if((display_cfg.frame_skip<9) && (display_cfg.frame_skip)){
      display_cfg.frame_skip++;
      print_ingame(120,gettext("Drawing Every %1d Frames"),display_cfg.frame_skip);
   }
}

static void frame_skip_down(void)
{
   if((display_cfg.frame_skip>1) && (display_cfg.frame_skip)){
      display_cfg.frame_skip--;
      if(display_cfg.frame_skip==1)
	 print_ingame(120,gettext("Drawing All Frames"));
      else
	 print_ingame(120,gettext("Drawing Every %1d Frames"),display_cfg.frame_skip);
   }
}

extern void cpu_speed_up(); // emumain.c
extern void cpu_slow_down(); // emumain.c

static int my_frame;
static double my_time;

static void toggle_limit_speed() {
	if(display_cfg.limit_speed){
		print_ingame(60,gettext("No speed limit!"));
		display_cfg.limit_speed = 0;
		my_frame = cpu_frame_count;
		my_time = timer_get_time();
		if (display_cfg.video_mode == 0) // opengl
		    update_ogl_dbuf(0);
	} else {
	    double t = timer_get_time();
	    if (t > my_time) {
		print_ingame(120,gettext("%d frames in %gs -> %g fps"),cpu_frame_count-my_frame,t-my_time,(cpu_frame_count-my_time)/(t-my_time));
	    }
	    print_ingame(120,gettext("Speed limit %g FPS"),fps);
	    display_cfg.limit_speed = 1;
	    if (display_cfg.video_mode == 0) // opengl
		update_ogl_dbuf(ogl.dbuf);
	}
}

extern void key_stop_emulation_esc(void);
extern void key_stop_emulation_tab(void);

void toggle_fullscreen() {
#if SDL == 1
  resize(1);
  SetupScreenBitmap();
  if (current_game) {
    init_video_core();
    reset_ingame_timer();
  }
#else
  // For some totally unknown reason, SDL_SetWindowFullscreen here is totally broken on my laptop but works on my desktop !
  // they both have very similar software configuration, windowmaker, same xorg, just the desktop uses some nvidia card and the laptop an intel
  // anyway the problem is the window receives quite a few events when going to fullscreen, in the end it's minimized and hidden, and bye bye, if I try to force call
  // SDL_ShowWindow then the screen starts to blink because the window manager keeps on trying to hide it !
  // Calling instead these 2 functions to manually set the position and the size which should be totally equivalent fixes the problem !!!
  if (display_cfg.fullscreen == 1) {
      if (hack_fs) {
	  SDL_SetWindowPosition(win,0,0);
	  SDL_SetWindowSize(win,desktop_w,desktop_h);
      } else
	  SDL_SetWindowFullscreen(win,SDL_WINDOW_FULLSCREEN_DESKTOP);
  } else if (display_cfg.fullscreen == 2) {
      SDL_SetWindowSize(win,desktop_w,desktop_h);
      SDL_SetWindowFullscreen(win,SDL_WINDOW_FULLSCREEN);
  } else {
      SDL_SetWindowFullscreen(win,0);
      SDL_SetWindowSize(win,display_cfg.prev_sx,display_cfg.prev_sy);
      SDL_SetWindowPosition(win,display_cfg.posx,display_cfg.posy); // posx & posy are not updated when switching to fullscreen
  }
  ScreenChange();
#endif
}

static void toggle_fullscreen_keyboard() {
  if (display_cfg.fullscreen) {
    display_cfg.fullscreen = 0;
  } else {
    display_cfg.fullscreen = 1;
  }
  toggle_fullscreen();
}

#ifdef HAS_CONSOLE
static void call_console() {
  do_console(0);
  reset_ingame_timer();
}
#endif

static void update_input_buffer(int ta, int input_valid);
static void remove_valid_input(int ta);

void invalidate_inputs() {
  if (!current_game || !current_game->input) return;
  int ta = 0;
  const INPUT_INFO *input_src = current_game->input;
  while(input_src[ta].name){

      update_input_buffer(ta,0); // say input is not valid for now
      remove_valid_input(ta);

      ta++;
  }
  memset(key,0,sizeof(key));
}

static void call_cheats() {
  do_cheats(0);
  reset_ingame_timer();
}

int get_console_key() {
  int n;
  int nb = raine_get_emu_nb_ctrl();
  for (n=nb-1; n>=0; n--) {
    if (!strcmp(def_input_emu[n].name,"Console")) {
      int code = def_input_emu[n].scancode;
      return code;
    }
  }
  return 0;
}

static struct DEF_INPUT_EMU *driver_emu_list = NULL;
static int driver_nb_emu_inputs;

#if SDL == 2
static void update_index(int n, int index) {
    // This function because SDL_JoystickSetPlayerIndex does not work on most joysticks !
    if (joy[n].controller)
	joy[n].index = SDL_GameControllerGetPlayerIndex(joy[n].controller);
    else {
	if (joy[n].index == index) {
	    if (nb_joy == index)
		joy[n].index = 0;
	    else
		joy[n].index = nb_joy;
	    char used[nb_joy];
	    memset(used,0,nb_joy);
	    for (int ta=0; ta<nb_joy; ta++) {
		used[joy[ta].index]++;
		if (used[joy[ta].index] > 1) {
		    printf("index %d used more than once, complete reset !\n",ta);
		    for (n=0; n<nb_joy; n++)
			joy[n].index = n;
		    return;
		}
	    }
	}
    }
}
#endif

int get_joy_playerindex(int n) {
    return joy[n].index;
}

void set_joy_playerindex(int n, int index) {
    // This one is for the controls.cpp graphic interface
    // it does no checks, just assign things, that's all
    joy[n].index = index;
#if SDL == 2
    if (joy[n].controller)
	SDL_GameControllerSetPlayerIndex(joy[n].controller,index);
    else
	SDL_JoystickSetPlayerIndex(joy[n].joy,index);
#endif
}

int get_joy_index_from_playerindex(int index) {
#if SDL == 2
    for (int ta=0; ta<nb_joy; ta++) {
	if (joy[ta].index == index)
	    return ta;
    }
    return -1;
#else
    return index; // not sure for sdl1, indexes don't make much sense there...
#endif
}

int get_joy_index_from_instance(int inst) {
#if SDL == 2
    for (int ta=0; ta<nb_joy; ta++) {
	if (joy[ta].instance == inst)
	    return ta;
    }
    return -1;
#else
    return inst; // function does not exist in sdl1, hotpluging is not supported
#endif
}

static int get_joy_index(int n) {
#if SDL == 2
    SDL_JoystickGUID guid;
    guid = SDL_JoystickGetDeviceGUID(n);
    char guid_str[35];
    SDL_JoystickGetGUIDString(guid,guid_str,35);
    int index = raine_get_config_int("emulator_joy_config",guid_str,-1);
    if (index == -1) {
	if (joy[n].controller)
	    index = SDL_GameControllerGetPlayerIndex(joy[n].controller);
	else
	    index = SDL_JoystickGetPlayerIndex(joy[n].joy);
	if (index == -1) {
	    index = nb_joy;
	    printf("can't get any index, using %d\n",index);
	    // Function doesn't work for most joysticks, but anyway... !!!
	    SDL_JoystickSetPlayerIndex(joy[n].joy,index);
	}
	printf("no index found, %d assigned\n",index);
    } else {
	printf("got index %d from config\n",index);
	if (index >= SDL_NumJoysticks()) {
	    index = SDL_NumJoysticks()-1;
	    printf("but index >= numjoysticks (%d), setting to %d\n",SDL_NumJoysticks(),index);
	}
	SDL_GameControllerSetPlayerIndex(joy[n].controller,index);
	for (int ta=0; ta<n; ta++) {
	    update_index(ta,index);
	    printf("%d: index %d\n",ta,joy[ta].index);
	}
    }
    return index;
#else
    return 0; // no guid in sdl1.2
#endif
}

#if SDL == 2
static void del_controller(int n) {
    if (joy[n].controller) {
	SDL_GameControllerClose(joy[n].controller);
	joy[n].controller = NULL;
    } else
    {
	SDL_JoystickClose(joy[n].joy);
	joy[n].joy = NULL;
    }
    for (int ta=n+1; ta<nb_joy; ta++) {
	printf("%d -> %d\n",ta,ta-1);
	joy[ta-1] = joy[ta];
    }
    memset(&joy[nb_joy-1],0,sizeof(tjoy));
    nb_joy--;
}
#endif

static void add_game_controller(int n) {
#if SDL == 2
    if (SDL_IsGameController(n)) {
	joy[n].controller = SDL_GameControllerOpen(n);
	joy[n].name = (char*)SDL_GameControllerNameForIndex(n);
	if (!joy[n].controller) {
	    fatal_error("Open controller failed: %s",SDL_GetError());
	}
    } else
#endif
    {
	joy[n].joy = SDL_JoystickOpen(n);
	if (!joy[n].joy) {
	    fatal_error("Open joystick failed: %s",SDL_GetError());
	}
#if SDL==2
	joy[n].name = (char*)SDL_JoystickName(joy[n].joy);
#else
	joy[n].name = (char*)SDL_JoystickName(n);
#endif
    }
    memset(&joy[n].jstate,0,sizeof(joy[n].jstate));
    joy[n].index = get_joy_index(n);
#if SDL == 2
    joy[n].instance = SDL_JoystickGetDeviceInstanceID(n);
#else
    joy[n].instance = n;
#endif
    nb_joy++;
    printf("controller %d opened (%s) index %d\n",n,joy[n].name,joy[n].index);
}

static void cold_boot() {
    if (!current_game) return; // can be called from the gui with no game loaded by remaping a joystick control !
    hs_close(); // save hiscores BEFORE erasing the ram !
#if HAS_NEO
    cdda_stop();
#endif
    if (current_game->input->offset < 0x100 &&
	    (!current_game->dsw || current_game->dsw->offset < 0x100))  {
	// Clear the ram only if no inputs and no dsw are stored in ram !
	if (Z80ROM > RAM && Z80ROM - RAM < RAMSize) {
	    memset(RAM,0,Z80ROM-RAM);
	    if (Z80ROM + 0x10000 - RAM < RAMSize)
		memset(Z80ROM + 0x10000,0,RAMSize - ((Z80ROM + 0x10000) - RAM));
	} else
	    memset(RAM,0,RAMSize);
    }
    reset_game_hardware();
}

// must be global for the controls dialog
struct DEF_INPUT_EMU def_input_emu[] =
{
 { SDL_SCANCODE_S ,       0x00,           _("Save Screenshot"), KMOD_CTRL, key_save_screen     },
 { SDL_SCANCODE_RETURN ,       0x00,           _("Fullscreen"), KMOD_ALT, toggle_fullscreen_keyboard     },
 { SDL_SCANCODE_PAGEUP,    0x00,           _("Increase frameskip"), 0, frame_skip_up  },
 { SDL_SCANCODE_PAGEDOWN,    0x00,           _("Decrease frameskip"), 0, frame_skip_down  },
 { SDL_SCANCODE_HOME,    0x00,           _("Increase CPU skip"),    0, cpu_speed_up},
 { SDL_SCANCODE_END,     0x00,           _("Decrease CPU skip"),    0, cpu_slow_down},
 // You must keep this one the 6th input, see special handling (KEYUP event)
 { SDL_SCANCODE_DELETE,  0x00,           _("Toggle limit speed"),   0, toggle_limit_speed},
 { SDL_SCANCODE_F2,      0x00,           _("Save state"),            0, GameSave},
 { SDL_SCANCODE_F3,      0x00,           _("Switch save slot"),     0, next_save_slot},
 { SDL_SCANCODE_F4,      0x00,           _("Load state"),            0, GameLoad},
#ifdef DARWIN
 { SDL_SCANCODE_F12,     0x00,           _("Switch FPS display"),   0, switch_fps_mode},
#else
 { SDL_SCANCODE_F11,     0x00,           _("Switch FPS display"),   0, switch_fps_mode},
#endif
 { SDL_SCANCODE_F1,      0x00,           _("Reset game"),           0, cold_boot},
 { SDL_SCANCODE_P,       0x00,           _("Pause game"),           0, key_pause_game},
 { SDL_SCANCODE_ESCAPE,  JOY(1,0,6,0),           _("Stop emulation"),    0, key_stop_emulation_esc},
 { SDL_SCANCODE_TAB,     0x00,           _("Return to GUI"),        0, key_stop_emulation_tab},
 // { SDL_SCANCODE_WORLD_18,   0x00,           _("Switch Mixer"), switch_mixer },
 { SDL_SCANCODE_F2 , 0x00, _("Save state with name"), KMOD_CTRL, GameSaveName },
 { SDL_SCANCODE_F4 , 0x00, _("Load state with name"), KMOD_CTRL, GameLoadName },
 { SDL_SCANCODE_F2 , 0x00, _("Save demo"), KMOD_SHIFT, GameSaveDemo },
 { SDL_SCANCODE_F4 , 0x00, _("Load demo"), KMOD_SHIFT, GameLoadDemo },
 { SDL_SCANCODE_SPACE, 0x00, _("Forward 1 frame in pause"), 0, key_pause_fwd},
#ifdef HAS_CONSOLE
 { SDL_SCANCODE_GRAVE, 0x00, _("Console"), 0, call_console},
#endif
 { SDL_SCANCODE_C , 0x00, _("Cheats"), KMOD_ALT, call_cheats},
 { SDL_SCANCODE_F4,       0x00,           _("Quit without saving"), KMOD_ALT, key_quit     },
};

struct INPUT InputList[MAX_INPUTS];	// Max 64 control inputs in a game

int InputCount;			// Number of Inputs in InputList

int hat_for_moves;

static void set_key_from_default(INPUT *inp)
{

    inp->Key = def_input[inp->default_key & 0xFF].scancode;
    inp->Joy = def_input[inp->default_key & 0xFF].joycode;
    inp->mousebtn = def_input[inp->default_key & 0xFF].mousebtn;

}

static void update_input_buffer(int ta, int input_valid) {
  UINT8 *buffer;

  if(InputList[ta].Address < 0x100)
     buffer = input_buffer;
   else
     buffer = RAM;

  buffer += InputList[ta].Address;

  if(input_valid ^ InputList[ta].high_bit)
    *buffer &= ~ InputList[ta].Bit;
   else
     *buffer |=   InputList[ta].Bit;
}

static int nb_valid_inputs;
// the number of valid inputs at the same time : rather 2 or 3 max, but we'll
// take some margin...
#define MAX_VALID_INPUTS 20
int valid_inputs[MAX_VALID_INPUTS]; // a list
static UINT8 autofire_timer[6];
static UINT8  stick_logic[4];

int has_input(int inp) {
    int n;
    for (n=0; n<InputCount; n++) {
	if (InputList[n].default_key == inp)
	    return 1;
    }
    return 0;
}

static void merge_inputs(const INPUT_INFO *input_src) {
    int srcCount = 0;
    int included = 0;
    while(input_src[srcCount].name){

	if (input_src[srcCount].flags == INPUT_INCLUDE) {
	    merge_inputs((const INPUT_INFO *)input_src[srcCount].name);
	    srcCount++;
	    included = 1;
	    continue;
	}

	int n,old = -1;
	// Input overwrite : it happens in case another input is included and
	// then modified
	if (included) { // only if some inputs were included
	    // Otherwise it prevents combinations like A+B in neocd from
	    // working !
	    for (n=0; n<InputCount; n++) {
		// Exact bit masks for normal inputs, only part of it to overwrite unknown or unused input
		// to be totally exact, I should split the bitmask of unknown/unused inputs, but I'll try to
		// forget this part to see how it works...
		if ((input_src[srcCount].default_key == KB_DEF_UNKNOWN ||
			    input_src[srcCount].default_key == KB_DEF_UNUSED) &&
			input_src[srcCount].offset == InputList[n].Address &&
			(input_src[srcCount].bit_mask & InputList[n].Bit)) {
		    // overwritten by unused/unknown -> the old input simply disappears in this case...
		    // printf("Erase input %s, InputCount %d offset %x mask %x from %s off %x mask %x\n",InputList[n].InputName,InputCount,InputList[n].Address,InputList[n].Bit,
			//    input_src[srcCount].name,input_src[srcCount].offset,input_src[srcCount].bit_mask);
		    memmove(&InputList[n],&InputList[n+1],(InputCount-n)*sizeof(struct INPUT));
		    InputCount--;
		    n--; continue;
		}
		if (input_src[srcCount].offset == InputList[n].Address &&
			(input_src[srcCount].bit_mask == InputList[n].Bit ||
			((input_src[srcCount].bit_mask & InputList[n].Bit) &&
			 (InputList[n].default_key == KB_DEF_UNKNOWN ||
			 InputList[n].default_key == KB_DEF_UNUSED)))) {
		    // printf("overwriting %s with %s\n",InputList[n].InputName,input_src[srcCount].name);
		    old = InputCount;
		    InputCount = n;
		    break;
		}
	    }
	}

	UINT16 def = InputList[InputCount].default_key = input_src[srcCount].default_key;
	InputList[InputCount].InputName   = input_src[srcCount].name;
	InputList[InputCount].Address     = input_src[srcCount].offset;
	InputList[InputCount].Bit         = input_src[srcCount].bit_mask;
	InputList[InputCount].high_bit    = input_src[srcCount].flags;
	InputList[InputCount].auto_rate   = 0;
	InputList[InputCount].active_time = 0;
	InputList[InputCount].link = 0;

	set_key_from_default(&InputList[InputCount]);

	update_input_buffer(InputCount,0); // say input is not valid for now

	if (def != KB_DEF_UNKNOWN && def != KB_DEF_SPECIAL &&
		def != KB_DEF_UNUSED) {
	    // Skip unknown and special inputs after they have been initialized
	    InputCount++;
	} else if (old > -1) {
	    // input overwritten by a hidden input, we must move the list...
	    if (old > InputCount+1)
		memmove(&InputList[InputCount],&InputList[InputCount+1],
			(old-(InputCount+1))*sizeof(struct INPUT));
	    old--;
	}
	if (old > -1) {
	    // Check if some other inputs match the bit mask and remove them
	    for (n=InputCount; n<old; n++) {
		if (input_src[srcCount].offset == InputList[n].Address &&
			(input_src[srcCount].bit_mask == InputList[n].Bit ||
			((input_src[srcCount].bit_mask & InputList[n].Bit) &&
			 (InputList[n].default_key == KB_DEF_UNKNOWN ||
			 InputList[n].default_key == KB_DEF_UNUSED)))) {
		    if (old > n+1)
			memmove(&InputList[n],&InputList[n+1],
				(old-(n+1))*sizeof(struct INPUT));
		    old--;
		    n--;
		}
	    }
	    InputCount = old;
	}
	srcCount++;
    }
}

void init_inputs(void)
{
   const INPUT_INFO *input_src;
   mickey_x_scaled = mickey_y_scaled = 0.0;

   memset(key,0,sizeof(key));
   memset(autofire_timer,0,sizeof(autofire_timer));
   memset(stick_logic,0,sizeof(stick_logic));
   for (int n=0; n<MAX_JOY; n++) {
       if (joy[n].joy
#if SDL == 2
	       || joy[n].controller
#endif
	       )
	   memset(&joy[n].jstate,0,sizeof(joy[n].jstate));
   }

   nb_valid_inputs = 0;

   InputCount = 0;

   input_src = current_game->input;

   if(input_src)
       merge_inputs(&input_src[InputCount]);

}

void release_inputs(void)
{
  if (!current_game) return;
  const INPUT_INFO *input_src;
  int ta;
  // just release the inputs when coming back from the gui...

  memset(key,0,sizeof(key));
  for (int n=0; n<MAX_JOY; n++) {
      if (joy[n].joy
#if SDL == 2
	      || joy[n].controller
#endif
	      )
	  memset(&joy[n].jstate,0,sizeof(joy[n].jstate));
  }
  memset(autofire_timer,0,sizeof(autofire_timer));
  memset(stick_logic,0,sizeof(stick_logic));

  nb_valid_inputs = 0;
  ta = 0;

  input_src = current_game->input;

  if(input_src && RAM){
    // Check for RAM because if loading a game fails, input_src is defined
    // but not ram

    while(input_src[ta].name){

      update_input_buffer(ta,0); // say input is not valid for now

      ta++;
    }

  }
}

void reset_game_keys(void)
{
   int ta;

   for(ta=0;ta<InputCount;ta++) {
     if (InputList[ta].link == 0 || InputList[ta].link > ta)
       set_key_from_default(&InputList[ta]);
   }
}

void no_spaces(char *str)
{
   int ta,tb;

   tb=strlen(str);

   for(ta=0;ta<tb;ta++){
      if(((str[ta]<'A')||(str[ta]>'Z'))&&
         ((str[ta]<'a')||(str[ta]>'z'))&&
         ((str[ta]<'0')||(str[ta]>'9'))){
         str[ta]='_';
      }
   }
}

static void load_emu_keys(char *section, struct DEF_INPUT_EMU *list_emu, int nb) {
  int ta,scan;
  char key_name[64];
  if (raine_get_config_int(section,"version",0) != 2)
      return;
   for(ta=0;ta<nb;ta++){
      sprintf(key_name,"%s",list_emu[ta].name);
      no_spaces(key_name);
      scan = raine_get_config_int(section,key_name,list_emu[ta].scancode);
      if (strncmp(key_name,"Screen_",7) || scan < SDLK_LEFT || scan > SDLK_DOWN) {
	// Forces modifiers for scrolling keys (previously in pause only)
	list_emu[ta].scancode = scan;
      }
      strcat(key_name,"_kmod");
      list_emu[ta].kmod = raine_get_config_int(section,key_name,list_emu[ta].kmod);
   }
}

void load_game_keys(char *section)
{
   int ta;
   char key_name[64],other_name[64];

   if (raine_get_config_int(section,"version",0) != 2)
       return;

   use_custom_keys = raine_get_config_int(section,"use_custom_keys",0);
   // load keys if using custom keys

   if(use_custom_keys){

      for(ta=0;ta<InputCount;ta++){
	int link;
	if (InputList[ta].link && InputList[ta].link < ta)
	  continue;
	if (!InputList[ta].InputName)
	    continue;
	sprintf(key_name,"%s",def_input[InputList[ta].default_key-0x100].name);
	no_spaces(key_name);
	InputList[ta].Key = raine_get_config_int(section,key_name,InputList[ta].Key);
	snprintf(other_name,64,"%s_joystick",key_name);
	InputList[ta].Joy = raine_get_config_int(section,other_name,InputList[ta].Joy);
	snprintf(other_name,64,"%s_mouse",key_name);
	InputList[ta].mousebtn = raine_get_config_int(section,other_name,InputList[ta].mousebtn);
	snprintf(other_name,64,"%s_auto_rate",key_name);
	InputList[ta].auto_rate = raine_get_config_int(section,other_name,0);
	snprintf(other_name,64,"%s_link",key_name);
	link = raine_get_config_int(section,other_name,0);
	if (link) {
	  InputList[ta].link = link;
	  InputList[link] = InputList[ta];
	  InputList[link].link = ta;
	  snprintf(other_name,64,"%s_linked_key",key_name);
	  InputList[link].Key = raine_get_config_int(section,other_name,0);
	  snprintf(other_name,64,"%s_linked_joy",key_name);
	  InputList[link].Joy = raine_get_config_int(section,other_name,0);
	  snprintf(other_name,64,"%s_linked_mouse",key_name);
	  InputList[link].mousebtn = raine_get_config_int(section,other_name,0);
	  snprintf(other_name,64,"%s_linked_auto_rate",key_name);
	  InputList[link].auto_rate = raine_get_config_int(section,other_name,0);
	  // Different name to allow it to be assigned !
	  char s[120];
	  sprintf(s,"%s %s",gettext("Autofire"),
		  gettext(InputList[link].InputName));
	  InputList[link].InputName = (char*)AllocateMem(strlen(s)+1);
	  strcpy(InputList[link].InputName,s);
	  InputCount = link+1;
	}
      }
   }
#if 0
  if (driver_nb_emu_inputs)
    load_emu_keys(section,driver_emu_list,driver_nb_emu_inputs);
#endif
}

void save_game_keys(char *section)
{
   int ta;
   char key_name[64],other_name[64];

   // clear the old settings first (keep the file tidy)

   raine_clear_config_section(section);
   raine_set_config_int(section,"version",2);

   // save keys if using custom keys

   if(use_custom_keys){

      raine_set_config_int(section,"use_custom_keys",use_custom_keys);

      for(ta=0;ta<InputCount;ta++){
	 if (InputList[ta].link && InputList[ta].link < ta)
	   continue;
	 if (!InputList[ta].InputName)
	    continue;
	 sprintf(key_name,"%s",def_input[InputList[ta].default_key-0x100].name);
         no_spaces(key_name);
	 raine_set_config_int(section,key_name,InputList[ta].Key);
	 snprintf(other_name,64,"%s_joystick",key_name);
	 raine_set_config_int(section,other_name,InputList[ta].Joy);

	 snprintf(other_name,64,"%s_mouse",key_name);
	 raine_set_config_int(section,other_name,InputList[ta].mousebtn);

	 if (InputList[ta].auto_rate) {
	   snprintf(other_name,64,"%s_auto_rate",key_name);
	   raine_set_config_int(section,other_name,InputList[ta].auto_rate);
	 }
	 if (InputList[ta].link > ta) {
	   int link = InputList[ta].link;
	   snprintf(other_name,64,"%s_link",key_name);
	   raine_set_config_int(section,other_name,link);
	   if (InputList[link].Key) {
	     snprintf(other_name,64,"%s_linked_key",key_name);
	     raine_set_config_int(section,other_name,InputList[link].Key);
	   }
	   if (InputList[link].Joy) {
	     snprintf(other_name,64,"%s_linked_joy",key_name);
	     raine_set_config_int(section,other_name,InputList[link].Joy);
	   }
	   if (InputList[link].mousebtn) {
	     snprintf(other_name,64,"%s_linked_mouse",key_name);
	     raine_set_config_int(section,other_name,InputList[link].mousebtn);
	   }
	   snprintf(other_name,64,"%s_linked_auto_rate",key_name);
	   raine_set_config_int(section,other_name,InputList[link].auto_rate);
	 }
      }
   }
#if 0
   /* Actually it's saved but they don't show in the inputs in the gui so
    * they can't be changed ! For now I just comment this out because it also
    * disables these inputs which is not convenient with the config mess in
    * neocd... */
   if (driver_nb_emu_inputs) {
     // custom emu keys...
     for(ta=0;ta<driver_nb_emu_inputs;ta++){
       sprintf(key_name,"%s",driver_emu_list[ta].name);
       no_spaces(key_name);
       raine_set_config_int(section,key_name,driver_emu_list[ta].scancode);
     }
     driver_nb_emu_inputs = 0;
     driver_emu_list = NULL;
   }
#endif
}

void load_default_keys(char *section)
{
   int ta;
   char key_name[64],other_name[64];

   use_custom_keys = 0;
   if (raine_get_config_int(section,"version",0) != 2)
       return;

   strncpy(analog_name,raine_get_config_string(section,"analog_name",""),40);
   analog_name[79] = 0;
   analog_num = -1;
   if (analog_name[0]) {
     int n;
     for (n=0; n<SDL_NumJoysticks(); n++) {
	 if (!strcmp(analog_name,joy[n].name)) {
	     analog_num = n;
	     break;
	 }
     }
     if (analog_num > -1) {
       analog_stick= raine_get_config_int(section,"analog_stick",0);
       analog_minx = raine_get_config_int(section,"analog_minx",0);
       analog_maxx = raine_get_config_int(section,"analog_maxx",0);
       analog_miny = raine_get_config_int(section,"analog_miny",0);
       analog_maxy = raine_get_config_int(section,"analog_maxy",0);
     }
   }

   for(ta=0;ta<KB_DEF_COUNT;ta++){
      sprintf(key_name,"%s",def_input[ta].name);
      no_spaces(key_name);
      def_input[ta].scancode = raine_get_config_int(section,key_name,def_input[ta].scancode);
      snprintf(other_name,64,"%s_joystick",key_name);
      def_input[ta].joycode = raine_get_config_int(section,other_name,def_input[ta].joycode);
      snprintf(other_name,64,"%s_mouse",key_name);
      def_input[ta].mousebtn = raine_get_config_int(section,other_name,def_input[ta].mousebtn);

   }

   for (ta=0; ta<MAX_LAYER_INFO; ta++) {
     sprintf(key_name,"key_layer_%d",ta);
     int code = SDLK_F5+ta;
     if (code == SDLK_F11) code = SDLK_F12; // skip F11, it's used by the fps
     if (code > SDLK_F12) code = SDLK_F12;
     layer_info_list[ta].keycode = raine_get_config_int(section,key_name,code);
   }
}

void save_default_keys(char *section)
{
   int ta;
   char key_name[64],other_name[64];

   raine_set_config_int(section,"version",2);
   if (analog_num >= 0) {
     raine_set_config_string(section,"analog_name",analog_name);
     raine_set_config_int(section,"analog_stick",analog_stick);
     raine_set_config_int(section,"analog_minx",analog_minx);
     raine_set_config_int(section,"analog_maxx",analog_maxx);
     raine_set_config_int(section,"analog_miny",analog_miny);
     raine_set_config_int(section,"analog_maxy",analog_maxy);
   }

   for(ta=0;ta<KB_DEF_COUNT;ta++){
      sprintf(key_name,"%s",def_input[ta].name);
      no_spaces(key_name);
      if (*key_name) {
	  raine_set_config_int(section,key_name,def_input[ta].scancode);
	  snprintf(other_name,64,"%s_joystick",key_name);
	  raine_set_config_int(section,other_name,def_input[ta].joycode);
	  snprintf(other_name,64,"%s_mouse",key_name);
	  raine_set_config_int(section,other_name,def_input[ta].mousebtn);
      }
   }

   for (ta=0; ta<MAX_LAYER_INFO; ta++) {
     sprintf(key_name,"key_layer_%d",ta);
     raine_set_config_int(section,key_name,layer_info_list[ta].keycode);
   }
}

int raine_get_emu_nb_ctrl() {
  return sizeof(def_input_emu)/sizeof(DEF_INPUT_EMU);
}

void load_emulator_keys(char *section) {
  int nb = raine_get_emu_nb_ctrl();
  load_emu_keys(section,def_input_emu,nb);
}

void register_driver_emu_keys(struct DEF_INPUT_EMU *list, int nb) {
  driver_emu_list = list;
  driver_nb_emu_inputs = nb;
}

void unregister_driver_emu_keys() {
    driver_nb_emu_inputs = 0;
    driver_emu_list = NULL;
}

void save_emulator_keys(char *section)
{
   int ta;
   char key_name[64];
   int nb = raine_get_emu_nb_ctrl();

   raine_set_config_int(section,"version",2);
   for(ta=0;ta<nb;ta++){
      sprintf(key_name,"%s",def_input_emu[ta].name);
      no_spaces(key_name);
      raine_set_config_int(section,key_name,def_input_emu[ta].scancode);
      strcat(key_name,"_kmod");
      raine_set_config_int(section,key_name,def_input_emu[ta].kmod);
   }
}

void load_emulator_joys(char *section)
{
    printf("load_emulator joys\n");
   int ta;
   char joy_name[64];
   int nb = raine_get_emu_nb_ctrl();

#if SDL == 2
   hat_for_moves = raine_get_config_int(section,"hat_for_moves", 1);
#endif
   for(ta=0;ta<nb;ta++){
      sprintf(joy_name,"%s",def_input_emu[ta].name);
      no_spaces(joy_name);
      def_input_emu[ta].joycode = raine_get_config_int(section,joy_name,def_input_emu[ta].joycode);
   }
}

void save_emulator_joys(char *section)
{
   int ta;
   char joy_name[64];
   int nb = raine_get_emu_nb_ctrl();

   for(ta=0;ta<nb;ta++){
      sprintf(joy_name,"%s",def_input_emu[ta].name);
      no_spaces(joy_name);
      raine_set_config_int(section,joy_name,def_input_emu[ta].joycode);
   }
#if SDL == 2
   for (ta=0; ta<nb_joy; ta++) {
       SDL_JoystickGUID guid;
       guid = SDL_JoystickGetDeviceGUID(ta);
       char guid_str[35];
       SDL_JoystickGetGUIDString(guid,guid_str,35);
       raine_set_config_int(section,guid_str,joy[ta].index);
   }
   raine_set_config_int(section,"hat_for_moves",hat_for_moves);
#endif
}

void update_rjoy_list(void)
{
}

static int pulse_time;

void set_pulse_time(int time)
{
  pulse_time = time;
}

static int find_input_from_keysym(int sym, int start) {
  int ta;
  for (ta=start; ta<InputCount; ta++) {
    if (InputList[ta].Key == sym) {
      return ta;
    }
  }


  return -1;
}

static int find_input_from_joy(int event, int start) {
  int ta;
  for (ta=start; ta<InputCount; ta++) {
    if (InputList[ta].Joy == event) {
      return ta;
    }
  }

  return -1;
}

static int find_input_from_mbtn(int btn, int start) {
  int ta;
  for (ta=start; ta<InputCount; ta++) {
    if (InputList[ta].mousebtn == btn) {
      return ta;
    }
  }


  return -1;
}


static int is_input_valid(int ta) {
  int    input_valid;

  // Increment active timer
  InputList[ta].active_time ++;

  // Assume Input is valid

  input_valid=1;

  // Toggle Autofire settings

  if(InputList[ta].auto_rate){
    if(autofire_timer[InputList[ta].auto_rate] >= InputList[ta].auto_rate) {
      return 0; // no need to go further
    }
  }

  // Disable the following situations:
  // 1) Impossible joystick inputs (joystick can be up or down, but not both)
  // 2) Coin inputs must last approx 250ms (prevent taito coin error)

  /* I am not sure stick_logic is very usefull, but if Antiriad bothered to
   * write this, then it probably means that some games insist of having this
   * logic (and anyway it's long to write, but fast to execute !) */
  switch(InputList[ta].default_key){
  case KB_DEF_P1_UP:
    stick_logic[0] |= 0x01;
    if((stick_logic[0]&0x02)) input_valid=0;
    break;
  case KB_DEF_P1_DOWN:
    stick_logic[0] |= 0x02;
    if((stick_logic[0]&0x01)) input_valid=0;
    break;
  case KB_DEF_P1_LEFT:
    stick_logic[0] |= 0x04;
    if((stick_logic[0]&0x08)) input_valid=0;
    break;
  case KB_DEF_P1_RIGHT:
    stick_logic[0] |= 0x08;
    if((stick_logic[0]&0x04)) input_valid=0;
    break;
  case KB_DEF_P2_UP:
    stick_logic[1] |= 0x01;
    if((stick_logic[1]&0x02)) input_valid=0;
    break;
  case KB_DEF_P2_DOWN:
    stick_logic[1] |= 0x02;
    if((stick_logic[1]&0x01)) input_valid=0;
    break;
  case KB_DEF_P2_LEFT:
    stick_logic[1] |= 0x04;
    if((stick_logic[1]&0x08)) input_valid=0;
    break;
  case KB_DEF_P2_RIGHT:
    stick_logic[1] |= 0x08;
    if((stick_logic[1]&0x04)) input_valid=0;
    break;
  case KB_DEF_P3_UP:
    stick_logic[2] |= 0x01;
    if((stick_logic[2]&0x02)) input_valid=0;
    break;
  case KB_DEF_P3_DOWN:
    stick_logic[2] |= 0x02;
    if((stick_logic[2]&0x01)) input_valid=0;
    break;
  case KB_DEF_P3_LEFT:
    stick_logic[2] |= 0x04;
    if((stick_logic[2]&0x08)) input_valid=0;
    break;
  case KB_DEF_P3_RIGHT:
    stick_logic[2] |= 0x08;
    if((stick_logic[2]&0x04)) input_valid=0;
    break;
  case KB_DEF_P4_UP:
    stick_logic[3] |= 0x01;
    if((stick_logic[3]&0x02)) input_valid=0;
    break;
  case KB_DEF_P4_DOWN:
    stick_logic[3] |= 0x02;
    if((stick_logic[3]&0x01)) input_valid=0;
    break;
  case KB_DEF_P4_LEFT:
    stick_logic[3] |= 0x04;
    if((stick_logic[3]&0x08)) input_valid=0;
    break;
  case KB_DEF_P4_RIGHT:
    stick_logic[3] |= 0x08;
    if((stick_logic[3]&0x04)) input_valid=0;
    break;
  case KB_DEF_COIN1:
  case KB_DEF_COIN2:
  case KB_DEF_COIN3:
  case KB_DEF_COIN4:
    if(InputList[ta].active_time > pulse_time) input_valid=0;
    break;
  default:
    break;
  }
  return input_valid;
}

static void key_up(int ta) {
  switch(InputList[ta].default_key){
  case KB_DEF_P1_UP:
    stick_logic[0] &= ~0x01;
    break;
  case KB_DEF_P1_DOWN:
    stick_logic[0] &= ~0x02;
    break;
  case KB_DEF_P1_LEFT:
    stick_logic[0] &= ~0x04;
    break;
  case KB_DEF_P1_RIGHT:
    stick_logic[0] &= ~0x08;
    break;
  case KB_DEF_P2_UP:
    stick_logic[1] &= ~0x01;
    break;
  case KB_DEF_P2_DOWN:
    stick_logic[1] &= ~0x02;
    break;
  case KB_DEF_P2_LEFT:
    stick_logic[1] &= ~0x04;
    break;
  case KB_DEF_P2_RIGHT:
    stick_logic[1] &= ~0x08;
    break;
  case KB_DEF_P3_UP:
    stick_logic[2] &= ~0x01;
    break;
  case KB_DEF_P3_DOWN:
    stick_logic[2] &= ~0x02;
    break;
  case KB_DEF_P3_LEFT:
    stick_logic[2] &= ~0x04;
    break;
  case KB_DEF_P3_RIGHT:
    stick_logic[2] &= ~0x08;
    break;
  case KB_DEF_P4_UP:
    stick_logic[3] &= ~0x01;
    break;
  case KB_DEF_P4_DOWN:
    stick_logic[3] &= ~0x02;
    break;
  case KB_DEF_P4_LEFT:
    stick_logic[3] &= ~0x04;
    break;
  case KB_DEF_P4_RIGHT:
    stick_logic[3] &= ~0x08;
    break;
  }
}

// update_inputs():
// Goes through the input list setting/clearing the mapped RAM[] bits

static void add_valid_input(int ta) {
    if (nb_valid_inputs == MAX_VALID_INPUTS) {
	fatal_error("Too many valid inputs");
    }
    int n;
    for (n=0; n<nb_valid_inputs; n++) {
	if (valid_inputs[n] == ta) {
	    print_debug("add_valid_input: already have %d\n",ta);
	    return;
	}
    }
    valid_inputs[nb_valid_inputs++] = ta;
}

static void remove_valid_input(int ta) {
  // remove an input from the list of valid input when the control is released
  int n;
  int updated = 0;
  for (n=0; n<nb_valid_inputs; n++) {
    if (valid_inputs[n] == ta) {
	updated = 1;
      if (n < nb_valid_inputs-1)
	memmove(&valid_inputs[n],&valid_inputs[n+1],(nb_valid_inputs-1-n)*sizeof(int));
      nb_valid_inputs--;
      break;
    }
  }
  InputList[ta].active_time = 0;
  if (!updated) {
      print_debug("remove_valid_input: didn't find %d\n",ta);
  }
}

static void add_joy_event(int event) {
  int ta = -1;
  /* We allow that 1 key is mapped to more than 1 control (loop)
   * it's because there are so many controls that you can very easily
   * map a key which is already used by another unused input and so
   * it's better to trigger them together in this case */
  if (reading_demo) return;
  do {
    ta = find_input_from_joy(event,ta+1);
    if (ta >= 0) {
      autofire_timer[InputList[ta].auto_rate] = 0;
      int input_valid = is_input_valid(ta);
      update_input_buffer(ta,input_valid);
      if (input_valid) {
	  add_valid_input(ta);
      }
    }
  } while (ta >= 0);
}

static void remove_joy_event(int event) {
  int ta = -1;
  if (reading_demo) return;
  if (event == def_input_emu[6].joycode) {
    // special case for the turbo key this one is a toggle
    def_input_emu[6].proc();
    return;
  }
  do {
    ta = find_input_from_joy(event,ta+1);
    if (ta >= 0) {
      key_up(ta);
      update_input_buffer(ta,0);
      remove_valid_input(ta);
    }
  } while (ta >= 0);
}

static void check_emu_joy_event(int jevent) {
  DEF_INPUT_EMU *emu = &def_input_emu[0];
  int nb = raine_get_emu_nb_ctrl();
  while (nb--) {
    if (emu->joycode == jevent) {
      emu->proc();
      break;
    }
    emu++;
  }
}

int get_axis_from_hat(int which, int hat) {
  return SDL_JoystickNumAxes(joy[which].joy) + (hat)*2;
}

static int check_layer_key(int input)
{
  int ta;
  int ret = 0;

  for(ta=0; ta<layer_info_count; ta++){

    if( layer_info_list[ta].keycode == input ){

      layer_info_list[ta].flip = 1;
      layer_info_list[ta].enabled ^= 1;
      print_ingame(60, "%s: %01d", layer_info_list[ta].name, layer_info_list[ta].enabled);
      ret = 1;
    }
  }
  return ret;
}

static int check_emu_inputs(DEF_INPUT_EMU *emu_input, int nb, int input, int modifier) {
  while (nb--) {
    if (emu_input->scancode == input) {
      int kmod = emu_input->kmod;
      if ((modifier == 0 && kmod == 0) ||
	  (modifier && (kmod & modifier) == (modifier & (KMOD_CTRL|KMOD_ALT|KMOD_SHIFT
#if SDL==2
							 |KMOD_GUI
#endif
							 )))) {
	emu_input->proc();
	return 1;
      }
    }
    emu_input++;
  }
  return 0;
}

static void handle_joy_axis(int which, int axis, int value) {
    int jevent = 0;
    if (value <= -16000 && joy[which].jstate.pos_axis[axis] > -1) {
	if (joy[which].jstate.pos_axis[axis] == 1) {
	    /* Special case for joysticks : sometimes they move so fast from one
	     * side to the other that we get nothing for the central position.
	     * In this case raine rejects the opposite control so we'd better
	     * avoid this here... */
	    remove_joy_event(get_joy_input(which,AXIS_RIGHT(axis),0,0));
	}
	joy[which].jstate.pos_axis[axis] = -1;
	add_joy_event((jevent = get_joy_input(which,AXIS_LEFT(axis),0,0)));
    } else if (value >= 16000 && joy[which].jstate.pos_axis[axis] < 1) {
	if (joy[which].jstate.pos_axis[axis] == -1)
	    remove_joy_event(get_joy_input(which,AXIS_LEFT(axis),0,0));
	joy[which].jstate.pos_axis[axis] = 1;
	add_joy_event((jevent = get_joy_input(which,AXIS_RIGHT(axis),0,0)));
    } else if ((joy[which].jstate.pos_axis[axis] == -1 && value >= -16000) ||
	    (joy[which].jstate.pos_axis[axis] == 1 && value <= 16000)) {
	/* With my choice to encode joystick events, there is no way to know
	 * if this is the end of a right or left movement. So I end both of
	 * them !
	 * It shouldn't matter all that much, this remains very fast */
	remove_joy_event(get_joy_input(which, AXIS_LEFT(axis),0,0));
	remove_joy_event(get_joy_input(which, AXIS_RIGHT(axis),0,0));
	joy[which].jstate.pos_axis[axis] = 0;
    }
    if (jevent)
	check_emu_joy_event(jevent);
}

#if SDL < 2
#define SDL_FIRSTEVENT 0
#endif

void control_handle_event(SDL_Event *event) {
  int input,which,axis,value,modifier,jevent,  ta;
  DEF_INPUT_EMU *emu_input;
  int input_valid,nb,hat,changed,btn;

  switch (event->type) {
#if SDL == 2
  case SDL_CONTROLLERBUTTONDOWN:
      which = get_joy_index_from_instance(event->cbutton.which);
      btn = event->cbutton.button;
      if (btn == SDL_CONTROLLER_BUTTON_DPAD_UP ||
	      btn == SDL_CONTROLLER_BUTTON_DPAD_DOWN ||
	      btn == SDL_CONTROLLER_BUTTON_DPAD_LEFT ||
	      btn == SDL_CONTROLLER_BUTTON_DPAD_RIGHT) {
	  joy[which].cancel_sticks = 1; // ignore all inputs coming from sticks if using the d-pad
      }
      if (hat_for_moves) {
	  switch (btn) {
	  case SDL_CONTROLLER_BUTTON_DPAD_UP:
	      handle_joy_axis(which,SDL_CONTROLLER_AXIS_LEFTY,-16000);
	      return;
	  case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
	      handle_joy_axis(which,SDL_CONTROLLER_AXIS_LEFTY,16000);
	      return;
	  case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
	      handle_joy_axis(which,SDL_CONTROLLER_AXIS_LEFTX,-16000);
	      return;
	  case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
	      handle_joy_axis(which,SDL_CONTROLLER_AXIS_LEFTX,16000);
	      return;
	  }
      }
      add_joy_event((jevent = get_joy_input(which,0,event->cbutton.button+1,0)));
      check_emu_joy_event(jevent);
      break;
  case SDL_CONTROLLERBUTTONUP:
      which = get_joy_index_from_instance(event->cbutton.which);
      if (hat_for_moves) {
	  switch (event->cbutton.button) {
	  case SDL_CONTROLLER_BUTTON_DPAD_UP:
	  case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
	      handle_joy_axis(which,SDL_CONTROLLER_AXIS_LEFTY,0);
	      return;
	  case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
	  case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
	      handle_joy_axis(which,SDL_CONTROLLER_AXIS_LEFTX,0);
	      return;
	  }
      }
      remove_joy_event(get_joy_input(which,0,event->cbutton.button+1,0));
      break;
  case SDL_CONTROLLERDEVICEADDED:
      {
	  int n = event->cdevice.which;
	  printf("Game controller device %d added.\n", n);
	  if (joy[n].controller || joy[n].joy) {
	      printf("Already have controller %d\n",n);
	  } else
	      add_game_controller(n);
      }
      break;

  case SDL_CONTROLLERDEVICEREMOVED:
      {
	  int n = event->cdevice.which;
	  printf("bye controller %d\n",n);
	  del_controller(n);
      }
      break;
    case SDL_CONTROLLERAXISMOTION:
      which = get_joy_index_from_instance(event->caxis.which);
      axis = event->caxis.axis;
      value = event->caxis.value;
      if (joy[which].cancel_sticks) {
	  if (abs(value) < 16000) {
	      event->type = SDL_FIRSTEVENT; // for the gui, just cancel the event
	      return;
	  }
	  joy[which].cancel_sticks = 0; // back to work
      }
      handle_joy_axis(which,axis,value);
      break;
#endif
    case SDL_KEYDOWN:
      if (event->key.repeat) break; // ignore repeating keys while in the emulation !!!
      input = event->key.keysym.scancode; // | ((event->key.keysym.mod & 0x4fc0)<<16);
      key[input & 0x1ff] = 1;
      if (!reading_demo) {
	  ta = -1;
	  /* We allow that 1 key is mapped to more than 1 control (loop)
	   * it's because there are so many controls that you can very easily
	   * map a key which is already used by another unused input and so
	   * it's better to trigger them together in this case */
	  do {
	      ta = find_input_from_keysym(input,ta+1);
	      if (ta >= 0) {
		  autofire_timer[InputList[ta].auto_rate] = 0;
		  input_valid = is_input_valid(ta);
		  if (input_valid) {
		      update_input_buffer(ta,input_valid);
		      add_valid_input(ta);
		  }
	      }
	  } while (ta >= 0);
      }

      // Now check the gui inputs, the logic is slightly different since
      // we check for the keysym + modifiers here

      // keep only shift, ctrl, alt, gui...
      modifier = (event->key.keysym.mod & 0xfc3);
      emu_input = &def_input_emu[0];
      nb = raine_get_emu_nb_ctrl();
      int handled=0;
      if (!check_emu_inputs(emu_input,nb,input,modifier) && driver_nb_emu_inputs)
	  handled = check_emu_inputs(driver_emu_list,driver_nb_emu_inputs,input,modifier);
      if (!handled)
	handled = check_layer_key(input);
      break;
    case SDL_KEYUP:
      input = event->key.keysym.scancode; //  | ((event->key.keysym.mod & 0x4fc0)<<16);
      key[input & 0x1ff] = 0;
      if (reading_demo) break;
      ta = -1;
      if (input == def_input_emu[6].scancode) {
	// special case for the turbo key this one is a toggle
	def_input_emu[6].proc();
	break;
      }

      do {
	ta = find_input_from_keysym(input,ta+1);
	if (ta >= 0) {
	  key_up(ta);
	  update_input_buffer(ta,0);
	  remove_valid_input(ta);

	  /* Now a particular case :
	   * sometimes when pressing rapidly 2 keys, we receive the 2nd keydown
	   * message before the key up for the 1st key. It can be critical in some
	   * action games !
	   * So the workaround : once a key has gone up, check the other keys
	   * to see if another input wouldn't become valid now ! */
	  int x;

	  for(x=0;x<InputCount;x++){
	    if(key[InputList[x].Key & 0x1ff]){
	      int found_valid = 0;
	      int n;
	      for (n=0; n<nb_valid_inputs; n++) {
		if (valid_inputs[n] == x) {
		  found_valid = 1;
		  break;
		}
	      }
	      if (!found_valid) {
		input_valid = is_input_valid(x);
		if (input_valid) {
		  // We found a key down which now trigers a valid input !
		  update_input_buffer(x,input_valid);
		  add_valid_input(x);
		}
	      }
	    }
	  } // for
	} // if ta >= 0
      } while (ta >= 0);
      // printf("key up %d stick logic %d\n",ta,stick_logic[0]);

      break;
    case SDL_MOUSEMOTION:
      // The cpu_frame_count test is because for some reason we get a SDL_MOUSEMOTION message when launching the emulation
      // if we want to keep the default values it's best to filter it out like that
      if (reading_demo || cpu_frame_count < 3) break;
      mickey_x = event->motion.xrel;
      mickey_y = event->motion.yrel;
      mickey_x_scaled += mickey_x * mouse_scale;
      mickey_y_scaled += mickey_y * mouse_scale;
      mouse_x += mickey_x_scaled;
      if (mouse_x > max_x) mouse_x = max_x;
      else if (mouse_x < min_x) mouse_x = min_x;
      mouse_y += mickey_y_scaled;
      if (mouse_y > max_y) mouse_y = max_y;
      else if (mouse_y < min_y) mouse_y = min_y;
      break;
    case SDL_MOUSEBUTTONDOWN:
      if (reading_demo) break;
      ta = -1;
      do {
	  ta = find_input_from_mbtn(event->button.button,ta+1);
	  if (ta >= 0) {
	      autofire_timer[InputList[ta].auto_rate] = 0;
	      input_valid = is_input_valid(ta);
	      update_input_buffer(ta,input_valid);
	      if (input_valid)
		  add_valid_input(ta);
	  }
      } while (ta >= 0);
      // apparently there is no need to update mouse_b since the loop just below sends the events as if the equivalent key was pressed...
      // mouse_b |= event->button.button;
      break;
    case SDL_MOUSEBUTTONUP:
      if (reading_demo) break;
      ta = -1;
      do {
	  ta = find_input_from_mbtn(event->button.button,ta+1);
	  if (ta >= 0) {
	      update_input_buffer(ta,0);
	      remove_valid_input(ta);
	  }
      } while (ta >= 0);
      mouse_b &= ~event->button.button;
      break;
#if SDL==1
    case SDL_VIDEORESIZE:
      resize(1);
      SetupScreenBitmap();
      init_video_core();
      reset_ingame_timer();
#else
    case SDL_WINDOWEVENT:
      if (event->window.event == SDL_WINDOWEVENT_RESIZED || event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
	  display_cfg.prev_sx = display_cfg.screen_x;
	  display_cfg.prev_sy = display_cfg.screen_y;
	  resize(1,event->window.data1,event->window.data2);
      } else if (event->window.event == SDL_WINDOWEVENT_MOVED) {
	  if (!display_cfg.maximized) {
	      display_cfg.prev_posx = display_cfg.posx;
	      display_cfg.prev_posy = display_cfg.posy;
	      display_cfg.posx = event->window.data1;
	      display_cfg.posy = event->window.data2;
	  }
      } else if (event->window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
	  display_cfg.lost_focus = 0;
      else if (event->window.event == SDL_WINDOWEVENT_FOCUS_LOST)
	  display_cfg.lost_focus = 1;
      if (event->window.event == SDL_WINDOWEVENT_MAXIMIZED) {
	  display_cfg.maximized = 1;
      } else if (event->window.event == SDL_WINDOWEVENT_RESTORED) {
	  if (!display_cfg.lost_focus) {
	      SDL_SetWindowPosition(win,display_cfg.prev_posx,display_cfg.prev_posy);
	      SDL_SetWindowSize(win,display_cfg.prev_sx,display_cfg.prev_sy);
	      display_cfg.posx = display_cfg.prev_posx;
	      display_cfg.posy = display_cfg.prev_posy;
	      display_cfg.screen_x = display_cfg.prev_sx;
	      display_cfg.screen_y = display_cfg.prev_sy;
	      display_cfg.maximized = 0;
	  }
      }
#endif
      break;
#ifdef USE_BEZELS
    case SDL_VIDEOEXPOSE:
      display_bezel();
      break;
#endif
    case SDL_JOYHATMOTION:
      // Ok here we remap automatically hats to joysticks events on axes which don't exist...
      which = get_joy_index_from_instance(event->jhat.which);
      if (is_game_controller(which)) {
	  return;
      }
      hat = event->jhat.hat;
      value = event->jhat.value;
      changed = joy[which].jstate.hat[hat] & (~value);
      // changed contains the bitmask of the axis which need to be centered !

      if (changed & SDL_HAT_LEFT)
	remove_joy_event(get_joy_input(which, 0,0,HAT(hat,SDL_HAT_LEFT)));
      if (changed & SDL_HAT_RIGHT)
	remove_joy_event(get_joy_input(which, 0,0,HAT(hat,SDL_HAT_RIGHT)));
      if (changed & SDL_HAT_UP)
	remove_joy_event(get_joy_input(which, 0,0,HAT(hat,SDL_HAT_UP)));
      if (changed & SDL_HAT_DOWN)
	remove_joy_event(get_joy_input(which, 0,0,HAT(hat,SDL_HAT_DOWN)));

      joy[which].jstate.hat[hat] = event->jhat.value;
      add_joy_event(get_joy_input(which,0,0,HAT(hat,value)));
      break;
    case SDL_JOYAXISMOTION:
      which = get_joy_index_from_instance(event->jaxis.which);
      if (is_game_controller(which)) {
	  event->type = SDL_FIRSTEVENT;
	  return;
      }
      axis = event->jaxis.axis;
      value = event->jaxis.value;
      if (joy[which].cancel_sticks) {
	  if (abs(value) < 16000) {
	      event->type = SDL_FIRSTEVENT; // for the gui, just cancel the event
	      return;
	  }
	  joy[which].cancel_sticks = 0; // back to work
      }
      if (which >= MAX_JOY || axis >= MAX_AXIS) {
	return;
      } else if (which == analog_num+1 && axis/2 == analog_stick) {
	// Normalized position for analog input...
	if (axis == analog_stick*2) {
	  if (value < 0) {
	    if (value < analog_minx) {
	      analog_minx = value;
	      analog_normx = -16384;
	    } else
	      analog_normx = value*-16384/analog_minx;
	  } else if (value > 0) {
	    if (value > analog_maxx) {
	      analog_maxx = value;
	      analog_normx = 16384;
	    } else
	      analog_normx = value*16384/analog_maxx;
	  }
	} else { // vertical axis
	  if (value < 0) {
	    if (value < analog_miny) {
	      analog_miny = value;
	      analog_normy = -16384;
	    } else
	      analog_normy = value*-16384/analog_miny;
	  } else if (value > 0) {
	    if (value > analog_maxy) {
	      analog_maxy = value;
	      analog_normy = 16384;
	    } else
	      analog_normy = value*16384/analog_maxy;
	  }
	}
      }
      handle_joy_axis(which, axis, value);
      break;
    case SDL_JOYBUTTONDOWN:
      which = get_joy_index_from_instance(event->jaxis.which);
      if (is_game_controller(which)) {
	  return;
      }
      add_joy_event((jevent = get_joy_input(which,0,event->jbutton.button+1,0)));
      check_emu_joy_event(jevent);
      break;
    case SDL_JOYBUTTONUP:
      which = get_joy_index_from_instance(event->jaxis.which);
      if (is_game_controller(which)) {
	  return;
      }
      remove_joy_event(get_joy_input(which,0,event->jbutton.button+1,0));
      break;
    case SDL_QUIT:
      exit(0);
#if SDL==1
    case SDL_ACTIVEEVENT:
      display_cfg.lost_focus = 0;
      break;
#endif
  }
}

void update_inputs(void)
{
  SDL_Event event;

  int    ta,n;
  int    input_valid;

  if (reading_demo) {
    write_demo_inputs();
    // And we don't return, need to handle inputs to get out of the demo !
  }

  // in case nothing moves, reset the mouse mickeys to 0 !
  mickey_x = mickey_y = 0;
  mickey_x_scaled -= trunc(mickey_x_scaled);
  mickey_y_scaled -= trunc(mickey_y_scaled);

  /* update valid inputs : this is not an option, it allows auto fire to work
   * and certain games insist on some inputs to be valid only if they last
   * only for a certain amount of time (coins in spf2t for example) */
  if (nb_valid_inputs) {
    // Autofire timer emulation

    for(ta=1;ta<6;ta++){
      autofire_timer[ta] ++;
      if(autofire_timer[ta] >= (ta<<1))
				autofire_timer[ta] = 0;
    }
    for (n=0; n<nb_valid_inputs; n++) {
      input_valid = is_input_valid(valid_inputs[n]);
      update_input_buffer(valid_inputs[n],input_valid);
      /* Notice : even if an input becomes invalid in this loop, it can be
       * removed from this list/array only by releasing the corresponding
       * input, otherwise the auto fire wouldn't work */
    }
  }

  while (SDL_PollEvent(&event)) {
    control_handle_event(&event);
  }
  if (! raine_cfg.req_pause_game && pause_on_focus && display_cfg.lost_focus) {
      // lost input -> go to pause
      raine_cfg.req_pause_game = 1;
  }
  if (recording_demo)
    save_demo_inputs();
}

/******************************************************************************/
/*                                                                            */
/*                              GLOBAL STRINGS                                */
/*                                                                            */
/******************************************************************************/

char MSG_COIN1[]        = _("Coin A");
char MSG_COIN2[]        = _("Coin B");
char MSG_COIN3[]        = _("Coin C");
char MSG_COIN4[]        = _("Coin D");

char MSG_TILT[]         = _("Tilt");
char MSG_SERVICE[]      = _("Service");
char MSG_TEST[]         = _("Test");
char MSG_UNKNOWN[]      = _("Unknown");
char MSG_YES[] = _("Yes");
char MSG_NO[] = _("No");
char MSG_FREE_PLAY[] = _("Free Play");
char MSG_UNUSED[] = _("Unused");
char MSG_COINAGE[] = _("Coinage");

char MSG_P1_START[]     = _("Player1 Start");

char MSG_P1_UP[]        = _("Player1 Up");
char MSG_P1_DOWN[]      = _("Player1 Down");
char MSG_P1_LEFT[]      = _("Player1 Left");
char MSG_P1_RIGHT[]     = _("Player1 Right");

char MSG_P1_B1[]        = _("Player1 Button1");
char MSG_P1_B2[]        = _("Player1 Button2");
char MSG_P1_B3[]        = _("Player1 Button3");
char MSG_P1_B4[]        = _("Player1 Button4");
char MSG_P1_B5[]        = _("Player1 Button5");
char MSG_P1_B6[]        = _("Player1 Button6");
char MSG_P1_B7[]        = _("Player1 Button7");
char MSG_P1_B8[]        = _("Player1 Button8");
char MSG_P1_AB[]        = _("Player1 A+B");
char MSG_P1_B1B2[]      = _("Player1 B1+B2");
char MSG_P1_B1B2B3[]    = _("Player1 B1+B2+B3");
char MSG_P1_B4B5B6[]    = _("Player1 B4+B5+B6");
char MSG_P3_B1B2[]      = _("Player3 B1+B2");
char MSG_P4_B1B2[]      = _("Player4 B1+B2");

char MSG_P2_START[]     = _("Player2 Start");

char MSG_P2_UP[]        = _("Player2 Up");
char MSG_P2_DOWN[]      = _("Player2 Down");
char MSG_P2_LEFT[]      = _("Player2 Left");
char MSG_P2_RIGHT[]     = _("Player2 Right");

char MSG_P2_B1[]        = _("Player2 Button1");
char MSG_P2_B2[]        = _("Player2 Button2");
char MSG_P2_B3[]        = _("Player2 Button3");
char MSG_P2_B4[]        = _("Player2 Button4");
char MSG_P2_B5[]        = _("Player2 Button5");
char MSG_P2_B6[]        = _("Player2 Button6");
char MSG_P2_B7[]        = _("Player2 Button7");
char MSG_P2_B8[]        = _("Player2 Button8");

char MSG_P2_B1B2[]      = _("Player2 B1+B2");
char MSG_P2_B1B2B3[]    = _("Player2 B1+B2+B3");
char MSG_P2_AB[]        = _("Player2 A+B");

char MSG_P3_START[]     = _("Player3 Start");

char MSG_P3_UP[]        = _("Player3 Up");
char MSG_P3_DOWN[]      = _("Player3 Down");
char MSG_P3_LEFT[]      = _("Player3 Left");
char MSG_P3_RIGHT[]     = _("Player3 Right");

char MSG_P3_B1[]        = _("Player3 Button1");
char MSG_P3_B2[]        = _("Player3 Button2");
char MSG_P3_B3[]        = _("Player3 Button3");
char MSG_P3_B4[]        = _("Player3 Button4");
char MSG_P3_B5[]        = _("Player3 Button5");
char MSG_P3_B6[]        = _("Player3 Button6");
char MSG_P3_B7[]        = _("Player3 Button7");
char MSG_P3_B8[]        = _("Player3 Button8");

char MSG_P4_START[]     = _("Player4 Start");

char MSG_P4_UP[]        = _("Player4 Up");
char MSG_P4_DOWN[]      = _("Player4 Down");
char MSG_P4_LEFT[]      = _("Player4 Left");
char MSG_P4_RIGHT[]     = _("Player4 Right");

char MSG_P4_B1[]        = _("Player4 Button1");
char MSG_P4_B2[]        = _("Player4 Button2");
char MSG_P4_B3[]        = _("Player4 Button3");
char MSG_P4_B4[]        = _("Player4 Button4");
char MSG_P4_B5[]        = _("Player4 Button5");
char MSG_P4_B6[]        = _("Player4 Button6");
char MSG_P4_B7[]        = _("Player4 Button7");
char MSG_P4_B8[]        = _("Player4 Button8");

char MSG_FLIPPER_1_L[]  = _("Flipper 1 Left");
char MSG_FLIPPER_1_R[]  = _("Flipper 1 Right");
char MSG_FLIPPER_2_L[]  = _("Flipper 2 Left");
char MSG_FLIPPER_2_R[]  = _("Flipper 2 Right");
char MSG_TILT_L[]       = _("Tilt Left");
char MSG_TILT_R[]       = _("Tilt Right");
char MSG_B1_L[]         = _("Button 1 Left");
char MSG_B1_R[]         = _("Button 1 Right");
char MSG_B2_L[]         = _("Button 2 Left");
char MSG_B2_R[]         = _("Button 2 Right");

char MSG_P1_A[]         = _("P1 A");    // Mahjong controls[]; at least in mahjong quest...
char MSG_P1_E[]         = _("P1 E");
char MSG_P1_I[]         = _("P1 I");
char MSG_P1_M[]         = _("P1 M");
char MSG_P1_KAN[]       = _("P1 Kan");

char MSG_P1_B[]         = _("P1 B");
char MSG_P1_F[]         = _("P1 F");
char MSG_P1_J[]         = _("P1 J");
char MSG_P1_N[]         = _("P1 N");
char MSG_P1_REACH[]     = _("P1 Reach");

char MSG_P1_C[]         = _("P1 C");
char MSG_P1_G[]         = _("P1 G");
char MSG_P1_K[]         = _("P1 K");
char MSG_P1_CHI[]       = _("P1 Chi");
char MSG_P1_RON[]       = _("P1 Ron");

char MSG_P1_D[]         = _("P1 D");
char MSG_P1_H[]         = _("P1 H");
char MSG_P1_L[]         = _("P1 L");
char MSG_P1_PON[]       = _("P1 Pon");

char *MSG_P1_SERVICE_A  = _("Service A");
char *MSG_P1_SERVICE_B  = _("Service B");
char *MSG_P1_SERVICE_C  = _("Service C");

/* DSW SECTION */

char MSG_DSWA_BIT1[]    = _("DSW A Bit 1");        // Since most dsw info sheets
char MSG_DSWA_BIT2[]    = _("DSW A Bit 2");        // number the bits 1-8, we will
char MSG_DSWA_BIT3[]    = _("DSW A Bit 3");        // too, although 0-7 is a more
char MSG_DSWA_BIT4[]    = _("DSW A Bit 4");        // correct syntax for progammers.
char MSG_DSWA_BIT5[]    = _("DSW A Bit 5");
char MSG_DSWA_BIT6[]    = _("DSW A Bit 6");
char MSG_DSWA_BIT7[]    = _("DSW A Bit 7");
char MSG_DSWA_BIT8[]    = _("DSW A Bit 8");

char MSG_DSWB_BIT1[]    = _("DSW B Bit 1");
char MSG_DSWB_BIT2[]    = _("DSW B Bit 2");
char MSG_DSWB_BIT3[]    = _("DSW B Bit 3");
char MSG_DSWB_BIT4[]    = _("DSW B Bit 4");
char MSG_DSWB_BIT5[]    = _("DSW B Bit 5");
char MSG_DSWB_BIT6[]    = _("DSW B Bit 6");
char MSG_DSWB_BIT7[]    = _("DSW B Bit 7");
char MSG_DSWB_BIT8[]    = _("DSW B Bit 8");

char MSG_DSWC_BIT1[]    = _("DSW C Bit 1");
char MSG_DSWC_BIT2[]    = _("DSW C Bit 2");
char MSG_DSWC_BIT3[]    = _("DSW C Bit 3");
char MSG_DSWC_BIT4[]    = _("DSW C Bit 4");
char MSG_DSWC_BIT5[]    = _("DSW C Bit 5");
char MSG_DSWC_BIT6[]    = _("DSW C Bit 6");
char MSG_DSWC_BIT7[]    = _("DSW C Bit 7");
char MSG_DSWC_BIT8[]    = _("DSW C Bit 8");

char MSG_COIN_SLOTS[]   = _("Coin Slots");

char MSG_1COIN_1PLAY[]  = _("1 Coin/1 Credit");
char MSG_1COIN_2PLAY[]  = _("1 Coin/2 Credits");
char MSG_1COIN_3PLAY[]  = _("1 Coin/3 Credits");
char MSG_1COIN_4PLAY[]  = _("1 Coin/4 Credits");
char MSG_1COIN_5PLAY[]  = _("1 Coin/5 Credits");
char MSG_1COIN_6PLAY[]  = _("1 Coin/6 Credits");
char MSG_1COIN_7PLAY[]  = _("1 Coin/7 Credits");
char MSG_1COIN_8PLAY[]  = _("1 Coin/8 Credits");
char MSG_1COIN_9PLAY[]  = _("1 Coin/9 Credits");

char MSG_2COIN_1PLAY[]  = _("2 Coins/1 Credit");
char MSG_2COIN_2PLAY[]  = _("2 Coins/2 Credits");
char MSG_2COIN_3PLAY[]  = _("2 Coins/3 Credits");
char MSG_2COIN_4PLAY[]  = _("2 Coins/4 Credits");
char MSG_2COIN_5PLAY[]  = _("2 Coins/5 Credits");
char MSG_2COIN_6PLAY[]  = _("2 Coins/6 Credits");
char MSG_2COIN_7PLAY[]  = _("2 Coins/7 Credits");
char MSG_2COIN_8PLAY[]  = _("2 Coins/8 Credits");

char MSG_3COIN_1PLAY[]  = _("3 Coins/1 Credit");
char MSG_3COIN_2PLAY[]  = _("3 Coins/2 Credits");
char MSG_3COIN_3PLAY[]  = _("3 Coins/3 Credits");
char MSG_3COIN_4PLAY[]  = _("3 Coins/4 Credits");
char MSG_3COIN_5PLAY[]  = _("3 Coins/5 Credits");
char MSG_3COIN_6PLAY[]  = _("3 Coins/6 Credits");
char MSG_3COIN_7PLAY[]  = _("3 Coins/7 Credits");
char MSG_3COIN_8PLAY[]  = _("3 Coins/8 Credits");

char MSG_4COIN_1PLAY[]  = _("4 Coins/1 Credit");
char MSG_4COIN_2PLAY[]  = _("4 Coins/2 Credits");
char MSG_4COIN_3PLAY[]  = _("4 Coins/3 Credits");
char MSG_4COIN_4PLAY[]  = _("4 Coins/4 Credits");
char MSG_4COIN_5PLAY[]  = _("4 Coins/5 Credits");
char MSG_4COIN_6PLAY[]  = _("4 Coins/6 Credits");
char MSG_4COIN_7PLAY[]  = _("4 Coins/7 Credits");
char MSG_4COIN_8PLAY[]  = _("4 Coins/8 Credits");

char MSG_5COIN_1PLAY[]  = _("5 Coins/1 Credit");
char MSG_5COIN_2PLAY[]  = _("5 Coins/2 Credits");
char MSG_5COIN_3PLAY[]  = _("5 Coins/3 Credits");
char MSG_5COIN_4PLAY[]  = _("5 Coins/4 Credits");

char MSG_6COIN_1PLAY[]  = _("6 Coins/1 Credit");
char MSG_6COIN_2PLAY[]  = _("6 Coins/2 Credits");
char MSG_6COIN_3PLAY[]  = _("6 Coins/3 Credits");
char MSG_6COIN_4PLAY[]  = _("6 Coins/4 Credits");

char MSG_7COIN_1PLAY[]  = _("7 Coins/1 Credit");
char MSG_7COIN_2PLAY[]  = _("7 Coins/2 Credits");
char MSG_7COIN_3PLAY[]  = _("7 Coins/3 Credits");
char MSG_7COIN_4PLAY[]  = _("7 Coins/4 Credits");

char MSG_8COIN_1PLAY[]  = _("8 Coins/1 Credit");
char MSG_8COIN_2PLAY[]  = _("8 Coins/2 Credits");
char MSG_8COIN_3PLAY[]  = _("8 Coins/3 Credits");
char MSG_8COIN_4PLAY[]  = _("8 Coins/4 Credits");

char MSG_9COIN_1PLAY[] = _("9 Coins/1 Credit");

char MSG_OFF[]          = _("Off");
char MSG_ON[]           = _("On");

char MSG_SCREEN[]       = _("Flip Screen");
char MSG_NORMAL[]       = _("Normal");
char MSG_INVERT[]       = _("Invert");

char MSG_TEST_MODE[]    = _("Test Mode");

char MSG_DEMO_SOUND[]   = _("Demo Sound");

char MSG_CONTINUE_PLAY[]= _("Continue Play");
char MSG_EXTRA_LIFE[]   = _("Extra Life");
char MSG_LIVES[]        = _("Lives");

char MSG_CHEAT[]        = _("Cheat");

char MSG_DIFFICULTY[]   = _("Difficulty");
char MSG_EASY[]         = _("Easy");
char MSG_HARD[]         = _("Hard");
char MSG_VERY_HARD[]         = _("Very Hard");
char MSG_HARDEST[]      = _("Hardest");
char MSG_MEDIUM[]       = _("Medium");

char MSG_CABINET[]      = _("Cabinet");
char MSG_UPRIGHT[]      = _("Upright");
char MSG_TABLE[]        = _("Table");
char MSG_ALT[]        = _("Alternate");

/******************************************************************************/

void inputs_preinit() {
  SDL_Event event;
  int handled;
#if SDL == 2
  int n;
  FILE *f = fopen(get_shared("gamecontrollerdb.txt"),"r");
  if (f) {
      fclose(f);
      int ret = SDL_GameControllerAddMappingsFromFile(get_shared("gamecontrollerdb.txt"));
      printf("mappings added %d\n",ret);
  }
  f = fopen(get_shared("config/userdb.txt"),"r");
  if (f) {
      fclose(f);
      int ret = SDL_GameControllerAddMappingsFromFile(get_shared("config/userdb.txt"));
      printf("mappings added %d from userdb.txt\n",ret);
  }

  for (n=0; n<SDL_NumJoysticks(); n++) {
      add_game_controller(n);
  }
#endif

  // Some peripherals like a certain microsoft keyboard is recognized as a
  // joystick when pluged in usb, and they send a few faulty events at start
  // this loop should get rid of them...
  int ticks = SDL_GetTicks();
  event.type = 0;
  while (!SDL_PollEvent(&event) && SDL_GetTicks()-ticks < 100);
  if (event.type) SDL_PushEvent(&event);
  do {
      handled = 0;
      if (SDL_PollEvent(&event)) {
	  switch(event.type) {
	  case SDL_JOYAXISMOTION:
	  case SDL_JOYBALLMOTION:
	  case SDL_JOYHATMOTION:
	  case SDL_JOYBUTTONDOWN:
	  case SDL_JOYBUTTONUP:
	      handled = 1;
	      break;
	  }
      }
  } while (handled);
}

void inputs_done() {
  int n;
  for (n=0; n<SDL_NumJoysticks(); n++)
    if (joy[n].joy) {
	SDL_JoystickClose(joy[n].joy);
	// free(joy[n].name);
    }
#if SDL == 2
    else if (joy[n].controller)
	SDL_GameControllerClose(joy[n].controller);
#endif
}

