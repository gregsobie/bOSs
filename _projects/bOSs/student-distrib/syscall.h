#ifndef _SYSCALL_H
#define _SYSCALL_H

#define asmlinkage __attribute__((regparm(0)))
#define KERNEL_STACK_SIZE 0x2000 //8KB


#include "types.h"


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


static void * kernel_top = (void *) 0x800000; //8 MB

typedef struct PCB{
	uint32_t pid;
	struct file fd[8];
	uint32_t esp0;
	uint32_t ebp0;
	uint32_t PDE_num; // Mem mapping will require full PD
	uint32_t esp;
	uint32_t ebp;
	//Register table

} PCB_t;

asmlinkage int32_t halt (uint8_t status);
asmlinkage int32_t execute (const uint8_t* command);
asmlinkage int32_t read (int32_t fd, void* buf, int32_t nbytes);
asmlinkage int32_t write (int32_t fd, const void* buf, int32_t nbytes);
asmlinkage int32_t open (const uint8_t* filename);
asmlinkage int32_t close (int32_t fd);
asmlinkage int32_t getargs (uint8_t* buf, int32_t nbytes);
asmlinkage int32_t vidmap (uint8_t** screen_start);
asmlinkage int32_t set_handler (int32_t signum, void* handler_address);
asmlinkage int32_t sigreturn (void);

#endif
