section .text
    global printStr

; a function to print a string to the standart output stream.
; params: const char* -> a zero terminated string
printStr:
    xor rdx, rdx
    mov rsi, rdi

printStrCountLoop:
    mov al, [rdi]
    inc rdi
    inc rdx
    cmp al, 0
    jne printStrCountLoop

    mov rax, 1
    mov rdi, 1
    syscall
    ret