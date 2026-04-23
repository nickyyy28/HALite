//
// Created by nickyyy on 26-4-20.
//
#include "st_iic.h"
#include "i2c.h"

std_ret st_iic_init(void* dev);
std_ret st_iic_deinit(void* dev);
std_ret st_iic_read(void* dev, uint16_t device_addr, uint16_t reg_addr, drv_iic_reg_addr_mode reg_mode, uint8_t* data, uint32_t data_len, uint32_t timeout);
std_ret st_iic_write(void* dev, uint16_t device_addr, uint16_t reg_addr, drv_iic_reg_addr_mode reg_mode, uint8_t* data, uint32_t data_len, uint32_t timeout);

drv_iic_obj_t drv_iic4_obj = {
	.dev = &hi2c4,
	.init = st_iic_init,
	.deinit = st_iic_deinit,
	.read = st_iic_read,
	.write = st_iic_write
};

std_ret st_iic_init(void* dev)
{
	if (NULL == dev)
	{
		return E_INVALID_PARAM;
	}

	MX_I2C4_Init();

	return E_OK;
}

std_ret st_iic_deinit(void* dev)
{
	if (NULL == dev)
	{
		return E_INVALID_PARAM;
	}

	HAL_I2C_MspDeInit(dev);

	return E_OK;
}

std_ret st_iic_read(void* dev, uint16_t device_addr, uint16_t reg_addr, drv_iic_reg_addr_mode reg_mode, uint8_t* data, uint32_t data_len, uint32_t timeout)
{
	if (NULL == dev || NULL == data || 0 == data_len)
	{
		return E_INVALID_PARAM;
	}

	I2C_HandleTypeDef *hi2c = (I2C_HandleTypeDef *)dev;
	HAL_StatusTypeDef hal_status;
	uint16_t mem_addr_size;

	if (reg_mode == IIC_REG_ADDR_8_BIT)
	{
		mem_addr_size = I2C_MEMADD_SIZE_8BIT;
		hal_status = HAL_I2C_Mem_Read(hi2c, device_addr, reg_addr, mem_addr_size, data, data_len, timeout);
	}
	else if (reg_mode == IIC_REG_ADDR_16_BIT)
	{
		mem_addr_size = I2C_MEMADD_SIZE_16BIT;
		hal_status = HAL_I2C_Mem_Read(hi2c, device_addr, reg_addr, mem_addr_size, data, data_len, timeout);
	}
	else
	{
		hal_status = HAL_I2C_Master_Receive(hi2c, device_addr, data, data_len, timeout);
	}

	return (hal_status == HAL_OK) ? E_OK : E_NOK;
}

std_ret st_iic_write(void* dev, uint16_t device_addr, uint16_t reg_addr, drv_iic_reg_addr_mode reg_mode, uint8_t* data, uint32_t data_len, uint32_t timeout)
{
	if (NULL == dev || NULL == data || 0 == data_len)
	{
		return E_INVALID_PARAM;
	}

	I2C_HandleTypeDef *hi2c = (I2C_HandleTypeDef *)dev;
	HAL_StatusTypeDef hal_status;
	uint16_t mem_addr_size;

	if (reg_mode == IIC_REG_ADDR_8_BIT)
	{
		mem_addr_size = I2C_MEMADD_SIZE_8BIT;
		hal_status = HAL_I2C_Mem_Write(hi2c, device_addr, reg_addr, mem_addr_size, data, data_len, timeout);
	}
	else if (reg_mode == IIC_REG_ADDR_16_BIT)
	{
		mem_addr_size = I2C_MEMADD_SIZE_16BIT;
		hal_status = HAL_I2C_Mem_Write(hi2c, device_addr, reg_addr, mem_addr_size, data, data_len, timeout);
	}
	else
	{
		hal_status = HAL_I2C_Master_Transmit(hi2c, device_addr, data, data_len, timeout);
	}

	return (hal_status == HAL_OK) ? E_OK : E_NOK;
}
