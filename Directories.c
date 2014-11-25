/**
 * Implementation file for directories
 * by Jon
 */
 
#include "Directories.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "errno.h"
#include "pwd.h"

// mounts a filesystem from a device
INT l2_mount(FileSystem* fs) {
    #ifdef DEBUG
    printf("Mounting filesystem...\n");
    #endif

    //disk superblock buffer
    BYTE dsb[BLK_SIZE];

    //read first block as superblock
    #ifdef DEBUG
    printf("Reading superblock from disk...\n");
    #endif
    INT diskFile = open(DISK_PATH, O_RDWR, 0666);
    if (diskFile == -1)
    printf("Disk open error %s\n", strerror(errno));
    lseek(diskFile, SUPERBLOCK_OFFSET, SEEK_SET);
    UINT bytesRead = read(diskFile, dsb, BLK_SIZE);
    if(bytesRead < BLK_SIZE) {
        fprintf(stderr, "Error: failed to read superblock from disk!\n");
        return -1;
    }
    close(diskFile);

    unblockify(dsb, &fs->superblock);
    #ifdef DEBUG
    printf("Unblockified superblock:\n");
    printSuperBlock(&fs->superblock);
    #endif

    //initialize filesystem parameters
    fs->nBytes = (BLK_SIZE + fs->superblock.nINodes * INODE_SIZE + fs->superblock.nDBlks * BLK_SIZE);
    fs->diskINodeBlkOffset = SUPERBLOCK_OFFSET + 1;
    fs->diskDBlkOffset = fs->diskINodeBlkOffset + fs->superblock.nINodes / INODES_PER_BLK;
    
    #ifdef DEBUG
    printf("Opening disk device...\n");
    #endif
    fs->disk = malloc(sizeof(DiskArray));
    openDisk(fs->disk, fs->nBytes);

    //load free block cache into superblock
    #ifdef DEBUG
    printf("Loading free dblk cache into superblock...\n");
    #endif
    readDBlk(fs, fs->superblock.pFreeDBlksHead, (BYTE*) (fs->superblock.freeDBlkCache));

    #ifdef DEBUG
    printf("Filesystem mount complete!\n");
    #endif
    return 0;
}

// unmounts a filesystem by syncing the superblock to disk
INT l2_unmount(FileSystem* fs) {
    #ifdef DEBUG
    printf("Unmounting filesystem...\n");
    #endif

    //write free block cache back to disk
    writeDBlk(fs, fs->superblock.pFreeDBlksHead, (BYTE*) (fs->superblock.freeDBlkCache));

    //write superblock to disk
    #ifdef DEBUG
    printf("Writing superblock to disk...\n"); 
    #endif
    BYTE superblockBuf[BLK_SIZE];
    blockify(&fs->superblock, superblockBuf);
    writeBlk(fs->disk, SUPERBLOCK_OFFSET, superblockBuf);
    fs->superblock.modified = false;
    
    //close disk to prevent future writes
    closefs(fs);

    #ifdef DEBUG
    printf("Filesystem unmount complete!\n");
    #endif
    return 0;
}

// make a new filesystem with a root directory
INT l2_initfs(UINT nDBlks, UINT nINodes, FileSystem* fs) {
    #ifdef DEBUG 
    printf("initfs(%d, %d, %p)\n", nDBlks, nINodes, (void*) fs); 
    #endif
    
    //call layer 1 makefs
    INT succ = makefs(nDBlks, nINodes, fs);
    if(succ != 0) {
        fprintf(stderr, "Error: internal makefs failed with error code: %d\n", succ);
        return 1;
    }
    
    //make the root directory
    #ifdef DEBUG
    printf("Creating root directory...\n");
    #endif
    INode rootINode;
    
    INT id = allocINode(fs, &rootINode); 
    if(id == -1) {
        fprintf(stderr, "Error: failed to allocate an inode for the root directory!\n");
        return 2;
    }
    
    // mark this as rootINodeID
    fs->superblock.rootINodeID = id;

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
    INT bytesWritten = writeINodeData(fs, &rootINode, (BYTE*) dirBuf, 0, 2 * sizeof(DirEntry));
    #ifdef DEBUG
    assert(bytesWritten == 2 * sizeof(DirEntry));
    #else
    if(bytesWritten < 2 * sizeof(DirEntry)) {
        fprintf(stderr, "Error: initfs failed to allocate data blocks for root directory!\n");
        return 2;
    }
    #endif
    
    // change the inode type to directory
    rootINode._in_type = DIRECTORY;

    // update the root inode file size
    rootINode._in_filesize = 2 * sizeof(DirEntry);

    // set init link count for . and ..
    rootINode._in_linkcount = 2;
    
    // set init permission
    rootINode._in_permissions = S_IFDIR | 0755;
    
    // write completed root inode to disk
    #ifdef DEBUG
    printf("Writing root inode to disk...\n");
    #endif
    writeINode(fs, id, &rootINode);
          
    return 0;
}

