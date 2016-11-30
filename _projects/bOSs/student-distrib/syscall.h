#ifndef SYSCALL_H
#define SYSCALL_H

#define asmlinkage __attribute__((regparm(0)))
#define KERNEL_STACK_SIZE 0x2000 //8KB
#define PCB_MASK 0xFFFFE000
#define MB4	0x00400000
#define KB8 0x2000
#define ELF_MAGIC_0 0x7f
#define KERNEL_TOP	0x00800000

#include "types.h"
#include "keyboard.h"
#include "lib.h"
//struct file_operations fops_table[8];


typedef struct PCB{
	uint8_t name[LINE_BUF_SIZE];
	uint8_t args[LINE_BUF_SIZE];
	uint32_t pid;
	uint32_t esp0;
	uint32_t ss0;
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
