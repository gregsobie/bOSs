#include "lib.h"
#include "paging.h"

/*
initialize the 4 mb kernel page 

*/
void init_kernel_pd(){
	int i;
	//iterate over all entries in the page directory
	for(i = 0; i < DIRECTORY_SIZE; i++)
	{
		//set these bits as able to write to/present
	    kernel_page_directory[i] = FLAG_WRITE_ENABLE;
	}
	for(i = 0; i < TABLE_SIZE; i++)
	{
		//iterate over the size of the table
		//set the vid mem page
	    video_page_table[i] = FLAG_WRITE_ENABLE;
	}


	/*
	alter all the entries to point to their proper table / page 

	*/
	video_page_table[VIDEO >> 12] = FLAG_WRITE_ENABLE | FLAG_PRESENT | VIDEO;
	kernel_page_directory[0] =  (uint32_t)video_page_table | FLAG_WRITE_ENABLE | FLAG_PRESENT;
	kernel_page_directory[1] = 0x00400000 | FLAG_4MB_PAGE | FLAG_WRITE_ENABLE | FLAG_PRESENT | FLAG_GLOBAL;
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
