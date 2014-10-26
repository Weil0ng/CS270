/**
 * Tests Layer 1, mostly FileSystem
 * by Jon
 */

#include <assert.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "FileSystem.h"

#define NUM_TEST_INODES (2)
#define TEST_OWNER (999)

int main(int args, char* argv[])
{
    if (args < 3) {
        printf("Not enough arguments!\n");
        exit(1);
    }

    UINT nDBlks = atoi(argv[1]);
    UINT nINodes = atoi(argv[2]);
    
    if(nDBlks <= 0 || nINodes <= 0) {
        printf("Must have positive number of blocks/inodes!\n");
        exit(1);
    }
    else if(nINodes < NUM_TEST_INODES) {
        printf("Must have at least %d inodes!\n", NUM_TEST_INODES);
        exit(1);
    }
    
    //test makefs
    FileSystem fs;
    makefs(nDBlks, nINodes, &fs);
    assert(fs.nBytes > 0);
    assert(fs.diskINodeBlkOffset == 1);
    assert(fs.diskDBlkOffset == 1 + nINodes / INODES_PER_BLK);
    
    //test allocINode until no free inodes are left
    for(UINT i = 0; i < nINodes; i++) {
        INode testINode;
        UINT id = allocINode(&fs, &testINode);
        assert(id >= 0);
    }
    
    assert(fs.superblock->nFreeINodes == 0);
    
    //allocINode should fail gracefully when no free inodes are left
    INode testINode;
    UINT id = allocINode(&fs, &testINode);
    assert(id == -1);
    
    //test freeINode, note that this free order is different than alloc order
    for(UINT i = 0; i < nINodes;i++) {
        freeINode(&fs, i);
    }
    
    //temporary variables for read/write test
    INode inodes[NUM_TEST_INODES];
    UINT inodeIds[NUM_TEST_INODES];
    for(UINT i = 0; i < NUM_TEST_INODES; i++) {
        inodeIds[i] = -1;
    }
    
    //test allocINode
    for(UINT i = 0; i < NUM_TEST_INODES; i++) {
        inodeIds[i] = allocINode(&fs, &inodes[i]);
        assert(inodeIds[i] >= 0);
        
        //ensure allocated IDs are unique
        for(UINT j = 0; j < i; j++) {
            assert(inodeIds[i] != inodeIds[j]);
        }
    }
    
    //test writeINode
    for(int i = 0; i < NUM_TEST_INODES; i++) {
        INode testINode;
        testINode._in_owner = TEST_OWNER; 
        
        UINT succ = writeINode(&fs, inodeIds[i], &testINode);
        assert(succ == 0);
    }
    
    //test readINode
    for(int i = 0; i < NUM_TEST_INODES; i++) {
        INode testINode;
        
        UINT succ = readINode(&fs, inodeIds[i], &testINode);
        assert(succ == 0);
        assert(testINode._in_type == FREE);
    }
    
    destroyfs(&fs);

    return 0;
}            
