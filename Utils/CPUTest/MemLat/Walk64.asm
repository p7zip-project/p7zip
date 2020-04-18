_TEXT	SEGMENT


PUSHREG macro
	push RBX
	push RSI
	push RDI
	push R10
	push R11
	push R12
	push R13
	push R14
	push R15
endm

POPREG macro
	pop R15
	pop R14
	pop R13
	pop R12
	pop R11
	pop R10
	pop RDI
	pop RSI
	pop RBX
endm

offs equ 8 * 9
p$  = (offs + 8)
num$= (p$ + 8)
a0$ = (num$ + 8)
a1$ = (a0$ + 8)
a2$ = (a1$ + 8)
a3$ = (a2$ + 8)
a4$ = (a3$ + 8)
a5$ = (a4$ + 8)
a6$ = (a5$ + 8)
a7$ = (a6$ + 8)
a8$ = (a7$ + 8)
a9$ = (a8$ + 8)
a10$ = (a9$ + 8)
a11$ = (a10$ + 8)

align 16
WalkSum	PROC

	PUSHREG
	xor RAX, RAX
	xor RBX, RBX
	xor RSI, RSI
	xor RDI, RDI
	shr RDX, 5
	align 16
@@:
	ttt = 0
	rept 4
	add RAX, QWORD PTR [RCX + ttt + 0]
	add RBX, QWORD PTR [RCX + ttt + 8]
	add RSI, QWORD PTR [RCX + ttt + 16]
	add RDI, QWORD PTR [RCX + ttt + 24]
	ttt = ttt + 32
	endm
	
	add RCX, 128
	dec RDX
	jne  @B
	
	add RAX, RBX
	add RAX, RSI
	add RAX, RDI
	POPREG
	ret 0
WalkSum	ENDP


align 16
WalkWrite PROC

	PUSHREG
	xor RAX, RAX
	shr RDX, 5
	align 16
@@:
	ttt = 0
	rept 16
	mov QWORD PTR [RCX + ttt], RAX
	ttt = ttt + 8
	endm
	
	add RCX, 128
	dec RDX
	jne  @B
	
	POPREG
	ret 0
WalkWrite ENDP

align 16
WalkSum2 PROC

	PUSHREG
	shl R8, 2
	mov R9, R8
	add R9, R8
	mov R10, R9
	add R10, R8
	mov R11, R10
	add R11, R8

	xor RAX, RAX
	xor RBX, RBX
	xor RSI, RSI
	xor RDI, RDI

	shl RDX, 2
	add RDX, RCX
	align 16
@@:
	add EAX, DWORD PTR [RCX]
	add EBX, DWORD PTR [RCX + R8]
	add ESI, DWORD PTR [RCX + R9]
	add EDI, DWORD PTR [RCX + R10]
	add RCX, R11

	cmp RCX, RDX
	jb  @B
	
	add RAX, RBX
	add RAX, RSI
	add RAX, RDI

	POPREG
	ret
WalkSum2 ENDP


align 16
WalkWrite2 PROC

	PUSHREG
	shl R8, 2
	mov R9, R8
	add R9, R8
	mov R10, R9
	add R10, R8
	mov R11, R10
	add R11, R8

	xor RAX, RAX

	shl RDX, 2
	add RDX, RCX
	align 16
@@:
	rept 2

	mov DWORD PTR [RCX], EAX
	mov DWORD PTR [RCX + R8], EAX
	mov DWORD PTR [RCX + R9], EAX
	mov DWORD PTR [RCX + R10], EAX
	add RCX, R11

	endm

	cmp RCX, RDX
	jb  @B
	
	POPREG
	ret
WalkWrite2 ENDP

align 16
Walk1Add PROC

	PUSHREG
	mov RAX, R8
	shr RDX, 3
	align 16
@@:
	rept 8
	add EAX, DWORD PTR [RCX + RAX*4]
	endm

	dec RDX
	jne  @B
	
	POPREG
	ret 0
Walk1Add ENDP


align 16
Walk1Nop PROC

	PUSHREG
	mov RAX, R8
	xor EBX, EBX
	align 16
@@:
	rept 40
	add EAX, EBX
	endm

	mov EAX, DWORD PTR [RCX + RAX*4]

	dec RDX
	jne  @B
	
	POPREG
	ret 0
