/*
 * This is the implementation of utility functions
 */

#include "Utility.h"
#include <stdio.h>

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
  default: break;
  }
  return;
}
