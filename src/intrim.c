#include <gtk/gtk.h>
#include <math.h>
#include <stdio.h>

#include "process.h"
#include "io.h"
#include "support.h"
#include "main.h"
#include "intrim.h"
#include "gtkmeter.h"
#include "state.h"

static GtkMeter *in_meter[2], *out_meter[2];
static GtkAdjustment *in_meter_adj[2], *out_meter_adj[2];
static GtkLabel	*pan_label;

void intrim_cb(int id, float value);
void inpan_cb(int id, float value);

float in_gain[2] = {1.0f, 1.0f};
float in_trim_gain = 1.0f;
float in_pan_gain[2] = {1.0f, 1.0f};

void bind_intrim()
{
    in_meter[0] = GTK_METER(lookup_widget(main_window, "inmeter_l"));
    in_meter[1] = GTK_METER(lookup_widget(main_window, "inmeter_r"));
    in_meter_adj[0] = gtk_meter_get_adjustment(in_meter[0]);
    in_meter_adj[1] = gtk_meter_get_adjustment(in_meter[1]);
    gtk_adjustment_set_value(in_meter_adj[0], -60.0);
    gtk_adjustment_set_value(in_meter_adj[1], -60.0);

    out_meter[0] = GTK_METER(lookup_widget(main_window, "outmeter_l"));
    out_meter[1] = GTK_METER(lookup_widget(main_window, "outmeter_r"));
    out_meter_adj[0] = gtk_meter_get_adjustment(out_meter[0]);
    out_meter_adj[1] = gtk_meter_get_adjustment(out_meter[1]);
    gtk_adjustment_set_value(out_meter_adj[0], -60.0);
    gtk_adjustment_set_value(out_meter_adj[1], -60.0);

    pan_label = GTK_LABEL(lookup_widget(main_window, "pan_label"));
    update_pan_label(0.0);

    s_set_callback(S_IN_GAIN, intrim_cb);
    s_set_adjustment(S_IN_GAIN, gtk_range_get_adjustment(lookup_widget(main_window, "in_trim_scale")));
    s_set_callback(S_IN_PAN, inpan_cb);
    s_set_adjustment(S_IN_PAN, gtk_range_get_adjustment(lookup_widget(main_window, "pan_scale")));
}

void intrim_cb(int id, float value)
{
    in_trim_gain = powf(10.0f, value * 0.05f);
    in_gain[0] = in_trim_gain * in_pan_gain[0];
    in_gain[1] = in_trim_gain * in_pan_gain[1];
}

void inpan_cb(int id, float value)
{
    in_pan_gain[0] = powf(10.0f, value * -0.025f);
    in_pan_gain[1] = powf(10.0f, value * 0.025f);
    in_gain[0] = in_trim_gain * in_pan_gain[0];
    in_gain[1] = in_trim_gain * in_pan_gain[1];
    update_pan_label(value);
}

void in_meter_value(float amp[])
{
    gtk_adjustment_set_value(in_meter_adj[0], 20.0f * log10f(amp[0]));
    gtk_adjustment_set_value(in_meter_adj[1], 20.0f * log10f(amp[1]));
    amp[0] = 0.0f;
    amp[1] = 0.0f;
}

void out_meter_value(float amp[])
{
    gtk_adjustment_set_value(out_meter_adj[0], 20.0f * log10f(amp[0]));
    gtk_adjustment_set_value(out_meter_adj[1], 20.0f * log10f(amp[1]));
    amp[0] = 0.0f;
    amp[1] = 0.0f;
}

void update_pan_label(float balance)
{
    char tmp[256];

    if (balance < -0.5f) {
	snprintf(tmp, 255, "left %.0fdB", -balance);
    } else if (balance > 0.5f) {
	snprintf(tmp, 255, "right %.0fdB", balance);
    } else {
	sprintf(tmp, "centre");
    }
    gtk_label_set_label(pan_label, tmp);
}

/* vi:set ts=8 sts=4 sw=4: */
