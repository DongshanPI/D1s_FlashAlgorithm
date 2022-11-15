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

#ifndef _KERNEL_XR_OS_RTTHREAD_XR_OS_TIME_H_
#define _KERNEL_XR_OS_RTTHREAD_XR_OS_TIME_H_

#include "kernel/os/os_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Parameters used to convert the time values */
// #define XR_OS_MSEC_PER_SEC     1000U       /* milliseconds per second */
// #define XR_OS_USEC_PER_MSEC    1000U       /* microseconds per millisecond */
// #define XR_OS_USEC_PER_SEC     1000000U    /* microseconds per second */

/* system clock's frequency, OS ticks per second */
// #define XR_OS_HZ               (RT_TICK_PER_SECOND)

// /* microseconds per OS tick (1000000 / XR_OS_HZ) */
// #define XR_OS_TICK             (XR_OS_USEC_PER_SEC / XR_OS_HZ)

/** @brief Get the number of ticks since OS start */
/* Due to portTICK_TYPE_IS_ATOMIC is 1, calling xTaskGetTickCount() in ISR is
 * safe also.
 */
#define XR_OS_GetTicks()       ((XR_OS_Time_t)krhino_sys_tick_get())

/** @brief Get the number of seconds since OS start */
#define XR_OS_GetTime()        (aos_now_ms() / 1000)

/** @brief Get the number of seconds since OS start */
#define XR_OS_GetTimeMS()        (aos_now_ms())

/**
 * @brief Macros used to convert various time units to each other
 *     - Secs stand for seconds
 *     - MSecs stand for milliseconds
 *     - Ticks stand for OS ticks
 *     - Jiffies stand for OS jiffies, which is a synonym for OS ticks
 */
#define XR_OS_SecsToTicks(sec)     (aos_kernel_ms2tick(sec*1000))
static inline XR_OS_Time_t XR_OS_MSecsToTicks(uint32_t msec)
{
	uint64_t t = aos_kernel_ms2tick(msec);

	return t > 0? t : 1;
}

#define XR_OS_TicksToMSecs(t)      ((uint64_t)(aos_kernel_tick2ms(t)))
#define XR_OS_TicksToSecs(t)       ((uint64_t)(aos_kernel_tick2ms(t)/1000)

#define XR_OS_GetJiffies()         XR_OS_GetTicks()
#define XR_OS_SecsToJiffies(sec)   XR_OS_SecsToTicks(sec)
#define XR_OS_MSecsToJiffies(msec) XR_OS_MSecsToTicks(msec)
#define XR_OS_JiffiesToMSecs(j)    XR_OS_TicksToMSecs(j)
#define XR_OS_JiffiesToSecs(j)     XR_OS_TicksToSecs(j)

/**
 * @brief Macros used to sleep for the given time (milliseconds or seconds)
 */

// extern rt_err_t rt_thread_sleep(rt_tick_t tick);

static inline void XR_OS_MSleep(uint32_t msec)
{
	aos_msleep(msec);
}
#define XR_OS_Sleep(sec)           XR_OS_MSleep((sec) * 1000)
#define XR_OS_SSleep(sec)          XR_OS_Sleep(sec)

/**
 * @brief Macros used to compare time values
 *
 *  These inlines deal with timer wrapping correctly. You are
 *  strongly encouraged to use them
 *  1. Because people otherwise forget
 *  2. Because if the timer wrap changes in future you won't have to
 *     alter your code.
 *
 * XR_OS_TimeAfter(a,b) returns true if the time a is after time b.
 *
 * Do this with "<0" and ">=0" to only test the sign of the result. A
 * good compiler would generate better code (and a really good compiler
 * wouldn't care). Gcc is currently neither.
 */
#define XR_OS_TimeAfter(a, b)              ((int32_t)(b) - (int32_t)(a) < 0)
#define XR_OS_TimeBefore(a, b)             XR_OS_TimeAfter(b, a)
#define XR_OS_TimeAfterEqual(a, b)         ((int32_t)(a) - (int32_t)(b) >= 0)
#define XR_OS_TimeBeforeEqual(a, b)        XR_OS_TimeAfterEqual(b, a)

/** @brief Macros used to generate fake random 32-bit value */
/* The fake random 32-bit value is generated by combining OS ticks and
 * the value of rand().
 */
int rand(void);
#define XR_OS_Rand32() ((uint32_t)((rand() & 0xffffff) | (XR_OS_GetTicks() << 24)))

#ifdef __cplusplus
}
#endif

#endif /* _KERNEL_XR_OS_RTTHREAD_XR_OS_TIME_H_ */
