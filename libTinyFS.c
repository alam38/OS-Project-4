#include <stdio.h>
#include <stdlib.h>
#include "libDisk.h"
#include "libTinyFS.h"

int tfs_mkfs(char *filename, int nBytes) {

	int f;

	if((f = openDisk(filename, nBytes)) < 0) { //error code should be set within the libDisk.c function
		return f;
	}

	//zeroOut(f, nBytes);
	initializeBlocks(f, nBytes);

	return 1; //1 = succes code
}

/*void zeroOut(int f, int bytes) {

	void *buffer = calloc(1, BLOCKSIZE);


	writeBlock(f, buffer);
}*/

void initializeBlocks(int f, int bytes) {

	//printf("initializeBlocks f: %d\n", f);
	writeSuper(f);	
	//setFreeBlocks(f, bytes);

}

void writeSuper(int f) {

	//printf("writeSuper f: %d\n", f);

	char *buffer = calloc(1, BLOCKSIZE);

	buffer[0] = 1; //block type
	buffer[1] = 0x44; // super number integrity flag
	buffer[2] = 1; // index of root inode
	buffer[3] = 0; // empty separator
	buffer[4] = 1; // index of free block head

	buffer = (void *)buffer;
	writeBlock(f, 0, buffer);
}

void setFreeBlocks(int f, int bytes) {

	//printf("setFreeBlocks f: %d\n", f);
	int x;
	char *buffer = calloc(1, BLOCKSIZE);

        buffer[0] = 4; // block type
        buffer[1] = 0x44; // super number integrity flag
	buffer[3] = 0; // empty

	for (x = 1; x < (bytes/BLOCKSIZE); x++) {

        	buffer[2] =  x + 1; //pointer to the next free block
		writeBlock(f, x, buffer);
	}
}


int tfs_mount(char *diskname) {

	

}

/*int tfs_unmount(void) {

}

fileDescriptor tfs_openFile(char *name) {

}

int tfs_closeFile(fileDescriptor FD) {

}

int tfs_writeFile(fileDescriptor FD,char *buffer, int size) {

}

int tfs_deleteFile(fileDescriptor FD) {

}

int tfs_readByte(fileDescriptor FD, char *buffer) {

}

int tfs_seek(fileDescriptor FD, int offset) {

}*/
