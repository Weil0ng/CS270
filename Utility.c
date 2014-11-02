/*
 * This is the implementation of utility functions
 */

#include "Utility.h"
#include <stdio.h>
#include <stdlib.h>

ERROR _err_last;

void THROW()
{
  switch(_err_last) {
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
  default: break;
  }
  return;
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