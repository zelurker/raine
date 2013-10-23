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

static struct DSW_DATA dsw_breakers_1[] =
{
    DSW_ON_OFF("Debug Menu (req dev mode, coin, p1 start)",1,0),
    DSW_ON_OFF("Screen Scroll Debug",2,0),
    DSW_ON_OFF("Kakuto Char No. Disp",4,0),
    DSW_ON_OFF("Kakuto Trigger Free (rapid fire)",8,0),
    DSW_ON_OFF("Kakuto Sound Test (not sure how it works)",16,0),
    DSW_ON_OFF("Obj. Status Debug (not sure how it works)",32,0),
    DSW_ON_OFF("1-7 Unknown",64,0),
    DSW_ON_OFF("Task Lsp. Weight Chk",128,0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_breakers_2[] =
{
    DSW_ON_OFF("for CPU Algo Debug (character data displayed on screen)",1,0),
    DSW_ON_OFF("Char Select mode (hold ABCD, then hit down to scroll)",2,0),
    DSW_ON_OFF("Center and Size Display (little ring appears to mark player position)",4,0),
    DSW_ON_OFF("Hit Area Display",8,0),
    DSW_ON_OFF("Ougi (pow) Max (unlimited pow)",16,0),
    DSW_ON_OFF("No Limit Battle (on time/health limits)",32,0),
    DSW_ON_OFF("Pal Sim (Europe only)",64,0),
    DSW_ON_OFF("Stop Motion (hit select to pause, start to unpause)",128, 0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_doubledr_2[] =
{
    DSW_ON_OFF("Slow motion",1,0),
    DSW_ON_OFF("No damage taken for either character",2,0),
    DSW_ON_OFF("Full charge for both players",4,0),
    DSW_ON_OFF("2-4 Unknown",8,0),
    DSW_ON_OFF("2-5 Unknown",16,0),
    DSW_ON_OFF("2-6 Unknown",32,0),
    DSW_ON_OFF("2-7 Unknown",64,0),
    DSW_ON_OFF("2-8 Unknown",128, 0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_fatfury2_1[] =
{
    DSW_ON_OFF("1-1 Unknown",1,0),
    DSW_ON_OFF("1-2 Unknown",2,0),
    DSW_ON_OFF("CPU vs CPU",4,0),
    DSW_ON_OFF("1-4 unknown",8,0),
    DSW_ON_OFF("Alternate character selection screen (enable at title screen)",16,0),
    DSW_ON_OFF("View characters ending",32,0),
    DSW_ON_OFF("1-7 unknown",64,0),
    DSW_ON_OFF("1-8 unknown",128,0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_fatfury2_2[] =
{
    DSW_ON_OFF("Invincible mode",1,0),
    DSW_ON_OFF("2-2 Unknown",2,0),
    DSW_ON_OFF("2-3 Unknown",4,0),
    DSW_ON_OFF("2-4 Unknown",8,0),
    DSW_ON_OFF("Press A+B+C+D to reduce opponents energy",16,0),
    DSW_ON_OFF("2-6 Unknown",32,0),
    DSW_ON_OFF("2-7 Unknown",64,0),
    DSW_ON_OFF("2-8 Unknown",128, 0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_fatfursp_1[] =
{
    DSW_ON_OFF("1-1 Pal Mode/Job Meter",1,0),
    DSW_ON_OFF("1-2 Both players are invincible",2,0),
    DSW_ON_OFF("1-3 Unlimited time, Ryo Sakazaki Selectable",4,0),
    DSW_ON_OFF("1-4 Unlocks Ryo Sakazaki",8,0),
    DSW_ON_OFF("1-5 Allows you to view selected character's endings",16,0),
    DSW_ON_OFF("1-6 Allows you to goto Dream Match vs Ryo Sakazaki",32,0),
    DSW_ON_OFF("1-7 Message screen, Use controller one to adjust.",64,0),
    DSW_ON_OFF("1-8 Sprite viewer, Press 'Select' on P1, Use P2 to view",128,0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_fatfursp_2[] =
{
    DSW_ON_OFF("2-1 Displays hit detection boxes",1,0),
    DSW_ON_OFF("2-2 Displays distance grid boxes",2,0),
    DSW_ON_OFF("2-3 Unknown",4,0),
    DSW_ON_OFF("2-4 Unknown",8,0),
    DSW_ON_OFF("2-5 Press A,B,C,& D at the same to to drop CPU's lifebar to near death",16,0),
    DSW_ON_OFF("2-6 Sprite location information",32,0),
    DSW_ON_OFF("2-7 Unknown",64,0),
    DSW_ON_OFF("2-8 Stage selection screen",128, 0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_fatfury3_1[] =
{
    DSW_ON_OFF("1-1 Pal Mode/Job Meter",1,0),
    DSW_ON_OFF("1-2 Both players are invincible",2,0),
    DSW_ON_OFF("1-3 Unlimited time; secret character selectable",4,0),
    DSW_ON_OFF("1-4 Set before character selection screen; unlock Ryo; CPU opponent",8,0),
    DSW_ON_OFF("1-5 View selected vs. character's ending",16,0),
    DSW_ON_OFF("1-6 Take selected character to dream match",32,0),
    DSW_ON_OFF("1-7 Message screen; use controller one",64,0),
    DSW_ON_OFF("1-8 Sprite viewer; press Select on controller one, use controller two to",128,0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_fatfury3_2[] =
{
    DSW_ON_OFF("2-1 Sprite character grids",1,0),
    DSW_ON_OFF("2-2 Sprite distance grids",2,0),
    DSW_ON_OFF("2-3 Show ending after selection of character",4,0),
    DSW_ON_OFF("2-4 Unknown",8,0),
    DSW_ON_OFF("2-5 Press A + B + C + D to set opponent's life to minimum + All characters",16,0),
    DSW_ON_OFF("2-6 Sprite location information",32,0),
    DSW_ON_OFF("2-7 Debug Object display",64,0),
    DSW_ON_OFF("2-8 Stage selection screen",128, 0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_ironclad_1[] =
{
    DSW_ON_OFF("1-1 Stage Select (must be enabled with 1-8)",1,0),
    DSW_ON_OFF("1-2 Unknown",2,0),
    DSW_ON_OFF("1-3 Unknown",4,0),
    DSW_ON_OFF("1-4 Unknown",8,0),
    DSW_ON_OFF("1-5 Unknown",16,0),
    DSW_ON_OFF("1-6 Unknown",32,0),
    DSW_ON_OFF("1-7 Unknown",64,0),
    DSW_ON_OFF("1-8 Stage Select + Additional Options!",128, 0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_kabukikl_1[] =
{
    DSW_ON_OFF("1-1 Debug info / Both P1/P2 Sides",1,0),
    DSW_ON_OFF("1-2 Unknown",2,0),
    DSW_ON_OFF("1-3 Infinite Time",4,0),
    DSW_ON_OFF("1-4 Use with 1-6 for both players invicible",8,0),
    DSW_ON_OFF("1-5 Debug info P2 side (info on range etc + amusing move data)",16,0),
    DSW_ON_OFF("1-6 Use with 1-4 for Both players invicible",32,0),
    DSW_ON_OFF("1-7 Unknown",64,0),
    DSW_ON_OFF("1-8 Unknown",128,0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_kabukikl_2[] =
{
    DSW_ON_OFF("2-1 CPU Controls Player",1,0),
    DSW_ON_OFF("2-2 Halt CPU opponent (they will be frozen while this code is in use)",2,0),
    DSW_ON_OFF("2-3 Skip straight to battling MANTO (1st boss) after selection of",4,0),
    DSW_ON_OFF("2-4 Show ending, right after selection of character",8,0),
    DSW_ON_OFF("2-5 Unknown",16,0),
    DSW_ON_OFF("2-6 Unknown",32,0),
    DSW_ON_OFF("2-7 Unknown",64,0),
    DSW_ON_OFF("2-8 Unknown",128, 0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_kof94_1[] =
{
    DSW_ON_OFF("1-1 Sprite debug menu",1,0),
    DSW_ON_OFF("1-2 Collision detection boxes can be seen",2,0),
    DSW_ON_OFF("1-3 Disable the timer",4,0),
    DSW_ON_OFF("1-4 Both Players Invincible, SDM's also always at max",8,0),
    DSW_ON_OFF("1-5 Display hex code on screen",16,0),
    DSW_ON_OFF("1-6 Unknown",32,0),
    DSW_ON_OFF("1-7 Stages now Selectable",64,0),
    DSW_ON_OFF("1-8 Pal Mode/Job Meter",128,0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_kof94_2[] =
{
    DSW_ON_OFF("2-1 Able to adjust RGB levels of color palette",1,0),
    DSW_ON_OFF("2-2 Program Editing Data",2,0),
    DSW_ON_OFF("2-3 Unknown",4,0),
    DSW_ON_OFF("2-4 Human player is invincible",8,0),
    DSW_ON_OFF("2-5 Displays frame information",16,0),
    DSW_ON_OFF("2-6 Sound menu. Press 'Select' to access",32,0),
    DSW_ON_OFF("2-7 CPU controls both teams",64,0),
    DSW_ON_OFF("2-8 Display character statistics",128, 0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_kof95_1[] =
{
    DSW_ON_OFF("1-1 Sprite debug menu. Also puts a sprite a Hiedern on the top of screen",1,0),
    DSW_ON_OFF("1-2 Collision detection boxes can be seen",2,0),
    DSW_ON_OFF("1-3 Allows you to play as Saishu Kusanagi and Omega Rugal",4,0),
    DSW_ON_OFF("1-4 Both players are invincible",8,0),
    DSW_ON_OFF("1-5 Displays hex code on screen",16,0),
    DSW_ON_OFF("1-6 Unknown",32,0),
    DSW_ON_OFF("1-7 Unknown",64,0),
    DSW_ON_OFF("1-8 Pal Mode/Job Meter",128,0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_kof95_2[] =
{
    DSW_ON_OFF("2-1 Unknown",1,0),
    DSW_ON_OFF("2-2 Unknown",2,0),
    DSW_ON_OFF("2-3 Unknown",4,0),
    DSW_ON_OFF("2-4 Human player is invincible",8,0),
    DSW_ON_OFF("2-5 Displays frame information",16,0),
    DSW_ON_OFF("2-6 Sound test menu. Press 'Select' to access",32,0),
    DSW_ON_OFF("2-7 CPU controls both teams",64,0),
    DSW_ON_OFF("2-8 Unknown",128, 0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_kof96_1[] =
{
    DSW_ON_OFF("1-1 Sprite debug menu",1,0),
    DSW_ON_OFF("1-2 Collision detection boxes can be seen",2,0),
    DSW_ON_OFF("1-3 Disable timer, Display coordinates and Goenitz and Chizuru selectable",4,0),
    DSW_ON_OFF("1-4 Edit menu",8,0),
    DSW_ON_OFF("1-5 Display hex code on screen",16,0),
    DSW_ON_OFF("1-6 Both characters are invincible",32,0),
    DSW_ON_OFF("1-7 Unknown",64,0),
    DSW_ON_OFF("1-8 Annoying shadow covering screen",128,0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_kof96_2[] =
{
    DSW_ON_OFF("2-1 Another edit menu inculding AI controls",1,0),
    DSW_ON_OFF("2-2 CPU controls both teams",2,0),
    DSW_ON_OFF("2-3 Human player is invincible",4,0),
    DSW_ON_OFF("2-4 Unknown",8,0),
    DSW_ON_OFF("2-5 Displays frame information",16,0),
    DSW_ON_OFF("2-6 Sound test menu. Press 'Select' to access",32,0),
    DSW_ON_OFF("2-7 Player 2 can control CPU characters",64,0),
    DSW_ON_OFF("2-8 Unknown",128, 0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_kof97_1[] =
{
    DSW_ON_OFF("1-1 Sprite debug menu",1,0),
    DSW_ON_OFF("1-2 Collision detection boxes can be seen",2,0),
    DSW_ON_OFF("1-3 Unlocks Orochi Chris, Shermie, Yashiro, Iori & Leona",4,0),
    DSW_ON_OFF("1-4 Displays player 1 data",8,0),
    DSW_ON_OFF("1-5 Displays hex code on screen",16,0),
    DSW_ON_OFF("1-6 Both players are invincible",32,0),
    DSW_ON_OFF("1-7 Unknown",64,0),
    DSW_ON_OFF("1-8 Pal Mode/Job Meter",128,0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_kof97_2[] =
{
    DSW_ON_OFF("2-1 Sprite edit menu",1,0),
    DSW_ON_OFF("2-2 CPU controls both teams",2,0),
    DSW_ON_OFF("2-3 Human player is invincible",4,0),
    DSW_ON_OFF("2-4 Unknown",8,0),
    DSW_ON_OFF("2-5 Displays frame information",16,0),
    DSW_ON_OFF("2-6 Sound test menu. Press 'Select' to access",32,0),
    DSW_ON_OFF("2-7 Unknown",64,0),
    DSW_ON_OFF("2-8 Unknown",128, 0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_kof98_1[] =
{
    DSW_ON_OFF("1-1 Sprite debug menu",1,0),
    DSW_ON_OFF("1-2 Collision detection boxes can be seen",2,0),
    DSW_ON_OFF("1-3 Stops timer",4,0),
    DSW_ON_OFF("1-4 Data menu",8,0),
    DSW_ON_OFF("1-5 Displays hex code on screen",16,0),
    DSW_ON_OFF("1-6 Both players are invincible",32,0),
    DSW_ON_OFF("1-7 Unknown",64,0),
    DSW_ON_OFF("1-8 Annoying shadow covering screen",128,0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_kof98_2[] =
{
    DSW_ON_OFF("2-1 Data menu 2",1,0),
    DSW_ON_OFF("2-2 CPU controls both teams",2,0),
    DSW_ON_OFF("2-3 Human player is invincible",4,0),
    DSW_ON_OFF("2-4 Unknown",8,0),
    DSW_ON_OFF("2-5 Displays frame information and column of hex code for CPU",16,0),
    DSW_ON_OFF("2-6 Sound test menu. Press 'Select' to access",32,0),
    DSW_ON_OFF("2-7 Player can control CPU player 2",64,0),
    DSW_ON_OFF("2-8 Unknown",128, 0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_kof99_1[] =
{
    DSW_ON_OFF("1-1 Sprite debug menu. Also puts a Sprite of K' on the screen.",1,0),
    DSW_ON_OFF("1-2 Collision detection boxes can be seen",2,0),
    DSW_ON_OFF("1-3 Stops timer, Play as Krizalid",4,0),
    DSW_ON_OFF("1-4 Data menu",8,0),
    DSW_ON_OFF("1-5 Displays hex code on screen",16,0),
    DSW_ON_OFF("1-6 Both players are invincible",32,0),
    DSW_ON_OFF("1-7 Unknown",64,0),
    DSW_ON_OFF("1-8 Annoying shadow covering screen",128,0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_kof99_2[] =
{
    DSW_ON_OFF("2-1 Data menu 2",1,0),
    DSW_ON_OFF("2-2 CPU controls both teams",2,0),
    DSW_ON_OFF("2-3 Human player is invincible, Play as Krizalid",4,0),
    DSW_ON_OFF("2-4 Unknown",8,0),
    DSW_ON_OFF("2-5 Displays frame information and column of hex code for CPU",16,0),
    DSW_ON_OFF("2-6 Sound test menu. Press 'Select' to access",32,0),
    DSW_ON_OFF("2-7 Unknown",64,0),
    DSW_ON_OFF("2-8 Unknown",128, 0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_kof2000_1[] =
{
    DSW_ON_OFF("1-1 Sprite debug menu",1,0),
    DSW_ON_OFF("1-2 Collision detection boxes can be seen",2,0),
    DSW_ON_OFF("1-3 Play as Zero",4,0),
    DSW_ON_OFF("1-4 Data menu",8,0),
    DSW_ON_OFF("1-5 Unknown",16,0),
    DSW_ON_OFF("1-6 Both players are invincible",32,0),
    DSW_ON_OFF("1-7 Unknown",64,0),
    DSW_ON_OFF("1-8 Annoying shadow covering screen",128,0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_kof2000_2[] =
{
    DSW_ON_OFF("2-1 Data menu 2",1,0),
    DSW_ON_OFF("2-2 CPU controls both teams",2,0),
    DSW_ON_OFF("2-3 Human player is invincible",4,0),
    DSW_ON_OFF("2-4 Displays hit count numbers?",8,0),
    DSW_ON_OFF("2-5 Displays frame information and column of hex code for CPU",16,0),
    DSW_ON_OFF("2-6 Sound test menu. Press 'Select' to access",32,0),
    DSW_ON_OFF("2-7 Disables all SDM's.",64,0),
    DSW_ON_OFF("2-8 Unknown",128, 0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_kof2001_1[] =
{
    DSW_ON_OFF("1-1 Debug Object (press select on 1UP side)",1,0),
    DSW_ON_OFF("1-2 Unknown",2,0),
    DSW_ON_OFF("1-3 Unknown",4,0),
    DSW_ON_OFF("1-4 Data Menu",8,0),
    DSW_ON_OFF("1-5 Enable boss selection",16,0),
    DSW_ON_OFF("1-6 both characters invincible",32,0),
    DSW_ON_OFF("1-7 Auto fire",64,0),
    DSW_ON_OFF("1-8 Screen Filter ON/OFF (also P1 can pause anywhere with Select)",128,0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_kof2001_2[] =
{
    DSW_ON_OFF("2-1 SEQ menu",1,0),
    DSW_ON_OFF("2-2 CPUvsCPU (CPU controlls both chars)",2,0),
    DSW_ON_OFF("2-3 Unknown",4,0),
    DSW_ON_OFF("2-4 Damage value display",8,0),
    DSW_ON_OFF("2-5 Sound Info/Log mode",16,0),
    DSW_ON_OFF("2-6 Sound test (Select on 2UP side)",32,0),
    DSW_ON_OFF("2-7 CPU operation may crash",64,0),
    DSW_ON_OFF("2-8 RGB Palette table",128, 0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_kof2002_1[] =
{
    DSW_ON_OFF("1-1 Debug objects (hold select to access)",1,0),
    DSW_ON_OFF("1-2 Collision boxes",2,0),
    DSW_ON_OFF("1-3 Inf Time",4,0),
    DSW_ON_OFF("1-4 Performance changes? (1P press select)",8,0),
    DSW_ON_OFF("1-5 Unknown",16,0),
    DSW_ON_OFF("1-6 Both invincible",32,0),
    DSW_ON_OFF("1-7 Auto Fire",64,0),
    DSW_ON_OFF("1-8 2P can pause via Select button at any time",128,0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_kof2002_2[] =
{
    DSW_ON_OFF("2-1 SEQ tool, 1P controls (CPU cannot move)",1,0),
    DSW_ON_OFF("2-2 CPU vs CPU",2,0),
    DSW_ON_OFF("2-3 ?",4,0),
    DSW_ON_OFF("2-4 Display extra score info",8,0),
    DSW_ON_OFF("2-5 Display extra Sound info",16,0),
    DSW_ON_OFF("2-6 Sound Test",32,0),
    DSW_ON_OFF("2-7 CPU cannot move",64,0),
    DSW_ON_OFF("2-8 Debug Colors",128, 0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_kof2003_1[] =
{
    DSW_ON_OFF("1-1 w/1-3 = Anim viewer",1,0),
    DSW_ON_OFF("1-2 Messes with the palettes for the lifebars after Coin/Select is pressed. Use A/B/C/D to scroll through",2,0),
    DSW_ON_OFF("1-3 Text viewer. Pretty nice. Press Coin/Select to view. A/B/C/D to scroll",4,0),
    DSW_ON_OFF("1-4 CLSN viewer.",8,0),
    DSW_ON_OFF("1-5 Unknown",16,0),
    DSW_ON_OFF("1-6 Invincibility for both players.",32,0),
    DSW_ON_OFF("1-7 Invincibility for Player 1.",64,0),
    DSW_ON_OFF("1-8 Unknown",128,0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_kof2003_2[] =
{
    DSW_ON_OFF("2-1 Unknown",1,0),
    DSW_ON_OFF("2-2 Unknown",2,0),
    DSW_ON_OFF("2-3 Shows commands on screen. Looks like something that was left out of practice mode",4,0),
    DSW_ON_OFF("2-4 Shows what appears to be a chart for the axis values and changes for p1 & p2",8,0),
    DSW_ON_OFF("2-5 Unknown",16,0),
    DSW_ON_OFF("2-6 Unknown",32,0),
    DSW_ON_OFF("2-7 Unknown",64,0),
    DSW_ON_OFF("2-8 Unknown",128, 0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_kotm2_2[] =
{
    DSW_ON_OFF("2-1 Enable with 2-2 for invincibility",1,0),
    DSW_ON_OFF("2-2 Enable with 2-1 for invincibility",2,0),
    DSW_ON_OFF("2-3 Debug text display",4,0),
    DSW_ON_OFF("2-4 Unknown",8,0),
    DSW_ON_OFF("2-5 Unknown",16,0),
    DSW_ON_OFF("2-6 Unknown",32,0),
    DSW_ON_OFF("2-7 Further text display",64,0),
    DSW_ON_OFF("2-8 Job Meter",128, 0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_lastblad_1[] =
{
    DSW_ON_OFF("1-1 Pal Mode/Job Meter / Raster mode",1,0),
    DSW_ON_OFF("1-2 Ingame debug info display",2,0),
    DSW_ON_OFF("1-3 Both P1 + P2 energy sapped to nothing, power bar maxxed.",4,0),
    DSW_ON_OFF("1-4 P1 Immediate Win when set before start of round",8,0),
    DSW_ON_OFF("1-5 P2 Immediate Win when set before start of round",16,0),
    DSW_ON_OFF("1-6 ????",32,0),
    DSW_ON_OFF("1-7 ????",64,0),
    DSW_ON_OFF("1-8 View Software Version",128,0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_lastblad_2[] =
{
    DSW_ON_OFF("2-1 Ingame debug info display (more information)",1,0),
    DSW_ON_OFF("2-2 ???",2,0),
    DSW_ON_OFF("2-3 ???",4,0),
    DSW_ON_OFF("2-4 Ingame menu - soundtest / pallete editor",8,0),
    DSW_ON_OFF("2-5 More debug display (different info)",16,0),
    DSW_ON_OFF("2-6 ???",32,0),
    DSW_ON_OFF("2-7 Background viewer - scrolls bg with location co-ordinates",64,0),
    DSW_ON_OFF("2-8 Target boxes around characters",128, 0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_lastbld2_1[] =
{
    DSW_ON_OFF("1-1 Unknown",1,0),
    DSW_ON_OFF("1-2 Attack info display",2,0),
    DSW_ON_OFF("1-3 Energy/Sword/Power MAX",4,0),
    DSW_ON_OFF("1-4 2UP energy zero",8,0),
    DSW_ON_OFF("1-5 1UP energy zero",16,0),
    DSW_ON_OFF("1-6 Pallete tool",32,0),
    DSW_ON_OFF("1-7 Unknown",64,0),
    DSW_ON_OFF("1-8 View software version",128,0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_lastbld2_2[] =
{
    DSW_ON_OFF("2-1 View Level",1,0),
    DSW_ON_OFF("2-2 CPU stop",2,0),
    DSW_ON_OFF("2-3 Unknown",4,0),
    DSW_ON_OFF("2-4 Tool Menu (Soundtest/Pallete Editor etc)",8,0),
    DSW_ON_OFF("2-5 Data display",16,0),
    DSW_ON_OFF("2-6 Pause screen",32,0),
    DSW_ON_OFF("2-7 Unknown",64,0),
    DSW_ON_OFF("2-8 Show attack roll",128, 0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_miexchng_1[] =
{
    DSW_ON_OFF("1-1 Job Meter (raster time)",1,0),
    DSW_ON_OFF("1-2 Unknown",2,0),
    DSW_ON_OFF("1-3 Unknown",4,0),
    DSW_ON_OFF("1-4 CPU Autopilot",8,0),
    DSW_ON_OFF("1-5 Select character and play against for ever (prob debug)",16,0),
    DSW_ON_OFF("1-6 Unknown",32,0),
    DSW_ON_OFF("1-7 Unknown",64,0),
    DSW_ON_OFF("1-8 Unknown",128,0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_miexchng_2[] =
{
    DSW_ON_OFF("2-1 Show ending after selection of character",7,0),
    DSW_ON_OFF("2-4 Unknown",8,0),
    DSW_ON_OFF("2-5 Unknown",16,0),
    DSW_ON_OFF("2-6 Unknown",32,0),
    DSW_ON_OFF("2-7 Unknown",64,0),
    DSW_ON_OFF("2-8 Debug Menu (req developper mode)",128, 0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_mslug_1[] =
{
    DSW_ON_OFF("1-1 Unknown",1,0),
    DSW_ON_OFF("1-2 Unknown",2,0),
    DSW_ON_OFF("1-3 Unknown",4,0),
    DSW_ON_OFF("1-4 Unknown",8,0),
    DSW_ON_OFF("1-5 Unknown",16,0),
    DSW_ON_OFF("1-6 Unknown",32,0),
    DSW_ON_OFF("1-7 Unknown",64,0),
    DSW_ON_OFF("1-8 Turns Blood On",128,0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_mslug_2[] =
{
    DSW_ON_OFF("2-1 On screen stats...",1,0),
    DSW_ON_OFF("2-2 Unknown",2,0),
    DSW_ON_OFF("2-3 Unknown",4,0),
    DSW_ON_OFF("2-4 Marco is invincible",8,0),
    DSW_ON_OFF("2-5 Unknown",16,0),
    DSW_ON_OFF("2-6 Unable to jump (seems to screw with button debounce)",32,0),
    DSW_ON_OFF("2-7 Unknown",64,0),
    DSW_ON_OFF("2-8 Unknown",128, 0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_mslug2_1[] =
{
    DSW_ON_OFF("1-1 - view onscreen information",1,0),
    DSW_ON_OFF("1-2 - Unknown",2,0),
    DSW_ON_OFF("1-3 - Pal Mode/Job Meter?",4,0),
    DSW_ON_OFF("1-4 - 1st plane check (shows the walkable ground)",8,0),
    DSW_ON_OFF("1-5 - 2nd plane check",16,0),
    DSW_ON_OFF("1-6 - view attack rectangle",32,0),
    DSW_ON_OFF("1-7 - view body rectangle",64,0),
    DSW_ON_OFF("1-8 - invincibility",128,0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_mslug2_2[] =
{
    DSW_ON_OFF("2-1 - press D to destroy all onscreen enemies!",1,0),
    DSW_ON_OFF("2-2 - display weird creature on screen",2,0),
    DSW_ON_OFF("2-3 - unlimited grenades",4,0),
    DSW_ON_OFF("2-4 - unlimited ammo",8,0),
    DSW_ON_OFF("2-5 - unknown",16,0),
    DSW_ON_OFF("2-6 - unknown",32,0),
    DSW_ON_OFF("2-7 - unknown",64,0),
    DSW_ON_OFF("2-8 - unknown",128, 0),
  { NULL, 0, 0}
};

static struct DSW_DATA dsw_mslugx_1[] =
{
    DSW_ON_OFF("1-1 - view onscreen information",1,0),
    DSW_ON_OFF("1-2 - pause mode",2,0),
    DSW_ON_OFF("1-3 - Pal Mode/Job Meter?",4,0),
    DSW_ON_OFF("1-4 - 1st plane check (shows the walkable ground)",8,0),
    DSW_ON_OFF("1-5 - 2nd plane check",16,0),
    DSW_ON_OFF("1-6 - view attack rectangle",32,0),
    DSW_ON_OFF("1-7 - view body rectangle",64,0),
    DSW_ON_OFF("1-8 - invincibility",128,0),
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
    dsw_debug[0].data = def_0;
    dsw_debug[1].data = def_1;
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
    } else if (is_current_game("breakers") || is_current_game("breakrev")) {
	dsw_debug[1].data = dsw_breakers_1;
	dsw_debug[0].data = dsw_breakers_2;
    } else if (is_current_game("doubledr")) {
	dsw_debug[0].data = dsw_doubledr_2;
    } else if (is_current_game("fatfury2")) {
	dsw_debug[1].data = dsw_fatfury2_1;
	dsw_debug[0].data = dsw_fatfury2_2;
    } else if (is_current_game("fatfursp")) {
	dsw_debug[1].data = dsw_fatfursp_1;
	dsw_debug[0].data = dsw_fatfursp_2;
    } else if (is_current_game("fatfury3")) {
	dsw_debug[1].data = dsw_fatfury3_1;
	dsw_debug[0].data = dsw_fatfury3_2;
    } else if (is_current_game("ironclad")) {
	dsw_debug[1].data = dsw_ironclad_1;
    } else if (is_current_game("kof94")) {
	dsw_debug[1].data = dsw_kof94_1;
	dsw_debug[0].data = dsw_kof94_2;
    } else if (is_current_game("kof95")) {
	dsw_debug[1].data = dsw_kof95_1;
	dsw_debug[0].data = dsw_kof95_2;
    } else if (is_current_game("kof96")) {
	dsw_debug[1].data = dsw_kof96_1;
	dsw_debug[0].data = dsw_kof96_2;
    } else if (is_current_game("kof97")) {
	dsw_debug[1].data = dsw_kof97_1;
	dsw_debug[0].data = dsw_kof97_2;
    } else if (is_current_game("kof98")) {
	dsw_debug[1].data = dsw_kof98_1;
	dsw_debug[0].data = dsw_kof98_2;
    } else if (is_current_game("kof99")) {
	dsw_debug[1].data = dsw_kof99_1;
	dsw_debug[0].data = dsw_kof99_2;
    } else if (is_current_game("kof2000")) {
	dsw_debug[1].data = dsw_kof2000_1;
	dsw_debug[0].data = dsw_kof2000_2;
    } else if (is_current_game("kof2001")) {
	dsw_debug[1].data = dsw_kof2001_1;
	dsw_debug[0].data = dsw_kof2001_2;
    } else if (is_current_game("kof2002")) {
	dsw_debug[1].data = dsw_kof2002_1;
	dsw_debug[0].data = dsw_kof2002_2;
    } else if (is_current_game("kof2003")) {
	dsw_debug[1].data = dsw_kof2003_1;
	dsw_debug[0].data = dsw_kof2003_2;
    } else if (is_current_game("kotm2")) {
	dsw_debug[0].data = dsw_kotm2_2;
    } else if (is_current_game("kabukikl")) {
	dsw_debug[1].data = dsw_kabukikl_1;
	dsw_debug[0].data = dsw_kabukikl_2;
    } else if (is_current_game("lastblad")) {
	dsw_debug[1].data = dsw_lastblad_1;
	dsw_debug[0].data = dsw_lastblad_2;
    } else if (is_current_game("lastbld2")) {
	dsw_debug[1].data = dsw_lastbld2_1;
	dsw_debug[0].data = dsw_lastbld2_2;
    } else if (is_current_game("miexchng")) {
	dsw_debug[1].data = dsw_miexchng_1;
	dsw_debug[0].data = dsw_miexchng_2;
    } else if (is_current_game("mslug")) {
	dsw_debug[1].data = dsw_mslug_1;
	dsw_debug[0].data = dsw_mslug_2;
    } else if (is_current_game("mslug2")) {
	dsw_debug[1].data = dsw_mslug2_1;
	dsw_debug[0].data = dsw_mslug2_2;
    } else if (is_current_game("mslugx")) {
	dsw_debug[1].data = dsw_mslugx_1;
	dsw_debug[0].data = dsw_mslug2_2;
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
    if (!is_neocd()) base &= 0xffff;
    printf("au final, debug dsw %x %x, go to %x\n",input_buffer[10],input_buffer[11],base);
    RAM[base ^ 1] = input_buffer[10];
    RAM[(base+1) ^ 1] = input_buffer[11];
    return 0;
}
