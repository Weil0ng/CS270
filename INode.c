/**
 * Implementation for INode
 * by Jon
 */

#include <stdio.h>

#include "INode.h"

void printINode(INode* inode) {
    printf("[INode: id = %d, type = %d, owner = %d, permissions = %d, modtime = %d, accesstime = %d, filesize = %d, refcount = %d]\n",
        inode->_in_id, inode->_in_type, inode->_in_owner, inode->_in_permissions, inode->_in_modtime, inode->_in_accesstime, inode->_in_filesize, inode->_in_refcount);
}