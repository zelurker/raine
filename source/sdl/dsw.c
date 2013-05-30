/******************************************************************************/
/*                                                                            */
/*                              DIPSWITCH SUPPORT                             */
/*                                                                            */
/******************************************************************************/

#include "raine.h"
#include "games.h"
#include "dsw.h"
#include "control.h"
#include "arpro.h"
#include "starhelp.h"
#include "conf-cpu.h"
#ifdef HAVE_6502
#include "6502/m6502hlp.h"
#endif
#include "gui/menu.h"
#include "dialogs/dlg_dsw.h"

menu_item_t dsw_items[MAX_DSW_SETTINGS];

s_dsw dipswitch[MAX_DIPSWITCHES];
struct ROMSW LanguageSw;	// ROMSwitch for Language Selection (Taito roms)

int dsw_mask[MAX_DSW_SETTINGS], dsw_bitset[MAX_DSW_SETTINGS];

void write_dsw(int index)
{
  UINT32 address = dipswitch[index].address;
  UINT32 data = dipswitch[index].value;

  // Find the mask for this dsw, because some games use the same byte for dsw
  // and inputs, so we must be sure not to erase the inputs with sdl !!!
  // (such as daioh or thunderl)
  int n,mask = 0;
  for (n=dipswitch[index].start; n<=dipswitch[index].end; n++)
    mask |= dsw_mask[n];

  if(address < 0x100)

    input_buffer[address] = (input_buffer[address] & (~mask)) |data;

  else

    RAM[address] = (RAM[address] & (~mask)) |data;
}

void make_dipswitch_bytes(void)
{
   UINT32 i;

   for(i=0;i<MAX_DIPSWITCHES; i++){

     if (dipswitch[i].address) {
       write_dsw(i);
       print_debug("DSW %d: %02x\n",i,dipswitch[i].value);
     }
   }
}

// RestoreDSWDefault():
// Restores DSW to the factory settings

void RestoreDSWDefault(void)
{
   int i;

   for(i=0;i<MAX_DIPSWITCHES;i++)
      dipswitch[i].value = dipswitch[i].def;
}

static void my_make_dipswitch_statlist(int reset) {
  DSW_DATA *dsw_data;
  const DSW_INFO *dsw_src;
  int index,tb,tc, start = 0;

  dsw_src = current_game->dsw;

  if(dsw_src){

    index=0;

    while(dsw_src[index].data){

      dsw_data = dsw_src[index].data;

      dipswitch[index].def       = dsw_src[index].factory_setting;
      dipswitch[index].address   = dsw_src[index].offset;
      dipswitch[index].start = start;
      if (reset)
	dipswitch[index].value     = dsw_src[index].factory_setting;

      tb = 0;

      while(dsw_data[tb].name){

	dsw_items[start].label = dsw_data[tb].name;
	dsw_mask[start]    = dsw_data[tb].bit_mask;
	dsw_items[start].values_list_size  = dsw_data[tb].count;
	dsw_bitset[start] = dipswitch[index].value & dsw_mask[start];
	dsw_items[start].value_int = &dsw_bitset[start];

	for (tc=1; tc<=dsw_data[tb].count; tc++) {
	  dsw_items[start].values_list[tc-1] = dsw_data[tb+tc].bit_mask;
	  dsw_items[start].values_list_label[tc-1] = dsw_data[tb+tc].name;
	}
	tb += tc;
	start++;
      }

      dipswitch[index].end     = start-1;
      write_dsw(index);
      index++;

    }

  }
  dsw_items[start].label = NULL;
}

void make_dipswitch_statlist() {
  /* External entry point : no reset, it's just usefull for 1 driver actually
   * and it would probably be possible to do without this function at all */
  my_make_dipswitch_statlist(0);
}

void init_dsw(void)
{
  int ta;

   for(ta = 0; ta < MAX_DIPSWITCHES; ta++){

      dipswitch[ta].address   = 0;
   }

  my_make_dipswitch_statlist(1);

}

