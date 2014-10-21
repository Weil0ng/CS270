/**
 * FileSystem implementation
 * by Jon
 */

#include "FileSystem.h"
#include <stdlib.h>

UINT makefs(UINT nDBlks, UINT nINodes, FileSystem* fs) {
	
    //validate number of blocks and inodes

    //allocate memory for filesystem struct
    fs = malloc(sizeof(FileSystem));
    fs->superblock = malloc(sizeof(SuperBlock));
    //fs->superblock->freeDBlks = malloc(NUM_FREE_DBLKS * sizeof(UINT));
    
    //initialize in-memory superblock
    fs->superblock->nDBlks = nDBlks;
    fs->superblock->nFreeDBlks = nDBlks;
    fs->superblock->pFreeDBlksHead = 0;
    fs->superblock->pNextFreeDBlk = FREE_DBLK_CACHE_SIZE - 1;
    
    fs->superblock->nINodes = nINodes;
    fs->superblock->nFreeINodes = nINodes;
    fs->superblock->pNextFreeINode = FREE_INODE_CACHE_SIZE - 1;

    fs->superblock->modified = true;

    //compute offset for inode/data blocks
    //superblock
    fs->diskINodeBlkOffset = 1;
    //superblock + inode blocks
    fs->diskDBlkOffset = fs->diskINodeBlkOffset + nINodes * INODE_SIZE / BLK_SIZE;

    //initialize the in-memory disk and write superblock
    initDisk(fs->disk, DISK_ARRAY_SIZE);
    BYTE superblockBuf[BLK_SIZE];
    blockify(fs->superblock, superblockBuf);
    writeBlk(fs->disk, SUPERBLOCK_OFFSET, superblockBuf);

    //create free block list on disk
    UINT nextListBlk = 0;
    UINT freeDBlkList[FREE_DBLK_CACHE_SIZE];

    while(nextListBlk < fs->superblock->nDBlks) {
        //special case: next head pointer goes in first entry
        freeDBlkList[0] = nextListBlk + FREE_DBLK_CACHE_SIZE;

        //rest of free blocks are enumerated in order
        for(UINT i = 1; i < FREE_DBLK_CACHE_SIZE; i++) {
            freeDBlkList[i] = nextListBlk + i;
        }

        //write completed block and advance to next head
        writeDBlk(fs, nextListBlk, (BYTE*) freeDBlkList);
        nextListBlk += FREE_DBLK_CACHE_SIZE;
    }
}

UINT destroyfs(FileSystem* fs) {

    destroyDisk(fs->disk);
    free(fs->superblock);
    free(fs);
}

UINT allocINode(FileSystem* fs) {
	
}

UINT freeINode(FileSystem* fs, UINT id) {

}

UINT readINode(FileSystem* fs, UINT id, INode* inode) {

}

UINT writeINode(FileSystem* fs, UINT id, INode* inode) {

}

UINT allocDBlk(FileSystem* fs) {

}

UINT freeDBlk(FileSystem* fs, UINT id) {

}

UINT readDBlk(FileSystem* fs, UINT id, BYTE* buf) {

}

// writes a data block to the disk
// dBlkId: the data block logical id (not raw logical id!)
// buf: the buffer to write (must be exactly block-sized)
UINT writeDBlk(FileSystem* fs, UINT dBlkId, BYTE* buf) {
    writeBlk(fs->disk, fs->diskDBlkOffset + dBlkId, buf);
}
