#ifndef _ASM_SIM_
#define _ASM_SIM_

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#define AFUNC(name) void name( asm_env* env )

typedef struct {
	unsigned long int regs[8];

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

struct assembly {
	unsigned char opcode;
	unsigned char lopcode;
	int bytes;

	void (*function)(asm_env*);
};

AFUNC(movb);
AFUNC(movl);
AFUNC(movw);
AFUNC(movr);
AFUNC(memeax);
AFUNC(nop);
AFUNC(sjmp);
AFUNC(ljmp);
AFUNC(inc);
AFUNC(dec);
AFUNC(idb);
AFUNC(rref);
AFUNC(arithl);
AFUNC(mull);
AFUNC(rmadd);
AFUNC(mradd);
AFUNC(rmsub);
AFUNC(mrsub);

static 
struct assembly InstructionSet[] = {
	{ '\xb0', '\xb3', 1, movb },
	{ '\xb8', '\xbf', 4, movl },
	{ '\x66', 0, 3, movw },
	{ '\x89', 0, 2, movr },
	{ '\x90', 0, 0, nop },
	{ '\xeb', 0, 1, sjmp },
	{ '\xe9', 0, 4, ljmp },
	{ '\xa2', '\xa3', 4, memeax },
	{ '\x40', '\x47', 0, inc },
	{ '\x48', '\x4f', 0, dec },
	{ '\xfe', 0, 1, idb },
	{ '\x8b', 0, 2, rref },
	{ '\x81', 0, 5, arithl },
	{ '\xf7', 0, 2, mull },
	{ '\x03', 0, 2, rmadd },
	{ '\x01', 0, 2, mradd },
	{ '\x2b', 0, 2, rmsub },
	{ '\x29', 0, 2, mrsub },
};

struct assembly getOpcode( unsigned char);
void endian_swap( unsigned int* );
void short_endian_swap( unsigned short int* );
void dumpRegs( asm_env* );
void dumpAll( asm_env* );
void die( asm_env*, char* );

#endif
