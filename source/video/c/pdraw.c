/******************************************************************************/
/*                                                                            */
/*                         RAINE 16x16 TILE DRAWING                           */
/*                                                                            */
/******************************************************************************/

#include "deftypes.h"
#include "raine.h"
#include "priorities.h"
#include "blit.h"

/*

These functions are used to draw to the screen while checking the priority bitmap to decide
wether a pixel must be drawn or not. They are usually used to draw sprites.

*/

// taken from 16x16_8.c. Difference : the priority bitmap
// x, y are passed to avoid the conversion of coordinates because of screen rotation
/* See the comments about pdraw and pbitmap usage in priorities.c */
/* Usage of macros : it makes the code less readable, but it also helps to make much
   less mistakes while adapting the code to different sprite sizes. Just use an editor
   like emacs with functions to help editing multi-line macros...
   Now the problem is to know wether the 8x8 version will become usefull one day or not ! */

/* Priorities test : sprites of the same priority should appear in the
   order they are drawn (the most recent overwrite the oldest). So the
   test is if (pline[xx] <= pri) draw_pixel */

/* Background version : it's a special way to handle 2 sprites overwriting each other :
   if you have a layer with priority p1
   and then a sprite of priority p2
   and finally a last sprite of priority p3
   with p2 > p1 and p3 < p1, then the last sprite makes the layer visible in most games using priority
   bitmaps.

   There is a fast way to emulate this : just draw the sprites first, updating the priority bitmap but
   without testing it. Then only the tiles actually test the priority bitmap. It's the fastest possible
   way to do it (no blit, no background bitmap).

   In fact after some tests, we should keep only the background version for the sprites,
   and replace the normal version by this one. But it obliges to change all the drivers
   using these functions to draw the sprites first. This includes at least mcatadv and
   gunbird. Maybe one day...

 */

extern BITMAP *BackBitmap;
#define declare_spr(SIZE)										   \
void pdraw##SIZE##x##SIZE##_Mapped_8(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)			   \
{													   \
   UINT8 *line,*pline;											   \
   int xx,yy;												   \
													   \
   for(yy=0; yy<SIZE; yy++){										   \
      line = GameBitmap->line[y+yy] + x;								   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=0; xx<SIZE; xx++){										   \
	if (pline[xx] <= pri) {										   \
	  pline[xx] = 0;										   \
	  line[xx] = cmap[ SPR[xx] ];									   \
	}												   \
      }													   \
      SPR += SIZE;											   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Mapped_8_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)		   \
{													   \
   UINT8 *line,*pline;											   \
   int xx,yy;												   \
													   \
   for(yy=(SIZE-1); yy>=0; yy--){									   \
      line = GameBitmap->line[y+yy] + x;								   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=0; xx<SIZE; xx++){										   \
	if (pline[xx] <= pri) {										   \
	  pline[xx] = 0;										   \
	  line[xx] = cmap[ SPR[xx] ];									   \
	}												   \
      }													   \
      SPR += SIZE;											   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Mapped_8_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)		   \
{													   \
   UINT8 *line,*pline;											   \
   int xx,yy;												   \
													   \
   for(yy=0; yy<SIZE; yy++){										   \
      line = GameBitmap->line[y+yy] + x;								   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){								   \
	if (pline[xx] <= pri) {										   \
	  pline[xx] = 0;										   \
	  line[xx] = cmap[ *SPR ];									   \
	}												   \
      }													   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Mapped_8_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)		   \
{													   \
   UINT8 *line,*pline;											   \
   int xx,yy;												   \
													   \
   for(yy=(SIZE-1); yy>=0; yy--){									   \
      line = GameBitmap->line[y+yy] + x;								   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){								   \
	if (pline[xx] <= pri) {										   \
	  pline[xx] = 0;										   \
	  line[xx] = cmap[ *SPR ];									   \
	}												   \
      }													   \
   }													   \
}													   \
													   \
/* mapped transparent sprites */									   \
													   \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_8(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)		   \
{													   \
   UINT8 *line,*pline;											   \
   int xx,yy;												   \
													   \
   for(yy=0; yy<SIZE; yy++){										   \
      line = GameBitmap->line[y+yy] + x;								   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=0; xx<SIZE; xx++){										   \
         if(SPR[xx] && pline[xx] <= pri) {								   \
	   pline[xx] = 0;										   \
	   line[xx] = cmap[ SPR[xx] ];									   \
	 }												   \
      }													   \
      SPR += SIZE;											   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_8_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)	   \
{													   \
   UINT8 *line,*pline;											   \
   int xx,yy;												   \
													   \
   for(yy=(SIZE-1); yy>=0; yy--){									   \
      line = GameBitmap->line[y+yy] + x;								   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=0; xx<SIZE; xx++){										   \
         if(SPR[xx] && pline[xx] <= pri) {								   \
	   pline[xx] = 0;										   \
	   line[xx] = cmap[ SPR[xx] ];									   \
	 }												   \
      }													   \
      SPR += SIZE;											   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_8_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)	   \
{													   \
   UINT8 *line,*pline;											   \
   int xx,yy;												   \
													   \
   for(yy=0; yy<SIZE; yy++){										   \
      line = GameBitmap->line[y+yy] + x;								   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){								   \
         if(*SPR && pline[xx] <= pri) {									   \
	   pline[xx] = 0;										   \
	   line[xx] = cmap[ *SPR ];									   \
	 }												   \
      }													   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_8_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)	   \
{													   \
   UINT8 *line,*pline;											   \
   int xx,yy;												   \
													   \
   for(yy=(SIZE-1); yy>=0; yy--){									   \
      line = GameBitmap->line[y+yy] + x;								   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){								   \
         if(*SPR && pline[xx] <= pri) {									   \
	   pline[xx] = 0;										   \
	   line[xx] = cmap[ *SPR ];									   \
	 }												   \
      }													   \
   }													   \
}													   \
													   \
