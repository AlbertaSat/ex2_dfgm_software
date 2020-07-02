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
 * @file dfgm_FIR.c
 * @author Andrew Williams
 * @author bandren
 * @date 2020-06-18
 */

/********************************************************************************/
/*        Finite impulse response (FIR) filtering functions for the DFGM        */
/********************************************************************************/

#include <stdint.h>
#include "dfgm_FIR.h"

#define FIR_DEFAULT_SHIFT 20000000000
#define FIR_DEFAULT_SAMPLE_PERIOD 100
#define FIR_DEFAULT_TOTAL_TAPS 101
#define FIR_DEFAULT_TAPS_1HZ {8636796,8908655,9386018,10078942,10996304,12145721,13533470,15164425,17041992,19168068,21542992,24165523,27032814,30140408,33482237,37050637,40836369,44828654,49015219,53382353,57914969,62596683,67409896,72335890,77354925,82446353,87588731,92759946,97937338,103097836,108218093,113274624,118243946,123102718,127827887,132396824,136787462,140978436,144949208,148680202,152152917,155350049,158255595,160854954,163135021,165084264,166692800,167952457,168856826,169401299,169583099,169401299,168856826,167952457,166692800,165084264,163135021,160854954,158255595,155350049,152152917,148680202,144949208,140978436,136787462,132396824,127827887,123102718,118243946,113274624,108218093,103097836,97937338,92759946,87588731,82446353,77354925,72335890,67409896,62596683,57914969,53382353,49015219,44828654,40836369,37050637,33482237,30140408,27032814,24165523,21542992,19168068,17041992,15164425,13533470,12145721,10996304,10078942,9386018,8908655,8636796}
#define FIR_DEFAULT_TAPS_10HZ {0,1391726,2793204,4138558,5326427,6213670,6620972,6352075,5225729,3116746,0,-4010410,-8635275,-13425330,-17782794,-21011622,-22394538,-21290464,-17241826,-10078062,0,12369899,25984061,39421276,50995310,58915725,61488798,57339856,45633456,26265283,0,-31467268,-65541738,-98859168,-127512184,-147358633,-154387760,-145111842,-116945774,-68535640,0,86945715,189001136,301435992,418381074,533234228,639148398,729559640,798707963,842103460,856894491,842103460,798707963,729559640,639148398,533234228,418381074,301435992,189001136,86945715,0,-68535640,-116945774,-145111842,-154387760,-147358633,-127512184,-98859168,-65541738,-31467268,0,26265283,45633456,57339856,61488798,58915725,50995310,39421276,25984061,12369899,0,-10078062,-17241826,-21290464,-22394538,-21011622,-17782794,-13425330,-8635275,-4010410,0,3116746,5225729,6352075,6620972,6213670,5326427,4138558,2793204,1391726,0}
#define FIR_DEFAULT_TAPS	FIR_DEFAULT_TAPS_1HZ

#define NULL ((void *)0)

/**
 * Accumulate one data product in x, y, and z dimensions using xdat, ydat, zdat as sample points
 * and the coefficient from FIR_config_t struct..
 */
static void accumulate( int64_t xdat, int64_t ydat, int64_t zdat, struct FIR_acc_t* acc, struct FIR_config_t* FIR_config )
{
	int64_t coef = FIR_config->taps[acc->step];
	acc->acc[X_DIM] += (xdat * coef) / FIR_config->shift;
	acc->acc[Y_DIM] += (ydat * coef) / FIR_config->shift;
	acc->acc[Z_DIM] += (zdat * coef) / FIR_config->shift;
	++acc->step;
}

/**
 * Insert xdat, ydat, zdat into FIR sequence so they can propagate through the filter. This function is used to
 * low pass filter and down sample the data points. This means, accumulation is not done for every single data point.
 * Instead, accumulation is only done such that the filter outputs valid data every FIR_config_t::sample_period data points.
 * This method will return SAMPLE_READY and the
 * input array 'sample' will be filled with the result of accumulation when a valid accumulation is finished.
 *
 * In otherwords, call this function every time a new x, y ,z sample is taken. When it returns SAMPLE_READY, the
 * input array 'sample' is filled with a filtered result of the inputs.
 *
 * @param sample
 * 		Must be array of at least length 3.
 * @param FIR_config
 * 		Used as configuration for filtering, ie, all coeficients and current filter state are drawn from this structure.
 * @returns
 * 		SAMPLE_BUSY:
 * 		Data is propagating through filter, no action need be taken by
 * 		application code.
 *
 * 		SAMPLE_READY:
 * 		Input array 'sample' will be filled with data from x, y, z dimensions using
 * 		macros X_DIM, Y_DIM, and Z_DIM to index the array.
 */
