#ifdef __cplusplus
extern "C" {
#endif

extern int show_song;

void init_assoc(int kind);
int get_asso_loop(int cmd);
void set_asso_loop(int cmd, int lp);
int get_assoc_adr();
char* get_assoc(int cmd);
void del_assoc(int cmd);
void assoc(int cmd, char *t);
void save_assoc(char *section);
void load_assoc(char *section);
int handle_sound_cmd(int cmd);
int get_assoc_type();

#ifdef __cplusplus
}
#endif
