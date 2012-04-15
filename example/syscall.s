.text
	.globl _start
_start:
	movl $0x2, %eax
	int	$0x80

	movl $0x1, %eax
	movl	$0x0, %ebx
	int	$0x80
