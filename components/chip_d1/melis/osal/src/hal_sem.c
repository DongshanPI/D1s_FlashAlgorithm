/*
 * ===========================================================================================
 *
 *       Filename:  hal_sem.c
 *
 *    Description:  sem impl for hal driver.
 *
 *        Version:  Melis3.0
 *         Create:  2019-11-27 14:20:53
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2019-11-27 17:51:06
 *
 * ===========================================================================================
 */

#include <hal_sem.h>
#include <hal_mem.h>
#include <hal_log.h>

#define SEM_PARAM_CHK(sem)                                     \
    do                                                         \
    {                                                          \
        if (sem == NULL || sem->sem.hdl == NULL)                   \
        {                                                      \
            hal_log_err("fatal error, parameter is invalid."); \
            return -1;                                         \
        }                                                      \
    } while (0)

#ifndef CONFIG_KERNEL_NONE

hal_sem_t hal_sem_create(unsigned int cnt)
{
    aos_status_t status;
    hal_sem_t sem_handle;

    sem_handle = hal_malloc(sizeof(hal_sem_t));
    if (sem_handle == NULL) {
        return NULL;
    }
    status = aos_sem_create(&sem_handle->sem, cnt, 0);
    if (status != 0) {
        hal_log_err("sem create error.");
        hal_free(sem_handle);
        return NULL;
    }
    return sem_handle;
}

int hal_sem_delete(hal_sem_t sem)
{
    SEM_PARAM_CHK(sem);
    aos_sem_free(&sem->sem);
    hal_free(sem);
    return 0;
}

int hal_sem_getvalue(hal_sem_t sem, int *val)
{
    SEM_PARAM_CHK(sem);
    if (val == NULL)
    {
        hal_log_err("fatal error, parameter is invalid.");
        return -1;
    }
    // TODO:

    return 0;
}

int hal_sem_post(hal_sem_t sem)
{
    SEM_PARAM_CHK(sem);
    aos_sem_signal(&sem->sem);
    return 0;
}

int hal_sem_timedwait(hal_sem_t sem, int timeout_ms)
{
    aos_status_t status;

    SEM_PARAM_CHK(sem);
    status = aos_sem_wait(&sem->sem, timeout_ms);
    if (status != 0) {
        hal_log_err("sem wait error, %dms, status:%d", timeout_ms, status);
        return -2;
    }

    return 0;
}

int hal_sem_trywait(hal_sem_t sem)
{
    SEM_PARAM_CHK(sem);
    // TODO:

    return 0;
}

int hal_sem_wait(hal_sem_t sem)
{
    aos_status_t  status;

    SEM_PARAM_CHK(sem);
    status = aos_sem_wait(&sem->sem, AOS_WAIT_FOREVER);
    if (status != 0) {
        hal_log_err("sem wait error");
        return -2;
    }
    return 0;
}

int hal_sem_clear(hal_sem_t sem)
{
    SEM_PARAM_CHK(sem);
    // TODO:

    return 0;
}

#else

hal_sem_t hal_sem_create(unsigned int cnt)
{
    hal_sem_t *sem_handle;

    sem_handle = hal_malloc(sizeof(hal_sem_t));
    if (sem_handle == NULL) {
        return NULL;
    }
    return (hal_sem_t *)sem_handle;
}

int hal_sem_delete(hal_sem_t sem)
{
    if (sem == NULL)
    {
        hal_log_err("fatal error, parameter is invalid.");
        return -1;
    }
    hal_free(sem);
    return 0;
}

int hal_sem_getvalue(hal_sem_t sem, int *val)
{
    return 0;
}

int hal_sem_post(hal_sem_t sem)
{
    return 0;
}

int hal_sem_timedwait(hal_sem_t sem, int timeout_ms)
{
    return 0;
}

int hal_sem_trywait(hal_sem_t sem)
{
    return 0;
}

int hal_sem_wait(hal_sem_t sem)
{
    return 0;
}

int hal_sem_clear(hal_sem_t sem)
{
    return 0;
}
#endif