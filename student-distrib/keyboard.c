/* keyboard.c - Functions to interact with the keyboard
 * command controller and keyboard encoder controller
 */

#include "keyboard.h"
#include "lib.h"
#include "x86_desc.h"

bool numlock, scrolllock, capslock, shift, alt, ctrl;

/* Prepares driver for use */
 void keyboard_install (int irq){
 	/* Installs interrupt handler */
 	//SET_IDT_ENTRY(idt[33], key_irq_handler);

 	/* Set lock keys and LED lights */
 	numlock = scrolllock = capslock = false;
 	keyboard_set_leds(numlock, scrolllock, capslock);

 	/* Set shift, ctrl, and alt keys */
 	shift = alt = ctrl = false;
 }

/* Read status from keyboard controller */
 uint8_t keyboard_ctrl_read_status (){
 	return inb(KEYBOARD_CTRL_STATUS_REG);
 }

 /* Send command byte to keyboard controller */
 void keyboard_ctrl_send_cmd (uint8_t cmd){
 	/* Wait for cleared keyboard controller input buffer */
 	while(1)
 		if((keyboard_ctrl_read_status() & KEYBOARD_CTRL_STATUS_MASK_IN_BUF) == 0)
 			break;
 	/* Send command byte to keyboard controller */
 	outb(cmd, KEYBOARD_CTRL_CMD_REG);
 }

  /* Read keyboard encoder buffer */
 uint8_t keyboard_encoder_read_buf (){
 	return inb(KEYBOARD_ENCODER_IN_BUF);
 }

 /* Send command byte to keyboard encoder */
 void keyboard_encoder_send_cmd (uint8_t cmd){
 	/* Wait for cleared keyboard controller input buffer --
 		this is because cmds sent to keyboard encoder are
 		sent to the keyboard controller first */
 	while(1)
 		if((keyboard_ctrl_read_status() & KEYBOARD_CTRL_STATUS_MASK_IN_BUF) == 0)
 			break;
 	/* Send command byte to keyboard encoder */
 	outb(cmd, KEYBOARD_ENCODER_CMD_REG);
 }

 /* Set LEDs (0-OFF, 1-ON):
  * 	Bit 0: Scroll lock LED
  * 	Bit 1: Num lock LED
  * 	Bit 2: Caps lock LED 
  */
 void keyboard_set_leds(bool numlock, bool scrolllock, bool capslock){
 	uint8_t data = 0;

 	/* Set or clear LEDs */
 	data |= (capslock&4) | (numlock&2) | (scrolllock&1);

 	/* Send the command -- update keyboard LEDS */
 	keyboard_encoder_send_cmd (KEYBOARD_ENCODER_CMD_SET_LED);
 	keyboard_encoder_send_cmd (data);
 }

/* Keyboard interrupt handler -- called whenever
	scan codes are sent to the controller */
 void key_irq_handler(){
 	/* Check scan codes */
 	//keyboard_ctrl_read_status
 }
