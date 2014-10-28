/*
 * This is the struct of superblock
 * by Weilong
*/

#include "Globals.h"

typedef struct SuperBlock
{
  /* on disk fields */

  //# of total data blocks in this file system
  UINT nDBlks;        

  //# of free data blocks in this file system
  UINT nFreeDBlks;
  
  //logical id of currently cached free list block
  UINT pFreeDBlksHead;

  //index of the next free data block in the free data block list
  UINT pNextFreeDBlk;
 
  //# of total inodes in this file system
  UINT nINodes;

  //# of free inodes in this file system
  UINT nFreeINodes;

  //free inode list cache
  INT freeINodeCache[FREE_INODE_CACHE_SIZE];  

  //index of the next free inode in the free inode list
  INT pNextFreeINode;

  /* in-memory fields */

  //SuperBlock cache of free data block list
  UINT freeDBlkCache[FREE_DBLK_CACHE_SIZE];

  //Modified bit
  BOOL modified;

  //Lock for synchronization

} SuperBlock;

// writes disk fields of superblock into a block-sized buffer
// note: exactly BLK_SIZE bytes of memory must be allocated for buf
UINT blockify(SuperBlock*, BYTE* buf);

#ifdef DEBUG
// prints out a superblock for debugging
void printSuperBlock(SuperBlock*);
#endif

#ifdef DEBUG
// prints out the free inode cache for debugging
void printFreeINodeCache(SuperBlock*);
#endif

#ifdef DEBUG
// prints out the free data block cache for debugging
void printFreeDBlkCache(SuperBlock*);
#endif