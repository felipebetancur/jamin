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
 *  $Id: stereo.c,v 1.6 2004/01/03 14:14:11 jdepner Exp $
 */

#include <stdio.h>

#include "process.h"
#include "support.h"
#include "state.h"
#include "main.h"

void stereo_cb(int id, float value);
void stereo_balance_cb(int id, float value);


static GtkLabel *band_balance_label[3];
static GtkRange *band_balance_scale[3];

void update_band_balance_label(int band, float balance)
{
    char tmp[256];

    if (balance < -0.5f) {
	snprintf(tmp, 255, "left %.0fdB", -balance);
    } else if (balance > 0.5f) {
	snprintf(tmp, 255, "right %.0fdB", balance);
    } else {
	sprintf(tmp, "centre");
    }
    gtk_label_set_label(band_balance_label[band], tmp);
}


void bind_stereo()
{
    int i;


    band_balance_label[0] = GTK_LABEL(lookup_widget(main_window, 
                                                    "low_balance_label"));
    band_balance_label[1] = GTK_LABEL(lookup_widget(main_window, 
                                                    "mid_balance_label"));
    band_balance_label[2] = GTK_LABEL(lookup_widget(main_window, 
                                                    "high_balance_label"));

    band_balance_scale[0] = GTK_RANGE(lookup_widget(main_window, 
                                                    "low_balance_scale"));
    band_balance_scale[1] = GTK_RANGE(lookup_widget(main_window, 
                                                    "mid_balance_scale"));
    band_balance_scale[2] = GTK_RANGE(lookup_widget(main_window, 
                                                    "high_balance_scale"));


    for (i = 0; i < 3; i++) {
	s_set_callback(S_STEREO_WIDTH(i), stereo_cb);
	process_set_stereo_width(i, 0.0f);

        s_set_callback(S_STEREO_BALANCE(i), stereo_balance_cb);
        s_set_adjustment(S_STEREO_BALANCE(i), 
                         gtk_range_get_adjustment(band_balance_scale[i]));
	process_set_stereo_balance(i, 0.0f);
        update_band_balance_label(i, 0.0);
    }
}

void stereo_cb(int id, float value)
{
    int band = id - S_STEREO_WIDTH(0);

    process_set_stereo_width(band, value);
}


void stereo_balance_cb(int id, float value)
{
    int band = id - S_STEREO_BALANCE(0);

    process_set_stereo_balance(band, value);
    update_band_balance_label(band, value);
}
