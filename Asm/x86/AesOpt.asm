; AesOpt.asm -- Intel's AES.
; 2009-12-12 : Igor Pavlov : Public domain

%include "7zAsm.asm"

MY_ASM_START

%ifndef x64
; FIXME    .xmm
%endif

%define rD  r2
%define rN  r0

%macro MY_PROLOG  1  ; MY_PROLOG macro reg:req
    %ifdef x64
	  %ifdef CYGWIN64
		; ivAes : %rcx
        ; data : %rdx
        ; numBlocks : %r8
	  %else
        mov     RCX,RDI
        mov     R8 ,RDX
        mov     RDX,RSI
      %endif
    ; movdqa  [r4 + 8], xmm6
    ; movdqa  [r4 + 8 + 16], xmm7
    %endif

    push    r3
    push    r5

%ifdef x64
    %ifdef CYGWIN64
	  push    r6
    %endif
    mov     rN, r8
%else
    push    r6
    mov     ecx,   [r4 + REG_SIZE * 4]
    mov     edx,   [r4 + REG_SIZE * 5]
    mov     rN,   [r4 + REG_SIZE * 6]
%endif

    mov     x6, [r1 + 16]
    shl     x6, 5

    movdqa  %1, [r1] ; reg
    add     r1, 32
%endmacro

%macro MY_EPILOG 0
%ifdef x64
    %ifdef CYGWIN64
		pop     r6
    %endif
%else
    pop     r6
%endif
    pop     r5
    pop     r3

    %ifdef x64
    ; movdqa  xmm6, [r4 + 8]
    ; movdqa  xmm7, [r4 + 8 + 16]
    %endif

    MY_ENDP
%endmacro

ways equ 4
ways16 equ (ways * 16)

%macro OP_W 2 ;  op, op2

%define i 0
  %1 xmm0,%2
%define i 1
  %1 xmm1,%2
%define i 2
  %1 xmm2,%2
%define i 3
  %1 xmm3,%2

%endmacro

%macro LOAD_OP 2  ; LOAD_OP macro op:req, offs:req
    %1      xmm0, [r1 + r3 %2]
%endmacro

%macro LOAD_OP_W 2 ; LOAD_OP_W macro op:req, offs:req
    movdqa  xmm7, [r1 + r3 %2]
    ; OP_W    %1, xmm7
  %1 xmm0,xmm7
  %1 xmm1,xmm7
  %1 xmm2,xmm7
  %1 xmm3,xmm7
%endmacro


; ---------- AES-CBC Decode ----------

%macro CBC_DEC_UPDATE 2  ; CBC_DEC_UPDATE macro reg, offs
    pxor    %1, xmm6
    movdqa  xmm6, [rD + %2]
    movdqa  [rD + %2], %1
%endmacro

%macro DECODE 1 ; macro op:req
    %1      aesdec, +16
  %%B:
    %1      aesdec, +0
    %1      aesdec, -16
    sub     x3, 32
    jnz     %%B
    %1      aesdeclast, +0
%endmacro

; void AesCbc_Decode_Intel(UInt32 *ivAes, Byte *data, size_t numBlocks)
MY_PROC AesCbc_Decode_Intel, 3
    MY_PROLOG xmm6

    sub     x6, 32

    jmp     check2

  align 16
  nextBlocks2:
    mov     x3, x6
    OP_W    movdqa, [rD + i * 16]

    LOAD_OP_W  pxor, +32

    DECODE  LOAD_OP_W

    ;OP_W    CBC_DEC_UPDATE, i * 16
    CBC_DEC_UPDATE xmm0, 0 * 16
    CBC_DEC_UPDATE xmm1, 1 * 16
    CBC_DEC_UPDATE xmm2, 2 * 16
    CBC_DEC_UPDATE xmm3, 3 * 16


    add     rD, ways16
  check2:
    sub     rN, ways
    jnc     nextBlocks2

    add     rN, ways
    jmp     check

  nextBlock:
    mov     x3, x6
    movdqa  xmm1, [rD]
    LOAD_OP movdqa, +32
    pxor    xmm0, xmm1

    DECODE  LOAD_OP

    pxor    xmm0, xmm6
    movdqa  [rD], xmm0
    movdqa  xmm6, xmm1
    add     rD, 16
  check:
    sub     rN, 1
    jnc     nextBlock

    movdqa  [r1 - 32], xmm6

    MY_EPILOG


