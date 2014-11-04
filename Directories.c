/**
 * Implementation file for directories
 * by Jon
 */
 
#include "Directories.h"
#include "Directory.h"
 
UINT mkdir(char* path) {
 
}

UINT mknod(char* path) {

}

UINT unlink(char* path) {

}

UINT open(char* path) {

}

UINT close(char* path) {

}

UINT read(char* path, UINT offset, BYTE* buf, UINT numBytes) {

}

UINT write(char* path, UINT offset, BYTE* buf, UINT numBytes) {

}

UINT namei(FileSystem *fs, char *path)
{
  UINT id = 0;
  UINT curId = 0; //root

  //parse path
  BYTE buf[BLK_SIZE];
  INode curINode;
  BOOL foundINode = false;
  UINT bid = -1;
  
  char *tok = strtok(path, "/");
  while (tok) {
    readINode(fs, curID, &curINode);
    //if not directory, throw error
    if (curINode._in_type != DIRECTORY) {
      _err_last = _fs_NonDirInPath;
      THROW();
      return -1;
    }
    //else, search for tok in curINode's direct 
    for (UINT k=0; k<INODE_NUM_DIRECT_BLKS; k++) {
      bid = curINode._in_directBlocks[k];
      if (bid == -1)
    }
    tok = strtok(NULL, "/");
  }
  
}
