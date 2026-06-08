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
 * @file drv_spi.c
 * @brief SPI/QSPI Driver Framework Implementation
 *
 * This file implements the hardware-independent SPI/QSPI API that calls
 * platform-specific function pointers.
 *
 * @author nickyyy
 * @date 2025-04-24
 */

#include "drv_spi.h"
#include <stddef.h>

/* ========================================================================= */
/* Public API Functions                                                      */
/* ========================================================================= */

std_ret drv_spi_init(drv_spi_obj_t *obj)
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

std_ret drv_spi_deinit(drv_spi_obj_t *obj)
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

std_ret drv_spi_write(drv_spi_obj_t *obj, const uint8_t *data, uint32_t size, drv_spi_lines_t line)
{
    std_ret ret = E_NOK;
    if (NULL != obj && NULL != obj->write)
    {
        if (obj->set_cs != NULL) {
            obj->set_cs(0); // Assert CS
        }
        ret = obj->write(obj->dev, data, size, line);
        if (obj->set_cs != NULL) {
            obj->set_cs(1); // Deassert CS
        }
    }
    else
    {
        ret = E_INVALID_PARAM;
    }
    return ret;
}

std_ret drv_spi_read(drv_spi_obj_t *obj, uint8_t *dst, uint32_t size, drv_spi_lines_t line)
{
    std_ret ret = E_NOK;
    if (NULL != obj && NULL != obj->read)
    {
        if (obj->set_cs != NULL) {
            obj->set_cs(0); // Assert CS
        }
        ret = obj->read(obj->dev, dst, size, line);
        if (obj->set_cs != NULL) {
            obj->set_cs(1); // Deassert CS
        }
    }
    else
    {
        ret = E_INVALID_PARAM;
    }
    return ret;
}

std_ret drv_spi_writeread(const drv_spi_obj_t *obj, uint8_t cmd, drv_spi_lines_t cmd_line,
    uint32_t addr, drv_spi_lines_t addr_line, drv_spi_addr_mode_t addr_mode, drv_spi_dummy_cycles_t dummy_cycles,
    uint8_t *data, uint32_t data_size, drv_spi_lines_t data_line, drv_spi_direction_t direction)
{
    std_ret ret = E_NOK;
    if (NULL != obj && NULL != obj->writeread)
    {
        if (obj->set_cs != NULL) {
            obj->set_cs(0); // Assert CS
        }
        ret = obj->writeread(obj->dev, cmd, cmd_line, addr, addr_line, addr_mode, dummy_cycles,
                             data, data_size, data_line, direction);
        if (obj->set_cs != NULL) {
            obj->set_cs(1); // Deassert CS
        }
    }
    else
    {
        ret = E_INVALID_PARAM;
    }
    return ret;
}
