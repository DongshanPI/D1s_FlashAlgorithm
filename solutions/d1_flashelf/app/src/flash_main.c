/*******************************************************************************
 * Copyright (c) 2012-2019 Hangzhou C-SKY Microsystems Co., Ltd.
 *
 * All rights reserved. This software is proprietary and confidential to
 * Hangzhou C-SKY Microsystems Co., Ltd. and its licensors.
 *
 * Contributors:
 *     Hangzhou C-SKY Microsystems Co., Ltd.
 *
 * 2009.12.1   Jing Qing(qing_jing@c-sky.com)
 * 2014.06.30  Fan Rundong(rundong_fan@c-sky.com)
 * 2019.6.18   Jiang Long(long_jiang@c-sky.com)
 *     Initial API and implementation
 *******************************************************************************/

/*
 * NOTING : this file read only.
 */

/*
 * set the version of flash driver
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define VERS      2
unsigned int fd_version = 0x0000 + VERS;

int flashUnInit(void);
int flashInit(void);
int flashID(unsigned int* flashID);
int flashProgram(uint32_t dst, char *src, int length);
int flashRead(char *dst, uint32_t src, int length);
int flashErase(uint32_t dst, int length);
int flashChipErase(void);
int flashChecksum(uint32_t dst, int length, int checksum);
int flashTest(void);

void __bkpt_label(void);
char g_rwBuffer[4096 * 2];
uint32_t g_dstAddress = 0;
uint32_t g_length = 0;
uint32_t g_checksum = 0;
__attribute__((section (".data"))) int g_runmode = 0;

/**
 * The definition of functional options,
 * 0 corresponds to check id,1 corresponds
 * to program,2 corresponds to read,3 corresponds
 * to erase,4 corresponds to chip erase.
 */
int g_func = 0;
unsigned int g_flashID = 0;

/**
 * The engineering-driven operation error return
 */
int g_error = 0;

int flash_handle(void) {
    // call nor flash drivers to program
    g_error = flashInit();
    // for debug flash driver
    // flashTest();

    do {
        __bkpt_label();
        switch (g_func) {
        case 0:
            g_error = flashID(&g_flashID);
            break;
        case 1:
            g_error = flashProgram((uint32_t) g_dstAddress, g_rwBuffer, g_length);
            break;
        case 2:
            g_error = flashRead(g_rwBuffer, (uint32_t) g_dstAddress, g_length);
            break;
        case 3:
            g_error = flashErase((uint32_t) g_dstAddress, g_length);
            break;
        case 4:
            g_error = flashChipErase();
            break;
        case 5:
            g_error = flashUnInit();
            break;
        case 6:
            g_error = flashChecksum((uint32_t)g_dstAddress, g_length, g_checksum);
            break;
        default:
            break;
        }
    } while (1);

}
