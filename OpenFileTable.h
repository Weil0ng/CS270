// Open file table keeps track of which files are opened for read/write

#include "INodeEntry.h"
#include "OpenFileEntry.h"

typedef struct OpenFileTable {

    UINT nOpenFiles;
    OpenFileEntry* head;

} OpenFileTable;

BOOL addOpenFileEntry(OpenFileTable*, char*, enum FILE_OP, INodeEntry*);

OpenFileEntry* getOpenFileEntry(OpenFileTable*, char*, enum FILE_OP);

BOOL removeOpenFileEntry(OpenFileTable*, char*, enum FILE_OP);
