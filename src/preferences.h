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
 *  $Id: preferences.h,v 1.2 2004/01/19 23:20:39 jdepner Exp $
 */

#ifndef PREFERENCES_H
#define PREFERENCES_H


#define LOW_BAND_COLOR       0
#define MID_BAND_COLOR       1
#define HIGH_BAND_COLOR      2
#define NORMAL_COLOR         3
#define GANG_HIGHLIGHT_COLOR 4


void preferences_init();
GdkColor *get_band_color (int band);
void set_band_color (int band, GdkColor color);
GdkColor *get_gang_color ();
void set_gang_color (GdkColor color);
void set_color (GdkColor *color, unsigned short red, unsigned short green, 
                unsigned short blue);
void popup_preferences_dialog (int updown);
void popup_color_dialog (int id);


#endif
