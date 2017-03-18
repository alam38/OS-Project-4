#define SUPERBLOCK_MNUMBER 0x44

int tfs_mkfs(char *filename, int nBytes){
   int fileSystem;
   char *buffer;
   fileSystem = openDisk(filename, nBytes);
   buffer = 0x44;
   buffer = buffer | 1;

   
}
int tfs_mount(char *diskname)
int tfs_umount(void)
fileDescriptor tfs_openFile(char *name)
   
int tfs_closeFile(fileDescriptor FD)
int tfs_writeFile(fileDescriptor FD, char *buffer, int size)
int tfs_deleteFile(fileDescriptor FD)
int tfs_readByte(fileDescriptor FD, char *buffer)
int tfs_seek(fileDescriptor FD, int offset)