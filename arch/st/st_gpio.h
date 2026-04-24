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
 * @file st_gpio.h
 * @brief STM32 GPIO Driver Header
 *
 * This file provides the STM32 HAL-based GPIO driver declarations.
 *
 * @author nickyyy
 * @date 2025-04-24
 */

#ifndef ST_GPIO_H
#define ST_GPIO_H

#include "drv_gpio.h"

/* ========================================================================= */
/* GPIO Port Definitions                                                     */
/* ========================================================================= */

#if defined(GPIOA)
#define ST_GPIO_PORT_A  ((void *)GPIOA)
#endif

#if defined(GPIOB)
#define ST_GPIO_PORT_B  ((void *)GPIOB)
#endif

#if defined(GPIOC)
#define ST_GPIO_PORT_C  ((void *)GPIOC)
#endif

#if defined(GPIOD)
#define ST_GPIO_PORT_D  ((void *)GPIOD)
#endif

#if defined(GPIOE)
#define ST_GPIO_PORT_E  ((void *)GPIOE)
#endif

#if defined(GPIOF)
#define ST_GPIO_PORT_F  ((void *)GPIOF)
#endif

#if defined(GPIOG)
#define ST_GPIO_PORT_G  ((void *)GPIOG)
#endif

#if defined(GPIOH)
#define ST_GPIO_PORT_H  ((void *)GPIOH)
#endif

#if defined(GPIOI)
#define ST_GPIO_PORT_I  ((void *)GPIOI)
#endif

#if defined(GPIOJ)
#define ST_GPIO_PORT_J  ((void *)GPIOJ)
#endif

#if defined(GPIOK)
#define ST_GPIO_PORT_K  ((void *)GPIOK)
#endif

/* ========================================================================= */
/* GPIO Pin Definitions                                                      */
/* ========================================================================= */

#define ST_GPIO_PIN_0   0x0001U
#define ST_GPIO_PIN_1   0x0002U
#define ST_GPIO_PIN_2   0x0004U
#define ST_GPIO_PIN_3   0x0008U
#define ST_GPIO_PIN_4   0x0010U
#define ST_GPIO_PIN_5   0x0020U
#define ST_GPIO_PIN_6   0x0040U
#define ST_GPIO_PIN_7   0x0080U
#define ST_GPIO_PIN_8   0x0100U
#define ST_GPIO_PIN_9   0x0200U
#define ST_GPIO_PIN_10  0x0400U
#define ST_GPIO_PIN_11  0x0800U
#define ST_GPIO_PIN_12  0x1000U
#define ST_GPIO_PIN_13  0x2000U
#define ST_GPIO_PIN_14  0x4000U
#define ST_GPIO_PIN_15  0x8000U

/* ========================================================================= */
/* GPIO Alternate Function Definitions                                       */
/* ========================================================================= */

#define ST_GPIO_AF_0    0U
#define ST_GPIO_AF_1    1U
#define ST_GPIO_AF_2    2U
#define ST_GPIO_AF_3    3U
#define ST_GPIO_AF_4    4U
#define ST_GPIO_AF_5    5U
#define ST_GPIO_AF_6    6U
#define ST_GPIO_AF_7    7U
#define ST_GPIO_AF_8    8U
#define ST_GPIO_AF_9    9U
#define ST_GPIO_AF_10   10U
#define ST_GPIO_AF_11   11U
#define ST_GPIO_AF_12   12U
#define ST_GPIO_AF_13   13U
#define ST_GPIO_AF_14   14U
#define ST_GPIO_AF_15   15U

#endif /* ST_GPIO_H */
