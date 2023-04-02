#include <string.h>
#include <stdio.h>
#include "ips.h"
#include "raine.h"
#include "sdl/dialogs/messagebox.h"
#include "translate.h"
#include "files.h"
#ifdef MEMWATCH
#include "memwatch.h"
#endif
#include "console/scripts.h" // atoh

tips_info ips_info;

typedef struct {
    int offset, len;
    int index;
    char *rom_name;
} tpatch;

static tpatch *patch;
int alloc_patch, nb_patch;
#define LEN 1024*5
static char ips_error[LEN];

void free_ips_patches() {
    if (patch) {
	free(patch);
	patch = NULL;
    }
}

char *get_ips_error() { return ips_error; }

static void check_patch(int offset, int len, int index, char *rom_name) {
    int end = offset + len;
    if (nb_patch == alloc_patch) {
	alloc_patch += 10;
	patch = realloc(patch, alloc_patch*sizeof(tpatch));
    }

    for (int n=0; n<nb_patch; n++) {
	int pend = patch[n].offset + patch[n].len;
	if (patch[n].rom_name != rom_name)
	    continue;
	if ((pend > offset && pend < offset + len) ||
		(patch[n].offset < end && pend > end)) {
	    snprintf(&ips_error[strlen(ips_error)],LEN-strlen(ips_error),"conflict for rom %s between %s offset %x len %x &\nips %s offset %x len %x\n",rom_name,ips_info.ips[index],offset,len,ips_info.ips[patch[n].index],patch[n].offset,patch[n].len);
	    ips_error[LEN-1] = 0;
	}
    }
    patch[nb_patch].offset = offset;
    patch[nb_patch].len = len;
    patch[nb_patch].rom_name = rom_name;
    patch[nb_patch++].index = index;
}

void load_ips(char *res, unsigned char *ROM, int max_size,int index,char *rom_name) {
    FILE *f = fopen(res,"rb");
    if (!f) {
	printf("couldn't load ips %s\n",res);
	return;
    }
    char buf[6];
    fread(buf,1,5,f);
    if (strncmp(buf,"PATCH",5)) {
	fclose(f);
	MessageBox("Error",_("Bad IPS header"), "Ok");
	return;
    }
    do {
	unsigned char ofs_str[4], len_str[2];
	int n = fread(ofs_str,1,3,f);
	if (!strncmp((char*)ofs_str,"EOF",3)) {
	    break; // normal end
	}
	fread(len_str,1,2,f);
	u32 ofs = (ofs_str[0] << 16) | (ofs_str[1] << 8) | ofs_str[2];
	u16 len = (len_str[0] << 8) | len_str[1];
	if (len == 0) {
	    // rle block : len, followed by byte
	    fread(len_str,1,2,f);
	    u16 len = (len_str[0] << 8) | len_str[1];
	    fread(ofs_str,1,1,f);
	    if (ofs >= max_size) {
		printf("ips : ofs overflow %s %x when max is %x\n",res,ofs,max_size);
		continue;
	    }
	    check_patch(ofs,len,index,rom_name);
	    memset(&ROM[ofs],ofs_str[0],len);
	    continue;
	}
	char buf[len];
	n = fread(buf,1,len,f);
	if (n < len) {
	    MessageBox("IPS Error","Preamture eof", "ok");
	    fclose(f);
	    return;
	}
	if (ofs >= max_size) {
	    printf("ips : ofs overflow %s ofs %x when max is %x\n",res,ofs,max_size);
	    continue;
	}
	if (ofs + len > max_size) {
	    printf("ips: len too big, ofs %x len %x max_size %x\n",ofs,len,max_size);
	    len = max_size-ofs-1;
	}
	check_patch(ofs,len,index,rom_name);
	memcpy(&ROM[ofs],buf,len); // no swap...
    } while (!feof(f));
    fclose(f);
}

void add_ips_file(char *file) {
    FILE *f = fopen(file,"r");
    if (!f) {
	char err[1024];
	sprintf(err,"add_ips_file: Can't open %s",file);
	MessageBox("error",err,"ok");
	return;
    }
    printf("add_ips_file %s\n",file);
    char buf[1024];
    char *s = strrchr(file,SLASH[0]);
    if (!s) {
	char err[1024];
	sprintf(err,"add_ips_file: Can't decode path : %s",file);
	MessageBox("Error",err,"ok");
	fclose(f);
	return;
    }
    char old = s[1];
    s[1] = 0;
    strcpy(ips_info.path,file);
    s[1] = old;
    int nb = ips_info.nb;
    while (!feof(f)) {
	myfgets(buf,1024,f);
	if (!buf[0]) continue; // no empty line at start normally... !
	if (!strncmp(buf,"#define",7)) // found this in a broken dat, nothing to do there
	    continue;
	char *tab = strchr(buf,9);
	if (!tab) break;
	*tab = 0;
	char *tab2 = strchr(&tab[1],9);
	if (tab2) *tab2 = 0; // optional crc after that
	ips_info.rom[nb] = strdup(buf);
	if (tab2)
	    ips_info.crc[nb] = atoh(tab2+5);
	ips_info.ips[nb++] = strdup(&tab[1]);
	printf("add_ips_file: adding rom %s ips %s crc %x (%d)\n",ips_info.rom[nb-1],ips_info.ips[nb-1],ips_info.crc[nb-1],nb-1);
	if (nb == MAX_IPS) {
	    MessageBox("dat error", "Too many roms in this dat !","ok");
	    fclose(f);
	    for (int n=0; n<nb; n++) {
		free(ips_info.rom[n]);
		free(ips_info.ips[n]);
	    }
	    return;
	}
    }
    fclose(f);
    if (nb == 0) {
	char msg[1024];
	sprintf(msg,"%s: couldn't find any rom there... !",&s[1]);

	MessageBox("dat error", msg,"ok");
	for (int n=0; n<nb; n++) {
	    free(ips_info.rom[n]);
	    free(ips_info.ips[n]);
	}
	return;
    }
    ips_info.nb = nb;
}

