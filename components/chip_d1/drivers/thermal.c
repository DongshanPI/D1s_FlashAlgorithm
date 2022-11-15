/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     thermal.c
 * @brief
 * @version
 * @date     2020-07-06
 ******************************************************************************/

#include <string.h>
#include <drv/common.h>
#include <drv/thermal.h>
#include "sunxi_hal_thermal.h"

int csi_ths_init(void)
{
    int ret = hal_ths_init();
    if (ret != 0)
    {
        return CSI_ERROR;
    }
    return CSI_OK;
}

int csi_ths_uninit(void)
{
    int ret = hal_ths_uninit();
    if (ret != 0)
    {
        return CSI_ERROR;
    }
    return CSI_OK;
}

int csi_ths_get_temp(unsigned int num, int *temp)
{
    CSI_PARAM_CHK(temp, CSI_ERROR);
    int ret = hal_ths_get_temp(num, temp);
    if (ret != 0)
    {
        return CSI_ERROR;
    }
    return CSI_OK;
}
