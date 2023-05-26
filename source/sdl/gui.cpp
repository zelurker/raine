#include "raine.h"
#include <SDL_image.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include "display.h"
#include "dejap.h"
#include "games/games.h"
#include "config.h"
#include "hiscore.h"
#include "history.h"
#include "games/default.h"
#include "video/res.h"
#include "control.h"
#include "dsw.h"
#include "sound/sasound.h"
#include "emumain.h"
#include "ingame.h" // print_ingame
#include "files.h" // get_shared
#include "gui.h" // prototypes
#include "bld.h"
#include "profile.h" // fps

#include "gui/menu.h"
#include "dialogs/about.h"
#include "dialogs/game_selection.h"
#include "dialogs/video_options.h"
#include "dialogs/dlg_dsw.h"
#include "dialogs/messagebox.h"
#include "dialogs/gui_options.h"
#include "dialogs/controls.h"
#include "dialogs/game_options.h"
#include "dialogs/fsel.h"
#include "dialogs/neocd_options.h"
#include "dialogs/cheats.h"
#ifdef HAS_CONSOLE
#include "console/console.h"
#include "console/scripts.h"
#endif
#if SDL == 2
#include "sdl2/compat.h"
#include "sdl2/display_sdl.h"
#else
#include "display_sdl.h"
#endif
#include "arpro.h" // CheatCount

#include "neocd/neocd.h"
#include "blit.h"
#include "newspr.h"
#include "neocd/cdrom.h"
#include "str_opaque.h"
#include "control_internal.h"
#include "version.h"
#include "SDL_gfx/SDL_gfxPrimitives.h"
#include "newmem.h"
#include "opengl.h"
#include "palette.h"
#include "cpumain.h"
#include "mz80help.h"
#include "starhelp.h"
#include "m6502hlp.h"
#include "ips.h"

static time_t played_time;
#if USE_MUSASHI == 2
extern "C" m68ki_cpu_core m68020_context;
#endif
extern "C" void init_glsl();
static int WantScreen;
static int WantQuit;
static int WantPlay;
static int autostart_drivers, ips_enabled;

#if SDL == 2

TRaineDesktop::TRaineDesktop() : TDesktop()
{
    tgame = 0;
}

void TRaineDesktop::preinit() {
    SetupScreenBitmap();
    init_video_core();
    reset_palette_map();
    start_cpu_main();
    saInitSoundCard( RaineSoundCard, audio_sample_rate );
    if (is_neocd())
	sa_unpause_sound();
    reset_ingame_timer();
    tgame = 1;
    clear_ingame_message_list(); // don't keep the messages about cpus reset...
}

void TRaineDesktop::end_preinit() {
    tgame = 2;
    // start_cpu_main will be recalled by run_game_emulation which will force a cpu context switch
    // if there's only 1 cpu active in a category, it means its context will be cleared
    // so to be safe we save the context if there is only 1 cpu in a category
    // Trouble between 68020 & 68000 if using musashi for both... !
    if (MZ80Engine >= 1) {
	switch_cpu(CPU_Z80_0);
	mz80GetContext(&Z80_context[0]);
    }
    if (StarScreamEngine >= 1) {
	switch_cpu(CPU_68K_0);
	s68000GetContext(&M68000_context[0]);
    }
#if USE_MUSASHI == 2
    if (MC68020) {
	switch_cpu(CPU_M68020_0);
	m68k_get_context(&m68020_context);
    }
#endif
    if (M6502Engine>=1) {
	switch_cpu(CPU_M6502_0);
	m6502GetContext(&M6502_context[0]);
    }

}

void TRaineDesktop::draw() {
    // tgame values :
    // 0 : the normal desktop
    // 1 : run the game until we play
    // 2 : just take the last screenshot of the game
    if (!current_game || !game_tex) tgame = 0;
    if (!tgame) return TDesktop::draw();
    if (tgame == 1) {
	// We are in 30 fps in the gui, so we'll run 2 frames here
	const VIDEO_INFO *vid_info = current_game->video;
	saUpdateSound(1);
	hs_update();
	if(current_game->exec) current_game->exec();
	saUpdateSound(1);
	if(current_game->exec) current_game->exec();
	if (vid_info->draw_game) vid_info->draw_game();
    }

    SDL_SetRenderDrawColor(rend, 0x0, 0x0, 0x0, 0xFF);
    SDL_RenderClear(rend);
    if (sdl2_color_format == SDL_PIXELFORMAT_RGBX8888) {
	SDL_UpdateTexture(game_tex,NULL,GameViewBitmap->line[0],GameScreen.xfull*sizeof(UINT32));
    } else {
	SDL_UpdateTexture(game_tex,NULL,GameViewBitmap->line[0],GameScreen.xfull*2);
    }
    if (work_area.w) {
	// The SetLogicalSize can't move the origin, so it's not appropriate here
	// and we have to make our dest rect
	int x = GameScreen.xview, y = GameScreen.yview;
	if (display_cfg.fix_aspect_ratio ) {
	     if (x > y) {
		 double ratio = y*1.0/x;
		 double dif_ratio = fabs(ratio-0.75);
		 if (display_cfg.fix_aspect_ratio == 2 || (dif_ratio > 0.001 && dif_ratio < 0.05)) {
		     y = 0.75*x;
		 }
	     } else {
		 double ratio = x*1.0/x;
		 double dif_ratio = fabs(ratio-0.75);
		 if (display_cfg.fix_aspect_ratio == 2 || (dif_ratio > 0.001 && dif_ratio < 0.05)) {
		     x = 0.75*y;
		 }
	     }
	}
	double ratio1 = work_area.w*1.0/x;
	double ratio2 = work_area.h*1.0/y;
	double ratio = MIN(ratio1,ratio2);
	SDL_Rect dest;
	int w = x*ratio, h = y*ratio;
	dest.x = (work_area.w-w)/2+work_area.x;
	dest.y = (work_area.h-h)/2+work_area.y;
	dest.w = w;
	dest.h = h;
	SDL_RenderCopy(rend,game_tex,NULL,&dest);
	return;
    }
    SDL_RenderSetLogicalSize(rend, GameScreen.xview,GameScreen.yview);
    SDL_RenderCopy(rend,pic,NULL,NULL);
    SDL_RenderSetLogicalSize(rend, 0,0);
}

