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
 * @file dfgm.c
 * @author Andrew Williams
 * @date 2020-06-18
 */

#include <stdbool.h>
#include <libio.h> // for file
#include <sys/stat.h>

#define DFGM_SELECT_FILE_LENGTH 1

bool init_dfgm_config( struct dfgm_config_t* config )
{
	// _IO_FILE* select_file;
	// char select_stream[DFGM_SELECT_FILE_LENGTH];
	// long stream_size;
	// int stat_err;
	// struct stat file_stat;
	// DEV_ASSERT(config);

	// config->stream = DFGM_DEFAULT_STREAM;

	/* Create RTOS mutex. */
	new_mutex(config->lock);
	if( config->lock == NULL ) {
		return false;
	}

	// /* Open up selection file and cache the previous selection. */
	// select_file = fopen(DFGM_SELECT_FILE_PATH, "r");
	// if( select_file == NULL ) {
	// 	/* Use defaults. */
	// 	return true;
	// }

	// stream_size = fread(select_stream, sizeof(char), DFGM_SELECT_FILE_LENGTH, select_file);
	// stat_err = fstat(fileno(select_file), &file_stat);
	// stream_size = file_stat.st_size;
	// fclose(select_file);
	// /* Check for corruption. */

	// if( stat_err >= 0 && stream_size != DFGM_SELECT_FILE_LENGTH ) {
	// 	/* Corruption, use defaults */
	// 	remove(DFGM_SELECT_FILE_PATH);
	// 	return true;
	// }

	// config->stream = select_stream[STREAM_ENABLE_BYTE_LOCATION];
	return true;
}

int testFunction( ) {
    struct dfgm_config_t* config;
    return init_dfgm_config(config);
}