#include <stdio.h>
#include <gtk/gtk.h>
#include <math.h>

#include "process.h"
#include "io.h"
#include "support.h"
#include "main.h"
#include "gtkmeter.h"

gboolean lh_changed(GtkAdjustment *adj, gpointer user_data);
GtkAdjustment *lh_adj;

static GtkProgressBar *in_meter, *att_meter;
static GtkMeter *out_meter;
static GtkAdjustment *out_meter_adj;

void bind_limiter()
{
    GtkWidget *scale;

    scale = lookup_widget(main_window, "lim_lh_scale");
    lh_adj = gtk_range_get_adjustment(GTK_RANGE(scale));
    g_signal_connect(G_OBJECT(lh_adj), "value-changed", G_CALLBACK(lh_changed),
	    NULL);
    in_meter = GTK_PROGRESS_BAR(lookup_widget(main_window, "lim_in_meter"));
    att_meter = GTK_PROGRESS_BAR(lookup_widget(main_window, "lim_att_meter"));
    out_meter = GTK_METER(lookup_widget(main_window, "lim_out_meter"));
    out_meter_adj = gtk_meter_get_adjustment(out_meter);
}

gboolean lh_changed(GtkAdjustment *adj, gpointer user_data)
{
    limiter.delay = adj->value * 0.0001f;

    return FALSE;
}

void limiter_meters_update()
{
    float peak_in = iec_scale(20.0f * log10f(lim_peak[LIM_PEAK_IN])) * 0.01f;
    float peak_out = 20.0f * log10f(lim_peak[LIM_PEAK_OUT]);
    float atten = limiter.attenuation / 12.0f;

    if (peak_in < 0.0f) {
	peak_in = 0.0f;
    } else if (peak_in > 1.0f) {
	peak_in = 1.0f;
    }

    /* this one causes GTK warnings when you move the output gain slider */
    if (atten < 0.0f) {
	atten = 0.0f;
    } else if (atten > 1.0f) {
	atten = 1.0f;
    }
    
    gtk_progress_bar_set_fraction(in_meter, peak_in);
    gtk_progress_bar_set_fraction(att_meter, atten);
    gtk_adjustment_set_value(out_meter_adj, peak_out);
}

/* vi:set ts=8 sts=4 sw=4: */
