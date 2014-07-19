#include "68000/starhelp.h"
#include "files.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "raine.h"

void ByteSwap(UINT8 *MEM, UINT32 size)
{
   UINT32 ta;
   for(ta=0;ta<size;ta+=2){
      WriteWord(&MEM[ta],ReadWord68k(&MEM[ta]));
   }
}

static int file_size(char *filename) {
  struct stat buff;
  if (stat(filename,&buff)<0)
    buff.st_size = 0;
  return buff.st_size;
}

int load_file(char *filename, UINT8 *dest, UINT32 size)
{
   FILE *file_ptr;

   file_ptr = fopen(filename,"rb");

   if(file_ptr){
      fread(dest, 1, size, file_ptr);
      fclose(file_ptr);
      return 1;		// Success
   }
   else{
      return 0;		// Failure
   }
}

int save_file(char *filename, UINT8 *source, UINT32 size)
{
   FILE *file_ptr;

   file_ptr = fopen(filename,"wb");

   if(file_ptr){
      fwrite(source, 1, size, file_ptr);
      fclose(file_ptr);
      return 1;		// Success
   }
   else{
      return 0;		// Failure
   }
}

int main(int argc, char **argv) {
    if (argc == 1) {
	printf("syntax : byteswap file\n");
	return 0;
    }
    int size = file_size(argv[1]);
    if (size) {
	UINT8 *buf = malloc(size);
	if (!buf) {
	    printf("alloc error for %d bytes\n",size);
	    return -1;
	}
	load_file(argv[1],buf,size);
	ByteSwap(buf,size);
	save_file(argv[1],buf,size);
	free(buf);
    }
    return 0;
}

