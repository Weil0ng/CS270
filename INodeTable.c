// This is the implementation of in core INodeTable
// by Weil0ng

#include "Utility.h"
#include "INodeTable.h"
#include "FileSystem.h"

BOOL getINodeEntry(INodeTable *iTable, UINT id, INodeEntry *iEntry)
{
  //hash id to bin
  UINT bin = id % INODE_TABLE_LENGTH;
  
  //search bin
  INodeEntry curEntry = iTable->hashQ[bin]; 
  INodeEntry tailEntry = NULL;
  while (curEntry != NULL) {
    if (curEntry->_in_id == id ) {
      iEntry = curEntry;
      return true;
    }
    tailEntry = curEntry;
    curEntry = curEntry->next;
  }

  //if not in table, instantiate an entry
  INode iNode;
  readINode(iTable->fs, id, &iNode);
  INodeEntry *newEntry;
  newEntry = malloc(sizeof(INodeTableEntry));
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
}

BOOL hasINodeEntry(INodeTable *iTable, UINT id)
{
  //hash id to bin
  UINT bin = id % INODE_TABLE_LENGTH;

  //search bin
  INodeEntry curEntry = iTable->hashQ[bin];
  while (curEntry != NULL) {
    if (curEntry->_in_id == id)
      return true;
    curEntry = curEntry->next;
  }
  return false;
}
