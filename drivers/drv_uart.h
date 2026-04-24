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
 * @file drv_uart.h
 * @brief UART Driver Framework Header
 * 
 * This file defines the UART driver abstraction layer that provides a unified
 * interface for UART communication across different hardware platforms.
 * 
 * @author nickyyy
 * @date 2025-04-24
 */

#ifndef DRV_UART_H
#define DRV_UART_H

#include "std_types.h"

/**
 * @defgroup DRV_UART UART Driver Framework
 * @brief UART driver abstraction layer for hardware-independent communication
 * 
 * This driver framework provides a consistent API for UART operations including
 * initialization, data transmission/reception, and flow control.
 * 
 * Usage Example:
 * @code
 * drv_uart_obj_t uart_drv = {
 *     .dev = &huart1,                    // Hardware handle
 *     .init = stm32_uart_init,           // Platform-specific init
 *     .deinit = stm32_uart_deinit,       // Platform-specific deinit
 *     .transmit = stm32_uart_transmit,   // Platform-specific transmit
 *     .receive = stm32_uart_receive,     // Platform-specific receive
 *     .transmit_dma = stm32_uart_tx_dma, // Optional DMA transmit
 *     .receive_dma = stm32_uart_rx_dma,  // Optional DMA receive
 *     .set_baudrate = stm32_uart_set_baudrate,
 *     .set_flow_control = stm32_uart_set_flow_ctrl,
 *     .tx_fifo_size = 256,
 *     .rx_fifo_size = 256,
 *     .timeout_ms = 1000
 * };
 * 
 * drv_uart_init(&uart_drv);
 * uint8_t data[] = "Hello, World!";
 * drv_uart_transmit(&uart_drv, data, sizeof(data));
 * @endcode
 * 
 * @{
 */

/**
 * @brief UART baud rate enumeration
 * 
 * Common baud rates used in serial communication.
 * Custom baud rates can be set using drv_uart_set_baudrate() with raw values.
 */
typedef enum
{
    UART_BAUD_1200   = 1200,    /**< 1200 bits per second */
    UART_BAUD_2400   = 2400,    /**< 2400 bits per second */
    UART_BAUD_4800   = 4800,    /**< 4800 bits per second */
    UART_BAUD_9600   = 9600,    /**< 9600 bits per second - Standard */
    UART_BAUD_19200  = 19200,   /**< 19200 bits per second */
    UART_BAUD_38400  = 38400,   /**< 38400 bits per second */
    UART_BAUD_57600  = 57600,   /**< 57600 bits per second */
    UART_BAUD_115200 = 115200,  /**< 115200 bits per second - High speed standard */
    UART_BAUD_230400 = 230400,  /**< 230400 bits per second */
    UART_BAUD_460800 = 460800,  /**< 460800 bits per second */
    UART_BAUD_921600 = 921600,  /**< 921600 bits per second */
    UART_BAUD_1M     = 1000000, /**< 1 megabit per second */
    UART_BAUD_2M     = 2000000, /**< 2 megabits per second */
    UART_BAUD_3M     = 3000000, /**< 3 megabits per second */
    UART_BAUD_4M     = 4000000, /**< 4 megabits per second */
    UART_BAUD_5M     = 5000000  /**< 5 megabits per second */
} drv_uart_baudrate_t;

/**
 * @brief UART data width enumeration
 * 
 * Defines the number of data bits in each UART frame.
 */
typedef enum
{
    UART_DATA_WIDTH_5_BIT = 0,  /**< 5 data bits (rarely used) */
    UART_DATA_WIDTH_6_BIT,      /**< 6 data bits (rarely used) */
    UART_DATA_WIDTH_7_BIT,      /**< 7 data bits (ASCII communication) */
    UART_DATA_WIDTH_8_BIT,      /**< 8 data bits - Standard (1 byte) */
    UART_DATA_WIDTH_9_BIT       /**< 9 data bits (parity + 8 data) */
} drv_uart_data_width_t;

