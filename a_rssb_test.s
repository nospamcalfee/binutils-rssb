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
	rssb scratch2		# acc gets address of target (wont skip)
    rssb scratch	#scratch,acc = 0 - p1 or -p1
    rssb scratch	#skipped always or was 0
    rssb PC         		# pc = pc - offset (branch)
	.set 	jump_data_\@ ,  origin-.
	.data
addr_data_\@:	.long	 jump_data_\@
	.text
.endm

# dst = src - dst
.macro sub src, dst
    clr scratch         # clear the scratch area so acc=0
    rssb \src 			# acc = src - 0
    rssb scratch		# scratch,acc = 0 - (-src)
    rssb scratch		# skipped always or was 0
    rssb \dst			# dst = dst - (-src)
    rssb scratch		# skipped unless 0
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
scratch: .long 0x1111111	#used in macros
scratch2: .long 0x2			#used in macros
j_scratch: .long 0x3			#used in macros

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
	jmpi LIT_before_neg #LIT_after_move #LIT__start
	jmp _start
before_neg:
	LITERAL 1
	neg fred	#test negate
clear:
	clr fred
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

	. = 0x400
nextdata:
