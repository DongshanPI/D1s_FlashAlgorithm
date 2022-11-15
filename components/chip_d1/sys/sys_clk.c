/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     sys_clk.c
 * @brief    source file for setting system frequency.
 * @version  V1.0
 * @date     14. Oct 2020
 ******************************************************************************/

#include <stdint.h>
#include <soc.h>
#include <sys_clk.h>
#include <drv/io.h>
#include <hal_clk.h>
#include <hal_reset.h>
#include <ccmu/common_ccmu.h>

uint32_t g_system_clock = IHS_VALUE;

uint32_t soc_get_pll_clk(void)
{
    // TODO:
    return 0;
}

uint32_t soc_get_cpu_clk(uint32_t idx)
{
    hal_clk_t clk = hal_clock_get(HAL_SUNXI_CCU, CLK_PLL_CPUX_DIV);
    uint32_t rate = hal_clk_get_rate(clk);
    return rate;
}
uint32_t soc_get_sys_clk(uint32_t idx)
{
    return g_system_clock;
}
uint32_t soc_get_ahb_clk(uint32_t idx)
{   
    hal_clk_t clk = hal_clock_get(HAL_SUNXI_CCU, CLK_PSI_AHB);
    uint32_t rate = hal_clk_get_rate(clk);
    return rate;
}
uint32_t soc_get_apb_clk(uint32_t idx)
{
    hal_clk_id_t id;
    switch (idx)
    {
    case 0:
        id = CLK_APB0;
        break;
    case 1:
        id = CLK_APB1;
        break;
    default:
        return 0;
    }
    hal_clk_t clk = hal_clock_get(HAL_SUNXI_CCU, id);
    uint32_t rate = hal_clk_get_rate(clk);
    return rate;
}
uint32_t soc_get_uart_clk(uint32_t idx)
{
    hal_clk_id_t id;
    switch (idx)
    {
    case 0:
        id = CLK_BUS_UART0;
        break;
    case 1:
        id = CLK_BUS_UART1;
        break;
    default:
        return 0;
    }
    hal_clk_t clk = hal_clock_get(HAL_SUNXI_CCU, id);
    uint32_t rate = hal_clk_get_rate(clk);
    return rate;
}
uint32_t soc_get_audio_clk(uint32_t idx)
{
    // TODO:
    return 0;
}
uint32_t soc_get_vad_clk(uint32_t idx)
{
    // TODO:
    return 0;
}
/************************************************/
uint32_t soc_get_uart_freq(uint32_t idx)
{
    return soc_get_uart_clk(idx);
}

uint32_t soc_get_iic_freq(uint32_t idx)
{
    return soc_get_apb_clk(idx);
}

uint32_t soc_get_spi_freq(uint32_t idx)
{
    hal_clk_id_t id;
    switch (idx)
    {
    case 0:
        id = CLK_SPI0;
        break;
    case 1:
        id = CLK_SPI1;
        break;
    default:
        return 0;
    }
    hal_clk_t clk = hal_clock_get(HAL_SUNXI_CCU, id);
    uint32_t rate = hal_clk_get_rate(clk);
    return rate;
}

uint32_t soc_get_qspi_freq(uint32_t idx)
{
    return soc_get_apb_clk(idx);
}

uint32_t soc_get_adc_freq(uint32_t idx)
{
    return soc_get_apb_clk(idx);
}

uint32_t soc_get_pwm_freq(uint32_t idx)
{
    return soc_get_apb_clk(idx);
}

uint32_t soc_get_wdt_freq(uint32_t idx)
{
    return soc_get_apb_clk(idx);
}

uint32_t soc_get_i2s_freq(uint32_t idx)
{
    return soc_get_audio_clk(idx);
}

uint32_t soc_get_timer_freq(uint32_t idx)
{
    return ILS_VALUE;
}

uint32_t soc_get_rtc_freq(uint32_t idx)
{
    return ILS_VALUE;
}

uint32_t soc_get_pll_freq(void)
{
    return soc_get_pll_clk();
}

uint32_t soc_get_cpu_freq(uint32_t idx)
{
    return soc_get_cpu_clk(idx);
}

uint32_t soc_get_sys_freq(uint32_t idx)
{
    return soc_get_sys_clk(idx);
}

uint32_t soc_get_ahb_freq(uint32_t idx)
{
    return soc_get_ahb_clk(idx);
}

uint32_t soc_get_apb_freq(uint32_t idx)
{
    return soc_get_apb_clk(idx);
}

uint32_t soc_get_cur_cpu_freq(void)
{
    return soc_get_cpu_clk(0);
}

uint32_t soc_get_coretim_freq(void)
{
    return g_system_clock;
}



void soc_clock_gate_single(clk_clock_gate_t ip, uint32_t state)
{
}

void soc_soft_reset_single(clk_soft_reset_t ip)
{
}

void soc_sysclk_srst_all(void)
{
    clk_soft_reset_t i = 0U;

    do {
        soc_soft_reset_single(i++);
    } while (i < SRST_MAXIMUM);
}

void soc_clkgate_en_all(void)
{
    clk_clock_gate_t i = 0U;

    do {
        soc_clock_gate_single(i++, 1);
    } while (i < CG_MAXIMUM);
}

csi_error_t soc_sysclk_config(system_clk_config_t *config)
{
    return CSI_OK;
}

void soc_set_sys_freq(uint32_t val)
{
    g_system_clock = val;
}

void soc_clk_init(void)
{
    hal_clock_init();
}
