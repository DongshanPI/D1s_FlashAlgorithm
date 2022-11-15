#include <stdlib.h>
#include <hal_mutex.h>
#include <hal_mem.h>
#include <hal_log.h>

#define MUTEX_PARAM_CHK(mutex)                                 \
    do                                                         \
    {                                                          \
        if (mutex == NULL || mutex->mtx.hdl == NULL)               \
        {                                                      \
            hal_log_err("fatal error, parameter is invalid."); \
            return -1;                                         \
        }                                                      \
    } while (0)

#ifndef CONFIG_KERNEL_NONE

hal_mutex_t hal_mutex_create(void)
{
    aos_status_t status;
    hal_mutex_t mutex = hal_malloc(sizeof(hal_mutex_t));
    if (mutex == NULL) {
        return NULL;
    }
    status = aos_mutex_new(&mutex->mtx);
    if (status != 0) {
        hal_free(mutex);
        return NULL;
    }
    return mutex;
}

int hal_mutex_delete(hal_mutex_t mutex)
{
    MUTEX_PARAM_CHK(mutex);
    aos_mutex_free(&mutex->mtx);
    hal_free(mutex);
    return 0;
}

int hal_mutex_lock(hal_mutex_t mutex)
{
    MUTEX_PARAM_CHK(mutex);
    aos_status_t status = aos_mutex_lock(&mutex->mtx, -1);
    if (status != 0) {
        return -1;
    }
    return 0;
}

int hal_mutex_unlock(hal_mutex_t mutex)
{
    MUTEX_PARAM_CHK(mutex);
    aos_status_t status = aos_mutex_unlock(&mutex->mtx);
    if (status != 0) {
        return -1;
    }
    return 0;
}

int hal_mutex_trylock(hal_mutex_t mutex)
{
    MUTEX_PARAM_CHK(mutex);
    // TODO:
    return 0;
}

int hal_mutex_timedwait(hal_mutex_t mutex, uint32_t timeout_ms)
{
    MUTEX_PARAM_CHK(mutex);
    aos_status_t status = aos_mutex_lock(&mutex->mtx, timeout_ms);
    if (status != 0) {
        return -1;
    }
    return 0;
}

#else

hal_mutex_t hal_mutex_create(void)
{
    hal_mutex_t *mutex = hal_malloc(sizeof(hal_mutex_t));
    if (mutex == NULL) {
        return NULL;
    }
    return mutex;
}

int hal_mutex_delete(hal_mutex_t mutex)
{
    if (!mutex)
    {
        return -1;
    }
    hal_free(mutex);
    return 0;
}

int hal_mutex_lock(hal_mutex_t mutex)
{
    return 0;
}

int hal_mutex_unlock(hal_mutex_t mutex)
{
    return 0;
}

int hal_mutex_trylock(hal_mutex_t mutex)
{
    return 0;
}

int hal_mutex_timedwait(hal_mutex_t mutex, uint32_t timeout_ms)
{
    return 0;
}

#endif