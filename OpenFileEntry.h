/**
 * Entries in the open file table
 * by Jon
 */

#pragma once
#include "INodeEntry.h"

enum FILE_OP {
        OP_READ = 0,
        OP_WRITE = 1,
        OP_READWRITE = 2
};

typedef struct OpenFileEntry OpenFileEntry;
struct OpenFileEntry {

    // path of open file
    char filePath[MAX_PATH_LEN];
    
    // file operations
    UINT fileOp[3];

    // the inode cached for the file
    INodeEntry* inodeEntry;

    // list pointer to next entry
    OpenFileEntry* next;

};