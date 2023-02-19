#ifdef __cplusplus

#include "gui.h"

// the only used option for now !
#define ONLY_DIRS 1
#define SAVE 2
#define NO_HEADER 4

// in case the classes need to be overloaded
// for TMultiFsel it's almost mandatory to be able to update the selection list on dir change
class TFileSel : public TMain_menu
{
  protected:
    int nb_files;
    char **ext;
    int options;
    char *title2;
  public:
    char path[FILENAME_MAX];
    char *res_file;
    TFileSel(char *my_title, char *mypath, char **myext, char *res_str,int opts = 0, char* mytitle2 = NULL);
    virtual char* get_top_string();
    virtual ~TFileSel();
    virtual void compute_nb_items();
    virtual int get_fgcolor(int n);
    int can_be_selected(int n) {
      return can_be_displayed(n);
    }
    int can_be_displayed(int n) {
	return 1;
    }
    virtual void set_dir(char *mypath);
    virtual int mychdir(int n);
    virtual int myexec_file(int sel);
    virtual void free_files();
};

class TMultiFileSel : public TFileSel
{
    protected:
	int *selected,wcb;
	int max_res,nb_sel;
	char **res_str;
	int alloc;
    public:
	TMultiFileSel(char *my_title, char *mypath, char **myext, char **my_res_str,int my_max_res,int opts = 0, char* mytitle2 = NULL);
	~TMultiFileSel();
	virtual void compute_nb_items();
	virtual int myexec_file(int sel);
	virtual void free_files();
	virtual void compute_width_from_font();
	virtual void disp_menu(int n,int x,int y,int w,int h);
};

// Must use this fsel_dlg when invoking an overloaded instance
// there are lots of C hooks in the gui, and you can't put member functions there, so I need a pointer like this to be able to link the right objects to these handlers
extern TFileSel *fsel_dlg;

#endif

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
void multi_fsel(char *mypath, char **ext, char **res_str, int max_res, char *title);

#ifdef __cplusplus
}
#endif
