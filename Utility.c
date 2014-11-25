/*
 * This is the implementation of utility functions
 */

#include "Utility.h"
#include <stdio.h>
#include <stdlib.h>

ERROR _err_last;

void THROW(const char *fname, int lineno, const char *fxname)
{
  /*printf("Error in %s, line %d, %s:\n", fname, lineno, fxname);
  switch(_err_last) {
  case _dsk_initFail: {
    printf("Disk init failed!\n");
    break;
  }
  case _dsk_full: {
    printf("Disk is full!\n");
    break;
  }
  case _dsk_readOutOfBoundry: {
    printf("Disk read out of boundry!\n");
    break;
  }
  case _dsk_writeOutOfBoundry: {
    printf("Disk write out of boundry!\n");
    break;
  }
  case _fs_DBlkOutOfNumber: {
    printf("No more free data block on disk!\n");
    break;
  }
  case _fs_NonDirInPath: {
    printf("Wrong path, containing non-dir type file!\n");
    break;
  }
  case _fs_EndOfDirEntry: {
    printf("Reaching end of cur dir!\n");
    break;
  }
  case _fs_NonExistFile: {
    printf("File/Directory does not exist!\n");
    break;
  }
  case _in_NonAllocDBlk: {
    printf("Trying to access block that is not allocated!\n");
    break;
  }
  case _in_NonAllocIndirectBlk: {
    printf("Trying to access single indirect block that is not allocated!\n");
    break;
  }
  case _in_IndexOutOfRange: {
    printf("INode indexing out of range!\n");
    break;
  }
  case _in_fileNameTooLong: {
    printf("File name too long!\n");
    break;
  }
  default: break;
  }
  return;*/
}

void shuffle(int* array, int n) {
    size_t i;
    for (i = 0; i < n - 1; i++) {
      size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
      int t = array[j];
      array[j] = array[i];
      array[i] = t;
    }
}
