#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "geq.h"
#include "interface.h"
#include "support.h"
#include "process.h"
#include "intrim.h"
#include "compressor-ui.h"
#include "gtkmeter.h"
#include "gtkmeterscale.h"
#include "state.h"
#include "db.h"

#define NINT(a) ((a)<0.0 ? (int) ((a) - 0.5) : (int) ((a) + 0.5))

#define EQ_INTERP                     (BINS / 2 - 1)
#define EQ_SPECTRUM_RANGE             90.0
#define XOVER_HANDLE_SIZE             10
#define XOVER_HANDLE_HALF_SIZE        (XOVER_HANDLE_SIZE / 2)
#define NOTCH_HANDLE_HEIGHT           14
#define NOTCH_HANDLE_HALF_HEIGHT      (NOTCH_HANDLE_HEIGHT / 2)
#define NOTCH_HANDLE_WIDTH            6
#define NOTCH_HANDLE_HALF_WIDTH       (NOTCH_HANDLE_WIDTH / 2)
#define NOTCHES                       5
#define NOTCH_INT                     ((int) ((EQ_INTERP + 1) / (NOTCHES - 1)))


/* vi:set ts=8 sts=4 sw=4: */

void interpolate (float, int, float, float, int *, float *, float *, float *, 
                  float *);
void draw_EQ_curve ();

static GtkHScale       *l_low2mid, *l_mid2high;
static GtkVScale       *l_eqb1;
static GtkWidget       *l_low_comp, *l_mid_comp, *l_high_comp;
static GtkLabel        *l_low2mid_lbl, *l_mid2high_lbl, *l_low_comp_lbl, 
                       *l_mid_comp_lbl, *l_high_comp_lbl, *l_EQ_curve_lbl,
                       *l_low_knee_lbl, *l_mid_knee_lbl, *l_high_knee_lbl,
                       *l_low_curve_lbl, *l_mid_curve_lbl, *l_high_curve_lbl;
static GtkDrawingArea  *l_EQ_curve, *l_comp_curve[3];
static GtkNotebook     *notebook1;
static GdkDrawable     *EQ_drawable, *comp_drawable[3];
static GdkColormap     *colormap;
static GdkColor        white, grey, black, red, green, blue, comp_color[4];
static GdkGC           *EQ_gc, *comp_gc[3];
static PangoContext    *comp_pc[3], *EQ_pc;
static GtkAdjustment   *l_low2mid_adj, *l_eqb1_adj;
static float           EQ_curve_range_x, EQ_curve_range_y, EQ_curve_width,
                       EQ_curve_height, EQ_xinterp[EQ_INTERP + 1], EQ_start, 
                       EQ_end, EQ_interval, EQ_yinterp[EQ_INTERP + 1], 
                       *EQ_xinput = NULL, *EQ_yinput = NULL, 
                       l_geq_freqs[EQ_BANDS], l_geq_gains[EQ_BANDS], 
                       comp_curve_range_x[3], comp_curve_range_y[3], 
                       comp_curve_width[3], comp_curve_height[3] , 
                       comp_start_x[3], comp_start_y[3], comp_end_x[3], 
                       comp_end_y[3], EQ_freq_xinterp[EQ_INTERP + 1],
                       EQ_freq_yinterp[EQ_INTERP + 1], 
                       EQ_notch_gain[NOTCHES] = {0.0, 0.0, 0.0, 0.0, 0.0};
static int             EQ_mod = 1, EQ_drawing = 0, EQ_input_points = 0, 
                       EQ_length = 0, comp_realized[3] = {0, 0, 0}, 
                       EQ_cleared = 1, EQ_realized = 0, xover_active = 0,
                       xover_handle_fa, xover_handle_fb, EQ_drag_fa = 0,
                       EQ_drag_fb = 0, 
                       EQ_notch_drag[NOTCHES] = {0, 0, 0, 0, 0}, 
                       EQ_notch_Q_drag[NOTCHES] = {0, 0, 0, 0, 0},
                       EQ_notch_handle[2][3][NOTCHES], 
                       EQ_notch_width[NOTCHES] = {0, 5, 5, 5, 0},
                       EQ_notch_index[NOTCHES] = {20, NOTCH_INT, 2 * NOTCH_INT,
                       3 * NOTCH_INT, EQ_INTERP - 20};


void
on_low2mid_value_changed               (GtkRange        *range,
                                        gpointer         user_data)
{
    double          value, other_value,lvalue, mvalue, hvalue;
    char            label[6];


    value = gtk_range_get_value (range);
    other_value = gtk_range_get_value ((GtkRange *) l_mid2high);
    s_set_value_ui(S_XOVER_FREQ(0), value);


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
    sprintf (label, _("Mid : %d - %d"), NINT (lvalue), NINT (hvalue));
    gtk_label_set_label (l_mid_comp_lbl, label);

    lvalue = pow (10.0, l_low2mid_adj->lower);
    mvalue = pow (10.0, value);
    sprintf (label, _("Low : %d - %d"), NINT (lvalue), NINT (mvalue));
    gtk_label_set_label (l_low_comp_lbl, label);

    draw_EQ_curve ();
}


void
on_mid2high_value_changed              (GtkRange        *range,
                                        gpointer         user_data)
{
    double          value, other_value, lvalue, mvalue, hvalue;
    char            label[6];


    value = gtk_range_get_value (range);
    other_value = gtk_range_get_value ((GtkRange *) l_low2mid);
    s_set_value_ui(S_XOVER_FREQ(1), value);


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
    sprintf (label, _("Mid : %d - %d"), NINT (lvalue), NINT (mvalue));
    gtk_label_set_label (l_mid_comp_lbl, label);

    hvalue = pow (10.0, l_low2mid_adj->upper);
    sprintf (label, _("High : %d - %d"), NINT (mvalue), NINT (hvalue));
    gtk_label_set_label (l_high_comp_lbl, label);

    draw_EQ_curve ();
}


gboolean
on_low2mid_button_press_event          (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
    xover_active = 1;

    return FALSE;
}


gboolean
on_low2mid_button_release_event        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
    xover_active = 0;

    return FALSE;
}


gboolean
on_mid2high2_button_press_event        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
    xover_active = 1;

    return FALSE;
}


gboolean
on_mid2high2_button_release_event      (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
    xover_active = 0;

    return FALSE;
}


void
on_low2mid_realize                     (GtkWidget       *widget,
                                        gpointer         user_data)
{
    l_low2mid = (GtkHScale *) widget;
    s_set_adjustment(S_XOVER_FREQ(0), gtk_range_get_adjustment(GTK_RANGE(widget)));
}


