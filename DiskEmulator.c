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
UINT diskSize = 0;

void initDisk(DiskArray *disk)
{
  mapFile = fopen("diskFile", "w+r");
  if (mapFile == NULL)
  {
        printf("Disk map file open failed!\n");
	exit(1);
  }
  //truncate the file to the right size
  ftruncate(fileno(mapFile), diskSize);
  
  //create the memory map
  disk->_dsk_dskArray = mmap(NULL, diskSize, PROT_READ|PROT_WRITE, MAP_SHARED, fileno(mapFile), 0);

  //no need for file stream anymore
  fclose(mapFile);

  if (disk->_dsk_dskArray == MAP_FAILED)
  {
	printf("mmap failed!\n");
	exit(1);
  }

  memset(disk->_dsk_dskArray, 0x1f, diskSize);
  disk->_dsk_numBlk = diskSize / BLK_SIZE;
}

void destroyDisk(DiskArray *disk)
{
  //Sync write operation force consistancy of the mapped file
  msync(disk->_dsk_dskArray, diskSize, MS_SYNC);
  munmap(disk->_dsk_dskArray, diskSize);
}

UINT readBlk(UINT bid)
{
  
}

int main(int args, char* argv[])
{
  if (args < 2)
  {
	printf("Not enough arguments!\n");
	exit(1);
  }
  
  diskSize = atoi(argv[1]);
  if (diskSize < 512)
  {
	printf("Not enough space for a fs!\n");
	exit(1);
  }  

  //Declare a disk
  DiskArray disk;
  
  initDisk(&disk);
  
  printf("Disk created with size: %d, %d block(s)\n", diskSize, disk._dsk_numBlk);

	
  destroyDisk(&disk);

  return 0;
}
