#include <stdio.h>
#include <gtk/gtk.h>
#include <math.h>

#include "process.h"
#include "io.h"
#include "support.h"
#include "main.h"
#include "compressor-ui.h"
#include "gtkmeter.h"

gboolean at_changed(GtkAdjustment *adj, gpointer user_data);
gboolean re_changed(GtkAdjustment *adj, gpointer user_data);
gboolean th_changed(GtkAdjustment *adj, gpointer user_data);
gboolean ra_changed(GtkAdjustment *adj, gpointer user_data);
gboolean kn_changed(GtkAdjustment *adj, gpointer user_data);
gboolean ma_changed(GtkAdjustment *adj, gpointer user_data);

void calc_auto_gain(int i);
void draw_comp_curve (int i);

static GtkWidget *ma[XO_BANDS];
static GtkAdjustment *adj_at[XO_BANDS];
static GtkAdjustment *adj_re[XO_BANDS];
static GtkAdjustment *adj_th[XO_BANDS];
static GtkAdjustment *adj_ra[XO_BANDS];
static GtkAdjustment *adj_kn[XO_BANDS];
static GtkAdjustment *adj_ma[XO_BANDS];
static int auto_gain[XO_BANDS];

static GtkMeter *le_meter[XO_BANDS], *ga_meter[XO_BANDS];
static GtkAdjustment *le_meter_adj[XO_BANDS], *ga_meter_adj[XO_BANDS];

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

    for (i=0; i<XO_BANDS; i++) {
	snprintf(name, 255, "comp_le_%d", i+1);
	le_meter[i] = GTK_METER(lookup_widget(main_window, name));
	le_meter_adj[i] = gtk_meter_get_adjustment(le_meter[i]);

	snprintf(name, 255, "comp_ga_%d", i+1);
	ga_meter[i] = GTK_METER(lookup_widget(main_window, name));
	ga_meter_adj[i] = gtk_meter_get_adjustment(ga_meter[i]);

	connect_scale(at, i, attack);
	connect_scale(re, i, release);
	connect_scale(th, i, threshold);
	connect_scale(ra, i, ratio);
	connect_scale(kn, i, knee);
	connect_scale(ma, i, makeup_gain);
	ma[i] = scale;

	auto_gain[i] = 0;
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
    if (auto_gain[(int)user_data]) {
	calc_auto_gain((int)user_data);
    } else {
	draw_comp_curve((int)user_data);
    }
    gtk_meter_set_warn_point(le_meter[(int)user_data], adj->value);

    return FALSE;
}

gboolean ra_changed(GtkAdjustment *adj, gpointer user_data)
{
    compressors[(int)user_data].ratio = adj->value;
    if (auto_gain[(int)user_data]) {
	calc_auto_gain((int)user_data);
    } else {
	draw_comp_curve((int)user_data);
    }

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

void calc_auto_gain(int i)
{
    if (adj_ma[i] && adj_th[i] && adj_ra[i]) {
	gtk_adjustment_set_value(adj_ma[i], adj_th[i]->value / adj_ra[i]->value - adj_th[i]->value);
    }
}

void compressor_meters_update()
{
    int i;

    for (i=0; i<XO_BANDS; i++) {
	gtk_adjustment_set_value(le_meter_adj[i], compressors[i].amplitude);
	gtk_adjustment_set_value(ga_meter_adj[i], compressors[i].gain_red);
    }
}

void comp_set_auto(int band, int state)
{
    auto_gain[band] = state;
    gtk_widget_set_sensitive(ma[band], !state);
    if (state) {
	calc_auto_gain(band);
    }
}

comp_settings comp_get_settings(int band)
{
    return (compressors[band]);
}

/* vi:set ts=8 sts=4 sw=4: */
