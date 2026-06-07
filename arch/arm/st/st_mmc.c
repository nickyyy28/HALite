/*
 * Copyright (c) 2025 nickyyy
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file st_mmc.c
 * @brief STM32 SDMMC Driver Implementation
 *
 * This file provides the STM32 HAL-based implementation of the SDMMC driver
 * interface defined in drv_sdmmc.h.
 *
 * @author nickyyy
 * @date 2025-04-23
 */

#include "drv_sdmmc.h"

#ifdef USE_ST_SDMMC

#if __has_include("sdmmc.h")
    #include "sdmmc.h"
#else
    #error "sdmmc.h not found. Please ensure the correct HAL SDMMC header is included."
#endif


#if defined STM32H743xx
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_sdmmc.h"
#endif

#if defined STM32F411xx || defined STM32F411xE
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_mmc.h"
#endif

/* Private function prototypes */
static std_ret st_sd_init(void *dev);
static std_ret st_sd_deinit(void *dev);
static std_ret st_sd_set_bus_width(void *dev, drv_sdmmc_bus_width_t width);
static std_ret st_sd_set_speed(void *dev, drv_sdmmc_speed_t speed);
static std_ret st_sd_get_info(void *dev, drv_sdmmc_info_t *info);
static drv_sdmmc_state_t st_sd_get_state(void *dev);
static std_ret st_sd_read_blocks(void *dev, uint32_t block_addr, uint8_t *data, uint32_t num_blocks);
static std_ret st_sd_write_blocks(void *dev, uint32_t block_addr, const uint8_t *data, uint32_t num_blocks);
static std_ret st_sd_erase_blocks(void *dev, uint32_t start_addr, uint32_t end_addr);

/* ========================================================================= */
/* Public Driver Objects                                                     */
/* ========================================================================= */

#ifdef USE_ST_MMC2

drv_sdmmc_obj_t drv_sdmmc_obj2 = {
    .dev           = &hsd2,
    .init          = st_sd_init,
    .deinit        = st_sd_deinit,
    .set_bus_width = st_sd_set_bus_width,
    .set_speed     = st_sd_set_speed,
    .get_info      = st_sd_get_info,
    .get_state     = st_sd_get_state,
    .read_blocks   = st_sd_read_blocks,
    .write_blocks  = st_sd_write_blocks,
    .erase_blocks  = st_sd_erase_blocks,
    .timeout_ms    = 1000
};

#endif /* USE_ST_MMC2 */

/* ========================================================================= */
/* Private Functions                                                         */
/* ========================================================================= */

static std_ret st_sd_init(void *dev)
{
    if (NULL == dev)
    {
        return E_INVALID_PARAM;
    }

    MX_SDMMC2_SD_Init();
    return E_OK;
}

static std_ret st_sd_deinit(void *dev)
{
    if (NULL == dev)
    {
        return E_INVALID_PARAM;
    }

    SD_HandleTypeDef *hsd = (SD_HandleTypeDef *)dev;
    if (HAL_SD_DeInit(hsd) != HAL_OK) {
        return E_NOK;
    }
    return E_OK;
}

static std_ret st_sd_set_bus_width(void *dev, drv_sdmmc_bus_width_t width)
{
    if (NULL == dev)
    {
        return E_INVALID_PARAM;
    }

    SD_HandleTypeDef *hsd = (SD_HandleTypeDef *)dev;
    uint32_t hal_width;

    switch (width) {
        case SDMMC_BUS_1_BIT: hal_width = SDMMC_BUS_WIDE_1B; break;
        case SDMMC_BUS_4_BIT: hal_width = SDMMC_BUS_WIDE_4B; break;
        case SDMMC_BUS_8_BIT: hal_width = SDMMC_BUS_WIDE_8B; break;
        default: return E_INVALID_PARAM;
    }

    if (HAL_SD_ConfigWideBusOperation(hsd, hal_width) != HAL_OK) {
        return E_NOK;
    }
    return E_OK;
}

static std_ret st_sd_set_speed(void *dev, drv_sdmmc_speed_t speed)
{
    if (NULL == dev)
    {
        return E_INVALID_PARAM;
    }

    /* STM32 HAL speed is typically configured in hsd->Init.ClockDiv during initialization.
     * For SD NAND, frequency is usually adjusted by changing the clock divider.
     * This is a simplified implementation - actual speed change may require re-initialization. */
    (void)speed; /* Unused parameter - suppress warning */

    return E_OK;
}

