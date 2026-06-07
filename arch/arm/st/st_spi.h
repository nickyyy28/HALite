#ifndef ST_SPI_H
#define ST_SPI_H

#include "drv_spi.h"

#ifdef USE_ST_SPI

#ifdef USE_ST_SPI1
    extern drv_spi_obj_t drv_spi1_obj;
#endif /* USE_ST_SPI1 */

#endif /* USE_ST_SPI */

#endif //ST_SPI_H
