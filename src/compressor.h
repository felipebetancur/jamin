#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <process.h>

#define COMP_RMS_PEAK                   0
#define COMP_ATTACK                     1
#define COMP_RELEASE                    2
#define COMP_THRESHOLD                  3
#define COMP_RATIO                      4
#define COMP_KNEE                       5
#define COMP_MAKEUP_GAIN                6
#define COMP_AMPLITUDE                  7
#define COMP_GAIN_RED                   8
#define COMP_LEFT_IN                    9
#define COMP_RIGHT_IN                   10
#define COMP_LEFT_OUT                   11
#define COMP_RIGHT_OUT                  12

typedef struct {
	float rms_peak;
	float attack;
	float release;
	float threshold;
	float ratio;
	float knee;
	float makeup_gain;
	float amplitude;
	float gain_red;
	LADSPA_Handle handle;
} comp_settings;

static inline void comp_connect(plugin *p, comp_settings *s, float *left, float
		*right) {
	plugin_connect_port(p, s->handle, COMP_RMS_PEAK, &(s->rms_peak));
	plugin_connect_port(p, s->handle, COMP_ATTACK, &(s->attack));
	plugin_connect_port(p, s->handle, COMP_RELEASE, &(s->release));
	plugin_connect_port(p, s->handle, COMP_THRESHOLD, &(s->threshold));
	plugin_connect_port(p, s->handle, COMP_RATIO, &(s->ratio));
	plugin_connect_port(p, s->handle, COMP_KNEE, &(s->knee));
	plugin_connect_port(p, s->handle, COMP_MAKEUP_GAIN, &(s->makeup_gain));
	plugin_connect_port(p, s->handle, COMP_AMPLITUDE, &(s->amplitude));
	plugin_connect_port(p, s->handle, COMP_GAIN_RED, &(s->gain_red));
	plugin_connect_port(p, s->handle, COMP_LEFT_IN, left);
	plugin_connect_port(p, s->handle, COMP_RIGHT_IN, right);
	plugin_connect_port(p, s->handle, COMP_LEFT_OUT, left);
	plugin_connect_port(p, s->handle, COMP_RIGHT_OUT, right);

	/* Make sure that it is set to something */
	s->rms_peak = 1.0f;
	s->attack = 100.0f;
	s->release = 200.0f;
	s->threshold = -20.0f;
	s->ratio = 2.0f;
	s->knee = 0.5f;
	s->makeup_gain = 0.0f;
}

comp_settings comp_get_settings(int i);

#endif