/* 16bpp */												   \
													   \
void pdraw##SIZE##x##SIZE##_Mapped_16(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)		   \
{													   \
   UINT16 *line;											   \
   UINT8 *pline;											   \
   int xx,yy;												   \
													   \
   for(yy=0; yy<SIZE; yy++){										   \
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x;							   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=0; xx<SIZE; xx++){										   \
	if (pline[xx] <= pri) {										   \
	  pline[xx] = 0;										   \
	  line[xx] = ((UINT16 *)cmap)[ SPR[xx] ];							   \
	}												   \
      }													   \
      SPR += SIZE;											   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Mapped_16_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)		   \
{													   \
   UINT16 *line;											   \
   UINT8 *pline;											   \
   int xx,yy;												   \
													   \
   for(yy=(SIZE-1); yy>=0; yy--){									   \
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x;							   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=0; xx<SIZE; xx++){										   \
	if (pline[xx] <= pri) {										   \
	  pline[xx] = 0;										   \
	  line[xx] = ((UINT16 *)cmap)[ SPR[xx] ];							   \
	}												   \
      }													   \
      SPR += SIZE;											   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Mapped_16_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)		   \
{													   \
   UINT16 *line;											   \
   UINT8 *pline;											   \
   int xx,yy;												   \
													   \
   for(yy=0; yy<SIZE; yy++){										   \
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x;							   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){								   \
	if (pline[xx] <= pri) {										   \
	  pline[xx] = 0;										   \
	  line[xx] = ((UINT16 *)cmap)[ *SPR ];								   \
	}												   \
      }													   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Mapped_16_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)		   \
{													   \
   UINT16 *line;											   \
   UINT8 *pline;											   \
   int xx,yy;												   \
													   \
   for(yy=(SIZE-1); yy>=0; yy--){									   \
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x;							   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){								   \
	if (pline[xx] <= pri) {										   \
	  pline[xx] = 0;										   \
	  line[xx] = ((UINT16 *)cmap)[ *SPR ];								   \
	}												   \
      }													   \
   }													   \
}													   \
													   \
/* mapped transparent sprites */									   \
													   \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_16(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)		   \
{													   \
   UINT16 *line;											   \
   UINT8 *pline;											   \
   int xx,yy;												   \
													   \
   for(yy=0; yy<SIZE; yy++){										   \
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x;							   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=0; xx<SIZE; xx++){										   \
         if(SPR[xx] && pline[xx] <= pri) {								   \
	   pline[xx] = 0;										   \
	   line[xx] = ((UINT16 *)cmap)[ SPR[xx] ];							   \
	 }												   \
      }													   \
      SPR += SIZE;											   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_16_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)	   \
{													   \
   UINT16 *line;											   \
   UINT8 *pline;											   \
   int xx,yy;												   \
													   \
   for(yy=(SIZE-1); yy>=0; yy--){									   \
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x;							   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=0; xx<SIZE; xx++){										   \
	if(SPR[xx] && pline[xx] <= pri) {								   \
	  pline[xx] = 0;										   \
	  line[xx] = ((UINT16 *)cmap)[ SPR[xx] ];							   \
	}												   \
      }													   \
      SPR += SIZE;											   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_16_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)	   \
{													   \
   UINT16 *line;											   \
   UINT8 *pline;											   \
   int xx,yy;												   \
													   \
   for(yy=0; yy<SIZE; yy++){										   \
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x;							   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){								   \
	if(*SPR && pline[xx] <= pri) {									   \
	  pline[xx] = 0;										   \
	  line[xx] = ((UINT16 *)cmap)[ *SPR ];								   \
	}												   \
      }													   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_16_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)	   \
{													   \
   UINT16 *line;											   \
   UINT8 *pline;											   \
   int xx,yy;												   \
													   \
   for(yy=(SIZE-1); yy>=0; yy--){									   \
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x;							   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){								   \
         if(*SPR && pline[xx] <= pri) {									   \
	   pline[xx] = 0;										   \
	   line[xx] = ((UINT16 *)cmap)[ *SPR ];								   \
	 }												   \
      }													   \
   }													   \
}													   \
													   \
/* 32 bpp */												   \
													   \
