#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "geq.h"
#include "interface.h"
#include "support.h"
#include "process.h"
#include "intrim.h"

#define NINT(a) ((a)<0.0 ? (int) ((a) - 0.5) : (int) ((a) + 0.5))
#define EQ_INTERP (BINS / 2 - 1)
#define EQ_INTRVL (EQ_INTERP / (EQ_BANDS + 1))

/* vi:set ts=8 sts=4 sw=4: */

void interpolate (float, int, float, float, int *, float *, float *, float *, 
                  float *);

static GtkHScale       *l_low2mid, *l_mid2high;
static GtkVScale       *l_eqb1;
static GtkWidget       *l_low_comp, *l_mid_comp, *l_high_comp;
static GtkLabel        *l_low2mid_lbl, *l_mid2high_lbl, *l_low_comp_lbl, 
                       *l_mid_comp_lbl, *l_high_comp_lbl, *l_EQ_curve_lbl,
                       *l_low_knee_lbl, *l_mid_knee_lbl, *l_high_knee_lbl;
static GtkDrawingArea  *l_EQ_curve;
static GdkDrawable     *EQ_drawable;
static GdkColormap     *colormap;
static GdkColor        white, black, red, green, blue;
static GdkGC           *norm_gc;
static GtkAdjustment   *l_low2mid_adj, *l_eqb1_adj;
static float           EQ_curve_range_x, EQ_curve_range_y, EQ_curve_width,
                       EQ_curve_height, EQ_xinterp[EQ_INTERP + 1], EQ_start, 
                       EQ_end, EQ_interval, EQ_yinterp[EQ_INTERP + 1], 
                       *EQ_xinput = NULL, *EQ_yinput = NULL, 
                       l_geq_freqs[EQ_BANDS], l_geq_gains[EQ_BANDS];
static int             EQ_mod = 1, EQ_drawing = 0, EQ_input_points = 0, 
                       EQ_length = 0;


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
clean_quit ()
{
    if (EQ_xinput) free (EQ_xinput);
    if (EQ_yinput) free (EQ_yinput);

    gtk_main_quit();
}


void
on_quit_clicked                        (GtkButton       *button,
                                        gpointer         user_data)
{
    clean_quit ();
}


gboolean
on_window1_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
    clean_quit ();

    return FALSE;
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

    colormap = gdk_colormap_get_system ();

    white.red = 65535;
    white.green = 65535;
    white.blue = 65535;

    gdk_colormap_alloc_color (colormap, &white, FALSE, TRUE);

    black.red = 0;
    black.green = 0;
    black.blue = 0;

    gdk_colormap_alloc_color (colormap, &black, FALSE, TRUE);

    red.red = 65535;
    red.green = 0;
    red.blue = 0;

    gdk_colormap_alloc_color (colormap, &red, FALSE, TRUE);

    green.red = 0;
    green.green = 65535;
    green.blue = 0;

    gdk_colormap_alloc_color (colormap, &green, FALSE, TRUE);

    blue.red = 0;
    blue.green = 0;
    blue.blue = 0;

    gdk_colormap_alloc_color (colormap, &blue, FALSE, TRUE);
}


gboolean
on_EQ_curve_configure_event            (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data)
{ 
    return FALSE;
}


gboolean
eqb_mod                                (GtkAdjustment *adj, gpointer user_data)
{
    EQ_mod = 1;

    return FALSE;
}


