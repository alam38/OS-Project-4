#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libTinyFS.h"

int main(int argc, char *argv[]) {

	char *name = "poop";

	int val;



	printf("tfs_mkfs returned: %d\n", tfs_mkfs(name, FILESIZE));
	printf("tfs_mount returned: %d\n", tfs_mount(name));
	printf("tfs_openFile returned: %d\n", val = tfs_openFile("test"));
	printf("tfs_writeFile returned: %d\n", val = tfs_writeFile(val,name, strlen(name)));
	printf("tfs_closeFile returned: %d\n", tfs_closeFile(val));
}
