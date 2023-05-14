#ifdef RAINE_WIN32
#include <windows.h> // GetLogicalDrives
#endif
#include "raine.h"
#include "../gui/menu.h"
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#ifdef RAINE_WIN32
#include "fnmatch/fnmatch.h"
#else
#include <fnmatch.h>
#endif
#include "fsel.h"
#include "gui.h"
#include "SDL_gfx/SDL_gfxPrimitives.h"
#include "dialogs/messagebox.h"

static int selected_path;
TFileSel *fsel_dlg;

static int sel_path(int sel) {
  selected_path = sel;
  return 1;
}

class path_sel {
  private:
    menu_item_t *menu;
    int nb,max;
  public:
    path_sel() {
      menu = NULL;
      nb = max = 0;
    }
    ~path_sel() {
      if (menu) {
	for(nb--; nb>= 0; nb--)
	  free((void*)menu[nb].label);
	free(menu);
      }
    }
    void add_item(char *path) {
      if (!path)
	return;
      if (nb == max) {
	max += 10;
	menu = (menu_item_t*)realloc(menu,(max+1)*sizeof(menu_item_t));
	memset(&menu[nb],0,(11)*sizeof(menu_item_t));
      }
      menu[nb].label = strdup(path);
      menu[nb].menu_func = sel_path;
      nb++;
    }
    menu_item_t *get_menu() {
      return menu;
    }
};

static int exec_dir(int sel) {
  // just a wrapper to call mychdir
  return fsel_dlg->mychdir(sel);
}

static int exec_file(int sel) {
  return fsel_dlg->myexec_file(sel);
}

class TPathDlg : public TMenu
{
  public:
  TPathDlg(char *my_title, menu_item_t *menu) :
    TMenu(my_title,menu, NULL,-1, -1,-1,-1, /* to_translate */ 0)
  {}
  char *get_top_string() {
    return fsel_dlg->get_top_string();
  }
};

static int do_paths(int sel) {
  path_sel *paths = new path_sel();
  paths->add_item(getenv("HOME"));
#ifdef RAINE_UNIX
  int pipe_opened = 0;
  FILE *f = fopen("/etc/mtab","r");
  if (!f) {
      // Darwin doesn't seem to have a file for that, so let's use popen...
      f = popen("mount","r");
      pipe_opened = 1;
  }
  if (f) {
    char buff[2048];
    while (!feof(f)) {
      fgets(buff,2048,f);
      if (strncmp(buff,"/dev/",5) && !pipe_opened) // keep only the /dev entries
	continue;
      char *s1 = strchr(buff,' ');
      if (!s1) continue;
      char *s2 = strchr(s1+1,' ');
      if (!s2) continue;
      *s2 = 0;
      if (pipe_opened && strcmp(s1+1,"on")) {
	  // Super darwin puts spaces everywhere in its mounts making it hard
	  // to parse, we'll jump directly to "on" then...
	  // Plus strnstr seems unreliable !
	  s1 = s2+1;
	  while (*s1 && (s1[0] != 'o' || s1[1] != 'n'))
	      s1++;
	  if (!*s1) {
	      s1 = s2;
	  } else {
	      s1[2] = 0;
	      s2 = s1+2;
	      s1--;
	  }
      }
      if (!strcmp(s1+1,"on")) {
	  // darwin speciality, just ignore it
	  s1 = s2;
	  s2 = strchr(s1+1,'(');
	  if (!s2) continue;
	  s2[-1] = 0;
      }
      if (strcmp(s1+1,"/dev")) {
	  paths->add_item(s1+1);
      }
    }
    if (pipe_opened)
	pclose(f);
    else
	fclose(f);
  }
#else
  // windows
  paths->add_item(getenv("PROGRAMFILES"));
  paths->add_item(getenv("USERPROFILE"));
  int drives = GetLogicalDrives();
  int n;
  char path[3];
  strcpy(path,"x:");
  for (n=0; n<32; n++) {
    if (drives & (1<<n)) {
      path[0]=65+n;
      paths->add_item(path);
    }
  }
#endif
  selected_path = -1;
  TMenu *menu = new TPathDlg("",paths->get_menu());
  menu->execute();
  if (selected_path >= 0)
    fsel_dlg->set_dir((char*)paths->get_menu()[selected_path].label);
  delete menu;
  delete paths;
  return 0;
}

