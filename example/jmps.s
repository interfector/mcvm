.text
	.globl _start
_start:
	movl	$10, %eax
	jmp	%eax
	nop
	push	%eax
	nop
	int3
