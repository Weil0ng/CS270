// This is the implementation of in core Open File Table
// by Jon

#include "OpenFileTable.h"

#include <stdlib.h>

BOOL addOpenFileEntry(OpenFileTable* table, char* path, enum FILE_OP op, INodeEntry* inode)
{
    //initialize new entry
    OpenFileEntry* newEntry = malloc(sizeof(OpenFileEntry));
    strcpy(newEntry->filePath, path);
    newEntry->fileOp = op;
    newEntry->inodeEntry = inode;
    
    //stack insert at linked list head
    newEntry->next = table->head;
    table->head = newEntry;
    table->nOpenFiles++;
    
    return true;
}

OpenFileEntry* getOpenFileEntry(OpenFileTable* table, char* path, enum FILE_OP op)
{
    OpenFileEntry* curEntry = table->head;
    while(curEntry != NULL) {
        if(strcmp(curEntry->filePath, path) == 0 && curEntry->fileOp == op)
            return curEntry;
        curEntry = curEntry->next;
    }
    
    return NULL;
}

BOOL removeOpenFileEntry(OpenFileTable* table, char* path, enum FILE_OP op)
{
    OpenFileEntry* prevEntry = NULL;
    OpenFileEntry* curEntry = table->head;
    while(curEntry != NULL) {
        if(strcmp(curEntry->filePath, path) == 0 && curEntry->fileOp == op) {
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
    printf("[OpenFileEntry: filePath = %s, fileOp = %d, inodeEntry = %x, inodeId = %d, next = %x]\n",
        entry->filePath, entry->fileOp, entry->inodeEntry, entry->inodeEntry->_in_id, entry->next);
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
