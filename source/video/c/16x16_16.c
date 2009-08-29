/******************************************************************************/
/*                                                                            */
/*                         RAINE 16x16 TILE DRAWING                           */
/*                                                                            */
/******************************************************************************/

#include "deftypes.h"
#include "raine.h"
#include "emudx.h"
#include "blit.h"

/*

Mapped
Transparent Mapped
Direct-Mapped
Transparent Direct-Mapped

*/

void init_spr16x16asm_16(void)
{
}

// 16x16 mapped sprites

void Draw16x16_Mapped_16(UINT8 *SPR, int x, int y, UINT8 *cmap)
{
   UINT16 *line;
   int xx,yy;

   for(yy=0; yy<16; yy++){
      line = ((UINT16 *)(GameBitmap->line[y+yy]))+ x;
      for(xx=0; xx<16; xx++){
         *line++ = ((UINT16 *)cmap)[ *SPR++ ];
      }
   }
}

void Draw16x16_Mapped_16_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap)
{
   UINT16 *line;
   int xx,yy;

   for(yy=15; yy>=0; yy--){
      line = ((UINT16 *)(GameBitmap->line[y+yy]))+ x;
      for(xx=0; xx<16; xx++){
         *line++ = ((UINT16 *)cmap)[ *SPR++ ];
      }
   }
}

void Draw16x16_Mapped_16_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap)
{
   UINT16 *line;
   int xx,yy;

   for(yy=0; yy<16; yy++){
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x + 15;
      for(xx=0; xx<16; xx++){
         *line-- = ((UINT16 *)cmap)[ *SPR++ ];
      }
   }
}

void Draw16x16_Mapped_16_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap)
{
   UINT16 *line;
   int xx,yy;

   for(yy=15; yy>=0; yy--){
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x + 15;
      for(xx=0; xx<16; xx++){
         *line-- = ((UINT16 *)cmap)[ *SPR++ ];
      }
   }
}

// 16x16 mapped transparent sprites

void Draw16x16_Trans_Mapped_16(UINT8 *SPR, int x, int y, UINT8 *cmap)
{
   UINT16 *line;
   int xx,yy;

   for(yy=0; yy<16; yy++){
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x;
      for(xx=0; xx<16; xx++, SPR++){
         if(*SPR)
            line[xx] = ((UINT16 *)cmap)[ *SPR ];
      }
   }
}

void Draw16x16_Trans_Mapped_16_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap)
{
   UINT16 *line;
   int xx,yy;

   for(yy=15; yy>=0; yy--){
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x;
      for(xx=0; xx<16; xx++, SPR++){
         if(*SPR)
            line[xx] = ((UINT16 *)cmap)[ *SPR ];
      }
   }
}

void Draw16x16_Trans_Mapped_16_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap)
{
   UINT16 *line;
   int xx,yy;

   for(yy=0; yy<16; yy++){
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x;
      for(xx=15; xx>=0; xx--, SPR++){
         if(*SPR)
            line[xx] = ((UINT16 *)cmap)[ *SPR ];
      }
   }
}

void Draw16x16_Trans_Mapped_16_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap)
{
   UINT16 *line;
   int xx,yy;

   for(yy=15; yy>=0; yy--){
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x;
      for(xx=15; xx>=0; xx--, SPR++){
         if(*SPR)
            line[xx] = ((UINT16 *)cmap)[ *SPR ];
      }
   }
}

// 16x16 sprites
/* These are for emudx */

void Draw16x16_16(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT16 *line;
   int xx,yy;

   for(yy=0; yy<16; yy++){
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x;
      for(xx=0; xx<16; xx++){
         *line++ = ReadWord(SPR); SPR+=2;
      }
   }
}

void Draw16x16_16_FlipX(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT16 *line;
   int xx,yy;

   for(yy=15; yy>=0; yy--){
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x;
      for(xx=0; xx<16; xx++){
         *line++ = ReadWord(SPR); SPR+=2;
      }
   }
}

void Draw16x16_16_FlipY(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT16 *line;
   int xx,yy;

   for(yy=0; yy<16; yy++){
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x + 15;
      for(xx=0; xx<16; xx++){
         *line-- = ReadWord(SPR); SPR+=2;
      }
   }
}

void Draw16x16_16_FlipXY(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT16 *line;
   int xx,yy;

   for(yy=15; yy>=0; yy--){
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x + 15;
      for(xx=0; xx<16; xx++){
         *line-- = ReadWord(SPR); SPR+=2;
      }
   }
}

// 16x16 transparent sprites

void Draw16x16_Trans_16(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT16 *line;
   int xx,yy;

   for(yy=0; yy<16; yy++){
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x;
      for(xx=0; xx<16; xx++, SPR+=2){
         if(ReadWord(SPR) != emudx_transp)
            line[xx] = ReadWord(SPR);
      }
   }
}

void Draw16x16_Trans_16_FlipX(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT16 *line;
   int xx,yy;

   for(yy=15; yy>=0; yy--){
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x;
      for(xx=0; xx<16; xx++, SPR+=2){
         if(ReadWord(SPR) != emudx_transp)
            line[xx] = ReadWord(SPR);
      }
   }
}

void Draw16x16_Trans_16_FlipY(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT16 *line;
   int xx,yy;

   for(yy=0; yy<16; yy++){
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x;
      for(xx=15; xx>=0; xx--, SPR+=2){
         if(ReadWord(SPR) != emudx_transp)
            line[xx] = ReadWord(SPR);
      }
   }
}

void Draw16x16_Trans_16_FlipXY(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT16 *line;
   int xx,yy;

   for(yy=15; yy>=0; yy--){
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x;
      for(xx=15; xx>=0; xx--, SPR+=2){
         if(ReadWord(SPR) != emudx_transp)
            line[xx] = ReadWord(SPR);
      }
   }
}
