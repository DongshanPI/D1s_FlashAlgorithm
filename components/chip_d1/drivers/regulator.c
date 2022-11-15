/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     regulator.c
 * @brief
 * @version
 * @date     2020-07-06
 ******************************************************************************/

#include <string.h>
#include <drv/common.h>
#include <drv/regulator.h>
#include "sunxi_hal_regulator.h"

int csi_regulator_get_voltage(regulator_dev_t *rdev, int *vol_uV)
{
    CSI_PARAM_CHK(rdev, CSI_ERROR);
    CSI_PARAM_CHK(vol_uV, CSI_ERROR);
    int ret = hal_regulator_get_voltage(rdev, vol_uV);
    if (ret != 0) {
        return CSI_ERROR;
    }
    return CSI_OK;
}

int csi_regulator_set_voltage(regulator_dev_t *rdev, int target_uV)
{
    CSI_PARAM_CHK(rdev, CSI_ERROR);
    int ret = hal_regulator_set_voltage(rdev, target_uV);
    if (ret != 0) {
        return CSI_ERROR;
    }
    return CSI_OK;
}

int csi_regulator_enable(regulator_dev_t *rdev)
{
    CSI_PARAM_CHK(rdev, CSI_ERROR);
    int ret = hal_regulator_enable(rdev);
    if (ret != 0) {
        return CSI_ERROR;
    }
    return CSI_OK;
}

int csi_regulator_disable(regulator_dev_t *rdev)
{
    CSI_PARAM_CHK(rdev, CSI_ERROR);
    int ret = hal_regulator_disable(rdev);
    if (ret != 0) {
        return CSI_ERROR;
    }
    return CSI_OK;
}

int csi_regulator_get(REGULATOR_TYPE_ENUM_e regulator_type, REGULATOR_ID_ENUM_e regulator_id, regulator_dev_t *rdev)
{
    CSI_PARAM_CHK(rdev, CSI_ERROR);
    // TODO:

    return CSI_OK;
}
