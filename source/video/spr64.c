
#include "raine.h"
#include "newspr.h"
#include "blit.h"

static UINT8 *ZOOM64;
static UINT8 *ZOOM64R;

/********************************************************************/
// Init64x64_Table():
// Setup zoom table data for 64x64 sprites.
// TPOS Points to $2000 bytes of memory.

void Init64x64_Table(UINT8 *TPOS)
{
   UINT8 *ZZZ;
   int xx,yy;

   ZOOM64=TPOS;

   for(yy=1;yy<65;yy++){	// NORMAL
      ZZZ=ZOOM64+((yy-1)<<6);
      for(xx=0;xx<yy;xx++){
         ZZZ[xx]=((xx<<6)/yy);
      }
   }

   ZOOM64R=TPOS+0x1000;

   for(yy=1;yy<65;yy++){	// FLIP
      ZZZ=ZOOM64R+((yy-1)<<6);
      for(xx=0;xx<yy;xx++){
         ZZZ[xx]=((((yy-1)-xx)<<6)/yy);
      }
   }
}

/********************************************************************/
// Draw64x64_Trans_Mapped_Zoom():
// Draw64x64_Trans_Mapped_Zoom_FlipY():
// Draw64x64_Trans_Mapped_Zoom_FlipX():
// Draw64x64_Trans_Mapped_Zoom_FlipXY():
// Zoomed 64x64 sprites, using colour mapping.
// From Looking at the compiled asm, I'm not sure it's worth trying
// to rewrite these in asm...

void Draw64x64_Trans_Mapped_Zoom(UINT8 *SPR, int x, int y, UINT8 *cmap, int zoom)
{
   UINT8 *BIT,*SSS,*ZZXY;
   int xx,yy;

   if(zoom>=63){
      Draw64x64_Trans_Mapped(SPR,x,y,cmap);
   }
   else{

   ZZXY=ZOOM64+(zoom<<6);

   BIT=GameBitmap->line[y]+x;

   for(yy=0;yy<=zoom;yy++){
      SSS=SPR+(ZZXY[yy]<<6);
      for(xx=0;xx<=zoom;xx++){
         if(SSS[ZZXY[xx]]!=0)BIT[xx]=cmap[SSS[ZZXY[xx]]];
      }
      BIT+=GameBitmap->w;
   }

   }
}

void Draw64x64_Trans_Mapped_Zoom_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap, int zoom)
{
   UINT8 *BIT,*SSS,*ZZX,*ZZY;
   int xx,yy;


   if(zoom>=63){
      Draw64x64_Trans_Mapped_FlipY(SPR,x,y,cmap);
   }
   else{

   ZZX=ZOOM64R+(zoom<<6);
   ZZY=ZOOM64+(zoom<<6);

   BIT=GameBitmap->line[y]+x;

   for(yy=0;yy<=zoom;yy++){
      SSS=SPR+(ZZY[yy]<<6);
      for(xx=0;xx<=zoom;xx++){
         if(SSS[ZZX[xx]]!=0)BIT[xx]=cmap[SSS[ZZX[xx]]];
      }
      BIT+=GameBitmap->w;
   }

   }
}

void Draw64x64_Trans_Mapped_Zoom_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap, int zoom)
{
   UINT8 *BIT,*SSS,*ZZX,*ZZY;
   int xx,yy;

   if(zoom>=63){
      Draw64x64_Trans_Mapped_FlipX(SPR,x,y,cmap);
   }
   else{

   ZZX=ZOOM64+(zoom<<6);
   ZZY=ZOOM64R+(zoom<<6);

   BIT=GameBitmap->line[y]+x;

   for(yy=0;yy<=zoom;yy++){
      SSS=SPR+(ZZY[yy]<<6);
      for(xx=0;xx<=zoom;xx++){
         if(SSS[ZZX[xx]]!=0)BIT[xx]=cmap[SSS[ZZX[xx]]];
      }
      BIT+=GameBitmap->w;
   }

   }
}

