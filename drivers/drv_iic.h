//
// Created by nickyyy on 26-4-20.
//

#ifndef DRV_IIC_H
#define DRV_IIC_H

#include "std_types.h"

typedef enum
{
	IIC_REG_ADDR_NONE,
	IIC_REG_ADDR_8_BIT,
	IIC_REG_ADDR_16_BIT,
}drv_iic_reg_addr_mode;

typedef struct
{
	void *dev;
	std_ret (*init)(void* dev);
	std_ret (*deinit)(void* dev);
	std_ret (*read)(void* dev, uint16_t device_addr, uint16_t reg_addr, drv_iic_reg_addr_mode reg_mode, uint8_t* data, uint32_t data_len, uint32_t timeout);
	std_ret (*write)(void* dev, uint16_t device_addr, uint16_t reg_addr, drv_iic_reg_addr_mode reg_mode, uint8_t* data, uint32_t data_len, uint32_t timeout);
}drv_iic_obj_t;

#endif //DRV_IIC_H
