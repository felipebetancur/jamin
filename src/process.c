#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <jack/jack.h>
#include <fftw3.h>

#include "process.h"
#include "compressor.h"
#include "limiter.h"
#include "geq.h"
#include "intrim.h"
#include "io.h"

#define BUF_MASK   (BINS-1)		/* BINS is a power of two */

/* These values need to be controlled by the UI, somehow */
float xover_fa = 207.0f;
float xover_fb = 2048.0f;
comp_settings compressors[XO_NBANDS];
lim_settings limiter;
float eq_coefs[BINS]; /* Linear gain of each FFT bin */
float lim_peak[2];

int global_bypass = 0;
int limiter_connected = FALSE;

float in_peak[NCHANNELS], out_peak[NCHANNELS];

static float band_f[BANDS];
static float gain_fix[BANDS];
static float bin_peak[BINS];
// Unused: static int peaks[BANDS];
// Unused: static int ptime[BANDS];
static int bands[BINS];
static float in_buf[NCHANNELS][BINS];
static float out_buf[NCHANNELS][XO_NBANDS][BINS];
static float window[BINS];
static float *real;
static float *comp;
static float *comp_tmp;
static float *out_tmp[NCHANNELS][XO_NBANDS];

static int spectrum_mode = SPEC_PRE_EQ;

/* Data for plugins */
plugin *comp_plugin, *lim_plugin;

/* FFTW data */
fftwf_plan plan_rc = NULL, plan_cr = NULL;

float sample_rate = 0.0f;

/* Desired block size for calling process_signal(). */
const jack_nframes_t step_size = BINS / OVER_SAMP;

void run_eq(unsigned int port, unsigned int in_pos);

