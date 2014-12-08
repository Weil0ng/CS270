// This is the in core INodeTable

#pragma once
#include "DBlkCacheEntry.h"
#include "Globals.h"
#include <string.h>
#include <assert.h>

typedef struct DBlkCache{
  UINT _dCache_size;
  DBlkCacheEntry dCache[DBLK_CACHE_SET_NUM];
} DBlkCache;

//initializes all the bin entries to null
//MUST be called at filesystem init time since arrays do not default to NULL
void initDBlkCache(DBlkCache*);

// addes a datablock to cache, replace the existing one 
INT putDBlkCacheEntry(DBlkCache *dCache, UINT id, BYTE *buf);

// read the datablk from the dcache into a buf
INT getDBlkCacheEntry(DBlkCache *dCache, UINT id, BYTE *buf);

//check if its a dcache hit
BOOL hasDBlkCacheEntry(DBlkCache *, UINT);

//removes an inode entry, returning true if successful
// this maybe not necessary, we can just replace an dcache entry when
// write(put)
INT removeDBlkCacheEntry(DBlkCache *, UINT);

#ifdef DEBUG_DCACHE
void printDBlkCacheEntry(DBlkCacheEntry *);

void printDBlkCacheTable(DBlkCache *, UINT);
#endif
