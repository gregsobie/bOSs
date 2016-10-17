#include "lib.h"
#include "paging.h"
#define VIDEO 0xB8000


void init_kernel_pd(){
	int i;
	for(i = 0; i < 1024; i++)
	{
	    kernel_page_directory[i] = FLAG_WRITE_ENABLE;
	}
	for(i = 0; i < 1024; i++)
	{
	    video_page_table[i] = FLAG_WRITE_ENABLE;
	}
	video_page_table[VIDEO >> 12] = FLAG_WRITE_ENABLE | FLAG_PRESENT | VIDEO;
	kernel_page_directory[0] =  (uint32_t)video_page_table | FLAG_WRITE_ENABLE | FLAG_PRESENT;
	kernel_page_directory[1] = 0x00400000 | FLAG_4MB_PAGE | FLAG_WRITE_ENABLE | FLAG_PRESENT | FLAG_GLOBAL;
}

void enablePaging(){
	asm volatile(
					"mov %%cr4, %%eax;"
					"orl $0x00000010, %%eax;"
					"mov %%eax, %%cr4;"
					"mov %%cr0, %%eax;"
				 	"orl $0x80000000, %%eax;"
					"mov %%eax, %%cr0;":::"eax");
}

void loadPageDirectory(unsigned int* pd){
	asm volatile("	mov %0, %%cr3;"::"r" (pd) );

}