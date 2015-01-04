#ifdef __cplusplus
extern "C" {
#endif
int get_spr_alpha(long value);
void free_alpha_tree();
void read_bld();

extern int use_bld,
       bld1,bld2; // values in % to use for values 1 & 2 in the file

#ifdef __cplusplus
}
#endif

