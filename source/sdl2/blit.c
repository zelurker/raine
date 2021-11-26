#include "raine.h"
#include "blit.h"
#include "SDL.h"
#include "games.h"
#include "video/tilemod.h"
#include "compat.h"
#include "ingame.h"
#include "profile.h"
#include "loadpng.h"
#include "bezel.h"
#include "SDL_gfx/SDL_framerate.h"
#include "palette.h"
#include <math.h>
#include "display_sdl.h"
#include "newspr.h" // init_video_core
#include "opengl.h"
#include "demos.h"

SDL_Surface *sdl_game_bitmap;
extern int disp_screen_x, disp_screen_y;
al_bitmap *GameBitmap = NULL; 	// *Full* Bitmap for generating the game screen (eg. 0,0 to 320+32,240+32)
al_bitmap *GameViewBitmap; 	// *Viewable* Bitmap for saving (pcx) the game screen (eg. 16,16 to 320+16,240+16)

extern void ScreenChange(void);
UINT32 pause_time;
int recording_video = 0,last_video_frame,video_fps;

raine_bitmap GameScreen;

static int destx, desty, xxx, yyy, xoff2, yoff2;

static void my_save_png(char *full_name) {
    if ((display_cfg.video_mode == 0) && ogl.save)
	ogl_save_png(full_name);
    else
	save_png(full_name, GameViewBitmap, pal);
}

static void do_save_screen(void)
{
   char full_name[256];
   char file_name[32];
   char *extension = "png";

   raine_cfg.req_save_screen = 0;

   /*

   first try gamename.pcx

   */

   snprintf(file_name, 32,"%s.%s", current_game->main_name,extension);

   snprintf(full_name, 256,"%s%s", dir_cfg.screen_dir, file_name);


   if (recording_video) {
     int current_video_frame = cpu_frame_count*video_fps / fps;

     if (current_video_frame != last_video_frame) {
       snprintf(full_name,256,"%s%s_%06d.%s",dir_cfg.screen_dir, current_game->main_name,cpu_frame_count,extension);
       my_save_png(full_name);
       last_video_frame = current_video_frame;
     }
   } else {

     /*

     otherwise, find the next gamename_NNN.pcx (or gamenNNN.pcx)

     */

     while( exists(full_name) ){
       if(dir_cfg.last_screenshot_num > 999)
	 return;
       if(dir_cfg.long_file_names)
	 snprintf(file_name,32, "%s_%03d.%s", current_game->main_name, dir_cfg.last_screenshot_num++,extension);
       else
	 snprintf(file_name,32, "%.5s%03d.%s", current_game->main_name, dir_cfg.last_screenshot_num++,extension);
       snprintf(full_name,256, "%s%s", dir_cfg.screen_dir, file_name);
     };
     my_save_png(full_name);
     print_ingame(120, gettext("Screen Saved to %s"), file_name);
   }

}

SDL_Rect area_overlay,area2;

void get_overlay_area(int *x, int *y, int *w, int *h) {
  *x = area_overlay.x;
  *y = area_overlay.y;
  *w = area_overlay.w;
  *h = area_overlay.h;
}

