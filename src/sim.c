#include <mcvm.h>
#include <unistd.h>
#include <fcntl.h>
#include <dlfcn.h>

int getOffset( unsigned char, unsigned char*, int );
unsigned char* getAssembly( asm_env*, int, int );
unsigned int getAddr( asm_env*, int, int );
void setAddr( asm_env*, int, int, unsigned int );

AFUNC(movb)
{
	uchar *creg = (uchar *) &(env->regs[env->current_op - 0xb0]);
	creg[0] = (uchar)env->current_args;
}

AFUNC(movbmr)
{
	uchar* cargs = (uchar*) &(env->current_args);

	if( env->regs[ cargs[0] ] < 0 || env->regs[ cargs[0] ] > env->mem_size )
		die( env, "SIGSEGV" );
	else
		env->mem_base[ env->regs[ cargs[0] ] ] = (uchar)cargs[1];
}

AFUNC(movbrm)
{
	unsigned char ooffset[] = {
		0x40,
		0x48,
		0x50,
		0x58
	};
	unsigned char ioffset[] = {
		0x00,
		0x08,
		0x10,
		0x18
	};
	int off, offset = 0;

	off = getOffset( env->current_args, ooffset, sizeof(ooffset) );

	if( off == -1 )
	{
		env->current_args = ((uchar*)&env->current_args)[0];
		env->eip--;
	} else {
		offset = ((uchar*)&env->current_args)[1];
		env->current_args = ((uchar*)&env->current_args)[0];
	}

	if( 0x00 <= env->current_args && env->current_args <= 0x1f )
	{
		off = getOffset( env->current_args, ioffset, sizeof(ioffset) );

		if( env->regs[ env->current_args - ioffset[off] ] < 0 || env->regs[ env->current_args - ioffset[off] ] > env->mem_size )
			die( env, "SIGSEGV" );
		else
			((uchar*)&env->regs[ off ])[0] = (uchar)env->mem_base[ env->regs[ env->current_args - ioffset[ off ] ] ];
	} else if( 0x40 <= env->current_args && env->current_args <= 0x5f ) {
		off = getOffset( env->current_args, ooffset, sizeof(ooffset) );

		if( env->regs[ env->current_args - ooffset[off] ] + offset < 0 || env->regs[ env->current_args - ooffset[ off ] ] + offset > env->mem_size )
			die( env, "SIGSEGV" );
		else
			((uchar*)&env->regs[ off ])[0] = (uchar)env->mem_base[ env->regs[ env->current_args - ooffset[ off ] ] + offset ];
	}
}

AFUNC(xchg_eax)
{
	unsigned int creg = env->current_op - 0x90;
	env->regs[0] ^= env->regs[creg];
	env->regs[creg] ^= env->regs[0];
	env->regs[0] ^= env->regs[creg];
}

AFUNC(movl)
{
	env->regs[ env->current_op - 0xb8 ] = env->current_args;
}

AFUNC(winstr)
{
	int off = (env->current_args & 0x000000FF) - 0xb8;

	env->current_args = ((env->current_args & 0x00FFFF00) >> 8);
	((uchar*)&env->regs[ off ])[0] = ((uchar*)&env->current_args)[0];
	((uchar*)&env->regs[ off ])[1] = ((uchar*)&env->current_args)[1];
}

AFUNC(int3)
{
	die( env, "SIGTRAP" );
}

AFUNC(cmpb)
{
	uchar* cargs = (uchar *) &(env->current_args), *creg;

	creg = (uchar *) &(env->regs[cargs[0] - 0xf8]);

	if (creg[0] == cargs[1]) 
		env->eflags = zeroBit(env->eflags, F_ZERO);
	else
		env->eflags = setBit(env->eflags, F_ZERO);
}

