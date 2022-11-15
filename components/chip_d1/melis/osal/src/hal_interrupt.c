/*
 * ===========================================================================================
 *
 *       Filename:  interrupt.c
 *
 *    Description:  interrupt osal impl. for hal layer.
 *
 *        Version:  Melis3.0
 *         Create:  2019-11-25 20:34:50
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-07-28 08:48:39
 *
 * ===========================================================================================
 */

#include <interrupt.h>
#include <kconfig.h>

uint32_t hal_interrupt_get_nest(void)
{
    // TODO:
    return 0;
    // uint32_t nest = rt_interrupt_get_nest();
    // return nest;
}

void hal_interrupt_enable(void)
{
    // TODO:
    // void local_irq_enable(void);
    // local_irq_enable();
}

void hal_interrupt_disable(void)
{
    // TODO:
    // void local_irq_disable(void);
    // local_irq_disable();
}

uint32_t hal_interrupt_save(void)
{
    // TODO:
    return 0;
    // return rt_hw_interrupt_disable();
}

void hal_interrupt_restore(uint32_t flag)
{
    // TODO:
    // return rt_hw_interrupt_enable(flag);
}
