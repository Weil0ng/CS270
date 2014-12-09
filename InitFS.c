#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "Directories.h"

void testBlockify();

LONG GIGA = 1024 * 1024 * 1024;
UINT MEGA = 1024 * 1024;

int main(int args, char* argv[])
{
    FileSystem fs;
    printf("Initializing file system with initfs...\n");
    UINT succ = l2_initfs(15*GIGA/BLK_SIZE, MEGA, &fs);
    if (succ != 0) {
        printf("Error: initfs failed with error code: %d\n", succ);
    }
    printf("res: %u\n", succ);
    l2_unmount(&fs);
    return 0;
}
