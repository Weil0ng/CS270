/**
 * FileSystem implementation
 * by Jon
 */

#include "FileSystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

UINT makefs(UINT nDBlks, UINT nINodes, FileSystem* fs) {
    #ifdef DEBUG 
    printf("makefs(%d, %d, %p)\n", nDBlks, nINodes, (void*) fs); 
    #endif
	
    //validate file system parameters
    #ifndef DEBUG
    if(nDBlks <= 0 || nINodes <= 0) {
        printf("Error: must have a positive number of data blocks/inodes!\n");
        return 1;
    }
    else if(nDBlks < FREE_DBLK_CACHE_SIZE) {
        printf("Error: must have at least %d data blocks!\n", FREE_DBLK_CACHE_SIZE);
        return 1;
    }
    else if(nINodes < FREE_INODE_CACHE_SIZE) {
        printf("Error: must have at least %d inodes!\n", FREE_INODE_CACHE_SIZE);
        return 1;
    }
    else if(nINodes % INODES_PER_BLK != 0) {
        printf("Error: inodes must divide evenly into blocks!\n");
        return 1;
    }
    #endif

    //compute file system size 
    UINT nBytes = BLK_SIZE + nINodes * INODE_SIZE + nDBlks * BLK_SIZE;
    #ifdef DEBUG 
    printf("nBytes = %d\n", nBytes); 
    #endif
    if(nBytes > MAX_FS_SIZE) {
        printf("Error: file system size %d exceeds max allowed size %d!\n", nBytes, MAX_FS_SIZE);
        return 1;
    }
    fs->nBytes = (BLK_SIZE + nINodes * INODE_SIZE + nDBlks * BLK_SIZE);
    
    //compute offsets for inode/data blocks
    fs->diskINodeBlkOffset = 1;
    fs->diskDBlkOffset = fs->diskINodeBlkOffset + nINodes / INODES_PER_BLK;

    //initialize in-memory superblock
    #ifdef DEBUG 
    printf("Initializing in-memory superblock...\n"); 
    #endif
    fs->superblock.nDBlks = nDBlks;
    fs->superblock.nFreeDBlks = nDBlks;
    fs->superblock.pFreeDBlksHead = 0;
    fs->superblock.pNextFreeDBlk = FREE_DBLK_CACHE_SIZE - 1;
    
    fs->superblock.nINodes = nINodes;
    fs->superblock.nFreeINodes = nINodes;
    fs->superblock.pNextFreeINode = FREE_INODE_CACHE_SIZE - 1;

    fs->superblock.modified = true;

    //initialize superblock free inode cache
    #ifdef DEBUG 
    printf("Initializing superblock free inode cache...\n");
    #endif
    for(UINT i = 0; i < FREE_INODE_CACHE_SIZE; i++) {
        fs->superblock.freeINodeCache[i] = i;
    }
    
    //initialize the in-memory disk
    #ifdef DEBUG 
    printf("Initializing in-memory disk emulator...\n"); 
    #endif
    fs->disk = malloc(sizeof(DiskArray));
    initDisk(fs->disk, fs->nBytes);

    //create inode list on disk
    #ifdef DEBUG 
    printf("Creating disk inode list...\n"); 
    #endif
    UINT nextINodeId = 0;
    UINT nextINodeBlkId = fs->diskINodeBlkOffset;
    BYTE nextINodeBlkBuf[INODES_PER_BLK * INODE_SIZE] = {}; //note that this zeroes out the buffer

    while(nextINodeBlkId < fs->diskDBlkOffset) {
        //fill inode blocks one at a time
        for(UINT i = 0; i < INODES_PER_BLK; i++) {
            INode* inode = (INode*) &nextINodeBlkBuf[i * INODE_SIZE];
            inode->_in_id = nextINodeId++;
            inode->_in_type = FREE;
        }

        writeBlk(fs->disk, nextINodeBlkId, nextINodeBlkBuf);

        nextINodeBlkId++;
    }

    //create free block list on disk
    #ifdef DEBUG 
    printf("Creating disk free block list...\n"); 
    #endif
    UINT nextListBlk = 0;
    UINT freeDBlkList[FREE_DBLK_CACHE_SIZE];
    
    while(nextListBlk < fs->superblock.nDBlks) {
        //special case: next head pointer goes in first entry
        freeDBlkList[0] = nextListBlk + FREE_DBLK_CACHE_SIZE;

        //rest of free blocks are enumerated in order
        for(UINT i = 1; i < FREE_DBLK_CACHE_SIZE; i++) {
            freeDBlkList[i] = nextListBlk + i;
        }

        //write completed block and advance to next head
        writeDBlk(fs, nextListBlk, (BYTE*) freeDBlkList);
        nextListBlk += FREE_DBLK_CACHE_SIZE;
    }
    
    //load free block cache into superblock
    readDBlk(fs, fs->superblock.pFreeDBlksHead, fs->superblock.freeDBlkCache);

    //write superblock to disk
    #ifdef DEBUG 
    printf("Writing superblock to disk...\n"); 
    #endif
    BYTE superblockBuf[BLK_SIZE];
    blockify(&fs->superblock, superblockBuf);
    writeBlk(fs->disk, SUPERBLOCK_OFFSET, superblockBuf);
    fs->superblock.modified = false;

    return 0;
}

