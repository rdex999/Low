section .text
    global printStr

; a function to print a string to the standart output stream.
; params: const char* -> a zero terminated string
printStr:
    push rbp
    mov rbp, rsp

    mov rsi, [rbp + 16]
    mov rdx, rsi

printStr_countLoop:
    inc rdx
    cmp BYTE [rdx], 0
    jne printStr_countLoop

    mov rax, 1
    mov rdi, 1
    sub rdx, rsi
    syscall 

    pop rbp 
    ret