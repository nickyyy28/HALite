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
 * @file st_uart.c
 * @brief STM32 UART Driver Implementation
 *
 * This file provides the STM32 HAL-based implementation of the UART driver
 * interface defined in drv_uart.h.
 *
 * @author nickyyy
 * @date 2025-04-24
 */

#include "st_uart.h"
#include <stddef.h>

/* Private function prototypes */
static std_ret st_uart_init(void *dev, const drv_uart_config_t *config);
static std_ret st_uart_deinit(void *dev);
static std_ret st_uart_transmit(void *dev, const uint8_t *data, uint32_t size, uint32_t timeout_ms);
static std_ret st_uart_receive(void *dev, uint8_t *buffer, uint32_t size, uint32_t timeout_ms);
static std_ret st_uart_transmit_dma(void *dev, const uint8_t *data, uint32_t size);
static std_ret st_uart_receive_dma(void *dev, uint8_t *buffer, uint32_t size);
static std_ret st_uart_set_baudrate(void *dev, uint32_t baudrate);
static std_ret st_uart_set_flow_control(void *dev, drv_uart_flow_control_t flow_control);
static bool st_uart_is_tx_ready(void *dev);
static bool st_uart_is_rx_available(void *dev);

/* Private helper functions */
static uint32_t get_hal_word_length(drv_uart_data_width_t data_width);
static uint32_t get_hal_stop_bits(drv_uart_stop_bits_t stop_bits);
static uint32_t get_hal_parity(drv_uart_parity_t parity);
static uint32_t get_hal_flow_control(drv_uart_flow_control_t flow_control);

/* ========================================================================= */
/* Public Driver Objects                                                     */
/* ========================================================================= */

#if defined(USART1) && defined(USE_UART1)
drv_uart_obj_t drv_uart1_obj = {
    .dev = &huart1,
    .init = st_uart_init,
    .deinit = st_uart_deinit,
    .transmit = st_uart_transmit,
    .receive = st_uart_receive,
    .transmit_dma = st_uart_transmit_dma,
    .receive_dma = st_uart_receive_dma,
    .set_baudrate = st_uart_set_baudrate,
    .set_flow_control = st_uart_set_flow_control,
    .is_tx_ready = st_uart_is_tx_ready,
    .is_rx_available = st_uart_is_rx_available,
    .tx_fifo_size = 256,
    .rx_fifo_size = 256,
    .timeout_ms = 1000
};
#endif

#if defined(USART2) && defined(USE_UART2)
drv_uart_obj_t drv_uart2_obj = {
    .dev = &huart2,
    .init = st_uart_init,
    .deinit = st_uart_deinit,
    .transmit = st_uart_transmit,
    .receive = st_uart_receive,
    .transmit_dma = st_uart_transmit_dma,
    .receive_dma = st_uart_receive_dma,
    .set_baudrate = st_uart_set_baudrate,
    .set_flow_control = st_uart_set_flow_control,
    .is_tx_ready = st_uart_is_tx_ready,
    .is_rx_available = st_uart_is_rx_available,
    .tx_fifo_size = 256,
    .rx_fifo_size = 256,
    .timeout_ms = 1000
};
#endif

#if defined(USART3) && defined(USE_UART3)
drv_uart_obj_t drv_uart3_obj = {
    .dev = &huart3,
    .init = st_uart_init,
    .deinit = st_uart_deinit,
    .transmit = st_uart_transmit,
    .receive = st_uart_receive,
    .transmit_dma = st_uart_transmit_dma,
    .receive_dma = st_uart_receive_dma,
    .set_baudrate = st_uart_set_baudrate,
    .set_flow_control = st_uart_set_flow_control,
    .is_tx_ready = st_uart_is_tx_ready,
    .is_rx_available = st_uart_is_rx_available,
    .tx_fifo_size = 256,
    .rx_fifo_size = 256,
    .timeout_ms = 1000
};
#endif

#if defined(UART4) && defined(USE_UART4)
drv_uart_obj_t drv_uart4_obj = {
    .dev = &huart4,
    .init = st_uart_init,
    .deinit = st_uart_deinit,
    .transmit = st_uart_transmit,
    .receive = st_uart_receive,
    .transmit_dma = st_uart_transmit_dma,
    .receive_dma = st_uart_receive_dma,
    .set_baudrate = st_uart_set_baudrate,
    .set_flow_control = st_uart_set_flow_control,
    .is_tx_ready = st_uart_is_tx_ready,
    .is_rx_available = st_uart_is_rx_available,
    .tx_fifo_size = 256,
    .rx_fifo_size = 256,
    .timeout_ms = 1000
};
#endif

#if defined(UART5) && defined(USE_UART5)
drv_uart_obj_t drv_uart5_obj = {
    .dev = &huart5,
    .init = st_uart_init,
    .deinit = st_uart_deinit,
    .transmit = st_uart_transmit,
    .receive = st_uart_receive,
    .transmit_dma = st_uart_transmit_dma,
    .receive_dma = st_uart_receive_dma,
    .set_baudrate = st_uart_set_baudrate,
    .set_flow_control = st_uart_set_flow_control,
    .is_tx_ready = st_uart_is_tx_ready,
    .is_rx_available = st_uart_is_rx_available,
    .tx_fifo_size = 256,
    .rx_fifo_size = 256,
    .timeout_ms = 1000
};
#endif

