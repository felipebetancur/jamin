/*
 *  Copyright (C) 2003 Jan C. Depner, Steve Harris
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
 *  $Id: scenes.h,v 1.12 2004/01/08 15:59:33 jdepner Exp $
 */

#ifndef SCENES_H
#define SCENES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

#include "state.h"


#define NUM_SCENES 6


void bind_scenes ();
void select_scene (int number, int button);
int get_current_scene ();
void set_scene (int scene_num, gboolean morph);
void clear_scene (int scene_num);
const char *get_scene_name(int number);
void set_scene_name (int number, const char *scene_name);
void unset_scene_buttons ();
s_state *get_scene (int number);
int get_previous_scene_num ();
void set_scene_warning_button ();
void set_scene_button (int scene);
void set_num_scene_warning_button (int scene);
int changed_scene_no(int s);
int is_changed_scene(int s);

#endif

