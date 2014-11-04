/**
 * Implementation file for directories
 * by Jon
 */
 
#include "Directories.h"
 
UINT mkdir(FileSystem* fs, char* path) {
 
}

UINT mknod(FileSystem* fs, char* path) {

}

UINT unlink(FileSystem* fs, char* path) {

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
