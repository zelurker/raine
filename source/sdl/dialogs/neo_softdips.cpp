#include "../gui/menu.h"
#include "games.h"
#include "raine.h"
#include "starhelp.h"
#include "neocd/neocd.h"

static menu_item_t menu[14]; // 14 soft dips max

/* Some rather lengthy code to be able to decode the soft dips since the games
 * don't give access to all of them, and sometimes none of them. It's
 * documented in the web, and you can get a bios which can display/change them
 * unibios for neogeo, and equivalent there for neocd :
 * http://smkdan.eludevisibility.org/ngcd/ */

static char* get_str(int &strings) {
    // Get a string from the string array
    char str[13];
    if (is_neocd())
	memcpy(str,&RAM[strings],12);
    else
	memcpy(str,&ROM[strings],12);
    strings += 12;
    ByteSwap((UINT8*)str,12);
    int n = 11;
    while (str[n] == ' ')
	n--;
    str[++n] = 0;
    return strdup(str);
}

static void set_bcd(int &bcd1,int &nb, int &strings) {
    menu[nb].label = get_str(strings);
    menu[nb].value_int = &bcd1;
    menu[nb].values_list_size = ITEM_EDIT;
    menu[nb].values_list[0] = 5; // max len
    menu[nb].values_list[1] = 0; // history ?
    menu[nb].values_list[2] = 100; // pixels
    int low = bcd1 & 0xff;
    int high = bcd1 >> 8;
    low = (low >> 4)*10+(low & 0xf); // convert from bcd
    high = (high >> 4)*10+(high & 0xf);
    char buff[6];
    sprintf(buff,"%02d:%02d",high,low);
    menu[nb++].values_list_label[0] = strdup(buff);
}

static int bcd(int x) {
    return (x/10)*16+(x%10);
}

static void get_bcd(int &nb,int code) {
    char *s = menu[nb].values_list_label[0];
    char *sep = strchr(s,':');
    int min = 0,sec;
    if (sep) {
	min = atoi(s);
	sec = atoi(sep+1);
    } else
	sec = atoi(s);
    if (sec > 59) sec = 59;
    if (sec < 0) sec = 0;
    if (min < 0) min = 0;
    if (min > 99) min = 99;
    int val = bcd(min)*100+bcd(sec);
    WriteWord(&RAM[code],val);
    nb++;
}

static int get_byte(int adr) {
    if (is_neocd()) return RAM[adr ^ 1];
    else if (adr < 0x100000) return ROM[adr ^ 1];
    return RAM[(adr ^ 1) & 0xffff];
}

int do_soft_dips(int sel) {
    if (!current_game) return 0;
    if (current_game->load_game != &load_neocd) return 0;
    int base = (is_neocd() ? ReadLongSc(&RAM[0x11e]) : ReadLongSc(&ROM[0x11e]));
    if (base > 0x200000) return 0;
    int code = (is_neocd() ? 0x10fd84 : 0xfd84);
    if (ReadLong(&RAM[code]) == 0) return 0; // uninitialized yet
    int strings = base + 0x20;
    int bcd1,bcd2,val1,val2;
    int nb = 0;

    if ((bcd1 = ReadWord(&RAM[code])) != 0xffff) {
	set_bcd(bcd1,nb,strings);
    }
    if ((bcd2 = ReadWord(&RAM[code+2])) != 0xffff) {
	set_bcd(bcd2,nb,strings);
    }
    if ((val1 = RAM[(code+4) ^ 1]) != 0xff) {
	// special 1
	menu[nb].label = get_str(strings);
	menu[nb].value_int = &val1;
	menu[nb].values_list_size = 3;
	menu[nb].values_list[0] = 1;
	menu[nb].values_list[1] = 99;
	menu[nb++].values_list[2] = 1;
    }
    if ((val2 = RAM[(code+5) ^ 1]) != 0xff) {
	// special 2
	menu[nb].label = get_str(strings);
	menu[nb].value_int = &val2;
	menu[nb].values_list_size = 3;
	menu[nb].values_list[0] = 0;
	menu[nb].values_list[1] = 100;
	menu[nb++].values_list[2] = 1;
    }
    code += 6;
    int defs = base + 0x16;
    int val[10];
    int x = 0;
    int choices;
    while ((choices = get_byte(defs) & 0xf)) {
	menu[nb].label = get_str(strings);
	menu[nb].value_int = &val[x];
	menu[nb].values_list_size = choices;
	for (int n=0; n<choices; n++) {
	    menu[nb].values_list[n] = n;
	    menu[nb].values_list_label[n] = get_str(strings);
	}
	val[x] = RAM[code ^ 1];
	code++;
	defs++;
	x++;
	if (choices <= 1) {
	    free((char*)menu[nb].label);
	    for (int n=0; n<choices; n++)
		if (menu[nb].values_list_label[n])
		    free(menu[nb].values_list_label[n]);
	} else
	    nb++;
	if (defs == base + 0x16 + 10) // max nb of entries
	    break;
    }
    TMenu *load = new TMenu("Neogeo bios",menu);
    load->execute();
    delete load;

    // Convert back to the native formats...
    code = (is_neocd() ? 0x10fd84 : 0xfd84);
    nb = 0;
    if (bcd1 != 0xffff)
	get_bcd(nb,code);
    if (bcd2 != 0xffff)
	get_bcd(nb,code+2);
    RAM[(code+4) ^ 1] = val1;
    RAM[(code+5) ^ 1] = val2;
    code += 6;
    defs = base + 0x16;
    x = 0;
    while ((choices = get_byte(defs) & 0xf)) {
	RAM[code ^ 1] = val[x++];
	code++;
	if (defs++ == base + 0x16 + 10) // max nb of entries
	    break;
    }

    // Free everything !
    nb = 0;
    while (menu[nb].label) {
	free((void*)menu[nb].label);
	menu[nb].label = NULL;
	for (int n=0; n<menu[nb].values_list_size; n++)
	    if (menu[nb].values_list_label[n]) {
		free(menu[nb].values_list_label[n]);
		menu[nb].values_list_label[n] = NULL;
	    }
	nb++;
    }
    return 0;
}
