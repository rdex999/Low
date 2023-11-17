bits 64

section .text
    global printNum

; prints a number to stdout.
; argument 0: the number to print (rdi)
; argument 1: (given by the compiler) the current location on the stack. (rsi)
printNum:
    add rsi, 10 ; alloc 10 bytes 
    mov rax, rdi
    xor rcx, rcx

.printNumLoop:
    xor rdx, rdx
    mov rdi, 10
    div rdi
    add dl, 48
    mov [rsi], dl
    dec rsi
    inc rcx
    cmp rax, 0
    jne .printNumLoop

    mov rax, 1
    mov rdi, 1
    inc rsi
    mov rdx, rcx
    syscall
    ret
