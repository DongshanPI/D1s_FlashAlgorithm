/*
 * ===========================================================================================
 *
 *       Filename:  hal_mailbox.c
 *
 *    Description:  mbox impl.for hal.
 *
 *        Version:  Melis3.0
 *         Create:  2019-11-27 17:32:09
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2019-11-27 18:02:12
 *
 * ===========================================================================================
 */

#include <hal_queue.h>
#include <hal_mem.h>
#include <log.h>
#include <hal_log.h>

#define MAILBOX_PARAM_CHK(mailbox)                             \
    do                                                         \
    {                                                          \
        if (mailbox == NULL || mailbox->queue.hdl == NULL)         \
        {                                                      \
            hal_log_err("fatal error, parameter is invalid."); \
            return -1;                                         \
        }                                                      \
    } while (0)

/* queue resource */
#define MESSAGE_MAX_LENGTH 10 /* maximum message length */

#ifndef CONFIG_KERNEL_NONE

hal_mailbox_t hal_mailbox_create(const char *name, unsigned int size)
{
    aos_status_t status;
    hal_mailbox_t mailbox;

    (void)name;

    mailbox = hal_malloc(sizeof(hal_mailbox_t));
    if (mailbox == NULL) {
        __err("fatal error, mem alloc failed.");
        return NULL;
    }

    status = aos_queue_create(&mailbox->queue, size, MESSAGE_MAX_LENGTH, 0);
    if (status != 0) {
        __err("fatal error, queue create failed.");
        hal_free(mailbox);
        return NULL;
    }
    return mailbox;
}

int hal_mailbox_delete(hal_mailbox_t mailbox)
{
    MAILBOX_PARAM_CHK(mailbox);
    aos_queue_free(&mailbox->queue);
    hal_free(mailbox);
    return 0;
}

int hal_mailbox_send(hal_mailbox_t mailbox, unsigned int value)
{
    aos_status_t status;

    MAILBOX_PARAM_CHK(mailbox);
    status = aos_queue_send(&mailbox->queue, &value, sizeof(unsigned int));
    if (status != 0)
    {
        // timeout.
        return -2;
    }

    return 0;
}

int hal_mailbox_send_wait(hal_mailbox_t mailbox, unsigned int value, int timeout)
{
    MAILBOX_PARAM_CHK(mailbox);
    // TODO:

    return 0;
}

int hal_mailbox_recv(hal_mailbox_t mailbox, unsigned int *value, int timeout)
{
    size_t rev_size = 0;
    aos_status_t status;

    MAILBOX_PARAM_CHK(mailbox);
    if (value == NULL)
    {
        __err("fatal error, parameter is invalid.");
        return -1;
    }
    status = aos_queue_recv(&mailbox->queue, timeout, value, &rev_size);
    if (status != 0)
    {
        // timeout.
        return -2;
    }

    return 0;
}

int hal_is_mailbox_empty(hal_mailbox_t mailbox)
{
    MAILBOX_PARAM_CHK(mailbox);
    // TODO:

    return 0;
}

void hal_work_init(hal_work *work, void (*work_func)(void *work_data), void *work_data)
{
    int ret;
    ret = aos_work_init(work, work_func, work_data, 0);
    if (ret != 0) {
        __err("fatal error, work init failed.");
    }
}

hal_workqueue *hal_workqueue_create(const char *name, unsigned short stack_size, unsigned char priority)
{
    int ret;
    aos_workqueue_t *workqueue;

    workqueue = hal_malloc(sizeof(aos_workqueue_t));
    if (workqueue == NULL) {
        __err("fatal error, parameter is invalid.");
        return NULL;
    }
    ret = aos_workqueue_create_ext(workqueue, name, priority, stack_size);
    if (ret != 0) {
        __err("fatal error, workqueue create failed.");
        return NULL;
    }
    return workqueue;
}

int hal_workqueue_dowork(hal_workqueue *queue, hal_work *work)
{
	int ret = 0;
	ret = aos_work_run(queue, work);
	return ret;
}

#else

hal_mailbox_t hal_mailbox_create(const char *name, unsigned int size)
{
    hal_mailbox_t *queue;

    (void)name;

    queue = (hal_mailbox_t *)hal_malloc(sizeof(hal_mailbox_t));
    if (queue == NULL) {
        __err("fatal error, mem alloc failed.");
        return NULL;
    }
    return (hal_mailbox_t)queue;
}

int hal_mailbox_delete(hal_mailbox_t mailbox)
{
    if (mailbox == NULL)
    {
        __err("fatal error, parameter is invalid.");
        return -1;
    }
    hal_free(mailbox);
    return 0;
}

int hal_mailbox_send(hal_mailbox_t mailbox, unsigned int value)
{
    return 0;
}

int hal_mailbox_send_wait(hal_mailbox_t mailbox, unsigned int value, int timeout)
{
    return 0;
}

int hal_mailbox_recv(hal_mailbox_t mailbox, unsigned int *value, int timeout)
{
    return 0;
}

int hal_is_mailbox_empty(hal_mailbox_t mailbox)
{
    return 0;
}

void hal_work_init(hal_work *work, void (*work_func)(void *work_data), void *work_data)
{
}

hal_workqueue *hal_workqueue_create(const char *name, unsigned short stack_size, unsigned char priority)
{
    hal_workqueue *workqueue;

    workqueue = hal_malloc(sizeof(hal_workqueue));
    if (workqueue == NULL) {
        __err("fatal error, parameter is invalid.");
        return NULL;
    }
    return workqueue;
}

int hal_workqueue_dowork(hal_workqueue *queue, hal_work *work)
{
	int ret = 0;
	return ret;
}

#endif