/**
 * Implementation file for directories
 * by Jon
 */
 
#include "Directories.h"
<<<<<<< HEAD
=======
#include "Directory.h"
#include <stdio.h>
>>>>>>> f69aa5545efb7b0aa0620bab2cabcd10eea69cf0
 
//UINT namei(FileSystem *fs, char *path);

// make a new directory
UINT mkdir(FileSystem* fs, char* path) {
    
    UINT id; // the inode id associated with the new directory
    UINT par_id; // the inode id of the parent directory
    //FIXME: assume a fixed maximum length of path
    char par_path[MAX_PATH_LEN];
   
    //check if the directory already exist
    if (namei(fs, path) != -1) {
        fprintf(stderr, "Directory %s already exists!\n", path);
        return -1;
    }
    else {
        
        char *ptr;
        int ch = '/';

        // find the last recurrence of '/'
        ptr = strrchr(path, ch);
        strncpy(par_path, path, strlen(path) - strlen(ptr));
       
        // find the inode id of the parent directory 
        par_id = namei(fs, par_path);

        // check if the parent directory exists
        if((int) par_id == -1) {
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
           
            // allocate a free inode for the new directory 
            id = allocINode(fs, &inode); 
            if(id == -1) {
                fprintf(stderr, "fail to alllocate an inode for the new directory!\n");
                return -1;
            }

            /* allocate one entry in the directory table: (ptr, id)
               FIXME: here I assume MAX_FILE_NUM_IN_DIR is the max number of
               entries in a directory table */
            BYTE parBuf[MAX_FILE_NUM_IN_DIR * sizeof(struct DirEntry)];
            
            // read the parent directory table
            readINodeData(&par_inode, parBuf, 0, MAX_FILE_NUM_IN_DIR * sizeof(struct DirEntry));

            // find an empty directory entry and insert with the new directory
            BOOL FIND = false;
            UINT i = 0;
            //FIXME: here we assume the directory table will never be full
            while(!FIND) {
                struct DirEntry *DEntry = (struct DirEntry *) (parBuf + i*sizeof(struct DirEntry));
                if (DEntry->INodeID < 0){
                    printf("find an empty entry in the parent directory table");
                    strcpy(DEntry->key, ptr);
                    DEntry->INodeID = id;
                    FIND = true;
                }
                else {
                    i ++;
                }
            }

            // update the parent directory table
            writeINodeData(&par_inode, parBuf, 0, MAX_FILE_NUM_IN_DIR * sizeof(struct DirEntry));

            
            /* allocate two entries in the new directory table (. , id) and (.., par_id) */
            BYTE newBuf[MAX_FILE_NUM_IN_DIR * sizeof(struct DirEntry)];
           
            // insert an entry for current directory 
            struct DirEntry *curDEntry = (struct DirEntry *) newBuf;
            strcpy(curDEntry->key, ptr);
            curDEntry->INodeID = id;

            // insert an entry for parent directory
            struct DirEntry *parDEntry = (struct DirEntry *) (newBuf + sizeof(struct DirEntry));
            //get the name of the parent directory
            char *par_ptr = strrchr(par_path, ch);
            strcpy(parDEntry->key, par_ptr);
            parDEntry->INodeID = par_id;

            // change the inode type to directory
            inode._in_type = DIRECTORY;

            // update the new directory table
            writeINodeData(&inode, newBuf, 0, MAX_FILE_NUM_IN_DIR * sizeof(struct DirEntry));
        }

    }

    return 0;
}

