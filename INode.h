/*
 * This is the inode structure
 */

#include "Globals.h"

typedef struct INode {

	//disk fields

	UINT owner;

	UINT type;

	UINT permissions;

	UINT modtime;

	UINT accesstime;

	UINT filesize;

	UINT* dblocks;

	UINT* siblocks;

	UINT* diblocks;

	//memory fields

	UINT id;

	Lock* lock;

	UINT refcount;

} INode;

// converts file byte offset in inode to logical block ID
UINT bmap(INode* inode, UINT offset);
