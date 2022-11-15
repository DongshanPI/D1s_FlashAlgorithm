/*
 * =====================================================================================
 *
 *       Filename:  port.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2020年06月08日 18时47分07秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zeng Zhijin
 *   Organization:
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <arch.h>
#include <csr.h>
#include <irqflags.h>

/* ----------------------------------------------------------------------------*/
/** @brief  awos_arch_irq_trap_enter <enter interrupt log.> */
/* ----------------------------------------------------------------------------*/
void awos_arch_irq_trap_enter(void)
{
    // TODO:
}

/* ----------------------------------------------------------------------------*/
/** @brief  awos_arch_irq_trap_leave <leave interrupt log.> */
/* ----------------------------------------------------------------------------*/
void awos_arch_irq_trap_leave(void)
{
    // TODO:
}

/* ----------------------------------------------------------------------------*/
/** @brief  awos_arch_tick_increase <system tick increase, must be invokded in
 *          timier irq context.>
 */
/* ----------------------------------------------------------------------------*/
void awos_arch_tick_increase(void)
{
    // TODO:
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_isin_irq <judge whether in irq context>
 *
 * @return
 *   -0: in task environment
 *   -1: in interrupt environment.
 */
/* ----------------------------------------------------------------------------*/
uint8_t awos_arch_isin_irq(void)
{
    // TODO:
    return 0;
}

unsigned long awos_arch_lock_irq(void)
{
    return arch_local_irq_save();
}

void awos_arch_unlock_irq(unsigned long cpu_sr)
{
    arch_local_irq_restore(cpu_sr);
}

int irqs_disabled(void)
{
    return arch_irqs_disabled();
}

void local_irq_enable(void)
{
    arch_local_irq_enable();
}

void local_irq_disable(void)
{
    arch_local_irq_disable();
}
