#include <stdio.h>
#include <gtk/gtk.h>
#include <math.h>

#include "process.h"
#include "io.h"
#include "support.h"
#include "main.h"

gboolean lh_changed(GtkAdjustment *adj, gpointer user_data);
GtkAdjustment *lh_adj;

static GtkProgressBar *in_meter, *out_meter, *att_meter;

void bind_limiter()
{
    GtkWidget *scale;

    scale = lookup_widget(main_window, "lim_lh_scale");
    lh_adj = gtk_range_get_adjustment(GTK_RANGE(scale));
    gtk_signal_connect(lh_adj, "value-changed", GTK_SIGNAL_FUNC(lh_changed),
	    NULL);
    in_meter = lookup_widget(main_window, "lim_in_meter");
    out_meter = lookup_widget(main_window, "lim_out_meter");
    att_meter = lookup_widget(main_window, "lim_att_meter");
}

gboolean lh_changed(GtkAdjustment *adj, gpointer user_data)
{
    limiter.delay = adj->value * 0.0001f;

    return FALSE;
}

void limiter_meters_update()
{
    gtk_progress_bar_set_fraction(att_meter, limiter.attenuation / 12.0f);
    gtk_progress_bar_set_fraction(in_meter, iec_scale(20.0f * log10f(lim_peak[LIM_PEAK_IN])) * 0.01f);
    gtk_progress_bar_set_fraction(out_meter, iec_scale(20.0f * log10f(lim_peak[LIM_PEAK_OUT])) * 0.01f);
}

/* vi:set ts=8 sts=4 sw=4: */
