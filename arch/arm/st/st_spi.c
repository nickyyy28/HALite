/*
 * Copyright (c) 2026 nickyyy
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
 * @file st_spi.c
 * @brief STM32 SPI Driver Implementation
 *
 * This file provides the STM32 HAL-based implementation of the SPI driver
 * interface defined in drv_spi.h.
 *
 * @author nickyyy
 * @date 2026-06-07
 */

#include "st_spi.h"
#include "log.h"


#ifdef USE_ST_SPI

#if __has_include("spi.h")
    #include "spi.h"
#else
    #error "spi.h not found. Please ensure the correct HAL SPI header is included."
#endif

#define ST_SPI_TIMEOUT_MS          500u

#if defined STM32H743xx
    #include "stm32h7xx_hal.h"
    #include "stm32h7xx_hal_spi.h"
#endif

#if defined STM32F411xx || defined STM32F411xE
    #include "stm32f4xx_hal.h"
    #include "stm32f4xx_hal_spi.h"
    #include "stm32f4xx_hal_gpio.h"
#endif

/* Private function prototypes */
static std_ret st_spi_init(void* dev);
static std_ret st_spi_deinit(void* dev);
static std_ret st_spi_write(void* dev, const uint8_t *data, uint32_t size, drv_spi_lines_t line);
static std_ret st_spi_read(void* dev, uint8_t *dst, uint32_t size, drv_spi_lines_t line);
static std_ret st_spi_writeread(void* dev, uint8_t cmd, drv_spi_lines_t cmd_line,
    uint32_t addr, drv_spi_lines_t addr_line, drv_spi_addr_mode_t addr_mode, drv_spi_dummy_cycles_t dummy_cycles,
    uint8_t *data, uint32_t data_size, drv_spi_lines_t data_line, drv_spi_direction_t direction);
static std_ret st_spi1_set_cs(uint8_t pulse);

/* ========================================================================= */
/* Public Driver Objects                                                     */
/* ========================================================================= */

#ifdef USE_ST_SPI1

drv_spi_obj_t drv_spi1_obj = {
    .dev = &hspi1,
    .init = st_spi_init,
    .deinit = st_spi_deinit,
    .write = st_spi_write,
    .read = st_spi_read,
    .writeread = st_spi_writeread,
    .set_cs = st_spi1_set_cs
};

#endif /* USE_ST_SPI1 */

/* ========================================================================= */
/* Private Functions                                                         */
/* ========================================================================= */

static std_ret st_spi_init(void* dev)
{
    if (NULL == dev)
    {
        return E_INVALID_PARAM;
    }
    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)dev;
    if (hspi->Instance == SPI1) {
        MX_SPI1_Init();
    }
    return E_OK;
}

static std_ret st_spi_deinit(void* dev)
{
    if (NULL == dev)
    {
        return E_INVALID_PARAM;
    }
    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)dev;
    if (hspi->Instance == SPI1) {
        HAL_SPI_MspDeInit(dev);
    }
    return E_OK;
}

static std_ret st_spi_write(void* dev, const uint8_t *data, uint32_t size, drv_spi_lines_t line)
{
    if (NULL == dev || NULL == data || size == 0)
    {
        return E_INVALID_PARAM;
    }
    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)dev;
    if (HAL_SPI_Transmit(hspi, (uint8_t *)data, size, 100) != HAL_OK)
    {
        return E_NOK;
    }
    return E_OK;
}

static std_ret st_spi_read(void* dev, uint8_t *dst, uint32_t size, drv_spi_lines_t line)
{
    if (NULL == dev || NULL == dst || size == 0)
    {
        return E_INVALID_PARAM;
    }
    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)dev;
    if (HAL_SPI_Receive(hspi, dst, size, 100) != HAL_OK)
    {
        return E_NOK;
    }
    return E_OK;
}

