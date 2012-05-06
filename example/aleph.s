.text
	.globl _start
_start:
	jmp	l2
l1:
	pop	%ecx

	movl	$4, %eax
	movl	$1, %ebx
	movl	$13, %edx
	int	$0x80

	movl	$1, %eax
	movl	$0, %ebx
	int	$0x80
l2:
	call	l1
	.string "Hello World!\n"