void Draw64x64_Trans_Mapped_Zoom_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap, int zoom)
{
   UINT8 *BIT,*SSS,*ZZXY;
   int xx,yy;

   if(zoom>=63){
      Draw64x64_Trans_Mapped_FlipXY(SPR,x,y,cmap);
   }
   else{

   ZZXY=ZOOM64R+(zoom<<6);

   BIT=GameBitmap->line[y]+x;

   for(yy=0;yy<=zoom;yy++){
      SSS=SPR+(ZZXY[yy]<<6);
      for(xx=0;xx<=zoom;xx++){
         if(SSS[ZZXY[xx]]!=0)BIT[xx]=cmap[SSS[ZZXY[xx]]];
      }
      BIT+=GameBitmap->w;
   }

   }
}

/********************************************************************/

void Draw64x64_Trans_Mapped_Zoom2(UINT8 *SPR, int x, int y, UINT8 *cmap, int zoomx, int zoomy)
{
   UINT8 *BIT,*SSS,*ZZX,*ZZY;
   int xx,yy;

   if((zoomx>=63)&&(zoomy>=63)){
      Draw64x64_Trans_Mapped(SPR,x,y,cmap);
   }
   else{

   ZZX=ZOOM64+(zoomx<<6);
   ZZY=ZOOM64+(zoomy<<6);

   BIT=GameBitmap->line[y]+x;

   for(yy=0;yy<=zoomy;yy++){
      SSS=SPR+(ZZY[yy]<<6);
      for(xx=0;xx<=zoomx;xx++){
         if(SSS[ZZX[xx]]!=0)BIT[xx]=cmap[SSS[ZZX[xx]]];
      }
      BIT+=GameBitmap->w;
   }

   }
}

void Draw64x64_Trans_Mapped_Zoom2_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap, int zoomx, int zoomy)
{
   UINT8 *BIT,*SSS,*ZZX,*ZZY;
   int xx,yy;


   if((zoomx>=63)&&(zoomy>=63)){
      Draw64x64_Trans_Mapped_FlipY(SPR,x,y,cmap);
   }
   else{

   ZZX=ZOOM64R+(zoomx<<6);
   ZZY=ZOOM64+(zoomy<<6);

   BIT=GameBitmap->line[y]+x;

   for(yy=0;yy<=zoomy;yy++){
      SSS=SPR+(ZZY[yy]<<6);
      for(xx=0;xx<=zoomx;xx++){
         if(SSS[ZZX[xx]]!=0)BIT[xx]=cmap[SSS[ZZX[xx]]];
      }
      BIT+=GameBitmap->w;
   }

   }
}

void Draw64x64_Trans_Mapped_Zoom2_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap, int zoomx, int zoomy)
{
   UINT8 *BIT,*SSS,*ZZX,*ZZY;
   int xx,yy;

   if((zoomx>=63)&&(zoomy>=63)){
      Draw64x64_Trans_Mapped_FlipX(SPR,x,y,cmap);
   }
   else{

   ZZX=ZOOM64+(zoomx<<6);
   ZZY=ZOOM64R+(zoomy<<6);

   BIT=GameBitmap->line[y]+x;

   for(yy=0;yy<=zoomy;yy++){
      SSS=SPR+(ZZY[yy]<<6);
      for(xx=0;xx<=zoomx;xx++){
         if(SSS[ZZX[xx]]!=0)BIT[xx]=cmap[SSS[ZZX[xx]]];
      }
      BIT+=GameBitmap->w;
   }

   }
}