void
on_mid2high_realize                    (GtkWidget       *widget,
                                        gpointer         user_data)
{
    l_mid2high = (GtkHScale *) widget;
    s_set_adjustment(S_XOVER_FREQ(1), gtk_range_get_adjustment(GTK_RANGE(widget)));
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

    grey.red = 40000;
    grey.green = 40000;
    grey.blue = 40000;

    gdk_colormap_alloc_color (colormap, &grey, FALSE, TRUE);

    red.red = 60000;
    red.green = 0;
    red.blue = 0;

    gdk_colormap_alloc_color (colormap, &red, FALSE, TRUE);
    comp_color[0] = red;

    green.red = 0;
    green.green = 50000;
    green.blue = 0;

    gdk_colormap_alloc_color (colormap, &green, FALSE, TRUE);
    comp_color[1] = green;

    blue.red = 0;
    blue.green = 0;
    blue.blue = 60000;

    gdk_colormap_alloc_color (colormap, &blue, FALSE, TRUE);
    comp_color[2] = blue;

    comp_color[3] = black;
}


gboolean
eqb_mod                                (GtkAdjustment *adj, gpointer user_data)
{
    EQ_mod = 1;

    return FALSE;
}


void
draw_EQ_spectrum_curve (float single_levels[])
{
    static int     x[EQ_INTERP], y[EQ_INTERP];
    int            i;
    float          step, range, freq;


    /*  Don't update if we're drawing an EQ curve.  */

    if (!EQ_drawing && !xover_active)
      {
        /*  Plot the curve.  */

        gdk_gc_set_foreground (EQ_gc, &grey);
        gdk_gc_set_function (EQ_gc, GDK_XOR);
        gdk_gc_set_line_attributes (EQ_gc, 1, GDK_LINE_SOLID, GDK_CAP_BUTT,
                                    GDK_JOIN_MITER);


        /*  If we've just cleared (redrawn) the curve, don't erase the previous
            line.  */

        if (!EQ_cleared)
          {
            for (i = 1 ; i < EQ_INTERP ; i++)
              {
                gdk_draw_line (EQ_drawable, EQ_gc, x[i - 1], y[i - 1], 
                    x[i], y[i]);
              }
          }


        /*  Convert the single levels to db, plot, and save the pixel positions
            so that we can erase them on the next pass.  */

        range = l_geq_freqs[EQ_BANDS - 1] - l_geq_freqs[0];
        step = range / (float) EQ_INTERP;

        for (i = 0 ; i < EQ_INTERP ; i++)
          {
            freq = l_geq_freqs[0] + (float) i * step;


            x[i] = NINT (((log10(freq) - l_low2mid_adj->lower) /
                          EQ_curve_range_x) * EQ_curve_width);


            /*  Most of the single_level values will be in the -90.0db to 
                -20.0db range.  We're using -90.0db to 0.0db as our range.  */

            y[i] = NINT (-(lin2db(single_levels[i]) / EQ_SPECTRUM_RANGE) * 
                EQ_curve_height);

            if (i) gdk_draw_line (EQ_drawable, EQ_gc, x[i - 1], y[i - 1], 
                                  x[i], y[i]);
          }

        gdk_gc_set_line_attributes (EQ_gc, 1, GDK_LINE_SOLID, GDK_CAP_BUTT,
                                    GDK_JOIN_MITER);
        gdk_gc_set_foreground (EQ_gc, &black);
        gdk_gc_set_function (EQ_gc, GDK_COPY);

        EQ_cleared = 0;
      }
}


int 
nearest_x (float freq)
{
    int          i, j = 0;
    float        dist, ndist;


    dist = 99999999.0;

    for (i = 0 ; i < EQ_length ; i++)
      {
        ndist = log10f (freq);
        if (fabs (ndist - EQ_xinterp[i]) < dist)
          {
            dist = fabs (ndist - EQ_xinterp[i]);
            j = i;
          }
      }

  return (j);
}


void EQ_write_annotation (int x, int y, float db)
{
    PangoLayout    *pl;
    PangoRectangle ink_rect;
    char           string[6];
    int            half_width, y_offset;


    /*  Clear the annotation area.  */

    pl = pango_layout_new (EQ_pc);  
    pango_layout_set_text (pl, "+00db", -1);
    pango_layout_get_pixel_extents (pl, &ink_rect, NULL);

    half_width = ink_rect.width / 2;

    if (y < EQ_curve_height / 2)
      {
        y_offset = ink_rect.height;
      }
    else
      {
        y_offset = -(2 * ink_rect.height);
      }

    sprintf (string, "%+03ddb", NINT (db));
    gdk_window_clear_area (EQ_drawable, x - half_width, y + y_offset, 
        ink_rect.width + 2, ink_rect.height + 2);
    gdk_gc_set_foreground (EQ_gc, &black);

    pl = pango_layout_new (EQ_pc);  
    pango_layout_set_text (pl, string, -1);


    gdk_draw_layout (EQ_drawable, EQ_gc, x - half_width, y + y_offset, pl);
}


