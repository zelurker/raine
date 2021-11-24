
#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************/
/*                                                                            */
/*                         RAINE PIXEL DOUBLE BLITTING                        */
/*                                                                            */
/******************************************************************************/

#include "raine.h"

void raine_blit_x2_y1(al_bitmap *src, al_bitmap *dest, int s_x, int s_y, int d_x, int d_y, int w, int h);

void raine_blit_x2_y2(al_bitmap *src, al_bitmap *dest, int s_x, int s_y, int d_x, int d_y, int w, int h);

void raine_blit_x2_y1_16(al_bitmap *src, al_bitmap *dest, int s_x, int s_y, int d_x, int d_y, int w, int h);

void raine_blit_x2_y2_16(al_bitmap *src, al_bitmap *dest, int s_x, int s_y, int d_x, int d_y, int w, int h);

void raine_blit_x2_y1_24(al_bitmap *src, al_bitmap *dest, int s_x, int s_y, int d_x, int d_y, int w, int h);

void raine_blit_x2_y2_24(al_bitmap *src, al_bitmap *dest, int s_x, int s_y, int d_x, int d_y, int w, int h);

void raine_blit_x2_y1_32(al_bitmap *src, al_bitmap *dest, int s_x, int s_y, int d_x, int d_y, int w, int h);

void raine_blit_x2_y2_32(al_bitmap *src, al_bitmap *dest, int s_x, int s_y, int d_x, int d_y, int w, int h);


#ifdef __cplusplus
}
#endif
