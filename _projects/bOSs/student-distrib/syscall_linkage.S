
#define ASM     1
//#include "syscall.h"

.global syscall_linkage

syscall_linkage:

//check eax, set up stack, set up error branch
cmpl $1, %eax
jl error
cmpl $12, %eax
ja error

pushl %ebp
pushl %edi
pushl %esi 
pushl %edx
pushl %ecx
pushl %ebx 

call *syscall_jumptable(,%eax,4)

addl $12, %esp #pop off args

popl %ebx
popl %ecx
popl %edx
popl %esi
popl %edi
popl %ebp

iret 

error:
movl $-1, %eax
 
iret 



syscall_jumptable:
.long halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn


.text