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

#ifndef _KERNEL_XR_OS_RTTHREAD_XR_OS_TIMER_H_
#define _KERNEL_XR_OS_RTTHREAD_XR_OS_TIMER_H_

#include "kernel/os/os_common.h"
#include <k_api.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Timer type definition
 *     - one shot timer: Timer will be in the dormant state after it expires.
 *     - periodic timer: Timer will auto-reload after it expires.
 */
typedef enum {
    XR_OS_TIMER_ONCE       = 0, /* one shot timer */
    XR_OS_TIMER_PERIODIC   = 1  /* periodic timer */
} XR_OS_TimerType;

/** @brief Timer expire callback function definition */
typedef void (*XR_OS_TimerCallback_t)(void *arg);

/** @brief Timer handle definition */
typedef aos_timer_t XR_OS_TimerHandle_t;

typedef struct XR_OS_Timer
{
	aos_timer_t	handle;
	XR_OS_TimerCallback_t cb;
	void *arg;
} XR_OS_Timer_t;


XR_OS_Status XR_OS_TimerCreate(XR_OS_Timer_t *timer, XR_OS_TimerType type,
                         XR_OS_TimerCallback_t cb, void *arg, XR_OS_Time_t periodMS);
XR_OS_Status XR_OS_TimerDelete(XR_OS_Timer_t *timer);
XR_OS_Status XR_OS_TimerStart(XR_OS_Timer_t *timer);
XR_OS_Status XR_OS_TimerChangePeriod(XR_OS_Timer_t *timer, XR_OS_Time_t periodMS);
XR_OS_Status XR_OS_TimerStop(XR_OS_Timer_t *timer);

/**
 * @brief Check whether the timer object is valid or not
 * @param[in] timer Pointer to the timer object
 * @return 1 on valid, 0 on invalid
 */
static inline int XR_OS_TimerIsValid(XR_OS_Timer_t *timer)
{
	return (timer->handle.hdl != XR_OS_INVALID_HANDLE);
}

/**
 * @brief Set the timer object to invalid state
 * @param[in] timer Pointer to the timer object
 * @return None
 */
static inline void XR_OS_TimerSetInvalid(XR_OS_Timer_t *timer)
{
	timer->handle.hdl = XR_OS_INVALID_HANDLE;
}

/**
 * @brief Check whether the timer is active or not
 *
 * A timer is inactive when it is in one of the following cases:
 *     - The timer has been created, but not started.
 *     - The timer is a one shot timer that has not been restarted since it
 *       expired.
 *
 * @param[in] timer Pointer to the timer object
 * @return 1 on active, 0 on inactive
 */
static inline int XR_OS_TimerIsActive(XR_OS_Timer_t *timer)
{
    ktimer_t *k_timer = timer->handle.hdl;

	return ((k_timer->timer_state == TIMER_ACTIVE) ? 1 : 0);
}

#ifdef __cplusplus
}
#endif

#endif /* _KERNEL_XR_OS_RTTHREAD_XR_OS_TIMER_H_ */
