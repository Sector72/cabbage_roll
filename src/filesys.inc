; Copyright (C) 2025 Sector72

; ========================================================================
; filesys.inc  —  NASM macros for filesystem access
; ========================================================================

; ---- Syscall numbers (x86_64 Linux) -----------------------------------
%define SYS_openat   257
%define SYS_read     0
%define SYS_write    1
%define SYS_close    3
%define SYS_mkdir    83
%define SYS_unlink   87
%define SYS_rmdir    84
%define AT_FDCWD     -100

; ---- Flags / Modes -----------------------------------------------------
%define O_RDONLY     0
%define O_WRONLY     1
%define O_RDWR       2
%define O_CREAT      64
%define O_TRUNC      512
%define O_APPEND     1024

; ========================================================================
; Low-level primitives
; ========================================================================


; ======================================================
; ---------------------- fopen -------------------------
; fopen filename, flags, mode
; ------------------------------------------------------
; Arguments:
;            %1 = pointer at zero-terminated filename
;            %2 = flags (O_RDONLY, O_WRONLY; ...)
;            %3 = mode (Permissions, only relev. O_CREAT
; Return Value:
;            rax = Filedescriptor | >=0 --> then error 
; ======================================================
%macro fopen 3
    mov rax, SYS_openat
    mov rdi, AT_FDCWD
    mov rsi, %1
    mov rdx, %2
    mov r10, %3
    syscall
%endmacro


; ======================================================
; --------------------- fwrite -------------------------
; fwrite fd, buffer, len
; ------------------------------------------------------
; Arguments:
;            %1 = File-Descriptor (fd)
;            %2 = pointer at buffer (buffer)
;            %3 = buffer length (len)
; Return Value:
;            rax = Amount of written bytes (Size) 
; ======================================================
%macro fwrite 3
    mov rax, SYS_write
    mov rdi, %1
    mov rsi, %2
    mov rdx, %3
    syscall
%endmacro


; ======================================================
; ---------------------- fread -------------------------
; fread fd, buffer, len
; ------------------------------------------------------
; Arguments:
;            %1 = File-Descriptor (fd)
;            %2 = pointer at buffer (buffer)
;            %3 = max. amount/size of bytes
; Return Value:
;            rax = amount of bytes which have been read
; ======================================================
%macro fread 3
    mov rax, SYS_read
    mov rdi, %1
    mov rsi, %2
    mov rdx, %3
    syscall
%endmacro


; ======================================================
; --------------------- fclose -------------------------
; fclose fd
; ------------------------------------------------------
; Arguments:
;            %1 = File-Descriptor (fd)
; Return Value:
;            rax = 0 --> Success or -errno ---> Error
; ======================================================
%macro fclose 1
    mov rax, SYS_close
    mov rdi, %1
    syscall
%endmacro

; ========================================================================
; High-level wrappers
; ========================================================================

; ========================================================================
; writes2file <path>, <string>
; Creates/truncates file and writes string literal
; Example: writes2file filename, "Hi NASM!"
; ========================================================================
%macro writes2file 2
    section .rodata
    	%%str db %2
    	%%strlen equ $ - %%str
    section .text
    	fopen %1, O_CREAT | O_WRONLY | O_TRUNC, 0644
    	mov r12, rax
    	fwrite r12, %%str, %%strlen
        fclose r12
%endmacro

; ------------------------------------------------------------------------
; writed2file <path>, <data_ptr>, <len>
; Same as writes2file but uses variables/buffers
; Example: writed2file "data.txt", mybuf, mylen
; ------------------------------------------------------------------------
%macro writed2file 3
    fopen %1, O_CREAT | O_WRONLY | O_TRUNC, 0644
    mov r12, rax                                  ; rbx --> original
    fwrite r12, %2, %3
    fclose r12
%endmacro

; ------------------------------------------------------------------------
; appendd2file <path>, <data_ptr>, <len>
; Doesn't overwrite file content like writed2file or writes2file ...
; Add's data to file content
; Example: appendd2file "data.txt", mybuf, mylen
; ------------------------------------------------------------------------
%macro appendd2file 3
    fopen %1, O_CREAT | O_WRONLY | O_APPEND, 0644
    mov rbx, rax
    fwrite rbx, %2, %3
    fclose rbx
%endmacro

; ------------------------------------------------------------------------
; appends2file <path>, <data_ptr>
; Like appendd2file but with immediate string and without len param.
; Example: appendd2file filename, "I do not overwrite ..."
; ------------------------------------------------------------------------
%macro appends2file 2
    section .rodata
    	%%str db %2
    	%%strlen equ $ - %%str
    section .text
    	fopen %1, O_CREAT | O_WRONLY | O_APPEND, 0644
    	mov r12, rax
    	fwrite r12, %%str, %%strlen
        fclose r12
%endmacro

; ------------------------------------------------------------------------
; readfile_loud <path>, <buf>, <buflen>
; Opens and prints file to stdout (print file content)
; Example: readfile_loud filename, buffer, 1024
; Anzahl der gelesenen Bytes ---> r13
; ------------------------------------------------------------------------
%macro readfile_loud 3
    fopen %1, O_RDONLY, 0
    mov r12, rax
    fread r12, %2, %3
    mov r13, rax
    fwrite 1, %2, r13
    fclose r12
%endmacro

; ------------------------------------------------------------------------
; readfile <path>, <buf>, <buflen>
; Opens file and reads it
; Example: readfile filename, buffer, 1024
; Anzahl der gelesenen Bytes ---> r13
; ------------------------------------------------------------------------
%macro readfile 3
    fopen %1, O_RDONLY, 0
    mov r12, rax
    fread r12, %2, %3
    mov r13, rax
    fclose r12
%endmacro

; ------------------------------------------------------------------------
; filesize <filename>, <reg>
; Example: filesize logfile, r13
; Amount of bytes read ---> r13
; ------------------------------------------------------------------------
%macro filesize 2
    ; %1 = filename (address)
    ; %2 = Register for filesize (rax, rbx, etc.)

    ; Reserve structure for stat on the stack
    sub rsp, 144              ; stat struct for x86-64 Linux
    mov rdi, %1               ; filename
    lea rsi, [rsp]            ; pointer on stat struct
    mov eax, 4                ; sys_stat (Linux x86-64)
    syscall

    ; Return size
    mov %2, qword [rsp + 48] ; Offset st_size in struct stat = 48 Bytes
    add rsp, 144
%endmacro
