#include "syscall.h"
#include "filesystem.h"
#include "paging.h"
#include "x86_desc.h"
#include "lib.h"

asmlinkage int32_t execute (const uint8_t* command){
    asm volatile("cli");
	printf("exec\n");

	//Parse args
	dentry_t d;
	if(read_dentry_by_name (command, &d) == -1)
		return -1;
	uint8_t head[40];
	read_data (d.inode, 0, head, 40);
	if(head[0] != 0x7f || head[1] != 0x45 || head[2] != 0x4c || head[3] != 0x46){
		return -1;
	}
	//Convert EIP from little endian to big endian
	//Could use x86 bswap %reg
	uint32_t eip = (head[27] << 24) | (head[26] << 16) | (head[25] << 8) | head[24];
	//obtain perm level from flags register?
	//Obtain PID
	int i;
	uint32_t pid = -1;
	for(i=0;i<MAX_USER_PROG;i++){
		if(proc_id_used[i] == false){
			pid = i;
			proc_id_used[i] = true;
			break;
		}
	}
	if(pid == -1){
		printf("Max processes used.");
		return -1;
	}

	//Change paging
	proc_page_directory[pid][0] =  (uint32_t)video_page_table | FLAG_WRITE_ENABLE | FLAG_PRESENT;
	proc_page_directory[pid][1] = 0x00400000 | FLAG_4MB_PAGE | FLAG_WRITE_ENABLE | FLAG_PRESENT | FLAG_GLOBAL;
	proc_page_directory[pid][USER_PROG_LOCATION >> 22] = (0x00400000 * (pid+2)) | FLAG_4MB_PAGE | FLAG_WRITE_ENABLE | FLAG_PRESENT |FLAG_USER;

	loadPageDirectory(proc_page_directory[pid]);

	//Load program into program area
	read_data (d.inode, 0, (uint8_t *)(USER_PROG_LOCATION), 0x400000); //Copy up to 4MB of program data (stack will kill some of it)
	//Create PCB
	printf("1\n");
	PCB_t * pcb = (PCB_t *)(0x800000-0x2000 * (pid+1));
	pcb->pid = pid;
	pcb->esp0 = tss.esp0; //Bottom of stack

	PCB_t * parent;
	cur_pcb(parent);
	pcb->parent = parent;
	asm volatile("\
		movl	%%esp,%0 	\n\
		movl    %%ebp, %1"
		: "=r"(parent->esp), "=r"(parent->ebp)
		: 
		:"memory");
	uint32_t u_esp = USER_MEM_LOCATION + 0x00400000 -4 ;
	//init 
	//pcb.fd[0].file_operations = 
	//
	printf("%x %x %x\n",eip, (0x00400000 * (pid+2)), u_esp);
	//Change TSS
	tss.esp0 = 0x800000-0x2000 * pid -4;//address of new kernel stack
	tss.ss0 = KERNEL_DS;
	//set up fake table thingy on stack
	printf("9\n");
	asm volatile("\
		cli	\n\
		movw	%2,%%ax 	\n\
		movw    %%ax, %%ds		\n\
		movw    %%ax, %%es		\n\
		movw    %%ax, %%fs		\n\
		movw    %%ax, %%gs		\n\
		movl	%%esp,%%eax		\n\
		pushl	%2		\n\
		pushl 	%1			\n\
		pushf					\n\
		orl $0x200,(%%esp)	# set the interrupt flag\n\
		pushl	%3		\n\
		pushl	%0				\n\
		iret	\n\
		halt_ret_label:"
		:
		: "r"(eip),"r"(u_esp),"i"(USER_DS),"i"(USER_CS)
		: "eax");
	//
	return 0;
}
asmlinkage int32_t halt (uint8_t status){
	printf("halt\n");
	PCB_t * pcb;
	cur_pcb(pcb);

	//Change paging back
	//Security-shuld clean old memory space
	loadPageDirectory(proc_page_directory[pcb->parent->pid]);
	//Change TSS
	//change esp/ebp
	//close any FDs that need it
	//jump to halt_ret_label in exec
	
	/*asm volatile("\
		movl	%0,%%esp 	\n\
		movl    %1,%%ebp,"	
		:
		: "r"(pcb->parent->esp),"r"(pcb->parent->ebp) 
		: "memory" );*/
	//goto halt_ret_label;
	return 0;
}

asmlinkage int32_t read (int32_t fd, void* buf, int32_t nbytes){
	printf("read\n");
	return 0;
}
asmlinkage int32_t write (int32_t fd, const void* buf, int32_t nbytes){
	printf("write\n");
	return 0;
}
asmlinkage int32_t open (const uint8_t* filename){
	printf("open\n");
	return 0;
}
asmlinkage int32_t close (int32_t fd){
	printf("close\n");
	return 0;
}
asmlinkage int32_t getargs (uint8_t* buf, int32_t nbytes);
asmlinkage int32_t vidmap (uint8_t** screen_start);
asmlinkage int32_t set_handler (int32_t signum, void* handler_address);
asmlinkage int32_t sigreturn (void);
