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

/* to build:
gcc -g -O0 urisc.c sym_lookup.c -o uriskit
*/
/* to assemble rssb code to run
#!/bin/bash
../../install/bin/rssb-elf-as ../a_rssb_test.s
../../install/bin/rssb-elf-ld -T ../rssb.ld --print-memory-usage a.out -o a.bin
../../install/bin/rssb-elf-objdump -s -t a.bin
../../install/bin/rssb-elf-objcopy -O binary a.bin a.flat
hexdump -v -e '1/4 "0x%08x\n"' a.flat >a.input

*/
/* to run:
./uriskit ../build/a.input
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include "sym_lookup.h"
#include <elf.h>

/*
 * Memory mapped registers.
 */
enum {
    MEM_PC,     /* program counter  */
    MEM_ACC=4,    /* accumulator      */
    MEM_ZERO=8,   /* zero register    */
    MEM_IN=0xc,     /* input port       */
    MEM_OUT=0x10,    /* output port      */
};

#define MEM_SIZE    (10*1024)
#define LINE_SIZE   80

uint32_t memory[MEM_SIZE];   /* the memory space of the machine  */
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
        memory[loc] = strtol( buf, NULL, 16 );
    }

    if( fname )
        fclose( f );
}
#define PC_STEP_SIZE 4
uint8_t *address(void * mem, uint32_t offs) {
    err_if(offs > MEM_SIZE, "address (%x) too big", offs);
    uint8_t *memo = mem;
    return memo + offs;
}
//function to process address and return one memory word
//for now offset is a byte address as required by binutils
uint32_t readmem(uint32_t offs) {
    return memory[offs>>2];
}//function to process address and write val to memory word
uint32_t writemem(uint32_t offs, uint32_t val) {
    memory[offs>>2] = val;
}
// function to dump simulator memory words
void hexdump(const char *desc, uint32_t addr, uint32_t len) {
    uint32_t i;
    unsigned char buff[17];

    if (desc != NULL)
        printf("%s:\n", desc);

    for (i = 0; i < len; i++) {
        uint32_t word = readmem(addr + (i * 4)); //access sim memory
        if ((i % 4) == 0) {
            if (i != 0)
                printf("  %s\n", buff);
            printf("  %06x ", addr + (i * 4));
        }

        printf(" %08x", word);

        for (int j = 0 ; j < 4; j++) {
            uint8_t byte = word >> (j * 8);
            if ((byte < 0x20) || (byte > 0x7e))
                buff[i % 16] = '.';
            else
                buff[i % 16] = byte;
            buff[(i % 16) + 1] = '\0';
        }
    }

    while ((i % 4) != 0) {
        printf("   ");
        i++;
    }
    printf("  %s\n", buff);
}

void execute( char *elf )
{
    int op;     /* the current operand pointer      */
    int pc;     /* the current instruction pointer  */
    int skip;   /* skip the next instruction?       */
    void* map;
    off_t size;
    int fd;
    struct var_info vars;

    if (elf) {
        fd = open(elf, O_RDONLY);
        err_if( fd < 0, "could not open elf file: %s", elf );

        // Map the file into memory for easier access
        size = lseek(fd, 0, SEEK_END);
        map = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    }
    printf( "Start at %4.4x optional elffile=%s\n\n", readmem(MEM_PC), elf);
    while( (pc = readmem(MEM_PC)) && pc <= MEM_SIZE ) {
        writemem(MEM_PC, pc + PC_STEP_SIZE); //incr, mempc, use temp pc
        if (elf && tracing) {
            vars = find_symbol_by_address(map, pc);
            if (vars.name && *vars.name) {
                trace("AT %s in %s\n", vars.name, vars.sectname);
            }
        }
        trace( "%4.4x: ", pc /* * PC_STEP_SIZE*/);
        err_if( pc >= MEM_SIZE || pc < 0, "%d: instruction out of range", pc );
        switch( op = readmem(pc) ) {
            case MEM_ZERO:
                writemem(MEM_ZERO, 0);
                break;
            case MEM_IN:
                writemem(MEM_IN, getchar());
                break;
            case MEM_OUT:
                //fixme what does the 2* mean?
                writemem(MEM_OUT, 2 * readmem(MEM_ACC));
                putchar( readmem(MEM_ACC) );
                break;
            default:
                err_if( op >= MEM_SIZE || op < 0,
                        "addr=0x%x @0x%x operand out of range", op, pc );
                break;
        }
        if (elf && tracing) {
            vars = find_symbol_by_address(map, op);
            if (vars.name && *vars.name) {
                trace( "%+4.4x (%11s in %-7s) =>", op, vars.name, vars.sectname);
            } else {
                trace( "%+4.4x =>", op);
            }
            trace(" %+4.4x - (acc)%+4.4x = ", readmem(op), readmem(MEM_ACC) );
        } else {
            trace( "%+4.4x => %+4.4x - (acc)%+4.4x = ",
               op, readmem(op), readmem(MEM_ACC) );
        }
        skip = readmem(op) < readmem(MEM_ACC);
        writemem(op, readmem(op) - readmem(MEM_ACC));
        writemem(MEM_ACC, readmem(op));
        trace( "%+4.4x\n", readmem(MEM_ACC) );
        if( skip && op) {
            //for now dont skip when pc is affected
            memory[MEM_PC] += PC_STEP_SIZE;
            trace( "skip\n" );
        }
        if (!op && tracing) {
            //pc change
            if (elf) {
                vars = find_sector_by_name(map, ".data");
                if (vars.name && *vars.name) {
                    hexdump(".data vars", vars.baseaddress, vars.sh_size / 4);
                }

            } else {
                hexdump("variables", 0x2000 , 0x20);
            }
        }
    }
    trace( "\nEnd\n" );
    if (elf) {
        // stay tidy
        munmap(map, size);
        close(fd);
    }
    err_if( pc != 0, "%d: abnormal termination", pc );
}

    int
main( int argc, char **argv )
{
    int i;
    char *prog = NULL;
    char *elffile = NULL;

    for( i = 1; i < argc; ++i ) {
        if( argv[i][0] == '-' ) {
            switch( argv[i][1] ) {
                case 't':
                    tracing = 1;
                    break;
                case 'e':
                    i++; //bad code practice skip to next argv
                    elffile = argv[i];
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
    execute(elffile);

    return( EXIT_SUCCESS );
}