void Draw64x64_Trans_Mapped_Zoom2_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap, int zoomx, int zoomy)
{
   UINT8 *BIT,*SSS,*ZZX,*ZZY;
   int xx,yy;

   if((zoomx>=63)&&(zoomy>=63)){
      Draw64x64_Trans_Mapped_FlipXY(SPR,x,y,cmap);
   }
   else{

   ZZX=ZOOM64R+(zoomx<<6);
   ZZY=ZOOM64R+(zoomy<<6);

   BIT=GameBitmap->line[y]+x;

   for(yy=0;yy<=zoomy;yy++){
      SSS=SPR+(ZZY[yy]<<6);
      for(xx=0;xx<=zoomx;xx++){
         if(SSS[ZZX[xx]]!=0)BIT[xx]=cmap[SSS[ZZX[xx]]];
      }
      BIT+=GameBitmap->w;
   }

   }
}

/********************************************************************/

void Draw64x64_Mapped_Zoom2(UINT8 *SPR, int x, int y, UINT8 *cmap, int zoomx, int zoomy)
{
   UINT8 *BIT,*SSS,*ZZX,*ZZY;
   int xx,yy;

   if((zoomx>=63)&&(zoomy>=63)){
      Draw64x64_Mapped(SPR,x,y,cmap);
   }
   else{

   ZZX=ZOOM64+(zoomx<<6);
   ZZY=ZOOM64+(zoomy<<6);

   BIT=GameBitmap->line[y]+x;

   for(yy=0;yy<=zoomy;yy++){
      SSS=SPR+(ZZY[yy]<<6);
      for(xx=0;xx<=zoomx;xx++){
         BIT[xx]=cmap[SSS[ZZX[xx]]];
      }
      BIT+=GameBitmap->w;
   }

   }
}

void Draw64x64_Mapped_Zoom2_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap, int zoomx, int zoomy)
{
   UINT8 *BIT,*SSS,*ZZX,*ZZY;
   int xx,yy;


   if((zoomx>=63)&&(zoomy>=63)){
      Draw64x64_Mapped_FlipY(SPR,x,y,cmap);
   }
   else{

   ZZX=ZOOM64R+(zoomx<<6);
   ZZY=ZOOM64+(zoomy<<6);

   BIT=GameBitmap->line[y]+x;

   for(yy=0;yy<=zoomy;yy++){
      SSS=SPR+(ZZY[yy]<<6);
      for(xx=0;xx<=zoomx;xx++){
         BIT[xx]=cmap[SSS[ZZX[xx]]];
      }
      BIT+=GameBitmap->w;
   }

   }
}

void Draw64x64_Mapped_Zoom2_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap, int zoomx, int zoomy)
{
   UINT8 *BIT,*SSS,*ZZX,*ZZY;
   int xx,yy;

   if((zoomx>=63)&&(zoomy>=63)){
      Draw64x64_Mapped_FlipX(SPR,x,y,cmap);
   }
   else{

   ZZX=ZOOM64+(zoomx<<6);
   ZZY=ZOOM64R+(zoomy<<6);

   BIT=GameBitmap->line[y]+x;

   for(yy=0;yy<=zoomy;yy++){
      SSS=SPR+(ZZY[yy]<<6);
      for(xx=0;xx<=zoomx;xx++){
         BIT[xx]=cmap[SSS[ZZX[xx]]];
      }
      BIT+=GameBitmap->w;
   }

   }
}

void Draw64x64_Mapped_Zoom2_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap, int zoomx, int zoomy)
{
   UINT8 *BIT,*SSS,*ZZX,*ZZY;
   int xx,yy;

   if((zoomx>=63)&&(zoomy>=63)){
      Draw64x64_Mapped_FlipXY(SPR,x,y,cmap);
   }
   else{

   ZZX=ZOOM64R+(zoomx<<6);
   ZZY=ZOOM64R+(zoomy<<6);

   BIT=GameBitmap->line[y]+x;

   for(yy=0;yy<=zoomy;yy++){
      SSS=SPR+(ZZY[yy]<<6);
      for(xx=0;xx<=zoomx;xx++){
         BIT[xx]=cmap[SSS[ZZX[xx]]];
      }
      BIT+=GameBitmap->w;
   }

   }
}

/********************************************************************/
