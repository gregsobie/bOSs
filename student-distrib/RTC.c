#include "RTC.h"

volatile int rtc_interrupt_occured;

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
	outb(val | MASKBIT6, CMOS);

	rtc_interrupt_occured = 1;

	/* Set Default Interrupt Rate */
	// Disable NMI and select Register A
	outb(REGISTER_A, NMI);
	// Read value of Register A
	val = inb(CMOS);
	// Set index
	outb(REGISTER_A, NMI);
	// Write rate into Register A (bottom 4 bits)
	outb((val & RATEMASK) | rate, CMOS);

	enable_irq(IRQ_NUM);
	// Enable interrupts
	//sti();
}

void rtc_irq_handler()
{

	// Disable NMI and select Register C
	outb(REGISTER_C, NMI);
	// Throw away contents
	inb(CMOS);
	
	//test_interrupts();
	//*((char *)0xB8001) = 50 ^ *((char *)0xB8001);

	rtc_interrupt_occured = 1;

	// Send end-of-interrupt signal
	send_eoi(IRQ_NUM);

 	asm volatile("leave;iret;");
}

int32_t RTC_open(const uint8_t* filename)
{
	RTC_init();
	return 0;
}

int32_t RTC_read(int32_t fd, void* buf, int32_t nbytes)
{
	cli();
	rtc_interrupt_occured = 0;
	sti();
	while(rtc_interrupt_occured == 0)
	{
		
	}
	//putc('1');
	return 0;
}

int32_t RTC_write(int32_t fd, const void* buf, int32_t nbytes)
{
	if (nbytes == 4)
	{
		unsigned char rate;

		int32_t fq = *(int32_t*)buf;

		if (fq == 1024)
		{
			rate = 0x06;
		}
		else if(fq == 512)
		{
			rate = 0x07;
		}
		else if (fq == 256)
		{
			rate = 0x08;
		}
		else if (fq == 128)
		{
			rate = 0x09;
		}
		else if (fq == 64)
		{
			rate = 0x0A;
		}
		else if (fq == 32)
		{
			rate = 0x0B;
		}
		else if (fq == 16)
		{
			rate = 0x0C;
		}
		else if (fq == 8)
		{
			rate = 0x0D;
		}
		else if (fq == 4)
		{
			rate = 0x0E;
		}
		else if (fq == 2)
		{
			rate = RTC_DEFAULT_RATE;
		}

		// Disable NMI and select Register A
		outb(REGISTER_A, NMI);
		// Read value of Register A
		char val = inb(CMOS);
		// Set index
		outb(REGISTER_A, NMI);
		// Write rate into Register A (bottom 4 bits)
		outb((val & RATEMASK) | rate, CMOS);

		return 0;

	}

	return -1;
}

int32_t RTC_close(int32_t fd)
{
	return 0;
}
