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
 * @file dfgm_config.c
 * @author Andrew Williams
 * @author bandren
 * @date 2020-06-18
 */

#include <stdio.h>
#include <dfgm_config.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>


#define DFGM_SELECT_FILE_LENGTH     1
#define STREAM_ENABLE_BYTE_LOCATION 0
#define CONFIG_LOCK_TIMEOUT         (50) /*50 ms*/
#define MUTEX_BUSY                  false
#define DFGM_SELECT_FILE_PATH "~/DFGM.txt"

static bool dfgm_config_set( struct dfgm_config_t* config, dfgm_config_status_e status, long int location )
{
	FILE* select_file;
	DEV_ASSERT(config);

	select_file = fopen(DFGM_SELECT_FILE_PATH, "r+");
	if( select_file == NULL ) {
		select_file = fopen(DFGM_SELECT_FILE_PATH, "w+");
		if( select_file == NULL ) {
			return false;
		}
	}

	rewind(select_file);

	if( fputc(status, select_file) != status ) {
		csp_printf("dfgm config: diag: failed write");
		fclose(select_file);
		return false;
	}
	fclose(select_file);
	return true;
}

dfgm_config_status_e dfgm_config_get_stream( struct dfgm_config_t* config )
{
	dfgm_config_status_e status;
	DEV_ASSERT(config);

	if( lock_mutex(config->lock, CONFIG_LOCK_TIMEOUT) == MUTEX_BUSY ) {
		return CONFIG_TIMEOUT;
	}
	status = config->stream;
	unlock_mutex(config->lock);

	return status;
}


bool dfgm_config_set_stream( struct dfgm_config_t* config, dfgm_config_status_e status )
{
	DEV_ASSERT(config);

	if( dfgm_config_set(config, status, STREAM_ENABLE_BYTE_LOCATION) == false ) {
		return false;
	}

	if( lock_mutex(config->lock, CONFIG_LOCK_TIMEOUT) == MUTEX_BUSY ) {
		return false;
	}
	config->stream = status;
	unlock_mutex(config->lock);

	return true;
}

bool init_dfgm_config( struct dfgm_config_t* config )
{
	FILE* select_file;
	char select_stream[DFGM_SELECT_FILE_LENGTH];
	off_t stream_size;
	int stat_err;
	struct stat file_stat;
	DEV_ASSERT(config);

	config->stream = FILTER_ENABLE;

	/* Create RTOS mutex. */
	new_mutex(config->lock);
	if( config->lock == NULL ) {
		return false;
	}

	/* Open up selection file and cache the previous selection. */
	select_file = fopen(DFGM_SELECT_FILE_PATH, "r");
	if( select_file == NULL ) {
		/* Use defaults. */
		return true;
	}

	stream_size = fread(select_stream, sizeof(char), DFGM_SELECT_FILE_LENGTH, select_file);
	stat_err = fstat(fileno(select_file), &file_stat);
	stream_size = file_stat.st_size;
	fclose(select_file);
	/* Check for corruption. */

	if( stat_err >= 0 && stream_size != DFGM_SELECT_FILE_LENGTH ) {
		/* Corruption, use defaults */
		remove(DFGM_SELECT_FILE_PATH);
		return true;
	}

	config->stream = select_stream[STREAM_ENABLE_BYTE_LOCATION];
	return true;
}