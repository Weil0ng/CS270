/*
 * This is the virtual directory interface header.  It defines the basic functions for
 * manipulating files and directories
 * by Jon
 */

#include "Globals.h"
#include "Directory.h"
#include "FileSystem.h"

// makes a new directory
UINT mkdir(FileSystem*, char*);

// makes a new file
UINT mknod(FileSystem*, char*);

// reads directory contents
//UINT readdir(Dir*, DFile*);

// deletes a file or directory
UINT unlink(FileSystem*, char*);

// opens a file
UINT open(FileSystem*, char*);

// closes a file
UINT close(FileSystem*, char*);

// reads a file
UINT read(FileSystem*, char*, UINT, BYTE*, UINT);

// writes to a file
UINT write(FileSystem*, char*, UINT, BYTE*, UINT);

//resolve path to inode id
UINT namei(FileSystem *, char *);
