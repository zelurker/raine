#ifdef __cplusplus
extern "C" {
#endif

// The params for fsel are a little strange : mypath is the initial path
// ext is an array of strings finished by null with the optional allowed extensions
// res will contain the final result, and initially contains the current selection
// which means that if you want to call fsel repeatedly to select a specific file, you have to compute the dir seperately
void fsel(char *mypath, char **ext, char *res,char *title);
void fsel_save(char *mypath, char **ext, char *res,char *title);
void dsel(char *mypath, char **ext, char *res_str,char *title);
int qsort_menu(const void *a, const void *b); // sort for qsort on label

#ifdef __cplusplus
}
#endif
