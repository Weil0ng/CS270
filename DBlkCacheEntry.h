// This is the in-memory inode table entry
// Each entry is an instance/index of an inode in buffer (or on disk)
//
#pragma once
#include "Globals.h"

typedef struct DBlkCacheEntry {
     
// datablk id (starting from 0)
  LONG _dblk_id;         

// the data block
  BYTE _data_blk[BLK_SIZE];
}DBlkCacheEntry;