void pdraw##SIZE##x##SIZE##_Mapped_32(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)		   \
{													   \
   UINT32 *line;											   \
   UINT8 *pline;											   \
   int xx,yy;												   \
													   \
   for(yy=0; yy<SIZE; yy++){										   \
      line = ((UINT32 *)(GameBitmap->line[y+yy])) + x;							   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=0; xx<SIZE; xx++){										   \
	if (pline[xx] <= pri) {										   \
	  pline[xx] = 0;										   \
	  line[xx] = ((UINT32 *)cmap)[ SPR[xx] ];							   \
	}												   \
      }													   \
      SPR += SIZE;											   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Mapped_32_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)		   \
{													   \
   UINT32 *line;											   \
   UINT8 *pline;											   \
   int xx,yy;												   \
													   \
   for(yy=(SIZE-1); yy>=0; yy--){									   \
      line = ((UINT32 *)(GameBitmap->line[y+yy])) + x;							   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=0; xx<SIZE; xx++){										   \
	if (pline[xx] <= pri) {										   \
	  pline[xx] = 0;										   \
	  line[xx] = ((UINT32 *)cmap)[ SPR[xx] ];							   \
	}												   \
      }													   \
      SPR += SIZE;											   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Mapped_32_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)		   \
{													   \
   UINT32 *line;											   \
   UINT8 *pline;											   \
   int xx,yy;												   \
													   \
   for(yy=0; yy<SIZE; yy++){										   \
      line = ((UINT32 *)(GameBitmap->line[y+yy])) + x;							   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){								   \
	if (pline[xx] <= pri) {										   \
	  pline[xx] = 0;										   \
	  line[xx] = ((UINT32 *)cmap)[ *SPR ];								   \
	}												   \
      }													   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Mapped_32_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)		   \
{													   \
   UINT32 *line;											   \
   UINT8 *pline;											   \
   int xx,yy;												   \
													   \
   for(yy=(SIZE-1); yy>=0; yy--){									   \
      line = ((UINT32 *)(GameBitmap->line[y+yy])) + x;							   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){								   \
	if (pline[xx] <= pri) {										   \
	  pline[xx] = 0;										   \
	  line[xx] = ((UINT32 *)cmap)[ *SPR ];								   \
	}												   \
      }													   \
   }													   \
}													   \
													   \
/* mapped transparent sprites */									   \
													   \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_32(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)		   \
{													   \
   UINT32 *line;											   \
   UINT8 *pline;											   \
   int xx,yy;												   \
													   \
   for(yy=0; yy<SIZE; yy++){										   \
      line = ((UINT32 *)(GameBitmap->line[y+yy])) + x;							   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=0; xx<SIZE; xx++){										   \
         if(SPR[xx] && pline[xx] <= pri) {								   \
	   pline[xx] = 0;										   \
	   line[xx] = ((UINT32 *)cmap)[ SPR[xx] ];							   \
	 }												   \
      }													   \
      SPR += SIZE;											   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_32_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)	   \
{													   \
   UINT32 *line;											   \
   UINT8 *pline;											   \
   int xx,yy;												   \
													   \
   for(yy=(SIZE-1); yy>=0; yy--){									   \
      line = ((UINT32 *)(GameBitmap->line[y+yy])) + x;							   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=0; xx<SIZE; xx++){										   \
         if(SPR[xx] && pline[xx] <= pri) {								   \
	   pline[xx] = 0;										   \
	   line[xx] = ((UINT32 *)cmap)[ SPR[xx] ];							   \
	 }												   \
      }													   \
      SPR += SIZE;											   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_32_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)	   \
{													   \
   UINT32 *line;											   \
   UINT8 *pline;											   \
   int xx,yy;												   \
													   \
   for(yy=0; yy<SIZE; yy++){										   \
      line = ((UINT32 *)(GameBitmap->line[y+yy])) + x;							   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){								   \
         if(*SPR && pline[xx] <= pri) {									   \
	   pline[xx] = 0;										   \
	   line[xx] = ((UINT32 *)cmap)[ *SPR ];								   \
	 }												   \
      }													   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_32_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)	   \
{													   \
   UINT32 *line;											   \
   UINT8 *pline;											   \
   int xx,yy;												   \
													   \
   for(yy=(SIZE-1); yy>=0; yy--){									   \
      line = ((UINT32 *)(GameBitmap->line[y+yy])) + x;							   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){								   \
         if(*SPR && pline[xx] <= pri) {									   \
	   pline[xx] = 0;										   \
	   line[xx] = ((UINT32 *)cmap)[ *SPR ];								   \
	 }												   \
      }													   \
   }													   \
}													   \
													   \
/* Background version, see comments at the top of this file */						   \
													   \
void pdraw##SIZE##x##SIZE##_Mapped_back_8(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)		   \
{													   \
   UINT8 *line,*pline;											   \
   int xx,yy;												   \
													   \
   for(yy=0; yy<SIZE; yy++){										   \
      line = GameBitmap->line[y+yy] + x;								   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=0; xx<SIZE; xx++){										   \
	pline[xx] = pri;										   \
	line[xx] = cmap[ SPR[xx] ];									   \
      }													   \
      SPR += SIZE;											   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Mapped_back_8_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)	   \
{													   \
   UINT8 *line,*pline;											   \
   int xx,yy;												   \
													   \
   for(yy=(SIZE-1); yy>=0; yy--){									   \
      line = GameBitmap->line[y+yy] + x;								   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=0; xx<SIZE; xx++){										   \
	pline[xx] = pri;										   \
	line[xx] = cmap[ SPR[xx] ];									   \
      }													   \
      SPR += SIZE;											   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Mapped_back_8_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)	   \
{													   \
   UINT8 *line,*pline;											   \
   int xx,yy;												   \
													   \
   for(yy=0; yy<SIZE; yy++){										   \
      line = GameBitmap->line[y+yy] + x;								   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){								   \
	  pline[xx] = pri;										   \
	  line[xx] = cmap[ *SPR ];									   \
      }													   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Mapped_back_8_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)	   \
{													   \
   UINT8 *line,*pline;											   \
   int xx,yy;												   \
													   \
   for(yy=(SIZE-1); yy>=0; yy--){									   \
      line = GameBitmap->line[y+yy] + x;								   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){								   \
	  pline[xx] = pri;										   \
	  line[xx] = cmap[ *SPR ];									   \
      }													   \
   }													   \
}													   \
													   \
