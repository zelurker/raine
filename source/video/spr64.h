
#ifdef __cplusplus
extern "C" {
#endif
#include "deftypes.h"

// 64x64 Transparent Mapped Zoomed Sprites

void Init64x64_Table(UINT8 *TPOS);

void Draw64x64_Trans_Mapped_Zoom(UINT8 *SPR, int x, int y, UINT8 *cmap, int zoom);
void Draw64x64_Trans_Mapped_Zoom_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap, int zoom);
void Draw64x64_Trans_Mapped_Zoom_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap, int zoom);
void Draw64x64_Trans_Mapped_Zoom_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap, int zoom);

void Draw64x64_Mapped_Zoom2(UINT8 *SPR, int x, int y, UINT8 *cmap, int zoomx, int zoomy);
void Draw64x64_Mapped_Zoom2_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap, int zoomx, int zoomy);
void Draw64x64_Mapped_Zoom2_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap, int zoomx, int zoomy);
void Draw64x64_Mapped_Zoom2_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap, int zoomx, int zoomy);

void Draw64x64_Trans_Mapped_Zoom2(UINT8 *SPR, int x, int y, UINT8 *cmap, int zoomx, int zoomy);
void Draw64x64_Trans_Mapped_Zoom2_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap, int zoomx, int zoomy);
void Draw64x64_Trans_Mapped_Zoom2_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap, int zoomx, int zoomy);
void Draw64x64_Trans_Mapped_Zoom2_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap, int zoomx, int zoomy);

/***************************************/

#ifdef __cplusplus
}
#endif
