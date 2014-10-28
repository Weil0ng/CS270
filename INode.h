/*
 * This is the inode structure
 */

#include "Globals.h"

enum FILE_TYPE {
        FREE = -1,
        INIT = 1,
        REGULAR = 2,
        DIRECTORY = 3
};

typedef struct INode {

	//disk fields

	enum FILE_TYPE _in_type;

	UINT _in_owner;

	UINT _in_permissions;

	LONG _in_modtime;

	LONG _in_accesstime;

	UINT _in_filesize;

	UINT _in_directBlocks[INODE_NUM_DIRECT_BLKS];

	UINT _in_sIndirectBlocks[INODE_NUM_S_INDIRECT_BLKS];

	UINT _in_dIndirectBlocks[INODE_NUM_D_INDIRECT_BLKS];

	//memory fields
        
        //UINT _in_status; // locked/modified/mount_point/

	UINT _in_id;

	//Lock* _in_lock;

	UINT _in_refcount;

} INode;

// prints an inode out for debugging
void printINode(INode*);
