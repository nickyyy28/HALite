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
 * @file at24cxx.c
 * @brief AT24CXX EEPROM Driver Implementation
 *
 * This file provides the driver implementation for AT24CXX series EEPROM devices
 * using the I2C framework interface defined in drv_iic.h.
 *
 * @author nickyyy
 * @date 2025-04-20
 */

#include "at24cxx.h"
#include "drv_tick.h"

#define AT24C_IIC_TIMEOUT 1000

/* Private function prototypes */
static std_ret at24c_init(at24c_dev_t *dev);
static std_ret at24c_read_bytes(at24c_dev_t *dev, uint16_t mem_addr, uint8_t *data, uint32_t len);
static std_ret at24c_write_bytes(at24c_dev_t *dev, uint16_t mem_addr, uint8_t *data, uint32_t len);

/* ========================================================================= */
/* Public API Functions                                                      */
/* ========================================================================= */

std_ret at24c_init(at24c_dev_t *dev)
{
    if (dev == NULL || dev->iic_drv == NULL) {
        return E_INVALID_PARAM;
    }

    /* Initialize underlying I2C driver via framework API */
    return drv_iic_init(dev->iic_drv);
}

std_ret at24c_read_bytes(at24c_dev_t *dev, uint16_t mem_addr, uint8_t *data, uint32_t len)
{
    if (dev == NULL || data == NULL || len == 0) {
        return E_INVALID_PARAM;
    }

    /* Boundary check */
    if ((mem_addr + len) > dev->capacity) {
        return E_NOK;
    }

    uint8_t real_dev_addr;
    uint16_t real_mem_addr;

    switch (dev->type)
    {
    case AT24C01:
    case AT24C02:
        real_dev_addr = dev->dev_addr;
        real_mem_addr = mem_addr;
        break;
    case AT24C04:
        real_dev_addr = mem_addr >= 256 ? 0xA2 : 0xA0;
        real_mem_addr = mem_addr & 0xFF;
        break;
    case AT24C08:
    case AT24C16:
        real_dev_addr = dev->dev_addr | ((mem_addr >> 8) << 1);
        real_mem_addr = mem_addr & 0xFF;
        break;
    case AT24C32:
    case AT24C64:
    case AT24C128:
    case AT24C256:
        real_dev_addr = dev->dev_addr;
        real_mem_addr = mem_addr;
        break;
    default:
        return E_INVALID_PARAM;
    }

    return drv_iic_read(dev->iic_drv,
                          real_dev_addr,
                          real_mem_addr,
                          dev->addr_mode,
                          data,
                          len,
                          AT24C_IIC_TIMEOUT);
}

std_ret at24c_write_bytes(at24c_dev_t *dev, uint16_t mem_addr, uint8_t *data, uint32_t len)
{
    std_ret ret = E_OK;
    uint32_t bytes_left = len;
    uint32_t current_addr = mem_addr;
    uint8_t *current_data = data;

    if (dev == NULL || data == NULL || len == 0) {
        return E_INVALID_PARAM;
    }

    /* Boundary check */
    if ((mem_addr + len) > dev->capacity) {
        return E_NOK;
    }

    while (bytes_left > 0)
    {
        /* Calculate current page offset */
        uint16_t page_offset = current_addr % dev->page_size;

        /* Calculate remaining space in current page */
        uint32_t space_in_page = dev->page_size - page_offset;

        /* Determine actual bytes to write in this transaction */
        uint32_t bytes_to_write = (bytes_left < space_in_page) ? bytes_left : space_in_page;

        /* Call underlying I2C write interface via framework API */
        ret = drv_iic_write(dev->iic_drv,
                            dev->dev_addr,
                            current_addr,
                            dev->addr_mode,
                            current_data,
                            bytes_to_write,
                            AT24C_IIC_TIMEOUT);

        if (ret != E_OK) {
            return ret; /* Write failed, return immediately */
        }

        /*
         * Key point: EEPROM requires ~5ms internal programming time after each physical write.
         * Without delay, the chip will NACK subsequent communications.
         * Advanced approach uses ACK polling, but fixed delay is used here for simplicity.
         */
        delay_ms(5);

        /* Update remaining data and address */
        current_addr += bytes_to_write;
        current_data += bytes_to_write;
        bytes_left   -= bytes_to_write;
    }

    return E_OK;
}
