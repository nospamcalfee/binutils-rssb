/* Written and released into the public domain by David Tanguay, 1986 */

/*
 * urisc - emulator for the ultimate reduced instruction set computer
 *
 * The urisc has one accumulator and one instruction: reverse subtract
 * and skip if borrow. Since there is only one instruction, it does not
 * need to be encoded in the program, therefore a program is just a
 * series of operands for the instruction. The next operand is fetched
 * from the location specified in location zero; i.e., memory location
 * zero is the program counter.
 *
 * The great instruction is: subtract the value in the accumulator from the
 * value in the address specified by the operand, and skip the next
 * instruction (more precisely, operand) if there was a borrow.
 *
 * The accumulator is mapped to memory location 1. Using this location as an
 * operand always clears the accumulator and there is never a skip.
 *
 * Another added extra is a zero register, mapped to location 2. This register
 * is always zero, although the accumulator will be set properly after
 * accessing it, and there will be a skip as appropriate.
 *
 * I/O is done with memory mapping. Memory location 3 always contains
 * the value of the next character from input (or -1 at the end of input).
 * Memory location 4 is used to output characters. At the start of an
 * access of location 4, it always has a value of double the current
 * value of the accumulator. The resulting value at the end of the access
 * is sent to the output stream. I.e., an access of memory location 3
 * causes the value in the accumulator to be sent to output, and the
 * accumulator is left unchanged, and there is never a skip (well,
 * not really, but it is true for "proper" character values).
 *
 * The process halts normally when the program counter executes at
 * location 0. Executing other registers will result in strange behaviour.
 */

/*
 * Program format:
 * The input urisc program is a series of integers, one per line, at the
 * beginning of the line. Line 1 inits memory location 0 (the ip),line 2
 * inits loc 1, etc. Comments can appear after the number. A line that
 * does not start with a digit or a '-' is a comment. There's not much in
 * the way or error checking or diagnostics. Good luck.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

/*
 * Memory mapped registers.
 */
enum {
    MEM_PC,     /* program counter  */
    MEM_ACC,    /* accumulator      */
    MEM_ZERO,   /* zero register    */
    MEM_IN,     /* input port       */
    MEM_OUT,    /* output port      */
    MEM_START   /* start of memory  */
};

#define MEM_SIZE    (10*1024)
#define LINE_SIZE   80

int memory[MEM_SIZE];   /* the memory space of the machine  */
int tracing = 0;        /* trace machine execution?         */

    void
trace( char *str, ... )
{
    va_list args;

    if( tracing ) {
        va_start( args, str );
        vfprintf( stderr, str, args );
        va_end( args );
    }
}

    void
err_if( int cond, char *str, ... )
{
    if( cond ) {
        va_list args;
        va_start( args, str );
        fprintf( stderr, "urisc: " );
        vfprintf( stderr, str, args );
        fprintf( stderr, "\n" );
        va_end( args );
        exit( EXIT_FAILURE );
    }
}

    void
load_program( char *fname )
{
    FILE *f;
    int loc, l;
    char buf[LINE_SIZE+2];

    if( fname ) {
        f = fopen( fname, "r" );
        err_if( !f, "can't open program file '%s'", fname );
    }
    else
        f = stdin;

    for( loc = 0; ; ++loc ) {
        err_if( loc >= MEM_SIZE, "program exceeds memory size (%d)", MEM_SIZE );
        if( !fgets( buf, LINE_SIZE, f ) )
            break;
        l = strlen( buf );
        err_if( l >= LINE_SIZE, "program line %d is too long", loc+1 );
        if( !isdigit(buf[0]) && buf[0] != '-' ) {
            --loc;
			if( tracing && buf[0] == '!' )
				fprintf( stderr, "%s", buf );
            continue;
        }
        memory[loc] = strtol( buf, NULL, 10 );
    }

    if( fname )
        fclose( f );
}

    void
execute( void )
{
    int op;     /* the current operand pointer      */
    int pc;     /* the current instruction pointer  */
    int skip;   /* skip the next instruction?       */

    trace( "Start at %4.4d\n\n", memory[MEM_PC] );
    while( (pc = memory[MEM_PC]) >= MEM_START ) {
        ++memory[MEM_PC];
        trace( "%4.4d: ", pc );
        err_if( pc >= MEM_SIZE || pc < 0, "%d: instruction out of range", pc );
        switch( op = memory[pc] ) {
            case MEM_ZERO:
                memory[MEM_ZERO] = 0;
                break;
            case MEM_IN:
                memory[MEM_IN] = getchar();
                break;
            case MEM_OUT:
                memory[MEM_OUT] = 2 * memory[MEM_ACC];
                putchar( memory[MEM_ACC] );
                break;
            default:
                err_if( op >= MEM_SIZE || op < 0,
                        "%d: operand out of range", pc );
                break;
        }
        trace( "%+4.4d => %+4.4d - %+4.4d = ",
               op, memory[op], memory[MEM_ACC] );
        skip = (unsigned) memory[op] < (unsigned) memory[MEM_ACC];
        memory[op] -= memory[MEM_ACC];
        memory[MEM_ACC] = memory[op];
        trace( "%+4.4d\n", memory[MEM_ACC] );
        if( skip ) {
            ++memory[MEM_PC];
            trace( "skip\n" );
        }
    }
    trace( "\nEnd\n" );
    err_if( pc != 0, "%d: abnormal termination", pc );
}

    int
main( int argc, char **argv )
{
    int i;
    char *prog = NULL;

    for( i = 1; i < argc; ++i ) {
        if( argv[i][0] == '-' ) {
            switch( argv[i][1] ) {
                case 't':
                    tracing = 1;
                    break;
                default:
                    err_if( 1, "usage:\n    urisc [-t] [progam]" );
			}
        }
        else {
            err_if( prog != NULL, "usage:\n    urisc [-t] [progam]" );
            prog = argv[i];
        }
    }
    load_program( prog );
    execute();

    return( EXIT_SUCCESS );
}
