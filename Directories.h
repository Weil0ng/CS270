/*
 * This is the virtual directory interface header.  It defines the basic functions for
 * manipulating files and directories
 * by Jon
 */

#include "Directory.h"
#include "FileSystem.h"
#include "sys/stat.h"

// makes a new filesystem with a root directory
UINT l2_initfs(UINT nDBlks, UINT nINodes, FileSystem* fs);

// getattr
UINT l2_getattr(FileSystem* fs, char *path, struct stat *stbuf);

// makes a new directory
UINT l2_mkdir(FileSystem* fs, char* path);

// makes a new file
UINT l2_mknod(FileSystem* fs, char* path);

// reads directory contents
UINT l2_readdir(FileSystem* fs, char* path, char** namelist);
//UINT readdir(Dir*, DFile*);

// deletes a file or directory
UINT l2_unlink(FileSystem* fs, char* path);

// opens a file
UINT l2_open(FileSystem* fs, char* path);

// closes a file
UINT l2_close(FileSystem* fs, char* path);

// reads a file
UINT l2_read(FileSystem* fs, char* path, UINT offset, BYTE* buf, UINT numBytes);

// writes to a file
UINT l2_write(FileSystem* fs, char* path, UINT offset, BYTE* buf, UINT numBytes);

//resolve path to inode id
UINT l2_namei(FileSystem *, char *);
