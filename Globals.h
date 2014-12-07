/*
 * * This is the file where all global params of the file system are defined.
 * */
#include <stdbool.h>
#include <stdint.h>

#ifndef DEBUG
//#define DEBUG
#endif

#ifndef DEBUG_VERBOSE
//#define DEBUG_VERBOSE
#endif

//disk partition path
#define DISK_PATH "diskFile"

//Architecture specific
#define UINT uint32_t
#define INT  int32_t
#define LONG uint64_t
#define BOOL bool
#define BYTE uint8_t

//File system specific
#define MAX_FS_SIZE (1099511627776) //Maximum supported filesystem size (1TB)

#define BLK_SIZE (2048) //Block size in bytes
#define INODE_SIZE (256) //INode size in bytes
#define INODES_PER_BLK (BLK_SIZE / INODE_SIZE) //INodes per block (computed)

#define SUPERBLOCK_OFFSET (0) //superblock id, default 0

#define FREE_DBLK_CACHE_SIZE (BLK_SIZE / sizeof(INT)) //In-memory free block cache size, 1 block of ints
#define FREE_INODE_CACHE_SIZE (4) //In-memory inode cache size, 100 = 400 bytes of superblock

#define INODE_OWNER_NAME_LEN (10) // number of characters of the owner name 
#define INODE_NUM_DIRECT_BLKS (20) // number of direct blocks per inode 
#define INODE_NUM_S_INDIRECT_BLKS (1) // number of single direct blocks per inode 
#define INODE_NUM_D_INDIRECT_BLKS (1) // number of double direct blocks per inode 
#define INODE_NUM_T_INDIRECT_BLKS (1) // number of triple direct blocks per inode 

#define OPEN_FILE_TABLE_LENGTH (1024)   //length of open file table
#define INODE_TABLE_LENGTH (1024)   //number of bins in the hash queue of in core INodeTable
#define INODE_CACHE_LENGTH (1024)   //number of inodes to keep in cache queue (cached inodes with 0 refcount)
#define FILE_NAME_LENGTH (256)      //number of bytes in the file name in bytes

#define MAX_PATH_LEN (512) //maximum length of the path
#define MAX_FILE_SIZE (BLK_SIZE * INODE_NUM_DIRECT_BLKS + BLK_SIZE * INODE_NUM_S_INDIRECT_BLKS * (BLK_SIZE / sizeof(UINT)) + BLK_SIZE * INODE_NUM_D_INDIRECT_BLKS * (BLK_SIZE / sizeof(UINT)) * (BLK_SIZE / sizeof(UINT)) + BLK_SIZE * INODE_NUM_T_INDIRECT_BLKS * (BLK_SIZE / sizeof(UINT)) * (BLK_SIZE / sizeof(UINT)) * (BLK_SIZE / sizeof(UINT))) 
#define MAX_FILE_BLKS (MAX_FILE_SIZE / BLK_SIZE) //max number of data blocks allocatable per file
#define MAX_FILE_NUM_IN_DIR (MAX_FILE_SIZE / (FILE_NAME_LENGTH + sizeof(INT))) //maximum number of files in a directory
//#define MAX_FILE_NUM_IN_DIR 10 //maximum number of files in a directory
#define MAX_DIR_TABLE_SIZE (MAX_FILE_NUM_IN_DIR * (FILE_NAME_LENGTH + sizeof(INT)))