int qsort_menu(const void *a, const void *b) {
  return stricmp(((menu_item_t *)a)->label, ((menu_item_t *)b)->label);
}

TFileSel::TFileSel(char *my_title, char *mypath, char **myext, char *res_str, int opts,char *mytitle2) :
  TMain_menu(my_title,NULL)
{
  char *s;
  title2 = mytitle2;
  options = opts;
  strcpy(path,mypath);
  int l = strlen(path);
  if (path[l-1] == SLASH[0])
    path[l-1] = 0;

  if ( !strchr(path,SLASH[0])
#ifdef RAINE_WIN32
    && path[1] != ':'
#endif
    ) {
    strcpy(path,dir_cfg.exe_path);
    path[strlen(path)-1] = 0;
  }
  ext = myext;

  // First check if the path exists, and if not take the 1st one which does
  struct stat buf;
  int res;
  do {
    res = stat(path,&buf);
    if (res < 0) { // does not exist
      s = strrchr(path,SLASH[0]);
      if (s) {
	*s = 0;
	if (s > path && s[-1] == SLASH[0]) s[-1] = 0; // for windows and its stupide double \\ !
      }
      else {
	strcat(path,SLASH);
	res = 0;
      }
    }
  } while (res < 0);
  title = path;
  res_file = res_str;
}

char* TFileSel::get_top_string() {
  if (title2 && *title2)
    return title2;
  else
    return TMain_menu::get_top_string();
}

static char res_file[FILENAME_MAX+1];
static int ihead;

static int validate_file(int res) {
    snprintf(fsel_dlg->res_file,FILENAME_MAX,"%s%s%s",fsel_dlg->path,SLASH,res_file);
    return 1;
}

static menu_item_t myheader_save[] = {
    { _("New file"), &validate_file, &ihead, ITEM_EDIT, { FILENAME_MAX, 0, 200 }, { res_file } },
    { _("-- Path... "), &do_paths },
    { NULL }
};

static menu_item_t myheader[] = {
    { _("-- Path... "), &do_paths },
    { NULL }
};

void TFileSel::compute_nb_items() {
  if (menu)
    free_files();
  nb_menu = 10;
  menu = (menu_item_t *)malloc(sizeof(menu_item_t)*(nb_menu+1));
  memset(menu,0,sizeof(menu_item_t)*(nb_menu+1));

  menu[0].label = "..";
  menu[0].menu_func = &exec_dir;
  nb_files = 1;
  add_files();
}

