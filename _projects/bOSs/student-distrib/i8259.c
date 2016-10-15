/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7 */
uint8_t slave_mask; /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void
i8259_init(void)
{
	master_mask = 0x00;
	slave_mask = 0x00;
	outb(ICW1,MASTER_8259_COMMAND);
	outb(ICW2_MASTER,MASTER_8259_DATA);
	outb(ICW3_MASTER,MASTER_8259_DATA);
	outb(ICW4,MASTER_8259_DATA);

	outb(ICW1,SLAVE_8259_COMMAND);
	outb(ICW2_SLAVE,SLAVE_8259_DATA);
	outb(ICW3_SLAVE,SLAVE_8259_DATA);
	outb(ICW4,SLAVE_8259_DATA);

	outb(master_mask,MASTER_8259_DATA);
	outb(slave_mask,SLAVE_8259_DATA);

}

/* Enable (unmask) the specified IRQ */
void
enable_irq(uint32_t irq_num)
{
	if(irq_num < 8){
		master_mask |= (1 << irq_num);
		outb(MASTER_8259_DATA,master_mask);
	}else{
		slave_mask |= (1 << (irq_num-8));
		outb(SLAVE_8259_DATA,slave_mask);
	}
}

/* Disable (mask) the specified IRQ */
void
disable_irq(uint32_t irq_num)
{
	if(irq_num < 8){
		master_mask &= ~(1 << irq_num);
		outb(MASTER_8259_DATA,master_mask);
	}else{
		slave_mask &= ~(1 << (irq_num-8));
		outb(SLAVE_8259_DATA,slave_mask);
	}
}

/* Send end-of-interrupt signal for the specified IRQ */
void
send_eoi(uint32_t irq_num)
{
	if(irq_num >= 8){
		outb(SLAVE_8259_COMMAND,EOI |  (1 << (irq_num-8)));
		outb(MASTER_8259_COMMAND,EOI |  (1 << ICW3_SLAVE));
	}else{
		outb(MASTER_8259_COMMAND,EOI |  (1 << irq_num));

	}
}