UINT destroyfs(FileSystem* fs) {
    destroyDisk(fs->disk);
    return 0;
}

UINT initializeINode(INode *inode){
    inode->_in_type = REGULAR;
    //FIXME: shall we make owner string?
    inode->_in_owner = 0;
    inode->_in_permissions = 777;
    //TODO: get time
    //inode->_in_modtime = get_time();
    //inode->_in_accesstime = get_time();
    inode->_in_filesize = 0;

    //since we are storing logical data blk id, 0 could be a valid blk
    for (UINT i = 0; i < INODE_NUM_DIRECT_BLKS; i ++) {
        inode->_in_directBlocks[i] = -1;
    }
    for (UINT i = 0; i < INODE_NUM_S_INDIRECT_BLKS; i ++) {
        inode->_in_sIndirectBlocks[i] = -1;
    }
    for (UINT i = 0; i < INODE_NUM_D_INDIRECT_BLKS; i ++) {
        inode->_in_dIndirectBlocks[i] = -1;
    }

    return 0;
}

//input: none
//output: INode id
//function: allocate a free inode
UINT allocINode(FileSystem* fs, INode* inode) {

    if(fs->superblock.nFreeINodes == 0) {
        fprintf(stderr, "error: no more free inodes available!\n");
        return -1;
    }

    UINT nextFreeINodeID;

    // the inode cache is empty
    if(fs->superblock.pNextFreeINode < 0) {
        
        // scan inode list and fill the inode cache list to capacity
        UINT nextINodeBlk = fs->diskINodeBlkOffset;
        BYTE nextINodeBlkBuf[BLK_SIZE];
        
        UINT k = 0; // index in inode cache 
        BOOL FULL = false;
        while(nextINodeBlk < fs->diskDBlkOffset && !FULL)  {
            
            // read the whole block out into a byte array
            readBlk(fs->disk, nextINodeBlk, nextINodeBlkBuf);
            
            // covert the byte array to INode structures
            INode* inode_s = (INode*) nextINodeBlkBuf; 
            
            // check inodes one at a time
            for(UINT i = 0; i < INODES_PER_BLK && !FULL; i++) {
                
                //move to the destination inode
                INode* inode_d = inode_s + i;

                // found a free inode
                if(inode_d->_in_type == FREE) {
                    fs->superblock.freeINodeCache[k] = (nextINodeBlk - fs->diskINodeBlkOffset) * INODES_PER_BLK + i;
                    k ++;

                    // inode cache full
                    if(k == FREE_INODE_CACHE_SIZE) {
                        FULL = true;
                        fs->superblock.pNextFreeINode = FREE_INODE_CACHE_SIZE - 1;
                    }
                }
            }
            nextINodeBlk ++;
        }
    }
   
    // the inode cache not emply, allocate one inode from the cache
    nextFreeINodeID = fs->superblock.freeINodeCache[fs->superblock.pNextFreeINode]; 

    // initialize the inode
    initializeINode(inode);

    // write the inode back to disk
    if(writeINode(fs, nextFreeINodeID, inode) == -1){
        fprintf(stderr, "error: write inode %d to disk\n", nextFreeINodeID);
        return -1;
    }
    
    // update the inode cache list
    fs->superblock.pNextFreeINode --;

    // decrement the free inodes count
    fs->superblock.nFreeINodes --;

    return nextFreeINodeID;

}

