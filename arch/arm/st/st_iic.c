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
 * @file st_iic.c
 * @brief STM32 I2C Driver Implementation
 *
 * This file provides the STM32 HAL-based implementation of the I2C driver
 * interface defined in drv_iic.h.
 *
 * @author nickyyy
 * @date 2025-04-20
 */

#include "st_iic.h"
#include "i2c.h"

/* Private function prototypes */
static std_ret st_iic_init(void* dev);
static std_ret st_iic_deinit(void* dev);
static std_ret st_iic_read(void* dev, uint16_t device_addr, uint16_t reg_addr, drv_iic_reg_addr_mode reg_mode, uint8_t* data, uint32_t data_len, uint32_t timeout);
static std_ret st_iic_write(void* dev, uint16_t device_addr, uint16_t reg_addr, drv_iic_reg_addr_mode reg_mode, uint8_t* data, uint32_t data_len, uint32_t timeout);

/* ========================================================================= */
/* Public Driver Objects                                                     */
/* ========================================================================= */

drv_iic_obj_t drv_iic4_obj = {
    .dev = &hi2c4,
    .init = st_iic_init,
    .deinit = st_iic_deinit,
    .read = st_iic_read,
    .write = st_iic_write
};

/* ========================================================================= */
/* Private Functions                                                         */
/* ========================================================================= */

static std_ret st_iic_init(void* dev)
{
    if (NULL == dev)
    {
        return E_INVALID_PARAM;
    }

    MX_I2C4_Init();

    return E_OK;
}

static std_ret st_iic_deinit(void* dev)
{
    if (NULL == dev)
    {
        return E_INVALID_PARAM;
    }

    HAL_I2C_MspDeInit(dev);

    return E_OK;
}

static std_ret st_iic_read(void* dev, uint16_t device_addr, uint16_t reg_addr, drv_iic_reg_addr_mode reg_mode, uint8_t* data, uint32_t data_len, uint32_t timeout)
{
    if (NULL == dev || NULL == data || 0 == data_len)
    {
        return E_INVALID_PARAM;
    }

    I2C_HandleTypeDef *hi2c = (I2C_HandleTypeDef *)dev;
    HAL_StatusTypeDef hal_status;
    uint16_t mem_addr_size;

    if (reg_mode == IIC_REG_ADDR_8_BIT)
    {
        mem_addr_size = I2C_MEMADD_SIZE_8BIT;
        hal_status = HAL_I2C_Mem_Read(hi2c, device_addr, reg_addr, mem_addr_size, data, data_len, timeout);
    }
    else if (reg_mode == IIC_REG_ADDR_16_BIT)
    {
        mem_addr_size = I2C_MEMADD_SIZE_16BIT;
        hal_status = HAL_I2C_Mem_Read(hi2c, device_addr, reg_addr, mem_addr_size, data, data_len, timeout);
    }
    else
    {
        hal_status = HAL_I2C_Master_Receive(hi2c, device_addr, data, data_len, timeout);
    }

    return (hal_status == HAL_OK) ? E_OK : E_NOK;
}

static std_ret st_iic_write(void* dev, uint16_t device_addr, uint16_t reg_addr, drv_iic_reg_addr_mode reg_mode, uint8_t* data, uint32_t data_len, uint32_t timeout)
{
    if (NULL == dev || NULL == data || 0 == data_len)
    {
        return E_INVALID_PARAM;
    }

    I2C_HandleTypeDef *hi2c = (I2C_HandleTypeDef *)dev;
    HAL_StatusTypeDef hal_status;
    uint16_t mem_addr_size;

    if (reg_mode == IIC_REG_ADDR_8_BIT)
    {
        mem_addr_size = I2C_MEMADD_SIZE_8BIT;
        hal_status = HAL_I2C_Mem_Write(hi2c, device_addr, reg_addr, mem_addr_size, data, data_len, timeout);
    }
    else if (reg_mode == IIC_REG_ADDR_16_BIT)
    {
        mem_addr_size = I2C_MEMADD_SIZE_16BIT;
        hal_status = HAL_I2C_Mem_Write(hi2c, device_addr, reg_addr, mem_addr_size, data, data_len, timeout);
    }
    else
    {
        hal_status = HAL_I2C_Master_Transmit(hi2c, device_addr, data, data_len, timeout);
    }

    return (hal_status == HAL_OK) ? E_OK : E_NOK;
}
