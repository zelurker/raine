typedef void pdraw_mapped_maskcps2_func(UINT8 *SPR, int x, int y, UINT8 *cmap, int mask);
extern pdraw_mapped_maskcps2_func *pdraw16x16_Trans_Mapped_Maskcps2_Flip_Rot[4],
       *pdraw16x16_Mapped_Maskcps2_Flip_Rot[4];

#define pdraw16x16_Trans_Mapped_Maskcps2_flip_Rot(a, b, c, d, e, g) (*pdraw16x16_Trans_Mapped_Maskcps2_Flip_Rot[g])(a, b, c, d, e)
#define pdraw16x16_Trans_Mapped_Maskcps2_Rot(a, b, c, d, e)        (*pdraw16x16_Trans_Mapped_Maskcps2_Flip_Rot[0])(a, b, c, d, e)
#define pdraw16x16_Trans_Mapped_Maskcps2_FlipY_Rot(a, b, c, d, e)  (*pdraw16x16_Trans_Mapped_Maskcps2_Flip_Rot[1])(a, b, c, d, e)
#define pdraw16x16_Trans_Mapped_Maskcps2_FlipX_Rot(a, b, c, d, e)  (*pdraw16x16_Trans_Mapped_Maskcps2_Flip_Rot[2])(a, b, c, d, e)
#define pdraw16x16_Trans_Mapped_Maskcps2_FlipXY_Rot(a, b, c, d, e) (*pdraw16x16_Trans_Mapped_Maskcps2_Flip_Rot[3])(a, b, c, d, e)

#define pdraw16x16_Mapped_Maskcps2_flip_Rot(a, b, c, d, e, g) (*pdraw16x16_Mapped_Maskcps2_Flip_Rot[g])(a, b, c, d, e)
#define pdraw16x16_Mapped_Maskcps2_Rot(a, b, c, d, e)        (*pdraw16x16_Mapped_Maskcps2_Flip_Rot[0])(a, b, c, d, e)
#define pdraw16x16_Mapped_Maskcps2_FlipY_Rot(a, b, c, d, e)  (*pdraw16x16_Mapped_Maskcps2_Flip_Rot[1])(a, b, c, d, e)
#define pdraw16x16_Mapped_Maskcps2_FlipX_Rot(a, b, c, d, e)  (*pdraw16x16_Mapped_Maskcps2_Flip_Rot[2])(a, b, c, d, e)
#define pdraw16x16_Mapped_Maskcps2_FlipXY_Rot(a, b, c, d, e) (*pdraw16x16_Mapped_Maskcps2_Flip_Rot[3])(a, b, c, d, e)

#undef declare
#undef ARG_MAP

#define ARG_MAP UINT8 *SPR, int x, int y, UINT8 *cmap, int mask
#define declare(SIZE,BPP)                                                  \
void pdraw##SIZE##x##SIZE##_Mapped_Maskcps2_##BPP(ARG_MAP);                \
void pdraw##SIZE##x##SIZE##_Mapped_Maskcps2_##BPP##_FlipX(ARG_MAP);        \
void pdraw##SIZE##x##SIZE##_Mapped_Maskcps2_##BPP##_FlipY(ARG_MAP);        \
void pdraw##SIZE##x##SIZE##_Mapped_Maskcps2_##BPP##_FlipXY(ARG_MAP);       \
                                                                           \
/* ##SIZE##x##SIZE## mapped transparent SPRites */                         \
                                                                           \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_Maskcps2_##BPP(ARG_MAP);          \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_Maskcps2_##BPP##_FlipX(ARG_MAP);  \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_Maskcps2_##BPP##_FlipY(ARG_MAP);  \
void pdraw##SIZE##x##SIZE##_Trans_Mapped_Maskcps2_##BPP##_FlipXY(ARG_MAP);

declare(16,8);
declare(16,16);
declare(16,32);

#undef declare
#undef ARG_MAP

