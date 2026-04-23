//
// Created by nickyyy on 26-4-15.
//
#include "drv_spi.h"

#include <stddef.h>

std_ret drv_spi_init(drv_spi_obj_t *obj)
{
    std_ret ret = E_NOK;
    if (NULL != obj)
    {
        ret = obj->init(obj->dev);
    } else
    {
        ret = E_INVALID_PARAM;
    }
    return ret;
}

std_ret drv_spi_deinit(drv_spi_obj_t *obj)
{
    std_ret ret = E_NOK;
    if (NULL != obj)
    {
        ret = obj->deinit(obj->dev);
    } else
    {
        ret = E_INVALID_PARAM;
    }
    return ret;
}

std_ret drv_spi_write(drv_spi_obj_t *obj, const uint8_t *data, uint32_t size, drv_spi_lines_t line)
{
    std_ret ret = E_NOK;
    if (NULL != obj)
    {
        ret = obj->write(obj->dev, data, size, line);
    } else
    {
        ret = E_INVALID_PARAM;
    }
    return ret;
}

std_ret drv_spi_read(drv_spi_obj_t *obj, uint8_t *dst, uint32_t size, drv_spi_lines_t line)
{
    std_ret ret = E_NOK;
    if (NULL != obj)
    {
        ret = obj->read(obj->dev, dst, size, line);
    } else
    {
        ret = E_INVALID_PARAM;
    }
    return ret;
}

std_ret drv_spi_writeread(drv_spi_obj_t *obj, uint8_t cmd, drv_spi_lines_t cmd_line,
    uint32_t addr, drv_spi_lines_t addr_line, drv_spi_addr_mode_t addr_mode, drv_spi_dummy_cycles_t dummy_cycles,
    uint8_t *data, uint32_t data_size, drv_spi_lines_t data_line, drv_spi_direction_t direction)
{
    std_ret ret = E_NOK;
    if (NULL != obj)
    {
        ret = obj->writeread(obj->dev, cmd, cmd_line, addr, addr_line, addr_mode, dummy_cycles, data, data_size, data_line, direction);
    } else
    {
        ret = E_INVALID_PARAM;
    }
    return ret;
}