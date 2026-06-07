//
// Created by nickyyy on 26-4-20.
//

#ifndef ST_IIC_H
#define ST_IIC_H

#include "drv_iic.h"

#ifdef USE_ST_IIC

#ifdef USE_ST_IIC4
    extern drv_iic_obj_t drv_iic4_obj;
#endif /* USE_ST_IIC4 */

#endif /* USE_ST_IIC */

#endif //ST_IIC_H
