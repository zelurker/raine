#ifdef __cplusplus
extern "C" {
#endif

void fsel(char *mypath, char **ext, char *res,char *title);
void fsel_save(char *mypath, char **ext, char *res,char *title);
void dsel(char *mypath, char **ext, char *res_str,char *title);
int qsort_menu(const void *a, const void *b); // sort for qsort on label

#ifdef __cplusplus
}
#endif
