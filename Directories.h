/*
 * This is the virtual directory interface header.  It defines the basic functions for
 * manipulating files and directories
 * by Jon
 */

#include "Directory.h"
#include "FileSystem.h"
#include "sys/stat.h"
#include "sys/types.h"

// mounts a filesystem from a device
INT l2_mount(FileSystem* fs);

// unmounts a filesystem into a device
INT l2_unmount(FileSystem* fs);

// makes a new filesystem with a root directory
INT l2_initfs(UINT nDBlks, UINT nINodes, FileSystem* fs);

// getattr
INT l2_getattr(FileSystem* fs, char *path, struct stat *stbuf);

// makes a new directory
INT l2_mkdir(FileSystem* fs, char* path, uid_t uid, gid_t gid);

// makes a new file
INT l2_mknod(FileSystem* fs, char* path, uid_t uid, gid_t gid);

// reads directory contents
INT l2_readdir(FileSystem* fs, char* path, char namelist[][FILE_NAME_LENGTH]);
//UINT readdir(Dir*, DFile*);

// deletes a file or directory
INT l2_unlink(FileSystem* fs, char* path);

// opens a file
INT l2_open(FileSystem* fs, char* path);

// closes a file
INT l2_close(FileSystem* fs, char* path);

// reads a file
INT l2_read(FileSystem* fs, char* path, UINT offset, BYTE* buf, UINT numBytes);

// writes to a file
INT l2_write(FileSystem* fs, char* path, UINT offset, BYTE* buf, UINT numBytes);

// updates the mod/access time of a file
INT l2_utimens(FileSystem* fs, char* path, struct timespec tv[2]);

//resolve path to inode id
INT l2_namei(FileSystem *, char *);
