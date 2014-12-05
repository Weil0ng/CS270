// This is the implementation of in core Open File Table
// by Jon

#include "OpenFileTable.h"

#include <assert.h>
#include <stdlib.h>

OpenFileEntry* addOpenFileEntry(OpenFileTable* table, char* path, INodeEntry* inode)
{
    //initialize new entry
    OpenFileEntry* newEntry = malloc(sizeof(OpenFileEntry));
    strcpy(newEntry->filePath, path);
    newEntry->inodeEntry = inode;
    for(UINT i = 0; i < 3; i++) {
        newEntry->fileOp[i] = 0;
    }
    
    //stack insert at linked list head
    newEntry->next = table->head;
    table->head = newEntry;
    table->nOpenFiles++;
    
    return newEntry;
}

UINT addOpenFileOperation(OpenFileEntry* entry, enum FILE_OP op)
{
    entry->fileOp[op]++;
    return entry->fileOp[OP_READ] + entry->fileOp[OP_WRITE] + entry->fileOp[OP_READWRITE];
}

OpenFileEntry* getOpenFileEntry(OpenFileTable* table, char* path)
{
    OpenFileEntry* curEntry = table->head;
    while(curEntry != NULL) {
        if(strcmp(curEntry->filePath, path) == 0)
            return curEntry;
        curEntry = curEntry->next;
    }
    
    return NULL;
}

UINT removeOpenFileOperation(OpenFileEntry* entry, enum FILE_OP op)
{
    assert(entry->fileOp[op] > 0);
    entry->fileOp[op]--;
    return entry->fileOp[OP_READ] + entry->fileOp[OP_WRITE] + entry->fileOp[OP_READWRITE];
}

BOOL removeOpenFileEntry(OpenFileTable* table, char* path)
{
    OpenFileEntry* prevEntry = NULL;
    OpenFileEntry* curEntry = table->head;
    while(curEntry != NULL) {
        if(strcmp(curEntry->filePath, path) == 0) {
            //linked list remove
            if(prevEntry == NULL)
                table->head = curEntry->next;
            else
                prevEntry->next = curEntry->next;
                
            free(curEntry);
            table->nOpenFiles--;
            return true;
        }
        prevEntry = curEntry;
        curEntry = curEntry->next;
    }
    return false;
}

#ifdef DEBUG
#include <stdio.h>
void printOpenFileEntry(OpenFileEntry *entry)
{
    printf("[OpenFileEntry: filePath = %s, fileOp = %d %d %d, inodeEntry = %x, inodeId = %d, next = %x]\n",
        entry->filePath, entry->fileOp[OP_READ], entry->fileOp[OP_WRITE], entry->fileOp[OP_READWRITE], entry->inodeEntry, entry->inodeEntry->_in_id, entry->next);
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