void TFileSel::add_files() {
#ifdef RAINE_WIN32
    wchar_t wpath[strlen(path)+1];
    mbstowcs(wpath,path,strlen(path)+1);
    _WDIR *dir = _wopendir(wpath);
#else
  DIR *dir = opendir(path);
#endif
  char cwd[1024];
  char tmp_path[1024];
  int found_cue = 0, found_iso = 0;
  char *oldsel = strrchr(res_file,SLASH[0]);
  int check_files = nb_files > 1;
  if (oldsel) oldsel++;

  if (!dir) {
    perror(path);
    nb_files = 0;
  } else {
      chdir(path);
    getcwd(cwd,1024);
#ifdef RAINE_WIN32
    struct _wdirent *dent;
    while ((dent = _wreaddir(dir))) {
	if (!wcscmp(dent->d_name,L".") || !wcscmp(dent->d_name,L".."))
	    continue;
	int max = 0;
	int len = wcslen(dent->d_name);
	for (int n=0; n<len; n++)
	    if (dent->d_name[n] > max)
		max = dent->d_name[n];
	if (max <= 255) {
	    char *s;
	    menu[nb_files].label = s = (char*)malloc(len+1);
	    for (int n=0; n<=len; n++)
		s[n] = dent->d_name[n];
	    snprintf(tmp_path,1024,"%s%s%s",path,SLASH,s);
	} else {
	    // complex part, we have a char out of bounds...
	    char utf[FILENAME_MAX];
	    *utf = 0;
	    WideCharToMultiByte(CP_UTF8,
		    0,
		    dent->d_name,
		    -1,
		    utf,
		    FILENAME_MAX,
		    NULL,
		    NULL);
	    menu[nb_files].label = strdup(utf);
	    wchar_t sh[FILENAME_MAX];
	    *sh = 0;
	    GetShortPathNameW(dent->d_name,sh,FILENAME_MAX);
	    int l = wcslen(sh)+1;
	    char *s;
	    menu[nb_files].values_list_label[0] = s = (char*)malloc(l);
	    for (int n=0; n<l; n++) {
		if (sh[n] > 255) {
		    printf("short name out of limits (%d=%d) : %ls\n",n,sh[n],sh);
		}
		s[n] = sh[n];
	    }
	    snprintf(tmp_path,1024,"%s%s%s",path,SLASH,s);
	}
#else
    struct dirent *dent;
    while ((dent = readdir(dir))) {
      if (!strcmp(dent->d_name,".") || !strcmp(dent->d_name,".."))
	continue;
      menu[nb_files].label = strdup(dent->d_name);
      snprintf(tmp_path,1024,"%s%s%s",path,SLASH,dent->d_name);
#endif
      if (check_files) {
	  int found = 0;
	  for( int n=1; n<nb_files; n++) {
	      if (!strcmp(menu[n].label,menu[nb_files].label)) {
		  found = 1;
		  break;
	      }
	  }
	  if (found) {
	      if (menu[nb_files].values_list_label[0]) {
		  free(menu[nb_files].values_list_label[0]);
		  menu[nb_files].values_list_label[0] = NULL;
	      }
	      continue;
	  }
      }
      /* The big stupidity of readdir is that it doesn't set d_type (posix
       * compliant !!!).  */
      struct stat buf;
      if (!stat(tmp_path,&buf) && S_ISDIR(buf.st_mode)) {
	menu[nb_files].menu_func = &exec_dir;
	nb_files++;
      } else if (!(options & ONLY_DIRS)) {
	menu[nb_files].menu_func = &exec_file;
	int found = 0,idx;
	char *s = menu[nb_files].label;
	if (strlen(s) > 3) {
	    if (!stricmp(&s[strlen(s)-3],"iso") ||
		    (strlen(s) > 6 && !stricmp(&s[strlen(s)-6],"iso.gz")))
		found_iso++;
	    else if (!stricmp(&s[strlen(s)-3],"cue"))
		found_cue++;
	}
	if (ext) {
	    for (s=ext[0], idx=1; s; s=ext[idx++]) {
		int l = strlen(s);
		if (strchr(s,'*') || strchr(s,'?')) { // pattern search
		    if (!fnmatch(s,menu[nb_files].label,FNM_CASEFOLD)) {
			found = 1;
			break;
		    }
		} else if (!stricmp(&menu[nb_files].label[strlen(menu[nb_files].label)-l],s)) {
		    // extension only
		    found = 1;
		    break;
		}
	    }
	} else
	    found = 1;
	if (!found) {
	  // doesn't match the extension given
	  free((void*)menu[nb_files].label);
	  if (menu[nb_files].values_list_label[0]) {
	      free(menu[nb_files].values_list_label[0]);
	      menu[nb_files].values_list_label[0] = NULL;
	  }
	} else
	  nb_files++;
      } else { // we got a file, and we want only dirs !
	  free((void*)menu[nb_files].label);
      }

      if (nb_files == nb_menu) {
	nb_menu += 10;
	menu = (menu_item_t *)realloc(menu,sizeof(menu_item_t)*(nb_menu+1));
	memset(&menu[nb_files],0,sizeof(menu_item_t)*11);
	if (!menu) {
	  fatal_error("Failed to reallocate files buffer (%d entries)",nb_menu);
	}
      }
    }
    menu[nb_files].label = NULL;
#ifdef RAINE_WIN32
    _wclosedir(dir);
#else
    closedir(dir);
#endif
    qsort(&menu[1],nb_files-1,sizeof(menu_item_t),&qsort_menu);
    if (oldsel)
	for (int n=1; n<nb_files; n++)
	    if (!strcmp(menu[n].label,oldsel))
		sel = n;
    chdir(cwd);
  }
  strcpy(res_file,path);
  if (path[strlen(path)-1] != SLASH[0])
    strcat(res_file,SLASH);
  if (options & SAVE)
      set_header(myheader_save);
  else if (!(options & NO_HEADER))
      set_header(myheader);
  int tmpsel = sel;
  if (found_iso && !found_cue && strcmp(ext[0],".iso")) {
      char *myexts[] = { ".iso", "iso.gz", "zip", "7z", NULL };
      char **old = ext;
      ext = myexts;
      compute_nb_items();
      ext = old;
  } else
      TMenu::compute_nb_items();
  if (tmpsel > -1)
      sel = tmpsel; // blocks find_new_sel from compute_nb_items
}

void TFileSel::free_files() {
  int nb;
  for (nb=1; nb<nb_files; nb++) {
    free((void*)menu[nb].label);
    if (menu[nb].values_list_label[0]) {
	free(menu[nb].values_list_label[0]);
    }
  }
  if (nb_files && strcmp(menu[0].label,"..")) // if entry 0 is different from .. it can be freed
    free((void*)menu[0].label);
  if (menu)
    free(menu);
  menu = NULL;
  nb_files = 0;
}

TFileSel::~TFileSel() {
    free_files();
}

int TFileSel::get_fgcolor(int n) {
  if (menu[n].menu_func == &exec_dir)
    return mymakecol(160,160,255);
  return fg;
}

void TFileSel::set_dir(char *mypath) {
    chdir(mypath);
    if (mypath != path)
	strcpy(path,mypath);
    if (!strstr(path,SLASH)) {
	getcwd(path,1024);
    }
    if (font) {
	delete font;
	font = NULL;
    }
#if SDL==2
    SDL_DestroyTexture(fg_layer);
    fg_layer = NULL;
#endif
    set_title(path);
    draw();
    strcpy(res_file,path);
    if (path[strlen(path)-1] != SLASH[0])
	strcat(res_file,SLASH); // To show it's a path more easily
}

int TFileSel::mychdir(int n) {
    char *old = NULL;
    if (n == 0 && !strcmp(menu[0].label,"..")) { // .. but check menu[0] in case something changed... !
	char *s = strrchr(path,SLASH[0]);
	if (s) {
	    *s = 0;
	    old = strdup(s+1);
#ifdef RAINE_WIN32
	    if (s > path && s[-1] == SLASH[0]) { s[-1] = 0; // double \ in windows
		printf("double replace for %s\n",path);
	    }
#endif
	}
#ifdef RAINE_WIN32
	if (strlen(path)==2) // only the drive letter is left
	    strcat(path,SLASH);
#else
	if (!*path)
	    strcpy(path,SLASH);
#endif
    } else
#ifdef RAINE_WIN32
	if (menu[n].values_list_label[0]) {
	    sprintf(&path[strlen(path)],"%s%s",SLASH,menu[n].values_list_label[0]);
	} else
#endif
	    sprintf(&path[strlen(path)],"%s%s",SLASH,menu[n].label);
    set_dir(path);
    if (old) {
	for (n=0; n<nb_items; n++)
	    if (!strcmp(menu[n].label,old)) {
		sel = n;
		break;
	    }
	free(old);
    }
    top = 0;
    if (top + rows - 1 < sel) top = sel-rows+1;

    return 0;
}

int TFileSel::myexec_file(int sel) {
  sprintf(res_file,"%s%s%s",path,SLASH,menu[sel].label);
  return 1;
}

void fsel(char *mypath, char **ext, char *res_str, char *title) {
  fsel_dlg = new TFileSel(mypath,mypath,ext,res_str,0,title);
  fsel_dlg->execute();
  delete fsel_dlg;
}

void fsel_save(char *mypath, char **ext, char *res_str, char *title) {
  fsel_dlg = new TFileSel(mypath,mypath,ext,res_str,SAVE,title);
  fsel_dlg->execute();
  delete fsel_dlg;
}

void dsel(char *mypath, char **ext, char *res_str,char *title) {
  fsel_dlg = new TFileSel(mypath,mypath,ext,res_str,ONLY_DIRS,title);
  fsel_dlg->execute();
  delete fsel_dlg;
}

void multi_fsel(char *mypath, char **ext, char **res_str, int max_res, char *title) {
  fsel_dlg = new TMultiFileSel(mypath,mypath,ext,res_str,max_res,0,title);
  fsel_dlg->execute();
  delete fsel_dlg;
}

TMultiFileSel::TMultiFileSel(char *my_title, char *mypath, char **myext, char **my_res_str,int my_max_res,int opts, char* mytitle2) :
    TFileSel(my_title,mypath,myext,my_res_str[0],opts,mytitle2)
{
    selected = NULL;
    max_res = my_max_res;
    res_str = my_res_str;
    if (!res_file) {
	res_file = (char*)malloc(FILENAME_MAX);
	alloc = 1;
	*res_file = 0;
    } else
	alloc = 0;
}

void TMultiFileSel::compute_nb_items() {
    char str[FILENAME_MAX];
    snprintf(str,FILENAME_MAX,"%s%s",path,SLASH);
    int path_changed = strcmp(str,res_file);
    TFileSel::compute_nb_items();
    if (path_changed) {
	if (selected) free(selected);
	nb_sel = 0;
	sel = 0;
	selected = (int*)calloc(nb_files,sizeof(int));
    }
}

int TMultiFileSel::myexec_file(int sel) {
    if (nb_sel == max_res && !selected[sel]) {
	MessageBox("Error","Already have max selections","ok");
	return 0;
    }
    selected[sel] ^= 1;
    if (selected[sel])
	nb_sel++;
    else
	nb_sel--;
    return 0;
}

void TMultiFileSel::compute_width_from_font() {
    TFileSel::compute_width_from_font();
    int h;
    font->dimensions("X",&wcb,&h);
    width_max += wcb+HMARGIN*2;
}

void TMultiFileSel::disp_menu(int n,int x,int y,int w,int h) {
  x += HMARGIN;
  if (menu[n].menu_func != &exec_dir) {
      rectangleColor(rend,x,y,x+wcb,y+h-1,fg);
      if (selected[n]) {
	  lineColor(rend,x,y,x+wcb,y+h-1,mymakecol(0,255,0));
#if 0 // def RAINE_WIN32
	  // Totally crazy bug in windows with sdl-2.0.20 at lest :
	  // the color chosen for the lines here is applied as a filter to the game bitmap when returning to the game
	  // as if using some kind of blend mode, but even calling SDL_SetBlendMode(BLEND_NONE) doesn't change anything
	  // to it. The only way to work around it seems to be to use a white color for the last line !!!
	  // windows only !
	  // Fixed in sdl 2.0.22 apparently ?
	  lineColor(rend,x+wcb,y,x,y+h-1,mymakecol(255,255,255));
#else
	  lineColor(rend,x+wcb,y,x,y+h-1,mymakecol(0,255,0));
#endif
      }
      TMenu::disp_menu(n,x+wcb,y,w-wcb,h);
      return;
  }
  TMenu::disp_menu(n,x+HMARGIN,y,w,h);
}

TMultiFileSel::~TMultiFileSel() {
    int x = 0;
    for (int n=0; n<nb_files; n++) {
	if (selected[n]) {
	    if (!res_str[x])
		res_str[x] = (char*)malloc(FILENAME_MAX);
	    if (res_str[x])
		sprintf(res_str[x++],"%s" SLASH "%s",path,menu[n].label);
	}
    }
    if (alloc)
	free(res_file);
    free_files();
    if (selected) {
	free(selected);
	selected = NULL;
    }
    nb_sel = 0;
}

