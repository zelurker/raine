#include <string.h>
#include <stdio.h>
#include "ips.h"
#include "deftypes.h"
#include "sdl/dialogs/messagebox.h"
#include "translate.h"

tips_info ips_info;

void load_ips(char *res, unsigned char *ROM, int max_size) {
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
	if (ofs + len >= max_size) {
	    printf("ips: len too big, ofs %x len %x max_size %x\n",ofs,len,max_size);
	    len = max_size-ofs-1;
	}
	memcpy(&ROM[ofs],buf,len); // no swap...
    } while (!feof(f));
    fclose(f);
}