static std_ret st_spi_writeread(void* dev, uint8_t cmd, drv_spi_lines_t cmd_line,
    uint32_t addr, drv_spi_lines_t addr_line, drv_spi_addr_mode_t addr_mode, drv_spi_dummy_cycles_t dummy_cycles,
    uint8_t *data, uint32_t data_size, drv_spi_lines_t data_line, drv_spi_direction_t direction)
{
    if (NULL == dev) {
        LOG_ERROR("%s:%d dev is NULL", __FUNCTION__, __LINE__);
        return E_INVALID_PARAM;
    }

    if (NULL == data && (data_size != 0 || data_line != SPI_LINE_NONE)) {
        LOG_ERROR("%s:%d data = 0x%x, data_size = %d, data_line = %d", __FUNCTION__, __LINE__, data, data_size, data_line);
        return E_INVALID_PARAM;
    }

    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)dev;

    // 检查 SPI 状态
    if (HAL_SPI_GetState(hspi) != HAL_SPI_STATE_READY) {
        return E_BUSY;
    }

    HAL_StatusTypeDef hal_ret;
    uint8_t tx_buf[5]; // 最大 1字节命令 + 4字节地址
    uint16_t tx_len = 0;

    // 1. 组装 Command
    if (cmd_line != SPI_LINE_NONE) {
        tx_buf[tx_len++] = cmd;
    }

    // 2. 组装 Address (Flash 通常要求 MSB First 高位在前)
    if (addr_line != SPI_LINE_NONE) {
        if (addr_mode == SPI_ADDR_3_BYTE) {
            tx_buf[tx_len++] = (uint8_t)((addr >> 16) & 0xFF);
            tx_buf[tx_len++] = (uint8_t)((addr >> 8) & 0xFF);
            tx_buf[tx_len++] = (uint8_t)(addr & 0xFF);
        } else if (addr_mode == SPI_ADDR_4_BYTE) {
            tx_buf[tx_len++] = (uint8_t)((addr >> 24) & 0xFF);
            tx_buf[tx_len++] = (uint8_t)((addr >> 16) & 0xFF);
            tx_buf[tx_len++] = (uint8_t)((addr >> 8) & 0xFF);
            tx_buf[tx_len++] = (uint8_t)(addr & 0xFF);
        } else {
            return E_INVALID_PARAM;
        }
    }

    // 3. 发送 Command 和 Address
    if (tx_len > 0) {
        // 假设定义了 ST_SPI_TIMEOUT_MS，或者你可以直接用 HAL_MAX_DELAY
        hal_ret = HAL_SPI_Transmit(hspi, tx_buf, tx_len, ST_SPI_TIMEOUT_MS); 
        if (hal_ret != HAL_OK) {
            LOG_ERROR("%s:%d HAL_SPI_Transmit cmd/addr return:%d", __FILE_NAME__, __LINE__, hal_ret);
            return E_NOK;
        }
    }

    // 4. 数据发送/接收阶段
    if (data_size != 0 && data_line != SPI_LINE_NONE) {
        if (direction == SPI_DATA_IN) {
            hal_ret = HAL_SPI_Receive(hspi, data, data_size, ST_SPI_TIMEOUT_MS);
            if (hal_ret != HAL_OK) {
                LOG_ERROR("%s:%d HAL_SPI_Receive return:%d", __FILE_NAME__, __LINE__, hal_ret);
                return E_NOK;
            }
        } else if (direction == SPI_DATA_OUT) {
            hal_ret = HAL_SPI_Transmit(hspi, data, data_size, ST_SPI_TIMEOUT_MS);
            if (hal_ret != HAL_OK) {
                LOG_ERROR("%s:%d HAL_SPI_Transmit data return:%d", __FILE_NAME__, __LINE__, hal_ret);
                return E_NOK;
            }
        } else {
            return E_INVALID_PARAM;
        }
    }

    return E_OK;
}

static std_ret st_spi1_set_cs(uint8_t pulse)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, pulse == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
    return E_OK;
}

#endif /* USE_ST_SPI */
