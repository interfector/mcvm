.text
	.globl _start
_start:
	movl	$0x4, %eax
	movl	$1, %ebx
	movl	$_msg, %ecx
	movl	$33, %edx
	int	$0x80

	movl	$0x3, %eax
	movl	$0, %ebx
	movl	$_data, %ecx
	movl	$100, %edx
	int	$0x80

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
_msg:
	.string "Inserisci il contenuto del file: "
_path:
	.string "./myfile"
_data:
	.rept 100
	nop
	.endr
