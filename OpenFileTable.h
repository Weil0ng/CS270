// Open file table keeps track of which files are opened for read/write

#include "OpenFileEntry.h"

typedef struct OpenFileTable {

    UINT nOpenFiles;
    OpenFileEntry* head;

} OpenFileTable;

//MUST be called at filesystem init time
void initOpenFileTable(OpenFileTable*);

OpenFileEntry* addOpenFileEntry(OpenFileTable*, char*, INodeEntry*);

UINT addOpenFileOperation(OpenFileEntry*, enum FILE_OP op);

OpenFileEntry* getOpenFileEntry(OpenFileTable*, char*);

UINT removeOpenFileOperation(OpenFileEntry*, enum FILE_OP op);

BOOL removeOpenFileEntry(OpenFileTable*, char*);

#ifdef DEBUG
void printOpenFileEntry(OpenFileEntry *);

void printOpenFileTable(OpenFileTable *);
#endif
