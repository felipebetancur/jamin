#include "scenes.h"
#include "main.h"
#include "support.h"


static GtkImage          *l_scene[NUM_SCENES];
static GtkEventBox       *l_scene_eventbox[NUM_SCENES];
static int               current_scene, next_scene;
static gboolean          scene_loaded[NUM_SCENES];
static s_state           scene_state[NUM_SCENES];


void bind_scenes ()
{
    int             i;
    char            name[20];

    current_scene = -1;
    next_scene = -1;

    for (i = 0 ; i < NUM_SCENES ; i++)
      {
        sprintf (name, "scene%1d", i + 1);
        l_scene[i] = GTK_IMAGE (lookup_widget (main_window, name));
        sprintf (name, "scene%1d_eventbox", i + 1);
        l_scene_eventbox[i] = 
          GTK_EVENT_BOX (lookup_widget (main_window, name));
        scene_loaded[i] = FALSE;
        scene_state[i].description = NULL; 
        gtk_widget_set_sensitive ((GtkWidget *) l_scene[i], FALSE);
        gtk_widget_set_sensitive ((GtkWidget *) l_scene_eventbox[i], FALSE);
      }
}


void select_scene (int number)
{
    int             i;

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
              }
            else
              {
                gtk_image_set_from_stock (l_scene[i], GTK_STOCK_NO, 
                                          GTK_ICON_SIZE_BUTTON);
              }
          }
      }
}


int get_current_scene ()
{
    return (current_scene);
}


void load_scene (s_state *state)
{
    int         i, num;
    GtkTooltips *tooltips = gtk_tooltips_new();


    next_scene++;


    num = next_scene % NUM_SCENES;

    gtk_widget_set_sensitive ((GtkWidget *) l_scene[num], TRUE);
    gtk_widget_set_sensitive ((GtkWidget *) l_scene_eventbox[num], TRUE);

    for (i = 0 ; i < S_SIZE ; i++) 
      scene_state[num].value[i] = state->value[i];

    scene_state[num].description = 
        (char *) realloc (scene_state[num].description, 
        strlen (state->description) + 1);

    strcpy (scene_state[num].description, state->description);

    scene_loaded[num] = TRUE;

    for (i = 0 ; i < NUM_SCENES ; i++)
      {
        if (i == num)
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

    gtk_tooltips_set_tip (tooltips, GTK_WIDGET (l_scene_eventbox[num]), 
                          scene_state[num].description, NULL);
}
