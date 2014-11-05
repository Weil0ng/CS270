// This is the implementation of in core Open File Table
// by Jon

#include "OpenFileTable.h"

BOOL addOpenFileEntry(OpenFileTable* table, char* path) {
    //phase 1 doesn't keep the inodes in memory, so we stub this off
    return true;
}

BOOL removeOpenFileEntry(OpenFileTable* table, char* path) {
    //phase 1 doesn't keep the inodes in memory, so we stub this off
    return true;
}
