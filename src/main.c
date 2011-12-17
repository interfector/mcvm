#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <mcvm.h>

unsigned char assembly_code[] = {
/*	0xb3, 0x02,
	0xb2, 0x03,
	0x90,
	0x90,
	0x90,
	0xeb, 0x0d,
	0xb8, 0x01, 0x02, 0x03, 0x04,
	0x66, 0xb9, 0x01, 0x2,
	0x89, 0xc3,
	0x89, 0xca,
	0xb8, 0x01, 0x00, 0x00, 0x00,
	0x89, 0x08 */
/*	0xb8, 0x01, 0x02, 0x03, 0x04,
	0xb1, 0x05,
	0xb8, 0x01, 0x00, 0x00, 0x00,
	0x89, 0x08,
	0x40, 0x41,*/
	0xb8, 0x00, 0x00, 0x00, 0x00,
	0xb9, 0x01, 0x00, 0x00, 0x00,
	/*0x8b, 0x08,*/
	0x89, 0x08,
};

int
main(int argc, char** argv)
{
	asm_env* env = malloc( sizeof(asm_env) );
	struct assembly opcode;
	unsigned char* args;
	int i;

	for(i = 0;i < R_REGS;i++)
		env->regs[ i ] = 0;

	env->current_op = 
	env->current_args =
	env->stack_length =
	env->mem_size = sizeof( assembly_code );
	env->mem_base = malloc( env->mem_size );

	memset( env->mem_base, 0x00, env->mem_size );
	memcpy( env->mem_base, assembly_code, sizeof(assembly_code) );

	for(env->eip = 0;env->eip < env->mem_size;env->eip++)
	{
		env->current_op = env->mem_base[ env->eip ];
		opcode = getOpcode( env->mem_base[ env->eip ] );

		if( opcode.bytes > 0 )
		{
			env->current_args = 0;
			args = malloc( opcode.bytes );
			memcpy( args, env->mem_base + env->eip + 1, opcode.bytes);

			for(i = 0;i < opcode.bytes;i++)
				env->current_args |= (args[i] << (i * 8));
		}

		opcode.function( env );

		env->eip += opcode.bytes;
	}

	env->eip -= (opcode.bytes + 1);

	dumpAll( env );

	free( env );

	return 0;
}
