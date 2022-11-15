/*
 * ===========================================================================================
 *
 *       Filename:  hal_timer.c
 *
 *    Description:  osal timer glue layer for hal.
 *
 *        Version:  Melis3.0
 *         Create:  2019-11-26 20:21:27
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2019-12-17 18:10:51
 *
 * ===========================================================================================
 */

#include <hal_timer.h>
#include <hal_mem.h>
#include <hal_log.h>

#define TIMER_PARAM_CHK(timer)                                 \
    do                                                         \
    {                                                          \
        if (timer == NULL || timer->timer.hdl == NULL)             \
        {                                                      \
            hal_log_err("fatal error, parameter is invalid."); \
            return -1;                                         \
        }                                                      \
    } while (0)

#ifndef CONFIG_KERNEL_NONE

#include <aos/list.h>

osal_timer_t osal_timer_create(const char *name,
                               timeout_func timer_callback,
                               void *parameter,
                               unsigned int timeout_ms,
                               unsigned char flag)
{
    osal_timer_t timer;
    int status;

    timer = hal_malloc(sizeof(osal_timer_t));
    if (timer == NULL) {
        return NULL;
    }
    status = aos_timer_new(&timer->timer, timer_callback, parameter, timeout_ms, flag);
    if (status != 0) {
        hal_free(timer);
        return NULL;
    }

    return timer;
}

int osal_timer_delete(osal_timer_t timer)
{
    TIMER_PARAM_CHK(timer);
    aos_timer_free(&timer->timer);
    hal_free(timer);
    return 0;
}

int osal_timer_start(osal_timer_t timer)
{
    TIMER_PARAM_CHK(timer);
    aos_status_t status = aos_timer_start(&timer->timer);
    if (status != 0) {
        return -1;
    }
    return 0;
}

int osal_timer_stop(osal_timer_t timer)
{
    TIMER_PARAM_CHK(timer);
    aos_status_t status = aos_timer_stop(&timer->timer);
    if (status != 0) {
        return -1;
    }
    return 0;
}

int osal_timer_control(osal_timer_t timer, int cmd, void *arg)
{
    TIMER_PARAM_CHK(timer);
    if (cmd == OSAL_TIMER_CTRL_SET_TIME) {
        uint32_t time_interval = *((uint32_t *)arg);
        /* stop the timer before modifying the timer parameter */
        aos_timer_stop(&timer->timer);
        /* the timer cycle is modified */
        aos_timer_change(&timer->timer, time_interval);
        /* start the timer after the timer parameter modification is complete */
        aos_timer_start(&timer->timer);
        return 0;
    }
    return -1;
}

extern void sleep(int seconds);
extern void aos_msleep(int ms);
extern int usleep(unsigned int usecs);

int hal_sleep(unsigned int secs)
{
    sleep(secs);
    return 0;
}

int hal_msleep(unsigned int msecs)
{
    aos_msleep(msecs);
    return 0;
}

int hal_usleep(unsigned int usecs)
{
    return usleep(usecs);
}

#else

osal_timer_t osal_timer_create(const char *name,
                               timeout_func timer_callback,
                               void *parameter,
                               unsigned int timeout_ms,
                               unsigned char flag)
{
    osal_timer_t timer;

    timer = hal_malloc(sizeof(osal_timer_t));
    if (timer == NULL) {
        return NULL;
    }

    return timer;
}

int osal_timer_delete(osal_timer_t timer)
{
    if (timer == NULL) {
        return -1;
    }
    hal_free(timer);
    return 0;
}

int osal_timer_start(osal_timer_t timer)
{
    return 0;
}

int osal_timer_stop(osal_timer_t timer)
{
    return 0;
}

int osal_timer_control(osal_timer_t timer, int cmd, void *arg)
{
    return 0;
}

extern void mdelay(uint32_t ms);
extern void udelay(uint32_t us);

int hal_sleep(unsigned int secs)
{
    mdelay(secs * 1000);
    return 0;
}

int hal_msleep(unsigned int msecs)
{
    mdelay(msecs);
    return 0;
}

int hal_usleep(unsigned int usecs)
{
    udelay(usecs);
    return 0;
}

#endif
