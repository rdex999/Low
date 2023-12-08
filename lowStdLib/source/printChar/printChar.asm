section .text
    global printChar

; prints a single character to the screen
; argument 1: the character
; argument 2: passed by the low compiler, the current location on the stack
printChar:
    push rbp
    mov rbp, rsp

    dec rsp
    mov [rbp - 1], dil
    mov rsi, rsp
    mov rax, 1
    mov rdi, 1
    mov rdx, 1
    syscall

    mov rsp, rbp
    pop rbp 
    ret