void
draw_EQ_curve ()
{
    int            i, x0 = 0, y0 = 0, x1, y1, inc;
    float          x[EQ_BANDS], y[EQ_BANDS];


    /*  Clear the curve drawing area.  */

    gdk_gc_set_foreground (norm_gc, &white);
    gdk_draw_rectangle (EQ_drawable, norm_gc, TRUE, 0, 0, EQ_curve_width, 
        EQ_curve_height);
    gdk_gc_set_foreground (norm_gc, &black);


    /*  Draw the grid lines.  */

    geq_get_freqs_and_gains (l_geq_freqs, l_geq_gains);

    for (i = 0 ; i < EQ_BANDS ; i++)
      {
        x[i] = log10 (l_geq_freqs[i]);
        y[i] = log10 (l_geq_gains[i]);

        x1 = NINT (((x[i] - l_low2mid_adj->lower) / EQ_curve_range_x) * 
            EQ_curve_width);

        gdk_draw_line (EQ_drawable, norm_gc, x1, 0, x1, EQ_curve_height);
      }

    inc = 10;
    if (EQ_curve_range_y < 10.0) inc = 1;
    for (i = NINT (l_eqb1_adj->lower) ; i < NINT (l_eqb1_adj->upper) ; i++)
      {
        if (!(i % inc))
          {
            y1 = EQ_curve_height - NINT (((i - l_eqb1_adj->lower) / 
                EQ_curve_range_y) * EQ_curve_height);

            gdk_draw_line (EQ_drawable, norm_gc, 0, y1, EQ_curve_width, y1);
          }
      }


    /*  If we've messed with the graphics EQ sliders, recompute the splined 
        curve.  */

    if (EQ_mod) interpolate (EQ_interval, EQ_BANDS, EQ_start, EQ_end, 
        &EQ_length, x, y, EQ_xinterp, EQ_yinterp);


    /*  Plot the curve.  */

    gdk_gc_set_foreground (norm_gc, &red);
    gdk_gc_set_line_attributes (norm_gc, 2, GDK_LINE_SOLID, GDK_CAP_BUTT,
        GDK_JOIN_MITER);
    for (i = 0 ; i < EQ_length ; i++)
      {
        x1 = NINT (((EQ_xinterp[i] - l_low2mid_adj->lower) / 
            EQ_curve_range_x) * EQ_curve_width);

        y1 = EQ_curve_height - NINT ((((EQ_yinterp[i] / 
            0.05) - l_eqb1_adj->lower) / EQ_curve_range_y) * 
            EQ_curve_height);

        if (i) gdk_draw_line (EQ_drawable, norm_gc, x0, y0, x1, y1);

        x0 = x1;
        y0 = y1;
      }
    gdk_gc_set_line_attributes (norm_gc, 1, GDK_LINE_SOLID, GDK_CAP_BUTT,
        GDK_JOIN_MITER);
    gdk_gc_set_foreground (norm_gc, &black);

    EQ_mod = 0;
}


gboolean
on_EQ_curve_expose_event               (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
    l_low2mid_adj = gtk_range_get_adjustment ((GtkRange *) l_low2mid);
    l_eqb1_adj = gtk_range_get_adjustment ((GtkRange *) l_eqb1);
    EQ_curve_range_x = l_low2mid_adj->upper - l_low2mid_adj->lower;
    EQ_curve_range_y = l_eqb1_adj->upper - l_eqb1_adj->lower;


    /*  Since allocation width and height are inclusive we need to decrement
        for calculations.  */

    EQ_curve_width = widget->allocation.width - 1;
    EQ_curve_height = widget->allocation.height - 1;


    draw_EQ_curve ();

    return FALSE;
}


void
on_EQ_curve_realize                    (GtkWidget       *widget,
                                        gpointer         user_data)
{
    l_EQ_curve = (GtkDrawingArea *) widget;

    EQ_drawable = widget->window;

    norm_gc = widget->style->fg_gc[GTK_WIDGET_STATE (widget)];

    geq_get_freqs_and_gains (l_geq_freqs, l_geq_gains);

    EQ_start = log10 (l_geq_freqs[0]);
    EQ_end = log10 (l_geq_freqs[EQ_BANDS - 1]);
    EQ_interval = (EQ_end - EQ_start) / EQ_INTERP;
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
    int            x, y, size;
    float          freq, gain;
    char           coords[20];


    x = NINT (event->x);
    y = NINT (event->y);


    /*  We only want to update things if we've actually moved the cursor.  */

    if (x != prev_x || y != prev_y)
      {
        freq = pow (10.0, (l_low2mid_adj->lower + (((double) x / 
            (double) EQ_curve_width) * EQ_curve_range_x)));


        gain = ((((double) EQ_curve_height - (double) y) / 
            (double) EQ_curve_height) * EQ_curve_range_y) + 
            l_eqb1_adj->lower;

        sprintf (coords, "%dHz , %ddb", NINT (freq), NINT (gain));
        gtk_label_set_text (l_EQ_curve_lbl, coords);


        /*  If we're in the midst of drawing the curve...  */

        if (EQ_drawing)
          {
            /*  Only allow the user to draw in the positive direction.  */

            if (!EQ_input_points || x > EQ_xinput[EQ_input_points - 1])
              {
                if (EQ_input_points) gdk_draw_line (EQ_drawable, norm_gc, 
                    NINT (EQ_xinput[EQ_input_points - 1]), 
                    NINT (EQ_yinput[EQ_input_points - 1]), x, y);

                size = (EQ_input_points + 1) * sizeof (float);
                EQ_xinput = (float *) realloc (EQ_xinput, size);
                EQ_yinput = (float *) realloc (EQ_yinput, size);

                if (EQ_yinput == NULL)
                  {
                    perror ("Allocating EQ_yinput in callbacks.c");
                    clean_quit ();
                  }

                EQ_xinput[EQ_input_points] = x;
                EQ_yinput[EQ_input_points] = y;
                EQ_input_points++;
              }
          }

        prev_x = x;
        prev_y = y;

      }

    return FALSE;
}