// input: inode number
// output: none
// function: free a inode, which updates the inode cache and/or inode table
UINT freeINode(FileSystem* fs, UINT id) {

    // if inode cache not full, store the inode number in the list
    if (fs->superblock.pNextFreeINode < FREE_INODE_CACHE_SIZE - 1){
        fs->superblock.freeINodeCache[fs->superblock.pNextFreeINode + 1] = id;
        fs->superblock.pNextFreeINode ++;
    }

    // update the inode table to mark the inode free
    INode inode;
    inode._in_type = FREE;
    
    if(writeINode(fs, id, &inode) == -1){
        fprintf(stderr, "error: write inode %d to disk\n", id);
        return -1;
    }
    
    // increase file system free inode count
    fs->superblock.nFreeINodes ++;

    return 0;

}

// input: inode number
// output: the pointer to the inode
// function: read a disk inode
UINT readINode(FileSystem* fs, UINT id, INode* inode) {
    UINT blk_num = fs->diskINodeBlkOffset + id / INODES_PER_BLK;
    UINT blk_offset = id % INODES_PER_BLK;

    BYTE INodeBlkBuf[BLK_SIZE];
    if(readBlk(fs->disk, blk_num, INodeBlkBuf) == -1) {
        fprintf(stderr, "error: read blk %d from disk\n", blk_num);
        return -1;
    }

    // find the inode to read
    INode* inode_d = (INode*) (INodeBlkBuf + blk_offset * INODE_SIZE);
    
    // asssign struct field from the buffer to the inode
    inode->_in_type = inode_d->_in_type;
    inode->_in_owner = inode_d->_in_owner;
    inode->_in_permissions = inode_d->_in_permissions;
    inode->_in_modtime =  inode_d->_in_modtime;
    inode->_in_accesstime = inode_d->_in_accesstime;
    inode->_in_filesize = inode_d->_in_filesize;
    for (UINT i = 0; i < INODE_NUM_DIRECT_BLKS; i ++) {
        inode->_in_directBlocks[i] = inode_d->_in_directBlocks[i];
    }
    for (UINT i = 0; i < INODE_NUM_S_INDIRECT_BLKS; i ++) {
        inode->_in_sIndirectBlocks[i] = inode_d->_in_sIndirectBlocks[i];
    }
    for (UINT i = 0; i < INODE_NUM_D_INDIRECT_BLKS; i ++) {
        inode->_in_dIndirectBlocks[i] = inode_d->_in_dIndirectBlocks[i];
    }

    //TODO: set id to parameter id instead of reading it (just making sure disk read is working properly for now)
    inode->_in_id = inode_d->_in_id;

    //TODO: reset refcount instead of reading it (just making sure disk read is working properly for now)
    inode->_in_refcount = inode_d->_in_refcount;

    return 0;
}

// input: inode number id, an inode
// output: none
// function: write the disk inode #id in the inode table
UINT writeINode(FileSystem* fs, UINT id, INode* inode) {
    UINT blk_num = fs->diskINodeBlkOffset + id / INODES_PER_BLK;
    UINT blk_offset = id % INODES_PER_BLK;
    
    BYTE INodeBlkBuf[BLK_SIZE];
    if(readBlk(fs->disk, blk_num, INodeBlkBuf) == -1) {
        fprintf(stderr, "error: read blk %d from disk\n", blk_num);
        return -1;
    }
    
    INode* inode_s = (INode*) INodeBlkBuf; 
    // find the inode to write
    INode *inode_d = inode_s + blk_offset;

    // replace the inode
    inode_d->_in_type = inode->_in_type;
    inode_d->_in_owner = inode->_in_owner;
    inode_d->_in_permissions = inode->_in_permissions;
    inode_d->_in_modtime =  inode->_in_modtime;
    inode_d->_in_accesstime = inode->_in_accesstime;
    inode_d->_in_filesize = inode->_in_filesize;
    for (UINT i = 0; i < INODE_NUM_DIRECT_BLKS; i ++) {
        inode_d->_in_directBlocks[i] = inode->_in_directBlocks[i];
    }
    for (UINT i = 0; i < INODE_NUM_S_INDIRECT_BLKS; i ++) {
        inode_d->_in_sIndirectBlocks[i] = inode->_in_sIndirectBlocks[i];
    }
    for (UINT i = 0; i < INODE_NUM_D_INDIRECT_BLKS; i ++) {
        inode_d->_in_dIndirectBlocks[i] = inode->_in_dIndirectBlocks[i];
    }

    // write the entire inode block back to disk
    if(writeBlk(fs->disk, blk_num, INodeBlkBuf) == -1) {
        fprintf(stderr, "error: write blk %d from disk\n", blk_num);
        return -1;
    }
    
    return 0;
}

