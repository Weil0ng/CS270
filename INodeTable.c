// This is the implementation of in core INodeTable
//

#include "Utility.h"
#include "INodeTable.h"
#include "FileSystem.h"

BOOL getINodeEntry(INodeTable *iTable, UINT id, INodeEntry *iEntry)
{
  //hash id to bin
  UINT bin = id % INODE_TABLE_LENGTH;
  
  //search bin
  INodeEntry curEntry = iTable->hashQ[bin]; 
  INodeEntry tailEntry = NULL;
  while (curEntry != NULL) {
    if (curEntry -> _in_id == id ) {
      iEntry = curEntry;
      return true;
    }
    tailEntry = curEntry;
    curEntry = curEntry -> next;
  }

  //if not in table, instantiate an entry
  INode iNode;
  readINode(iTable->fs, id, &iNode);
  INodeEntry *newEntry;
  newEntry = malloc(sizeof(INodeTableEntry));
  newEntry->_in_id = id;
  newEntry->_in_ref = 1;
  newEntry->next = NULL;

  //insert to table
  if (tailEntry != NULL)
    tailEntry->next = newEntry;
  else
    iTable->hashQ[bin] = newEntry;

  //return entry
  iEntry = newEntry;
  return iEntry;
}


//take absolute path as argument
//"/foo/bar"
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
