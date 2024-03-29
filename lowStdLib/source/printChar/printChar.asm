section .text
    global printChar

; prints a single character to the screen
; argument 1: the character
; argument 2: passed by the low compiler, the current location on the stack
printChar:
    push rbp
    mov rbp, rsp

    lea rsi, [rbp + 16]
    mov rax, 1
    mov rdi, 1
    mov rdx, 1
    syscall

    mov rsp, rbp
    pop rbp 
    ret