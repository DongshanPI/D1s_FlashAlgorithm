/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

 /******************************************************************************
 * @file     startup.S
 * @brief    startup file. Should use with
 *           GCC for RISC-V Embedded Processors
 * @version  V1.0
 * @date     29. July 2019
 ******************************************************************************/

#include <csi_config.h>
#include <k_arch.h>

.global sys_stack_base
.global sys_stack_top
.global exception_stack_base
.global exception_stack_top
.globl  Reset_Handler

.section .bss
    .align  3
sys_stack_base:
    .space CONFIG_ARCH_INTERRUPTSTACK
sys_stack_top:

exception_stack_base:
    .space 256
exception_stack_top:

 .text
    .align  2
    j        Reset_Handler
    .align   2
    .long    0x594B5343 /* CSKY ASCII */
    .long    0x594B5343 /* CSKY ASCII */
    .align   2
    .long    Reset_Handler
_start:
    .type   Reset_Handler, %function
Reset_Handler:
.option push
.option norelax
    /* disable ie and clear all interrupts */
    csrw    mie, zero
    csrw    mip, zero

    /* Disable MIE to avoid triggering interrupts before the first task starts. */
    /* This bit is set when a task recovers context. */
    li      a0, SR_MIE
    csrc    mstatus, a0

    la      gp, __global_pointer$
.option pop

    la      a0, __Vectors
    csrw    mtvec, a0

#if (defined(CONFIG_CPU_E906F) || defined(CONFIG_CPU_E906D))|| defined(CONFIG_CPU_C910D)
    li      a0, 0x2000
    csrs    mstatus, a0
#endif

#if (defined(CONFIG_CPU_E906F) || defined(CONFIG_CPU_E906D))
    la      a0, __Vectors
    csrw    mtvt, a0
#endif

    la      sp, sys_stack_top

#ifndef __NO_SYSTEM_INIT
    la   a0, SystemInit
    jalr a0
#endif

#ifndef __NO_BOARD_INIT
    la   a0, board_pre_init
    jalr a0
#endif

#ifdef CONFIG_KERNEL_NONE
    /* enable interrupt */
    li      a0, SR_MIE
    csrs    mstatus, a0
#endif

    la   a0, pre_main
    jalr a0

    .size   Reset_Handler, . - Reset_Handler

__exit:
    j      __exit


