#ifndef PAGING_H
#define PAGING_H

#include "types.h"

#define USER_MEM_LOCATION  (128 * 0x00100000)
#define USER_PROG_LOCATION ((128 * 0x00100000) + 0x048000)
#define MAX_USER_PROG      0x06

#define FLAG_PRESENT       0x01
#define FLAG_WRITE_ENABLE  0x02
#define FLAG_USER          0x04
#define FLAG_WRITE_THROUGH 0x08
#define FLAG_CACHE_DISABLE 0x10
#define FLAG_ACCESSED      0x20
#define FLAG_4MB_PAGE      0x80
#define FLAG_GLOBAL        0x100
#define TABLE_SIZE         1024
#define DIRECTORY_SIZE     1024

extern void loadPageDirectory(uint32_t *);
extern void enablePaging();

void init_kernel_pd();

uint32_t kernel_page_directory[1024] __attribute__((aligned(4096)));
uint32_t video_page_table[1024] __attribute__((aligned(4096)));
uint32_t proc_page_directory[MAX_USER_PROG][1024]__attribute__((aligned(4096)));
static  uint8_t proc_id_used[MAX_USER_PROG] = {false};


#endif
