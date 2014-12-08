#include <stdio.h>
#include "DBlkCache.h"

int main() {
    struct DBlkCache *dCache = (struct DBlkCache *)malloc(sizeof(struct DBlkCache));
    initDBlkCache(dCache);
    #ifdef DEBUG_DCACHE
    printDBlkCache(dCache);
    #endif
    
    BYTE buf[BLK_SIZE];
    memset(buf, 1, BLK_SIZE);
    for(INT i = 0; i < DBLK_CACHE_SET_NUM / 2; i ++) {
        if(hasDBlkCacheEntry(dCache, i) == false) {
            putDBlkCacheEntry(dCache, i, buf);
        }
    }

    #ifdef DEBUG_DCACHE
    printDBlkCache(dCache);
    #endif

    for(UINT i = 0; i < DBLK_CACHE_SET_NUM; i ++) {
        if(hasDBlkCacheEntry(dCache, i) == true) {
            printf("cache hit on blk: %d\n", i);
            getDBlkCacheEntry(dCache, i, buf);
        }
    }

    return 0;
}
