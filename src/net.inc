; Copyright (C) 2025 Sector72

; ============================================================
; net.inc – Syscall-based Socket-Makros for NASM
; Plattform: Linux x86_64
; Direct Syscall-Access without libc
; 
; Makros:
;   socket(domain, type, protocol)        ; return: rax = socket fd
;   bind(fd, sockaddr_ptr, socklen)       ; bind socket
;   listen(fd, backlog)                   ; listen auf socket
;   accept(fd, sockaddr_ptr, socklen_ptr) ; accept connection, return: rax = client fd
;   connect(fd, sockaddr_ptr, socklen)    ; connect to server
;   send(fd, buf, len, flags)             ; send data
;   recv(fd, buf, len, flags)             ; receive data
;   close(fd)                             ; close socket
;   server                                ; socket, bind and listen, all in one
;   accept_loop                           ; accept macro (points to client-handling routine)
; ============================================================


; =================================================
; Socket-Domains
; =================================================
%define AF_UNIX      1
%define AF_INET      2
%define AF_INET6     10

; =================================================
; Socket-Typen
; =================================================
%define SOCK_STREAM  1
%define SOCK_DGRAM   2
%define SOCK_RAW     3

; =================================================
; Flags für send / recv
; =================================================
%define MSG_OOB     0x1
%define MSG_PEEK    0x2
%define MSG_DONTWAIT 0x40

; =================================================
; Standard Ports / INADDR
; =================================================
%define INADDR_ANY   0x00000000
%define INADDR_LOOPBACK 0x7f000001


; =================================================
; socket(domain, type, protocol)
; Syscall #41
; rdi = domain (z.B. AF_INET=2)
; rsi = type (z.B. SOCK_STREAM=1)
; rdx = protocol (0)
; Return: rax = fd, <0 = errno
; =================================================
%macro socket 3
    mov     rax, 41
    mov     rdi, %1
    mov     rsi, %2
    mov     rdx, %3
    syscall
%endmacro

; =================================================
; bind(fd, sockaddr_ptr, socklen)
; Syscall #49
; rdi = fd
; rsi = sockaddr pointer
; rdx = socklen
; Return: rax = 0 success, <0 = errno
; =================================================
%macro bind 3
    mov     rax, 49
    mov     rdi, %1
    mov     rsi, %2
    mov     rdx, %3
    syscall
%endmacro

; =================================================
; listen(fd, backlog)
; Syscall #50
; rdi = fd
; rsi = backlog
; Return: rax = 0 success, <0 = errno
; =================================================
%macro listen 2
    mov     rax, 50
    mov     rdi, %1
    mov     rsi, %2
    syscall
%endmacro

; =================================================
; accept(fd, sockaddr_ptr, socklen_ptr)
; Syscall #43
; rdi = fd
; rsi = sockaddr pointer
; rdx = socklen pointer
; Return: rax = client fd, <0 = errno
; =================================================
%macro accept 3
    mov     rax, 43
    mov     rdi, %1
    mov     rsi, %2
    mov     rdx, %3
    syscall
%endmacro

; =================================================
; connect(fd, sockaddr_ptr, socklen)
; Syscall #42
; rdi = fd
; rsi = sockaddr pointer
; rdx = socklen
; Return: rax = 0 success, <0 = errno
; =================================================
%macro connect 3
    mov     rax, 42
    mov     rdi, %1
    mov     rsi, %2
    mov     rdx, %3
    syscall
%endmacro

; =================================================
; send(fd, buf, len, flags)
; Syscall #44
; rdi = fd
; rsi = buffer pointer
; rdx = length
; r10 = flags
; Return: rax = bytes sent, <0 = errno
; =================================================
%macro send 4
    mov     rax, 44
    mov     rdi, %1
    mov     rsi, %2
    mov     rdx, %3
    mov     r10, %4
    syscall
%endmacro

; =================================================
; recv(fd, buf, len, flags)
; Syscall #45
; rdi = fd
; rsi = buffer pointer
; rdx = length
; r10 = flags
; Return: rax = bytes received, <0 = errno
; =================================================
%macro recv 4
    mov     rax, 45
    mov     rdi, %1
    mov     rsi, %2
    mov     rdx, %3
    mov     r10, %4
    syscall
%endmacro

; =================================================
; close(fd)
; Syscall #3
; rdi = fd
; Return: rax = 0 success, <0 = errno
; =================================================
%macro close 1
    mov     rax, 3
    mov     rdi, %1
    syscall
%endmacro


; ========================================================================
; server(domain, type, addr_ptr, backlog)
; Creates a socket, binds it, and starts listening.
; r12 = server FD
; ========================================================================
%macro server 4
    socket %1, %2, 0          ; socket(domain, type, protocol)
    mov r12, rax              ; save server FD
    bind r12, %3, 16          ; bind(server_fd, addr_ptr, 16)
    listen r12, %4            ; listen(server_fd, backlog)
%endmacro


; =================================================
; accept_loop(server_fd, code_block)
; Accept connections in a loop and execute a code block for each client.
; %1 = server FD (r12, or beliebig)
; %2 = code block (z.B. send/prints)
; rbx = client FD inside the block
; =================================================
%macro accept_loop 2
accept_loop_start:
    accept %1, client_addr, client_len
    mov rbx, rax
    call %2
    close rbx
    jmp accept_loop_start
%endmacro





