//
// Created by nickyyy on 26-4-19.
//

#ifndef DRV_TICK_H
#define DRV_TICK_H

#include <stdint.h>

uint32_t get_tick_s(void);
uint32_t get_tick_ms(void);
uint32_t get_tick_us(void);

void delay_ms(uint32_t ms);

#endif //DRV_TICK_H
