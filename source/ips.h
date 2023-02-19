#ifdef __cplusplus
extern "C" {
#endif

#define MAX_IPS 20

typedef struct {
    char path[1024];
    int nb;
    char *rom[MAX_IPS],*ips[MAX_IPS];
} tips_info;

extern tips_info ips_info;

void load_ips(char *res, unsigned char *ROM, int max_size);
void add_ips_file(char *file);

#ifdef __cplusplus
}
#endif

