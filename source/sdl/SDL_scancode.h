/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2021 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

// It's simply the scancodes definition extracted from sdl2 for sdl1 !
// and edited because the scancodes returned by sdl-1.2 do not match at all the codes defined for sdl2 !

/**
 *  \file SDL_scancode.h
 *
 *  Defines keyboard scancodes.
 */

#ifndef SDL_scancode_h_
#define SDL_scancode_h_

/**
 *  \brief The SDL keyboard scancode representation.
 *
 *  Values of this type are used to represent keyboard keys, among other places
 *  in the \link SDL_Keysym::scancode key.keysym.scancode \endlink field of the
 *  SDL_Event structure.
 *
 *  The values in this enumeration are based on the USB usage page standard:
 *  https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf
 */
typedef enum
{
    SDL_SCANCODE_UNKNOWN = 0,

    /**
     *  \name Usage page 0x07
     *
     *  These values are from usage page 0x07 (USB keyboard page).
     */
    /* @{ */

    SDL_SCANCODE_ESCAPE = 9,

    SDL_SCANCODE_1 = 10,
    SDL_SCANCODE_2 = 11,
    SDL_SCANCODE_3 = 12,
    SDL_SCANCODE_4 = 13,
    SDL_SCANCODE_5 = 14,
    SDL_SCANCODE_6 = 15,
    SDL_SCANCODE_7 = 16,
    SDL_SCANCODE_8 = 17,
    SDL_SCANCODE_9 = 18,
    SDL_SCANCODE_0 = 19,
    SDL_SCANCODE_MINUS = 20,
    SDL_SCANCODE_EQUALS = 21,
    SDL_SCANCODE_BACKSPACE = 22,

    SDL_SCANCODE_TAB = 23,
    SDL_SCANCODE_Q = 24,
    SDL_SCANCODE_W = 25,
    SDL_SCANCODE_E = 26,
    SDL_SCANCODE_R = 27,
    SDL_SCANCODE_T = 28,
    SDL_SCANCODE_Y = 29,
    SDL_SCANCODE_U = 30,
    SDL_SCANCODE_I = 31,
    SDL_SCANCODE_O = 32,
    SDL_SCANCODE_P = 33,
    SDL_SCANCODE_LEFTBRACKET = 34,
    SDL_SCANCODE_RIGHTBRACKET = 35,
    SDL_SCANCODE_RETURN = 36,

    SDL_SCANCODE_LCTRL = 37,

    SDL_SCANCODE_A = 38,
    SDL_SCANCODE_S = 39,
    SDL_SCANCODE_D = 40,
    SDL_SCANCODE_F = 41,
    SDL_SCANCODE_G = 42,
    SDL_SCANCODE_H = 43,
    SDL_SCANCODE_J = 44,
    SDL_SCANCODE_K = 45,
    SDL_SCANCODE_L = 46,
    SDL_SCANCODE_SEMICOLON = 47,
    SDL_SCANCODE_APOSTROPHE = 48,
    SDL_SCANCODE_GRAVE = 49,

    SDL_SCANCODE_LSHIFT = 50,
    SDL_SCANCODE_BACKSLASH = 51,
    SDL_SCANCODE_Z = 52,
    SDL_SCANCODE_X = 53,
    SDL_SCANCODE_C = 54,
    SDL_SCANCODE_V = 55,
    SDL_SCANCODE_B = 56,
    SDL_SCANCODE_N = 57,
    SDL_SCANCODE_M = 58,
    SDL_SCANCODE_COMMA = 59,
    SDL_SCANCODE_PERIOD = 60,
    SDL_SCANCODE_SLASH = 61,

    SDL_SCANCODE_RSHIFT = 62,
    SDL_SCANCODE_LALT = 64,
    SDL_SCANCODE_SPACE = 65,
    SDL_SCANCODE_CAPSLOCK = 66,

    SDL_SCANCODE_F1 = 67,
    SDL_SCANCODE_F2 = 68,
    SDL_SCANCODE_F3 = 69,
    SDL_SCANCODE_F4 = 70,
    SDL_SCANCODE_F5 = 71,
    SDL_SCANCODE_F6 = 72,
    SDL_SCANCODE_F7 = 73,
    SDL_SCANCODE_F8 = 74,
    SDL_SCANCODE_F9 = 75,
    SDL_SCANCODE_F10 = 76,
    SDL_SCANCODE_F11 = 77,
    SDL_SCANCODE_F12 = 78,

    SDL_SCANCODE_KP_7 = 79,
    SDL_SCANCODE_KP_8 = 80,
    SDL_SCANCODE_KP_9 = 81,
    SDL_SCANCODE_KP_MINUS = 82,
    SDL_SCANCODE_KP_4 = 83,
    SDL_SCANCODE_KP_5 = 84,
    SDL_SCANCODE_KP_6 = 85,
    SDL_SCANCODE_KP_PLUS = 86,
    SDL_SCANCODE_KP_1 = 87,
    SDL_SCANCODE_KP_2 = 88,
    SDL_SCANCODE_KP_3 = 89,
    SDL_SCANCODE_KP_0 = 90,
    SDL_SCANCODE_KP_PERIOD = 91,
    SDL_SCANCODE_KP_ENTER = 104,

    SDL_SCANCODE_RCTRL = 105,
    SDL_SCANCODE_RALT = 108,

    SDL_SCANCODE_HOME = 110,

    SDL_SCANCODE_UP = 111,
    SDL_SCANCODE_PAGEUP = 112,
    SDL_SCANCODE_LEFT = 113,
    SDL_SCANCODE_RIGHT = 114,
    SDL_SCANCODE_END = 115,
    SDL_SCANCODE_DOWN = 116,
    SDL_SCANCODE_PAGEDOWN = 117,

    SDL_SCANCODE_INSERT = 118,
    SDL_SCANCODE_DELETE = 119,

    SDL_SCANCODE_KP_LGUI  = 133,
    SDL_SCANCODE_MENU  = 135,

    /* Add any other keys here. */

    SDL_NUM_SCANCODES = 512 /**< not a key, just marks the number of scancodes
                                 for array bounds */
} SDL_Scancode;

#endif /* SDL_scancode_h_ */

/* vi: set ts=4 sw=4 expandtab: */
