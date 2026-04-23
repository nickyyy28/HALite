//
// Created by nickyyy on 26-4-18.
//

#ifndef FLASH_H
#define FLASH_H

#include "std_types.h"

typedef struct
{
	uint32_t flash_size;
	uint32_t maximum_write_size;
	uint32_t erase_size;
	std_ret (*flash_write)(uint32_t addr, uint8_t *data, uint32_t len);
	std_ret (*flash_read)(uint32_t addr, uint8_t *data, uint32_t len);
	std_ret (*flash_erase)(uint32_t addr, uint32_t len);
}flash_device_t;

#endif //FLASH_H
