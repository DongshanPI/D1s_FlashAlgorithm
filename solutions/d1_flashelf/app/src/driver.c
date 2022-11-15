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

/**
 * Driver for flash program.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
// #include <sunxi_hal_spinor.h>
#include <drv/spiflash.h>

static csi_spiflash_t spiflash_hd;
static csi_spiflash_info_t spiflash_info;


/**
 * ERROR TYPE. MUST NOT BE MODIFIED
 */
#define ERROR_INIT      -200
#define ERROR_READID    -201
#define ERROR_PROGRAM   -202
#define ERROR_READ      -203
#define ERROR_ERASE     -204
#define ERROR_CHIPERASE -205
#define ERROR_UNINIT	-206
#define ERROR_CHECKSUM  -207

// #define FLASH_DEBUG

#ifdef FLASH_DEBUG
#define flash_log printf
#else
#define flash_log(fmt, ...)
#endif
static int __flash_info_get(void)
{
    csi_error_t ret = csi_spiflash_get_flash_info(&spiflash_hd, &spiflash_info);
    if (ret != CSI_OK) {
        printf("csi_spiflash_get_flash_info failed,ret:%d\n", ret);
        return -1;
    }
    // printf("info->start_addr:0x%x\n", spiflash_info.xip_addr);
    // printf("info->sector_size:0x%x\n", spiflash_info.sector_size);
    // printf("info->sector_count:0x%x\n", spiflash_info.flash_size / spiflash_info.sector_size);
    return 0;
}

static int __flash_read(unsigned long addr, void *data, size_t data_len)
{
    // printf("%s, %d, 0x%x\n", __func__, __LINE__, addr);
    int ret = csi_spiflash_read(&spiflash_hd, addr - spiflash_info.xip_addr, data, data_len);
    // printf("%s, %d, 0x%x, ret:%d\n", __func__, __LINE__, addr, ret);
    if (ret < 0) {
        return -1;
    }
    return 0;
}

static int __flash_write(unsigned long addr, void *data, size_t data_len)
{
    int ret = csi_spiflash_program(&spiflash_hd, addr - spiflash_info.xip_addr, data, data_len);
    if ( ret < 0) {
        return -1;
    }
    return 0; 
}

static int __flash_erase(unsigned long addr, size_t len)
{
    if (csi_spiflash_erase(&spiflash_hd, addr, len)) {
        return -1;
    }

    return 0;
}

/**
 * Customize this method to perform any initialization
 * needed to access your flash device.
 *
 * @return: if this method returns an error,MUST RUTURN ERROR_INIT,
 * Otherwise return 0.
 */
int  flashInit(void) {
    // TODO
    csi_error_t ret = csi_spiflash_qspi_init(&spiflash_hd, 0, NULL);
    if (ret != 0) {
        printf("csi flash init failed.\n");
        return ERROR_INIT;
    }
    if ( __flash_info_get()) {
        return ERROR_INIT;
    }
    // printf("spi nor flash init over.\r\n");
    return 0;
}

/**
 * Customize this method to perform any un-initialization
 * needed to access your flash device.
 *
 * @return: if this method returns an error,MUST RUTURN ERROR_UNINIT,
 * Otherwise return 0.
 */
int  flashUnInit(void) {
    // TODO
    csi_spiflash_qspi_uninit(&spiflash_hd);
    return 0;
}

/**
 * Customize this method to read flash ID
 *
 * @param flashID: returns for flash ID
 *
 * @return: if this method returns an error,MUST RUTURN ERROR_READID,
 * Otherwise return 0.
 */
int  flashID(unsigned int* flashID) {
    // TODO
    flash_log("%s\r\n", __FUNCTION__);
    return 0;
}

/**
 * This method takes the data pointed to by the src parameter
 * and writes it to the flash blocks indicated by the
 * dst parameter.
 *
 * @param dst : destination address where flash program
 * @param src : address of data
 * @param length : data length
 *
 * @return : if this method returns an error,MUST RUTURN ERROR_PROGRAM,
 * Otherwise return 0.
 */
int flashProgram(uint32_t dst, char *src, int length) {
    // TODO
    int w_flag = 0;
    for (int i = 0; i < length; i++) {
        if (src[i] != 0xff) {
            w_flag = 1;
            break;
        }
    }
    if (w_flag == 1) {
        int ret = __flash_write(dst, src, length);
        flash_log("-------%s, ret: %d, dst: 0x%x, src:%p len:%d\r\n",  __FUNCTION__, ret, dst, src, length);
        if (ret)
            return ERROR_PROGRAM;
    } else {
        // printf("skip 0x%x, len: 0x%x\r\n", dst, length);
    }
    return 0;
}

