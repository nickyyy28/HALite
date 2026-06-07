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
 * @file drv_gpio.c
 * @brief GPIO Driver Framework Implementation
 *
 * This file implements the hardware-independent GPIO API that calls
 * platform-specific function pointers.
 *
 * @author nickyyy
 * @date 2025-04-24
 */

#include "drv_gpio.h"
#include <stddef.h>

/* ========================================================================= */
/* Public API Functions                                                      */
/* ========================================================================= */

std_ret drv_gpio_init(drv_gpio_obj_t *obj, const drv_gpio_config_t *config)
{
    std_ret ret = E_NOK;
    if (NULL != obj && NULL != config)
    {
        ret = obj->init(obj->dev, obj->pin_mask, config);
    }
    else
    {
        ret = E_INVALID_PARAM;
    }
    return ret;
}

std_ret drv_gpio_deinit(drv_gpio_obj_t *obj)
{
    std_ret ret = E_NOK;
    if (NULL != obj)
    {
        ret = obj->deinit(obj->dev, obj->pin_mask);
    }
    else
    {
        ret = E_INVALID_PARAM;
    }
    return ret;
}

std_ret drv_gpio_write(drv_gpio_obj_t *obj, drv_gpio_pin_state_t state)
{
    std_ret ret = E_NOK;
    if (NULL != obj)
    {
        ret = obj->write(obj->dev, obj->pin_mask, state);
    }
    else
    {
        ret = E_INVALID_PARAM;
    }
    return ret;
}

drv_gpio_pin_state_t drv_gpio_read(drv_gpio_obj_t *obj)
{
    if (NULL != obj)
    {
        return obj->read(obj->dev, obj->pin_mask);
    }
    return DRV_GPIO_PIN_RESET;
}

std_ret drv_gpio_toggle(drv_gpio_obj_t *obj)
{
    std_ret ret = E_NOK;
    if (NULL != obj)
    {
        ret = obj->toggle(obj->dev, obj->pin_mask);
    }
    else
    {
        ret = E_INVALID_PARAM;
    }
    return ret;
}

std_ret drv_gpio_enable_irq(drv_gpio_obj_t *obj, drv_gpio_mode_t mode, drv_gpio_irq_callback_t callback)
{
    std_ret ret = E_NOK;
    if (NULL != obj)
    {
        ret = obj->enable_irq(obj->dev, obj->pin_mask, mode, callback);
    }
    else
    {
        ret = E_INVALID_PARAM;
    }
    return ret;
}

std_ret drv_gpio_disable_irq(drv_gpio_obj_t *obj)
{
    std_ret ret = E_NOK;
    if (NULL != obj)
    {
        ret = obj->disable_irq(obj->dev, obj->pin_mask);
    }
    else
    {
        ret = E_INVALID_PARAM;
    }
    return ret;
}

std_ret drv_gpio_write_port(drv_gpio_obj_t *obj, uint16_t pin_mask, drv_gpio_pin_state_t state)
{
    std_ret ret = E_NOK;
    if (NULL != obj)
    {
        ret = obj->write(obj->dev, pin_mask, state);
    }
    else
    {
        ret = E_INVALID_PARAM;
    }
    return ret;
}

uint16_t drv_gpio_read_port(drv_gpio_obj_t *obj)
{
    if (NULL != obj)
    {
        /* Platform-specific implementation should provide port reading */
        /* For now, return 0 as generic framework cannot access hardware registers */
        return 0;
    }
    return 0;
}
