#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "geq.h"
#include "interface.h"
#include "support.h"
#include "process.h"


#define NINT(a) ((a)<0.0 ? (int) ((a) - 0.5) : (int) ((a) + 0.5))


static GtkHScale       *l_low2mid, *l_mid2high;
static GtkVScale       *l_eqb1;
static GtkWidget       *l_low_comp, *l_mid_comp, *l_high_comp;
static GtkLabel        *l_low2mid_lbl, *l_mid2high_lbl, *l_low_comp_lbl, 
                       *l_mid_comp_lbl, *l_high_comp_lbl, *l_EQ_curve_lbl;
static GtkDrawingArea  *l_EQ_curve;
static GdkGC           *norm_gc;
static GtkAdjustment   *l_low2mid_adj, *l_eqb1_adj;
static float           EQ_curve_range_x, EQ_curve_range_y, EQ_curve_width,
                       EQ_curve_height;


void
on_low2mid_value_changed               (GtkRange        *range,
                                        gpointer         user_data)
{
    double          value, other_value,lvalue, mvalue, hvalue;
    char            label[6];

    value = gtk_range_get_value (range);
    other_value = gtk_range_get_value ((GtkRange *) l_mid2high);


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


    /*  If the low slider is at the bottom of it's range, desensitize the low 
        band compressor.  */

    l_low2mid_adj = gtk_range_get_adjustment (range);

    if (value == l_low2mid_adj->lower)
      {
        gtk_widget_set_sensitive (l_low_comp, FALSE);
      }
    else
      {
        gtk_widget_set_sensitive (l_low_comp, TRUE);
      }


    /*  Set the label using log scale.  */

    lvalue = pow (10.0, value);
    sprintf (label, "%05d", NINT (lvalue));
    gtk_label_set_label (l_low2mid_lbl, label);


    /* Write value into DSP code */

    xover_fa = lvalue;


    /*  Set the compressor labels.  */

    hvalue = pow (10.0, other_value);
    sprintf (label, "Mid : %d - %d", NINT (lvalue), NINT (hvalue));
    gtk_label_set_label (l_mid_comp_lbl, label);

    lvalue = pow (10.0, l_low2mid_adj->lower);
    mvalue = pow (10.0, value);
    sprintf (label, "Low : %d - %d", NINT (lvalue), NINT (mvalue));
    gtk_label_set_label (l_low_comp_lbl, label);
}


