#include "syscall.h"

int 
getBit(unsigned int word, int pos)
{
	unsigned int nword = setBit(word, pos);
	nword ^= word;
	return (nword == 0 ? 1 : 0);
}

unsigned char*
_getAssembly( asm_env* env, int index, int size )
{
	int i;
	unsigned char* code = malloc( size );

	for(i = 0;i < size;i++)
		code[ i ] = env->mem_base[ index + i ];

	return code;
}

unsigned int
_getAddr( asm_env* env, int index, int size )
{
	unsigned int addr = 0;
	unsigned char* code;

	if( !size || size > 4 )
		_die( env, "SIGILL" );

	code = _getAssembly( env, index, size );

	addr = (code[3] << 24) + (code[2] << 16) + (code[1] << 8) + code[0];

	free( code );
/* 
 * for(i = 0;i < size;i++)
 * 	addr = (addr << 8) | env->mem_base[ index + i ];
 *
 * endian_swap( &addr );
*/
	return addr;
}

void
_setAddr( asm_env* env, int index, int size, unsigned int val )
{
	int i;

	if( !size || size > 4 )
		_die( env, "SIGILL" );

	for(i = 0;i < size;i++, val>>=8)
		env->mem_base[ index + i ] = val & 0x000000FF;
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
_dumpStack( asm_env* env )
{
	int i;

	if( env->regs[ R_ESP ] < env->regs[ R_EBP ] )
		puts("Empty!");
	else
	{

		printf("   ");

		for (i = 0; i < 6; i++)
			printf("      \e[1;30m%02x\e[0m ", i );

		for(i = 0;i < ((env->regs[ R_ESP ] - env->regs[ R_EBP ]) / 4) + 1;i++)
		{
			if (i % 6 == 0) 
				printf("\n\e[1;30m%02x\e[0m", i );

			printf(" %08x", (unsigned int)_getAddr( env, env->regs[ R_EBP ] + (i * 4), sizeof(int)) );
		}
	}
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

	_dumpStack( env );

	printf("\n\e[1;31mMem base:\e[0m\n  ");

	for (i = 0; i < 16; i++)
		printf(" \e[1;30m%02x\e[0m", i);

	for(i = 0;i < (env->mem_size - STACKSTART - STACKSIZE);i++)
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
