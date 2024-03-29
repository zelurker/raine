/*
  This module handles the sprite of the gun, required in games like space gun,
  operation wolf...
*/

#include "raine.h"
#include "compat_sdl.h"
#include "palette.h"
#include "spr16x16.h"

static UINT8 SpriteGun1[] =
{
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0xfe,0xfc,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0xfe,0xfc,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xfe,0xfc,0xfc,0xfc,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0xfe,0xfc,0xfe,0xfe,0xfe,0xfc,0xfe,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0xfe,0xfc,0xfe,0x00,0x00,0x00,0xfe,0xfc,0xfe,0x00,0x00,0x00,0x00,
0x00,0xfe,0xfe,0xfc,0xfe,0x00,0x00,0x00,0x00,0x00,0xfe,0xfc,0xfe,0xfe,0x00,0x00,
0xfe,0xfc,0xfc,0xfc,0xfe,0x00,0x00,0x00,0x00,0x00,0xfe,0xfc,0xfc,0xfc,0xfe,0x00,
0x00,0xfe,0xfe,0xfc,0xfe,0x00,0x00,0x00,0x00,0x00,0xfe,0xfc,0xfe,0xfe,0x00,0x00,
0x00,0x00,0x00,0xfe,0xfc,0xfe,0x00,0x00,0x00,0xfe,0xfc,0xfe,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0xfe,0xfc,0xfe,0xfe,0xfe,0xfc,0xfe,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xfe,0xfc,0xfc,0xfc,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0xfe,0xfc,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0xfe,0xfc,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

static UINT8 SpriteGun2[] =
{
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0xfe,0xfd,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0xfe,0xfd,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xfe,0xfd,0xfd,0xfd,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0xfe,0xfd,0xfe,0xfe,0xfe,0xfd,0xfe,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0xfe,0xfd,0xfe,0x00,0x00,0x00,0xfe,0xfd,0xfe,0x00,0x00,0x00,0x00,
0x00,0xfe,0xfe,0xfd,0xfe,0x00,0x00,0x00,0x00,0x00,0xfe,0xfd,0xfe,0xfe,0x00,0x00,
0xfe,0xfd,0xfd,0xfd,0xfe,0x00,0x00,0x00,0x00,0x00,0xfe,0xfd,0xfd,0xfd,0xfe,0x00,
0x00,0xfe,0xfe,0xfd,0xfe,0x00,0x00,0x00,0x00,0x00,0xfe,0xfd,0xfe,0xfe,0x00,0x00,
0x00,0x00,0x00,0xfe,0xfd,0xfe,0x00,0x00,0x00,0xfe,0xfd,0xfe,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0xfe,0xfd,0xfe,0xfe,0xfe,0xfd,0xfe,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xfe,0xfd,0xfd,0xfd,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0xfe,0xfd,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0xfe,0xfd,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

static UINT8 *sprite_gun[2];
static UINT8 map[256*4];
static int old_bpp = 0;

static void update_palette() {
  // required only in 8bpp, update the 3 needed colors for this sprite
  /* Notice : when in 8bpp, this function is called once for each sprite ->
     one time too much ! The advantage being that it's hiden completely from the game
     driver and not called at all when in bpp > 8. It is *VERY* acceptable ! */
#ifdef SDL
     int max = 256;
#else
     int max = 64;
#endif

   pal[254].r=0;
   pal[254].g=0;
   pal[254].b=0;

   pal[253].r=max/2-1;
   pal[253].g=max/2-1;
   pal[253].b=max-1;

   pal[252].r=max-1;
   pal[252].g=max/2-1;
   pal[252].b=max/2-1;
}

void disp_gun(int nb, int x,int y) {
  // nb is 0 for player 1 and 1 for player 2
  if (old_bpp != display_cfg.bpp) {
    sprite_gun[0] = SpriteGun1;
    sprite_gun[1] = SpriteGun2;

    if (display_cfg.bpp == 16 || display_cfg.bpp == 15) {
      int n;
      update_palette();
      for (n=252; n<=254; n++) {
	UINT16 res;
	GET_PEN_FOR_COLOUR_16(
			      pal[n].r,
			      pal[n].g,
			      pal[n].b,
			      res);
	WriteWord(&map[n*2],res);
      }
    }
    old_bpp = display_cfg.bpp;
  } else if (display_cfg.bpp == 32) {
      int n;
      update_palette();
      for (n=252; n<=254; n++) {
	UINT32 res;
	GET_PEN_FOR_COLOUR_32(
			      pal[n].r,
			      pal[n].g,
			      pal[n].b,
			      res);
	WriteLong(&map[n*4],res);
      }
    }
    old_bpp = display_cfg.bpp;

  switch(display_cfg.bpp) {
  case 8:  Draw16x16_Trans_8(sprite_gun[nb],x,y,0); update_palette(); break;
  case 15:
  case 16: Draw16x16_Trans_Mapped_16(sprite_gun[nb],x,y,map); break;
  case 32: Draw16x16_Trans_Mapped_32(sprite_gun[nb],x,y,map); break;
  }
}

