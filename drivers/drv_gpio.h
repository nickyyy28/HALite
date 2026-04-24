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
 * @file drv_gpio.h
 * @brief GPIO Driver Framework Header
 *
 * This file defines the GPIO driver abstraction layer that provides a unified
 * interface for GPIO operations across different hardware platforms.
 *
 * @author nickyyy
 * @date 2025-04-24
 */

#ifndef DRV_GPIO_H
#define DRV_GPIO_H

#include "std_types.h"

/**
 * @defgroup DRV_GPIO GPIO Driver Framework
 * @brief GPIO driver abstraction layer for hardware-independent pin control
 *
 * This driver framework provides a consistent API for GPIO operations including
 * pin initialization, read/write operations, and interrupt configuration.
 *
 * Usage Example:
 * @code
 * drv_gpio_obj_t gpio_led = {
 *     .dev = GPIOA,                      // GPIO port
 *     .pin = 5,                          // Pin number
 *     .init = stm32_gpio_init,           // Platform-specific init
 *     .deinit = stm32_gpio_deinit,       // Platform-specific deinit
 *     .write = stm32_gpio_write,         // Platform-specific write
 *     .read = stm32_gpio_read,           // Platform-specific read
 *     .toggle = stm32_gpio_toggle,       // Platform-specific toggle
 *     .set_mode = stm32_gpio_set_mode,   // Platform-specific mode set
 *     .set_pull = stm32_gpio_set_pull    // Platform-specific pull config
 * };
 *
 * drv_gpio_config_t cfg = {
 *     .mode = GPIO_MODE_OUTPUT_PP,
 *     .pull = GPIO_PULL_NONE,
 *     .speed = GPIO_SPEED_LOW
 * };
 *
 * drv_gpio_init(&gpio_led, &cfg);
 * drv_gpio_write(&gpio_led, GPIO_PIN_SET);
 * @endcode
 *
 * @{
 */

/**
 * @brief GPIO pin mode enumeration
 */
typedef enum
{
    GPIO_MODE_INPUT = 0,        /**< Input mode */
    GPIO_MODE_OUTPUT_PP,        /**< Output push-pull mode */
    GPIO_MODE_OUTPUT_OD,        /**< Output open-drain mode */
    GPIO_MODE_AF_PP,            /**< Alternate function push-pull */
    GPIO_MODE_AF_OD,            /**< Alternate function open-drain */
    GPIO_MODE_ANALOG,           /**< Analog mode */
    GPIO_MODE_IT_RISING,        /**< External interrupt rising edge */
    GPIO_MODE_IT_FALLING,       /**< External interrupt falling edge */
    GPIO_MODE_IT_RISING_FALLING /**< External interrupt both edges */
} drv_gpio_mode_t;

/**
 * @brief GPIO pull-up/pull-down enumeration
 */
typedef enum
{
    GPIO_PULL_NONE = 0, /**< No pull-up or pull-down */
    GPIO_PULL_UP,       /**< Pull-up enabled */
    GPIO_PULL_DOWN      /**< Pull-down enabled */
} drv_gpio_pull_t;

/**
 * @brief GPIO speed enumeration
 */
typedef enum
{
    GPIO_SPEED_LOW = 0,    /**< Low speed */
    GPIO_SPEED_MEDIUM,     /**< Medium speed */
    GPIO_SPEED_HIGH,       /**< High speed */
    GPIO_SPEED_VERY_HIGH   /**< Very high speed */
} drv_gpio_speed_t;

/**
 * @brief GPIO pin state enumeration
 */
typedef enum
{
    GPIO_PIN_RESET = 0, /**< Pin low level */
    GPIO_PIN_SET        /**< Pin high level */
} drv_gpio_pin_state_t;

/**
 * @brief GPIO configuration structure
 */
typedef struct
{
    drv_gpio_mode_t mode;   /**< Pin mode */
    drv_gpio_pull_t pull;   /**< Pull-up/pull-down configuration */
    drv_gpio_speed_t speed; /**< Pin speed */
    uint8_t alternate;      /**< Alternate function number (0-15) */
} drv_gpio_config_t;

/**
 * @brief GPIO interrupt callback function type
 */
typedef void (*drv_gpio_irq_callback_t)(void);

