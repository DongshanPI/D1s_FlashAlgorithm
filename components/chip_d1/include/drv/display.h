/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sunxi_display2.h"

typedef enum tag_DISP_CMD disp_cmd_e;
typedef struct disp_color disp_color_t;
typedef struct disp_layer_config disp_layer_config_t;
typedef struct disp_layer_config2 disp_layer_config2_t;

int csi_display_init(void);
int csi_display_ioctl(disp_cmd_e cmd, void *arg);

#ifdef __cplusplus
}
#endif

#endif