//
// Created by nickyyy on 26-4-19.
//

#include "drv_tick.h"

extern uint32_t HAL_GetTick(void);

uint32_t get_tick_ms(void)
{
	return HAL_GetTick();
}


void delay_ms(uint32_t ms)
{
    uint32_t start_tick = HAL_GetTick();
    while ((HAL_GetTick() - start_tick) < ms) {
        // 等待直到达到指定的延迟时间
        asm("nop"); // 可以使用空操作来节省功耗
    }
}
