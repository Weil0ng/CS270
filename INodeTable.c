// This is the implementation of in core INodeTable
// by Weil0ng

#include "Utility.h"
#include "INodeTable.h"

void initializeINodeTable(INodeTable* iTable)
{
  for(UINT bin = 0; bin < INODE_TABLE_LENGTH; bin++) {
    iTable->hashQ[bin] = NULL;
  }
}

BOOL putINodeEntry(INodeTable *iTable, UINT id, INode *inode)
{
  if(hasINodeEntry(iTable, id))
    return false;

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
  
  return true;
}

INodeEntry* getINodeEntry(INodeTable *iTable, UINT id)
{
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
/*
  //if not in table, instantiate an entry
  INode iNode;
  readINode(iTable->fs, id, &iNode);
  INodeEntry *newEntry;
  newEntry = malloc(sizeof(INodeEntry));
  newEntry->_in_id = id;
  newEntry->_in_ref = 1;
  newEntry->next = NULL;

  //insert to table
  if (tailEntry != NULL)
    tailEntry->next = newEntry;
  else
    iTable->hashQ[bin] = newEntry;

  //return entry
  iEntry = newEntry;

  return iEntry;
*/
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
