/*
 * This is the file system interface header.  It defines the basic functions for
 * configuring the file system and reading/writing inodes and blocks
 * by Jon
 */

#include "Globals.h"

// creates the file system
void makefs();

// allocate a free inode
UINT allocInode();

// free an allocated inode
UINT freeInode(UINT);

// reads an inode
INode* readInode(UINT);

// writes to an inode
void writeInode(UINT, INode*);

// allocate a free block
UINT allocBlock();

// free an allocated block
UINT freeBlock(UINT);

// reads a block
Block readBlock(UINT);

// writes a block
void writeBlock(UINT, byte*); 
