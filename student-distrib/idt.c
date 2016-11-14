#include "x86_desc.h"
#include "lib.h"
#include "RTC.h"
#include "idt.h"
#include "keyboard.h"
#include "i8259.h"
#include "debug.h"
#include "multiboot.h"
#include "types.h"
#include "syscall_linkage.h"

void initialize_idt()
{
	int i;
	/*
	exceptions/interrupts = kernel cs, dpl = 0 
	system calls = trap gates , dpl = 3 
	*/

	for (i=0;i<32;i++)
		{
		/*
			set up proper bits to make into an trap gate exception 
		*/
			idt[i].present = 1;
			idt[i].seg_selector = KERNEL_CS;
			idt[i].dpl = 0;
			
			idt[i].reserved4 = 0;
			idt[i].reserved3 = 1;
			idt[i].reserved2 = 1;
			idt[i].reserved1 = 1;
			idt[i].size = 1;
			idt[i].reserved0 = 0;

			SET_IDT_ENTRY(idt[i],&generic_handler);
		
		}	
	/*
	interrupt handlers
	all are interrupt gates w kernel CS and priv = 0 

	idt[32] = timer chip
	idt[33] = keys
	idt[40] = rtc
	*/

	for (i=32;i<NUM_VEC;i++)
	{
			idt[i].present = 1;
			idt[i].seg_selector = KERNEL_CS;
			idt[i].dpl = 0;
		
			idt[i].reserved4 = 0;
			idt[i].reserved3 = 0;
			idt[i].reserved2 = 1;
			idt[i].reserved1 = 1;
			
			
			idt[i].size = 1;
			idt[i].reserved0 = 0;
		
			
	}


	/*

	set idt entries 
	first 32 = exceptions
	next (256-32) = interrupts
	0x80 will have the system call jump table function, but not yet
	be sure to set dpl to 3 for that
	*/
	SET_IDT_ENTRY(idt[0],&de_handler);
	SET_IDT_ENTRY(idt[1],&db_handler);
	SET_IDT_ENTRY(idt[2],&nmi_handler);
	SET_IDT_ENTRY(idt[3],&bp_handler);
	SET_IDT_ENTRY(idt[4],&of_handler);
	SET_IDT_ENTRY(idt[5],&br_handler);
	SET_IDT_ENTRY(idt[6],&ud_handler);
	SET_IDT_ENTRY(idt[7],&nm_handler);
	SET_IDT_ENTRY(idt[8],&df_handler);
	SET_IDT_ENTRY(idt[9],&co_segment_overrun_handler);
	SET_IDT_ENTRY(idt[10],&ts_handler);
	SET_IDT_ENTRY(idt[11],&np_handler);
	SET_IDT_ENTRY(idt[12],&ss_handler);
	SET_IDT_ENTRY(idt[13],&gp_handler);
	SET_IDT_ENTRY(idt[14],&pf_handler);
	SET_IDT_ENTRY(idt[16],&mf_handler);
	SET_IDT_ENTRY(idt[17],&ac_handler);
	SET_IDT_ENTRY(idt[18],&mc_handler);
	SET_IDT_ENTRY(idt[19],&xf_handler);

	SET_IDT_ENTRY(idt[32],&pit_irq_handler);
	SET_IDT_ENTRY(idt[33],&key_irq_handler);
	SET_IDT_ENTRY(idt[40],&rtc_irq_handler);
	
	SET_IDT_ENTRY(idt[128],&systemcall_linkage);
	idt[128].dpl = 3;
}
void fail(){
	//When we implement program execution, we will need to extend/move this
	asm volatile("hlt;");
}
void de_handler()
{
	cli();
	printf("divide by zero exception");
	fail();

}
void db_handler()
{
	cli();
	printf("db exception");
	fail();
}
void nmi_handler()
{
	cli();
	printf("nmi exception");
	fail();
}
void bp_handler()
{
	cli();
	printf("bp exception");
	fail();
}
void of_handler()
{
	cli();
	printf("of exception");
	fail();

}
void br_handler()
{
	cli();
	printf("br exception");
	fail();
}
void ud_handler()
{
	cli();
	printf("ud exception");
	fail();
}
void nm_handler()
{
	cli();
	printf("nm exception");
	fail();

}
void df_handler()
{
	cli();
	printf("df exception");
	fail();
}
void co_segment_overrun_handler()
{
	cli();
	printf("co_segment_overrun exception");
	fail();
}
void ts_handler()
{
	cli();
	printf("ts exception");
	fail();
}
void np_handler()
{
	cli();
	printf("np exception");
	fail();
}
void ss_handler()
{
	cli();
	printf("ss exception");
	fail();
}
void gp_handler()
{
	cli();
	printf("gp exception");
	fail();
}
void pf_handler()
{
	cli();
	uint32_t cr2;
	uint32_t eec;
	asm volatile("mov %%cr2,%0; popl %1":"=r"(cr2),"=r"(eec)::"memory");
	printf("pf exception at %x, %x",cr2,eec);
	fail();
}
void mf_handler()
{
	cli();
	printf("mf exception");
	fail();
}
void ac_handler()
{
	cli();
	printf("ac exception");
	fail();
}
void mc_handler()
{
	cli();
	printf("mc exception");
	fail();
}
void xf_handler()
{
	cli();
	printf("xf exception");
	fail();
}
void generic_handler()
{
	cli();
	printf("Unknown exception");
	fail();
}
void pit_irq_handler()
{
	send_eoi(0);
 	asm volatile("leave;iret;");
}
