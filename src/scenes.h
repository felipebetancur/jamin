#ifndef SCENES_H
#define SCENES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

#include "state.h"


#define NUM_SCENES 6


void bind_scenes ();
void select_scene (int number);
int get_current_scene ();
void load_scene (s_state *state);


#endif

