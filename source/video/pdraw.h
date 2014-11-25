
#ifdef __cplusplus
extern "C" {
#endif

// 16x16 mapped sprites

void pdraw16x16_Mapped_8(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mapped_8_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mapped_8_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mapped_8_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

// 16x16 mapped transparent sprites

void pdraw16x16_Trans_Mapped_8(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Trans_Mapped_8_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Trans_Mapped_8_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Trans_Mapped_8_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

// 16bpp

void pdraw16x16_Mapped_16(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mapped_16_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mapped_16_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mapped_16_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

// 16x16 mapped transparent sprites

void pdraw16x16_Trans_Mapped_16(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Trans_Mapped_16_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Trans_Mapped_16_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Trans_Mapped_16_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

// 32 bpp

void pdraw16x16_Mapped_32(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mapped_32_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mapped_32_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mapped_32_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

// 16x16 mapped transparent sprites

void pdraw16x16_Trans_Mapped_32(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Trans_Mapped_32_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Trans_Mapped_32_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Trans_Mapped_32_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

  // back version : use background bitmap

// 16x16 mapped sprites

void pdraw16x16_Mapped_back_8(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mapped_back_8_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mapped_back_8_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mapped_back_8_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

// 16x16 mapped transparent sprites

void pdraw16x16_Trans_Mapped_back_8(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Trans_Mapped_back_8_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Trans_Mapped_back_8_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Trans_Mapped_back_8_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

// 16bpp

void pdraw16x16_Mapped_back_16(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mapped_back_16_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mapped_back_16_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mapped_back_16_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

// 16x16 mapped transparent sprites

void pdraw16x16_Trans_Mapped_back_16(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Trans_Mapped_back_16_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Trans_Mapped_back_16_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Trans_Mapped_back_16_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

// 32 bpp

void pdraw16x16_Mapped_back_32(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mapped_back_32_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mapped_back_32_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mapped_back_32_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

// 16x16 mapped transparent sprites

void pdraw16x16_Trans_Mapped_back_32(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Trans_Mapped_back_32_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Trans_Mapped_back_32_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Trans_Mapped_back_32_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

  // mask version : update pbitmap and GameBitmap at the same time

// 16x16 mapped sprites

void pdraw16x16_Mask_Mapped_8(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mask_Mapped_8_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mask_Mapped_8_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mask_Mapped_8_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

// 16x16 mapped transparent sprites

void pdraw16x16_Mask_Trans_Mapped_8(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mask_Trans_Mapped_8_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mask_Trans_Mapped_8_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mask_Trans_Mapped_8_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

// 16bpp

void pdraw16x16_Mask_Mapped_16(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mask_Mapped_16_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mask_Mapped_16_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mask_Mapped_16_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

// 16x16 mapped transparent sprites

void pdraw16x16_Mask_Trans_Mapped_16(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mask_Trans_Mapped_16_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mask_Trans_Mapped_16_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mask_Trans_Mapped_16_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

// 32 bpp

void pdraw16x16_Mask_Mapped_32(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mask_Mapped_32_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mask_Mapped_32_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mask_Mapped_32_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

// 16x16 mapped transparent sprites

void pdraw16x16_Mask_Trans_Mapped_32(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mask_Trans_Mapped_32_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mask_Trans_Mapped_32_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw16x16_Mask_Trans_Mapped_32_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

  /* Rot definitions */

typedef void draw_mapped_func_pri(UINT8 *SPR, int x, int y, UINT8 *cmap, UINT8 pri);

extern draw_mapped_func_pri *pdraw16x16_Mapped_Flip_Rot[4];

#define pdraw16x16_Mapped_flip_Rot(a, b, c, d, e, f) (*pdraw16x16_Mapped_Flip_Rot[e])(a, b, c, d, f)

#define pdraw16x16_Mapped_Rot(a, b, c, d, f)        (*pdraw16x16_Mapped_Flip_Rot[0])(a, b, c, d, f)
#define pdraw16x16_Mapped_FlipY_Rot(a, b, c, d, f)  (*pdraw16x16_Mapped_Flip_Rot[1])(a, b, c, d, f)
#define pdraw16x16_Mapped_FlipX_Rot(a, b, c, d, f)  (*pdraw16x16_Mapped_Flip_Rot[2])(a, b, c, d, f)
#define pdraw16x16_Mapped_FlipXY_Rot(a, b, c, d, f) (*pdraw16x16_Mapped_Flip_Rot[3])(a, b, c, d, f)

extern draw_mapped_func_pri *pdraw16x16_Trans_Mapped_Flip_Rot[4];

#define pdraw16x16_Trans_Mapped_flip_Rot(a, b, c, d, e, f) (*pdraw16x16_Trans_Mapped_Flip_Rot[e])(a, b, c, d, f)

#define pdraw16x16_Trans_Mapped_Rot(a, b, c, d, f)        (*pdraw16x16_Trans_Mapped_Flip_Rot[0])(a, b, c, d, f)
#define pdraw16x16_Trans_Mapped_FlipY_Rot(a, b, c, d, f)  (*pdraw16x16_Trans_Mapped_Flip_Rot[1])(a, b, c, d, f)
#define pdraw16x16_Trans_Mapped_FlipX_Rot(a, b, c, d, f)  (*pdraw16x16_Trans_Mapped_Flip_Rot[2])(a, b, c, d, f)
#define pdraw16x16_Trans_Mapped_FlipXY_Rot(a, b, c, d, f) (*pdraw16x16_Trans_Mapped_Flip_Rot[3])(a, b, c, d, f)

extern draw_mapped_func_pri *pdraw16x16_Mapped_back_Flip_Rot[4];

#define pdraw16x16_Mapped_back_flip_Rot(a, b, c, d, e, f) (*pdraw16x16_Mapped_back_Flip_Rot[e])(a, b, c, d, f)

#define pdraw16x16_Mapped_back_Rot(a, b, c, d, f)        (*pdraw16x16_Mapped_back_Flip_Rot[0])(a, b, c, d, f)
#define pdraw16x16_Mapped_back_FlipY_Rot(a, b, c, d, f)  (*pdraw16x16_Mapped_back_Flip_Rot[1])(a, b, c, d, f)
#define pdraw16x16_Mapped_back_FlipX_Rot(a, b, c, d, f)  (*pdraw16x16_Mapped_back_Flip_Rot[2])(a, b, c, d, f)
#define pdraw16x16_Mapped_back_FlipXY_Rot(a, b, c, d, f) (*pdraw16x16_Mapped_back_Flip_Rot[3])(a, b, c, d, f)

extern draw_mapped_func_pri *pdraw16x16_Trans_Mapped_back_Flip_Rot[4];

#define pdraw16x16_Trans_Mapped_back_flip_Rot(a, b, c, d, e, f) (*pdraw16x16_Trans_Mapped_back_Flip_Rot[e])(a, b, c, d, f)

#define pdraw16x16_Trans_Mapped_back_Rot(a, b, c, d, f)        (*pdraw16x16_Trans_Mapped_back_Flip_Rot[0])(a, b, c, d, f)
#define pdraw16x16_Trans_Mapped_back_FlipY_Rot(a, b, c, d, f)  (*pdraw16x16_Trans_Mapped_back_Flip_Rot[1])(a, b, c, d, f)
#define pdraw16x16_Trans_Mapped_back_FlipX_Rot(a, b, c, d, f)  (*pdraw16x16_Trans_Mapped_back_Flip_Rot[2])(a, b, c, d, f)
#define pdraw16x16_Trans_Mapped_back_FlipXY_Rot(a, b, c, d, f) (*pdraw16x16_Trans_Mapped_back_Flip_Rot[3])(a, b, c, d, f)

extern draw_mapped_func_pri *pdraw16x16_Mask_Mapped_Flip_Rot[4];

#define pdraw16x16_Mask_Mapped_flip_Rot(a, b, c, d, e, f) (*pdraw16x16_Mask_Mapped_Flip_Rot[e])(a, b, c, d, f)

#define pdraw16x16_Mask_Mapped_Rot(a, b, c, d, f)        (*pdraw16x16_Mask_Mapped_Flip_Rot[0])(a, b, c, d, f)
#define pdraw16x16_Mask_Mapped_FlipY_Rot(a, b, c, d, f)  (*pdraw16x16_Mask_Mapped_Flip_Rot[1])(a, b, c, d, f)
#define pdraw16x16_Mask_Mapped_FlipX_Rot(a, b, c, d, f)  (*pdraw16x16_Mask_Mapped_Flip_Rot[2])(a, b, c, d, f)
#define pdraw16x16_Mask_Mapped_FlipXY_Rot(a, b, c, d, f) (*pdraw16x16_Mask_Mapped_Flip_Rot[3])(a, b, c, d, f)

extern draw_mapped_func_pri *pdraw16x16_Mask_Trans_Mapped_Flip_Rot[4];

#define pdraw16x16_Mask_Trans_Mapped_flip_Rot(a, b, c, d, e, f) (*pdraw16x16_Mask_Trans_Mapped_Flip_Rot[e])(a, b, c, d, f)

#define pdraw16x16_Mask_Trans_Mapped_Rot(a, b, c, d, f)        (*pdraw16x16_Mask_Trans_Mapped_Flip_Rot[0])(a, b, c, d, f)
#define pdraw16x16_Mask_Trans_Mapped_FlipY_Rot(a, b, c, d, f)  (*pdraw16x16_Mask_Trans_Mapped_Flip_Rot[1])(a, b, c, d, f)
#define pdraw16x16_Mask_Trans_Mapped_FlipX_Rot(a, b, c, d, f)  (*pdraw16x16_Mask_Trans_Mapped_Flip_Rot[2])(a, b, c, d, f)
#define pdraw16x16_Mask_Trans_Mapped_FlipXY_Rot(a, b, c, d, f) (*pdraw16x16_Mask_Trans_Mapped_Flip_Rot[3])(a, b, c, d, f)

  // 8x8 definitions : a copy & paste of the 16x16 declarations with replace...

  // mask version : update pbitmap and GameBitmap at the same time

// 8x8 mapped sprites

void pdraw8x8_Mask_Mapped_8(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Mask_Mapped_8_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Mask_Mapped_8_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Mask_Mapped_8_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

// 8x8 mapped transparent sprites

void pdraw8x8_Mask_Trans_Mapped_8(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Mask_Trans_Mapped_8_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Mask_Trans_Mapped_8_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Mask_Trans_Mapped_8_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

// 16bpp

void pdraw8x8_Mask_Mapped_16(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Mask_Mapped_16_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Mask_Mapped_16_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Mask_Mapped_16_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

// 8x8 mapped transparent sprites

void pdraw8x8_Mask_Trans_Mapped_16(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Mask_Trans_Mapped_16_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Mask_Trans_Mapped_16_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Mask_Trans_Mapped_16_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

// 32 bpp

void pdraw8x8_Mask_Mapped_32(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Mask_Mapped_32_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Mask_Mapped_32_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Mask_Mapped_32_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

// 8x8 mapped transparent sprites

void pdraw8x8_Mask_Trans_Mapped_32(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Mask_Trans_Mapped_32_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Mask_Trans_Mapped_32_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Mask_Trans_Mapped_32_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

// 32x32 mapped sprites

void pdraw32x32_Mask_Mapped_8(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw32x32_Mask_Mapped_8_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw32x32_Mask_Mapped_8_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw32x32_Mask_Mapped_8_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

// 32x32 mapped transparent sprites

void pdraw32x32_Mask_Trans_Mapped_8(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw32x32_Mask_Trans_Mapped_8_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw32x32_Mask_Trans_Mapped_8_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw32x32_Mask_Trans_Mapped_8_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

// 16bpp

void pdraw32x32_Mask_Mapped_16(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw32x32_Mask_Mapped_16_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw32x32_Mask_Mapped_16_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw32x32_Mask_Mapped_16_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

// 32x32 mapped transparent sprites

void pdraw32x32_Mask_Trans_Mapped_16(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw32x32_Mask_Trans_Mapped_16_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw32x32_Mask_Trans_Mapped_16_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw32x32_Mask_Trans_Mapped_16_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

// 32 bpp

void pdraw32x32_Mask_Mapped_32(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw32x32_Mask_Mapped_32_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw32x32_Mask_Mapped_32_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw32x32_Mask_Mapped_32_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

// 32x32 mapped transparent sprites

void pdraw32x32_Mask_Trans_Mapped_32(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw32x32_Mask_Trans_Mapped_32_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw32x32_Mask_Trans_Mapped_32_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw32x32_Mask_Trans_Mapped_32_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

  /* Rot definitions */

extern draw_mapped_func_pri *pdraw8x8_Mask_Mapped_Flip_Rot[4];

#define pdraw8x8_Mask_Mapped_flip_Rot(a, b, c, d, e, f) (*pdraw8x8_Mask_Mapped_Flip_Rot[e])(a, b, c, d, f)

#define pdraw8x8_Mask_Mapped_Rot(a, b, c, d, f)        (*pdraw8x8_Mask_Mapped_Flip_Rot[0])(a, b, c, d, f)
#define pdraw8x8_Mask_Mapped_FlipY_Rot(a, b, c, d, f)  (*pdraw8x8_Mask_Mapped_Flip_Rot[1])(a, b, c, d, f)
#define pdraw8x8_Mask_Mapped_FlipX_Rot(a, b, c, d, f)  (*pdraw8x8_Mask_Mapped_Flip_Rot[2])(a, b, c, d, f)
#define pdraw8x8_Mask_Mapped_FlipXY_Rot(a, b, c, d, f) (*pdraw8x8_Mask_Mapped_Flip_Rot[3])(a, b, c, d, f)

extern draw_mapped_func_pri *pdraw8x8_Mask_Trans_Mapped_Flip_Rot[4];

#define pdraw8x8_Mask_Trans_Mapped_flip_Rot(a, b, c, d, e, f) (*pdraw8x8_Mask_Trans_Mapped_Flip_Rot[e])(a, b, c, d, f)

#define pdraw8x8_Mask_Trans_Mapped_Rot(a, b, c, d, f)        (*pdraw8x8_Mask_Trans_Mapped_Flip_Rot[0])(a, b, c, d, f)
#define pdraw8x8_Mask_Trans_Mapped_FlipY_Rot(a, b, c, d, f)  (*pdraw8x8_Mask_Trans_Mapped_Flip_Rot[1])(a, b, c, d, f)
#define pdraw8x8_Mask_Trans_Mapped_FlipX_Rot(a, b, c, d, f)  (*pdraw8x8_Mask_Trans_Mapped_Flip_Rot[2])(a, b, c, d, f)
#define pdraw8x8_Mask_Trans_Mapped_FlipXY_Rot(a, b, c, d, f) (*pdraw8x8_Mask_Trans_Mapped_Flip_Rot[3])(a, b, c, d, f)

  // 32x32

extern draw_mapped_func_pri *pdraw32x32_Mask_Mapped_Flip_Rot[4];

#define pdraw32x32_Mask_Mapped_flip_Rot(a, b, c, d, e, f) (*pdraw32x32_Mask_Mapped_Flip_Rot[e])(a, b, c, d, f)

#define pdraw32x32_Mask_Mapped_Rot(a, b, c, d, f)        (*pdraw32x32_Mask_Mapped_Flip_Rot[0])(a, b, c, d, f)
#define pdraw32x32_Mask_Mapped_FlipY_Rot(a, b, c, d, f)  (*pdraw32x32_Mask_Mapped_Flip_Rot[1])(a, b, c, d, f)
#define pdraw32x32_Mask_Mapped_FlipX_Rot(a, b, c, d, f)  (*pdraw32x32_Mask_Mapped_Flip_Rot[2])(a, b, c, d, f)
#define pdraw32x32_Mask_Mapped_FlipXY_Rot(a, b, c, d, f) (*pdraw32x32_Mask_Mapped_Flip_Rot[3])(a, b, c, d, f)

extern draw_mapped_func_pri *pdraw32x32_Mask_Trans_Mapped_Flip_Rot[4];

#define pdraw32x32_Mask_Trans_Mapped_flip_Rot(a, b, c, d, e, f) (*pdraw32x32_Mask_Trans_Mapped_Flip_Rot[e])(a, b, c, d, f)

#define pdraw32x32_Mask_Trans_Mapped_Rot(a, b, c, d, f)        (*pdraw32x32_Mask_Trans_Mapped_Flip_Rot[0])(a, b, c, d, f)
#define pdraw32x32_Mask_Trans_Mapped_FlipY_Rot(a, b, c, d, f)  (*pdraw32x32_Mask_Trans_Mapped_Flip_Rot[1])(a, b, c, d, f)
#define pdraw32x32_Mask_Trans_Mapped_FlipX_Rot(a, b, c, d, f)  (*pdraw32x32_Mask_Trans_Mapped_Flip_Rot[2])(a, b, c, d, f)
#define pdraw32x32_Mask_Trans_Mapped_FlipXY_Rot(a, b, c, d, f) (*pdraw32x32_Mask_Trans_Mapped_Flip_Rot[3])(a, b, c, d, f)

  // 8x8 normal sprites

void pdraw8x8_Mapped_8(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Mapped_8_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Mapped_8_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Mapped_8_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

// 8x8 mapped transparent sprites

void pdraw8x8_Trans_Mapped_8(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Trans_Mapped_8_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Trans_Mapped_8_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Trans_Mapped_8_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

// 16bpp

void pdraw8x8_Mapped_16(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Mapped_16_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Mapped_16_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Mapped_16_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

// 8x8 mapped transparent sprites

void pdraw8x8_Trans_Mapped_16(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Trans_Mapped_16_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Trans_Mapped_16_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Trans_Mapped_16_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

// 32 bpp

void pdraw8x8_Mapped_32(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Mapped_32_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Mapped_32_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Mapped_32_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

// 8x8 mapped transparent sprites

void pdraw8x8_Trans_Mapped_32(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Trans_Mapped_32_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Trans_Mapped_32_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);
void pdraw8x8_Trans_Mapped_32_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap,  UINT8 pri);

  /* Rot definitions */

extern draw_mapped_func_pri *pdraw8x8_Mapped_Flip_Rot[4];

#define pdraw8x8_Mapped_flip_Rot(a, b, c, d, e, f) (*pdraw8x8_Mapped_Flip_Rot[e])(a, b, c, d, f)

#define pdraw8x8_Mapped_Rot(a, b, c, d, f)        (*pdraw8x8_Mapped_Flip_Rot[0])(a, b, c, d, f)
#define pdraw8x8_Mapped_FlipY_Rot(a, b, c, d, f)  (*pdraw8x8_Mapped_Flip_Rot[1])(a, b, c, d, f)
#define pdraw8x8_Mapped_FlipX_Rot(a, b, c, d, f)  (*pdraw8x8_Mapped_Flip_Rot[2])(a, b, c, d, f)
#define pdraw8x8_Mapped_FlipXY_Rot(a, b, c, d, f) (*pdraw8x8_Mapped_Flip_Rot[3])(a, b, c, d, f)

extern draw_mapped_func_pri *pdraw8x8_Trans_Mapped_Flip_Rot[4];

#define pdraw8x8_Trans_Mapped_flip_Rot(a, b, c, d, e, f) (*pdraw8x8_Trans_Mapped_Flip_Rot[e])(a, b, c, d, f)

#define pdraw8x8_Trans_Mapped_Rot(a, b, c, d, f)        (*pdraw8x8_Trans_Mapped_Flip_Rot[0])(a, b, c, d, f)
#define pdraw8x8_Trans_Mapped_FlipY_Rot(a, b, c, d, f)  (*pdraw8x8_Trans_Mapped_Flip_Rot[1])(a, b, c, d, f)
#define pdraw8x8_Trans_Mapped_FlipX_Rot(a, b, c, d, f)  (*pdraw8x8_Trans_Mapped_Flip_Rot[2])(a, b, c, d, f)
#define pdraw8x8_Trans_Mapped_FlipXY_Rot(a, b, c, d, f) (*pdraw8x8_Trans_Mapped_Flip_Rot[3])(a, b, c, d, f)

#ifdef __cplusplus
}
#endif
