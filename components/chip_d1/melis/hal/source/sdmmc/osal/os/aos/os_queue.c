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

#include "os_queue.h"
#include "os_util.h"

#ifndef CONFIG_KERNEL_NONE

/**
 * @brief Create and initialize a queue object
 * @param[in] queue Pointer to the queue object
 * @param[in] queueLen The maximum number of items that the queue can hold at
 *                     any one time.
 * @param[in] itemSize The size, in bytes, of each data item that can be stored
 *                     in the queue.
 * @retval OS_Status, OS_OK on success
 */
OS_Status OS_QueueCreate(OS_Queue_t *queue, uint32_t queueLen, uint32_t itemSize)
{	
	queue->buf = malloc(itemSize * queueLen);
	if (!queue->buf) {
		return OS_FAIL;
	}
	int ret = aos_queue_new(&queue->handle, queue->buf, itemSize * queueLen, itemSize);

	if (ret != 0) {
		return OS_FAIL;
	}
	queue->itemSize = itemSize;
	return OS_OK;
}

/**
 * @brief Delete the queue object
 * @param[in] queue Pointer to the queue object
 * @retval OS_Status, OS_OK on success
 */
OS_Status OS_QueueDelete(OS_Queue_t *queue)
{
	aos_queue_free(&queue->handle);
	free(queue->buf);
	OS_QueueSetInvalid(queue);
	return OS_OK;
}

/**
 * @brief Send (write) an item to the back of the queue
 * @param[in] queue Pointer to the queue object
 * @param[in] item Pointer to the data to be copied into the queue.
 *                 The size of each item the queue can hold is set when the
 *                 queue is created, and that many bytes will be copied from
 *                 item into the queue storage area.
 * @param[in] waitMS The maximum amount of time the thread should remain in the
 *                   blocked state to wait for space to become available on the
 *                   queue, should the queue already be full.
 *                   OS_WAIT_FOREVER for waiting forever, zero for no waiting.
 * @retval OS_Status, OS_OK on success
 */
OS_Status OS_QueueSend(OS_Queue_t *queue, const void *item, OS_Time_t waitMS)
{
	int ret;

	if (waitMS == 0) {
		ret = aos_queue_send(&queue->handle, (void *)item, queue->itemSize);
		if (ret != 0) {
			return OS_FAIL;
		}
	} else if (waitMS == OS_WAIT_FOREVER) {
		while (1) {
			ret = aos_queue_send(&queue->handle, (void *)item, queue->itemSize);
			if (ret == 0) {
				break;
			} else {
				aos_msleep(10);
			}
		}
	} else {
		long long endTick = OS_GetTimeMS() + waitMS;
		while (endTick < OS_GetTimeMS()) {
			ret = aos_queue_send(&queue->handle, (void *)item, queue->itemSize);
			if (ret == 0) {
				break;
			} else {
				aos_msleep(10);
			}
		}
	}

	return OS_OK;
}

/**
 * @brief Receive (read) an item from the queue
 * @param[in] queue Pointer to the queue object
 * @param[in] item Pointer to the memory into which the received data will be
 *                 copied. The length of the buffer must be at least equal to
 *                 the queue item size which is set when the queue is created.
 * @param[in] waitMS The maximum amount of time the thread should remain in the
 *                   blocked state to wait for data to become available on the
 *                   queue, should the queue already be empty.
 *                   OS_WAIT_FOREVER for waiting forever, zero for no waiting.
 * @retval OS_Status, OS_OK on success
 */
OS_Status OS_QueueReceive(OS_Queue_t *queue, void *item, OS_Time_t waitMS)
{
	int ret;
	size_t len;

	ret = aos_queue_recv(&queue->handle, waitMS, item, &len);
	if (ret != 0) {
		return OS_FAIL;
	}

	return OS_OK;
}

#else

/**
 * @brief Create and initialize a queue object
 * @param[in] queue Pointer to the queue object
 * @param[in] queueLen The maximum number of items that the queue can hold at
 *                     any one time.
 * @param[in] itemSize The size, in bytes, of each data item that can be stored
 *                     in the queue.
 * @retval OS_Status, OS_OK on success
 */
OS_Status OS_QueueCreate(OS_Queue_t *queue, uint32_t queueLen, uint32_t itemSize)
{
	return OS_OK;
}

/**
 * @brief Delete the queue object
 * @param[in] queue Pointer to the queue object
 * @retval OS_Status, OS_OK on success
 */
OS_Status OS_QueueDelete(OS_Queue_t *queue)
{
	return OS_OK;
}

/**
 * @brief Send (write) an item to the back of the queue
 * @param[in] queue Pointer to the queue object
 * @param[in] item Pointer to the data to be copied into the queue.
 *                 The size of each item the queue can hold is set when the
 *                 queue is created, and that many bytes will be copied from
 *                 item into the queue storage area.
 * @param[in] waitMS The maximum amount of time the thread should remain in the
 *                   blocked state to wait for space to become available on the
 *                   queue, should the queue already be full.
 *                   OS_WAIT_FOREVER for waiting forever, zero for no waiting.
 * @retval OS_Status, OS_OK on success
 */
OS_Status OS_QueueSend(OS_Queue_t *queue, const void *item, OS_Time_t waitMS)
{
	return OS_OK;
}

/**
 * @brief Receive (read) an item from the queue
 * @param[in] queue Pointer to the queue object
 * @param[in] item Pointer to the memory into which the received data will be
 *                 copied. The length of the buffer must be at least equal to
 *                 the queue item size which is set when the queue is created.
 * @param[in] waitMS The maximum amount of time the thread should remain in the
 *                   blocked state to wait for data to become available on the
 *                   queue, should the queue already be empty.
 *                   OS_WAIT_FOREVER for waiting forever, zero for no waiting.
 * @retval OS_Status, OS_OK on success
 */
OS_Status OS_QueueReceive(OS_Queue_t *queue, void *item, OS_Time_t waitMS)
{
	return OS_OK;
}
#endif