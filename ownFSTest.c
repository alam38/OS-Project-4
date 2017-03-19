#include <stdio.h>
#include <stdlib.h>
#include "libTinyFS.h"

int main(int argc, char *argv[]) {

	char *name = "poop";

	printf("tfs_mkfs returned: %d\n", tfs_mkfs(name, BLOCKSIZE));
}
