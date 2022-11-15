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

#include "kernel/os/os_mutex.h"
#include "os_util.h"


/**
 * @brief Create and initialize a mutex object
 * @note A mutex can only be locked by a single thread at any given time.
 * @param[in] mutex Pointer to the mutex object
 * @retval XR_OS_Status, XR_OS_OK on success
 */
XR_OS_Status XR_OS_MutexCreate(XR_OS_Mutex_t *mutex)
{
	int ret = aos_mutex_new(&mutex->handle);

	if (ret != 0) {
		XR_OS_ERR("err %"XR_OS_HANDLE_F"\n", &mutex->handle);
		return XR_OS_FAIL;
	}

	return XR_OS_OK;
}

/**
 * @brief Delete the mutex object
 * @param[in] mutex Pointer to the mutex object
 * @retval XR_OS_Status, XR_OS_OK on success
 */
XR_OS_Status XR_OS_MutexDelete(XR_OS_Mutex_t *mutex)
{
	aos_mutex_free(&mutex->handle);
	XR_OS_MutexSetInvalid(mutex);
	return XR_OS_OK;
}

/**
 * @brief Lock the mutex object
 * @note A mutex can only be locked by a single thread at any given time. If
 *       the mutex is already locked, the caller will be blocked for the
 *       specified time duration.
 * @param[in] mutex Pointer to the mutex object
 * @param[in] waitMS The maximum amount of time (in millisecond) the thread
 *                   should remain in the blocked state to wait for the mutex
 *                   to become unlocked.
 *                   XR_OS_WAIT_FOREVER for waiting forever, zero for no waiting.
 * @retval XR_OS_Status, XR_OS_OK on success
 */
XR_OS_Status XR_OS_MutexLock(XR_OS_Mutex_t *mutex, XR_OS_Time_t waitMS)
{
	int ret = aos_mutex_lock(&mutex->handle, waitMS);
	if (ret != 0) {
		//XR_OS_DBG("%s() fail @ %d, %"XR_OS_TIME_F" ms\n", __func__, __LINE__, waitMS);
		return XR_OS_FAIL;
	}

	return XR_OS_OK;
}

/**
 * @brief Unlock the mutex object previously locked using XR_OS_MutexLock()
 * @note The mutex should be unlocked from the same thread context from which
 *       it was locked.
 * @param[in] mutex Pointer to the mutex object
 * @retval XR_OS_Status, XR_OS_OK on success
 */
XR_OS_Status XR_OS_MutexUnlock(XR_OS_Mutex_t *mutex)
{
	int ret = aos_mutex_unlock(&mutex->handle);
	if (ret != 0) {
		//XR_OS_DBG("%s() fail @ %d\n", __func__, __LINE__);
		return XR_OS_FAIL;
	}

	return XR_OS_OK;
}
