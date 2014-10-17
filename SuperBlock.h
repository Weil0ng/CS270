/*
 * This is the struct of superblock
 * by Weilong
*/

#incldue "Globals.h"

struct SuperBlock
{
  //# of total blocks in this file system
  UINT _fs_nBlks;        

  //# of free blocks in this file system
  UINT _fs_nFreeBlks;

  //Logical id of head of free list
  UINT _fs_headFreeBlk;
 
  //# of inodes of this file system
  UINT _fs_nINodes;

  //list of free inodes
  UINT _fs_freeINodes[NUM_INODES];  

  //pointer to the next free inode in the free inode list
  UINT _fs_pNextFreeINode;

  //pointer to the next free space in free inode list
  UINT _fs_pNextFreeINodeSpace;

  //Modified bit
  BOOL _fs_modified;

  //Padding
};
