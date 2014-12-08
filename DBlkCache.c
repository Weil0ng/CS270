// This is the in core INodeTable

#pragma once
#include "DBlkCache.h"

//initializes all the bin entries to null
//MUST be called at filesystem init time since arrays do not default to NULL
void initDBlkCache(DBlkCache  *dCache) {
    dCache->_dCache_size = DBLK_CACHE_SET_NUM * BLK_SIZE;
    for (UINT i = 0; i < DBLK_CACHE_SET_NUM; i ++) {
        dCache->dCache[i]._dblk_id = -1;
        memset(dCache->dCache[i]._data_blk, NULL, BLK_SIZE);
    }
}


// adds a datablock to cache, replace the existing one 
INT putDBlkCacheEntry(DBlkCache *dCache, UINT id, BYTE *buf) {
    //printf("put an entry to DatablkCache, %d:\n", dCache);

    UINT set = id % DBLK_CACHE_SET_NUM;
    printf("write to set %d:\n", set);
    /*
    for (UINT j = 0; j < BLK_SIZE; j +=4) {
        printf("%d", buf[j]);
    }
    */
    dCache->dCache[set]._dblk_id = id;   
    /*printf("Inside putDBlkCacheENtry, buf: \n");
    for (UINT i = 0; i < BLK_SIZE; i += 4) {
        printf("%d", buf[i]);
    }
    printf("\n");*/
    
    memcpy(dCache->dCache[set]._data_blk, buf, BLK_SIZE);
   /* 
    //printf("after memcpy, in dCache %d,  data_blk: \n", dCache);
    printf("after memcpy,  data_blk: \n");
    for (UINT j = 0; j < BLK_SIZE; j +=4) {
        printf("%d", dCache->dCache[set]._data_blk[j]);
    }
    */
   
    
    return 0;
}

// read the datablk from the dcache into a buf
INT getDBlkCacheEntry(DBlkCache *dCache, UINT id, BYTE *buf) {
    #ifdef DEBUG_DCACHE
    assert(hasDBlkCacheEntry(dCache, id));
    #endif
    
    UINT set = id % DBLK_CACHE_SET_NUM;

    assert(id == dCache->dCache[set]._dblk_id);
    buf = dCache->dCache[set]._data_blk + set;
    return 0;
}

//check if its a dcache hit
BOOL hasDBlkCacheEntry(DBlkCache *dCache, UINT id) {
    UINT set = id % DBLK_CACHE_SET_NUM;
    printf("check if hit datablk id = %d, and the entry has id %d\n", id, dCache->dCache[set]._dblk_id);
    if(id == dCache->dCache[set]._dblk_id) {
        return true;
    }
    else {
        return false;
    }
}


#ifdef DEBUG_DCACHE
void printDBlkCache(DBlkCache *dCache, UINT set) {
    printf("Print the DatablkCache:\n");
    for (UINT i = 0; i < DBLK_CACHE_SET_NUM; i ++) {
        printf("set %d, id = %d; ", i, dCache->dCache[i]._dblk_id);
    }
    printf("\n");

    printf("datablk in the %d entry is:\n", set);
    for (UINT j = 0; j < BLK_SIZE; j +=4) {
        printf("%d", dCache->dCache[set%DBLK_CACHE_SET_NUM]._data_blk[j]);
    }
    printf("\n");
}

void printDBlkCacheEntry(DBlkCacheEntry *dCacheEntry) {
    printf("print the DBlkCache entry, its id = %d\n", dCacheEntry->_dblk_id);

}
#endif