/* mapped transparent sprites */									   \
													   \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_back_8(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)	   \
{													   \
   UINT8 *line,*pline;											   \
   int xx,yy;												   \
													   \
   for(yy=0; yy<SIZE; yy++){										   \
      line = GameBitmap->line[y+yy] + x;								   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=0; xx<SIZE; xx++){										   \
         if(SPR[xx]) {											   \
	   pline[xx] = pri;										   \
	   line[xx] = cmap[ SPR[xx] ];									   \
	 }												   \
      }													   \
      SPR += SIZE;											   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_back_8_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)	   \
{													   \
   UINT8 *line,*pline;											   \
   int xx,yy;												   \
													   \
   for(yy=(SIZE-1); yy>=0; yy--){									   \
      line = GameBitmap->line[y+yy] + x;								   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=0; xx<SIZE; xx++){										   \
         if(SPR[xx]) {											   \
	   pline[xx] = pri;										   \
	   line[xx] = cmap[ SPR[xx] ];									   \
	 }												   \
      }													   \
      SPR += SIZE;											   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_back_8_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)	   \
{													   \
   UINT8 *line,*pline;											   \
   int xx,yy;												   \
													   \
   for(yy=0; yy<SIZE; yy++){										   \
      line = GameBitmap->line[y+yy] + x;								   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){								   \
         if(*SPR) {											   \
	   pline[xx] = pri;										   \
	   line[xx] = cmap[ *SPR ];									   \
	 }												   \
      }													   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_back_8_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)	   \
{													   \
   UINT8 *line,*pline;											   \
   int xx,yy;												   \
													   \
   for(yy=(SIZE-1); yy>=0; yy--){									   \
      line = GameBitmap->line[y+yy] + x;								   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){								   \
         if(*SPR) {											   \
	   pline[xx] = pri;										   \
	   line[xx] = cmap[ *SPR ];									   \
	 }												   \
      }													   \
   }													   \
}													   \
													   \
/* 16bpp */												   \
													   \
void pdraw##SIZE##x##SIZE##_Mapped_back_16(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)		   \
{													   \
   UINT16 *line;											   \
   UINT8 *pline;											   \
   int xx,yy;												   \
													   \
   for(yy=0; yy<SIZE; yy++){										   \
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x;							   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=0; xx<SIZE; xx++){										   \
	  pline[xx] = pri;										   \
	  line[xx] = ((UINT16 *)cmap)[ SPR[xx] ];							   \
      }													   \
      SPR += SIZE;											   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Mapped_back_16_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)	   \
{													   \
   UINT16 *line;											   \
   UINT8 *pline;											   \
   int xx,yy;												   \
													   \
   for(yy=(SIZE-1); yy>=0; yy--){									   \
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x;							   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=0; xx<SIZE; xx++){										   \
	  pline[xx] = pri;										   \
	  line[xx] = ((UINT16 *)cmap)[ SPR[xx] ];							   \
      }													   \
      SPR += SIZE;											   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Mapped_back_16_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)	   \
{													   \
   UINT16 *line;											   \
   UINT8 *pline;											   \
   int xx,yy;												   \
													   \
   for(yy=0; yy<SIZE; yy++){										   \
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x;							   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){								   \
	  pline[xx] = pri;										   \
	  line[xx] = ((UINT16 *)cmap)[ *SPR ];								   \
      }													   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Mapped_back_16_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)	   \
{													   \
   UINT16 *line;											   \
   UINT8 *pline;											   \
   int xx,yy;												   \
													   \
   for(yy=(SIZE-1); yy>=0; yy--){									   \
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x;							   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){								   \
	  pline[xx] = pri;										   \
	  line[xx] = ((UINT16 *)cmap)[ *SPR ];								   \
      }													   \
   }													   \
}													   \
													   \
/* mapped transparent sprites */									   \
													   \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_back_16(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)	   \
{													   \
   UINT16 *line;											   \
   UINT8 *pline;											   \
   int xx,yy;												   \
													   \
   for(yy=0; yy<SIZE; yy++){										   \
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x;							   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=0; xx<SIZE; xx++){										   \
         if(SPR[xx]) {											   \
	   pline[xx] = pri;										   \
	   line[xx] = ((UINT16 *)cmap)[ SPR[xx] ];							   \
	 }												   \
      }													   \
      SPR += SIZE;											   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_back_16_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)  \
{													   \
   UINT16 *line;											   \
   UINT8 *pline;											   \
   int xx,yy;												   \
													   \
   for(yy=(SIZE-1); yy>=0; yy--){									   \
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x;							   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=0; xx<SIZE; xx++){										   \
	if(SPR[xx]) {											   \
	  pline[xx] = pri;										   \
	  line[xx] = ((UINT16 *)cmap)[ SPR[xx] ];							   \
	}												   \
      }													   \
      SPR += SIZE;											   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_back_16_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)  \
{													   \
   UINT16 *line;											   \
   UINT8 *pline;											   \
   int xx,yy;												   \
													   \
   for(yy=0; yy<SIZE; yy++){										   \
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x;							   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){								   \
	if(*SPR) {											   \
	  pline[xx] = pri;										   \
	  line[xx] = ((UINT16 *)cmap)[ *SPR ];								   \
	}												   \
      }													   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_back_16_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri) \
{													   \
   UINT16 *line;											   \
   UINT8 *pline;											   \
   int xx,yy;												   \
													   \
   for(yy=(SIZE-1); yy>=0; yy--){									   \
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x;							   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){								   \
         if(*SPR) {											   \
	   pline[xx] = pri;										   \
	   line[xx] = ((UINT16 *)cmap)[ *SPR ];								   \
	 }												   \
      }													   \
   }													   \
}													   \
													   \
