/*
 * This is the definition of utility functions
 */

#include "Globals.h"

//Debugging
typedef enum{
  _dsk_full,
  _dsk_readOutOfBoundry,
  _dsk_writeOutOfBoundry
} ERROR;

extern ERROR _err_last;    

// Print out error msg
void THROW();
