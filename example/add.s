.text
	.globl _start
_start:
	movl	$1, %eax

	add	%al, (%eax)

	.byte 0xeb
	.byte 0xf7
/*	jmp	l1
l2:
	int	$0x80
l1:
	movl	$0, %ebx
	jmp	l2*/
