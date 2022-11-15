/*
 * ===========================================================================================
 *
 *       Filename:  interrupt.c
 *
 *    Description:  porting from linux.
 *
 *        Version:  Melis3.0
 *         Create:  2019-11-19 10:33:38
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2021-01-18 14:22:08
 *
 * ===========================================================================================
 */

#include "interrupt.h"
#include "irqdesc.h"
#include "irq_internal.h"
// #include <rtthread.h>
#include <typedef.h>
#include <excep.h>
#include <debug.h>
#include <arch.h>
#include <csr.h>
#include <log.h>
#include <irqflags.h>
#include <stdlib.h>

/*
 * Generic no controller implementation
 */
extern struct irq_chip plic_chip;
struct irq_chip no_irq_chip =
{
    .name           = "none",
    .irq_mask       = NULL,
    .irq_mask_ack   = NULL,
    .irq_unmask     = NULL,
    .irq_eoi        = NULL,
};

static void handle_level_irq(struct irq_desc *desc);
struct irq_desc g_irq_desc[NR_IRQS]  =
{
    [0 ... NR_IRQS - 1] = {
        .handle_irq     = handle_level_irq,
        .action         = NULL,
        .number         = 0,
        .nr_actions     = 0,
        .dev_name       = '\0',
        .name           = '\0',
    }
};

void handle_level_irq(struct irq_desc *desc)
{
    struct irq_chip *chip = desc->irq_data.chip;
    unsigned int irq = desc->irq_data.irq;
    struct irqaction *action;

    for_each_action_of_desc(desc, action)
    {
        irqreturn_t res;
        res = action->handler(irq, action->dev_id);
        action->irq_nums ++;

        switch (res)
        {
            case IRQ_WAKE_THREAD:
            /* Fall through - to add to randomness */
            case IRQ_HANDLED:
                break;
            default:
                break;
        }
    }

    // ack eoi.
    chip->irq_eoi(&desc->irq_data);
}

int d1_interrupt_init(void)
{
    int irqno = 0;

    for (irqno = 0; irqno < NR_IRQS; irqno ++)
    {
        g_irq_desc[irqno].irq_data.mask       = 0xffffffff;
        g_irq_desc[irqno].irq_data.irq        = irqno;
        g_irq_desc[irqno].irq_data.chip       = &plic_chip;
        g_irq_desc[irqno].irq_data.chip_data  = NULL;
    }

    /* Enable all interrupts */
    csr_write(CSR_SIE, -1);
    return 0;
}

struct irq_desc *irq_to_desc(unsigned int irq)
{
    return (irq < NR_IRQS) ? g_irq_desc + irq : NULL;
}

/*
 * Internal function to register an irqaction - typically used to
 * allocate special interrupts that are part of the architecture.
 *
 * Locking rules:
 *
 * desc->request_mutex  Provides serialization against a concurrent free_irq()
 *   chip_bus_lock  Provides serialization for slow bus operations
 *     desc->lock   Provides serialization against hard interrupts
 *
 * chip_bus_lock and desc->lock are sufficient for all other management and
 * interrupt related functions. desc->request_mutex solely serializes
 * request/free_irq().
 */
static int __setup_irq(unsigned int irq, struct irq_desc *desc, struct irqaction *new)
{
    struct irqaction *old, **old_ptr;
    unsigned long flags;
    // int ret, nested;

    if (!desc)
    {
        __err("no desc for this irq %d.", irq);
        return -1;
    }

    if (desc->irq_data.chip == NULL)
    {
        __err("irq chip is null.");
        return -1;
    }

    new->irq = irq;
    desc->dev_name = (char *)&(new->name);
    desc->name = desc->dev_name;
    desc->number = 0;
    /*
     * Drivers are often written to work w/o knowledge about the
     * underlying irq chip implementation, so a request for a
     * threaded irq without a primary hard irq context handler
     * requires the ONESHOT flag to be set. Some irq chips like
     * MSI based interrupts are per se one shot safe. Check the
     * chip flags, so we can avoid the unmask dance at the end of
     * the threaded handler for those.
     */
    if (desc->irq_data.chip->flags & IRQCHIP_ONESHOT_SAFE)
    {
        new->flags &= ~IRQF_ONESHOT;
    }

    /*
     * The following block of code has to be executed atomically
     * protected against a concurrent interrupt and any of the other
     * management calls which are not serialized via
     * desc->request_mutex or the optional bus lock.
     */
    flags = awos_arch_lock_irq();
    old_ptr = &desc->action;
    old = *old_ptr;
    if (old)
    {
        /* add new interrupt at end of irq queue */
        do
        {
            old_ptr = &old->next;
            old = *old_ptr;
        } while (old);
    }

    *old_ptr = new;

    /*
     * Check whether we disabled the irq via the spurious handler
     * before. Reenable it and give it another chance.
     */
    void enable_irq(unsigned int irq);
    enable_irq(irq);

    awos_arch_unlock_irq(flags);

    return 0;
}