#endif

static void save_menu_config() {
  raine_set_config_int("GUI", "return_mandatory", return_mandatory);
  raine_set_config_int("GUI", "use_transparency", use_transparency);

  raine_set_config_32bit_hex("GUI", "fg_color", fg_color);
  raine_set_config_32bit_hex("GUI", "bg_color", bg_color);
  raine_set_config_32bit_hex("GUI", "fgframe_color", fgframe_color);
  raine_set_config_32bit_hex("GUI", "bgframe_color", bgframe_color);

  raine_set_config_32bit_hex("GUI", "slider_border", cslider_border);
  raine_set_config_32bit_hex("GUI", "slider_bar", cslider_bar);
  raine_set_config_32bit_hex("GUI", "slider_lift", cslider_lift);

  raine_set_config_32bit_hex("GUI", "bg_dialog_bar", bg_dialog_bar);
  raine_set_config_int("GUI", "keep_vga", keep_vga);
  raine_set_config_int("GUI", "bg_anim", bg_anim);
  raine_set_config_int("General", "ips", ips_enabled);
}

static void read_menu_config() {
    ips_enabled = raine_get_config_int("General","ips",1);
  return_mandatory = raine_get_config_int("GUI", "return_mandatory", 0);
  use_transparency = raine_get_config_int("GUI", "use_transparency", 1);
  fg_color = raine_get_config_hex("GUI", "fg_color", mymakecol(255,255,255));
  bg_color = raine_get_config_hex("GUI", "bg_color", makecol_alpha(0x11,0x07,0x78,128));
  fgframe_color = raine_get_config_hex("GUI", "fgframe_color", mymakecol(255,255,255));
  bgframe_color = raine_get_config_hex("GUI", "bgframe_color", mymakecol(0,0,128));
  cslider_border = raine_get_config_hex("GUI", "slider_border", mymakecol(0,0,0));
  cslider_bar = raine_get_config_hex("GUI", "slider_bar", mymakecol(0xc0,0xc0,0xc0));
  cslider_lift = raine_get_config_hex("GUI", "slider_lift", mymakecol(0xff,0xff,0xff));
  bg_dialog_bar = raine_get_config_hex("GUI", "bg_dialog_bar", mymakecol(0,0,0));
#if SDL == 2
  int a = bg_dialog_bar & 0xff, b = (bg_dialog_bar >> 8) & 0xff, g = (bg_dialog_bar >> 16) & 0xff, r = (bg_dialog_bar >> 24);
  bg_dialog_bar_gfx = makecol_alpha(a,b,g,r);
#endif
  keep_vga = raine_get_config_int("GUI","keep_vga",1);
  bg_anim = raine_get_config_int("GUI","bg_anim",0);
}

static void save_font_config() {
  raine_set_config_int("GUI", "min_font_size", min_font_size);
  raine_set_config_int("GUI", "max_font_size", max_font_size);
  raine_set_config_string("GUI","jap_font", jap_font);
}

static void read_font_config() {
  min_font_size = raine_get_config_int("GUI", "min_font_size", 10);
  max_font_size = raine_get_config_int("GUI", "max_font_size", 30);
  jap_font = raine_get_config_string("GUI","jap_font",jap_font);
}

void read_gui_config() {
#if SDL == 2
#define SDL_DEFAULT_REPEAT_DELAY 500
#define SDL_DEFAULT_REPEAT_INTERVAL 50
#endif
  repeat_delay = raine_get_config_int("GUI","repeat_delay",SDL_DEFAULT_REPEAT_DELAY);
  repeat_interval = raine_get_config_int("GUI","repeat_interval",SDL_DEFAULT_REPEAT_INTERVAL);
  autostart_drivers = raine_get_config_int("GUI","autostart_drivers",1);
  read_game_list_config();
#if HAS_NEO
  restore_cdrom_config();
  restore_neocd_config();
#endif
  read_font_config();
  read_menu_config();
  opaque_hud = raine_get_config_int("gui","opaque_hud",0);
  silent_hud = raine_get_config_int("gui","silent_hud",0);
  pause_on_focus = raine_get_config_int("gui","pause_on_focus",0);
}

