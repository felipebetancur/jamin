#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <jack/jack.h>
#include <fftw3.h>

#include "process.h"
#include "compressor.h"
#include "limiter.h"
#include "geq.h"

#define BUF_MASK   (BINS-1)
#define OVER_SAMP  8

/* These values need to be controlled by the UI, somehow */
float xover_fa = 207.0f;
float xover_fb = 2048.0f;
comp_settings compressors[3];
lim_settings limiter;
float eq_coefs[BINS]; /* Linear gain of each FFT bin */
float in_trim_gain = 1.0f;
float lim_peak[2];

int global_bypass = 0;

float in_peak;

static float band_f[BANDS];
static float gain_fix[BANDS];
static float band_amp[BANDS];
static int peaks[BANDS];
static int ptime[BANDS];
static int bands[BINS];
static float in_buf[2][BINS];
static float out_buf[2][3][BINS];
static float window[BINS];
static float *real;
static float *comp;
static float *comp_tmp;
static float *out_tmp[2][3];

/* Data for plugins */
plugin *comp_plugin, *lim_plugin;
int lim_connected = 0;

/* FFTW data */
fftwf_plan plan_rc = NULL, plan_cr = NULL;

float sample_rate = 0.0f;

void run_eq(unsigned int port, unsigned int in_pos);

void process_init(float fs, int buffer_size)
{
    float centre = 25.0f;
    unsigned int i, j, band;

    sample_rate = fs;

    for (i = 0; i < BANDS; i++) {
	band_f[i] = centre;
	//printf("band %d = %fHz\n", i, centre);
	centre *= 1.25992105f;	// Go up a thrid of an octave
	gain_fix[i] = 0.0f;
    }

    band = 0;
    for (i = 0; i < BINS / 2; i++) {
	const float binfreq =
	    sample_rate * 0.5f * (i + 0.5f) / (float) BINS;

	while (binfreq > (band_f[band] + band_f[band + 1]) * 0.5f) {
	    band++;
	    if (band >= BANDS - 1) {
		band = BANDS - 1;
		break;
	    }
	}
	bands[i] = band;
	gain_fix[band]++;
	//printf("bin %d (%f) -> band %d (%f) #%d\n", i, binfreq, band, band_f[band], (int)gain_fix[band]);
    }

    for (i = 0; i < BANDS; i++) {
	if (gain_fix[i] != 0.0f) {
	    gain_fix[i] = 1.0f / gain_fix[i];
	} else {
	    /* There are no bins for this band, reassign a nearby one */
	    for (j = 0; j < BINS / 2; j++) {
		if (bands[j] > i) {
		    gain_fix[bands[j]]--;
		    bands[j] = i;
		    gain_fix[i] = 1.0f;
		    break;
		}
	    }
	}
    }

    /* Allocate space for FFT data */
    real = fftwf_malloc(sizeof(float) * BINS);
    comp = fftwf_malloc(sizeof(float) * BINS);
    comp_tmp = fftwf_malloc(sizeof(float) * BINS);

    plan_rc = fftwf_plan_r2r_1d(BINS, real, comp, FFTW_R2HC, FFTW_MEASURE);
    plan_cr = fftwf_plan_r2r_1d(BINS, comp_tmp, real, FFTW_HC2R, FFTW_MEASURE);

    /* Calculate raised cosine window */
    for (i = 0; i < BINS; i++) {
	window[i] = -0.5f * cosf(2.0f * M_PI * (float) i /
				 (float) BINS) + 0.5f;
    }

    plugin_init();
    comp_plugin = plugin_load("sc4_1882.so");
    lim_plugin = plugin_load("lookahead_limiter_1435.so");
    if (comp_plugin == NULL || lim_plugin == NULL)  {
           fprintf(stderr, "Required plugin missing.\n");
           exit(1);
    }

    out_tmp[0][XO_LOW] = calloc(buffer_size, sizeof(float));
    out_tmp[1][XO_LOW] = calloc(buffer_size, sizeof(float));
    out_tmp[0][XO_MID] = calloc(buffer_size, sizeof(float));
    out_tmp[1][XO_MID] = calloc(buffer_size, sizeof(float));
    out_tmp[0][XO_HIGH] = calloc(buffer_size, sizeof(float));
    out_tmp[1][XO_HIGH] = calloc(buffer_size, sizeof(float));

    compressors[0].handle = plugin_instantiate(comp_plugin, fs);
    comp_connect(comp_plugin, &compressors[0], out_tmp[0][XO_LOW],
		 out_tmp[1][XO_LOW]);

    compressors[1].handle = plugin_instantiate(comp_plugin, fs);
    comp_connect(comp_plugin, &compressors[1], out_tmp[0][XO_MID],
		 out_tmp[1][XO_MID]);

    compressors[2].handle = plugin_instantiate(comp_plugin, fs);
    comp_connect(comp_plugin, &compressors[2], out_tmp[0][XO_HIGH],
		 out_tmp[1][XO_HIGH]);

    limiter.handle = plugin_instantiate(lim_plugin, fs);
}

