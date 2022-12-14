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

#include "kernel/os/os_timer.h"
#include "os_util.h"


/**
 * @brief Create and initialize a timer object
 *
 * @note Creating a timer does not start the timer running. The XR_OS_TimerStart()
 *       and XR_OS_TimerChangePeriod() API functions can all be used to start the
 *       timer running.
 *
 * @param[in] timer Pointer to the timer object
 * @param[in] type Timer type
 * @param[in] cb Timer expire callback function
 * @param[in] arg Argument of Timer expire callback function
 * @param[in] periodMS Timer period in milliseconds
 * @retval XR_OS_Status, XR_OS_OK on success
 */
static void aos_timer_cb(void *tmr, void *arg)
{
	XR_OS_Timer_t *timer = (XR_OS_Timer_t *)arg;
	if (timer && timer->cb) {
		timer->cb(timer->arg);
	}
}

XR_OS_Status XR_OS_TimerCreate(XR_OS_Timer_t *timer, XR_OS_TimerType type,
                         XR_OS_TimerCallback_t cb, void *arg, XR_OS_Time_t periodMS)
{
	int flag = 1;

	if (type == XR_OS_TIMER_PERIODIC) {
		flag = 1;
	} else {
		flag = 0;
	}
	
	if (periodMS == AOS_WAIT_FOREVER) {
		periodMS = 1000;
	}

	int ret = aos_timer_new(&timer->handle, aos_timer_cb, timer, periodMS, flag);
	if (ret != 0) {
		return XR_OS_FAIL;
	}

	timer->cb = cb;
	timer->arg = arg;

	return XR_OS_OK;
}

XR_OS_Status XR_OS_TimerDelete(XR_OS_Timer_t *timer)
{
	aos_timer_free(&timer->handle);
	XR_OS_TimerSetInvalid(timer);
	return XR_OS_OK;
}

/**
 * @brief Start a timer running.
 * @note If the timer is already running, this function will re-start the timer.
 * @param[in] timer Pointer to the timer object
 * @retval XR_OS_Status, XR_OS_OK on success
 */
XR_OS_Status XR_OS_TimerStart(XR_OS_Timer_t *timer)
{
	int ret = aos_timer_start(&timer->handle);
	if (ret != 0) {
		//XR_OS_ERR("err %"XR_OS_BASETYPE_F"\n", ret);
		return XR_OS_FAIL;
	}

	return XR_OS_OK;
}

/**
 * @brief Change the period of a timer
 *
 * If XR_OS_TimerChangePeriod() is used to change the period of a timer that is
 * already running, then the timer will use the new period value to recalculate
 * its expiry time. The recalculated expiry time will then be relative to when
 * XR_OS_TimerChangePeriod() was called, and not relative to when the timer was
 * originally started.

 * If XR_OS_TimerChangePeriod() is used to change the period of a timer that is
 * not already running, then the timer will use the new period value to
 * calculate an expiry time, and the timer will start running.
 *
 * @param[in] timer Pointer to the timer object
 * @retval XR_OS_Status, XR_OS_OK on success
 */
XR_OS_Status XR_OS_TimerChangePeriod(XR_OS_Timer_t *timer, XR_OS_Time_t periodMS)
{
	int ret;

	if (aos_timer_is_valid(&timer->handle)) {
		ret = aos_timer_stop(&timer->handle);
		if (ret != 0) {
			//XR_OS_ERR("err %"XR_OS_BASETYPE_F"\n", ret);
			return XR_OS_FAIL;
		}
	}

	ret = aos_timer_change_once(&timer->handle, aos_kernel_tick2ms(periodMS));
	if (ret != 0) {
		//XR_OS_ERR("err %"XR_OS_BASETYPE_F"\n", ret);
		return XR_OS_FAIL;
	}
	ret = aos_timer_start(&timer->handle);
	if (ret != 0) {
		//XR_OS_ERR("err %"XR_OS_BASETYPE_F"\n", ret);
		return XR_OS_FAIL;
	}

	return XR_OS_OK;
}

/**
 * @brief Stop a timer running.
 * @param[in] timer Pointer to the timer object
 * @retval XR_OS_Status, XR_OS_OK on success
 */
XR_OS_Status XR_OS_TimerStop(XR_OS_Timer_t *timer)
{
	if (!aos_timer_is_valid(&timer->handle)) {
		return XR_OS_OK;
	}

	int ret = aos_timer_stop(&timer->handle);
	if (ret != 0) {
		// //XR_OS_ERR("err %"XR_OS_BASETYPE_F"\n", ret);
		return XR_OS_FAIL;
	}

	return XR_OS_OK;
}
