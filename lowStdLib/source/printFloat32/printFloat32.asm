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
; 3) {byte*} the location on the stack (given by the compiler) ( rsi )
printFloat32:
    cvtss2si eax, xmm0 ; convert to int 13.1234 -> 13
    mov rbx, rdi

    mov edi, eax 
    shr edi, 31
    jz printFloat32Init

    mov BYTE [rsi], '-'
    mov rax, 1
    mov rdi, 1
    mov rdx, 1
    syscall

    movss xmm1, DWORD [mOneF32]
    mulss xmm0, xmm1
    cvtss2si eax, xmm0 ; convert to int 13.1234 -> 13

printFloat32Init: 
    add rsi, 10 ; alloc 10 bytes 
    mov rcx, 1
    mov BYTE [rsi+1], '.'

.printIntLoop:
    xor rdx, rdx
    mov edi, 10
    div edi
    add dl, 48
    mov [rsi], dl
    dec rsi
    inc rcx
    cmp eax, 0
    jne .printIntLoop

    mov rax, 1
    mov rdi, 1
    inc rsi
    mov rdx, rcx
    syscall

    cvtss2si edi, xmm0 ; convert to int 13.1234 -> 13
    cvtsi2ss xmm1, edi ; convert to float32 13 -> 13.0
    subss xmm0, xmm1   ; gen the fractional part 13.1234 - 13 = 0.1234

    mov rcx, 0
    movss xmm1, DWORD [tenF32]
.mulTenLoop: ;          multiply by 10.0 to shift the fractional part 0.1234 -> 1.234 -> 12.34 -> .....
    mulss xmm0, xmm1
    inc rcx
    cmp rcx, rbx
    jne .mulTenLoop

    cvtss2si eax, xmm0 ; convert to int 123.4 -> 123

    add rsi, 10 ; alloc 10 bytes 
    xor rcx, rcx

.printIntLoop2:
    xor rdx, rdx
    mov edi, 10
    div edi
    add dl, 48
    mov [rsi], dl
    dec rsi
    inc rcx
    cmp rax, 0
    jne .printIntLoop2

    mov rax, 1
    mov rdi, 1
    inc rsi
    mov rdx, rcx
    syscall
    ret