//
// Created by nickyyy on 26-4-18.
//

#ifndef SPINOR_H
#define SPINOR_H

#include <stdbool.h>

#include "drv_spi.h"

typedef struct
{
    uint8_t command;
    drv_spi_lines_t cmd_line;
    drv_spi_addr_mode_t addr_mode;
    drv_spi_lines_t addr_line;
    drv_spi_lines_t data_line;
    drv_spi_dummy_cycles_t dummy_cycles;
}spi_nor_operate_t;


typedef struct spi_nor_device_t
{
    uint32_t jedec_id;
    std_ret (*flash_probe)(drv_spi_obj_t *spi_drv, struct spi_nor_device_t *device);
    bool (*flash_is_busy)(const drv_spi_obj_t *spi_drv);
    spi_nor_operate_t read_id_op;
    spi_nor_operate_t read_op;
    spi_nor_operate_t program_page_op;
    spi_nor_operate_t erase_sector_op;
    spi_nor_operate_t write_enable_op;
}spi_nor_device_t;

std_ret spinor_probe_device(const drv_spi_obj_t *spi_drv, spi_nor_device_t **spi_nor);

std_ret spinor_erase_sector(const drv_spi_obj_t *spi_drv, const spi_nor_device_t *spi_nor, uint32_t addr);
std_ret spinor_page_program(const drv_spi_obj_t *spi_drv, const spi_nor_device_t *spi_nor, uint32_t addr, const uint8_t *data, uint32_t size);
std_ret spinor_read(const drv_spi_obj_t *spi_drv, const spi_nor_device_t *spi_nor, uint32_t addr, uint8_t *data, uint32_t size);
std_ret spinor_wait_not_busy(const drv_spi_obj_t *spi_drv, const spi_nor_device_t *spi_nor, uint32_t timeout_ms);



#endif //SPINOR_H
