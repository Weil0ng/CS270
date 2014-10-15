/*
 * This is the file where all global params of the file system are defined.
 */

#include <stdint.h>

//Architecture specific
#define UINT uint32_t
#define BOOL bool
#define BYTE uint8_t


//File system specific
#define BLK_SIZE (512)    //Block size in bytes
#define INODE_SIZE (256)  //INode size in bytes, 256 = 
