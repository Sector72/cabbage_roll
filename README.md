# Simple Macro Headers

A collection of simple, amateur macro headers.  
The goal is to be more productive with less code and greater speed.  
That's it. Everything is open — do whatever you want if something catches your eye.  

> **Note:** These sources and macros only work on **Linux (64-bit)**.  
> You need NASM (Netwide Assembler) and binutils (ld) installed on your system.

---

## Requirements

- NASM (Netwide Assembler)  
- binutils (ld)  

---

## Installation

Open a terminal and run the following commands:

```bash
sudo apt update                 # Update package lists
sudo apt install binutils       # Install ld if not installed
sudo apt install nasm           # Install NASM assembler
```

---

## Build Instructions

1. Ensure that the required `.inc` headers are in the same directory as the source file.  

2. Assemble the source file:

```bash
nasm -f elf64 sourcefile.asm -o objectfile.o
```

3. Link the object file to create the executable:

```bash
ld objectfile.o -o program
```

---

## About the Examples

I have programmed some examples to demonstrate the use of the macros.  
Examples include:

- A simple HTTP server (very primitive — serves an HTTP header together with HTML code in one file)  
- Input/output examples  
- File access examples  

Everything is still a work in progress.

---

## Example: HTTP Server (`http.asm`)

```asm
; Copyright (C) 2025 Sector72
; =================================================
; Simple and minimalistic HTTP-Server (1.0)
; Written using self-made NASM macros
; Still not secure (Buffer/Stack-Overload)
; Updates/Patches coming soon ...
; =================================================

%include "net.inc"         ; contains server, accept_loop, prints, etc.
%include "filesys.inc"     ; contains readfile, writed2file, writes2file, etc.

section .data              ; Section for initialized data
    filename db "site.dat",0
    server_addr:           
        dw AF_INET         ; IPv4
        dw 0x901F          ; Port 8080 Little-Endian
        dd INADDR_ANY      ; 0.0.0.0 (every interface)
        dq 0               ; 8-byte padding

section .bss               ; Section for uninitialized data
    client_addr: resb 16   ; Client address structure
    client_len:  resq 1    ; Client length
    response: resb 512     ; Response buffer (bytes read from file)

section .text
global _start

_start:
    readfile filename, response, 512              ; Read 512 bytes from "site.dat"
    server AF_INET, SOCK_STREAM, server_addr, 5    ; Create socket and bind. 5 = backlog
    accept_loop r12, handle_client                 ; r12 = socket FD, handle_client = send/recv routine

handle_client:                                    ; Send/recv routine
    send rbx, response, 512, 0                    ; Send 512 bytes. rbx = client FD
    ret
```

---

## Notes

- This code is **still experimental**.  
- The HTTP server is **very basic and not secure**.  
- Use it only for learning or testing purposes.  
- All macros are open — feel free to modify or extend them.  

---

## Note from the Author

I hope you find something useful in my repo.  
And in advance: "I am terribly sorry for the poor code quality — I am still learning ..."

Greetings from Austria,  
**Sector72**

© 2025 Sector72
