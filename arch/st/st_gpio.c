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
 * @file st_gpio.c
 * @brief STM32 GPIO Driver Implementation
 *
 * This file provides the STM32 HAL-based implementation of the GPIO driver
 * interface defined in drv_gpio.h.
 *
 * @author nickyyy
 * @date 2025-04-24
 */

#include "st_gpio.h"
#include <stddef.h>

/* Private function prototypes */
static std_ret st_gpio_init(void *dev, uint16_t pin, const drv_gpio_config_t *config);
static std_ret st_gpio_deinit(void *dev, uint16_t pin);
static std_ret st_gpio_write(void *dev, uint16_t pin, drv_gpio_pin_state_t state);
static drv_gpio_pin_state_t st_gpio_read(void *dev, uint16_t pin);
static std_ret st_gpio_toggle(void *dev, uint16_t pin);
static std_ret st_gpio_set_mode(void *dev, uint16_t pin, drv_gpio_mode_t mode);
static std_ret st_gpio_set_pull(void *dev, uint16_t pin, drv_gpio_pull_t pull);
static std_ret st_gpio_set_speed(void *dev, uint16_t pin, drv_gpio_speed_t speed);
static std_ret st_gpio_set_alternate(void *dev, uint16_t pin, uint8_t alternate);
static std_ret st_gpio_enable_irq(void *dev, uint16_t pin, drv_gpio_mode_t mode, drv_gpio_irq_callback_t callback);
static std_ret st_gpio_disable_irq(void *dev, uint16_t pin);

/* Private helper functions */
static uint32_t get_hal_mode(drv_gpio_mode_t mode);
static uint32_t get_hal_pull(drv_gpio_pull_t pull);
static uint32_t get_hal_speed(drv_gpio_speed_t speed);

/* IRQ callback storage (one per pin number, shared across ports) */
static drv_gpio_irq_callback_t gpio_irq_callbacks[16] = {NULL};

/* ========================================================================= */
/* Private Functions                                                         */
/* ========================================================================= */

/**
 * @brief Convert GPIO mode enum to HAL mode
 */
static uint32_t get_hal_mode(drv_gpio_mode_t mode)
{
    switch (mode)
    {
        case GPIO_MODE_INPUT:
            return GPIO_MODE_INPUT;
        case GPIO_MODE_OUTPUT_PP:
            return GPIO_MODE_OUTPUT_PP;
        case GPIO_MODE_OUTPUT_OD:
            return GPIO_MODE_OUTPUT_OD;
        case GPIO_MODE_AF_PP:
            return GPIO_MODE_AF_PP;
        case GPIO_MODE_AF_OD:
            return GPIO_MODE_AF_OD;
        case GPIO_MODE_ANALOG:
            return GPIO_MODE_ANALOG;
        case GPIO_MODE_IT_RISING:
            return GPIO_MODE_IT_RISING;
        case GPIO_MODE_IT_FALLING:
            return GPIO_MODE_IT_FALLING;
        case GPIO_MODE_IT_RISING_FALLING:
            return GPIO_MODE_IT_RISING_FALLING;
        default:
            return GPIO_MODE_INPUT;
    }
}

/**
 * @brief Convert GPIO pull enum to HAL pull
 */
static uint32_t get_hal_pull(drv_gpio_pull_t pull)
{
    switch (pull)
    {
        case GPIO_PULL_NONE:
            return GPIO_NOPULL;
        case GPIO_PULL_UP:
            return GPIO_PULLUP;
        case GPIO_PULL_DOWN:
            return GPIO_PULLDOWN;
        default:
            return GPIO_NOPULL;
    }
}

/**
 * @brief Convert GPIO speed enum to HAL speed
 */
