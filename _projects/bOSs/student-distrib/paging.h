#ifndef _PAGING_H
#define _PAGING_H

extern void loadPageDirectory(unsigned int*);
extern void enablePaging();

void init_kernel_pd();

uint32_t kernel_page_directory[1024] __attribute__((aligned(4096)));
uint32_t video_page_table[1024] __attribute__((aligned(4096)));

#define FLAG_PRESENT       0x01
#define FLAG_WRITE_ENABLE  0x02
#define FLAG_USER          0x04
#define FLAG_WRITE_THROUGH 0x08
#define FLAG_CACHE_DISABLE 0x10
#define FLAG_ACCESSED      0x20
#define FLAG_4MB_PAGE      0x80
#define FLAG_GLOBAL        0x100

#endif
