#include "../gui/menu.h"
#include "about.h"
#include <SDL_image.h>
#include "confile.h"
#include "raine.h"
#include "games.h"
#include "files.h"
#include "SDL_gfx/SDL_gfxPrimitives.h"
#include "dialogs/romdirs.h"
#include "gui.h"
#include "game_selection.h"

/* This is so far the most complex dialog in the sdl version :
 *  - it changes the bg picture while browsing the list of games
 *  - it has a custom frame (top and bottom bars informations)
 *  - It has quite a few filtering options
 *  - It has a special "options" entry at the begining of the list of games
 *  Anyway, even if it makes quite a big source file, it's not even comparable
 *  to the big mess that was required to do that in allegro.
 *  It remains relatively easy to read for what it does. */

#if SDL==2
#define boxColor(sf,x,y,w,h,col) boxColor(rend,x,y,w,h,col)
#endif

static int game_list_mode,company,status = 1,category,driver,clones = 1,short_names;

static int change_names(int sel);
static char *avail;

// The list of driver names, which must follow the alphabetical order of the
// driver names in options[] below.
char *drivers[] =
{ "cave.c", "cps1drv.c", "cps2drv.c","neogeo.c", "nmk.c", "gunbird.c", "seta.c",
  "taito_f2.c", "lsystem.c", "xsystem2.c", "toaplan1.c", "toaplan2.c" };

static menu_item_t options[] =
{
{  _("Display"), NULL, &game_list_mode, 3, {0, 1, 2},{_("All games"), _("Available games"), _("Missing games")} },
{ _("Status"), NULL, &status, 4, {0, 1, 2, 3}, {_("All status"), _("Only working games"), _("Only partially working games"), _("Only not working games")} },
{ _("Category"), NULL, &category, NB_GAME_TYPE, }, // this one is finished dynamically
{ _("Company"), NULL, &company, }, // finished dynamically also
{ _("Driver"), NULL, &driver, 13, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 },
{ _("All drivers"), _("Cave"), _("Capcom CPS1"), _("Capcom CPS2"), _("Neo-Geo"), _("NMK"), _("Psikyo (gunbird)"), _("Seta"), _("Taito F2"), _("Taito L-System"), _("Taito X-System 2"), _("Toaplan 1"), _("Toaplan 2") } },
{ _("Clones"), NULL, &clones, 2, {0, 1 }, { _("Without"), _("With") } },
{ _("Display short names too"), &change_names, &short_names, 2, {0, 1}, {_("No"), _("Yes")} },
{ _("Rom directories..."), &do_romdir },
{ NULL },
};

static int recompute_list();

static int do_options(int sel) {
    int n;
    // init category
    for (n=0; n<NB_GAME_TYPE; n++) {
	options[2].values_list[n] = n;
	options[2].values_list_label[n] = game_type[n];
    }
    // init game driver
    options[3].values_list_size = (nb_companies>64 ? 64 : nb_companies);
    for (n=0; n<nb_companies && n<64; n++) {
	options[3].values_list[n] = n;
	options[3].values_list_label[n] = game_company_name(n);
    }
    options[3].values_list_label[0] = _("All companies");
    sort_menu(options);
    TMenu *mymenu = new TMenu("",options);
    mymenu->execute();
    delete mymenu;
    recompute_list();
    return 0;
}

static menu_item_t header[] =
{
    {  _("-- Options --"), &do_options },
    { NULL }, // 5 lines at most of favorite games
    { NULL },
    { NULL },
    { NULL },
    { NULL },
    { NULL },
};

#if SDL == 1
static SDL_Rect get_max_area(SDL_Rect &work_area, SDL_Rect &fgdst) {
  SDL_Rect bg = work_area;
  if (fgdst.x < bg.x) {
    bg.x = fgdst.x;
    if (fgdst.x+fgdst.w > work_area.x+work_area.w)
      bg.w = fgdst.w;
    else
      bg.w = work_area.x+work_area.w - bg.x;
  }
  if (fgdst.y < bg.y) {
    bg.y = fgdst.y;
    if (fgdst.y+fgdst.h > work_area.y+work_area.h)
      bg.h = fgdst.h;
    else
      bg.h = work_area.y+work_area.h - bg.y;
  }
  return bg;
}
#endif

void read_game_list_config() {
   game_list_mode	= raine_get_config_int( "GUI", "game_list_mode", 1);
   short_names	= raine_get_config_int( "GUI", "short_names", 0);
   clones = raine_get_config_int("GUI","clones",1);
   status = raine_get_config_int("GUI","status",1);
}

void save_game_list_config() {
   raine_set_config_int(	"GUI", "game_list_mode", game_list_mode);
   raine_set_config_int(	"GUI", "short_names", short_names);
   raine_set_config_int(	"GUI", "clones", clones);
   raine_set_config_int(	"GUI", "status", status);
}

