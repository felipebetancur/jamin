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
#include "interface.h"
#include "support.h"
#include "state.h"


static GtkWidget       *preferences_dialog;
static GdkColormap     *colormap = NULL;
static GdkColor        band_color[4], gang_color;



void preferences_init()
{
    preferences_dialog = create_preferences_dialog ();

    colormap = gdk_colormap_get_system ();

    set_color (&band_color[0], 60000, 0, 0);
    set_color (&band_color[1], 0, 50000, 0);
    set_color (&band_color[2], 0, 0, 60000);
    set_color (&band_color[3], 0, 0, 0);

    set_color (&gang_color, 60000, 0, 0);
}


GdkColor *get_band_color (int band)
{
  return (&band_color[band]);
}


void set_band_color (int band, GdkColor color)
{
}


GdkColor *get_gang_color ()
{
  return (&gang_color);
}


void set_gang_color (GdkColor color)
{
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




