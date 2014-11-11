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
    DirEntry dirBuf[MAX_FILE_NUM_IN_DIR];
   
    // insert an entry for current directory 
    strcpy(dirBuf[0].key, ".");
    dirBuf[0].INodeID = id;

    // special parent directory points back to root
    strcpy(dirBuf[1].key, "..");
    dirBuf[1].INodeID = id;

    // zero out remaining entries
    for (UINT i = 2; i < MAX_FILE_NUM_IN_DIR; i ++ ) {
        strcpy(dirBuf[i].key, "");
        dirBuf[i].INodeID = -1;
    }
    
    // update the new directory table
    UINT bytesWritten = writeINodeData(fs, &rootINode, (BYTE*) dirBuf, 0, MAX_FILE_NUM_IN_DIR * sizeof(DirEntry));
    assert(bytesWritten == MAX_FILE_NUM_IN_DIR * sizeof(DirEntry));
    
    // update the root inode file size
    if(bytesWritten > 0) {
        rootINode._in_filesize = bytesWritten;
    }


    // change the inode type to directory
    rootINode._in_type = DIRECTORY;

    
    // write completed root inode to disk
    writeINode(fs, id, &rootINode);
          
    return 0;
}

// make a new directory
UINT mkdir(FileSystem* fs, char* path) {
    
    UINT id; // the inode id associated with the new directory
    UINT par_id; // the inode id of the parent directory
    char par_path[MAX_PATH_LEN];

    //check if the directory already exist
    if ((int)namei(fs, path) != -1) {
        fprintf(stderr, "Directory %s already exists!\n", path);
        return -1;
    }
    else {
        //printf("directory does not exist, try create a new dir\n");

        // root directory already created in initfs()
        assert(strcmp(path, "/")!=0);

        char *ptr;
        int ch = '/';

        // find the last recurrence of '/'
        ptr = strrchr(path, ch);
        
        // ptr = "/dir_name"
        char *dir_name = strtok(ptr, "/");
       
        strncpy(par_path, path, strlen(path) - strlen(ptr));
        par_path[strlen(path) - strlen(ptr)] = '\0';

        // special case for root
        if(strcmp(par_path, "") == 0) {
            printf("its parent is root\n");
            strcpy(par_path, "/");
        }
        
        // find the inode id of the parent directory 
        par_id = namei(fs, par_path);

        // check if the parent directory exists
        if((int) par_id == -1) {
            fprintf(stderr, "Parent directory %s not found!\n", par_path);
            return -1;
        }
        else {

            INode par_inode;
            INode inode;

            // read the parent inode
            if(readINode(fs, par_id, &par_inode) == -1) {
                fprintf(stderr, "fail to read parent director inode %d\n", par_id);
                return -1;
            }
           
            // allocate a free inode for the new directory 
            id = allocINode(fs, &inode); 
            printf("allocated inode id %d for directory %s\n", id, dir_name);
            if((int) id == -1) {
                fprintf(stderr, "fail to alllocate an inode for the new directory!\n");
                return -1;
            }

            /* allocate one entry in the directory table: (dir_name, id) */
            BYTE parBuf[MAX_FILE_NUM_IN_DIR * sizeof(DirEntry)];
            
            // read the parent directory table
            readINodeData(fs, &par_inode, parBuf, 0, MAX_FILE_NUM_IN_DIR * sizeof(DirEntry));

            // find an empty directory entry and insert with the new directory
            BOOL FIND = false;
            for (UINT i = 0; i < MAX_FILE_NUM_IN_DIR && !FIND; i ++) {
                DirEntry *DEntry = (DirEntry *) (parBuf + i*sizeof(DirEntry));
                //printf("inode id of this entry is %d\n", DEntry->INodeID);
                if ((int)DEntry->INodeID < 0){
                    printf("insert a new directory to an empty entry in the parent directory table\n");
                    strcpy(DEntry->key, dir_name);
                    DEntry->INodeID = id;
                    FIND = true;
                }
            }

            // update the parent directory table
            UINT bytesWritten = writeINodeData(fs, &par_inode, parBuf, 0, MAX_FILE_NUM_IN_DIR * sizeof(DirEntry));
            assert(bytesWritten == MAX_FILE_NUM_IN_DIR * sizeof(DirEntry));
            if(bytesWritten > 0) {
                inode._in_filesize = bytesWritten;
            }
            
            /* allocate two entries in the new directory table (. , id) and (.., par_id) */
            // init directory table for the new directory
            DirEntry newBuf[MAX_FILE_NUM_IN_DIR];
           
            // insert an entry for current directory 
            strcpy(newBuf[0].key, ".");
            newBuf[0].INodeID = id;

            // special parent directory points back to parent
            strcpy(newBuf[1].key, "..");
            newBuf[1].INodeID = id;

            // initialize other entries to empty
            for (UINT i = 2; i < MAX_FILE_NUM_IN_DIR; i ++ ) {
                strcpy(newBuf[i].key, "");
                newBuf[i].INodeID = -1;
            }

            // change the inode type to directory
            inode._in_type = DIRECTORY;
            
            // update the new directory table
            writeINodeData(fs, &inode, (BYTE*) newBuf, 0, MAX_FILE_NUM_IN_DIR * sizeof(DirEntry));
            
            // update the disk inode
            writeINode(fs, id, &inode);
        }

    }

    return 0;
}

