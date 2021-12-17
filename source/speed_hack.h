#ifdef __cplusplus
extern "C" {
#endif

void undo_hack();
void set_speed_hack();
int get_speed_hack();
void apply_hack(UINT32 pc, int kind);
void pWriteWord(UINT32 myadr, UINT16 val);
UINT32 get_speed_hack_adr(int n);

#ifdef __cplusplus
}
#endif
