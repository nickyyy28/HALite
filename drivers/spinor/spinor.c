//
// Created by nickyyy on 26-4-18.
//
#include "spinor.h"
#include "log.h"
#include "drv_tick.h"

static std_ret spinor_execute_operation(const drv_spi_obj_t *spi_drv, spi_nor_operate_t op, uint32_t addr, uint8_t *data, uint32_t size, drv_spi_direction_t direction);

std_ret w25q256jviq_probe(drv_spi_obj_t *spi_obj, struct spi_nor_device_t *device);
bool w25q256jviq_is_busy(const drv_spi_obj_t *spi_obj);

spi_nor_device_t w25q256jviq_dual_flash = {
	0xEF4019,
	w25q256jviq_probe,
	w25q256jviq_is_busy,
	{0x9F, SPI_LINE_SINGLE, SPI_ADDR_4_BYTE, SPI_LINE_NONE, SPI_LINE_SINGLE, SPI_DUMMY_NONE},
	{0xEC, SPI_LINE_SINGLE, SPI_ADDR_4_BYTE, SPI_LINE_QUAD, SPI_LINE_QUAD, SPI_DUMMY_6},
	{0x34, SPI_LINE_SINGLE, SPI_ADDR_4_BYTE, SPI_LINE_SINGLE, SPI_LINE_QUAD, SPI_DUMMY_NONE},
	{0x21, SPI_LINE_SINGLE, SPI_ADDR_4_BYTE, SPI_LINE_SINGLE, SPI_LINE_NONE, SPI_DUMMY_NONE},
	{0x06, SPI_LINE_SINGLE, SPI_ADDR_4_BYTE, SPI_LINE_NONE, SPI_LINE_NONE, SPI_DUMMY_NONE},
};

spi_nor_device_t w25q256jviq_dual_flash_single_line = {
	0xEF4019,
	w25q256jviq_probe,
	w25q256jviq_is_busy,
	{0x9F, SPI_LINE_SINGLE, SPI_ADDR_4_BYTE, SPI_LINE_NONE, SPI_LINE_SINGLE, SPI_DUMMY_NONE},
	{0x13, SPI_LINE_SINGLE, SPI_ADDR_4_BYTE, SPI_LINE_SINGLE, SPI_LINE_SINGLE, SPI_DUMMY_NONE},
	{0x12, SPI_LINE_SINGLE, SPI_ADDR_4_BYTE, SPI_LINE_SINGLE, SPI_LINE_SINGLE, SPI_DUMMY_NONE},
	{0x21, SPI_LINE_SINGLE, SPI_ADDR_4_BYTE, SPI_LINE_SINGLE, SPI_LINE_NONE, SPI_DUMMY_NONE},
	{0x06, SPI_LINE_SINGLE, SPI_ADDR_4_BYTE, SPI_LINE_NONE, SPI_LINE_NONE, SPI_DUMMY_NONE},
};

bool w25q256jviq_is_busy(const drv_spi_obj_t *spi_obj)
{
	if (NULL == spi_obj)
	{
		return true;
	}

	uint8_t read_buffer[2] = {0};

	//read status register1
	std_ret ret = spi_obj->writeread(spi_obj->dev, 0x05, SPI_LINE_SINGLE, 0x0, SPI_LINE_NONE, SPI_ADDR_4_BYTE, SPI_DUMMY_NONE, read_buffer, 2, SPI_LINE_SINGLE, SPI_DATA_IN);
	if (E_OK != ret)
	{
		LOG_ERROR("read status register1 fail");
		return true;
	}

	if ((read_buffer[0] & 0x1) || (read_buffer[1] & 0x1))
	{
		return true;
	}

	return false;
}

