#define SUPERBLOCK_MNUMBER 0x44
#define SUPERBLOCK_MNUMBER 0x44
#define SUPERBLOCK 1
#define INODE 2
#define FILE_EXTENT 3
#define FREE 4
#define BYTE_OFF 8

int tfs_mkfs(char *filename, int nBytes){
   int fileSystem, blocks;
   char *buffer;
   fileSystem = openDisk(filename, nBytes);
   
   buffer = SUPERBLOCK_MNUMBER;
   buffer <<= BYTE_OFF;
   buffer = buffer | 1;
   writeBlock(fileSystem, SUPERBLOCK, buffer);
   
   
}
/*int tfs_mount(char *diskname){
   int status, mNum, tfs;
   char* buffer;
   
   tfs = openDisk(diskname, 
   
}*/

//int tfs_umount(void);
fileDescriptor tfs_openFile(char *name);
   
int tfs_closeFile(fileDescriptor FD);

//block writing happens in full bytes
//buffer contains an entire file's contents
int tfs_writeFile(fileDescriptor FD, char *buffer, int size){ 
   int i = 0, loops = size / BLOCKSIZE;
   char *copyBuffer = malloc(BLOCKSIZE);

   while(i < loops){
      memcpy(copyBuffer, buffer, BLOCKSIZE); 
      if(writeBlock(FD, i, copyBuffer) == -1){
         return -2;
      }
      buffer = buffer >> BLOCKSIZE*4
   }
   free(copyBuffer);
   lseek(FD, 0, SEEK_SET);
   return 0;
}

int tfs_deleteFile(fileDescriptor FD){
   
}

int tfs_readByte(fileDescriptor FD, char *buffer){
   if(read(FD, buffer, 1) == 0){
      return -1;
   }
   // file cursor is automatically adjusted when read does work
   return 0;  
}

int tfs_seek(fileDescriptor FD, int offset){
   if(lseek(FD, offset, SEEK_SET) == -1){
      return -1;
   }
   return 0;
}