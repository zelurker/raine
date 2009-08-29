
#ifdef __cplusplus
extern "C" {
#endif

void LoadNGaiden(void);
void LoadTKnight(void);

void ExecuteTecmoSysFrame(void);

void DrawTecmoSys(void);

void ClearTecmoSys(void);

#define ClearNGaiden		ClearTecmoSys
#define ClearTKnight		ClearTecmoSys

#ifdef __cplusplus
}
#endif