void write_gui_config() {
  raine_set_config_int("GUI","repeat_delay",repeat_delay);
  raine_set_config_int("GUI","repeat_interval",repeat_interval);
  raine_set_config_int("GUI","autostart_drivers",autostart_drivers);
  save_game_list_config();
#if HAS_NEO
  save_cdrom_config();
  save_neocd_config();
#endif
  save_font_config();
  save_menu_config();
  raine_set_config_int("GUI","opaque_hud",opaque_hud);
  raine_set_config_int("GUI","silent_hud",silent_hud);
  raine_set_config_int("GUI","pause_on_focus",pause_on_focus);
}

static int set_repeat_rate(int sel) {
#if SDL < 2
  SDL_EnableKeyRepeat(repeat_delay,
		      repeat_interval);
#endif
  return 0;
}

static menu_item_t gui_options_menu[] =
{
{  _("Key repeat delay"), &set_repeat_rate, &repeat_delay, 11, {0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000 } },
{  _("Key repeat interval"), &set_repeat_rate, &repeat_interval, 10, { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 } },
{ _("Autostart drivers"), NULL, &autostart_drivers, 2, { 0, 1 }, { "No", "Yes" } }
};

int add_gui_options(menu_item_t *menu) {
  menu[0] = gui_options_menu[0];
  menu[1] = gui_options_menu[1];
  menu[2] = gui_options_menu[2];
  return 3;
}

static TDialog *loading_dialog;
static int progress_count;
static int curl_progress;

static menu_item_t load_items[] =
{
  { "", },
  { _("Progress"), NULL, &progress_count, ITEM_PROGRESS_BAR },
  { "...", },
  { NULL }, // curl msg
  { _("Progress"), NULL, &curl_progress, ITEM_PROGRESS_BAR },
  { _("from internet archive"), },
  { NULL, },
};

static int last_tick = 0;

void load_progress(char *rom,int count)
{
  int ticks = SDL_GetTicks();
  if (ticks - last_tick < 16 && count < 100)
    return;
  progress_count = count;
  if (loading_dialog) {
    loading_dialog->update_label(2,rom);
    loading_dialog->redraw_fg_layer();
  }
  last_tick = SDL_GetTicks();
}

void setup_curl_dlg(char *name) {
    load_items[3].label = name;
    if (raine_cfg.no_gui)
	return;
    delete loading_dialog;
    loading_dialog = new TDialog(_("Loading Game"),load_items);
    loading_dialog->draw();
}

void curl_progress_f(int count) {
    if (!loading_dialog) return;
    if (curl_progress != count) {
	curl_progress = count;
	if (loading_dialog)
	    loading_dialog->redraw_fg_layer();
    }
}

void load_message(char *msg) {
  load_items[0].label = msg;
  load_items[2].label = " ";
  if (loading_dialog) {
      loading_dialog->update_label(0,msg);
      loading_dialog->update_label(2," ");
      loading_dialog->redraw_fg_layer();
  }
}

static void load_game_proc()
{
    LoadDefault();

    load_error = 0;
    load_debug = (char*)malloc(0x10000);

    load_debug[0] = 0;

    current_game = game_list[raine_cfg.req_game_index];
    fps = current_game->video->fps;
    if (fps < 0.1)
	fps = 60.0; // 60 fps (default)
    if (fps > max_fps)
	fps = max_fps;
    if (ips_enabled) {
	char file[FILENAME_MAX];
	snprintf(file,1024,"%sips" SLASH "%s.ini",dir_cfg.exe_path,current_game->main_name);
	file[FILENAME_MAX-1] = 0;
	FILE *f = fopen(file,"r");
	file[strlen(file)-4] = 0; // just keep the path, without the .ini extension
	int l = strlen(file);
	if (f) {
	    char buf[1024];
	    while (!feof(f)) {
		if (myfgets(buf,1024,f)) {
		    if (!buf[0] || !strncmp(buf,"//",2))
			continue;
		    snprintf(&file[l],FILENAME_MAX-l,SLASH "%s",buf);
		    add_ips_file(file);
		}
	    }
	    fclose(f);
	}
    }

    // I have to change the depth BEFORE loading.
    // Probably because of the set_color_mapper in the loading function

#if SDL == 1
    if((display_cfg.auto_mode_change && display_cfg.video_mode == 2) || // normal blits
	     display_cfg.keep_ratio) { // && switch_res(current_game->video))){
      switch_res(current_game->video);
    } else {
      print_debug("no resolution switching wanted\n");
      if (current_game->video->flags & VIDEO_NEEDS_8BPP) {
	bestbpp = 8;
	display_cfg.video_mode = 2; // only possibility for 8bpp
      }
      update_stretch();
    }
#endif

    progress_count = 0;
    curl_progress = 0;
    load_items[0].label = _("Applying graphics layouts and stuff..."); // init
    if (!raine_cfg.no_gui) {
#if SDL == 1
	adjust_gui_resolution();
#endif
	loading_dialog = new TDialog(_("Loading game"),load_items);
	loading_dialog->draw();
    }

    load_game_rom_info();

    if (!(load_error & LOAD_FATAL_ERROR)) {
	init_inputs();
	init_dsw();
	init_romsw();
	init_sound();
	read_bld();
	current_game->nb_loaded++;
    }
}

