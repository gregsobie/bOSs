// #define ASM 1
// #include "syscall_linkage.h"

// .globl systemcall_linkage
// .globl bad

// .text 
// .align 4

// systemcall_linkage:



// 	cmpl $1, %eax
// 	jl bad
// 	cmpl $10, %eax
// 	jg bad

// 	pushl %ebp 
// 	pushl %edi
// 	pushl %esi 
// 	pushl %edx
// 	pushl %ecx
// 	pushl %ebx 

// 	call *syscall_jumptable(,%eax,4)

// 	addl $12, %esp 

// 	popl %ebx
// 	popl %ecx
// 	popl %edx
// 	popl %esi
// 	popl %edi
// 	popl %ebp

// 	iret 

// bad:
// 	movl $-1, %eax
	 
// 	iret 



// syscall_jumptable:
// 	.long 0x0, halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn




