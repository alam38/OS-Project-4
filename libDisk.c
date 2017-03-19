#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "libDisk.h"
#include <errno.h>

int openDisk(char *filename, int nBytes) {

	int /*diskSize, */ f;
	
	if(nBytes == 0) {
	        if((f = open(filename, O_RDWR|O_APPEND)) == -1) { //add more error testing with own errno for everything
                	return -2;
        	}
	}
	else if(nBytes < BLOCKSIZE) {
		return -1;
	}
	else {
		//diskSize = nBytes - (nBytes % BLOCKSIZE);

	        if((f = open(filename, O_RDWR|O_CREAT, 0666)) == -1) {
                	return -2;
        	}
	}
	return f;

}

int readBlock(int disk, int bNum, void *block){

	if((lseek(disk, (bNum * BLOCKSIZE), SEEK_SET)) == -1) {
		printf("The given file is not open (read)\n");
		return -3;
	}


	read(disk, block, (size_t)BLOCKSIZE);
	return 0;
}

int writeBlock(int disk, int bNum, void *block) {

        if((lseek(disk, (bNum * BLOCKSIZE), SEEK_SET)) == -1) {
                printf("The given file is not open (write)\n");
                return -3;
        }
	
        write(disk, block, (size_t)BLOCKSIZE);
        return 0;

}

