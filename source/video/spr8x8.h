
#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************/
/*                                                                            */
/*                          RAINE 8x8 TILE DRAWING                            */
/*                                                                            */
/******************************************************************************/

/*

Mapped
Transparent Mapped
Direct-Mapped
Transparent Direct-Mapped

*/

void init_spr8x8asm(void);

// 8x8 mapped sprites

void Draw8x8_Mapped(UINT8 *SPR, int x, int y, UINT8 *cmap);
void Draw8x8_Mapped_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap);
void Draw8x8_Mapped_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap);
void Draw8x8_Mapped_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap);

// 8x8 mapped transparent sprites

void Draw8x8_Trans_Mapped(UINT8 *SPR, int x, int y, UINT8 *cmap);
void Draw8x8_Trans_Mapped_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap);
void Draw8x8_Trans_Mapped_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap);
void Draw8x8_Trans_Mapped_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap);

void init_spr8x8asm_16(void);

// 8x8 mapped sprites

void Draw8x8_Mapped_16(UINT8 *SPR, int x, int y, UINT8 *cmap);
void Draw8x8_Mapped_16_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap);
void Draw8x8_Mapped_16_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap);
void Draw8x8_Mapped_16_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap);

// 8x8 mapped transparent sprites

void Draw8x8_Trans_Mapped_16(UINT8 *SPR, int x, int y, UINT8 *cmap);
void Draw8x8_Trans_Mapped_16_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap);
void Draw8x8_Trans_Mapped_16_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap);
void Draw8x8_Trans_Mapped_16_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap);

void init_spr8x8asm_32(void);

// 8x8 mapped sprites

void Draw8x8_Mapped_32(UINT8 *SPR, int x, int y, UINT8 *cmap);
void Draw8x8_Mapped_32_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap);
void Draw8x8_Mapped_32_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap);
void Draw8x8_Mapped_32_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap);

// 8x8 mapped transparent sprites

void Draw8x8_Trans_Mapped_32(UINT8 *SPR, int x, int y, UINT8 *cmap);
void Draw8x8_Trans_Mapped_32_FlipX(UINT8 *SPR, int x, int y, UINT8 *cmap);
void Draw8x8_Trans_Mapped_32_FlipY(UINT8 *SPR, int x, int y, UINT8 *cmap);
void Draw8x8_Trans_Mapped_32_FlipXY(UINT8 *SPR, int x, int y, UINT8 *cmap);

// 8x8 sprites

void Draw8x8(UINT8 *SPR, int x, int y, UINT8 cmap);
void Draw8x8_FlipX(UINT8 *SPR, int x, int y, UINT8 cmap);
void Draw8x8_FlipY(UINT8 *SPR, int x, int y, UINT8 cmap);
void Draw8x8_FlipXY(UINT8 *SPR, int x, int y, UINT8 cmap);

// 8x8 transparent sprites

void Draw8x8_Trans(UINT8 *SPR, int x, int y, UINT8 cmap);
void Draw8x8_Trans_FlipX(UINT8 *SPR, int x, int y, UINT8 cmap);
void Draw8x8_Trans_FlipY(UINT8 *SPR, int x, int y, UINT8 cmap);
void Draw8x8_Trans_FlipXY(UINT8 *SPR, int x, int y, UINT8 cmap);

#ifdef __cplusplus
}
#endif
