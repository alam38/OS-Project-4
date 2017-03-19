CC = gcc
CFLAGS = -g -Wall -Werror

all : diskTest ownFSTest
	@echo "COMPLETED BUILDING ALL TESTS..."
	@echo "-------------------------------"	

diskTest : diskTest.c libDisk.c libDisk.h
	clear
	@echo "BUILDING diskTest..."
	$(CC) $(CFLAGS) -o diskTest diskTest.c libDisk.c

ownFSTest : ownFSTest.c libTinyFS.c libTinyFS.h
	@echo "BUILDING ownFSTest..."
	$(CC) $(CFLAGS) -o ownFSTest ownFSTest.c libDisk.c libTinyFS.c

clean :
	rm disk0.dsk disk1.dsk disk2.dsk disk3.dsk

rofs:
	clear
	@echo "RUNNING ownFSTest..."
	./ownFSTest

rdt:  
	clear
	@echo "RUNNING diskTest..."
	./diskTest	
	
