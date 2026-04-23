//
// Created by nickyyy on 26-4-20.
//

#ifndef AT24CXX_H
#define AT24CXX_H

#include "drv_iic.h"

typedef enum
{
	AT24C01,
	AT24C02,
	AT24C04,
	AT24C08,
	AT24C16,
	AT24C32,
	AT24C64,
	AT24C128,
	AT24C256
}at24cxx_type_t;

typedef struct {
	drv_iic_obj_t *iic_drv;
	uint16_t dev_addr;
	uint32_t capacity;
	uint16_t page_size;
	drv_iic_reg_addr_mode addr_mode;
	at24cxx_type_t type;
} at24c_dev_t;

std_ret at24c_init(at24c_dev_t *dev);
std_ret at24c_read_bytes(at24c_dev_t *dev, uint16_t mem_addr, uint8_t *data, uint32_t len);
std_ret at24c_write_bytes(at24c_dev_t *dev, uint16_t mem_addr, uint8_t *data, uint32_t len);

#endif //AT24CXX_H
