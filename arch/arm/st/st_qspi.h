//
// Created by nickyyy on 26-4-18.
//

#ifndef ST_QSPI_H
#define ST_QSPI_H

#include "drv_spi.h"

#ifdef USE_ST_QSPI

#ifdef USE_ST_QSPI1
    extern drv_spi_obj_t drv_qspi_obj1;
#endif /* USE_ST_QSPI1 */

#endif /* USE_ST_QSPI */

#endif //ST_QSPI_H
