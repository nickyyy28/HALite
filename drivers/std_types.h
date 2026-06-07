//
// Created by nickyyy on 26-4-16.
//

#ifndef STD_TYPES_H
#define STD_TYPES_H

#include <stdint.h>
#include <stddef.h>

#include "HALite_conf.h"

#define E_OK                0
#define E_NOK               1
#define E_NO_MEM            2
#define E_NOT_FOUND         3
#define E_NOT_IMP           4
#define E_NOT_ENOUGH        5
#define E_BUSY              6
#define E_TIMEOUT           7
#define E_INVALID_PARAM     8


typedef uint8_t std_ret;

#endif //STD_TYPES_H
