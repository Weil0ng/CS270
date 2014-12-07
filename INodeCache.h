// This is the in core INode cache
// INodes that are read but not opened go to cache (i.e. namei)
// Also INodes that reach refcount 0 move from table to cache (until capacity)

#pragma once
#include "INodeEntry.h"

typedef struct INodeCache {
  UINT nINodes;
  INodeEntry* head;
  INodeEntry* tail;
} INodeCache;

//MUST be called at filesystem init time
void initINodeCache(INodeCache *);

//adds an inode to the cache, creating the entry
//returns the entry that was added
INodeEntry* cacheINode(INodeCache *cache, UINT id, INode *inode);

//adds an inode entry to the cache
void cacheINodeEntry(INodeCache *cache, INodeEntry *entry);

//interface to layer2 funcs, return the pointer to an INodeEntry
INodeEntry* getINodeCacheEntry(INodeCache *cache, UINT id);

//check if a certain entry is already loaded
BOOL hasINodeCacheEntry(INodeCache *cache, UINT id);

//removes an inode entry, returning the entry if successful
INodeEntry* removeINodeCacheEntry(INodeCache *cache, UINT id);

#ifdef DEBUG
void printINodeCache(INodeCache *);
#endif
