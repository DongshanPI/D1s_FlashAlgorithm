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

#ifndef _KERNEL_XR_OS_RTTHREAD_XR_OS_COMMON_H_
#define _KERNEL_XR_OS_RTTHREAD_XR_OS_COMMON_H_

#include <stdint.h>
#include <stdio.h>
#include <aos/kernel.h>
// #include "rtthread.h"
// #include "rthw.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum  {
    XR_OS_PRIORITY_IDLE            = 62, //(RT_THREAD_PRIORITY_MAX - 1) - 0,   // 31
    XR_OS_PRIORITY_LOW             = AOS_MAX_APP_PRI, //(RT_THREAD_PRIORITY_MAX - 1) - 1,   // 30
    XR_OS_PRIORITY_BELOW_NORMAL    = AOS_DEFAULT_APP_PRI+1, //(RT_THREAD_PRIORITY_MAX - 1) - 2,   // 29
    XR_OS_PRIORITY_NORMAL          = AOS_DEFAULT_APP_PRI, //(RT_THREAD_PRIORITY_MAX) / (3),     // 10
    XR_OS_PRIORITY_ABOVE_NORMAL    = AOS_DEFAULT_APP_PRI-1, //(RT_THREAD_PRIORITY_MAX + 4) / (4), // 9
    XR_OS_PRIORITY_HIGH            =  8, //(RT_THREAD_PRIORITY_MAX) / (5),     // 6
    XR_OS_PRIORITY_REAL_TIME       =  2                                        // 1
} XR_OS_Priority;

/**
 * @brief OS status definition
 */
typedef enum {
    XR_OS_OK           = 0,    /* success */
    XR_OS_FAIL         = -1,   /* general failure */
    XR_OS_E_NOMEM      = -2,   /* out of memory */
    XR_OS_E_PARAM      = -3,   /* invalid parameter */
    XR_OS_E_TIMEOUT    = -4,   /* operation timeout */
    XR_OS_E_ISR        = -5,   /* not allowed in ISR context */
} XR_OS_Status;

/** @brief Type definition of OS time */
typedef long long XR_OS_Time_t;

#define XR_OS_WAIT_FOREVER         AOS_WAIT_FOREVER /* Wait forever timeout value */
#define XR_OS_SEMAPHORE_MAX_COUNT  0xffffffffU /* Maximum count value for semaphore */
#define XR_OS_INVALID_HANDLE       NULL        /* OS invalid handle */

#ifdef __cplusplus
}
#endif

#endif /* _KERNEL_XR_OS_RTTHREAD_XR_OS_COMMON_H_ */