/**
 * @brief UART stop bits enumeration
 * 
 * Defines the number of stop bits used to signal the end of a frame.
 */
typedef enum
{
    UART_STOP_BITS_1   = 0,     /**< 1 stop bit - Standard */
    UART_STOP_BITS_1_5,         /**< 1.5 stop bits (used with 5 data bits) */
    UART_STOP_BITS_2            /**< 2 stop bits (better error detection) */
} drv_uart_stop_bits_t;

/**
 * @brief UART parity enumeration
 * 
 * Parity bit configuration for error detection.
 */
typedef enum
{
    UART_PARITY_NONE = 0,       /**< No parity bit - No error checking */
    UART_PARITY_ODD,            /**< Odd parity - Parity bit makes total 1s odd */
    UART_PARITY_EVEN            /**< Even parity - Parity bit makes total 1s even */
} drv_uart_parity_t;

/**
 * @brief UART hardware flow control enumeration
 * 
 * Hardware flow control using RTS/CTS signals to prevent buffer overflow.
 */
typedef enum
{
    UART_FLOW_CONTROL_NONE = 0, /**< No hardware flow control */
    UART_FLOW_CONTROL_RTS,      /**< Request To Send only */
    UART_FLOW_CONTROL_CTS,      /**< Clear To Send only */
    UART_FLOW_CONTROL_RTS_CTS   /**< Full hardware flow control */
} drv_uart_flow_control_t;

/**
 * @brief UART configuration structure
 * 
 * Complete UART configuration parameters for initialization.
 * Used with init() function pointer in driver object.
 */
typedef struct
{
    drv_uart_baudrate_t     baudrate;       /**< Communication speed in bps */
    drv_uart_data_width_t   data_width;     /**< Number of data bits per frame */
    drv_uart_stop_bits_t    stop_bits;      /**< Number of stop bits */
    drv_uart_parity_t       parity;         /**< Parity configuration */
    drv_uart_flow_control_t flow_control;   /**< Hardware flow control mode */
} drv_uart_config_t;

/**
 * @brief UART driver object structure
 * 
 * This structure encapsulates all UART operations and configuration.
 * It uses a function pointer-based approach for hardware abstraction,
 * allowing the same API to work across different MCU platforms.
 * 
 * The function pointers must be initialized with platform-specific
 * implementations before calling drv_uart_init().
 */
