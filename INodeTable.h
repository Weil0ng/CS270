// This is the in-memory inode table
// // Each entry is an instance/index of an inode in buffer (or on disk)
//
#include "Globals.h"
#include "Utility.h"

struct INodeEntry {
// __index property__
     
//inode id of this entry
  UINT _in_id;
           

// __instance property__

//ref count of this entry
  UINT _in_ref;
                   
//pointer to previous entry, head points to NULL
  INodeEntry *prev;

//pointer to next entry, tail points to NULL
  INodeEntry *next;
};

//interface to layer2 funcs, pass back the pointer to an INodeEntry, return status
BOOL getINodeEntry(INodeEntry *);