/**
 * @brief GPIO driver object structure
 */
typedef struct
{
    void *dev;          /**< Hardware device handle (e.g., GPIO_TypeDef*) */
    uint16_t pin;       /**< Pin number (0-15) */
    uint16_t pin_mask;  /**< Pin mask (1 << pin) */

    /* Lifecycle functions */
    /**
     * @brief Initialize GPIO pin with configuration
     * @param dev Hardware device handle
     * @param pin Pin number
     * @param config Pointer to GPIO configuration structure
     * @return E_OK on success, error code otherwise
     */
    std_ret (*init)(void *dev, uint16_t pin, const drv_gpio_config_t *config);

    /**
     * @brief Deinitialize GPIO pin
     * @param dev Hardware device handle
     * @param pin Pin number
     * @return E_OK on success, error code otherwise
     */
    std_ret (*deinit)(void *dev, uint16_t pin);

    /* I/O functions */
    /**
     * @brief Write pin state
     * @param dev Hardware device handle
     * @param pin Pin number
     * @param state Pin state (GPIO_PIN_RESET or GPIO_PIN_SET)
     * @return E_OK on success, error code otherwise
     */
    std_ret (*write)(void *dev, uint16_t pin, drv_gpio_pin_state_t state);

    /**
     * @brief Read pin state
     * @param dev Hardware device handle
     * @param pin Pin number
     * @return Pin state (GPIO_PIN_RESET or GPIO_PIN_SET)
     */
    drv_gpio_pin_state_t (*read)(void *dev, uint16_t pin);

    /**
     * @brief Toggle pin state
     * @param dev Hardware device handle
     * @param pin Pin number
     * @return E_OK on success, error code otherwise
     */
    std_ret (*toggle)(void *dev, uint16_t pin);

    /* Configuration functions */
    /**
     * @brief Set pin mode
     * @param dev Hardware device handle
     * @param pin Pin number
     * @param mode Pin mode
     * @return E_OK on success, error code otherwise
     */
    std_ret (*set_mode)(void *dev, uint16_t pin, drv_gpio_mode_t mode);

    /**
     * @brief Set pin pull-up/pull-down
     * @param dev Hardware device handle
     * @param pin Pin number
     * @param pull Pull configuration
     * @return E_OK on success, error code otherwise
     */
    std_ret (*set_pull)(void *dev, uint16_t pin, drv_gpio_pull_t pull);

    /**
     * @brief Set pin speed
     * @param dev Hardware device handle
     * @param pin Pin number
     * @param speed Pin speed
     * @return E_OK on success, error code otherwise
     */
    std_ret (*set_speed)(void *dev, uint16_t pin, drv_gpio_speed_t speed);

    /**
     * @brief Set alternate function
     * @param dev Hardware device handle
     * @param pin Pin number
     * @param alternate Alternate function number (0-15)
     * @return E_OK on success, error code otherwise
     */
    std_ret (*set_alternate)(void *dev, uint16_t pin, uint8_t alternate);

    /* Interrupt functions */
    /**
     * @brief Enable external interrupt
     * @param dev Hardware device handle
     * @param pin Pin number
     * @param mode Interrupt mode (rising/falling/both)
     * @param callback Interrupt callback function
     * @return E_OK on success, error code otherwise
     */
    std_ret (*enable_irq)(void *dev, uint16_t pin, drv_gpio_mode_t mode, drv_gpio_irq_callback_t callback);

    /**
     * @brief Disable external interrupt
     * @param dev Hardware device handle
     * @param pin Pin number
     * @return E_OK on success, error code otherwise
     */
    std_ret (*disable_irq)(void *dev, uint16_t pin);

    /* Default configuration */
    drv_gpio_config_t default_cfg;
} drv_gpio_obj_t;

/* ========================================================================= */
/* Public API Function Declarations                                          */
/* ========================================================================= */

/**
 * @brief Initialize GPIO pin
 *
 * @param obj Pointer to GPIO driver object
 * @param config Pointer to GPIO configuration structure
 * @return E_OK on success, E_INVALID_PARAM if obj is NULL, error code otherwise
 */
std_ret drv_gpio_init(drv_gpio_obj_t *obj, const drv_gpio_config_t *config);

