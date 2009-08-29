/******************************************************************************/
/*                                                                            */
/*                         RAINE 16x16 TILE DRAWING                           */
/*                                                                            */
/******************************************************************************/

#include "deftypes.h"
#include "raine.h"
#include "blit.h"

/*

Mapped
Transparent Mapped
Direct-Mapped
Transparent Direct-Mapped

*/

void init_spr16x16asm(void)
{
}

static BITMAP *mask_bitmap;
void init_spr16x16asm_mask(BITMAP *my_bitmap)
{
  mask_bitmap = my_bitmap;
}

// 16x16 mapped sprites

void Draw16x16_Mapped(UINT8 *SPR, int x, int y, UINT8 *cmap)
{
   UINT8 *line;
   int xx,yy;

   for(yy=0; yy<16; yy++){
      line = GameBitmap->line[y+yy] + x;
      for(xx=0; xx<16; xx++){
         *line++ = cmap[ *SPR++ ];
      }
   }
}

void Draw16x16_Mapped_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap)
{
   UINT8 *line;
   int xx,yy;

   for(yy=15; yy>=0; yy--){
      line = GameBitmap->line[y+yy] + x;
      for(xx=0; xx<16; xx++){
         *line++ = cmap[ *SPR++ ];
      }
   }
}

void Draw16x16_Mapped_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap)
{
   UINT8 *line;
   int xx,yy;

   for(yy=0; yy<16; yy++){
      line = GameBitmap->line[y+yy] + x + 15;
      for(xx=0; xx<16; xx++){
         *line-- = cmap[ *SPR++ ];
      }
   }
}

void Draw16x16_Mapped_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap)
{
   UINT8 *line;
   int xx,yy;

   for(yy=15; yy>=0; yy--){
      line = GameBitmap->line[y+yy] + x + 15;
      for(xx=0; xx<16; xx++){
         *line-- = cmap[ *SPR++ ];
      }
   }
}

// 16x16 mapped transparent sprites

void Draw16x16_Trans_Mapped(UINT8 *SPR, int x, int y, UINT8 *cmap)
{
   UINT8 *line;
   int xx,yy;

   for(yy=0; yy<16; yy++){
      line = GameBitmap->line[y+yy] + x;
      for(xx=0; xx<16; xx++, SPR++){
         if(*SPR)
            line[xx] = cmap[ *SPR ];
      }
   }
}

void Draw16x16_Trans_Mapped_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap)
{
   UINT8 *line;
   int xx,yy;

   for(yy=15; yy>=0; yy--){
      line = GameBitmap->line[y+yy] + x;
      for(xx=0; xx<16; xx++, SPR++){
         if(*SPR)
            line[xx] = cmap[ *SPR ];
      }
   }
}

void Draw16x16_Trans_Mapped_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap)
{
   UINT8 *line;
   int xx,yy;

   for(yy=0; yy<16; yy++){
      line = GameBitmap->line[y+yy] + x;
      for(xx=15; xx>=0; xx--, SPR++){
         if(*SPR)
            line[xx] = cmap[ *SPR ];
      }
   }
}

void Draw16x16_Trans_Mapped_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap)
{
   UINT8 *line;
   int xx,yy;

   for(yy=15; yy>=0; yy--){
      line = GameBitmap->line[y+yy] + x;
      for(xx=15; xx>=0; xx--, SPR++){
         if(*SPR)
            line[xx] = cmap[ *SPR ];
      }
   }
}

// 16x16 sprites

void Draw16x16(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT8 *line;
   int xx,yy;

   for(yy=0; yy<16; yy++){
      line = GameBitmap->line[y+yy] + x;
      for(xx=0; xx<16; xx++){
         *line++ = *SPR++ | cmap;
      }
   }
}

