/*
 *  Copyright (C) 2003 Jack O'Quin, Steve Harris
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  $Id: process.h,v 1.24 2004/04/04 15:47:15 jdepner Exp $
 */

#ifndef PROCESS_H
#define PROCESS_H

#include <jack/jack.h>

#define BINS  2048			/* must be power of two */
#define OVER_SAMP  8			/* buffer overlap count, must
					   be a factor of BINS */
#define BANDS 30

#define UPPER_SPECTRUM_DB  3.0
#define LOWER_SPECTRUM_DB  -60.0
#define SPECTRUM_RANGE_DB  (UPPER_SPECTRUM_DB - LOWER_SPECTRUM_DB)


/*  Using this because "round" isn't available (AFAICT) in gcc 2.9X.  */

#define NINT(a) ((a)<0.0 ? (int) ((a) - 0.5) : (int) ((a) + 0.5))


#include "plugin.h"
#include "compressor.h"
#include "limiter.h"

/* number of input and output channels */
#define NCHANNELS 2
#define CHANNEL_L 0
#define CHANNEL_R 1

/* crossover bands */
#define XO_LOW  0
#define XO_MID  1
#define XO_HIGH 2
#define XO_NBANDS 3

#define LIM_PEAK_IN  0
#define LIM_PEAK_OUT 1

#define SPEC_PRE_EQ    0
#define SPEC_POST_EQ   1
#define SPEC_POST_COMP 2
#define SPEC_OUTPUT    3

#define ACTIVE	0
#define MUTE    1
#define BYPASS  2

extern const jack_nframes_t dsp_block_size;
extern float sample_rate;
extern float xover_fa, xover_fb;
extern float eq_coefs[];
extern float in_peak[], out_peak[];
extern float lim_peak[];

extern volatile int global_bypass;

float bin_peak_read_and_clear(int bin);

void process_set_spec_mode(int mode);

int process_get_spec_mode();

void process_set_stereo_width(int xo_band, float width);

void process_set_stereo_balance(int xo_band, float bias);

void process_set_limiter_input_gain(float gain);

void process_set_ws_boost(float val);

void process_init(float fs);

int process_signal(jack_nframes_t nframes, int nchannels,
		   jack_default_audio_sample_t *in[],
		   jack_default_audio_sample_t *out[]);

float eval_comp(float thresh, float ratio, float knee, float in);

void process_set_xo_band_action(int band, int action);

void process_set_eq_bypass(int bypass);
void process_set_limiter_bypass(int bypass);

extern comp_settings compressors[XO_NBANDS];
extern lim_settings limiter;

extern plugin *comp_plugin;

#endif
