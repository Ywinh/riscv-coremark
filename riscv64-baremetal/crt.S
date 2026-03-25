.global _start
.section .text.init

.balign 16
_start:

.option push
.option norelax
    la gp, __global_pointer$
.option pop
    la sp, stack_top
    li a0, 0
    li a1, 0
    call main

1:
    fence
    li a7, 93
    ecall
    j 1b