struct DSW_DATA dsw_data_default_0[] =
{
   { MSG_DSWA_BIT1,           0x01, 0x02 },
   { MSG_OFF,                 0x01, 0x00 },
   { MSG_ON,                  0x00, 0x00 },
   { MSG_DSWA_BIT2,           0x02, 0x02 },
   { MSG_OFF,                 0x02, 0x00 },
   { MSG_ON,                  0x00, 0x00 },
   { MSG_DSWA_BIT3,           0x04, 0x02 },
   { MSG_OFF,                 0x04, 0x00 },
   { MSG_ON,                  0x00, 0x00 },
   { MSG_DSWA_BIT4,           0x08, 0x02 },
   { MSG_OFF,                 0x08, 0x00 },
   { MSG_ON,                  0x00, 0x00 },
   { MSG_DSWA_BIT5,           0x10, 0x02 },
   { MSG_OFF,                 0x10, 0x00 },
   { MSG_ON,                  0x00, 0x00 },
   { MSG_DSWA_BIT6,           0x20, 0x02 },
   { MSG_OFF,                 0x20, 0x00 },
   { MSG_ON,                  0x00, 0x00 },
   { MSG_DSWA_BIT7,           0x40, 0x02 },
   { MSG_OFF,                 0x40, 0x00 },
   { MSG_ON,                  0x00, 0x00 },
   { MSG_DSWA_BIT8,           0x80, 0x02 },
   { MSG_OFF,                 0x80, 0x00 },
   { MSG_ON,                  0x00, 0x00 },
   { NULL,                    0,    0,   },
};

struct DSW_DATA dsw_data_default_1[] =
{
   { MSG_DSWB_BIT1,           0x01, 0x02 },
   { MSG_OFF,                 0x01, 0x00 },
   { MSG_ON,                  0x00, 0x00 },
   { MSG_DSWB_BIT2,           0x02, 0x02 },
   { MSG_OFF,                 0x02, 0x00 },
   { MSG_ON,                  0x00, 0x00 },
   { MSG_DSWB_BIT3,           0x04, 0x02 },
   { MSG_OFF,                 0x04, 0x00 },
   { MSG_ON,                  0x00, 0x00 },
   { MSG_DSWB_BIT4,           0x08, 0x02 },
   { MSG_OFF,                 0x08, 0x00 },
   { MSG_ON,                  0x00, 0x00 },
   { MSG_DSWB_BIT5,           0x10, 0x02 },
   { MSG_OFF,                 0x10, 0x00 },
   { MSG_ON,                  0x00, 0x00 },
   { MSG_DSWB_BIT6,           0x20, 0x02 },
   { MSG_OFF,                 0x20, 0x00 },
   { MSG_ON,                  0x00, 0x00 },
   { MSG_DSWB_BIT7,           0x40, 0x02 },
   { MSG_OFF,                 0x40, 0x00 },
   { MSG_ON,                  0x00, 0x00 },
   { MSG_DSWB_BIT8,           0x80, 0x02 },
   { MSG_OFF,                 0x80, 0x00 },
   { MSG_ON,                  0x00, 0x00 },
   { NULL,                    0,    0,   },
};

struct DSW_DATA dsw_data_default_2[] =
{
   { MSG_DSWC_BIT1,           0x01, 0x02 },
   { MSG_OFF,                 0x01, 0x00 },
   { MSG_ON,                  0x00, 0x00 },
   { MSG_DSWC_BIT2,           0x02, 0x02 },
   { MSG_OFF,                 0x02, 0x00 },
   { MSG_ON,                  0x00, 0x00 },
   { MSG_DSWC_BIT3,           0x04, 0x02 },
   { MSG_OFF,                 0x04, 0x00 },
   { MSG_ON,                  0x00, 0x00 },
   { MSG_DSWC_BIT4,           0x08, 0x02 },
   { MSG_OFF,                 0x08, 0x00 },
   { MSG_ON,                  0x00, 0x00 },
   { MSG_DSWC_BIT5,           0x10, 0x02 },
   { MSG_OFF,                 0x10, 0x00 },
   { MSG_ON,                  0x00, 0x00 },
   { MSG_DSWC_BIT6,           0x20, 0x02 },
   { MSG_OFF,                 0x20, 0x00 },
   { MSG_ON,                  0x00, 0x00 },
   { MSG_DSWC_BIT7,           0x40, 0x02 },
   { MSG_OFF,                 0x40, 0x00 },
   { MSG_ON,                  0x00, 0x00 },
   { MSG_DSWC_BIT8,           0x80, 0x02 },
   { MSG_OFF,                 0x80, 0x00 },
   { MSG_ON,                  0x00, 0x00 },
   { NULL,                    0,    0,   },
};


UINT8 get_dsw(int i)
{
   return dipswitch[i].value;
}

static char *save_name[MAX_DIPSWITCHES] =
{
  "dswa", "dswb", "dswc", "dswd", "dswe", "dswf", "dswg", "dswh", "dswi",
  "dswj"
};

