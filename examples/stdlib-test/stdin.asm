; Copyright (C) 2025 Sector72

%include "stdlib.inc"

section .text
global _start

_start:
    prints "Type username: "
    read name, 32
    prints "Username: "
    printd name, r12      ; gibt die eingegebenen Bytes aus

    prints "Type password: "
    read password, 32
    prints "Password: "
    printd password, r12

    exit 0


