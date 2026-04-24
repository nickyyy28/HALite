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
 * @file drv_iic.c
 * @brief I2C Driver Framework Implementation
 *
 * This file implements the hardware-independent I2C API that calls
 * platform-specific function pointers.
 *
 * @author nickyyy
 * @date 2025-04-24
 */

#include "drv_iic.h"
#include <stddef.h>

/* ========================================================================= */
/* Public API Functions                                                      */
/* ========================================================================= */

std_ret drv_iic_init(drv_iic_obj_t *obj)
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

std_ret drv_iic_deinit(drv_iic_obj_t *obj)
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

std_ret drv_iic_read(drv_iic_obj_t *obj, uint16_t device_addr, uint16_t reg_addr, drv_iic_reg_addr_mode reg_mode, uint8_t* data, uint32_t data_len, uint32_t timeout)
{
    std_ret ret = E_NOK;
    if (NULL != obj && NULL != obj->read)
    {
        ret = obj->read(obj->dev, device_addr, reg_addr, reg_mode, data, data_len, timeout);
    }
    else
    {
        ret = E_INVALID_PARAM;
    }
    return ret;
}

std_ret drv_iic_write(drv_iic_obj_t *obj, uint16_t device_addr, uint16_t reg_addr, drv_iic_reg_addr_mode reg_mode, uint8_t* data, uint32_t data_len, uint32_t timeout)
{
    std_ret ret = E_NOK;
    if (NULL != obj && NULL != obj->write)
    {
        ret = obj->write(obj->dev, device_addr, reg_addr, reg_mode, data, data_len, timeout);
    }
    else
    {
        ret = E_INVALID_PARAM;
    }
    return ret;
}