// create a new file specified by an absolute path
UINT mknod(FileSystem* fs, char* path) {

    UINT id; // the inode id associated with the new file
    UINT par_id; // the inode id of the parent directory
    //FIXME: assume a fixed maximum length of path
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
        strncpy(par_path, path, strlen(path) - strlen(ptr));
       
        // find the inode id of the parent directory 
        par_id = namei(fs, par_path);
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
            if(id == -1) {
                fprintf(stderr, "fail to alllocate an inode for the new file!\n");
                return -1;
            }

            // allocate one entry in the parent directory table: (ptr, id)
            BYTE parBuf[MAX_FILE_NUM_IN_DIR * sizeof(struct DirEntry)];
            readINodeData(&par_inode, parBuf, 0, MAX_FILE_NUM_IN_DIR * sizeof(struct DirEntry));

            // find an empty directory entry and insert with the new file
            BOOL FIND = false;
            UINT i = 0;
            //FIXME: here we assume the directory table will never be full
            while(!FIND) {
                struct DirEntry *DEntry = (struct DirEntry *) (parBuf + i*sizeof(struct DirEntry));
                if (DEntry->INodeID < 0){
                    printf("find an empty entry in the parent directory table");
                    strcpy(DEntry->key, ptr);
                    DEntry->INodeID = id;
                    FIND = true;
                }
                else {
                    i ++;
                }
            }

            // update the parent directory table
            writeINodeData(&par_inode, parBuf, 0, MAX_FILE_NUM_IN_DIR * sizeof(struct DirEntry));

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
    //FIXME: assume a fixed maximum length of path
    char par_path[MAX_PATH_LEN];
    
    char *ptr;
    int ch = '/';

    // find the last recurrence of '/'
    ptr = strrchr(path, ch);
    strncpy(par_path, path, strlen(path) - strlen(ptr));
   
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

        /* allocate one entry in the directory table: (ptr, id)
           FIXME: here I assume MAX_FILE_NUM_IN_DIR is the max number of
           entries in a directory table */
        BYTE parBuf[MAX_FILE_NUM_IN_DIR * sizeof(struct DirEntry)];
        
        // read the parent directory table
        readINodeData(&par_inode, parBuf, 0, MAX_FILE_NUM_IN_DIR * sizeof(struct DirEntry));

        // find an empty directory entry and insert with the new directory
        BOOL FIND = false;
        UINT i = 0;
        //FIXME: here we assume the directory table will never be full
        while(!FIND) {
            struct DirEntry *DEntry = (struct DirEntry *) (parBuf + i*sizeof(struct DirEntry));
            if (DEntry->INodeID  == id){
                printf("find the to-be-unlinked entry in the parent directory table");
                strcpy(DEntry->key, "");
                DEntry->INodeID = -1;
                FIND = true;
            }
            else {
                i ++;
            }
        }

        // update the parent directory table
        writeINodeData(&par_inode, parBuf, 0, MAX_FILE_NUM_IN_DIR * sizeof(struct DirEntry));
        
        // read the file inode
        if(readINode(fs, id, &inode) == -1) {
            fprintf(stderr, "fail to read to-be-unlinked file inode %d\n", par_id);
            return -1;
        }

        // decrement the link count of the file inode
        inode._in_linkcount --;

        // write the file inode to disk
        if (inode._in_linkcount != 0) {
            writeINode(fs, id, &inode);
        }
        else {
            // free file inode when its link count is 0
            freeINode(fs, id);
            
            //TODO: free the data blocks corresponding to the inode

        }
    }

    return 0;
}

UINT open(FileSystem* fs, char* path) {

}

UINT close(FileSystem* fs, char* path) {

}

// read file from offset for numBytes
// 1. resolve path
// 2. get INodeTable Entry (ommitted for now)
// 3. load inode
// 4. check length (what if numBytes > fileSize ?)
// 5. call readINodeData on current INode, offset to the buf for numBytes
UINT read(FileSystem* fs, char* path, UINT offset, BYTE* buf, UINT numBytes) {
  INode curINode;
  //1. resolve path
  UINT curINodeID = namei(fs, path);
  if (curINodeID == -1)
    exit(1);
  else {
    //2. get INodeTable Entry
    //3. load inode
    readINode(fs, curINodeID, &curINode);
    //4. check length
    //5. readINodeData
    readINodeData(&curINode, buf, offset, numBytes);
  }
  return numBytes; // what should we return?
}

//write file from offset for numBytes
//1. resolve path
//2. get INodeTable Entry
//3. load inode
//4. check length
//5. call writeINodeData
UINT write(FileSystem* fs, char* path, UINT offset, BYTE* buf, UINT numBytes) {
  INode curINode;
  //1. resolve path
  UINT curINodeID = namei(fs, path);
  if (curINodeID == -1)
    exit(1);
  else {
    //2. get INodeTable Entry
    //3. load inode
    readINode(fs, curINodeID, &curINode);
    //4. check length
    //5. writeINodeData
    writeINodeData(&curINode, buf, offset, numBytes);
  }
  return numBytes;	
}

//resolve a path to its corresponding inode id
//1. parse the path
//2. traverse along the tokens from the root, assuming root is 0
//	2. read in the inode
//		2.1 check type
//		2.2 read in the data
//	3. scan through to find next tok's id
UINT namei(FileSystem *fs, char *path)
{
  // current inode ID in traversal
  UINT curID = 0; //root

  // memory for INode
  INode curINode;

  // pointer to dir entry
  UINT curDirEntry = 0;

  // memory for current directory
  DirEntry curDir[MAX_FILE_NUM_IN_DIR];
  
  // flag for scan result
  BOOL entryFound = false;

  //1. parse path
  char *tok = strtok(path, "/");
  //2 traverse along the tokens
  while (tok) {
    readINode(fs, curID, &curINode);
    //2.1 if not directory, throw error
    if (curINode._in_type != DIRECTORY) {
      _err_last = _fs_NonDirInPath;
      THROW();
      return -1;
    }

    //2.2 read in the directory
    memset(curDir, 0, sizeof(curDir));
    readINodeData(fs, &curINode, &curDir, 0, MAX_FILE_NUM_IN_DIR * sizeof(DirEntry));
    
    //3 scan through the dir
    curDirEntry = 0;
    entryFound = false;
    // Given the assumption that all blocks are initialized to be 0
    while (strcmp((curDir[curDirEntry]).key, "") != 0) {
      if (strcmp(tok, curDir[curDirEntry].key) == 1) {
	entryFound = true;
        curID = (curDir[curDirEntry]).INodeID; // move pointer to the next inode of dir or file
        break;
      }
      curDirEntry ++;
    }

    //exception: dir does not contain target tok
    if (!entryFound) {
      _err_last = _fs_NonExistFile;
      THROW();
      return -1;
    }

    //1. advance in traversal
    tok = strtok(NULL, "/");
  }

  return curID;
}
