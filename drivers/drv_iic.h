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

std_ret drv_iic_init(drv_iic_obj_t *obj);
std_ret drv_iic_deinit(drv_iic_obj_t *obj);
std_ret drv_iic_read(drv_iic_obj_t *obj, uint16_t device_addr, uint16_t reg_addr, drv_iic_reg_addr_mode reg_mode, uint8_t* data, uint32_t data_len, uint32_t timeout);
std_ret drv_iic_write(drv_iic_obj_t *obj, uint16_t device_addr, uint16_t reg_addr, drv_iic_reg_addr_mode reg_mode, uint8_t* data, uint32_t data_len, uint32_t timeout);

#endif //DRV_IIC_H
