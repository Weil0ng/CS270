/*
 * This is the struct of superblock
 * by Weilong
*/

#include "Globals.h"

typedef struct SuperBlock
{
  //# of total data blocks in this file system
  UINT nDBlks;        

  //# of free data blocks in this file system
  UINT nFreeDBlks;
    
  //SuperBlock cache of free data block list
  UINT freeDBlks[NUM_FREE_DBLKS];

  //index of the next free data block in the free data block list
  UINT pNextFreeDBlk;
 
  //# of total inodes in this file system
  UINT nINodes;

  //list of free inodes
  UINT freeINodes[NUM_INODES];  

  //index of the next free inode in the free inode list
  UINT pNextFreeINode;

  //Modified bit
  BOOL modified;

  //Lock for synchronization

  //Padding
} SuperBlock;
