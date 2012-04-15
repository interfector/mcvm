#include <syscall.h>

SYSCALL(iexit);
SYSCALL(isys);

SYSHANDLER()
{
	int (*ifunc[])(asm_env*) = {
		NULL,
		iexit,
		isys,
	};
	int len = sizeof(ifunc)/sizeof(void*);

	if( env->regs[ R_EAX ] < 0 || env->regs[ R_EAX ] >= len || ifunc[ env->regs[ R_EAX ] ] == NULL )
		_die( env, "SIGSEGV" );
	else
		env->regs[ R_EAX ] = ifunc[ env->regs[ R_EAX ] ]( env );
}

SYSCALL(iexit)
{
	printf("\e[1;31mExit code:\e[0m\t0x%08x\n", (unsigned int)env->regs[ R_EBX ] );

	_dumpAll( env );

	return 0;
}

SYSCALL(isys)
{
	printf("LOL\n");

	return 0;
}
