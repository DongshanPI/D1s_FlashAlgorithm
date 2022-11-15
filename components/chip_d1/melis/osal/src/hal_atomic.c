/*
 * ===========================================================================================
 *
 *       Filename:  hal_atomic.c
 *
 *    Description:  atomic impl. for hal layer.
 *
 *        Version:  Melis3.0
 *         Create:  2019-11-26 15:16:41
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-04-02 17:10:48
 *
 * ===========================================================================================
 */

#include <hal_atomic.h>
#include "arch.h"

extern uint32_t csi_kernel_sched_suspend(void);
extern void csi_kernel_sched_resume(uint32_t sleep_ticks);

int __os_critical_enter(unsigned int *lock)
{
    (void)lock;
#ifndef CONFIG_KERNEL_NONE
    csi_kernel_sched_suspend();
#endif

    return 0;
}

int __os_critical_exit(unsigned int *lock)
{
    (void)lock;
#ifndef CONFIG_KERNEL_NONE
    csi_kernel_sched_resume(0);
#endif

    return 0;
}

/* FIXME: handle lock */
void hal_spin_lock(hal_spinlock_t *lock)
{
    // rt_enter_critical();
    __os_critical_enter(0);
    return;
}

/* FIXME: handle lock */
void hal_spin_unlock(hal_spinlock_t *lock)
{
    // rt_exit_critical();
    __os_critical_exit(0);
    return;
}

/* FIXME: handle lock */
uint32_t hal_spin_lock_irqsave(hal_spinlock_t *lock)
{
    MELIS_CPSR_ALLOC();
    MELIS_CPU_CRITICAL_ENTER();
    __os_critical_enter(0);

    return cpsr;
}

/* FIXME: handle lock */
void hal_spin_unlock_irqrestore(hal_spinlock_t *lock, uint32_t cpsr)
{
    __os_critical_exit(0);
    MELIS_CPU_CRITICAL_LEAVE();
}