void ReClipScreen(void)
{
   // clip x
 int game_x,game_y;
 double ratio1, ratio2;

  // I need to use 2 sets of xview/yview because destx/desty must be calculated
  // taking into account the scaling factor, but the width and height must be
  // unscaled (it's automatically multiplied during the blit)
  int xview,yview,xview2,yview2;

  xview2 = xview = GameScreen.xview;
  yview2 = yview = GameScreen.yview;
  yyy = yview;

  // bezel_fix_screen_size(&xview,&yview);

  if(xview2 < disp_screen_x){
    destx = (disp_screen_x - xview2)>>1;
  }
  else{
    destx = 0;
  }
  if (xview < disp_screen_x) {
    // We can't write outside the game bitmap so we have to use xview
    xxx = xview;
    xoff2 = GameScreen.xtop;
  } else {
    // If the game bitmap is bigger than the screen, then no risk (if stretched)
    if(xview <= disp_screen_x){
      xxx = xview;
      xoff2 = GameScreen.xtop;
    } else {
      xxx = disp_screen_x;
      xoff2 = GameScreen.xtop + ((xview - disp_screen_x)>>1);
    }
  }

  // clip y

  if(yview2 < disp_screen_y){
    desty = (disp_screen_y - yview2)>>1;
  }
  else{
    desty = 0;
  }

 if (current_game) {
     // if (reading_demo) display_cfg.screen_y -= 8;
     game_x = xxx;
     game_y = yyy;
     ratio1 = display_cfg.screen_x*1.0/game_x;
     ratio2 = display_cfg.screen_y*1.0/game_y;
     // printf("ratio1 %g ratio2 %g game_x %d game_y %d\n",ratio1,ratio2,game_x,game_y);

     if (display_cfg.video_mode == 0) { // opengl
	 if (display_cfg.fix_aspect_ratio ) {
	     /* Fixing the aspect ratio must be done on the game resolution and not
	      * on the screen resolution, otherwise the screen size will change
	      * everytime we return to the game */
	     if (game_x > game_y) {
		 double ratio = game_y*1.0/game_x;
		 double dif_ratio = fabs(ratio-0.75);
		 if (dif_ratio > 0.001 && dif_ratio < 0.05) {
		     game_y = 0.75*game_x;
		 }
	     } else {
		 double ratio = game_x*1.0/game_x;
		 double dif_ratio = fabs(ratio-0.75);
		 if (dif_ratio > 0.001 && dif_ratio < 0.05) {
		     game_x = 0.75*game_y;
		 }
	     }
	 }

	 /* Now fix the aspect ratio of the overlay inside the game screen */
	 int xxx2,yyy2,destx2,desty2;
	 if (ratio1 < ratio2) {
	     xxx2 = display_cfg.screen_x;
	     yyy2 = round(ratio1 * game_y);
	     destx2 = 0;
	     desty2 = (display_cfg.screen_y - yyy2)/2;
	     if (desty2 < 0) {
		 /* Out of limits, harmless for texture, creates black screen
		  * in win32 when using drawpixels.
		  * Anyway should investigate why screen_y < game_y*ratio1
		  * here one day, all this will be rewritten for sdl2 anyway */
		 desty2 = 0;
		 yyy2 = display_cfg.screen_y;
	     }
	 } else {
	     yyy2 = display_cfg.screen_y;
	     xxx2 = round(ratio2 * game_x);
	     desty2 = 0;
	     destx2 = (display_cfg.screen_x - xxx2) /2;
	     if (destx2 < 0) {
		 // copy the out of limits code from ratio1 < ratio2
		 destx2 = 0;
		 xxx2 = display_cfg.screen_x;
	     }
	 }
#ifdef USE_BEZELS
	 bezel_fix_screen_size(&xxx2,&yyy2);
	 bezel_fix_screen_coordinates(&destx2,&desty2,xxx2,yyy2,display_cfg.screen_x,display_cfg.screen_y);
#endif
#ifdef DARWIN
	 if (display_cfg.video_mode == 1 && overlays_workarounds) {
	     // I have a bug here which makes the overlay invisible if it doesn't start
	     // at the top of the screen !!!
	     // It seems so stupid that it's probably a bug of my video driver, but to
	     // be sure I'll put a fix here, it's better than a black screen...
	     area_overlay.x = destx2 = 0;
	     area_overlay.y = desty2 = 0;
	 }
#endif
	 if (display_cfg.keep_ratio) {
	     area_overlay.x = destx2;
	     area_overlay.y = desty2;
	     area_overlay.w = xxx2;
	     area_overlay.h = yyy2;
	 } else {
	     area_overlay.x = area_overlay.y = 0;
	     area_overlay.w = display_cfg.screen_x;
	     area_overlay.h = display_cfg.screen_y;
	 }
	 print_debug("area overlay %d %d %d %d with xxx %d yyy %d display %d %d ratio1 %g ratio2 %g\n",destx2,desty2,xxx2,yyy2,xxx,yyy,display_cfg.screen_x,display_cfg.screen_y,ratio1,ratio2);
     }
     area2.x = destx; // x2;
     area2.y = desty; // y2;
     area2.w = xxx;
     area2.h = yyy;
     if (GameViewBitmap)
	 clear_bitmap(GameViewBitmap);

     RefreshBuffers=1;
     // if (reading_demo) display_cfg.screen_y += 8;
 }

#ifdef USE_BEZELS
  if(yview2 <= disp_screen_y && xview2 <= disp_screen_x && current_game)
    bezel_fix_screen_coordinates(&destx,&desty,xview2,yview2,disp_screen_x,disp_screen_y);
#endif

  if(yview < disp_screen_y){
    yyy = yview;
    yoff2 = GameScreen.ytop;
  } else {
    if(yview <= disp_screen_y){
      yyy = yview;
      yoff2 = GameScreen.ytop;
    } else {
      yyy = disp_screen_y;
      yoff2 = GameScreen.ytop + ((yview - disp_screen_y)>>1);
    }
  }

  // GameScreen.xview = oldxview;
  // GameScreen.yview = oldyview;
}