AFUNC(movr)
{
	
	unsigned char roffset[] = {
		0xc0,
		0xc8,
		0xd0,
		0xd8,
		0xe0,
		0xe8,
		0xf0,
		0xf8
	};
	unsigned char moffset[] = {
		0x00,
		0x08,
		0x10,
		0x18,
		0x20,
		0x28,
		0x30,
		0x38
	};
	unsigned char ooffset[] = {
		0x40,
		0x48,
		0x50,
		0x58,
		0x60,
		0x68,
		0x70,
		0x78,
		0x7f
	};
	int off, offset = 0;

	off = getOffset( env->current_args, ooffset, sizeof(ooffset) );

	if( off == -1 )
	{
		/*env->current_args = env->current_args & 0x000000FF;*/
		env->current_args = ((uchar*)&env->current_args)[0];
		env->eip--;
	} else {
		/*offset = (env->current_args & 0x0000FF00) >> 8;*/
		offset = ((uchar*)&env->current_args)[1];
		env->current_args = ((uchar*)&env->current_args)[0];
		/*env->current_args = env->current_args & 0x000000FF;*/
	}

	if( 0xc0 <= env->current_args && env->current_args <= 0xff )
	{
		off = getOffset( env->current_args, roffset, sizeof(roffset) );
		/* setAddr( env, env->regs[ env->current_args - roffset[ off ] ], sizeof(int), env->regs[ off ] );*/
		env->regs[ env->current_args - roffset[off] ] = env->regs[ off ];
	} else //	if( 0x00 <= env->current_args && env->current_args <= 0x3f )
	{
		if( 0x00 <= env->current_args && env->current_args <= 0x3f )
		{
			off = getOffset( env->current_args, moffset, sizeof(moffset) );

			if( env->regs[ env->current_args - moffset[off] ] + offset < 0 || env->regs[ env->current_args - moffset[off] ] + offset > env->mem_size )
				die( env, "SIGSEGV" );
			else {
				setAddr( env, env->regs[ env->current_args - moffset[ off ] ] + offset, sizeof(int), env->regs[ off ] );
				/*env->mem_base[ env->regs[ env->current_args - moffset[off] ] + offset ] = env->regs[ off ];*/
			}
		}
		else if( 0x40 <= env->current_args && env->current_args <= 0x7f ) {
			off = getOffset( env->current_args, ooffset, sizeof(ooffset) );

			if( env->regs[ env->current_args - ooffset[off] ] + offset < 0 || env->regs[ env->current_args - ooffset[off] ] + offset > env->mem_size )
				die( env, "SIGSEGV" );
			else {
				setAddr( env, env->regs[ env->current_args - ooffset[ off ] ] + offset, sizeof(int), env->regs[ off ] );
				/*env->mem_base[ env->regs[ env->current_args - ooffset[off] ] + offset ] = env->regs[ off ];*/
			}
		}

		#ifdef _DEBUG
		if( 0x00 <= env->current_args && env->current_args <= 0x3f )
			printf("MEM[ %d ] = %x\n", env->regs[ env->current_args - moffset[off] ] + offset, env->regs[ off ] );
		else
			printf("MEM[ %d ] = %x\n", env->regs[ env->current_args - ooffset[off] ] + offset, env->regs[ off ] );
		#endif
	}
}

AFUNC(memeax)
{
	if( env->current_op == 0xa2 )
		env->mem_base[ env->current_args ] = (uchar)env->regs[ R_EAX ];
	else if( env->current_op == 0xa3 )
		env->mem_base[ env->current_args ] = (unsigned int)env->regs[ R_EAX ];
}

AFUNC(sjmp)
{
	int off = 0;
/*
	if( 0x80 <= env->current_args && env->current_args <= 0xfd )
		off = 0xff - env->current_args;
	else if( 0x00 <= env->current_args && env->current_args <= 0x7f )
		off = env->current_args;
*/
	off = (signed char)env->current_args;
	env->eip += off;
}

AFUNC(ljmp)
{ /* To FIX */
	env->eip += env->current_args;
}

AFUNC(inc)
{
	env->regs[ env->current_op - 0x40 ]++;
}

AFUNC(dec)
{
	env->regs[ env->current_op - 0x48 ]--;
}

AFUNC(idb)
{
	unsigned char lreg;

	if( 0x40 <= env->current_args && env->current_args <= 0x47 )
	{
		lreg = (uchar) (env->regs[ env->current_args - 0x40 ] & 0x000000FF) + 1;

		env->regs[ env->current_args - 0x40 ] &= 0xFFFFFF00;
		env->regs[ env->current_args - 0x40 ] |= lreg;
	} else if( 0x48 <= env->current_args && env->current_args <= 0x4f ) {
		lreg = (uchar) (env->regs[ env->current_args - 0x48 ] & 0x000000FF) - 1;

		env->regs[ env->current_args - 0x48 ] &= 0xFFFFFF00;
		env->regs[ env->current_args - 0x48 ] |= lreg;
	}
}

