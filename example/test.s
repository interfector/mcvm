.text
	.globl _start
_start:
	movl $0x3, %eax
	movl $0x0, %ebx
	movl $_data, %ecx
	movl $12, %edx
	int	$0x80

	movl $0x4, %eax
	movl $0x1, %ebx
	movl $_data, %ecx
	movl $12, %edx

	int	$0x80

	movl $0x1, %eax
	movl	$0x0, %ebx
	int	$0x80

_data:
	.rept 100
		nop
	.endr
