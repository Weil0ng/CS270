// This is the in core INodeTable

#include "Globals.h"
#include "INodeTableEntry.h"

typedef struct {
  INodeEntry* hashQ[INODE_TABLE_LENGTH];
} INodeTable;

//interface to layer2 funcs, pass back the pointer to an INodeEntry, return status
//args: table, inode id, return pointer to entry
BOOL getINodeEntry(INodeTable *, UINT, INodeEntry *);

//check if a certain entry is already loaded
BOOL hasINodeEntry(INodeTable *, UINT);
