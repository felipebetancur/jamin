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

static GtkMeter *in_meter, *att_meter, *out_meter;
static GtkAdjustment *in_meter_adj, *att_meter_adj, *out_meter_adj;

void bind_limiter()
{
    GtkWidget *scale;

    scale = lookup_widget(main_window, "lim_lh_scale");
    lh_adj = gtk_range_get_adjustment(GTK_RANGE(scale));
    g_signal_connect(G_OBJECT(lh_adj), "value-changed", G_CALLBACK(lh_changed),
	    NULL);
    in_meter = GTK_METER(lookup_widget(main_window, "lim_in_meter"));
    att_meter = GTK_METER(lookup_widget(main_window, "lim_att_meter"));
    out_meter = GTK_METER(lookup_widget(main_window, "lim_out_meter"));
    in_meter_adj = gtk_meter_get_adjustment(in_meter);
    att_meter_adj = gtk_meter_get_adjustment(att_meter);
    out_meter_adj = gtk_meter_get_adjustment(out_meter);
}

gboolean lh_changed(GtkAdjustment *adj, gpointer user_data)
{
    limiter.delay = adj->value * 0.0001f;

    return FALSE;
}

void limiter_meters_update()
{
    float peak_in = 20.0f * log10f(lim_peak[LIM_PEAK_IN]);
    float peak_out = 20.0f * log10f(lim_peak[LIM_PEAK_OUT]);
    float atten = -limiter.attenuation;

    gtk_adjustment_set_value(in_meter_adj, peak_in);
    gtk_adjustment_set_value(att_meter_adj, atten);
    gtk_adjustment_set_value(out_meter_adj, peak_out);
}

/* vi:set ts=8 sts=4 sw=4: */
