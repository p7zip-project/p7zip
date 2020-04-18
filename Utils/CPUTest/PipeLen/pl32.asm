.386
.model flat

OPTION PROLOGUE:NONE
OPTION EPILOGUE:NONE

.data
_TEXT$00	SEGMENT PARA PUBLIC 'CODE'


MY_START macro
	push EBX
	push EBP
	xor EAX, EAX
	xor EBX, EBX
	mov EBP, ECX
	xor ECX, ECX
	shr EDX, 2
endm

FIN macro
	dec EDX
	jnz lab1
	pop EBP
	pop EBX
	ret 0
endm

REP_MOVZX macro num
	rept num
	movzx EAX, BYTE PTR [ECX + EBX]
	xor EBX, EAX
	endm
endm

REP_IMUL macro num
	rept num
	IMUL EBX, 1
	endm
endm

REP_ADD macro num
	rept num
	add EBX, EAX
	endm
endm

READ_VAL_8 macro
	movzx EBX, BYTE PTR [EBP + EBX]
	add EBP, 1
endm

READ_VAL_32 macro
	mov EBX, DWORD PTR [EBP + EBX]
	add EBP, 4
endm

READ_VAL_32_2 macro
	sub EBP, EBX
	add EBP, EBX
	mov EBX, DWORD PTR [EBP]
	add EBP, 4
endm

TEST_MOVZX macro numOps1, numOps2
MY_START
	shl EDX, 2
	mov ECX, EBP
	sub ECX, 256
	READ_VAL_8
	align 16
lab1:
	ttt = 0
	rept 8
	REP_MOVZX numOps1
	shr EBX, 1
	jc @F
	REP_MOVZX numOps2
	ttt = ttt + 1
@@:
	endm

	READ_VAL_8
	FIN
endm

TEST_IMUL macro numOps1, numOps2
MY_START
	READ_VAL_32
	align 16
lab1:
	ttt = 0
	rept 32
	REP_IMUL numOps1
	add EBX, EBX
	jc @F
	REP_IMUL numOps2
	ttt = ttt + 1
@@:
	endm

	READ_VAL_32
	FIN
endm

TEST_ADD_ADD macro numOps1, numOps2
MY_START
	READ_VAL_32_2
	align 16
lab1:
	ttt = 0
	rept 32
	REP_ADD numOps1
	add EBX, EBX
	jc @F
	REP_ADD numOps2
	ttt = ttt + 1
@@:
	endm

	READ_VAL_32_2
	FIN
endm

TEST_ADD_TEST macro numOps1, numOps2
MY_START
	READ_VAL_32_2
	align 16
lab1:
	ttt = 0
	rept 32
	REP_ADD numOps1
	test EBX, 1 shl ttt
	jnz @F
	REP_ADD numOps2
	ttt = ttt + 1
@@:
	endm

	READ_VAL_32_2
	FIN
endm


BBB macro kkk
  align 16
  @CatStr(@PL_, %kkk, @8) PROC
endm

EEE macro kkk
  @CatStr(@PL_, %kkk, @8) ENDP
  kkk = kkk + 1
endm


MY_REPT macro kkk: req, num: req, my_func: req
  kkkStart = kkk
rept num
  BBB kkk
  my_func (kkk - kkkStart), (num - 1 - (kkk - kkkStart))
  EEE kkk
endm
endm


kkk = 0


  MY_REPT kkk, 13, TEST_ADD_TEST
  MY_REPT kkk, 15, TEST_ADD_ADD
  MY_REPT kkk,  6, TEST_MOVZX
  MY_REPT kkk, 11, TEST_MOVZX
  MY_REPT kkk,  5, TEST_IMUL


_TEXT$00	ENDS


end
