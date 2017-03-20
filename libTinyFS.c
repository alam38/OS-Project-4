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
	writeBlock(f, x, buffer);
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
	strcpy(&outBuffer[6], name); // spot[6] is always our name

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


int tfs_closeFile(int FD) {

	int8_t *inBuffer = calloc(1, BLOCKSIZE), *outBuffer = calloc(1, BLOCKSIZE), *endBuffer = calloc(1, BLOCKSIZE);

	int8_t previousFD, end;

	int flag = 0, x, index;

	for (x = 0; x < numFiles; x++) {
		if (fileTable[x].inodeIndex == FD) {
			index = x;
			flag = 1;
		}
	}

        if (flag == 0) {
                return -9; //the given file descriptor does not point to an open file
        }

	if (fileTable[index].open == 0) {
		return -11; //the file is not open
	}

	readBlock(mountedFS, FD, (void *)inBuffer);

	previousFD = findPreviousInodeFD(FD);
	readBlock(mountedFS, previousFD, (void *)outBuffer);
	outBuffer[2] = inBuffer[2];
	writeBlock(mountedFS, previousFD, (void *)outBuffer);

        inBuffer[0] = FREEBLOCK;
	inBuffer[2] = inBuffer[6]; //this makes the inode point to its data extents which will turn into free blocks
	cleanFileExtents(inBuffer);

	end = freeBlocksTailFD();
	readBlock(mountedFS, end, (void *)endBuffer);
	endBuffer[2] = FD;

	fileTable[index].inodeIndex = 0;
	memset(fileTable[index].name, 0, 9);
	fileTable[index].open = 0;
	fileTable[index].size = 0;

	numFiles--;

	return 1;
}

int freeBlocksTailFD() {

        int8_t *outBuffer = calloc(1, BLOCKSIZE);
	int8_t address, previous;

	readBlock(mountedFS, 0, (void *)outBuffer);

	address = outBuffer[4];
	previous = address;

	while (address != -1) {
		previous = address;
		readBlock(mountedFS, address, (void *)outBuffer);
		address = outBuffer[2];
	}

	return previous;
}

void cleanFileExtents(int8_t *buffer) {

	int8_t *outBuffer = calloc(1, BLOCKSIZE);

	int8_t size, nextAddress;

	int x, y;

        size = buffer[4];
        nextAddress = buffer[6];

        for (x = 0; x < (size/BLOCKSIZE); x++) {
                readBlock(mountedFS, nextAddress, outBuffer);
                outBuffer[0] = FREEBLOCK;
		
		for (y = 0; y < (BLOCKSIZE - 4); y++) {
			outBuffer[y] = 0;
		}

		nextAddress = outBuffer[6];
        }


}

int8_t findPreviousInodeFD(int FD) {

	int8_t *inBuffer = calloc(1, BLOCKSIZE);
	int8_t index = 0;

	int flag = 0;

        readBlock(mountedFS, 0, (void *)inBuffer);


        while (inBuffer[2] != -1 && inBuffer[0] != FREEBLOCK) {
                if (inBuffer[2] == FD) {
                        flag = 1;
                        break;
                }
		index = inBuffer[2];
                readBlock(mountedFS, inBuffer[2], (void *)inBuffer);
        }

        if (flag == 1) {
		return index;
        }
	else {
		return -7; // some error
	}


}




