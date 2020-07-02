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
 * @file dfgm_FIR.h
 * @author Andrew Williams
 * @author bandren
 * @date 2020-06-18
 */

#ifndef DFGM_FIR_H
#define DFGM_FIR_H

/********************************************************************************/
/*        Finite impulse response (FIR) filtering functions for the DFGM        */
/********************************************************************************/

#include <stdint.h>

#define MAG_DIM 3
#define X_DIM 0
#define Y_DIM 1
#define Z_DIM 2

/* Max number of FIR taps that can ever be used. */
#define MAX_TAPS 1001

/* minimum period between samples. 10 means at minimum every 10'th sample is taken. */
/* 100 means at minimum every 100'th sample is taken, if this was defined as 100, it would */
/* be impossible to take every 10'th sample because the minimum is 100. */
/* This must never be zero. */
#define FIR_MIN_DOWN_SAMPLE 1

#define CONFIG_MAX_ACCUMULATORS ((MAX_TAPS/FIR_MIN_DOWN_SAMPLE)+1) /* floor of division plus one. */

/**
 * @struct FIR_acc_t
 * @var FIR_acc_t::acc
 * 		An array of length three to hold the accumulation of x, y, and z mag data.
 * @var FIR_acc_t::step
 * 		Propagation point within the FIR filter. For example:
 * 			step = 0 => No accumulation has been done
 * 			step = 1 => product of first sample/coef pair
 * 			step = 2 => product of second sample/coef pair
 * 			step = n => product of n'th sample/coef pair
 */
struct FIR_acc_t {
	int64_t       acc[MAG_DIM];
	unsigned long step;
};

/**
 * @struct FIR_config_t
 * @var FIR_config_t::taps
 * 		Coefficients where the product is taken to be:
 * 			sample zero * taps[0]
 * 			sample one	* taps[1]
 * 			sample two	* taps[2]
 * 			...
 * @var FIR_config_t::total_taps
 * 		Total number of coefficients in FIR_acc_t::taps
 * @var FIR_config_t::shift
 * 		A decimal value to divide accumulation results by. Set this as '1' to ignore it.
 * @var FIR_config_t::accumulators
 *		Array of accumulators for each sample progressing through filter. If down sampling was 1 for 1 (ie, no down sampling
 *		just filter) then this array is the same length as MAX_TAPS. If down sampling was 1 every 100 samples and there were
 *		101 coefficients, then this array would be length 2.
 * @var FIR_config_t::running_acc
 * 		The running accumulator channel. Accumulators in this array are currently being used in the filter.
 * @var FIR_config_t::idle_acc
 * 		Idle accumulator channel. Accumulators in this array are not being used by the filter and can be taken
 * 		for use.
 * @var FIR_config_t::lead_acc
 * 		Pointer to accumulator which is the most progressed through the filter (ie, the next sample output by the filter will
 * 		be from this accumulator).
 * @var FIR_config_t::tail_acc
 * 		Pointer to accumulator which is the least progressed through the filter (ie, this accumulator contains the least amount
 * 		of product sum pairs out of all running accumulators).
 */
struct FIR_config_t {
	int64_t             taps[MAX_TAPS];
	unsigned long 	    total_taps;
	int64_t	            shift;
  	unsigned long    	sample_period;
	struct FIR_acc_t 	accumulators[CONFIG_MAX_ACCUMULATORS];
  	struct FIR_acc_t*	running_acc[CONFIG_MAX_ACCUMULATORS];
  	struct FIR_acc_t* 	idle_acc[CONFIG_MAX_ACCUMULATORS];
	struct FIR_acc_t* 	lead_acc;
  	struct FIR_acc_t* 	tail_acc;
};

/* Results of filter_data method. */
typedef enum {
	SAMPLE_READY = 0,
	SAMPLE_BUSY = 1
} filt_result_e;

filt_result_e filter_data( int32_t, int32_t, int32_t, int32_t*, struct FIR_config_t* );

#endif /* DFGM_FIR_H_ */