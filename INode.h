/*
 * This is the inode structure
 */

#include "Globals.h"

typedef struct INode {

	//disk fields

	UINT _in_owner;

	UINT _in_type;

	UINT _in_permissions;

	LONG _in_modtime;

	LONG _in_accesstime;

	UINT _in_filesize;

	UINT _in_directBlocks[INODE_NUM_DIRECT_BLKS];

	//UINT* _in_sIndirectBlocks[INODE_NUM_SINDIRECT_BLKS];

	//UINT* _in_dIndirectBlocks[INODE_NUM_DINDIRECT_BLKS];

	//memory fields

	UINT _in_id;

	//Lock* _in_lock;

	UINT _in_refcount;

} INode;

// converts file byte offset in inode to logical block ID
UINT bmap(INode* inode, UINT offset);
