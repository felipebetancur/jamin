#ifndef PROCESS_H
#define PROCESS_H

#include <jack/jack.h>

#define BINS  2048
#define BANDS 29

#include "plugin.h"
#include "compressor.h"
#include "limiter.h"

#define XO_LOW  0
#define XO_MID  1
#define XO_HIGH 2

#define LIM_PEAK_IN  0
#define LIM_PEAK_OUT 1

extern jack_port_t *input_ports[2];
extern jack_port_t *output_ports[2];

extern float xover_fa, xover_fb;
extern float eq_coefs[];
extern float in_peak[], out_peak[];
extern float lim_peak[];

extern int global_bypass;

void process_init(float fs, int buffer_size);

int iec_scale(float db);

int process(jack_nframes_t nframes, void *arg);

float eval_comp(float thresh, float ratio, float knee, float in);

extern comp_settings compressors[3];
extern lim_settings limiter;

extern plugin *comp_plugin;

#endif