void
draw_EQ_curve ()
{
    int            i, x0 = 0, y0 = 0, x1, y1, inc;
    float          x[EQ_BANDS], y[EQ_BANDS];


    if (!EQ_realized) return;


    /*  Clear the curve drawing area.  */

    EQ_cleared = 1;
    gdk_window_clear_area (EQ_drawable, 0, 0, EQ_curve_width + 1, 
        EQ_curve_height + 1);
    gdk_gc_set_foreground (EQ_gc, &black);


    /*  Draw the grid lines.  */

    geq_get_freqs_and_gains (l_geq_freqs, l_geq_gains);

    for (i = 0 ; i < EQ_BANDS ; i++)
      {
        x[i] = log10 (l_geq_freqs[i]);
        y[i] = log10 (l_geq_gains[i]);

        x1 = NINT (((x[i] - l_low2mid_adj->lower) / EQ_curve_range_x) * 
            EQ_curve_width);

        gdk_draw_line (EQ_drawable, EQ_gc, x1, 0, x1, EQ_curve_height);
      }

    inc = 10;
    if (EQ_curve_range_y < 10.0) inc = 1;
    for (i = NINT (l_eqb1_adj->lower) ; i < NINT (l_eqb1_adj->upper) ; i++)
      {
        if (!(i % inc))
          {
            y1 = EQ_curve_height - NINT (((i - l_eqb1_adj->lower) / 
                EQ_curve_range_y) * EQ_curve_height);

            gdk_draw_line (EQ_drawable, EQ_gc, 0, y1, EQ_curve_width, y1);
          }
      }


    /*  Add the crossover bars.  */

    gdk_gc_set_line_attributes (EQ_gc, 2, GDK_LINE_SOLID, GDK_CAP_BUTT,
        GDK_JOIN_MITER);

    gdk_gc_set_foreground (EQ_gc, &red);
    x1 = NINT (((log10 (xover_fa) - l_low2mid_adj->lower) / 
        EQ_curve_range_x) * EQ_curve_width);
    gdk_draw_line (EQ_drawable, EQ_gc, x1, 0, x1, EQ_curve_height);
    gdk_draw_rectangle (EQ_drawable, EQ_gc, TRUE, x1 - XOVER_HANDLE_HALF_SIZE, 0, 
        XOVER_HANDLE_SIZE, XOVER_HANDLE_SIZE);
    gdk_draw_rectangle (EQ_drawable, EQ_gc, TRUE, x1 - XOVER_HANDLE_HALF_SIZE, 
        EQ_curve_height - XOVER_HANDLE_SIZE, XOVER_HANDLE_SIZE, XOVER_HANDLE_SIZE);
    gdk_gc_set_foreground (EQ_gc, &black);
    gdk_draw_rectangle (EQ_drawable, EQ_gc, FALSE, x1 - XOVER_HANDLE_HALF_SIZE, 0, 
        XOVER_HANDLE_SIZE, XOVER_HANDLE_SIZE);
    gdk_draw_rectangle (EQ_drawable, EQ_gc, FALSE, x1 - XOVER_HANDLE_HALF_SIZE, 
        EQ_curve_height - XOVER_HANDLE_SIZE, XOVER_HANDLE_SIZE, XOVER_HANDLE_SIZE);

    xover_handle_fa = x1;


    gdk_gc_set_foreground (EQ_gc, &blue);
    x1 = NINT (((log10 (xover_fb) - l_low2mid_adj->lower) / 
        EQ_curve_range_x) * EQ_curve_width);
    gdk_draw_line (EQ_drawable, EQ_gc, x1, 0, x1, EQ_curve_height);
    gdk_draw_rectangle (EQ_drawable, EQ_gc, TRUE, x1 - XOVER_HANDLE_HALF_SIZE, 0, 
        XOVER_HANDLE_SIZE, XOVER_HANDLE_SIZE);
    gdk_draw_rectangle (EQ_drawable, EQ_gc, TRUE, x1 - XOVER_HANDLE_HALF_SIZE, 
        EQ_curve_height - XOVER_HANDLE_SIZE, XOVER_HANDLE_SIZE, XOVER_HANDLE_SIZE);
    gdk_gc_set_foreground (EQ_gc, &black);
    gdk_draw_rectangle (EQ_drawable, EQ_gc, FALSE, x1 - XOVER_HANDLE_HALF_SIZE, 0, 
        XOVER_HANDLE_SIZE, XOVER_HANDLE_SIZE);
    gdk_draw_rectangle (EQ_drawable, EQ_gc, FALSE, x1 - XOVER_HANDLE_HALF_SIZE, 
        EQ_curve_height - XOVER_HANDLE_SIZE, XOVER_HANDLE_SIZE, XOVER_HANDLE_SIZE);

    xover_handle_fb = x1;

    
    /*  If we've messed with the graphics EQ sliders, recompute the splined 
        curve.  */

    if (EQ_mod) interpolate (EQ_interval, EQ_BANDS, EQ_start, EQ_end, 
        &EQ_length, x, y, EQ_xinterp, EQ_yinterp);


    /*  Plot the curve.  */

    gdk_gc_set_foreground (EQ_gc, &red);
    for (i = 0 ; i < EQ_length ; i++)
      {
        x1 = NINT (((EQ_xinterp[i] - l_low2mid_adj->lower) / 
            EQ_curve_range_x) * EQ_curve_width);

        y1 = EQ_curve_height - NINT ((((EQ_yinterp[i] * 20.0) - 
            l_eqb1_adj->lower) / EQ_curve_range_y) * EQ_curve_height);

        if (i) gdk_draw_line (EQ_drawable, EQ_gc, x0, y0, x1, y1);

        x0 = x1;
        y0 = y1;
      }


    /*  Add the notch handles.  */

    for (i = 0 ; i < NOTCHES ; i++)
      {
        gdk_gc_set_foreground (EQ_gc, &green);

        x1 = NINT (((EQ_xinterp[EQ_notch_index[i]] - l_low2mid_adj->lower) / 
            EQ_curve_range_x) * EQ_curve_width); 

        y1 = EQ_curve_height - NINT ((((EQ_yinterp[EQ_notch_index[i]] * 20.0) -
            l_eqb1_adj->lower) / EQ_curve_range_y) * EQ_curve_height);

        gdk_draw_rectangle (EQ_drawable, EQ_gc, TRUE, 
            x1 - NOTCH_HANDLE_HALF_WIDTH, y1 - NOTCH_HANDLE_HALF_HEIGHT, 
            NOTCH_HANDLE_WIDTH, NOTCH_HANDLE_HEIGHT);
        gdk_gc_set_foreground (EQ_gc, &black);
        gdk_draw_rectangle (EQ_drawable, EQ_gc, FALSE, 
            x1 - NOTCH_HANDLE_HALF_WIDTH, y1 - NOTCH_HANDLE_HALF_HEIGHT, 
            NOTCH_HANDLE_WIDTH, NOTCH_HANDLE_HEIGHT);

        EQ_notch_handle[0][1][i] = x1;
        EQ_notch_handle[1][1][i] = y1;


        EQ_write_annotation (x1, y1, EQ_notch_gain[i]);


        if (i && i < NOTCHES - 1)
          {
            gdk_gc_set_foreground (EQ_gc, &green);

            x0 = EQ_notch_index[i] - EQ_notch_width[i];
            
            x1 = NINT (((EQ_xinterp[x0] - l_low2mid_adj->lower) / 
                EQ_curve_range_x) * EQ_curve_width); 

            if (EQ_notch_handle[0][1][i] - x1 < NOTCH_HANDLE_HALF_WIDTH) 
                x1 = EQ_notch_handle[0][1][i] - NOTCH_HANDLE_WIDTH;

            y1 = EQ_curve_height - NINT ((((EQ_yinterp[x0] * 20.0) -
                l_eqb1_adj->lower) / EQ_curve_range_y) * EQ_curve_height);

            gdk_draw_arc (EQ_drawable, EQ_gc, TRUE, 
                x1 - NOTCH_HANDLE_WIDTH, y1 - NOTCH_HANDLE_HALF_HEIGHT, 
                NOTCH_HANDLE_WIDTH * 2, NOTCH_HANDLE_HEIGHT, 5760, 11520);
            gdk_gc_set_foreground (EQ_gc, &black);
            gdk_draw_arc (EQ_drawable, EQ_gc, FALSE, 
                x1 - NOTCH_HANDLE_WIDTH, y1 - NOTCH_HANDLE_HALF_HEIGHT, 
                NOTCH_HANDLE_WIDTH * 2, NOTCH_HANDLE_HEIGHT, 5760, 11520);

            EQ_notch_handle[0][0][i] = x1;
            EQ_notch_handle[1][0][i] = y1;


            gdk_gc_set_foreground (EQ_gc, &green);

            x0 = EQ_notch_index[i] + EQ_notch_width[i];
            
            x1 = NINT (((EQ_xinterp[x0] - l_low2mid_adj->lower) / 
                EQ_curve_range_x) * EQ_curve_width); 

            if (x1 - EQ_notch_handle[0][1][i] < NOTCH_HANDLE_HALF_WIDTH) 
                x1 = EQ_notch_handle[0][1][i] + NOTCH_HANDLE_WIDTH;

            y1 = EQ_curve_height - NINT ((((EQ_yinterp[x0] * 20.0) -
                l_eqb1_adj->lower) / EQ_curve_range_y) * EQ_curve_height);

            gdk_draw_arc (EQ_drawable, EQ_gc, TRUE, 
                x1 - NOTCH_HANDLE_WIDTH, y1 - NOTCH_HANDLE_HALF_HEIGHT, 
                NOTCH_HANDLE_WIDTH * 2, NOTCH_HANDLE_HEIGHT, 17280, 11520);
            gdk_gc_set_foreground (EQ_gc, &black);
            gdk_draw_arc (EQ_drawable, EQ_gc, FALSE, 
                x1 - NOTCH_HANDLE_WIDTH, y1 - NOTCH_HANDLE_HALF_HEIGHT, 
                NOTCH_HANDLE_WIDTH * 2, NOTCH_HANDLE_HEIGHT, 17280, 11520);

            EQ_notch_handle[0][2][i] = x1;
            EQ_notch_handle[1][2][i] = y1;
          }
      }

    gdk_gc_set_line_attributes (EQ_gc, 1, GDK_LINE_SOLID, GDK_CAP_BUTT,
        GDK_JOIN_MITER);


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

    EQ_gc = widget->style->fg_gc[GTK_WIDGET_STATE (widget)];

    EQ_pc = gtk_widget_get_pango_context (widget);

    geq_get_freqs_and_gains (l_geq_freqs, l_geq_gains);

    EQ_start = log10 (l_geq_freqs[0]);
    EQ_end = log10 (l_geq_freqs[EQ_BANDS - 1]);
    EQ_interval = (EQ_end - EQ_start) / EQ_INTERP;

    EQ_realized = 1;
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
    static int     prev_x = -1, prev_y = -1, current_cursor = -1;
    int            i, j, x, y, size, diffx_fa, diffx_fb, diff_notch[2], 
                   cursor, drag, notch_flag = -1, lo, hi;
    float          freq, gain, s_gain;
    char           coords[20];



    x = NINT (event->x);
    y = NINT (event->y);
    drag = 0;


    /*  We only want to update things if we've actually moved the cursor.  */

    if (x != prev_x || y != prev_y)
      {
        freq = pow (10.0, (l_low2mid_adj->lower + (((double) x / 
            (double) EQ_curve_width) * EQ_curve_range_x)));


        gain = ((((double) EQ_curve_height - (double) y) / 
            (double) EQ_curve_height) * EQ_curve_range_y) + 
            l_eqb1_adj->lower;

        s_gain = -(EQ_SPECTRUM_RANGE - (((((double) EQ_curve_height - 
            (double) y) / (double) EQ_curve_height) * EQ_SPECTRUM_RANGE)));

        sprintf (coords, _("%dHz , EQ : %ddb , Spectrum : %ddb"), NINT (freq),
            NINT (gain), NINT (s_gain));
        gtk_label_set_text (l_EQ_curve_lbl, coords);


        /*  If we're in the midst of drawing the curve...  */

        if (EQ_drawing)
          {
            /*  Only allow the user to draw in the positive direction.  */

            if (!EQ_input_points || x > EQ_xinput[EQ_input_points - 1])
              {
                if (EQ_input_points) gdk_draw_line (EQ_drawable, EQ_gc, 
                    NINT (EQ_xinput[EQ_input_points - 1]), 
                    NINT (EQ_yinput[EQ_input_points - 1]), x, y);

                size = (EQ_input_points + 1) * sizeof (float);
                EQ_xinput = (float *) realloc (EQ_xinput, size);
                EQ_yinput = (float *) realloc (EQ_yinput, size);

                if (EQ_yinput == NULL)
                  {
                    perror (_("Allocating EQ_yinput in callbacks.c"));
                    clean_quit ();
                  }

                EQ_xinput[EQ_input_points] = (float) x;
                EQ_yinput[EQ_input_points] = (float) y;
                EQ_input_points++;
              }
          }
        else if (EQ_drag_fa)
          {
            freq = log10f (freq);
            gtk_range_set_value ((GtkRange *) l_low2mid, freq);
          }
        else if (EQ_drag_fb)
          {
            freq = log10f (freq);
            gtk_range_set_value ((GtkRange *) l_mid2high, freq);
          }
        else
          {
            notch_flag = -1;


            /*  Check for notch drag.  */

            for (i = 0 ; i < NOTCHES ; i++)
              {
                if (EQ_notch_drag[i])
                  {
                    /*  If we're shifted we're raising or lowering notch 
                        gain.  */

                    if (event->state & GDK_SHIFT_MASK)
                      {
                        /*  Assuming +-90dbFS.  */

                        if (y>= 0 && y <= EQ_curve_height)
                          {
                            EQ_notch_gain[i] = -90.0 + (180.0 * 
                                ((EQ_curve_height - y) / EQ_curve_height));


                            /*  We only need to rewrite the annotation, not
                                redraw everything.  */

                            EQ_write_annotation (EQ_notch_handle[0][1][i], 
                                EQ_notch_handle[1][1][i], EQ_notch_gain[i]);

                            drag = 1;
                            notch_flag = i;
                            break;
                          }
                      }
                    else
                      {
                        if (x >= 0 && x <= EQ_curve_width)
                          {
                            EQ_notch_index[i] = nearest_x (freq);
                            draw_EQ_curve ();
                            drag = 1;
                            notch_flag = i;
                            break;
                          }
                      }
                  }


                /*  Dragging the Q/width handles for the notch filters.  */

                if (EQ_notch_Q_drag[i])
                  {
                    if (x >= 0 && x <= EQ_curve_width)
                      {
                        j = nearest_x (freq);
                        if (EQ_notch_index[i] > j)
                          {
                            EQ_notch_width[i] = EQ_notch_index[i] - j;
                          }
                        else
                          {
                            EQ_notch_width[i] = j - EQ_notch_index[i];
                          }
                        if (!EQ_notch_width[i]) EQ_notch_width[i] = 1;

                        draw_EQ_curve ();
                        drag = 1;
                        notch_flag = i;
                        break;
                      }
                  }
              }


            /*  If we're not dragging a notch filter...  */

            if (!drag)
              {
                /*  If we pass over any of the handles we want to change the
                    cursor.  */

                cursor = GDK_PENCIL;

                if (EQ_drag_fa || EQ_drag_fb) cursor = GDK_SB_H_DOUBLE_ARROW;

                diffx_fa = abs (x - xover_handle_fa);
                diffx_fb = abs (x - xover_handle_fb);

                if ((diffx_fa <= XOVER_HANDLE_HALF_SIZE ||
                    diffx_fb <= XOVER_HANDLE_HALF_SIZE) &&
                    (y <= XOVER_HANDLE_SIZE ||
                    y >= EQ_curve_height - XOVER_HANDLE_SIZE)) 
                    cursor = GDK_SB_H_DOUBLE_ARROW;


                /*  No point in checking all these if we're already passing 
                    over one of the xover bars.  */

                if (cursor != GDK_SB_H_DOUBLE_ARROW)
                  {
                    for (i = 0 ; i < NOTCHES ; i++)
                      {

                        if (EQ_notch_drag[i] || EQ_notch_Q_drag[i])
                          {
                            if (event->state & GDK_SHIFT_MASK)
                              {
                                cursor = GDK_SB_V_DOUBLE_ARROW;
                              }
                            else
                              {
                                cursor = GDK_SB_H_DOUBLE_ARROW;
                              }
                            notch_flag = i;
                            break;
                          }

                        diff_notch[0] = abs (x - EQ_notch_handle[0][1][i]);
                        diff_notch[1] = abs (y - EQ_notch_handle[1][1][i]);

                        if (diff_notch[0] <= NOTCH_HANDLE_HALF_WIDTH &&
                            diff_notch[1] <= NOTCH_HANDLE_HALF_HEIGHT)
                          {
                            if (event->state & GDK_SHIFT_MASK)
                              {
                                cursor = GDK_SB_V_DOUBLE_ARROW;
                              }
                            else
                              {
                                cursor = GDK_SB_H_DOUBLE_ARROW;
                              }
                            notch_flag = i;
                            break;
                          }

                        if (i && i < NOTCHES - 1)
                          {
                            diff_notch[0] = abs (x - EQ_notch_handle[0][0][i]);
                            diff_notch[1] = abs (y - EQ_notch_handle[1][0][i]);

                            if (diff_notch[0] <= NOTCH_HANDLE_HALF_WIDTH &&
                                diff_notch[1] <= NOTCH_HANDLE_HALF_HEIGHT)
                              {
                                cursor = GDK_SB_H_DOUBLE_ARROW;
                                notch_flag = i;
                                break;
                              }


                            diff_notch[0] = abs (x - EQ_notch_handle[0][2][i]);
                            diff_notch[1] = abs (y - EQ_notch_handle[1][2][i]);

                            if (diff_notch[0] <= NOTCH_HANDLE_HALF_WIDTH &&
                                diff_notch[1] <= NOTCH_HANDLE_HALF_HEIGHT)
                              {
                                cursor = GDK_SB_H_DOUBLE_ARROW;
                                notch_flag = i;
                                break;
                              }
                          }
                      }
                  }


                if (current_cursor != cursor)
                  {
                    current_cursor = cursor;
                    gdk_window_set_cursor (EQ_drawable, 
                        gdk_cursor_new (cursor));
                  }
              }


            if (notch_flag != -1)
              {
                i = EQ_notch_index[notch_flag] - EQ_notch_width[notch_flag];
                if (i < 0 || notch_flag == 0) i = 0;
                j = EQ_notch_index[notch_flag] + EQ_notch_width[notch_flag];
                if (j >= EQ_length || notch_flag == NOTCHES - 1) 
                    j = EQ_length - 1;
                lo = NINT (pow (10.0, EQ_xinterp[i]));
                hi = NINT (pow (10.0, EQ_xinterp[j]));
                sprintf (coords, _("%+03ddb , %dHz - %dHz"), 
                    NINT (EQ_notch_gain[notch_flag]), lo, hi);
                gtk_label_set_text (l_EQ_curve_lbl, coords);
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
    float               *x = NULL, *y = NULL, interval;
    int                 diffx_fa, diffx_fb, diff_notch[2], i, j, i_start = 0, 
                        i_end = 0, size, ex, ey;
    static int          interp_pad = 5;


    ex = event->x;
    ey = event->y;

    switch (event->button)
      {

        /*  Button 1 - start drawing or end drawing unless we're over a notch
            or xover handle in which case we will be grabbing and sliding the
            handle in the X direction.  Shift button 1 is for grabbing and 
            sliding in the Y direction (notch/shelf filters only - look at the
            motion  callback).  */

      case 1:

        /*  Start drawing.  */

        if (!EQ_drawing)
          {
            /*  Checking for position over xover bar or notch handles.  */

            diffx_fa = abs (ex - xover_handle_fa);
            diffx_fb = abs (ex - xover_handle_fb);
            if (diffx_fa <= XOVER_HANDLE_HALF_SIZE && 
                (ey <= XOVER_HANDLE_SIZE ||
                ey >= EQ_curve_height - XOVER_HANDLE_SIZE))
              {
                EQ_drag_fa = 1;
                xover_active = 1;
              }
            else if (diffx_fb <= XOVER_HANDLE_HALF_SIZE && 
                (ey <= XOVER_HANDLE_SIZE || 
                ey >= EQ_curve_height - XOVER_HANDLE_SIZE))
              {
                EQ_drag_fb = 1;
                xover_active = 1;
              }
            else
              {
                for (i = 0 ; i < NOTCHES ; i++)
                  {
                    diff_notch[0] = abs (ex - EQ_notch_handle[0][1][i]);
                    diff_notch[1] = abs (ey - EQ_notch_handle[1][1][i]);

                    if (diff_notch[0] <= NOTCH_HANDLE_HALF_WIDTH &&
                        diff_notch[1] <= NOTCH_HANDLE_HALF_HEIGHT)
                      {
                        EQ_notch_drag[i] = 1;
                        xover_active = 1;
                        break;
                      }


                    if (i && i < NOTCHES - 1)
                      {
                        diff_notch[0] = abs (ex - EQ_notch_handle[0][0][i]);
                        diff_notch[1] = abs (ey - EQ_notch_handle[1][0][i]);

                        if (diff_notch[0] <= NOTCH_HANDLE_HALF_WIDTH &&
                            diff_notch[1] <= NOTCH_HANDLE_HALF_HEIGHT)
                          {
                            EQ_notch_Q_drag[i] = 1;
                            xover_active = 1;

                            break;
                          }


                        diff_notch[0] = abs (ex - EQ_notch_handle[0][2][i]);
                        diff_notch[1] = abs (ey - EQ_notch_handle[1][2][i]);

                        if (diff_notch[0] <= NOTCH_HANDLE_HALF_WIDTH &&
                            diff_notch[1] <= NOTCH_HANDLE_HALF_HEIGHT)
                          {
                            EQ_notch_Q_drag[i] = 1;
                            xover_active = 1;

                            break;
                          }
                      }
                  }


                /*  If we aren't over a handle we must be starting to draw 
                    the curve so mark the starting point.  */

                if (!xover_active)
                  {
                    /*  Save the first point so we can do real narrow EQ 
                        changes.  */

                    size = (EQ_input_points + 1) * sizeof (float);
                    EQ_xinput = (float *) realloc (EQ_xinput, size);
                    EQ_yinput = (float *) realloc (EQ_yinput, size);

                    if (EQ_yinput == NULL)
                      {
                        perror (_("Allocating EQ_yinput in callbacks.c"));
                        clean_quit ();
                      }

                    EQ_xinput[EQ_input_points] = (float) ex;
                    EQ_yinput[EQ_input_points] = (float) ey;
                    EQ_input_points++;

                    EQ_drawing = 1;
                  }
              }
          }


        /*  End drawing - combine the drawn data with any parts of the 
            previous that haven't been superceded by what was drawn.  Use 
            an "interp_pad" cushion on either side of the drawn section 
            so it will merge nicely with the old data.  */

        else
          {
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
                    i_start = i - interp_pad;
                    break;
                  }
              }

            for (i = EQ_length - 1 ; i >= 0 ; i--)
              {
                if (EQ_xinterp[i] <= EQ_xinput[EQ_input_points - 1])
                  {
                    i_end = i + interp_pad;
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
                    perror (_("Allocating y in callbacks.c"));
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
                    perror (_("Allocating y in callbacks.c"));
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


            /*  Recompute the splined curve in the log(freq) domain for
                plotting the EQ.  */

            interpolate (EQ_interval, j, EQ_start, EQ_end, &EQ_length, x, 
                y, EQ_xinterp, EQ_yinterp);


            /*  Recompute the splined curve in the freq domain for setting 
                the eq_coefs.  */

            for (i = 0 ; i < j ; i++)
                x[i] = pow (10.0, (double) x[i]);

            interval = ((l_geq_freqs[EQ_BANDS - 1]) - l_geq_freqs[0]) / 
                EQ_INTERP;

            interpolate (interval, j, l_geq_freqs[0], 
                l_geq_freqs[EQ_BANDS - 1], &EQ_length, x, y, 
                EQ_freq_xinterp, EQ_freq_yinterp);


            if (x) free (x);
            if (y) free (y);


            EQ_input_points = 0;


            /*  Set the graphic EQ sliders based on the hand-drawn curve.  */

            geq_set_sliders (EQ_length, EQ_freq_xinterp, EQ_freq_yinterp);


            /*  Set EQ coefficients based on the hand-drawn curve.  */

            geq_set_coefs (EQ_length, EQ_freq_xinterp, EQ_freq_yinterp);


            EQ_mod = 0;


            /*  Redraw the curve.  */

            draw_EQ_curve ();
          }
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
    int           i;


    switch (event->button)
      {
      case 1:
        if (EQ_drawing == 1)
          {
            EQ_drawing = 2;
          }
        else if (EQ_drawing == 2)
          {
            EQ_drawing = 0;
          }
        break;


        /*  Button 2 or 3 - discard the drawn curve.  */

      case 2:
      case 3:
        EQ_drawing = 0;

        EQ_input_points = 0;

        draw_EQ_curve ();
        
        break;
      }


    xover_active = 0;
    EQ_drag_fa = 0;
    EQ_drag_fb = 0;
    for (i = 0 ; i < NOTCHES ; i++) 
      {
        EQ_notch_drag[i] = 0;
        EQ_notch_Q_drag[i] = 0;
      }

    return FALSE;
}


gboolean
on_EQ_curve_event_box_leave_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    gtk_label_set_text (l_EQ_curve_lbl, "                ");

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
        gtk_label_set_label (l_low_knee_lbl, _("Knee"));
      }
    else if (value < 0.5)
      {
        gtk_label_set_label (l_low_knee_lbl, _("Hard"));
      }
    else
      {
        gtk_label_set_label (l_low_knee_lbl, _("Soft"));
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
        gtk_label_set_label (l_mid_knee_lbl, _("Knee"));
      }
    else if (value < 0.5)
      {
        gtk_label_set_label (l_mid_knee_lbl, _("Hard"));
      }
    else
      {
        gtk_label_set_label (l_mid_knee_lbl, _("Soft"));
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
        gtk_label_set_label (l_high_knee_lbl, _("Knee"));
      }
    else if (value < 0.5)
      {
        gtk_label_set_label (l_high_knee_lbl, _("Hard"));
      }
    else
      {
        gtk_label_set_label (l_high_knee_lbl, _("Soft"));
      }
}


void
on_geq_min_gain_spinner_value_changed  (GtkSpinButton   *spinbutton,
                                        gpointer         user_data)
{
    geq_set_range (gtk_spin_button_get_value (spinbutton), geq_get_adjustment(0)->upper);
}


void
on_geq_max_gain_spinner_value_changed  (GtkSpinButton   *spinbutton,
                                        gpointer         user_data)
{
    geq_set_range (geq_get_adjustment(0)->lower, gtk_spin_button_get_value (spinbutton));
}


void
on_out_trim_scale_value_changed        (GtkRange        *range,
                                        gpointer         user_data)
{
    s_set_value_ui(S_LIM_LIMIT,
		    gtk_range_get_adjustment(GTK_RANGE(range))->value);
}


void
on_in_trim_scale_value_changed         (GtkRange        *range,
                                        gpointer         user_data)
{
    s_set_value_ui(S_IN_GAIN, gtk_range_get_adjustment(range)->value);
}


void
on_pan_scale_value_changed             (GtkRange        *range,
                                        gpointer         user_data)
{
    s_set_value_ui(S_IN_PAN, gtk_range_get_adjustment(range)->value);
}


void comp_write_annotation (int i, char *string)
{
    PangoLayout    *pl;
    PangoRectangle ink_rect;


    /*  Clear the annotation area.  */

    pl = pango_layout_new (comp_pc[i]);  
    pango_layout_set_text (pl, "-99 , -99", -1);
    pango_layout_get_pixel_extents (pl, &ink_rect, NULL);

    gdk_window_clear_area (comp_drawable[i], 3, 3, ink_rect.width + 5,
		    ink_rect.height + 5);
    gdk_gc_set_foreground (comp_gc[i], &black);

    pl = pango_layout_new (comp_pc[i]);  
    pango_layout_set_text (pl, string, -1);


    gdk_draw_layout (comp_drawable[i], comp_gc[i], 5, 5, pl);
}


void
draw_comp_curve (int i)
{
    int              j, x0, y0 = 0.0, x1 = 0.0, y1 = 0.0;
    float            x, y;
    comp_settings    comp;


    if (!comp_realized[i]) return;


    /*  Clear the curve drawing area.  */

    gdk_window_clear_area (comp_drawable[i], 0, 0, comp_curve_width[i], 
        comp_curve_height[i]);
    gdk_gc_set_foreground (comp_gc[i], &grey);
    gdk_gc_set_line_attributes (comp_gc[i], 1, GDK_LINE_SOLID, GDK_CAP_BUTT, 
        GDK_JOIN_MITER);


    /*  Plot the grid lines.  */

    for (j = NINT (comp_start_x[i]) ; j <= NINT (comp_end_x[i]) ; j++)
      {
        if (!(j % 10))
          {
            x1 = NINT (((float) (j - comp_start_x[i]) / 
                comp_curve_range_x[i]) * comp_curve_width[i]);

            gdk_draw_line (comp_drawable[i], comp_gc[i], x1, 0, x1, 
                comp_curve_height[i]);
          }
      }

    for (j = NINT (comp_start_y[i]) ; j <= NINT (comp_end_y[i]) ; j++)
      {
        if (!(j % 10))
          {
            if (!j)
              {
                gdk_gc_set_line_attributes (comp_gc[i], 2, GDK_LINE_SOLID, 
                    GDK_CAP_BUTT, GDK_JOIN_MITER);
              }
            else
              {
                gdk_gc_set_line_attributes (comp_gc[i], 1, GDK_LINE_SOLID, 
                    GDK_CAP_BUTT, GDK_JOIN_MITER);
              }

            y1 = comp_curve_height[i] - NINT (((float) (j - comp_start_y[i]) / 
                comp_curve_range_y[i]) * comp_curve_height[i]);

            gdk_draw_line (comp_drawable[i], comp_gc[i], 0, y1, 
                comp_curve_width[i], y1);
          }
      }


    /*  Plot the curves.  */

    gdk_gc_set_line_attributes (comp_gc[i], 2, GDK_LINE_SOLID, GDK_CAP_BUTT,
        GDK_JOIN_MITER);
    gdk_gc_set_foreground (comp_gc[i], &comp_color[i]);


    comp = comp_get_settings (i);

    x0 = 999.0;
    for (x = comp_start_x[i] ; x <= comp_end_x[i] ; x += 0.5f) 
      {
        x1 = NINT (((x - comp_start_x[i]) / comp_curve_range_x[i]) * 
            comp_curve_width[i]);

        y = eval_comp (comp.threshold, comp.ratio, comp.knee, x) +
	    comp.makeup_gain;

        y1 = comp_curve_height[i] - NINT (((y - comp_start_y[i]) / 
            comp_curve_range_y[i]) * comp_curve_height[i]);

        if (x0 != 999.0) 
            gdk_draw_line (comp_drawable[i], comp_gc[i], x0, y0, x1, y1);

        x0 = x1;
        y0 = y1;
      }
    gdk_gc_set_line_attributes (comp_gc[i], 1, GDK_LINE_SOLID, GDK_CAP_BUTT,
        GDK_JOIN_MITER);
    gdk_gc_set_foreground (comp_gc[i], &black);
}


void
comp_curve_expose (GtkWidget *widget, int i)
{
    /*  Since we're doing inclusive plots on the compressor curves we'll
        not decrement the width and height.  */

    comp_curve_width[i] = widget->allocation.width;
    comp_curve_height[i] = widget->allocation.height;

    draw_comp_curve (i);
}


void
comp_curve_realize (GtkWidget *widget, int i)
{
    l_comp_curve[i] = (GtkDrawingArea *) widget;

    comp_drawable[i] = widget->window;

    comp_start_x[i] = -60.0;
    comp_end_x[i] = 0.0;
    comp_start_y[i] = -60.0;
    comp_end_y[i] = 30.0;

    comp_curve_range_x[i] = comp_end_x[i] - comp_start_x[i];
    comp_curve_range_y[i] = comp_end_y[i] - comp_start_y[i];

    comp_gc[i] = widget->style->fg_gc[GTK_WIDGET_STATE (widget)];


    comp_pc[i] = gtk_widget_get_pango_context (widget);


    comp_realized[i] = 1;
}



gboolean
on_comp1_curve_expose_event            (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
    comp_curve_expose (widget, 0);

    return FALSE;
}


void
on_comp1_curve_realize                 (GtkWidget       *widget,
                                        gpointer         user_data)
{
    comp_curve_realize (widget, 0);
}


gboolean
on_comp2_curve_expose_event            (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
    comp_curve_expose (widget, 1);

    return FALSE;
}


void
on_comp2_curve_realize                 (GtkWidget       *widget,
                                        gpointer         user_data)
{
    comp_curve_realize (widget, 1);
}


gboolean
on_comp3_curve_expose_event            (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
    comp_curve_expose (widget, 2);

    return FALSE;
}


void
on_comp3_curve_realize                 (GtkWidget       *widget,
                                        gpointer         user_data)
{
    comp_curve_realize (widget, 2);
}


void 
comp_curve_box_motion (int i, GdkEventMotion  *event)
{
    float          x, y;
    char           coords[20];


    x = comp_start_x[i] + (((float) event->x / 
        (float) comp_curve_width[i]) * comp_curve_range_x[i]);


    y = comp_start_y[i] + ((((float) comp_curve_height[i] - (float) event->y) /
        (float) comp_curve_height[i]) * comp_curve_range_y[i]);


    sprintf (coords, "%d , %d    ", NINT (x), NINT (y));
    comp_write_annotation (i, coords);
}


gboolean
on_low_curve_box_motion_notify_event   (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data)
{
    comp_curve_box_motion (0, event);

    return FALSE;
}


gboolean
on_mid_curve_box_motion_notify_event   (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data)
{
    comp_curve_box_motion (1, event);

    return FALSE;
}


gboolean
on_high_curve_box_motion_notify_event  (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data)
{
    comp_curve_box_motion (2, event);

    return FALSE;
}

gboolean
on_low_curve_box_leave_notify_event    (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    draw_comp_curve (0);

    gtk_widget_modify_fg ((GtkWidget *) l_low_comp_lbl, GTK_STATE_NORMAL, 
        &comp_color[3]);
    gtk_widget_modify_fg ((GtkWidget *) l_low_curve_lbl, GTK_STATE_NORMAL, 
        &comp_color[3]);

    return FALSE;
}


gboolean
on_mid_curve_box_leave_notify_event    (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    draw_comp_curve (1);

    gtk_widget_modify_fg ((GtkWidget *) l_mid_comp_lbl, GTK_STATE_NORMAL, 
         &comp_color[3]);
    gtk_widget_modify_fg ((GtkWidget *) l_mid_curve_lbl, GTK_STATE_NORMAL, 
         &comp_color[3]);

    return FALSE;
}


gboolean
on_high_curve_box_leave_notify_event   (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    draw_comp_curve (2);

    gtk_widget_modify_fg ((GtkWidget *) l_high_comp_lbl, GTK_STATE_NORMAL,
        &comp_color[3]);
    gtk_widget_modify_fg ((GtkWidget *) l_high_curve_lbl, GTK_STATE_NORMAL, 
        &comp_color[3]);

    return FALSE;
}

gboolean
on_low_curve_box_enter_notify_event    (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    gtk_widget_modify_fg ((GtkWidget *) l_low_comp_lbl, GTK_STATE_NORMAL, 
        &comp_color[0]);
    gtk_widget_modify_fg ((GtkWidget *) l_low_curve_lbl, GTK_STATE_NORMAL, 
        &comp_color[0]);

    return FALSE;
}


void
on_low_curve_lbl_realize               (GtkWidget       *widget,
                                        gpointer         user_data)
{
    l_low_curve_lbl = (GtkLabel *) widget;
}


gboolean
on_mid_curve_box_enter_notify_event    (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    gtk_widget_modify_fg ((GtkWidget *) l_mid_comp_lbl, GTK_STATE_NORMAL, 
        &comp_color[1]);
    gtk_widget_modify_fg ((GtkWidget *) l_mid_curve_lbl, GTK_STATE_NORMAL, 
        &comp_color[1]);

    return FALSE;
}


void
on_mid_curve_lbl_realize               (GtkWidget       *widget,
                                        gpointer         user_data)
{
    l_mid_curve_lbl = (GtkLabel *) widget;
}


gboolean
on_high_curve_box_enter_notify_event   (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    gtk_widget_modify_fg ((GtkWidget *) l_high_comp_lbl, GTK_STATE_NORMAL, 
        &comp_color[2]);
    gtk_widget_modify_fg ((GtkWidget *) l_high_curve_lbl, GTK_STATE_NORMAL, 
        &comp_color[2]);

    return FALSE;
}


void
on_high_curve_lbl_realize              (GtkWidget       *widget,
                                        gpointer         user_data)
{
    l_high_curve_lbl = (GtkLabel *) widget;
}


gboolean
on_low_comp_event_box_enter_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    gtk_widget_modify_fg ((GtkWidget *) l_low_comp_lbl, GTK_STATE_NORMAL, 
        &comp_color[0]);
    gtk_widget_modify_fg ((GtkWidget *) l_low_curve_lbl, GTK_STATE_NORMAL, 
        &comp_color[0]);

    return FALSE;
}


gboolean
on_low_comp_event_box_leave_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    gtk_widget_modify_fg ((GtkWidget *) l_low_comp_lbl, GTK_STATE_NORMAL, 
        &comp_color[3]);
    gtk_widget_modify_fg ((GtkWidget *) l_low_curve_lbl, GTK_STATE_NORMAL, 
        &comp_color[3]);

    return FALSE;
}


