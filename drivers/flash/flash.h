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
 * @file flash.h
 * @brief Flash Device Driver Interface
 *
 * This file provides the generic flash device interface that can be used
 * with various flash implementations (internal flash, SPI NOR flash, etc.).
 *
 * @author nickyyy
 * @date 2025-04-18
 */

#ifndef FLASH_H
#define FLASH_H

#include "std_types.h"

/**
 * @brief Flash device structure
 *
 * This structure defines a generic flash device with its properties
 * and operation function pointers.
 */
typedef struct
{
    uint32_t flash_size;            /**< Total flash size in bytes */
    uint32_t maximum_write_size;    /**< Maximum write size per operation */
    uint32_t erase_size;            /**< Erase sector/block size in bytes */
    std_ret (*flash_write)(uint32_t addr, uint8_t *data, uint32_t len);
    std_ret (*flash_read)(uint32_t addr, uint8_t *data, uint32_t len);
    std_ret (*flash_erase)(uint32_t addr, uint32_t len);
} flash_device_t;

/* ========================================================================= */
/* Public API Functions                                                      */
/* ========================================================================= */

/**
 * @brief Write data to flash device
 * @param dev Pointer to flash device structure
 * @param addr Flash address to write to
 * @param data Buffer containing data to write
 * @param len Number of bytes to write
 * @return std_ret E_OK on success, error code otherwise
 */
std_ret flash_write(flash_device_t *dev, uint32_t addr, uint8_t *data, uint32_t len);

/**
 * @brief Read data from flash device
 * @param dev Pointer to flash device structure
 * @param addr Flash address to read from
 * @param data Buffer to store read data
 * @param len Number of bytes to read
 * @return std_ret E_OK on success, error code otherwise
 */
std_ret flash_read(flash_device_t *dev, uint32_t addr, uint8_t *data, uint32_t len);

/**
 * @brief Erase flash sectors
 * @param dev Pointer to flash device structure
 * @param addr Start address to erase
 * @param len Number of bytes to erase
 * @return std_ret E_OK on success, error code otherwise
 */
std_ret flash_erase(flash_device_t *dev, uint32_t addr, uint32_t len);

#endif /* FLASH_H */
