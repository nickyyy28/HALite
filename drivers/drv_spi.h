//
// Created by nickyyy on 26-4-16.
//

#ifndef DRV_SPI_H
#define DRV_SPI_H

#include "std_types.h"

typedef enum
{
    SPI_LINE_NONE = 0,
    SPI_LINE_SINGLE,
    SPI_LINE_DUAL,
    SPI_LINE_QUAD,
    SPI_LINE_OCTAL
}drv_spi_lines_t;

typedef enum
{
    SPI_DUMMY_NONE = 0,
    SPI_DUMMY_1,
    SPI_DUMMY_2,
    SPI_DUMMY_3,
    SPI_DUMMY_4,
    SPI_DUMMY_5,
    SPI_DUMMY_6,
    SPI_DUMMY_7,
    SPI_DUMMY_8,
    SPI_DUMMY_9,
    SPI_DUMMY_10,
    SPI_DUMMY_11,
    SPI_DUMMY_12,
    SPI_DUMMY_13,
    SPI_DUMMY_14,
    SPI_DUMMY_15,
    SPI_DUMMY_16,
    SPI_DUMMY_17,
    SPI_DUMMY_18,
    SPI_DUMMY_19,
    SPI_DUMMY_20,
    SPI_DUMMY_21,
    SPI_DUMMY_22,
    SPI_DUMMY_23,
    SPI_DUMMY_24,
    SPI_DUMMY_25,
    SPI_DUMMY_26,
    SPI_DUMMY_27,
    SPI_DUMMY_28,
    SPI_DUMMY_29,
    SPI_DUMMY_30,
    SPI_DUMMY_31,
    SPI_DUMMY_32,
}drv_spi_dummy_cycles_t;

typedef enum
{
    SPI_ADDR_3_BYTE,
    SPI_ADDR_4_BYTE,
}drv_spi_addr_mode_t;

typedef enum
{
    SPI_DATA_IN,
    SPI_DATA_OUT,
}drv_spi_direction_t;


typedef struct
{
    void *dev;
    std_ret (*init)(void* dev);
    std_ret (*deinit)(void* dev);
    std_ret (*write)(void* dev, const uint8_t *data, uint32_t size, drv_spi_lines_t line);
    std_ret (*read)(void* dev, uint8_t *dst, uint32_t size, drv_spi_lines_t line);
    std_ret (*writeread)(void* dev, uint8_t cmd, drv_spi_lines_t cmd_line,
        uint32_t addr, drv_spi_lines_t addr_line, drv_spi_addr_mode_t addr_mode, drv_spi_dummy_cycles_t dummy_cycles,
        uint8_t *data, uint32_t data_size, drv_spi_lines_t data_line, drv_spi_direction_t direction);
    std_ret (*set_cs)(uint8_t pulse);
    uint32_t timeout_ms;
}drv_spi_obj_t;


std_ret drv_spi_init(drv_spi_obj_t *obj);
std_ret drv_spi_deinit(drv_spi_obj_t *obj);
std_ret drv_spi_write(drv_spi_obj_t *obj, const uint8_t *data, uint32_t size, drv_spi_lines_t line);
std_ret drv_spi_read(drv_spi_obj_t *obj, uint8_t *dst, uint32_t size, drv_spi_lines_t line);
std_ret drv_spi_writeread(const drv_spi_obj_t *obj, uint8_t cmd, drv_spi_lines_t cmd_line,
    uint32_t addr, drv_spi_lines_t addr_line, drv_spi_addr_mode_t addr_mode, drv_spi_dummy_cycles_t dummy_cycles,
    uint8_t *data, uint32_t data_size, drv_spi_lines_t data_line, drv_spi_direction_t direction);



#endif //DRV_SPI_H
