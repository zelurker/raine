#ifdef __cplusplus
extern "C" {
#endif
#include "games/games.h"

#if SDL != 2
extern int stretchx, stretchy; // max stretch size, keeping ratio
extern int wants_switch_res;
extern int use_scale2x;

extern void get_best_resolution(const VIDEO_INFO *vid,int *w,int *h);
extern void update_stretch();
void InitLUTs(void);
#endif

#ifdef __cplusplus
}
#endif