void process_init(float fs, int buffer_size)
{
    float centre = 25.0f;
    unsigned int i, j, band;

    sample_rate = fs;

    io_set_granularity(step_size);

    for (i = 0; i < BANDS; i++) {
	band_f[i] = centre;
	//printf("band %d = %fHz\n", i, centre);
	centre *= 1.25992105f;		/* up a third of an octave */
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

    compressors[XO_LOW].handle = plugin_instantiate(comp_plugin, fs);
    comp_connect(comp_plugin, &compressors[XO_LOW], out_tmp[0][XO_LOW],
		 out_tmp[1][XO_LOW]);

    compressors[XO_MID].handle = plugin_instantiate(comp_plugin, fs);
    comp_connect(comp_plugin, &compressors[XO_MID], out_tmp[0][XO_MID],
		 out_tmp[1][XO_MID]);

    compressors[XO_HIGH].handle = plugin_instantiate(comp_plugin, fs);
    comp_connect(comp_plugin, &compressors[XO_HIGH], out_tmp[0][XO_HIGH],
		 out_tmp[1][XO_HIGH]);

    limiter.handle = plugin_instantiate(lim_plugin, fs);
}

void run_eq(unsigned int port, unsigned int in_ptr)
{
    const float fix = 2.0f / ((float) BINS * (float) OVER_SAMP);
    float peak;
    unsigned int i, j;
    int targ_bin;
    float *peak_data;

    for (i = 0; i < BINS; i++) {
	real[i] = window[i] * in_buf[port][(in_ptr + i) & BUF_MASK];
    }

    fftwf_execute(plan_rc);

    /* run the EQ + spectrum an. + xover process */

    if (spectrum_mode == SPEC_PRE_EQ) {
	peak_data = comp;
    } else {
	peak_data = comp_tmp;
    }

    memset(comp_tmp, 0, BINS * sizeof(float));
    targ_bin = xover_fa / sample_rate * (float) (BINS * 2);
    comp_tmp[0] = comp[0];
    for (i = 0; i < targ_bin && i < BINS / 2 - 1; i++) {
	comp_tmp[i] = comp[i] * eq_coefs[i];
	comp_tmp[BINS - i] = comp[BINS - i] * eq_coefs[i];

	peak = sqrtf(peak_data[i] * peak_data[i] + peak_data[BINS - i] *
		peak_data[BINS - i]);
	if (peak > bin_peak[i]) {
	    bin_peak[i] = peak;
	}
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
	peak = sqrtf(peak_data[i] * peak_data[i] + peak_data[BINS - i] *
		peak_data[BINS - i]);
	if (peak > bin_peak[i]) {
	    bin_peak[i] = peak;
	}
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
	peak = sqrtf(peak_data[i] * peak_data[i] + peak_data[BINS - i] *
		peak_data[BINS - i]);
	if (peak > bin_peak[i]) {
	    bin_peak[i] = peak;
	}
    }
    fftwf_execute(plan_cr);
    for (j = 0; j < BINS; j++) {
	out_buf[port][XO_HIGH][(in_ptr + j) & BUF_MASK] += real[j] * fix *
	    window[j];
    }
}

float bin_peak_read_and_clear(int bin)
{
    float ret = bin_peak[bin];
    const float fix = 2.0f / ((float) BINS * (float) OVER_SAMP);

    bin_peak[bin] = 0.0f;

    return ret * fix;
}

int process_signal(jack_nframes_t nframes,
		   int nchannels,
		   jack_default_audio_sample_t *in[],
		   jack_default_audio_sample_t *out[])
{
    unsigned int pos, port;
    const unsigned int latency = BINS - step_size;
    static unsigned int in_ptr = 0;
    static unsigned int n_calc_pt = BINS - (BINS / OVER_SAMP);

    if (!limiter_connected) {
	limiter_connected = TRUE;
	lim_connect(lim_plugin, &limiter, out[0], out[1]);
    } else {				/* already connected */
	/* need to update buffer addresses anyway */
	plugin_connect_port(lim_plugin, limiter.handle, LIM_IN_1, out[0]);
	plugin_connect_port(lim_plugin, limiter.handle, LIM_IN_2, out[1]);
	plugin_connect_port(lim_plugin, limiter.handle, LIM_OUT_1, out[0]);
	plugin_connect_port(lim_plugin, limiter.handle, LIM_OUT_2, out[1]);
    }

    for (pos = 0; pos < nframes; pos++) {
	const unsigned int op = (in_ptr - latency) & BUF_MASK;
	float amp;

	for (port = 0; port < nchannels; port++) {
	    in_buf[port][in_ptr] = in[port][pos] * in_gain[port];
	    amp = fabs(in_buf[port][in_ptr]);
	    if (amp > in_peak[port]) {
		in_peak[port] = amp;
	    }

	    out_tmp[port][XO_LOW][pos] = out_buf[port][XO_LOW][op];
	    out_buf[port][XO_LOW][op] = 0.0f;
	    out_tmp[port][XO_MID][pos] = out_buf[port][XO_MID][op];
	    out_buf[port][XO_MID][op] = 0.0f;
	    out_tmp[port][XO_HIGH][pos] = out_buf[port][XO_HIGH][op];
	    out_buf[port][XO_HIGH][op] = 0.0f;
	}

	in_ptr = (in_ptr + 1) & BUF_MASK;

	if (in_ptr == n_calc_pt) {	/* time to do the FFT? */
	    if (!global_bypass) {
		run_eq(0, in_ptr);
		run_eq(1, in_ptr);
	    }
	    n_calc_pt = (in_ptr + step_size) & BUF_MASK;
	}
    }

    //printf("rolled fifo's...\n");

    plugin_run(comp_plugin, compressors[XO_LOW].handle, nframes);
    plugin_run(comp_plugin, compressors[XO_MID].handle, nframes);
    plugin_run(comp_plugin, compressors[XO_HIGH].handle, nframes);

    //printf("run compressors...\n");

    for (port = 0; port < nchannels; port++) {
	for (pos = 0; pos < nframes; pos++) {
	    out[port][pos] =
		out_tmp[port][XO_LOW][pos] + out_tmp[port][XO_MID][pos] +
		out_tmp[port][XO_HIGH][pos];
	}
    }

    //printf("done something...\n");

    for (pos = 0; pos < nframes; pos++) {
	for (port = 0; port < nchannels; port++) {
	    if (out[port][pos] > lim_peak[LIM_PEAK_IN]) {
		lim_peak[LIM_PEAK_IN] = out[port][pos];
	    }
	}
    }

    plugin_run(lim_plugin, limiter.handle, nframes);

    //printf("run limiter...\n");

    /* If bypass is on override all the stuff done by the crossover section,
     * limiter and so on */
    if (global_bypass) {
	for (port = 0; port < nchannels; port++) {
	    for (pos = 0; pos < nframes; pos++) {
		out[port][pos] = in_buf[port][(in_ptr + pos - latency) & BUF_MASK];
	    }
	}
    }

    for (pos = 0; pos < nframes; pos++) {
	for (port = 0; port < nchannels; port++) {
	    const float oa = fabs(out[port][pos]);

	    if (oa > lim_peak[LIM_PEAK_OUT]) {
		lim_peak[LIM_PEAK_OUT] = oa;
	    }
	    if (oa > out_peak[port]) {
		out_peak[port] = oa;
	    }
	}
    }

    return 0;
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

void process_set_spec_mode(int mode)
{
    spectrum_mode = mode;
}

/* vi:set ts=8 sts=4 sw=4: */
