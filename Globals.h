/*
 * * This is the file where all global params of the file system are defined.
 * */
#include <stdint.h>

//Architecture specific
#define UINT uint32_t
#define LONG uint64_t
#define BOOL bool
#define BYTE uint8_t

//File system specific
#define BLK_SIZE (512) //Block size in bytes
#define INODE_SIZE (256) //INode size in bytes, 256 = 
#define NUM_INODES (128) // total numbe of inodes in the file system
#define INODE_NUM_DIRECT_BLKS (10) // number of direct blocks per inode 
#define INODE_NUM_S_INDIRECT_BLKS (1) // number of single direct blocks per inode 
#define INODE_NUM_D_INDIRECT_BLKS (1) // number of double direct blocks per inode 
