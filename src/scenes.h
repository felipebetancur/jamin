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
void set_scene (int scene_num);
void clear_scene (int scene_num);
void set_scene_name (int number, char *scene_name);
void unset_scene_buttons ();
s_state *get_scene (int number);


#endif

