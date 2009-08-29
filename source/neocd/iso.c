#include "raine.h"
#include "sdl/dialogs/messagebox.h"

/* Extremely simplified iso image handling, can handle only files in the
 * root directory (I don't need anything else !) */

int iso_sector_size;

static void myfseek(FILE *f, int pos, int where) {
  fseek(f,pos,where);
  if (iso_sector_size == 2352) {
    fseek(f,0x10,SEEK_CUR);
  }
}

static int find_file(char *iso, char *filename, int *size, int *start) {
  FILE *f = fopen(iso,"rb");
  if (!f) {
    char msg[256];
    sprintf(msg,"Couldn't open iso file:|%s",iso);
    MessageBox("Error",msg,"Ok");
    return 0;
  }
  myfseek(f,iso_sector_size*0x10 + 0x9e,SEEK_SET);
  int root_dir = 0;
  fread(&root_dir,1,4,f);
  myfseek(f,root_dir * iso_sector_size, SEEK_SET);
  unsigned char buff[2048];
  fread(buff,2048,1,f);
  unsigned char *ptr = buff;
  int len_record;
  int len_dir = 0;
  int found = 0;
  do {
    len_record = *ptr;
    if (len_record == 0 && len_dir > 0) {
      if (iso_sector_size > 2048)
	fseek(f,iso_sector_size - 2048, SEEK_CUR);
      fread(buff,2048,1,f);
      len_dir -= iso_sector_size;
      if (len_dir) {
	ptr = buff;
	len_record = *ptr;
      }
    }
    if (len_record) {
      // int len_ext = ptr[1];
      int location = *((int *)&ptr[2]);
      int len_file = *((int *)&ptr[10]);
      int len_name = ptr[32];
      char name[40];
      memcpy(name,&ptr[33],len_name);
      name[len_name] = 0;
      char *s = strchr(name,';'); // extension iso : ; + number
      if (s)
	*s = 0;
      // printf("%s len %d location %x\n",name,len_file,location);
      if (!name[0] && len_file && !len_dir) {
	len_dir = len_file;
	// printf("init len_dir %d\n",len_dir);
      } else if (!stricmp(name,filename)) {
	*size = len_file;
	*start = location;
	found = 1;
	break;
      }
    }
    ptr += len_record;
  } while (len_record);
  fclose(f);
  if (!found) {
    print_debug("could not find %s in iso\n",filename);
  }
  return found;
}

int iso_size(char *iso, char *name) {
  int len, location;
  if (find_file(iso,name,&len,&location))
    return len;
  return 0;
}

int load_from_iso(char *iso, char *name, unsigned char *dest, int offset, int size) {
  static int chunk;
  if (iso_sector_size > 2048 && offset) {
    // add the number of crc areas crossed
    offset += (offset/2048)*(iso_sector_size-2048);
  }
  int len, location;
  if (find_file(iso, name, &len, &location)) {
    FILE *f = fopen(iso,"rb");
    myfseek(f,location * iso_sector_size + offset, SEEK_SET);
    if (iso_sector_size > 2048) {
      while (size > 0) {
	if (offset) {
	  /* We must stop at 2048 boundaries, after it's just crc code... */
	  offset = 0;
	  chunk = (size > 2048-chunk ? 2048-chunk : size);
	} else
	  chunk = (size > 2048 ? 2048 : size);
	fread(dest,1,chunk,f);
	size -= chunk;
	fseek(f,iso_sector_size - 2048 , SEEK_CUR);
	dest += chunk;
      }
    } else
      fread(dest,1,size,f);
    fclose(f);
    return 1;
  }
  return 0;
}