static void do_load_game(void)
{
     char neocd_wm_title[160];

     char *error = get_ips_error();
     *error = 0;

   /*

   satisfy the request

   */

   raine_cfg.req_load_game = 0;

   /*

   close down the current game (if there is one)

   */

   if(current_game){


      ClearDefault();
   }


   load_game_proc(); // load without gui

   // Check if the load was successful, if ok, load game config, if bad clear the mess

   switch(load_error&3){
   case LOAD_WARNING:			// WARNING - IT MIGHT RUN OK

     strcat(load_debug,"\n\nThe game might not run correctly.");
     MessageBox(_("Loading problem"), load_debug);

   case 0x00:				// SUCCESS
     memcpy(&prev_display_cfg, &display_cfg, sizeof(DISPLAY_CFG));

     if (bestbpp > 0 && bestbpp <=32) {
       WantScreen = 1;
       display_cfg.bpp = bestbpp;
       bestbpp = 0;
     }

     if (bestw && besth && display_cfg.video_mode == 2) {
       //display_cfg.screen_type = bestmode;
       display_cfg.screen_x = bestw;
       display_cfg.screen_y = besth;
       WantScreen = 1;
       bestw = besth = 0;
     }

     reset_game_hardware(); // must be done BEFORE load_game_config for scripts

     load_game_config();

     if (loading_dialog) {
	 delete loading_dialog;
	 load_items[3].label = NULL;
	 loading_dialog = NULL;
     }
     if (raine_cfg.save_game_screen_settings)
       WantScreen=1;
     sprintf(neocd_wm_title,"Raine %s - %s",VERSION,current_game->long_name);
     SDL_WM_SetCaption(neocd_wm_title,neocd_wm_title);
     break;
   case LOAD_FATAL_ERROR:			// FATAL ERROR - REMOVE GAME
   case LOAD_FATAL_ERROR|LOAD_WARNING:

     MessageBox(_("Loading error"),load_debug);

     ClearDefault();

     current_game = NULL;

     break;
   }

   if (*error)
       MessageBox(_("IPS error"),error,"OK");
   free(load_debug);
}

static int play_game(int sel) {
  WantPlay = 1;
  return 1;
}

static int quit_raine(int sel) {
  WantQuit = 1;
  return 1;
}

static int region_code;

static int set_region(int sel) {
  SetLanguageSwitch(region_code);
  return 0;
}

#if HAS_NEO
static int load_neo_game(int sel) {
  char res[1024];
  char *exts[] = { ".zip",
      // iso handled specifically by fsel, if no cue is found but iso or iso.gz
      // is found, then a special list of extensions is used
  // ".iso",
  ".cue", "ipl.txt", "7z", NULL };
  fsel(neocd_dir,exts,res,_("Load Neo-Geo CD game"));
  return load_neo_from_name(res);
}
#endif

class TMyMultiFileSel : public TMultiFileSel
{
    private:
	TFont_ttf *fb;
	int bot_sel,used,wi,hi;
	menu_item_t *menu_bot;
	SDL_Texture *tex;
    public:
	TMyMultiFileSel(char *my_title, char *mypath, char **myext, char **my_res_str,int my_max_res,int opts = 0, char* mytitle2 = NULL) :
	    TMultiFileSel(my_title, mypath, myext, my_res_str,my_max_res, opts, mytitle2)
    {
  fb = new TFont_ttf(12,font_name);
  bot_sel = -1;
  used = 0;
  menu_bot = NULL;
  tex = NULL;
  wi = 0;
    }

	~TMyMultiFileSel();
	void free_bot_info();
	virtual void compute_nb_items();
	virtual int mychdir(int n);
#ifdef RAINE_UNIX
	virtual void set_dir(char *mypath);
#endif
	virtual int get_max_bot_frame_dimensions(int &w, int &h);
	virtual void draw_bot_frame();
};

#ifdef RAINE_UNIX
void TMyMultiFileSel::set_dir(char *mypath) {
    if (chdir(mypath)) {
	char *slash = strrchr(path, SLASH[0]);
	if (slash) {
	    char file[FILENAME_MAX];
	    strncpy(file,slash+1,FILENAME_MAX);
	    snprintf(path,FILENAME_MAX,"%sips/%s",dir_cfg.share_path,file);
	    TMultiFileSel::set_dir(path);
	    snprintf(path,FILENAME_MAX,"%sips/%s",dir_cfg.exe_path,file);
	}
    } else {
	TMultiFileSel::set_dir(mypath);
    }
}
#endif

TMyMultiFileSel::~TMyMultiFileSel() {
    delete fb;
    free_bot_info();
    if (!nb_sel) {
	char res[FILENAME_MAX];
	char *shared = get_shared("ips");
	strcpy(res,shared);
	strcat(res,SLASH);
	if (strcmp(res,res_file)) { // if not in the root of the ips directory...
	    strcpy(res,res_file);
	    int l = strlen(res);
	    if (res[l-1] == SLASH[0]) {
		res[l-1] = 0;
		l--;
	    }
	    if (res[l-1] == SLASH[0]) {
		res[l-1] = 0;
		l--;
	    }
	    strcat(res,".ini");
	    unlink(res);
	}
    }
}

