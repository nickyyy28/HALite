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
 * @file spinor.h
 * @brief SPI NOR Flash Driver Interface
 *
 * This file provides the interface for SPI NOR flash device operations
 * using the SPI framework driver.
 *
 * @author nickyyy
 * @date 2025-04-18
 */

#ifndef SPINOR_H
#define SPINOR_H

#include <stdbool.h>
#include "drv_spi.h"

/**
 * @brief SPI NOR flash operation structure
 */
typedef struct
{
    uint8_t command;                /**< Command byte */
    drv_spi_lines_t cmd_line;       /**< Command line mode */
    drv_spi_addr_mode_t addr_mode;  /**< Address mode */
    drv_spi_lines_t addr_line;      /**< Address line mode */
    drv_spi_lines_t data_line;      /**< Data line mode */
    drv_spi_dummy_cycles_t dummy_cycles; /**< Dummy cycles */
} spi_nor_operate_t;

/**
 * @brief SPI NOR device structure
 */
typedef struct spi_nor_device_t
{
    uint32_t jedec_id;              /**< JEDEC ID */
    std_ret (*flash_probe)(drv_spi_obj_t *spi_drv, struct spi_nor_device_t *device);
    bool (*flash_is_busy)(const drv_spi_obj_t *spi_drv);
    spi_nor_operate_t read_id_op;   /**< Read ID operation */
    spi_nor_operate_t read_op;      /**< Read operation */
    spi_nor_operate_t program_page_op; /**< Page program operation */
    spi_nor_operate_t erase_sector_op; /**< Sector erase operation */
    spi_nor_operate_t write_enable_op; /**< Write enable operation */
} spi_nor_device_t;

/* ========================================================================= */
/* Public API Functions                                                      */
/* ========================================================================= */

/**
 * @brief Probe SPI NOR flash device
 * @param spi_drv Pointer to SPI driver object
 * @param spi_nor Pointer to store detected device pointer
 * @return std_ret E_OK on success, error code otherwise
 */
std_ret spinor_probe_device(const drv_spi_obj_t *spi_drv, spi_nor_device_t **spi_nor);

/**
 * @brief Erase a sector of SPI NOR flash
 * @param spi_drv Pointer to SPI driver object
 * @param spi_nor Pointer to SPI NOR device structure
 * @param addr Sector address to erase
 * @return std_ret E_OK on success, error code otherwise
 */
std_ret spinor_erase_sector(const drv_spi_obj_t *spi_drv, const spi_nor_device_t *spi_nor, uint32_t addr);

/**
 * @brief Program a page to SPI NOR flash
 * @param spi_drv Pointer to SPI driver object
 * @param spi_nor Pointer to SPI NOR device structure
 * @param addr Page address to program
 * @param data Buffer containing data to write
 * @param size Number of bytes to write
 * @return std_ret E_OK on success, error code otherwise
 */
std_ret spinor_page_program(const drv_spi_obj_t *spi_drv, const spi_nor_device_t *spi_nor, uint32_t addr, const uint8_t *data, uint32_t size);

/**
 * @brief Read data from SPI NOR flash
 * @param spi_drv Pointer to SPI driver object
 * @param spi_nor Pointer to SPI NOR device structure
 * @param addr Address to read from
 * @param data Buffer to store read data
 * @param size Number of bytes to read
 * @return std_ret E_OK on success, error code otherwise
 */
std_ret spinor_read(const drv_spi_obj_t *spi_drv, const spi_nor_device_t *spi_nor, uint32_t addr, uint8_t *data, uint32_t size);

/**
 * @brief Wait for SPI NOR flash to be not busy
 * @param spi_drv Pointer to SPI driver object
 * @param spi_nor Pointer to SPI NOR device structure
 * @param timeout_ms Timeout in milliseconds
 * @return std_ret E_OK on success, E_TIMEOUT on timeout
 */
std_ret spinor_wait_not_busy(const drv_spi_obj_t *spi_drv, const spi_nor_device_t *spi_nor, uint32_t timeout_ms);

#endif /* SPINOR_H */