//Try to alloc a free data block from disk:
//1. check if there are free DBlk at all
//2. check the pNextFreeDBlk
//	if DBlk cache has free entry 
//		alloc that entry
//	else
//		alloc current free list blk
//		move pFreeDBlksHead to next list blk
//3. # free blocks --
//4. return the logical id of allocaed DBlk
UINT allocDBlk(FileSystem* fs) {
    //1. check full
    if (fs->superblock.nFreeDBlks == 0) {
        _err_last = _fs_DBlkOutOfNumber;
	THROW();
	return -1;
    }
    
    UINT returnID = -1;

    //2. check pNextFreeDBlk
    if (fs->superblock.pNextFreeDBlk != 0) {
        returnID = (fs->superblock.freeDBlkCache)[fs->superblock.pNextFreeDBlk];
        // mark it as allocated
        (fs->superblock.freeDBlkCache)[fs->superblock.pNextFreeDBlk] = -1;
    }
    else {
        //alloc this very block
        returnID = fs->superblock.pFreeDBlksHead;
	//move head
	fs->superblock.pFreeDBlksHead = (fs->superblock.freeDBlkCache)[0];
	//load cache
	readDBlk(fs, fs->superblock.pFreeDBlksHead, fs->superblock.freeDBlkCache);
	//move pNextFreeDBlk
	fs->superblock.pNextFreeDBlk = FREE_DBLK_CACHE_SIZE - 1;
    }

    fs->superblock.nFreeDBlks --;
    return returnID;
    //return 0;

}

// Try to insert a free DBlk back to free list
// 1. check pNextFreeDBlk
// 2.	if current cache full
// 		wrtie cache back
// 		use newly free block NFB as head free block list
// 	else
// 		insert to current cache
// 3. # Free DBlks ++
UINT freeDBlk(FileSystem* fs, UINT id) {
    // if current cache is full
    if (fs->superblock.pNextFreeDBlk == FREE_DBLK_CACHE_SIZE - 1) {
	//write cache back
	writeDBlk(fs, fs->superblock.pFreeDBlksHead, &(fs->superblock.freeDBlkCache));
	//init cache
	(fs->superblock.freeDBlkCache)[0] = fs->superblock.pFreeDBlksHead;
	for (UINT i=1; i<FREE_DBLK_CACHE_SIZE - 1; i++)
	    (fs->superblock.freeDBlkCache)[i] = -1;
	//move pNextFreeDBlk
	fs->superblock.pNextFreeDBlk = 0;
	//move head
	fs->superblock.pFreeDBlksHead = id;
    }
    else {
	fs->superblock.pNextFreeDBlk ++;
	(fs->superblock.freeDBlkCache)[fs->superblock.pNextFreeDBlk] = id;
    }
    fs->superblock.nFreeDBlks ++;
    return 0;
}

// Try to read out a block from disk
// 1. convert logical id of DBlk to logical id of disk block
// 2. read data
UINT readDBlk(FileSystem* fs, UINT id, BYTE* buf) {
    UINT bid = id + fs->diskDBlkOffset;
    return readBlk(fs->disk, bid, buf);

}

// writes a data block to the disk
// dBlkId: the data block logical id (not raw logical id!)
// buf: the buffer to write (must be exactly block-sized)
UINT writeDBlk(FileSystem* fs, UINT id, BYTE* buf) {
    UINT bid = id + fs->diskDBlkOffset;
    return writeBlk(fs->disk, bid, buf);
}

