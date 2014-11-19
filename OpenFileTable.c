// This is the implementation of in core Open File Table
// by Jon

#include "OpenFileTable.h"

BOOL addOpenFileEntry(OpenFileTable* table, char* path, enum FILE_OP op, INodeEntry* inode) {
    //phase 1 doesn't keep the inodes in memory, so we stub this off
    return true;
}

OpenFileEntry* getOpenFileEntry(OpenFileTable* table, char* path, enum FILE_OP op) {
    return NULL;
}

BOOL removeOpenFileEntry(OpenFileTable* table, char* path, enum FILE_OP op) {
    //phase 1 doesn't keep the inodes in memory, so we stub this off
    return true;
}