void DrawPaused(void)
{
   // blit(pause_buffer, GameBitmap, xoff2, yoff2, xoff2, yoff2, xxx, yyy);

   DrawNormal();		// Overlay text interface, blit to screen

   pause_time++;

   if(display_cfg.limit_speed==1){
	      SDL_framerateDelay(&fpsm);
   }
}

static void SetScreenBitmap(int xfull, int yfull, int xtop, int ytop, int xview, int yview)
{
   const VIDEO_INFO *vid_info;

   vid_info = current_game->video;

   display_cfg.rotate = 0;
   display_cfg.flip = 0;

   if(vid_info->flags & VIDEO_ROTATABLE){

      /*

      check if we use the game rotation

      */

      if(!display_cfg.no_rotate)

	 display_cfg.rotate = VIDEO_ROTATE( vid_info->flags );

      /*

      check if we use the game flipping

      */

      if(!display_cfg.no_flip)

	 display_cfg.flip = VIDEO_FLIP( vid_info->flags );

      /*

      add user rotation

      */

      display_cfg.rotate += display_cfg.user_rotate;
      display_cfg.rotate &= 3;

      /*

      add user flipping

      */

      display_cfg.flip ^= display_cfg.user_flip;

   }

   switch(display_cfg.rotate){
   case 0x00:
   case 2:
      GameScreen.xfull=xfull;
      GameScreen.yfull=yfull;
      GameScreen.xtop =xtop;
      GameScreen.ytop =ytop;
      GameScreen.xview=xview;
      GameScreen.yview=yview;
   break;
   case 0x01:
   case 3:
      GameScreen.xfull=yfull;
      GameScreen.yfull=xfull;
      GameScreen.xtop =ytop;
      GameScreen.ytop =xtop;
      GameScreen.xview=yview;
      GameScreen.yview=xview;
   break;
   }

   check_tile_rotation();

   disp_screen_y = display_cfg.screen_y;
   disp_screen_x = display_cfg.screen_x;

   ReClipScreen();
}

void SetupScreenBitmap(void)
{
 const VIDEO_INFO *vid_info;
 int oldbpp;

 if (!current_game)
   return;

 vid_info = current_game->video;

 SetScreenBitmap(
    vid_info->screen_x + vid_info->border_size + vid_info->border_size,
    vid_info->screen_y + vid_info->border_size + vid_info->border_size,
    vid_info->border_size,
    vid_info->border_size,
    vid_info->screen_x,
    vid_info->screen_y
 );

 if (!GameBitmap || (GameBitmap->w != GameScreen.xfull || GameBitmap->h != GameScreen.yfull) ||
	 (display_cfg.video_mode == 0 && display_cfg.bpp != 16 && display_cfg.bpp != 32) ) {
   print_debug("regen GameBitmap\n");
   do {
     oldbpp = display_cfg.bpp;
     if (GameBitmap) {
       DestroyScreenBitmap(); // destroys gameviewbitmap too
     }
     GameBitmap = sdl_create_bitmap_ex(internal_bpp(display_cfg.bpp), GameScreen.xfull, GameScreen.yfull);
     sdl_game_bitmap = get_surface_from_bmp(GameBitmap);
     // GameBitmap = sdl_create_overlay(GameScreen.xfull, GameScreen.yfull);
     // GameBitmap = create_system_bitmap( GameScreen.xfull, GameScreen.yfull);
     GameViewBitmap = sdl_create_sub_bitmap(GameBitmap, GameScreen.xtop, GameScreen.ytop, GameScreen.xview, GameScreen.yview);
     if (game_tex) {
	 printf("Already a game texture\n");
	 exit(1);
     }
     game_tex = SDL_CreateTexture(rend,SDL_PIXELFORMAT_RGBX8888,SDL_TEXTUREACCESS_STREAMING,GameScreen.xview,GameScreen.yview);
     if (!game_tex) {
	 printf("couldn't create game texture : %s\n",SDL_GetError());
	 exit(1);
     }
     SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");  // make the scaled rendering look smoother.
     if (display_cfg.video_mode == 0)
	 ScreenChange();
     else
	 SDL_RenderSetLogicalSize(rend, GameScreen.xview, GameScreen.yview);

   } while (oldbpp != display_cfg.bpp);
   init_video_core(); // GameBitmap just changed -> regen all the functions
   // which depend on it !
 }
}