/* 32 bpp */												   \
													   \
void pdraw##SIZE##x##SIZE##_Mapped_back_32(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)		   \
{													   \
   UINT32 *line;											   \
   UINT8 *pline;											   \
   int xx,yy;												   \
													   \
   for(yy=0; yy<SIZE; yy++){										   \
      line = ((UINT32 *)(GameBitmap->line[y+yy])) + x;							   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=0; xx<SIZE; xx++){										   \
	  pline[xx] = pri;										   \
	  line[xx] = ((UINT32 *)cmap)[ SPR[xx] ];							   \
      }													   \
      SPR += SIZE;											   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Mapped_back_32_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)	   \
{													   \
   UINT32 *line;											   \
   UINT8 *pline;											   \
   int xx,yy;												   \
													   \
   for(yy=(SIZE-1); yy>=0; yy--){									   \
      line = ((UINT32 *)(GameBitmap->line[y+yy])) + x;							   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=0; xx<SIZE; xx++){										   \
	  pline[xx] = pri;										   \
	  line[xx] = ((UINT32 *)cmap)[ SPR[xx] ];							   \
      }													   \
      SPR += SIZE;											   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Mapped_back_32_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)	   \
{													   \
   UINT32 *line;											   \
   UINT8 *pline;											   \
   int xx,yy;												   \
													   \
   for(yy=0; yy<SIZE; yy++){										   \
      line = ((UINT32 *)(GameBitmap->line[y+yy])) + x;							   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){								   \
	  pline[xx] = pri;										   \
	  line[xx] = ((UINT32 *)cmap)[ *SPR ];								   \
      }													   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Mapped_back_32_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)	   \
{													   \
   UINT32 *line;											   \
   UINT8 *pline;											   \
   int xx,yy;												   \
													   \
   for(yy=(SIZE-1); yy>=0; yy--){									   \
      line = ((UINT32 *)(GameBitmap->line[y+yy])) + x;							   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){								   \
	  pline[xx] = pri;										   \
	  line[xx] = ((UINT32 *)cmap)[ *SPR ];								   \
      }													   \
   }													   \
}													   \
													   \
/* mapped transparent sprites */									   \
													   \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_back_32(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)	   \
{													   \
   UINT32 *line;											   \
   UINT8 *pline;											   \
   int xx,yy;												   \
													   \
   for(yy=0; yy<SIZE; yy++){										   \
      line = ((UINT32 *)(GameBitmap->line[y+yy])) + x;							   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=0; xx<SIZE; xx++){										   \
         if(SPR[xx]) {											   \
	   pline[xx] = pri;										   \
	   line[xx] = ((UINT32 *)cmap)[ SPR[xx] ];							   \
	 }												   \
      }													   \
      SPR += SIZE;											   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_back_32_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)  \
{													   \
   UINT32 *line;											   \
   UINT8 *pline;											   \
   int xx,yy;												   \
													   \
   for(yy=(SIZE-1); yy>=0; yy--){									   \
      line = ((UINT32 *)(GameBitmap->line[y+yy])) + x;							   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=0; xx<SIZE; xx++){										   \
         if(SPR[xx]) {											   \
	   pline[xx] = pri;										   \
	   line[xx] = ((UINT32 *)cmap)[ SPR[xx] ];							   \
	 }												   \
      }													   \
      SPR += SIZE;											   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_back_32_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)  \
{													   \
   UINT32 *line;											   \
   UINT8 *pline;											   \
   int xx,yy;												   \
													   \
   for(yy=0; yy<SIZE; yy++){										   \
      line = ((UINT32 *)(GameBitmap->line[y+yy])) + x;							   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){								   \
         if(*SPR) {											   \
	   pline[xx] = pri;										   \
	   line[xx] = ((UINT32 *)cmap)[ *SPR ];								   \
	 }												   \
      }													   \
   }													   \
}													   \
													   \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_back_32_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri) \
{													   \
   UINT32 *line;											   \
   UINT8 *pline;											   \
   int xx,yy;												   \
													   \
   for(yy=(SIZE-1); yy>=0; yy--){									   \
      line = ((UINT32 *)(GameBitmap->line[y+yy])) + x;							   \
      pline = pbitmap->line[y+yy] + x;									   \
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){								   \
         if(*SPR) {											   \
	   pline[xx] = pri;										   \
	   line[xx] = ((UINT32 *)cmap)[ *SPR ];								   \
	 }												   \
      }													   \
   }													   \
}


