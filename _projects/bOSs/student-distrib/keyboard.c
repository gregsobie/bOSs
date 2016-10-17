/* keyboard.c - Functions to interact with the keyboard
 * command controller and keyboard encoder controller
 */

#include "keyboard.h"
#include "lib.h"
#include "x86_desc.h"
#include "i8259.h"

bool numlock, scrolllock, capslock, left_shift, right_shift, alt, ctrl;

/* Character data corresponding to make codes */
uint8_t keyboard_chars[128] = {
	    0,  0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0,
	 	'q', 'w', 'e', 'r','t', 'y', 'u', 'i', 'o', 'p', '[', ']', 0, 0, 'a', 's',	
	 	'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,'\\', 'z', 'x', 'c', 'v', 
	 	'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ',	 0,
	    0,	/* 59 - F1 key ... > */
	    0,   0,   0,   0,   0,   0,   0,   0,
	    0,	/* < ... F10 */
	    0,	/* 69 - Num lock*/
	    0,	/* Scroll Lock */
	    0,	/* Home key */
	    0,	/* Up Arrow */
	    0,	/* Page Up */
	  '-',
	    0,	/* Left Arrow */
	    0,
	    0,	/* Right Arrow */
	  '+',
	    0,	/* 79 - End key*/
	    0,	/* Down Arrow */
	    0,	/* Page Down */
	    0,	/* Insert Key */
	    0,	/* Delete Key */
	    0,   0,   0,
	    0,	/* F11 Key */
	    0,	/* F12 Key */
	    0,	/* All other keys are undefined */
	};
uint8_t shift_keyboard_chars[128] = {
	    0,  0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0, 0,
	 	'Q', 'W', 'E', 'R','T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0, 0, 'A', 'S',	
	 	'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0,'|', 'Z', 'X', 'C', 'V', 
	 	'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ',	 0,
	    0,	/* 59 - F1 key ... > */
	    0,   0,   0,   0,   0,   0,   0,   0,
	    0,	/* < ... F10 */
	    0,	/* 69 - Num lock*/
	    0,	/* Scroll Lock */
	    0,	/* Home key */
	    0,	/* Up Arrow */
	    0,	/* Page Up */
	  '-',
	    0,	/* Left Arrow */
	    0,
	    0,	/* Right Arrow */
	  '+',
	    0,	/* 79 - End key*/
	    0,	/* Down Arrow */
	    0,	/* Page Down */
	    0,	/* Insert Key */
	    0,	/* Delete Key */
	    0,   0,   0,
	    0,	/* F11 Key */
	    0,	/* F12 Key */
	    0,	/* All other keys are undefined */
	};

/* Prepares driver for use */
 void keyboard_install (int irq){
 	/* Installs interrupt handler */
 	//SET_IDT_ENTRY(idt[33], key_irq_handler);

 	/* Set lock keys and LED lights */
 	numlock = scrolllock = capslock = false;
 	keyboard_set_leds(numlock, scrolllock, capslock);

 	/* Set shift, ctrl, and alt keys */
 	left_shift = right_shift = alt = ctrl = false;
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

  /* Read keyboard encoder input buffer */
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
 	data |= ((capslock<<2)&4) | ((numlock<<1)&2) | (scrolllock&1);

 	/* Send the command -- update keyboard LEDS */
 	keyboard_encoder_send_cmd (KEYBOARD_ENCODER_CMD_SET_LED);
 	keyboard_encoder_send_cmd (data);
 }

/* Keyboard interrupt handler -- called whenever
	scan codes are sent to the controller */
 void key_irq_handler(){
 	/* Check scan codes */
 	//keyboard_ctrl_read_status

 	/* Mask interrupts */
 	cli();

 	/* Byte received from keyboard */
 	uint8_t keyboard_scancode;
 	uint8_t keyboard_character;
 	uint8_t keyboard_data;

 	/* MSB denotes whether key is pressed, bits 0-6 denote the make code */
 	keyboard_data = inb(KEYBOARD_ENCODER_CMD_REG) & 0x7F;
 	keyboard_scancode = keyboard_data & 0x7F;
 	
 	/* MSB=1 denotes key press, MSB=0 denotes key released */
 	if(keyboard_data & 0x80){
 		if(keyboard_scancode == KEYBOARD_LEFT_SHIFT)
 			left_shift = true;
 		else if(keyboard_scancode == KEYBOARD_RIGHT_SHIFT)
 			right_shift = true;
 		else if(keyboard_scancode == KEYBOARD_CAPS_LOCK)
 			capslock = true;
 		else if(keyboard_scancode == KEYBOARD_ALT)
 			alt = true;
 		else if(keyboard_scancode == KEYBOARD_NUM_LOCK)
 			numlock = true;
 		else if(keyboard_scancode == KEYBOARD_SCROLL_LOCK)
 			scrolllock = true;
 		else{
 			/* Grab the proper character, depending on the modifier keys. */
		 	if(capslock ^ (left_shift | right_shift))
		 		keyboard_character = shift_keyboard_chars[keyboard_scancode];
		 	else
		 		keyboard_character = keyboard_chars[keyboard_scancode];
		}
	} else {
 		if(keyboard_scancode == KEYBOARD_LEFT_SHIFT)
 			left_shift = false;
 		else if(keyboard_scancode == KEYBOARD_RIGHT_SHIFT)
 			right_shift = false; 		
 		else if(keyboard_scancode == KEYBOARD_CAPS_LOCK)
 			capslock = false;
 		else if(keyboard_scancode == KEYBOARD_ALT)
 			alt = false;
 		else if(keyboard_scancode == KEYBOARD_NUM_LOCK)
 			numlock = false;
 		else if(keyboard_scancode == KEYBOARD_SCROLL_LOCK)
 			scrolllock = false;
	}
	/* Prints pressed character to display */
 	printf("%c", keyboard_character);

 	/* Send End-of-Interrupt */
 	send_eoi(KEYBOARD_IRQ);

 	/* Unmask interrupt */
 	sti();
 }
