; Copyright (C) 2025 Sector72

; =================================================
; Simple and minimalistic HTTP-Server (1.0)
; Written using self made NASM-Macros
; Still not secure (Buffer/Stack-Overload)
; Updates/Patches coming soon ...
; =================================================

%include "net.inc"                                 ; contains server, accept_loop, prints, etc.
%include "filesys.inc"                             ; contains readfile, writed2file, writes2file etc.

section .data                                      ; Section .data, for initialized data
    filename db "site.dat",0                       ; Filename of the HTTP/HTML file
    server_addr:                                   ; Server_Address_Structure
        dw AF_INET                                 ; IPv4
        dw 0x901F                                  ; Port 8080 Little-Endian
        dd INADDR_ANY                              ; 0.0.0.0 Every Interface
        dq 0                                       ; 8 Byte Padding (2+2+4+8 = 16)

section .bss                                       ; Section .bss, for uninitialized data
    client_addr: resb 16                           ; Client_Address_Structure
    client_len:  resq 1                            ; Client_Length
    response: resb 65536                           ; Response buffer (for bytes read from file) = 64KB
    bytes_read: resq 1                             ; buffer for amount of bytes read from site.dat

section .text                                      ; Section .text, here begins the code
global _start                                      ; for the linker, entry is at _start

_start:                                            ; Here starts the entry
	filesize filename, r13                         ; Get filesize of site.dat
	mov [bytes_read], r13                          ; Secure the filesize (in bytes) in bytes_read
    readfile filename, response, bytes_read        ; Read [bytes_read] Bytes from "site.dat"
    server AF_INET, SOCK_STREAM, server_addr, 5    ; Create Socket und Bind. 5 for Backlog
    accept_loop r12, handle_client                 ; handle_client for send/recv routine
    
    handle_client:                                 ; Send/Recv Routine
        send rbx, response, [bytes_read], 0        ; Send response [bytes_read] Bytes of length. rbx = Client-FD
        ret                                        ; Return
