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

#ifndef _XR_OS_UTIL_H_
#define _XR_OS_UTIL_H_

#include <string.h>
#include <stdlib.h>
//#include "driver/chip/hal_cmsis.h"
#include "kernel/os/os_time.h"
#include "os_debug.h"

#if 0
/* check if in ISR context or not */
static __always_inline int XR_OS_IsISRContext(void)
{
	return __get_IPSR();
}
#endif

static __always_inline long long XR_OS_CalcWaitTicks(XR_OS_Time_t msec)
{
	long long tick;
	if (msec == XR_OS_WAIT_FOREVER) {
		tick = XR_OS_WAIT_FOREVER;
	} else if (msec != 0) {
		tick = XR_OS_MSecsToTicks(msec);
		if (tick == 0)
			tick = 1;
	} else {
		tick = 0;
	}
	return tick;
}

/* memory */
#define XR_OS_Malloc(l)        malloc(l)
#define XR_OS_Free(p)          free(p)
#define XR_OS_Memcpy(d, s, l)  memcpy(d, s, l)
#define XR_OS_Memset(d, c, l)  memset(d, c, l)
#define XR_OS_Memcmp(a, b, l)  memcmp(a, b, l)
#define XR_OS_Memmove(d, s, n) memmove(d, s, n)

#endif /* _XR_OS_UTIL_H_ */
