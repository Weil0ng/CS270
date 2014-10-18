/*
 * This is the struct of superblock
 * by Weilong
*/

#incldue "Globals.h"

struct SuperBlock
{
  //# of total data blocks in this file system
  UINT _fs_nDBlks;        

  //# of free data blocks in this file system
  UINT _fs_nFreeDBlks;

  //SuperBlock cache of free data block list
  UINT _fs_freeDBlks[NUM_DBLKS];

  //index of the next free data block in the free data block list
  UINT _fs_pNextFreeDBlk;
 
  //# of total inodes in this file system
  UINT _fs_nINodes;

  //list of free inodes
  UINT _fs_freeINodes[NUM_INODES];  

  //index of the next free inode in the free inode list
  UINT _fs_pNextFreeINode;

  //Modified bit
  BOOL _fs_modified;

  //Lock for synchronization

  //Padding
};
