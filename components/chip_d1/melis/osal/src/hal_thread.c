/*
 * ===========================================================================================
 *
 *       Filename:  thread.c
 *
 *    Description:  thread osal for hal layer
 *
 *        Version:  Melis3.0
 *         Create:  2019-11-25 20:29:13
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-03-05 09:01:13
 *
 * ===========================================================================================
 */

#include <stdint.h>
#include <hal_thread.h>

#include <hal_mem.h>
#include <kconfig.h>
#include <log.h>
#include <init.h>

#ifndef CONFIG_KERNEL_NONE

#include <aos/kernel.h>

void *kthread_create(void (*threadfn)(void *data), void *data, const char *namefmt, ...)
{
    aos_task_t *task;
    aos_status_t status;

    task = hal_malloc(sizeof(aos_task_t));
    status = aos_task_create(task, namefmt, threadfn, data, NULL, HAL_THREAD_STACK_SIZE, HAL_THREAD_PRIORITY, AOS_TASK_NONE);
    if (status != 0) {
        return NULL;
    }

    return (void *)task;
}

int kthread_start(void *thread)
{
    if (thread == NULL) {
        return -1;
    }
    aos_status_t status;
    aos_task_t *task = (aos_task_t *)thread;
    status = aos_task_resume(task);
    if (status != 0) {
        return -1;
    }

    return 0;
}

int kthread_stop(void *thread)
{
    if (thread == NULL) {
        return -1;
    }
    aos_status_t status;
    aos_task_t *task = (aos_task_t *)thread;
    aos_task_exit(0);
    status = aos_task_delete(task);
    if (status != 0) {
        return -1;
    }
    hal_free(task);
    return 0;
}

int kthread_wakeup(void *thread)
{
    if (thread == NULL) {
        return -1;
    }
    aos_status_t status;
    aos_task_t *task = (aos_task_t *)thread;
    status = aos_task_resume(task);
    if (status != 0) {
        return -1;
    }

    return 0;
}

int kthread_suspend(void *thread)
{
    if (thread == NULL) {
        return -1;
    }
    aos_status_t status;
    aos_task_t *task = (aos_task_t *)thread;
    status = aos_task_suspend(task);
    if (status != 0) {
        return -1;
    }

    return 0;
}

void *kthread_run(void (*threadfn)(void *data), void *data, const char *namefmt, ...)
{
    aos_task_t *task;
    aos_status_t status;

    task = hal_malloc(sizeof(aos_task_t));
    status = aos_task_create(task, namefmt, threadfn, data, NULL, HAL_THREAD_STACK_SIZE, HAL_THREAD_PRIORITY, AOS_TASK_AUTORUN);
    if (status != 0) {
        return NULL;
    }

    return (void *)task; 
}

#else

typedef void *aos_task_t;

void *kthread_create(void (*threadfn)(void *data), void *data, const char *namefmt, ...)
{
    aos_task_t *task;

    task = hal_malloc(sizeof(aos_task_t));

    return (void *)task;
}

int kthread_start(void *thread)
{
    return 0;
}

int kthread_stop(void *thread)
{
    if (thread == NULL) {
        return -1;
    }
    aos_task_t *task = (aos_task_t *)thread;
    hal_free(task);
    return 0;
}

int kthread_wakeup(void *thread)
{
    return 0;
}

int kthread_suspend(void *thread)
{
    return 0;
}

void *kthread_run(void (*threadfn)(void *data), void *data, const char *namefmt, ...)
{
    aos_task_t *task;

    task = hal_malloc(sizeof(aos_task_t));

    return (void *)task; 
}

#endif