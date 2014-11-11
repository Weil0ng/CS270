/**
 * Implementation file for directories
 * by Jon
 */
 
#include "Directories.h"
#include <stdio.h>
#include <assert.h>

// make a new filesystem with a root directory
UINT initfs(UINT nDBlks, UINT nINodes, FileSystem* fs) {
    #ifdef DEBUG 
    printf("initfs(%d, %d, %p)\n", nDBlks, nINodes, (void*) fs); 
    #endif
    
    //call layer 1 makefs
    UINT succ = makefs(nDBlks, nINodes, fs);
    if(succ != 0) {
        fprintf(stderr, "Error: internal makefs failed with error code: %d\n", succ);
        return 1;
    }
    
    //make the root directory
    #ifdef DEBUG
    printf("Creating root directory...\n");
    #endif
    INode rootINode;
    
    UINT id = allocINode(fs, &rootINode); 
    if(id == -1) {
        fprintf(stderr, "Error: failed to allocate an inode for the root directory!\n");
        return 2;
    }
    
    // mark this as rootINodeID
    fs->rootINodeID = id;

    // init directory table for root directory
    DirEntry dirBuf[2];
   
    // insert an entry for current directory 
    strcpy(dirBuf[0].key, ".");
    dirBuf[0].INodeID = id;

    // special parent directory points back to root
    strcpy(dirBuf[1].key, "..");
    dirBuf[1].INodeID = id;

    // update the new directory table
    #ifdef DEBUG
    printf("Writing root inode directory table...\n");
    #endif
    UINT bytesWritten = writeINodeData(fs, &rootINode, (BYTE*) dirBuf, 0, 2 * sizeof(DirEntry));
    #ifdef DEBUG
    assert(bytesWritten == 2 * sizeof(DirEntry));
    #else
    if((int)bytesWritten < 2 * sizeof(DirEntry)) {
        fprintf(stderr, "Error: initfs failed to allocate data blocks for root directory!\n");
        return 2;
    }
    #endif
    
    // change the inode type to directory
    rootINode._in_type = DIRECTORY;

    // update the root inode file size
    rootINode._in_filesize = 2 * sizeof(DirEntry);
    
    // write completed root inode to disk
    #ifdef DEBUG
    printf("Writing root inode to disk...\n");
    #endif
    writeINode(fs, id, &rootINode);
          
    return 0;
}

