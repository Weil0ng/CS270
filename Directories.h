/*
 * This is the virtual directory interface header.  It defines the basic functions for
 * manipulating files and directories
 * by Jon
 */

#include "Globals.h"
#include "Directory.h"
#include "FileSystem.h"

// makes a new directory
UINT mkdir(FileSystem* fs, char* path);

// makes a new file
UINT mknod(FileSystem* fs, char* path);

// reads directory contents
//UINT readdir(Dir*, DFile*);

// deletes a file or directory
UINT unlink(FileSystem* fs, char* path);

// opens a file
UINT open(char* path);

// closes a file
UINT close(char* path);

// reads a file
UINT read(FileSystem* fs, char* path, UINT offset, BYTE* buf, UINT numBytes);

// writes to a file
UINT write(FileSystem* fs, char* path, UINT offset, BYTE* buf, UINT numBytes);

//resolve path to inode id
UINT namei(FileSystem *, char *);
