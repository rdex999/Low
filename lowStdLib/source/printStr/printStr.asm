section .text
    global printStr

; a function to print a string to the standart output stream.
; params: const char* -> a zero terminated string
printStr:
    ;push rbp
    ;mov rbp, rsp

    xor rdx, rdx
    mov rdx, rdi

printStrCountLoop:
    mov al, [rdx]
    inc rdx
    test al, al
    jnz printStrCountLoop

    mov rax, 1
    sub rdx, rdi
    dec rdx 
    mov rsi, rdi 
    mov rdi, 1
    syscall

    ;pop rbp
    ret