// make a new directory
UINT mkdir(FileSystem* fs, char* path) {
    printf("mkdir(%s)\n", path);
    
    UINT id; // the inode id associated with the new directory
    UINT par_id; // the inode id of the parent directory
    char par_path[MAX_PATH_LEN];

    //check if the directory already exist
    if (strcmp(path, "/") == 0) {
        fprintf(stderr, "Error: cannot create root directory outside of initfs!\n");
        return -1;
    }
    else if ((int)namei(fs, path) != -1) {
        fprintf(stderr, "Error: file or directory %s already exists!\n", path);
        return -1;
    }

    // find the last recurrence of '/'
    char *ptr;
    int ch = '/';
    ptr = strrchr(path, ch);
    
    // ptr = "/dir_name"
    char *dir_name = strtok(ptr, "/");
   
    strncpy(par_path, path, strlen(path) - strlen(ptr));
    par_path[strlen(path) - strlen(ptr)] = '\0';

    // special case for root
    if(strcmp(par_path, "") == 0) {
        //printf("its parent is root\n");
        strcpy(par_path, "/");
    }
    
    // find the inode id of the parent directory
    #ifdef DEBUG
    printf("Computing parent directory inode id...\n");
    #endif
    par_id = namei(fs, par_path);
    #ifdef DEBUG
    printf("Parent directory inode id: %d\n", par_id);
    #endif

    // check if the parent directory exists
    if((int) par_id == -1) {
        fprintf(stderr, "Parent directory %s is invalid or doesn't exist!\n", par_path);
        return -1;
    }

    INode par_inode;
    INode inode;

    // read the parent inode
    if(readINode(fs, par_id, &par_inode) == -1) {
        fprintf(stderr, "fail to read parent directory inode %d\n", par_id);
        return -1;
    }
   
    // allocate a free inode for the new directory 
    id = allocINode(fs, &inode); 
    #ifdef DEBUG
    printf("allocated inode id %d for directory %s\n", id, dir_name);
    #endif
    if((int) id == -1) {
        fprintf(stderr, "fail to allocate an inode for the new directory!\n");
        return -1;
    }

    // read the parent directory table
    BYTE parBuf[par_inode._in_filesize];
    readINodeData(fs, &par_inode, parBuf, 0, par_inode._in_filesize);

    // insert new directory entry into parent directory list
    DirEntry newEntry;
    strcpy(newEntry.key, dir_name);
    newEntry.INodeID = id;

    UINT offset;
    for(offset = 0; offset < par_inode._in_filesize; offset += sizeof(DirEntry)) {
        DirEntry *DEntry = (DirEntry *) (parBuf + offset);
        // empty directory entry found, overwrite it
        if (DEntry->INodeID == -1){
            break;
        }
    }
    
    #ifdef DEBUG
    if(offset < par_inode._in_filesize) {
        printf("Inserting new entry into parent directory at index %d\n", offset / sizeof(DirEntry));
    }
    else {
        printf("Appending new entry to parent directory at offset %d\n", offset);
    }
    #endif
    UINT bytesWritten = writeINodeData(fs, &par_inode, (BYTE*) &newEntry, par_inode._in_filesize, sizeof(DirEntry));
    if(bytesWritten != sizeof(DirEntry)) {
        fprintf(stderr, "Error: failed to write new entry into parent directory!\n");
        return -1;
    }

    // update parent directory file size, if it changed
    if(offset + bytesWritten > par_inode._in_filesize) {
        par_inode._in_filesize = offset + bytesWritten;
    }
    
    /* allocate two entries in the new directory table (. , id) and (.., par_id) */
    // init directory table for the new directory
    DirEntry newBuf[2];
   
    // insert an entry for current directory 
    strcpy(newBuf[0].key, ".");
    newBuf[0].INodeID = id;

    // special parent directory points back to parent
    strcpy(newBuf[1].key, "..");
    newBuf[1].INodeID = par_id;
    
    bytesWritten = writeINodeData(fs, &inode, (BYTE*) newBuf, 0, 2 * sizeof(DirEntry));
    #ifdef DEBUG
    assert(bytesWritten == 2 * sizeof(DirEntry));
    #else
    if((int)bytesWritten < 2 * sizeof(DirEntry)) {
        fprintf(stderr, "Error: failed to allocate data blocks for new file!\n");
        return 2;
    }
    #endif

    // change the inode type to directory
    inode._in_type = DIRECTORY;

    // update the inode file size
    inode._in_filesize = 2 * sizeof(DirEntry);

    // update the disk inode
    writeINode(fs, id, &inode);

    return id;
}