/**
 * Customize this method to read data from a group of flash blocks into a buffer
 *
 * @param dst : reads the contents of those flash blocks into the address pointed to by
 * the dst parameter.
 * @param src : a pointer to a single flash.
 * @param length : data length
 *
 *  @return: if this method returns an error,MUST RUTURN ERROR_READ,
 * Otherwise return 0.
 */
int flashRead(char *dst, uint32_t src, int length) {
    // TODO
    int ret = __flash_read(src, dst, length);
    flash_log("-------%s, ret: %d, dst: %p, src:0x%x len:%d\r\n",  __FUNCTION__, ret, dst, src, length);
    if (ret)
        return ERROR_READ;
    return 0;
}

/**
 * Customize this method to erase a group of flash blocks.
 *
 * @param dst : a pointer to the base of the flash device.
 * NOTE: dst will always be sector aligned, the sector size is stored in FlashDev.c#FlashDevices#Devs#PageSize
 * @param length : erase length
 * NOTE: length will always be sector aligned, the sector size is stored in FlashDev.c#FlashDevices#Devs#PageSize
 *
 * @return : if this method returns an error,MUST RUTURN ERROR_ERASE,
 * Otherwise return 0
 */
int flashErase(uint32_t dst, int length) {
    // TODO
    int ret = __flash_erase(dst, length);
    flash_log("-------%s, ret: %d, dst: 0x%x, len:0x%x\r\n",  __FUNCTION__, ret, dst, length);
    if (ret)
        return ERROR_ERASE;
    return 0;
}

/**
 * Customize this method to erase the whole flash.
 *
 * @return : if this method returns an error,MUST RUTURN ERROR_CHIPERASE,
 * Otherwise return 0.
 */
int flashChipErase(void) {
    // TODO
    extern int32_t hal_spinor_erase_chip(void);
    int ret = hal_spinor_erase_chip();
    flash_log("%s\r\n", __FUNCTION__);
    if (ret)
        return ERROR_CHIPERASE;
    return 0;
}

/**
 * Customize this method to make the veryfiy process more quickly.
 *
 * @param dst : a pointer to the base of the flash device.
 * NOTE: dst will always be 4 aligned.
 * @param length : the lenght of the data which will be used for checksum
 * NOTE: the length will always be 4 aligned.
 * @param checksum : the expected checksum value in the programmed file(ihex,bin,elf format)
 *
 * @return : if the specified data's checksum from dst and lenght is checksum, return 0, else return ERROR_CHECKSUM
 * @example if the flash can be read directly, you can copy following code to replace current "return 0;"
 *
  int i, sum = 0;
  for (i = 0; i < length; i++) {
   sum += dst[i];
  }
  return sum == checksum ? 0 : ERROR_CHECKSUM;
 *
 */
#define CHECKSUM_BUF_LEN 1024
char g_checksumBuffer[CHECKSUM_BUF_LEN];

int flashChecksum(uint32_t dst, int length, int checksum) {
    flash_log("%s\r\n", __FUNCTION__);
    int i, sum = 0;
    int len = length;
    uint32_t flash_addr = dst;

    while (len) {
        if (len >= CHECKSUM_BUF_LEN) {
            __flash_read(flash_addr, g_checksumBuffer, CHECKSUM_BUF_LEN);
            for (i = 0; i < CHECKSUM_BUF_LEN; i++) {
                sum += g_checksumBuffer[i];
            }
            len -= CHECKSUM_BUF_LEN;
            flash_addr += CHECKSUM_BUF_LEN;
        } else if (len > 0 && len < CHECKSUM_BUF_LEN) {
            __flash_read(flash_addr, g_checksumBuffer, len);
            for (i = 0; i < len; i++) {
                sum += g_checksumBuffer[i];
            }
            len = 0;
        }
    }

    return sum == checksum ? 0 : ERROR_CHECKSUM;
}

// NOTING: when debug the driver, this macro defined as 1, and then
// it must be set as 0, for release to flash programmer library
#define DEBUG_DRIVER	0

/**
 * Debug entry for driver.
 *
 * @return : if this method returns an error,MUST RUTURN ERROR_CHIPERASE,
 * Otherwise return 0.
 */
int flashTest(void) {
    flash_log("%s\r\n", __FUNCTION__);
#if DEBUG_DRIVER

    unsigned int ID;
    // read flash id
    flashID(&ID);

    // other drivers test
    return ID;
#else
    return 0;
#endif
}

