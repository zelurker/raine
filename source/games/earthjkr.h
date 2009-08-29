
#ifdef __cplusplus
extern "C" {
#endif

void LoadEarthJoker(void);
void LoadMazeOfFlott(void);
void LoadGalmedes(void);
void LoadEto(void);

void ClearEarthJoker(void);

#define ClearMazeOfFlott	ClearEarthJoker
#define ClearGalmedes		ClearEarthJoker
#define ClearEto		ClearEarthJoker

void DrawEarthJoker(void);
void DrawEto(void);
void ExecuteEarthJokerFrame(void);

#ifdef __cplusplus
}
#endif
