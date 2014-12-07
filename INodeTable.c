// This is the implementation of in core INodeTable
// by Weil0ng

#include "INodeTable.h"

#include <stdlib.h>

#ifdef DEBUG
#include <assert.h>
#endif

void initINodeTable(INodeTable* iTable)
{
  iTable->nINodes = 0;
  for(UINT bin = 0; bin < INODE_TABLE_LENGTH; bin++) {
    iTable->hashQ[bin] = NULL;
  }
}

INodeEntry* putINode(INodeTable *iTable, UINT id, INode *inode)
{
  #ifdef DEBUG
  assert(!hasINodeEntry(iTable, id));
  #endif

  //hash id to bin
  UINT bin = id % INODE_TABLE_LENGTH;
  
  //initialize new entry
  INodeEntry *newEntry;
  newEntry = malloc(sizeof(INodeEntry));
  newEntry->_in_id = id;
  newEntry->_in_node = malloc(sizeof(INode));
  memcpy(newEntry->_in_node, inode, sizeof(INode));
  newEntry->_in_ref = 0;

  //insert to table
  newEntry->next = iTable->hashQ[bin];
  iTable->hashQ[bin] = newEntry;
  iTable->nINodes++;
  
  return newEntry;
}

void putINodeEntry(INodeTable *iTable, INodeEntry *entry)
{
  #ifdef DEBUG
  assert(!hasINodeEntry(iTable, entry->_in_id));
  #endif

  //hash id to bin
  UINT bin = entry->_in_id % INODE_TABLE_LENGTH;
  
  //insert to table
  entry->next = iTable->hashQ[bin];
  iTable->hashQ[bin] = entry;
  iTable->nINodes++;
}

INodeEntry* getINodeEntry(INodeTable *iTable, UINT id)
{
  //hash id to bin
  UINT bin = id % INODE_TABLE_LENGTH;
  
  //search bin
  INodeEntry *curEntry = iTable->hashQ[bin]; 
  while (curEntry != NULL) {
    if (curEntry->_in_id == id ) {
      return curEntry;
    }
    curEntry = curEntry->next;
  }
  
  return NULL;
}

BOOL hasINodeEntry(INodeTable *iTable, UINT id)
{
  //hash id to bin
  UINT bin = id % INODE_TABLE_LENGTH;

  //search bin
  INodeEntry *curEntry = iTable->hashQ[bin];
  while (curEntry != NULL) {
    if (curEntry->_in_id == id)
      return true;
    curEntry = curEntry->next;
  }
  return false;
}

INodeEntry* removeINodeEntry(INodeTable *iTable, UINT id)
{
  //hash id to bin
  UINT bin = id % INODE_TABLE_LENGTH;

  //search bin
  INodeEntry *prevEntry = NULL;
  INodeEntry *curEntry = iTable->hashQ[bin];
  while (curEntry != NULL) {
    if (curEntry->_in_id == id) {
      //linked list remove
      if(prevEntry == NULL) 
        iTable->hashQ[bin] = curEntry->next;
      else
        prevEntry->next = curEntry->next;
        
      iTable->nINodes--;
      return curEntry;
    }
    
    prevEntry = curEntry;
    curEntry = curEntry->next;
  }
  return NULL;
}

#ifdef DEBUG
#include <stdio.h>
void printINodeTable(INodeTable *iTable)
{
  printf("[INodeTable: nINodes = %d]\n", iTable->nINodes);
  for(INT bin = 0; bin < INODE_TABLE_LENGTH; bin++) {
    INodeEntry *curEntry = iTable->hashQ[bin];
    while (curEntry != NULL) {
      printINodeEntry(curEntry);
      curEntry = curEntry->next;
    }
  }
}
#endif
