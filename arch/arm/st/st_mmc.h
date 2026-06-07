//
// Created by nickyyy on 26-4-23.
//

#ifndef ST_MMC_H
#define ST_MMC_H

#include "drv_sdmmc.h"

#ifdef USE_ST_SDMMC

#ifdef USE_ST_MMC2
    extern drv_sdmmc_obj_t drv_sdmmc_obj2;
#endif /* USE_ST_MMC2 */

#endif /* USE_ST_SDMMC */

#endif //ST_MMC_H
