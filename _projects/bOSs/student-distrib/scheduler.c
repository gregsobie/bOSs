#include "scheduler.h"
#include "syscall.h"
#include "i8259.h"
#include "x86_desc.h"
volatile uint8_t shells_started = 1;
void init_PIT(){
	//possibly configure pit for different frequencies?
	enable_irq(0);

}

void sched(){
	cli();
	uint32_t pid = -1;
	PCB_t * pcb;

	cur_pcb(pcb);
	pid = pcb->pid;
	active[pcb->pid] = true;


	uint8_t * video = (uint8_t *) VIDEO;
	video[(70 << 1)+1] ^= 0x30;
	video[((70+1+pid)<<1) + 1] = 0x40;
	

	asm volatile("\
		movl	%%esp,%0 	\n\
		movl    %%ebp, %1"
		: "=r"(pcb->esp), "=r"(pcb->ebp)
		: 
		:"memory"); //change segment registers 

	if(shells_started < 3){
		shells_started++;
		execute((uint8_t *)"shell");
	}
	uint32_t next = get_next_proc(pid);
	active[next] = false;
	PCB_t * next_pcb = 	(PCB_t *)(KERNEL_TOP-KB8 * (next+1));
	video[((70+1+next)<<1) + 1] = 0x20;
	loadPageDirectory(proc_page_directory[next]);
	//printf("%d -> %d\n",pcb->pid,next);
	tss.esp0 = next_pcb->esp0;
	asm volatile("\
		movl	%0,%%esp 	\n\
		movl    %1,%%ebp"
		:
		: "r"(next_pcb->esp),"r"(next_pcb->ebp)\
		: "memory" );
}


uint32_t get_next_proc(uint32_t curr){
	do {
		curr++;		
		if(curr >= MAX_USER_PROG)
			curr -= MAX_USER_PROG;
	}while(!active[curr]);
	return curr;
}
