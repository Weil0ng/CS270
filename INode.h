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

	UINT _in_dblocks[INODE_NUM_DBLOCKS];

	//UINT* _in_siblocks[INODE_NUM_SIBLOCKS];

	//UINT* _in_diblocks[INODE_NUM_DIBLOCKS];

	//memory fields

	UINT _in_id;

	//Lock* _in_lock;

	UINT _in_refcount;

} INode;

// converts file byte offset in inode to logical block ID
UINT bmap(INode* inode, UINT offset);
