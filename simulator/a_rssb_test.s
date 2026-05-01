.include "rssb_macros.inc"

#this is some local test data
fred:   .long 0x444444
    .skip 8
xtest: .long 0xfedcba98
ytest: .long 3
arg1:   .long 5     #function arg 1
arg2:   .long 6     #function arg 2
arg3:   .long 7

#this is the test program
    .text
origin:
    rssb _start #_start #pc, match simulator assumptions
    rssb 1 # for testing initial acc value
    rssb 0x2
    rssb 0x3
    rssb 0x4
_start:
    LITERAL 1
    LITERAL 2
    LITERAL _start
    LITERAL xtest
    LITERAL ytest
    LITERAL fred    #address of fred varible
                            .if 0
                            add LIT_2, fred
                            zjmp fred, add_done
                            sub LIT_2, fred
                            zjmp fred, add_done
                        add_done:
                           .endif
.if 0
                        jmpi_test:
                            LITERAL jmpi_test
                            jmpi LIT_jmpi_test
                        mov LIT_xtest, arg1
                        mov LIT_fred, arg2
                        mov LIT_2, arg3
                        call word_mov_function
.endif
    LITERAL 1023
    mov LIT_1023, arg1
    LITERAL 121
    mov LIT_121, arg2
    call divide_function
halt:    HALT
.if 0
                        LITERAL before_neg
                        LITERAL after_move
                    cc:
                        call test_function
                        #jmpi LIT_before_neg #LIT_after_move #LIT__start

                        jeq ytest, fred, _start
                    bb:
                        jge ytest, fred, before_neg
                    aa: jle fred, ytest, before_neg
                    a:
                        jle ytest, ytest, before_neg
                    b:
                        jge fred, ytest, _start
                    c:
                    before_neg:
                        neg fred    #test negate
                        zjmp fred _start

                    clear:
                        clr fred
                        zjmp fred clear
                    after_clear:
                        add ytest, fred
                        sub LIT_1, fred
                    jump:
                        jmp move
                    move:
                        mov xtest,fred
                        mov fred, ytest
                    after_move:
                        jmpi LIT__start
.endif
#function
test_function:
    return
#function to move data indirectly
#arg1 and arg2 contain the address literals of data to move
movi_function:
    mov arg2, mpatch1   #self modify code, set dst
    mov arg2, mpatch1+4 #self modify code, set dst
    mov arg2, mpatch1+2*4 #self modify code, set dst
    mov arg1, mpatch1+5*4 #self modify code, set src
    mov arg2, mpatch1+8*4 #self modify code, set dst
    .set mpatch1, .     #addr of clr code in mov macro below
    mov 2,3
    return
#function to move n data words
#arg1 and arg2 contain the address literals of data to move
#arg3 contains number of words to more
word_mov_function:
    VAR myret
    LITERAL 4

    mov __return, myret #preserve my return address
word_loop:
    zjmp arg3, word_return
    call movi_function   #move next word
wla:
    add LIT_4, arg1
wlb:
    add LIT_4, arg2
wls:
    dec arg3
    jmp word_loop
word_return:
    return myret

#call with arg1 is dividend, arg2 is divisor, arg3 is result
#return arg1 is remainder arg2 is divisor arg3 is result/quotient
divide_function:
    clr arg3            #clear result
    mov arg2, fred #preserve divisor
divide_loop:
    jeq arg2, arg1, div_eq #not done
    jge arg2, arg1, div_done
div_eq:
    mov arg1, scratch2
    #dividend = dividend - divisor
tsub:
#    subres arg1, arg2, res #arg2= dst-src and res= src-dst
tsub2:
    subres  scratch2, arg2, arg1  #subtract divisor from dividend
    mov fred, arg2          #restore divisor
    inc arg3        #incr quotient
    jmp divide_loop
div_done:
    return
last_addr:
