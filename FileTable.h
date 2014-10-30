// This is the in memory file table
// Store the info of access mode of each file descriptor
// Each fopen creates an entry and each file close deletes an entry

#include "Globals.h"
#include "INodeTable.h"

struct FileTableEntry {
  UINT permission;

  UINT refCount;

  INodeTable *_in_entry;

  FileTableEntry *prev;

  FileTableEntry *next;
};

BOOL insertFileTableEntry(FileTableEntry **);

BOOL removeFileTableEntry(FileTableENtry **);
