/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     cache.c
 * @brief    source file for setting cache.
 * @version  V1.0
 * @date     9. Oct 2020
 ******************************************************************************/

#include <stdint.h>
#include <drv/porting.h>
#include <csi_core.h>

void soc_dcache_clean_invalid_range(unsigned long addr, uint32_t size)
{
    csi_dcache_clean_invalid_range((unsigned long *)addr, size);
}

void soc_dcache_clean_invalid(void)
{
    csi_dcache_clean_invalid();
}

void soc_dcache_invalid_range(unsigned long addr, uint32_t size)
{
    csi_dcache_invalid_range((unsigned long *)addr, size);
}
