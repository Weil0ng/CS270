/**
 * FileSystem implementation
 * by Jon
 */
 
#include "FileSystem.h"
#include "SuperBlock.h"

UINT makefs(UINT nDBlks, UINT nINodes, FileSystem* fs) {

    //allocate memory for filesystem struct
    fs = malloc(sizeof(FileSystem));
    fs->freeDBlks = malloc(NUM_FREE_DBLKS * sizeof(UINT));
    fs->superblock = malloc(sizeof(SuperBlock));
    
    //initialize in-memory superblock
    fs->superblock->nDBlks = nDBlks;
    fs->superblock->nFreeDBlks = nDBlks;
    fs->superblock->pNextFreeDBlk = nDBlks - 1;    
    
    fs->superblock->nINodes = nINodes;
    fs->superblock->freeINodes = nINodes;
    fs->superblock->pNextFreeINode = nINodes - 1;
    
    //create free block list on disk
    
}

UINT allocINode() {

}

UINT freeINode(UINT) {

}

UINT readINode(UINT, INode*) {

}

UINT writeINode(UINT, INode*) {

}

UINT allocDBlk() {

}

UINT freeDBlk(UINT) {

}

UINT readDBlk(UINT, BYTE*) {

}

UINT writeDBlk(UINT, BYTE*) {

}