Walk1Nop ENDP

align 16
Walk1 PROC

	PUSHREG
	mov RAX, R8
	shr RDX, 3
	align 16
@@:
	rept 8
	mov EAX, DWORD PTR [RCX + RAX*4]
	endm

	dec RDX
	jne  @B
	
	POPREG
	ret 0
Walk1 ENDP

PPP macro x, y
	mov x, DWORD PTR [RCX + y * 4]
endm

QQQ macro x, y
	mov DWORD PTR [RCX + y * 4], x
endm


align 16
Walk1w PROC

	PUSHREG
	mov RAX, R8
	shr RDX, 3
	align 16
@@:
	rept 4
	
	PPP R15d, RAX
	
	QQQ R15d, RAX
	
	PPP EAX, R15
	
	QQQ EAX, R15
	
	endm

	dec RDX
	jne  @B
	
	POPREG
	ret 0
Walk1w ENDP

align 16
Walk2 PROC

	PUSHREG
	align 16
@@:
	mov R8d, DWORD PTR [RCX + R8*4]
	mov R9d, DWORD PTR [RCX + R9*4]

	dec RDX
	jne  @B
	
	mov RAX, R8
	add RAX, R9

	POPREG
	ret 0
Walk2 ENDP

align 16
Walk2w PROC

	PUSHREG
	shr RDX, 1
	align 16
@@:
	PPP R15d, R8
	PPP R14d, R9
	
	QQQ R15d, R8
	QQQ R14d, R9

	PPP R8d, R15
	PPP R9d, R14
	
	QQQ R8d, R15
	QQQ R9d, R14

	dec RDX
	jne  @B
	
	mov RAX, R8
	add RAX, R9

	POPREG
	ret 0
Walk2w ENDP


align 16
Walk3 PROC
	PUSHREG
	mov EAX, DWORD PTR a2$[rsp]
	align 16
@@:
	mov R8d, DWORD PTR [RCX + R8*4]
	mov R9d, DWORD PTR [RCX + R9*4]
	mov EAX, DWORD PTR [RCX + RAX*4]

	dec RDX
	jne  @B
	
	add RAX, R8
	add RAX, R9

	POPREG
	ret
Walk3 ENDP


align 16
Walk3w PROC
	PUSHREG
	mov EAX, DWORD PTR a2$[rsp]
	shr RDX, 1
	align 16
@@:
	PPP R15d, R8
	PPP R14d, R9
	PPP R13d, RAX
	
	QQQ R15d, R8
	QQQ R14d, R9
	QQQ R13d, RAX

	PPP R8d, R15
	PPP R9d, R14
	PPP EAX, R13
	
	QQQ R8d, R15
	QQQ R9d, R14
	QQQ EAX, R13

	dec RDX
	jne  @B
	
	add RAX, R8
	add RAX, R9

	POPREG
	ret
Walk3w ENDP


align 16
Walk4 PROC
	PUSHREG
	mov EAX, DWORD PTR a2$[rsp]
	mov ESI, DWORD PTR a3$[rsp]
	align 16
@@:
	mov R8d, DWORD PTR [RCX + R8*4]
	mov R9d, DWORD PTR [RCX + R9*4]
	mov EAX, DWORD PTR [RCX + RAX*4]
	mov ESI, DWORD PTR [RCX + RSI*4]

	dec RDX
	jne  @B
	
	add RAX, R8
	add RAX, R9
	add RAX, RSI

	POPREG
	ret
Walk4 ENDP


align 16
Walk4w PROC
	PUSHREG
	mov EAX, DWORD PTR a2$[rsp]
	mov ESI, DWORD PTR a3$[rsp]
	shr RDX, 1
	align 16
@@:

	PPP R15d, R8
	PPP R14d, R9
	PPP R13d, RAX
	PPP R12d, RSI
	
	QQQ R15d, R8
	QQQ R14d, R9
	QQQ R13d, RAX
	QQQ R12d, RSI

	PPP R8d, R15
	PPP R9d, R14
	PPP EAX, R13
	PPP ESI, R12
	
	QQQ R8d, R15
	QQQ R9d, R14
	QQQ EAX, R13
	QQQ ESI, R12

	dec RDX
	jne  @B
	
	add RAX, R8
	add RAX, R9
	add RAX, RSI

	POPREG
	ret