void run_eq(unsigned int port, unsigned int in_ptr)
{
    const float fix = 2.0f / ((float) BINS * (float) OVER_SAMP);
    float amp[BANDS];
    unsigned int i, j;
    int targ_bin;

    for (i = 0; i < BINS; i++) {
	real[i] = window[i] * in_buf[port][(in_ptr + i) & BUF_MASK];
    }

    fftwf_execute(plan_rc);

    /* run the EQ + spectrum an. + xover process */

    for (i = 0; i < BANDS; i++) {
	amp[i] = 0.0f;
    }

    memset(comp_tmp, 0, BINS * sizeof(float));
    targ_bin = xover_fa / sample_rate * (float) (BINS * 2);
    comp_tmp[0] = comp[0];
    for (i = 1; i < targ_bin && i < BINS / 2 - 1; i++) {
	comp_tmp[i] = comp[i] * eq_coefs[i];
	comp_tmp[BINS - i] = comp[BINS - i] * eq_coefs[i];
	amp[bands[i]] += sqrtf(comp[i] * comp[i] + comp[BINS - i] *
			      comp[BINS - i]) * gain_fix[bands[i]];
    }
    fftwf_execute(plan_cr);
    for (j = 0; j < BINS; j++) {
	out_buf[port][XO_LOW][(in_ptr + j) & BUF_MASK] += real[j] * fix *
	    window[j];
    }

    memset(comp_tmp, 0, BINS * sizeof(float));
    targ_bin = xover_fb / sample_rate * (float) (BINS * 2);
    for (; i < targ_bin && i < BINS / 2 - 1; i++) {
	comp_tmp[i] = comp[i] * eq_coefs[i];
	comp_tmp[BINS - i] = comp[BINS - i] * eq_coefs[i];
	amp[bands[i]] += sqrtf(comp[i] * comp[i] + comp[BINS - i] *
			      comp[BINS - i]) * gain_fix[bands[i]];
    }
    fftwf_execute(plan_cr);
    for (j = 0; j < BINS; j++) {
	out_buf[port][XO_MID][(in_ptr + j) & BUF_MASK] += real[j] * fix *
	    window[j];
    }

    memset(comp_tmp, 0, BINS * sizeof(float));
    for (; i < BINS / 2 - 1; i++) {
	comp_tmp[i] = comp[i] * eq_coefs[i];
	comp_tmp[BINS - i] = comp[BINS - i] * eq_coefs[i];
	amp[bands[i]] += sqrtf(comp[i] * comp[i] + comp[BINS - i] *
			      comp[BINS - i]) * gain_fix[bands[i]];
    }
    fftwf_execute(plan_cr);
    for (j = 0; j < BINS; j++) {
	out_buf[port][XO_HIGH][(in_ptr + j) & BUF_MASK] += real[j] * fix *
	    window[j];
    }

    for (i = 0; i < BANDS; i++) {
	band_amp[i] =
	    (amp[i] * fix * BANDS - band_amp[i]) * 0.3f + band_amp[i];
    }
}

