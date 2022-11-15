/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

#ifndef __G2D_H__
#define __G2D_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sunxi_g2d_driver.h"

int csi_g2d_init(void);
int csi_g2d_open(void);
int csi_g2d_close(void);
int csi_g2d_ioctl(g2d_cmd cmd, void *arg);

#ifdef __cplusplus
}
#endif

#endif