/******************************************************************************/
/*                                                                            */
/*                          RAINE 8x8 TILE DRAWING                            */
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

void init_spr8x8asm_16(void)
{
}

// 8x8 mapped sprites

void Draw8x8_Mapped_16(UINT8 *SPR, int x, int y, UINT8 *cmap)
{
   UINT16 *line;
   int xx,yy;

   for(yy=0; yy<8; yy++)
   {
      line = ((UINT16 *)(GameBitmap->line[y+yy]))+ x;
      for(xx=0; xx<8; xx++)
      {
         *line++ = ((UINT16 *)cmap)[ *SPR++ ];
      }
   }
}

void Draw8x8_Mapped_16_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap)
{
   UINT16 *line;
   int xx,yy;

   for(yy=7; yy>=0; yy--)
   {
      line = ((UINT16 *)(GameBitmap->line[y+yy]))+ x;
      for(xx=0; xx<8; xx++)
      {
         *line++ = ((UINT16 *)cmap)[ *SPR++ ];
      }
   }
}

void Draw8x8_Mapped_16_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap)
{
   UINT16 *line;
   int xx,yy;

   for(yy=0; yy<8; yy++)
   {
      line = ((UINT16 *)(GameBitmap->line[y+yy]))+ x + 7;
      for(xx=0; xx<8; xx++)
      {
         *line-- = ((UINT16 *)cmap)[ *SPR++ ];
      }
   }
}

void Draw8x8_Mapped_16_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap)
{
   UINT16 *line;
   int xx,yy;

   for(yy=7; yy>=0; yy--)
   {
      line = ((UINT16 *)(GameBitmap->line[y+yy]))+ x + 7;
      for(xx=0; xx<8; xx++)
      {
         *line-- = ((UINT16 *)cmap)[ *SPR++ ];
      }
   }
}

// 8x8 mapped transparent sprites

void Draw8x8_Trans_Mapped_16(UINT8 *SPR, int x, int y, UINT8 *cmap)
{
   UINT16 *line;
   int xx,yy;

   for(yy=0; yy<8; yy++)
   {
      line = ((UINT16 *)(GameBitmap->line[y+yy]))+ x;
      for(xx=0; xx<8; xx++, SPR++)
      {
         if(*SPR)
            line[xx] = ((UINT16 *)cmap)[ *SPR ];
      }
   }
}

void Draw8x8_Trans_Mapped_16_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap)
{
   UINT16 *line;
   int xx,yy;

   for(yy=7; yy>=0; yy--)
   {
      line = ((UINT16 *)(GameBitmap->line[y+yy]))+ x;
      for(xx=0; xx<8; xx++, SPR++)
      {
         if(*SPR)
            line[xx] = ((UINT16 *)cmap)[ *SPR ];
      }
   }
}

void Draw8x8_Trans_Mapped_16_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap)
{
   UINT16 *line;
   int xx,yy;

   for(yy=0; yy<8; yy++)
   {
      line = ((UINT16 *)(GameBitmap->line[y+yy]))+ x;
      for(xx=7; xx>=0; xx--, SPR++)
      {
         if(*SPR)
            line[xx] = ((UINT16 *)cmap)[ *SPR ];
      }
   }
}

void Draw8x8_Trans_Mapped_16_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap)
{
   UINT16 *line;
   int xx,yy;

   for(yy=7; yy>=0; yy--)
   {
      line = ((UINT16 *)(GameBitmap->line[y+yy]))+ x;
      for(xx=7; xx>=0; xx--, SPR++)
      {
         if(*SPR)
            line[xx] = ((UINT16 *)cmap)[ *SPR ];
      }
   }
}

// 8x8 sprites

void Draw8x8_16(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT16 *line;
   int xx,yy;

   for(yy=0; yy<8; yy++)
   {
      line = ((UINT16 *)(GameBitmap->line[y+yy]))+ x;
      for(xx=0; xx<8; xx++)
      {
         *line++ = *SPR++ | cmap;
      }
   }
}

void Draw8x8_16_FlipX(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT16 *line;
   int xx,yy;

   for(yy=7; yy>=0; yy--)
   {
      line = ((UINT16 *)(GameBitmap->line[y+yy]))+ x;
      for(xx=0; xx<8; xx++)
      {
         *line++ = *SPR++ | cmap;
      }
   }
}

void Draw8x8_16_FlipY(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT16 *line;
   int xx,yy;

   for(yy=0; yy<8; yy++)
   {
      line = ((UINT16 *)(GameBitmap->line[y+yy]))+ x + 7;
      for(xx=0; xx<8; xx++)
      {
         *line-- = *SPR++ | cmap;
      }
   }
}

void Draw8x8_16_FlipXY(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT16 *line;
   int xx,yy;

   for(yy=7; yy>=0; yy--)
   {
      line = ((UINT16 *)(GameBitmap->line[y+yy]))+ x + 7;
      for(xx=0; xx<8; xx++)
      {
         *line-- = *SPR++ | cmap;
      }
   }
}

// 8x8 transparent sprites

void Draw8x8_Trans_16(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT16 *line;
   int xx,yy;

   for(yy=0; yy<8; yy++)
   {
      line = ((UINT16 *)(GameBitmap->line[y+yy]))+ x;
      for(xx=0; xx<8; xx++, SPR++)
      {
         if(*SPR)
            line[xx] = *SPR | cmap;
      }
   }
}

void Draw8x8_Trans_16_FlipX(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT16 *line;
   int xx,yy;

   for(yy=7; yy>=0; yy--)
   {
      line = ((UINT16 *)(GameBitmap->line[y+yy]))+ x;
      for(xx=0; xx<8; xx++, SPR++)
      {
         if(*SPR)
            line[xx] = *SPR | cmap;
      }
   }
}

void Draw8x8_Trans_16_FlipY(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT16 *line;
   int xx,yy;

   for(yy=0; yy<8; yy++)
   {
      line = ((UINT16 *)(GameBitmap->line[y+yy]))+ x;
      for(xx=7; xx>=0; xx--, SPR++)
      {
         if(*SPR)
            line[xx] = *SPR | cmap;
      }
   }
}

void Draw8x8_Trans_16_FlipXY(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT16 *line;
   int xx,yy;

   for(yy=7; yy>=0; yy--)
   {
      line = ((UINT16 *)(GameBitmap->line[y+yy]))+ x;
      for(xx=7; xx>=0; xx--, SPR++)
      {
         if(*SPR)
            line[xx] = *SPR | cmap;
      }
   }
}
