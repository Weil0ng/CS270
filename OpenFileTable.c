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

#ifdef DEBUG
#include <stdio.h>
void printOpenFileEntry(OpenFileEntry *entry)
{
    printf("[OpenFileEntry: filePath = %s, fileOp = %d, inodeEntry = %x, next = %x]\n",
        entry->filePath, entry->fileOp, entry->inodeEntry, entry->next);
}

void printOpenFileTable(OpenFileTable *openFileTable)
{
    printf("[OpenFileTable: nOpenFiles = %d]\n", openFileTable->nOpenFiles);
    OpenFileEntry *curEntry = openFileTable->head;
    while (curEntry != NULL) {
        printOpenFileEntry(curEntry);
        curEntry = curEntry->next;
    }
}
#endif