gboolean
on_mid_comp_event_box_enter_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    gtk_widget_modify_fg ((GtkWidget *) l_mid_comp_lbl, GTK_STATE_NORMAL, 
        &comp_color[1]);
    gtk_widget_modify_fg ((GtkWidget *) l_mid_curve_lbl, GTK_STATE_NORMAL, 
        &comp_color[1]);

    return FALSE;
}


gboolean
on_mid_comp_event_box_leave_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    gtk_widget_modify_fg ((GtkWidget *) l_mid_comp_lbl, GTK_STATE_NORMAL, 
         &comp_color[3]);
    gtk_widget_modify_fg ((GtkWidget *) l_mid_curve_lbl, GTK_STATE_NORMAL, 
         &comp_color[3]);

    return FALSE;
}


gboolean
on_high_comp_event_box_enter_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    gtk_widget_modify_fg ((GtkWidget *) l_high_comp_lbl, GTK_STATE_NORMAL, 
        &comp_color[2]);
    gtk_widget_modify_fg ((GtkWidget *) l_high_curve_lbl, GTK_STATE_NORMAL, 
        &comp_color[2]);

    return FALSE;
}


gboolean
on_high_comp_event_box_leave_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    gtk_widget_modify_fg ((GtkWidget *) l_high_comp_lbl, GTK_STATE_NORMAL, 
        &comp_color[3]);
    gtk_widget_modify_fg ((GtkWidget *) l_high_curve_lbl, GTK_STATE_NORMAL, 
        &comp_color[3]);

    return FALSE;
}