AFUNC(rref)
{
	unsigned char rfoffset[] = {
		0x00,
		0x08,
		0x10,
		0x18,
		0x20,
		0x28,
		0x30,
		0x38
	};
	unsigned char rfooffset[] = {
		0x40,
		0x48,
		0x50,
		0x58,
		0x60,
		0x68,
		0x70,
		0x78
	};
	int off, offset = 0;
	unsigned int addr = 0;

	off = getOffset( env->current_args, rfooffset, sizeof(rfooffset) );

	if( off == -1 )
	{
		env->current_args = env->current_args & 0x000000FF;
		env->eip--;
	} else {
		offset = (char)((env->current_args & 0x0000FF00) >> 8);
		env->current_args = env->current_args & 0x000000FF;
	}

	if( 0x00 <= env->current_args && env->current_args <= 0x3f )
	{
		off = getOffset( env->current_args, rfoffset, sizeof(rfoffset) );

		if( env->regs[ env->current_args - rfoffset[off] ] < 0 || env->regs[ env->current_args - rfoffset[off] ] > env->mem_size )
			die( env, "SIGSEGV" );
		else {
			/*env->regs[ off ] = env->mem_base[ env->regs[ env->current_args - rfoffset[ off ] ] ];*/
			addr = getAddr( env, env->regs[ env->current_args - rfoffset[ off ] ], sizeof(int) );
			env->regs[ off ] = addr;

			if( env->mem_base[ env->eip + 3 ] == 0x24 )
				env->eip++;
		}
	} else if( 0x40 <= env->current_args && env->current_args <= 0x7f )
	{
		off = getOffset( env->current_args, rfooffset, sizeof(rfooffset) );

		if( env->regs[ env->current_args - rfooffset[off] ] + offset < 0 || env->regs[ env->current_args - rfooffset[off] ] + offset > env->mem_size )
			die( env, "SIGSEGV" );
		else {
			/*env->regs[ off ] = env->mem_base[ env->regs[ env->current_args - rfooffset[ off ] ] + offset ];*/

			if( env->mem_base[ env->eip + 2 ] == 0x24 )
			{
				offset = (char)env->mem_base[ env->eip + 3 ];
				addr = getAddr( env, (signed)env->regs[ R_ESP ] + (int)offset, sizeof(int) );
				env->regs[ off ] = addr;

				env->eip++;
			} else {
				addr = getAddr( env, (signed)env->regs[ env->current_args - rfooffset[ off ] ] + (int)offset, sizeof(int) );
				env->regs[ off ] = addr;
			}
		}
	}
}

AFUNC(arithl)
{
	unsigned char foffset[] = {
		0xc0,
		0xc8,
		0xe0,
		0xe8,
		0xf0,
		0xf8,
		0xff,
	};
	/*
	void (*arithmetic[])(asm_env*, unsigned char*,int) = {
		addl,
		orl,
		andl,
		subl,
		xorl
	}; */
	int off, index;
	unsigned char* code;
	unsigned int arg = 0;

	code = getAssembly( env, env->eip, 6 );

#ifdef _DEBUG
	int i;

	for(i = 0;i < 6;i++)
		printf("%02x", code[i] );

	putchar('\n');
#endif

	off = getOffset( code[1], foffset, sizeof(foffset) );

	index = code[1] - foffset[ off ];
	memcpy( &arg, code + 2, sizeof(int) );

	switch( off ){
		case 0:
			env->regs[ index ] += arg;
			break;
		case 1:
			env->regs[ index ] |= arg;
			break;
		case 2:
			env->regs[ index ] &= arg;
			break;
		case 3:
			env->regs[ index ] -= arg;
			break;
		case 4:
			env->regs[ index ] ^= arg;
			break;
		case 5:
			if( arg == env->regs[ index ] )
				env->eflags = zeroBit(env->eflags, F_ZERO);
			else
				env->eflags = setBit(env->eflags, F_ZERO);
			break;
		default:
			break;
	}
/*	arithmetic[ off ]( env, code, code[1] - foffset[ off ] ); */
}

