#include <stdio.h>
#include <stdlib.h>

void init_stack() __attribute__ ((constructor));

void** stack;
int    stack_len;

void* sp;
void* bp;

void /* initialize the stack */
init_stack()
{
	stack = malloc(sizeof(void*));

	stack_len = 0;

	bp = &stack[ 0 ];

	sp = &stack[ 0 ];
}

void /* push an element on the top of the stack */
push(unsigned int addr)
{
	stack = realloc(stack, sizeof(void*) * ++stack_len);

	stack[ stack_len - 1 ] = (void*)addr;

	sp = &stack[ stack_len - 1 ];
}

unsigned int
pop() /* pop an element from the top of the stack */
{
	void* ptr;

	ptr = stack[ stack_len - 1 ];

	stack = realloc(stack, sizeof(void*) * --stack_len);

	sp = &stack[ stack_len - 1 ]; 

	return (unsigned int)ptr;
}

void /* assembly prologue */
enter() 
{
	/*   How starts a call in a disassembled executable */
	push( (unsigned int)bp );
	bp = sp;
	sp -= 4;
}

void
ret()
{
	sp = bp;
	bp = (void*)pop();
	
	asm(
	    "jmp *%0"
	    :
	    :"r"( *(unsigned int*)(sp) )
	   );	
}

int
dummy(void)
{
	int i;

	enter();

	printf("Dummy function!!\n");

	printf("(%%ebp) = 0x%x\t\t( old ebp )\n", *(unsigned int*)bp);
	printf("-0x4(%%ebp) = 0x%x\t\t( return address )\n", *(unsigned int*)(bp - 4));
	printf("-0x8(%%ebp) = 0x%x\t\t( first argument )\n", *(unsigned int*)(bp - 8));
	printf("-0x12(%%ebp) = 0x%x\t( second argument )\n", *(unsigned int*)(bp - 12));
	printf("-0x16(%%ebp) = 0x%x \t( third argument )\n", *(unsigned int*)(bp - 16));
	printf("-0x20(%%ebp) = 0x%x \t( fourth argument )\n", *(unsigned int*)(bp - 20));

/*	write(1, (unsigned int*)(bp - 12), 8); */
	for(i = 0;i < 8;i++)
		putchar(*(unsigned int*)((bp - 12) + i));

	int* c = (int*)(* (unsigned int*)(bp - 16));

	*c = 42;

	printf("String passed: \"%s\"\n", (char*)*(unsigned int*)(bp - 20) );

	ret();

	return 0;
}

void
call(unsigned int called)
{
	unsigned int ebp;

	asm("movl 4(%%ebp),%0"
		:"=r"(ebp)
	   );

	push( ebp );

	((void (*)()) called)();
}

int i = 0;

int
main()
{
/*	int i = 0;  */
	char buf[] = "Hello World!";

	push( &buf );
	push( &i );
	push( 0x206c6f6c );
	push( 0x0a6c6f6c );

	call( &dummy );

	printf("i = %d\n", i);

	exit(0);

	return 0;
}