class TGame_sel : public TMenu
{
  protected:
    int last_sel,image_counter;
    char current_picture[256];
    int h_title,h_bot,w_year,h_year,w_categ,h_categ;

  public:
    TGame_sel(char *title, menu_item_t *items) : TMenu(title,items) {
	regen_menu(0);
	last_sel = -1;
	current_picture[0] = 0;
	image_counter = 0;
	w_year = h_year = w_categ = h_categ = 0;
    }
    ~TGame_sel() {
	if (short_names) {
	    for (int n=0; n<game_count; n++)
		free((void*)menu[n].label);
	}
	for (int n=1; n<=5; n++) {
	    if (header[n].label) {
		free(header[n].label);
		header[n].label = NULL;
		header[n].values_list[0] =
		    header[n].values_list[1] = 0;
	    }
	}
	free(menu);
#if SDL == 2
	desktop->set_picture(NULL);
#endif
    }
    virtual void compute_nb_items();
    void regen();

  int can_be_displayed(int n) {
    switch (game_list_mode) {
      case 1: // Available games
        if (!avail[n]) {
	  return 0;
	}
	break;
      case 2: // missing games
        if (avail[n])
	  return 0;
	break;
    }
    // other tests...
    if (company && game_list[n]->company_id != company)
      return 0;
    if ((status == 1 && (game_list[n]->flags & (GAME_PARTIALLY_WORKING|GAME_NOT_WORKING))) ||
      (status == 2 && !(game_list[n]->flags & GAME_PARTIALLY_WORKING)) ||
      (status == 3 && !(game_list[n]->flags & GAME_NOT_WORKING)))
      return 0;
    if (category && !(game_list[n]->flags & (1<<(category-1))))
      return 0;
    if (driver && !strstr(game_list[n]->source_file,drivers[driver-1]))
      return 0;
    if (!clones && is_clone(game_list[n]))
	return 0;
    return 1;
  }

  int can_be_selected(int n) {
    return can_be_displayed(n);
  }

  void call_handler() {
    if (!focus && current_game != game_list[sel]) {
      raine_cfg.req_load_game = 1;
      raine_cfg.req_game_index = sel;
      exit_menu = 1;
    } else { // options
      if (hsel) {
	  // a favorite game
	  raine_cfg.req_load_game = 1;
	  raine_cfg.req_game_index = header[hsel].values_list[1];
	  exit_menu = 1;
      } else
	  TMenu::call_handler();
    }
  }

  int load_picture(int nb_to_update, char *buffer, int is_header = 0) {
      if (!strcmp(buffer,current_picture)) {
	// already has loaded this picture
	  if (is_header) {
	      TMenu::update_header_entry(nb_to_update);
	  } else
	      TMenu::update_fg_layer(nb_to_update);
	return 1;
      }

      if (exists(buffer)) {
#if SDL == 1
	SDL_Surface *surf = IMG_Load(buffer);
	if (surf) {
	  setup_bg_layer(surf);
	  SDL_FreeSurface(surf);
	} else {
	  return 0;
	}
	update_fg_layer(-1);
	if (lift) lift->draw();
	// update bg layer on work_area, not bgdst, because we want
	// to clear the whole work_area
	SDL_Rect bg = get_max_area(work_area,fgdst);
	update_bg_layer(&bg);
	SDL_BlitSurface(fg_layer,NULL,sdl_screen,&fgdst);
	do_update(&bg);
#else
	if (desktop->set_picture(buffer))
	    strcpy(current_picture,buffer);
	else
	    return 0;
#endif
	return 1;
      }
      return 0;
  }

