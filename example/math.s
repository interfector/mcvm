.text
	.globl _start
_start:
	movl	$3, %eax
	movl	$0x00000102, %ebx

	mul	%bl

	movl	%eax, %ecx

	movl	$1, %eax
	movl	$0, %ebx
	int	$0x80
