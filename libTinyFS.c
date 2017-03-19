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

	//printf("initializeBlocks f: %d\n", f);
	writeSuper(f);	
	setFreeBlocks(f, bytes);

}

void writeSuper(int f) {

	//printf("writeSuper f: %d\n", f);

	char *buffer = (char *)calloc(BLOCKSIZE, 1);

	buffer[0] = 1; //block type
	buffer[1] = MAGIC; // super number integrity flag
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

	void *buffer;

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

	//fileTable = calloc(sizeof(fileData), ((fsTable[mountedFSIndex]).size/BLOCKSIZE) - 1);
	printf("mountedFSINDEXSIZE %d\n", (fsTable[mountedFSIndex]).size);
	x = (fsTable[mountedFSIndex]).size - 1;
	fileData temp[x];
	fileTable = temp;
	printf("x: %d\n", x);

	readBlock(mountedFS, 0, buffer);
	printf("magic num: %c\n", ((char *)buffer)[1]);
	if ((((char *)buffer)[1]) != MAGIC) {
		return -4; //error code for improper magic number
	}

	return 1;
}

int tfs_unmount(void) {

	close(mountedFS);
	mountedFSIndex = -1;

}

int tfs_openFile(char *name) {
	int x;

	char *inBuffer = calloc(1, BLOCKSIZE), *outBuffer = calloc(1, BLOCKSIZE);
	printf("before for loop%d\n", (fsTable[mountedFSIndex]).size);
	for (x = 0; x < ((fsTable[mountedFSIndex]).size); x++) {
		if (strcmp(fileTable[x].name, name) == 0) {
			if (fileTable[x].open == 0) { //0 = false
				fileTable[x].open = 1;
			}
			return fileTable[x].inodeIndex;
		}
	}

	printf("exit loop\n");
	strcpy(fileTable[x].name, name);
	fileTable[x].open = 1;
	printf("before first findNextOpen\n");
	fileTable[x].inodeIndex = findNextOpen();
	printf("after first findNextOpen\n");
	fileTable[x].size = 0;

	readBlock(mountedFS, fileTable[x].inodeIndex, (void *)inBuffer);

	outBuffer[0] = 2;
	outBuffer[1] = MAGIC;
	outBuffer[2] = inBuffer[2]; // outBuffer[4] = 0 this is our size
	strcpy(&outBuffer[5], name); 

	writeBlock(mountedFS, fileTable[x].inodeIndex, (void *)outBuffer);

	return fileTable[x].inodeIndex;
}

int8_t findNextOpen() {

	int8_t previousIndex;
	int8_t *buffer;

	if (mountedFS == -1) {
		return -5; //there is no mounted fs
	}

	readBlock(mountedFS, 0, (void *)buffer); //reading in the super block
	printf("super block %c\n", buffer[0]);
	previousIndex = buffer[2];

	while (buffer[2] != -1 || buffer[0] != 4) {
		previousIndex = buffer[2];
		readBlock(mountedFS,buffer[2], buffer); // the issue seems to be tied to initializing of the super block and free blocks
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
