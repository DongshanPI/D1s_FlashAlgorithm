/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     mbus.c
 * @brief
 * @version
 * @date     2020-07-05
 ******************************************************************************/

#include <string.h>
#include <drv/mbus.h>


mbus_status_t csi_mbus_pmu_get_value(enum mbus_pmu type, unsigned int *value)
{
    return hal_mbus_pmu_get_value(type, value);
}

mbus_status_t csi_mbus_pmu_enable(void)
{
    return hal_mbus_pmu_enable();
}
