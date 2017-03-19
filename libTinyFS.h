#ifndef LIBTINYFS_H
#define LIBTINYFS_H

#define BLOCKSIZE 256
#define FILESIZE 10240

int tfs_mkfs(char *filename, int nBytes);
int tfs_mount(char *diskname);
int tfs_unmount(void);
int tfs_openFile(char *name);

int tfs_closeFile(int FD);
int tfs_writeFile(int FD,char *buffer, int size);
int tfs_deleteFile(int FD);
int tfs_readByte(int FD, char *buffer);

int tfs_seek(int FD, int offset);
void initializeBlocks(int f, int bytes);
void writeSuper(int f);
void setFreeBlocks(int f, int bytes);

#endif

