#include <stdio.h>

#define SYSHANDLER() void syscall_handler(asm_env* env)
#define SYSCALL(f) int f(asm_env* env)

#define setBit(word, pos) (word | (1 << (pos)))

typedef struct {
	unsigned long int regs[8];
	unsigned long int eflags;
			
	void**	stack;
	int		stack_length;

	unsigned int eip;
	unsigned int current_op;
	unsigned int current_args;

	unsigned char* mem_base;
	unsigned int   mem_size;
} asm_env;

enum {
	R_EAX = 0,
	R_ECX,
	R_EDX,
	R_EBX,
	R_ESP,
	R_EBP,
	R_ESI,
	R_EDI,
	R_REGS
};

enum {
	F_CARRY = 0,
	F_R1,
	F_PARITY,
	F_R2,
	F_ADJUST,
	F_R3,
	F_ZERO,
	F_SIGN,
	F_TRAP,
	F_INTERRUPT,
	F_DIRECTION,
	F_OVERFLOW,
	F_IOPL1,
	F_IOPL2,
	F_NT,
	F_R4,
	F_RESUME,
	F_VM,
	F_ALIGNMENT,
	F_VIF,
	F_VIP,
	F_CPUID,
	F_EFLAGS
};


void die( asm_env*, char* );
void dumpAll( asm_env* );
