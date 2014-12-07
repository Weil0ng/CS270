/*
 * This is the inode structure
 */

#pragma once
#include "Globals.h"
#include "time.h"
#include "string.h"
#include "sys/types.h"

enum FILE_TYPE {
        FREE = -1,
        INIT = 1,
        REGULAR = 2,
        DIRECTORY = 3,
	FIFO = 4,
        CHAR = 5,
	BLOCK = 6
};

typedef struct INode {

	//disk fields

	enum FILE_TYPE _in_type;

	char _in_owner[INODE_OWNER_NAME_LEN];
       
//uncomment these will induce crash
	uid_t _in_uid;

	gid_t _in_gid;

	UINT _in_permissions;

	LONG _in_modtime;

	LONG _in_accesstime;

	LONG _in_filesize;
    
        UINT _in_linkcount;

	LONG _in_directBlocks[INODE_NUM_DIRECT_BLKS];

	LONG _in_sIndirectBlocks[INODE_NUM_S_INDIRECT_BLKS];

	LONG _in_dIndirectBlocks[INODE_NUM_D_INDIRECT_BLKS];

        LONG _in_tIndirectBlocks[INODE_NUM_T_INDIRECT_BLKS];

} INode;

UINT initializeINode(INode*, UINT);

// prints an inode out for debugging
void printINode(INode*);