filt_result_e filter_data( int32_t xdat, int32_t ydat, int32_t zdat, int32_t* sample, struct FIR_config_t* FIR_config )
{
	/* Do accumulation for all running accumulators. */
	int i;
	for( i = 0; i < CONFIG_MAX_ACCUMULATORS; ++i ) {
		if( FIR_config->running_acc[i] != NULL ) {
			accumulate(xdat, ydat, zdat, FIR_config->running_acc[i], FIR_config);
		}
	}

	/* Normally, there would be an accumulator for every tap in the filter. However, we are downsampling */
	/* the filtered data, so we only need enough accumulators to meet this downsampling requirement. */
	/* So, check if the tail accumulator is far enough into the filter such that a new accumulator */
	/* needs to start running. */
	if( FIR_config->tail_acc->step >= FIR_config->sample_period ) {
		/* Add new accumulator to running list. */
		insert_tail_acc(FIR_config);
	}

	/* Check if the lead accumulator is full. If it is, cache its data and move it into the idle list. */
	/* return data ready flag. */
	if( FIR_config->lead_acc->step >= FIR_config->total_taps ) {
		/* Cache lead accumulator's data. */
		sample[X_DIM] = (int32_t) (FIR_config->lead_acc->acc[X_DIM]);/* * FIR_config.shift; FIXME, this multiply causes overflow */
		sample[Y_DIM] = (int32_t) (FIR_config->lead_acc->acc[Y_DIM]);/* * FIR_config.shift; */
		sample[Z_DIM] = (int32_t) (FIR_config->lead_acc->acc[Z_DIM]);/* * FIR_config.shift; */

		/* Move the lead accumulator into the idle list (ie, remove it from running list). */
		remove_lead_acc(FIR_config);

		return SAMPLE_READY;
	}

	return SAMPLE_BUSY;
}

/**
 * Find an accumulator in the idle list and move it into the running list as the
 * new tail accumulator.
 */
static void insert_tail_acc( struct FIR_config_t* FIR_config )
{
	struct FIR_acc_t* available_idle_acc = NULL;

	/* Find an idle accumulator. */
	int i;
	for( i = 0; i < CONFIG_MAX_ACCUMULATORS; ++i ) {
		if( FIR_config->idle_acc[i] != NULL ) {
			/* Found an available idle accumulator. */
			available_idle_acc = FIR_config->idle_acc[i];

			/* Remove it from the idle list. */
			FIR_config->idle_acc[i] = NULL;

			break;
		}
	}

	if( available_idle_acc == NULL ) {
		/* Ran out of accumulators. */
		return;
	}

	/* Find an empty ready to run spot to place the idle accumulator. */
	for( i = 0; i < CONFIG_MAX_ACCUMULATORS; ++i ) {
		if( FIR_config->running_acc[i] == NULL ) {
			/* Found a free spot. */
			FIR_config->running_acc[i] = available_idle_acc;

			/* This will be the accumulator at the tail of the input data to the filter. */
			FIR_config->tail_acc = available_idle_acc;

			return;
		}
	}
}

/**
 * Move the lead accumulator into the idle list. Then, find a new lead accumulator out
 * of all the running accumulators. The new lead accumulator is the most progressed through
 * the filter, ie, the next valid sample output from the filter will be from this accumulator.
 */
static void remove_lead_acc( struct FIR_config_t* FIR_config )
{
	struct FIR_acc_t* next_lead = NULL;

	/* Reset state of lead acc. */
	FIR_config->lead_acc->acc[X_DIM] = 0;
	FIR_config->lead_acc->acc[Y_DIM] = 0;
	FIR_config->lead_acc->acc[Z_DIM] = 0;
	FIR_config->lead_acc->step = 0;

	/* Find lead acc in the running list and the accumulator which will succeed it as the new leader. */
	int i;
    unsigned long step_comp = 0;
	for( i = 0; i < CONFIG_MAX_ACCUMULATORS; ++i ) {
		if( FIR_config->running_acc[i] == FIR_config->lead_acc ) {
			/* Found the lead accumulator. Remove it from the running list */
			FIR_config->running_acc[i] = NULL;
		}
		if( FIR_config->running_acc[i] != NULL && FIR_config->running_acc[i]->step >= step_comp && FIR_config->running_acc[i] != FIR_config->lead_acc ) {
			/* Found a running accumulator further into the filter which isn't the current lead. */
			next_lead = FIR_config->running_acc[i];
			step_comp = next_lead->step;
		}
	}

	/* Find a spot in idle list for the lead acc. */
	for( i = 0; i < CONFIG_MAX_ACCUMULATORS; ++i ) {
		if( FIR_config->idle_acc[i] == NULL ) {
			/* Found a spot. */
			FIR_config->idle_acc[i] = FIR_config->lead_acc;
		}
	}

	if( next_lead != NULL ) {
		/* Have the new lead. */
		FIR_config->lead_acc = next_lead;
	}
	else {
		/* all accumulators are idle, any of them can be used as new lead. */
		FIR_config->lead_acc = FIR_config->idle_acc[0];
	}
}

/**
 * Initialize the FIR_config structure.
 */
void init_FIR_config( struct FIR_config_t* FIR_config )
{
	int i;
	int64_t default_taps[FIR_DEFAULT_TOTAL_TAPS] = FIR_DEFAULT_TAPS;

	for( i = 0; i < FIR_DEFAULT_TOTAL_TAPS; ++i ) {
		FIR_config->taps[i] = default_taps[i];
	}
	FIR_config->total_taps = FIR_DEFAULT_TOTAL_TAPS;
	FIR_config->shift = FIR_DEFAULT_SHIFT;
	FIR_config->sample_period = FIR_DEFAULT_SAMPLE_PERIOD;

	for( i = 0; i < CONFIG_MAX_ACCUMULATORS; ++i ) {
		FIR_config->idle_acc[i] = &FIR_config->accumulators[i];
		FIR_config->running_acc[i] = NULL;
		FIR_config->accumulators[i].step = 0;
		FIR_config->accumulators[i].acc[X_DIM] = 0;
		FIR_config->accumulators[i].acc[Y_DIM] = 0;
		FIR_config->accumulators[i].acc[Z_DIM] = 0;
	}

	FIR_config->lead_acc = FIR_config->idle_acc[0];
	FIR_config->tail_acc = FIR_config->idle_acc[0];
	FIR_config->running_acc[0] = FIR_config->idle_acc[0];
	FIR_config->idle_acc[0] = NULL;
}