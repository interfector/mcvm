.text
	.globl _start
_start:
	movl	$label, %eax
	jmp	(%eax)
	nop
	push	%eax
	nop
	int3
label:
	.long 0x0000000a
	
