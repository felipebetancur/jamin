#include <stdio.h>
#include <gtk/gtk.h>
#include <math.h>

#include "process.h"
#include "io.h"
#include "support.h"
#include "main.h"

gboolean at_changed(GtkAdjustment *adj, gpointer user_data);
gboolean re_changed(GtkAdjustment *adj, gpointer user_data);
gboolean th_changed(GtkAdjustment *adj, gpointer user_data);
gboolean ra_changed(GtkAdjustment *adj, gpointer user_data);
gboolean kn_changed(GtkAdjustment *adj, gpointer user_data);
gboolean ma_changed(GtkAdjustment *adj, gpointer user_data);

void draw_comp_curve (int i);

static GtkAdjustment *adj_at[3];
static GtkAdjustment *adj_re[3];
static GtkAdjustment *adj_th[3];
static GtkAdjustment *adj_ra[3];
static GtkAdjustment *adj_kn[3];
static GtkAdjustment *adj_ma[3];

static GtkProgressBar *le_meter[3], *ga_meter[3];

#define connect_scale(sym, i, member) \
	snprintf(name, 255, "comp_" # sym "_%d", i+1); \
	scale = lookup_widget(main_window, name); \
	adj_##sym[i] = gtk_range_get_adjustment(GTK_RANGE(scale)); \
	g_signal_connect(G_OBJECT(adj_##sym[i]), "value-changed", G_CALLBACK(sym##_changed), (gpointer)i); \
	gtk_adjustment_set_value(adj_##sym[i], compressors[i].member);

void bind_compressors()
{
    GtkWidget *scale;
    char name[256];
    int i;

    for (i=0; i<3; i++) {
	snprintf(name, 255, "comp_le_%d", i+1);
	le_meter[i] = GTK_PROGRESS_BAR(lookup_widget(main_window, name));
	snprintf(name, 255, "comp_ga_%d", i+1);
	ga_meter[i] = GTK_PROGRESS_BAR(lookup_widget(main_window, name));

	connect_scale(at, i, attack);
	connect_scale(re, i, release);
	connect_scale(th, i, threshold);
	connect_scale(ra, i, ratio);
	connect_scale(kn, i, knee);
	connect_scale(ma, i, makeup_gain);
    }
}

gboolean at_changed(GtkAdjustment *adj, gpointer user_data)
{
    compressors[(int)user_data].attack = adj->value;
    draw_comp_curve((int)user_data);

    return FALSE;
}

gboolean re_changed(GtkAdjustment *adj, gpointer user_data)
{
    compressors[(int)user_data].release = adj->value;
    draw_comp_curve((int)user_data);

    return FALSE;
}

gboolean th_changed(GtkAdjustment *adj, gpointer user_data)
{
    compressors[(int)user_data].threshold = adj->value;
    draw_comp_curve((int)user_data);

    return FALSE;
}

gboolean ra_changed(GtkAdjustment *adj, gpointer user_data)
{
    compressors[(int)user_data].ratio = adj->value;
    draw_comp_curve((int)user_data);

    return FALSE;
}

gboolean kn_changed(GtkAdjustment *adj, gpointer user_data)
{
    compressors[(int)user_data].knee = adj->value * 9.0f + 1.0f;
    draw_comp_curve((int)user_data);

    return FALSE;
}

gboolean ma_changed(GtkAdjustment *adj, gpointer user_data)
{
    compressors[(int)user_data].makeup_gain = adj->value;
    draw_comp_curve((int)user_data);

    return FALSE;
}

void compressor_meters_update()
{
    int i;
    float met;

    for (i=0; i<3; i++) {
	met = iec_scale(compressors[i].amplitude) * 0.01f;
	if (met > 1.0f) {
	    met = 1.0f;
	} else if (met < 0.0f) {
	    met = 0.0f;
	}
	gtk_progress_bar_set_fraction(le_meter[i], met);

	met = 1.0f - iec_scale(compressors[i].gain_red) * 0.01f;
	if (met > 1.0f) {
	    met = 1.0f;
	} else if (met < 0.0f) {
	    met = 0.0f;
	}
	gtk_progress_bar_set_fraction(ga_meter[i], met);
    }
}

comp_settings comp_get_settings(int i)
{
    return (compressors[i]);
}

/* vi:set ts=8 sts=4 sw=4: */
