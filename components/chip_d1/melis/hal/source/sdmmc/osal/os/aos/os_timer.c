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

#include "os_timer.h"
#include "os_util.h"

#ifndef CONFIG_KERNEL_NONE
/**
 * @brief Create and initialize a timer object
 *
 * @note Creating a timer does not start the timer running. The OS_TimerStart()
 *       and OS_TimerChangePeriod() API functions can all be used to start the
 *       timer running.
 *
 * @param[in] timer Pointer to the timer object
 * @param[in] type Timer type
 * @param[in] cb Timer expire callback function
 * @param[in] arg Argument of Timer expire callback function
 * @param[in] periodMS Timer period in milliseconds
 * @retval OS_Status, OS_OK on success
 */

static void aos_timer_cb(void *tmr, void *arg)
{
	OS_Timer_t *timer = (OS_Timer_t *)arg;
	if (timer && timer->cb) {
		timer->cb(timer->arg);
	}
}

OS_Status OS_TimerCreate(OS_Timer_t *timer, OS_TimerType type,
                         OS_TimerCallback_t cb, void *arg, OS_Time_t periodMS)
{
	int flag;

	flag = 1;
	if (type == OS_TIMER_PERIODIC) {
		flag = 1;
	} else {
		flag = 0;
	}

	if (periodMS == AOS_WAIT_FOREVER) {
		periodMS = 1000;
	}

	int ret = aos_timer_new(&timer->handle, aos_timer_cb, timer, periodMS, flag);
	if (ret != 0) {
		return OS_FAIL;
	}

	timer->cb = cb;
	timer->arg = arg;
	return OS_OK;
}

OS_Status OS_TimerDelete(OS_Timer_t *timer)
{
	aos_timer_free(&timer->handle);
	OS_TimerSetInvalid(timer);
	return OS_OK;
}

/**
 * @brief Start a timer running.
 * @note If the timer is already running, this function will re-start the timer.
 * @param[in] timer Pointer to the timer object
 * @retval OS_Status, OS_OK on success
 */
OS_Status OS_TimerStart(OS_Timer_t *timer)
{
	int ret;

	ret = aos_timer_start(&timer->handle);
	if (ret != 0) {
		//OS_ERR("err %"OS_BASETYPE_F"\n", ret);
		return OS_FAIL;
	}

	return OS_OK;
}

/**
 * @brief Change the period of a timer
 *
 * If OS_TimerChangePeriod() is used to change the period of a timer that is
 * already running, then the timer will use the new period value to recalculate
 * its expiry time. The recalculated expiry time will then be relative to when
 * OS_TimerChangePeriod() was called, and not relative to when the timer was
 * originally started.

 * If OS_TimerChangePeriod() is used to change the period of a timer that is
 * not already running, then the timer will use the new period value to
 * calculate an expiry time, and the timer will start running.
 *
 * @param[in] timer Pointer to the timer object
 * @retval OS_Status, OS_OK on success
 */
OS_Status OS_TimerChangePeriod(OS_Timer_t *timer, OS_Time_t periodMS)
{
	int ret;

	if (aos_timer_is_valid(&timer->handle)) {
		ret = aos_timer_stop(&timer->handle);
		if (ret != 0) {
			//OS_ERR("err %"OS_BASETYPE_F"\n", ret);
			return OS_FAIL;
		}
	}
	ret = aos_timer_change_once(&timer->handle, aos_kernel_tick2ms(periodMS));
	if (ret != 0) {
		//OS_ERR("err %"OS_BASETYPE_F"\n", ret);
		return OS_FAIL;
	}
	ret = aos_timer_start(&timer->handle);
	if (ret != 0) {
		//OS_ERR("err %"OS_BASETYPE_F"\n", ret);
		return OS_FAIL;
	}

	return OS_OK;
}

/**
 * @brief Stop a timer running.
 * @param[in] timer Pointer to the timer object
 * @retval OS_Status, OS_OK on success
 */
OS_Status OS_TimerStop(OS_Timer_t *timer)
{
	int ret;

	if (!aos_timer_is_valid(&timer->handle)) {
		return OS_OK;
	}

	ret = aos_timer_stop(&timer->handle);
	if (ret != 0) {
		//OS_ERR("err %"OS_BASETYPE_F"\n", ret);
		return OS_FAIL;
	}

	return OS_OK;
}
#else
OS_Status OS_TimerCreate(OS_Timer_t *timer, OS_TimerType type,
                         OS_TimerCallback_t cb, void *arg, OS_Time_t periodMS)
{
	return OS_OK;
}

OS_Status OS_TimerDelete(OS_Timer_t *timer)
{
	return OS_OK;
}

/**
 * @brief Start a timer running.
 * @note If the timer is already running, this function will re-start the timer.
 * @param[in] timer Pointer to the timer object
 * @retval OS_Status, OS_OK on success
 */
OS_Status OS_TimerStart(OS_Timer_t *timer)
{
	return OS_OK;
}

/**
 * @brief Change the period of a timer
 *
 * If OS_TimerChangePeriod() is used to change the period of a timer that is
 * already running, then the timer will use the new period value to recalculate
 * its expiry time. The recalculated expiry time will then be relative to when
 * OS_TimerChangePeriod() was called, and not relative to when the timer was
 * originally started.

 * If OS_TimerChangePeriod() is used to change the period of a timer that is
 * not already running, then the timer will use the new period value to
 * calculate an expiry time, and the timer will start running.
 *
 * @param[in] timer Pointer to the timer object
 * @retval OS_Status, OS_OK on success
 */
OS_Status OS_TimerChangePeriod(OS_Timer_t *timer, OS_Time_t periodMS)
{
	return OS_OK;
}

/**
 * @brief Stop a timer running.
 * @param[in] timer Pointer to the timer object
 * @retval OS_Status, OS_OK on success
 */
OS_Status OS_TimerStop(OS_Timer_t *timer)
{
	return OS_OK;
}
#endif