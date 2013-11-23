#ifdef __cplusplus
extern "C" {
#endif

extern int supports_sound_assoc;
char* get_assoc(int cmd);
void del_assoc(int cmd);
void assoc(int cmd, char *t);
void save_assoc(char *section);
void load_assoc(char *section);
int handle_sound_cmd(int cmd);

#ifdef __cplusplus
}
#endif
