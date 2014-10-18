/*
 * This is the file system interface header.  It defines the basic functions for
 * configuring the file system and reading/writing inodes and blocks
 * by Jon
 */

#include "Globals.h"

// creates the file system
UINT makefs();

// allocate a free inode
UINT allocInode();

// free an allocated inode
UINT freeInode(UINT);

// reads an inode
UINT readInode(UINT, INode*);

// writes to an inode
UINT writeInode(UINT, INode*);

// allocate a free data block
UINT allocDataBlk();

// free an allocated data block
UINT freeDataBlk(UINT);

// reads a data block
UINT readDataBlk(UINT, byte*);

// writes a data block
UINT writeDataBlk(UINT, byte*); 
