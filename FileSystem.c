/**
 * FileSystem implementation
 * by Jon
 */

#include "FileSystem.h"
#include <stdlib.h>

UINT makefs(UINT nDBlks, UINT nINodes, FileSystem* fs) {
	
    //validate number of blocks and inodes
    //num inodes divisible by inodes per block
    //fs size < max size
    //num blocks >= num inodes
    //num inodes >= inode cache size
    //num blocks >= block cache size 

    //allocate memory for filesystem struct
    fs = malloc(sizeof(FileSystem));
    
    //initialize in-memory superblock
    fs->superblock.nDBlks = nDBlks;
    fs->superblock.nFreeDBlks = nDBlks;
    fs->superblock.pFreeDBlksHead = 0;
    fs->superblock.pNextFreeDBlk = FREE_DBLK_CACHE_SIZE - 1;
    
    fs->superblock.nINodes = nINodes;
    fs->superblock.nFreeINodes = nINodes;
    fs->superblock.pNextFreeINode = FREE_INODE_CACHE_SIZE - 1;

    fs->superblock.modified = true;

    //initialize superblock free inode cache
    for(UINT i = 0; i < FREE_INODE_CACHE_SIZE; i++) {
        fs->superblock.freeINodeCache[i] = i;
    }

    //compute file system size and offset for inode/data blocks
    fs->nBytes = (BLK_SIZE + nINodes * INODE_SIZE + nDBlks * BLK_SIZE);
    fs->diskINodeBlkOffset = 1;
    fs->diskDBlkOffset = fs->diskINodeBlkOffset + nINodes / INODES_PER_BLK;

    //initialize the in-memory disk
    initDisk(fs->disk, fs->nBytes);

    //create inode list on disk
    UINT nextINodeId = 0;
    UINT nextINodeBlk = fs->diskINodeBlkOffset;
    INode nextINodeBlkBuf[INODES_PER_BLK];

    while(nextINodeBlk < fs->diskDBlkOffset) {
        //fill inode blocks one at a time
        for(UINT i = 0; i < INODES_PER_BLK; i++) {
            nextINodeBlkBuf[i]._in_type = 0; //TODO CHANGE THIS TO ENUM TYPE "FREE"
        }

        writeBlk(fs->disk, nextINodeBlk, (BYTE*) nextINodeBlkBuf);
        nextINodeBlk++;
    }

    //create free block list on disk
    UINT nextListBlk = 0;
    UINT freeDBlkList[FREE_DBLK_CACHE_SIZE];

    while(nextListBlk < fs->superblock.nDBlks) {
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

    //write superblock to disk
    BYTE superblockBuf[BLK_SIZE];
    blockify(&fs->superblock, superblockBuf);
    writeBlk(fs->disk, SUPERBLOCK_OFFSET, superblockBuf);
    fs->superblock.modified = false;
}

UINT destroyfs(FileSystem* fs) {

    destroyDisk(fs->disk);
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
