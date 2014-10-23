/*
 * This is the inode structure
 */

#include "Globals.h"
//#include "FileSystem.h"

enum FILE_TYPE {
        FREE,
        REGULAR,
        DIRECTORY
};

typedef struct INode {

	//disk fields

	UINT _in_owner;

	enum FILE_TYPE _in_type;

	UINT _in_permissions;

	LONG _in_modtime;

	LONG _in_accesstime;

	UINT _in_filesize;

	INT _in_directBlocks[INODE_NUM_DIRECT_BLKS];

	INT _in_sIndirectBlocks[INODE_NUM_S_INDIRECT_BLKS];

	INT _in_dIndirectBlocks[INODE_NUM_D_INDIRECT_BLKS];

	//memory fields
        
        //UINT _in_status; // locked/modified/mount_point/

	UINT _in_id;

	//Lock* _in_lock;

	UINT _in_refcount;

} INode;