static uint32_t get_hal_speed(drv_gpio_speed_t speed)
{
    switch (speed)
    {
        case GPIO_SPEED_LOW:
            return GPIO_SPEED_FREQ_LOW;
        case GPIO_SPEED_MEDIUM:
            return GPIO_SPEED_FREQ_MEDIUM;
        case GPIO_SPEED_HIGH:
            return GPIO_SPEED_FREQ_HIGH;
        case GPIO_SPEED_VERY_HIGH:
            return GPIO_SPEED_FREQ_VERY_HIGH;
        default:
            return GPIO_SPEED_FREQ_LOW;
    }
}

/**
 * @brief Get pin number from mask (0-15)
 */
static uint8_t get_pin_number(uint16_t pin_mask)
{
    uint8_t pin = 0;
    while ((pin_mask & 0x0001) == 0 && pin < 16)
    {
        pin_mask >>= 1;
        pin++;
    }
    return pin;
}

/**
 * @brief Initialize GPIO pin with configuration
 */
static std_ret st_gpio_init(void *dev, uint16_t pin, const drv_gpio_config_t *config)
{
    if (NULL == dev || NULL == config || pin == 0)
    {
        return E_INVALID_PARAM;
    }

    GPIO_TypeDef *gpio_port = (GPIO_TypeDef *)dev;

    /* Enable GPIO clock */
    if (gpio_port == GPIOA)
        __HAL_RCC_GPIOA_CLK_ENABLE();
    else if (gpio_port == GPIOB)
        __HAL_RCC_GPIOB_CLK_ENABLE();
    else if (gpio_port == GPIOC)
        __HAL_RCC_GPIOC_CLK_ENABLE();
#if defined(GPIOD)
    else if (gpio_port == GPIOD)
        __HAL_RCC_GPIOD_CLK_ENABLE();
#endif
#if defined(GPIOE)
    else if (gpio_port == GPIOE)
        __HAL_RCC_GPIOE_CLK_ENABLE();
#endif
#if defined(GPIOF)
    else if (gpio_port == GPIOF)
        __HAL_RCC_GPIOF_CLK_ENABLE();
#endif
#if defined(GPIOG)
    else if (gpio_port == GPIOG)
        __HAL_RCC_GPIOG_CLK_ENABLE();
#endif
#if defined(GPIOH)
    else if (gpio_port == GPIOH)
        __HAL_RCC_GPIOH_CLK_ENABLE();
#endif
#if defined(GPIOI)
    else if (gpio_port == GPIOI)
        __HAL_RCC_GPIOI_CLK_ENABLE();
#endif
#if defined(GPIOJ)
    else if (gpio_port == GPIOJ)
        __HAL_RCC_GPIOJ_CLK_ENABLE();
#endif
#if defined(GPIOK)
    else if (gpio_port == GPIOK)
        __HAL_RCC_GPIOK_CLK_ENABLE();
#endif

    GPIO_InitTypeDef gpio_init = {0};
    gpio_init.Pin = pin;
    gpio_init.Mode = get_hal_mode(config->mode);
    gpio_init.Pull = get_hal_pull(config->pull);
    gpio_init.Speed = get_hal_speed(config->speed);

    if (config->mode == GPIO_MODE_AF_PP || config->mode == GPIO_MODE_AF_OD)
    {
        gpio_init.Alternate = config->alternate;
    }

    HAL_GPIO_Init(gpio_port, &gpio_init);

    return E_OK;
}

/**
 * @brief Deinitialize GPIO pin
 */
static std_ret st_gpio_deinit(void *dev, uint16_t pin)
{
    if (NULL == dev || pin == 0)
    {
        return E_INVALID_PARAM;
    }

    GPIO_TypeDef *gpio_port = (GPIO_TypeDef *)dev;
    HAL_GPIO_DeInit(gpio_port, pin);

    return E_OK;
}

/**
 * @brief Write GPIO pin state
 */
