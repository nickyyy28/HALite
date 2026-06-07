//
// Created by nickyyy on 26-4-19.
//

#include "drv_tick.h"

#if defined STM32H743xx
#include "stm32h7xx_hal.h"
#endif

#if defined STM32F411xx || defined STM32F411xE
#include "stm32f4xx_hal.h"
#endif

uint32_t get_tick_ms(void)
{
	return HAL_GetTick();
}


void delay_ms(uint32_t ms)
{
	HAL_Delay(ms);
}