void
on_mid2high_value_changed              (GtkRange        *range,
                                        gpointer         user_data)
{
    double          value, other_value, lvalue, mvalue, hvalue;
    char            label[6];


    value = gtk_range_get_value (range);
    other_value = gtk_range_get_value ((GtkRange *) l_low2mid);


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


    if (value == l_low2mid_adj->upper)
      {
        gtk_widget_set_sensitive (l_high_comp, FALSE);
      }
    else
      {
        gtk_widget_set_sensitive (l_high_comp, TRUE);
      }


    /*  Set the label using log scale.  */

    mvalue = pow (10.0, value);
    sprintf (label, "%05d", NINT (mvalue));
    gtk_label_set_label (l_mid2high_lbl, label);


    /* Write value into DSP code */

    xover_fb = mvalue;


    /*  Set the compressor labels.  */

    lvalue = pow (10.0, other_value);
    sprintf (label, "Mid : %d - %d", NINT (lvalue), NINT (mvalue));
    gtk_label_set_label (l_mid_comp_lbl, label);

    hvalue = pow (10.0, l_low2mid_adj->upper);
    sprintf (label, "High : %d - %d", NINT (mvalue), NINT (hvalue));
    gtk_label_set_label (l_high_comp_lbl, label);
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
on_low2mid_lbl_realize                 (GtkWidget       *widget,
                                        gpointer         user_data)
{
    l_low2mid_lbl = (GtkLabel *) widget;
}


void
on_mid2high_lbl_realize                (GtkWidget       *widget,
                                        gpointer         user_data)
{
    l_mid2high_lbl = (GtkLabel *) widget;
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


void
on_in_trim_scale_value_changed         (GtkRange        *range,
                                        gpointer         user_data)
{
  in_trim_gain = powf(10.0f, gtk_range_get_adjustment(range)->value * 0.05f);
}

void
on_label_Low_realize                   (GtkWidget       *widget,
                                        gpointer         user_data)
{
    l_low_comp_lbl = (GtkLabel *) widget;
}


void
on_label_Mid_realize                   (GtkWidget       *widget,
                                        gpointer         user_data)
{
    l_mid_comp_lbl = (GtkLabel *) widget;
}


void
on_label_High_realize                  (GtkWidget       *widget,
                                        gpointer         user_data)
{
    l_high_comp_lbl = (GtkLabel *) widget;
}


void
on_window1_show                        (GtkWidget       *widget,
                                        gpointer         user_data)
{
    on_low2mid_value_changed ((GtkRange *) l_low2mid, NULL);

    on_mid2high_value_changed ((GtkRange *) l_mid2high, NULL);
}


gboolean
on_EQ_curve_configure_event            (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data)
{ 
    return FALSE;
}


gboolean
on_EQ_curve_expose_event               (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
    int            i, x0 = 0, y0 = 0, x1, y1;



    l_low2mid_adj = gtk_range_get_adjustment ((GtkRange *) l_low2mid);
    l_eqb1_adj = gtk_range_get_adjustment ((GtkRange *) l_eqb1);
    EQ_curve_range_x = l_low2mid_adj->upper - l_low2mid_adj->lower;
    EQ_curve_range_y = l_eqb1_adj->upper - l_eqb1_adj->lower;
    EQ_curve_width = widget->allocation.width;
    EQ_curve_height = widget->allocation.height;

    for (i = 0 ; i < EQ_BANDS ; i++)
      {
        x1 = NINT (((log10 (geq_freqs[i]) - l_low2mid_adj->lower) / 
            EQ_curve_range_x) * EQ_curve_width);

        y1 = EQ_curve_height - NINT (((((log10 (geq_gains[i])) / 
            0.05) - l_eqb1_adj->lower) / EQ_curve_range_y) * 
            EQ_curve_height);


        gdk_draw_line (widget->window, norm_gc, x1, 0, x1, 
            EQ_curve_height);


        if (i) gdk_draw_line (widget->window, norm_gc, x0, y0, x1, y1);

        x0 = x1;
        y0 = y1;
      }


    return FALSE;
}


void
on_EQ_curve_realize                    (GtkWidget       *widget,
                                        gpointer         user_data)
{
    l_EQ_curve = (GtkDrawingArea *) widget;

    norm_gc = widget->style->fg_gc[GTK_WIDGET_STATE (widget)];
}


void
on_EQ_curve_lbl_realize                (GtkWidget       *widget,
                                        gpointer         user_data)
{
    l_EQ_curve_lbl = (GtkLabel *) widget;
}


void
on_eqb1_realize                        (GtkWidget       *widget,
                                        gpointer         user_data)
{
    l_eqb1 = (GtkVScale *) widget;
}


gboolean
on_EQ_curve_event_box_motion_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data)
{
    static int     prev_x = -1, prev_y = -1;
    int            x, y;
    float          freq, gain;
    char           coords[20];


    x = NINT (event->x);
    y = NINT (event->y);

    if (x != prev_x || y != prev_y)
      {
        
        freq = pow (10.0, (l_low2mid_adj->lower + (((double) x / 
            (double) EQ_curve_width) * EQ_curve_range_x)));


        gain = ((((double) EQ_curve_height - (double) y) / 
            (double) EQ_curve_height) * EQ_curve_range_y) + 
            l_eqb1_adj->lower;

        sprintf (coords, "%dHz , %ddb", NINT (freq), NINT (gain));
        gtk_label_set_text (l_EQ_curve_lbl, coords);

        prev_x = x;
        prev_y = y;
      }

    return FALSE;
}

