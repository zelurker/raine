
#ifdef __cplusplus
extern "C" {
#endif

void load_vimana(void);
void load_outzone(void);
void load_rally_bike(void);
void load_zero_wing(void);
void load_demons_world(void);
void load_hell_fire(void);
void load_fire_shark(void);
void load_truxton(void);

void ClearToaplan1(void);

#define clear_vimana		ClearToaplan1
#define clear_outzone		ClearToaplan1
#define clear_rally_bike	ClearToaplan1
#define clear_zero_wing		ClearToaplan1
#define clear_demons_world	ClearToaplan1
#define clear_hell_fire		ClearToaplan1
#define clear_fire_shark	ClearToaplan1
#define clear_truxton		ClearToaplan1

void ExecuteToaplan1Frame(void);

void ExecuteToaplan1Frame_Sound(void);

void DrawToaplan1(void);

#ifdef __cplusplus
}
#endif