// input: inode, logic file byte offset
// output: logical data block #,
// function: block map of a logic file byte offset to a file system block.
// note that the coverted byte offset in this block, num of bytes in to read in the block
// can be easily calculated: i.e. off= offset % BLK_SIZE, len = BLK_SIZE - off
UINT bmap(FileSystem* fs, INode* inode, UINT offset, UINT* cvt_blk_num){

    UINT space_per_sInBlk; // the address range provided by one single indirect block
    UINT space_per_dInBlk; // the address range provdied by one double indirect block

    UINT direct_space;    // total address range provided by direct blocks
    UINT s_indirect_space; // total address range provided by single indirect blocks
    UINT d_indirect_space; // total address range provided by double indirect blocks

    space_per_sInBlk = (BLK_SIZE / 4) * BLK_SIZE;     
    space_per_dInBlk = (BLK_SIZE / 4) * (BLK_SIZE / 4) * BLK_SIZE; 

    direct_space = INODE_NUM_DIRECT_BLKS * BLK_SIZE; 
    s_indirect_space = INODE_NUM_S_INDIRECT_BLKS * space_per_sInBlk;
    d_indirect_space = INODE_NUM_D_INDIRECT_BLKS * space_per_dInBlk;

    if (offset < direct_space) {
        // look up in a direct block
        UINT dBlk_index = offset / BLK_SIZE;

        // read the direct block to find the data block #
        *cvt_blk_num = inode->_in_directBlocks[dBlk_index];
        printf("Found in a direct block, physical block # = %d\n", *cvt_blk_num);
    }
    else if (offset < s_indirect_space) {
        // look up in single indirect blocks
        BYTE readBuf[BLK_SIZE];

        // locate which indirect block to look up
        UINT sInBlks_index = (offset - direct_space) / space_per_sInBlk;

        // read the indirect block that contains a list of direct blocks 
        readDBlk(fs, inode->_in_sIndirectBlocks[sInBlks_index], readBuf);

        // locate which direct block to look up
        UINT dBlk_index = (offset - direct_space - sInBlks_index * space_per_sInBlk) / BLK_SIZE;

        *cvt_blk_num = readBuf[dBlk_index];
        printf("Found in a single indirect block, physical block # = %d\n", *cvt_blk_num);
    }
    else if (offset < d_indirect_space){
        // look up in double indirect blocks
        BYTE readBuf_d[BLK_SIZE]; // buffer to store the double indirect block
        BYTE readBuf_s[BLK_SIZE]; // buffer to store the single indirect block

        // locate which double indirect block to look up
        UINT dInBlks_index = (offset - direct_space - s_indirect_space) / space_per_dInBlk;
        
        // read the double indirect block which contains a list of single indirect blocks
        readDBlk(fs, inode->_in_dIndirectBlocks[dInBlks_index], readBuf_d);

        // locate which single indirect block to look up
        UINT sInBlks_index = (offset - direct_space - s_indirect_space - dInBlks_index * space_per_dInBlk) / space_per_sInBlk;

        // read the single indirect block which contains a list of direct blocks
        readDBlk(fs, readBuf_d[sInBlks_index], readBuf_s);

        // locate which direct block to look up
        UINT dBlk_index = (offset - direct_space - s_indirect_space - dInBlks_index * space_per_dInBlk - sInBlks_index * space_per_sInBlk) / BLK_SIZE;
        
        *cvt_blk_num = readBuf_d[dBlk_index];
        printf("Found in a double indirect block, physical block # = %d\n", *cvt_blk_num);
    }
    else {
        fprintf(stderr, "bmap fail: out of inode address space!\n");
        return 1;
    }
    return 0;
}

#ifdef DEBUG
void printINodes(FileSystem* fs) {
    for(UINT i = 0; i < fs->superblock.nINodes; i++) {
        INode inode;
        readINode(fs, i, &inode);
        printINode(&inode);
    }
}
#endif

#ifdef DEBUG
void printDBlks(FileSystem* fs) {
    for(UINT i = 0; i < fs->superblock.nDBlks; i++) {
        BYTE buf[BLK_SIZE];
        readDBlk(fs, i, buf);
        printf("%d\t| ", i);
        for(UINT k = 0; k < BLK_SIZE; k+=4) {
            printf("%d ", buf[k]);
        }
        printf("\n");
    }
}
#endif