//
// Created by nickyyy on 26-4-23.
//
#include "drv_sdmmc.h"    /* 包含你定义的框架头文件 */

/* 注意：请根据你实际使用的芯片系列替换头文件，例如 stm32h7xx_hal.h */
#include "sdmmc.h"


/* ========================================================================= */
/* 1. 具体底层函数的实现 (Static Functions)                                  */
/* ========================================================================= */

static std_ret st_sd_init(void *dev)
{
    // SD_HandleTypeDef *hsd = (SD_HandleTypeDef *)dev;
    // if (HAL_SD_Init(hsd) != HAL_OK) {
    //     return E_NOK;
    // }
    MX_SDMMC2_SD_Init();
    return E_OK;
}

static std_ret st_sd_deinit(void *dev)
{
    SD_HandleTypeDef *hsd = (SD_HandleTypeDef *)dev;
    if (HAL_SD_DeInit(hsd) != HAL_OK) {
        return E_NOK;
    }
    return E_OK;
}

static std_ret st_sd_set_bus_width(void *dev, drv_sdmmc_bus_width_t width)
{
    SD_HandleTypeDef *hsd = (SD_HandleTypeDef *)dev;
    uint32_t hal_width;

    switch (width) {
        case SDMMC_BUS_1_BIT: hal_width = SDMMC_BUS_WIDE_1B; break;
        case SDMMC_BUS_4_BIT: hal_width = SDMMC_BUS_WIDE_4B; break;
        case SDMMC_BUS_8_BIT: hal_width = SDMMC_BUS_WIDE_8B; break;
        default: return E_NOK;
    }

    if (HAL_SD_ConfigWideBusOperation(hsd, hal_width) != HAL_OK) {
        return E_NOK;
    }
    return E_OK;
}

static std_ret st_sd_set_speed(void *dev, drv_sdmmc_speed_t speed)
{
    /* STM32 HAL 的速度通常在 Init 的 hsd->Init.ClockDiv 中设置。
     * 对于 SD NAND，一般通过切换频率分频器来实现。
     * 这里给出一个简化的实现思路，或者你可以留空，直接在 CubeMX 中配好固定频率。*/

    // SD_HandleTypeDef *hsd = (SD_HandleTypeDef *)dev;
    // hsd->Init.ClockDiv = (speed == SDMMC_SPEED_HIGH) ? 0 : 2; // 示例分频
    // return st_sd_init(dev); // 重新初始化生效

    return E_OK;
}

static std_ret st_sd_get_info(void *dev, drv_sdmmc_info_t *info)
{
    SD_HandleTypeDef *hsd = (SD_HandleTypeDef *)dev;
    HAL_SD_CardInfoTypeDef hal_info;

    if (HAL_SD_GetCardInfo(hsd, &hal_info) != HAL_OK) {
        return E_NOK;
    }

    /* 将 HAL 层的信息提取到我们的通用结构体中 */
    info->block_size  = hal_info.LogBlockSize;
    info->block_count = hal_info.LogBlockNbr;
    info->capacity_mb = (hal_info.LogBlockNbr / (1024 * 1024 / hal_info.LogBlockSize));
    info->rca         = hal_info.RelCardAdd;

    return E_OK;
}

static drv_sdmmc_state_t st_sd_get_state(void *dev)
{
    SD_HandleTypeDef *hsd = (SD_HandleTypeDef *)dev;
    HAL_SD_CardStateTypeDef state = HAL_SD_GetCardState(hsd);

    /* 状态映射 */
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
    SD_HandleTypeDef *hsd = (SD_HandleTypeDef *)dev;
    /* 使用轮询阻塞读取。如果底层使用 DMA，可以替换为 HAL_SD_ReadBlocks_DMA 并在此处或上层轮询状态 */
    if (HAL_SD_ReadBlocks(hsd, data, block_addr, num_blocks, 1000) != HAL_OK) {
        return E_NOK;
    }

    /* 等待卡回到传输状态 (TRANSFER) */
    while(HAL_SD_GetCardState(hsd) != HAL_SD_CARD_TRANSFER) {
        /* 如果有 RTOS，可以在这里加入 osDelay 释放 CPU */
    }

    return E_OK;
}

static std_ret st_sd_write_blocks(void *dev, uint32_t block_addr, const uint8_t *data, uint32_t num_blocks)
{
    SD_HandleTypeDef *hsd = (SD_HandleTypeDef *)dev;

    if (HAL_SD_WriteBlocks(hsd, (uint8_t *)data, block_addr, num_blocks, 1000) != HAL_OK) {
        return E_NOK;
    }

    /* 等待内部 Flash 烧写完成，此时卡处于 PROGRAMMING 状态 */
    while(HAL_SD_GetCardState(hsd) != HAL_SD_CARD_TRANSFER) {
        /* 如果有 RTOS，必须在此释放 CPU 调度 */
    }

    return E_OK;
}

static std_ret st_sd_erase_blocks(void *dev, uint32_t start_addr, uint32_t end_addr)
{
    SD_HandleTypeDef *hsd = (SD_HandleTypeDef *)dev;

    if (HAL_SD_Erase(hsd, start_addr, end_addr) != HAL_OK) {
        return E_NOK;
    }

    while(HAL_SD_GetCardState(hsd) != HAL_SD_CARD_TRANSFER) {
    }

    return E_OK;
}


/* 暴露给上层的具体实例对象 */
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