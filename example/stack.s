.text
	.globl _start
_start:
/*	push	$0xdeadc0de

	movl	$_data, %ebx
	movl	$0x0, %eax
	push	%eax

	push (%eax)

	movl	(%eax), %eax
	movl	%eax, (%ebx)

	int3
_data:
	.long 0x41414141*/

	push	$0xdeadc0de

	movl	$0, %eax
	push	1(%eax)

	movl	$_data, %ebx

	pop	(%ebx)

	pop	%ecx

	jmp	func

_data:
	.long 0x41414141

func:
	movl	$1, %eax
	movl	$0, %ebx
	int	$0x80
