#ifdef __cplusplus
extern "C" {
#endif

#define MAX_IPS 20

typedef struct {
    char path[1024];
    int nb;
    char *rom[MAX_IPS],*ips[MAX_IPS];
    int crc[MAX_IPS];
} tips_info;

extern tips_info ips_info;

void load_ips(char *res, unsigned char *ROM, int max_size, int index,char *rom_name);
void add_ips_file(char *file);
char *get_ips_error();
void free_ips_patches();

#ifdef __cplusplus
}
#endif

