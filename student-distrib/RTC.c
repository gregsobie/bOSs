#include "RTC.h"

void RTC_init()
{
	// Initialize rate variable
	unsigned char rate;
	// Initialize 2 Hz frequency by setting rate to 15
	// frequency = 32768 >> (rate-1)
	// 			 = 32768 >> (15-1) = 2 Hz
	rate = RTC_DEFAULT_RATE;

	// Disable interrupts
	//cli();

	/* Turn on IRQ 8 */
	// Disable NMI and select Register B
	outb(REGISTER_B, NMI);
	// Read value of Register B
	char val = inb(CMOS);
	// Set index
	outb(REGISTER_B, NMI);
	// Turn on bit 6 of Register B
	outb(CMOS, val | MASKBIT6);

	/* Set Default Interrupt Rate */
	// Disable NMI and select Register A
	outb(REGISTER_A, NMI);
	// Read value of Register A
	val = inb(CMOS);
	// Set index
	outb(REGISTER_A, NMI);
	// Write rate into Register A (bottom 4 bits)
	outb((val & RATEMASK) | rate, CMOS);

	// Enable interrupts
	//sti();
}

void rtc_irq_handler()
{
	// Disable interrupts
	//cli();

	// Disable NMI and select Register C
	outb(REGISTER_C, NMI);
	// Throw away contents
	inb(CMOS);
	
	test_interrupts();
	// Send end-of-interrupt signal
	send_eoi(IRQ_NUM);

	// Enable interrupts
	//sti();
}