Walk4w ENDP


align 16
Walk5 PROC
	PUSHREG
	mov EAX, DWORD PTR a2$[rsp]
	mov ESI, DWORD PTR a3$[rsp]
	mov EDI, DWORD PTR a4$[rsp]
	align 16
@@:
	mov R8d, DWORD PTR [RCX + R8*4]
	mov R9d, DWORD PTR [RCX + R9*4]
	mov EAX, DWORD PTR [RCX + RAX*4]
	mov ESI, DWORD PTR [RCX + RSI*4]
	mov EDI, DWORD PTR [RCX + RDI*4]

	dec RDX
	jne  @B
	
	add RAX, R8
	add RAX, R9
	add RAX, RSI
	add RAX, RDI

	POPREG
	ret
Walk5 ENDP

align 16
Walk5w PROC
	PUSHREG
	mov EAX, DWORD PTR a2$[rsp]
	mov ESI, DWORD PTR a3$[rsp]
	mov EDI, DWORD PTR a4$[rsp]
	shr RDX, 1
	align 16
@@:
	PPP R15d, R8
	PPP R14d, R9
	PPP R13d, RAX
	PPP R12d, RSI
	PPP R11d, RDI
	
	QQQ R15d, R8
	QQQ R14d, R9
	QQQ R13d, RAX
	QQQ R12d, RSI
	QQQ R11d, RDI

	PPP R8d, R15
	PPP R9d, R14
	PPP EAX, R13
	PPP ESI, R12
	PPP EDI, R11

	QQQ R8d, R15
	QQQ R9d, R14
	QQQ EAX, R13
	QQQ ESI, R12
	QQQ EDI, R11

	dec RDX
	jne  @B
	
	add RAX, R8
	add RAX, R9
	add RAX, RSI
	add RAX, RDI

	POPREG
	ret
Walk5w ENDP

align 16
Walk6 PROC
	PUSHREG
	mov EAX, DWORD PTR a2$[rsp]
	mov ESI, DWORD PTR a3$[rsp]
	mov EDI, DWORD PTR a4$[rsp]
	mov EBX, DWORD PTR a5$[rsp]
	align 16
@@:
	mov R8d, DWORD PTR [RCX + R8*4]
	mov R9d, DWORD PTR [RCX + R9*4]
	mov EAX, DWORD PTR [RCX + RAX*4]
	mov ESI, DWORD PTR [RCX + RSI*4]
	mov EDI, DWORD PTR [RCX + RDI*4]
	mov EBX, DWORD PTR [RCX + RBX*4]

	dec RDX
	jne  @B
	
	add RAX, R8
	add RAX, R9
	add RAX, RSI
	add RAX, RDI
	add RAX, RBX

	POPREG
	ret
Walk6 ENDP

align 16
Walk6w PROC
	PUSHREG
	mov EAX, DWORD PTR a2$[rsp]
	mov ESI, DWORD PTR a3$[rsp]
	mov EDI, DWORD PTR a4$[rsp]
	mov EBX, DWORD PTR a5$[rsp]
	shr RDX, 1
	align 16
@@:
	PPP R15d, R8
	PPP R14d, R9
	PPP R13d, RAX
	PPP R12d, RSI
	PPP R11d, RDI
	PPP R10d, RBX
	
	QQQ R15d, R8
	QQQ R14d, R9
	QQQ R13d, RAX
	QQQ R12d, RSI
	QQQ R11d, RDI
	QQQ R10d, RBX

	PPP R8d, R15
	PPP R9d, R14
	PPP EAX, R13
	PPP ESI, R12
	PPP EDI, R11
	PPP EBX, R10

	QQQ R8d, R15
	QQQ R9d, R14
	QQQ EAX, R13
	QQQ ESI, R12
	QQQ EDI, R11
	QQQ EBX, R10

	dec RDX
	jne  @B
	
	add RAX, R8
	add RAX, R9
	add RAX, RSI
	add RAX, RDI
	add RAX, RBX

	POPREG
	ret
Walk6w ENDP

