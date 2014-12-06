/*
  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.

  gcc -Wall fuseDaemon.c `pkg-config fuse --cflags --libs` -o fuseDaemon
*/

#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "Directories.h"

static FileSystem fs;
static const char *hello_str = "Hello World!\n";
static const char *hello_path = "/hello";

static int l3_getattr(const char *path, struct stat *stbuf)
{
	int res = 0;
	
	#ifdef DEBUG
	//printf("getattr %s\n", path);
	#endif

	memset(stbuf, 0, sizeof(struct stat));

	return l2_getattr(&fs, path, stbuf);
}

static int l3_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi)
{
	//printf("offset: %u\n", offset);
	(void) fi;
 	DirEntry curEntry;
	//UINT entryL = (24 + FILE_NAME_LENGTH + 7) & (~7);
	
	INT res = l2_readdir(&fs, path, offset, &curEntry);
	while (res == 0) {
		if (curEntry.INodeID != -1) {
			//printf("filling %s\n", curEntry.key);
			offset ++;
			if (filler(buf, curEntry.key, NULL, offset) == 1) {
				//printf("fuse_filler buf full!\n");
				break;
			}
			//printf("%s\n", (char *)buf);
			return 0;
		}
		offset ++;
		res = l2_readdir(&fs, path, offset, &curEntry);
	}
	if (res == -1)
		return -ENOENT;
	return 0;
}

static int l3_mknod(const char *path, mode_t mode, dev_t dev)
{
	struct fuse_context* fctx = fuse_get_context();
	INT res = l2_mknod(&fs, path, fctx->uid, fctx->gid);
	return res>0?0:res;
}

static int l3_mkdir(const char *path, mode_t mode)
{
	struct fuse_context* fctx = fuse_get_context();
	INT res = l2_mkdir(&fs, path, fctx->uid, fctx->gid);
	return res>0?0:res;
}

static int l3_unlink(const char *path)
{
	return l2_unlink(&fs, path);
}

static int l3_rmdir(const char *path)
{
	return l2_unlink(&fs, path);
}

static int l3_rename(const char *path, const char *new_path)
{
<<<<<<< HEAD
	//printf("old name: %s\n", path);
	//printf("new name: %s\n", new_path);
=======
	#ifdef DEBUG
	printf("old name: %s\n", path);
	printf("new name: %s\n", new_path);
	#endif
>>>>>>> fix fuse interface to l2_open and l2_close
	return l2_rename(&fs, path, new_path);
}

static int l3_chmod(const char *path, mode_t mode)
{
	#ifdef DEBUG
	printf("l3_chmod with mode: %x\n", mode);
	#endif
	return l2_chmod(&fs, path, mode);
}

static int l3_chown(const char *path, uid_t uid, gid_t gid)
{
	printf("l3_chown\n");
	return l2_chown(&fs, path, uid, gid);
}

static int l3_truncate(const char *path, off_t offset)
{
    //printf("truncate %s to be length %u\n", path, offset);
	return l2_truncate(&fs, path, offset);;
}

static int l3_open(const char *path, struct fuse_file_info *fi)
{
    //parse file operation from flags
    enum FILE_OP fileOp;
    if(fi->flags & O_RDONLY)
        fileOp = OP_READ;
    else if(fi->flags & O_WRONLY)
        fileOp = OP_WRITE;
    else if(fi->flags & O_RDWR)
        fileOp = OP_READWRITE;
    else {
        fprintf(stderr, "Error: no file operation specified in flags!\n");
        return -EINVAL;
    }
    #ifdef DEBUG
    printf("File operation from flags: %d\n", fileOp);
    #endif
    
    //parse create flag
    if(fi->flags & O_CREAT) {
        #ifdef DEBUG
        printf("O_CREAT flag detected, creating file: %s\n", path);
        #endif
        struct fuse_context* fctx = fuse_get_context();
        INT succ = l2_mknod(&fs, path, fctx->uid, fctx->gid);
        
        //parse exists flag
        if((fi->flags & O_EXCL) && (succ == -EEXIST)) {
            fprintf(stderr, "Error: O_EXCL specified for open but file already exists!\n");
            return -EEXIST;
        }
        else if(succ < 0) {
            return succ;
        }
    }
    
    //parse truncate flag
    if(fi->flags & (O_TRUNC | O_WRONLY | O_RDWR)) {
        #ifdef DEBUG
        printf("O_TRUNC flag detected, truncating file: %s\n", path);
        #endif
        //TODO truncate file
    }
    
	return (int)l2_open(&fs, path, fileOp);
}

static int l3_release(const char *path, struct fuse_file_info *fi)
{
    //parse file operation from flags
    enum FILE_OP fileOp;
    if(fi->flags & O_RDONLY)
        fileOp = OP_READ;
    else if(fi->flags & O_WRONLY)
        fileOp = OP_WRITE;
    else if(fi->flags & O_RDWR)
        fileOp = OP_READWRITE;
    else {
        fprintf(stderr, "Error: no file operation specified in flags!\n");
        return -EINVAL;
    }
    #ifdef DEBUG
    printf("File operation from flags: %d\n", fileOp);
    #endif
    
	return (int)l2_close(&fs, path, fileOp);
}

static int l3_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi)
{
        /*
	size_t len;
	(void) fi;
	if(strcmp(path, hello_path) != 0)
		return -ENOENT;

	len = strlen(hello_str);
	if (offset < len) {
		if (offset + size > len)
			size = len - offset;
		memcpy(buf, hello_str + offset, size);
	} else
		size = 0;
	*/
	#ifdef DEBUG
	printf("Calling l2_read for path \"%s\" and offset: %u for size: %u\n", path, offset, size);
	#endif
	return (int)l2_read(&fs, path, offset, buf, size);
}

static int l3_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	#ifdef DEBUG_VERBOSE
    	printf("l3_write received buffer to write: %s\n", buf);
	#endif
	printf("Calling l2_write for path \"%s\" and offset: %u for size: %u\n", path, offset, size);
	return (int)l2_write(&fs, path, offset, buf, size);
}

static int l3_utimens(const char *path, const struct timespec tv[2]) 
{
	return l2_utimens(&fs, path, tv);
}

static int l3_statfs(const char *path, struct statvfs *stat)
{
	;
}

void * l3_mount(struct fuse_conn_info *conn)
{
	UINT succ = l2_mount(&fs);
}

void * l3_unmount(void *conn)
{
	UINT succ = l2_unmount(&fs);
}

static struct fuse_operations l3_oper = {
	.getattr	= l3_getattr,
	.readdir	= l3_readdir,
	.mknod		= l3_mknod,
	.mkdir		= l3_mkdir,
	.unlink		= l3_unlink,
	.rmdir		= l3_rmdir,
	.rename		= l3_rename,
	.chmod		= l3_chmod,
	.chown		= l3_chown,
	.truncate	= l3_truncate,
	.open		= l3_open,
	.release	= l3_release,
	.read		= l3_read,
	.write		= l3_write,
	.utimens	= l3_utimens,
	.statfs		= l3_statfs,
	.init		= l3_mount,
	.destroy	= l3_unmount
};

int main(int argc, char *argv[])
{
	/*printf("Initializing file system with initfs...\n");
    	UINT succ = l2_initfs(128, 16, &fs);
    	if(succ == 0) {
        	printf("initfs succeeded with filesystem size: %d\n", fs.nBytes);
    	}
    	else {
        	printf("Error: initfs failed with error code: %d\n", succ);
    	}*/
	UINT succ = l2_mount(&fs);
	return fuse_main(argc, argv, &l3_oper, NULL);	
}
