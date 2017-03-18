#include <fcntl.h>
#include "tinyFS.h"


int openDisk(char *filename, int nBytes){
   int file;

   if(nBytes < BLOCKSIZE){
      return -1;
   }
   else if(nBytes == 0){
      file = open(filename, O_WRONLY);
   }
   else{
      nBytes = nBytes - (nBytes % BLOCKSIZE);
      file = open(filename, O_WRONLY);
   }
   return file;
}

int readBlock(int disk, int bNum, void *block){
   //block = malloc(BLOCKSIZE);
   if(lseek(disk, bNum * BLOCKSIZE, SEEK_SET) == -1){
      return -1;
   }
   if(read(disk, block, BLOCKSIZE) == -1){
      return -1;
   }
   return 0;
}

int writeBlock(int disk, int bNum, void *block){
   if(lseek(disk, bNum * BLOCKSIZE, SEEK_SET) == -1){
      return -1;
   }
   if(write(disk, block, BLOCKSIZE) == -1){
      return -1;
   }
   return 0;
}