GtkWidget*
make_meter (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2)
{
    GtkWidget *ret;
    gint dir = GTK_METER_UP;
    GtkAdjustment *adjustment = (GtkAdjustment*) gtk_adjustment_new (0.0,
		    (float)int1, (float)int2, 0.0, 0.0, 0.0);

    if (!string1 || !strcmp(string1, "up")) {
        dir = GTK_METER_UP;
    } else if (!strcmp(string1, "down")) {
	dir = GTK_METER_DOWN;
    } else if (!strcmp(string1, "left")) {
	dir = GTK_METER_LEFT;
    } else if (!strcmp(string1, "right")) {
	dir = GTK_METER_RIGHT;
    }

    ret = gtk_meter_new(adjustment, dir);

    return ret;
}


GtkWidget*
make_mscale (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2)
{
    int sides = 0;
    GtkWidget *ret;

    if (string1 && strstr(string1, "left")) {
	sides |= GTK_METERSCALE_LEFT;
    }
    if (string1 && strstr(string1, "right")) {
	sides |= GTK_METERSCALE_RIGHT;
    }
    if (string1 && strstr(string1, "top")) {
	sides |= GTK_METERSCALE_TOP;
    }
    if (string1 && strstr(string1, "bottom")) {
	sides |= GTK_METERSCALE_BOTTOM;
    }

    ret = gtk_meterscale_new(sides, int1, int2);

    return ret;
}

