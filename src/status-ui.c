/*
 *  status-ui.c -- JACK status user interface.
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
#include <string.h>
#include <inttypes.h>
#include <gtk/gtk.h>
#include <jack/jack.h>
#include <config.h>
#include "jackstatus.h"
#include "transport.h"
#include "status-ui.h"
#include "support.h"


static GtkLabel *l_status_label = NULL;
static GtkLabel *l_time_label = NULL;


void status_update(GtkWidget *main_window)
{
    char *state_msg, *rt;
    gchar string[256];
    jack_status_t j;

    io_get_status(&j);

    if (!j.active)
	state_msg = "Disconnected";
    else
	switch (transport_get_state()) {
	case JackTransportStopped:
	    state_msg = "Stopped";
	    break;

#ifdef NEW_JACK_TRANSPORT

	case JackTransportStarting:
	    state_msg = "Starting";
	    break;

#else /* old JACK transport interface */

	case JackTransportLooping:
	    state_msg = "Looping";
	    break;

#endif /* NEW_JACK_TRANSPORT */

	case JackTransportRolling:
	    state_msg = "Rolling";
	    break;

	default:
	    state_msg = "[unknown]";
	}


    if (j.realtime)
	rt = "  |  Realtime";
    else
	rt = "";

    snprintf(string, sizeof(string), "%s  |  %4.1f%% CPU  |  %" PRIu32
	     " frames  |  %" PRIu32 " Hz%s",
             state_msg, j.cpu_load, j.buf_size, j.sample_rate, rt);

    if (l_status_label == NULL)
	l_status_label = GTK_LABEL(lookup_widget(main_window, "status_label"));

    gtk_label_set_text(l_status_label, string);
}


void status_set_time(GtkWidget *main_window)
{
    gchar string[32];
    unsigned int hours, minutes, seconds, hundreths;
    double time = transport_get_time();

    hours = time / 3600.0;
    time -=  hours * 3600.0;
    minutes = time / 60.0;
    time -= minutes * 60.0;
    seconds = time;
    time -= seconds;
    hundreths = time * 100.0;

    snprintf(string, sizeof(string), "%02u:%02u:%02u:%02u",
             hours, minutes, seconds, hundreths);

    if (l_time_label == NULL)
	l_time_label = GTK_LABEL(lookup_widget(main_window, "time_label"));

    gtk_label_set_text(l_time_label, string);
}
