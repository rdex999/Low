bits 64

section .data
    print_formatErrorMsg: db "print: Error, unknown format option.", 10
    print_tenF32: dd 10.0
    print_mOneF32: dd -1.0

section .text
    global print

; Formats and prints a string to stdout.
; Usage: print("val is: %i\n", val);
; Params:
; 1) {char ptr} zero terminated string
; 2...) args
print:
    push rbp
    mov rbp, rsp
    sub rsp, 40     ; [rbp - 8] -> text pointer | [rbp - 16] -> argument pointer

    lea r8, [rbp + 24]  ; address of second argument
    mov [rbp - 16], r8

    mov rbx, [rbp + 16] ; first argument (char*)
    mov [rbp - 8], rbx

    xor rcx, rcx
print_loop:
    cmp BYTE [rbx], 0
    je print_stringEnd
    cmp BYTE [rbx], '\'
    je print_backslash
    cmp BYTE [rbx], '%'
    je print_findType

    inc rbx
    inc rcx
    jmp print_loop

print_stringEnd:
    mov rax, 1
    mov rdi, 1
    mov rsi, [rbp - 8]
    mov rdx, rcx
    syscall

    mov rsp, rbp
    pop rbp
    ret

print_backslash:
    mov rax, 1
    mov rdi, 1
    mov rsi, [rbp - 8]
    mov rdx, rcx
    syscall

    add rbx, 2
    mov [rbp - 8], rbx
    xor rcx, rcx 
    jmp print_loop

print_findType:
    mov al, [rbx + 1]
    cmp al, 'i'
    je print_int32
    cmp al, 'c'
    je print_char
    cmp al, 's'
    je print_string
    cmp al, 'f'
    je print_float32

    ; if not found, then error
    mov rax, 1
    mov rdi, 2
    mov rsi, print_formatErrorMsg
    mov rdx, 37
    syscall

    mov rsp, rbp
    pop rbp
    mov rax, 60
    mov rdi, 1
    syscall

print_int32:
    mov rax, 1
    mov rdi, 1
    mov rsi, [rbp - 8]
    mov rdx, rcx
    syscall

    add rbx, 2
    mov [rbp - 8], rbx

    mov rax, [r8]
    add r8, 8
    mov [rbp - 16], r8

    mov ecx, eax
    shr ecx, 31
    mov ebx, eax 
    jz print_int32posInit
    lea rsi, [rbp - 17]
    mov rax, 1
    mov rdi, 1
    mov BYTE [rsi], '-'
    mov rdx, 1
    syscall

    mov eax, ebx
    mov ebx, -1
    mul ebx
    xor rcx, rcx
    lea rsi, [rbp - 17]
    jmp print_int32loop


print_int32posInit:
    xor rcx, rcx
    lea rsi, [rbp - 17]
print_int32loop:
    mov rbx, 10
    xor rdx, rdx
    div ebx
    add dl, 48
    mov [rsi], dl
    inc rcx
    dec rsi
    test eax, eax
    jnz print_int32loop

    mov rax, 1
    mov rdi, 1
    inc rsi
    mov rdx, rcx
    syscall

    mov rbx, [rbp - 8] 
    xor rcx, rcx
    jmp print_loop

print_char:
    mov rax, 1
    mov rdi, 1
    mov rsi, [rbp - 8]
    mov rdx, rcx
    syscall

    mov al, [r8]
    add r8, 8
    mov [rbp - 16], r8

    lea rsi, [rbp - 17]
    mov [rsi], al
    mov rax, 1
    mov rdi, 1
    mov rdx, 1
    syscall

    add rbx, 2
    xor rcx, rcx
    mov [rbp - 8], rbx
    jmp print_loop

print_string:
    mov rax, 1
    mov rdi, 1
    mov rsi, [rbp - 8]
    mov rdx, rcx
    syscall

    mov rax, [r8]
    add r8, 8
    mov [rbp - 16], r8

    mov rsi, rax
    xor rdx, rdx
print_stringLoop:
    inc rdx
    inc rax
    cmp BYTE [rax], 0
    jne print_stringLoop

    mov rax, 1
    mov rdi, 1
    syscall

    add rbx, 2
    xor rcx, rcx
    mov [rbp - 8], rbx
    jmp print_loop

print_float32:
    mov rax, 1
    mov rdi, 1
    mov rsi, [rbp - 8]
    mov rdx, rcx
    syscall

    add rbx, 2
    mov [rbp - 8], rbx

    movss xmm0, [r8]
    add r8, 8
    mov [rbp - 16], r8
    mov ecx, 8

    lea rsi, [rbp - 17]
    cvtss2si eax, xmm0 ; convert xmm0 to int in eax
    shr eax, 31
    jz print_float32posInit ; jump if eax is positivea

    mov BYTE [rsi], '-'
    mov rax, 1
    mov rdi, 1
    mov rdx, 1
    syscall

    movss xmm1, DWORD [print_mOneF32] ; multiply by -1
    mulss xmm0, xmm1
    movss xmm2, xmm0

    cvtss2si eax, xmm2  ; floor xmm2
    cvtsi2ss xmm2, eax  ;

    movss xmm1, xmm0
    subss xmm1, xmm2    ; get the fractional part in xmm1 (13.123 -> 0.123)

    movss xmm2, DWORD [print_tenF32]
    jmp print_float32fracLoop

print_float32posInit:
    movss xmm2, xmm0
    cvtss2si eax, xmm2  ; floor xmm2
    cvtsi2ss xmm2, eax  ; 
    movss xmm1, xmm0
    subss xmm1, xmm2
    movss xmm2, DWORD [print_tenF32]

print_float32fracLoop:
    mulss xmm1, xmm2
    dec ecx
    jnz print_float32fracLoop

    cvtss2si eax, xmm1
    xor rcx, rcx
print_float32fracPrintLoop:
    xor rdx, rdx
    mov edi, 10
    div edi
    add dl, 48
    mov [rsi], dl
    inc rcx
    dec rsi
    test eax, eax
    jnz print_float32fracPrintLoop

    mov BYTE [rsi], '.'
    dec rsi
    cvtss2si eax, xmm0
print_float32intLoop:
    xor rdx, rdx
    mov edi, 10
    div edi
    add dl, 48
    mov [rsi], dl
    dec rsi
    inc rcx
    test eax, eax
    jnz print_float32intLoop

    mov rax, 1
    mov rdi, 1
    inc rsi
    mov rdx, rcx
    inc rdx
    syscall

    mov rbx, [rbp - 8]
    xor rcx, rcx
    jmp print_loop