void Draw16x16_FlipX(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT8 *line;
   int xx,yy;

   for(yy=15; yy>=0; yy--){
      line = GameBitmap->line[y+yy] + x;
      for(xx=0; xx<16; xx++){
         *line++ = *SPR++ | cmap;
      }
   }
}

void Draw16x16_FlipY(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT8 *line;
   int xx,yy;

   for(yy=0; yy<16; yy++){
      line = GameBitmap->line[y+yy] + x + 15;
      for(xx=0; xx<16; xx++){
         *line-- = *SPR++ | cmap;
      }
   }
}

void Draw16x16_FlipXY(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT8 *line;
   int xx,yy;

   for(yy=15; yy>=0; yy--){
      line = GameBitmap->line[y+yy] + x + 15;
      for(xx=0; xx<16; xx++){
         *line-- = *SPR++ | cmap;
      }
   }
}

// Draw16x16_Mask : just one 16x16 opaque block of the same colour

void Draw16x16_Mask(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT8 *line;
   int xx,yy;
   UINT32 mask = cmap | (cmap << 8);
   mask |= (mask << 16);

   for(yy=0; yy<16; yy++){
      line = mask_bitmap->line[y+yy] + x;
      for(xx=0; xx<16; xx+=4){
         WriteLong(&line[x],mask);
      }
   }
}

// 16x16 transparent sprites

void Draw16x16_Trans(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT8 *line;
   int xx,yy;

   for(yy=0; yy<16; yy++){
      line = GameBitmap->line[y+yy] + x;
      for(xx=0; xx<16; xx++, SPR++){
         if(*SPR)
            line[xx] = *SPR | cmap;
      }
   }
}

void Draw16x16_Trans_FlipX(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT8 *line;
   int xx,yy;

   for(yy=15; yy>=0; yy--){
      line = GameBitmap->line[y+yy] + x;
      for(xx=0; xx<16; xx++, SPR++){
         if(*SPR)
            line[xx] = *SPR | cmap;
      }
   }
}

void Draw16x16_Trans_FlipY(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT8 *line;
   int xx,yy;

   for(yy=0; yy<16; yy++){
      line = GameBitmap->line[y+yy] + x;
      for(xx=15; xx>=0; xx--, SPR++){
         if(*SPR)
            line[xx] = *SPR | cmap;
      }
   }
}

void Draw16x16_Trans_FlipXY(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT8 *line;
   int xx,yy;

   for(yy=15; yy>=0; yy--){
      line = GameBitmap->line[y+yy] + x;
      for(xx=15; xx>=0; xx--, SPR++){
         if(*SPR)
            line[xx] = *SPR | cmap;
      }
   }
}

// Draw16x16_Mask_Trans

void Draw16x16_Mask_Trans(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT8 *line;
   int xx,yy;

   for(yy=0; yy<16; yy++){
      line = mask_bitmap->line[y+yy] + x;
      for(xx=0; xx<16; xx++, SPR++){
         if(*SPR)
            line[xx] = cmap;
      }
   }
}

void Draw16x16_Mask_Trans_FlipX(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT8 *line;
   int xx,yy;

   for(yy=15; yy>=0; yy--){
      line = mask_bitmap->line[y+yy] + x;
      for(xx=0; xx<16; xx++, SPR++){
         if(*SPR)
            line[xx] = cmap;
      }
   }
}

void Draw16x16_Mask_Trans_FlipY(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT8 *line;
   int xx,yy;

   for(yy=0; yy<16; yy++){
      line = mask_bitmap->line[y+yy] + x;
      for(xx=15; xx>=0; xx--, SPR++){
         if(*SPR)
            line[xx] = cmap;
      }
   }
}

void Draw16x16_Mask_Trans_FlipXY(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT8 *line;
   int xx,yy;

   for(yy=15; yy>=0; yy--){
      line = mask_bitmap->line[y+yy] + x;
      for(xx=15; xx>=0; xx--, SPR++){
         if(*SPR)
            line[xx] = cmap;
      }
   }
}
