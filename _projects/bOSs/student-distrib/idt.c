#include "x86_desc.h"
#include "lib.h"
#include "RTC.h"
#include "idt.h"
#include "keyboard.h"
#include "i8259.h"
#include "debug.h"
#include "multiboot.h"


void initialize_idt()
{
/*
exceptions/interrupts = kernel cs, pl = 0 
system calls = trap gates , pl = 3 
*/
int i;
	for (i=0;i<32;i++)
		{
		/*
			set up proper bits to make into an trap gate exception 
		*/
			idt[i].present = 1;
			idt[i].seg_selector = KERNEL_CS;
			idt[i].dpl = 0;
		
			idt[i].reserved1 = 1;
			idt[i].reserved2 = 1;
			idt[i].reserved3 = 1;
			idt[i].size = 1;
			idt[i].reserved0 = 0;
		
			idt[i].reserved4 = 0;
		}	
/* place system calls(trap gates) at end of IDT */

	for (i=(NUM_VEC -12);i<NUM_VEC;i++)
	{
			idt[i].present = 1;
			idt[i].seg_selector = KERNEL_CS;
			idt[i].dpl = 3;
		
			idt[i].reserved1 = 1;
			idt[i].reserved2 = 1;
			idt[i].reserved3 = 1;
			idt[i].size = 1;
			idt[i].reserved0 = 0;
		
			idt[i].reserved4 = 0;
			//SET_IDT_ENTRY(idt[i],sys_call);
	}
/*
interrupt handlers
all are interrupt gates w kernel CS and priv = 0 

idt[32] = timer chip
idt[33] = keys
idt[40] = rtc
idt[44] = mouse 
*/

	for (i=32;i<(NUM_VEC-12);i++)
	{
			idt[i].present = 1;
			idt[i].seg_selector = KERNEL_CS;
			idt[i].dpl = 0;
		
			idt[i].reserved1 = 1;
			idt[i].reserved2 = 1;
			idt[i].reserved3 = 0;
			idt[i].size = 1;
			idt[i].reserved0 = 0;
		
			idt[i].reserved4 = 0;
	}
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
	SET_IDT_ENTRY(idt[15],&mf_handler);
	SET_IDT_ENTRY(idt[16],&ac_handler);
	SET_IDT_ENTRY(idt[17],&mc_handler);
	SET_IDT_ENTRY(idt[18],&xf_handler);

	SET_IDT_ENTRY(idt[32],&pit_irq_handler);
	SET_IDT_ENTRY(idt[33],&key_irq_handler);
	SET_IDT_ENTRY(idt[40],&rtc_irq_handler);
	SET_IDT_ENTRY(idt[44],&mouse_irq_handler);


	




	
	

}
void de_handler()
{
	cli();
	printf("divide by zero exception");
	sti();

}
void db_handler()
{
	cli();
	printf("db exception");
	sti();
}
void nmi_handler()
{
	cli();
	printf("nmi exception");
	sti();
}
void bp_handler()
{
	cli();
	printf("bp exception");
	sti();
}
void of_handler()
{
	cli();
	printf("of exception");
	sti();
}
void br_handler()
{
	cli();
	printf("br exception");
	sti();
}
void ud_handler()
{
	cli();
	printf("ud exception");
	sti();
}
void nm_handler()
{
	cli();
	printf("nm exception");
	sti();

}
void df_handler()
{
	cli();
	printf("df exception");
	sti();
}
void co_segment_overrun_handler()
{
	cli();
	printf("co_segment_overrun exception");
	sti();
}
void ts_handler()
{
	cli();
	printf("ts exception");
	sti();
}
void np_handler()
{
	cli();
	printf("np exception");
	sti();
}
void ss_handler()
{
	cli();
	printf("ss exception");
	sti();
}
void gp_handler()
{
	cli();
	printf("gp exception");
	sti();
}
void pf_handler()
{
	cli();
	printf("pf exception");
	sti();
}
void mf_handler()
{
	cli();
	printf("mf exception");
	sti();
}
void ac_handler()
{
	cli();
	printf("ac exception");
	sti();
}
void mc_handler()
{
	cli();
	printf("mc exception");
	sti();
}
void xf_handler()
{
	cli();
	printf("xf exception");
	sti();
}
void pit_irq_handler()
{
	cli();
	printf("pit irq handler ");
	sti();
}


void mouse_irq_handler()
{
		cli();
	printf("fake mouse");
	sti();
}