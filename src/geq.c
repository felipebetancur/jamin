/* code to control the graphic eq's, swh */

#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <math.h>

#include "geq.h"
#include "process.h"
#include "io.h"
#include "support.h"
#include "main.h"
#include "db.h"

GtkAdjustment *geqa[EQ_BANDS];
GtkRange *geqr[EQ_BANDS];

static int EQ_drawn = 0;

/* Linear gain of the 1/3rd octave EQ bands */
float geq_gains[EQ_BANDS + 1];
/* Frequency of each band of the EQ */
float geq_freqs[EQ_BANDS];

int bin_base[BINS];
float bin_delta[BINS];

gboolean eqb_changed(GtkAdjustment *adj, gpointer user_data);
gboolean eqb_mod(GtkAdjustment *adj, gpointer user_data);
void geq_set_gains();

void bind_geq()
{
    char name[16];
    int i, bin;
    float last_bin, next_bin;
    const double hz_per_bin = sample_rate / (double)BINS;
    GtkTooltips *tooltips = gtk_tooltips_new();
    char tip[255];

    for (i=0; i<EQ_BANDS; i++) {
	geq_freqs[i] = 1000.0 * pow(10.0, (double)(i-16) * 0.1);
	/* printf("GEQ band %d = %g Hz\n", i, geq_freqs[i]); */
    }

    for (i=0; i<EQ_BANDS; i++) {
	sprintf(name, "eqb%d", i+1);
	geqr[i] = GTK_RANGE(lookup_widget(main_window, name));
	snprintf(tip, 255, "%'.0f Hz", floor(geq_freqs[i] + 0.5));
	gtk_tooltips_set_tip(tooltips, GTK_WIDGET(geqr[i]), tip, NULL);
	geqa[i] = GTK_ADJUSTMENT(gtk_range_get_adjustment(GTK_RANGE(geqr[i])));
        g_signal_connect(G_OBJECT(geqa[i]), "value-changed", G_CALLBACK(eqb_mod), NULL);
	g_signal_connect(G_OBJECT(geqa[i]), "value-changed", G_CALLBACK(eqb_changed), (gpointer)i+1);
    }

    for (i=0; i<BANDS + 1; i++) {
	geq_gains[i] = 1.0f;
    }

    bin = 0;
    while (bin <= geq_freqs[0] / hz_per_bin && bin < (BINS / 2) - 1) {
	bin_base[bin] = 0;
	bin_delta[bin++] = 0.0f;
    }

    for (i = 1; i < BANDS - 1 && bin < (BINS / 2) - 1
	 && geq_freqs[i+1] < sample_rate / 2; i++) {
	last_bin = bin;
	next_bin = geq_freqs[i+1] / hz_per_bin;
	while (bin <= next_bin) {
	    bin_base[bin] = i;
	    bin_delta[bin] = (float)(bin-last_bin) / (float)(next_bin-last_bin);
	    bin++;
	}
    }

    for (; bin < (BINS / 2); bin++) {
	bin_base[bin] = BANDS - 1;
	bin_delta[bin] = 0.0f;
    }

    geq_set_gains();
}

void geq_set_gains()
{
    unsigned int bin;

    if (!EQ_drawn)
      {
        eq_coefs[0] = 1.0f;
        for (bin = 1; bin < (BINS/2 - 1); bin++) {
          eq_coefs[bin] = ((1.0f-bin_delta[bin]) * geq_gains[bin_base[bin]])
            + (bin_delta[bin] * geq_gains[bin_base[bin]+1]);
        }
      }
}

void geq_set_sliders(int length, float x[], float y[])
{
    int i, j;
    float value;


    if (length != BINS / 2 - 1)
      {
        fprintf (stderr, 
            _("Splined length %d does not match BINS / 2 - 1 (%d)\n"), length,
             BINS / 2 - 1);
      }
    else
      {
        /*  Set eq_coefs using linear gain values.  */

        eq_coefs[0] = 1.0f;
        for (i = 1 ; i < length ; i++) 
          {
            eq_coefs[i] = ((1.0f - bin_delta[i]) * pow (10.0, (double) y[i])) +
              (bin_delta[i] * pow (10.0, (double) y[i + 1]));
          }


        /*  Make sure that we don't reset the coefficients when we set the
            graphic EQ adjustments (see geq_set_gains).  */

        EQ_drawn = 1;


        /*  Set the faders in the graphic EQ.  First and last should be
            exact since that's what we splined to.  Use linear interpolation 
            for the others.  */

        gtk_adjustment_set_value (geqa[0], y[0] / 0.05);
        for (j = 1, i = 0 ; j < EQ_BANDS - 1 ; j++)
          {
            while (geq_freqs[j] > x[i]) i++;

            value = y[i - 1] + (y[i] - y[i - 1]) * ((geq_freqs[j] - x[i - 1]) /
                (x[i] - x[i - 1]));

            gtk_adjustment_set_value (geqa[j], value / 0.05);
          }
        gtk_adjustment_set_value (geqa[EQ_BANDS - 1], y[length - 1] / 0.05);


        /*  Release the restriction on the graphic EQ adjustments.  */

        EQ_drawn = 0;
      }
}

void geq_set_range(double min, double max)
{
    int             i;

    for (i = 0 ; i < EQ_BANDS ; i++)
      {
        gtk_range_set_range (geqr[i], min, max);
      }
}

void geq_get_freqs_and_gains(float *freqs, float *gains)
{
    int              i;

    for (i = 0 ; i < EQ_BANDS ; i++)
      {
        freqs[i] = geq_freqs[i];
        gains[i] = geq_gains[i];
      }
}
    
gboolean eqb_changed(GtkAdjustment *adj, gpointer user_data)
{
    int band = (int)user_data;

    geq_gains[band-1] = db2lin(adj->value);
    geq_set_gains();

    return FALSE;
}

GtkAdjustment *geq_get_adjustment(int band)
{
    if (band < 0 || band > EQ_BANDS) {
	fprintf(stderr, _("jam error: Adjustment from out-of-range band %d requested\n"), band);
	exit(1);
    }
    return geqa[band];
}

/* vi:set ts=8 sts=4 sw=4: */
