From fear!mahar Sun Mar 23 15:02:18 EST 1986

Several people have asked me to mail them or post the macro package
for a one instruction machine called urisc. Ultimate risc.
The instruction is:
Reverse subtract and skip if borrow.
The accumulator is subtracted from the memory location and the next
instruction is skipped if there was a borrow. The result is stored in
both the accumulator and the memory location. The PC is memory location 0.

The original work on this package was done by Mike Albaugh of Atari Games.

The ADDRESS macro takes two operands. A word of memory is allocated
which contains the difference between the two operands. If the first
operand is less than the second operand 1 is subtracted from the result.
The address of the memory word is the instruction emitted.

macro clr %p1   ; clear accumulator and memory
    %p1         ; accessing the same location three
    %p1         ; time clears it and acc
    %p1
endmacro

macro mov %p1,%p2   ; move data from p1 to p2
    clr %p2         ; clear destination and acc
    scratch         ; load scratch to accumulator
    scratch         ; clear scratch and acc - note: no skip here
    %p1             ; load p1 into acc (acc = p1 - 0)
    scratch         ; scratch,acc = 0 - p1 or -p1
    scratch         ; skipped always or 0
    %p2             ; p2 = p2 - (-p1) p2 was zero acc was -p1
    scratch         ; skipped always or 0
endmacro

macro jmp %p1           ; jump to label p1
    clr scratch         ; clear the scratch area
    ADDRESS %next,%p1   ; ADDRESS is a pointer to a location
                        ; containing the expression. If p1 is less than
                        ; next subtract 1 from expression.
                        ; acc contains the difference between the label
                        ; and the current pc
%next:  pc              ; pc = pc - offset (branch)
endmacro

macro zajmp %p1         ; jump to label p1 if acc is zero. zero = 0
    zero                ; zero,acc = zero - 0 ; zero,acc = zero - #
    ADDRESS %p1,%next   ; acc = p1 - next     ; skipped
    zero                ; zero - (p1 - next)  ; zero - zero = 0
    zero                ; skipped             ; zero - 0 = 0
%next:  pc              ; pc = pc - offset    ; pc = pc - 0
endmacro

macro zjmp %p1,%p2  ; jump to p2 if p1 = 0
    clr zero        ; make zero = 0
    %p1             ; load p1
    zajmp %1        ; jump if acc zero
endmacro

sub %p1,%p2     ; p2 = p2 - p1
    clr scratch ; clear scratch,acc
    %p1         ; load p1
    %p2         ; p2,acc = p2 - p1
    scratch     ; skip if borrow
    scratch     ;
endmacro

add %p1,%p2         ; p2 = p1 + p2
    clr scratch2
    %p1
    scratch2        ; scratch2 = -p1
    sub scratch2,%p2
endmacro
