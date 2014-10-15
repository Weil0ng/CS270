/*
 * This is the disk emulator implementation file.
 * by Weilong
 */

#include "DiskEmulator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

FILE *mapFile;

//Disk size in bytes
const UINT diskSize = 512;

void initDisk(DiskArray *disk)
{
  mapFile = fopen("diskFile", "w+r");
  if (mapFile == NULL)
  {
        printf("Disk map file open failed!");
	exit(1);
  }
  ftruncate(fileno(mapFile), diskSize);
  disk->_dsk_dskArray = mmap(NULL, diskSize, PROT_READ|PROT_WRITE, MAP_SHARED, fileno(mapFile), 0);
  fclose(mapFile);
  if (disk->_dsk_dskArray == MAP_FAILED)
  {
	printf("mmap failed!");
	exit(1);
  }
  memset(disk->_dsk_dskArray, 0x1f, diskSize);

  disk->_dsk_numBlk = diskSize / BLK_SIZE;
}

void destroyDisk(DiskArray *disk)
{
  msync(disk->_dsk_dskArray, diskSize, MS_SYNC);
  munmap(disk->_dsk_dskArray, diskSize);
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
