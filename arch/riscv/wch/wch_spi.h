#ifndef WCH_SPI_H
#define WCH_SPI_H

#include "drv_spi.h"

#ifdef USE_WCH_SPI

#ifdef USE_WCH_SPI1
    extern drv_spi_obj_t drv_spi1_obj;
#endif /* USE_WCH_SPI1 */

#endif /* USE_WCH_SPI */

#endif //WCH_SPI_H
