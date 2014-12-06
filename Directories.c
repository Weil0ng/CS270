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
    if (INodeID < 0)
	return INodeID;

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
    if (rRes == -ENOTDIR) {
        _err_last = _fs_NonDirInPath;
        THROW(__FILE__, __LINE__, __func__);
        return -ENOTDIR;
    }
    if (rRes > 0) {
        fprintf(stderr, "Error: file or directory %s already exists!\n", path);
        return -EEXIST;
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
    if(par_id < 0) {
	_err_last = _fs_NonExistFile;
	THROW(__FILE__, __LINE__, __func__);
        return par_id;
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
        return -EDQUOT;
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
        return -EDQUOT;
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
    if (rRes == -ENOTDIR ) {
        _err_last = _fs_NonDirInPath;
        THROW(__FILE__, __LINE__, __func__);
        return rRes;
    }
    if (rRes > 0) {
        fprintf(stderr, "Error: file or directory %s already exists!\n", path);
        return -EEXIST;
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
    if(par_id < 0) {
        fprintf(stderr, "Parent directory %s is invalid or doesn't exist!\n", par_path);
        return par_id;
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
    
    //weilong: check for long names 
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
        return -EDQUOT;
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
    
    if(id < 0) { // directory does not exist
        fprintf(stderr, "Directory %s not found!\n", path);
        return id;
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

// remove a file/remove dir
INT l2_unlink(FileSystem* fs, char* path) {

    // 1. get the inode of the parent directory using l2_namei
    // 2. clears the corresponding entry in the parent directory table, write
    // inode number to -1
    // 3. write the parent inode back to disk
    // 4. decrement file inode link count, write to disk
    // 5. if file link count = 0, 
    //   5.1 if file is reg file, release the inode and the data blocks
    //   5.2 if file is dir, recursively release all the concerned inodes and DBlks
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
    if(par_id < 0) { // parent directory does not exist
        fprintf(stderr, "Directory %s not found!\n", par_path);
        return par_id;
    }
    else {

        INode par_inode;
        INode inode;
        
        id = l2_namei(fs, path);
        if(id < 0) { // file does not exist
            fprintf(stderr, "Error: file \"%s\" not found!\n", path);
            return id;
        }

        // read the parent inode
        if(readINode(fs, par_id, &par_inode) == -1) {
            fprintf(stderr, "Error: fail to read parent directory inode %d\n", par_id);
            return -1;
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

	UINT offset = 0;
        // write the file inode to disk
        if (inode._in_linkcount != 0) {
            writeINode(fs, id, &inode);
        }
        else {
            // free file inode when its link count is 0, which also frees the
            // associated data blocks.
	    //weilong: remove dir
	    if (inode._in_type == DIRECTORY) {
          	// search parent directory table
		#ifdef DEBUG
		    printf("rm -r\n");
		#endif
		//skip . and ..
		for(offset = 2*sizeof(DirEntry); offset < inode._in_filesize; offset += sizeof(DirEntry)) {
            	    DirEntry entry;
                    readINodeData(fs, &inode, (BYTE*) &entry, offset, sizeof(DirEntry));
	 	    if (entry.INodeID != -1) {
			//call unlink
			char *recur_path = (char *)malloc(strlen(path) + 1 + strlen(entry.key));
			strcat(recur_path, path);
			strcat(recur_path, "/");
			strcat(recur_path, entry.key);
			#ifdef DEBUG
			    printf("recursively rm -r %s\n", entry.key);
			#endif
			if (l2_unlink(fs, recur_path) != 0) {
			    _err_last = _fs_recursiveUnlinkFail;
			    THROW(__FILE__, __LINE__, __func__);
		            return -1;
			}
		    }
        	}
	    }
            freeINode(fs, id);
	    #ifdef DEBUG
            printf("free the inode %d associated with this file/dir\n", id);
	    #endif
        }
	//remove this inode last because we might use it in namei to recurse
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
    }

    return 0;
}

INT l2_rename(FileSystem* fs, char* path, char* new_path) {

    printf("Enter l2_rename, move from %s to %s\n", path, new_path);

    INT par_id, new_par_id;
    char par_path[MAX_PATH_LEN];
    char new_par_path[MAX_PATH_LEN];
    char *ptr;
    char *new_ptr;
    int ch = '/';

    // find the old parent path
    ptr = strrchr(path, ch);
    strncpy(par_path, path, strlen(path) - strlen(ptr));
    par_path[strlen(path) - strlen(ptr)] = '\0';
    // special case for root
    if(strcmp(par_path, "") == 0) {
        strcpy(par_path, "/");
    }
    // ptr = "/node_name"
    char *node_name = strtok(ptr, "/");
   
    par_id = l2_namei(fs, par_path);
    if(par_id == -1) { // parent directory does not exist
        fprintf(stderr, "Directory %s not found!\n", par_path);
        return -1;
    }
    
    INode par_inode;

    // read the parent inode
    if(readINode(fs, par_id, &par_inode) == -1) {
        fprintf(stderr, "Error: fail to read old parent directory inode %d\n", par_id);
        return -1;
    }
    
    
    INT node_id;
    for(UINT i = 0; i < par_inode._in_filesize; i += sizeof(DirEntry)) {
        // search parent directory table
        DirEntry entry;
        readINodeData(fs, &par_inode, (BYTE*) &entry, i, sizeof(DirEntry));

        // directory entry found, mark it as removed
        if (strcmp(entry.key, node_name) == 0){
            #ifdef DEBUG
            printf("File/Dir to be moved found at offset: %d\n", i);
            #endif
            node_id = entry.INodeID;
            strcpy(entry.key, "");
            entry.INodeID = -1;

            // update the parent directory table
            writeINodeData(fs, &par_inode, (BYTE*) &entry, i, sizeof(DirEntry));
        }
    }

    // find the new parent path
    new_ptr = strrchr(new_path, ch);
    strncpy(new_par_path, new_path, strlen(new_path) - strlen(new_ptr));
    new_par_path[strlen(new_path) - strlen(new_ptr)] = '\0';

    printf("new path = %s, new parent path = %s\n", new_path, new_par_path);

    // new_ptr = "/new_node_name"
    char *new_node_name = strtok(new_ptr, "/");
   
    // special case for root
    if(strcmp(new_par_path, "") == 0) {
        strcpy(new_par_path, "/");
    }

    new_par_id = l2_namei(fs, new_par_path);
    
    INode new_par_inode;
    if(readINode(fs, new_par_id, &new_par_inode) == -1) {
        fprintf(stderr, "Error: fail to read new parent directory inode %d\n", new_par_id);
        return -1;
    }

    // insert new directory entry into parent directory list
    DirEntry newEntry;
    strcpy(newEntry.key, new_node_name);
    newEntry.INodeID = node_id;
    printf("node name = %s, node_id = %d\n", newEntry.key, newEntry.INodeID);

    UINT j;
    for(j = 0; j < new_par_inode._in_filesize; j += sizeof(DirEntry)) {
        // search parent directory table
        DirEntry parEntry;
        readINodeData(fs, &new_par_inode, (BYTE*) &parEntry, j, sizeof(DirEntry));
        
        // empty directory entry found, overwrite it
        if (parEntry.INodeID == -1){
            break;
        }
    }
    
    INT bytesWritten = writeINodeData(fs, &new_par_inode, (BYTE*) &newEntry, j, sizeof(DirEntry));
    
    printf("byteswritten = %d, and afterwards inode table size = %d\n", bytesWritten, new_par_inode._in_filesize);
    if(bytesWritten != sizeof(DirEntry)) {
        fprintf(stderr, "Error: failed to write new entry into parent directory!\n");
        return -1;
    }

    // update parent directory file size, if it changed
    if(j + bytesWritten > new_par_inode._in_filesize) {
        new_par_inode._in_filesize = j + bytesWritten;
        writeINode(fs, new_par_id, &new_par_inode);
    }

    // update the disk inode
    writeINode(fs, new_par_id, &new_par_inode);
	
    return 0;
}

//1. resolve path and read inode
//2. check uid/gid
//3. set uid/gid and write inode
INT l2_chown(FileSystem *fs, char *path, uid_t uid, gid_t gid)
{
    //1. resolve path
    INT INodeID = l2_namei(fs, path);
    if (INodeID < 0) {
        _err_last = _fs_NonExistFile;
        THROW(__FILE__, __LINE__, __func__);
        return INodeID;
    }
    INode curINode;
    if(readINode(fs, INodeID, &curINode) == -1) {
        fprintf(stderr, "Error: fail to read inode for file %s\n", path);
        return -1;
    }
    //2. check uid/gid
    //3. set owner
    curINode._in_uid = uid;
    curINode._in_gid = gid;
    writeINode(fs, INodeID, &curINode);
    return 0;
}

//1. resolve path and read inode
//2. check uid/gid
//3. set mode and write inode
INT l2_chmod(FileSystem* fs, char* path, UINT mode)
{
    //1. resolve path
    INT INodeID = l2_namei(fs, path);
    if (INodeID < 0) {
	_err_last = _fs_NonExistFile;
	THROW(__FILE__, __LINE__, __func__);
	return INodeID;
    }
    INode curINode;
    if(readINode(fs, INodeID, &curINode) == -1) {
        fprintf(stderr, "Error: fail to read inode for file %s\n", path);
        return -1;
    }
    printf("cur mode: %x\n", curINode._in_permissions);
    //2. check uid/gid
    //3. set mode
    curINode._in_permissions = mode;
    writeINode(fs, INodeID, &curINode);
    return 0;
}

// truncate a file
INT l2_truncate(FileSystem* fs, char* path, INT new_length) {
    // namei to find the inode
    // compare the filesize with new-length
    // if new_length > filesize, extend the file, balloc
    // else if they are equal, do nothing
    // else, truncate the file, free the data blocks and blocks in the inodes
#ifdef TRUNCATE_DEBUG
        printf("Enter l2_truncate, new file to size %d\n", new_length);
#endif
    
    UINT INodeID = l2_namei(fs, path);
    if (INodeID< 0) {
	_err_last = _fs_NonExistFile;
	THROW(__FILE__, __LINE__, __func__);
	return INodeID;
    }
    INode curINode;
    if(readINode(fs, INodeID, &curINode) == -1) {
        fprintf(stderr, "Error: fail to read inode for file %s\n", path);
        return -1;
    }

    if (new_length > curINode._in_filesize) {
#ifdef TRUNCATE_DEBUG
        printf("in l2_truncate, start extend the file to size %d\n", new_length);
#endif
        // extend the file
        // len of bytes to be extended
        INT len = new_length - curINode._in_filesize;
        // next file blk to be allocated
        UINT fileBlkId = (curINode._in_filesize - 1 + BLK_SIZE) / BLK_SIZE;

        // special case, extend the filelength without allocating a new blk
        if ((new_length - 1 + BLK_SIZE) / BLK_SIZE == (curINode._in_filesize -1 + BLK_SIZE) / BLK_SIZE) {
        //    curINode._in_filesize = new_length;
            len = 0;
        }
        else {
            len -= fileBlkId * BLK_SIZE - curINode._in_filesize;
        //    fileBlkId ++;
        }

        INT dataBlkId;
        while (len > 0 && fileBlkId < MAX_FILE_BLKS)  {
            dataBlkId = balloc(fs, &curINode, fileBlkId);
#ifdef TRUNCATE_DEBUG
            printf("fileblk %d is extended! \n", fileBlkId);
#endif
            if(dataBlkId < 0) {
                printf("Warning: could not allocate more data blocks for truncate!\n");
                return -1;
            }

            if(len <= BLK_SIZE) {
                len = 0;
            }
            else {

                len -= BLK_SIZE;
                fileBlkId ++;
            }
        }
    }
    else if (new_length == curINode._in_filesize) {
        // do nothing
        ;
    }
    else {
#ifdef TRUNCATE_DEBUG
        printf("in l2_truncate, start truncate the file to size %d\n", new_length);
#endif
        // truncate it
        // len of bytes to be truncated
        INT len = curINode._in_filesize - new_length;
        // next file blk to be truncated
        UINT fileBlkId = (curINode._in_filesize -1 + BLK_SIZE) / BLK_SIZE - 1;
#ifdef TRUNCATE_DEBUG
        printf("the first fileblk to be truncated: %d\n", fileBlkId);
#endif
        if ((new_length - 1 + BLK_SIZE) / BLK_SIZE == (curINode._in_filesize -1 + BLK_SIZE) / BLK_SIZE) {
        //    curINode._in_filesize = new_length;
            len = 0;
        }
        else {
            len -= curINode._in_filesize - fileBlkId * BLK_SIZE;
            bfree(fs, &curINode, fileBlkId);
#ifdef TRUNCATE_DEBUG
            printf("fileblk %d is truncated! \n", fileBlkId);
#endif
            fileBlkId --;
        }
        
        while (len > 0 && fileBlkId >= 0)  {

            if(len <BLK_SIZE) {
                len = 0;
            }
            else {
                bfree(fs, &curINode, fileBlkId);
#ifdef TRUNCATE_DEBUG
                printf("fileblk %d is truncated! \n", fileBlkId);
#endif
                len -= BLK_SIZE;
                fileBlkId --;
            }
        }
    }

    curINode._in_filesize = new_length;
    if(writeINode(fs, INodeID, &curINode) == -1) {
        fprintf(stderr, "Error: fail to write inode for file %s\n", path);
        return -1;
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
  if (curINodeID < 0) {
    _err_last = _fs_NonExistFile;
    THROW(__FILE__, __LINE__, __func__);
    return curINodeID;
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
  #ifdef DEBUG
  printf("l2_read successfully read %d bytes\n", returnSize);
  #endif
  return returnSize; 
}

//write file from offset for numBytes
//1. resolve path
//2. get INodeTable Entry
//3. load inode
//4. call writeINodeData
//5. modify inode if necessary
INT l2_write(FileSystem* fs, char* path, UINT offset, BYTE* buf, UINT numBytes) {
  INode curINode;
  INT bytesWritten = 0;
  //1. resolve path
  INT curINodeID = l2_namei(fs, path);
  if (curINodeID < 0) {
    _err_last = _fs_NonExistFile;
    THROW(__FILE__, __LINE__, __func__);
    return curINodeID;
  }
  else {
    //2. TODO: get INodeTable Entry
    //3. load inode
    readINode(fs, curINodeID, &curINode);
    //4. writeINodeData
    bytesWritten = writeINodeData(fs, &curINode, buf, offset, numBytes);
    printf("bytesWritten: %d\n", bytesWritten);
    //5. modify inode
    if(offset + bytesWritten > curINode._in_filesize) {
        curINode._in_filesize = offset + bytesWritten;
        printf("update filesize to be %d\n", curINode._in_filesize);
    }
    curINode._in_modtime = time(NULL);
    //update INode
    writeINode(fs, curINodeID, &curINode);
  }
  #ifdef DEBUG
  printf("l2_write successfully wrote %d bytes\n", bytesWritten);
  #endif
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
  if (curINodeID < 0) {
    _err_last = _fs_NonExistFile;
    THROW(__FILE__, __LINE__, __func__);
    return curINodeID;
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
  #ifdef DEBUG
  printf("Resolving path \"%s\" using namei...\n", path);
  #endif
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
      return -ENOTDIR;
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
      if (strcmp(tok, DEntry->key) == 0 && DEntry->INodeID != -1) {
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
      return -ENOENT;
    }
    //1. advance in traversal
    tok = strtok(NULL, "/");
  }
  #ifdef DEBUG
  printf("Namei succeeded with id: %d\n", curID);
  #endif
  return curID;
}

