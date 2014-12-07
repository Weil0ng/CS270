// This is the in-memory inode table entry
// Each entry is an instance/index of an inode in buffer (or on disk)
//
#pragma once
#include "INode.h"

typedef struct INodeEntry INodeEntry;
struct INodeEntry {
// __index property__
     
//inode id of this entry
  UINT _in_id;         

// __instance property__

//ref count of this entry
  UINT _in_ref;
  
//pointer to in-core inode
  INode* _in_node;

//pointer to the next entry in this bin
  INodeEntry *next;
};

#ifdef DEBUG
void printINodeEntry(INodeEntry *);
#endif