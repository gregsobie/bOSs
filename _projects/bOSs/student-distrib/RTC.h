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
#define RATEMASK	0xF0
#define IRQ_NUM		8
#define RTC_DEFAULT_RATE	0x0F		

// Define RTC functions
extern void RTC_init();
extern void rtc_irq_handler();
extern int32_t RTC_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t RTC_open(const uint8_t* filename);
extern int32_t RTC_write(int32_t fd, const void* buf, int32_t nbytes);
extern int32_t RTC_close(int32_t fd);

#endif
