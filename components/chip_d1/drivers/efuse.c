/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     efuse.c
 * @brief
 * @version
 * @date     2020-07-05
 ******************************************************************************/

#include <string.h>
#include <soc.h>
#include <sunxi_hal_efuse.h>
#include <efuse.h>
#include <drv/efuse.h>

/**
  \brief       Initialize EFUSEC Interface. 1. Initializes the resources needed for the EFUSEC interface
  \param[in]   idx  Device id
  \return      Error code
*/
csi_error_t drv_efuse_init(csi_efuse_t *efuse, int32_t idx)
{
    CSI_PARAM_CHK(efuse, CSI_ERROR);

    csi_error_t ret = CSI_OK;

    target_get(DEV_WJ_EFUSE_TAG, idx, &efuse->dev);

    efuse->info.start = CONFIG_OTP_BASE_ADDR;
    efuse->info.end = CONFIG_OTP_BANK_SIZE - 1;

    return ret;
}

/**
  \brief       De-initialize EFUSEC Interface. stops operation and releases the software resources used by the interface
  \param[in]   efuse  Efuse efuse to operate.
  \return      None
*/
void drv_efuse_uninit(csi_efuse_t *efuse)
{
    CSI_PARAM_CHK_NORETVAL(efuse);

    memset(efuse, 0, sizeof(csi_efuse_t));
}

/**
  \brief       Read data from Efuse.
  \param[in]   efuse Efuse handle to operate.
  \param[in]   addr  Data address.
  \param[out]  data  Pointer to a buffer storing the data read from Efuse.
  \param[in]   size  Number of data items to read.
  \return      Number of data items read or error code
*/
int32_t drv_efuse_read(csi_efuse_t *efuse, uint32_t addr, void *data, uint32_t size)
{
#define EFUSE_ROUND_UP(x, y) ((((x) + ((y)-1)) / (y)) * (y))
    uint tmp = 0, i = 0, k_u32_l = 0, bit_lft = 0;
    int offset = 0, tmp_sz = 0;
    /*if key_data not aligned ,u32_p will not be accessed*/
    unsigned int *u32_p = (unsigned int *)data;
    unsigned char *u8_p = (unsigned char *)data;

    CSI_PARAM_CHK(efuse, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    if (addr < efuse->info.start || addr > efuse->info.end || addr + size - 1 < efuse->info.start || addr + size - 1 > efuse->info.end)
    {
        return CSI_ERROR;
    }

    k_u32_l = size / 32;
    bit_lft = size % 32;
    offset = addr;

    for (i = 0; i < k_u32_l; i++)
    {
        tmp = efuse_sram_read_key(offset);
        if (((unsigned long)data & 0x3) == 0)
        {
            u32_p[i] = tmp;
        }
        else
        {
            memcpy((void *)(u8_p + i * 4), (void *)(&tmp), 4);
        }
        offset += 4;
        tmp_sz += 4;
    }

    if (bit_lft)
    {
        // EFUSE_DBG("bit lft is %d\n",bit_lft);
        tmp = efuse_sram_read_key(offset);
        memcpy((void *)(u8_p + k_u32_l * 4), (void *)(&tmp),
               EFUSE_ROUND_UP(bit_lft, 8) / 8);
        tmp_sz += (EFUSE_ROUND_UP(bit_lft, 8) / 8);
    }

    // EFUSE_DBG_DUMP(key_name, key_data, key_bit_len/8);

    return tmp_sz;
}

/**
  \brief       Program data to Efuse.
  \param[in]   efuse Efuse handle to operate.
  \param[in]   addr  Data address.
  \param[in]   data  Pointer to a buffer containing the data to be programmed to Efuse.
  \param[in]   cnt   Number of data items to program.
  \return      number of data items programmed or error code
*/
int32_t drv_efuse_program(csi_efuse_t *efuse, uint32_t addr, const void *data, uint32_t size)
{
    unsigned long k_src = 0;
    unsigned int niddle = 0, tmp_data = 0, k_d_lft = 0;
    size_t key_len = size;

    CSI_PARAM_CHK(efuse, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    if (addr < efuse->info.start || addr > efuse->info.end || addr + size - 1 < efuse->info.start || addr + size - 1 > efuse->info.end)
    {
        return CSI_ERROR;
    }

    if ((key_len == 0) || ((unsigned long)data % 4 != 0))
        return CSI_ERROR;

    niddle = addr;
    k_d_lft = key_len;
    k_src = (unsigned long)data;

    while (k_d_lft >= 4)
    {
        tmp_data = *(unsigned int *)k_src;
        // EFUSE_DBG("offset:0x%x val:0x%x\n", niddle, tmp_data);
        if (tmp_data)
        {
            if (efuse_uni_burn_key(niddle, tmp_data))
            {
                return CSI_ERROR;
            }
        }
        k_d_lft -= 4;
        niddle += 4;
        k_src += 4;
    }

    if (k_d_lft)
    {
        uint mask = (1UL << (k_d_lft << 3)) - 1;
        tmp_data = *(unsigned int *)k_src;
        mask &= tmp_data;
        // EFUSE_DBG("offset:0x%x val:0x%x\n", niddle, mask);
        if (mask)
        {
            if (efuse_uni_burn_key(niddle, mask))
            {
                return CSI_ERROR;
            }
        }
    }

    return CSI_OK;
}

/**
  \brief       Get Efuse information.
  \param[in]   efuse  Efuse handle to operate.
  \param[out]  info   Efuse info \refs csi_efuse_info_t.
  \return      Error code
*/
csi_error_t drv_efuse_get_info(csi_efuse_t *efuse, csi_efuse_info_t *info)
{
    CSI_PARAM_CHK(efuse, CSI_ERROR);

    info->start = efuse->info.start;
    info->end = efuse->info.end;
    return CSI_OK;
}