typedef struct
{
    void *dev;                          /**< Hardware device handle (e.g., UART_HandleTypeDef*) */
    
    /* Lifecycle functions */
    /**
     * @brief Initialize UART hardware with configuration
     * @param dev Hardware device handle
     * @param config Pointer to UART configuration structure
     * @return E_OK on success, error code otherwise
     */
    std_ret (*init)(void *dev, const drv_uart_config_t *config);
    
    /**
     * @brief Deinitialize UART hardware
     * @param dev Hardware device handle
     * @return E_OK on success, error code otherwise
     */
    std_ret (*deinit)(void *dev);
    
    /* Data transmission functions */
    /**
     * @brief Transmit data in blocking mode
     * @param dev Hardware device handle
     * @param data Pointer to data buffer to transmit
     * @param size Number of bytes to transmit
     * @param timeout_ms Timeout in milliseconds
     * @return E_OK on success, E_TIMEOUT on timeout, error code otherwise
     */
    std_ret (*transmit)(void *dev, const uint8_t *data, uint32_t size, uint32_t timeout_ms);
    
    /**
     * @brief Receive data in blocking mode
     * @param dev Hardware device handle
     * @param buffer Pointer to buffer for received data
     * @param size Number of bytes to receive
     * @param timeout_ms Timeout in milliseconds
     * @return E_OK on success, E_TIMEOUT on timeout, error code otherwise
     */
    std_ret (*receive)(void *dev, uint8_t *buffer, uint32_t size, uint32_t timeout_ms);
    
    /* DMA/Interrupt based functions (optional, can be NULL) */
    /**
     * @brief Transmit data using DMA (non-blocking)
     * @param dev Hardware device handle
     * @param data Pointer to data buffer to transmit
     * @param size Number of bytes to transmit
     * @return E_OK on success, error code otherwise
     */
    std_ret (*transmit_dma)(void *dev, const uint8_t *data, uint32_t size);
    
    /**
     * @brief Receive data using DMA (non-blocking)
     * @param dev Hardware device handle
     * @param buffer Pointer to buffer for received data
     * @param size Number of bytes to receive
     * @return E_OK on success, error code otherwise
     */
    std_ret (*receive_dma)(void *dev, uint8_t *buffer, uint32_t size);
    
    /* Configuration functions */
    /**
     * @brief Change baud rate at runtime
     * @param dev Hardware device handle
     * @param baudrate New baud rate value
     * @return E_OK on success, error code otherwise
     */
    std_ret (*set_baudrate)(void *dev, uint32_t baudrate);
    
    /**
     * @brief Configure hardware flow control
     * @param dev Hardware device handle
     * @param flow_control Flow control mode
     * @return E_OK on success, error code otherwise
     */
    std_ret (*set_flow_control)(void *dev, drv_uart_flow_control_t flow_control);
    
    /* Status functions */
    /**
     * @brief Check if UART is ready for transmission
     * @param dev Hardware device handle
     * @return true if ready, false otherwise
     */
    bool (*is_tx_ready)(void *dev);
    
    /**
     * @brief Check if data is available for reading
     * @param dev Hardware device handle
     * @return true if data available, false otherwise
     */
    bool (*is_rx_available)(void *dev);
    
    /* Buffer/FIFO configuration */
    uint32_t tx_fifo_size;              /**< Transmit buffer/FIFO size in bytes */
    uint32_t rx_fifo_size;              /**< Receive buffer/FIFO size in bytes */
    
    /* Timeout configuration */
    uint32_t timeout_ms;                /**< Default timeout for operations in milliseconds */
} drv_uart_obj_t;

/* ========================================================================= */
/* Public API Function Declarations                                          */
/* ========================================================================= */

/**
 * @brief Initialize UART driver
 * 
 * Calls the platform-specific initialization function with the provided
 * configuration. Must be called before any other UART operations.
 * 
 * @param obj Pointer to UART driver object
 * @param config Pointer to UART configuration structure
 * @return E_OK on success, E_INVALID_PARAM if obj is NULL, error code otherwise
 */
std_ret drv_uart_init(drv_uart_obj_t *obj, const drv_uart_config_t *config);

/**
 * @brief Deinitialize UART driver
 * 
 * Releases UART hardware resources and disables the peripheral.
 * 
 * @param obj Pointer to UART driver object
 * @return E_OK on success, E_INVALID_PARAM if obj is NULL, error code otherwise
 */
std_ret drv_uart_deinit(drv_uart_obj_t *obj);

/**
 * @brief Transmit data (blocking mode)
 * 
 * Sends data over UART and waits for completion or timeout.
 * Uses the default timeout specified in the driver object.
 * 
 * @param obj Pointer to UART driver object
 * @param data Pointer to data buffer to transmit
 * @param size Number of bytes to transmit
 * @return E_OK on success, E_INVALID_PARAM if parameters invalid, 
 *         E_TIMEOUT on timeout, error code otherwise
 */
std_ret drv_uart_transmit(drv_uart_obj_t *obj, const uint8_t *data, uint32_t size);

/**
 * @brief Receive data (blocking mode)
 * 
 * Receives data from UART and waits for completion or timeout.
 * Uses the default timeout specified in the driver object.
 * 
 * @param obj Pointer to UART driver object
 * @param buffer Pointer to buffer for received data
 * @param size Number of bytes to receive
 * @return E_OK on success, E_INVALID_PARAM if parameters invalid,
 *         E_TIMEOUT on timeout, error code otherwise
 */
