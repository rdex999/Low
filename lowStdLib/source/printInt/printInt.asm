bits 64

section .text
    global printInt

; prints an integer to stdout. (a 32 bit integer)
; argument 0: the integer to print (rdi)
; argument 1: (given by the compiler) the current location on the stack. (rsi)
printInt:
    add rsi, 10 ; alloc 10 bytes
    mov [rsi], edi ; push rdi
    shr edi, 31
    jz posInt   ; jump if rdi is positive

    dec rsi
    mov BYTE [rsi], '-'
    mov rax, 1
    mov rdi, 1
    mov rdx, 1
    syscall

    inc rsi
    mov eax, [rsi]
    mov edi, -1
    mul edi
    xor rcx, rcx
    jmp printNumLoop


posInt:
    mov eax, [rsi] ; pop to rax
    xor rcx, rcx

printNumLoop:
    xor rdx, rdx
    mov edi, 10
    div edi
    add dl, 48
    mov [rsi], dl
    dec rsi
    inc rcx
    cmp eax, 0
    jne printNumLoop

    mov rax, 1
    mov rdi, 1
    inc rsi
    mov rdx, rcx
    syscall
    ret