std_ret w25q256jviq_probe(drv_spi_obj_t *spi_obj, struct spi_nor_device_t *device)
{
	if (NULL == spi_obj)
	{
		return E_INVALID_PARAM;
	}

	uint8_t read_buffer[6] = {0};
	uint8_t write_buffer[2] = {0};
	std_ret ret = spinor_execute_operation(spi_obj, device->read_id_op, 0x0, &read_buffer[0], 6, SPI_DATA_IN);
	uint32_t jedec1 = read_buffer[0] << 16 | read_buffer[2] << 8 | read_buffer[4];
	uint32_t jedec2 = read_buffer[1] << 16 | read_buffer[3] << 8 | read_buffer[5];
	LOG_INFO("w25q256jviq_init read jedec1 = 0x%08x, jedec2 = 0x%08x, ret = %d", jedec1, jedec2, ret);
	if (E_OK != ret)
	{
		return E_NOK;
	}

	if (jedec1 != jedec2 || jedec1 != device->jedec_id)
	{
		return E_NOK;
	}

	//read status register2
	ret = spi_obj->writeread(spi_obj->dev, 0x35, SPI_LINE_SINGLE, 0x0, SPI_LINE_NONE, SPI_ADDR_4_BYTE, SPI_DUMMY_NONE, read_buffer, 2, SPI_LINE_SINGLE, SPI_DATA_IN);
	if (E_OK != ret)
	{
		LOG_ERROR("read status register2 fail");
		return E_NOK;
	}

	LOG_INFO("status chip1 reg1 = 0x%x, chip2 reg2 = 0x%x", read_buffer[0], read_buffer[1]);

	// check QE bit
	if (!(read_buffer[0] & 0x2) || !(read_buffer[1] & 0x2))
	{
		ret = spinor_execute_operation(spi_obj, device->write_enable_op, 0x0, NULL, 0, SPI_DATA_OUT);
		if (E_OK != ret)
		{
			LOG_ERROR("Write enable status register fail");
			return E_NOK;
		}

		//enable qe bit
		write_buffer[0] = read_buffer[0] | 0x2;
		write_buffer[1] = read_buffer[1] | 0x2;
		ret = spi_obj->writeread(spi_obj->dev, 0x31, SPI_LINE_SINGLE, 0x0, SPI_LINE_NONE, SPI_ADDR_4_BYTE, SPI_DUMMY_NONE, write_buffer, 2, SPI_LINE_SINGLE, SPI_DATA_IN);
		if (E_OK != ret)
		{
			LOG_ERROR("write status register2 fail");
			return E_NOK;
		}

		ret = spi_obj->writeread(spi_obj->dev, 0x35, SPI_LINE_SINGLE, 0x0, SPI_LINE_NONE, SPI_ADDR_4_BYTE, SPI_DUMMY_NONE, read_buffer, 2, SPI_LINE_SINGLE, SPI_DATA_IN);
		if (E_OK != ret)
		{
			LOG_ERROR("read back status register2 fail");
			return E_NOK;
		}
		if (!(read_buffer[0] & 0x2) || !(read_buffer[1] & 0x2))
		{
			LOG_ERROR("read back status register2 compare fail, reg1 = 0x%x, reg2 = 0x%x", read_buffer[0], read_buffer[1]);
			return E_NOK;
		}
	}

	//enter 4byte addr mode
	ret = spi_obj->writeread(spi_obj->dev, 0xB7, SPI_LINE_SINGLE, 0x0, SPI_LINE_NONE, SPI_ADDR_4_BYTE, SPI_DUMMY_NONE, NULL, 0, SPI_LINE_NONE, SPI_DATA_IN);
	if (E_OK != ret)
	{
		LOG_ERROR("enter 4byte addr mode fail");
		return E_NOK;
	}

	return E_OK;
}

static std_ret spinor_execute_operation(const drv_spi_obj_t *spi_drv, spi_nor_operate_t op, uint32_t addr, uint8_t *data, uint32_t size, drv_spi_direction_t direction)
{
	if (NULL == spi_drv)
	{
		return E_INVALID_PARAM;
	}

	return spi_drv->writeread(spi_drv->dev, op.command, op.cmd_line, addr, op.addr_line, op.addr_mode, op.dummy_cycles, data, size, op.data_line, direction);
}

std_ret spinor_probe_device(const drv_spi_obj_t *spi_drv, spi_nor_device_t **spi_nor)
{
	if (NULL == spi_drv || NULL == spi_nor)
	{
		return E_INVALID_PARAM;
	}

	*spi_nor = &w25q256jviq_dual_flash;

	return E_OK;
}

std_ret spinor_erase_sector(const drv_spi_obj_t *spi_drv, const spi_nor_device_t *spi_nor, uint32_t addr)
{
	if (NULL == spi_drv || NULL == spi_nor)
	{
		return E_INVALID_PARAM;
	}

	//write enable
	std_ret ret = spinor_execute_operation(spi_drv, spi_nor->write_enable_op, 0x0, NULL, 0, SPI_DATA_OUT);
	if (E_OK != ret)
	{
		return E_NOK;
	}

	return spinor_execute_operation(spi_drv, spi_nor->erase_sector_op, addr, NULL, 0, SPI_DATA_OUT);
}

std_ret spinor_page_program(const drv_spi_obj_t *spi_drv, const spi_nor_device_t *spi_nor, uint32_t addr, const uint8_t *data, uint32_t size)
{
	if (NULL == spi_drv || NULL == spi_nor)
	{
		return E_INVALID_PARAM;
	}

	//write enable
	std_ret ret = spinor_execute_operation(spi_drv, spi_nor->write_enable_op, 0x0, NULL, 0, SPI_DATA_OUT);
	if (E_OK != ret)
	{
		return E_NOK;
	}
	return  spinor_execute_operation(spi_drv, spi_nor->program_page_op, addr, (uint8_t*)data, size, SPI_DATA_OUT);
}


std_ret spinor_read(const drv_spi_obj_t *spi_drv, const spi_nor_device_t *spi_nor, uint32_t addr, uint8_t *data, uint32_t size)
{
	if (NULL == spi_drv || NULL == spi_nor)
	{
		return E_INVALID_PARAM;
	}

	return  spinor_execute_operation(spi_drv, spi_nor->read_op, addr, data, size, SPI_DATA_IN);
}

std_ret spinor_wait_not_busy(const drv_spi_obj_t *spi_drv, const spi_nor_device_t *spi_nor, uint32_t timeout_ms)
{
	if (spi_drv == NULL || spi_nor == NULL) return E_INVALID_PARAM;

	uint32_t start = get_tick_ms();

	do {
		if (!spi_nor->flash_is_busy(spi_drv)) {
			return E_OK;
		}
        delay_ms(1);
	} while (get_tick_ms() - start < timeout_ms);

	if (!spi_nor->flash_is_busy(spi_drv)) return E_OK;

	return E_TIMEOUT;
}