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
 * @file at24cxx.h
 * @brief AT24CXX EEPROM Driver Interface
 *
 * This file provides the interface definition for AT24CXX series EEPROM devices.
 *
 * @author nickyyy
 * @date 2025-04-20
 */

#ifndef AT24CXX_H
#define AT24CXX_H

#include "drv_iic.h"

/**
 * @brief AT24CXX device type enumeration
 */
typedef enum
{
    AT24C01,
    AT24C02,
    AT24C04,
    AT24C08,
    AT24C16,
    AT24C32,
    AT24C64,
    AT24C128,
    AT24C256
} at24cxx_type_t;

/**
 * @brief AT24CXX device structure
 */
typedef struct {
    drv_iic_obj_t *iic_drv;         /**< Pointer to I2C driver object */
    uint16_t dev_addr;              /**< Device I2C address */
    uint32_t capacity;              /**< Total capacity in bytes */
    uint16_t page_size;             /**< Page size in bytes */
    drv_iic_reg_addr_mode addr_mode; /**< Address mode (8-bit or 16-bit) */
    at24cxx_type_t type;            /**< Device type */
} at24c_dev_t;

/* ========================================================================= */
/* Public API Functions                                                      */
/* ========================================================================= */

/**
 * @brief Initialize AT24CXX device
 * @param dev Pointer to device structure
 * @return std_ret E_OK on success, error code otherwise
 */
std_ret at24c_init(at24c_dev_t *dev);

/**
 * @brief Read bytes from AT24CXX
 * @param dev Pointer to device structure
 * @param mem_addr Memory address to read from
 * @param data Buffer to store read data
 * @param len Number of bytes to read
 * @return std_ret E_OK on success, error code otherwise
 */
std_ret at24c_read_bytes(at24c_dev_t *dev, uint16_t mem_addr, uint8_t *data, uint32_t len);

/**
 * @brief Write bytes to AT24CXX
 * @param dev Pointer to device structure
 * @param mem_addr Memory address to write to
 * @param data Buffer containing data to write
 * @param len Number of bytes to write
 * @return std_ret E_OK on success, error code otherwise
 */
std_ret at24c_write_bytes(at24c_dev_t *dev, uint16_t mem_addr, uint8_t *data, uint32_t len);

#endif /* AT24CXX_H */
