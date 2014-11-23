// This is the implementation of in core INodeTable
// by Weil0ng

#include "Utility.h"
#include "INodeTable.h"

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

INodeEntry* putINodeEntry(INodeTable *iTable, UINT id, INode *inode)
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
  newEntry->_in_node = inode;
  newEntry->_in_ref = 1;

  //insert to table
  newEntry->next = iTable->hashQ[bin];
  iTable->hashQ[bin] = newEntry;
  iTable->nINodes++;
  
  return newEntry;
}

INodeEntry* getINodeEntry(INodeTable *iTable, UINT id)
{
  #ifdef DEBUG
  assert(hasINodeEntry(iTable, id));
  #endif

  //hash id to bin
  UINT bin = id % INODE_TABLE_LENGTH;
  
  //search bin
  INodeEntry *curEntry = iTable->hashQ[bin]; 
  INodeEntry *tailEntry = NULL;
  while (curEntry != NULL) {
    if (curEntry->_in_id == id ) {
      return curEntry;
    }
    //tailEntry = curEntry;
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

#ifdef DEBUG
#include <stdio.h>
void printINodeEntry(INodeEntry *entry)
{
  printf("[INodeEntry: id = %d, ref = %d, node = %x, next = %x]\n",
    entry->_in_id, entry->_in_ref, entry->_in_node, entry->next);
}

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
