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

#define SPEC_PRE_EQ  0
#define SPEC_POST_EQ 1

extern float xover_fa, xover_fb;
extern float eq_coefs[];
extern float in_peak[], out_peak[];
extern float lim_peak[];

extern int global_bypass;

float bin_peak_read_and_clear(int bin);

void process_set_spec_mode(int mode);

void process_set_stereo_width(int xo_band, float width);

void process_set_limiter_input_gain(float gain);

void process_init(float fs, int buffer_size);

int process_signal(jack_nframes_t nframes, int nchannels,
		   jack_default_audio_sample_t *in[],
		   jack_default_audio_sample_t *out[]);

float eval_comp(float thresh, float ratio, float knee, float in);

extern comp_settings compressors[XO_NBANDS];
extern lim_settings limiter;

extern plugin *comp_plugin;

#endif
