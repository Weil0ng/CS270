// This is the implementation of in core INodeTable
// by Weil0ng

#include "Utility.h"
#include "INodeTable.h"

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
  newEntry->_in_ref = 1;

  //insert to table
  newEntry->next = iTable->hashQ[bin];
  iTable->hashQ[bin] = newEntry;
  
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
