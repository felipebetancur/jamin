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
 *  $Id: preferences.h,v 1.1 2004/01/19 20:31:07 jdepner Exp $
 */

#ifndef PREFERENCES_H
#define PREFERENCES_H


void preferences_init();
GdkColor *get_band_color (int band);
void set_band_color (int band, GdkColor color);
GdkColor *get_gang_color ();
void set_gang_color (GdkColor color);
void set_color (GdkColor *color, unsigned short red, unsigned short green, 
                unsigned short blue);
void popup_preferences_dialog (int updown);


#endif
