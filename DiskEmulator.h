/*
 * This is the disk emulator header file. The emulator uses a big one-dimentional array
 * to abstract the logical blocks of a disk.
 * by Weilong
 */

#include "GlobalParams.h"

typedef struct
{
  //# of total blocks on this disk
  UINT _dsk_numBlk;
  
  //The actual array of the disk
  BYTE* _dsk_dskArray;
} DiskArray;

//initialize a disk array in memory
void initDisk(DiskArray *);

//destroy the in-memory disk array
void destroyDisk(DiskArray *);

//convert block id to disk array offset
UINT bid2Offset

//read a block from logical id i
UINT readBlk(UINT);

//write to a block of logical id i
UINT writeBLK(UINT);
