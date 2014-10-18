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
UINT readdir(Dir*, DFile*);

// deletes a file or directory
UINT unlink(char* path);

// opens a file. returning the descriptor
UINT open(char* path);

// closes a file
// params: file descriptor
UINT close(UINT);

// reads a file
// params: file descriptor, buffer, number of bytes
UINT read(UINT, byte*, UINT);

// writes to a file
// params: file descriptor, buffer, number of bytes
UINT write(UINT, byte*, UINT);
