/*
 *  Copyright (C) 2003 Steve Harris
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
 *  $Id: intrim.h,v 1.5 2003/11/20 21:19:55 theno23 Exp $
 */

#ifndef INTRIM_H
#define INTRIM_H

void bind_intrim();
void in_meter_value(float amp[]);
void out_meter_value(float amp[]);
void update_pan_label(float balance);

extern float in_gain[], out_gain;
extern float in_trim_gain;
extern float in_pan_gain[];

#endif
