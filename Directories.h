/*
 * This is the virtual directory interface header.  It defines the basic functions for
 * manipulating files and directories
 * by Jon
 */

#include "GlobalParams.h"

// makes a new directory
void mkdir(char* path);

// makes a new file
void mknod(char* path);

// reads directory contents
Dir* readdir(char* path);

// deletes a file or directory
void unlink(char* path);

// opens a file. returning the descriptor
UINT open(char* path);

// closes a file
void close(UINT);

// reads a file
UINT read(UINT, byte*, UINT);

// writes to a file
void write(UINT, byte*, UINT);
