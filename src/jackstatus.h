/*
 *  Copyright (C) 2003 Jack O'Quin
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
 *  $Id: jackstatus.h,v 1.7 2003/11/19 15:28:17 theno23 Exp $
 */

#ifndef JACKSTATUS_H
#define JACKSTATUS_H

#include <jack/transport.h>

typedef struct {
    int		realtime;
    int		active;
    int		xruns;
    float	cpu_load;
    jack_nframes_t sample_rate;
    jack_nframes_t buf_size;
    jack_nframes_t latency;
} jack_status_t;

void io_get_status(jack_status_t *jp);

#endif