static std_ret st_gpio_write(void *dev, uint16_t pin, drv_gpio_pin_state_t state)
{
    if (NULL == dev || pin == 0)
    {
        return E_INVALID_PARAM;
    }

    GPIO_TypeDef *gpio_port = (GPIO_TypeDef *)dev;
    HAL_GPIO_WritePin(gpio_port, pin, (state == GPIO_PIN_SET) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    return E_OK;
}

/**
 * @brief Read GPIO pin state
 */
static drv_gpio_pin_state_t st_gpio_read(void *dev, uint16_t pin)
{
    if (NULL == dev || pin == 0)
    {
        return GPIO_PIN_RESET;
    }

    GPIO_TypeDef *gpio_port = (GPIO_TypeDef *)dev;
    GPIO_PinState hal_state = HAL_GPIO_ReadPin(gpio_port, pin);

    return (hal_state == GPIO_PIN_SET) ? GPIO_PIN_SET : GPIO_PIN_RESET;
}

/**
 * @brief Toggle GPIO pin state
 */
static std_ret st_gpio_toggle(void *dev, uint16_t pin)
{
    if (NULL == dev || pin == 0)
    {
        return E_INVALID_PARAM;
    }

    GPIO_TypeDef *gpio_port = (GPIO_TypeDef *)dev;
    HAL_GPIO_TogglePin(gpio_port, pin);

    return E_OK;
}

/**
 * @brief Set GPIO pin mode
 */
static std_ret st_gpio_set_mode(void *dev, uint16_t pin, drv_gpio_mode_t mode)
{
    if (NULL == dev || pin == 0)
    {
        return E_INVALID_PARAM;
    }

    GPIO_TypeDef *gpio_port = (GPIO_TypeDef *)dev;

    GPIO_InitTypeDef gpio_init = {0};
    gpio_init.Pin = pin;
    gpio_init.Mode = get_hal_mode(mode);
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(gpio_port, &gpio_init);

    return E_OK;
}

/**
 * @brief Set GPIO pin pull-up/pull-down
 */
static std_ret st_gpio_set_pull(void *dev, uint16_t pin, drv_gpio_pull_t pull)
{
    if (NULL == dev || pin == 0)
    {
        return E_INVALID_PARAM;
    }

    GPIO_TypeDef *gpio_port = (GPIO_TypeDef *)dev;

    /* Read current configuration and modify pull */
    GPIO_InitTypeDef gpio_init = {0};
    gpio_init.Pin = pin;
    gpio_init.Mode = GPIO_MODE_INPUT;
    gpio_init.Pull = get_hal_pull(pull);
    gpio_init.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(gpio_port, &gpio_init);

    return E_OK;
}

/**
 * @brief Set GPIO pin speed
 */
static std_ret st_gpio_set_speed(void *dev, uint16_t pin, drv_gpio_speed_t speed)
{
    if (NULL == dev || pin == 0)
    {
        return E_INVALID_PARAM;
    }

    GPIO_TypeDef *gpio_port = (GPIO_TypeDef *)dev;

    GPIO_InitTypeDef gpio_init = {0};
    gpio_init.Pin = pin;
    gpio_init.Mode = GPIO_MODE_INPUT;
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Speed = get_hal_speed(speed);

    HAL_GPIO_Init(gpio_port, &gpio_init);

    return E_OK;
}

/**
 * @brief Set GPIO alternate function
 */
static std_ret st_gpio_set_alternate(void *dev, uint16_t pin, uint8_t alternate)
{
    if (NULL == dev || pin == 0 || alternate > 15)
    {
        return E_INVALID_PARAM;
    }

    GPIO_TypeDef *gpio_port = (GPIO_TypeDef *)dev;

    GPIO_InitTypeDef gpio_init = {0};
    gpio_init.Pin = pin;
    gpio_init.Mode = GPIO_MODE_AF_PP;
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
    gpio_init.Alternate = alternate;

    HAL_GPIO_Init(gpio_port, &gpio_init);

    return E_OK;
}

/**
 * @brief Enable GPIO external interrupt
 */
static std_ret st_gpio_enable_irq(void *dev, uint16_t pin, drv_gpio_mode_t mode, drv_gpio_irq_callback_t callback)
{
    if (NULL == dev || pin == 0 || NULL == callback)
    {
        return E_INVALID_PARAM;
    }

    GPIO_TypeDef *gpio_port = (GPIO_TypeDef *)dev;
    uint8_t pin_num = get_pin_number(pin);

    if (pin_num >= 16)
    {
        return E_INVALID_PARAM;
    }

    /* Store callback */
    gpio_irq_callbacks[pin_num] = callback;

    /* Configure GPIO as interrupt source */
    GPIO_InitTypeDef gpio_init = {0};
    gpio_init.Pin = pin;
    gpio_init.Mode = get_hal_mode(mode);
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(gpio_port, &gpio_init);

    /* Configure NVIC */
    IRQn_Type irqn;
    if (pin_num < 5)
    {
        irqn = (IRQn_Type)(EXTI0_IRQn + pin_num);
    }
    else if (pin_num < 10)
    {
        irqn = EXTI9_5_IRQn;
    }
    else
    {
        irqn = EXTI15_10_IRQn;
    }

    HAL_NVIC_SetPriority(irqn, 5, 0);
    HAL_NVIC_EnableIRQ(irqn);

    return E_OK;
}

/**
 * @brief Disable GPIO external interrupt
 */
static std_ret st_gpio_disable_irq(void *dev, uint16_t pin)
{
    if (NULL == dev || pin == 0)
    {
        return E_INVALID_PARAM;
    }

    uint8_t pin_num = get_pin_number(pin);

    if (pin_num < 16)
    {
        gpio_irq_callbacks[pin_num] = NULL;
    }

    /* Disable NVIC interrupt */
    IRQn_Type irqn;
    if (pin_num < 5)
    {
        irqn = (IRQn_Type)(EXTI0_IRQn + pin_num);
    }
    else if (pin_num < 10)
    {
        irqn = EXTI9_5_IRQn;
    }
    else
    {
        irqn = EXTI15_10_IRQn;
    }

    HAL_NVIC_DisableIRQ(irqn);

    return E_OK;
}

/* ========================================================================= */
/* Public Functions                                                          */
/* ========================================================================= */

/**
 * @brief GPIO interrupt handler (called from HAL callbacks)
 */
void st_gpio_exti_callback(uint16_t gpio_pin)
{
    uint8_t pin_num = get_pin_number(gpio_pin);

    if (pin_num < 16 && gpio_irq_callbacks[pin_num] != NULL)
    {
        gpio_irq_callbacks[pin_num]();
    }
}

/* ========================================================================= */
/* HAL Callback Implementations                                              */
/* ========================================================================= */

/**
 * @brief HAL GPIO EXTI callback
 */
void HAL_GPIO_EXTI_Callback(uint16_t gpio_pin)
{
    st_gpio_exti_callback(gpio_pin);
}

/* ========================================================================= */
/* Driver Objects Helper                                                     */
/* ========================================================================= */

/**
 * @brief Create a GPIO driver object
 *
 * Helper function to initialize a GPIO driver object structure
 */
void st_gpio_obj_init(drv_gpio_obj_t *obj, void *port, uint16_t pin)
{
    if (NULL == obj)
    {
        return;
    }

    obj->dev = port;
    obj->pin = get_pin_number(pin);
    obj->pin_mask = pin;
    obj->init = st_gpio_init;
    obj->deinit = st_gpio_deinit;
    obj->write = st_gpio_write;
    obj->read = st_gpio_read;
    obj->toggle = st_gpio_toggle;
    obj->set_mode = st_gpio_set_mode;
    obj->set_pull = st_gpio_set_pull;
    obj->set_speed = st_gpio_set_speed;
    obj->set_alternate = st_gpio_set_alternate;
    obj->enable_irq = st_gpio_enable_irq;
    obj->disable_irq = st_gpio_disable_irq;
    obj->default_cfg.mode = GPIO_MODE_INPUT;
    obj->default_cfg.pull = GPIO_PULL_NONE;
    obj->default_cfg.speed = GPIO_SPEED_LOW;
    obj->default_cfg.alternate = 0;
}
