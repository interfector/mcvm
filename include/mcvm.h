#ifndef _ASM_SIM_
#define _ASM_SIM_

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#define indexof( addr, i ) (( addr & (0xFF << (i * 8)) ) >> (i * 8))
#define AFUNC(name) void name( asm_env* env )
#define IFUNC(name) int name( asm_env* env )
#define setBit(word, pos) (word | (1 << (pos)))
#define dgetBit(word,pos) ((word & (1 << (pos))) >> (pos))
#define inRange(num, num1, num2) ((num >= num1 && num <= num2) ? 1 : 0)
#define pdie(s,ec) do{ perror(s); exit(ec); }while(0)

#define STACKSIZE	1024
#define STACKSTART	(2 * STACKSIZE)

typedef struct {
	unsigned long int regs[8];
	unsigned long int eflags;
	
	unsigned int eip;
	unsigned int current_op;
	unsigned int current_args;

	unsigned char* mem_base;
	unsigned int   mem_size;
} asm_env;

typedef unsigned char uchar;

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
AFUNC(movbmr);
AFUNC(movbrm);
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
AFUNC(int3);
AFUNC(cmpb);
AFUNC(interrupt);
AFUNC(xchg_eax);
AFUNC(push);
AFUNC(pushb);
AFUNC(pushr);
AFUNC(pushm);
AFUNC(popr);
AFUNC(popm);
AFUNC(call);
AFUNC(ret);

/* {'start_range', 'end_range', 'arg_len', 'function_pointer'} */
static 
struct assembly InstructionSet[] = {
	{ '\xb0', '\xb3', 1, movb },
	{ '\xc6', 0, 2, movbmr },
	{ '\x8a', 0, 2, movbrm },
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
	{ '\xcc', 0, 0, int3 },
	{ '\x80', 0, 2, cmpb },
	{ '\x91', '\x97', 0, xchg_eax },
	{ '\x68', 0, 4, push },
	{ '\x6a', 0, 1, push },
	{ '\x50', '\x57', 0, pushr },
	{ '\xff', 0, 1, pushm },
	{ '\x58', '\x5f', 0, popr },
	{ '\x8f', 0, 1, popm },
	{ '\xe8', 0, 4, call },
	{ '\xc3', 0, 0, ret },
	{ '\xcd', 0, 1, interrupt },
};

struct interrupth {
	unsigned char code;
	void (*function)(asm_env*);
};

AFUNC(int80);

static
struct interrupth InterruptHandler[] = {
	{ 0x80, int80 },
};

struct assembly getOpcode( unsigned char);
void endian_swap( unsigned int* );
void short_endian_swap( unsigned short int* );
void dumpRegs( asm_env* );
void dumpAll( asm_env* );
void die( asm_env*, char* );
int getBit(unsigned int, int);
unsigned int zeroBit(unsigned int, int);

void loadHandler(void);
#endif
