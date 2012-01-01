.text
	.globl _start
_start:
	movl $0x5, %eax
	movl $_path, %ebx
	movl $0x41, %ecx
	movl $420, %edx
	int	$0x80

	movl %eax, %esi

	movl $0x4, %eax
	movl %esi, %ebx
	movl $_data, %ecx
	movl $13, %edx

	int	$0x80

	movl $0x6, %eax
	movl	%esi, %ebx
	int	$0x80

	movl $0x1, %eax
	movl	$0x0, %ebx
	int	$0x80
_data:
	.string "Hello World!"
_path:
	.string "./file"
