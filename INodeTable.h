// This is the in core INodeTable

#pragma once
#include "Globals.h"
#include "INodeEntry.h"

typedef struct {
  INodeEntry* hashQ[INODE_TABLE_LENGTH];
} INodeTable;

//adds an inode to the table
//returns true if it was added, false if it already existed
BOOL putINodeEntry(INodeTable *iTable, UINT id, INode *inode);

//interface to layer2 funcs, return the pointer to an INodeEntry
//args: table, inode id, return pointer to entry
INodeEntry* getINodeEntry(INodeTable *iTable, UINT id);

//check if a certain entry is already loaded
BOOL hasINodeEntry(INodeTable *, UINT);
