// clear accumulator and memory
.macro clr p1
    rssb \p1 # clearing \p1
    rssb \p1
    rssb \p1
.endm
.macro mov p1, p2 #move data from p1 to p2
    clr \p2         #clear destination and acc
    rssb scratch    #load scratch to accumulator
    rssb scratch    #clear scratch and acc - note: no skip here
    rssb \p1        #load p1 into acc (acc = p1 - 0)
    rssb scratch    #scratch,acc = 0 - p1 or -p1
    rssb scratch    #skipped always or was 0
    rssb \p2        #p2 = p2 - (-p1) p2 was zero acc was -p1
    rssb scratch    #skipped always or 0
.endm

.equ PC, 0

.macro jmp p1           # jump to label p1
    clr scratch         # clear the scratch area so acc=0
# arithmetic is done in byte addressable mode, not word addressable so *4
    rssb __ad_\@       # acc gets address of target (wont skip)
    rssb scratch    #scratch,acc = 0 - p1 or -p1
    rssb scratch    #skipped always or was 0
    rssb PC                 # pc = pc - offset (branch)

    .set    __jd_\@ ,  \p1-.
    .pushsection .rodata
__ad_\@:   .long    __jd_\@
    .popsection
.endm
# save return address after call and do the function jmp
.macro call function_addr
    mov __ad_\@, __return  #save return address
    jmp \function_addr          #start the function

    .set    __jd_\@ , .
    .pushsection .rodata
__ad_\@:   .long    __jd_\@
    .popsection
.endm

# simply use the __return data
.macro return var=__return
     jmpi \var
.endm
# an internal macro, scr must be cleared by caller
# take jump if acc==0
.macro ZAJMP scr, label # if acc=0      if acc<>0
    rssb \scr          # 0 (noskip)     -acc (must skip)
    rssb __ad_\@  # if zero acc, acc= -__jd_\@ (noskip)
    rssb \scr          # negative __ad_\@-l (mustskip) 0 (noskip)
    rssb \scr          # skipped unless 0 (nop)
    rssb PC            # conditionally changes the PC

    .set    __jd_\@ ,  \label-.
    .pushsection .rodata
__ad_\@:   .long    __jd_\@
    .popsection

.endm

#Jump to label if test=0
.macro zjmp test label
    clr scratch        #now scratch and mem == 0
    rssb \test         #acc,mem = mem-acc wont skip
    ZAJMP scratch, \label
.endm

.macro jge src1, src2, label
    mov \src1, j_scratch
    clr scratch2    #acc=0
    rssb \src2      #acc=src2 wont skip
    rssb j_scratch  #compare with src1 skip if src2>src1 (borrow)
    rssb j_scratch  #no skip, zero acc won't skip
    ZAJMP scratch2, \label
.endm
.macro jle src1, src2, label
    jge \src2, \src1, \label
.endm
.macro jeq src1, src2, label
    mov \src1, j_scratch
    sub \src2, j_scratch
    zjmp j_scratch, \label
.endm
.macro LITERAL constant
    .pushsection .rodata
LIT_\constant: .long \constant
    .popsection
.endm


//every jmpi instruction needs a literal reflecting the pc adjustment.
//Use scratches to deref a variable and adjust the target with the pc
//this is hard...
.macro jmpi p1          # jump to address stored in p1
    mov \p1, scratch2 #jump dest is in ..
    sub __ad_\@, scratch2 #adjust to contents of ..
    clr scratch         # clear the scratch area so acc=0
    # arithmetic is done in byte addressable mode, not word addressable so *4
    rssb scratch2       # m-0 == acc gets address of target (wont skip)
    #rssb scratch    #scratch,acc = 0 - p1 or -p1
    #rssb scratch    #skipped always or was 0
    rssb PC                 # pc = pc - offset (branch)
    .set    __jd_\@ ,  .
    .pushsection .rodata
__ad_\@:   .long    __jd_\@
    .popsection
.endm

# dst = src - dst
.macro sub src, dst
    clr  sub_scratch       # clear the sub_scratch area so acc=0
    rssb \src              # acc,src = src no skip
    rssb \dst              # dst-src might skip dst == dst-src
    rssb sub_scratch       # acc,sub_scratch = src-dst will skip unless src=dst
    rssb sub_scratch       # mainly noop
    mov  sub_scratch,\dst  # get correct result
.endm

# negate src
.macro neg src
    clr scratch2
    rssb \src           # a,src = src - 0
    rssb scratch2       # a, scratch = 0-src
    rssb scratch2       #skipped always or was 0
    mov scratch2, \src  # now move negative src to src
.endm

# dst = src + dst == src - (-dst)
.macro add src, dst
    neg \dst            # dst = -dst
    sub \src, \dst      # dst = src - (-dst)
.endm

.macro HALT
    rssb 0x100000       #cause simulator mem fault
.endm

#macro to create a data variable
.macro VAR name
    .pushsection .data
\name:   .long    0
    .popsection
.endm

#this is the programs initial data needed by macros
    .data
scratch: .long 0x1111111    #used in macros
scratch2: .long 0x2         #used in macros
j_scratch: .long 0x3        #used in macros
sub_scratch: .long 0x4
__return: .long 0x4         #function return address





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
.endif
    mov LIT_xtest, arg1
    mov LIT_fred, arg2
    mov LIT_2, arg3
    call word_mov_function
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
.set src, arg1
.set dst, arg2
.set cnt, arg3
    LITERAL 4

    mov __return, myret #preserve my return address
word_loop:
    zjmp cnt, word_return
    call movi_function   #move next word
wla:
    add LIT_4, src
wlb:
    add LIT_4, dst
    mov LIT_1, scratch2
    sub cnt, scratch2
    mov scratch2, cnt
    jmp word_loop
word_return:
    return myret
last_addr:
