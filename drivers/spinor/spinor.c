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
 * @file spinor.c
 * @brief SPI NOR Flash Driver Implementation
 *
 * This file provides the implementation for SPI NOR flash device operations
 * using the SPI framework driver.
 *
 * @author nickyyy
 * @date 2025-04-18
 */

#include "spinor.h"
#include "../../kernel/log.h"
#include "drv_tick.h"

/* Private function prototypes */
static std_ret spinor_execute_operation(const drv_spi_obj_t *spi_drv, spi_nor_operate_t op, uint32_t addr, uint8_t *data, uint32_t size, drv_spi_direction_t direction);

/* ========================================================================= */
/* W25Q256JVIQ Specific Definitions                                          */
/* ========================================================================= */

/* ========================================================================= */
/* W25Q256JVIQ Specific Definitions                                          */
/* ========================================================================= */
static bool w25q256jviq_is_busy_dualbank(const drv_spi_obj_t *spi_drv);
static std_ret w25q256jviq_probe_dualbank(drv_spi_obj_t *spi_drv, struct spi_nor_device_t *device);

/* ========================================================================= */
/* W25Q64 Specific Definitions                                               */
/* ========================================================================= */
static bool w25q64_is_busy(const drv_spi_obj_t *spi_drv);
static std_ret w25q64_probe(drv_spi_obj_t *spi_drv, struct spi_nor_device_t *device);

/* ========================================================================= */
/* Device Definitions                                                        */
/* ========================================================================= */

spi_nor_device_t w25q256jviq_dual_flash = {
    .jedec_id = 0xEF4019,
    .flash_probe = w25q256jviq_probe_dualbank,
    .flash_is_busy = w25q256jviq_is_busy_dualbank,
    .read_id_op = {0x9F, SPI_LINE_SINGLE, SPI_ADDR_4_BYTE, SPI_LINE_NONE, SPI_LINE_SINGLE, SPI_DUMMY_NONE},
    .read_op = {0xEC, SPI_LINE_SINGLE, SPI_ADDR_4_BYTE, SPI_LINE_QUAD, SPI_LINE_QUAD, SPI_DUMMY_6},
    .program_page_op = {0x34, SPI_LINE_SINGLE, SPI_ADDR_4_BYTE, SPI_LINE_SINGLE, SPI_LINE_QUAD, SPI_DUMMY_NONE},
    .erase_sector_op = {0x21, SPI_LINE_SINGLE, SPI_ADDR_4_BYTE, SPI_LINE_SINGLE, SPI_LINE_NONE, SPI_DUMMY_NONE},
    .write_enable_op = {0x06, SPI_LINE_SINGLE, SPI_ADDR_4_BYTE, SPI_LINE_NONE, SPI_LINE_NONE, SPI_DUMMY_NONE},
};

spi_nor_device_t w25q256jviq_dual_flash_single_line = {
    .jedec_id = 0xEF4019,
    .flash_probe = w25q256jviq_probe_dualbank,
    .flash_is_busy = w25q256jviq_is_busy_dualbank,
    .read_id_op = {0x9F, SPI_LINE_SINGLE, SPI_ADDR_4_BYTE, SPI_LINE_NONE, SPI_LINE_SINGLE, SPI_DUMMY_NONE},
    .read_op = {0x13, SPI_LINE_SINGLE, SPI_ADDR_4_BYTE, SPI_LINE_SINGLE, SPI_LINE_SINGLE, SPI_DUMMY_NONE},
    .program_page_op = {0x12, SPI_LINE_SINGLE, SPI_ADDR_4_BYTE, SPI_LINE_SINGLE, SPI_LINE_SINGLE, SPI_DUMMY_NONE},
    .erase_sector_op = {0x21, SPI_LINE_SINGLE, SPI_ADDR_4_BYTE, SPI_LINE_SINGLE, SPI_LINE_NONE, SPI_DUMMY_NONE},
    .write_enable_op = {0x06, SPI_LINE_SINGLE, SPI_ADDR_4_BYTE, SPI_LINE_NONE, SPI_LINE_NONE, SPI_DUMMY_NONE},
};

