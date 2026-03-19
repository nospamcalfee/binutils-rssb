// clear accumulator and memory
.macro clr p1
    rssb \p1
    rssb \p1
    rssb \p1
.endm
	.data
fred:	.skip 4
xtest: .long 0xfedcba98
	.text
_start:
	rssb 0x444
	rssb fred
	rssb xtest
	clr fred
	clr 0x123
cont:	rssb 100
	rssb 0x12345678
	rssb fred + 0x20 + 0x30
next:	rssb _start
	. = 512
   rssb 0x55
   rssb 0xAA
nextdata:
