/*
 * ===========================================================================================
 *
 *       Filename:  irqflags.h
 *
 *    Description:  porting from linux.
 *
 *        Version:  Melis3.0
 *         Create:  2020-06-24 09:44:39
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-07-03 16:20:02
 *
 * ===========================================================================================
 */

#ifndef _ASM_RISCV_IRQFLAGS_H
#define _ASM_RISCV_IRQFLAGS_H

#include "csr.h"

/* read interrupt enabled status */
static inline unsigned long arch_local_save_flags(void)
{
    return csr_read(CSR_SSTATUS);
}

/* test flags */
static inline int arch_irqs_disabled_flags(unsigned long flags)
{
    return !(flags & SR_SIE);
}

/* test hardware interrupt enable bit */
static inline int arch_irqs_disabled(void)
{
    return arch_irqs_disabled_flags(arch_local_save_flags());
}

/* unconditionally enable interrupts */
static inline void arch_local_irq_enable(void)
{
    csr_set(CSR_SSTATUS, SR_SIE);
}

/* unconditionally disable interrupts */
static inline void arch_local_irq_disable(void)
{
    csr_clear(CSR_SSTATUS, SR_SIE);
}

/* get status and disable interrupts */
static inline unsigned long arch_local_irq_save(void)
{
    return csr_read_clear(CSR_SSTATUS, SR_SIE);
}

/* set interrupt enabled status */
static inline void arch_local_irq_restore(unsigned long flags)
{
    csr_set(CSR_SSTATUS, flags & SR_SIE);
}

static inline void wait_for_interrupt(void)
{
    __asm__ __volatile__("wfi");
}

#endif /* _ASM_RISCV_IRQFLAGS_H */