int request_threaded_irq(unsigned int irq, irq_handler_t handler,
                         irq_handler_t thread_fn, unsigned long irqflags,
                         const char *devname, void *dev_id)
{
    struct irqaction *action;
    struct irq_desc *desc;
    int retval;

    if (irq == IRQ_NOTCONNECTED)
    {
        __err("irq invalided!");
        return -1;
    }

    /*
    ¦* Sanity-check: shared interrupts must pass in a real dev-ID,
    ¦* otherwise we'll have trouble later trying to figure out
    ¦* which interrupt is which (messes up the interrupt freeing
    ¦* logic etc).
    ¦*
    ¦* Also IRQF_COND_SUSPEND only makes sense for shared interrupts and
    ¦* it cannot be set along with IRQF_NO_SUSPEND.
    ¦*/
    if (((irqflags & IRQF_SHARED) && !dev_id) ||
        (!(irqflags & IRQF_SHARED) && (irqflags & IRQF_COND_SUSPEND)) ||
        ((irqflags & IRQF_NO_SUSPEND) && (irqflags & IRQF_COND_SUSPEND)))
    {
        __err("parameter invalided!");
        return -1;
    }

    desc = irq_to_desc(irq);
    if (!desc)
    {
        __err("no desc for this irq %d.!", irq);
        return -1;
    }

    if (!handler)
    {
        __err("handler parameter is null");
        return -1;
    }

    action = malloc(sizeof(struct irqaction));
    if (!action)
    {
        __err("no free buffer.");
        return -1;
    }

    memset(action, 0x00, sizeof(struct irqaction));
    action->handler = handler;
    action->flags = irqflags;
    if (devname) {
        strncpy((char *)action->name, devname, IRQACTION_NAME_MAX - 1);
    } else {
        strncpy((char *)action->name, "none", IRQACTION_NAME_MAX - 1);
    }
    action->dev_id = dev_id;
    action->irq_nums = 0;

    retval = __setup_irq(irq, desc, action);
    if (retval)
    {
        free(action);
        action = NULL;
    }

    if (!retval && (irqflags & IRQF_SHARED))
    {
        /*
        ¦* It's a shared IRQ -- the driver ought to be prepared for it
        ¦* to happen immediately, so let's make sure....
        ¦* We disable the irq to make sure that a 'real' IRQ doesn't
        ¦* run in parallel with our fake.
        ¦*/
        unsigned long flags;

        flags = awos_arch_lock_irq();
        handler(irq, dev_id);
        awos_arch_unlock_irq(flags);
    }

    return retval;
}

int generic_handle_irq(unsigned int irq)
{
    struct irq_desc *desc = irq_to_desc(irq);
    if (!desc)
    {
        // __err("fatal error, desc is null for irq %d.", irq);
        // software_break();
        return -1;
    }
    generic_handle_irq_desc(desc);
    return 0;
}


static void ack_bad_irq(unsigned int irq)
{
    __err("PANIC: %s line %d, fata error.", __func__, __LINE__);
    while (1);
}

int __handle_domain_irq(void *domain, unsigned int hwirq,
                        bool lookup, irq_regs_t *regs)
{
    int ret = 0;
    unsigned irq = hwirq;
    int32_t nr_irqs = NR_IRQS;
    /*
    ¦* Some hardware gives randomly wrong interrupts.  Rather
    ¦* than crashing, do something sensible.
    ¦*/
    if (unlikely(!irq || irq >= nr_irqs))
    {
        ack_bad_irq(irq);
        ret = -1;
    }
    else
    {
        generic_handle_irq(irq);
    }

    return ret;
}

static inline int handle_domain_irq(void *domain, unsigned int hwirq, irq_regs_t *regs)
{
    return __handle_domain_irq(domain, hwirq, true, regs);
}

void gic_handle_irq(irq_regs_t *regs)
{

}

/*
 * Internal function to unregister an irqaction - used to free
 * regular and special interrupts that are part of the architecture.
 */
static struct irqaction *__free_irq(struct irq_desc *desc, void *dev_id)
{
    unsigned irq = desc->irq_data.irq;
    struct irqaction *action, **action_ptr;
    unsigned long flags;

    flags = awos_arch_lock_irq();

    /*
    ¦* There can be multiple actions per IRQ descriptor, find the right
    ¦* one based on the dev_id:
    ¦*/
    action_ptr = &desc->action;
    for (; ;)
    {
        action = *action_ptr;

        if (!action)
        {
            __err("Trying to free already-free IRQ %d", irq);
            awos_arch_unlock_irq(flags);
            return NULL;
        }

        if (action->dev_id == dev_id)
        {
            break;
        }
        action_ptr = &action->next;
    }

