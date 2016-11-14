#include "syscall.h"
#include "filesystem.h"
#include "paging.h"
#include "x86_desc.h"
#include "lib.h"
#include "RTC.h"
#include "keyboard.h"

void check_i(){
	uint32_t eflags;
	asm volatile("pushfl      \n\
			movl (%%esp),%0; 			\n\
			addl $4,%%esp"
			: "=r" (eflags):: "memory");   				
	printf("%x\n",eflags & 0x200);
}

asmlinkage int32_t execute (const uint8_t* command){
    asm volatile("cli");
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
	PCB_t * pcb = (PCB_t *)(KERNEL_TOP-KB4 * (pid+1));

	pcb->pid = pid;
	pcb->esp0 = tss.esp0;
	if(pcb->pid == 0)
		pcb->parent = pcb;
	//Parse args
	for(i=0;i<=MAX_BUF_INDEX && command[i] != ' '&& command[i] != '\0';i++){
		pcb->name[i] = command[i];
	}
	pcb->name[i] = '\0';
	while(command[i] == ' ')i++;
	uint32_t offset = i;
	for(;i<=MAX_BUF_INDEX && command[i] != '\0';i++){
		pcb->args[i-offset] = command[i];
	}
	dentry_t d;
	if(read_dentry_by_name (pcb->name, &d) == -1){
		proc_id_used[pid] = false;
		return -1;
	}
	uint8_t head[40];
	read_data (d.inode, 0, head, 40);
	if(head[0] != ELF_MAGIC_0 || head[1] != 'E' || head[2] != 'L' || head[3] != 'F'){
		proc_id_used[pid] = false;
		return -1;
	}
	//Convert EIP from little endian to big endian
	//Could use x86 bswap %reg
	uint32_t eip = (head[27] << 24) | (head[26] << 16) | (head[25] << 8) | head[24];
	//obtain perm level from flags register?

	//Change paging
	proc_page_directory[pid][0] =  (uint32_t)video_page_table | FLAG_WRITE_ENABLE | FLAG_PRESENT;
	proc_page_directory[pid][1] = MB4 | FLAG_4MB_PAGE | FLAG_WRITE_ENABLE | FLAG_PRESENT | FLAG_GLOBAL;
	proc_page_directory[pid][USER_MEM_LOCATION >> 22] = (KERNEL_TOP + MB4 * pid) | FLAG_4MB_PAGE | FLAG_WRITE_ENABLE | FLAG_PRESENT |FLAG_USER;

	loadPageDirectory(proc_page_directory[pid]);

	//Load program into program area
	read_data (d.inode, 0, (uint8_t *)(USER_PROG_LOCATION), MB4); //Copy up to 4MB of program data (stack will kill some of it)
	//Create PCB
	

	PCB_t * parent;
	cur_pcb(parent);
	pcb->parent = parent;
	asm volatile("\
		movl	%%ss,%2		\n\
		movl	%%esp,%0 	\n\
		movl    %%ebp, %1"
		: "=r"(pcb->esp), "=r"(pcb->ebp), "=r"(pcb->ss0)
		: 
		:"memory");
	uint32_t u_esp = USER_MEM_LOCATION + MB4 -4 ;
	//init file ops
	//printf("%x\n",pcb);
	//printf("%x\n",parent);

	pcb->fd[0].f_op = &terminal_ops;
	pcb->fd[0].flags =1;
	pcb->fd[1].f_op = &terminal_ops;
	pcb->fd[1].flags =1;
	//
	//printf("%x %x %x %d\n",eip, (KERNEL_TOP + MB4 * pid), u_esp,USER_MEM_LOCATION >> 22);
	//Change TSS
	tss.esp0 = KERNEL_TOP-KB4 * pid -4;//address of new kernel stack
	tss.ss0 = KERNEL_DS;
	//set up fake table thingy on stack
	asm volatile("\
		movw	%2,%%ax 	\n\
		movw    %%ax, %%ds		\n\
		movw    %%ax, %%es		\n\
		movw    %%ax, %%fs		\n\
		movw    %%ax, %%gs		\n\
		movl	%%esp,%%eax		\n\
		pushl	%2		\n\
		pushl 	%1			\n\
		pushf					\n\
		popl	%%eax \n\
		orl $0x200,%%eax	# set the interrupt flag\n\
		pushl	%%eax	\n\
		pushl	%3		\n\
		pushl	%0				\n\
		iret	\n\
		.globl halt_ret_label	\n\
		halt_ret_label:"
		:
		: "r"(eip),"r"(u_esp),"i"(USER_DS),"i"(USER_CS)
		: "eax");
	uint32_t ret;
	asm volatile("movl %%eax, %0":"=r" (ret));

	return ret;
}
asmlinkage int32_t halt (uint8_t status){
	PCB_t * pcb;
	cur_pcb(pcb);

	//Change paging back

	//Security-shuld clean old memory space
	loadPageDirectory(proc_page_directory[pcb->parent->pid]);
	//Change TSS
	tss.esp0 = pcb->parent->esp0;
	tss.ss0 = pcb->parent->ss0;
	//close any FDs that need it
	int i = 0;
	for (i=0;i<8;i++){
		close(i);
	}
	proc_id_used[pcb->pid] = false;
	//change esp/ebp
	if(pcb->pid == 0){
		execute((uint8_t *)"shell");
	}
	asm volatile("\
		movl 	%2,%%eax	\n\
		movl	%0,%%esp 	\n\
		movl    %1,%%ebp"	
		:
		: "r"(pcb->esp),"r"(pcb->ebp),"r"((uint32_t)status)
		: "memory" );

	asm volatile("jmp halt_ret_label");
	return 0;
}

