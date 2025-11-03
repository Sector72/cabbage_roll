; Copyright (C) 2025 Sector72

; ============================================================
; stdlib.inc – Mini Macro header for Linux x86_64 NASM
; Contains:
;   printd – (print-data) writes String to stdout
;   prints - (print-string) prints String
;   println - (print-line) prints but with \n ...
;   readd - (read-data) reads data and saves it to <prt> and <len>
;   read - input handling (stdin) with labels
;   reg64var <label> <reg64> - saves <reg64> content to buffer/label <label>
;   exit – exit program with code --> exit 0
; ============================================================

; =================================================
; printd <ptr>, <len>
; writes len Bytes from ptr to stdout
; =================================================
%macro printd 2
    mov     rax, 1          ; syscall: write
    mov     rdi, 1          ; fd = stdout
    mov     rsi, %1         ; pointer at string
    mov     rdx, %2         ; length of string
    syscall
%endmacro

; =================================================
; prints "Text"
; prints text
; usage: prints "Hello, World!"
; =================================================
%macro prints 1
    section .rodata
    %%str db %1
    %%strlen equ $ - %%str
    section .text
    mov     rax, 1
    mov     rdi, 1
    mov     rsi, %%str
    mov     rdx, %%strlen
    syscall
%endmacro


; =================================================
; prints - but with next a next line ;)
; =================================================
%macro println 1
    prints %1
    mov rax, 1
    mov rdi, 1
    mov rsi, 10
    mov rdx, 1
    syscall
%endmacro


; =================================================
; input handling (stdin) with labels 
; =================================================
; readd <ptr>, <len>
; reads len Bytes from stdin (fd=0) and saves these in ptr
; Ret-Value in r12
; =================================================
%macro readd 2
    mov     rax, 0          ; syscall: read
    mov     rdi, 0          ; fd = stdin
    mov     rsi, %1         ; Pointer at Buffer
    mov     rdx, %2         ; max. Bytes
    syscall
    mov r12, rax
    
%endmacro


; =================================================
; input handling (stdin) with labels 
; =================================================
; example: read label, buff_len
; example: read user_input 128
; Arguments:
;            %1 = label for buffer
;            %2 = buffer length
; Ret-Value in r12
; =================================================
%macro read 2
    section .bss
    %1: resb %2
    section .text
    readd %1, %2
%endmacro


; ==================================================
;              reg64var <label> <reg64>
; reg64var creates a label/buffer <label> and saves
;            the content of <reg64> to it.
; ==================================================
%macro reg64var 2
    section .bss
    %1: resq 1
    section .text
    mov qword [%1], %2
%endmacro



; =================================================
; exit <code>
; exits program with code <code>
; =================================================
%macro exit 1
    mov     rax, 60         ; syscall: exit
    mov     rdi, %1         ; Exit-Code
    syscall
%endmacro


