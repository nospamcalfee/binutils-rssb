// clear accumulator and memory
.macro clr p1
    rssb \p1 # clearing \p1
    rssb \p1
    rssb \p1
.endm
.macro mov p1, p2 #move data from p1 to p2
    clr \p2         #clear destination and acc
    rssb scratch    #m-acc = m-0 so no skip
    rssb scratch    #m-0 - m wont skip, clears m, acc
    rssb \p1        #load p1 into acc (acc = p1 - 0)
    rssb scratch    #scratch,acc = 0 - p1 or -p1
    rssb scratch    #skipped always or was 0
    rssb \p2        #p2 = p2 - (-p1) move result to dest
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

    .set    __jmp_\@ ,  \p1-.
    .pushsection .rodata
__ad_\@:   .long    __jmp_\@
    .popsection
.endm
# save return address after call and do the function jmp
.macro call function_addr
    mov __ad_\@, __return  #save return address
    jmp \function_addr          #start the function

    .set    __call_\@ , .
    .pushsection .rodata
__ad_\@:   .long    __call_\@
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
    rssb __ad_\@  # if zero acc, acc= -__zajmp_\@ (noskip)
    rssb \scr          # negative __ad_\@-l (mustskip) 0 (noskip)
    rssb \scr          # skipped unless 0 (nop)
    rssb PC            # conditionally changes the PC

    .set    __zajmp_\@ ,  \label-.
    .pushsection .rodata
__ad_\@:   .long    __zajmp_\@
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
    .set    __jmpi_\@ ,  .
    .pushsection .rodata
__ad_\@:   .long    __jmpi_\@
    .popsection
.endm
// dst = dst-src res=src-dst all 3 unique
.macro subres src, dst, rdst
    clr \rdst               #reverse store cleared
    rssb \src               # acc,src = src no skip
    rssb \dst               # dst-src might skip dst == dst-src
    rssb \rdst              # acc,\rdst = src-dst will skip unless src=dst
    rssb \rdst              # mainly noop ==0
.endm
# dst = src - dst
.macro sub src, dst
    subres \src, \dst, sub_scratch
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

.macro dec src
    mov LIT_1, scratch2
    sub \src, scratch2 # dst = src - dst
    mov scratch2, \src
.endm

.macro inc argu
    add LIT_1, \argu     #dst = src + dst
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


