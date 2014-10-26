// This is to test layer1 data block operations
// by weil0ng

#include "FileSystem.h"

int main(int args, char* argv[])
{
  if (args < 3)
  {
        printf("Not enough arguments!\n");
        exit(1);
  }
  
  UINT nDBlks = atoi(argv[1]);
  UINT nINodes = atoi(argv[2]);

  FileSystem fs;
  makefs(nDBlks, nINodes, &fs);

  #ifdef DEBUG
  printf("%d\n", fs.nBytes);
  dumpDisk(fs.disk);
  #endif
  
  return 0;
}
