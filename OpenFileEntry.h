/**
 * Entries in the open file table
 * by Jon
 */

#pragma once

enum FILE_OP {
        OP_READ = 1,
        OP_WRITE = 2,
        OP_READWRITE = 3
};

typedef struct OpenFileEntry OpenFileEntry;
struct OpenFileEntry {

    // path of open file
    char filePath[MAX_PATH_LEN];
    
    // file operation opened under
    enum FILE_OP fileOp;

    // the inode cached for the file
    INodeEntry* inodeEntry;

    // list pointer to next entry
    OpenFileEntry* next;

};