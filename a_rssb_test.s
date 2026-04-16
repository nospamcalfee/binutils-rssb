// clear accumulator and memory
.macro clr p1
    rssb \p1 # clearing \p1
    rssb \p1
    rssb \p1
.endm
.macro mov p1, p2 #move data from p1 to p2
    clr \p2			#clear destination and acc
    rssb scratch 	#load scratch to accumulator
    rssb scratch	#clear scratch and acc - note: no skip here
    rssb \p1		#load p1 into acc (acc = p1 - 0)
    rssb scratch	#scratch,acc = 0 - p1 or -p1
    rssb scratch	#skipped always or was 0
    rssb \p2		#p2 = p2 - (-p1) p2 was zero acc was -p1
    rssb scratch	#skipped always or 0
.endm

.equ PC, 0

.macro jmp p1           # jump to label p1
    clr scratch         # clear the scratch area so acc=0
# arithmetic is done in byte addressable mode, not word addressable so *4
	rssb addr_data_\@		# acc gets address of target (wont skip)
    rssb scratch	#scratch,acc = 0 - p1 or -p1
    rssb scratch	#skipped always or was 0
    rssb PC         		# pc = pc - offset (branch)

	.set 	jump_data_\@ ,  \p1-.
	.data
addr_data_\@:	.long	 jump_data_\@
	.text
.endm
# save return address after call and do the function jmp
.macro call function_addr
	mov addr_data_\@, __return  #save return address
    jmp \function_addr          #start the function

	.set 	jump_data_\@ , .
	.data
addr_data_\@:	.long	 jump_data_\@
	.text
.endm

# simply use the __return data
.macro return
     jmpi __return
.endm
# an internal macro, scr must be cleared by caller
# take jump if acc==0
.macro ZAJMP scr, label # if acc=0		if acc<>0
	rssb \scr          # 0 (noskip)		-acc (must skip)
	rssb addr_data_\@  # if zero acc, acc= -jump_data_\@ (noskip)
	rssb \scr          # negative addr_data_\@-l (mustskip)	0 (noskip)
	rssb \scr          # skipped unless 0 (nop)
	rssb PC            # conditionally changes the PC

	.set 	jump_data_\@ ,  \label-.
	.data
addr_data_\@:	.long	 jump_data_\@
	.text

.endm

#Jump to label if test=0
.macro zjmp test label
    clr scratch        #now scratch and mem == 0
    rssb \test         #acc,mem = mem-acc wont skip
	ZAJMP scratch, \label
.endm

.macro jge src1, src2, label
	mov \src1, j_scratch
	clr scratch2	#acc=0
	rssb \src2 		#acc=src2 wont skip
	rssb j_scratch 	#compare with src1 skip if src2>src1 (borrow)
	rssb j_scratch 	#no skip, zero acc won't skip
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
	.data
LIT_\constant: .long \constant
	.text
.endm


//every jmpi instruction needs a literal reflecting the pc adjustment.
//Use scratches to deref a variable and adjust the target with the pc
//this is hard...
.macro jmpi p1          # jump to address stored in p1
	mov \p1, scratch2 #jump dest is in ..
	sub addr_data_\@, scratch2 #adjust to contents of ..
    clr scratch         # clear the scratch area so acc=0
	# arithmetic is done in byte addressable mode, not word addressable so *4
	rssb scratch2		# m-0 == acc gets address of target (wont skip)
    rssb scratch	#scratch,acc = 0 - p1 or -p1
    rssb scratch	#skipped always or was 0
    rssb PC         		# pc = pc - offset (branch)
	.set 	jump_data_\@ ,  .
	.data
addr_data_\@:	.long	 jump_data_\@
	.text
.endm

# dst = src - dst
.macro sub src, dst
    clr scratch         # clear the scratch area so acc=0
    rssb \src 			# acc,src = src no skip
    rssb \dst 			# dst-src might skip
    rssb scratch 		# acc,scratch = acc-scratch will skip unless 0
    rssb scratch 		# mainly noop
.endm

# negate src
.macro neg src
	clr scratch2
	rssb \src 			# a,src = src - 0
	rssb scratch2		# a, scratch = 0-src
    rssb scratch2		#skipped always or was 0
    mov scratch2, \src 	# now move negative src to src
.endm

# dst = src + dst == src - (-dst)
.macro add src, dst
	neg \dst			# dst = -dst
	sub \src, \dst 		# dst = src - (-dst)
.endm
	.data
scratch: .long 0x1111111    #used in macros
scratch2: .long 0x2         #used in macros
j_scratch: .long 0x3        #used in macros
__return: .long 0x4         #function return address

fred:	.long 0x444444
	.skip 4
xtest: .long 0xfedcba98
ytest: .long 3
	.text
origin:
	rssb _start #_start #pc, match simulator assumptions
	rssb 1 # for testing initial acc value
	rssb 0x2
	rssb 0x3
	rssb 0x4
_start:
	LITERAL after_move
	LITERAL _start
	LITERAL before_neg
cc:
	call test_function
	jmpi LIT_before_neg #LIT_after_move #LIT__start

	jeq ytest, fred, _start
bb:
	jge ytest, fred, before_neg
aa:	jle fred, ytest, before_neg
a:
	jle ytest, ytest, before_neg
b:
	jge	fred, ytest, _start
c:
before_neg:
	LITERAL 1
	neg fred	#test negate
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

#function
test_function:
	return
last_addr:
	. = 0x600
nextdata:
