#include<stdio.h>
#include<stdlib.h>
#include"DiskEmulator.h"

int main(int args, char* argv[])
{
  if (args < 2)
  {
        printf("Not enough arguments!\n");
        exit(1);
  }

  UINT diskSize = atoi(argv[1]);
  if (diskSize < 512)
  {
        printf("Not enough space for a fs!\n");
        exit(1);
  }

  //Declare a disk
  DiskArray disk;
  
  initDisk(&disk, diskSize);
  
  printf("Disk created with size: %d, %d block(s)\n", diskSize, disk._dsk_numBlk);
  
  readBlk(&disk, 1);
  
  destroyDisk(&disk);
  
  return 0;
}            