// create a new file specified by an absolute path
UINT mknod(FileSystem* fs, char* path) {

    UINT id; // the inode id associated with the new directory
    UINT par_id; // the inode id of the parent directory
    char par_path[MAX_PATH_LEN];

    //check if the directory already exist
    if (strcmp(path, "/") == 0) {
        fprintf(stderr, "Error: cannot create root directory outside of initfs!\n");
        return -1;
    }
    else if ((int)namei(fs, path) != -1) {
        fprintf(stderr, "Error: file or directory %s already exists!\n", path);
        return -1;
    }

    // find the last recurrence of '/'
    char *ptr;
    int ch = '/';
    ptr = strrchr(path, ch);
    
    // ptr = "/dir_name"
    char *dir_name = strtok(ptr, "/");
   
    strncpy(par_path, path, strlen(path) - strlen(ptr));
    par_path[strlen(path) - strlen(ptr)] = '\0';

    // special case for root
    if(strcmp(par_path, "") == 0) {
        //printf("its parent is root\n");
        strcpy(par_path, "/");
    }
    
    // find the inode id of the parent directory 
    par_id = namei(fs, par_path);

    // check if the parent directory exists
    if((int) par_id == -1) {
        fprintf(stderr, "Parent directory %s is invalid or doesn't exist!\n", par_path);
        return -1;
    }

    INode par_inode;
    INode inode;

    // read the parent inode
    if(readINode(fs, par_id, &par_inode) == -1) {
        fprintf(stderr, "fail to read parent directory inode %d\n", par_id);
        return -1;
    }
   
    // allocate a free inode for the new directory 
    id = allocINode(fs, &inode); 
    #ifdef DEBUG
    printf("allocated inode id %d for directory %s\n", id, dir_name);
    #endif
    if((int) id == -1) {
        fprintf(stderr, "fail to allocate an inode for the new directory!\n");
        return -1;
    }

    // read the parent directory table
    BYTE parBuf[par_inode._in_filesize];
    readINodeData(fs, &par_inode, parBuf, 0, par_inode._in_filesize);

    // insert new directory entry into parent directory list
    DirEntry newEntry;
    strcpy(newEntry.key, dir_name);
    newEntry.INodeID = id;

    UINT offset;
    for(offset = 0; offset < par_inode._in_filesize; offset += sizeof(DirEntry)) {
        DirEntry *DEntry = (DirEntry *) (parBuf + offset);
        // empty directory entry found, overwrite it
        if (DEntry->INodeID == -1){
            break;
        }
    }
    
    #ifdef DEBUG
    if(offset < par_inode._in_filesize) {
        printf("Inserting new entry into parent directory at index %d\n", offset / sizeof(DirEntry));
    }
    else {
        printf("Appending new entry to parent directory at offset %d\n", offset);
    }
    #endif
    UINT bytesWritten = writeINodeData(fs, &par_inode, (BYTE*) &newEntry, par_inode._in_filesize, sizeof(DirEntry));
    if(bytesWritten != sizeof(DirEntry)) {
        fprintf(stderr, "Error: failed to write new entry into parent directory!\n");
        return -1;
    }

    // update parent directory file size, if it changed
    if(offset + bytesWritten > par_inode._in_filesize) {
        par_inode._in_filesize = offset + bytesWritten;
    }

    // change the inode type to directory
    inode._in_type = REGULAR;

    // update the disk inode
    writeINode(fs, id, &inode);

    return id;
}

UINT readdir(FileSystem* fs, char* path) {
    UINT id; // the inode of the dir

    id = namei(fs, path);
    
    if((int) id == -1) { // directory does not exist
        fprintf(stderr, "Directory %s not found!\n", path);
        return -1;
    }
    else {
        INode inode;
        
        if(readINode(fs, id, &inode) == -1) {
            fprintf(stderr, "fail to read directory inode %d\n", id);
            return -1;
        }

        if(inode._in_type != DIRECTORY) {
            fprintf(stderr, "NOT a directory\n");
            return -1;
        }
        else {
            UINT numDirEntry = (inode._in_filesize)/sizeof(DirEntry);
            BYTE dirBuf[inode._in_filesize];
            
            // read the directory table
            readINodeData(fs, &inode, dirBuf, 0, inode._in_filesize);
            
            for(UINT i=0; i < numDirEntry; i ++){
                DirEntry *DEntry = (DirEntry *) (dirBuf + i*sizeof(DirEntry));
                if ((int) DEntry->INodeID >= 0){
                    printf("%s, %d\n", DEntry->key, DEntry->INodeID);
                }
            }
        }
    }

    return 0;

}

