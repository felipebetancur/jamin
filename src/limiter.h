#ifndef LIMITER_H
#define LIMITER_H

#include <process.h>

#define LIM_LIMIT         0
#define LIM_DELAY         1
#define LIM_ATTENUATION   2
#define LIM_IN_1          3
#define LIM_IN_2          4
#define LIM_OUT_1         5
#define LIM_OUT_2         6

typedef struct {
	float limit;
	float delay;
	float attenuation;
	LADSPA_Handle handle;
} lim_settings;

static inline void lim_connect(plugin *p, lim_settings *s, float *left, float
		*right) {
	plugin_connect_port(p, s->handle, LIM_LIMIT, &(s->limit));
	plugin_connect_port(p, s->handle, LIM_DELAY, &(s->delay));
	plugin_connect_port(p, s->handle, LIM_ATTENUATION, &(s->attenuation));
	plugin_connect_port(p, s->handle, LIM_IN_1, left);
	plugin_connect_port(p, s->handle, LIM_IN_2, right);
	plugin_connect_port(p, s->handle, LIM_OUT_1, left);
	plugin_connect_port(p, s->handle, LIM_OUT_2, right);

	/* Make sure that it is set to something */
	s->limit = 0.0f;
	s->attenuation = 0.0f;
	s->delay = 0.5f;
}

#endif
