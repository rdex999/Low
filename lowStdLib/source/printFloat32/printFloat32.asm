bits 64

section .data
    tenF32: dd 10.0
    mOneF32: dd -1.0

section .text
    global printFloat32

; Prints a 32 bit floating point
; params:
; 1) {float} the float to print ( xmm0 )
; 2) {int} the amount of digits to print after the point ( rdi )
printFloat32:
    push rbp
    mov rbp, rsp
    sub rsp, 20 

    lea rsi, [rbp - 1]
    mov ecx, edi ; store second argument in ecx
    cvtss2si eax, xmm0 ; convert xmm0 to int in eax
    shr eax, 31
    jz printFloat32_posInit ; jump if eax is positivea

    mov BYTE [rsi], '-'
    mov rax, 1
    mov rdi, 1
    mov rdx, 1
    syscall

    movss xmm1, DWORD [mOneF32] ; multiply by -1
    mulss xmm0, xmm1
    movss xmm2, xmm0

    cvtss2si eax, xmm2  ; floor xmm2
    cvtsi2ss xmm2, eax  ;

    movss xmm1, xmm0
    subss xmm1, xmm2    ; get the fractional part in xmm1 (13.123 -> 0.123)

    movss xmm2, DWORD [tenF32]
    jmp printFloat32_fracLoop

printFloat32_posInit:
    movss xmm2, xmm0
    cvtss2si eax, xmm2  ; floor xmm2
    cvtsi2ss xmm2, eax  ; 
    movss xmm1, xmm0
    subss xmm1, xmm2
    movss xmm2, DWORD [tenF32]

printFloat32_fracLoop:
    mulss xmm1, xmm2
    dec ecx
    jnz printFloat32_fracLoop

    cvtss2si eax, xmm1
    xor rcx, rcx
printFloat32_fracPrintLoop:
    xor rdx, rdx
    mov edi, 10
    div edi
    add dl, 48
    mov [rsi], dl
    inc rcx
    dec rsi
    test eax, eax
    jnz printFloat32_fracPrintLoop

    mov BYTE [rsi], '.'
    dec rsi
    cvtss2si eax, xmm0
printFloat32_intLoop:
    xor rdx, rdx
    mov edi, 10
    div edi
    add dl, 48
    mov [rsi], dl
    dec rsi
    inc rcx
    test eax, eax
    jnz printFloat32_intLoop

    mov rax, 1
    mov rdi, 1
    inc rsi
    mov rdx, rcx
    inc rdx
    syscall

    mov rsp, rbp
    pop rbp
    ret