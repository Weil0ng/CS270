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

void printSuperBlock(SuperBlock* sb) {
    printf("[Superblock: nDBLks = %d, nFreeDBlks = %d, pFreeDBlksHead = %d, pNextFreeDBlk = %d, nINodes = %d, nFreeINodes = %d, pNextFreeINode = %d, modified = %d]\n", 
        sb->nDBlks, sb->nFreeDBlks, sb->pFreeDBlksHead, sb->pNextFreeDBlk, sb->nINodes, sb->nFreeINodes, sb->pNextFreeINode, sb->modified);
}