// getattr
INT l2_getattr(FileSystem* fs, char *path, struct stat *stbuf) {
    INT INodeID = l2_namei(fs, path);
    if (INodeID == -1)
	return -ENOENT;

    INode inode;
    readINode(fs, INodeID, &inode);

    stbuf->st_dev = 0;
    stbuf->st_ino = INodeID;
    stbuf->st_mode = inode._in_permissions;
    stbuf->st_nlink = inode._in_linkcount;
    stbuf->st_uid = inode._in_uid;
    stbuf->st_gid = inode._in_gid;
    stbuf->st_size = inode._in_filesize;
    stbuf->st_blksize = BLK_SIZE;
    stbuf->st_blocks = inode._in_filesize / BLK_SIZE;
    stbuf->st_atime = inode._in_accesstime;
    stbuf->st_mtime = inode._in_modtime;
    //stbuf->st_ctime;
    return 0;
}

// make a new directory
INT l2_mkdir(FileSystem* fs, char* path, uid_t uid, gid_t gid) {
    printf("mkdir(%s)\n", path);
    
    INT id; // the inode id associated with the new directory
    INT par_id; // the inode id of the parent directory
    char par_path[MAX_PATH_LEN];

    //check if the directory already exist
    if (strcmp(path, "/") == 0) {
        fprintf(stderr, "Error: cannot create root directory outside of initfs!\n");
        return -1;
    }
    INT rRes = l2_namei(fs, path);
    if (rRes == -2) {
        _err_last = _fs_NonDirInPath;
        THROW(__FILE__, __LINE__, __func__);
        return -1;
    }
    if (rRes != -1) {
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
    par_id = l2_namei(fs, par_path);
    #ifdef DEBUG
    printf("Parent directory inode id: %d\n", par_id);
    #endif

    // check if the parent directory exists
    if(par_id == -1) {
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
  
    //weilong: check for max_file_in_dir
    if (par_inode._in_filesize >= MAX_FILE_NUM_IN_DIR * sizeof(DirEntry)) {
	_err_last = _in_tooManyEntriesInDir;
	THROW(__FILE__, __LINE__, __func__);
	return -ENOSPC;
    }


    if (strlen(dir_name) > FILE_NAME_LENGTH) {
	_err_last = _in_fileNameTooLong;
	THROW(__FILE__, __LINE__, __func__);
	return -ENAMETOOLONG;
    }
 
    // allocate a free inode for the new directory 
    id = allocINode(fs, &inode); 
    #ifdef DEBUG
    printf("allocated inode id %d for directory %s\n", id, dir_name);
    #endif
    if(id == -1) {
        fprintf(stderr, "fail to allocate an inode for the new directory!\n");
        return -1;
    }

    // insert new directory entry into parent directory list
    DirEntry newEntry;
    strcpy(newEntry.key, dir_name);
    newEntry.INodeID = id;

    UINT offset;
    for(offset = 0; offset < par_inode._in_filesize; offset += sizeof(DirEntry)) {
        // search parent directory table
        DirEntry parEntry;
        readINodeData(fs, &par_inode, (BYTE*) &parEntry, offset, sizeof(DirEntry));
        
        // empty directory entry found, overwrite it
        if (parEntry.INodeID == -1){
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
    INT bytesWritten = writeINodeData(fs, &par_inode, (BYTE*) &newEntry, offset, sizeof(DirEntry));
    if(bytesWritten != sizeof(DirEntry)) {
        fprintf(stderr, "Error: failed to write new entry into parent directory!\n");
        return -1;
    }

    // update parent directory file size, if it changed
    if(offset + bytesWritten > par_inode._in_filesize) {
        par_inode._in_filesize = offset + bytesWritten;
        writeINode(fs, par_id, &par_inode);
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
    if(bytesWritten < 2 * sizeof(DirEntry)) {
        fprintf(stderr, "Error: failed to allocate data blocks for new file!\n");
        return 2;
    }
    #endif

    // change the inode type to directory
    inode._in_type = DIRECTORY;
    
    inode._in_uid = uid;

    inode._in_gid = gid;

    struct passwd *ppwd = getpwuid(uid);

    strcpy(inode._in_owner, ppwd->pw_name);

    // init the mode
    inode._in_permissions = S_IFDIR | 0755;

    // init link count
    inode._in_linkcount = 1;

    // update the inode file size
    inode._in_filesize = 2 * sizeof(DirEntry);

    // update the disk inode
    writeINode(fs, id, &inode);

    return id;
}

// create a new file specified by an absolute path
INT l2_mknod(FileSystem* fs, char* path, uid_t uid, gid_t gid) {
    printf("mknod %s\n", path);
    INT id; // the inode id associated with the new directory
    INT par_id; // the inode id of the parent directory
    char par_path[MAX_PATH_LEN];

    //check if the directory already exist
    if (strcmp(path, "/") == 0) {
        fprintf(stderr, "Error: cannot create root directory outside of initfs!\n");
        return -1;
    }
    INT rRes = l2_namei(fs, path);
    if (rRes == -2) {
        _err_last = _fs_NonDirInPath;
        THROW(__FILE__, __LINE__, __func__);
        return -1;
    }
    if (rRes != -1) {
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
    par_id = l2_namei(fs, par_path);

    // check if the parent directory exists
    if(par_id == -1) {
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
    
    //weilong: check for max_file_in_dir
    if (par_inode._in_filesize >= MAX_FILE_NUM_IN_DIR * sizeof(DirEntry)) {
        _err_last = _in_tooManyEntriesInDir;
        THROW(__FILE__, __LINE__, __func__);
        return -ENOSPC;
    }

    // allocate a free inode for the new directory 
    id = allocINode(fs, &inode); 
    #ifdef DEBUG
    printf("allocated inode id %d for directory %s\n", id, dir_name);
    #endif
    if(id == -1) {
        fprintf(stderr, "fail to allocate an inode for the new directory!\n");
        return -1;
    }

    // insert new directory entry into parent directory list
    DirEntry newEntry;
    strcpy(newEntry.key, dir_name);
    newEntry.INodeID = id;

    UINT offset;
    for(offset = 0; offset < par_inode._in_filesize; offset += sizeof(DirEntry)) {
        // search parent directory table
        DirEntry parEntry;
        readINodeData(fs, &par_inode, (BYTE*) &parEntry, offset, sizeof(DirEntry));
        
        // empty directory entry found, overwrite it
        if (parEntry.INodeID == -1){
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
    INT bytesWritten = writeINodeData(fs, &par_inode, (BYTE*) &newEntry, offset, sizeof(DirEntry));
    if(bytesWritten != sizeof(DirEntry)) {
        fprintf(stderr, "Error: failed to write new entry into parent directory!\n");
        return -1;
    }

    // update parent directory file size, if it changed
    if(offset + bytesWritten > par_inode._in_filesize) {
        par_inode._in_filesize = offset + bytesWritten;
        writeINode(fs, par_id, &par_inode);
    }

    inode._in_uid = uid;

    inode._in_gid = gid;

    struct passwd *ppwd = getpwuid(uid);

    strcpy(inode._in_owner, ppwd->pw_name);

    // change the inode type to directory
    inode._in_type = REGULAR;

    // init the mode
    inode._in_permissions = S_IFREG | 0666;

    // init link count
    inode._in_linkcount = 1;

    // update the disk inode
    writeINode(fs, id, &inode);

    return id;
}

INT l2_readdir(FileSystem* fs, char* path, UINT offset, DirEntry* curEntry) {
    INT id; // the inode of the dir
    UINT numDirEntry = 0;

    id = l2_namei(fs, path);
    
    if(id == -1) { // directory does not exist
        fprintf(stderr, "Directory %s not found!\n", path);
        return -ENOENT;
    }
    else {
        INode inode;
        
        if(readINode(fs, id, &inode) == -1) {
            fprintf(stderr, "fail to read directory inode %d\n", id);
            return -1;
        }

        if(inode._in_type != DIRECTORY) {
            fprintf(stderr, "NOT a directory\n");
            return -ENOTDIR;
        }

        numDirEntry = (inode._in_filesize)/sizeof(DirEntry);

	if (numDirEntry - 1 < offset) {
	    _err_last = _fs_EndOfDirEntry;
	    THROW(__FILE__, __LINE__, __func__);
	    return -1;		
	}

	#ifdef DEBUG
	printf("%d entries in cur dir %s, readding %u\n", numDirEntry, path, offset);
	#endif
        // read the directory table
        readINodeData(fs, &inode, curEntry, offset * sizeof(DirEntry), sizeof(DirEntry));
    }
    return 0; 
}

// remove a file
INT l2_unlink(FileSystem* fs, char* path) {

    // 1. get the inode of the parent directory using l2_namei
    // 2. clears the corresponding entry in the parent directory table, write
    // inode number to 0 (or -1)
    // 3. write the parent inode back to disk
    // 4. decrement file inode link count, write to disk
    // 5. if file link count = 0, release the inode and the data blocks (free)
    #ifdef DEBUG
    printf("Unlinking file path: %s\n", path);
    #endif
    if (strcmp(path, "/") == 0) {
        fprintf(stderr, "Error: cannot unlink root directory!\n");
        return -1;
    }
    
    INT id; // the inode id of the unlinked file
    INT par_id; // the inode id of the parent directory
    char par_path[MAX_PATH_LEN];
    
    char *ptr;
    int ch = '/';

    // find the last recurrence of '/'
    ptr = strrchr(path, ch);
    strncpy(par_path, path, strlen(path) - strlen(ptr));
    par_path[strlen(path) - strlen(ptr)] = '\0';
    
    // ptr = "/node_name"
    char *node_name = strtok(ptr, "/");
    
    // special case for root
    if(strcmp(par_path, "") == 0) {
        strcpy(par_path, "/");
    }
    
    #ifdef DEBUG
    printf("Resolved file name \"%s\" in parent directory: %s\n", node_name, par_path);
    #endif
   
    // find the inode id of the parent directory 
    par_id = l2_namei(fs, par_path);
    if(par_id == -1) { // parent directory does not exist
        fprintf(stderr, "Directory %s not found!\n", par_path);
        return -1;
    }
    else {

        INode par_inode;
        INode inode;
        
        id = l2_namei(fs, path);
        if(id == -1) { // file does not exist
            fprintf(stderr, "Error: file \"%s\" not found!\n", path);
            return -1;
        }

        // read the parent inode
        if(readINode(fs, par_id, &par_inode) == -1) {
            fprintf(stderr, "Error: fail to read parent directory inode %d\n", par_id);
            return -1;
        }

        UINT offset;
        for(offset = 0; offset < par_inode._in_filesize; offset += sizeof(DirEntry)) {
            // search parent directory table
            DirEntry entry;
            readINodeData(fs, &par_inode, (BYTE*) &entry, offset, sizeof(DirEntry));
            
            // directory entry found, mark it as removed
            if (strcmp(entry.key, node_name) == 0){
                #ifdef DEBUG
                printf("File to be unlinked found at offset: %d\n", offset);
                #endif
                //strcpy(DEntry->key, "");
                entry.INodeID = -1;
                
                // update the parent directory table
                writeINodeData(fs, &par_inode, (BYTE*) &entry, offset, sizeof(DirEntry));
            }
        }
       
        //weilong: update parent dir size
        /*par_inode._in_filesize -= sizeof(DirEntry);
	if (writeINode(fs, par_id, &par_inode) == -1) {
	    fprintf(stderr, "fail to write parent dir inode %d\n", par_id);
	    return -1;
	}*/
 
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

INT l2_rename(FileSystem* fs, char* path, char* new_path) {

    INT par_id; // the inode id of the parent directory
    char par_path[MAX_PATH_LEN];
   
    char *ptr;
    char *new_ptr;
    int ch = '/';

    // find the last recurrence of '/'
    ptr = strrchr(path, ch);
    strncpy(par_path, path, strlen(path) - strlen(ptr));
    par_path[strlen(path) - strlen(ptr)] = '\0';
   
    // ptr = "/node_name"
    char *node_name = strtok(ptr, "/");

    // new_ptr = "/new_node_name"
    new_ptr = strrchr(new_path, ch);
    char *new_node_name = strtok(new_ptr, "/");
   
    // special case for root
    if(strcmp(par_path, "") == 0) {
        strcpy(par_path, "/");
    }

    // find the inode id of the parent directory
    par_id = l2_namei(fs, par_path);
    if(par_id == -1) { // parent directory does not exist
        fprintf(stderr, "Directory %s not found!\n", par_path);
        return -1;
    }
    else {
        INode par_inode;

        // read the parent inode
        if(readINode(fs, par_id, &par_inode) == -1) {
            fprintf(stderr, "Error: fail to read parent directory inode %d\n", par_id);
            return -1;
        }

        UINT offset;
        for(offset = 0; offset < par_inode._in_filesize; offset += sizeof(DirEntry)) {
            // search parent directory table
            DirEntry entry;
            readINodeData(fs, &par_inode, (BYTE*) &entry, offset, sizeof(DirEntry));

            // directory entry found, mark it as removed
            if (strcmp(entry.key, node_name) == 0){
                #ifdef DEBUG
                printf("File to be renamed found at offset: %d\n", offset);
                #endif
                strcpy(entry.key, new_node_name);

                // update the parent directory table
                writeINodeData(fs, &par_inode, (BYTE*) &entry, offset, sizeof(DirEntry));
            }
        }
    }
	
    return 0;
}

INT l2_open(FileSystem* fs, char* path) {
    //addOpenFileEntry(&fs->openFileTable, path);
    return 0;
}

INT l2_close(FileSystem* fs, char* path) {
    //removeOpenFileEntry(&fs->openFileTable, path);
    return 0;
}

// read file from offset for numBytes
// 1. resolve path
// 2. get INodeTable Entry (ommitted for now)
// 3. load inode
// 4. modify modtime
// 5. call readINodeData on current INode, offset to the buf for numBytes
// 6. write back inode
INT l2_read(FileSystem* fs, char* path, UINT offset, BYTE* buf, UINT numBytes) {
  INT returnSize = 0;
  INode curINode;
  //1. resolve path
  INT curINodeID = l2_namei(fs, path);
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
INT l2_write(FileSystem* fs, char* path, UINT offset, BYTE* buf, UINT numBytes) {
  printf("writing %d bytes to %s with offset %d\n", numBytes, path, offset);
  INode curINode;
  INT bytesWritten = 0;
  //1. resolve path
  INT curINodeID = l2_namei(fs, path);
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
    printf("update filesize to be %d\n", curINode._in_filesize);
    curINode._in_modtime = time(NULL);
    //update INode
    writeINode(fs, curINodeID, &curINode);
  }
  return bytesWritten;
}

//update mod/access time of a file
//1. resolve path
//1. check permission (*)
//2. update inode cache (*)
//3. sync inode (*)
//4. load inode
//5. write inode
INT l2_utimens(FileSystem *fs, char *path, struct timespec tv[2]) 
{
  INT curINodeID = l2_namei(fs, path);
  if (curINodeID == -1) {
    _err_last = _fs_NonExistFile;
    THROW(__FILE__, __LINE__, __func__);
    return -ENOENT;
  }
  INode curINode;
  readINode(fs, curINodeID, &curINode);
  curINode._in_modtime = tv[0].tv_sec;
  curINode._in_accesstime = tv[1].tv_sec;
  writeINode(fs, curINodeID, &curINode);
  return 0;
}

//resolve a path to its corresponding inode id
//1. parse the path
//2. traverse along the tokens from the root, assuming root is 0
// 2. read in the inode
// 2.1 check type
// 2.2 read in the data
// 3. scan through to find next tok's id
INT l2_namei(FileSystem *fs, char *path)
{
  char local_path[MAX_PATH_LEN]; // cannot use "path" directly, namei will truncate it
  strcpy(local_path, path);

  // current inode ID in traversal
  UINT curID = fs->superblock.rootINodeID; //root
  // memory for INode
  INode curINode;
  // pointer to dir entry
  UINT curDirEntry = 0;
  // memory for current directory
  // DirEntry curDir[MAX_FILE_NUM_IN_DIR];
  BYTE *curDir;
  
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
      return -2;
    }
    // alloc space for curDir
    curDir = (BYTE *)malloc(curINode._in_filesize);
    //2.2 read in the directory
    memset(curDir, 0, curINode._in_filesize);
    readINodeData(fs, &curINode, curDir, 0, curINode._in_filesize);
    
    //3 scan through the dir
    entryFound = false;
    // Given the assumption that all blocks are initialized to be 0
    //  while (strcmp((curDir[curDirEntry]).key, "") != 0) 
    for (curDirEntry = 0; curDirEntry < (curINode._in_filesize / sizeof(DirEntry)) && !entryFound; curDirEntry ++) {
      DirEntry *DEntry = (DirEntry *) (curDir + curDirEntry*sizeof(DirEntry));
      if (strcmp(tok, DEntry->key) == 0) {
        entryFound = true;
        curID = DEntry->INodeID; // move pointer to the next inode of dir or file
        //printf("find the inode for %s, its inode id = %d\n", tok, curID);
      }
    }
    //release curDir
    free(curDir);
    //exception: dir does not contain target tok
    if (!entryFound) {
      _err_last = _fs_NonExistFile;
      THROW(__FILE__, __LINE__, __func__);
      printf("target: %s\n", tok);
      return -1;
    }
    //1. advance in traversal
    tok = strtok(NULL, "/");
  }
  return curID;
}

