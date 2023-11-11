section .text
    global printChar

; prints a single character to the screen
; argument 1: the character
; argument 2: passed by the low compiler, the current location on the stack
printChar:
    mov [rsi], rdi
    mov rax, 1
    mov rdi, 1
    mov rdx, 1
    syscall
    ret