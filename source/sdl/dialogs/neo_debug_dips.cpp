/* In fact this file doesn't do anything related to dialogs, but since it's
 * called from a dialog...
 * All it does is just assign the right dsw to the current game's dsw and
 * then call the standard dsw dialog */

#include "raine.h"
#include "games.h"
#include "control.h"
#include "neocd/neocd.h"
#include "starhelp.h"
#include "sdl/dialogs/dlg_dsw.h"

static struct DSW_DATA def_0[] =
{
    DSW_ON_OFF("high 2-1",1,0),
    DSW_ON_OFF("high 2-2",2,0),
    DSW_ON_OFF("high 2-3",4,0),
    DSW_ON_OFF("high 2-4",8,0),
    DSW_ON_OFF("high 2-5",16,0),
    DSW_ON_OFF("high 2-6",32,0),
    DSW_ON_OFF("high 2-7",64,0),
    DSW_ON_OFF("high 2-8",128,0),
  { NULL, 0, 0}
};

static struct DSW_DATA def_1[] =
{
    DSW_ON_OFF("low 1-1",1,0),
    DSW_ON_OFF("low 1-2",2,0),
    DSW_ON_OFF("low 1-3",4,0),
    DSW_ON_OFF("low 1-4",8,0),
    DSW_ON_OFF("low 1-5",16,0),
    DSW_ON_OFF("low 1-6",32,0),
    DSW_ON_OFF("low 1-7",64,0),
    DSW_ON_OFF("low 1-8",128,0),
  { NULL, 0, 0}
};

// The following info comes from
// http://www.neo-geo.com/wiki/index.php?title=Neo-Geo_Big_List_of_Debug_Dipswitches