/* pdrawNNxNN_Mask version : combines pdraw with Draw_Mask, that is : update the screen and the priority */
/* bitmap at the same time */

#define declare(SIZE)												\
void pdraw##SIZE##x##SIZE##_Mask_Mapped_8(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)			\
{														\
   UINT8 *line,*pline;												\
   int xx,yy;													\
														\
   for(yy=0; yy<SIZE; yy++){											\
      line = GameBitmap->line[y+yy] + x;									\
      pline = pbitmap->line[y+yy] + x;										\
      for(xx=0; xx<SIZE; xx++){											\
	if (pline[xx] <= pri) {											\
	  line[xx] = cmap[ SPR[xx] ];										\
	  pline[xx] = pri;											\
	}													\
      }														\
      SPR += SIZE;												\
   }														\
}														\
														\
void pdraw##SIZE##x##SIZE##_Mask_Mapped_8_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)		\
{														\
   UINT8 *line,*pline;												\
   int xx,yy;													\
														\
   for(yy=(SIZE-1); yy>=0; yy--){										\
      line = GameBitmap->line[y+yy] + x;									\
      pline = pbitmap->line[y+yy] + x;										\
      for(xx=0; xx<SIZE; xx++){											\
	if (pline[xx] <= pri) {											\
	  line[xx] = cmap[ SPR[xx] ];										\
	  pline[xx] = pri;											\
	}													\
      }														\
      SPR += SIZE;												\
   }														\
}														\
														\
void pdraw##SIZE##x##SIZE##_Mask_Mapped_8_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)		\
{														\
   UINT8 *line,*pline;												\
   int xx,yy;													\
														\
   for(yy=0; yy<SIZE; yy++){											\
      line = GameBitmap->line[y+yy] + x;									\
      pline = pbitmap->line[y+yy] + x;										\
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){									\
	if (pline[xx] <= pri) {											\
	  line[xx] = cmap[ *SPR ];										\
	  pline[xx] = pri;											\
	}													\
      }														\
   }														\
}														\
														\
void pdraw##SIZE##x##SIZE##_Mask_Mapped_8_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)		\
{														\
   UINT8 *line,*pline;												\
   int xx,yy;													\
														\
   for(yy=(SIZE-1); yy>=0; yy--){										\
      line = GameBitmap->line[y+yy] + x;									\
      pline = pbitmap->line[y+yy] + x;										\
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){									\
	if (pline[xx] <= pri) {											\
	  line[xx] = cmap[ *SPR ];										\
	  pline[xx] = pri;											\
	}													\
      }														\
   }														\
}														\
														\
/* mapped transparent sprites */										\
														\
void pdraw##SIZE##x##SIZE##_Mask_Trans_Mapped_8(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)		\
{														\
   UINT8 *line,*pline;												\
   int xx,yy;													\
														\
   for(yy=0; yy<SIZE; yy++){											\
      line = GameBitmap->line[y+yy] + x;									\
      pline = pbitmap->line[y+yy] + x;										\
      for(xx=0; xx<SIZE; xx++){											\
	if(SPR[xx] && pline[xx] <= pri) {									\
	  line[xx] = cmap[ SPR[xx] ];										\
	  pline[xx] = pri;											\
	}													\
      }														\
      SPR += SIZE;												\
   }														\
}														\
														\
void pdraw##SIZE##x##SIZE##_Mask_Trans_Mapped_8_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)		\
{														\
   UINT8 *line,*pline;												\
   int xx,yy;													\
														\
   for(yy=(SIZE-1); yy>=0; yy--){										\
      line = GameBitmap->line[y+yy] + x;									\
      pline = pbitmap->line[y+yy] + x;										\
      for(xx=0; xx<SIZE; xx++){											\
	if(SPR[xx] && pline[xx] <= pri) {									\
	  line[xx] = cmap[ SPR[xx] ];										\
	  pline[xx] = pri;											\
	}													\
      }														\
      SPR += SIZE;												\
   }														\
}														\
														\
void pdraw##SIZE##x##SIZE##_Mask_Trans_Mapped_8_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)		\
{														\
   UINT8 *line,*pline;												\
   int xx,yy;													\
														\
   for(yy=0; yy<SIZE; yy++){											\
      line = GameBitmap->line[y+yy] + x;									\
      pline = pbitmap->line[y+yy] + x;										\
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){									\
	if(*SPR && pline[xx] <= pri) {										\
	  line[xx] = cmap[ *SPR ];										\
	  pline[xx] = pri;											\
	}													\
      }														\
   }														\
}														\
														\
void pdraw##SIZE##x##SIZE##_Mask_Trans_Mapped_8_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)		\
{														\
   UINT8 *line,*pline;												\
   int xx,yy;													\
														\
   for(yy=(SIZE-1); yy>=0; yy--){										\
      line = GameBitmap->line[y+yy] + x;									\
      pline = pbitmap->line[y+yy] + x;										\
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){									\
	if(*SPR && pline[xx] <= pri) {										\
	  line[xx] = cmap[ *SPR ];										\
	  pline[xx] = pri;											\
	}													\
      }														\
   }														\
}														\
														\
/* 16bpp */													\
														\