AFUNC(arithb)
{
	unsigned char foffset[] = {
		0xc0,
		0xc8,
		0xe0,
		0xe8,
		0xf0,
		0xf8,
		0xff,
	};
	int off, index;
	unsigned char* code;
	unsigned int arg = 0;

	code = (unsigned char*)&env->current_args;

#ifdef _DEBUG
	int i;

	for(i = 0;i < 2;i++)
		printf("%02x", code[i] );

	putchar('\n');
#endif

	off = getOffset( code[0], foffset, sizeof(foffset) );

	index = code[0] - foffset[ off ];
	arg = (unsigned int)code[1];

#ifdef _DEBUG
	printf("regs[ %d ] %c= 0x%x\n", index, ("+|&-^")[off], arg );
#endif

	switch( off ){
		case 0:
			env->regs[ index ] += arg;
			break;
		case 1:
			env->regs[ index ] |= arg;
			break;
		case 2:
			env->regs[ index ] &= arg;
			break;
		case 3:
			env->regs[ index ] -= arg;
			break;
		case 4:
			env->regs[ index ] ^= arg;
			break;
		case 5:
			if( arg == env->regs[ index ] )
				env->eflags = zeroBit(env->eflags, F_ZERO);
			else
				env->eflags = setBit(env->eflags, F_ZERO);
			break;
		default:
			break;
	}
}

AFUNC(mull)
{
	unsigned int addr;
	unsigned int code = env->current_args & 0x000000FF;

	if( 0x60 <= code && code <= 0x68 )
	{
		if( env->regs[ env->current_args - 0x60 ] < 0 || env->regs[ env->current_args - 0x60 ] > env->mem_size )
			die( env, "SIGSEGV" );
		else {
			addr = getAddr( env, env->regs[ code - 0x60 ] + ((env->current_args & 0xFF00) >> 8), sizeof(int) );
			env->regs[ R_EAX ] *= addr;
		}
	} else {
		env->current_args = env->current_args & 0x000000FF;
		if( 0x20 <= env->current_args && env->current_args <= 0x28 )
		{
			if( env->regs[ env->current_args - 0x20 ] < 0 || env->regs[ env->current_args - 0x20 ] > env->mem_size )
				die( env, "SIGSEGV" );
			else {
				addr = getAddr( env, env->regs[ env->current_args - 0x20 ], sizeof(int) );
				env->regs[ R_EAX ] *= addr;
			}
		} else if( 0xe0 <= env->current_args && env->current_args <= 0xe8 ) {
			env->regs[ R_EAX ] *= env->regs[ env->current_args - 0xe0 ];
		} else {
			die( env, "SIGILL" );
		}

		env->eip--;
	}
}

AFUNC(mulb)
{
	if( 0xe0 <= env->current_args && env->current_args <= 0xe8 )
		env->regs[ R_EAX ] *= ((uchar*)&env->regs[ env->current_args - 0xe0 ])[0];
	else
		die( env, "SIGSEGV" );
}

AFUNC(rmadd)
{
	unsigned char roffset[] = {
		0x00,
		0x08,
		0x10,
		0x18,
		0x20,
		0x28,
		0x30,
		0x38
	};
	unsigned char rooffset[] = {
		0x40,
		0x48,
		0x50,
		0x58,
		0x60,
		0x68,
		0x70,
		0x78
	};
	int off, offset = 0;
	unsigned int addr;

	off = getOffset( env->current_args, rooffset, sizeof(rooffset) );

	if( off == -1 )
	{
		env->current_args = env->current_args & 0x00000000FF;
		env->eip--;
	} else {
		offset = (env->current_args & 0x0000FF00) >> 8;
		env->current_args = env->current_args & 0x00000000FF;
	}

	if( 0x00 <= env->current_args && env->current_args <= 0x3f )
	{
		off = getOffset( env->current_args, roffset, sizeof(roffset) );

		if( env->regs[ env->current_args - roffset[off] ] < 0 || env->regs[ env->current_args - roffset[off] ] > env->mem_size )
			die( env, "SIGSEGV" );
		else {
			addr = getAddr( env, env->regs[ env->current_args - roffset[ off ] ], sizeof(int) );
			env->regs[ off ] += addr;

		}
	} else if( 0x40 <= env->current_args && env->current_args <= 0x7f )
	{
		off = getOffset( env->current_args, rooffset, sizeof(rooffset) );

		if( env->regs[ env->current_args - rooffset[off] ] + offset < 0 || env->regs[ env->current_args - rooffset[off] ] + offset > env->mem_size )
			die( env, "SIGSEGV" );
		else {
			addr = getAddr( env, env->regs[ env->current_args - rooffset[ off ] ] + offset, sizeof(int) );
			env->regs[ off ] += addr;
		}
	}
}

