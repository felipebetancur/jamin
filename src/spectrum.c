/*
 *  Copyright (C) 2003 Jan C. Depner, Steve Harris
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
 *  $Id: spectrum.c,v 1.11 2003/11/19 15:43:38 theno23 Exp $
 */

#include <math.h>
#include <stdio.h>
#include <gtk/gtk.h>

#include "support.h"
#include "main.h"
#include "process.h"
#include "gtkmeter.h"
#include "gtkmeterscale.h"
#include "db.h"

static char *band_lbls[BANDS] = {
    "25.0", "31.5", "40.0", "50.0", "63.0", "80.0", "100", "125", "160", "200",
    "250",  "315",  "400",  "500",  "630",  "800",  "1k",  "1k25", "1k6", "2k",
    "2k5",  "3k1",  "4k",   "5k",   "6k3",  "8k",   "10k", "10k2", "16k", "20k"
};

GtkWidget *make_mini_label(const char *text);
static GtkAdjustment *adjustment[BANDS];

static int bin_bands[BINS];
static int band_bin[BANDS];


void bind_spectrum()
{
    GtkWidget *root;
    GtkWidget *hbox;
    GtkWidget *vbox;
    GtkWidget *label;
    GtkWidget *meter;
    GtkWidget *mscale;
    int i, bin, band;
    float band_freq[BANDS];
    float band_bin_count[BANDS];

    root = lookup_widget(main_window, "spectrum_hbox");
    hbox = gtk_hbox_new(TRUE, 0);
    gtk_box_pack_start(GTK_BOX(root), hbox, FALSE, FALSE, 0);
    gtk_widget_show(hbox);

    vbox = gtk_vbox_new(FALSE, 1);
    gtk_widget_show(vbox);
    gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, TRUE, 0);
    mscale = gtk_meterscale_new(GTK_METERSCALE_RIGHT, LOWER_SPECTRUM_DB, 
                                UPPER_SPECTRUM_DB);
    gtk_widget_show(mscale);
    gtk_box_pack_start(GTK_BOX(vbox), mscale, TRUE, TRUE, 0);
    label = make_mini_label(" ");
    gtk_widget_show(label);
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, TRUE, 0);

    for (i = 0; i < BANDS; i++) {
	vbox = gtk_vbox_new(FALSE, 1);
	gtk_widget_show(vbox);
	gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, TRUE, 0);

	adjustment[i] = GTK_ADJUSTMENT(gtk_adjustment_new(LOWER_SPECTRUM_DB, 
                                                          LOWER_SPECTRUM_DB, 
                                                          UPPER_SPECTRUM_DB,
                                                          0.0, 0.0, 0.0));
	meter = gtk_meter_new(adjustment[i], GTK_METER_UP);
	//gtk_widget_set_usize(GTK_WIDGET(meter), 14, -1);
	gtk_meter_set_warn_point(GTK_METER(meter), 0.0);
	gtk_widget_show(meter);
	gtk_box_pack_start(GTK_BOX(vbox), meter, TRUE, TRUE, 0);

	label = make_mini_label(band_lbls[i]);
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, TRUE, 0);
    }

    vbox = gtk_vbox_new(FALSE, 1);
    gtk_widget_show(vbox);
    gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, TRUE, 0);
    mscale = gtk_meterscale_new(GTK_METERSCALE_LEFT, LOWER_SPECTRUM_DB, 
                                UPPER_SPECTRUM_DB);
    gtk_widget_show(mscale);
    gtk_box_pack_start(GTK_BOX(vbox), mscale, TRUE, TRUE, 0);
    label = make_mini_label(" ");
    gtk_widget_show(label);
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, TRUE, 0);

    /* Calcuate the centre frequency for each band */
    for (band=0; band<BANDS; band++) {
	band_freq[band] = 1000.0 * pow(10.0, (double)(band-16) * 0.1);
	//printf("band %d is at %f Hz\n", band, band_freq[band]);
	band_bin_count[band] = 0;
    }

    for (bin=0; bin<BINS/2; bin++) {
	const float bin_freq = (bin + 0.5f) * sample_rate / BINS;
	int nearest_band = 0;
	float nearest_dist = 9999999.0f;
	for (band=0; band<BANDS; band++) {
	    if (fabs(bin_freq - band_freq[band]) < nearest_dist) {
		nearest_band = band;
		nearest_dist = fabs(bin_freq - band_freq[band]);
	    }
	}
	bin_bands[bin] = nearest_band;
	//printf("bin %d (%f Hz) is nearest band %d (%f Hz)\n", bin, bin_freq, nearest_band, band_freq[nearest_band]);
	band_bin_count[nearest_band]++;
    }

    for (band=0; band<BANDS; band++) {
	if (band_bin_count[band] == 0) {
	    band_bin[band] = band_freq[band] * BINS / sample_rate;
	    //printf("band %d is unassigned, use bin %d\n", band, band_bin[band]);
	} else {
	    /* Mark for no reverse lookup */
	    band_bin[band] = -1;
	}
    }
}

void spectrum_update()
{
    int i, page, count;
    float levels[BANDS];
    float single_levels[BINS/2];

    void draw_EQ_spectrum_curve (float *);
    int get_current_notebook1_page ();


    page = get_current_notebook1_page ();
    count = BINS / 2;

    if (page == 2) {
      for (i=0; i<BANDS; i++) {
        levels[i] = 0.0f;
      }
      for (i=0; i<count; i++) {
        single_levels[i] = bin_peak_read_and_clear(i);
        levels[bin_bands[i]] += single_levels[i];
      }
      for (i=0; i<BANDS; i++) {
        if (band_bin[i] > 0) {
          levels[i] = (single_levels[band_bin[i]] +
                       single_levels[band_bin[i]+1]) * 0.5;
        }
        gtk_adjustment_set_value(adjustment[i], lin2db(levels[i]));
      }
    }
    else if (page == 0) {
      for (i=0; i<count; i++) {
        single_levels[i] = bin_peak_read_and_clear(i);
      }
      draw_EQ_spectrum_curve (single_levels);
    }
}

GtkWidget *make_mini_label(const char *text)
{
    GtkLabel *label;
    char markup[256];

    label = GTK_LABEL(gtk_label_new(NULL));
    snprintf(markup, 255, "<span size=\"%d\">%s</span>", 6 * PANGO_SCALE, text);
    gtk_label_set_markup(label, markup);

    return GTK_WIDGET(label);
}

/* vi:set ts=8 sts=4 sw=4: */
