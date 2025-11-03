; Copyright (C) 2025 Sector72

%include "stdlib.inc"
%include "filesys.inc"

section .data
filename db "test.txt",0
second_line db 10, "Second Line!", 10
second_line_size equ $ - second_line

section .bss
buffer resb 128

section .text
global _start

_start:
    writes2file filename, "I overwrite everything!!!"  ; Schreiben oder überschreiben
    appendd2file filename, second_line, second_line_size
    readfile filename, buffer, 128        ; 128 Bytes lesen und ausgeben (stdout)
    exit 0                                ; Exit with code 0