static std_ret st_sd_get_info(void *dev, drv_sdmmc_info_t *info)
{
    if (NULL == dev || NULL == info)
    {
        return E_INVALID_PARAM;
    }

    SD_HandleTypeDef *hsd = (SD_HandleTypeDef *)dev;
    HAL_SD_CardInfoTypeDef hal_info;

    if (HAL_SD_GetCardInfo(hsd, &hal_info) != HAL_OK) {
        return E_NOK;
    }

    /* Extract HAL info to generic structure */
    info->block_size  = hal_info.LogBlockSize;
    info->block_count = hal_info.LogBlockNbr;
    info->capacity_mb = (hal_info.LogBlockNbr / (1024 * 1024 / hal_info.LogBlockSize));
    info->rca         = hal_info.RelCardAdd;

    return E_OK;
}

static drv_sdmmc_state_t st_sd_get_state(void *dev)
{
    if (NULL == dev)
    {
        return SDMMC_STATE_ERROR;
    }

    SD_HandleTypeDef *hsd = (SD_HandleTypeDef *)dev;
    HAL_SD_CardStateTypeDef state = HAL_SD_GetCardState(hsd);

    /* State mapping */
    switch (state) {
        case HAL_SD_CARD_READY:       return SDMMC_STATE_READY;
        case HAL_SD_CARD_IDENTIFICATION: return SDMMC_STATE_IDENT;
        case HAL_SD_CARD_STANDBY:     return SDMMC_STATE_STANDBY;
        case HAL_SD_CARD_TRANSFER:    return SDMMC_STATE_TRANSFER;
        case HAL_SD_CARD_SENDING:     return SDMMC_STATE_SENDING;
        case HAL_SD_CARD_RECEIVING:   return SDMMC_STATE_RECEIVING;
        case HAL_SD_CARD_PROGRAMMING: return SDMMC_STATE_PROGRAMMING;
        case HAL_SD_CARD_DISCONNECTED:return SDMMC_STATE_DISCONNECTED;
        case HAL_SD_CARD_ERROR:
        default:                      return SDMMC_STATE_ERROR;
    }
}

static std_ret st_sd_read_blocks(void *dev, uint32_t block_addr, uint8_t *data, uint32_t num_blocks)
{
    if (NULL == dev || NULL == data || num_blocks == 0)
    {
        return E_INVALID_PARAM;
    }

    SD_HandleTypeDef *hsd = (SD_HandleTypeDef *)dev;

    /* Use polling blocking read. Can be replaced with HAL_SD_ReadBlocks_DMA for DMA mode */
    if (HAL_SD_ReadBlocks(hsd, data, block_addr, num_blocks, drv_sdmmc_obj2.timeout_ms) != HAL_OK) {
        return E_NOK;
    }

    /* Wait for card to return to transfer state */
    while(HAL_SD_GetCardState(hsd) != HAL_SD_CARD_TRANSFER) {
        /* If using RTOS, add osDelay here to release CPU */
    }

    return E_OK;
}

static std_ret st_sd_write_blocks(void *dev, uint32_t block_addr, const uint8_t *data, uint32_t num_blocks)
{
    if (NULL == dev || NULL == data || num_blocks == 0)
    {
        return E_INVALID_PARAM;
    }

    SD_HandleTypeDef *hsd = (SD_HandleTypeDef *)dev;

    if (HAL_SD_WriteBlocks(hsd, (uint8_t *)data, block_addr, num_blocks, drv_sdmmc_obj2.timeout_ms) != HAL_OK) {
        return E_NOK;
    }

    /* Wait for internal flash programming to complete */
    while(HAL_SD_GetCardState(hsd) != HAL_SD_CARD_TRANSFER) {
        /* If using RTOS, must release CPU scheduling here */
    }

    return E_OK;
}

static std_ret st_sd_erase_blocks(void *dev, uint32_t start_addr, uint32_t end_addr)
{
    if (NULL == dev)
    {
        return E_INVALID_PARAM;
    }

    SD_HandleTypeDef *hsd = (SD_HandleTypeDef *)dev;

    if (HAL_SD_Erase(hsd, start_addr, end_addr) != HAL_OK) {
        return E_NOK;
    }

    while(HAL_SD_GetCardState(hsd) != HAL_SD_CARD_TRANSFER) {
    }

    return E_OK;
}

#endif /* USE_ST_SDMMC */