std_ret drv_uart_receive(drv_uart_obj_t *obj, uint8_t *buffer, uint32_t size);

/**
 * @brief Transmit data using DMA (non-blocking)
 * 
 * Initiates DMA transmission. Completion must be checked via interrupt
 * callback or polling is_tx_ready().
 * 
 * @param obj Pointer to UART driver object
 * @param data Pointer to data buffer to transmit
 * @param size Number of bytes to transmit
 * @return E_OK on success, E_INVALID_PARAM if parameters invalid,
 *         E_NOT_IMP if DMA not supported, error code otherwise
 */
std_ret drv_uart_transmit_dma(drv_uart_obj_t *obj, const uint8_t *data, uint32_t size);

/**
 * @brief Receive data using DMA (non-blocking)
 * 
 * Initiates DMA reception. Completion must be checked via interrupt
 * callback or polling is_rx_available().
 * 
 * @param obj Pointer to UART driver object
 * @param buffer Pointer to buffer for received data
 * @param size Number of bytes to receive
 * @return E_OK on success, E_INVALID_PARAM if parameters invalid,
 *         E_NOT_IMP if DMA not supported, error code otherwise
 */
std_ret drv_uart_receive_dma(drv_uart_obj_t *obj, uint8_t *buffer, uint32_t size);

/**
 * @brief Set UART baud rate
 * 
 * Changes the baud rate at runtime. May require reconfiguring the hardware.
 * 
 * @param obj Pointer to UART driver object
 * @param baudrate New baud rate value (use drv_uart_baudrate_t values or raw)
 * @return E_OK on success, E_INVALID_PARAM if obj is NULL,
 *         E_NOT_IMP if not supported, error code otherwise
 */
std_ret drv_uart_set_baudrate(drv_uart_obj_t *obj, uint32_t baudrate);

/**
 * @brief Set hardware flow control
 * 
 * Configures RTS/CTS hardware flow control at runtime.
 * 
 * @param obj Pointer to UART driver object
 * @param flow_control Flow control mode
 * @return E_OK on success, E_INVALID_PARAM if obj is NULL,
 *         E_NOT_IMP if not supported, error code otherwise
 */
std_ret drv_uart_set_flow_control(drv_uart_obj_t *obj, drv_uart_flow_control_t flow_control);

/**
 * @brief Check if transmitter is ready
 * 
 * Polls the UART status to determine if a new transmission can be started.
 * 
 * @param obj Pointer to UART driver object
 * @return true if ready for transmission, false if busy or obj is NULL
 */
bool drv_uart_is_tx_ready(drv_uart_obj_t *obj);

/**
 * @brief Check if receive data is available
 * 
 * Polls the UART status to determine if received data is available.
 * 
 * @param obj Pointer to UART driver object
 * @return true if data available for reading, false if empty or obj is NULL
 */
bool drv_uart_is_rx_available(drv_uart_obj_t *obj);

/**
 * @brief Transmit a single character
 * 
 * Convenience function for sending a single byte/character.
 * 
 * @param obj Pointer to UART driver object
 * @param ch Character to transmit
 * @return E_OK on success, error code otherwise
 */
std_ret drv_uart_putchar(drv_uart_obj_t *obj, uint8_t ch);

/**
 * @brief Receive a single character
 * 
 * Convenience function for receiving a single byte/character.
 * 
 * @param obj Pointer to UART driver object
 * @param ch Pointer to store received character
 * @return E_OK on success, error code otherwise
 */
std_ret drv_uart_getchar(drv_uart_obj_t *obj, uint8_t *ch);

/**
 * @brief Transmit null-terminated string
 * 
 * Convenience function for sending C strings.
 * 
 * @param obj Pointer to UART driver object
 * @param str Null-terminated string to transmit
 * @return E_OK on success, error code otherwise
 */
std_ret drv_uart_puts(drv_uart_obj_t *obj, const char *str);

/** @} */ /* End of DRV_UART group */

#endif /* DRV_UART_H */