spi_nor_device_t w25q64_flash = {
    .jedec_id = 0xEF4017,
    .flash_probe = w25q64_probe,
    .flash_is_busy = w25q64_is_busy,
    /* W25Q64 uses 3-byte addressing mode */
    .read_id_op = {0x9F, SPI_LINE_SINGLE, SPI_ADDR_3_BYTE, SPI_LINE_NONE, SPI_LINE_SINGLE, SPI_DUMMY_NONE},
    .read_op = {0x03, SPI_LINE_SINGLE, SPI_ADDR_3_BYTE, SPI_LINE_SINGLE, SPI_LINE_SINGLE, SPI_DUMMY_NONE},
    .program_page_op = {0x02, SPI_LINE_SINGLE, SPI_ADDR_3_BYTE, SPI_LINE_SINGLE, SPI_LINE_SINGLE, SPI_DUMMY_NONE},
    .erase_sector_op = {0x20, SPI_LINE_SINGLE, SPI_ADDR_3_BYTE, SPI_LINE_SINGLE, SPI_LINE_NONE, SPI_DUMMY_NONE},
    .write_enable_op = {0x06, SPI_LINE_SINGLE, SPI_ADDR_3_BYTE, SPI_LINE_NONE, SPI_LINE_NONE, SPI_DUMMY_NONE},
};

/* driver same with w25q64 */
spi_nor_device_t py25q64_flash = {
    .jedec_id = 0x852017,
    .flash_probe = w25q64_probe,
    .flash_is_busy = w25q64_is_busy,
    /* W25Q64 uses 3-byte addressing mode */
    .read_id_op = {0x9F, SPI_LINE_SINGLE, SPI_ADDR_3_BYTE, SPI_LINE_NONE, SPI_LINE_SINGLE, SPI_DUMMY_NONE},
    .read_op = {0x03, SPI_LINE_SINGLE, SPI_ADDR_3_BYTE, SPI_LINE_SINGLE, SPI_LINE_SINGLE, SPI_DUMMY_NONE},
    .program_page_op = {0x02, SPI_LINE_SINGLE, SPI_ADDR_3_BYTE, SPI_LINE_SINGLE, SPI_LINE_SINGLE, SPI_DUMMY_NONE},
    .erase_sector_op = {0x20, SPI_LINE_SINGLE, SPI_ADDR_3_BYTE, SPI_LINE_SINGLE, SPI_LINE_NONE, SPI_DUMMY_NONE},
    .write_enable_op = {0x06, SPI_LINE_SINGLE, SPI_ADDR_3_BYTE, SPI_LINE_NONE, SPI_LINE_NONE, SPI_DUMMY_NONE},
};

/* ========================================================================= */
/* Private Functions                                                         */
/* ========================================================================= */

static bool w25q256jviq_is_busy_dualbank(const drv_spi_obj_t *spi_drv)
{
    if (NULL == spi_drv)
    {
        return true;
    }

    uint8_t read_buffer[2] = {0};

    /* Read status register 1 */
    std_ret ret = drv_spi_writeread(spi_drv, 0x05, SPI_LINE_SINGLE, 0x0, SPI_LINE_NONE, SPI_ADDR_4_BYTE, SPI_DUMMY_NONE, read_buffer, 2, SPI_LINE_SINGLE, SPI_DATA_IN);
    if (E_OK != ret)
    {
        LOG_ERROR("read status register1 fail");
        return true;
    }

    if ((read_buffer[0] & 0x1) || (read_buffer[1] & 0x1))
    {
        return true;
    }

    return false;
}