// create a new file specified by an absolute path
UINT mknod(FileSystem* fs, char* path) {

    UINT id; // the inode id associated with the new file
    UINT par_id; // the inode id of the parent directory
    char par_path[MAX_PATH_LEN];

    //check if the directory already exist
    if (namei(fs, path) != -1) {
        fprintf(stderr, "file %s already exists!\n", path);
        return -1;
    }
    else {

        char *ptr;
        int ch = '/';

        // find the last recurrence of '/'
        ptr = strrchr(path, ch);
        // ptr = "/file_name"
        char *file_name = strtok(ptr, "/");
        
        strncpy(par_path, path, strlen(path) - strlen(ptr));
        par_path[strlen(path) - strlen(ptr)] = '\0';
        
        // special case for root
        if(strcmp(par_path, "") == 0) {
            printf("Its parent is root\n");
            strcpy(par_path, "/");
        }
       
        // find the inode id of the parent directory 
        par_id = namei(fs, par_path);
        //printf("parent inode id = %d\n", par_id);
        if((int) par_id == -1) { // parent directory does not exist
            fprintf(stderr, "Directory %s not found!\n", par_path);
            return -1;
        }
        else {

            INode par_inode;
            INode inode;

            // read the parent inode
            if(readINode(fs, par_id, &par_inode) == -1) {
                fprintf(stderr, "fail to read parent director inode %d\n", par_id);
                return -1;
            }
            
            // allocate a free inode for the new file
            id = allocINode(fs, &inode); 
#ifdef DEBUG
            printf("allocated inode id %d for file %s\n", id, file_name);
#endif
            if(id == -1) {
                fprintf(stderr, "fail to alllocate an inode for the new file!\n");
                return -1;
            }
            
            /* allocate one entry in the directory table: (dir_name, id) */
            BYTE parBuf[MAX_FILE_NUM_IN_DIR * sizeof(DirEntry)];
            
            // read the parent directory table
            readINodeData(fs, &par_inode, parBuf, 0, MAX_FILE_NUM_IN_DIR * sizeof(DirEntry));

            // find an empty directory entry and insert with the new directory
            BOOL FIND = false;
            for (UINT i = 0; i < MAX_FILE_NUM_IN_DIR && !FIND; i ++) {
                DirEntry *DEntry = (DirEntry *) (parBuf + i*sizeof(DirEntry));
                if ((int)DEntry->INodeID < 0){
                    printf("insert a new file to an empty entry in the parent directory table\n");
                    strcpy(DEntry->key, file_name);
                    DEntry->INodeID = id;
                    FIND = true;
                }
            }
            
            // update the parent directory table
            writeINodeData(fs, &par_inode, parBuf, 0, MAX_FILE_NUM_IN_DIR * sizeof(DirEntry));

            // change the newly allocated inode type to REGULAR file
            inode._in_type = REGULAR;
            writeINode(fs, id, &inode);
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
// 4. check length and update inode info
// 5. call readINodeData on current INode, offset to the buf for numBytes
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
    //4.
    returnSize = curINode._in_filesize < numBytes?curINode._in_filesize:numBytes;
    //curINode._in_modtime
    //5. readINodeData
    readINodeData(fs, &curINode, buf, offset, numBytes);
  }
  return returnSize; 
}

//write file from offset for numBytes
//1. resolve path
//2. get INodeTable Entry
//3. load inode
//4. modify inode if necessary
//5. call writeINodeData
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
    //4. modify inode
    UINT curFileSize = curINode._in_filesize;
    /*
    //4.1 if file needs to be extended
    if (offset + numBytes > curFileSize) {
      // this is the ?th data block we are on
      UINT DBlkOffset = curFileSize / BLK_SIZE;
      UINT remainSize = (offset + numBytes)<MAX_FILE_SIZE?(offset + numBytes):MAX_FILE_SIZE;
      remainSize -= curFileSize;
      //use up the current DBlk
      remainSize -= (BLK_SIZE - curFileSize % BLK_SIZE);

      //position
      UINT pDBlkToAlloc = DBlkOffset + 1;
      //id
      UINT DBlkToAlloc = -1;
      while (remainSize) {
        DBlkToAlloc = allocDBlk(fs);
        if (DBlkToAlloc == -1)
          break;
        //TODO: conver position to inode directory index
        //TODO: update inode directory, may need to update indirect inodes
        remainSize = remainSize > BLK_SIZE ? (remainSize - BLK_SIZE) : 0;
      }
    }
    //TODO: update filesize and remainsize
    */
    //5. writeINodeData
    bytesWritten = writeINodeData(fs, &curINode, buf, offset, numBytes);
    curINode._in_filesize += bytesWritten;
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
    //  while (strcmp((curDir[curDirEntry]).key, "") != 0) {
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
