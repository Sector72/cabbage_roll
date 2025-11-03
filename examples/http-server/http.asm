; Copyright (C) 2025 Sector72

; =================================================
; Simple and minimalistic HTTP-Server (1.0)
; Written using self made NASM-Macros
; Still not secure (Buffer/Stack-Overload)
; Updates/Patches coming soon ...
; =================================================

%include "net.inc"                                 ; enthält server, accept_loop, prints, etc.
%include "filesys.inc"                             ; enthält readfile, writed2file, writes2file etc.

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
    response: resb 512                             ; Response buffer (for bytes read from file)

section .text                                      ; Section .text, here begins the code
global _start                                      ; for the linker, entry is at _start

_start:                                            ; Here starts the entry
    readfile filename, response, 512               ; Read 512 Bytes from"site.dat"
    server AF_INET, SOCK_STREAM, server_addr, 5    ; Create Socket und Bind. 5 for Backlog
    accept_loop r12, handle_client                 ; r12 = Socket-FD and handle_client for send/recv routine

    handle_client:                                 ; Send/Recv Routine
        send rbx, response, 512, 0                 ; Send response, 512 Bytes of length. rbx = Client-FD

        ret                                        ; Return



