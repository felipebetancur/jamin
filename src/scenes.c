/*
 *  scenes.c -- Scene changing buttons for the JAMin (JACK Audio Mastering 
 *              interface) program.
 *
 *  Copyright (C) 2003 Jan C. Depner.
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


#include "scenes.h"
#include "main.h"
#include "support.h"
#include "interface.h"


static GtkMenu           *scene_menu;
static GtkImage          *l_scene[NUM_SCENES];
static GtkEventBox       *l_scene_eventbox[NUM_SCENES];
static GtkEntry          *l_scene_name[NUM_SCENES];
static int               current_scene, menu_scene;
static gboolean          scene_loaded[NUM_SCENES];
static s_state           scene_state[NUM_SCENES];


void set_EQ_curve_values ();


/*  Initialize all scene related structures and get the widget addresses.  */

void bind_scenes ()
{
    int             i;
    char            name[20];


    scene_menu = (GtkMenu *) create_scene_menu();


    current_scene = -1;
    menu_scene = -1;

    for (i = 0 ; i < NUM_SCENES ; i++)
      {
        sprintf (name, "scene%1d", i + 1);
        l_scene[i] = GTK_IMAGE (lookup_widget (main_window, name));
        sprintf (name, "scene%1d_eventbox", i + 1);
        l_scene_eventbox[i] = 
          GTK_EVENT_BOX (lookup_widget (main_window, name));
        sprintf (name, "scene%1d_name", i + 1);
        l_scene_name[i] = 
          GTK_ENTRY (lookup_widget (main_window, name));
        scene_loaded[i] = FALSE;
        scene_state[i].description = NULL; 
        gtk_widget_set_sensitive ((GtkWidget *) l_scene[i], FALSE);
      }
}


/*  Select one of the scenes as the current scene or pop up the set/clear
    menu.  */

void select_scene (int number, int button)
{
    int             i;


    /*  Left button selects the scene.  */

    switch (button)
      {
      case 1:
        if (scene_loaded[number])
          {
            for (i = 0 ; i < NUM_SCENES ; i++)
              {
                if (i == number)
                  {
                    gtk_image_set_from_stock (l_scene[i], GTK_STOCK_YES, 
                                              GTK_ICON_SIZE_BUTTON);

                    current_scene = i;

                    s_restore_state (&scene_state[i]);

                    set_EQ_curve_values ();
                  }
                else
                  {
                    gtk_image_set_from_stock (l_scene[i], GTK_STOCK_NO, 
                                              GTK_ICON_SIZE_BUTTON);
                  }
              }
          }
        break;


        /*  Middle button is ignored.  */

      case 2:
        break;


        /*  Right button pops up the set/clear menu.  */

      case 3:
        menu_scene = number;
        gtk_menu_popup (scene_menu, NULL, NULL, NULL, NULL, button, 
                        gtk_get_current_event_time());
        break;
      }
}


/*  Returns the current active scene number or -1 if no scene is active.  */

int get_current_scene ()
{
    return (current_scene);
}


/*  Returns the requested scene state or NULL if that scene is not loaded.  */

s_state *get_scene (int number)
{
  if (!scene_loaded[number]) return (NULL);

  return (&scene_state[number]);
}


/*  Set the scene state from the current settings.  Get the scene name from
    the scene_name text entry widget.  If scene_num is -1 use the last pressed
    scene button number.  */

void set_scene (int scene_num)
{
    int         i;
    char        name[256];
    GtkTooltips *tooltips = gtk_tooltips_new();


    if (scene_num >= 0) menu_scene = scene_num;


    gtk_widget_set_sensitive ((GtkWidget *) l_scene[menu_scene], TRUE);

    for (i = 0 ; i < S_SIZE ; i++) 
      scene_state[menu_scene].value[i] = s_get_value(i);

    
    strcpy (name, gtk_entry_get_text (l_scene_name[menu_scene]));
    scene_state[menu_scene].description = 
            (char *) realloc (scene_state[menu_scene].description, 
            strlen (name) + 1);

    strcpy (scene_state[menu_scene].description, name);


    /*  Set the scene loaded flag.  */

    scene_loaded[menu_scene] = TRUE;


    /*  Change the selected icon to green/yes.  */

    for (i = 0 ; i < NUM_SCENES ; i++)
      {
        if (i == menu_scene)
          {
            gtk_image_set_from_stock (l_scene[i], GTK_STOCK_YES, 
                                      GTK_ICON_SIZE_BUTTON);

            current_scene = i;
          }
        else
          {
            gtk_image_set_from_stock (l_scene[i], GTK_STOCK_NO, 
                                      GTK_ICON_SIZE_BUTTON);
          }
      }


    /*  Set the tooltip to the full name (it may be too long to show up
        completely in the text widget).  */

    gtk_tooltips_set_tip (tooltips, GTK_WIDGET (l_scene_eventbox[menu_scene]), 
                          scene_state[menu_scene].description, NULL);
}


/* Gets the scene name */

const char *get_scene_name(int number)
{
    if (!scene_loaded[number]) return (NULL);
    return gtk_entry_get_text(l_scene_name[number]);
}


/*  Set the scene name.  If the scene_name passed in is null get the name 
    from the scene_name text entry widget.  This is called from callbacks.c
    on a change to the scene_name widget.  */

void set_scene_name (int number, const char *scene_name)
{
    char        name[256];
    GtkTooltips *tooltips = gtk_tooltips_new();


    /*  If we are trying to modify the name without anything loaded, bypass
        this.  */

    if (menu_scene < 0) return;


    if (scene_name == NULL)
      {
        strcpy (name, gtk_entry_get_text (l_scene_name[number]));
      }
    else
      {
        strcpy (name, scene_name);

        gtk_entry_set_text (l_scene_name[number], name);
      }

    scene_state[number].description = 
        (char *) realloc (scene_state[number].description, 
        strlen (name) + 1);

    strcpy (scene_state[menu_scene].description, name);


    /*  Set the tooltip to the full name (it may be too long to show up
        completely in the text widget).  */

    gtk_tooltips_set_tip (tooltips, GTK_WIDGET (l_scene_eventbox[menu_scene]), 
                          scene_state[menu_scene].description, NULL);
}


/*  Clear the scene state.  If scene_num is -1 use the last pressed scene 
    button number.  */

void clear_scene (int scene_num)
{
    GtkTooltips *tooltips = gtk_tooltips_new();


    if (scene_num >= 0) menu_scene = scene_num;


    gtk_widget_set_sensitive ((GtkWidget *) l_scene[menu_scene], FALSE);

    gtk_tooltips_set_tip (tooltips, GTK_WIDGET (l_scene_eventbox[menu_scene]), 
                          "Right click for menu", NULL);

    gtk_image_set_from_stock (l_scene[menu_scene], GTK_STOCK_NO, 
                              GTK_ICON_SIZE_BUTTON);

    scene_loaded[menu_scene] = FALSE;
}


/*  Set all of the buttons to unselected state.  This should be done whenever 
    there is a state change.  I'm not sure quite where to put the calls at
    present.  */

void unset_scene_buttons ()
{
    int         i;


    current_scene = -1;
    for (i = 0 ; i < NUM_SCENES ; i++)
      {
        gtk_image_set_from_stock (l_scene[i], GTK_STOCK_NO, 
                                  GTK_ICON_SIZE_BUTTON);
      }
}
