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
 * @file st_qspi.c
 * @brief STM32 QSPI Driver Implementation
 *
 * This file provides the STM32 HAL-based implementation of the QSPI driver
 * interface defined in drv_spi.h.
 *
 * @author nickyyy
 * @date 2025-04-17
 */

#include "st_qspi.h"
#include "log.h"

#ifdef USE_ST_QSPI

#if __has_include("quadspi.h")
    #include "quadspi.h"
#else
    #error "quadspi.h not found. Please ensure the correct HAL QSPI header is included."
#endif

#define ST_QSPI_TIMEOUT_MS          500u

/* Private function prototypes */
static std_ret st_qspi_init(void* dev);
static std_ret st_qspi_deinit(void* dev);
static std_ret st_qspi_write(void* dev, const uint8_t *data, uint32_t size, drv_spi_lines_t line);
static std_ret st_qspi_read(void* dev, uint8_t *dst, uint32_t size, drv_spi_lines_t line);
static std_ret st_qspi_writeread(void* dev, uint8_t cmd, drv_spi_lines_t cmd_line,
    uint32_t addr, drv_spi_lines_t addr_line, drv_spi_addr_mode_t addr_mode, drv_spi_dummy_cycles_t dummy_cycles,
    uint8_t *data, uint32_t data_size, drv_spi_lines_t data_line, drv_spi_direction_t direction);

#define WRAP_MODE(mode) case mode: return #mode

const char* addrmode2str(uint32_t var)
{
    switch (var)
    {
        WRAP_MODE(QSPI_ADDRESS_NONE);
        WRAP_MODE(QSPI_ADDRESS_1_LINE);
        WRAP_MODE(QSPI_ADDRESS_2_LINES);
        WRAP_MODE(QSPI_ADDRESS_4_LINES);
        default:
        return "Unknown";
    }
}

const char* addrsize2str(uint32_t var)
{
    switch (var)
    {
        WRAP_MODE(QSPI_ADDRESS_8_BITS);
        WRAP_MODE(QSPI_ADDRESS_16_BITS);
        WRAP_MODE(QSPI_ADDRESS_24_BITS);
        WRAP_MODE(QSPI_ADDRESS_32_BITS);
    default:
        return "Unknown";
    }
}

const char* instmode2str(uint32_t var)
{
    switch (var)
    {
        WRAP_MODE(QSPI_INSTRUCTION_NONE);
        WRAP_MODE(QSPI_INSTRUCTION_1_LINE);
        WRAP_MODE(QSPI_INSTRUCTION_2_LINES);
        WRAP_MODE(QSPI_INSTRUCTION_4_LINES);
    default:
        return "Unknown";
    }
}

const char* datamode2str(uint32_t var)
{
    switch (var)
    {
        WRAP_MODE(QSPI_DATA_NONE);
        WRAP_MODE(QSPI_DATA_1_LINE);
        WRAP_MODE(QSPI_DATA_2_LINES);
        WRAP_MODE(QSPI_DATA_4_LINES);
    default:
        return "Unknown";
    }
}

/* ========================================================================= */
/* Public Driver Objects                                                     */
/* ========================================================================= */

drv_spi_obj_t drv_qspi_obj1 = {
    .dev = &hqspi,
    .init = st_qspi_init,
    .deinit = st_qspi_deinit,
    .read = st_qspi_read,
    .write = st_qspi_write,
    .writeread = st_qspi_writeread,
    .timeout_ms = 100
};

/* ========================================================================= */
/* Private Functions                                                         */
/* ========================================================================= */

static std_ret st_qspi_init(void* dev)
{
    if (NULL == dev)
    {
        return E_INVALID_PARAM;
    }
    MX_QUADSPI_Init();
    return E_OK;
}

static std_ret st_qspi_deinit(void* dev)
{
    if (NULL == dev)
    {
        return E_INVALID_PARAM;
    }
    HAL_QSPI_MspDeInit(dev);
    return E_OK;
}