AFUNC(rmaddb)
{
	unsigned char moffset[] = {
		0x00,
		0x08,
		0x10,
		0x18,
		0x20,
		0x28,
		0x30,
		0x38
	};
	int off = 0;
	unsigned int addr;
	uchar *creg;

	off = getOffset( env->current_args, moffset, sizeof(moffset) );

	if( env->regs[ env->current_args - moffset[off] ] < 0 || env->regs[ env->current_args - moffset[off] ] > env->mem_size )
		die( env, "SIGSEGV" );
	else {
		creg = (uchar *) &(env->regs[ off ]);
		addr = getAddr( env, env->regs[ env->current_args - moffset[ off ] ], sizeof(int) );
		addr += creg[0];
		setAddr( env, env->regs[ env->current_args - moffset[ off ] ], sizeof(int), addr );
	}
}

AFUNC(mradd)
{
	unsigned char roffset[] = {
		0x00,
		0x08,
		0x10,
		0x18,
		0x20,
		0x28,
		0x30,
		0x38
	};
	unsigned char rooffset[] = {
		0x40,
		0x48,
		0x50,
		0x58,
		0x60,
		0x68,
		0x70,
		0x78,
		0x7f
	};
	unsigned char rfoffset[] = {
		0xc0,
		0xc8,
		0xd0,
		0xd8,
		0xe0,
		0xe8,
		0xf0,
		0xf8
	};
	int off, offset = 0;
	unsigned int addr;

	off = getOffset( env->current_args, rooffset, sizeof(rooffset) );

	if( off == -1 )
	{
		env->current_args = env->current_args & 0x00000000FF;
		env->eip--;
	} else {
		offset = (env->current_args & 0x0000FF00) >> 8;
		env->current_args = env->current_args & 0x00000000FF;
	}

	if( 0x00 <= env->current_args && env->current_args <= 0x3f )
	{
		off = getOffset( env->current_args, roffset, sizeof(roffset) );

		if( env->regs[ env->current_args - roffset[off] ] < 0 || env->regs[ env->current_args - roffset[off] ] > env->mem_size )
			die( env, "SIGSEGV" );
		else {
			addr = getAddr( env, env->regs[ env->current_args - roffset[ off ] ], sizeof(int) );
			addr += env->regs[ off ];
			setAddr( env, env->regs[ env->current_args - roffset[ off ] ], sizeof(int), addr );
		}
	} else if( 0xc0 <= env->current_args && env->current_args <= 0xff ) {
		off = getOffset( env->current_args, rfoffset, sizeof(rfoffset) );

		env->regs[ env->current_args - rfoffset[ off ] ] += env->regs[ off ];
	} else if( 0x40 <= env->current_args && env->current_args <= 0x7f )
	{
		off = getOffset( env->current_args, rooffset, sizeof(rooffset) );

		if( env->regs[ env->current_args - rooffset[off] ] + offset < 0 || env->regs[ env->current_args - rooffset[off] ] + offset > env->mem_size )
			die( env, "SIGSEGV" );
		else {
			addr = getAddr( env, env->regs[ env->current_args - rooffset[ off ] ] + offset, sizeof(int) );
			addr += env->regs[ off ];
			setAddr( env, env->regs[ env->current_args - rooffset[ off ] ] + offset, sizeof(int), addr );
		}
	}
}

AFUNC(rmsub)
{
	unsigned char roffset[] = {
		0x00,
		0x08,
		0x10,
		0x18,
		0x20,
		0x28,
		0x30,
		0x38
	};
	unsigned char rooffset[] = {
		0x40,
		0x48,
		0x50,
		0x58,
		0x60,
		0x68,
		0x70,
		0x78
	};
	int off, offset = 0;
	unsigned int addr;

	off = getOffset( env->current_args, rooffset, sizeof(rooffset) );

	if( off == -1 )
	{
		env->current_args = env->current_args & 0x00000000FF;
		env->eip--;
	} else {
		offset = (env->current_args & 0x0000FF00) >> 8;
		env->current_args = env->current_args & 0x00000000FF;
	}

	if( 0x00 <= env->current_args && env->current_args <= 0x3f )
	{
		off = getOffset( env->current_args, roffset, sizeof(roffset) );

		if( env->regs[ env->current_args - roffset[off] ] < 0 || env->regs[ env->current_args - roffset[off] ] > env->mem_size )
			die( env, "SIGSEGV" );
		else {
			addr = getAddr( env, env->regs[ env->current_args - roffset[ off ] ], sizeof(int) );
			env->regs[ off ] -= addr;

		}
	} else if( 0x40 <= env->current_args && env->current_args <= 0x7f )
	{
		off = getOffset( env->current_args, rooffset, sizeof(rooffset) );

		if( env->regs[ env->current_args - rooffset[off] ] + offset < 0 || env->regs[ env->current_args - rooffset[off] ] + offset > env->mem_size )
			die( env, "SIGSEGV" );
		else {
			addr = getAddr( env, env->regs[ env->current_args - rooffset[ off ] ] + offset, sizeof(int) );
			env->regs[ off ] -= addr;
		}
	}
}

