.text
	.globl _start
_start:
	movl	$label, %eax
	jmp	(%eax)
	nop
	push	%eax
	nop
	int3
#jmp l2
label:
	.long 0x0000000a
/*
l2:
	nop
	nop
	movl	$1, %eax
	jmp	l3
	.byte 0x66
l3:
	movl	$0, %ebx
	int	$0x80
*/	