static std_ret st_qspi_write(void* dev, const uint8_t *data, uint32_t size, drv_spi_lines_t line)
{
    if (NULL == dev || NULL == data || size == 0)
    {
        return E_INVALID_PARAM;
    }
    QSPI_HandleTypeDef *hqspi = (QSPI_HandleTypeDef *)dev;
    if (HAL_QSPI_GetState(hqspi) != HAL_QSPI_STATE_READY)
    {
        return E_BUSY;
    }

    uint32_t data_mode;
    switch (line) {
    case SPI_LINE_SINGLE:
        data_mode = QSPI_DATA_1_LINE;
        break;
    case SPI_LINE_DUAL:
        data_mode = QSPI_DATA_2_LINES;
        break;
    case SPI_LINE_QUAD:
        data_mode = QSPI_DATA_4_LINES;
        break;
    default:
        return E_INVALID_PARAM;
    }

    QSPI_CommandTypeDef qspi_cmd = {0};
    qspi_cmd.InstructionMode   = QSPI_INSTRUCTION_NONE;
    qspi_cmd.AddressMode       = QSPI_ADDRESS_NONE;
    qspi_cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    qspi_cmd.DataMode          = data_mode;
    qspi_cmd.DummyCycles       = 0;
    qspi_cmd.NbData            = size;
    qspi_cmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
    qspi_cmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    HAL_StatusTypeDef hal_ret = HAL_QSPI_Command(hqspi, &qspi_cmd, ST_QSPI_TIMEOUT_MS);
    if (hal_ret != HAL_OK) {
        return E_NOK;
    }

    hal_ret = HAL_QSPI_Transmit(hqspi, (uint8_t *)data, ST_QSPI_TIMEOUT_MS);
    if (hal_ret != HAL_OK) {
        return E_NOK;
    }

    return E_OK;
}

static std_ret st_qspi_read(void* dev, uint8_t *dst, uint32_t size, drv_spi_lines_t line)
{
    if (NULL == dev || NULL == dst || size == 0)
    {
        return E_INVALID_PARAM;
    }
    QSPI_HandleTypeDef *hqspi = (QSPI_HandleTypeDef *)dev;
    if (HAL_QSPI_GetState(hqspi) != HAL_QSPI_STATE_READY)
    {
        return E_BUSY;
    }

    uint32_t data_mode;
    switch (line) {
    case SPI_LINE_SINGLE:
        data_mode = QSPI_DATA_1_LINE;
        break;
    case SPI_LINE_DUAL:
        data_mode = QSPI_DATA_2_LINES;
        break;
    case SPI_LINE_QUAD:
        data_mode = QSPI_DATA_4_LINES;
        break;
    default:
        return E_INVALID_PARAM;
    }

    QSPI_CommandTypeDef qspi_cmd = {0};
    qspi_cmd.InstructionMode   = QSPI_INSTRUCTION_NONE;
    qspi_cmd.AddressMode       = QSPI_ADDRESS_NONE;
    qspi_cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    qspi_cmd.DataMode          = data_mode;
    qspi_cmd.DummyCycles       = 0;
    qspi_cmd.NbData            = size;
    qspi_cmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
    qspi_cmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    HAL_StatusTypeDef hal_ret = HAL_QSPI_Command(hqspi, &qspi_cmd, ST_QSPI_TIMEOUT_MS);
    if (hal_ret != HAL_OK) {
        LOG_ERROR("%s:%d HAL_QSPI_Command return:%d", __FILE_NAME__, __LINE__, hal_ret);
        return E_NOK;
    }

    hal_ret = HAL_QSPI_Receive(hqspi, (uint8_t *)dst, ST_QSPI_TIMEOUT_MS);
    if (hal_ret != HAL_OK) {
        LOG_ERROR("%s:%d HAL_QSPI_Receive return:%d", __FILE_NAME__, __LINE__, hal_ret);
        return E_NOK;
    }

    return E_OK;
}

