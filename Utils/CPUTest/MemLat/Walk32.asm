.386
.model flat

OPTION PROLOGUE:NONE
OPTION EPILOGUE:NONE

.data
_TEXT$00	SEGMENT PARA PUBLIC 'CODE'


PUSHREG macro
	push EBP
	push EBX
	push ESI
	push EDI
endm

POPREG macro
	pop EDI
	pop ESI
	pop EBX
	pop EBP
endm

offs equ 4 * 4
a0$ = (offs + 4)
a1$ = (a0$ + 4)
a2$ = (a1$ + 4)
a3$ = (a2$ + 4)
a4$ = (a3$ + 4)
a5$ = (a4$ + 4)

align 16
@WalkSum@8 PROC

	PUSHREG
	xor EAX, EAX
	xor EBX, EBX
	xor ESI, ESI
	xor EDI, EDI
	shr EDX, 4
	align 16
@@:
	ttt = 0
	rept 4
	add EAX, DWORD PTR [ECX + ttt + 0]
	add EBX, DWORD PTR [ECX + ttt + 4]
	add ESI, DWORD PTR [ECX + ttt + 8]
	add EDI, DWORD PTR [ECX + ttt + 12]
	ttt = ttt + 16
	endm

	add ECX, 64
	dec EDX
	jne  @B

	add EAX, EBX
	add EAX, ESI
	add EAX, EDI
	POPREG
	ret 0
@WalkSum@8	ENDP


align 16
@WalkWrite@8 PROC

	PUSHREG
	xor EAX, EAX
	shr EDX, 4
	align 16
@@:
	ttt = 0
	rept 16
	mov DWORD PTR [ECX + ttt], EAX
	ttt = ttt + 4
	endm

	add ECX, 64
	dec EDX
	jne  @B

	POPREG
	ret 0
@WalkWrite@8	ENDP

align 16
@WalkSum2@12 PROC

	PUSHREG
	mov ESI, DWORD PTR a0$[ESP]
	shl ESI, 2
	mov EDI, ESI
	shl EDI, 1
	xor EAX, EAX
	xor EBX, EBX
	shl EDX, 2
	add EDX, ECX
	align 16
@@:
	add EAX, DWORD PTR [ECX]
	add EBX, DWORD PTR [ECX + ESI]
	add ECX, EDI

	cmp ECX, EDX
	jb  @B
	
	add EAX, EBX
	POPREG
	ret 4
@WalkSum2@12 ENDP

align 16
@WalkWrite2@12 PROC

	PUSHREG
	mov ESI, DWORD PTR a0$[ESP]
	shl ESI, 2
	mov EDI, ESI
	mov EBX, ESI
	add EBX, ESI
	add EBX, ESI
	shl EDI, 2
	xor EAX, EAX
	shl EDX, 2
	add EDX, ECX
	align 16
@@:
	rept 2
	mov DWORD PTR [ECX], EAX
	mov DWORD PTR [ECX + ESI], EAX
	mov DWORD PTR [ECX + ESI * 2], EAX
	mov DWORD PTR [ECX + EBX], EAX
	add ECX, EDI
	endm

	cmp ECX, EDX
	jb  @B
	
	POPREG
	ret 4
@WalkWrite2@12 ENDP

align 16
@Walk1Add@12 PROC

	PUSHREG
	mov EAX, DWORD PTR a0$[ESP]
	shr EDX, 3
	align 16
@@:
	rept 8
	add EAX, DWORD PTR [ECX + EAX*4]
	endm

	dec EDX
	jne  @B
	
	POPREG
	ret 4
@Walk1Add@12 ENDP



align 16
@Walk1Nop@12 PROC

	PUSHREG
	mov EAX, DWORD PTR a0$[ESP]
	xor EBX, EBX
	align 16
@@:
	rept 40
	add EAX, EBX
	endm
	
	mov EAX, DWORD PTR [ECX + EAX*4]

	dec EDX
	jne  @B
	
	POPREG
	ret 4
@Walk1Nop@12 ENDP


align 16
@Walk1@12 PROC

	PUSHREG
	mov EAX, DWORD PTR a0$[ESP]
	shr EDX, 3
	align 16
@@:
	rept 8
	mov EAX, DWORD PTR [ECX + EAX*4]
	endm

	dec EDX
	jne  @B
	
	POPREG
	ret 4
@Walk1@12 ENDP
  
PPP macro x, y
	mov x, DWORD PTR [ECX + y * 4]
endm

QQQ macro x, y
	mov DWORD PTR [ECX + y * 4], x
endm

align 16
@Walk1w@12 PROC

	PUSHREG
	mov EAX, DWORD PTR a0$[ESP]
	shr EDX, 3
	align 16
@@:
	rept 4
	PPP ESI, EAX
	QQQ ESI, EAX
	PPP EAX, ESI
	QQQ EAX, ESI
	endm

	dec EDX
	jne  @B
	
	POPREG
	ret 4
