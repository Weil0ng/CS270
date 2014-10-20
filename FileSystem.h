/*
 * This is the file system interface header.  It defines the basic functions for
 * configuring the file system and reading/writing inodes and blocks
 * by Jon
 */

#include "Globals.h"

typedef struct FileSystem {
    
    // the superblock of the filesystem
    SuperBlock* superblock;
    
} FileSystem;

// creates the file system
UINT makefs(UINT, UINT, FileSystem*);

// allocate a free inode
UINT allocINode();

// free an allocated inode
UINT freeINode(UINT);

// reads an inode
UINT readINode(UINT, INode*);

// writes to an inode
UINT writeINode(UINT, INode*);

// allocate a free data block
UINT allocDBlk();

// free an allocated data block
UINT freeDBlk(UINT);

// reads a data block
UINT readDBlk(UINT, BYTE*);

// writes a data block
UINT writeDBlk(UINT, BYTE*); 
