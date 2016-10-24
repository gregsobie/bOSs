#ifndef _SYSCALL_H
#define _SYSCALL_H

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

struct file_operations fops_table[8];

#endif
