//
// Created by nickyyy on 26-4-22.
//

#ifndef DRV_SDMMC_H
#define DRV_SDMMC_H

#include "std_types.h"

typedef enum
{
    SDMMC_BUS_1_BIT = 0,
    SDMMC_BUS_4_BIT,
    SDMMC_BUS_8_BIT   // eMMC 常用的 8 线模式
} drv_sdmmc_bus_width_t;

typedef enum
{
    SDMMC_SPEED_DEFAULT = 0, // 默认速度 (通常 <= 25MHz)
    SDMMC_SPEED_HIGH         // 高速模式 (通常 <= 50MHz)
} drv_sdmmc_speed_t;

typedef enum
{
    SDMMC_STATE_IDLE = 0,
    SDMMC_STATE_READY,
    SDMMC_STATE_IDENT,
    SDMMC_STATE_STANDBY,
    SDMMC_STATE_TRANSFER,
    SDMMC_STATE_SENDING,
    SDMMC_STATE_RECEIVING,
    SDMMC_STATE_PROGRAMMING,
    SDMMC_STATE_DISCONNECTED,
    SDMMC_STATE_ERROR
} drv_sdmmc_state_t;

typedef struct
{
    uint32_t block_size;     // 块大小 (通常固定为 512 字节)
    uint32_t block_count;    // 总逻辑块数
    uint32_t capacity_mb;    // 容量 (MB)
    uint32_t rca;            // 相对卡地址 (Relative Card Address)
} drv_sdmmc_info_t;

/* SDMMC 驱动对象结构体 */
typedef struct
{
    void *dev;  // 底层硬件句柄 (例如 STM32 的 SD_HandleTypeDef*)

    /* 基础生命周期控制 */
    std_ret (*init)(void* dev);
    std_ret (*deinit)(void* dev);

    /* 配置接口 */
    std_ret (*set_bus_width)(void* dev, drv_sdmmc_bus_width_t width);
    std_ret (*set_speed)(void* dev, drv_sdmmc_speed_t speed);

    /* 信息与状态接口 */
    std_ret (*get_info)(void* dev, drv_sdmmc_info_t *info);
    drv_sdmmc_state_t (*get_state)(void* dev);

    /* 核心块读写接口 (面向文件系统) */
    // block_addr: 目标扇区号
    // data: 数据指针
    // num_blocks: 读写的扇区数量
    std_ret (*read_blocks)(void* dev, uint32_t block_addr, uint8_t *data, uint32_t num_blocks);
    std_ret (*write_blocks)(void* dev, uint32_t block_addr, const uint8_t *data, uint32_t num_blocks);
    std_ret (*erase_blocks)(void* dev, uint32_t start_addr, uint32_t end_addr);

    /* 超时设置 */
    uint32_t timeout_ms;
} drv_sdmmc_obj_t;

/* ========================================================================= */
/* 对上层应用的 API 包装声明 */
/* ========================================================================= */

std_ret drv_sdmmc_init(drv_sdmmc_obj_t *obj);
std_ret drv_sdmmc_deinit(drv_sdmmc_obj_t *obj);
std_ret drv_sdmmc_set_bus_width(drv_sdmmc_obj_t *obj, drv_sdmmc_bus_width_t width);
std_ret drv_sdmmc_set_speed(drv_sdmmc_obj_t *obj, drv_sdmmc_speed_t speed);
std_ret drv_sdmmc_get_info(drv_sdmmc_obj_t *obj, drv_sdmmc_info_t *info);
drv_sdmmc_state_t drv_sdmmc_get_state(drv_sdmmc_obj_t *obj);
std_ret drv_sdmmc_read_blocks(drv_sdmmc_obj_t *obj, uint32_t block_addr, uint8_t *data, uint32_t num_blocks);
std_ret drv_sdmmc_write_blocks(drv_sdmmc_obj_t *obj, uint32_t block_addr, const uint8_t *data, uint32_t num_blocks);
std_ret drv_sdmmc_erase_blocks(drv_sdmmc_obj_t *obj, uint32_t start_addr, uint32_t end_addr);

#endif //DRV_SDMMC_H