static struct DSW_DATA dsw_3countb_1[] =
{
    DSW_ON_OFF("Pal Mode/Job Meter",1,0),
    DSW_ON_OFF("Debug Text on screen",2,0),
    DSW_ON_OFF("Gives game link (assembly date) + extra text info",4,0),
    DSW_ON_OFF("Target boxes ingame",8,0),
    DSW_ON_OFF("Massive amount of textual info over game",16,0),
    DSW_ON_OFF("Sprite data on screen",32,0),
    DSW_ON_OFF("Font data on screen",64,0),
    DSW_ON_OFF("Possible sprite viewer...",128,0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_3countb_2[] =
{
    DSW_ON_OFF("More debug text",1,0),
    DSW_ON_OFF("debug text",2,0),
    DSW_ON_OFF("debug text",4,0),
    DSW_ON_OFF("Disables P1 buttons?",8,0),
    DSW_ON_OFF("unknown",16,0),
    DSW_ON_OFF("Dont Touch Me + Safety Mode appear on screen",32,0),
    DSW_ON_OFF("?",64,0),
    DSW_ON_OFF("Enables pause with select button",128,0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_aof_1[] =
{
    DSW_ON_OFF("Player 1 controls CPU",1,0),
    DSW_ON_OFF("Same as above",2,0),
    DSW_ON_OFF("Stat boxes",4,0),
    DSW_ON_OFF("Play during 'How to Play' screen",8,0),
    DSW_ON_OFF("Vs. mode collision detection boxes (press Start)",16,0),
    DSW_ON_OFF("Both players are invincible",32,0),
    DSW_ON_OFF("Unknown",64,0),
    DSW_ON_OFF("Press Start during fight to skip to next fight",128,0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_aof_2[] =
{
    DSW_ON_OFF("Slow mode",1,0),
    DSW_ON_OFF("Slower mode (combine with prior code for super slow)",2,0),
    DSW_ON_OFF("Clean paused game screen",4,0),
    DSW_ON_OFF("Press Select for RGB test palette",8,0),
    DSW_ON_OFF("Unknown",16,0),
    DSW_ON_OFF("Unknown",32,0),
    DSW_ON_OFF("Stop timer",64,0),
    DSW_ON_OFF("Pal Mode/Job Meter",128,0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_aof2_1[] =
{
    DSW_ON_OFF("P1 is CPU (play Geese if you load Special Stage)",1,0),
    DSW_ON_OFF("P2 is cpu",2,0),
    DSW_ON_OFF("Press select for data display",4,0),
    DSW_ON_OFF("P2 No KO (also play behind how to play screen)",8,0),
    DSW_ON_OFF("unknown",16,0),
    DSW_ON_OFF("invincible cpu",32,0),
    DSW_ON_OFF("numeric display for each player ?",64,0),
    DSW_ON_OFF("unknown",128,0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_aof2_2[] =
{
    DSW_ON_OFF("Slow mode",1,0),
    DSW_ON_OFF("Slower mode (combine with prior code for super slow)",2,0),
    DSW_ON_OFF("Pause without pause display",4,0),
    DSW_ON_OFF("Press Select for sound test",8,0),
    DSW_ON_OFF("Press Select for RGB test",12,0),
    DSW_ON_OFF("Unknown",16,0),
    DSW_ON_OFF("Skip to ending",32,0),
    DSW_ON_OFF("Stop timer",64,0),
    DSW_ON_OFF("Pal Mode/Job Meter",128,0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_aof3_1[] =
{
    DSW_ON_OFF("1-1 Unknown",1,0),
    DSW_ON_OFF("P1 & P2 take no damage",2,0),
    DSW_ON_OFF("1-3 Unknown",4,0),
    DSW_ON_OFF("1-4 Unknown",8,0),
    DSW_ON_OFF("1-5 Unknown",16,0),
    DSW_ON_OFF("Practice mode? Must be on before match starts",32,0),
    DSW_ON_OFF("1-7 Unknown",64,0),
    DSW_ON_OFF("Unlock Sinclair and Wyler",128,0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_aof3_2[] =
{
    DSW_ON_OFF("Player 1 is CPU controlled?",1,0),
    DSW_ON_OFF("Player 2 is CPU controlled? (if 2-8 is on, P1 is CPU instead. P2 doesn't move)",2,0),
    DSW_ON_OFF("Displays 'W!' at the top of the screen",4,0),
    DSW_ON_OFF("Players are invincible",8,0),
    DSW_ON_OFF("2-5 Unknown",16,0),
    DSW_ON_OFF("2-6 Unknown",32,0),
    DSW_ON_OFF("'Random Mode' displayed",64,0),
    DSW_ON_OFF("2-8 Total Round Display",128, 0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_alpham2_1[] =
{
    DSW_ON_OFF("Debug display + in console mode press pause, select level then unpause",1,0),
  { NULL, 0, 0}
};

static struct DSW_INFO dsw_debug[] =
{
  { 0x1, 0, def_0 },
  { 0x0, 0, def_1 },
  { 0, 0, NULL }
};

int do_debug_dsw(int sel) {
    if (!current_game) return 0;
    if (current_game->load_game != &load_neocd) return 0;
    if (is_current_game("3countb")) {
	dsw_debug[1].data = dsw_3countb_1;
	dsw_debug[0].data = dsw_3countb_2;
    } else if (is_current_game("aof")) {
	dsw_debug[1].data = dsw_aof_1;
	dsw_debug[0].data = dsw_aof_2;
    } else if (is_current_game("aof2")) {
	dsw_debug[1].data = dsw_aof2_1;
	dsw_debug[0].data = dsw_aof2_2;
    } else if (is_current_game("aof3")) {
	dsw_debug[1].data = dsw_aof3_1;
	dsw_debug[0].data = dsw_aof3_2;
    } else if (is_current_game("alpham2")) {
	dsw_debug[1].data = dsw_alpham2_1;
    }
    UINT8 *ram = (is_neocd() ? RAM : ROM);
    int base = ReadLongSc(&ram[0x10e]);
    if (dsw_debug[0].data) dsw_debug[0].offset = 11;
    /* factory_setting : remarkably, the dsw are write-only in ram, they
     * read their default value, and then can be changed only with the dsw
     * dialog, not from the ram. So there is nothing to read from the ram, and
     * here we must assign the factory_setting from what there is in ram... */
    if (is_neocd())
	dsw_debug[1].factory_setting = ram[base ^ 1];
    else
	dsw_debug[1].factory_setting = RAM[(base ^ 1) & 0xffff];
    dsw_debug[1].offset = 10;
    if (is_neocd())
	dsw_debug[0].factory_setting = ram[(base+1) ^ 1];
    else
	dsw_debug[0].factory_setting = RAM[((base + 1) ^ 1) & 0xffff];
    printf("init dsw %x %x\n",dsw_debug[0].factory_setting,dsw_debug[1].factory_setting);
    const DSW_INFO *old = current_game->dsw;
    current_game->dsw = NULL;
    current_game->dsw = dsw_debug;
    init_dsw();
    do_dlg_dsw(0);
    current_game->dsw = old;
    init_dsw();
    printf("au final, debug dsw %x %x, go to %x\n",input_buffer[10],input_buffer[11],base);
    if (!is_neocd()) base &= 0xffff;
    RAM[base ^ 1] = input_buffer[10];
    RAM[(base+1) ^ 1] = input_buffer[11];
    return 0;
}
