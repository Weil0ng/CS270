/*
 * This is the definition of utility functions
 */

//Debugging
typedef enum{
  _dsk_initFail,
  _dsk_full,
  _dsk_readOutOfBoundry,
  _dsk_writeOutOfBoundry,
  _fs_DBlkOutOfNumber,
  _fs_NonDirInPath,
  _fs_NonExistFile,
  _in_NonAllocDBlk,
  _in_NonAllocIndirectBlk,
  _in_IndexOutOfRange,
  _in_fileNameTooLong,
  _in_tooManyEntriesInDir,
} ERROR;

extern ERROR _err_last;    

// Print out error msg
void THROW(const char *, int, const char *);

// Shuffle an array of ints
void shuffle(int* array, int n);