/**
 * @brief Deinitialize GPIO pin
 *
 * @param obj Pointer to GPIO driver object
 * @return E_OK on success, E_INVALID_PARAM if obj is NULL, error code otherwise
 */
std_ret drv_gpio_deinit(drv_gpio_obj_t *obj);

/**
 * @brief Write GPIO pin state
 *
 * @param obj Pointer to GPIO driver object
 * @param state Pin state (GPIO_PIN_SET or GPIO_PIN_RESET)
 * @return E_OK on success, E_INVALID_PARAM if obj is NULL, error code otherwise
 */
std_ret drv_gpio_write(drv_gpio_obj_t *obj, drv_gpio_pin_state_t state);

/**
 * @brief Read GPIO pin state
 *
 * @param obj Pointer to GPIO driver object
 * @return Pin state (GPIO_PIN_SET or GPIO_PIN_RESET), GPIO_PIN_RESET if obj is NULL
 */
drv_gpio_pin_state_t drv_gpio_read(drv_gpio_obj_t *obj);

/**
 * @brief Toggle GPIO pin state
 *
 * @param obj Pointer to GPIO driver object
 * @return E_OK on success, E_INVALID_PARAM if obj is NULL, error code otherwise
 */
std_ret drv_gpio_toggle(drv_gpio_obj_t *obj);

/**
 * @brief Set GPIO pin mode
 *
 * @param obj Pointer to GPIO driver object
 * @param mode Pin mode
 * @return E_OK on success, E_INVALID_PARAM if obj is NULL, error code otherwise
 */
std_ret drv_gpio_set_mode(drv_gpio_obj_t *obj, drv_gpio_mode_t mode);

/**
 * @brief Set GPIO pin pull-up/pull-down
 *
 * @param obj Pointer to GPIO driver object
 * @param pull Pull configuration
 * @return E_OK on success, E_INVALID_PARAM if obj is NULL, error code otherwise
 */
std_ret drv_gpio_set_pull(drv_gpio_obj_t *obj, drv_gpio_pull_t pull);

/**
 * @brief Set GPIO pin speed
 *
 * @param obj Pointer to GPIO driver object
 * @param speed Pin speed
 * @return E_OK on success, E_INVALID_PARAM if obj is NULL, error code otherwise
 */
std_ret drv_gpio_set_speed(drv_gpio_obj_t *obj, drv_gpio_speed_t speed);

/**
 * @brief Set GPIO alternate function
 *
 * @param obj Pointer to GPIO driver object
 * @param alternate Alternate function number (0-15)
 * @return E_OK on success, E_INVALID_PARAM if obj is NULL, error code otherwise
 */
std_ret drv_gpio_set_alternate(drv_gpio_obj_t *obj, uint8_t alternate);

/**
 * @brief Enable GPIO external interrupt
 *
 * @param obj Pointer to GPIO driver object
 * @param mode Interrupt mode
 * @param callback Interrupt callback function
 * @return E_OK on success, E_INVALID_PARAM if obj is NULL, error code otherwise
 */
std_ret drv_gpio_enable_irq(drv_gpio_obj_t *obj, drv_gpio_mode_t mode, drv_gpio_irq_callback_t callback);

/**
 * @brief Disable GPIO external interrupt
 *
 * @param obj Pointer to GPIO driver object
 * @return E_OK on success, E_INVALID_PARAM if obj is NULL, error code otherwise
 */
std_ret drv_gpio_disable_irq(drv_gpio_obj_t *obj);

/**
 * @brief Write multiple pins on the same port
 *
 * @param obj Pointer to GPIO driver object (defines the port)
 * @param pin_mask Mask of pins to write (e.g., 0x0003 for pin 0 and 1)
 * @param state Pin state to write
 * @return E_OK on success, E_INVALID_PARAM if obj is NULL, error code otherwise
 */
std_ret drv_gpio_write_port(drv_gpio_obj_t *obj, uint16_t pin_mask, drv_gpio_pin_state_t state);

/**
 * @brief Read entire port state
 *
 * @param obj Pointer to GPIO driver object (defines the port)
 * @return 16-bit port value, 0 if obj is NULL
 */
uint16_t drv_gpio_read_port(drv_gpio_obj_t *obj);

/** @} */ /* End of DRV_GPIO group */

#endif /* DRV_GPIO_H */
