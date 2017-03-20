#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "libDisk.h"
#include "libTinyFS.h"

int mountedFSIndex = -1;
int maxFSIndex = 0;
fileSystem fsTable[MAXFILESYSTEMS];
fileData *fileTable;

int tfs_mkfs(char *filename, int nBytes) {

	int f;

	if((f = openDisk(filename, nBytes)) < 0) { //error code should be set within the libDisk.c function
		return f;
	}

	;

	//zeroOut(f, nBytes);
	initializeBlocks(f, nBytes);

	strcpy((fsTable[maxFSIndex]).name, filename);
	(fsTable[maxFSIndex]).size = nBytes;

	maxFSIndex++;

	return 1; //1 = succes code
}

/*void zeroOut(int f, int bytes) {

	void *buffer = calloc(1, BLOCKSIZE);


	writeBlock(f, buffer);
}*/

void initializeBlocks(int f, int bytes) {

	writeSuper(f);	
	setFreeBlocks(f, bytes);

}

void writeSuper(int f) {

	char *buffer = (char *)calloc(BLOCKSIZE, 1);

	buffer[0] = SUPERBLOCK; //block type
	buffer[1] = MAGIC; // super number integrity flag
	buffer[2] = 1; // index of root inode
	buffer[3] = 0; // empty separator
	buffer[4] = 2; // index of free block head

	buffer = (void *)buffer;
	writeBlock(f, 0, buffer);
}

void setFreeBlocks(int f, int bytes) {

	int x;
	char *buffer = calloc(1, BLOCKSIZE);

        buffer[0] = FREEBLOCK; // block type
        buffer[1] = MAGIC; // super number integrity flag
	buffer[3] = 0; // empty

	for (x = 1; x < ((bytes/BLOCKSIZE) - 1); x++) {

        	buffer[2] =  x + 1; //pointer to the next free block
		writeBlock(f, x, buffer);
	}

	buffer[2] = -1; 
	writeBlock(f, x + 1, buffer);
}


int tfs_mount(char *diskname) {

	void *buffer = calloc(1, BLOCKSIZE);

	int x;

	if ((mountedFS = openDisk(diskname, 0)) < 0) { // attempts to open the disk
		return mountedFS; // this should return the proper error code
	}

	for (x = 0; x < maxFSIndex; x++) {
		if (strcmp(fsTable[x].name, diskname) == 0) {
			mountedFSIndex = x;
			break;
		}
	}

	fileTable = calloc(sizeof(fileData), (fsTable[mountedFSIndex]).size/BLOCKSIZE - 1);

	numFiles = 0;

	readBlock(mountedFS, 0, buffer);
	if ((((char *)buffer)[1]) != MAGIC) {
		return -4; //error code for improper magic number
	}

	return 1;
}

int tfs_unmount(void) {

	close(mountedFS);

	if (mountedFSIndex == -1) {
		return -9; // no mounted fs
	}

	mountedFSIndex = -1;

	return 1;
}

int tfs_openFile(char *name) {
	int x;

	char *inBuffer = calloc(1, BLOCKSIZE), *outBuffer = calloc(1, BLOCKSIZE);
	for (x = 0; x < numFiles; x++) {
		if (strcmp(fileTable[x].name, name) == 0) {
			if (fileTable[x].open == 0) { //0 = false
				fileTable[x].open = 1;
			}
			return fileTable[x].inodeIndex;
		}
	}

	strcpy(fileTable[x].name, name);
	fileTable[x].open = 1;
	fileTable[x].inodeIndex = findNextInode();
	fileTable[x].size = 0;

	readBlock(mountedFS, fileTable[x].inodeIndex, (void *)inBuffer);

	outBuffer[0] = INODE;
	outBuffer[1] = MAGIC;
	outBuffer[2] = findNextInode(); //this finds the next free inode
	// outBuffer[4] = 0 this is our size
	strcpy(&outBuffer[5], name); // spot[5] is always our name

	writeBlock(mountedFS, fileTable[x].inodeIndex, (void *)outBuffer);
	numFiles++;

	moveFreeHead(); //moves the head of the free blocks, need to do this everytime a block is converted from free

	return fileTable[x].inodeIndex;
}

void moveFreeHead() {

	int8_t previousIndex;

	int8_t *inBuffer = calloc(1, BLOCKSIZE), *outBuffer = calloc(1, BLOCKSIZE);

	readBlock(mountedFS, 0, inBuffer);

	outBuffer[0] = inBuffer[0];
	outBuffer[1] = inBuffer[1];
	outBuffer[2] = inBuffer[2];
	outBuffer[3] = inBuffer[3];
	outBuffer[4] = inBuffer[4];

        while (inBuffer[4] != -1 && inBuffer[0] != FREEBLOCK) {
                previousIndex = inBuffer[4];
                readBlock(mountedFS, inBuffer[4], (void *)inBuffer); 
        }

	outBuffer[4] = previousIndex;

	writeBlock(mountedFS, 0, (void *)outBuffer);
}

int8_t findNextInode() {

	int8_t previousIndex;
	int8_t *buffer = (int8_t *)calloc(1, BLOCKSIZE);

	if (mountedFS == -1) {
		return -5; //there is no mounted fs
	}

	readBlock(mountedFS, 0, (void *)buffer); //reading in the super block

	while (buffer[2] != -1 && buffer[0] != FREEBLOCK) {
		previousIndex = buffer[2];
		readBlock(mountedFS,buffer[2], buffer); 
	}

	if (buffer[2] == -1) {
		return -6; // error no more space in fs
	}

	return previousIndex;
}

/*int tfs_closeFile(int FD) {

	

}

int tfs_writeFile(int FD,char *buffer, int size) {

}

int tfs_deleteFile(int FD) {

}

int tfs_readByte(int FD, char *buffer) {

}

int tfs_seek(int FD, int offset) {

}*/