@Walk1w@12 ENDP

align 16
@Walk2@16 PROC

	PUSHREG
	mov EAX, DWORD PTR a0$[ESP]
	mov EBX, DWORD PTR a1$[ESP]
	shr EDX, 2
	align 16
@@:
	rept 4
	mov EAX, DWORD PTR [ECX + EAX*4]
	mov EBX, DWORD PTR [ECX + EBX*4]
	endm

	dec EDX
	jne  @B
	
	add EAX, EBX

	POPREG
	ret 8
@Walk2@16 ENDP


align 16
@Walk2w@16 PROC

	PUSHREG
	mov EAX, DWORD PTR a0$[ESP]
	mov EBX, DWORD PTR a1$[ESP]
	shr EDX, 2
	align 16
@@:
	rept 2

	PPP ESI, EAX
	PPP EDI, EBX
	
	QQQ ESI, EAX
	QQQ EDI, EBX
	
	PPP EAX, ESI
	PPP EBX, EDI
	
	QQQ EAX, ESI
	QQQ EBX, EDI
	endm

	dec EDX
	jne  @B
	
	add EAX, EBX

	POPREG
	ret 8
@Walk2w@16 ENDP

align 16
@Walk3@20 PROC
	PUSHREG
	mov EAX, DWORD PTR a0$[ESP]
	mov EBX, DWORD PTR a1$[ESP]
	mov ESI, DWORD PTR a2$[ESP]
	shr EDX, 1
	align 16
@@:
	rept 2
	mov EAX, DWORD PTR [ECX + EAX*4]
	mov EBX, DWORD PTR [ECX + EBX*4]
	mov ESI, DWORD PTR [ECX + ESI*4]
	endm

	dec EDX
	jne  @B
	
	add EAX, EBX
	add EAX, ESI

	POPREG
	ret 12
@Walk3@20 ENDP


align 16
@Walk4@24 PROC
	PUSHREG
	mov EAX, DWORD PTR a0$[ESP]
	mov EBX, DWORD PTR a1$[ESP]
	mov ESI, DWORD PTR a2$[ESP]
	mov EDI, DWORD PTR a3$[ESP]
	shr EDX, 1
	align 16
@@:
	rept 2
	mov EAX, DWORD PTR [ECX + EAX*4]
	mov EBX, DWORD PTR [ECX + EBX*4]
	mov ESI, DWORD PTR [ECX + ESI*4]
	mov EDI, DWORD PTR [ECX + EDI*4]
	endm

	dec EDX
	jne  @B
	
	add EAX, EBX
	add EAX, ESI
	add EAX, EDI

	POPREG
	ret 16
@Walk4@24 ENDP


align 16
@Walk5@28 PROC
	PUSHREG
	mov EAX, DWORD PTR a0$[ESP]
	mov EBX, DWORD PTR a1$[ESP]
	mov ESI, DWORD PTR a2$[ESP]
	mov EDI, DWORD PTR a3$[ESP]
	mov EBP, DWORD PTR a4$[ESP]
	shr EDX, 1
	align 16
@@:
	rept 2
	mov EAX, DWORD PTR [ECX + EAX*4]
	mov EBX, DWORD PTR [ECX + EBX*4]
	mov ESI, DWORD PTR [ECX + ESI*4]
	mov EDI, DWORD PTR [ECX + EDI*4]
	mov EBP, DWORD PTR [ECX + EBP*4]
	endm

	dec EDX
	jne  @B
	
	add EAX, EBX
	add EAX, ESI
	add EAX, EDI
	add EAX, EBP

	POPREG
	ret  20
@Walk5@28 ENDP


align 16
@Walk1d@8 PROC

	PUSHREG
	xor EAX, EAX
	shr ECX, 3
	align 16
@@:
	rept 8
	mov EDX, DWORD PTR [EDX]
	endm

	dec ECX
	jne  @B

	mov EAX, EDX
	
	POPREG
	ret 0
@Walk1d@8 ENDP


MMM macro x, y
	mov x, DWORD PTR [y]
endm

NNN macro x, y
	mov DWORD PTR [y], x
endm


align 16
@Walk1wd@8 PROC

	PUSHREG
	xor EAX, EAX
	shr ECX, 3
	align 16
@@:
	rept 4

	MMM ESI, EDX
	
	NNN ESI, EDX
	
	MMM EDX, ESI
	
	NNN EDX, ESI

	endm

	dec ECX
	jne  @B

	mov EAX, EDX
	
	POPREG
	ret 0
@Walk1wd@8 ENDP

  
align 16
@Walk2d@12 PROC

	PUSHREG
	mov EBX, DWORD PTR a0$[ESP]
	shr ECX, 2
	align 16
