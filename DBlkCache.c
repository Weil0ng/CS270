// This is the in core INodeTable

#pragma once
#include "DBlkCache.h"
#include <assert.h>

//initializes all the bin entries to null
//MUST be called at filesystem init time since arrays do not default to NULL
void initDBlkCache(DBlkCache *dCache) {
    dCache->_dCache_size = 0;
    for (UINT i = 0; i < DBLK_CACHE_SET_NUM; i++) {
        dCache->dCache[i]._dblk_id = -1;
        memset(dCache->dCache[i]._data_blk, 0, BLK_SIZE);
    }
}

// adds a datablock to cache, replace the existing one 
INT putDBlkCacheEntry(DBlkCache *dCache, LONG id, BYTE *buf) {
    UINT set = id % DBLK_CACHE_SET_NUM;
    //new entry, update size counter
    if(dCache->dCache[set]._dblk_id == -1) {
        dCache->_dCache_size++;
    }
    
    dCache->dCache[set]._dblk_id = id;
    memcpy(dCache->dCache[set]._data_blk, buf, BLK_SIZE);
    return 0;
}

// read the datablk from the dcache into a buf
INT getDBlkCacheEntry(DBlkCache *dCache, LONG id, BYTE *buf) {
    #ifdef DEBUG_DCACHE
    assert(hasDBlkCacheEntry(dCache, id));
    #endif
    
    UINT set = id % DBLK_CACHE_SET_NUM;
    memcpy(buf, dCache->dCache[set]._data_blk, BLK_SIZE);
    return 0;
}

// remove a cache entry when the datablock is freed
INT removeDBlkCacheEntry(DBlkCache *dCache, LONG id) {
    assert(hasDBlkCacheEntry(dCache, id) == true);
    UINT set = id % DBLK_CACHE_SET_NUM;
    
    dCache->dCache[set]._dblk_id = -1;
    memset(dCache->dCache[set]._data_blk, 0, BLK_SIZE);
    
    dCache->_dCache_size--;
    return 0;
}

//check if its a dcache hit
BOOL hasDBlkCacheEntry(DBlkCache *dCache, LONG id) {
    UINT set = id % DBLK_CACHE_SET_NUM;
    return id == dCache->dCache[set]._dblk_id;
}

#ifdef DEBUG_DCACHE
#include <stdio.h>
void printDBlkCache(DBlkCache *dCache) {    
  printf("[DBlkCache: _dCache_size = %d]\n", dCache->_dCache_size);
  for(INT set = 0; set < DBLK_CACHE_SET_NUM; set++) {
    printf("Set %d: ", set);
    DBlkCacheEntry* curEntry = &dCache->dCache[set];
    printDBlkCacheEntry(curEntry);
  }
}

void printDBlkCacheEntry(DBlkCacheEntry *dCacheEntry) {
    printf("[DBlkCacheEntry: id = %d]\n", dCacheEntry->_dblk_id);
    /*
    for(UINT k = 0; k < BLK_SIZE / sizeof(char); k++) {
        printf("%c ", (char) dCacheEntry->_data_blk[k]);
    }
    printf("\n");
    */
}
#endif
