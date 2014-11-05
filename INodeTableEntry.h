// This is the in-memory inode table entry
// Each entry is an instance/index of an inode in buffer (or on disk)
//
#include "Globals.h"

typedef struct INodeEntry {
// __index property__
     
//inode id of this entry
  UINT _in_id;         

// __instance property__

//ref count of this entry
  UINT _in_ref;

//pointer to the next entry in this bin
  struct INodeEntry *next;
} INodeEntry;
