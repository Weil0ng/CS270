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
    printf("Initializing file system with initfs...\n");
    UINT succ = l2_initfs(nDBlks, nINodes, &fs);
    if(succ == 0) {
        printf("initfs succeeded with filesystem size: %d\n", fs.nBytes);
    }
    else {
        printf("Error: initfs failed with error code: %d\n", succ);
    }
    
    BOOL quit = false;
    char command[1024];
    char path[1024];
    UINT flags;
    char buf[1024];
    char namelist[MAX_FILE_NUM_IN_DIR][FILE_NAME_LENGTH];
    UINT offset;
    UINT len;
    while(!quit) {
        printMenu();
        printf("\nEnter a command: ");
        scanf("%s", command);

        memset(buf, 0, sizeof(buf));        

        if(strcmp(command, "mkdir") == 0) {
            //printf("Enter directory path: ");
            scanf("%s", path);
            
            l2_mkdir(&fs, path, 0, 0);
        }
        else if(strcmp(command, "mknod") == 0) {
            //printf("Enter file path: ");
            scanf("%s", path);
            
            l2_mknod(&fs, path, 0, 0);
        }
        else if (strcmp(command, "truncate") == 0) {
            printf("Enter file path: ");
            scanf("%s", path);
            printf("Enter file offset: ");
            scanf("%d", &offset);
            
            l2_truncate(&fs, path, offset);
        }
        else if(strcmp(command, "readdir") == 0) {
            //printf("Enter file path: ");
            scanf("%s", path);
            
            //l2_readdir(&fs, path, namelist);
        }
        else if(strcmp(command, "unlink") == 0) {
            //printf("Enter file path: ");
            scanf("%s", path);
            
            l2_unlink(&fs, path);
        }
        else if(strcmp(command, "open") == 0) {
            printf("Enter file path: ");
            scanf("%s", path);
            printf("Enter open flags: ");
            scanf("%d", flags);
            
            l2_open(&fs, path, flags);
        }
        else if(strcmp(command, "close") == 0) {
            printf("Enter file path: ");
            scanf("%s", path);
            printf("Enter close flags: ");
            scanf("%d", flags);
            
            l2_close(&fs, path, flags);
        }
        else if(strcmp(command, "read") == 0) {
            printf("Enter file path: ");
            scanf("%s", path);
            printf("Enter file offset: ");
            scanf("%d", &offset);
            printf("Enter read length: ");
            scanf("%d", &len);
            
            l2_read(&fs, path, offset, buf, len);
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
            l2_write(&fs, path, offset, buf, len);
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
    printf("IMPORTANT: please provide an absolute path!\n");
    printf(" mkdir /path/to/directory\n");
    printf(" mknod /path/to/file\n");
    printf(" unlink /path/to/file_or_dir\n");
    printf(" open /path/to/file_or_dir\n");
    printf(" close /path/to/file_or_dir\n");
    printf(" read /path/to/file_or_dir\n");
    printf(" write /path/to/file_or_dir\n");
    printf("--------------------\n");
    printf(" stats\n");
    printf(" quit\n");
    printf("====================\n");
}