align 16
Walk7 PROC
	PUSHREG
	mov EAX, DWORD PTR a2$[rsp]
	mov ESI, DWORD PTR a3$[rsp]
	mov EDI, DWORD PTR a4$[rsp]
	mov EBX, DWORD PTR a5$[rsp]
	mov r10d, DWORD PTR a6$[rsp]
	align 16
@@:
	mov R8d, DWORD PTR [RCX + R8*4]
	mov R9d, DWORD PTR [RCX + R9*4]
	mov EAX, DWORD PTR [RCX + RAX*4]
	mov ESI, DWORD PTR [RCX + RSI*4]
	mov EDI, DWORD PTR [RCX + RDI*4]
	mov EBX, DWORD PTR [RCX + RBX*4]
	mov r10d, DWORD PTR [RCX + R10*4]

	dec RDX
	jne  @B
	
	add RAX, R8
	add RAX, R9
	add RAX, RSI
	add RAX, RDI
	add RAX, RBX
	add RAX, R10

	POPREG
	ret
Walk7 ENDP


align 16
Walk8 PROC
	PUSHREG
	mov EAX, DWORD PTR a2$[rsp]
	mov ESI, DWORD PTR a3$[rsp]
	mov EDI, DWORD PTR a4$[rsp]
	mov EBX, DWORD PTR a5$[rsp]
	mov r10d, DWORD PTR a6$[rsp]
	mov r11d, DWORD PTR a7$[rsp]
	align 16
@@:
	mov R8d, DWORD PTR [RCX + R8*4]
	mov R9d, DWORD PTR [RCX + R9*4]
	mov EAX, DWORD PTR [RCX + RAX*4]
	mov ESI, DWORD PTR [RCX + RSI*4]
	mov EDI, DWORD PTR [RCX + RDI*4]
	mov EBX, DWORD PTR [RCX + RBX*4]
	mov r10d, DWORD PTR [RCX + R10*4]
	mov r11d, DWORD PTR [RCX + R11*4]

	dec RDX
	jne  @B
	
	add RAX, R8
	add RAX, R9
	add RAX, RSI
	add RAX, RDI
	add RAX, RBX
	add RAX, R10
	add RAX, R11

	POPREG
	ret
Walk8 ENDP


align 16
Walk9 PROC
	PUSHREG
	mov EAX, DWORD PTR a2$[rsp]
	mov ESI, DWORD PTR a3$[rsp]
	mov EDI, DWORD PTR a4$[rsp]
	mov EBX, DWORD PTR a5$[rsp]
	mov r10d, DWORD PTR a6$[rsp]
	mov r11d, DWORD PTR a7$[rsp]
	mov r12d, DWORD PTR a8$[rsp]
	align 16
@@:
	mov R8d, DWORD PTR [RCX + R8*4]
	mov R9d, DWORD PTR [RCX + R9*4]
	mov EAX, DWORD PTR [RCX + RAX*4]
	mov ESI, DWORD PTR [RCX + RSI*4]
	mov EDI, DWORD PTR [RCX + RDI*4]
	mov EBX, DWORD PTR [RCX + RBX*4]
	mov r10d, DWORD PTR [RCX + R10*4]
	mov r11d, DWORD PTR [RCX + R11*4]
	mov r12d, DWORD PTR [RCX + R12*4]

	dec RDX
	jne  @B
	
	add RAX, R8
	add RAX, R9
	add RAX, RSI
	add RAX, RDI
	add RAX, RBX
	add RAX, R10
	add RAX, R11
	add RAX, R12

	POPREG
	ret
Walk9 ENDP


align 16
Walk10 PROC
	PUSHREG
	mov EAX, DWORD PTR a2$[rsp]
	mov ESI, DWORD PTR a3$[rsp]
	mov EDI, DWORD PTR a4$[rsp]
	mov EBX, DWORD PTR a5$[rsp]
	mov r10d, DWORD PTR a6$[rsp]
	mov r11d, DWORD PTR a7$[rsp]
	mov r12d, DWORD PTR a8$[rsp]
	mov r13d, DWORD PTR a9$[rsp]
	align 16
