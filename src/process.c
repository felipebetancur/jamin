#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <jack/jack.h>
#include <fftw3.h>
#include <assert.h>

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

volatile int global_bypass = 0;		/* updated from GUI thread */

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
static float sw_m_gain[XO_NBANDS];
static float sw_s_gain[XO_NBANDS];
static float limiter_gain = 1.0f;

static int spectrum_mode = SPEC_POST_EQ;

/* Data for plugins */
plugin *comp_plugin, *lim_plugin;

/* FFTW data */
fftwf_plan plan_rc = NULL, plan_cr = NULL;

float sample_rate = 0.0f;

/* Desired block size for calling process_signal(). */
const jack_nframes_t step_size = BINS / OVER_SAMP;

void run_eq(unsigned int port, unsigned int in_pos);
void run_width(int xo_band, float *left, float *right, int nframes);

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

    /* This compressor is specifically stereo, so there are always two
     * channels. */
    for (band = 0; band < XO_NBANDS; band++) {
	out_tmp[CHANNEL_L][band] = calloc(step_size, sizeof(float));
	out_tmp[CHANNEL_R][band] = calloc(step_size, sizeof(float));
	compressors[band].handle = plugin_instantiate(comp_plugin, fs);
	comp_connect(comp_plugin, &compressors[band],
		     out_tmp[CHANNEL_L][band], out_tmp[CHANNEL_R][band]);
    }

    limiter.handle = plugin_instantiate(lim_plugin, fs);
    lim_connect(lim_plugin, &limiter, NULL, NULL);
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

    /* The limiters i/o ports potentially change with every call */
    plugin_connect_port(lim_plugin, limiter.handle, LIM_IN_1, out[CHANNEL_L]);
    plugin_connect_port(lim_plugin, limiter.handle, LIM_IN_2, out[CHANNEL_R]);
    plugin_connect_port(lim_plugin, limiter.handle, LIM_OUT_1, out[CHANNEL_L]);
    plugin_connect_port(lim_plugin, limiter.handle, LIM_OUT_2, out[CHANNEL_R]);

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
		run_eq(CHANNEL_L, in_ptr);
		run_eq(CHANNEL_R, in_ptr);
	    }
	    /* Work out when we can run it again */
	    n_calc_pt = (in_ptr + step_size) & BUF_MASK;
	}
    }

    //printf("rolled fifo's...\n");

    plugin_run(comp_plugin, compressors[XO_LOW].handle, nframes);
    run_width(XO_LOW, out_tmp[CHANNEL_L][XO_LOW], out_tmp[CHANNEL_R][XO_LOW],
	    nframes);
    plugin_run(comp_plugin, compressors[XO_MID].handle, nframes);
    run_width(XO_MID, out_tmp[CHANNEL_L][XO_MID], out_tmp[CHANNEL_R][XO_MID],
	    nframes);
    plugin_run(comp_plugin, compressors[XO_HIGH].handle, nframes);
    run_width(XO_HIGH, out_tmp[CHANNEL_L][XO_HIGH], out_tmp[CHANNEL_R][XO_HIGH],
	    nframes);

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
	    /* Apply input gain */
	    out[port][pos] *= limiter_gain;

	    /* Check for peaks */
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

void process_set_stereo_width(int xo_band, float width)
{
    assert(xo_band >= 0 && xo_band < XO_NBANDS);

    /* Scale width to be pi/4 - pi/2, the sqrt(2) factor saves us some cycles
     * later */
    sw_m_gain[xo_band] = cosf((width + 1.0f) * 0.78539815f) * 0.7071067811f;
    sw_s_gain[xo_band] = sinf((width + 1.0f) * 0.78539815f) * 0.7071067811f;
}

void run_width(int xo_band, float *left, float *right, int nframes)
{
    unsigned int pos;

    for (pos = 0; pos < nframes; pos++) {
	const float mid = (left[pos] + right[pos]) * sw_m_gain[xo_band];
	const float side = (left[pos] - right[pos]) * sw_s_gain[xo_band];

	left[pos] = mid + side;
	right[pos] = mid - side;
    }
}

void process_set_limiter_input_gain(float gain)
{
        limiter_gain = gain;
}

/* vi:set ts=8 sts=4 sw=4: */
