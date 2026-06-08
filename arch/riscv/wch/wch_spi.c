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
 * @file wch_spi.c
 * @brief WCH SPI Driver Implementation
 *
 * This file provides the WCH Standard Library implementation of the SPI driver
 * interface defined in drv_spi.h.
 *
 * @author nickyyy
 * @date 2026-06-07
 */

#include "wch_spi.h"
#include "log.h"
#include "drv_tick.h"

#ifdef USE_WCH_SPI

#define WCH_SPI_TIMEOUT_MS          500u

#if defined CH32H417
    #include "ch32h417.h"
    #include "ch32h417_spi.h"
    #include "ch32h417_gpio.h"
#endif

/* Private function prototypes */
static std_ret wch_spi_init(void* dev);
static std_ret wch_spi_deinit(void* dev);
static std_ret wch_spi_write(void* dev, const uint8_t *data, uint32_t size, drv_spi_lines_t line);
static std_ret wch_spi_read(void* dev, uint8_t *dst, uint32_t size, drv_spi_lines_t line);
static std_ret wch_spi_writeread(void* dev, uint8_t cmd, drv_spi_lines_t cmd_line,
    uint32_t addr, drv_spi_lines_t addr_line, drv_spi_addr_mode_t addr_mode, drv_spi_dummy_cycles_t dummy_cycles,
    uint8_t *data, uint32_t data_size, drv_spi_lines_t data_line, drv_spi_direction_t direction);
static std_ret wch_spi1_set_cs(uint8_t pulse);

static std_ret wch_spi_send_bytes(SPI_TypeDef * spix, const uint8_t *data, uint32_t size, uint32_t timeout);
static std_ret wch_spi_receive_bytes(SPI_TypeDef * spix, uint8_t *data, uint32_t size, uint32_t timeout);

/* ========================================================================= */
/* Public Driver Objects                                                     */
/* ========================================================================= */

#ifdef USE_WCH_SPI1

drv_spi_obj_t drv_spi1_obj = {
    .dev = SPI1,
    .init = wch_spi_init,
    .deinit = wch_spi_deinit,
    .write = wch_spi_write,
    .read = wch_spi_read,
    .writeread = wch_spi_writeread,
    .set_cs = wch_spi1_set_cs
};

#endif /* USE_WCH_SPI1 */

/* ========================================================================= */
/* Private Functions                                                         */
/* ========================================================================= */

static std_ret wch_spi_init(void* dev)
{
    if (NULL == dev)
    {
        return E_INVALID_PARAM;
    }
    if (dev == SPI1) {
        // do nothing
    }
    return E_OK;
}

static std_ret wch_spi_deinit(void* dev)
{
    if (NULL == dev)
    {
        return E_INVALID_PARAM;
    }
    if (dev == SPI1) {
        SPI_Cmd(dev, DISABLE);
    }
    return E_OK;
}

static std_ret wch_spi_write(void* dev, const uint8_t *data, uint32_t size, drv_spi_lines_t line)
{
    if (NULL == dev || NULL == data || size == 0)
    {
        return E_INVALID_PARAM;
    }
    
    return wch_spi_send_bytes((SPI_TypeDef *)dev, data, size, WCH_SPI_TIMEOUT_MS);
}

static std_ret wch_spi_read(void* dev, uint8_t *dst, uint32_t size, drv_spi_lines_t line)
{
    if (NULL == dev || NULL == dst || size == 0)
    {
        return E_INVALID_PARAM;
    }
    return wch_spi_receive_bytes((SPI_TypeDef *)dev, dst, size, WCH_SPI_TIMEOUT_MS);
}

static std_ret wch_spi_writeread(void* dev, uint8_t cmd, drv_spi_lines_t cmd_line,
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
    std_ret hal_ret = E_OK;
    uint8_t tx_buf[5];
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

    // 3. 发送 Command（和可能的 Address）
    if (tx_len > 0) {
        hal_ret = wch_spi_send_bytes((SPI_TypeDef *)dev, tx_buf, tx_len, WCH_SPI_TIMEOUT_MS);
        if (hal_ret != E_OK) {
            LOG_ERROR("%s:%d wch_spi_send_bytes cmd/addr return:%d", __FILE_NAME__, __LINE__, hal_ret);
            return E_NOK;
        }
    }

    // 4. 数据发送/接收阶段
    if (data_size != 0 && data_line != SPI_LINE_NONE) {
        if (direction == SPI_DATA_IN) {
            hal_ret = wch_spi_receive_bytes((SPI_TypeDef *)dev, data, data_size, WCH_SPI_TIMEOUT_MS);
            if (hal_ret != E_OK) {
                LOG_ERROR("%s:%d wch_spi_receive_bytes return:%d", __FILE_NAME__, __LINE__, hal_ret);
                return E_NOK;
            }
        } else if (direction == SPI_DATA_OUT) {
            hal_ret = wch_spi_send_bytes((SPI_TypeDef *)dev, data, data_size, WCH_SPI_TIMEOUT_MS);
            if (hal_ret != E_OK) {
                LOG_ERROR("%s:%d wch_spi_send_bytes data return:%d", __FILE_NAME__, __LINE__, hal_ret);
                return E_NOK;
            }
        } else {
            return E_INVALID_PARAM;
        }
    }

    return E_OK;
}

static std_ret wch_spi1_set_cs(uint8_t pulse)
{
    GPIO_WriteBit( GPIOA, GPIO_Pin_4, pulse == 0 ? Bit_RESET : Bit_SET );
    return E_OK;
}

/**
 * @brief 纯发送，不处理RX。
 *        全双工模式下同时会收到数据，由上层决定是否清除。
 */
static std_ret wch_spi_send_bytes(SPI_TypeDef * spix, const uint8_t *data, uint32_t size, uint32_t timeout)
{
    std_ret ret = E_OK;
    uint32_t start_tick = get_tick_ms();
    for (uint32_t i = 0; i < size; i++) {
        while (SPI_I2S_GetFlagStatus(spix, SPI_I2S_FLAG_TXE) == RESET) {
            if ((get_tick_ms() - start_tick) >= timeout) {
                return E_TIMEOUT;
            }
        }
        SPI_I2S_SendData(spix, data[i]);

        while (SPI_I2S_GetFlagStatus(spix, SPI_I2S_FLAG_RXNE) == RESET) {
            if ((get_tick_ms() - start_tick) >= timeout) {
                return E_TIMEOUT;
            }
        }
        (void)SPI_I2S_ReceiveData(spix);
    }
    return ret;
}

/**
 * @brief 发送dummy字节产生SCK再接收数据。
 *        每次循环写0xFF触发8个SCK脉冲，Flash将数据驱动到MISO。
 */
static std_ret wch_spi_receive_bytes(SPI_TypeDef * spix, uint8_t *data, uint32_t size, uint32_t timeout)
{
    std_ret ret = E_OK;
    uint32_t start_tick = get_tick_ms();
    for (uint32_t i = 0; i < size; i++) {
        while (SPI_I2S_GetFlagStatus(spix, SPI_I2S_FLAG_TXE) == RESET) {
            if ((get_tick_ms() - start_tick) >= timeout) {
                return E_TIMEOUT;
            }
        }
        SPI_I2S_SendData(spix, 0xFF);
        while (SPI_I2S_GetFlagStatus(spix, SPI_I2S_FLAG_RXNE) == RESET) {
            if ((get_tick_ms() - start_tick) >= timeout) {
                return E_TIMEOUT;
            }
        }
        data[i] = (uint8_t)SPI_I2S_ReceiveData(spix);
    }
    return ret;
}

#endif /* USE_WCH_SPI */