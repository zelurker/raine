/******************************************************************************/
/*                                                                            */
/*                  PSYCHIC FORCE (C) 1991 TAITO CORPORATION                  */
/*                                                                            */
/******************************************************************************/

#include "gameinc.h"
#include "psyforce.h"
#include "taitosnd.h"
#include "sasound.h"		// sample support routines
#include "blit.h" // clear_game_screen

static struct DIR_INFO psychic_force_dirs[] =
{
   { "psychic_force", },
   { "psyforce", },
   { NULL, },
};

static struct ROM_INFO psychic_force_roms[] =
{
   {       "e22-01", 0x00200000, 0x808b8340, 0, 0, 0, },
   {       "e22-06", 0x00020000, 0x739af589, 0, 0, 0, },
   {           NULL,          0,          0, 0, 0, 0, },
};

static struct INPUT_INFO psychic_force_inputs[] =
{
   { KB_DEF_P1_UP,        MSG_P1_UP,               0x000000, 0x01, BIT_ACTIVE_0 },
   { KB_DEF_P1_DOWN,      MSG_P1_DOWN,             0x000000, 0x02, BIT_ACTIVE_0 },
   { KB_DEF_P1_LEFT,      MSG_P1_LEFT,             0x000000, 0x04, BIT_ACTIVE_0 },
   { KB_DEF_P1_RIGHT,     MSG_P1_RIGHT,            0x000000, 0x08, BIT_ACTIVE_0 },
   { KB_DEF_P1_B1,        MSG_P1_B1,               0x000000, 0x10, BIT_ACTIVE_0 },

   { 0,                   NULL,                    0,        0,    0            },
};


static struct VIDEO_INFO psychic_force_video =
{
   draw_psychic_force,
   320,
   224,
   32,
   VIDEO_ROTATE_NORMAL,
};

GAME( psychic_force ,
   psychic_force_dirs,
   psychic_force_roms,
   psychic_force_inputs,
   NULL,
   NULL,

   load_psychic_force,
   clear_psychic_force,
   &psychic_force_video,
   execute_psychic_force_frame,
   "psyforce",
   "Psychic Force (audio board)",
   NULL,
   COMPANY_ID_TAITO,
   "E22",
   1990,
   taito_ym2610_sound,
      GAME_MISC
);

void load_psychic_force(void)
{
   if(!(RAM=AllocateMem(0x020000))) return;
   RAMSize=0x20000;

   Z80ROM=RAM+0x00000;
   if(!load_rom("e22-06", Z80ROM, 0x20000)) return;			// Z80 SOUND ROM

   if(!(PCMROM=AllocateMem(0x200000))) return;
   if(!load_rom("e22-01", PCMROM, 0x200000)) return;	// ADPCM A rom
   YM2610SetBuffers(NULL, PCMROM+0x000000, 0, 0x200000);

   AddTaitoYM2610(0x0198, 0x0168, 0x20000);
}

void clear_psychic_force(void)
{
   RemoveTaitoYM2610();

   #ifdef RAINE_DEBUG
      save_debug("RAM.bin",RAM,0x020000,1);
   #endif
}

static UINT32 command;

void execute_psychic_force_frame(void)
{
   static UINT32 flip_0;
   static UINT32 flip_1;
   static UINT32 flip_2;
   static UINT32 flip_3;
   static UINT32 flip_4;

   if(!(input_buffer[0] & 1)){

      if(!flip_0)

         command = (command + 0x10) & 0xFF;

      flip_0 = 1;
   }
   else

      flip_0 = 0;


   if(!(input_buffer[0] & 2)){

      if(!flip_1)

         command = (command - 0x10) & 0xFF;

      flip_1 = 1;
   }
   else

      flip_1 = 0;


   if(!(input_buffer[0] & 4)){

      if(!flip_2)

         command = (command - 0x01) & 0xFF;

      flip_2 = 1;
   }
   else

      flip_2 = 0;


   if(!(input_buffer[0] & 8)){

      if(!flip_3)

         command = (command + 0x01) & 0xFF;

      flip_3 = 1;
   }
   else

      flip_3 = 0;


   if(!(input_buffer[0] & 16)){

      if(!flip_4){

         tc0140syt_write_main_68k(0,0);
         tc0140syt_write_main_68k(2,(command >> 0) & 0x0F);
         tc0140syt_write_main_68k(2,(command >> 4) & 0x0F);

      }

      flip_4 = 1;
   }
   else

      flip_4 = 0;

   Taito2610_Frame();			// Z80 and YM2610
}

void draw_psychic_force(void)
{
   clear_game_screen(0);

   clear_ingame_message_list();
   print_ingame(900,"PSYCHIC FORCE AUDIO BOARD");
   print_ingame(900,"-------------------------");
   print_ingame(900," ");
   print_ingame(900,"Sound Byte:%02x",command);
   print_ingame(900," ");
   print_ingame(900,"Joystick: Select Byte");
   print_ingame(900,"Button: Send Command");
}
