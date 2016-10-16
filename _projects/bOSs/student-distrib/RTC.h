#ifndef _RTC_H
#define _RTC_H

#include "i8259.h"
#include "lib.h"

// Define MACROS
#define REGISTER_A	0x8A
#define REGISTER_B 	0x8B
#define REGISTER_C	0x8C

#define NMI			0x70
#define CMOS		0x71

#define MASKBIT6	0x40
#define IRQ_NUM		8		

// Define RTC functions
extern void RTC_init();
extern void rtc_irq_handler();

#endif 
