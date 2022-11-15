/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include "app_config.h"
#include "app_main.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <soc.h>
#include <drv/tick.h>

void boot_sys_reboot(void)
{
    extern void drv_reboot(void);
    drv_reboot();
}

int main(int argc, char *argv[])
{
    extern void board_yoc_init(void);
    board_yoc_init();

	printf("come to flash elf...\r\n");
    extern int flash_handle(void);
    flash_handle();
    csi_tick_uninit();
    while(1);
    return 0;
}

//////////////////////////////////////////////////////////////
#include <sys/stat.h>
int _unlink(const char *path)
{
    (void)path;
    return 0;
}

int stat(const char *path, struct stat *buf)
{
    (void)path;
    (void)buf;
    return 0;
}