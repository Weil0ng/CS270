// This is the in core INodeTable

#pragma once
#include "DBlkCacheEntry.h"
#include "string.h"

typedef struct DBlkCache{
  UINT _dCache_size;
  DBlkCacheEntry dCache[DBLK_CACHE_SET_NUM];
} DBlkCache;

//initializes all the bin entries to null
//MUST be called at filesystem init time since arrays do not default to NULL
void initDBlkCache(DBlkCache*);

// addes a datablock to cache, replace the existing one 
INT putDBlkCacheEntry(DBlkCache *dCache, LONG id, BYTE *buf);

// read the datablk from the dcache into a buf
INT getDBlkCacheEntry(DBlkCache *dCache, LONG id, BYTE *buf);

//check if its a dcache hit
BOOL hasDBlkCacheEntry(DBlkCache *, LONG);

//removes an DBlkCache entry, returning true if successful
INT removeDBlkCacheEntry(DBlkCache *, LONG);

#ifdef DEBUG_DCACHE
void printDBlkCache(DBlkCache *);

void printDBlkCacheEntry(DBlkCacheEntry *);
#endif