int process(jack_nframes_t nframes, void *arg)
{
    unsigned int pos, port;
    const unsigned int step_size = BINS / OVER_SAMP;
    const unsigned int latency = BINS - step_size;
    static unsigned int in_ptr = 0;
    static unsigned int n_calc_pt = BINS - (BINS / OVER_SAMP);
    jack_default_audio_sample_t *in[2], *out[2];

    /* just incase the ports aren't registered */
    if (input_ports[0] == 0 || input_ports[1] == 0) {
	return 0;
    }
    //printf("we seem to have some ports...\n");

    in[0] = (jack_default_audio_sample_t *)
	jack_port_get_buffer(input_ports[0], nframes);
    out[0] = (jack_default_audio_sample_t *)
	jack_port_get_buffer(output_ports[0], nframes);
    in[1] = (jack_default_audio_sample_t *)
	jack_port_get_buffer(input_ports[1], nframes);
    out[1] = (jack_default_audio_sample_t *)
	jack_port_get_buffer(output_ports[1], nframes);

    //printf("got buffers...\n");

    if (!lim_connected) {
	lim_connect(lim_plugin, &limiter, out[0], out[1]);
	lim_connected = 1;
	//printf("connected the limiter...\n");
    }

    for (pos = 0; pos < nframes; pos++) {
	const unsigned int op = (in_ptr - latency) & BUF_MASK;
	float amp;

	for (port = 0; port < 2; port++) {
	    in_buf[port][in_ptr] = in[port][pos] * in_trim_gain;
	    amp = fabs(in_buf[port][in_ptr]);
	    if (amp > in_peak) {
		in_peak = amp;
	    } else {
		in_peak = in_peak * 0.99f + amp * 0.01f;
	    }

	    out_tmp[port][XO_LOW][pos] = out_buf[port][XO_LOW][op];
	    out_buf[port][XO_LOW][op] = 0.0f;
	    out_tmp[port][XO_MID][pos] = out_buf[port][XO_MID][op];
	    out_buf[port][XO_MID][op] = 0.0f;
	    out_tmp[port][XO_HIGH][pos] = out_buf[port][XO_HIGH][op];
	    out_buf[port][XO_HIGH][op] = 0.0f;
	}

	in_ptr = (in_ptr + 1) & BUF_MASK;

	if (in_ptr == n_calc_pt && !global_bypass) {
	    run_eq(0, in_ptr);
	    run_eq(1, in_ptr);

	    n_calc_pt = (in_ptr + step_size) & BUF_MASK;
	}
    }

    //printf("rolled fifo's...\n");

    plugin_run(comp_plugin, compressors[XO_LOW].handle, nframes);
    plugin_run(comp_plugin, compressors[XO_MID].handle, nframes);
    plugin_run(comp_plugin, compressors[XO_HIGH].handle, nframes);

    //printf("run compressors...\n");

    for (port = 0; port < 2; port++) {
	for (pos = 0; pos < nframes; pos++) {
	    out[port][pos] =
		out_tmp[port][XO_LOW][pos] + out_tmp[port][XO_MID][pos] +
		out_tmp[port][XO_HIGH][pos];
	}
    }

    //printf("done something...\n");

    for (pos = 0; pos < nframes; pos++) {
	for (port = 0; port < 2; port++) {
	    if (out[port][pos] > lim_peak[LIM_PEAK_IN]) {
		lim_peak[LIM_PEAK_IN] = out[port][pos];
	    } else {
		lim_peak[LIM_PEAK_IN] = lim_peak[LIM_PEAK_IN] * 0.9999f +
		    out[port][pos] * 0.0001f;
	    }
	}
    }

    plugin_run(lim_plugin, limiter.handle, nframes);

    //printf("run limiter...\n");

    /* If bypass is on override all the stuff done by the crossover section,
     * limiter and so on */
    if (global_bypass) {
	for (port = 0; port < 2; port++) {
	    for (pos = 0; pos < nframes; pos++) {
		out[port][pos] = in_buf[port][(in_ptr + pos - latency) & BUF_MASK];
	    }
	}
    }

    for (pos = 0; pos < nframes; pos++) {
	for (port = 0; port < 2; port++) {
	    if (out[port][pos] > lim_peak[LIM_PEAK_OUT]) {
		lim_peak[LIM_PEAK_OUT] = out[port][pos];
	    } else {
		lim_peak[LIM_PEAK_OUT] = lim_peak[LIM_PEAK_OUT] * 0.9999f +
		    out[port][pos] * 0.0001f;
	    }
	}
    }

    return 0;
}

int iec_scale(float db)
{
    float def = 0.0f;		/* Meter deflection %age */

    if (db < -70.0f) {
	def = 0.0f;
    } else if (db < -60.0f) {
	def = (db + 70.0f) * 0.25f;
    } else if (db < -50.0f) {
	def = (db + 60.0f) * 0.5f + 5.0f;
    } else if (db < -40.0f) {
	def = (db + 50.0f) * 0.75f + 7.5;
    } else if (db < -30.0f) {
	def = (db + 40.0f) * 1.5f + 15.0f;
    } else if (db < -20.0f) {
	def = (db + 30.0f) * 2.0f + 30.0f;
    } else if (db < 0.0f) {
	def = (db + 20.0f) * 2.5f + 50.0f;
    } else {
	def = 100.0f;
    }

    return (int) def;
}

float eval_comp(float thresh, float ratio, float knee, float in)
{
    /* Below knee */
    if (in <= thresh - knee) {
	return in;
    }

    /* In knee */
    if (in < thresh + knee) {
	const float x = -(thresh - knee - in) / knee;
	return in - knee * x * x * 0.25f * (ratio - 1.0f) / ratio;
    }

    /* Above knee */
    return in + (thresh - in) * (ratio - 1.0f) / ratio;
}

/* vi:set ts=8 sts=4 sw=4: */
