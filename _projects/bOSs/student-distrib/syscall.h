#ifndef SYSCALL_H
#define SYSCALL_H

#define asmlinkage __attribute__((regparm(0)))
#define KERNEL_STACK_SIZE 0x2000 //8KB
#define PCB_MASK 0xFFFFE000

#include "types.h"
#include "keyboard.h"

struct file;

typedef struct file_operations{
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
	uint32_t fd_index;
};

struct file_operations fops_table[8];


static void * kernel_top = (void *) 0x800000; //8 MB

typedef struct PCB{
	uint8_t name[MAX_BUF_INDEX];
	uint8_t args[MAX_BUF_INDEX];
	uint32_t pid;
	uint32_t esp0;
	uint32_t esp;
	uint32_t ebp;
	struct file fd[8];	
	struct PCB * parent;
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

#define cur_pcb(addr)                   \
do {                                    \
	asm volatile("movl %%esp,%0      \n\
			andl %1,%0"			\
			: "=r" (addr)               \
			: "i"(PCB_MASK)                          \
			: "memory");   				\
} while(0)

#endif
