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
 * @author bandren
 * @date 2020-06-18
 */

#ifndef DFGM_CONFIG_H_
#define DFGM_CONFIG_H_

#include <stdbool.h>
#include <pthread.h>

typedef enum
{
	RAW_ENABLE = 0,			/* Raw data selected. */
	FILTER_ENABLE = 1,		/* Filetered data selected. */
	CONFIG_TIMEOUT			/* Timeout determining selected stream. */
} dfgm_config_status_e;

struct dfgm_config_t
{
	dfgm_config_status_e stream;
	pthread_mutex_t lock;
};

/**
 * Get the active data stream.
 * @param config
 * 		Typically driver_toolkit_t::dfgm::dfgm_config
 * @return
 * 		Currently selected data stream.
 */
dfgm_config_status_e dfgm_config_get_stream( struct dfgm_config_t* config );

/**
 * Set the active data stream of the dfgm filter.
 * @param config
 * 		The config struct being used (probably driver_toolkit_t::dfgm::dfgm_config).
 * @param status
 * 		Data stream of choice.
 * @return
 * 		false: invalid parameter or failure to set the enable status.
 * 		true: success.
 */
bool dfgm_config_set_stream( struct dfgm_config_t* config, dfgm_config_status_e status );

/**
 * Initialize a dfgm_config_t structure.
 * @param config
 * 		The structure to initialize.
 * @return
 * 		true: when successful.
 * 		false: when failed to init properly.
 */
bool init_dfgm_config( struct dfgm_config_t* config );

#endif /* LIB_LIBCORE_INCLUDE_DFGM_DFGM_CONFIG_H_ */