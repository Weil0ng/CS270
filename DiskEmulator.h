/*
 * This is the disk emulator header file. The emulator uses a big one-dimentional array
 * to abstract the logical blocks of a disk.
 * by Weilong
 */

#include "Globals.h"

typedef struct
{
  //# of total disk space in bytes
  UINT _dsk_size;

  //# of total blocks on this disk
  UINT _dsk_numBlk;
  
  //The actual array of the disk
  BYTE* _dsk_dskArray;
} DiskArray;

//initialize a disk array in memory
//args: device,
//      size
void initDisk(DiskArray *, UINT);

//destroy the in-memory disk array
void destroyDisk(DiskArray *);

//convert block id to disk array offset
UINT bid2Offset(UINT);

//read a block from logical id i
//args: device, 
//      block id
UINT readBlk(DiskArray *, UINT);

//write to a block of logical id i
//args: device, 
//      block id, 
//      content buf
UINT writeBLK(DiskArray *, UINT, BYTE *);
