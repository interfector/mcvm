.text
	.globl _start
_start:
	movl	$5, %ecx
	push	$0xdeadc0de
	push	%ecx
	call	func

	movb	$1, %al
	movl	$0, %ebx
	int	$0x80
func:
	push	%ebp
	movl %esp, %ebp
	movl	-8(%ebp), %esi
	movl	-12(%ebp), %edi
	movl	%ebp, %esp
	pop	%ebp
	ret
/*
enter:
	push	%ebp
	movl %esp, %ebp
	sub	$0x4, %esp

ret:
	movl	%ebp, %esp
	pop	%ebp
*/
