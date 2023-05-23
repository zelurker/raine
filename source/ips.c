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
#include "7z/7zCrc.h"

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
	nb_patch = alloc_patch = 0;
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
	MessageBox("Error",_("Bad IPS header"), "OK");
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
		if (remaining_b && ofs - max_size < remaining_size) {
		    // load_continue handling
		    ofs -= max_size;
		    if (ofs + len > remaining_size) {
			printf("ips : rle len overflow on remaining_size ofs %x len %x rem size %x\n",ofs,len,remaining_size);
			len = remaining_size - ofs;
		    }
		    check_patch(ofs+max_size,len,index,rom_name);
		    memset(remaining_b+ofs,ofs_str[0],len);
		    continue;
		}
		printf("ips : rle ofs overflow %s %x when max is %x\n",res,ofs,max_size);
		continue;
	    }
	    if (ofs + len > max_size) {
		printf("ips: rle overflow on len ofs %x len %x max %x\n",ofs,len,max_size);
		len = max_size - ofs;
	    }
	    check_patch(ofs,len,index,rom_name);
	    memset(&ROM[ofs],ofs_str[0],len);
	    continue;
	}
	char buf[len];
	n = fread(buf,1,len,f);
	if (n < len) {
	    MessageBox("IPS Error","Premature EOF", "OK");
	    fclose(f);
	    return;
	}
	if (ofs >= max_size) {
	    if (remaining_b && ofs - max_size < remaining_size) {
		/* Applies on a LOAD_CONTINUE : in this case we get a reamining_b & remaining_size pointing to the rest of the file */
		ofs -= max_size;
		if (ofs + len > remaining_size) {
		    printf("ips : len overflow on remaining_size ofs %x len %x rem size %x\n",ofs,len,remaining_size);
		    len = remaining_size - ofs;
		}
		if (memcpy(remaining_b + ofs, buf, len)) {
		    check_patch(ofs+max_size,len,index,rom_name);
		    memcpy(remaining_b + ofs, buf, len);
		}
		continue;
	    }
	    printf("ips : ofs overflow %s ofs %x when max is %x\n",res,ofs,max_size);
	    continue;
	}
	if (ofs + len > max_size) {
	    printf("ips: len too big, ofs %x len %x max_size %x\n",ofs,len,max_size);
	    len = max_size-ofs-1;
	}
	if (memcpy(&ROM[ofs],buf,len)) {
	    check_patch(ofs,len,index,rom_name);
	    memcpy(&ROM[ofs],buf,len); // no swap...
	}
    } while (!feof(f));
    fclose(f);
}

void add_ips_file(char *file) {
    FILE *f = fopen(file,"r");
#ifdef RAINE_UNIX
    if (!f) {
	char *s = strstr(file,"ips");
	char f2[FILENAME_MAX];
	snprintf(f2,FILENAME_MAX,"%s%s",dir_cfg.share_path,s);
	strcpy(file,f2);
	f = fopen(file,"r");
    }
#endif
    if (!f) {
	char err[1024];
	sprintf(err,"add_ips_file: Can't open %s",file);
	MessageBox("Error",err,"OK");
	return;
    }
    printf("add_ips_file %s\n",file);
    char buf[1024];
    char *s = strrchr(file,SLASH[0]);
    if (!s) {
	char err[1024];
	sprintf(err,"add_ips_file: Can't decode path : %s",file);
	MessageBox("Error",err,"OK");
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
	if (buf[0] == '[') break; // don't go into the description !
	char *tab = strchr(buf,9);
	if (!tab) tab = strchr(buf,32);
	if (!tab) break;
	*tab = 0;
	tab++;
	while (*tab == 9 || *tab == 32) tab++;
	char *tab2 = strchr(&tab[1],9);
	if (!tab2) tab2 = strchr(&tab[1],32);
	if (tab2) *tab2 = 0; // optional crc after that
	ips_info.rom[nb] = strdup(buf);
	if (tab2) {
	    tab2++;
	    while (*tab2 == 9 || *tab2 == 32) tab2++;
	    ips_info.crc[nb] = atoh(tab2+4);
	}
	ips_info.ips[nb++] = strdup(&tab[0]);
	printf("add_ips_file: adding rom %s ips %s crc %x (%d)\n",ips_info.rom[nb-1],ips_info.ips[nb-1],ips_info.crc[nb-1],nb-1);
	if (nb >= MAX_IPS) {
	    static int warned;
	    if (!warned) {
			    MessageBox("*.dat file error", "Too many IPS patches in the *.dat files!","OK");
		warned = 1;
	    }
	    fclose(f);
	    nb--;
	    free(ips_info.ips[nb]);
	    free(ips_info.rom[nb]);
	    ips_info.ips[nb] = NULL;
	    ips_info.rom[nb] = NULL;
	    ips_info.nb = nb;
	    return;
	}
    }
    fclose(f);
    if (nb == 0) {
	char msg[1024];
	sprintf(msg,"%s: couldn't find any rom there... !",&s[1]);

	MessageBox("*.dat file error", msg,"OK");
	for (int n=0; n<nb; n++) {
	    free(ips_info.rom[n]);
	    free(ips_info.ips[n]);
	}
	return;
    }
    ips_info.nb = nb;
}

