/*
 *  transport.c -- JACK transport control functions.
 *
 *  Copyright (C) 2003 Jack O'Quin.
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
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <unistd.h>
#include <jack/jack.h>

#include "debug.h"
#include "jackstatus.h"
#include "io.h"
#include "transport.h"

volatile int jamin_is_timebase_master = 0;

typedef struct {
    volatile jack_nframes_t guard1;
    volatile jack_transport_info_t info;
    volatile jack_nframes_t guard2;
} guarded_transport_info_t;

guarded_transport_info_t tpt = {0};


/* transport_control -- JACK transport control handler.
 *
 *  Called from the realtime JACK process() thread.  Must not wait.
 */
void transport_control(jack_nframes_t nframes)
{
    if (jamin_is_timebase_master) {

	/* don't update transport info until state is valid */
	if ((tpt.info.valid & (JackTransportState|JackTransportPosition))
	    == (JackTransportState|JackTransportPosition)) {

	    jack_set_transport_info(client,
				    (jack_transport_info_t *) &tpt.info);

	    /* frame number for next cycle */
	    if (tpt.info.transport_state != JackTransportStopped) {
		tpt.info.frame += nframes;
	    }
	}

    } else {				/* not timebase master */

	tpt.guard1 = jack_frame_time(client);
	jack_get_transport_info(client, (jack_transport_info_t *) &tpt.info);
	tpt.guard2 = tpt.guard1;	/* tpt.info now consistent */
    }
}


/******************* user interface functions *******************/

/* These functions are all called from a GUI thread.  So, their
 * interaction with the DSP engine and its threads needs to be
 * thread-safe.
 */


/* transport_status -- carefully copy JACK transport status.
 *
 *  Can be called from any non-realtime thread.
 */
void transport_status(jack_transport_info_t *jp)
{
    guarded_transport_info_t tmp;	/* temporary guarded copy */
    int tries = 0;

    /* Since "tpt" is updated from the process() thread every
     * buffer period, we must copy it carefully to avoid getting
     * an incoherent hash of multiple versions. */
    do {
	/* Throttle the busy wait if we don't get the a clean
	 * copy very quickly. */
	if (tries > 10) {
	    usleep(20);			/* not if realtime! */
	    tries = 0;
	}
	tmp = tpt;
	tries++;

    } while (tmp.guard1 != tmp.guard2);

    *jp = (jack_transport_info_t) tmp.info;
}


/* transport_master -- take over as timebase master, if possible.
 *
 *  This logic isn't completely air-tight, but should serve until
 *  there is a cleaner JACK transport design.
 */
int transport_master()
{
    if (jamin_is_timebase_master)
	return 1;			/* I already am master */

    if (client) {
	if (jack_engine_takeover_timebase(client) == 0) {

	    /* stop reading transport info */
	    jamin_is_timebase_master = 1;

	    tpt.info.valid = 0;
	    tpt.info.transport_state = JackTransportStopped;
	    tpt.info.frame = 0;

	    /* begin updating transport info */
	    tpt.info.valid = (JackTransportState|JackTransportPosition);
	}
    }

    return jamin_is_timebase_master;
}


void transport_play()
{
    if (transport_master()) {
	tpt.info.transport_state = JackTransportRolling;
	IF_DEBUG(DBG_TERSE,
		 fprintf(stderr,"Transport started.\n"));
    } else {
	IF_DEBUG(DBG_TERSE,
		 fprintf(stderr,"Not transport master.\n"));
    }
}


void transport_rewind()
{
    if (transport_master()) {
	tpt.info.transport_state = JackTransportStopped;
	tpt.info.frame = 0;
	IF_DEBUG(DBG_TERSE,
		 fprintf(stderr,"Transport rewound.\n"));
    } else {
	IF_DEBUG(DBG_TERSE,
		 fprintf(stderr,"Not transport master.\n"));
    }
}


void transport_stop()
{
    if (transport_master()) {
	tpt.info.transport_state = JackTransportStopped;
	IF_DEBUG(DBG_TERSE,
		 fprintf(stderr,"Transport stopped.\n"));
    } else {
	IF_DEBUG(DBG_TERSE,
		 fprintf(stderr,"Not transport master.\n"));
    }
}