static void save_ips_ini(char **res) {
    if (!res[0]) return;
    char *slash = strrchr(res[0],SLASH[0]);
    if (slash) {
	*slash = 0;
	if (slash > res[0] && slash[-1] == SLASH[0]) slash[-1] = 0; // for windows and its stupide double \\ !
    }
    char *slash2 = strrchr(res[0],SLASH[0]);
    if (!slash2 || !slash) {
	// Not an error it just means the user didn't select anything
	return;
    }
    char game[1024];
    strcpy(game,res[0]);
    strcat(game,".ini");
    *slash = SLASH[0];
    printf("save_ips_ini %s\n",game);
    FILE *g = fopen(game,"w");
    if (!g) {
	MessageBox("Error","Can't create ini file","ok");
	return;
    }
    ips_info.nb = 0; // just to be sure, but normally already at 0
    for (int n=0; n<10; n++) {
	if (res[n]) {
	    char *s = strrchr(res[n],SLASH[0]);
	    if (s) {
		*s = 0;
		fprintf(g,"%s\n",&s[1]);
	    } else
		fprintf(g,"%s\n",res[n]); // rather unlikely it ever happens... !
	    free(res[n]); // must free the results allocated by multi_fsel
	    res[n] = NULL;
	}
    }
    fclose(g);
}

void TMyMultiFileSel::free_bot_info() {
    for (int n=0; n<used; n++)
	free(menu_bot[n].label);
    if (menu_bot) {
	free(menu_bot);
	menu_bot = NULL;
	wi = 0;
	used = 0;
	bot_sel = -1;
    }
    if (tex) {
	SDL_DestroyTexture(tex);
	tex = NULL;
    }
}

int TMyMultiFileSel::mychdir(int n) {
    free_bot_info();
    if (n == 0 && !strcmp(menu[n].label,"..")) {
	int x = 0;
	if (!nb_sel) {
	    char res[FILENAME_MAX];
	    char *s = strrchr(path,SLASH[0]);
	    if (s) { // always the case normally
		snprintf(res,FILENAME_MAX,"%sips" SLASH "%s.ini",dir_cfg.exe_path,s+1);
		unlink(res);
	    }
	} else {
	    for (int n=0; n<nb_files; n++) {
		if (selected[n]) {
		    if (!res_str[x])
			res_str[x] = (char*)malloc(FILENAME_MAX);
		    if (res_str[x])
			sprintf(res_str[x++],"%s" SLASH "%s",path,menu[n].label);
		}
	    }
	}
	save_ips_ini(res_str);
    }
    return TMultiFileSel::mychdir(n);
}

void TMyMultiFileSel::compute_nb_items()
{
    char str[FILENAME_MAX];
    snprintf(str,FILENAME_MAX,"%s%s",path,SLASH);
    int path_changed = strcmp(str,res_file);
    TMultiFileSel::compute_nb_items();
#ifdef RAINE_UNIX
    char tpath[FILENAME_MAX];
    snprintf(tpath,FILENAME_MAX,"%sips",dir_cfg.exe_path);
    if (!strcmp(path,tpath)) {
	snprintf(path,FILENAME_MAX,"%s%s",dir_cfg.share_path,"ips");
	int old_files = nb_files;
	add_files();
	snprintf(path,FILENAME_MAX,"%s%s",dir_cfg.exe_path,"ips");
	if (old_files != nb_files) {
	    free(selected);
	    selected = (int*)calloc(nb_files,sizeof(int));
	}
    }
#endif
#if 0
    char path2[FILENAME_MAX];
    strncpy(path2,path,FILENAME_MAX);
#endif

    if (!strcmp(path,get_shared("ips")) && !strcmp(menu[0].label,"..")) {
	// remove .. entry if in the root of the ips directory
	if (nb_files == 1) throw _("ips directory empty !!!");
	memmove(&menu[0],&menu[1],(nb_files-1)*sizeof(menu_item_t));
	nb_files--;
	nb_items--;
	nb_disp_items--;
	return;
    }
    if (path_changed) {
	// compute_nb_items is called also when the window size changes, re-read selection only when the path changes !
	char file[FILENAME_MAX];
	char *fi = strrchr(path,SLASH[0]);
	if (fi) snprintf(file,1024,"%sips" SLASH "%s.ini",dir_cfg.exe_path,fi+1); // Normally always the case
	else
	    snprintf(file,1024,"%s.ini",path);
	file[1023] = 0;
	// read the ini to initialize the selected entries
	FILE *f = fopen(file,"r");
	if (!f) return;
	while (!feof(f)) {
	    char buf[1024];
	    myfgets(buf,1024,f);
	    // Ultra basic attempt to allow files edited by hand : skip blank lines, and lines starting with //
	    // notice that a line starting with a space and then // will be rejected, so it's really ultra basic !
	    // This thing has never been intended to be edited
	    if (!buf[0] || !strncmp(buf,"//",2))
		continue;
	    for (int n=0; n<nb_files; n++) {
		if (!strcmp(menu[n].label,buf)) {
		    selected[n] = 1;
		    nb_sel++;
		}
	    }
	}
	fclose(f);
    }
}

