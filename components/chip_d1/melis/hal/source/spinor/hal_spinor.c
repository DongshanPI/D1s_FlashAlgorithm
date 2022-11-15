/* This is all about APIs for/from other modules */

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <hal_timer.h>
#include <sunxi_hal_common.h>
#include "inter.h"

static sunxi_hal_spinor_info hal_spinor_info = {0};

sunxi_hal_version_t hal_spinor_get_version(int32_t dev)
{
    sunxi_hal_version_t ver;
    HAL_ARG_UNUSED(dev);

    ver.api = SUNXI_HAL_SPINOR_API_VERSION;
    ver.drv = SUNXI_HAL_SPINOR_DRV_VERSION;
    return ver;
}

sunxi_hal_spinor_capabilities_t hal_spinor_get_capabilities(void)
{
    sunxi_hal_spinor_capabilities_t cap;

    cap.event_ready = 1;
    cap.data_width = 1;
    cap.erase_chip = 1;
    cap.reserved = 0;

    return cap;
}

int32_t hal_spinor_init(sunxi_hal_spinor_signal_event_t cb_event)
{
    return nor_init();
}

int32_t hal_spinor_deinit(void)
{
    hal_spinor_sync();
    return nor_deinit();
}

int32_t hal_spinor_power_control(sunxi_hal_power_state_e state)
{
    return SUNXI_HAL_OK;
}

int32_t hal_spinor_read_data(uint32_t addr, void *buf, uint32_t cnt)
{
#ifdef CONFIG_DRIVERS_SPINOR_CACHE
    return nor_cache_read(addr, buf, cnt);
#else
    return nor_read(addr, (void *)buf, cnt);
#endif
}

int32_t hal_spinor_program_data(uint32_t addr, const void *buf, uint32_t cnt)
{
#ifdef CONFIG_DRIVERS_SPINOR_CACHE
    return nor_cache_write(addr, (void *)buf, cnt);
#else
    return nor_write(addr, (void *)buf, cnt);
#endif
}

int32_t hal_spinor_sync(void)
{
#ifdef CONFIG_DRIVERS_SPINOR_CACHE
    return nor_cache_sync();
#else
    return 0;
#endif
}

int32_t hal_spinor_erase_sector(uint32_t addr, uint32_t size)
{
#ifdef CONFIG_DRIVERS_SPINOR_CACHE
    return nor_cache_erase(addr, size);
#else
    return nor_erase(addr, size);
#endif
}

int32_t hal_spinor_erase_chip(void)
{
    struct nor_flash *nor = get_nor_flash();

#ifdef CONFIG_DRIVERS_SPINOR_CACHE
    nor_cache_sync();
#endif
    return nor_erase(0, nor->total_size);
}

sunxi_hal_spinor_status_t hal_spinor_get_status(void)
{
    sunxi_hal_spinor_status_t spinor_status = {0, 0, 0};

    return spinor_status;
}

sunxi_hal_spinor_info *hal_spinor_get_info(void)
{
    struct nor_flash *nor = get_nor_flash();

    if (!nor->info)
        return NULL;

    hal_spinor_info.sector_size = nor->blk_size;
    hal_spinor_info.sector_count = nor->total_size / nor->blk_size;
    hal_spinor_info.page_size = nor->page_size;
    hal_spinor_info.program_unit = nor->page_size;
    return &hal_spinor_info;
}

void hal_spinor_signal_event(uint32_t event)
{
    return;
}

int32_t hal_spinor_control(int32_t dev, uint32_t command, uint32_t arg)
{
    return SUNXI_HAL_OK;
}
