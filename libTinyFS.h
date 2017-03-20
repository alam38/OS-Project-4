#ifndef LIBTINYFS_H
#define LIBTINYFS_H

#define BLOCKSIZE 256
#define FILESIZE 10240
#define MAGIC 0x44
#define MAXFILESYSTEMS 256

#define MAXNAMESIZE 32
#define MAXFILENAME 9

#define SUPERBLOCK 0x01
#define INODE 0x02
#define FILEEXTENT 0x03
#define FREEBLOCK 0x04

typedef struct {
	char name[MAXFILENAME];	
	int8_t inodeIndex; //index in the inodes linked list||this is also our implemented file descriptor
	int open; //flag to determine if the file is open or not
	int size;
	int seekVal;
} fileData;

typedef struct {
	int size;
	char name[MAXNAMESIZE];
} fileSystem;

int tfs_mkfs(char *filename, int nBytes);
int tfs_mount(char *diskname);
int tfs_unmount(void);
int tfs_openFile(char *name);

int tfs_closeFile(int FD);
int tfs_writeFile(int FD, char *buffer, int size);
int tfs_deleteFile(int FD);
int tfs_readByte(int FD, char *buffer);

int tfs_seek(int FD, int offset);
void initializeBlocks(int f, int bytes);
void writeSuper(int f);
void setFreeBlocks(int f, int bytes);

int8_t findNextInode();
void moveFreeHead();
int8_t findPreviousInodeFD(int FD);
void cleanFileExtents(int8_t *buffer);

int freeBlocksTailFD();
int numFreeBlocks();

int mountedFS;
int mountedFSIndex;
int maxFSIndex;
int numFiles;

#endif

