/*
 *  Copyright (C) 2003 Steve Harris
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
 *  $Id: limiter.h,v 1.6 2003/11/19 15:28:17 theno23 Exp $
 */

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
	s->delay = 0.01f;
}

#endif
