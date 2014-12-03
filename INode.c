/**
 * Implementation for INode
 * by Jon
 */

#include <stdio.h>
#include "INode.h"

UINT initializeINode(INode *inode, UINT id) {
    inode->_in_type = INIT;
    strcpy(inode->_in_owner, "NULL");
    inode->_in_permissions = 777;
    inode->_in_modtime = time(NULL);
    inode->_in_accesstime = time(NULL);
    inode->_in_filesize = 0;
    inode->_in_linkcount = 0;

    //since we are storing logical data blk id, 0 could be a valid blk
    for (UINT i = 0; i < INODE_NUM_DIRECT_BLKS; i ++) {
        inode->_in_directBlocks[i] = -1;
    }
    for (UINT i = 0; i < INODE_NUM_S_INDIRECT_BLKS; i ++) {
        inode->_in_sIndirectBlocks[i] = -1;
    }
    for (UINT i = 0; i < INODE_NUM_D_INDIRECT_BLKS; i ++) {
        inode->_in_dIndirectBlocks[i] = -1;
    }
    for (UINT i = 0; i < INODE_NUM_T_INDIRECT_BLKS; i ++) {
        inode->_in_tIndirectBlocks[i] = -1;
    }

    return 0;
}

void printINode(INode* inode) {
    printf("[INode: type = %d, owner = %s, permissions = %d, modtime = %llu, accesstime = %llu, filesize = %d, linkcount = %d]\n",
        inode->_in_type, inode->_in_owner, inode->_in_permissions, inode->_in_modtime, inode->_in_accesstime, inode->_in_filesize, inode->_in_linkcount);
}
