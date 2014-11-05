/**
 * Tests Layer 2 directories
 * by Jon
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Directories.h"

void printMenu();

int main(int args, char* argv[])
{
    srand(time(NULL));
    
    if (args < 3) {
        printf("Usage: TestMain FS_NUM_DATA_BLOCKS FS_NUM_INODES\n");
        exit(1);
    }

    UINT nDBlks = atoi(argv[1]);
    UINT nINodes = atoi(argv[2]);
    
    FileSystem fs;
    printf("Initializing file system with makefs...\n");
    UINT succ = initfs(nDBlks, nINodes, &fs);
    if(succ == 0) {
        printf("makefs succeeded with filesystem size: %d\n", fs.nBytes);
    }
    else {
        printf("Error: initfs failed with error code: %d\n", succ);
    }
    
    BOOL quit = false;
    char command[1024];
    char path[1024];
    char buf[1024];
    UINT offset;
    UINT len;
    while(!quit) {
        printMenu();
        printf("\nEnter a command: ");
        scanf("%s", command);
        
        if(strcmp(command, "mkdir") == 0) {
            printf("Enter directory path: ");
            scanf("%s", path);
            
            mkdir(&fs, path);
        }
        else if(strcmp(command, "mknod") == 0) {
            printf("Enter file path: ");
            scanf("%s", path);
            
            mknod(&fs, path);
        }
        else if(strcmp(command, "unlink") == 0) {
            printf("Enter file path: ");
            scanf("%s", path);
            
            unlink(&fs, path);
        }
        else if(strcmp(command, "open") == 0) {
            printf("Enter file path: ");
            scanf("%s", path);
            
            open(&fs, path);
        }
        else if(strcmp(command, "close") == 0) {
            printf("Enter file path: ");
            scanf("%s", path);
            
            close(&fs, path);
        }
        else if(strcmp(command, "read") == 0) {
            printf("Enter file path: ");
            scanf("%s", path);
            printf("Enter file offset: ");
            scanf("%d", &offset);
            printf("Enter read length: ");
            scanf("%d", &len);
            
            read(&fs, path, offset, buf, len);
            printf("Read data: %s", buf);
        }
        else if(strcmp(command, "write") == 0) {
            printf("Enter file path: ");
            scanf("%s", path);
            printf("Enter file offset: ");
            scanf("%d", &offset);
            printf("Enter data to write (text): ");
            scanf("%s", buf);
            
            len = strlen(buf);
            write(&fs, path, offset, buf, len);
        }
        else if(strcmp(command, "stats") == 0) {
            #ifdef DEBUG
            printf("\nSuperblock:\n");
            printSuperBlock(&fs.superblock);
            printf("\nINodes:\n");
            printINodes(&fs);
            printf("\nData blocks:\n");
            printDBlks(&fs);
            printf("\nFree inode cache:\n");
            printFreeINodeCache(&fs.superblock);
            printf("\nFree dblk cache:\n");
            printFreeDBlkCache(&fs.superblock);
            #else
            printf("Stats only available in DEBUG mode!\n");
            #endif
        }
        else if(strcmp(command, "quit") == 0) {
            quit = true;
        }
        else {
            printf("Invalid command entered!\n");
        }
    }

    return 0;
}

void printMenu() {
    printf("\n");
    printf("====================\n");
    printf("File system commands\n");
    printf("--------------------\n");
    printf(" mkdir\n");
    printf(" mknod\n");
    printf(" unlink\n");
    printf(" open\n");
    printf(" close\n");
    printf(" read\n");
    printf(" write\n");
    printf("--------------------\n");
    printf(" stats\n");
    printf(" quit\n");
    printf("====================\n");
}