/* Writes buffer ‘buffer’ of size ‘size’, which represents an entire
file’s content, to the file system. Previous content (if any) will be
completely lost. Sets the file pointer to 0 (the start of file) when
done. Returns success/error codes. */
int tfs_writeFile(int FD,char *buffer, int size) {
	int blocksNeeded, ndx, lastBlockSize, currentDataBlock, fdExists = 0,
	 totalFreeBlocks, totalBlocksNeeded, currentFileSizeInBlocks;
	int8_t * inBuffer, * outBuffer, inodeIndex, firstDataIndex;

	for(ndx = 0; ndx < numFiles; ndx++) {
		if (fileTable[ndx].inodeIndex == FD) {
			fdExists = 1;
			break;
		}
	}
	if (fdExists == 0) {
		printf("FD does not exist\n");
		return -1;
	}

	inBuffer = calloc(1,BLOCKSIZE);
	outBuffer = calloc(1,BLOCKSIZE);

	blocksNeeded = size/BLOCKSIZE;
	lastBlockSize = size % BLOCKSIZE;

	totalFreeBlocks = numFreeBlocks();

	if (lastBlockSize > 0) {
		totalBlocksNeeded = blocksNeeded + 1;
	} else {
		totalBlocksNeeded = blocksNeeded;
	}

	if (fileTable[ndx].size % 252 > 0) {
		currentFileSizeInBlocks = (fileTable[ndx].size /BLOCKSIZE) + 1;
	} else {
		currentFileSizeInBlocks = fileTable[ndx].size / BLOCKSIZE;
	}

	if (totalBlocksNeeded > currentFileSizeInBlocks) {
		if (totalBlocksNeeded > currentFileSizeInBlocks + totalFreeBlocks) {
			printf("Not enough space to write the file...\n");
			return -1;
		}
	}

	/*totalBlocks = fsTable[mountedFSIndex].size/BLOCKSIZE;
	totalFreeBlocks = totalBlocks - ;*/

	inodeIndex = findPreviousInodeFD(FD);
	readBlock(mountedFS, inodeIndex, (void *)inBuffer);

	inodeIndex = (int8_t)inBuffer[2];

	readBlock(mountedFS, inodeIndex, (void *)inBuffer);

	firstDataIndex = inBuffer[6];

	if (inBuffer[4] != 0) {// If there is data already in the inode
		outBuffer[0] = 2;
		outBuffer[1] = MAGIC;
		outBuffer[2] = findNextInode();
		outBuffer[4] = size;
		strcpy((char *)&(outBuffer[6]), fileTable[FD].name);
		outBuffer[5] = firstDataIndex;

	} else { //If there is not data already in inode
		outBuffer[0] = 2;
		outBuffer[1] = MAGIC;
		outBuffer[2] = findNextInode();
		outBuffer[4] = size;
		strcpy((char *)&(outBuffer[6]), fileTable[FD].name);

		readBlock(mountedFS, 0, inBuffer);
		outBuffer[5] = inBuffer[4];
	}

	writeBlock(mountedFS, inodeIndex, (void *) outBuffer);

	ndx = 0;
	readBlock(mountedFS, outBuffer[5], (void *) inBuffer);

	currentDataBlock = outBuffer[5];

	while(ndx < blocksNeeded) {
		memcpy(&(inBuffer[4]), buffer + (ndx * 252), 252);
		writeBlock(mountedFS, currentDataBlock, (void *) inBuffer);
		currentDataBlock = inBuffer[2];
		readBlock(mountedFS, inBuffer[2], ((void*)inBuffer));
		ndx++;
	}

	if (lastBlockSize > 0) {
		memcpy(&(inBuffer[4]), buffer + (ndx * 252), lastBlockSize);
		writeBlock(mountedFS, currentDataBlock, (void *) inBuffer);
	}
	return 1;
}

int numFreeBlocks() {
	int totalBlocks, usedBlocks, ndx, blocksForFile;

	totalBlocks = fsTable[mountedFSIndex].size / BLOCKSIZE;
	usedBlocks = 1; //Starting with superblock

	for (ndx = 0; ndx < numFiles; ndx++) {
		blocksForFile = fileTable[ndx].size/252;
		if ((fileTable[ndx].size % 252) != 0) {
			blocksForFile++;
		}
		blocksForFile++; //For inode 
		usedBlocks += blocksForFile;
	}

	return totalBlocks - usedBlocks;
}

/*int tfs_deleteFile(int FD) {

}

int tfs_readByte(int FD, char *buffer) {

}

int tfs_seek(int FD, int offset) {

}*/