void pdraw##SIZE##x##SIZE##_Mask_Mapped_16(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)			\
{														\
   UINT16 *line;												\
   UINT8 *pline;												\
   int xx,yy;													\
														\
   for(yy=0; yy<SIZE; yy++){											\
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x;								\
      pline = pbitmap->line[y+yy] + x;										\
      for(xx=0; xx<SIZE; xx++){											\
	if (pline[xx] <= pri) {											\
	  line[xx] = ((UINT16 *)cmap)[ SPR[xx] ];								\
	  pline[xx] = pri;											\
	}													\
      }														\
      SPR += SIZE;												\
   }														\
}														\
														\
void pdraw##SIZE##x##SIZE##_Mask_Mapped_16_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)		\
{														\
   UINT16 *line;												\
   UINT8 *pline;												\
   int xx,yy;													\
														\
   for(yy=(SIZE-1); yy>=0; yy--){										\
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x;								\
      pline = pbitmap->line[y+yy] + x;										\
      for(xx=0; xx<SIZE; xx++){											\
	if (pline[xx] <= pri) {											\
	  line[xx] = ((UINT16 *)cmap)[ SPR[xx] ];								\
	  pline[xx] = pri;											\
	}													\
      }														\
      SPR += SIZE;												\
   }														\
}														\
														\
void pdraw##SIZE##x##SIZE##_Mask_Mapped_16_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)		\
{														\
   UINT16 *line;												\
   UINT8 *pline;												\
   int xx,yy;													\
														\
   for(yy=0; yy<SIZE; yy++){											\
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x;								\
      pline = pbitmap->line[y+yy] + x;										\
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){									\
	if (pline[xx] <= pri) {											\
	  line[xx] = ((UINT16 *)cmap)[ *SPR ];									\
	  pline[xx] = pri;											\
	}													\
      }														\
   }														\
}														\
														\
void pdraw##SIZE##x##SIZE##_Mask_Mapped_16_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)		\
{														\
   UINT16 *line;												\
   UINT8 *pline;												\
   int xx,yy;													\
														\
   for(yy=(SIZE-1); yy>=0; yy--){										\
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x;								\
      pline = pbitmap->line[y+yy] + x;										\
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){									\
	if (pline[xx] <= pri) {											\
	  line[xx] = ((UINT16 *)cmap)[ *SPR ];									\
	  pline[xx] = pri;											\
	}													\
      }														\
   }														\
}														\
														\
/* mapped transparent sprites */										\
														\
void pdraw##SIZE##x##SIZE##_Mask_Trans_Mapped_16(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)		\
{														\
   UINT16 *line;												\
   UINT8 *pline;												\
   int xx,yy;													\
														\
   for(yy=0; yy<SIZE; yy++){											\
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x;								\
      pline = pbitmap->line[y+yy] + x;										\
      for(xx=0; xx<SIZE; xx++){											\
	if(SPR[xx] && pline[xx] <= pri) {									\
	  line[xx] = ((UINT16 *)cmap)[ SPR[xx] ];								\
	  pline[xx] = pri;											\
	}													\
      }														\
      SPR += SIZE;												\
   }														\
}														\
														\
void pdraw##SIZE##x##SIZE##_Mask_Trans_Mapped_16_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)	\
{														\
   UINT16 *line;												\
   UINT8 *pline;												\
   int xx,yy;													\
														\
   for(yy=(SIZE-1); yy>=0; yy--){										\
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x;								\
      pline = pbitmap->line[y+yy] + x;										\
      for(xx=0; xx<SIZE; xx++){											\
	if(SPR[xx] && pline[xx] <= pri) {									\
	  line[xx] = ((UINT16 *)cmap)[ SPR[xx] ];								\
	  pline[xx] = pri;											\
	}													\
      }														\
      SPR += SIZE;												\
   }														\
}														\
														\
void pdraw##SIZE##x##SIZE##_Mask_Trans_Mapped_16_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)	\
{														\
   UINT16 *line;												\
   UINT8 *pline;												\
   int xx,yy;													\
														\
   for(yy=0; yy<SIZE; yy++){											\
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x;								\
      pline = pbitmap->line[y+yy] + x;										\
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){									\
	if(*SPR && pline[xx] <= pri) {										\
	  line[xx] = ((UINT16 *)cmap)[ *SPR ];									\
	  pline[xx] = pri;											\
	}													\
      }														\
   }														\
}														\
														\
void pdraw##SIZE##x##SIZE##_Mask_Trans_Mapped_16_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)	\
{														\
   UINT16 *line;												\
   UINT8 *pline;												\
   int xx,yy;													\
														\
   for(yy=(SIZE-1); yy>=0; yy--){										\
      line = ((UINT16 *)(GameBitmap->line[y+yy])) + x;								\
      pline = pbitmap->line[y+yy] + x;										\
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){									\
	if(*SPR && pline[xx] <= pri) {										\
	  line[xx] = ((UINT16 *)cmap)[ *SPR ];									\
	  pline[xx] = pri;											\
	}													\
      }														\
   }														\
}														\
														\
/* 32 bpp */													\
														\
