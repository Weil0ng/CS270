/**
 * Tests Layer 1, mostly FileSystem
 * by Jon
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FileSystem.h"

#define NUM_TEST_INODES (1)
#define TEST_OWNER (999)

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
    printf("\nTesting makefs...\n");
    FileSystem fs;
    UINT succ = makefs(nDBlks, nINodes, &fs);
    if(succ == 0) {
        printf("makefs succeeded with filesystem size: %d\n", fs.nBytes);
    }
    //printDisk(fs.disk);
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
    printf("\nTesting allocINode...\n");
    for(UINT i = 0; i < nINodes; i++) {
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

    //test freeINode, note that this free order is different than alloc order
    printf("\nTesting freeINode...\n");
    for(UINT i = 0; i < nINodes;i++) {
        printf("Freeing inode id: %d\n", i);
        UINT succ = freeINode(&fs, i);
        assert(succ == 0);
        printINodes(&fs);
    }
    /*
    //temporary variables for read/write test
    if(nINodes < NUM_TEST_INODES) {
        printf("Must have at least %d inodes to do read/write test!\n", NUM_TEST_INODES);
        exit(1);
    }
    INode inodes[NUM_TEST_INODES];
    UINT inodeIds[NUM_TEST_INODES];
    for(UINT i = 0; i < NUM_TEST_INODES; i++) {
        inodeIds[i] = -1;
    }
    
    //test allocINode
    printf("\nTesting allocINode part 2...\n");
    for(UINT i = 0; i < NUM_TEST_INODES; i++) {
        inodeIds[i] = allocINode(&fs, &inodes[i]);
        assert(inodeIds[i] >= 0);
        
        //ensure allocated IDs are unique
        for(UINT j = 0; j < i; j++) {
            assert(inodeIds[i] != inodeIds[j]);
        }
    }
    
    //test writeINode
    printf("\nTesting writeINode...\n");
    for(int i = 0; i < NUM_TEST_INODES; i++) {
        INode testINode;
        testINode._in_owner = TEST_OWNER; 
        
        UINT succ = writeINode(&fs, inodeIds[i], &testINode);
        assert(succ == 0);
    }
    
    //test readINode
    printf("\nTesting readINode...\n");
    for(int i = 0; i < NUM_TEST_INODES; i++) {
        INode testINode;
        
        UINT succ = readINode(&fs, inodeIds[i], &testINode);
        assert(succ == 0);
        assert(testINode._in_owner = TEST_OWNER);
    }
    */
    
    printf("\nTesting destroyfs...\n");
    succ = destroyfs(&fs);
    assert(succ == 0);

#endif
    return 0;
}            