// remove a file
UINT unlink(FileSystem* fs, char* path) {

    // 1. get the inode of the parent directory using namei
    // 2. clears the corresponding entry in the parent directory table, write
    // inode number to 0 (or -1)
    // 3. write the parent inode back to disk
    // 4. decrement file inode link count, write to disk
    // 5. if file link count = 0, release the inode and the data blocks (free)
    
    UINT id; // the inode id of the unlinked file
    UINT par_id; // the inode id of the parent directory
    char par_path[MAX_PATH_LEN];
    
    char *ptr;
    int ch = '/';

    // find the last recurrence of '/'
    ptr = strrchr(path, ch);
    strncpy(par_path, path, strlen(path) - strlen(ptr));
    par_path[strlen(path) - strlen(ptr)] = '\0';
    
    // special case for root
    if(strcmp(par_path, "") == 0) {
        printf("Its parent is root\n");
        strcpy(par_path, "/");
    }
   
    // find the inode id of the parent directory 
    par_id = namei(fs, par_path);
    if((int) par_id == -1) { // parent directory does not exist
        fprintf(stderr, "Directory %s not found!\n", par_path);
        return -1;
    }
    else {

        INode par_inode;
        INode inode;
        
        id = namei(fs, path);
        if((int) id == -1) { // file does not exist
            fprintf(stderr, "file %s not found!\n", path);
            return -1;
        }

        // read the parent inode
        if(readINode(fs, par_id, &par_inode) == -1) {
            fprintf(stderr, "fail to read parent director inode %d\n", par_id);
            return -1;
        }

        /* allocate one entry in the directory table: (ptr, id)*/
        BYTE parBuf[MAX_FILE_NUM_IN_DIR * sizeof(DirEntry)];
        
        // read the parent directory table
        readINodeData(fs, &par_inode, parBuf, 0, MAX_FILE_NUM_IN_DIR * sizeof(DirEntry));

        // find an empty directory entry and insert with the new directory
        BOOL FIND = false;
        UINT i = 0;
        //FIXME: here we assume the directory table will never be full
        while(!FIND) {
            DirEntry *DEntry = (DirEntry *) (parBuf + i*sizeof(DirEntry));
            if (DEntry->INodeID  == id){
                printf("zero out the to-be-unlinked entry in the parent directory table\n");
                strcpy(DEntry->key, "");
                DEntry->INodeID = -1;
                FIND = true;
            }
            else {
                i ++;
            }
        }

        // update the parent directory table
        writeINodeData(fs, &par_inode, parBuf, 0, MAX_FILE_NUM_IN_DIR * sizeof(DirEntry));
        
        // read the file inode
        if(readINode(fs, id, &inode) == -1) {
            fprintf(stderr, "fail to read to-be-unlinked file inode %d\n", par_id);
            return -1;
        }

        // decrement the link count of the file inode
        // TODO: does mkdir/mknod increment the linkcount? otherwise this goes
        // to -1
        inode._in_linkcount --;
        printf("update the link count to this file/directory\n");

        // write the file inode to disk
        if (inode._in_linkcount != 0) {
            writeINode(fs, id, &inode);
        }
        else {
            // free file inode when its link count is 0, which also frees the
            // associated data blocks.
            freeINode(fs, id);
            printf("free the inode %d associated with this file/dir\n", id);
        }
    }

    return 0;
}

UINT open(FileSystem* fs, char* path) {
    //addOpenFileEntry(&fs->openFileTable, path);
}

UINT close(FileSystem* fs, char* path) {
    //removeOpenFileEntry(&fs->openFileTable, path);
}

// read file from offset for numBytes
// 1. resolve path
// 2. get INodeTable Entry (ommitted for now)
// 3. load inode
// 4. modify modtime
// 5. call readINodeData on current INode, offset to the buf for numBytes
// 6. write back inode
UINT read(FileSystem* fs, char* path, UINT offset, BYTE* buf, UINT numBytes) {
  UINT returnSize = 0;
  INode curINode;
  //1. resolve path
  UINT curINodeID = namei(fs, path);
  if (curINodeID == -1) {
    _err_last = _fs_NonExistFile;
    THROW(__FILE__, __LINE__, __func__);
    return -1;
  }
  else {
    //2. TODO: get INodeTable Entry
    //3. load inode
    readINode(fs, curINodeID, &curINode);
    //4. curINode._in_modtime
    curINode._in_modtime = time(NULL);
    //5. readINodeData
    returnSize = readINodeData(fs, &curINode, buf, offset, numBytes);
    //6. write back INode
    writeINode(fs, curINodeID, &curINode);
  }
  return returnSize; 
}

