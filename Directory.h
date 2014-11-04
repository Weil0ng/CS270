// This is the header for in memory directory-type file
//

#include "Globals.h"

typedef struct DirEntry {
  BYTE key[FILE_NAME_LENGTH];
  UINT INodeID;
} DirEntry;


