/* keyboard.c - Functions to interact with the keyboard
 * command controller and keyboard encoder controller
 */

#include "keyboard.h"
#include "lib.h"
#include "x86_desc.h"
#include "i8259.h"

bool numlock, scrolllock, capslock, left_shift, right_shift, alt, ctrl;
volatile bool typingLine;

/* Character data corresponding to make codes, where
 * Zero denotes an unprintable character */
uint8_t keyboard_chars[128] = {
	    0,  0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0,
	 	'q', 'w', 'e', 'r','t', 'y', 'u', 'i', 'o', 'p', '[', ']', 0, 0, 'a', 's',	
	 	'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,'\\', 'z', 'x', 'c', 'v', 
	 	'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ',	 0,
	    0,	0,   0,   0,   0,   0,   0,   0,   0, 0, 0,	0, 0, 0, 0,	/* Page Up */
	  	'-', 0,	0, 0, '+', 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	/* Remaining keys undefined */
	};
/* Capital letters and symbols */
uint8_t shift_keyboard_chars[128] = {
	    0,  0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0, 0,
	 	'Q', 'W', 'E', 'R','T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0, 0, 'A', 'S',	
	 	'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0,'|', 'Z', 'X', 'C', 'V', 
	 	'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0, 0,	0, 0, 0, 0, 0, 0, 0, 0, 0,
	    0, 0, 0, 0, 0, '-', 0, 0, 0,'+', 0, 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, /* Remaining keys undefined */
	};
/* Capital letters and numbers */
uint8_t caps_lock_chars[128] =  {
	    0,  0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0,
	 	'Q', 'W', 'E', 'R','T', 'Y', 'U', 'I', 'O', 'P', '[', ']', 0, 0, 'A', 'S',	
	 	'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`', 0,'\\', 'Z', 'X', 'C', 'V', 
	 	'B', 'N', 'M', ',', '.', '/', 0, '*', 0, ' ', 0, 0,	0, 0, 0, 0, 0, 0, 0, 0, 0,
	    0, 0, 0, 0, 0, '-', 0, 0, 0,'+', 0, 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, /* Remaining keys undefined */
	};
/* Lowercase letters and symbols */
uint8_t caps_lock_and_shift[128] =  {
	    0,  0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0, 0,
	 	'q', 'w', 'e', 'r','t', 'y', 'u', 'i', 'o', 'p', '{', '}', 0, 0, 'a', 's',	
	 	'd', 'f', 'g', 'h', 'j', 'k', 'l', ':', '\"', '~', 0,'|', 'z', 'x', 'c', 'v', 
	 	'b', 'n', 'm', '<', '>', '?', 0, '*', 0, ' ', 0, 0,	0, 0, 0, 0, 0, 0, 0, 0, 0,
	    0, 0, 0, 0, 0, '-', 0, 0, 0,'+', 0, 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, /* Remaining keys undefined */
	};

/* Prepares driver for use */
 void keyboard_install (int irq){
 	/* Set shift, ctrl, and alt keys */
 	left_shift = right_shift = alt = ctrl = false;

 	typingLine = true;
 	line_buffer_index=0;
 	/* Set lock keys and LED lights */
 	numlock = scrolllock = capslock = false;
 	keyboard_set_leds(numlock, scrolllock, capslock);
 	cli();
	enable_irq(irq);
	sti();
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
 	data = ((capslock<<2)&MASK_BIT_2) | ((numlock<<1)&MASK_BIT_1) | (scrolllock&MASK_BIT_0);

 	/* Send the command -- update keyboard LEDS */
 	keyboard_encoder_send_cmd (KEYBOARD_ENCODER_CMD_SET_LED);
 	keyboard_encoder_send_cmd (data);
 }