void pdraw##SIZE##x##SIZE##_Mask_Mapped_32(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)			\
{														\
   UINT32 *line;												\
   UINT8 *pline;												\
   int xx,yy;													\
														\
   for(yy=0; yy<SIZE; yy++){											\
      line = ((UINT32 *)(GameBitmap->line[y+yy])) + x;								\
      pline = pbitmap->line[y+yy] + x;										\
      for(xx=0; xx<SIZE; xx++){											\
	if (pline[xx] <= pri) {											\
	  line[xx] = ((UINT32 *)cmap)[ SPR[xx] ];								\
	  pline[xx] = pri;											\
	}													\
      }														\
      SPR += SIZE;												\
   }														\
}														\
														\
void pdraw##SIZE##x##SIZE##_Mask_Mapped_32_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)		\
{														\
   UINT32 *line;												\
   UINT8 *pline;												\
   int xx,yy;													\
														\
   for(yy=(SIZE-1); yy>=0; yy--){										\
      line = ((UINT32 *)(GameBitmap->line[y+yy])) + x;								\
      pline = pbitmap->line[y+yy] + x;										\
      for(xx=0; xx<SIZE; xx++){											\
	if (pline[xx] <= pri) {											\
	  line[xx] = ((UINT32 *)cmap)[ SPR[xx] ];								\
	  pline[xx] = pri;											\
	}													\
      }														\
      SPR += SIZE;												\
   }														\
}														\
														\
void pdraw##SIZE##x##SIZE##_Mask_Mapped_32_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)		\
{														\
   UINT32 *line;												\
   UINT8 *pline;												\
   int xx,yy;													\
														\
   for(yy=0; yy<SIZE; yy++){											\
      line = ((UINT32 *)(GameBitmap->line[y+yy])) + x;								\
      pline = pbitmap->line[y+yy] + x;										\
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){									\
	if (pline[xx] <= pri) {											\
	  line[xx] = ((UINT32 *)cmap)[ *SPR ];									\
	  pline[xx] = pri;											\
	}													\
      }														\
   }														\
}														\
														\
void pdraw##SIZE##x##SIZE##_Mask_Mapped_32_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)		\
{														\
   UINT32 *line;												\
   UINT8 *pline;												\
   int xx,yy;													\
														\
   for(yy=(SIZE-1); yy>=0; yy--){										\
      line = ((UINT32 *)(GameBitmap->line[y+yy])) + x;								\
      pline = pbitmap->line[y+yy] + x;										\
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){									\
	if (pline[xx] <= pri) {											\
	  line[xx] = ((UINT32 *)cmap)[ *SPR ];									\
	  pline[xx] = pri;											\
	}													\
      }														\
   }														\
}														\
														\
/* mapped transparent sprites */										\
														\
void pdraw##SIZE##x##SIZE##_Mask_Trans_Mapped_32(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)		\
{														\
   UINT32 *line;												\
   UINT8 *pline;												\
   int xx,yy;													\
														\
   for(yy=0; yy<SIZE; yy++){											\
      line = ((UINT32 *)(GameBitmap->line[y+yy])) + x;								\
      pline = pbitmap->line[y+yy] + x;										\
      for(xx=0; xx<SIZE; xx++){											\
	if(SPR[xx] && pline[xx] <= pri) {									\
	  line[xx] = ((UINT32 *)cmap)[ SPR[xx] ];								\
	  pline[xx] = pri;											\
	}													\
      }														\
      SPR += SIZE;												\
   }														\
}														\
														\
void pdraw##SIZE##x##SIZE##_Mask_Trans_Mapped_32_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)	\
{														\
   UINT32 *line;												\
   UINT8 *pline;												\
   int xx,yy;													\
														\
   for(yy=(SIZE-1); yy>=0; yy--){										\
      line = ((UINT32 *)(GameBitmap->line[y+yy])) + x;								\
      pline = pbitmap->line[y+yy] + x;										\
      for(xx=0; xx<SIZE; xx++){											\
	if(SPR[xx] && pline[xx] <= pri) {									\
	  line[xx] = ((UINT32 *)cmap)[ SPR[xx] ];								\
	  pline[xx] = pri;											\
	}													\
      }														\
      SPR += SIZE;												\
   }														\
}														\
														\
void pdraw##SIZE##x##SIZE##_Mask_Trans_Mapped_32_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)	\
{														\
   UINT32 *line;												\
   UINT8 *pline;												\
   int xx,yy;													\
														\
   for(yy=0; yy<SIZE; yy++){											\
      line = ((UINT32 *)(GameBitmap->line[y+yy])) + x;								\
      pline = pbitmap->line[y+yy] + x;										\
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){									\
	if(*SPR && pline[xx] <= pri) {										\
	  line[xx] = ((UINT32 *)cmap)[ *SPR ];									\
	  pline[xx] = pri;											\
	}													\
      }														\
   }														\
}														\
														\
void pdraw##SIZE##x##SIZE##_Mask_Trans_Mapped_32_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri)	\
{														\
   UINT32 *line;												\
   UINT8 *pline;												\
   int xx,yy;													\
														\
   for(yy=(SIZE-1); yy>=0; yy--){										\
      line = ((UINT32 *)(GameBitmap->line[y+yy])) + x;								\
      pline = pbitmap->line[y+yy] + x;										\
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){									\
	if(*SPR && pline[xx] <= pri) {										\
	  line[xx] = ((UINT32 *)cmap)[ *SPR ];									\
	  pline[xx] = pri;											\
	}													\
      }														\
   }														\
}

declare(16);
declare(8);
declare(32);

declare_spr(16);
declare_spr(8);