@@:
	rept 4
	mov EDX, DWORD PTR [EDX]
	mov EBX, DWORD PTR [EBX]
	endm

	dec ECX
	jne  @B

	mov EAX, EDX
	add EAX, EBX

	POPREG
	ret 4
@Walk2d@12 ENDP


align 16
@Walk2wd@12 PROC

	PUSHREG
	mov EBX, DWORD PTR a0$[ESP]
	shr ECX, 2
	align 16
@@:
	rept 2

	MMM ESI, EDX
	MMM EDI, EBX
	
	NNN ESI, EDX
	NNN EDI, EBX
	
	MMM EDX, ESI
	MMM EBX, EDI
	
	NNN EDX, ESI
	NNN EBX, EDI

	endm

	dec ECX
	jne  @B

	mov EAX, EDX
	add EAX, EBX

	POPREG
	ret 4
@Walk2wd@12 ENDP

align 16
@Walk3d@16 PROC
	PUSHREG
	mov EBX, DWORD PTR a0$[ESP]
	mov ESI, DWORD PTR a1$[ESP]
	shr ECX, 1
	align 16
@@:
	rept 2
	mov EDX, DWORD PTR [EDX]
	mov EBX, DWORD PTR [EBX]
	mov ESI, DWORD PTR [ESI]
	endm

	dec ECX
	jne  @B
	
	mov EAX, EDX
	add EAX, EBX
	add EAX, ESI

	POPREG
	ret 8
@Walk3d@16 ENDP


align 16
@Walk3wd@16 PROC
	PUSHREG
	mov EBX, DWORD PTR a0$[ESP]
	mov EAX, DWORD PTR a1$[ESP]
	shr ECX, 1
	align 16
@@:
	rept 1

	MMM ESI, EDX
	MMM EDI, EBX
	MMM EBP, EAX
	
	NNN ESI, EDX
	NNN EDI, EBX
	NNN EBP, EAX
	
	MMM EDX, ESI
	MMM EBX, EDI
	MMM EAX, EBP
	
	NNN EDX, ESI
	NNN EBX, EDI
	NNN EAX, EBP

	endm

	dec ECX
	jne  @B
	
	add EAX, EDX
	add EAX, EBX

	POPREG
	ret 8
@Walk3wd@16 ENDP

align 16
@Walk4d@20 PROC
	PUSHREG
	mov EBX, DWORD PTR a0$[ESP]
	mov ESI, DWORD PTR a1$[ESP]
	mov EDI, DWORD PTR a2$[ESP]
	shr ECX, 1
	align 16
@@:
	rept 2
	mov EDX, DWORD PTR [EDX]
	mov EBX, DWORD PTR [EBX]
	mov ESI, DWORD PTR [ESI]
	mov EDI, DWORD PTR [EDI]
	endm

	dec ECX
	jne  @B
	
	mov EAX, EDX
	add EAX, EBX
	add EAX, ESI
	add EAX, EDI

	POPREG
	ret 12
@Walk4d@20 ENDP


align 16
@Walk5d@24 PROC
	PUSHREG
	mov EBX, DWORD PTR a0$[ESP]
	mov ESI, DWORD PTR a1$[ESP]
	mov EDI, DWORD PTR a2$[ESP]
	mov EBP, DWORD PTR a3$[ESP]
	shr ECX, 1
	align 16
@@:
	rept 2
	mov EDX, DWORD PTR [EDX]
	mov EBX, DWORD PTR [EBX]
	mov ESI, DWORD PTR [ESI]
	mov EDI, DWORD PTR [EDI]
	mov EBP, DWORD PTR [EBP]
	endm

	dec ECX
	jne  @B
	
	mov EAX, EDX
	add EAX, EBX
	add EAX, ESI
	add EAX, EDI
	add EAX, EBP

	POPREG
	ret  16
@Walk5d@24 ENDP


align 16
@Walk6d@28 PROC
	PUSHREG
	mov EBX, DWORD PTR a0$[ESP]
	mov ESI, DWORD PTR a1$[ESP]
	mov EDI, DWORD PTR a2$[ESP]
	mov EBP, DWORD PTR a3$[ESP]
	mov EAX, DWORD PTR a4$[ESP]
	shr ECX, 1
	align 16
@@:
	rept 2
	mov EDX, DWORD PTR [EDX]
	mov EBX, DWORD PTR [EBX]
	mov ESI, DWORD PTR [ESI]
	mov EDI, DWORD PTR [EDI]
	mov EBP, DWORD PTR [EBP]
	mov EAX, DWORD PTR [EAX]
	endm

	dec ECX
	jne  @B
	
	add EAX, EDX
	add EAX, EBX
	add EAX, ESI
	add EAX, EDI
	add EAX, EBP

	POPREG
	ret  20
@Walk6d@28 ENDP



_TEXT$00	ENDS


end
