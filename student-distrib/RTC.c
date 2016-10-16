#include "RTC.h"

void RTC_init()
{
	// Disable interrupts
	cli();

	// Disable NMI and select Register B
	outb(REGISTER_B, NMI);
	// Read value of Register B
	char val = inb(CMOS);
	// Set index
	outb(REGISTER_B, NMI);
	// Turn on bit 6 of Register B
	outb(CMOS, val | MASKBIT6);

	// Enable interrupts
	sti();
}

void rtc_irq_handler()
{
	// Disable interrupts
	cli();

	// Disable NMI and select Register C
	outb(REGISTER_C, NMI);
	// Throw away contents
	inb(CMOS);
	// Send end-of-interrupt signal
	send_eoi(IRQ_NUM);

	// Enable interrupts
	sti();
}
