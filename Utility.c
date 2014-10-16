/*
 * This is the implementation of utility functions
 */

#include "Utility.h"

void interpErr(ERROR error)
{
  switch(error) {
  case ERROR._dsk_full: {
    printf("Disk is full");
    break;
  }
  
  default:
  }
  return;
}