void
on_autoutton1_toggled                  (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    comp_set_auto(0, gtk_toggle_button_get_active(togglebutton));
}


void
on_autoutton2_toggled                  (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    comp_set_auto(1, gtk_toggle_button_get_active(togglebutton));
}


void
on_autoutton3_toggled                  (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    comp_set_auto(2, gtk_toggle_button_get_active(togglebutton));
}


void
on_pre_eq_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    process_set_spec_mode(SPEC_PRE_EQ);
}


void
on_post_eq_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    process_set_spec_mode(SPEC_POST_EQ);
}


void
on_post_compressor_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_output_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_button10_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
	s_undo();
}

void
on_lim_lh_scale_value_changed          (GtkRange        *range,
                                        gpointer         user_data)
{
	s_set_value_ui(S_LIM_TIME,
			gtk_range_get_adjustment(GTK_RANGE(range))->value);
}

void
on_release_val_label_realize           (GtkWidget       *widget,
                                        gpointer         user_data)
{
	GtkRequisition size;

	gtk_widget_size_request(widget, &size);
	gtk_widget_set_usize(widget, size.width, -1);
}

void
on_hscale2_value_changed               (GtkRange        *range,
                                        gpointer         user_data)
{
	s_set_value_ui(S_STEREO_WIDTH(0),
			gtk_range_get_adjustment(GTK_RANGE(range))->value);
}


