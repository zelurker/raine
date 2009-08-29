#include "raine.h"
#include "alpha.h"
#include "emudx.h"

/* Alpha blending support - mmx */
/* This module just handles global variables required by the mmx functions */

static UINT32 alpha;

/* The CAPITALS for the global variables are kept because they were like that
   in the original code and I'd like to change it as little as possible */

#ifdef DARWIN
#undef RAINE_UNIX
#endif

UINT64 ALPHA64, ALPHABY4, COLORKEY64;

UINT64 MASKRED = 0xF800F800F800F800;
UINT64 MASKGREEN = 0x07E007E007E007E0;
UINT64 MASKBLUE = 0x001F001F001F001F;

UINT64 ADD64 = 0x0040004000400040;

/* May Microsoft be cursed for the eternity with its stupid _ everywhere !!! */

void init_alpha(UINT32 my_alpha) {
  alpha = my_alpha;

  asm(
#ifdef RAINE_UNIX
      "movd alpha,%mm2    \n" // Copy ALPHA into %mm2
#else
      "movd _alpha,%mm2    \n" // Copy ALPHA into %mm2
#endif
      "punpcklwd %mm2,%mm2 \n" // Unpack %mm2 - 0000 0000 00aa 00aa
      "punpckldq %mm2,%mm2 \n" // Unpack %mm2 - 00aa 00aa 00aa 00aa
#ifdef RAINE_UNIX
      "movq %mm2,ALPHA64  \n" // Save the result into ALPHA64
#else
      "movq %mm2,_ALPHA64  \n" // Save the result into ALPHA64
#endif
      "psrlw  $2,%mm2      \n" // Divide each ALPHA value by 4
#ifdef RAINE_UNIX
      "movq %mm2,ALPHABY4 \n" // Save the result to ALPHABY4
#else
      "movq %mm2,_ALPHABY4 \n" // Save the result to ALPHABY4
#endif

#ifdef RAINE_UNIX
      "movd emudx_transp,%mm4 \n" // Copy ColorKey into %mm4
#else
      "movd _emudx_transp,%mm4 \n" // Copy ColorKey into %mm4
#endif
      "punpcklwd %mm4,%mm4 \n" // Unpack %mm4 - 0000 0000 cccc cccc
      "punpckldq %mm4,%mm4 \n" // Unpack %mm4 - cccc cccc cccc cccc
#ifdef RAINE_UNIX
      "movq %mm4,COLORKEY64 \n" // Save the result into COLORKEY64
#else
      "movq %mm4,_COLORKEY64 \n" // Save the result into COLORKEY64
#endif
      "finit \n"
      );
}