//write file from offset for numBytes
//1. resolve path
//2. get INodeTable Entry
//3. load inode
//4. call writeINodeData
//5. modify inode if necessary
UINT write(FileSystem* fs, char* path, UINT offset, BYTE* buf, UINT numBytes) {
  INode curINode;
  UINT bytesWritten = 0;
  //1. resolve path
  UINT curINodeID = namei(fs, path);
  if (curINodeID == -1) {
    _err_last = _fs_NonExistFile;
    THROW(__FILE__, __LINE__, __func__);
    return -1;
  }
  else {
    //2. TODO: get INodeTable Entry
    //3. load inode
    readINode(fs, curINodeID, &curINode);
    //4. writeINodeData
    bytesWritten = writeINodeData(fs, &curINode, buf, offset, numBytes);
    //5. modify inode
    curINode._in_filesize += bytesWritten;
    curINode._in_modtime = time(NULL);
    //update INode
    writeINode(fs, curINodeID, &curINode);
  }
  return bytesWritten;
}

//resolve a path to its corresponding inode id
//1. parse the path
//2. traverse along the tokens from the root, assuming root is 0
// 2. read in the inode
// 2.1 check type
// 2.2 read in the data
// 3. scan through to find next tok's id
UINT namei(FileSystem *fs, char *path)
{
  char local_path[MAX_PATH_LEN]; // cannot use "path" directly, namei will truncate it
  strcpy(local_path, path);

  // current inode ID in traversal
  UINT curID = fs->rootINodeID; //root
  // memory for INode
  INode curINode;
  // pointer to dir entry
  UINT curDirEntry = 0;
  // memory for current directory
  // DirEntry curDir[MAX_FILE_NUM_IN_DIR];
  BYTE curDir[MAX_FILE_NUM_IN_DIR * sizeof(DirEntry)];
  
  // flag for scan result
  BOOL entryFound = false;
  //1. parse path
  char *tok = strtok(local_path, "/");
  //2 traverse along the tokens
  while (tok) {
    //printf("looking for the inode for %s\n", tok);
    readINode(fs, curID, &curINode);
    //2.1 if not directory, throw error
    if (curINode._in_type != DIRECTORY) {
      _err_last = _fs_NonDirInPath;
      THROW(__FILE__, __LINE__, __func__);
      return -1;
    }
    //2.2 read in the directory
    memset(curDir, 0, sizeof(curDir));
    readINodeData(fs, &curINode, (BYTE*) &curDir, 0, MAX_FILE_NUM_IN_DIR * sizeof(DirEntry));
    
    //3 scan through the dir
    entryFound = false;
    // Given the assumption that all blocks are initialized to be 0
    //  while (strcmp((curDir[curDirEntry]).key, "") != 0) 
    for (curDirEntry = 0; curDirEntry < MAX_FILE_NUM_IN_DIR && !entryFound; curDirEntry ++) {
      DirEntry *DEntry = (DirEntry *) (curDir + curDirEntry*sizeof(DirEntry));
      if (strcmp(tok, DEntry->key) == 0) {
        entryFound = true;
        curID = DEntry->INodeID; // move pointer to the next inode of dir or file
        //printf("find the inode for %s, its inode id = %d\n", tok, curID);
      }
    }
    //exception: dir does not contain target tok
    if (!entryFound) {
      _err_last = _fs_NonExistFile;
      THROW(__FILE__, __LINE__, __func__);
      return -1;
    }
    //1. advance in traversal
    tok = strtok(NULL, "/");
  }
  return curID;
}