#if defined(USART6) && defined(USE_UART6)
drv_uart_obj_t drv_uart6_obj = {
    .dev = &huart6,
    .init = st_uart_init,
    .deinit = st_uart_deinit,
    .transmit = st_uart_transmit,
    .receive = st_uart_receive,
    .transmit_dma = st_uart_transmit_dma,
    .receive_dma = st_uart_receive_dma,
    .set_baudrate = st_uart_set_baudrate,
    .set_flow_control = st_uart_set_flow_control,
    .is_tx_ready = st_uart_is_tx_ready,
    .is_rx_available = st_uart_is_rx_available,
    .tx_fifo_size = 256,
    .rx_fifo_size = 256,
    .timeout_ms = 1000
};
#endif

/* ========================================================================= */
/* Private Functions                                                         */
/* ========================================================================= */

/**
 * @brief Convert data width enum to HAL word length
 */
static uint32_t get_hal_word_length(drv_uart_data_width_t data_width)
{
    switch (data_width)
    {
        case UART_DATA_WIDTH_7_BIT:
            return UART_WORDLENGTH_7B;
        case UART_DATA_WIDTH_8_BIT:
            return UART_WORDLENGTH_8B;
        case UART_DATA_WIDTH_9_BIT:
            return UART_WORDLENGTH_9B;
        default:
            return UART_WORDLENGTH_8B;
    }
}

/**
 * @brief Convert stop bits enum to HAL stop bits
 */
static uint32_t get_hal_stop_bits(drv_uart_stop_bits_t stop_bits)
{
    switch (stop_bits)
    {
        case UART_STOP_BITS_1:
            return UART_STOPBITS_1;
        case UART_STOP_BITS_1_5:
            return UART_STOPBITS_1_5;
        case UART_STOP_BITS_2:
            return UART_STOPBITS_2;
        default:
            return UART_STOPBITS_1;
    }
}

/**
 * @brief Convert parity enum to HAL parity
 */
static uint32_t get_hal_parity(drv_uart_parity_t parity)
{
    switch (parity)
    {
        case UART_PARITY_NONE:
            return UART_PARITY_NONE;
        case UART_PARITY_ODD:
            return UART_PARITY_ODD;
        case UART_PARITY_EVEN:
            return UART_PARITY_EVEN;
        default:
            return UART_PARITY_NONE;
    }
}

/**
 * @brief Convert flow control enum to HAL flow control
 */
static uint32_t get_hal_flow_control(drv_uart_flow_control_t flow_control)
{
    switch (flow_control)
    {
        case UART_FLOW_CONTROL_NONE:
            return UART_HWCONTROL_NONE;
        case UART_FLOW_CONTROL_RTS:
            return UART_HWCONTROL_RTS;
        case UART_FLOW_CONTROL_CTS:
            return UART_HWCONTROL_CTS;
        case UART_FLOW_CONTROL_RTS_CTS:
            return UART_HWCONTROL_RTS_CTS;
        default:
            return UART_HWCONTROL_NONE;
    }
}

/**
 * @brief Initialize UART with configuration
 */
static std_ret st_uart_init(void *dev, const drv_uart_config_t *config)
{
    if (NULL == dev || NULL == config)
    {
        return E_INVALID_PARAM;
    }

    UART_HandleTypeDef *huart = (UART_HandleTypeDef *)dev;

    /* Configure UART parameters */
    huart->Init.BaudRate = config->baudrate;
    huart->Init.WordLength = get_hal_word_length(config->data_width);
    huart->Init.StopBits = get_hal_stop_bits(config->stop_bits);
    huart->Init.Parity = get_hal_parity(config->parity);
    huart->Init.Mode = UART_MODE_TX_RX;
    huart->Init.HwFlowCtl = get_hal_flow_control(config->flow_control);
    huart->Init.OverSampling = UART_OVERSAMPLING_16;

#if defined(USART_CR1_FIFOEN)
    /* Enable FIFO mode if available */
    huart->Init.FIFOMode = UART_FIFOMODE_ENABLE;
    huart->Init.TXFIFOThreshold = UART_TXFIFO_THRESHOLD_1_8;
    huart->Init.RXFIFOThreshold = UART_RXFIFO_THRESHOLD_1_8;
#endif

    HAL_StatusTypeDef hal_ret = HAL_UART_Init(huart);
    if (hal_ret != HAL_OK)
    {
        return E_NOK;
    }

    return E_OK;
}

/**
 * @brief Deinitialize UART
 */
static std_ret st_uart_deinit(void *dev)
{
    if (NULL == dev)
    {
        return E_INVALID_PARAM;
    }

    UART_HandleTypeDef *huart = (UART_HandleTypeDef *)dev;

    HAL_StatusTypeDef hal_ret = HAL_UART_DeInit(huart);
    if (hal_ret != HAL_OK)
    {
        return E_NOK;
    }

    return E_OK;
}

