/*
 * Copyright (C) 2015  University of Alberta
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
 * @file dfgm_config.h
 * @author Andrew Williams
 * @date 2020-06-18
 */

#ifndef DFGM_CONFIG_H
#define DFGM_CONFIG_H

typedef enum {
	RAW_ENABLE = 0,			/* Raw data selected. */
	FILTER_ENABLE = 1,		/* Filetered data selected. */
	CONFIG_TIMEOUT			/* Timeout determining selected stream. */
} dfgm_config_status_e;

struct dfgm_config_t {
	dfgm_config_status_e stream;
	//mutex_t lock;
};

// void vTaskDFGM(void * pvParameters);
// void vTaskDFGMWrite(void * pvParameters);
//
// int32_t DFGM_filter(int32_t *buffer);
// bool_t DFGM_init(void);
// bool_t DFGM_start(void);
// void DFGM_stop(void);
// void DFGM_lock(void);
// void DFGM_unlock(void);
// void DFGM_printConfig(void);
// bool_t DFGM_is_running( );

#endif /* DFGM_CONFIG_H_ */