  void update_fg_layer(int nb_to_update) {
      if (sel != last_sel && sel >= 0) {
	  image_counter = 0;
	  last_sel = sel;
	  draw_bot_frame();
      } else if (++image_counter == 10 && sel >= 0) {
	  // Wait at least 10 ticks before updating the picture
	  // it allows the selection to be moved smoothly with the mouse
	  // instead of "jumping" when changing pictures all the time
	  char buffer[256];
	  snprintf(buffer,256,"%s%s.pcx",dir_cfg.screen_dir,game_list[sel]->main_name);
	  if (load_picture(nb_to_update,buffer))
	      return;
	  snprintf(buffer,256,"%s%s.png",dir_cfg.screen_dir,game_list[sel]->main_name);
	  if (load_picture(nb_to_update,buffer))
	      return;

	  /* We didn't find any picture for the current game.
	   * Check if it's a clone, and in this case look for a picture for the
	   * parent */

	  const DIR_INFO *head;
	  char *dir;
	  head = game_list[sel]->dir_list;
	  for (; head; head++) {
	      dir = head[0].maindir;

	      if( dir ){

		  if( IS_ROMOF(dir) ){

		      GAME_MAIN *game_romof;

		      game_romof = find_game(dir+1);

		      snprintf(buffer,256,"%s%s.pcx",dir_cfg.screen_dir,game_romof->main_name);
		      if (load_picture(nb_to_update,buffer))
			  return;

		      snprintf(buffer,256,"%s%s.png",dir_cfg.screen_dir,game_romof->main_name);
		      if (load_picture(nb_to_update,buffer))
			  return;
		  }
	      } else
		  break;
	  }
	  // Still here -> no picture at all then !
	  if (current_picture[0]) { // is there a previously loaded picture ?
	      // In this case erase it
#if SDL == 1
	      TMenu::update_fg_layer(nb_to_update);
	      setup_bg_layer(NULL);
	      SDL_Rect bg = get_max_area(work_area,fgdst);
	      update_bg_layer(&bg);
	      SDL_BlitSurface(fg_layer,NULL,sdl_screen,&fgdst);
	      do_update(&bg);
	      current_picture[0] = 0;
	      return;
#else
	      desktop->set_picture(NULL);
#endif
	  }
      }
      TMenu::update_fg_layer(nb_to_update);
  }

  void draw_frame(SDL_Rect *r = NULL);
  void draw_top_frame();
  void draw_bot_frame();
  void regen_menu(int free_labels);
};

void TGame_sel::compute_nb_items() {
    TMenu::compute_nb_items();
    if (current_game) {
	for (sel=0; sel<game_count; sel++)
	    if (game_list[sel]->rom_list == current_game->rom_list)
		break;
	if (sel >= game_count)
	    sel = 0;
	for (int n=0; n<nb_disp_items; n++)
	    if (menu_disp[n] >= sel) {
		sel = menu_disp[n];
		break;
	    }
    }
}

void TGame_sel::draw_top_frame() {
  char mytitle[80];
  char *s;
  int w_title;
  int fw = HMARGIN;
  switch(game_list_mode) {
    case 0: s = _("All"); break;
    case 1: s = _("Avail"); break;
    case 2: s = _("Missing"); break;
  }
  snprintf(mytitle,80,"%s %d",s,nb_disp_items);
  font->dimensions(mytitle,&w_title,&h_title);
#if SDL==2
  boxColor(sdl_screen,0,0,sdl_screen->w,h_title-1,bg_frame_gfx);
#else
  boxColor(sdl_screen,0,0,sdl_screen->w,h_title-1,bg_frame);
#endif
  font->put_string(fw,0,title,fg_frame,bg_frame);
  font->put_string(sdl_screen->w-w_title,0,mytitle,fg_frame,bg_frame);
}

void TGame_sel::draw_bot_frame() {
  if (!w_year) {
    char *year_string = _("Year : 2006");
    font->dimensions(year_string,&w_year,&h_year);
    unsigned int max_game_type = 0, nb_max = 0;
    int n;
    for (n=0; n<NB_GAME_TYPE; n++) {
      if (strlen(game_type[n]) > max_game_type) {
	max_game_type = strlen(game_type[n]);
	nb_max = n;
      }
    }
    char scateg[80];
    snprintf(scateg,80,_("Category : %s"),game_type[nb_max]);
    font->dimensions(scateg,&w_categ,&h_categ);
    h_bot = h_categ + h_year;
  }
  int base = sdl_screen->h-h_bot;
#if SDL==1
  boxColor(sdl_screen,0,base,sdl_screen->w,sdl_screen->h,bg_frame);
#else
  boxColor(sdl_screen,0,base,sdl_screen->w,sdl_screen->h,bg_frame_gfx);
#endif
  char game_string[140],year_string[80],category_string[80],company_string[100];
  snprintf(game_string,140,_("Game : %s"),(sel >= 0 ? game_list[sel]->long_name : "-"));
  snprintf(company_string,100,_("Company : %s"),(sel >= 0 ? game_company_name(game_list[sel]->company_id) : "-"));
  company_string[99] = 0;
  if (sel >= 0) {
    snprintf(year_string,80,_("Year : %d"),game_list[sel]->year);
    sprintf(category_string,_("Category : "));
    int n;
    for (n=1; n<=NB_GAME_TYPE; n++) {
      if (game_list[sel]->flags & (1<<(n-1))) {
	strcat(category_string,game_type[n]);
	strcat(category_string," ");
      }
    }
  } else {
    sprintf(year_string,_("Year : -"));
    sprintf(category_string,_("Category : -"));
  }
  int fw = HMARGIN;
  font->put_string(fw,base,game_string,fg_frame,bg_frame);
  font->put_string(sdl_screen->w-w_year,base,year_string,fg_frame,bg_frame);
  font->put_string(fw,base+h_year,company_string,fg_frame,bg_frame);
  font->put_string(sdl_screen->w-w_categ,base+h_year,category_string,fg_frame,bg_frame);
#if SDL == 1
  SDL_Rect area;
  area.x = 0; area.y = base; area.w = sdl_screen->w; area.h = h_bot;
  if (!(sdl_screen->flags & SDL_DOUBLEBUF))
    do_update(&area);
#endif
}

