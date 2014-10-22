#include "INode.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

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
        UINT readBlk(&fs, inode->_in_sIndirectBlocks[sInBlks_index], readBuf);

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
        UINT readBlk(&fs, inode->_in_dIndirectBlocks[dIBlks_index], readBuf_d);

        // locate which single indirect block to look up
        UINT sInBlks_index = (offset - direct_space - s_indirect_space - dInBlks_index * space_per_dInBlk) / space_per_sInBlk;

        // read the single indirect block which contains a list of direct blocks
        UINT readBlk(&fs, readBuf_d[sInBlks_index], readBuf_s);

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
