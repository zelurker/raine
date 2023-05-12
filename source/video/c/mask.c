#include "deftypes.h"
#include "raine.h"
#include "blit.h"
#include "priorities.h"
#include "alpha.h"

#define ARG_MAP UINT8 *SPR, int x, int y, UINT8 *cmap, int mask
#define render(SIZE,BPP)                                                        \
void pdraw##SIZE##x##SIZE##_Mapped_Maskcps2_##BPP(ARG_MAP)                      \
{                                                                               \
   UINT8 *pline;                                                                \
   UINT##BPP *line;                                                             \
   int xx,yy;                                                                   \
                                                                                \
   for(yy=0; yy<SIZE; yy++){                                                    \
      line = ((UINT##BPP *)GameBitmap->line[y+yy]) + x;                         \
      pline = pbitmap->line[y+yy] + x;                                          \
      for(xx=0; xx<SIZE; xx++){                                                 \
          if (((1 << pline[xx]) & mask) == 0)                                   \
              line[xx] = ((UINT##BPP *)cmap)[ SPR[xx] ];                        \
            pline[xx] = 31;                                                     \
      }                                                                         \
      SPR += SIZE;                                                              \
   }                                                                            \
}                                                                               \
                                                                                \
void pdraw##SIZE##x##SIZE##_Mapped_Maskcps2_##BPP##_FlipX(ARG_MAP)              \
{                                                                               \
   UINT8 *pline;                                                                \
   UINT##BPP *line;                                                             \
   int xx,yy;                                                                   \
                                                                                \
   for(yy=(SIZE-1); yy>=0; yy--){                                               \
      line = ((UINT##BPP *)GameBitmap->line[y+yy]) + x;                         \
      pline = pbitmap->line[y+yy] + x;                                          \
      for(xx=0; xx<SIZE; xx++){                                                 \
          if (((1 << pline[xx]) & mask) == 0)                                   \
              line[xx] = ((UINT##BPP *)cmap)[ SPR[xx] ];                        \
          pline[xx] = 31;                                                       \
      }                                                                         \
      SPR += SIZE;                                                              \
   }                                                                            \
}                                                                               \
                                                                                \
void pdraw##SIZE##x##SIZE##_Mapped_Maskcps2_##BPP##_FlipY(ARG_MAP)              \
{                                                                               \
   UINT8 *pline;                                                                \
   UINT##BPP *line;                                                             \
   int xx,yy;                                                                   \
                                                                                \
   for(yy=0; yy<SIZE; yy++){                                                    \
      line = ((UINT##BPP *)GameBitmap->line[y+yy]) + x;                         \
      pline = pbitmap->line[y+yy] + x;                                          \
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){                                      \
          if (((1 << pline[xx]) & mask) == 0)                                   \
              line[xx] = ((UINT##BPP *)cmap)[ *SPR ];                           \
          pline[xx] = 31;                                                       \
      }                                                                         \
   }                                                                            \
}                                                                               \
                                                                                \
void pdraw##SIZE##x##SIZE##_Mapped_Maskcps2_##BPP##_FlipXY(ARG_MAP)             \
{                                                                               \
   UINT8 *pline;                                                                \
   UINT##BPP *line;                                                             \
   int xx,yy;                                                                   \
                                                                                \
   for(yy=(SIZE-1); yy>=0; yy--){                                               \
      line = ((UINT##BPP *)GameBitmap->line[y+yy]) + x;                         \
      pline = pbitmap->line[y+yy] + x;                                          \
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){                                      \
          if (((1 << pline[xx]) & mask) == 0)                                   \
              line[xx] = ((UINT##BPP *)cmap)[ *SPR ];                           \
          pline[xx] = 31;                                                       \
      }                                                                         \
   }                                                                            \
}                                                                               \
                                                                                \
/* Transparent */                                                               \
                                                                                \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_Maskcps2_##BPP(ARG_MAP)                \
{                                                                               \
   UINT8 *pline;                                                                \
   UINT##BPP *line;                                                             \
   int xx,yy;                                                                   \
                                                                                \
   for(yy=0; yy<SIZE; yy++){                                                    \
      line = ((UINT##BPP *)GameBitmap->line[y+yy]) + x;                         \
      pline = pbitmap->line[y+yy] + x;                                          \
      for(xx=0; xx<SIZE; xx++){                                                 \
        if(SPR[xx]) {                                                           \
            if (((1 << pline[xx]) & mask) == 0)                                 \
                line[xx] = ((UINT##BPP *)cmap)[ SPR[xx] ];                      \
            pline[xx] = 31;                                                     \
        }                                                                       \
      }                                                                         \
      SPR += SIZE;                                                              \
   }                                                                            \
}                                                                               \
                                                                                \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_Maskcps2_##BPP##_FlipX(ARG_MAP)        \
{                                                                               \
   UINT8 *pline;                                                                \
   UINT##BPP *line;                                                             \
   int xx,yy;                                                                   \
                                                                                \
   for(yy=(SIZE-1); yy>=0; yy--){                                               \
      line = ((UINT##BPP *)GameBitmap->line[y+yy]) + x;                         \
      pline = pbitmap->line[y+yy] + x;                                          \
      for(xx=0; xx<SIZE; xx++){                                                 \
        if(SPR[xx]) {                                                           \
            if (((1 << pline[xx]) & mask) == 0)                                 \
                line[xx] = ((UINT##BPP *)cmap)[ SPR[xx] ];                      \
            pline[xx] = 31;                                                     \
        }                                                                       \
      }                                                                         \
      SPR += SIZE;                                                              \
   }                                                                            \
}                                                                               \
                                                                                \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_Maskcps2_##BPP##_FlipY(ARG_MAP)        \
{                                                                               \
   UINT8 *pline;                                                                \
   UINT##BPP *line;                                                             \
   int xx,yy;                                                                   \
                                                                                \
   for(yy=0; yy<SIZE; yy++){                                                    \
      line = ((UINT##BPP *)GameBitmap->line[y+yy]) + x;                         \
      pline = pbitmap->line[y+yy] + x;                                          \
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){                                      \
        if(*SPR) {                                                              \
            if (((1 << pline[xx]) & mask) == 0)                                 \
                line[xx] = ((UINT##BPP *)cmap)[ *SPR ];                         \
            pline[xx] = 31;                                                     \
        }                                                                       \
      }                                                                         \
   }                                                                            \
}                                                                               \
                                                                                \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_Maskcps2_##BPP##_FlipXY(ARG_MAP)       \
{                                                                               \
   UINT8 *pline;                                                                \
   UINT##BPP *line;                                                             \
   int xx,yy;                                                                   \
                                                                                \
   for(yy=(SIZE-1); yy>=0; yy--){                                               \
      line = ((UINT##BPP *)GameBitmap->line[y+yy]) + x;                         \
      pline = pbitmap->line[y+yy] + x;                                          \
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){                                      \
        if(*SPR) {                                                              \
            if (((1 << pline[xx]) & mask) == 0)                                 \
                line[xx] = ((UINT##BPP *)cmap)[ *SPR ];                         \
            pline[xx] = 31;                                                     \
        }                                                                       \
      }                                                                         \
   }                                                                            \
}                                                                               \
                                                                                \
/* Alpha blending */                                                            \
                                                                                \
void pdraw##SIZE##x##SIZE##_Mapped_Maskcps2_alpha_##BPP(ARG_MAP)                \
{                                                                               \
   UINT8 *pline;                                                                \
   UINT##BPP *line;                                                             \
   int xx,yy;                                                                   \
                                                                                \
   for(yy=0; yy<SIZE; yy++){                                                    \
      line = ((UINT##BPP *)GameBitmap->line[y+yy]) + x;                         \
      pline = pbitmap->line[y+yy] + x;                                          \
      for(xx=0; xx<SIZE; xx++){                                                 \
          if (((1 << pline[xx]) & mask) == 0)                                   \
              blend_##BPP(&line[xx],((UINT##BPP *)cmap)[ SPR[xx] ]);            \
            pline[xx] = 31;                                                     \
      }                                                                         \
      SPR += SIZE;                                                              \
   }                                                                            \
}                                                                               \
                                                                                \
void pdraw##SIZE##x##SIZE##_Mapped_Maskcps2_alpha_##BPP##_FlipX(ARG_MAP)        \
{                                                                               \
   UINT8 *pline;                                                                \
   UINT##BPP *line;                                                             \
   int xx,yy;                                                                   \
                                                                                \
   for(yy=(SIZE-1); yy>=0; yy--){                                               \
      line = ((UINT##BPP *)GameBitmap->line[y+yy]) + x;                         \
      pline = pbitmap->line[y+yy] + x;                                          \
      for(xx=0; xx<SIZE; xx++){                                                 \
          if (((1 << pline[xx]) & mask) == 0)                                   \
              blend_##BPP(line[xx], ((UINT##BPP *)cmap)[ SPR[xx] ]);            \
          pline[xx] = 31;                                                       \
      }                                                                         \
      SPR += SIZE;                                                              \
   }                                                                            \
}                                                                               \
                                                                                \
void pdraw##SIZE##x##SIZE##_Mapped_Maskcps2_alpha_##BPP##_FlipY(ARG_MAP)        \
{                                                                               \
   UINT8 *pline;                                                                \
   UINT##BPP *line;                                                             \
   int xx,yy;                                                                   \
                                                                                \
   for(yy=0; yy<SIZE; yy++){                                                    \
      line = ((UINT##BPP *)GameBitmap->line[y+yy]) + x;                         \
      pline = pbitmap->line[y+yy] + x;                                          \
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){                                      \
          if (((1 << pline[xx]) & mask) == 0)                                   \
              blend_##BPP(line[xx], ((UINT##BPP *)cmap)[ *SPR ]);               \
          pline[xx] = 31;                                                       \
      }                                                                         \
   }                                                                            \
}                                                                               \
                                                                                \
void pdraw##SIZE##x##SIZE##_Mapped_Maskcps2_alpha_##BPP##_FlipXY(ARG_MAP)       \
{                                                                               \
   UINT8 *pline;                                                                \
   UINT##BPP *line;                                                             \
   int xx,yy;                                                                   \
                                                                                \
   for(yy=(SIZE-1); yy>=0; yy--){                                               \
      line = ((UINT##BPP *)GameBitmap->line[y+yy]) + x;                         \
      pline = pbitmap->line[y+yy] + x;                                          \
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){                                      \
          if (((1 << pline[xx]) & mask) == 0)                                   \
              blend_##BPP(line[xx], ((UINT##BPP *)cmap)[ *SPR ]);               \
          pline[xx] = 31;                                                       \
      }                                                                         \
   }                                                                            \
}                                                                               \
                                                                                \
/* Transparent */                                                               \
                                                                                \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_Maskcps2_alpha_##BPP(ARG_MAP)          \
{                                                                               \
   UINT8 *pline;                                                                \
   UINT##BPP *line;                                                             \
   int xx,yy;                                                                   \
                                                                                \
   for(yy=0; yy<SIZE; yy++){                                                    \
      line = ((UINT##BPP *)GameBitmap->line[y+yy]) + x;                         \
      pline = pbitmap->line[y+yy] + x;                                          \
      for(xx=0; xx<SIZE; xx++){                                                 \
        if(SPR[xx]) {                                                           \
            if (((1 << pline[xx]) & mask) == 0)                                 \
                blend_##BPP(line[xx], ((UINT##BPP *)cmap)[ SPR[xx] ]);          \
            pline[xx] = 31;                                                     \
        }                                                                       \
      }                                                                         \
      SPR += SIZE;                                                              \
   }                                                                            \
}                                                                               \
                                                                                \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_Maskcps2_alpha_##BPP##_FlipX(ARG_MAP)  \
{                                                                               \
   UINT8 *pline;                                                                \
   UINT##BPP *line;                                                             \
   int xx,yy;                                                                   \
                                                                                \
   for(yy=(SIZE-1); yy>=0; yy--){                                               \
      line = ((UINT##BPP *)GameBitmap->line[y+yy]) + x;                         \
      pline = pbitmap->line[y+yy] + x;                                          \
      for(xx=0; xx<SIZE; xx++){                                                 \
        if(SPR[xx]) {                                                           \
            if (((1 << pline[xx]) & mask) == 0)                                 \
                blend_##BPP(line[xx], ((UINT##BPP *)cmap)[ SPR[xx] ]);          \
            pline[xx] = 31;                                                     \
        }                                                                       \
      }                                                                         \
      SPR += SIZE;                                                              \
   }                                                                            \
}                                                                               \
                                                                                \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_Maskcps2_alpha_##BPP##_FlipY(ARG_MAP)  \
{                                                                               \
   UINT8 *pline;                                                                \
   UINT##BPP *line;                                                             \
   int xx,yy;                                                                   \
                                                                                \
   for(yy=0; yy<SIZE; yy++){                                                    \
      line = ((UINT##BPP *)GameBitmap->line[y+yy]) + x;                         \
      pline = pbitmap->line[y+yy] + x;                                          \
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){                                      \
        if(*SPR) {                                                              \
            if (((1 << pline[xx]) & mask) == 0)                                 \
                blend_##BPP(line[xx], ((UINT##BPP *)cmap)[ *SPR ]);             \
            pline[xx] = 31;                                                     \
        }                                                                       \
      }                                                                         \
   }                                                                            \
}                                                                               \
                                                                                \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_Maskcps2_alpha_##BPP##_FlipXY(ARG_MAP) \
{                                                                               \
   UINT8 *pline;                                                                \
   UINT##BPP *line;                                                             \
   int xx,yy;                                                                   \
                                                                                \
   for(yy=(SIZE-1); yy>=0; yy--){                                               \
      line = ((UINT##BPP *)GameBitmap->line[y+yy]) + x;                         \
      pline = pbitmap->line[y+yy] + x;                                          \
      for(xx=(SIZE-1); xx>=0; xx--,SPR++){                                      \
        if(*SPR) {                                                              \
            if (((1 << pline[xx]) & mask) == 0)                                 \
                blend_##BPP(line[xx], ((UINT##BPP *)cmap)[ *SPR ]);             \
            pline[xx] = 31;                                                     \
        }                                                                       \
      }                                                                         \
   }                                                                            \
}

render(16,8);
render(16,16);
render(16,32);
