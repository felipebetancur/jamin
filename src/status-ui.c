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
#include <gtk/gtk.h>
#include <jack/jack.h>

#include "debug.h"
#include "main.h"
#include "support.h"
#include "jackstatus.h"
#include "transport.h"
#include "status-ui.h"

static GtkLabel *label_state;
static GtkLabel *label_CPU_load;
static GtkLabel *label_period;
static GtkLabel *label_rate;
static GtkLabel *label_realtime;

void status_init()
{
    /* look up widget addresses */
    label_state = GTK_LABEL(lookup_widget(main_window, "JACK_transport_state"));
    label_CPU_load = GTK_LABEL(lookup_widget(main_window, "JACK_CPU_load"));
    label_period = GTK_LABEL(lookup_widget(main_window, "JACK_period"));
    label_rate = GTK_LABEL(lookup_widget(main_window, "JACK_sample_rate"));
    label_realtime = GTK_LABEL(lookup_widget(main_window, "JACK_realtime"));
}

void status_update()
{
    char *state_msg;
    char msg[20];
    jack_status_t j;

    io_get_status(&j);

    if (!j.active)
	state_msg = "Disconnected";
    else if (j.tinfo.valid & JackTransportState) {
	switch (j.tinfo.transport_state) {
	case JackTransportStopped:
	    state_msg = "Stopped";
	    break;
	case JackTransportRolling:
	    state_msg = "Rolling";
	    break;
	case JackTransportLooping:
	    state_msg = "Looping";
	    break;
	default:
	    state_msg = "[unknown]";
	}
    } else
	state_msg = "";

    gtk_label_set_text(label_state, state_msg);

    snprintf(msg, sizeof(msg), "%.1f%% CPU", j.cpu_load);
    gtk_label_set_text(label_CPU_load, msg);

    snprintf(msg, sizeof(msg), "%ld frames", j.buf_size);
    gtk_label_set_text(label_period, msg);

    snprintf(msg, sizeof(msg), "%ld Hz", j.sample_rate);
    gtk_label_set_text(label_rate, msg);

    if (j.realtime)
	gtk_label_set_text(label_realtime, "Realtime");
    else
	gtk_label_set_text(label_realtime, "Not realtime");
}
