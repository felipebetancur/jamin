#include <gtk/gtk.h>
#include <math.h>

#include "process.h"
#include "io.h"
#include "support.h"
#include "main.h"

static GtkProgressBar *in_meter;

void in_meter_value(float amp);

void bind_intrim()
{
    in_meter = GTK_PROGRESS_BAR(lookup_widget(main_window, "inmeter"));
    gtk_progress_bar_set_fraction(in_meter, 0.5);
}

void in_meter_value(float amp)
{
    gtk_progress_bar_set_fraction(in_meter, iec_scale(20.0f * log10f(amp)) *
				    0.01f);
}

/* vi:set ts=8 sts=4 sw=4: */
