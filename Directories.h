/*
 * This is the virtual directory interface header.  It defines the basic functions for
 * manipulating files and directories
 * by Jon
 */

#include "Globals.h"

// makes a new directory
UINT mkdir(char* path);

// makes a new file
UINT mknod(char* path);

// reads directory contents
//UINT readdir(Dir*, DFile*);

// deletes a file or directory
UINT unlink(char* path);

// opens a file
UINT open(char* path);

// closes a file
UINT close(char* path);

// reads a file
UINT read(char* path, UINT offset, BYTE* buf, UINT numBytes);

// writes to a file
UINT write(char* path, UINT offset, BYTE* buf, UINT numBytes);