int TMyMultiFileSel::get_max_bot_frame_dimensions(int &w, int &h) {
  if (font) {
      font->dimensions("ABCDEFGHIJ",&w,&h);
      w*= 10;
      h*= 3;
      if (h < 224) {
	  h = 224;
      }
  }
  return 100;
}

#define MAX_SIZE 10000

void TMyMultiFileSel::draw_bot_frame() {
    int base = work_area.y+work_area.h;
    boxColor(rend,0,base,sdl_screen->w,sdl_screen->h,bg_frame_gfx);
    if (sel >= 0) {
	int y = base;
	if (sel != bot_sel) {
	    char *l = menu[sel].label;
	    int len0 = strlen(l);
	    if (strcmp(&l[len0-4],".dat")) return;
	    // display the what there is in an eventual [en_US] block in the selected .dat...
	    FILE *f = fopen(l,"r");
	    if (!f) return;
	    while (!feof(f)) {
		char buf[1024];
		myfgets(buf,1024,f);
		if (!strncmp(buf,"[en_US]",7))
		    break;
	    }
	    char buf[MAX_SIZE];
	    char *s = buf;
	    *s = 0;
	    int len = 0;
	    while (!feof(f)) {
		myfgets(s,MAX_SIZE - len,f);
		if (s[0] == '[') break;
		len = strlen(buf);
		if (len < MAX_SIZE - 1) {
		    strcat(buf,"\n");
		    len++;
		    s = &buf[len];
		} else
		    break;
	    }
	    fclose(f);
	    char pic[len0+1];
	    strcpy(pic,l);
	    strcpy(&pic[len0-4],".png");
	    wi = 0;
	    free_bot_info();
	    if (exists(pic)) {
		tex = IMG_LoadTexture(rend,pic);
	    }
	    int access; u32 format;
	    SDL_QueryTexture(tex,&format,&access,&wi,&hi);
	    if (hi > 224) hi = 224;
	    if (wi > 320) wi = 320;
	    menu_bot = get_menu_from_text(buf,fb,&used,sdl_screen->w-wi-HMARGIN);
	    bot_sel = sel;
	}
	// Opengl obliges to redraw the thing for every frame, which is a total waste of cycles here
	// hence all the data I buffered just before this point, but in the end there are still some cycles to waste here... !
	// Nothing to do about that. Maybe vulkan is less crazy about that ? But using vulkan just to draw an interface would be overkill anyway !
	if (tex) {
	    SDL_Rect dest = { sdl_screen->w-wi, base, wi, hi };
	    SDL_RenderCopy(rend,tex,NULL, &dest);
	}
	for (int n=0; n<used; n++) {
	    int w,h;
	    fb->dimensions(menu_bot[n].label,&w,&h);
	    if (y + h > sdl_screen->h) break;
	    fb->put_string(HMARGIN,y,menu_bot[n].label,fg_frame,bg_frame);
	    y += h;
	}
    }
}

static menu_item_t header_fsel[] =
{
    { "IPS files", NULL, &ips_enabled, 2, {0, 1}, {"Disabled", "Enabled"} },
    { NULL }
};

static void my_multi_fsel(char *mypath, char **ext, char **res_str, int max_res, char *title) {
  fsel_dlg = new TMyMultiFileSel(mypath,mypath,ext,res_str,max_res,NO_HEADER,title);
  fsel_dlg->set_header(header_fsel);
  try {
      fsel_dlg->execute();
  }
  catch(const char *msg) {
      MessageBox(_("Error"),(char*)msg,"Ok");
  }
  delete fsel_dlg;
}

int do_preload_ips(int sel) {
    char *res[10];
    memset(res,0,10*sizeof(char*));
    char *exts[] = { ".dat", NULL };
    char path[FILENAME_MAX];
    snprintf(path,FILENAME_MAX,"%s%s",dir_cfg.exe_path,"ips");
    my_multi_fsel(path,exts,res,10,_("Select IPS *.dat files"));
    // Not very convenient : when the fsel is closed by esc, it just returns its last path in res
    // so the only way to test this is to test if res contains a directory, since directories can be opened as normal files in linux !
    struct stat stbuf;
    if (!res[0] || (!stat(res[0],&stbuf) && S_ISDIR(stbuf.st_mode)))
	return 0; // it's a dir indeed
    // Here we have at least 1 result, 1st find the name of the game
    save_ips_ini(res);
    return 0;
}

static int do_ips(int sel) {
    char res[1024];
    char *exts[] = { ".ips", NULL };
    fsel(get_shared("ips"),exts,res,_("Select IPS file"));
    if (!*res) return 0;
    // Not very convenient : when the fsel is closed by esc, it just returns its last path in res
    // so the only way to test this is to test if res contains a directory, since directories can be opened as normal files in linux !
    struct stat stbuf;
    if (!stat(res,&stbuf) && S_ISDIR(stbuf.st_mode))
	return 0; // it's a dir indeed
    load_ips(res,ROM,get_region_size(REGION_CPU1),0,"");
    return 0;
}

extern int do_sound_options(int sel);
static TMain_menu *main_menu;

