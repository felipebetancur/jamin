#include "scenes.h"
#include "main.h"
#include "support.h"


static GtkImage *l_scene[NUM_SCENES];


void select_scene (int number)
{
    int             i;
    char            name[20];
    static int      first = 1;


    if (first)
      {
        for (i = 0 ; i < NUM_SCENES ; i++)
          {
            sprintf (name, "scene%1d", i + 1);
            l_scene[i] = GTK_IMAGE (lookup_widget (main_window, name));
          }
        first = 0;
      }


    for (i = 0 ; i < NUM_SCENES ; i++)
      {
        if (i == number)
          {
            gtk_image_set_from_stock (l_scene[i], GTK_STOCK_YES, 
                                      GTK_ICON_SIZE_BUTTON);
          }
        else
          {
            gtk_image_set_from_stock (l_scene[i], GTK_STOCK_NO, 
                                      GTK_ICON_SIZE_BUTTON);
          }
      }
  fprintf (stderr, "%s %d Scene %d\n",__FILE__,__LINE__,number);
}
