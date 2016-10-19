#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include "types.h"

#define FILE_NAME_LEN 32
#define BLOCK_BYTES 4096


typedef struct dentry{
	char name[FILE_NAME_LEN+1];//leave room for null termination
	uint32_t type;
	uint32_t inode;
} dentry_t;

typedef struct dentry_int{
	char name[FILE_NAME_LEN];
	uint32_t type;
	uint32_t inode;
	uint8_t res1[24];
} dentry_int_t;

typedef struct boot_block{
	uint32_t num_entries;
	uint32_t num_inodes;
	uint32_t num_data_blocks;
	uint8_t res1[52];
	dentry_int_t dentries[63];
} boot_block_t;

typedef struct inode{
	uint32_t length_bytes;
	uint32_t data_blocks[BLOCK_BYTES/4 -1];
} inode_t;

extern boot_block_t * fs_base;
extern boot_block_t * fs_end;

struct file;

struct file_operations{
	int32_t (*read) (struct file *, char *, uint32_t);
	int32_t (*write) (struct file *, const char *, uint32_t);
	int32_t (*open) (struct file *);
	int32_t (*close) (struct file *);
} file_operations_t;

struct file{
	struct file_operations * f_op;
	uint32_t f_inode;
	uint32_t f_pos;
	uint32_t flags;
};




int32_t file_read (struct file *, char *, uint32_t);
int32_t file_write (struct file *, const char *, uint32_t);
int32_t file_open (struct file *);
int32_t file_close (struct file *);

static struct file_operations file_ops = {
	.read = file_read,
	.write = file_write,
	.open = file_open,
	.close = file_close
};

int32_t dir_read (struct file *, char *, uint32_t);
int32_t dir_write (struct file *, const char *, uint32_t);
int32_t dir_open (struct file *);
int32_t dir_close (struct file *);

static struct file_operations dir_ops = {
	.read = dir_read,
	.write = dir_write,
	.open = dir_open,
	.close = dir_close
};

int32_t test_read(const uint8_t* filename, const void* buf, int32_t nbytes);

int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

#endif