    /* Found it - now remove it from the list of entries: */
    *action_ptr = action->next;

    /* If this was the last handler, shut down the IRQ line: */
    if (!desc->action)
    {
        /* Only shutdown. Deactivate after synchronize_hardirq() */
        void disable_irq(unsigned int irq);
        disable_irq(irq);
    }

    awos_arch_unlock_irq(flags);

    return action;
}
/**
 *      free_irq - free an interrupt allocated with request_irq
 *      @irq: Interrupt line to free
 *      @dev_id: Device identity to free
 *
 *      Remove an interrupt handler. The handler is removed and if the
 *      interrupt line is no longer in use by any driver it is disabled.
 *      On a shared IRQ the caller must ensure the interrupt is disabled
 *      on the card it drives before calling this function. The function
 *      does not return until any executing interrupts for this IRQ
 *      have completed.
 *
 *      This function must not be called from interrupt context.
 *
 *      Returns the devname argument passed to request_irq.
 */
const void *free_irq(unsigned int irq, void *dev_id)
{
    struct irq_desc *desc = irq_to_desc(irq);
    struct irqaction *action;
    const char *devname;

    if (!desc)
    {
        __err("no desc for irq %d.", irq);
        return NULL;
    }

    action = __free_irq(desc, dev_id);

    if (!action)
    {
        __err("find no desc for irq %d.", irq);
        return NULL;
    }

    devname = action->name;
    free(action);
    return devname;
}

/**
 *      remove_irq - free an interrupt
 *      @irq: Interrupt line to free
 *      @act: irqaction for the interrupt
 *
 * Used to remove interrupts statically setup by the early boot process.
 */
void remove_irq(unsigned int irq, struct irqaction *act)
{
    struct irq_desc *desc = irq_to_desc(irq);

    if (desc)
    {
        __free_irq(desc, act->dev_id);
    }
}

static void __disable_irq(struct irq_desc *desc)
{
    int32_t irq_disable(uint32_t irq_no);

    desc->number--;
    irq_disable(desc->irq_data.irq);
}

static int __disable_irq_nosync(unsigned int irq)
{
    unsigned long flags;

    flags = awos_arch_lock_irq();
    struct irq_desc *desc = irq_to_desc(irq);
    if (!desc)
    {
        awos_arch_unlock_irq(flags);
        return -1;
    }
    __disable_irq(desc);
    awos_arch_unlock_irq(flags);

    return 0;
}
/**
 *      disable_irq - disable an irq and wait for completion
 *      @irq: Interrupt to disable
 *
 *      Disable the selected interrupt line.  Enables and Disables are
 *      nested.
 *      This function waits for any pending IRQ handlers for this interrupt
 *      to complete before returning. If you use this function while
 *      holding a resource the IRQ handler may need you will deadlock.
 *
 *      This function may be called - with care - from IRQ context.
 */
void disable_irq(unsigned int irq)
{
    __disable_irq_nosync(irq);
}

void __enable_irq(struct irq_desc *desc)
{
    int32_t irq_enable(uint32_t irq_no);

    desc->number++;
    irq_enable(desc->irq_data.irq);
}

/**
 *      enable_irq - enable handling of an irq
 *      @irq: Interrupt to enable
 *
 *      Undoes the effect of one call to disable_irq().  If this
 *      matches the last disable, processing of interrupts on this
 *      IRQ line is re-enabled.
 *
 *      This function may be called from IRQ context only when
 *      desc->irq_data.chip->bus_lock and desc->chip->bus_sync_unlock are NULL !
 */
void enable_irq(unsigned int irq)
{
    unsigned long flags;

    flags = awos_arch_lock_irq();

    struct irq_desc *desc = irq_to_desc(irq);
    if (!desc)
    {
        printf("cant find the desc of irq %d.\r\n", irq);
        awos_arch_unlock_irq(flags);
        return;
    }
    __enable_irq(desc);

    awos_arch_unlock_irq(flags);
}

/**
 *      disable_hardirq - disables an irq and waits for hardirq completion
 *      @irq: Interrupt to disable
 *
 *      Disable the selected interrupt line.  Enables and Disables are
 *      nested.
 *      This function waits for any pending hard IRQ handlers for this
 *      interrupt to complete before returning. If you use this function while
 *      holding a resource the hard IRQ handler may need you will deadlock.
 *
 *      When used to optimistically disable an interrupt from atomic context
 *      the return value must be checked.
 *
 *      Returns: false if a threaded handler is active.
 *
 *      This function may be called - with care - from IRQ context.
 */
bool disable_hardirq(unsigned int irq)
{
    __disable_irq_nosync(irq);
    return true;
}

