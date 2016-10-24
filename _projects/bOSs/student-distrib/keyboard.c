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
 	left_shift = right_shift = alt = ctrl = typingLine = false;
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
 	data = ((capslock<<2)&4) | ((numlock<<1)&2) | (scrolllock&1);

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
 	//printf("key_handler\n");
 	/* Byte received from keyboard */
 	uint8_t keyboard_scancode;
 	uint8_t keyboard_character;
 	uint8_t keyboard_data;
 	uint32_t fd=0;
 	/* MSB denotes whether key is pressed, bits 0-6 denote the make code */
 	keyboard_data = inb(KEYBOARD_ENCODER_IN_BUF);
 	keyboard_scancode = keyboard_data & KEY_STATE_MASK;
 	
 	/* MSB=0 denotes key press, MSB=1 denotes key released */
 	/* Key is pressed */
 	if(!(keyboard_data & 0x80)){
 		if(keyboard_scancode == KEYBOARD_LEFT_SHIFT)
 			left_shift = true;
 		else if(keyboard_scancode == KEYBOARD_RIGHT_SHIFT)
 			right_shift = true;
 		else if(keyboard_scancode == KEYBOARD_CAPS_LOCK)
 			capslock = !capslock;
 		else if(keyboard_scancode == KEYBOARD_ALT)
 			alt = true;
 		else if(keyboard_scancode == KEYBOARD_NUM_LOCK)
 			numlock = !numlock;
 		else if(keyboard_scancode == KEYBOARD_SCROLL_LOCK)
 			scrolllock = !scrolllock;
 		else if(keyboard_scancode == KEYBOARD_LEFT_CONTROL)
 			ctrl = true;
 		else if(keyboard_scancode == KEYBOARD_ENTER){
 			line_buffer[line_buffer_index] = KEYBOARD_LINEFEED;
 			//line_buffer_index++;
 			//line_buffer_index %= 128; // 0<line_buffer_index<127
 			typingLine = false;
 			if(getY() == 24){
 				scroll();
 			 	move_csr(0,getY());					
 			}
 			else
			 	move_csr(0,getY()+1);	


 			//if(typingLine)
 			//	typingLine=0;
 			//typingLine=0;
 			//move_csr(0,0);
 			//line_buffer_index=0;
 		}
 		else if(keyboard_scancode == KEYBOARD_BACKSPACE){
 			if(line_buffer_index > 0){
 				delete_char();
 				line_buffer_index--;
 			}
 		}
 		else if(ctrl && keyboard_chars[keyboard_scancode] == 'l'){
 				clear();
 				move_csr(0,0);
 				while(line_buffer_index > 0)
 					line_buffer[line_buffer_index--] = '\0';
 				line_buffer_index = 0;
 		}
 		/* else if(line_buffer_index < 127){
	 			// Grab the proper character, depending on the modifier keys.
			 	if(capslock ^ (left_shift | right_shift))
			 		keyboard_character = shift_keyboard_chars[keyboard_scancode];
				else 
				 	keyboard_character = keyboard_chars[keyboard_scancode];
				line_buffer[line_buffer_index++] = keyboard_character;
				putc(keyboard_character);
		} */
 		else{
 			if(line_buffer_index < 127){
	 			/* Grab the proper character, depending on the modifier keys. */
			 	/*if(capslock ^ (left_shift | right_shift))
			 		keyboard_character = shift_keyboard_chars[keyboard_scancode];
				*/
			 	if (capslock & (left_shift | right_shift))
			 	{
			 			keyboard_character = caps_lock_and_shift[keyboard_scancode];
			 	}
			 	else if (left_shift | right_shift)
			 	{
			 			keyboard_character = shift_keyboard_chars[keyboard_scancode];
			 	}	
			 	else if (capslock)
			 	{
			 			keyboard_character = caps_lock_chars[keyboard_scancode];
			 	}
			 	else
			 	{
			 			keyboard_character = keyboard_chars[keyboard_scancode];
			 	}
			 	line_buffer[line_buffer_index] = keyboard_character;
			 	terminal_write(fd, line_buffer + line_buffer_index, 1);	
			 	if(getX() == 79 && getY()==24)
			 		scroll();
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
 	
	//terminal_read();
 	/* Send End-of-Interrupt */
 	//terminal_read(fd,&line_buffer,line_buffer_index);
 	//terminal_write(fd,&line_buffer,line_buffer_index);

 	send_eoi(KEYBOARD_IRQ);

 	//*((char *)0xB8000) = keyboard_character;
	//putc(keyboard_character);
 	/* Unmask interrupt */
 	asm volatile("leave;iret;");
 }

int32_t terminal_open(const uint8_t* filename){
	return 0;
}

int32_t terminal_close(int32_t fd){
	return 0;
}

int32_t terminal_read(int32_t fd, unsigned char* buf, int32_t nbytes){
	//printf("I'm reading from the terminal now");
	char* terminal_buffer = (char*)buf;
	if(terminal_buffer==NULL)
		return -1;
	/* Wait until enter key has been pressed */
	while(typingLine);
	typingLine = true;
	if(nbytes > line_buffer_index+1)
		nbytes = line_buffer_index+1;
	/* Copy from line_buffer into terminal_buffer */
	int i;
	for(i=0; i<nbytes; i++){
		terminal_buffer[i] = line_buffer[i];
	}
	/* Reset cursor position, line_buffer index */
 	line_buffer_index=0;
	return (strlen((int8_t*)terminal_buffer));
}

int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes){
	//printf("I'm writing to the terminal now");
	volatile char* terminal_buffer = (char*)buf;
	if(terminal_buffer==NULL)
			return -1;
	//move_csr(0,24);
	/* Write to screen */
	cli();
	int bytes_written=0;
	int i;
	for(i=0; i<nbytes; i++){
		putc(terminal_buffer[i]);
		bytes_written++;
	}
	move_csr(getX(),getY());
	sti();

	return bytes_written;
}
