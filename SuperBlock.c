/*
 * Implementation for superblock
 * by Jon
 */

#include <stdio.h>

#include "SuperBlock.h"

UINT blockify(SuperBlock* superblock, BYTE* buf) {

    //to do
    return 0;
}

#ifdef DEBUG
void printSuperBlock(SuperBlock* sb) {
    printf("[Superblock: nDBLks = %d, nFreeDBlks = %d, pFreeDBlksHead = %d, pNextFreeDBlk = %d, nINodes = %d, nFreeINodes = %d, pNextFreeINode = %d, modified = %d]\n", 
        sb->nDBlks, sb->nFreeDBlks, sb->pFreeDBlksHead, sb->pNextFreeDBlk, sb->nINodes, sb->nFreeINodes, sb->pNextFreeINode, sb->modified);
}
#endif

#ifdef DEBUG
// prints out the free inode cache for debugging
void printFreeINodeCache(SuperBlock* sb) {
    for(int i = 0; i < FREE_INODE_CACHE_SIZE; i++) {
        printf("%d ", sb->freeINodeCache[i]);
    }
    printf("\n");
}
#endif

#ifdef DEBUG
// prints out the free data block cache for debugging
void printFreeDBlkCache(SuperBlock* sb) {
    for(int i = 0; i < FREE_DBLK_CACHE_SIZE; i++) {
        printf("%d ", sb->freeDBlkCache[i]);
    }
    printf("\n");
}
#endif