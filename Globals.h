/*
 * * This is the file where all global params of the file system are defined.
 * */
#include <stdbool.h>
#include <stdint.h>

#ifndef DEBUG
#define DEBUG
#endif

//Architecture specific
#define UINT uint32_t
#define INT  int32_t
#define LONG uint64_t
#define BOOL bool
#define BYTE uint8_t

//File system specific
#define MAX_FS_SIZE (4294967296) //Maximum supported filesystem size (2^32 bytes)

#define BLK_SIZE (512) //Block size in bytes
#define INODE_SIZE (256) //INode size in bytes
#define INODES_PER_BLK (BLK_SIZE / INODE_SIZE) //INodes per block (computed)

#define SUPERBLOCK_OFFSET (0) //superblock id, default 0

#define FREE_DBLK_CACHE_SIZE (BLK_SIZE / sizeof(UINT)) //In-memory free block cache size, 1 block of ints
#define FREE_INODE_CACHE_SIZE (4) //In-memory inode cache size, 100 = 400 bytes of superblock

#define INODE_NUM_DIRECT_BLKS (10) // number of direct blocks per inode 
#define INODE_NUM_S_INDIRECT_BLKS (1) // number of single direct blocks per inode 
#define INODE_NUM_D_INDIRECT_BLKS (1) // number of double direct blocks per inode 

#define INODE_TABLE_LENGTH (1024)   //number of bins in the hash queue of in core INodeTable