gboolean
on_EQ_curve_event_box_button_press_event
                                        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
    switch (event->button)
      {

        /*  Button 1 - start drawing.  */

      case 1:
        EQ_drawing = 1;
        break;

      default:
        break;
      }

    return FALSE;
}


gboolean
on_EQ_curve_event_box_button_release_event
                                        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
    float               *x = NULL, *y = NULL;
    int                 i, j, i_start = 0, i_end = 0, size;


    switch (event->button)
      {

        /*  Button 2 - discard the drawn curve.  */

      case 2:
        EQ_drawing = 0;

        EQ_input_points = 0;

        draw_EQ_curve ();
        
        break;


        /*  Button 3 - combine the drawn data with any parts of the previous 
            that haven't been superceded by what was drawn.  Use an EQ_INTRVL 
            cushion on either side of the drawn section so it will merge 
            nicely with the old data.  */

      case 3:
        EQ_drawing = 0;


        /*  Convert the x and y input positions to "real" values.  */

        for (i = 0 ; i < EQ_input_points ; i++)
          {
            EQ_xinput[i] = l_low2mid_adj->lower + (((double) EQ_xinput[i] / 
                 (double) EQ_curve_width) * EQ_curve_range_x);


            EQ_yinput[i] = (((((double) EQ_curve_height - 
                (double) EQ_yinput[i]) / (double) EQ_curve_height) * 
                EQ_curve_range_y) + l_eqb1_adj->lower) * 0.05;
          }


        /*  Merge the drawn section with the old curve.  */

        for (i = 0 ; i < EQ_length ; i++)
          {
            if (EQ_xinterp[i] >= EQ_xinput[0])
              {
                i_start = i - EQ_INTRVL;
                break;
              }
          }

        for (i = EQ_length - 1 ; i >= 0 ; i--)
          {
            if (EQ_xinterp[i] <= EQ_xinput[EQ_input_points - 1])
              {
                i_end = i + EQ_INTRVL;
                break;
              }
          }


        j = 0;
        for (i = 0 ; i < i_start ; i++)
          {
            size = (j + 1) * sizeof (float);
            x = (float *) realloc (x, size);
            y = (float *) realloc (y, size);

            if (y == NULL)
              {
                perror ("Allocating y in callbacks.c");
                clean_quit ();
              }

            x[j] = EQ_xinterp[i];
            y[j] = EQ_yinterp[i];
            j++;
          }

        for (i = 0 ; i < EQ_input_points ; i++)
          {
            size = (j + 1) * sizeof (float);
            x = (float *) realloc (x, size);
            y = (float *) realloc (y, size);

            if (y == NULL)
              {
                perror ("Allocating y in callbacks.c");
                clean_quit ();
              }

            x[j] = EQ_xinput[i];
            y[j] = EQ_yinput[i];
            j++;
          }

        for (i = i_end ; i < EQ_length ; i++)
          {
            size = (j + 1) * sizeof (float);
            x = (float *) realloc (x, size);
            y = (float *) realloc (y, size);

            x[j] = EQ_xinterp[i];
            y[j] = EQ_yinterp[i];
            j++;
          }


        /*  Recompute the splined curve.  */

        interpolate (EQ_interval, j, EQ_start, EQ_end, &EQ_length, x, 
            y, EQ_xinterp, EQ_yinterp);


        if (x) free (x);
        if (y) free (y);


        EQ_input_points = 0;


        /*  Set the graphic EQ sliders and the EQ settings based on the 
            hand-drawn curve.  */

        geq_set_sliders (EQ_length, EQ_xinterp, EQ_yinterp);

        EQ_mod = 0;


        /*  Redraw the curve.  */

        draw_EQ_curve ();

        break;
      }

    return FALSE;
}