/**
 *      disable_nmi_nosync - disable an nmi without waiting
 *      @irq: Interrupt to disable
 *
 *      Disable the selected interrupt line. Disables and enables are
 *      nested.
 *      The interrupt to disable must have been requested through request_nmi.
 *      Unlike disable_nmi(), this function does not ensure existing
 *      instances of the IRQ handler have completed before returning.
 */
void disable_nmi_nosync(unsigned int irq)
{
    __disable_irq_nosync(irq);
}

/**
 *      setup_irq - setup an interrupt
 *      @irq: Interrupt line to setup
 *      @act: irqaction for the interrupt
 *
 * Used to statically setup interrupts in the early boot process.
 */
int setup_irq(unsigned int irq, struct irqaction *act)
{
    int retval;
    struct irq_desc *desc = irq_to_desc(irq);

    if (!desc || !act)
    {
        __err("cant find irq descriptor.");
        return -1;
    }

    retval = __setup_irq(irq, desc, act);

    return retval;
}

void irq_suspend(void)
{
	struct irq_desc *desc;
	struct irqaction *action;
	unsigned long l;
	int i;

	l = awos_arch_lock_irq();
	for (i = 0; i < NR_IRQS; i++) {
		desc = &g_irq_desc[i];

		for_each_action_of_desc(desc, action) {
			if (desc->number && !(action->flags & IRQF_NO_SUSPEND)) {
				int32_t irq_disable(uint32_t irq_no);
				irq_disable(desc->irq_data.irq);
				break;
			}
		}
	}
	awos_arch_unlock_irq(l);
}

void irq_resume(void)
{
	struct irq_desc *desc;
	struct irqaction *action;
	unsigned long l;
	int i;

	l = awos_arch_lock_irq();
	for (i = 0; i < NR_IRQS; i++) {
		desc = &g_irq_desc[i];

		for_each_action_of_desc(desc, action) {
			if (desc->number && !(action->flags & IRQF_NO_SUSPEND)) {
				int32_t irq_enable(uint32_t irq_no);
				irq_enable(desc->irq_data.irq);
				break;
			}
		}
	}
	awos_arch_unlock_irq(l);
}

void show_irqs(void)
{
    int i;
    struct irq_desc *desc = NULL;
    struct irqaction *action = NULL;;

    printf(" irqno    devname     name     irqname          handler        active.\n");
    printf(" ---------------------------------------------------------------------\n");
    for (i = 0; i < NR_IRQS; i ++)
    {
        int line = 0;
        desc = irq_to_desc(i);
        if (!desc || desc->action == NULL)
        {
            continue;
        }

        printf("%6d%11s%9s", i, desc->dev_name, desc->name);
        for_each_action_of_desc(desc, action)
        {
            if (line !=  0)
            {
                printf("%-*.s", 26, "");
            }
            line ++;
            printf("%*.*s       0x%lx     0x%08lx\n", 12, 10, action->name, (unsigned long)action->handler, action->irq_nums);
        }
    }
    printf(" ---------------------------------------------------------------------\n");
}


void plic_handle_irq(irq_regs_t *);
void handle_arch_irq(irq_regs_t *regs)
{
    unsigned long sip = csr_read(CSR_SIP);

    if (!(sip & (SIE_STIE | SIE_SEIE)))
    {
        __err("sip status error.");
        software_break();
    }

    plic_handle_irq(regs);

    return;
}

void riscv_pmu_handle_irq(void);
void riscv_timer_interrupt(void);
extern uint32_t  g_irq_nested_level;
unsigned long riscv_cpu_handle_interrupt(unsigned long scause, unsigned long sepc, unsigned long stval, irq_regs_t *regs)
{
    g_irq_nested_level++;
    if (!(scause & SCAUSE_IRQ_FLAG))
    {
        __err("fatal error, scause corruption.");
        software_break();
        g_irq_nested_level--;
        return 1;
    }

    switch (scause & ~SCAUSE_IRQ_FLAG)
    {
        /*
         * Possible interrupt causes:
         */
        case IRQ_U_SOFT:
        case IRQ_M_SOFT:
        case IRQ_U_TIMER:
        case IRQ_M_TIMER:
        case IRQ_U_EXT:
        case IRQ_M_EXT:
            {
                __err("unexpected interrupt cause 0x%lx", scause);
                software_break();
                break;
            }
        case IRQ_S_SOFT:
            {
                __err("i dont know how todo becasue only smp support IPI use this, cause 0x%lx", scause);
                software_break();
                break;
            }
        case IRQ_S_PMU:
            riscv_pmu_handle_irq();
            break;

        case IRQ_S_TIMER:
            riscv_timer_interrupt();
            break;

        case IRQ_S_EXT:
            handle_arch_irq(regs);
            break;

        default:
            __err("unexpected interrupt cause 0x%lx", scause);
            software_break();
            break;
    }

    g_irq_nested_level--;
    return 0;
}