AFUNC(mrsub)
{
	unsigned char roffset[] = {
		0x00,
		0x08,
		0x10,
		0x18,
		0x20,
		0x28,
		0x30,
		0x38
	};
	unsigned char rooffset[] = {
		0x40,
		0x48,
		0x50,
		0x58,
		0x60,
		0x68,
		0x70,
		0x78,
		0x7f
	};
	unsigned char rfoffset[] = {
		0xc0,
		0xc8,
		0xd0,
		0xd8,
		0xe0,
		0xe8,
		0xf0,
		0xf8
	};
	int off, offset = 0;
	unsigned int addr;

	off = getOffset( env->current_args, rooffset, sizeof(rooffset) );

	if( off == -1 )
	{
		env->current_args = env->current_args & 0x00000000FF;
		env->eip--;
	} else {
		offset = (env->current_args & 0x0000FF00) >> 8;
		env->current_args = env->current_args & 0x00000000FF;
	}

	if( 0x00 <= env->current_args && env->current_args <= 0x3f )
	{
		off = getOffset( env->current_args, roffset, sizeof(roffset) );

		if( env->regs[ env->current_args - roffset[off] ] < 0 || env->regs[ env->current_args - roffset[off] ] > env->mem_size )
			die( env, "SIGSEGV" );
		else {
			addr = getAddr( env, env->regs[ env->current_args - roffset[ off ] ], sizeof(int) );
			addr -= env->regs[ off ];
			setAddr( env, env->regs[ env->current_args - roffset[ off ] ], sizeof(int), addr );
		}
	} else if( 0xc0 <= env->current_args && env->current_args <= 0xff ) {
		off = getOffset( env->current_args, rfoffset, sizeof(rfoffset) );

		env->regs[ env->current_args - rfoffset[ off ] ] -= env->regs[ off ];
	} else if( 0x40 <= env->current_args && env->current_args <= 0x7f )
	{
		off = getOffset( env->current_args, rooffset, sizeof(rooffset) );

		if( env->regs[ env->current_args - rooffset[off] ] + offset < 0 || env->regs[ env->current_args - rooffset[off] ] + offset > env->mem_size )
			die( env, "SIGSEGV" );
		else {
			addr = getAddr( env, env->regs[ env->current_args - rooffset[ off ] ] + offset, sizeof(int) );
			addr -= env->regs[ off ];
			setAddr( env, env->regs[ env->current_args - rooffset[ off ] ] + offset, sizeof(int), addr );

		}
	}
}

AFUNC(push)
{
	env->regs[ R_ESP ] -= 4; /* + */

	setAddr( env, env->regs[ R_ESP ], sizeof(int), (unsigned int)env->current_args );
}

AFUNC(pushr)
{
	env->regs[ R_ESP ] -= 4; /* + */

	setAddr( env, env->regs[ R_ESP ], sizeof(int), env->regs[ env->current_op - 0x50 ] );
}

AFUNC(pushm)
{
	if( env->current_args >= 0x20 && env->current_args <= 0x27 )
	{
		env->eip = getAddr( env, env->regs[ env->current_args - 0x20 ], sizeof(int) ) - 2;
	} else if( env->current_args >= 0xe0 && env->current_args <= 0xe7 ) {
		env->eip = env->regs[ env->current_args - 0xe0 ] - 2;
	} else if( env->current_args >= 0xd0 && env->current_args <= 0xd7 ) {
	} else if( env->current_args >= 0x10 && env->current_args <= 0x17 ) {
	} else {
		env->regs[ R_ESP ] -= 4; /* + */

		if( env->current_args >= 0x30 && env->current_args <= 0x37 )
			setAddr( env, env->regs[ R_ESP ], sizeof(int), getAddr( env, env->regs[ env->current_args - 0x30 ], sizeof(int) ) );
		else {
			unsigned char* code = getAssembly( env, env->eip, 3 );

			if( code[1] >= 0x70 && code[1] <= 0x77 )
				setAddr( env, env->regs[ R_ESP ], sizeof(int), getAddr( env, env->regs[ code[1] - 0x70 ] + code[2], sizeof(int) ) );

			env->eip++;
		}
	}
}

