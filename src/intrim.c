#include <gtk/gtk.h>
#include <math.h>
#include <stdio.h>

#include "process.h"
#include "io.h"
#include "support.h"
#include "main.h"
#include "intrim.h"

static GtkProgressBar *in_meter[2];
static GtkLabel	*pan_label;

float in_gain[2] = {1.0f, 1.0f};
float in_trim_gain = 1.0f;
float in_pan_gain[2] = {1.0f, 1.0f};

void bind_intrim()
{
    in_meter[0] = GTK_PROGRESS_BAR(lookup_widget(main_window, "inmeter_l"));
    in_meter[1] = GTK_PROGRESS_BAR(lookup_widget(main_window, "inmeter_r"));
    pan_label = GTK_LABEL(lookup_widget(main_window, "pan_label"));
    gtk_progress_bar_set_fraction(in_meter[0], 0.5);
    gtk_progress_bar_set_fraction(in_meter[1], 0.5);
    update_pan_label(0.0);
}

void in_meter_value(float amp[])
{
    gtk_progress_bar_set_fraction(in_meter[0],
	    iec_scale(20.0f * log10f(amp[0])) * 0.01f);
    gtk_progress_bar_set_fraction(in_meter[1],
	    iec_scale(20.0f * log10f(amp[1])) * 0.01f);
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
