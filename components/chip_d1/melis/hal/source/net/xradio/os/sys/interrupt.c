#include <stdio.h>
#include "csi_core.h"

int arch_irq_save(void)
{
    return csi_irq_save();
}

/*
 * restore saved IRQ state
 */
void arch_irq_restore(int flags)
{
    csi_irq_restore(flags);
}	

/*
 * Enable IRQs
 */
void arch_irq_enable(void)
{
    __enable_irq();
}

/*
 * Disable IRQs
 */
void arch_irq_disable(void)
{
	__disable_irq();
}
