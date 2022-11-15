/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _XR_SYS_INTERRUPT_H_
#define _XR_SYS_INTERRUPT_H_
#include "compiler.h"
#include "csi_core.h"

// #include <rthw.h>
#include <aos/kernel.h>
/*
 * Save the current interrupt enable state & disable IRQs
 */
int arch_irq_save(void);
/*
 * restore saved IRQ state
 */
void arch_irq_restore(int flags);	

/*
 * Enable IRQs
 */
void arch_irq_enable(void);
/*
 * Disable IRQs
 */
void arch_irq_disable(void);

#define xr_irq_save arch_irq_save
#define xr_irq_restore arch_irq_restore
#define arch_local_save_flags arch_irq_get_flags
#define xr_irq_enable arch_irq_enable
#define xr_irq_disable arch_irq_disable
#define xr_fiq_enable arch_fiq_enable
#define xr_fiq_disable arch_fiq_disable

#endif /* _SYS_INTERRUPT_H_ */
