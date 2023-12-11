bits 64

section .text
    global printInt

; prints an integer to stdout. (a 32 bit integer)
; argument 0: the integer to print (rdi)
; argument 1: (given by the compiler) the current location on the stack. (rsi)
printInt:
    push rbp
    mov rbp, rsp
    sub rsp, 14     ; alloc 14 bytes

    lea rsi, [rbp - 1]
    mov eax, [rbp + 16]     ; first argument
    mov ebx, eax
    xor rcx, rcx
    shr ebx, 31
    jz printInt_loop

    mov ebx, eax
    mov rax, 1
    mov rdi, 1
    mov BYTE [rsi], '-'
    mov rdx, 1
    syscall

    mov eax, ebx
    mov ebx, -1
    mul ebx
    xor rcx, rcx

printInt_loop:
    mov ebx, 10
    xor rdx, rdx
    div ebx
    add dl, 48
    mov [rsi], dl
    dec rsi
    inc rcx
    test eax, eax
    jnz printInt_loop

    mov rax, 1
    mov rdi, 1
    inc rsi
    mov rdx, rcx
    syscall

    mov rsp, rbp
    pop rbp
    ret