static std_ret w25q256jviq_probe_dualbank(drv_spi_obj_t *spi_drv, struct spi_nor_device_t *device)
{
    if (NULL == spi_drv)
    {
        return E_INVALID_PARAM;
    }

    uint8_t read_buffer[6] = {0};
    uint8_t write_buffer[2] = {0};

    std_ret ret = spinor_execute_operation(spi_drv, device->read_id_op, 0x0, &read_buffer[0], 6, SPI_DATA_IN);
    uint32_t jedec1 = read_buffer[0] << 16 | read_buffer[2] << 8 | read_buffer[4];
    uint32_t jedec2 = read_buffer[1] << 16 | read_buffer[3] << 8 | read_buffer[5];
    LOG_INFO("w25q256jviq_init read jedec1 = 0x%08x, jedec2 = 0x%08x, ret = %d", jedec1, jedec2, ret);
    if (E_OK != ret)
    {
        return E_NOK;
    }

    if (jedec1 != jedec2 || jedec1 != device->jedec_id)
    {
        return E_NOK;
    }

    /* Read status register 2 */
    ret = drv_spi_writeread(spi_drv, 0x35, SPI_LINE_SINGLE, 0x0, SPI_LINE_NONE, SPI_ADDR_4_BYTE, SPI_DUMMY_NONE, read_buffer, 2, SPI_LINE_SINGLE, SPI_DATA_IN);
    if (E_OK != ret)
    {
        LOG_ERROR("read status register2 fail");
        return E_NOK;
    }

    LOG_INFO("status chip1 reg1 = 0x%x, chip2 reg2 = 0x%x", read_buffer[0], read_buffer[1]);

    /* Check QE bit */
    if (!(read_buffer[0] & 0x2) || !(read_buffer[1] & 0x2))
    {
        ret = spinor_execute_operation(spi_drv, device->write_enable_op, 0x0, NULL, 0, SPI_DATA_OUT);
        if (E_OK != ret)
        {
            LOG_ERROR("Write enable status register fail");
            return E_NOK;
        }

        /* Enable QE bit */
        write_buffer[0] = read_buffer[0] | 0x2;
        write_buffer[1] = read_buffer[1] | 0x2;
        ret = drv_spi_writeread(spi_drv, 0x31, SPI_LINE_SINGLE, 0x0, SPI_LINE_NONE, SPI_ADDR_4_BYTE, SPI_DUMMY_NONE, write_buffer, 2, SPI_LINE_SINGLE, SPI_DATA_IN);
        if (E_OK != ret)
        {
            LOG_ERROR("write status register2 fail");
            return E_NOK;
        }

        ret = drv_spi_writeread(spi_drv, 0x35, SPI_LINE_SINGLE, 0x0, SPI_LINE_NONE, SPI_ADDR_4_BYTE, SPI_DUMMY_NONE, read_buffer, 2, SPI_LINE_SINGLE, SPI_DATA_IN);
        if (E_OK != ret)
        {
            LOG_ERROR("read back status register2 fail");
            return E_NOK;
        }
        if (!(read_buffer[0] & 0x2) || !(read_buffer[1] & 0x2))
        {
            LOG_ERROR("read back status register2 compare fail, reg1 = 0x%x, reg2 = 0x%x", read_buffer[0], read_buffer[1]);
            return E_NOK;
        }
    }

    /* Enter 4-byte address mode */
    ret = drv_spi_writeread(spi_drv, 0xB7, SPI_LINE_SINGLE, 0x0, SPI_LINE_NONE, SPI_ADDR_4_BYTE, SPI_DUMMY_NONE, NULL, 0, SPI_LINE_NONE, SPI_DATA_IN);
    if (E_OK != ret)
    {
        LOG_ERROR("enter 4byte addr mode fail");
        return E_NOK;
    }

    return E_OK;
}

/* --- W25Q64 Single SPI Implementations --- */

static bool w25q64_is_busy(const drv_spi_obj_t *spi_drv)
{
    if (NULL == spi_drv) return true;

    uint8_t status = 0;

    /* Read status register 1 */
    std_ret ret = drv_spi_writeread(spi_drv, 0x05, SPI_LINE_SINGLE, 0x0, SPI_LINE_NONE, SPI_ADDR_3_BYTE, SPI_DUMMY_NONE, &status, 1, SPI_LINE_SINGLE, SPI_DATA_IN);
    if (E_OK != ret)
    {
        LOG_ERROR("w25q64 read status register1 fail");
        return true;
    }

    /* Check BUSY/WIP bit (bit 0) */
    if (status & 0x01)
    {
        return true;
    }

    return false;
}

static std_ret w25q64_probe(drv_spi_obj_t *spi_drv, struct spi_nor_device_t *device)
{
    if (NULL == spi_drv) return E_INVALID_PARAM;

    uint8_t read_buffer[3] = {0};

    /* Standard read JEDEC ID (0x9F) */
    std_ret ret = spinor_execute_operation(spi_drv, device->read_id_op, 0x0, read_buffer, 3, SPI_DATA_IN);
    if (E_OK != ret) {
        LOG_ERROR("w25q64 read jedec id fail, ret = %d", ret);
        return E_NOK;
    }

    uint32_t jedec = (read_buffer[0] << 16) | (read_buffer[1] << 8) | read_buffer[2];
    
    if (jedec != device->jedec_id)
    {
        LOG_ERROR("w25q64 read jedec id compare fail, read = 0x%08x, expected = 0x%08x", jedec, device->jedec_id);
        return E_NOK;
    }

    LOG_INFO("w25q64_probe success: jedec = 0x%08x", jedec);

    return E_OK;
}

