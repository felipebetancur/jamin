/*
 *  Copyright (C) 2003 Jan C. Depner
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  $Id: hdeq.h,v 1.8 2004/01/19 20:31:06 jdepner Exp $
 */

#ifndef HDEQ_H
#define HDEQ_H

void clean_quit ();
GdkColor *get_band_color (int band);
void bind_hdeq ();
void hdeq_low2mid_set (GtkRange *range);
void hdeq_mid2high_set (GtkRange *range);
void hdeq_low2mid_button (int active);
void hdeq_mid2high_button (int active);
void hdeq_low2mid_init ();
void hdeq_mid2high_init ();
void crossover_init ();
void hdeq_eqb_mod ();
void draw_EQ_spectrum_curve (float single_levels[]);
void reset_hdeq ();
void hdeq_curve_exposed (GtkWidget *widget, GdkEventExpose  *event);
void hdeq_curve_init (GtkWidget *widget);
void hdeq_curve_motion (GdkEventMotion *event);
void hdeq_curve_button_press (GdkEventButton *event);
void hdeq_curve_button_release (GdkEventButton  *event);
void hdeq_curve_set_label (char *string);
void set_EQ_curve_values (int id, float value);
void popup_EQ_options_dialog (int updown);
void hdeq_set_lower_gain (float gain);
void hdeq_set_upper_gain (float gain);
void hdeq_set_xover ();
void draw_comp_curve (int i);
void comp_curve_expose (GtkWidget *widget, int i);
void comp_curve_realize (GtkWidget *widget, int i);
void comp_curve_box_motion (int i, GdkEventMotion  *event);
void comp_box_leave (int i);
void comp_box_enter (int i);
void hdeq_notebook1_set_page (guint page_num);
int get_current_notebook1_page ();


GtkNotebook *l_notebook1;


#endif
