/**
 * Tests Layer 1, mostly FileSystem
 * by Jon
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "FileSystem.h"

#define NUM_TEST_INODES (8)
#define NUM_TEST_DBLKS (8)
//#define TEST_OWNER (999)
#define TEST_PERMISSIONS (555)
#define TEST_MODTIME (42)
#define TEST_ACCESSTIME (24)
#define TEST_FILESIZE (9000)

#define TEST_DBLK_BYTE_LEN (12)
#define TEST_DBLK_BYTE_OFF (48)

const char TEST_OWNER[INODE_OWNER_NAME_LEN]= "JARIC";

void testDBlks(UINT, UINT);
void testINodes(UINT, UINT);

int main(int args, char* argv[])
{
    #ifdef DEBUG

    srand(time(NULL));
  
    if (args < 4) {
        printf("Not enough arguments!\n");
        exit(1);
    }

    char* testType = argv[1];

    UINT nDBlks = atoi(argv[2]);
    UINT nINodes = atoi(argv[3]);

    if(strcmp(testType, "d") == 0)
        testDBlks(nDBlks, nINodes);
    else if(strcmp(testType, "i") == 0)
        testINodes(nDBlks, nINodes);

    #endif

    return 0;
}

#ifdef DEBUG
void testDBlks(UINT nDBlks, UINT nINodes) {
    printf("\n==== dblk test ====\n");

    //test makefs
    printf("\n---- makefs ----\n");
    FileSystem fs;
    UINT succ = makefs(nDBlks, nINodes, &fs);
    if(succ == 0) {
        printf("makefs succeeded with filesystem size: %d\n", fs.nBytes);
    }

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

    assert(fs.diskINodeBlkOffset == 1);
    assert(fs.diskDBlkOffset == 1 + nINodes / INODES_PER_BLK);

    //test allocDBlk until no free dblks are left
    printf("\n---- allocDBlk ----\n");
    for(int i = 0; i < nDBlks; i++) {
        UINT id = allocDBlk(&fs);
        printf("allocDBlk call %d returned ID %d\n", i, id);
    }

    assert(fs.superblock.nFreeDBlks == 0);

    //allocDBlk should fail gracefully when no free dblks are left
    UINT id = allocDBlk(&fs);
    printf("Invalid allocDBlk call returned ID %d\n", id);
    assert(id == -1);

    printf("\nSuperblock:\n");
    printSuperBlock(&fs.superblock);
    printf("\nDBlks:\n");
    printDBlks(&fs);
    printf("\nFree dblk cache:\n");
    printFreeDBlkCache(&fs.superblock);

    //test random freeDBlk
    printf("\n---- random freeDBlk ----\n");
    
    //produce a random ordering to free
    int shuffled[nDBlks];
    for(int i = 0; i < nDBlks; i++) {
        shuffled[i] = i;
    }
    shuffle(shuffled, nDBlks);
    
    for(int i = 0; i < nDBlks; i++) {
        printf("Freeing dblk id: %d\n", shuffled[i]);
        succ = freeDBlk(&fs, shuffled[i]);
        printFreeDBlkCache(&fs.superblock);
        assert(succ == 0);
    }

    assert(fs.superblock.nFreeDBlks == nDBlks);

    printf("\nSuperblock:\n");
    printSuperBlock(&fs.superblock);
    printf("\nDBlks:\n");
    printDBlks(&fs);
    printf("\nFree dblk cache:\n");
    printFreeDBlkCache(&fs.superblock);

    //test allocDBlk until no free dblks are left
    printf("\n---- allocDBlk ----\n");
    for(int i = 0; i < nDBlks; i++) {
        UINT id = allocDBlk(&fs);
        printf("allocDBlk call %d returned ID %d\n", i, id);
    }

    assert(fs.superblock.nFreeDBlks == 0);

    //test ordered freeDBlk
    printf("\n---- ordered freeDBlk ----\n");
    for(int i = 0; i < nDBlks; i++) {
        printf("Freeing dblk id: %d\n", i);
        succ = freeDBlk(&fs, i);
        printFreeDBlkCache(&fs.superblock);
        assert(succ == 0);
    }

    assert(fs.superblock.nFreeDBlks == nDBlks);

    printf("\nSuperblock:\n");
    printSuperBlock(&fs.superblock);
    printf("\nDBlks:\n");
    printDBlks(&fs);
    printf("\nFree dblk cache:\n");
    printFreeDBlkCache(&fs.superblock);

    //temporary variables for read/write test
    if(nDBlks < NUM_TEST_DBLKS) {
        printf("\nError: must have at least %d dblks to do read/write test!\n", NUM_TEST_DBLKS);
        exit(1);
    }
    BYTE dblks[NUM_TEST_DBLKS][BLK_SIZE];
    UINT dblkIds[NUM_TEST_DBLKS];
    for(int i = 0; i < NUM_TEST_DBLKS; i++) {
        dblkIds[i] = -1;
    }
    
    //test allocDBlk
    printf("\n---- allocDBlk (2) ----\n");
    for(int i = 0; i < NUM_TEST_DBLKS; i++) {
        dblkIds[i] = allocDBlk(&fs);
        printf("allocDBlk call %d returned ID %d\n", i, dblkIds[i]);

        //ensure dblk IDs are valid and unique
        assert(dblkIds[i] >= 0 && dblkIds[i] < nDBlks);
        for(UINT j = 0; j < i; j++) {
            assert(dblkIds[i] != dblkIds[j]);
        }
    }
    
    //test group writeDBlk
    printf("\n---- writeDBlk ----\n");
    for(int i = 0; i < NUM_TEST_DBLKS; i++) {
        UINT* buf = (UINT*) &dblks[i];
        for(int j = 0; j < BLK_SIZE / sizeof(UINT); j++) {
            buf[j] = -j;
        }

        printf("Writing test data to dblk id: %d\n", dblkIds[i]);
        succ = writeDBlk(&fs, dblkIds[i], dblks[i]);
        assert(succ == 0);
    }

    printf("\nDBlks:\n");
    printDBlks(&fs);

    //test group readDBlk
    printf("\n---- readDBlk ----\n");
        
    for(int i = 0; i < NUM_TEST_DBLKS; i++) {
        BYTE testDBlk[BLK_SIZE];
        
        printf("Reading test data from dblk id: %d\n", dblkIds[i]);
        succ = readDBlk(&fs, dblkIds[i], testDBlk);
        printDBlkInts(testDBlk);
        assert(succ == 0);
        
        UINT* buf = (UINT*) &testDBlk;
        for(int j = 0; j < BLK_SIZE / sizeof(UINT); j++) {
            assert(buf[j] == -j);
        }
    }
    
    //test writeDBlkOffset
    printf("\n---- writeDBlkOffset ----\n");
    for(int i = 0; i < NUM_TEST_DBLKS; i++) {
        UINT* buf = (UINT*) &dblks[i];
        for(int j = 0; j < TEST_DBLK_BYTE_LEN / sizeof(UINT); j++) {
            buf[j] = - (int) (j+ TEST_DBLK_BYTE_OFF / sizeof(UINT) + 3);
        }

        printf("Writing test data to dblk id: %d, with offset: %d, and length: %d\n", dblkIds[i], TEST_DBLK_BYTE_OFF, TEST_DBLK_BYTE_LEN);
        succ = writeDBlkOffset(&fs, dblkIds[i], (BYTE*) buf, TEST_DBLK_BYTE_OFF, TEST_DBLK_BYTE_LEN);
        assert(succ == 0);
    }

    //test readDblkOffset
    printf("\n---- readDBlkOffset ----\n");

    for(int i = 0; i < NUM_TEST_DBLKS; i++) {
        BYTE testDBlkOff[TEST_DBLK_BYTE_LEN];
        printf("Reading test data from dblk id: %d, with offset: %d, and length: %d\n", dblkIds[i], TEST_DBLK_BYTE_OFF, TEST_DBLK_BYTE_LEN);
        succ = readDBlkOffset(&fs, dblkIds[i], testDBlkOff, TEST_DBLK_BYTE_OFF, TEST_DBLK_BYTE_LEN);
        assert(succ == 0);
        
        UINT* buf = (UINT*) &testDBlkOff;
        for(int j = 0; j < TEST_DBLK_BYTE_LEN / sizeof(UINT); j++) {
            //printf("%d, %d\n", buf[j], - (int) (j + TEST_READ_DBLK_OFF / sizeof(UINT) + 3));
            assert(buf[j] == - (int) (j + TEST_DBLK_BYTE_OFF / sizeof(UINT) + 3));
        }

    }
    
    //test modifying read/writeDBlk
    printf("\n---- modify read/writeDBlk ----\n");
    printDBlks(&fs);
    for(int i = NUM_TEST_DBLKS - 1; i >= 0; i--) {
        succ = readDBlk(&fs, dblkIds[i], dblks[i]);
        assert(succ == 0);

        printf("Modifying dblk id: %d\n", dblkIds[i]);
        UINT* buf = (UINT*) &dblks[i];
        for(int j = 0; j < BLK_SIZE / sizeof(UINT); j++) {
            buf[j] = dblkIds[i];
        }
        succ = writeDBlk(&fs, dblkIds[i], dblks[i]);
        assert(succ == 0);

        BYTE testDBlk2[BLK_SIZE];
        succ = readDBlk(&fs, dblkIds[i], testDBlk2);
        printDBlkInts(testDBlk2);
        assert(succ == 0);
        
        UINT* buf2 = (UINT*) &testDBlk2;
        for(int j = 0; j < BLK_SIZE / sizeof(UINT); j++) {
            assert(buf2[j] == dblkIds[i]);
        }
    }
    
    printf("\n---- destroyfs ----\n");
    succ = destroyfs(&fs);
    assert(succ == 0);

    printf("\n==== dblk test complete ====\n");
}

void testINodes(UINT nDBlks, UINT nINodes) {
    printf("\n==== inode test ====\n");

    //test makefs
    printf("\n---- makefs ----\n");
    FileSystem fs;
    UINT succ = makefs(nDBlks, nINodes, &fs);
    if(succ == 0) {
        printf("makefs succeeded with filesystem size: %d\n", fs.nBytes);
    }

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

    assert(fs.diskINodeBlkOffset == 1);
    assert(fs.diskDBlkOffset == 1 + nINodes / INODES_PER_BLK);

    //test allocINode until no free inodes are left
    printf("\n---- allocINode ----\n");
    for(int i = 0; i < nINodes; i++) {
        INode testINode;
        UINT id = allocINode(&fs, &testINode);
        printf("allocINode call %d returned ID %d\n", i, id);
        printINode(&testINode);
    }

    assert(fs.superblock.nFreeINodes == 0);

    //allocINode should fail gracefully when no free inodes are left
    INode testINode;
    UINT id = allocINode(&fs, &testINode);
    printf("Invalid allocINode call returned ID %d\n", id);
    assert(id == -1);

    printf("\nSuperblock:\n");
    printSuperBlock(&fs.superblock);
    printf("\nINodes:\n");
    printINodes(&fs);
    printf("\nFree inode cache:\n");
    printFreeINodeCache(&fs.superblock);

    //test random freeINode
    printf("\n---- random freeINode ----\n");
    
    int shuffled[nINodes];
    for(int i = 0; i < nINodes; i++) {
        shuffled[i] = i;
    }
    shuffle(shuffled, nINodes);
    
    for(int i = 0; i < nINodes; i++) {
        printf("Freeing inode id: %d\n", shuffled[i]);
        succ = freeINode(&fs, shuffled[i]);
        assert(succ == 0);
    }

    printf("%d, %d\n", fs.superblock.nFreeINodes, nINodes);
    assert(fs.superblock.nFreeINodes == nINodes);

    printf("\nSuperblock:\n");
    printSuperBlock(&fs.superblock);
    printf("\nINodes:\n");
    printINodes(&fs);
    printf("\nFree inode cache:\n");
    printFreeINodeCache(&fs.superblock);

    //test allocINode until no free inodes are left
    printf("\n---- allocINode ----\n");
    for(int i = 0; i < nINodes; i++) {
        INode testINode;
        UINT id = allocINode(&fs, &testINode);
        printf("allocINode call %d returned ID %d\n", i, id);
    }

    assert(fs.superblock.nFreeINodes == 0);

    //test ordered freeINode
    printf("\n---- ordered freeINode ----\n");
    for(int i = 0; i < nINodes; i++) {
        printf("Freeing inode id: %d\n", i);
        succ = freeINode(&fs, i);
        assert(succ == 0);
    }

    assert(fs.superblock.nFreeINodes == nINodes);

    printf("\nSuperblock:\n");
    printSuperBlock(&fs.superblock);
    printf("\nINodes:\n");
    printINodes(&fs);
    printf("\nFree inode cache:\n");
    printFreeINodeCache(&fs.superblock);

    //temporary variables for read/write test
    if(nINodes < NUM_TEST_INODES) {
        printf("\nError: must have at least %d inodes to do read/write test!\n", NUM_TEST_INODES);
        exit(1);
    }
    INode inodes[NUM_TEST_INODES];
    UINT inodeIds[NUM_TEST_INODES];
    for(int i = 0; i < NUM_TEST_INODES; i++) {
        inodeIds[i] = -1;
    }
    
    //test allocINode
    printf("\n---- allocINode (2) ----\n");
    for(int i = 0; i < NUM_TEST_INODES; i++) {
        inodeIds[i] = allocINode(&fs, &inodes[i]);
        printf("allocINode call %d returned ID %d\n", i, inodeIds[i]);
        printINode(&inodes[i]);

        //ensure allocated IDs are valid and unique
        assert(inodeIds[i] >= 0 && inodeIds[i] < nINodes);
        for(UINT j = 0; j < i; j++) {
            assert(inodeIds[i] != inodeIds[j]);
        }
    }
    
    //test group writeINode
    printf("\n---- writeINode ----\n");
    for(int i = 0; i < NUM_TEST_INODES; i++) {
        //inodes[i]._in_owner = TEST_OWNER;
        strcpy(inodes[i]._in_owner, TEST_OWNER);
        inodes[i]._in_permissions = TEST_PERMISSIONS;
        inodes[i]._in_modtime = TEST_MODTIME;
        inodes[i]._in_accesstime = TEST_ACCESSTIME;
        inodes[i]._in_filesize = TEST_FILESIZE;
        
        printf("Writing test data to inode id: %d\n", inodeIds[i]);
        succ = writeINode(&fs, inodeIds[i], &inodes[i]);
        assert(succ == 0);
    }
    printINodes(&fs);

    //test group readINode
    printf("\n---- readINode ----\n");
        
    for(int i = 0; i < NUM_TEST_INODES; i++) {
        INode testINode;
        
        printf("Reading test data from inode id: %d\n", inodeIds[i]);
        succ = readINode(&fs, inodeIds[i], &testINode);
        printINode(&testINode);
        assert(succ == 0);
        //assert(testINode._in_owner = TEST_OWNER);
        assert(!strcmp(testINode._in_owner, TEST_OWNER));
        assert(testINode._in_permissions = TEST_PERMISSIONS);
        assert(testINode._in_modtime = TEST_MODTIME);
        assert(testINode._in_accesstime = TEST_ACCESSTIME);
        assert(testINode._in_filesize = TEST_FILESIZE);
    }

    //test modifying read/writeINode
    printf("\n---- modify read/writeINode ----\n");
    printINodes(&fs);
    for(int i = NUM_TEST_INODES - 1; i >= 0; i--) {
        succ = readINode(&fs, inodeIds[i], &inodes[i]);
        assert(succ == 0);

        printf("Modifying inode id: %d\n", inodeIds[i]);
        //inodes[i]._in_owner = "WILL";
        strcpy(inodes[i]._in_owner, "WILL");
        inodes[i]._in_permissions = TEST_PERMISSIONS - 1;
        inodes[i]._in_modtime = TEST_MODTIME - 1;
        inodes[i]._in_accesstime = TEST_ACCESSTIME - 1;
        inodes[i]._in_filesize = TEST_FILESIZE - 1;
        succ = writeINode(&fs, inodeIds[i], &inodes[i]);
        assert(succ == 0);

        INode testINode2;
        succ = readINode(&fs, inodeIds[i], &testINode2);
        printINode(&testINode2);
        assert(succ == 0);
        //assert(testINode2._in_owner = inodes[i]._in_owner);
        assert(!strcmp(testINode2._in_owner, inodes[i]._in_owner));
        assert(testINode2._in_permissions = inodes[i]._in_permissions);
        assert(testINode2._in_modtime = inodes[i]._in_modtime);
        assert(testINode2._in_accesstime = inodes[i]._in_accesstime);
        assert(testINode2._in_filesize = inodes[i]._in_filesize);
    }
    
    printf("\n---- destroyfs ----\n");
    succ = destroyfs(&fs);
    assert(succ == 0);

    printf("\n==== inode test complete ====\n");
}

#endif