static menu_item_t main_items[] =
{
{ _("Play game"), &play_game, },
{ _("Game options"), &do_game_options },
{ _("Game commands list"), &show_moves },
{ _("Game cheats"), &do_cheats, },
{ _("Apply IPS to ROM code"), &do_ips, },
{ _("Region"), &set_region, },
{ _("Dipswitches"), &do_dlg_dsw, },
{ _("Change/Load game"), &do_game_sel },
#if HAS_NEO
{ _("Load Neo-Geo CD game"), &load_neo_game },
{ _("Neo-Geo/Neo-Geo CD options"), &do_neocd_options },
#endif
{ _("Video options"), &do_video_options },
{ _("Sound options"), &do_sound_options },
{ _("GUI options"), &do_gui_options },
{ _("Inputs"), &do_controls },
{ _("About..."), &do_about,},
#ifdef HAS_CONSOLE
{ _("Console"), &do_console, },
#endif
{ _("Quit"), &quit_raine, },
{ NULL, NULL, NULL },
};

int TMain_menu::can_be_displayed(int n) {
    switch(n) {
    case 0: // Play Game
    case 1: // game options
    case 4: // ips
	return current_game != NULL;
    case 2: // game commands
	return nb_commands > 0;
    case 5: // Region
	return current_game != NULL && current_game->romsw != NULL;
    case 3: // cheats
	return current_game != NULL && (CheatCount > 0
#ifdef HAS_CONSOLE
		|| nb_scripts > 0
#endif
		);
    case 6: // dsw
	return current_game != NULL && current_game->dsw != NULL;
    default:
	return 1;
    }
}

char* TMain_menu::get_top_string() {
    return EMUNAME " " VERSION;
}

char *TMain_menu::get_bot_frame_text() {
  int size = GetMemoryPoolSize()/1024; // Minimum Kb
  static char game[100];
  snprintf(game,100,"%s",(current_game ? current_game->long_name :_("No game loaded")));
  if (current_game) {
      snprintf(&game[strlen(game)], 100-strlen(game),
	      " (%s)",current_game->main_name);
      if (size < 1024)
	  snprintf(&game[strlen(game)],100-strlen(game),
		  " (%d Kb)",size);
      else
	  snprintf(&game[strlen(game)], 100-strlen(game),
		  " (%d Mb)",size/1024);
  }
  game[99] = 0;
  return game;
}

void setup_font()
{
  if (ingame_font) {
    return;
  }
  ingame_font = (UINT8*)calloc(1,1792);
  FILE *f = fopen (get_shared("font6x8.bin"), "rb");
  if (!f)
    f = fopen(get_shared("fonts" SLASH "font6x8.bin"),"rb");
  if (f) {
   fread(ingame_font,1,1792,f);
   fclose(f);
  } else {
    fatal_error("No font, byebye!");
  }
}

#if !defined(SDL) || SDL==1
extern UINT32 videoflags; // display.c
#endif
int goto_debuger = 0;

#if SDL == 2
static void my_event(SDL_Event *event) {
    switch (event->type) {
    case SDL_CONTROLLERDEVICEADDED:
    case SDL_CONTROLLERDEVICEREMOVED:
    case SDL_CONTROLLERBUTTONDOWN:
    case SDL_CONTROLLERBUTTONUP:
    case SDL_CONTROLLERAXISMOTION:
    case SDL_JOYAXISMOTION:
	// Notice button up must be passed if button down is passed, so that
	// both cancel each other
	control_handle_event(event);
	break;
    case SDL_WINDOWEVENT:
	if (event->window.event == SDL_WINDOWEVENT_RESIZED || event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
	    resize(1,event->window.data1,event->window.data2);
	}
#ifndef RAINE_WIN32
	else if (event->window.event == SDL_WINDOWEVENT_MOVED) {
	    if (event->common.timestamp < 2000 && display_cfg.posy) { // probably some fancy window manager event
		// Specific to some window managers which seem to correct window placement after adding decorations in linux
		// well we don't want this correction
		// Notice the timestamp is sadly some moving target, I got it at more than 1000 with 2 gamepads and 1 joystick adding events...
		// which makes this thing rather unprecise...
		if (display_cfg.fullscreen) {
		    // See comments about broken toggle fullscreen in control.c
		    SDL_SetWindowPosition(win,0,0);
		    return;
		}
		SDL_SetWindowPosition(win,display_cfg.posx,display_cfg.posy);
	    }
	}
#endif
    }
}

static void gui_end() {
    // Actually a simple call to reset_shaders might be enough here
    // the problem is that calling glsl functions is messy and doing it from the gui wouldn't be nice
    // so maybe it's better to keep the hook for now with this inside...
    if (raine_cfg.req_load_game) return;
    if (GameScreen.xview) {
	if (display_cfg.video_mode > 0)
	    SDL_RenderSetLogicalSize(rend, GameScreen.xview, GameScreen.yview);
	else if (display_cfg.video_mode == 0)
	    ScreenChange();
    }
    if (!main_menu)
	sa_unpause_sound();
    invalidate_inputs();
    SDL_ShowCursor(0);
}

static void gui_start() {
    if (!main_menu)
	sa_pause_sound();
    SDL_ShowCursor(1);
}
#endif

