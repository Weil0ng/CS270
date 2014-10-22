/**
 * FileSystem implementation
 * by Jon
 */

#include "FileSystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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
            nextINodeBlkBuf[i]._in_type = FREE; //TODO CHANGE THIS TO ENUM TYPE "FREE"
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
    return 0;
}

UINT destroyfs(FileSystem* fs) {

    destroyDisk(fs->disk);
    free(fs);
    return 0;
}

UINT allocINode(FileSystem* fs) {
    return 0;
	
}

UINT freeINode(FileSystem* fs, UINT id) {
    return 0;

}

UINT readINode(FileSystem* fs, UINT id, INode* inode) {
    return 0;

}

UINT writeINode(FileSystem* fs, UINT id, INode* inode) {
    return 0;

}

UINT allocDBlk(FileSystem* fs) {
    return 0;

}

UINT freeDBlk(FileSystem* fs, UINT id) {
    return 0;

}

UINT readDBlk(FileSystem* fs, UINT id, BYTE* buf) {
    return 0;

}

// writes a data block to the disk
// dBlkId: the data block logical id (not raw logical id!)
// buf: the buffer to write (must be exactly block-sized)
UINT writeDBlk(FileSystem* fs, UINT dBlkId, BYTE* buf) {
    writeBlk(fs->disk, fs->diskDBlkOffset + dBlkId, buf);
    return 0;
}


// block map of a logic file byte offset to a file system block
// input: inode, logic file byte offset
// output: physical disk block #, byte offset in this block, num of bytes in to read in the block
struct INode_out bmap(FileSystem* fs, INode* inode, UINT offset){
    struct INode_out INode_output;

    UINT space_per_sInBlk; // the address range provided by one single indirect block
    UINT space_per_dInBlk; // the address range provdied by one double indirect block

    UINT direct_space;    // total address range provided by direct blocks
    UINT s_indirect_space; // total address range provided by single indirect blocks
    UINT d_indirect_space; // total address range provided by double indirect blocks

    space_per_sInBlk = (BLK_SIZE / 4) * BLK_SIZE;     
    space_per_dInBlk = (BLK_SIZE / 4) * (BLK_SIZE / 4) * BLK_SIZE; 

    direct_space = INODE_NUM_DIRECT_BLKS * BLK_SIZE; 
    s_indirect_space = INODE_NUM_S_INDIRECT_BLKS * space_per_sInBlk;
    d_indirect_space = INODE_NUM_D_INDIRECT_BLKS * space_per_dInBlk;

    if (offset < direct_space) {
        // look up in a direct block
        UINT dBlk_index = offset / BLK_SIZE;

        // read the direct block to find the data block #
        INode_output._blk_num = inode->_in_directBlocks[dBlk_index];
        INode_output._byte_offset = offset - dBlk_index * BLK_SIZE;
        assert (INode_output._byte_offset < BLK_SIZE);
        INode_output._num_bytes = BLK_SIZE - INode_output._byte_offset;
        printf("Found in a direct block, physical block # = %d, byte offset = %d\n", INode_output._blk_num, INode_output._byte_offset);
    }
    else if (offset < s_indirect_space) {
        // look up in single indirect blocks
        BYTE readBuf[BLK_SIZE];

        // locate which indirect block to look up
        UINT sInBlks_index = (offset - direct_space) / space_per_sInBlk;

        // read the indirect block that contains a list of direct blocks 
        // fixme: disk not declared here. Shall we make it global?
        readDBlk(fs, inode->_in_sIndirectBlocks[sInBlks_index], readBuf);

        // locate which direct block to look up
        UINT dBlk_index = (offset - direct_space - sInBlks_index * space_per_sInBlk) / BLK_SIZE;

        INode_output._blk_num = readBuf[dBlk_index];
        INode_output._byte_offset = offset - direct_space - sInBlks_index * space_per_sInBlk - dBlk_index * BLK_SIZE;
        assert (INode_output._byte_offset < BLK_SIZE);
        INode_output._num_bytes = BLK_SIZE - INode_output._byte_offset;
        printf("Found in a single indirect block, physical block # = %d, byte offset = %d\n", INode_output._blk_num, INode_output._byte_offset);
    }
    else if (offset < d_indirect_space){
        // look up in double indirect blocks
        BYTE readBuf_d[BLK_SIZE]; // buffer to store the double indirect block
        BYTE readBuf_s[BLK_SIZE]; // buffer to store the single indirect block

        // locate which double indirect block to look up
        UINT dInBlks_index = (offset - direct_space - s_indirect_space) / space_per_dInBlk;
        
        // read the double indirect block which contains a list of single indirect blocks
        readDBlk(fs, inode->_in_dIndirectBlocks[dInBlks_index], readBuf_d);

        // locate which single indirect block to look up
        UINT sInBlks_index = (offset - direct_space - s_indirect_space - dInBlks_index * space_per_dInBlk) / space_per_sInBlk;

        // read the single indirect block which contains a list of direct blocks
        readDBlk(fs, readBuf_d[sInBlks_index], readBuf_s);

        // locate which direct block to look up
        UINT dBlk_index = (offset - direct_space - s_indirect_space - dInBlks_index * space_per_dInBlk - sInBlks_index * space_per_sInBlk) / BLK_SIZE;
        
        INode_output._blk_num = readBuf_d[dBlk_index];
        INode_output._byte_offset = offset - direct_space -s_indirect_space - dInBlks_index * space_per_dInBlk - sInBlks_index * space_per_sInBlk - dBlk_index * BLK_SIZE;
        assert (INode_output._byte_offset < BLK_SIZE);
        INode_output._num_bytes = BLK_SIZE - INode_output._byte_offset;
        printf("Found in a double indirect block, physical block # = %d, byte offset = %d\n", INode_output._blk_num, INode_output._byte_offset);

    }
    else {
        printf("bmap fail: out of inode address space!\n");
        exit(1);
    }

    return INode_output;
}
