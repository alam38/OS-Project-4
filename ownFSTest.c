#include <stdio.h>
#include <stdlib.h>
#include "libTinyFS.h"

int main(int argc, char *argv[]) {

	char *name = "poop";

	printf("tfs_mkfs returned: %d\n", tfs_mkfs(name, BLOCKSIZE));
	printf("tfs_mount returned: %d\n", tfs_mount(name));
	printf("tfs_openFile returned: %d\n", tfs_openFile("test"));
}
