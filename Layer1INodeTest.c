/**
 * Tests Layer 1, mostly FileSystem
 * by Jon
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FileSystem.h"

#define NUM_TEST_INODES (2)
#define TEST_OWNER (999)
#define TEST_PERMISSIONS (555)
#define TEST_MODTIME (42)
#define TEST_ACCESSTIME (24)
#define TEST_FILESIZE (9000)

int main(int args, char* argv[])
{
#ifdef DEBUG
    if (args < 3) {
        printf("Not enough arguments!\n");
        exit(1);
    }

    UINT nDBlks = atoi(argv[1]);
    UINT nINodes = atoi(argv[2]);
    
    //test makefs
    printf("\n==== makefs ====\n");
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
    printf("\nFree data block cache:\n");
    printFreeDBlkCache(&fs.superblock);

    assert(fs.diskINodeBlkOffset == 1);
    assert(fs.diskDBlkOffset == 1 + nINodes / INODES_PER_BLK);

    //test allocINode until no free inodes are left
    printf("\n==== allocINode ====\n");
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

    //test freeINode, note that this free order is different than alloc order
    printf("\n==== freeINode ====\n");
    for(int i = 0; i < nINodes;i++) {
        printf("Freeing inode id: %d\n", i);
        succ = freeINode(&fs, i);
        assert(succ == 0);
    }

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
    printf("\n==== allocINode (2) ====\n");
    for(int i = 0; i < NUM_TEST_INODES; i++) {
        inodeIds[i] = allocINode(&fs, &inodes[i]);
        printf("allocINode call %d returned ID %d\n", i, inodeIds[i]);
        printINode(&inodes[i]);

        //ensure allocated IDs are valid and unique
        assert(inodeIds[i] == inodes[i]._in_id);
        assert(inodeIds[i] >= 0 && inodeIds[i] < nINodes);
        for(UINT j = 0; j < i; j++) {
            assert(inodeIds[i] != inodeIds[j]);
        }
    }
    
    //test group writeINode
    printf("\n==== writeINode ====\n");
    for(int i = 0; i < NUM_TEST_INODES; i++) {
        inodes[i]._in_owner = TEST_OWNER;
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
    printf("\n==== readINode ====\n");
    for(int i = 0; i < NUM_TEST_INODES; i++) {
        INode testINode;
        
        printf("Reading test data from inode id: %d\n", inodeIds[i]);
        succ = readINode(&fs, inodeIds[i], &testINode);
        printINode(&testINode);
        assert(succ == 0);
        assert(testINode._in_owner = TEST_OWNER);
        assert(testINode._in_permissions = TEST_PERMISSIONS);
        assert(testINode._in_modtime = TEST_MODTIME);
        assert(testINode._in_accesstime = TEST_ACCESSTIME);
        assert(testINode._in_filesize = TEST_FILESIZE);
    }

    //test modifying read/writeINode
    printf("\n==== modify read/writeINode ====\n");
    printINodes(&fs);
    for(int i = NUM_TEST_INODES - 1; i >= 0; i--) {
        succ = readINode(&fs, inodeIds[i], &inodes[i]);
        assert(succ == 0);

        printf("Modifying inode id: %d\n", inodeIds[i]);
        inodes[i]._in_owner = TEST_OWNER - 1;
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
        assert(testINode2._in_owner = inodes[i]._in_owner);
        assert(testINode2._in_permissions = inodes[i]._in_permissions);
        assert(testINode2._in_modtime = inodes[i]._in_modtime);
        assert(testINode2._in_accesstime = inodes[i]._in_accesstime);
        assert(testINode2._in_filesize = inodes[i]._in_filesize);
    }
    
    printf("\n==== destroyfs ====\n");
    succ = destroyfs(&fs);
    assert(succ == 0);

#endif
    return 0;
}            