void TGame_sel::regen() {
  compute_nb_items();
  if (font) {
      // Force the regeneration of the font because it's associated to width_max
      // and we need to recompute this if the game list changed !
      delete font;
      font = NULL;
  }
#if SDL==2
  SDL_DestroyTexture(fg_layer);
  fg_layer = NULL;
#endif
  redraw(NULL);
}

void TGame_sel::draw_frame(SDL_Rect *r) {
  if (!font) setup_font(80);

  draw_top_frame();
  draw_bot_frame();
  int base = sdl_screen->h-h_bot;

  work_area.x = 0;
  work_area.y = h_title;
  work_area.w = sdl_screen->w;
  work_area.h = (base - (h_title));
}

void TGame_sel::regen_menu(int free_labels) {
    int n,freed = 0;
    if (free_labels) {
	for (n=0; n<game_count; n++)
	    free((void*)menu[n].label);
    }
    if (menu) {
	free(menu);
	freed = 1;
    }
    menu = (menu_item_t *)malloc(sizeof(menu_item_t)*(game_count+1));
    for (n=0; n<game_count; n++) {
	if (short_names) {
	    char buf[256];
	    snprintf(buf,256,"\E[31m%s:\E[0m %s",game_list[n]->main_name,game_list[n]->long_name);
	    buf[255] = 0;
	    menu[n].label = strdup(buf);
	} else
	    menu[n].label = game_list[n]->long_name;
      menu[n].menu_func = NULL;
      menu[n].value_int = NULL;
    }
    menu[game_count].label = NULL;
    if (freed)
	compute_nb_items();
}

static TGame_sel *game_sel;

static int change_names(int sel) {
    game_sel->regen_menu(1-short_names);
    return 0;
}

static void compute_avail() {
    for (int n=0; n<game_count; n++)
	avail[n] = game_exists(game_list,n);
}

int recompute_list() {
    // options
    compute_avail();
    game_sel->regen();
    game_sel->draw();
    return 0;
}

static void add_header_info(int n, int index) {
    char s[512];
    sprintf(s,"%s, played %d time%s for ",game_list[index]->long_name,game_list[index]->nb_loaded,(game_list[index]->nb_loaded > 1 ? "s" : ""));
    int t = game_list[index]->time_played;
    int d = t/(24*3600);
    if (d) {
	snprintf(&s[strlen(s)],512-strlen(s),"%d days ",d);
	t %= 24*3600;
    }
    int h = t/3600;
    if (h) {
	snprintf(&s[strlen(s)],512-strlen(s),"%02d:",h);
	t %= 3600;
    }
    int m = t/60;
    if (m) {
	snprintf(&s[strlen(s)],512-strlen(s),"%02d:",m);
	t %= 60;
    }
    snprintf(&s[strlen(s)],512-strlen(s),"%02d",t);
    if (!m && !h)
	snprintf(&s[strlen(s)],512-strlen(s)," seconds");
    header[n].label = strdup(s);
    header[n].values_list[0] = game_list[index]->time_played;
    header[n].values_list[1] = index;
    header[n].menu_func = &do_options; // so that call_handler is called for this one, menu_func is actually ignored
}

static void add_header(int index) {
    int n;
    for (n=1; n<=5; n++) {
	if (((UINT32)header[n].values_list[0]) < game_list[index]->time_played) {
	    if (n < 5) {
		if (header[5].label) {
		    free(header[5].label);
		}
		memmove(&header[n+1],&header[n],sizeof(menu_item_t)*(5-n));
	    }
	    add_header_info(n,index);
	    return;
	}
    }
}

int do_game_sel(int sel) {
    int n;
    if (!avail) {
	avail = (char*)malloc(game_count);
	compute_avail();
    }
    game_sel = new TGame_sel(_("Game selection"),NULL);
    game_sel->set_header(header);
    for (n=0; n<game_count; n++) {
	if (game_list[n]->time_played)
	    add_header(n);
    }

    game_sel->execute();
    delete game_sel;
    return raine_cfg.req_load_game;
}

void done_game_selection() {
    if (avail) {
	free(avail);
	avail = NULL;
    }
}