/* Keyboard interrupt handler -- called after each keypress
	
	INPUTS: NONE
	OUTPUTS: NONE
	SIDE EFFECTS: writes to line_buffer, interacts with keyboard ports
	sets various flags. 

	*/
 void key_irq_handler(){
 	/* Check scan codes */
 	//keyboard_ctrl_read_status
 	/* Mask interrupts */
 	//printf("key_handler\n");
 	/* Byte received from keyboard */
 	uint8_t keyboard_scancode;
 	uint8_t keyboard_character;
 	uint8_t keyboard_data;
 	/* MSB denotes whether key is pressed, bits 0-6 denote the make code */
 	keyboard_data = inb(KEYBOARD_ENCODER_IN_BUF);
 	keyboard_scancode = keyboard_data & KEY_STATE_MASK;
 	
 	/* MSB=0 denotes key press, MSB=1 denotes key released */
 	/* Key is pressed */
 	if(!(keyboard_data & KEY_STATUS_MASK)){
 		if(keyboard_scancode == KEYBOARD_LEFT_SHIFT)
 			left_shift = true;
 		else if(keyboard_scancode == KEYBOARD_RIGHT_SHIFT)
 			right_shift = true;
 		else if(keyboard_scancode == KEYBOARD_ALT)
 			alt = true;
 		else if(keyboard_scancode == KEYBOARD_LEFT_CONTROL)
 			ctrl = true;
 		else if(keyboard_scancode == KEYBOARD_CAPS_LOCK)
 			capslock = !capslock;
 		else if(keyboard_scancode == KEYBOARD_NUM_LOCK)
 			numlock = !numlock;
 		else if(keyboard_scancode == KEYBOARD_SCROLL_LOCK)
 			scrolllock = !scrolllock;
 		else if(keyboard_scancode == KEYBOARD_ENTER){
 			/* Input line must end with a linefeed character */
 			line_buffer[line_buffer_index] = KEYBOARD_LINEFEED;
 			typingLine = false;
 			/* If located at max row, move all other entries upward */
 			if(getY() == MAX_ROW_INDEX){
 				scroll();
 			 	move_csr(0,getY());
 			/* Else move current entry to next row */
 			}else
			 	move_csr(0,getY()+1);
		/* Deletes the last typed character and updates the
		 * line input buffer to reflect element's absense */
 		}else if(keyboard_scancode == KEYBOARD_BACKSPACE){
 			if(line_buffer_index > 0){
 				delete_char();
 				line_buffer_index--;
 			}
 		/* Resets the terminal by clearing screen, setting cursor
 		 * to original position, and clearing input line buffer */
 		}else if(ctrl && keyboard_chars[keyboard_scancode] == 'l'){
 				clear();
 				move_csr(0,0);
 				while(line_buffer_index > 0)
 					line_buffer[line_buffer_index--] = '\0';
 				line_buffer_index = 0;
 		}else{
 			/* If input line buffer is not full */
 			if(line_buffer_index < MAX_BUF_INDEX){
	 			/* Grab the proper character, depending on the modifier keys. */
	 			/* Both caps lock and shift are active */
			 	if(capslock & (left_shift | right_shift)){
			 			keyboard_character = caps_lock_and_shift[keyboard_scancode];
			 	/* Shift key is active */
			 	}else if(left_shift | right_shift){
			 			keyboard_character = shift_keyboard_chars[keyboard_scancode];
			 	/* Caps lock is active */
			 	}else if(capslock){
			 			keyboard_character = caps_lock_chars[keyboard_scancode];
			 	/* Neither caps lock or shift key are active */
			 	}else{
			 			keyboard_character = keyboard_chars[keyboard_scancode];
			 	}
			 	/* Store character to input line buffer and echo to terminal */
			 	line_buffer[line_buffer_index] = keyboard_character;
			 	terminal_write(NULL, line_buffer + line_buffer_index, BYTE_PER_CHAR);
			 	/* If X is at max column and Y is at max row,
			 	 * shift all elements upward by one row */
			 	if(getX() == MAX_COL_INDEX && getY()==MAX_ROW_INDEX)
			 		scroll();
			 	/* Another key has been pressed */
			 	line_buffer_index++;
			}
 		}
	/* Key is released */
	} else {
 		if(keyboard_scancode == KEYBOARD_LEFT_SHIFT)
 			left_shift = false;
 		else if(keyboard_scancode == KEYBOARD_RIGHT_SHIFT)
 			right_shift = false; 		
 		else if(keyboard_scancode == KEYBOARD_ALT)
 			alt = false;
 	 	else if(keyboard_scancode == KEYBOARD_LEFT_CONTROL)
 			ctrl = false;
	}
 	
 	/* Send End-of-Interrupt */
 	send_eoi(KEYBOARD_IRQ);

 	//*((char *)0xB8000) = keyboard_character;
 	/* Unmask interrupt */
 	asm volatile("leave;iret;");
 }


/*
INPUTS: filename
OUTPUTS: return 0 on success 

System call for open of file type = terminal 

*/

int32_t terminal_open(struct file * f){
	return -1;
}

/*
INPUTS: file descriptor 
OUTPUTS: return 0 on success 

System call for close of file type = terminal 

*/

int32_t terminal_close(struct file * f){
	return -1;
}


/*
INPUTS: file descriptor, buf, nbytes
OUTPUTS: return amount of bytes written 

takes buffer passed in, and copies from line_buffer into the terminal buffer

*/


int32_t terminal_read(struct file * f, char * buf, uint32_t nbytes){
	char* terminal_buffer = (char*)buf;
	if(terminal_buffer==NULL)
		return -1;
	/* Wait until enter key has been pressed */
	while(typingLine);
	/* Any key pressed after enter should be
	 * a part of the next line */
	typingLine = true;
	/* We will read at most 128 bytes per line */
	if(nbytes > line_buffer_index+1)
		nbytes = line_buffer_index+1;
	/* Copy from line_buffer into terminal_buffer */
	int32_t i;
	for(i=0; i<nbytes; i++){
		terminal_buffer[i] = line_buffer[i];
	}
	terminal_buffer[i] = '\0';
	/* Reset input line buffer index */
	for(i=0; i<nbytes; i++){
		line_buffer[i] = '\0';
	}
 	line_buffer_index=0;
	return (strlen((int8_t*)terminal_buffer));
}


/*
INPUTS: file descriptor, buf, nbytes
OUTPUTS: return amount of bytes written 

takes buffer passed in, and outputs to terminal and update cursor 

*/
int32_t terminal_write(struct file * f, const char* buf, uint32_t nbytes){
	volatile char* terminal_buffer = (char*)buf;
	if(terminal_buffer==NULL)
			return -1;
	int bytes_written=0;
	int i;
	/* Write to screen and update cursor */
	cli();
	for(i=0; i<nbytes; i++){
		if(terminal_buffer[i] == '\0')
			break;
		putc(terminal_buffer[i]);
		bytes_written++;
	}
	move_csr(getX(),getY());
	sti();
	return bytes_written;
}
