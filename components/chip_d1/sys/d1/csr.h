/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2015 Regents of the University of California
 */

#ifndef _ASM_RISCV_CSR_H
#define _ASM_RISCV_CSR_H

#include <kconfig.h>
#include "asm.h"
#include "consts.h"

/* Status register flags */
#define SR_SIE      _AC(0x00000002, UL) /* Supervisor Interrupt Enable */
#define SR_SPIE     _AC(0x00000020, UL) /* Previous Supervisor IE */
#define SR_SPP      _AC(0x00000100, UL) /* Previously Supervisor */
#define SR_SUM      _AC(0x00040000, UL) /* Supervisor User Memory Access */

#define SR_FS       _AC(0x00006000, UL) /* Floating-point Status */
#define SR_FS_OFF   _AC(0x00000000, UL)
#define SR_FS_INITIAL   _AC(0x00002000, UL)
#define SR_FS_CLEAN _AC(0x00004000, UL)
#define SR_FS_DIRTY _AC(0x00006000, UL)

#define SR_XS       _AC(0x00018000, UL) /* Extension Status */
#define SR_XS_OFF   _AC(0x00000000, UL)
#define SR_XS_INITIAL   _AC(0x00008000, UL)
#define SR_XS_CLEAN _AC(0x00010000, UL)
#define SR_XS_DIRTY _AC(0x00018000, UL)

#ifndef CONFIG_64BIT
#define SR_SD       _AC(0x80000000, UL) /* FS/XS dirty */
#else
#define SR_SD       _AC(0x8000000000000000, UL) /* FS/XS dirty */
#endif

/* SATP flags */
#ifndef CONFIG_64BIT
#define SATP_PPN    _AC(0x003FFFFF, UL)
#define SATP_MODE_32    _AC(0x80000000, UL)
#define SATP_MODE   SATP_MODE_32
#else
#define SATP_PPN    _AC(0x00000FFFFFFFFFFF, UL)
#define SATP_MODE_39    _AC(0x8000000000000000, UL)
#define SATP_MODE   SATP_MODE_39
#define SATP_ASID_BITS  16
#define SATP_ASID_SHIFT 44
#define SATP_ASID_MASK  _AC(0xFFFF, UL)
#endif

/* SCAUSE */
#define SCAUSE_IRQ_FLAG     (_AC(1, UL) << (__riscv_xlen - 1))

#define IRQ_U_SOFT      0
#define IRQ_S_SOFT      1
#define IRQ_M_SOFT      3
#define IRQ_U_TIMER     4
#define IRQ_S_TIMER     5
#define IRQ_M_TIMER     7
#define IRQ_U_EXT       8
#define IRQ_S_EXT       9
#define IRQ_M_EXT       11
#define IRQ_S_PMU       17

#define EXC_INST_MISALIGNED 0
#define EXC_INST_ACCESS     1
#define EXC_INST_ILLEGAL    2
#define EXC_BREAKPOINT      3
#define EXC_LOAD_MISALIGN   4
#define EXC_LOAD_ACCESS     5
#define EXC_STORE_MISALIGN  6
#define EXC_STORE_ACCESS    7
#define EXC_SYSCALL_FRM_U   8
#define EXC_SYSCALL_FRM_S   9
#define EXC_SYSCALL_FRM_M   11
#define EXC_INST_PAGE_FAULT 12
#define EXC_LOAD_PAGE_FAULT 13
#define EXC_STORE_PAGE_FAULT    15

/* SIE (Interrupt Enable) and SIP (Interrupt Pending) flags */
#define SIE_SSIE        (_AC(0x1, UL) << IRQ_S_SOFT)
#define SIE_STIE        (_AC(0x1, UL) << IRQ_S_TIMER)
#define SIE_SEIE        (_AC(0x1, UL) << IRQ_S_EXT)
#define SIE_SMIE        (_AC(0x1, UL) << IRQ_S_PMU)

#define CSR_CYCLE       0xc00
#define CSR_TIME        0xc01
#define CSR_INSTRET     0xc02
#define CSR_SSTATUS     0x100
#define CSR_SIE         0x104
#define CSR_STVEC       0x105
#define CSR_SCOUNTEREN      0x106
#define CSR_SSCRATCH        0x140
#define CSR_SEPC        0x141
#define CSR_SCAUSE      0x142
#define CSR_STVAL       0x143
#define CSR_SIP         0x144
#define CSR_SATP        0x180
#define CSR_CYCLEH      0xc80
#define CSR_TIMEH       0xc81
#define CSR_INSTRETH        0xc82

/*MSTATUS 机器模式读写0x300 机器模式处理器状态寄存器*/
#define CSR_MSTATUS 0x300
/*MISA 机器模式读写0x301 机器模式处理器指令集架构寄存器*/
#define CSR_MISA 0x301
/*MEDELEG 机器模式读写0x302 机器模式异常降级控制寄存器*/
#define CSR_MEDELEG 0x302
/*MIDELEG 机器模式读写0x303 机器模式中断降级控制寄存器*/
#define CSR_MIDELEG 0x303
/*MIE 机器模式读写0x304 机器模式中断使能控制寄存器*/
#define CSR_MIE 0x304
/*MTVEC 机器模式读写0x305 机器模式向量基址寄存器*/
#define CSR_MTVEC 0x305
/*MCOUNTEREN 机器模式读写0x306 机器模式计数器授权控制寄存器*/
#define CSR_MCOUNTEREN 0x306

#ifndef __ASSEMBLY__

#define csr_swap(csr, val)                  \
    ({                              \
        unsigned long __v = (unsigned long)(val);       \
        __asm__ __volatile__ ("csrrw %0, " __ASM_STR(csr) ", %1"\
                              : "=r" (__v) : "rK" (__v)     \
                              : "memory");          \
        __v;                            \
    })

#define csr_read(csr)                       \
    ({                              \
        register unsigned long __v;             \
        __asm__ __volatile__ ("csrr %0, " __ASM_STR(csr)    \
                              : "=r" (__v) :            \
                              : "memory");          \
        __v;                            \
    })

#define csr_write(csr, val)                 \
    ({                              \
        unsigned long __v = (unsigned long)(val);       \
        __asm__ __volatile__ ("csrw " __ASM_STR(csr) ", %0" \
                              : : "rK" (__v)            \
                              : "memory");          \
    })

#define csr_read_set(csr, val)                  \
    ({                              \
        unsigned long __v = (unsigned long)(val);       \
        __asm__ __volatile__ ("csrrs %0, " __ASM_STR(csr) ", %1"\
                              : "=r" (__v) : "rK" (__v)     \
                              : "memory");          \
        __v;                            \
    })

#define csr_set(csr, val)                   \
    ({                              \
        unsigned long __v = (unsigned long)(val);       \
        __asm__ __volatile__ ("csrs " __ASM_STR(csr) ", %0" \
                              : : "rK" (__v)            \
                              : "memory");          \
    })

#define csr_read_clear(csr, val)                \
    ({                              \
        unsigned long __v = (unsigned long)(val);       \
        __asm__ __volatile__ ("csrrc %0, " __ASM_STR(csr) ", %1"\
                              : "=r" (__v) : "rK" (__v)     \
                              : "memory");          \
        __v;                            \
    })

#define csr_clear(csr, val)                 \
    ({                              \
        unsigned long __v = (unsigned long)(val);       \
        __asm__ __volatile__ ("csrc " __ASM_STR(csr) ", %0" \
                              : : "rK" (__v)            \
                              : "memory");          \
    })

#endif /* __ASSEMBLY__ */

#endif /* _ASM_RISCV_CSR_H */