; ---------- AES-CBC Encode ----------

%macro ENCODE 1 ; macro op:req
    %1      aesenc, -16
  %%B:
    %1      aesenc, +0
    %1      aesenc, +16
    add     r3, 32
    jnz     %%B
    %1      aesenclast, +0
%endmacro

MY_PROC AesCbc_Encode_Intel, 3
    MY_PROLOG xmm0

    add     r1, r6
    neg     r6
    add     r6, 32

    jmp     check_e

  align 16
  nextBlock_e:
    mov     r3, r6
    pxor    xmm0, [rD]
    pxor    xmm0, [r1 + r3 - 32]

    ENCODE  LOAD_OP

    movdqa  [rD], xmm0
    add     rD, 16
  check_e:
    sub     rN, 1
    jnc     nextBlock_e

    movdqa  [r1 + r6 - 64], xmm0
    MY_EPILOG


; ---------- AES-CTR ----------

%macro XOR_UPD_1 2 ; reg, offs
    pxor    %1, [rD + %2]
%endmacro

%macro XOR_UPD_2 2 ; reg, offs
    movdqa  [rD + %2], %1
%endmacro

MY_PROC AesCtr_Code_Intel, 3
    MY_PROLOG xmm6

    mov     r5, r4
    shr     r5, 4
    dec     r5
    shl     r5, 4

    mov     DWORD [r5], 1
    mov     DWORD [r5 + 4], 0
    mov     DWORD [r5 + 8], 0
    mov     DWORD [r5 + 12], 0

    add     r1, r6
    neg     r6
    add     r6, 32

    jmp     check2_c

  align 16
  nextBlocks2_c:
    movdqa  xmm7, [r5]

;    i = 0
;    rept ways
;    paddq   xmm6, xmm7
;    movdqa  @CatStr(xmm,%i), xmm6
;    i = i + 1
;    endm
    paddq   xmm6, xmm7
    movdqa  xmm0, xmm6

    paddq   xmm6, xmm7
    movdqa  xmm1, xmm6

    paddq   xmm6, xmm7
    movdqa  xmm2, xmm6

    paddq   xmm6, xmm7
    movdqa  xmm3, xmm6



    mov     r3, r6
    LOAD_OP_W  pxor, -32

    ENCODE  LOAD_OP_W

    ;OP_W    XOR_UPD_1, i * 16
    XOR_UPD_1 xmm0, 0 * 16
    XOR_UPD_1 xmm1, 1 * 16
    XOR_UPD_1 xmm2, 2 * 16
    XOR_UPD_1 xmm3, 3 * 16

    ;OP_W    XOR_UPD_2, i * 16
    XOR_UPD_2 xmm0, 0 * 16
    XOR_UPD_2 xmm1, 1 * 16
    XOR_UPD_2 xmm2, 2 * 16
    XOR_UPD_2 xmm3, 3 * 16

    add     rD, ways16
  check2_c:
    sub     rN, ways
    jnc     nextBlocks2_c

    add     rN, ways
    jmp     check_c

  nextBlock_c:
    paddq   xmm6, [r5]
    mov     r3, r6
    movdqa  xmm0, [r1 + r3 - 32]
    pxor    xmm0, xmm6
    ENCODE  LOAD_OP
    XOR_UPD_1 xmm0, 0
    XOR_UPD_2 xmm0, 0
    add     rD, 16
  check_c:
    sub     rN, 1
    jnc     nextBlock_c

    movdqa  [r1 + r6 - 64], xmm6
    MY_EPILOG

; end

%ifidn __OUTPUT_FORMAT__,elf
section .note.GNU-stack noalloc noexec nowrite progbits
%endif