void
on_hscale2_realize                     (GtkWidget       *widget,
                                        gpointer         user_data)
{
	s_set_adjustment(S_STEREO_WIDTH(0),
			gtk_range_get_adjustment(GTK_RANGE(widget)));
}


void
on_hscale3_value_changed               (GtkRange        *range,
                                        gpointer         user_data)
{
	s_set_value_ui(S_STEREO_WIDTH(1),
			gtk_range_get_adjustment(GTK_RANGE(range))->value);

}


void
on_hscale3_realize                     (GtkWidget       *widget,
                                        gpointer         user_data)
{
	s_set_adjustment(S_STEREO_WIDTH(1),
			gtk_range_get_adjustment(GTK_RANGE(widget)));
}


void
on_hscale4_value_changed               (GtkRange        *range,
                                        gpointer         user_data)
{
	s_set_value_ui(S_STEREO_WIDTH(2),
			gtk_range_get_adjustment(GTK_RANGE(range))->value);
}


void
on_hscale4_realize                     (GtkWidget       *widget,
                                        gpointer         user_data)
{
	s_set_adjustment(S_STEREO_WIDTH(2),
			gtk_range_get_adjustment(GTK_RANGE(widget)));
}


void
on_lim_input_hscale_value_changed      (GtkRange        *range,
                                        gpointer         user_data)
{
	s_set_value_ui(S_LIM_INPUT,
			gtk_range_get_adjustment(GTK_RANGE(range))->value);
}


void
on_lim_input_hscale_realize            (GtkWidget       *widget,
                                        gpointer         user_data)
{
	s_set_adjustment(S_LIM_INPUT,
			gtk_range_get_adjustment(GTK_RANGE(widget)));
}


int get_current_notebook1_page ()
{
    return (gtk_notebook_get_current_page (notebook1));
}

void
on_notebook1_realize                   (GtkWidget       *widget,
                                        gpointer         user_data)
{
    notebook1 = (GtkNotebook *) widget;
}