static std_ret st_qspi_writeread(void* dev, uint8_t cmd, drv_spi_lines_t cmd_line,
    uint32_t addr, drv_spi_lines_t addr_line, drv_spi_addr_mode_t addr_mode, drv_spi_dummy_cycles_t dummy_cycles,
    uint8_t *data, uint32_t data_size, drv_spi_lines_t data_line, drv_spi_direction_t direction)
{
    if (NULL == dev) {
        return E_INVALID_PARAM;
    }

    if (NULL == data && (data_size != 0 || data_line != SPI_LINE_NONE))
    {
        return E_INVALID_PARAM;
    }

    QSPI_HandleTypeDef *hqspi = (QSPI_HandleTypeDef *)dev;

    if (HAL_QSPI_GetState(hqspi) != HAL_QSPI_STATE_READY) {
        return E_BUSY;
    }
    uint32_t hal_inst_mode;
    switch (cmd_line) {
        case SPI_LINE_NONE:
            hal_inst_mode = QSPI_INSTRUCTION_NONE;
            break;
        case SPI_LINE_SINGLE:
            hal_inst_mode = QSPI_INSTRUCTION_1_LINE;
            break;
        case SPI_LINE_DUAL:
            hal_inst_mode = QSPI_INSTRUCTION_2_LINES;
            break;
        case SPI_LINE_QUAD:
            hal_inst_mode = QSPI_INSTRUCTION_4_LINES;
            break;
        default:
            return E_INVALID_PARAM;
    }
    uint32_t hal_addr_mode;
    uint32_t hal_addr_size;
    switch (addr_mode) {
        case SPI_ADDR_3_BYTE:
            hal_addr_size = QSPI_ADDRESS_24_BITS;
            break;
        case SPI_ADDR_4_BYTE:
            hal_addr_size = QSPI_ADDRESS_32_BITS;
            break;
        default:
            return E_INVALID_PARAM;
    }

    switch (addr_line)
    {
        case  SPI_LINE_NONE:
            hal_addr_mode = QSPI_ADDRESS_NONE;
            break;
        case SPI_LINE_SINGLE:
            hal_addr_mode = QSPI_ADDRESS_1_LINE;
            break;
        case SPI_LINE_DUAL:
            hal_addr_mode = QSPI_ADDRESS_2_LINES;
            break;
        case SPI_LINE_QUAD:
            hal_addr_mode = QSPI_ADDRESS_4_LINES;
            break;
        default:
            return E_INVALID_PARAM;
    }

    uint32_t hal_data_mode;
    switch (data_line) {
        case  SPI_LINE_NONE:
            hal_data_mode = QSPI_DATA_NONE;
            break;
        case SPI_LINE_SINGLE:
            hal_data_mode = QSPI_DATA_1_LINE;
            break;
        case SPI_LINE_DUAL:
            hal_data_mode = QSPI_DATA_2_LINES;
            break;
        case SPI_LINE_QUAD:
            hal_data_mode = QSPI_DATA_4_LINES;
            break;
        default:
            return E_INVALID_PARAM;
    }

    QSPI_CommandTypeDef qspi_cmd = {0};
    qspi_cmd.Instruction         = cmd;
    qspi_cmd.InstructionMode     = hal_inst_mode;
    qspi_cmd.Address             = addr;
    qspi_cmd.AddressMode         = hal_addr_mode;
    qspi_cmd.AddressSize         = hal_addr_size;
    qspi_cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
    qspi_cmd.DataMode            = hal_data_mode;
    qspi_cmd.DummyCycles         = (uint32_t)dummy_cycles;
    qspi_cmd.NbData              = data_size;
    qspi_cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
    qspi_cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;

    //log out
    // LOG_DEBUG("cmd: 0x%x, cmd mode: %s, addr: 0x%x, addr mode: %s, addr size: %s, data size: %d, data mode: %s, dummy cycle: %d, direction: %s",
    //     cmd, instmode2str(hal_inst_mode), addr, addrmode2str(hal_addr_mode), addrsize2str(hal_addr_size), data_size, datamode2str(hal_data_mode), dummy_cycles, direction == SPI_DATA_IN ? "in" : "out");

    HAL_StatusTypeDef hal_ret = HAL_QSPI_Command(hqspi, &qspi_cmd, ST_QSPI_TIMEOUT_MS);
    if (hal_ret != HAL_OK) {
        LOG_ERROR("%s:%d HAL_QSPI_Command return:%d", __FILE_NAME__, __LINE__, hal_ret);
        return E_NOK;
    }

    if (data_size != 0)
    {
        if (direction == SPI_DATA_IN)
        {
            hal_ret = HAL_QSPI_Receive(hqspi, data, ST_QSPI_TIMEOUT_MS);
            if (hal_ret != HAL_OK) {
                LOG_ERROR("%s:%d HAL_QSPI_Receive return:%d", __FILE_NAME__, __LINE__, hal_ret);
                return E_NOK;
            }
        } else if (direction == SPI_DATA_OUT)
        {
            hal_ret = HAL_QSPI_Transmit(hqspi, data, ST_QSPI_TIMEOUT_MS);
            if (hal_ret != HAL_OK) {
                LOG_ERROR("%s:%d HAL_QSPI_Transmit return:%d", __FILE_NAME__, __LINE__, hal_ret);
                return E_NOK;
            }
        } else
        {
            return E_INVALID_PARAM;
        }
    }

    return E_OK;
}

#endif /* USE_ST_QSPI */
