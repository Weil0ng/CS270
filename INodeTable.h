// This is the in core INodeTable

#include "Globals.h"
#include "INodeTableEntry.h"
#include "FileSystem.h"

struct INodeTable {
  FileSystem *fs;

  INodeTableEntry *[INODE_TABLE_LENGTH] hashQ;
};

//interface to layer2 funcs, pass back the pointer to an INodeEntry, return status
//args: table, inode id, return pointer to entry
BOOL getINodeEntry(INodeTable *, UINT, INodeEntry *);

//resolve path to inode id
UINT namei(char *);
