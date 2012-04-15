#include "syscall.h"

int 
getBit(unsigned int word, int pos)
{
	unsigned int nword = setBit(word, pos);
	nword ^= word;
	return (nword == 0 ? 1 : 0);
}

void
_dumpRegs( asm_env* env )
{
	char* string_tab[] = {
		"%eax",
		"%ecx",
		"%edx",
		"%ebx",
		"%esp",
		"%edp",
		"%esi",
		"%edi",
	};
	char* eflags_tab[] = {
		"CF",
		"RS",
		"PF",
		"RS",
		"AF",
		"RS",
		"ZF",
		"SF",
		"TF",
		"IF",
		"DF",
		"OF",
		"Iopl",
		"Iopl",
		"NTF",
		"RS",
		"Resume",
		"Vm",
		"Align",
		"Vif",
		"Vip",
		"Cpuid",
	};
	int i;

	for(i = 0;i < R_REGS;i++)
		printf("%s\t0x%08lx\n", string_tab[ i ], env->regs[ i ] );

	printf("\n\e[1;31mEFlags:\e[0m\n");

	for(i = 0;i < /*F_EFLAGS*/F_IOPL1;i++)
		printf("%s\t%01x\n", eflags_tab[ i ], getBit(env->eflags, i) );
}

void
_dumpAll( asm_env* env )
{
	int i;

	printf("\e[1;32m@\e[0m EIP:    \t0x%08x\n"
		  "\e[1;32m@\e[0m OpCode: \t0x%08x\n"
		  "\e[1;32m@\e[0m CodeArg:\t0x%08x\n\n",
		  env->eip,
		  env->current_op,
		  env->current_args );

	printf("\e[1;31mRegs:\e[0m\n");

	_dumpRegs( env );

	printf("\n\e[1;31mStack:\e[0m\n");

	//dumpStack( env );

	printf("\n\e[1;31mMem base:\e[0m\n  ");

	for (i = 0; i < 16; i++)
		printf(" \e[1;30m%02x\e[0m", i);

	for(i = 0;i < env->mem_size;i++)
	{
		if (i % 16 == 0) 
			printf("\n\e[1;30m%02x\e[0m", i );

		printf(" %02x", env->mem_base[ i ]);
	}

	putchar('\n');

	exit( 0 );
}

void
_die( asm_env* env, char* string )
{
	printf("[%s]\n\n", string);

	_dumpAll( env );

	exit( 1 );
}
