#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include "FileSystem.h"

void PrintBlock(BYTE *buf)
{
  for (UINT i=0; i<BLK_SIZE; i++) {
    printf("%x ", *(buf + i));
  }
  printf("\n");
  return;
}

int main(int args, char* argv[])
{
    FileSystem *fs;
    makefs(10000, 500, fs);
//    printf("Created a file system with %d inodes, %d data blocks\n", fs->superblock.nINodes, fs->superblock.nDBlks);
/*
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

  BYTE writeBuf[1024];
  BYTE readBuf[BLK_SIZE];

  memset(writeBuf, 0xff, 1024);

  writeBlk(&disk, 1, writeBuf); 
  writeBlk(&disk, 2, writeBuf + BLK_SIZE);  

  if (readBlk(&disk, 0, readBuf) == 0)
    PrintBlock(readBuf);

  if (readBlk(&disk, 1, readBuf) == 0)
    PrintBlock(readBuf);

  if (readBlk(&disk, 2, readBuf) == 0)
    PrintBlock(readBuf);

  destroyDisk(&disk);
*/  
  return 0;
}            
