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

void init_spr8x8asm_32(void)
{
}

// 8x8 mapped sprites

void Draw8x8_Mapped_32(UINT8 *SPR, int x, int y, UINT8 *cmap)
{
   UINT32 *line;
   int xx,yy;

   for(yy=0; yy<8; yy++)
   {
      line = ((UINT32 *)(GameBitmap->line[y+yy]))+ x;
      for(xx=0; xx<8; xx++)
      {
         *line++ = ((UINT32 *)cmap)[ *SPR++ ];
      }
   }
}

void Draw8x8_Mapped_32_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap)
{
   UINT32 *line;
   int xx,yy;

   for(yy=7; yy>=0; yy--)
   {
      line = ((UINT32 *)(GameBitmap->line[y+yy]))+ x;
      for(xx=0; xx<8; xx++)
      {
         *line++ = ((UINT32 *)cmap)[ *SPR++ ];
      }
   }
}

void Draw8x8_Mapped_32_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap)
{
   UINT32 *line;
   int xx,yy;

   for(yy=0; yy<8; yy++)
   {
      line = ((UINT32 *)(GameBitmap->line[y+yy]))+ x + 7;
      for(xx=0; xx<8; xx++)
      {
         *line-- = ((UINT32 *)cmap)[ *SPR++ ];
      }
   }
}

void Draw8x8_Mapped_32_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap)
{
   UINT32 *line;
   int xx,yy;

   for(yy=7; yy>=0; yy--)
   {
      line = ((UINT32 *)(GameBitmap->line[y+yy]))+ x + 7;
      for(xx=0; xx<8; xx++)
      {
         *line-- = ((UINT32 *)cmap)[ *SPR++ ];
      }
   }
}

// 8x8 mapped transparent sprites

void Draw8x8_Trans_Mapped_32(UINT8 *SPR, int x, int y, UINT8 *cmap)
{
   UINT32 *line;
   int xx,yy;

   for(yy=0; yy<8; yy++)
   {
      line = ((UINT32 *)(GameBitmap->line[y+yy]))+ x;
      for(xx=0; xx<8; xx++, SPR++)
      {
         if(*SPR)
            line[xx] = ((UINT32 *)cmap)[ *SPR ];
      }
   }
}

void Draw8x8_Trans_Mapped_32_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap)
{
   UINT32 *line;
   int xx,yy;

   for(yy=7; yy>=0; yy--)
   {
      line = ((UINT32 *)(GameBitmap->line[y+yy]))+ x;
      for(xx=0; xx<8; xx++, SPR++)
      {
         if(*SPR)
            line[xx] = ((UINT32 *)cmap)[ *SPR ];
      }
   }
}

void Draw8x8_Trans_Mapped_32_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap)
{
   UINT32 *line;
   int xx,yy;

   for(yy=0; yy<8; yy++)
   {
      line = ((UINT32 *)(GameBitmap->line[y+yy]))+ x;
      for(xx=7; xx>=0; xx--, SPR++)
      {
         if(*SPR)
            line[xx] = ((UINT32 *)cmap)[ *SPR ];
      }
   }
}

void Draw8x8_Trans_Mapped_32_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap)
{
   UINT32 *line;
   int xx,yy;

   for(yy=7; yy>=0; yy--)
   {
      line = ((UINT32 *)(GameBitmap->line[y+yy]))+ x;
      for(xx=7; xx>=0; xx--, SPR++)
      {
         if(*SPR)
            line[xx] = ((UINT32 *)cmap)[ *SPR ];
      }
   }
}

// 8x8 sprites

void Draw8x8_32(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT32 *line;
   int xx,yy;

   for(yy=0; yy<8; yy++)
   {
      line = ((UINT32 *)(GameBitmap->line[y+yy]))+ x;
      for(xx=0; xx<8; xx++)
      {
         *line++ = *SPR++ | cmap;
      }
   }
}

void Draw8x8_32_FlipX(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT32 *line;
   int xx,yy;

   for(yy=7; yy>=0; yy--)
   {
      line = ((UINT32 *)(GameBitmap->line[y+yy]))+ x;
      for(xx=0; xx<8; xx++)
      {
         *line++ = *SPR++ | cmap;
      }
   }
}

void Draw8x8_32_FlipY(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT32 *line;
   int xx,yy;

   for(yy=0; yy<8; yy++)
   {
      line = ((UINT32 *)(GameBitmap->line[y+yy]))+ x + 7;
      for(xx=0; xx<8; xx++)
      {
         *line-- = *SPR++ | cmap;
      }
   }
}

void Draw8x8_32_FlipXY(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT32 *line;
   int xx,yy;

   for(yy=7; yy>=0; yy--)
   {
      line = ((UINT32 *)(GameBitmap->line[y+yy]))+ x + 7;
      for(xx=0; xx<8; xx++)
      {
         *line-- = *SPR++ | cmap;
      }
   }
}

// 8x8 transparent sprites

void Draw8x8_Trans_32(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT32 *line;
   int xx,yy;

   for(yy=0; yy<8; yy++)
   {
      line = ((UINT32 *)(GameBitmap->line[y+yy]))+ x;
      for(xx=0; xx<8; xx++, SPR++)
      {
         if(*SPR)
            line[xx] = *SPR | cmap;
      }
   }
}

void Draw8x8_Trans_32_FlipX(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT32 *line;
   int xx,yy;

   for(yy=7; yy>=0; yy--)
   {
      line = ((UINT32 *)(GameBitmap->line[y+yy]))+ x;
      for(xx=0; xx<8; xx++, SPR++)
      {
         if(*SPR)
            line[xx] = *SPR | cmap;
      }
   }
}

void Draw8x8_Trans_32_FlipY(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT32 *line;
   int xx,yy;

   for(yy=0; yy<8; yy++)
   {
      line = ((UINT32 *)(GameBitmap->line[y+yy]))+ x;
      for(xx=7; xx>=0; xx--, SPR++)
      {
         if(*SPR)
            line[xx] = *SPR | cmap;
      }
   }
}

void Draw8x8_Trans_32_FlipXY(UINT8 *SPR, int x, int y, UINT8 cmap)
{
   UINT32 *line;
   int xx,yy;

   for(yy=7; yy>=0; yy--)
   {
      line = ((UINT32 *)(GameBitmap->line[y+yy]))+ x;
      for(xx=7; xx>=0; xx--, SPR++)
      {
         if(*SPR)
            line[xx] = *SPR | cmap;
      }
   }
}
