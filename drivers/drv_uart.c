//
// Created by nickyyy on 26-4-25.
//
#include "drv_uart.h"

std_ret drv_uart_init(drv_uart_obj_t *obj, const drv_uart_config_t *config)
{
	if (NULL == obj || NULL == config || NULL == obj->init)
	{
		return E_INVALID_PARAM;
	}

	return obj->init(obj->dev, config);
}

std_ret drv_uart_deinit(drv_uart_obj_t *obj)
{
	if (NULL == obj || NULL == obj->deinit)
	{
		return E_INVALID_PARAM;
	}

	return obj->deinit(obj->dev);
}

std_ret drv_uart_transmit(drv_uart_obj_t *obj, const uint8_t *data, uint32_t size, uint32_t timeout_ms)
{
	if (NULL == obj || NULL == data || 0 == size)
	{
		return E_INVALID_PARAM;
	}

	if (NULL == obj->transmit)
	{
		return E_INVALID_PARAM;
	}

	return obj->transmit(obj->dev, data, size, timeout_ms);
}