/**
 * @brief Transmit data in blocking mode
 */
static std_ret st_uart_transmit(void *dev, const uint8_t *data, uint32_t size, uint32_t timeout_ms)
{
    if (NULL == dev || NULL == data || size == 0)
    {
        return E_INVALID_PARAM;
    }

    UART_HandleTypeDef *huart = (UART_HandleTypeDef *)dev;

    HAL_StatusTypeDef hal_ret = HAL_UART_Transmit(huart, (uint8_t *)data, size, timeout_ms);

    switch (hal_ret)
    {
        case HAL_OK:
            return E_OK;
        case HAL_TIMEOUT:
            return E_TIMEOUT;
        case HAL_BUSY:
            return E_BUSY;
        default:
            return E_NOK;
    }
}

/**
 * @brief Receive data in blocking mode
 */
static std_ret st_uart_receive(void *dev, uint8_t *buffer, uint32_t size, uint32_t timeout_ms)
{
    if (NULL == dev || NULL == buffer || size == 0)
    {
        return E_INVALID_PARAM;
    }

    UART_HandleTypeDef *huart = (UART_HandleTypeDef *)dev;

    HAL_StatusTypeDef hal_ret = HAL_UART_Receive(huart, buffer, size, timeout_ms);

    switch (hal_ret)
    {
        case HAL_OK:
            return E_OK;
        case HAL_TIMEOUT:
            return E_TIMEOUT;
        case HAL_BUSY:
            return E_BUSY;
        default:
            return E_NOK;
    }
}

/**
 * @brief Transmit data using DMA (non-blocking)
 */
static std_ret st_uart_transmit_dma(void *dev, const uint8_t *data, uint32_t size)
{
    if (NULL == dev || NULL == data || size == 0)
    {
        return E_INVALID_PARAM;
    }

    UART_HandleTypeDef *huart = (UART_HandleTypeDef *)dev;

    HAL_StatusTypeDef hal_ret = HAL_UART_Transmit_DMA(huart, (uint8_t *)data, size);

    switch (hal_ret)
    {
        case HAL_OK:
            return E_OK;
        case HAL_BUSY:
            return E_BUSY;
        default:
            return E_NOK;
    }
}

/**
 * @brief Receive data using DMA (non-blocking)
 */
static std_ret st_uart_receive_dma(void *dev, uint8_t *buffer, uint32_t size)
{
    if (NULL == dev || NULL == buffer || size == 0)
    {
        return E_INVALID_PARAM;
    }

    UART_HandleTypeDef *huart = (UART_HandleTypeDef *)dev;

    HAL_StatusTypeDef hal_ret = HAL_UART_Receive_DMA(huart, buffer, size);

    switch (hal_ret)
    {
        case HAL_OK:
            return E_OK;
        case HAL_BUSY:
            return E_BUSY;
        default:
            return E_NOK;
    }
}

/**
 * @brief Set UART baud rate at runtime
 */
static std_ret st_uart_set_baudrate(void *dev, uint32_t baudrate)
{
    if (NULL == dev || baudrate == 0)
    {
        return E_INVALID_PARAM;
    }

    UART_HandleTypeDef *huart = (UART_HandleTypeDef *)dev;

    HAL_StatusTypeDef hal_ret = HAL_UART_SetBaudRate(huart, baudrate);
    if (hal_ret != HAL_OK)
    {
        return E_NOK;
    }

    return E_OK;
}

/**
 * @brief Set hardware flow control at runtime
 */
static std_ret st_uart_set_flow_control(void *dev, drv_uart_flow_control_t flow_control)
{
    if (NULL == dev)
    {
        return E_INVALID_PARAM;
    }

    UART_HandleTypeDef *huart = (UART_HandleTypeDef *)dev;

    uint32_t hal_flow_ctl = get_hal_flow_control(flow_control);

    HAL_StatusTypeDef hal_ret = HAL_UART_SetFlowControl(huart, hal_flow_ctl);
    if (hal_ret != HAL_OK)
    {
        return E_NOK;
    }

    return E_OK;
}

/**
 * @brief Check if UART transmitter is ready
 */
static bool st_uart_is_tx_ready(void *dev)
{
    if (NULL == dev)
    {
        return false;
    }

    UART_HandleTypeDef *huart = (UART_HandleTypeDef *)dev;

    /* Check if UART is ready (not busy transmitting) */
    if (HAL_UART_GetState(huart) == HAL_UART_STATE_READY ||
        HAL_UART_GetState(huart) == HAL_UART_STATE_BUSY_RX)
    {
        return true;
    }

    return false;
}

/**
 * @brief Check if UART receive data is available
 */
static bool st_uart_is_rx_available(void *dev)
{
    if (NULL == dev)
    {
        return false;
    }

    UART_HandleTypeDef *huart = (UART_HandleTypeDef *)dev;

    /* Check if there is data in the receive register/FIFO */
    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE))
    {
        return true;
    }

    return false;
}
