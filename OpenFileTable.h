// Open file table keeps track of which files are opened for read/write

#include "INodeEntry.h"

//not used yet
enum FILE_OP {
        READ = 1,
        WRITE = 2,
        APPEND = 3
};

typedef struct OpenFileEntry {
    char path[MAX_PATH_LEN];
    enum FILE_OP op;
    INodeEntry* inodeEntry;
} OpenFileEntry;

typedef struct OpenFileTable {
    OpenFileEntry* openFiles[OPEN_FILE_TABLE_LENGTH];
} OpenFileTable;

BOOL addOpenFileEntry(OpenFileTable* table, char* path);

BOOL removeOpenFileEntry(OpenFileTable* table, char* path);