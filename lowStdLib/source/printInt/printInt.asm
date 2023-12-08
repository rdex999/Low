bits 64

section .text
    global printInt

; prints an integer to stdout. (a 32 bit integer)
; argument 0: the integer to print (rdi)
; argument 1: (given by the compiler) the current location on the stack. (rsi)
printInt:
    push rbp
    mov rbp, rsp
    sub rsp, 14 ; alloc 14 bytes

    mov [rbp - 4], edi
    shr edi, 31
    jz printIntPosInit ; jump if edi is positive

    lea rsi, [rbp - 5]
    mov BYTE [rsi], '-'
    mov rax, 1
    mov rdi, 1
    mov rdx, 1
    syscall

    mov eax, [rbp - 4]
    mov ebx, -1
    mul ebx
    xor rcx, rcx 
    lea rsi, [rbp - 5]
    jmp printIntLoop


printIntPosInit:
    xor rcx, rcx
    lea rsi, [rbp - 5]
    mov eax, [rbp - 4]

printIntLoop:
    mov ebx, 10
    xor rdx, rdx
    div ebx
    add dl, 48
    mov [rsi], dl
    inc rcx
    dec rsi
    test eax, eax
    jnz printIntLoop

    mov rax, 1
    mov rdi, 1
    mov rdx, rcx
    inc rsi
    syscall

    ;mov rsp, rbp
    ;pop rbp
    leave 
    ret