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
//#include "Directories.h"

static struct fuse_context fs_ctx;
static const char *hello_str = "Hello World!\n";
static const char *hello_path = "/hello";

static int l2_getattr(const char *path, struct stat *stbuf)
{
	int res = 0;
	
	printf("getattr %p\n", fs_ctx.private_data);
	
	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	} else if (strcmp(path, hello_path) == 0) {
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = strlen(hello_str);
	} else
		res = -ENOENT;

	return res;
}

static int l2_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi)
{
	(void) offset;
	(void) fi;

	if (strcmp(path, "/") != 0)
		return -ENOENT;

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	filler(buf, hello_path + 1, NULL, 0);

	return 0;
}

static int l2_mknod(const char *path, mode_t mode, dev_t dev)
{
	;
}

static int l2_mkdir(const char *path, mode_t mode)
{
	;
}

static int l2_unlink(const char *path)
{
	;
}

static int l2_rmdir(const char *path)
{
	;
}

static int l2_rename(const char *path, const char *new_path)
{
	printf("old name: %s\n", path);
	printf("new name: %s\n", new_path);
}

static int l2_chmod(const char *path, mode_t mode)
{
	;
}

static int l2_chown(const char *path, uid_t uid, gid_t gid)
{
	;
}

static int l2_truncate(const char *path, off_t offset)
{
	;
}

static int l2_open(const char *path, struct fuse_file_info *fi)
{
	if (strcmp(path, hello_path) != 0)
		return -ENOENT;

	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;

	return 0;
}

static int l2_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi)
{
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

	return size;
}

static int l2_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	;
}

static int l2_statfs(const char *path, struct statvfs *stat)
{
	;
}

static struct fuse_operations l2_oper = {
	.getattr	= l2_getattr,
	.readdir	= l2_readdir,
	.mknod		= l2_mknod,
	.mkdir		= l2_mkdir,
	.unlink		= l2_unlink,
	.rmdir		= l2_rmdir,
	.rename		= l2_rename,
	.chmod		= l2_chmod,
	.chown		= l2_chown,
	.truncate	= l2_truncate,
	.open		= l2_open,
	.read		= l2_read,
	.write		= l2_write,
	.statfs		= l2_statfs,
};

int main(int argc, char *argv[])
{
	return fuse_main(argc, argv, &l2_oper, NULL);
}