AFUNC(popr)
{
	void* ptr;

	if( env->regs[ R_ESP ] > env->regs[ R_EBP ] ) /* < STK-4 */
		die( env, "SIGSEGV" );

	ptr = (void*)getAddr( env, env->regs[ R_ESP ], sizeof(int) );

	env->regs[ R_ESP ] += 4; /* - */

	env->regs[ env->current_op - 0x58 ] = (unsigned long int) ptr;
}

AFUNC(popm)
{
	void* ptr;

	if( env->regs[ R_ESP ] > env->regs[ R_EBP ] )
		die( env, "SIGSEGV" );

	ptr = (void*)getAddr( env, env->regs[ R_ESP ], sizeof(int) );

	env->regs[ R_ESP ] += 4; /* - */

	if( env->current_args <= 0x7 )
		setAddr( env, env->regs[ env->current_args ], sizeof(int), (unsigned int)ptr );
	else {
		unsigned char* code = getAssembly( env, env->eip, 3 );

		if( code[1] >= 0x40 && code[1] <= 0x47 )
			setAddr( env, env->regs[ code[1] - 0x40 ] + code[2], sizeof(int), (unsigned int)ptr );

		env->eip++;
	}
}

AFUNC(call)
{
	env->regs[ R_ESP ] -= 4; /* + */

	setAddr( env, env->regs[ R_ESP ], sizeof(int), (unsigned int)env->eip + 5 );

	env->eip += env->current_args;
}

AFUNC(ret)
{
	void* ptr;

	if( env->regs[ R_ESP ] > env->regs[ R_EBP ] )
		die( env, "SIGSEGV" );

	ptr = (void*)getAddr( env, env->regs[ R_ESP ], sizeof(int) );

	env->regs[ R_ESP ] += 4; /* - */

	env->eip = (unsigned int)ptr - 1;
}

AFUNC(nop)
{
}

IFUNC(iexit)
{
	printf("\e[1;31mExit code:\e[0m\t0x%08x\n", (unsigned int)env->regs[ R_EBX ] );

	dumpAll( env );

	return 0;
}

IFUNC(iread)
{
	return read( env->regs[ R_EBX ], env->mem_base + env->regs[ R_ECX ], env->regs[ R_EDX ] );
}

IFUNC(iwrite)
{
	return write( env->regs[ R_EBX ], env->mem_base + env->regs[ R_ECX ], env->regs[ R_EDX ] );
}

IFUNC(iopen)
{
	return open( (char *)( env->mem_base + env->regs[ R_EBX ]), env->regs[ R_ECX ], env->regs[ R_EDX ] );
}

IFUNC(iclose)
{
	return close( env->regs[ R_EBX ] );
}

AFUNC(int80)
{
	int (*ifunc[])(asm_env*) = {
		NULL,
		iexit,
		NULL,
		iread,
		iwrite,
		iopen,
		iclose,
	};
	int len = sizeof(ifunc)/sizeof(void*);

	if( env->regs[ R_EAX ] < 0 || env->regs[ R_EAX ] >= len || ifunc[ env->regs[ R_EAX ] ] == NULL )
		die( env, "SIGSEGV" );
	else
		env->regs[ R_EAX ] = ifunc[ env->regs[ R_EAX ] ]( env );
}

AFUNC(interrupt)
{
	int i;
	int len = sizeof(InterruptHandler)/sizeof(struct interrupth);
	uchar carg = (uchar)env->current_args;


	for(i = 0;i < len;i++)
	{
		if( InterruptHandler[ i ].code == carg )
		{
			InterruptHandler[ i ].function( env );
			break;
		}
	}
}

int 
getBit(unsigned int word, int pos)
{
	unsigned int nword = setBit(word, pos);
	nword ^= word;
	return (nword == 0 ? 1 : 0);
}

unsigned int
zeroBit(unsigned int word, int pos)
{
	unsigned int base = 0;
	if (getBit(word, pos))
	{
		base = setBit(base, pos);
		word ^= base;
	}
	
	return word;
}

