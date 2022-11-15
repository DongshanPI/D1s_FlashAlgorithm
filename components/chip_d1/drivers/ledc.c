/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ledc.c
 * @brief
 * @version
 * @date     2020-06-24
 ******************************************************************************/

#include <string.h>
#include <drv/ledc.h>
#include "sunxi_hal_ledc.h"


void csi_ledc_init(void)
{
    hal_ledc_init();
}

void csi_ledc_deinit(void)
{
    hal_ledc_deinit();
}

void csi_ledc_trans_data(struct ledc_config *ledc)
{
    hal_ledc_trans_data(ledc);
}

void csi_ledc_clear_all_irq(void)
{
    hal_ledc_clear_all_irq();
}

unsigned int csi_ledc_get_irq_status(void)
{
    return hal_ledc_get_irq_status();
}

void csi_ledc_dma_callback(void *para)
{
    hal_ledc_dma_callback(para);
}

void csi_ledc_reset(void)
{
    hal_ledc_reset();
}