@@:
	mov R8d, DWORD PTR [RCX + R8*4]
	mov R9d, DWORD PTR [RCX + R9*4]
	mov EAX, DWORD PTR [RCX + RAX*4]
	mov ESI, DWORD PTR [RCX + RSI*4]
	mov EDI, DWORD PTR [RCX + RDI*4]
	mov EBX, DWORD PTR [RCX + RBX*4]
	mov r10d, DWORD PTR [RCX + R10*4]
	mov r11d, DWORD PTR [RCX + R11*4]
	mov r12d, DWORD PTR [RCX + R12*4]
	mov r13d, DWORD PTR [RCX + R13*4]

	dec RDX
	jne  @B
	
	add RAX, R8
	add RAX, R9
	add RAX, RSI
	add RAX, RDI
	add RAX, RBX
	add RAX, R10
	add RAX, R11
	add RAX, R12
	add RAX, R13

	POPREG
	ret
Walk10 ENDP

align 16
Walk11 PROC
	PUSHREG
	mov EAX, DWORD PTR a2$[rsp]
	mov ESI, DWORD PTR a3$[rsp]
	mov EDI, DWORD PTR a4$[rsp]
	mov EBX, DWORD PTR a5$[rsp]
	mov r10d, DWORD PTR a6$[rsp]
	mov r11d, DWORD PTR a7$[rsp]
	mov r12d, DWORD PTR a8$[rsp]
	mov r13d, DWORD PTR a9$[rsp]
	mov r14d, DWORD PTR a10$[rsp]
	align 16
@@:
	mov R8d, DWORD PTR [RCX + R8*4]
	mov R9d, DWORD PTR [RCX + R9*4]
	mov EAX, DWORD PTR [RCX + RAX*4]
	mov ESI, DWORD PTR [RCX + RSI*4]
	mov EDI, DWORD PTR [RCX + RDI*4]
	mov EBX, DWORD PTR [RCX + RBX*4]
	mov r10d, DWORD PTR [RCX + R10*4]
	mov r11d, DWORD PTR [RCX + R11*4]
	mov r12d, DWORD PTR [RCX + R12*4]
	mov r13d, DWORD PTR [RCX + R13*4]
	mov r14d, DWORD PTR [RCX + R14*4]

	dec RDX
	jne  @B
	
	add RAX, R8
	add RAX, R9
	add RAX, RSI
	add RAX, RDI
	add RAX, RBX
	add RAX, R10
	add RAX, R11
	add RAX, R12
	add RAX, R13
	add RAX, R14

	POPREG
	ret
Walk11 ENDP

align 16
Walk12 PROC
	PUSHREG
	mov EAX, DWORD PTR a2$[rsp]
	mov ESI, DWORD PTR a3$[rsp]
	mov EDI, DWORD PTR a4$[rsp]
	mov EBX, DWORD PTR a5$[rsp]
	mov r10d, DWORD PTR a6$[rsp]
	mov r11d, DWORD PTR a7$[rsp]
	mov r12d, DWORD PTR a8$[rsp]
	mov r13d, DWORD PTR a9$[rsp]
	mov r14d, DWORD PTR a10$[rsp]
	mov r15d, DWORD PTR a11$[rsp]
	align 16
@@:
	mov R8d, DWORD PTR [RCX + R8*4]
	mov R9d, DWORD PTR [RCX + R9*4]
	mov EAX, DWORD PTR [RCX + RAX*4]
	mov ESI, DWORD PTR [RCX + RSI*4]
	mov EDI, DWORD PTR [RCX + RDI*4]
	mov EBX, DWORD PTR [RCX + RBX*4]
	mov r10d, DWORD PTR [RCX + R10*4]
	mov r11d, DWORD PTR [RCX + R11*4]
	mov r12d, DWORD PTR [RCX + R12*4]
	mov r13d, DWORD PTR [RCX + R13*4]
	mov r14d, DWORD PTR [RCX + R14*4]
	mov r15d, DWORD PTR [RCX + R15*4]

	dec RDX
	jne  @B
	
	add RAX, R8
	add RAX, R9
	add RAX, RSI
	add RAX, RDI
	add RAX, RBX
	add RAX, R10
	add RAX, R11
	add RAX, R12
	add RAX, R13
	add RAX, R14
	add RAX, R15

	POPREG
	ret
Walk12 ENDP

_TEXT	ENDS


end