static u64 bps_decode(FILE *f) {
  u64 data = 0, shift = 1;
  while(1) {
    u8 x;
    fread(&x,1,1,f);
    data += (x & 0x7f) * shift;
    if(x & 0x80) break;
    shift <<= 7;
    data += shift;
  }
  return data;
}

static void source_read(u8 *src, u8 *dst, int *offset,int len, int max_size) {
    if (*offset + len > max_size) {
	printf("source_read: offset=%x + len=%x > max_size %x\n",*offset,len,max_size);
	exit(1);
    }
    memcpy(&dst[*offset],&src[*offset],len);
    *offset += len;
}

static void target_read(FILE *f, u8 *dst, int *offset,int len, int max_size) {
    if (*offset + len > max_size) {
	printf("target_read len %x offset %x would read past max_size %x\n",len,*offset,max_size);
	exit(1);
    }
    int read = fread(&dst[*offset],1,len,f);
    if (read < len) {
	printf("target_read: asked %d, got %d\n",len,read);
	exit(1);
    }
    *offset += len;
}

static void source_copy(FILE *f, u8 *src, u8 *dst, int *rel_src, int *offset,int len, int max_size) {
    u64 data = bps_decode(f);
    *rel_src += (data & 1 ? -1 : +1)* (data >> 1);
    if (*rel_src < 0) {
	printf("source_copy: rel_src<0 : %d\n",*rel_src);
	exit(1);
    }
    if (*rel_src + len > max_size) {
	printf("source_copy: source_offset %x + len %x > max_size %x\n",*rel_src,len,max_size);
	exit(1);
    }
    if (*offset + len > max_size) {
	printf("source_copy: offset %x + len %x > max_size %x\n",*offset,len,max_size);
	exit(1);
    }
    memcpy(&dst[*offset],&src[*rel_src],len);
    *offset += len;
    *rel_src += len;
}

static void target_copy(FILE *f, u8 *dst, int *rel_dst, int *offset,int len, int max_size) {
    u64 data = bps_decode(f);
    *rel_dst += (data & 1 ? -1 : +1)* (data >> 1);
    if (*rel_dst < 0) {
	printf("target_copy: rel_dst<0 : %d\n",*rel_dst);
	exit(1);
    }
    if (*rel_dst + len > max_size) {
	printf("target_copy: rel_dst %x + len %x > max_size %x\n",*rel_dst,len,max_size);
	exit(1);
    }
    if (*offset + len > max_size) {
	printf("target_copy: offset %x + len %x > max_size %x\n",*offset,len,max_size);
	exit(1);
    }
    while (len--) {
	// Here I must have a real loop and not a call to memcpy because it's a copy target to target
	// and of course it can be used to copy a single byte to an any buffer size !
	dst[*offset] = dst[*rel_dst];
	*offset += 1;
	*rel_dst += 1;
    }
}

void load_bps(char *res, unsigned char *ROM, int max_size,int index,char *rom_name) {
    FILE *f = fopen(res,"rb");
    if (!f) {
	printf("couldn't load bps %s\n",res);
	return;
    }
    char buf[6];
    fread(buf,1,4,f);
    if (strncmp(buf,"BPS1",4)) {
	fclose(f);
	MessageBox("Error",_("Bad BPS header"), "Ok");
	return;
    }
    int src_size = bps_decode(f);
    int dst_size = bps_decode(f);
    int meta_size = bps_decode(f);
    if (src_size != max_size) {
	fclose(f);
	MessageBox(_("Error"),"BPS: source size! = ROM size", "OK");
	return;
    }
    if (dst_size != max_size) {
	fclose(f);
	MessageBox(_("Error"),"BPS: target size! = ROM size", "OK");
	return;
    }
    if (meta_size) {
	char meta[meta_size+1];
	meta[meta_size] = 0;
	fread(meta,meta_size,1,f);
	printf("bps metadata: %s\n",meta);
    }
    u8 dst[dst_size];
    int target_offset = 0,rel_src = 0, rel_dst = 0;
    memset(dst,255,dst_size);
    while (target_offset < max_size && !feof(f)) {
	int action = bps_decode(f);
	int len = (action >> 2) + 1;
	action &= 3;
	switch(action) {
	case 0: source_read(ROM,dst,&target_offset,len,max_size); break;
	case 1: target_read(f,dst,&target_offset,len,max_size); break;
	case 2: source_copy(f,ROM,dst,&rel_src,&target_offset,len,max_size); break;
	case 3: target_copy(f,dst,&rel_dst,&target_offset,len,max_size); break;
	}
    }
    int crc[3];
    int read = fread(crc,1,12,f);
    if (read < 12) {
	printf("bps: footer: expected read 12 for crc block, got %d\n",read);
	exit(1);
    }
    printf("crc src ");
    int mycrc = CrcCalc(ROM,max_size);
    if (mycrc != crc[0]) printf("NOT ");
    printf("ok\n");

    printf("crc dst ");
    mycrc = CrcCalc(dst,max_size);
    if (mycrc != crc[1]) printf("NOT ");
    printf("ok\n");
    memcpy(ROM,dst,max_size);

    fclose(f);
}

