/* keyboard.h - Defines used in interactions with the keyboard
 * controller
 */

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "types.h"

/* IO ports for each keyboard controller */
#define KEYBOARD_ENCODER_IN_BUF	 0x60
#define KEYBOARD_ENCODER_CMD_REG 0x60
#define KEYBOARD_CTRL_STATUS_REG 0x64
#define KEYBOARD_CTRL_CMD_REG	 0x64

/* IRQ Constant */
#define KEYBOARD_IRQ 	1

/* Bit masks for R/W to the status register */
#define KEYBOARD_CTRL_STATUS_MASK_OUT_BUF	 0x1 	//0000 0001
#define KEYBOARD_CTRL_STATUS_MASK_IN_BUF	 0x2 	//0000 0010
#define KEYBOARD_CTRL_STATUS_MASK_SYSTEM	 0x4 	//0000 0100
#define KEYBOARD_CTRL_STATUS_MASK_CMD_DATA	 0x8 	//0000 1000
#define KEYBOARD_CTRL_STATUS_MASK_LOCKED	 0x10 	//0001 0000
#define KEYBOARD_CTRL_STATUS_MASK_AUX_BUF	 0x20 	//0010 0000
#define KEYBOARD_CTRL_STATUS_MASK_TIMEOUT	 0x40 	//0100 0000
#define KEYBOARD_CTRL_STATUS_MASK_PARITY	 0x80 	//1000 0000

/* Keyboard encoder commands */
#define KEYBOARD_ENCODER_CMD_SET_LED 		 0xED

/* Stores the current state of certain keys */
extern bool numlock, scrolllock, capslock, shift, alt, ctrl;
extern uint8_t _lastScanCode;

/* Prepares driver for use */
void keyboard_install(int irq);

/* Read status from keyboard controller */
uint8_t keyboard_ctrl_read_status ();

/* Send command byte to keyboard controller */
void keyboard_ctrl_send_cmd(uint8_t cmd);

/* Read keyboard encoder buffer */
uint8_t keyboard_encoder_read_buf ();

/* Send command byte to keyboard encoder */
void keyboard_encoder_send_cmd(uint8_t cmd);

/* Set LEDs */
void keyboard_set_leds(bool numlock, bool scrolllock, bool capslock);

/* Interrupt handler */
void key_irq_handler();

#endif
