# RSSB cpu

## background

Long ago there was a discussion on usenet about what is the simplest
possible Turing complete computer. Several were proposed, but it seems
that probably a URISC or OISC is the simplest. This computer only has
one op-code, so no bits are used in memory to describe the operation,
all memory bits are available for addresses. There are many possible
variants, some using two "words" per instruction, some three.

I am interested in implementing a nano-scale, maybe biological
mechanical computer and/or a optical based computer. So a simple
implementation is essential. I choose the simplest possible
implementation, each instruction is one memory word long and does
a "reverse subtract and skip on Borrow". This machine has two
registers, a PC and an accummulator. Every operation subtracts the
accumulator from the memory pointed to by the PC. If the operation
generates a borrow, the PC is skipped to the next instruction.

This computer is very inefficient in its use of memory bandwidth. Each
instruction requires 3 memory accesses.

I like it because the ALU could be done 1-bit serial and it is
conceivable to implement millions of processors running
simultaneously.

Note that a 1 bit subtractor could be a 3 input 2 output lookup table.
So for an optical computer it could be done all optically where at
least the cpu and memory data rates could be lots of gigahertz. Each
cpu would spend lots of time waiting for its memory to arrive serially.
However there are strategies to deal with this depending on "hardware"
implementation details.

See an older javascript/html version of rssb: 

https://github.com/nospamcalfee/RSSB/tree/master

It is pretty terrible.

## Binutils

So lately I decided to try to convert real tools for this hypothetical processor. Big problem, I converted Binutils to add tools and assembler for rssb. The tools are a horrible convoluted mess of C programs hacked at over the years as new "must support" processors were added.

I wanted a 64 bit word with the low 6 bits being a bit address. Binutils believes every computer is byte oriented. Extending Binutils is an exercise for the reader. I was willing to fallback to 32 bits and have word addressing. That also is a no-go. Supposedly there are DSP processors supported where their "bytes" are 24 or 32 bits, but they are not in this current tree.

I could not have gotten even this primitive, hampered version of a Binutils for a rssb cpu without code from some other nut who implemented a simple machine using Binutils.

https://github.com/joe-legg/subleq-binutils

### Description of CPU operation

As a fall back, this implementation uses addresses that are 32 bits, with the low 2 bits being a byte offset. non-zero byte offsets are Not implemented yet by my simulator.

This allows really tricky programming where the top n-32 bits of mem
are used in the subtract. It also is a right shift of acc by n bits
before the subtract. The only legal values of n in this implementation is 0, 8, 16, 24 (or offsets to the desired bytes).

The high n-32 bits of mem is subtracted from the low n-32 bits of
acc. Only n-32 bits in mem and acc are the updated by the subtract.
The other n bits are unchanged.

acc,mem <- mem-acc. 32 bits if on 32 bit aligned address
acc,mem <- mem-(acc<<bits) if unaligned.
borrow is set on the 32-n bits actually acted on in the subtract.

so  if n==8 24=32-8 bits are affected in both mem=MMM..x ACC=x..AAA,
M,A are replaced by the result of the subtract where M is the
subtracted memory bytes and A is the subtracted ACC bytes and x are
unchanged by the subtract. borrow flag is set based on the bits in
the subtract. This is a TODO issue for my simulator and may cause problems in Binutils.

For now, I am making a simplified machine. Borrow is only used for
skips, and is not used to borrow from the next subtract. Borrow
assumes unsigned 32 bit cardinals are used, (no signs). The program
counter, pc will never have the low byte address bits set.

The simulator supposedly supports a few extra "cheats" registers that are not necessary and so I have not tested. I want the simplest possible hardware implementation. The accumulator register and the zero register are such cheats. Some cpus commonly called "move machines", implement cpu instructions by storing and loading from special memory registers where hardware does magic. Not in the spirit of the simplest possible cpu.

I do think that eventually a "fork" and "join" location will be needed for starting up other cpus which are waiting to get started. Memory will need to be more complicated with local and global areas for multicpu processing. There is no need for interrupts if I have enough cpus to dedicate some to handling hardware.

I am personally uncomfortable with the pc being a register. Perhaps everything could be position independent and changes to the pc could be count of the number of instructions to skip.

I also try to not use the acc after jump instructions, so future hardware could use the nbits of acc as some kind of flag for testing against serial memory data arriving on all unused cpus. If we have 1,000,000 processors, the cpus will have 4,000,000 bytes of hardware memory and maybe another 4,000,000 bytes of pc. That is a lot, but will fit in current x86 semiconductor cpu caches.

I doubt I will ever live to see it but an optical h/w designer could create constraints that I would be able to handle.

## Build Binutils

Ha, that is a job that may work! Look at the README files in the base directory. Lots of on-line help.

## Use Binutils

In a terminal go to the simulator directory. Assemble, link and prep the test a_rssb_test.s assembly language program using ./assm It is a batch file using several of the rssb custom Binutils tools.

Then build the simulator:
...
gcc -g -O0 urisc.c sym_lookup.c -o uriskit
...

The simulator can be debugged by gdb on an x86 Linux box using

...
gdb --args ../simulator/uriskit ../build/a.input
...

The simulated rssb cpu code cannot be debugged with gdb, you'll have to look at terminal output.

## BACKGROUND INFO

Inspiration on why do a mechanical computer came from a friend of mine
who is a docent at the Computer History Museum in Mountain View, Ca. He
gives a great demo on a Babbage Difference Engine II

A Babbage Difference Engine II is not a computer, it is a calculator,
designed by Babbage to calculate successive Polynomials. What is
impressive is it is all done with gears and is the size of a grand
piano and the weight of a small car.

Interesting old newsgroup stuff was saved in .../binutils/simulator/oisc_mail.

## A Challenge

I have done lots of assembly language programming in my 50+ year career (not so much in the last 30 years). The rssb assembly is by far the hardest to code and get working.