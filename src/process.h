#ifndef PROCESS_H
#define PROCESS_H

#include <jack/jack.h>

#define BINS  2048
#define BANDS 30

#include "plugin.h"
#include "compressor.h"
#include "limiter.h"

#define XO_LOW  0
#define XO_MID  1
#define XO_HIGH 2

#define LIM_PEAK_IN  0
#define LIM_PEAK_OUT 1

#define SPEC_PRE_EQ  0
#define SPEC_POST_EQ 1

extern jack_port_t *input_ports[2];
extern jack_port_t *output_ports[2];

extern float xover_fa, xover_fb;
extern float eq_coefs[];
extern float in_peak[], out_peak[];
extern float lim_peak[];

extern int global_bypass;

float bin_peak_read_and_clear(int bin);

void process_set_spec_mode(int mode);

void process_init(float fs, int buffer_size);

int iec_scale(float db);

int process_signal(jack_nframes_t nframes, int nchannels,
		   jack_default_audio_sample_t *in[],
		   jack_default_audio_sample_t *out[]);

float eval_comp(float thresh, float ratio, float knee, float in);

extern comp_settings compressors[3];
extern lim_settings limiter;

extern plugin *comp_plugin;

#endif
