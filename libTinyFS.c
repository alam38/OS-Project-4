#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "libTinyFS.h"

int openDisk(char *filename, int nBytes) {

	int diskSize, f;
	
	if(nBytes == 0) {
	        if(!(f = open(filename, O_RDWR|O_APPEND))) {
                	printf("The given file does not exist\n");
                	exit(-2);
        	}
	
	}
	else if(nBytes < BLOCKSIZE) {
		return -1;
	}
	else {
		diskSize = nBytes - (nBytes % BLOCKSIZE);

	        if(!(f = open(filename, O_RDWR))) {
        	        printf("The given file does not exist\n");
                	exit(-2);
        	}
	}

	return f;

}

int readBlock(int disk, int bNum, void *block){

	void *buffer;

	if(lseek(disk, (bNum * BLOCKSIZE), SEEK_SET)) {
		printf("The given file is not open\n");
		return -3;
	}


	printf("read check size: %zu\n", read(disk, block, (size_t)BLOCKSIZE));
	printf("block check: %s\n", block);
	return 0;
}

int writeBlock(int disk, int bNum, void *block) {

        if(lseek(disk, (bNum * BLOCKSIZE), SEEK_SET)) {
                printf("The given file is not open\n");
                return -3;
        }

	printf("block: %s\n", block);
	
        printf("write check size: %zu\n", write(disk, block, (size_t)BLOCKSIZE));
        return 0;

}

