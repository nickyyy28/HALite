//
// Created by nickyyy on 26-4-19.
//

#include "drv_tick.h"
#include "stm32h7xx_hal.h"

uint32_t get_tick_ms(void)
{
	return HAL_GetTick();
}


void delay_ms(uint32_t ms)
{
	HAL_Delay(ms);
}