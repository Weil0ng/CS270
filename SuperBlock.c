/*
 * Implementation for superblock
 * by Jon
 */

#include <stdio.h>
#include <string.h>

#include "SuperBlock.h"

INT blockify(SuperBlock* superblock, BYTE* buf) {

    DSuperBlock* dsb = (DSuperBlock*) buf;

    dsb->nDBlks = superblock->nDBlks;
    dsb->nFreeDBlks = superblock->nFreeDBlks;
    dsb->pFreeDBlksHead = superblock->pFreeDBlksHead;
    dsb->pNextFreeDBlk = superblock->pNextFreeDBlk;

    dsb->nINodes = superblock->nINodes;
    dsb->nFreeINodes = superblock->nFreeINodes;
    memcpy(dsb->freeINodeCache, superblock->freeINodeCache, FREE_INODE_CACHE_SIZE * sizeof(UINT));
    dsb->pNextFreeINode = superblock->pNextFreeINode;

    return 0;
}

INT unblockify(BYTE* buf, SuperBlock* superblock) {

    DSuperBlock* dsb = (DSuperBlock*) buf;

    superblock->nDBlks = dsb->nDBlks;
    superblock->nFreeDBlks = dsb->nFreeDBlks;
    superblock->pFreeDBlksHead = dsb->pFreeDBlksHead;
    superblock->pNextFreeDBlk = dsb->pNextFreeDBlk;

    superblock->nINodes = dsb->nINodes;
    superblock->nFreeINodes = dsb->nFreeINodes;
    memcpy(superblock->freeINodeCache, dsb->freeINodeCache, FREE_INODE_CACHE_SIZE * sizeof(UINT));
    superblock->pNextFreeINode = dsb->pNextFreeINode;

    superblock->modified = false;

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