void
on_bypass_button_toggled               (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	global_bypass = gtk_toggle_button_get_active(togglebutton);
}




void
on_low_knee_lbl_realize                (GtkWidget       *widget,
                                        gpointer         user_data)
{
    l_low_knee_lbl = (GtkLabel *) widget;
}


void
on_comp_kn_1_value_changed             (GtkRange        *range,
                                        gpointer         user_data)
{
    double              value;


    /*  Make sure it exists before we set it.  */

    if (l_low_knee_lbl == NULL) return;


    value = gtk_range_get_value (range);

    if (value == 0.5)
      {
        gtk_label_set_label (l_low_knee_lbl, "Knee");
      }
    else if (value < 0.5)
      {
        gtk_label_set_label (l_low_knee_lbl, "Hard");
      }
    else
      {
        gtk_label_set_label (l_low_knee_lbl, "Soft");
      }
}


void
on_mid_knee_lbl_realize                (GtkWidget       *widget,
                                        gpointer         user_data)
{
    l_mid_knee_lbl = (GtkLabel *) widget;
}


void
on_comp_kn_2_value_changed             (GtkRange        *range,
                                        gpointer         user_data)
{
    double              value;


    /*  Make sure it exists before we set it.  */

    if (l_mid_knee_lbl == NULL) return;


    value = gtk_range_get_value (range);

    if (value == 0.5)
      {
        gtk_label_set_label (l_mid_knee_lbl, "Knee");
      }
    else if (value < 0.5)
      {
        gtk_label_set_label (l_mid_knee_lbl, "Hard");
      }
    else
      {
        gtk_label_set_label (l_mid_knee_lbl, "Soft");
      }
}


void
on_high_knee_lbl_realize               (GtkWidget       *widget,
                                        gpointer         user_data)
{
    l_high_knee_lbl = (GtkLabel *) widget;
}


void
on_comp_kn_3_value_changed             (GtkRange        *range,
                                        gpointer         user_data)
{
    double              value;


    /*  Make sure it exists before we set it.  */

    if (l_high_knee_lbl == NULL) return;


    value = gtk_range_get_value (range);

    if (value == 0.5)
      {
        gtk_label_set_label (l_high_knee_lbl, "Knee");
      }
    else if (value < 0.5)
      {
        gtk_label_set_label (l_high_knee_lbl, "Hard");
      }
    else
      {
        gtk_label_set_label (l_high_knee_lbl, "Soft");
      }
}


void
on_geq_min_gain_spinner_value_changed  (GtkSpinButton   *spinbutton,
                                        gpointer         user_data)
{
    geq_set_range (gtk_spin_button_get_value (spinbutton), geqa[0]->upper);
}


void
on_geq_max_gain_spinner_value_changed  (GtkSpinButton   *spinbutton,
                                        gpointer         user_data)
{
    geq_set_range (geqa[0]->lower, gtk_spin_button_get_value (spinbutton));
}


void
on_out_trim_scale_value_changed        (GtkRange        *range,
                                        gpointer         user_data)
{
    limiter.attenuation = gtk_range_get_adjustment(range)->value;
}


void
on_in_trim_scale_value_changed         (GtkRange        *range,
                                        gpointer         user_data)
{
    in_trim_gain = powf(10.0f, gtk_range_get_adjustment(range)->value * 0.05f);
    in_gain[0] = in_trim_gain * in_pan_gain[0];
    in_gain[1] = in_trim_gain * in_pan_gain[1];
}

void
on_pan_scale_value_changed             (GtkRange        *range,
                                        gpointer         user_data)
{
    const float balance = gtk_range_get_adjustment(range)->value;

    in_pan_gain[0] = powf(10.0f, balance * -0.025f);
    in_pan_gain[1] = powf(10.0f, balance * 0.025f);
    in_gain[0] = in_trim_gain * in_pan_gain[0];
    in_gain[1] = in_trim_gain * in_pan_gain[1];
    update_pan_label(balance);
}
