/*
 * Copyright (C) 2021  University of Alberta
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
/**
 * @file dfgm.h
 * @author Daniel Sacro
 * @date 2022-02-08
 */

#ifndef DFGM_H
#define DFGM_H

#include "dfgm_handler.h"
#include <stdint.h>
#include <time.h>

typedef struct __attribute__((packed)) {
    uint16 coreVoltage;
    uint16 sensorTemp;
    uint16 refTemp;
    uint16 boardTemp;
    uint16 posRailVoltage;
    uint16 inputVoltage;
    uint16 refVoltage;
    uint16 inputCurrent;
    uint16 reserved1;
    uint16 reserved2;
    uint16 reserved3;
    uint16 reserved4;
} DFGM_Housekeeping;

DFGM_return HAL_DFGM_run(int32_t givenRuntime);
DFGM_return HAL_DFGM_stop();
DFGM_return HAL_DFGM_get_HK(DFGM_Housekeeping *DFGM_hk);

#endif /* DFGM_H */
