#include "lib.h"
#include "paging.h"
#include "syscall.h"

/*
initialize the 4 mb kernel page 

*/
void init_kernel_pd(){
	int i,i2;
	//iterate over all entries in the page directory
	for(i = 0; i < DIRECTORY_SIZE; i++)
	{
		//set these bits as able to write to/present
	    kernel_page_directory[i] = FLAG_WRITE_ENABLE;
	}
	for(i = 0; i < 3; i++){

		for(i2 = 0; i2 < TABLE_SIZE; i2++)
		{
			//iterate over the size of the table
			//set the vid mem page
		    video_page_tables[i][i2] = FLAG_WRITE_ENABLE;
		}
		video_page_tables[i][VIDEO >> 12] = VIDEO | FLAG_USER | FLAG_WRITE_ENABLE | FLAG_PRESENT;

		/* Set the next three entries in the video page table to support multiple terminal functionality */
		video_page_tables[i][(VIDEO >> 12)+1] = (VIDEO + _4KB) | FLAG_USER | FLAG_WRITE_ENABLE | FLAG_PRESENT;
		video_page_tables[i][(VIDEO >> 12)+2] = (VIDEO + _4KB*2) | FLAG_USER | FLAG_WRITE_ENABLE | FLAG_PRESENT;
		video_page_tables[i][(VIDEO >> 12)+3] = (VIDEO + _4KB*3) | FLAG_USER | FLAG_WRITE_ENABLE | FLAG_PRESENT;
	}
	video_page_tables[0][(VIDEO >> 12)+1] = FLAG_WRITE_ENABLE | FLAG_PRESENT | VIDEO;

	/* Alter all entries to point to their proper table / page */
	kernel_page_directory[0] =  (uint32_t)video_page_tables[0] | FLAG_WRITE_ENABLE | FLAG_PRESENT;
	kernel_page_directory[1] = PAGE_4MB | FLAG_4MB_PAGE | FLAG_WRITE_ENABLE | FLAG_PRESENT | FLAG_GLOBAL;





	// /* Setup user video memory and initialize the first entry */
	// proc_page_directory[0][(int)(USER_VIDEO/PAGE_4MB)] = ((unsigned int)proc_video_tables) | FLAG_USER | FLAG_WRITE_ENABLE | FLAG_PRESENT;
	// for(i = 0; i < TABLE_SIZE; i++)
	//     proc_video_tables[0][i]=0;
	// proc_video_tables[0][0] = VIDEO | FLAG_USER | FLAG_WRITE_ENABLE | FLAG_PRESENT;
}



/*
Enables hi bit of cr0 to enable paging, 
enables bit 4 of cr4 to enable PSE 
*/
void enablePaging(){
	asm volatile(
		"mov %%cr4, %%eax;"
		"orl $0x00000010, %%eax;"
		"mov %%eax, %%cr4;"
		"mov %%cr0, %%eax;"
	 	"orl $0x80000000, %%eax;"
		"mov %%eax, %%cr0;":::"eax");
}



/*
loads the pointer of the page directory to link
to page directory 

*/
void loadPageDirectory(unsigned int* pd){
	asm volatile("	mov %0, %%cr3;"::"r" (pd) );

}


uint32_t is_kernel_ptr(const void * ptr){
	return (uint32_t)ptr < 0x00800000; 
}

/* 
 * INPUTS: inactiveTerminal - buffer to assign user video memory
 * OUTPUTS: None
 * Sets user video memory to respective terminal buffer, so that
 * the data is not lost when it becomes active again
 */
void deactivateVideo(uint32_t inactiveTerminal){
	// //printf("deactive\n");
	// proc_video_tables[0][0] = inactiveTerminal | FLAG_USER | FLAG_WRITE_ENABLE | FLAG_PRESENT;
	// //loadPageDirectory(&proc_page_directory[0][0]);

	// PCB_t * current;
	// cur_pcb(current);
	// video_page_table[(VIDEO>>22) + current->terminal_id] = inactiveTerminal | FLAG_WRITE_ENABLE | FLAG_PRESENT | FLAG_USER;
}

/* 
 * INPUTS: None
 * OUTPUTS: None
 * Sets user video memory to physical video memory. Note that user video
 * memory has already been populated with the new terminal's data before
 * this function is called
 */
void activateVideo(){
	// //printf("active\n");
	// proc_video_tables[0][0] = VIDEO | FLAG_USER | FLAG_WRITE_ENABLE | FLAG_PRESENT;
	// //loadPageDirectory(&proc_page_directory[0][0]);

	// PCB_t * current;
	// cur_pcb(current);
	// video_page_table[(VIDEO>>22) + current->terminal_id] = VIDEO | FLAG_WRITE_ENABLE | FLAG_PRESENT | FLAG_USER;
}

