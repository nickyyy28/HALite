/*
 * Copyright (c) 2025 nickyyy
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file drv_sdmmc.c
 * @brief SDMMC Driver Framework Implementation
 *
 * This file implements the hardware-independent SDMMC API that calls
 * platform-specific function pointers.
 *
 * @author nickyyy
 * @date 2025-04-24
 */

#include "drv_sdmmc.h"
#include <stddef.h>

/* ========================================================================= */
/* Public API Functions                                                      */
/* ========================================================================= */

std_ret drv_sdmmc_init(drv_sdmmc_obj_t *obj)
{
    std_ret ret = E_NOK;
    if (NULL != obj && NULL != obj->init)
    {
        ret = obj->init(obj->dev);
    }
    else
    {
        ret = E_INVALID_PARAM;
    }
    return ret;
}

std_ret drv_sdmmc_deinit(drv_sdmmc_obj_t *obj)
{
    std_ret ret = E_NOK;
    if (NULL != obj && NULL != obj->deinit)
    {
        ret = obj->deinit(obj->dev);
    }
    else
    {
        ret = E_INVALID_PARAM;
    }
    return ret;
}

std_ret drv_sdmmc_set_bus_width(drv_sdmmc_obj_t *obj, drv_sdmmc_bus_width_t width)
{
    std_ret ret = E_NOK;
    if (NULL != obj && NULL != obj->set_bus_width)
    {
        ret = obj->set_bus_width(obj->dev, width);
    }
    else
    {
        ret = E_INVALID_PARAM;
    }
    return ret;
}

std_ret drv_sdmmc_set_speed(drv_sdmmc_obj_t *obj, drv_sdmmc_speed_t speed)
{
    std_ret ret = E_NOK;
    if (NULL != obj && NULL != obj->set_speed)
    {
        ret = obj->set_speed(obj->dev, speed);
    }
    else
    {
        ret = E_INVALID_PARAM;
    }
    return ret;
}

std_ret drv_sdmmc_get_info(drv_sdmmc_obj_t *obj, drv_sdmmc_info_t *info)
{
    std_ret ret = E_NOK;
    if (NULL != obj && NULL != obj->get_info)
    {
        ret = obj->get_info(obj->dev, info);
    }
    else
    {
        ret = E_INVALID_PARAM;
    }
    return ret;
}

drv_sdmmc_state_t drv_sdmmc_get_state(drv_sdmmc_obj_t *obj)
{
    if (NULL != obj && NULL != obj->get_state)
    {
        return obj->get_state(obj->dev);
    }
    return SDMMC_STATE_ERROR;
}

std_ret drv_sdmmc_read_blocks(drv_sdmmc_obj_t *obj, uint32_t block_addr, uint8_t *data, uint32_t num_blocks)
{
    std_ret ret = E_NOK;
    if (NULL != obj && NULL != obj->read_blocks)
    {
        ret = obj->read_blocks(obj->dev, block_addr, data, num_blocks);
    }
    else
    {
        ret = E_INVALID_PARAM;
    }
    return ret;
}

std_ret drv_sdmmc_write_blocks(drv_sdmmc_obj_t *obj, uint32_t block_addr, const uint8_t *data, uint32_t num_blocks)
{
    std_ret ret = E_NOK;
    if (NULL != obj && NULL != obj->write_blocks)
    {
        ret = obj->write_blocks(obj->dev, block_addr, data, num_blocks);
    }
    else
    {
        ret = E_INVALID_PARAM;
    }
    return ret;
}

std_ret drv_sdmmc_erase_blocks(drv_sdmmc_obj_t *obj, uint32_t start_addr, uint32_t end_addr)
{
    std_ret ret = E_NOK;
    if (NULL != obj && NULL != obj->erase_blocks)
    {
        ret = obj->erase_blocks(obj->dev, start_addr, end_addr);
    }
    else
    {
        ret = E_INVALID_PARAM;
    }
    return ret;
}