static void do_main_menu() {
  int old_region;
  // init romsw
  if (current_game && current_game->romsw) {
    main_items[5].values_list_size = LanguageSw.Count;
    main_items[5].value_int = &region_code;
    old_region = region_code = GetLanguageSwitch();
    for (int n=0; n<LanguageSw.Count; n++) {
      main_items[5].values_list[n] = n;
      main_items[5].values_list_label[n] = LanguageSw.data[n].Mode;
    }
  }

  main_menu = new TMain_menu(_("Main menu"),main_items);
  main_menu->execute();
  if (current_game && current_game->romsw && old_region != region_code)
      reset_game_hardware();
  delete main_menu;
#if SDL==2
  // Apparently a simple SDL_DestroyTexture from the delete main_menu is enough to turn the screen upside down if we don't call gui_end here...
  // At this level a hook can't do it... !
  gui_end();
#endif
  main_menu = NULL;
}

void StartGUI(void)
{
#if SDL == 2
    desktop = new TRaineDesktop();
    gui_end_hook = &gui_end;
    gui_start_hook = &gui_start;
    event_hook = &my_event;
#ifdef RAINE_WIN32
    init_glsl();
#endif
#else
    setup_mouse_cursor(IMG_Load("bitmaps" SLASH "cursor.png"));
#endif
    get_shared_hook = &get_shared;

#ifdef RAINE_DEBUG
    print_debug("StartGUI(): START\n");
#endif

    init_display();

    setup_font(); // Usefull even without gui for the messages on screen

    WantScreen=0;
    WantQuit=0;
    WantPlay=0;

#ifdef RAINE_DEBUG
    print_debug("StartGUI(): prepare international\n");
#endif

#if SDL < 2
   // We need to know if we have hw overlay support, and the only way to know
   // is to try to create one...
    if (display_cfg.video_mode == 1 && !sdl_overlay && display_cfg.bpp >= 16) {
	sdl_create_overlay(display_cfg.screen_x,display_cfg.screen_y);
    }
#endif

   while(!WantQuit){		// ----- Main Loop ------

       /*

	  check for load game request (from the command line)

*/

       if(raine_cfg.req_load_game) {

	   do_load_game();
#if SDL == 2
	   if (current_game && !raine_cfg.no_gui && autostart_drivers) rdesktop->preinit();
#endif
       }

       if(!raine_cfg.no_gui)	// GUI MODE
       {
#ifdef HAS_CONSOLE
	   if (goto_debuger) {
	       do_console(0);
	       WantPlay = 1;
	   } else
#endif
	   {
	       do_main_menu();
	       if (!WantQuit && !WantPlay && !raine_cfg.req_load_game && current_game)
		   WantPlay = 1;
	   }
       } else {

	   WantQuit=1;
	   ScreenChange();

	   if(current_game)

	       WantPlay = 1;

	   else

	       WantPlay = 0;
       }
#if SDL==2
       if (!raine_cfg.no_gui) rdesktop->end_preinit();
#endif

       if(current_game && (display_cfg.auto_mode_change && display_cfg.video_mode == 2)) {
	   switch_res(current_game->video);
	   if (bestw) {
	       display_cfg.screen_x = bestw;
	       display_cfg.screen_y = besth;
	       bestw = besth = 0;
	   }
       }

       /*
	* ScreenChange is now called at the end of execute when exiting from the gui
       if(!WantQuit &&
	       (WantScreen || display_cfg.screen_x != sdl_screen->w ||
	       display_cfg.screen_y != sdl_screen->h ||
	       (display_cfg.bpp != sdl_screen->format->BitsPerPixel && !sdl_overlay) ||
	       ((videoflags & SDL_DOUBLEBUF) && !(sdl_screen->flags & SDL_DOUBLEBUF)))){
	   // Are we here for a screenchange?
	   WantScreen=0;
	   print_debug("calling ScreenChange from StartGUI, WantScreen=0\n");
	   ScreenChange();
       }*/

       clear_raine_screen();

       if(WantPlay){		// Are we able to and wanting to play?
	   WantPlay = 0;
	   played_time = time(NULL);
	   current_game->last_played = played_time;

	   SDL_ShowCursor(0);
#ifndef RAINE_DEBUG
	   if (GameMouse)
	       SDL_WM_GrabInput(SDL_GRAB_ON);
#endif
	   if(run_game_emulation()){ // In no gui mode, tab will reactivate the gui (req. by bubbles)
	       raine_cfg.no_gui = 0;
	       WantQuit = 0;
	   }
	   current_game->time_played += time(NULL) - played_time;
#if SDL == 2
	   if (!rend)
	       rend = SDL_CreateRenderer(win,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	   if (display_cfg.video_mode > 0)
	       SDL_RenderSetLogicalSize(rend, 0,0);
#else
	   if (!(sdl_screen->flags & SDL_DOUBLEBUF) && !emulate_mouse_cursor)
#endif
	       SDL_ShowCursor(1);
	   if (GameMouse)
	       SDL_WM_GrabInput(SDL_GRAB_OFF);
       }

   }			// ----- Main Loop -----

   /*

   close down the current game (if there is one)

   */

   if(current_game){

     ClearDefault();

     current_game = NULL;
   }

}

void *old_draw = NULL;

void switch_mixer() {
  print_ingame(60,_("No mixer yet"));
}

