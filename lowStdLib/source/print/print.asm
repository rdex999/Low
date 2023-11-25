bits 64

section .text
    global print

; Formats and prints a string to stdout.
; Usage: print("val is: %i\n", val);
; Params:
; 1) {char ptr} zero terminated string
; 2...) args
print:
    ret