void load_dipswitches(char *section)
{
   int i;

   for(i = 0; i < MAX_DIPSWITCHES; i++)
      if(dipswitch[i].address) {
         dipswitch[i].value = raine_get_config_hex(section, save_name[i], dipswitch[i].value);
      }

   my_make_dipswitch_statlist(0);
}

void save_dipswitches(char *section)
{
   int i;

   for(i = 0; i < MAX_DIPSWITCHES; i++)
      if(dipswitch[i].address) {
         raine_set_config_hex(section, save_name[i], dipswitch[i].value);
      }
}

/******************************************************************************/
/*                                                                            */
/*         ROM SWITCH SUPPORT (consider them unofficial dip switches)         */
/*                                                                            */
/******************************************************************************/

/*

todo:

- multiple romswitch support : required for sfzch
   you are allowed to specify multiple offsets in the ROMSW_INFO struct
   but this address will get the same data from the ROMSW_DATA struct.
- multiple byte patchin support (also not required)
- remove array limit on number of rom switch states (better code)

*/

extern UINT8 read_z80_from_rom(UINT32 address);
extern void write_z80_to_rom(UINT32 address, UINT8 data);

static void gen_cpu_write_byte_rom(UINT32 address, UINT8 data)
{
  gen_cpu_write_byte(address,data);
}

static UINT8 gen_cpu_read_byte_rom(UINT32 address)
{
  return gen_cpu_read_byte(address);
}

void init_romsw(void)
{
  const ROMSW_INFO *romsw_src;
  ROMSW_DATA *romsw_data;
  int ta,tb;

  LanguageSw.Address = 0;
  LanguageSw.Count   = 0;

  ta=0;

  romsw_src = current_game->romsw;

  if(romsw_src){

    while(romsw_src[ta].data){

      romsw_data = romsw_src[ta].data;

      LanguageSw.Address      = romsw_src[ta].offset;
      LanguageSw.def          = romsw_src[ta].factory_setting;

      tb = 0;

      while(romsw_data[tb].name){

	LanguageSw.Mode[tb] = romsw_data[tb].name;
	LanguageSw.Data[tb] = romsw_data[tb].data;
	tb++;

      }

      LanguageSw.Count    = tb;
      gen_cpu_write_byte_rom(LanguageSw.Address,LanguageSw.def);

      ta++;

    }
  }
}

void SetupLanguageSwitch(UINT32 addr)
{
   LanguageSw.Address=addr;
   LanguageSw.Count=0;
}

void AddLanguageSwitch(UINT8 ldata, char *lname)
{
   LanguageSw.Mode[LanguageSw.Count]=lname;
   LanguageSw.Data[LanguageSw.Count]=ldata;
   LanguageSw.Count++;
}

void SetLanguageSwitch(int number)
{
  const ROMSW_INFO *romsw_src;
  int ta;

  romsw_src = current_game->romsw;
  ta = 0;
  if(romsw_src){

    while(romsw_src[ta].data){

      LanguageSw.Address      = romsw_src[ta++].offset;
      gen_cpu_write_byte_rom(LanguageSw.Address,LanguageSw.Data[number]);
    }
  }
}

int GetLanguageSwitch(void)
{
   int ta,tb;

   if(LanguageSw.Address){

      tb = gen_cpu_read_byte_rom(LanguageSw.Address);

      for(ta=0;ta<LanguageSw.Count;ta++){
          if(LanguageSw.Data[ta]==tb)
             return ta;
      }

   }
   return 0;
}

void load_romswitches(char *section)
{
   if(LanguageSw.Address)
      SetLanguageSwitch( raine_get_config_hex(section,"Version",GetLanguageSwitch()) );
}

void save_romswitches(char *section)
{
   if(LanguageSw.Address)
      raine_set_config_hex(section,"Version",GetLanguageSwitch());
}

/******************************************************************************/
/*                                                                            */
/*                ROM PATCHING SUPPORT (something like an .ips)               */
/*                                                                            */
/******************************************************************************/

void patch_rom(UINT8 *src, ROM_PATCH *patch)
{
   int ta;

   ta = 0;

   while(patch[ta].offset != -1){

      WriteLong68k(&src[patch[ta].offset + 0], patch[ta].data_0);
      WriteLong68k(&src[patch[ta].offset + 4], patch[ta].data_1);

      ta++;
   };

}

/******************************************************************************/

