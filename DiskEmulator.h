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

void initDisk(DiskArray *);
void destroyDisk(DiskArray *);
