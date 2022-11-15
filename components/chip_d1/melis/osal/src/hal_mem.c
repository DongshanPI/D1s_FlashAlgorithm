/*
 * ===========================================================================================
 *
 *       Filename:  hal_sem.c
 *
 *    Description:  memory allocator impl for hal driver.
 *
 *        Version:  Melis3.0
 *         Create:  2019-11-27 14:20:53
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-07-28 08:49:06
 *
 * ===========================================================================================
 */

#include <hal_mem.h>
#include <log.h>
#ifndef CONFIG_KERNEL_NONE
#include <aos/kernel.h>
#else
#define aos_malloc malloc
#define aos_free free
#endif

void *hal_malloc(uint32_t size)
{
    return aos_malloc(size);
}

void hal_free(void *p)
{
    aos_free(p);
}

void *hal_malloc_align(uint32_t size, int align)
{
    void *ptr;
    void *align_ptr;
    int uintptr_size;
    unsigned long align_size;

    /* sizeof pointer */
    uintptr_size = sizeof(void *);
    uintptr_size -= 1;

    /* align the alignment size to uintptr size byte */
    align = ((align + uintptr_size) & ~uintptr_size);

    /* get total aligned size */
    align_size = ((size + uintptr_size) & ~uintptr_size) + align;
    /* allocate memory block from heap */
    ptr = aos_malloc(align_size);
    if (ptr != NULL)
    {
        /* the allocated memory block is aligned */
        if (((unsigned long)ptr & (align - 1)) == 0)
        {
            align_ptr = (void *)((unsigned long)ptr + align);
        }
        else
        {
            align_ptr = (void *)(((unsigned long)ptr + (align - 1)) & ~(align - 1));
        }

        /* set the pointer before alignment pointer to the real pointer */
        *((unsigned long *)((unsigned long)align_ptr - sizeof(void *))) = (unsigned long)ptr;

        ptr = align_ptr;
    }

    return ptr;
}

void hal_free_align(void *p)
{
    void *real_ptr;

    real_ptr = (void *) * (unsigned long *)((unsigned long)p - sizeof(void *));
    aos_free(real_ptr);
}

