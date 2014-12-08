/*
 * This is the file system interface header.  It defines the basic functions for
 * configuring the file system and reading/writing inodes and blocks
 * by Jon
 */

#include "DiskEmulator.h"
#include "OpenFileTable.h"
#include "INodeCache.h"
#include "INodeTable.h"
#include "DBlkCache.h"
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
    
    //the inode table of the filesystem
    INodeTable inodeTable;
    
    //the inode cache of the filesystem
    INodeCache inodeCache;
    
    //the datablk cache of the filesystem
    DBlkCache dCache;

    //the disk device of the filesystem
    //in Phase 1, this is an in-memory array
    DiskArray* disk;
    
} FileSystem;

// creates the file system
INT makefs(LONG, UINT, FileSystem*);

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

// reads an inode without looking for/adding to the inode cache
INT readINodeNoCache(FileSystem*, UINT, INode*);

// reads from the file section of the inode
// returns the number of bytes read, -1 on failure
LONG readINodeData(FileSystem*, INode*, BYTE*, LONG, LONG);

// writes to an inode
INT writeINode(FileSystem*, UINT, INode*);

// writes to the file section of the inode
// returns the number of bytes written, -1 on failure
LONG writeINodeData(FileSystem*, INode*, BYTE*, LONG, LONG);

// allocate a free data block
LONG allocDBlk(FileSystem*);

// free an allocated data block
INT freeDBlk(FileSystem*, LONG);

// reads a data block
INT readDBlk(FileSystem*, LONG, BYTE*);

// writes a data block
INT writeDBlk(FileSystem*, LONG, BYTE*); 

// reads certain number of bytes from a block with offset
INT readDBlkOffset(FileSystem*, LONG, BYTE*, UINT, UINT);

// writes certain number of bytes of a block with offset
INT writeDBlkOffset(FileSystem*, LONG, BYTE*, UINT, UINT); 

// converts file byte offset in inode to logical block ID
LONG bmap(FileSystem* fs, INode* inode, LONG fileBlkId);

// map flattened index to internal index of the inode
LONG balloc(FileSystem*, INode *, LONG);

// free file blk in an inode
INT bfree(FileSystem*, INode *, LONG);

#ifdef DEBUG
// prints all the inodes for debugging
void printINodes(FileSystem*);

void printDBlkInts(BYTE *buf);

void printDBlkBytes(BYTE *buf);

void printDBlkChars(BYTE *buf);

// prints all the data blocks for debugging
void printDBlks(FileSystem*);
#endif
