/**
 * Implementation for INode
 * by Jon
 */

#include <stdio.h>

#include "INode.h"

UINT initializeINode(INode *inode, UINT id) {
    inode->_in_type = INIT;
    //FIXME: shall we make owner string?
    inode->_in_owner = -1;
    inode->_in_permissions = 777;
    //TODO: get time
    //inode->_in_modtime = get_time();
    inode->_in_modtime = -1;
    inode->_in_accesstime = -1;
    //inode->_in_accesstime = get_time();
    inode->_in_filesize = 0;

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
    
    //initialize in-memory fields
    inode->_in_id = id;
    inode->_in_refcount = 0;

    return 0;
}

void printINode(INode* inode) {
    printf("[INode: id = %d, type = %d, owner = %d, permissions = %d, modtime = %d, accesstime = %d, filesize = %d, refcount = %d]\n",
        inode->_in_id, inode->_in_type, inode->_in_owner, inode->_in_permissions, inode->_in_modtime, inode->_in_accesstime, inode->_in_filesize, inode->_in_refcount);
}