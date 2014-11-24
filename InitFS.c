#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "Directories.h"

void testBlockify();

int main(int args, char* argv[])
{
    FileSystem fs;

    printf("Initializing file system with initfs...\n");
    UINT succ = l2_initfs(128, 26, &fs);
    if(succ == 0) {
        printf("initfs succeeded with filesystem size: %d\n", fs.nBytes);
    }
    else {
        printf("Error: initfs failed with error code: %d\n", succ);
    }
    l2_unmount(&fs);
    return 0;
}
