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
 * @file st_uart.h
 * @brief STM32 UART Driver Header
 *
 * This file provides the STM32 HAL-based UART driver declarations.
 *
 * @author nickyyy
 * @date 2025-04-24
 */

#ifndef ST_UART_H
#define ST_UART_H

#include "drv_uart.h"

/* ========================================================================= */
/* External UART Handle Declarations                                         */
/* ========================================================================= */

#include "usart.h"

#define USE_UART1

#if defined(USART1) && defined(USE_UART1)
extern drv_uart_obj_t drv_uart1_obj;
#endif

#if defined(USART2) && defined(USE_UART2)
extern drv_uart_obj_t drv_uart2_obj;
#endif

#if defined(USART3) && defined(USE_UART3)
extern drv_uart_obj_t drv_uart3_obj;
#endif

#if defined(UART4) && defined(USE_UART4)
extern drv_uart_obj_t drv_uart4_obj;
#endif

#if defined(UART5) && defined(USE_UART5)
extern drv_uart_obj_t drv_uart5_obj;
#endif

#if defined(USART6) && defined(USE_UART6)
extern drv_uart_obj_t drv_uart6_obj;
#endif

#endif /* ST_UART_H */
