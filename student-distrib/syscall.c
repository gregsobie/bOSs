#include "syscall.h"
#include "filesystem.h"
#include "lib.h"
asmlinkage int32_t halt (uint8_t status){
	printf("halt\n");
	//Change paging back
	//Change TSS
	//change esp/ebp
	//close any FDs that need it
	//jump to halt_ret_label in exec
	//goto halt_ret_label;
	return 0;
}
asmlinkage int32_t execute (const uint8_t* command){
	printf("exec\n");
	//Parse args
	/*dentry_t d;
	read_dentry_by_name (command, &d);
	uint8_t head[40];
	read_data (d.inode, 0, head, 40);
	if(head[0] != 0x7f || head[1] != 0x45 || head[2] != 0x4c || head[3] != 0x46){
		return -1;
	}
	//while we're here, grab the EIP (?endian)
	//Change paging
	//Load program into program area
	halt_ret_label:*/
	return 0;
}
asmlinkage int32_t read (int32_t fd, void* buf, int32_t nbytes){
	printf("read\n");
	return 0;
}
asmlinkage int32_t write (int32_t fd, const void* buf, int32_t nbytes){
	printf("write\n");
	return 0;
}
asmlinkage int32_t open (const uint8_t* filename){
	printf("open\n");
	return 0;
}
asmlinkage int32_t close (int32_t fd){
	printf("close\n");
	return 0;
}
asmlinkage int32_t getargs (uint8_t* buf, int32_t nbytes);
asmlinkage int32_t vidmap (uint8_t** screen_start);
asmlinkage int32_t set_handler (int32_t signum, void* handler_address);
asmlinkage int32_t sigreturn (void);
