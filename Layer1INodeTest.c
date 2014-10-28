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
    if (args < 3) {
        printf("Not enough arguments!\n");
        exit(1);
    }

    UINT nDBlks = atoi(argv[1]);
    UINT nINodes = atoi(argv[2]);
    
    //test makefs
    printf("Testing makefs...\n");
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
    printf("\n");

    assert(fs.diskINodeBlkOffset == 1);
    assert(fs.diskDBlkOffset == 1 + nINodes / INODES_PER_BLK);

    //test allocINode until no free inodes are left
    /*
    printf("Testing allocINode...\n");
    for(UINT i = 0; i < nINodes; i++) {
        INode testINode;
        UINT succ = allocINode(&fs, &testINode);
        if(succ) printf("allocINode call %d returned ID %d\n", i, testINode._in_id);
        assert(testINode._in_id >= 0);
    }
    
    assert(fs.superblock.nFreeINodes == 0);
    
    //allocINode should fail gracefully when no free inodes are left
    INode testINode;
    UINT id = allocINode(&fs, &testINode);
    printf("Invalid allocINode call returned ID %d\n", 
    assert(id == -1);
    
    //test freeINode, note that this free order is different than alloc order
    printf("Testing freeINode...\n");
    for(UINT i = 0; i < nINodes;i++) {
        freeINode(&fs, i);
    }
    
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
    printf("Testing allocINode part 2...\n");
    for(UINT i = 0; i < NUM_TEST_INODES; i++) {
        inodeIds[i] = allocINode(&fs, &inodes[i]);
        assert(inodeIds[i] >= 0);
        
        //ensure allocated IDs are unique
        for(UINT j = 0; j < i; j++) {
            assert(inodeIds[i] != inodeIds[j]);
        }
    }
    
    //test writeINode
    printf("Testing writeINode...\n");
    for(int i = 0; i < NUM_TEST_INODES; i++) {
        INode testINode;
        testINode._in_owner = TEST_OWNER; 
        
        UINT succ = writeINode(&fs, inodeIds[i], &testINode);
        assert(succ == 0);
    }
    
    //test readINode
    printf("Testing readINode...\n");
    for(int i = 0; i < NUM_TEST_INODES; i++) {
        INode testINode;
        
        UINT succ = readINode(&fs, inodeIds[i], &testINode);
        assert(succ == 0);
        assert(testINode._in_owner = TEST_OWNER);
    }
    */
    
    printf("Testing destroyfs...\n");
    destroyfs(&fs);

    return 0;
}            
