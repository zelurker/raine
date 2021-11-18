#include <stdio.h>
#include <SDL2/SDL.h>
#include "gui/menuitem.h"
#include "gui/menu.h"
#include "compat.h"

SDL_Window *win;
SDL_Renderer *rend;

static int play_game(int sel) {
    printf("play game\n");
    return 0;
}

static int quit_raine(int sel) {
    printf("quit\n");
    return 1;
}

static menu_item_t main_items[] =
{
{ "Play game", &play_game, },
{ "Quit", &quit_raine, },
{ NULL, NULL, NULL },
};

int main(int argc, char * argv[])
{
	// Initialize SDL with video
	SDL_Init(SDL_INIT_TIMER|SDL_INIT_AUDIO| SDL_INIT_VIDEO|SDL_INIT_GAMECONTROLLER);
	if ( TTF_Init() < 0 ) {
	    fprintf(stderr, "Couldn't initialize TTF: %s\n",SDL_GetError());
	    SDL_Quit();
	    return -1;
	}

	// Create an SDL window
	win = SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	// if failed to create a window
	if(!win)
	{
		// we'll print an error message and exit
		printf("Error failed to create window!\n");
		return 1;
	}

	rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (rend == NULL)
	{
	    SDL_DestroyWindow(win);
	    printf ("SDL_CreateRenderer Error: %s", SDL_GetError());
	    SDL_Quit();
	    return 3;
	}

	// The init part, usually read from config, here it's direct...
	return_mandatory = 0;
	use_transparency = 1;
	fg_color = mymakecol(255,255,255);
	bg_color = makecol_alpha(0x11,0x07,0x78,128);
	fgframe_color = mymakecol(255,255,255);
	bgframe_color = mymakecol(0,0,128);
	cslider_border = mymakecol(0,0,0);
	cslider_bar = mymakecol(0xc0,0xc0,0xc0);
	cslider_lift = mymakecol(0xff,0xff,0xff);
	bg_dialog_bar = mymakecol(0,0,0);
	keep_vga = 1;
	SDL_PumpEvents();

	TMenu *main_menu = new TMenu(_("Main menu"),main_items);
	main_menu->execute();
	delete main_menu;

	// Destroy the window
	SDL_DestroyWindow(win);

	// And quit SDL
	SDL_Quit();

	return 0;
}
