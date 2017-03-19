diskTest : diskTest.c libDisk.c libDisk.h
	gcc -o diskTest diskTest.c libDisk.c

ownFSTest : ownFSTest.c libTinyFS.c libTinyFS.h
	gcc -o ownFSTest ownFSTest.c libDisk.c libTinyFS.c

clean :
	rm disk0.dsk disk1.dsk disk2.dsk disk3.dsk
