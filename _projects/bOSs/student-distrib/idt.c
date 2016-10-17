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

/*
typedef union idt_desc_t {
	uint32_t val[2];
	struct {
		uint16_t offset_15_00;
		uint16_t seg_selector;
		uint8_t reserved4;
		uint32_t reserved3 : 1;
		uint32_t reserved2 : 1;
		uint32_t reserved1 : 1;
		uint32_t size : 1;
		uint32_t reserved0 : 1;
		uint32_t dpl : 2;
		uint32_t present : 1;
		uint16_t offset_31_16;
	} __attribute__((packed));
} idt_desc_t;
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
			
			idt[i].reserved4 = 0;
			idt[i].reserved3 = 1;
			idt[i].reserved2 = 1;
			idt[i].reserved1 = 1;
			idt[i].size = 1;
			idt[i].reserved0 = 0;
		
		}	
/* place system calls(trap gates) at end of IDT */

	// for (i=(NUM_VEC -12);i<NUM_VEC;i++)
	// {
	// 		idt[i].present = 1;
	// 		idt[i].seg_selector = KERNEL_CS;
	// 		idt[i].dpl = 3;
		
	// 		idt[i].reserved1 = 1;
	// 		idt[i].reserved2 = 1;
	// 		idt[i].reserved3 = 1;
	// 		idt[i].size = 1;
	// 		idt[i].reserved0 = 0;
		
	// 		idt[i].reserved4 = 0;
	// 		//SET_IDT_ENTRY(idt[i],sys_call);
	// }
/*
interrupt handlers
all are interrupt gates w kernel CS and priv = 0 

idt[32] = timer chip
idt[33] = keys
idt[40] = rtc
idt[44] = mouse 
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
	SET_IDT_ENTRY(idt[45],&pic_handler);


	




	
	

}
void de_handler()
{
	cli();
	printf("divide by zero exception");
		while(1)
	 {

	 }
	sti();

}
void db_handler()
{
	cli();
	printf("db exception");
		while(1)
	 {

	 }
	sti();
}
void nmi_handler()
{
	cli();
	printf("nmi exception");
		while(1)
	 {

	 }
	sti();
}
void bp_handler()
{
	cli();
	printf("bp exception");
		while(1)
	 {

	 }
	sti();
}
void of_handler()
{
	cli();
	printf("of exception");
	while(1)
	 {

	 }
	sti();

}
void br_handler()
{
	cli();
	printf("br exception");
		while(1)
	 {

	 }
	sti();
}
void ud_handler()
{
	cli();
	printf("ud exception");
		while(1)
	 {

	 }
	sti();
}
void nm_handler()
{
	cli();
	printf("nm exception");
		while(1)
	 {

	 }
	sti();

}
void df_handler()
{
	cli();
	printf("df exception");
		while(1)
	 {

	 }
	sti();
}
void co_segment_overrun_handler()
{
	cli();
	printf("co_segment_overrun exception");
		while(1)
	 {

	 }
	sti();
}
void ts_handler()
{
	cli();
	printf("ts exception");
		while(1)
	 {

	 }
	sti();
}
void np_handler()
{
	cli();
	printf("np exception");
		while(1)
	 {

	 }
	sti();
}
void ss_handler()
{
	cli();
	printf("ss exception");
		while(1)
	 {

	 }
	sti();
}
void gp_handler()
{
	cli();
	printf("gp exception");
		while(1)
	 {

	 }
	sti();
}
void pf_handler()
{
	cli();
	printf("pf exception");
		while(1)
	 {

	 }
	sti();
}
void mf_handler()
{
	cli();
	printf("mf exception");
		while(1)
	 {

	 }
	sti();
}
void ac_handler()
{
	cli();
	printf("ac exception");
		while(1)
	 {

	 }
	sti();
}
void mc_handler()
{
	cli();
	printf("mc exception");
		while(1)
	 {

	 }
	sti();
}
void xf_handler()
{
	cli();
	printf("xf exception");
		while(1)
	 {

	 }
	sti();
}
void pit_irq_handler()
{
	cli();
	printf("pit irq handler ");
		while(1)
	 {

	 }
	sti();
}


void mouse_irq_handler()
{
		cli();
	printf("fake mouse");
		while(1)
	 {

	 }
	sti();
}
void pic_handler()
{
	cli();
	printf("this is the pic_handler");
		while(1)
	 {

	 }
	sti();
}