static std_ret spinor_execute_operation(const drv_spi_obj_t *spi_drv, spi_nor_operate_t op, uint32_t addr, uint8_t *data, uint32_t size, drv_spi_direction_t direction)
{
    if (NULL == spi_drv)
    {
        return E_INVALID_PARAM;
    }
    std_ret ret = drv_spi_writeread(spi_drv, op.command, op.cmd_line, addr, op.addr_line, op.addr_mode, op.dummy_cycles, data, size, op.data_line, direction);
    return ret;
}

/* ========================================================================= */
/* Public API Functions                                                      */
/* ========================================================================= */

std_ret spinor_probe_device(const drv_spi_obj_t *spi_drv, spi_nor_device_t **spi_nor)
{
    if (NULL == spi_drv || NULL == spi_nor)
    {
        return E_INVALID_PARAM;
    }

    /* Array of supported devices */
    spi_nor_device_t *supported_devices[] = {
        &w25q256jviq_dual_flash,
        &w25q64_flash,
        &py25q64_flash,
    };

    /* Loop through and probe each device */
    for (uint32_t i = 0; i < sizeof(supported_devices) / sizeof(supported_devices[0]); i++)
    {
        LOG_INFO("Probing for SPI NOR Flash: JEDEC ID 0x%06X", supported_devices[i]->jedec_id);
        if (supported_devices[i]->flash_probe((drv_spi_obj_t *)spi_drv, supported_devices[i]) == E_OK)
        {
            *spi_nor = supported_devices[i];
            LOG_INFO("SPI NOR Flash probed successfully. JEDEC: 0x%06X", supported_devices[i]->jedec_id);
            return E_OK;
        }
    }

    LOG_ERROR("SPI NOR Flash probe failed. No supported device found.");
    return E_NOK;
}

std_ret spinor_erase_sector(const drv_spi_obj_t *spi_drv, const spi_nor_device_t *spi_nor, uint32_t addr)
{
    if (NULL == spi_drv || NULL == spi_nor)
    {
        return E_INVALID_PARAM;
    }

    /* Write enable */
    std_ret ret = spinor_execute_operation(spi_drv, spi_nor->write_enable_op, 0x0, NULL, 0, SPI_DATA_OUT);
    if (E_OK != ret)
    {
        return E_NOK;
    }

    return spinor_execute_operation(spi_drv, spi_nor->erase_sector_op, addr, NULL, 0, SPI_DATA_OUT);
}

std_ret spinor_page_program(const drv_spi_obj_t *spi_drv, const spi_nor_device_t *spi_nor, uint32_t addr, const uint8_t *data, uint32_t size)
{
    if (NULL == spi_drv || NULL == spi_nor)
    {
        return E_INVALID_PARAM;
    }

    /* Write enable */
    std_ret ret = spinor_execute_operation(spi_drv, spi_nor->write_enable_op, 0x0, NULL, 0, SPI_DATA_OUT);
    if (E_OK != ret)
    {
        return E_NOK;
    }

    return spinor_execute_operation(spi_drv, spi_nor->program_page_op, addr, (uint8_t*)data, size, SPI_DATA_OUT);
}

std_ret spinor_read(const drv_spi_obj_t *spi_drv, const spi_nor_device_t *spi_nor, uint32_t addr, uint8_t *data, uint32_t size)
{
    if (NULL == spi_drv || NULL == spi_nor)
    {
        return E_INVALID_PARAM;
    }

    return spinor_execute_operation(spi_drv, spi_nor->read_op, addr, data, size, SPI_DATA_IN);
}

std_ret spinor_wait_not_busy(const drv_spi_obj_t *spi_drv, const spi_nor_device_t *spi_nor, uint32_t timeout_ms)
{
    if (spi_drv == NULL || spi_nor == NULL) return E_INVALID_PARAM;

    uint32_t start = get_tick_ms();

    do {
        if (!spi_nor->flash_is_busy(spi_drv)) {
            return E_OK;
        }
        delay_ms(1);
    } while (get_tick_ms() - start < timeout_ms);

    if (!spi_nor->flash_is_busy(spi_drv)) return E_OK;

    return E_TIMEOUT;
}
