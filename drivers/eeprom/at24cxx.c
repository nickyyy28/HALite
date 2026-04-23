//
// Created by nickyyy on 26-4-20.
//
#include "at24cxx.h"
#include "drv_tick.h"

#define AT24C_IIC_TIMEOUT 1000

std_ret at24c_init(at24c_dev_t *dev)
{
	if (dev == NULL || dev->iic_drv == NULL) {
		return E_INVALID_PARAM;
	}

	// 调用底层 I2C 初始化
	if (dev->iic_drv->init) {
		return dev->iic_drv->init(dev->iic_drv->dev);
	}

	return E_OK;
}

std_ret at24c_read_bytes(at24c_dev_t *dev, uint16_t mem_addr, uint8_t *data, uint32_t len)
{
	if (dev == NULL || data == NULL || len == 0) {
		return E_INVALID_PARAM;
	}

	// 越界保护
	if ((mem_addr + len) > dev->capacity) {
		return E_NOK;
	}

	uint8_t real_dev_addr;
	uint16_t real_mem_addr;

	switch (dev->type)
	{
	case AT24C01:
	case AT24C02:
		real_dev_addr = dev->dev_addr;
		real_mem_addr = mem_addr;
		break;
	case AT24C04:
		real_dev_addr = mem_addr >= 256 ? 0xA2 : 0xA0;
		real_mem_addr = mem_addr & 0xFF;
		break;
	case AT24C08:
	case AT24C16:
		real_dev_addr = dev->dev_addr | ((mem_addr >> 8) << 1);
		real_mem_addr = mem_addr & 0xFF;
		break;
	case AT24C32:
	case AT24C64:
	case AT24C128:
	case AT24C256:
		real_dev_addr = dev->dev_addr;
		real_mem_addr = mem_addr;
		break;
	default:
		return E_INVALID_PARAM;
	}

	return dev->iic_drv->read(dev->iic_drv->dev,
							  real_dev_addr,
							  real_mem_addr,
							  dev->addr_mode,
							  data,
							  len,
							  AT24C_IIC_TIMEOUT);
}

std_ret at24c_write_bytes(at24c_dev_t *dev, uint16_t mem_addr, uint8_t *data, uint32_t len)
{
	std_ret ret = E_OK;
	uint32_t bytes_left = len;
	uint32_t current_addr = mem_addr;
	uint8_t *current_data = data;

	if (dev == NULL || data == NULL || len == 0) {
		return E_INVALID_PARAM;
	}

	// 越界保护
	if ((mem_addr + len) > dev->capacity) {
		return E_NOK;
	}

	while (bytes_left > 0)
	{
		// 计算当前地址在当前页的偏移量
		uint16_t page_offset = current_addr % dev->page_size;

		// 计算当前页还剩多少空间可以写
		uint32_t space_in_page = dev->page_size - page_offset;

		// 确定本次实际写入的字节数
		uint32_t bytes_to_write = (bytes_left < space_in_page) ? bytes_left : space_in_page;

		// 调用底层 I2C 写接口
		ret = dev->iic_drv->write(dev->iic_drv->dev,
								  dev->dev_addr,
								  current_addr,
								  dev->addr_mode,
								  current_data,
								  bytes_to_write,
								  AT24C_IIC_TIMEOUT);

		if (ret != E_OK) {
			return ret; // 写入失败直接返回
		}

		/* * 关键点：EEPROM 每次物理写操作后，需要 5ms 左右的内部编程时间。
		 * 如果不延时直接发起下一次通信，芯片会拒收（不回复 ACK）。
		 * 高阶做法是用空写(ACK Polling)轮询，这里为了兼容你的基础结构使用固定延时。
		 */
		delay_ms(5);

		// 更新剩余数据和地址
		current_addr += bytes_to_write;
		current_data += bytes_to_write;
		bytes_left   -= bytes_to_write;
	}

	return E_OK;
}