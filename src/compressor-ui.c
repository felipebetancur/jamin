#include <stdio.h>
#include <gtk/gtk.h>
#include <math.h>

#include "process.h"
#include "io.h"
#include "support.h"
#include "main.h"
#include "compressor-ui.h"
#include "gtkmeter.h"
#include "state.h"

gboolean adj_cb(GtkAdjustment *adj, gpointer p);
void at_changed(int id, float value);
void re_changed(int id, float value);
void th_changed(int id, float value);
void ra_changed(int id, float value);
void kn_changed(int id, float value);
void ma_changed(int id, float value);

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

#define connect_scale(sym, i, member, state_id) \
	snprintf(name, 255, "comp_" # sym "_%d", i+1); \
	scale = lookup_widget(main_window, name); \
	adj_##sym[i] = gtk_range_get_adjustment(GTK_RANGE(scale)); \
	s_set_callback(state_id, sym##_changed); \
	s_set_adjustment(state_id, adj_##sym[i]); \
	s_set_value(state_id, compressors[i].member, 0); \
	g_signal_connect(G_OBJECT(adj_##sym[i]), "value-changed", G_CALLBACK(adj_cb), (gpointer)state_id); 

	//g_signal_connect(G_OBJECT(adj_##sym[i]), "value-changed", G_CALLBACK(sym##_changed), (gpointer)i); 
	//gtk_adjustment_set_value(adj_##sym[i], compressors[i].member);

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

	connect_scale(at, i, attack, S_COMP_ATTACK(i));
	connect_scale(re, i, release, S_COMP_RELEASE(i));
	connect_scale(th, i, threshold, S_COMP_THRESH(i));
	connect_scale(ra, i, ratio, S_COMP_RATIO(i));
	connect_scale(kn, i, knee, S_COMP_KNEE(i));
	connect_scale(ma, i, makeup_gain, S_COMP_MAKEUP(i));
	ma[i] = scale;

	auto_gain[i] = 0;
    }
}

gboolean adj_cb(GtkAdjustment *adj, gpointer p)
{
    s_set_value_ui((int)p, adj->value);

    return FALSE;
}

void at_changed(int id, float value)
{
    compressors[id - S_COMP_ATTACK(0)].attack = value;
    draw_comp_curve(id - S_COMP_ATTACK(0));
}

void re_changed(int id, float value)
{
    compressors[id - S_COMP_RELEASE(0)].release = value;
    draw_comp_curve(id - S_COMP_RELEASE(0));
}

void th_changed(int id, float value)
{
    int band = id - S_COMP_THRESH(0);

    compressors[band].threshold = value;
    if (auto_gain[band]) {
	calc_auto_gain(band);
    } else {
	draw_comp_curve(band);
    }
    gtk_meter_set_warn_point(le_meter[band], value);
}

void ra_changed(int id, float value)
{
    int band = id - S_COMP_RATIO(0);

    compressors[band].ratio = value;
    if (auto_gain[band]) {
	calc_auto_gain(band);
    } else {
	draw_comp_curve(band);
    }
}

void kn_changed(int id, float value)
{
    compressors[id - S_COMP_KNEE(0)].knee = value * 9.0f + 1.0f;
    draw_comp_curve(id - S_COMP_KNEE(0));
}

void ma_changed(int id, float value)
{
    compressors[id - S_COMP_MAKEUP(0)].makeup_gain = value;
    draw_comp_curve(id - S_COMP_MAKEUP(0));
}

void calc_auto_gain(int i)
{
    if (adj_ma[i] && adj_th[i] && adj_ra[i]) {
	s_set_value_no_history(S_COMP_MAKEUP(i), adj_th[i]->value / adj_ra[i]->value - adj_th[i]->value);
	//gtk_adjustment_set_value(adj_ma[i], adj_th[i]->value / adj_ra[i]->value - adj_th[i]->value);
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
