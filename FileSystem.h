/*
 * This is the file system interface header.  It defines the basic functions for
 * configuring the file system and reading/writing inodes and blocks
 * by Jon
 */

#include "DiskEmulator.h"
#include "OpenFileTable.h"
#include "INodeTable.h"
#include "SuperBlock.h"
#include "Utility.h"

typedef struct FileSystem {

    //the superblock of the filesystem
    SuperBlock superblock;

    //the size, in bytes, of the whole filesystem
    LONG nBytes;

    //logical id of the first inode block
    UINT diskINodeBlkOffset;

    //logical id of the first data block
    UINT diskDBlkOffset;
    
    //the open file table of the filesystem
    OpenFileTable openFileTable;
    
    //the inode cache of the filesystem
    INodeTable inodeTable;

    //the disk device of the filesystem
    //in Phase 1, this is an in-memory array
    DiskArray* disk;
    
} FileSystem;

// creates the file system
INT makefs(UINT, UINT, FileSystem*);

// destroys a file system
INT closefs(FileSystem*);

// allocate a free inode
INT allocINode(FileSystem*, INode*);

// free an allocated inode
INT freeINode(FileSystem*, UINT);

// reads an inode (iget)
// input: file system inode number
// output: locked inode
INT readINode(FileSystem*, UINT, INode*);

// reads from the file section of the inode
// returns the number of bytes read, -1 on failure
INT readINodeData(FileSystem*, INode*, BYTE*, UINT, UINT);

// writes to an inode
INT writeINode(FileSystem*, UINT, INode*);

// writes to the file section of the inode
// returns the number of bytes written, -1 on failure
INT writeINodeData(FileSystem*, INode*, BYTE*, UINT, UINT);

// allocate a free data block
INT allocDBlk(FileSystem*);

// free an allocated data block
INT freeDBlk(FileSystem*, UINT);

// reads a data block
INT readDBlk(FileSystem*, UINT, BYTE*);

// writes a data block
INT writeDBlk(FileSystem*, UINT, BYTE*); 

// reads certain number of bytes from a block with offset
INT readDBlkOffset(FileSystem*, UINT, BYTE*, UINT, UINT);

// writes certain number of bytes of a block with offset
INT writeDBlkOffset(FileSystem*, UINT, BYTE*, UINT, UINT); 

// converts file byte offset in inode to logical block ID
INT bmap(FileSystem* fs, INode* inode, UINT fileBlkId);

// map flattened index to internal index of the inode
INT balloc(FileSystem*, INode *, UINT);

// free file blk in an inode
INT bfree(FileSystem*, INode *, UINT);

#ifdef DEBUG
// prints all the inodes for debugging
void printINodes(FileSystem*);

void printDBlkInts(BYTE *buf);

void printDBlkBytes(BYTE *buf);

void printDBlkChars(BYTE *buf);

// prints all the data blocks for debugging
void printDBlks(FileSystem*);
#endif
