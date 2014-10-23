/*
 * This is the file system interface header.  It defines the basic functions for
 * configuring the file system and reading/writing inodes and blocks
 * by Jon
 */

#include "Globals.h"
#include "DiskEmulator.h"
#include "INode.h"
#include "SuperBlock.h"

typedef struct FileSystem {

    //the superblock of the filesystem
    SuperBlock superblock;

    //the size, in bytes, of the whole filesystem
    UINT nBytes;

    //logical id of the first inode block
    UINT diskINodeBlkOffset;

    //logical id of the first data block
    UINT diskDBlkOffset;

    //the disk device of the filesystem
    //in Phase 1, this is an in-memory array
    DiskArray* disk;
    
} FileSystem;

// creates the file system
UINT makefs(UINT, UINT, FileSystem*);

// destroys a file system
UINT destroyfs(FileSystem*);

// allocate a free inode
UINT allocINode(FileSystem*, INode*);

// free an allocated inode
UINT freeINode(FileSystem*, UINT);

// reads an inode (iget)
// input: file system inode number
// output: locked inode
UINT readINode(FileSystem*, UINT, INode*);

// writes to an inode
UINT writeINode(FileSystem*, UINT, INode*);

// allocate a free data block
UINT allocDBlk(FileSystem*);

// free an allocated data block
UINT freeDBlk(FileSystem*, UINT);

// reads a data block
UINT readDBlk(FileSystem*, UINT, BYTE*);

// writes a data block
UINT writeDBlk(FileSystem*, UINT, BYTE*); 

// converts file byte offset in inode to logical block ID
UINT bmap(FileSystem* fs, INode* inode, UINT offset, UINT* cvt_blk_num);

