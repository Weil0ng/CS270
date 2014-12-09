// This is the implementation of in core INodeCache
// by Jon

#include "INodeCache.h"

#include <stdlib.h>

#ifdef DEBUG
#include <assert.h>
#endif

void initINodeCache(INodeCache *cache) {
  cache->nINodes = 0;
  cache->head = NULL;
  cache->tail = NULL;
}

INodeEntry* cacheINode(INodeCache *cache, UINT id, INode *inode)
{
  #ifdef DEBUG
  assert(!hasINodeCacheEntry(cache, id));
  #endif

  //initialize new entry
  INodeEntry *newEntry;
  newEntry = malloc(sizeof(INodeEntry));
  newEntry->_in_id = id;
  memcpy(&newEntry->_in_node, inode, sizeof(INode));
  newEntry->_in_ref = 0;
  newEntry->next = NULL;

  //insert to queue
  if(cache->nINodes == 0) {
    cache->head = newEntry;
    cache->tail = newEntry;
  }
  else {
    cache->tail->next = newEntry;
    cache->tail = newEntry;
  }

  cache->nINodes++;  
  return newEntry;
}

void cacheINodeEntry(INodeCache *cache, INodeEntry* entry)
{
  #ifdef DEBUG
  assert(!hasINodeCacheEntry(cache, entry->_in_id));
  assert(entry->_in_ref == 0);
  #endif
  
  //insert to queue
  if(cache->nINodes == 0) {
    cache->head = entry;
    cache->tail = entry;
  }
  else {
    cache->tail->next = entry;
    cache->tail = entry;
  }

  cache->nINodes++;  
}

INodeEntry* getINodeCacheEntry(INodeCache *cache, UINT id)
{
  //search list
  INodeEntry *curEntry = cache->head; 
  while (curEntry != NULL) {
    if (curEntry->_in_id == id ) {
      return curEntry;
    }
    curEntry = curEntry->next;
  }
  
  return NULL;
}

BOOL hasINodeCacheEntry(INodeCache *cache, UINT id)
{
  //search list
  INodeEntry *curEntry = cache->head; 
  while (curEntry != NULL) {
    if (curEntry->_in_id == id ) {
      return true;
    }
    curEntry = curEntry->next;
  }
  
  return false;
}

INodeEntry* removeINodeCacheEntry(INodeCache *cache, UINT id)
{  
  //search list
  INodeEntry *prevEntry = NULL;
  INodeEntry *curEntry = cache->head;
  while (curEntry != NULL) {
    if (curEntry->_in_id == id) {
      //linked list remove
      if(prevEntry == NULL)
        cache->head = curEntry->next;
      else
        prevEntry->next = curEntry->next;
        
      if(curEntry->next == NULL)
        cache->tail = prevEntry;
        
      cache->nINodes--;
      return curEntry;
    }
    
    prevEntry = curEntry;
    curEntry = curEntry->next;
  }
  return NULL;
}

#ifdef DEBUG
#include <stdio.h>
void printINodeCache(INodeCache *cache)
{
  printf("[INodeCache: nINodes = %d]\n", cache->nINodes);
  INodeEntry *curEntry = cache->head;
  while (curEntry != NULL) {
    printINodeEntry(curEntry);
    curEntry = curEntry->next;
  }
}
#endif