struct assembly
getOpcode( unsigned char code )
{
	int i;
	int size = sizeof(InstructionSet) / sizeof(struct assembly);

	for(i = 0;i < size;i++)
	{
		if( InstructionSet[ i ].lopcode != 0 )
		{
			if( InstructionSet[ i ].opcode <= code && code <= InstructionSet[ i ].lopcode )
				return InstructionSet[ i ];
		} else {
			if( InstructionSet[ i ].opcode == code )
				return InstructionSet[ i ];
		}
	}

	return (struct assembly){ 0, 0, 0, 0 };
}

void
dumpRegs( asm_env* env )
{
	char* string_tab[] = {
		"%eax",
		"%ecx",
		"%edx",
		"%ebx",
		"%esp",
		"%ebp",
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
dumpStack( asm_env* env )
{
	int i;

	if( env->regs[ R_ESP ] > env->regs[ R_EBP ] ) /* < */
		puts("Empty!");
	else
	{

		printf("   ");

		for (i = 0; i < 6; i++)
			printf("      \e[1;30m%02x\e[0m ", i );

		for(i = 0;i < ((env->regs[ R_EBP /* ESP */ ] - env->regs[ R_ESP /* EBP */ ]) / 4) + 1;i++)
		{
			if (i % 6 == 0) 
				printf("\n\e[1;30m%02x\e[0m", i );

			printf(" %08x", (unsigned int)getAddr( env, env->regs[ R_EBP ] - /* + */ (i * 4), sizeof(int)) );
		}
	}
}

void
dumpAll( asm_env* env )
{
	int i;

	printf("\e[1;32m@\e[0m EIP:    \t0x%08x\n"
		  "\e[1;32m@\e[0m OpCode: \t0x%08x\n"
		  "\e[1;32m@\e[0m CodeArg:\t0x%08x\n\n",
		  env->eip,
		  env->current_op,
		  env->current_args );

	printf("\e[1;31mRegs:\e[0m\n");

	dumpRegs( env );

	printf("\n\e[1;31mStack:\e[0m\n");

	dumpStack( env );

	printf("\n\e[1;31mMem base:\e[0m\n  ");

	for (i = 0; i < 16; i++)
		printf(" \e[1;30m%02x\e[0m", i);

	for(i = 0;i < (env->mem_size - STACKSIZE - STACKSTART);i++)
	{
		if (i % 16 == 0) 
			printf("\n\e[1;30m%02x\e[0m", i );

		printf(" %02x", env->mem_base[ i ]);
	}

	putchar('\n');

	exit( 0 );
}

void
die( asm_env* env, char* string )
{
	printf("[%s]\n\n", string);

	dumpAll( env );

	exit( 1 );
}

unsigned char*
getAssembly( asm_env* env, int index, int size )
{
	int i;
	unsigned char* code = malloc( size );

	for(i = 0;i < size;i++)
		code[ i ] = env->mem_base[ index + i ];

	return code;
}

unsigned int
getAddr( asm_env* env, int index, int size )
{
	unsigned int addr = 0;
	unsigned char* code;

	if( !size || size > 4 )
		die( env, "SIGILL" );

	code = getAssembly( env, index, size );

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
setAddr( asm_env* env, int index, int size, unsigned int val )
{
	int i;

	if( !size || size > 4 )
		die( env, "SIGILL" );

	for(i = 0;i < size;i++, val>>=8)
		env->mem_base[ index + i ] = val & 0x000000FF;
}

int
getOffset( unsigned char args, unsigned char* array, int size )
{
	int i, off = -1;

	if( args > array[ size - 1 ] )
	{
		if( size == 9 )
		{
			if( args < array[ size - 1 ] )
				off = size - 1;
		} else 
			off = size - 1;
	}
	else {
		for(i = 0;i < size - 1;i++)
		{
			if( array[i] <= args && args < array[i + 1] )
			{
				off = i;
				break;
			}
		}
	}

	return off;
}

void
loadHandler(void)
{
	FILE* dl;

	if( !(dl = dlopen( getenv("LIBSYSCALL"), RTLD_GLOBAL | RTLD_NOW )) )
		pdie("dlopen", 5);

	InterruptHandler[ 0 ].function = dlsym( dl, "syscall_handler" );
}

void
endian_swap(unsigned int* x)
{
	*x = (*x >> 24) | ((*x << 8) & 0x00FF0000) | ((*x >> 8) & 0x0000FF00) | (*x << 24);
}

void
short_endian_swap( unsigned short int* x)
{
	*x = ((*x >> 8) & 0x00FF) | ((*x << 8) & 0xFF00);
}
