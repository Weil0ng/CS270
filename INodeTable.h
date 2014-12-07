// This is the in core INodeTable

#pragma once
#include "INodeEntry.h"

typedef struct INodeTable {
  UINT nINodes;
  INodeEntry* hashQ[INODE_TABLE_LENGTH];
} INodeTable;

//initializes all the bin entries to null
//MUST be called at filesystem init time since arrays do not default to NULL
void initINodeTable(INodeTable*);

//adds an inode to the table, creating the entry
//returns the entry that was added
INodeEntry* putINode(INodeTable *iTable, UINT id, INode *inode);

//adds an inode entry to the cache
void putINodeEntry(INodeTable *iTable, INodeEntry *entry);

//interface to layer2 funcs, return the pointer to an INodeEntry
//args: table, inode id, return pointer to entry
INodeEntry* getINodeEntry(INodeTable *iTable, UINT id);

//check if a certain entry is already loaded
BOOL hasINodeEntry(INodeTable *, UINT);

//removes an inode entry, returning the entry if successful
INodeEntry* removeINodeEntry(INodeTable *, UINT);

#ifdef DEBUG
void printINodeTable(INodeTable *);
#endif
