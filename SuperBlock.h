/*
 * This is the disk emulator to emulate the disk structure and operation, serves as layer 0
 * by Weilong
*/

#incldue "GlobalParams.h"

struct SuperBlock
{
  //# of total blocks in this file system
  unsigned int _fs_size;        

  //# of free blocks in this file system
  unsigned int _fs_nFreeBlks;

  //Logical id of head of free list
  unsigned int _fs_headFreeBlk;
 
  //# of inodes of this file system
  unsigned int _fs_iSize;

  //list of free inodes
  unsigned int _fs_freeINodeList[NUM_INODES];  

  //
};
