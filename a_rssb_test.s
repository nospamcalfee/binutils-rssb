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
.macro ADDRESS p1, p2
	.data
addr_data_\@:	.long	\p1
	.text
	rssb addr_data_\@
.endm
.equ PC, 0
.macro jmp p1           # jump to label p1
    clr scratch         # clear the scratch area so acc=0
				    #ADDRESS jump_data_\@,\p1   	# ADDRESS is a pointer to a location
				                        # containing the expression. If p1 is less than
				                        # following address subtract 1 from expression.
				                        # acc contains the difference between the label
				                        # and the current pc
# arithmetic is done in byte addressable mode, not word addressable so *4
	rssb addr_data_		# acc gets address of target (wont skip)
    rssb scratch	#scratch,acc = 0 - p1 or -p1
    rssb scratch	#skipped always or was 0
    rssb PC         		# pc = pc - offset (branch)
#jump_data_\@:
	.set 	jump_data_ ,  \p1-. #origin-. #\p1

	.data
addr_data_:	.long	 jump_data_
	.text
.endm

	.data
scratch: .long 0x1111111
fred:	.long 0x444444
	.skip 4
xtest: .long 0xfedcba98
ytest: .long 0
	.text
origin:
	rssb _start #_start #pc, match simulator assumptions
	rssb 0xffff0001 # for testing initial acc value
	rssb 0x2
	rssb 0x3
	rssb 0x4
_start:
clear:
	clr fred
after_clear:
jump:
	jmp jump
move:
	mov xtest,fred
after_move:
	mov fred, ytest
after_jump:
	rssb fred
	rssb xtest
cont:	rssb 100
	rssb 0x12345678
	rssb fred + 0x20 + 0x30
next:	rssb _start
	. = 512
nextdata:
