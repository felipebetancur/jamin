/*
 *  preferences.c -- Preferences (color, crossfade time) dialog.
 *
 *  Copyright (C) 2004 Jan C. Depner.
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
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <gtk/gtk.h>


#include "preferences.h"
#include "hdeq.h"
#include "main.h"
#include "help.h"
#include "interface.h"
#include "support.h"
#include "compressor-ui.h"
#include "state.h"
#include "gtkmeter.h"


static char color_help[] = {
"    This is a standard color selection dialog.  Push buttons and see what \
happens.  If you don't like the color just press cancel.  When you've got \
the color you want (fuschia, puce, chartreuse, whatever) just press OK.\n"
};


static GtkWidget         *preferences_dialog, *color_dialog, *colorsel;
static GdkColormap       *colormap = NULL;
static GdkColor          color[COLORS];
static int               color_id;

static void color_ok_callback (GtkWidget *w, gpointer user_data);
static void color_cancel_callback (GtkWidget *w, gpointer user_data);
static void color_help_callback (GtkWidget *w, gpointer user_data);


void preferences_init()
{
    preferences_dialog = create_preferences_dialog ();

    color_dialog = create_colorselectiondialog1 ();

    colorsel = GTK_COLOR_SELECTION_DIALOG (color_dialog)->colorsel;

    g_signal_connect (GTK_OBJECT 
                      (GTK_COLOR_SELECTION_DIALOG (color_dialog)->ok_button),
                      "clicked", G_CALLBACK (color_ok_callback), color_dialog);

    g_signal_connect (GTK_OBJECT 
                      (GTK_COLOR_SELECTION_DIALOG (color_dialog)->cancel_button),
                      "clicked", G_CALLBACK (color_cancel_callback), color_dialog);

    g_signal_connect (GTK_OBJECT 
                      (GTK_COLOR_SELECTION_DIALOG (color_dialog)->help_button),
                      "clicked", G_CALLBACK (color_help_callback), color_dialog);


    colormap = gdk_colormap_get_system ();

    set_color (&color[NORMAL_COLOR], 0, 0, 0);
    set_color (&color[LOW_BAND_COLOR], 0, 50000, 0);
    set_color (&color[MID_BAND_COLOR], 0, 0, 60000);
    set_color (&color[HIGH_BAND_COLOR], 60000, 0, 0);
    set_color (&color[GANG_HIGHLIGHT_COLOR], 65535, 0, 0);

    set_color (&color[HANDLE_COLOR], 65535, 65535, 0);
    set_color (&color[HDEQ_CURVE_COLOR], 65535, 65535, 65535);
    set_color (&color[HDEQ_GRID_COLOR], 0, 36611, 0);
    set_color (&color[HDEQ_SPECTRUM_COLOR], 32768, 32768, 32768);
    set_color (&color[HDEQ_BACKGROUND_COLOR], 0, 21611, 0);
    set_color (&color[METER_NORMAL_COLOR], 0, 60000, 0);
    set_color (&color[METER_WARNING_COLOR], 50000, 55000, 0);
    set_color (&color[METER_OVER_COLOR], 60000, 0, 0);
    set_color (&color[METER_PEAK_COLOR], 60000, 60000, 0);
}


GdkColor *get_color (int color_id)
{
  return (&color[color_id]);
}


/*  Generic color setting.  */

void set_color (GdkColor *color, unsigned short red, unsigned short green, 
                unsigned short blue)
{
    color->red = red;
    color->green = green;
    color->blue = blue;

    gdk_colormap_alloc_color (colormap, color, FALSE, TRUE);
}


/*  Pop up the preferences dialog.  */

void popup_preferences_dialog (int updown)
{
  if (updown)
    {
      gtk_widget_show (preferences_dialog);
    }
  else
    {
      gtk_widget_hide (preferences_dialog);
    }
}


/*  Pop up the color dialog.  */

void popup_color_dialog (int id)
{
  GdkColor *ptr;

  color_id = id;

  ptr = &color[id];


  gtk_color_selection_set_current_color ((GtkColorSelection *) colorsel, ptr);


  gtk_widget_show (color_dialog);
}


static void color_ok_callback (GtkWidget *w, gpointer user_data)
{
  GdkColor l_color;


  gtk_color_selection_get_current_color ((GtkColorSelection *) colorsel, 
                                         &l_color);

  set_color (&color[color_id], l_color.red, l_color.green, l_color.blue);

  repaint_gang_labels ();
  draw_EQ_curve ();
  draw_comp_curve (0);
  draw_comp_curve (1);
  draw_comp_curve (2);
  gtk_meter_set_color (color_id);

  gtk_widget_hide (color_dialog);
}


static void color_cancel_callback (GtkWidget *w, gpointer user_data)
{
  gtk_widget_hide (color_dialog);
}


static void color_help_callback (GtkWidget *w, gpointer user_data)
{
  message (GTK_MESSAGE_INFO, color_help);
}