asmlinkage int32_t read (int32_t fd, void* buf, int32_t nbytes){
	PCB_t * pcb;
	cur_pcb(pcb);
	if (pcb->fd[fd].flags == 0 || fd >= 8 || fd < 0)
	{
		return -1;
	}

	//struct file * fp = (struct file*) (&(pcb->fd[fd]));
	int32_t val = (pcb->fd[fd].f_op->read(&(pcb->fd[fd]), buf, nbytes));
	return val;
}
asmlinkage int32_t write (int32_t fd, const void* buf, int32_t nbytes){

	PCB_t* pcb;
	cur_pcb(pcb);
	if (pcb->fd[fd].flags==0 || buf==NULL || fd > 7 || fd < 0)
		return -1;

	struct file* fp=(struct file*) (&(pcb->fd[fd]));

	int32_t val=(fp->f_op->write(fp, buf, nbytes));
	return val;
}
asmlinkage int32_t open (const uint8_t* filename)
{
	int i;
	dentry_t d;
	uint32_t success;
	PCB_t * current;
	cur_pcb(current);
	if(filename == NULL)
		return -1;
	//printf("%x %s\n",filename,filename);
	success = read_dentry_by_name(filename, &d);
	if (success == 0)
	{
		for (i=2;i<8;i++)
		{
			/*
			set flags == 1 if that file descriptor is in use 
			*/

			if ( current->fd[i].flags == 0)
			{
				
				//set the operations table, inode, flags?
				//inode_t * node = (inode_t *)(&fs_base[d->inode+1]);
				

				
				if (d.type == 0)
				{



					// current->fd[i].f_op->read = RTC_read;
					// current->fd[i].f_op->write =  RTC_write;
					// current->fd[i].f_op->open =  RTC_open;
					// current->fd[i].f_op->close =  RTC_close;

					current->fd[i].f_inode = NULL;
					current->fd[i].f_pos = 0;
					current->fd[i].flags = 1;
					current->fd[i].f_op = &RTC_ops;
					//fops_table[i] = RTC_ops;
				}
				
				else if (d.type == 1)
				{
					// current->fd[i].f_op->read = dir_read;
					// current->fd[i].f_op->write = dir_write;
					// current->fd[i].f_op->open = dir_open;
					// current->fd[i].f_op->close = dir_close;
					current->fd[i].f_inode = d.inode;
					current->fd[i].f_pos = 0;
					current->fd[i].flags = 1;
					current->fd[i].f_op = &dir_ops;
					 //current->fd[i].f_op = fops_table[FILE_DAT_TYPE];
					//fops_table[i] = dir_ops;
				}
				
				else if (d.type == 2)
				{
					// current->fd[i].f_op->read = file_read;
					// current->fd[i].f_op->write = file_write;
					// current->fd[i].f_op->open = file_open;
					// current->fd[i].f_op->close = file_close;
					current->fd[i].f_inode = d.inode;
					current->fd[i].f_pos = 0;
					current->fd[i].flags = 1;
					current->fd[i].f_op = &file_ops;
					//current->fd[i].f_op = fops_table[FILE_DIR_TYPE];
					//fops_table[i] = file_ops;
				}
				current->fd[i].fd_index = i;
				//current->fd[i].f_op->open(&(current->fd[i]));
				//struct file* fp=(struct file*) (&(current->fd[i]));
				if(current->fd[i].f_op->open((&(current->fd[i]))) == -1)
					return -1;
				return i;


			}
		}
	
	}
	return -1;


	
}
asmlinkage int32_t close (int32_t fd){
		
	PCB_t * current;
	cur_pcb(current);

	if (fd > 7 || fd < 2)
	{
		return -1;
	}
	if(current->fd[fd].flags == 0){
		return -1;
	}
	int32_t ret = current->fd[fd].f_op->close(&(current->fd[fd]));

	current->fd[fd].f_inode = NULL;
	current->fd[fd].f_pos = NULL;
	current->fd[fd].f_op = NULL;
	current->fd[fd].flags = NULL;
	current->fd[fd].fd_index = NULL;

	//struct file* fp=(struct file*) (&(current->fd[fd]));


	return ret;
}
asmlinkage int32_t getargs (uint8_t* buf, int32_t nbytes)
{
	PCB_t * current;
	cur_pcb(current);
	if(strlen((int8_t *)current->args) > nbytes)
		return -1;
	if(nbytes > LINE_BUF_SIZE)
		nbytes = LINE_BUF_SIZE;

	int i;
	for(i=0;i<nbytes;i++){
		buf[i] = current->args[i];
		if(current->args[i] == '\0')
			break;
	}
	return 0;
}
asmlinkage int32_t vidmap (uint8_t** screen_start)
{
	return 0;
}
asmlinkage int32_t set_handler (int32_t signum, void* handler_address){
	return 0;
}
asmlinkage int32_t sigreturn (void){

return 0;
}
