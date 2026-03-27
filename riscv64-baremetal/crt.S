.global _start
.section .text.init

.balign 16
_start:
    li  x1, 0
    li  x2, 0
    li  x3, 0
    li  x4, 0
    li  x5, 0
    li  x6, 0
    li  x7, 0
    li  x8, 0
    li  x9, 0
    li  x10,0
    li  x11,0
    li  x12,0
    li  x13,0
    li  x14,0
    li  x15,0
    li  x16,0
    li  x17,0
    li  x18,0
    li  x19,0
    li  x20,0
    li  x21,0
    li  x22,0
    li  x23,0
    li  x24,0
    li  x25,0
    li  x26,0
    li  x27,0
    li  x28,0
    li  x29,0
    li  x30,0
    li  x31,0

.option push
.option norelax
    la gp, __global_pointer$
.option pop

#ifndef enable_920
#define enable_920 0
#endif

#if enable_920
    li t0, 0x70013
    csrs mcor, t0

    li t0, 0x11ff
    csrs mhcr, t0

    li t0, 0x6e30c
    csrs mhint, t0

    li t0, 0x6b8000
    csrs mxstatus, t0

    li t0, 0xe0000009
    csrs mccr2, t0

    li t0, 0x1
    csrs 0x323, t0

    li t0, 0x2
    csrs 0x324, t0

    li t0, 0x3
    csrs 0x325, t0

    li t0, 0x5
    csrs 0x326, t0

    li t0, 0x6
    csrs 0x327, t0

    li t0, 0x7
    csrs 0x328, t0

    li t0, 0x8
    csrs 0x329, t0

    li t0, 0x9
    csrs 0x32a, t0

    li t0, 0x1b
    csrs 0x32b, t0

    li t0, 0x1c
    csrs 0x32c, t0

    li t0, 0x27
    csrs 0x32d, t0

    li t0, 0x4
    csrs 0x32e, t0

    li t0, 0xa
    csrs 0x32f, t0

    li t0, 0xb
    csrs 0x330, t0

    li t0, 0xc
    csrs 0x331, t0

    li t0, 0xd
    csrs 0x332, t0

    li t0, 0xe
    csrs 0x333, t0

    li t0, 0xf
    csrs 0x334, t0

    li t0, 0x10
    csrs 0x335, t0

    li t0, 0x11
    csrs 0x336, t0

    li t0, 0x12
    csrs 0x337, t0

    li t0, 0x13
    csrs 0x338, t0

    li t0, 0x17
    csrs 0x339, t0

    li t0, 0x18
    csrs 0x33a, t0

    li t0, 0x28
    csrs 0x33b, t0

    li t0, 0x1d
    csrs 0x33c, t0

    li t0, 0x1e
    csrs 0x33d, t0

    li t0, 0x1f
    csrs 0x33e, t0

    li t0, 0x2a
    csrs 0x33f, t0
#endif

    la sp, stack_top
    li a0, 0
    li a1, 0
    call main

1:
    fence
    li a7, 93
    ecall
    j 1b
