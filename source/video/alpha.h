void init_alpha(UINT32 my_alpha);
// the variables are accessed by asm functions and do not need to be declared here
void blend_16(UINT16 *dest, UINT16 src);
void blend50_16(UINT16 *dest, UINT16 src);
void blend_32(UINT32 *dest, UINT32 src);
void blend50_32(UINT32 *dest, UINT32 src);