void DestroyScreenBitmap(void)
{
  destroy_bitmap(GameBitmap);
  destroy_bitmap(GameViewBitmap);
  if (game_tex)
      SDL_DestroyTexture(game_tex);
  game_tex = NULL;
  GameBitmap = NULL;
  GameViewBitmap = NULL;
  sdl_game_bitmap = NULL;
}

extern void *old_draw; // dlg_sound

void DrawNormal(void)
{
   /*

   save screenshots now (before we overwrite the image)

   */
  if (old_draw) return;

   /*

   scroll game screen

   */

   if (raine_cfg.req_pause_scroll) {
     if(raine_cfg.req_pause_scroll & 1)
        if(yoff2 > GameScreen.ytop)
  	  yoff2--;

     if(raine_cfg.req_pause_scroll & 2)
        if((yoff2 + disp_screen_y) < (GameScreen.ytop + GameScreen.yview))
	  yoff2++;

     if(raine_cfg.req_pause_scroll & 4)
        if(xoff2 > GameScreen.xtop)
	  xoff2--;

     if(raine_cfg.req_pause_scroll & 8)
        if((xoff2 + disp_screen_x) < (GameScreen.xtop + GameScreen.xview))
	  xoff2++;

     raine_cfg.req_pause_scroll = 0;
   }

   if(raine_cfg.req_save_screen == 1)
      do_save_screen();

   /*

   draw the ingame 'interface' (message list, fps)

   */

   if (!ogl.overlay || display_cfg.video_mode != 0)
       overlay_ingame_interface(0);
   else
       overlay_ingame_interface(1);
   if (display_cfg.video_mode == 0)
       finish_opengl(); // overlay interface eventually !

   if (display_cfg.video_mode == 0) {
       draw_opengl(ogl.filter);
       RefreshBuffers = 0;
       return;
   }

   if (!game_tex)
       game_tex = SDL_CreateTexture(rend,SDL_PIXELFORMAT_RGBX8888,SDL_TEXTUREACCESS_STREAMING,GameScreen.xview,GameScreen.yview);
   if (!game_tex) {
       printf("no texture\n");
       exit(1);
   }
   SDL_UpdateTexture(game_tex,NULL,GameViewBitmap->line[0],GameScreen.xfull*sizeof(UINT32));
   SDL_SetRenderDrawColor(rend, 0x0, 0x0, 0x0, 0xFF);
   SDL_RenderClear(rend);
   SDL_RenderCopy(rend,game_tex,NULL,NULL);
   SDL_RenderPresent(rend);

   RefreshBuffers = 0;
}

void clear_game_screen(int pen)
{
  int x,y,len;
  switch(display_cfg.bpp) {
  case 8:
    len = GameViewBitmap->w;
    for (y=0; y<GameViewBitmap->h; y++)
      memset(GameViewBitmap->line[y],pen,len);
    break;
  case 15:
  case 16:
    len = GameViewBitmap->w*2;
    pen = (pen & 0xffff) | (pen<<16);
    for (y=0; y<GameViewBitmap->h; y++) {
      unsigned char *src = GameViewBitmap->line[y];
      for (x=0; x<len; x+= 4) {
	WriteLong(&src[x],pen);
      }
    }
    break;
  case 32:
    len = GameViewBitmap->w*4;
    for (y=0; y<GameViewBitmap->h; y++) {
      unsigned char *src = GameViewBitmap->line[y];
      for (x=0; x<len; x+= 4) {
	WriteLong(&src[x],pen);
      }
    }
    break;
  }
}

void get_screen_coordinates(int *Xoff2,int *Yoff2, int *Destx, int *Desty, int *Xxx, int *Yyy){
  *Xoff2 = xoff2;
  *Yoff2 = yoff2;
  *Xxx = xxx;
  *Yyy = yyy;
  *Destx = destx;
  *Desty = desty;
}

