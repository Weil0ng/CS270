/*
 * This is the disk emulator implementation file.
 * by Weilong
 */

#include "DiskEmulator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Disk size in bytes
const UINT diskSize = 512;

void initDisk(DiskArray *disk)
{
  disk->_dsk_dskArray = malloc(diskSize);
  memset(disk->_dsk_dskArray, 0, diskSize);

  disk->_dsk_numBlk = diskSize / BLK_SIZE;
}

void destroyDisk(DiskArray *disk)
{
  free(disk->_dsk_dskArray);
}

int main(int args, char* argv[])
{
  //Declare a disk
  DiskArray disk;
  
  initDisk(&disk);
  
  printf("Disk created with size: %d\n", diskSize);
	
  for (unsigned int i=0; i<diskSize; i++)
	printf("%x ", *(disk._dsk_dskArray + i));
  printf("\n");

  destroyDisk(&disk);

  return 0;
}
