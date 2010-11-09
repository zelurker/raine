
#ifdef __cplusplus
extern "C" {
#endif

void load_iga_ninjyutsuden(void);
void load_soldam(void);
void load_chimera_beast(void);
void LoadSaintDragon(void);
void LoadKickOff(void);
void LoadHachoo(void);
void LoadPlusAlpha(void);
void LoadCybattler(void);
void LoadEarthDefForce(void);
void LoadLegendOfMakaj(void);
void LoadPeekABoo(void);

void DrawLegendOfMakaj(void);
void DrawPeekABoo(void);

void DrawMegaSystem1(void);
void DrawMegaSystem1Vertical(void);

void DrawMegaSystem2(void);
void DrawMegaSystem2Vertical(void);

void ExecuteMegaSystem1Frame(void);
void ExecuteBSPFrame(void);
void ExecuteMegaSystem1FrameKO(void);

void ExecuteMegaSystem2Frame(void);

// edf, hachoo, iganinju, plusalph, rodland, tshingen
#define COINAGE_6BITS \
   { MSG_COIN1,               0x07, 0x08 },\
   { MSG_3COIN_1PLAY,         0x04 },\
   { MSG_2COIN_1PLAY,         0x02 },\
   { MSG_1COIN_1PLAY,         0x07 },\
   { MSG_1COIN_2PLAY,         0x03 },\
   { MSG_1COIN_3PLAY,         0x05 },\
   { MSG_1COIN_4PLAY,         0x01 },\
   { MSG_1COIN_5PLAY,         0x06 },\
   { MSG_FREE_PLAY,           0x00 },\
   { MSG_COIN2,               0x38, 0x08 },\
   { MSG_4COIN_1PLAY,         0x00 },\
   { MSG_3COIN_1PLAY,         0x20 },\
   { MSG_2COIN_1PLAY,         0x10 },\
   { MSG_1COIN_1PLAY,         0x38 },\
   { MSG_1COIN_2PLAY,         0x18 },\
   { MSG_1COIN_3PLAY,         0x28 },\
   { MSG_1COIN_4PLAY,         0x08 },\
   { MSG_1COIN_5PLAY,         0x30 },

// lomakai, p47, peekaboo, stdragon, soldamj
#define COINAGE_6BITS_ALT \
   { MSG_COIN1,               0x07, 0x08 },\
   { MSG_4COIN_1PLAY,         0x01 },\
   { MSG_3COIN_1PLAY,         0x02 },\
   { MSG_2COIN_1PLAY,         0x03 },\
   { MSG_1COIN_1PLAY,         0x07 },\
   { MSG_1COIN_2PLAY,         0x06 },\
   { MSG_1COIN_3PLAY,         0x05 },\
   { MSG_1COIN_4PLAY,         0x04 },\
   { MSG_FREE_PLAY,           0x00 },\
   { MSG_COIN2,               0x38, 0x08 },\
   { MSG_4COIN_1PLAY,         0x08 },\
   { MSG_3COIN_1PLAY,         0x10 },\
   { MSG_2COIN_1PLAY,         0x18 },\
   { MSG_1COIN_1PLAY,         0x38 },\
   { MSG_1COIN_2PLAY,         0x30 },\
   { MSG_1COIN_3PLAY,         0x28 },\
   { MSG_1COIN_4PLAY,         0x20 },\
   { MSG_FREE_PLAY,           0x00 },

#ifdef __cplusplus
}
#endif
