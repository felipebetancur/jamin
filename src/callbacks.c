#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <errno.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "process.h"


static GtkHScale *l_low2mid, *l_mid2high;
static GtkWidget *l_low_comp, *l_mid_comp, *l_high_comp;


void
on_low2mid_value_changed               (GtkRange        *range,
                                        gpointer         user_data)
{
    double          value, other_value;

    value = gtk_range_get_value (range);
    other_value = gtk_range_get_value ((GtkRange *) l_mid2high);

    /* Write value into DSP code */
    xover_fa = value;

    /*  Don't let the two sliders cross each other and desensitize the mid
        band compressor if they are the same value.  */

    if (value >= other_value)
      {
        gtk_range_set_value ((GtkRange *) l_mid2high, value);
        gtk_widget_set_sensitive (l_mid_comp, FALSE);
      }
    else
      {
        gtk_widget_set_sensitive (l_mid_comp, TRUE);
      }


    /*  If the low slider is set to zero, desensitize the low band
        compressor.  */

    if (value <= 25.1)
      {
        gtk_widget_set_sensitive (l_low_comp, FALSE);
      }
    else
      {
        gtk_widget_set_sensitive (l_low_comp, TRUE);
      }
}


void
on_mid2high_value_changed              (GtkRange        *range,
                                        gpointer         user_data)
{
    double          value, other_value;
    GtkAdjustment   *adj;


    value = gtk_range_get_value (range);
    other_value = gtk_range_get_value ((GtkRange *) l_low2mid);

    /* Write value into DSP code */
    xover_fb = value;


    /*  Don't let the two sliders cross each other and desensitize the mid
        band compressor if they are the same value.  */

    if (value <= other_value)
      {
        gtk_range_set_value ((GtkRange *) l_low2mid, value);
        gtk_widget_set_sensitive (l_mid_comp, FALSE);
      }
    else
      {
        gtk_widget_set_sensitive (l_mid_comp, TRUE);
      }


    /*  If the slider is at the top of it's range, desensitize the high band
        compressor.  */


    adj = gtk_range_get_adjustment (range);

    if (value == adj->upper)
      {
        gtk_widget_set_sensitive (l_high_comp, FALSE);
      }
    else
      {
        gtk_widget_set_sensitive (l_high_comp, TRUE);
      }
}


void
on_low2mid_realize                     (GtkWidget       *widget,
                                        gpointer         user_data)
{
    l_low2mid = (GtkHScale *) widget;
}


void
on_mid2high_realize                    (GtkWidget       *widget,
                                        gpointer         user_data)
{
    l_mid2high = (GtkHScale *) widget;
}

void
on_low_comp_realize                    (GtkWidget       *widget,
                                        gpointer         user_data)
{
    l_low_comp = widget;
}


void
on_mid_comp_realize                    (GtkWidget       *widget,
                                        gpointer         user_data)
{
    l_mid_comp = widget;
}


void
on_high_comp_realize                   (GtkWidget       *widget,
                                        gpointer         user_data)
{
    l_high_comp = widget;
}


void
on_quit_clicked                        (GtkButton       *button,
                                        gpointer         user_data)
{
    gtk_main_quit();
}


gboolean
on_window1_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  gtk_main_quit();

  return FALSE;
}
