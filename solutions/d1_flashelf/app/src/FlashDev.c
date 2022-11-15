/*******************************************************************************
 * Copyright (c) 2012-2019 Hangzhou C-SKY Microsystems Co., Ltd.
 * 
 * All rights reserved. This software is proprietary and confidential to
 * Hangzhou C-SKY Microsystems Co., Ltd. and its licensors.
 *
 * Contributors:
 *     Hangzhou C-SKY Microsystems Co., Ltd.
 *
 * 2019.6.18   Jiang Long(long_jiang@c-sky.com)
 *     Initial API and implementation
 *******************************************************************************/
#include "FlashOS.h"

/**
 * structure to describe flash device
 */
struct FlashDevice const FlashDevices  INDEVSECTION =  {
    6,                      // Reserved version description, do not modify!!!
    "P25Q64H",              // Flash name
    "c906",              // CPU name, must in low case
    0x176085,               // Flash ID
    "NorFlash",             // type
    8*1024*1024,               // 8M
    0,                      // Access directly
    1,                      // RangeNumbers
	// {start address, the flash size, sector size}
    {{0x0, 0x800000, 0x1000}}
};
