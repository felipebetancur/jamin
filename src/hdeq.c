/*
 *  hdeq.c -- Hand drawn EQ, crossover, and compressor graph interface for
 *            the JAMin (JACK Audio Mastering interface) program.
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


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <gtk/gtk.h>


#include "hdeq.h"
#include "main.h"
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
#include "transport.h"
#include "scenes.h"


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
#define MOTION_CLOCK_DIFF             ((int) (sysconf (_SC_CLK_TCK) * 0.05))


void interpolate (float, int, float, float, int *, float *, float *, float *, 
                  float *);
static void draw_EQ_curve ();


/* vi:set ts=8 sts=4 sw=4: */

static GtkHScale       *l_low2mid, *l_mid2high;
static GtkWidget       *l_comp[3];
static GtkLabel        *l_low2mid_lbl, *l_mid2high_lbl, *l_comp_lbl[3], 
                       *l_EQ_curve_lbl, *l_c_curve_lbl[3];
static GtkDrawingArea  *l_EQ_curve, *l_comp_curve[3];
static GdkDrawable     *EQ_drawable, *comp_drawable[3];
static GdkColormap     *colormap = NULL;
static GdkColor        white, black, comp_color[4], EQ_back_color, 
                       EQ_fore_color, EQ_spectrum_color, EQ_grid_color, 
                       EQ_notch_color;
static GdkGC           *EQ_gc, *comp_gc[3];
static PangoContext    *comp_pc[3], *EQ_pc;
static GtkAdjustment   *l_low2mid_adj;
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
                       EQ_notch_gain[NOTCHES] = {0.0, 0.0, 0.0, 0.0, 0.0},
                       EQ_x_notched[EQ_INTERP + 1], 
                       EQ_y_notched[EQ_INTERP + 1], EQ_gain_lower = -12.0, 
                       EQ_gain_upper = 12.0;
static int             EQ_mod = 1, EQ_drawing = 0, EQ_input_points = 0, 
                       EQ_length = 0, comp_realized[3] = {0, 0, 0}, 
                       EQ_cleared = 1, EQ_realized = 0, xover_active = 0,
                       xover_handle_fa, xover_handle_fb, EQ_drag_fa = 0,
                       EQ_drag_fb = 0, EQ_partial = 0, part_x[2], part_y[2],
                       EQ_notch_drag[NOTCHES] = {0, 0, 0, 0, 0}, 
                       EQ_notch_Q_drag[NOTCHES] = {0, 0, 0, 0, 0},
                       EQ_notch_handle[2][3][NOTCHES], 
                       EQ_notch_width[NOTCHES] = {0, 5, 5, 5, 0},
                       EQ_notch_index[NOTCHES] = {20, NOTCH_INT, 2 * NOTCH_INT,
                       3 * NOTCH_INT, EQ_INTERP - 20}, 
                       EQ_notch_flag[NOTCHES] = {0, 0, 0, 0, 0};
static guint           notebook1_page = 0;


/*  Clear out the hand drawn EQ curves on exit.  */

void clean_quit ()
{
    if (EQ_xinput) free (EQ_xinput);
    if (EQ_yinput) free (EQ_yinput);

    gtk_main_quit();
}


/*  Generic color setting.  */

static void set_color (GdkColor *color, unsigned short red, 
                       unsigned short green, unsigned short blue)
{
    if (colormap == NULL) colormap = gdk_colormap_get_system ();

    color->red = red;
    color->green = green;
    color->blue = blue;

    gdk_colormap_alloc_color (colormap, color, FALSE, TRUE);
}


/*  Setup widget bindings based on names from glade-2.  DON'T CHANGE WIDGET
    NAMES in glade-2 without checking first.  */

void bind_hdeq ()
{
    l_low2mid = GTK_HSCALE (lookup_widget (main_window, "low2mid"));
    l_mid2high = GTK_HSCALE (lookup_widget (main_window, "mid2high"));
    l_low2mid_lbl = GTK_LABEL (lookup_widget (main_window, "low2mid_lbl"));
    l_mid2high_lbl = GTK_LABEL (lookup_widget (main_window, "mid2high_lbl"));

    l_comp[0] = lookup_widget (main_window, "frame_l");
    l_comp[1] = lookup_widget (main_window, "frame_m");
    l_comp[2] = lookup_widget (main_window, "frame_h");

    l_comp_lbl[0] = GTK_LABEL (lookup_widget (main_window, "label_freq_l"));
    l_comp_lbl[1] = GTK_LABEL (lookup_widget (main_window, "label_freq_m"));
    l_comp_lbl[2] = GTK_LABEL (lookup_widget (main_window, "label_freq_h"));

    l_EQ_curve = GTK_DRAWING_AREA (lookup_widget (main_window, "EQ_curve"));
    l_EQ_curve_lbl = GTK_LABEL (lookup_widget (main_window, "EQ_curve_lbl"));

    l_comp_curve[0] = GTK_DRAWING_AREA (lookup_widget (main_window, 
                                                       "comp1_curve"));
    l_comp_curve[1] = GTK_DRAWING_AREA (lookup_widget (main_window, 
                                                       "comp2_curve"));
    l_comp_curve[2] = GTK_DRAWING_AREA (lookup_widget (main_window, 
                                                       "comp3_curve"));

    l_c_curve_lbl[0] = GTK_LABEL (lookup_widget (main_window, 
                                                 "low_curve_lbl"));
    l_c_curve_lbl[1] = GTK_LABEL (lookup_widget (main_window, 
                                                 "mid_curve_lbl"));
    l_c_curve_lbl[2] = GTK_LABEL (lookup_widget (main_window, 
                                                 "high_curve_lbl"));


    set_color (&white, 65535, 65535, 65535);
    set_color (&black, 0, 0, 0);
    set_color (&EQ_notch_color, 65535, 65535, 0);
    set_color (&comp_color[0], 60000, 0, 0);
    set_color (&comp_color[1], 0, 50000, 0);
    set_color (&comp_color[2], 0, 0, 60000);
    set_color (&comp_color[3], 0, 0, 0);
    set_color (&EQ_back_color, 0, 21611, 0);
    set_color (&EQ_fore_color, 65535, 65535, 65535);
    set_color (&EQ_grid_color, 0, 36611, 0);
    set_color (&EQ_spectrum_color, 32768, 32768, 32768);
}


/*  Setting the low to mid crossover.  Called from callbacks.c.  */

void hdeq_low2mid_set (GtkRange *range)
{
    double          value, other_value, lvalue, mvalue, hvalue;
    char            label[6];


    value = gtk_range_get_value (range);
    other_value = gtk_range_get_value ((GtkRange *) l_mid2high);
    s_set_value_ui(S_XOVER_FREQ(0), value);


    /*  Don't let the two sliders cross each other and desensitize the mid
        band compressor if they are the same value.  */

    if (value >= other_value)
      {
	s_suppress_push();
        gtk_range_set_value ((GtkRange *) l_mid2high, value);
	s_suppress_pop();
        gtk_widget_set_sensitive (l_comp[1], FALSE);
      }
    else
      {
        gtk_widget_set_sensitive (l_comp[1], TRUE);
      }


    /*  If the low slider is at the bottom of it's range, desensitize the low 
        band compressor.  */

    l_low2mid_adj = gtk_range_get_adjustment (range);

    if (value == l_low2mid_adj->lower)
      {
        gtk_widget_set_sensitive (l_comp[0], FALSE);
      }
    else
      {
        gtk_widget_set_sensitive (l_comp[0], TRUE);
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
    gtk_label_set_label (l_comp_lbl[1], label);

    lvalue = pow (10.0, l_low2mid_adj->lower);
    mvalue = pow (10.0, value);
    sprintf (label, _("Low : %d - %d"), NINT (lvalue), NINT (mvalue));
    gtk_label_set_label (l_comp_lbl[0], label);

    draw_EQ_curve ();
}


/*  Setting the mid to high crossover.  Called from callbacks.c.  */

void hdeq_mid2high_set (GtkRange *range)
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
	s_suppress_push();
        gtk_range_set_value ((GtkRange *) l_low2mid, value);
	s_suppress_pop();
        gtk_widget_set_sensitive (l_comp[1], FALSE);
      }
    else
      {
        gtk_widget_set_sensitive (l_comp[1], TRUE);
      }


    /*  If the slider is at the top of it's range, desensitize the high band
        compressor.  */


    if (value == l_low2mid_adj->upper)
      {
        gtk_widget_set_sensitive (l_comp[2], FALSE);
      }
    else
      {
        gtk_widget_set_sensitive (l_comp[2], TRUE);
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
    gtk_label_set_label (l_comp_lbl[1], label);

    hvalue = pow (10.0, l_low2mid_adj->upper);
    sprintf (label, _("High : %d - %d"), NINT (mvalue), NINT (hvalue));
    gtk_label_set_label (l_comp_lbl[2], label);

    draw_EQ_curve ();
}


/*  Someone has pressed the low to mid crossover button.  Called from 
    callbacks.c.  */

void hdeq_low2mid_button (int active)
{
    xover_active = active;

    set_scene_warning_button ();
}


/*  Someone has pressed the mid to high crossover button.  Called from 
    callbacks.c.  */

void hdeq_mid2high_button (int active)
{
    xover_active = active;

    set_scene_warning_button ();
}


/*  Initialize the low to mid crossover adjustment state.  */

void hdeq_low2mid_init ()
{
    s_set_adjustment(S_XOVER_FREQ(0), gtk_range_get_adjustment(GTK_RANGE(l_low2mid)));
}


/*  Initialize the mid to high crossover adjustment state.  */

void hdeq_mid2high_init ()
{
    s_set_adjustment(S_XOVER_FREQ(1), gtk_range_get_adjustment(GTK_RANGE(l_mid2high)));
}


/*  Set the low to mid and mid to high crossovers.  This is from the 
    window1_show callback so it only gets called once.  */

void crossover_init ()
{
    hdeq_low2mid_set ((GtkRange *) l_low2mid);
    hdeq_mid2high_set ((GtkRange *) l_mid2high);
}


/*  If we've modified the graphic EQ (geq) then we want to build the hand
    drawn EQ from the geq.  This flag will cause that to happen on the next
    redraw.  */

void hdeq_eqb_mod ()
{
    EQ_mod = 1;
}


/*  Convert log frequency to X pixels in the hdeq.  */

static void
logfreq2xpix (float log_freq, int *x)
{
    *x = NINT (((log_freq - l_low2mid_adj->lower) / EQ_curve_range_x) * 
        EQ_curve_width);
}


/*  Convert frequency to X pixels in the hdeq.  */

static void 
freq2xpix (float freq, int *x)
{
    float log_freq;

    log_freq = log10f (freq);
    logfreq2xpix (log_freq, x);
}


/*  Convert gain to Y pixels in the hdeq.  */

static void 
gain2ypix (float gain, int *y)
{
    *y = EQ_curve_height - NINT (((gain - EQ_gain_lower) / 
                EQ_curve_range_y) * EQ_curve_height);
}


/*  Convert log gain to Y pixels in the hdeq.  */

static void
loggain2ypix (float log_gain, int *y)
{
    float gain;

    gain = log_gain * 20.0;
    gain2ypix (gain, y);
}


/*  Draw the spectrum in the hdeq window.  This is called from spectrum update
    which is called based on the timer set up in main.c.  */

void draw_EQ_spectrum_curve (float single_levels[])
{
    static int     x[EQ_INTERP], y[EQ_INTERP];
    int            i;
    float          step, range, freq;


    /*  Don't update if we're drawing an EQ curve.  */

    if (!EQ_drawing && !xover_active)
      {
        /*  Plot the curve.  */

        gdk_gc_set_foreground (EQ_gc, &EQ_spectrum_color);
        gdk_gc_set_function (EQ_gc, GDK_XOR);
        gdk_gc_set_line_attributes (EQ_gc, 1, GDK_LINE_SOLID, GDK_CAP_BUTT,
                                    GDK_JOIN_MITER);


        /*  If we've just cleared (redrawn) the curve, don't erase the previous
            line.  */

        if (EQ_partial || !EQ_cleared)
          {
            for (i = 1 ; i < EQ_INTERP ; i++)
              {
                if (!EQ_partial || x[i] < part_x[0] || x[i] > part_x[1] ||
                    y[i] < part_y[0] || y[i] > part_y[1])
                  {
                    gdk_draw_line (EQ_drawable, EQ_gc, x[i - 1], y[i - 1], 
                                   x[i], y[i]);
                  }
              }
          }
        EQ_partial = 0;


        /*  Convert the single levels to db, plot, and save the pixel positions
            so that we can erase them on the next pass.  */

        range = l_geq_freqs[EQ_BANDS - 1] - l_geq_freqs[0];
        step = range / (float) EQ_INTERP;

        for (i = 0 ; i < EQ_INTERP ; i++)
          {
            freq = l_geq_freqs[0] + (float) i * step;


            freq2xpix (freq, &x[i]);


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


/*  Given the frequency this returns the nearest array index in the X direction
    in the hand drawn EQ curve.  This will be one of 1024 values.  */

static int nearest_x (float freq)
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


/*  Set the graphic EQ (geq) sliders and the full set of EQ coefficients
    based on the hand drawn EQ curve.  */

static void set_EQ ()
{
    float    *x = NULL, *y = NULL, interval;
    int      i, size;


    /*  Make sure we have enough space.  */

    size = EQ_length * sizeof (float);
    x = (float *) realloc (x, size);
    y = (float *) realloc (y, size);

    if (y == NULL)
      {
        perror (_("Allocating y in callbacks.c"));
        clean_quit ();
      }


    /*  Recompute the splined curve in the freq domain for setting 
        the eq_coefs.  */

    for (i = 0 ; i < EQ_length ; i++)
        x[i] = pow (10.0, (double) EQ_x_notched[i]);

    interval = ((l_geq_freqs[EQ_BANDS - 1]) - l_geq_freqs[0]) / EQ_INTERP;

    interpolate (interval, EQ_length, l_geq_freqs[0], 
        l_geq_freqs[EQ_BANDS - 1], &EQ_length, x, EQ_y_notched, 
        EQ_freq_xinterp, EQ_freq_yinterp);


    if (x) free (x);
    if (y) free (y);


    /*  Set EQ coefficients based on the hand-drawn curve.  */

    geq_set_coefs (EQ_length, EQ_freq_xinterp, EQ_freq_yinterp);


    /*  Set the graphic EQ sliders based on the hand-drawn curve.  */

    geq_set_sliders (EQ_length, EQ_freq_xinterp, EQ_freq_yinterp);

    EQ_mod = 0;
}


/*  Place the sliding notch filters in the hand drawn EQ curve.  */

static void insert_notch ()
{
    int        i, j, ndx, left, right, length;
    float      x[5], y[5];


    for (i = 0 ; i < EQ_length ; i++)
      {
        EQ_x_notched[i] = EQ_xinterp[i];
        EQ_y_notched[i] = EQ_yinterp[i];
      }


    for (j = 0 ; j < NOTCHES ; j++)
      {
        if (EQ_notch_flag[j])
          {
            if (!j)
              {
                ndx = EQ_notch_index[j];
                for (i = 0 ; i < ndx - 10 ; i++)
                    EQ_y_notched[i] = EQ_notch_gain[j];

                x[0] = EQ_x_notched[ndx - 10];
                y[0] = EQ_notch_gain[j];
                x[1] = EQ_x_notched[ndx - 9];
                y[1] = EQ_notch_gain[j];
                x[2] = EQ_x_notched[ndx - 1];
                y[2] = EQ_y_notched[ndx - 1];
                x[3] = EQ_x_notched[ndx];
                y[3] = EQ_y_notched[ndx];

                interpolate (EQ_interval, 4, x[0], x[3], &length, x, 
                    y, &EQ_x_notched[ndx - 10], &EQ_y_notched[ndx - 10]);
              }
            else if (j == NOTCHES - 1)
              {
                ndx = EQ_notch_index[j];
                for (i = ndx + 10 ; i < EQ_length ; i++)
                    EQ_y_notched[i] = EQ_notch_gain[j];

                x[0] = EQ_x_notched[ndx];
                y[0] = EQ_y_notched[ndx];
                x[1] = EQ_x_notched[ndx + 1];
                y[1] = EQ_y_notched[ndx + 1];
                x[2] = EQ_x_notched[ndx + 9];
                y[2] = EQ_notch_gain[j];
                x[3] = EQ_x_notched[ndx + 10];
                y[3] = EQ_notch_gain[j];

                interpolate (EQ_interval, 4, x[0], x[3], &length, x, 
                    y, &EQ_x_notched[ndx], &EQ_y_notched[ndx]);
              }
            else
              {
                left = EQ_notch_index[j] - EQ_notch_width[j];
                right = EQ_notch_index[j] + EQ_notch_width[j];

                x[0] = EQ_x_notched[left];
                y[0] = EQ_y_notched[left];
                x[1] = EQ_x_notched[left + 1];
                y[1] = EQ_y_notched[left + 1];
                x[2] = EQ_x_notched[EQ_notch_index[j]];
                y[2] = EQ_notch_gain[j];
                x[3] = EQ_x_notched[right - 1];
                y[3] = EQ_y_notched[right - 1];
                x[4] = EQ_x_notched[right];
                y[4] = EQ_y_notched[right];

                interpolate (EQ_interval, 5, x[0], x[4], &length, x, 
                    y, &EQ_x_notched[left], &EQ_y_notched[left]);
              }
          }
      }
}


/*  Draw the EQ curve.  This may be from the graphic EQ sliders if they have
    been modified.  Usually from the hand drawn EQ though.  */

static void draw_EQ_curve ()
{
    int            i, x0 = 0, y0 = 0, x1, y1, inc;
    float          x[EQ_BANDS], y[EQ_BANDS];


    /*  If we're not visible, go away.  */

    if (!EQ_realized) return;


    /*  Clear the curve drawing area.  */

    EQ_cleared = 1;
    gdk_gc_set_foreground (EQ_gc, &EQ_back_color);
    gdk_draw_rectangle (EQ_drawable, EQ_gc, TRUE, 0, 0, EQ_curve_width + 1, 
        EQ_curve_height + 1);


    /*  Draw the grid lines.  */

    geq_get_freqs_and_gains (l_geq_freqs, l_geq_gains);


    gdk_gc_set_foreground (EQ_gc, &EQ_grid_color);


    /*  Box around the area.  */

    gdk_gc_set_line_attributes (EQ_gc, 2, GDK_LINE_SOLID, GDK_CAP_BUTT,
        GDK_JOIN_MITER);
    gdk_draw_line (EQ_drawable, EQ_gc, 1, 1, 1, EQ_curve_height);
    gdk_draw_line (EQ_drawable, EQ_gc, 1, EQ_curve_height, EQ_curve_width, 
                   EQ_curve_height);
    gdk_draw_line (EQ_drawable, EQ_gc, EQ_curve_width, EQ_curve_height, 
                   EQ_curve_width, 1);
    gdk_draw_line (EQ_drawable, EQ_gc, EQ_curve_width, 1, 1, 1);


    /*  Frequency lines on log scale in X.  */

    gdk_gc_set_line_attributes (EQ_gc, 1, GDK_LINE_SOLID, GDK_CAP_BUTT,
        GDK_JOIN_MITER);
    i = ((int) (l_geq_freqs[0] + 10.0) / 10) * 10;
    inc = 10;
    while (i < l_geq_freqs[EQ_BANDS - 1])
      {
        for (x0 = i ; x0 <= inc * 10 ; x0 += inc)
          {
            freq2xpix ((float) x0, &x1);

            gdk_draw_line (EQ_drawable, EQ_gc, x1, 0, x1, EQ_curve_height);
          }
        i = inc * 10;
        inc *= 10;
      } 
        

    /*  Gain lines in Y.  */

    inc = 10;
    if (EQ_curve_range_y < 10.0) inc = 1;
    for (i = NINT (EQ_gain_lower) ; i < NINT (EQ_gain_upper) ; i++)
      {
        if (!(i % inc))
          {
            gain2ypix ((float) i, &y1);

            gdk_draw_line (EQ_drawable, EQ_gc, 0, y1, EQ_curve_width, y1);
          }
      }


    /*  Add the crossover bars.  */

    gdk_gc_set_line_attributes (EQ_gc, 2, GDK_LINE_SOLID, GDK_CAP_BUTT,
        GDK_JOIN_MITER);

    gdk_gc_set_foreground (EQ_gc, &comp_color[0]);
    freq2xpix (xover_fa, &x1);
    gdk_draw_line (EQ_drawable, EQ_gc, x1, 0, x1, EQ_curve_height);
    gdk_draw_rectangle (EQ_drawable, EQ_gc, TRUE, x1 - XOVER_HANDLE_HALF_SIZE,
        0, XOVER_HANDLE_SIZE, XOVER_HANDLE_SIZE);
    gdk_draw_rectangle (EQ_drawable, EQ_gc, TRUE, x1 - XOVER_HANDLE_HALF_SIZE, 
        EQ_curve_height - XOVER_HANDLE_SIZE, XOVER_HANDLE_SIZE, 
        XOVER_HANDLE_SIZE);
    gdk_gc_set_foreground (EQ_gc, &black);
    gdk_draw_rectangle (EQ_drawable, EQ_gc, FALSE, x1 - XOVER_HANDLE_HALF_SIZE,
        0, XOVER_HANDLE_SIZE, XOVER_HANDLE_SIZE);
    gdk_draw_rectangle (EQ_drawable, EQ_gc, FALSE, x1 - XOVER_HANDLE_HALF_SIZE,
        EQ_curve_height - XOVER_HANDLE_SIZE, XOVER_HANDLE_SIZE, 
        XOVER_HANDLE_SIZE);

    xover_handle_fa = x1;


    gdk_gc_set_foreground (EQ_gc, &comp_color[2]);
    freq2xpix (xover_fb, &x1);
    gdk_draw_line (EQ_drawable, EQ_gc, x1, 0, x1, EQ_curve_height);
    gdk_draw_rectangle (EQ_drawable, EQ_gc, TRUE, x1 - XOVER_HANDLE_HALF_SIZE,
        0, XOVER_HANDLE_SIZE, XOVER_HANDLE_SIZE);
    gdk_draw_rectangle (EQ_drawable, EQ_gc, TRUE, x1 - XOVER_HANDLE_HALF_SIZE,
        EQ_curve_height - XOVER_HANDLE_SIZE, XOVER_HANDLE_SIZE, 
        XOVER_HANDLE_SIZE);
    gdk_gc_set_foreground (EQ_gc, &black);
    gdk_draw_rectangle (EQ_drawable, EQ_gc, FALSE, x1 - XOVER_HANDLE_HALF_SIZE,
        0, XOVER_HANDLE_SIZE, XOVER_HANDLE_SIZE);
    gdk_draw_rectangle (EQ_drawable, EQ_gc, FALSE, x1 - XOVER_HANDLE_HALF_SIZE,
        EQ_curve_height - XOVER_HANDLE_SIZE, XOVER_HANDLE_SIZE, 
        XOVER_HANDLE_SIZE);

    xover_handle_fb = x1;

    
    /*  If we've messed with the graphics EQ sliders, recompute the splined 
        curve.  */

    if (EQ_mod) 
      {
        for (i = 0 ; i < EQ_BANDS ; i++)
          {
            x[i] = log10 (l_geq_freqs[i]);
            y[i] = log10 (l_geq_gains[i]);
          }


        interpolate (EQ_interval, EQ_BANDS, EQ_start, EQ_end, 
            &EQ_length, x, y, EQ_xinterp, EQ_yinterp);


        /*  Save state of the EQ curve.  */

        s_set_value_block (EQ_yinterp, S_EQ_GAIN(0), EQ_length);


        /*  Reset all of the shelves/notches.  */

        for (i = 0 ; i < NOTCHES ; i++)
          {
            EQ_notch_flag[i] = 0;
            EQ_notch_gain[i] = 0.0;

            if (!i || i == NOTCHES - 1)
              {
                EQ_notch_width[i] = 0;
              }
            else
              {
                EQ_notch_width[i] = 5;
              }

	    s_set_description (S_NOTCH_GAIN (i) ,
			       g_strdup_printf("Reset notch %d", i));
            s_set_value_ns (S_NOTCH_GAIN (i), EQ_notch_gain[i]);
            s_set_value_ns (S_NOTCH_Q (i), (float) EQ_notch_width[i]);
            s_set_value_ns (S_NOTCH_FLAG (i), (float) EQ_notch_flag[i]);
          }

        insert_notch ();
      }


    /*  Plot the curve.  */

    gdk_gc_set_foreground (EQ_gc, &EQ_fore_color);
    for (i = 0 ; i < EQ_length ; i++)
      {
        logfreq2xpix (EQ_x_notched[i], &x1);
        loggain2ypix (EQ_y_notched[i], &y1);

        if (i) gdk_draw_line (EQ_drawable, EQ_gc, x0, y0, x1, y1);

        x0 = x1;
        y0 = y1;
      }


    /*  Add the notch handles.  */

    for (i = 0 ; i < NOTCHES ; i++)
      {
        gdk_gc_set_foreground (EQ_gc, &EQ_notch_color);

        logfreq2xpix (EQ_x_notched[EQ_notch_index[i]], &x1);


        /*  Make the shelf handles follow the shelf.  */

        if (EQ_notch_flag[i] && (!i || i == NOTCHES - 1))
          {
            loggain2ypix (EQ_notch_gain[i], &y1);
          }
        else
          {
            loggain2ypix (EQ_y_notched[EQ_notch_index[i]], &y1);
          }

        gdk_draw_rectangle (EQ_drawable, EQ_gc, TRUE, 
            x1 - NOTCH_HANDLE_HALF_WIDTH, y1 - NOTCH_HANDLE_HALF_HEIGHT, 
            NOTCH_HANDLE_WIDTH, NOTCH_HANDLE_HEIGHT);
        gdk_gc_set_foreground (EQ_gc, &black);
        gdk_draw_rectangle (EQ_drawable, EQ_gc, FALSE, 
            x1 - NOTCH_HANDLE_HALF_WIDTH, y1 - NOTCH_HANDLE_HALF_HEIGHT, 
            NOTCH_HANDLE_WIDTH, NOTCH_HANDLE_HEIGHT);

        EQ_notch_handle[0][0][i] = EQ_notch_handle[0][1][i] = 
            EQ_notch_handle[0][2][i]= x1;
        EQ_notch_handle[1][1][i] = y1;

        if (!i)
          {
            EQ_notch_handle[0][0][i] = 0;
          }
        else
          {
            EQ_notch_handle[0][2][i] = EQ_curve_width;
          }


        /*  Notch handles, not shelf.  */

        if (i && i < NOTCHES - 1)
          {
            gdk_gc_set_foreground (EQ_gc, &EQ_notch_color);

            x0 = EQ_notch_index[i] - EQ_notch_width[i];

            logfreq2xpix (EQ_x_notched[x0], &x1);
            loggain2ypix (EQ_y_notched[x0], &y1);

            if (EQ_notch_handle[0][1][i] - x1 < NOTCH_HANDLE_HALF_WIDTH) 
                x1 = EQ_notch_handle[0][1][i] - NOTCH_HANDLE_WIDTH;

            gdk_draw_arc (EQ_drawable, EQ_gc, TRUE, 
                x1 - NOTCH_HANDLE_WIDTH, y1 - NOTCH_HANDLE_HALF_HEIGHT, 
                NOTCH_HANDLE_WIDTH * 2, NOTCH_HANDLE_HEIGHT, 5760, 11520);
            gdk_gc_set_foreground (EQ_gc, &black);
            gdk_draw_arc (EQ_drawable, EQ_gc, FALSE, 
                x1 - NOTCH_HANDLE_WIDTH, y1 - NOTCH_HANDLE_HALF_HEIGHT, 
                NOTCH_HANDLE_WIDTH * 2, NOTCH_HANDLE_HEIGHT, 5760, 11520);

            EQ_notch_handle[0][0][i] = x1;
            EQ_notch_handle[1][0][i] = y1;


            gdk_gc_set_foreground (EQ_gc, &EQ_notch_color);

            x0 = EQ_notch_index[i] + EQ_notch_width[i];

            logfreq2xpix (EQ_x_notched[x0], &x1);
            loggain2ypix (EQ_y_notched[x0], &y1);

            if (x1 - EQ_notch_handle[0][1][i] < NOTCH_HANDLE_HALF_WIDTH) 
                x1 = EQ_notch_handle[0][1][i] + NOTCH_HANDLE_WIDTH;

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


/*  Whenever the curve is exposed, which will happen on a resize, we need to
    get the current dimensions and redraw the curve.  */

void hdeq_curve_exposed (GtkWidget *widget, GdkEventExpose *event)
{
    l_low2mid_adj = gtk_range_get_adjustment ((GtkRange *) l_low2mid);
    EQ_curve_range_x = l_low2mid_adj->upper - l_low2mid_adj->lower;

    EQ_curve_range_y = EQ_gain_upper - EQ_gain_lower;


    /*  Since allocation width and height are inclusive we need to decrement
        for calculations.  */

    EQ_curve_width = widget->allocation.width - 1;
    EQ_curve_height = widget->allocation.height - 1;


    /*  If we only get part of the window exposed we don't want to redraw the
        entire spectrum.  */

    if (event->area.height != widget->allocation.height ||
        event->area.width != widget->allocation.width ||
        event->area.x != widget->allocation.x ||
        event->area.y != widget->allocation.y) EQ_partial = 1;

    part_x[0] = event->area.x;
    part_y[0] = event->area.y;
    part_x[1] = part_x[0] + event->area.width;
    part_y[1] = part_y[0] + event->area.height;

    draw_EQ_curve ();
}


/*  Initialize the hdeq.  This comes from the realize callback for the hdeq
    drawing area.  Called from callbacks.c.  */

void hdeq_curve_init (GtkWidget *widget)
{
    EQ_drawable = widget->window;

    EQ_gc = widget->style->fg_gc[GTK_WIDGET_STATE (widget)];

    EQ_pc = gtk_widget_get_pango_context (widget);

    geq_get_freqs_and_gains (l_geq_freqs, l_geq_gains);

    EQ_start = log10 (l_geq_freqs[0]);
    EQ_end = log10 (l_geq_freqs[EQ_BANDS - 1]);
    EQ_interval = (EQ_end - EQ_start) / EQ_INTERP;

    s_set_callback(S_NOTCH_GAIN(0), set_EQ_curve_values);

    EQ_realized = 1;
}


/*  Don't let the notches overlap.  */

static int check_notch (int notch, int new, int q)
{
    int         j, k, left, right, width, ret;


    ret = 1;


    /*  Left shelf.  */

    if (!notch)
      {
        j = EQ_notch_index[notch + 1] - EQ_notch_width[notch + 1];
        if (new >= j || new < 10) ret = 0;
      }


    /*  Right shelf.  */

    else if (notch == NOTCHES - 1)
      {
        k = EQ_notch_index[notch - 1] + EQ_notch_width[notch - 1];
        if (new <= k || new > EQ_length - 10) ret = 0;
      }


    /*  Notches.  */

    else
      {
        j = EQ_notch_index[notch - 1] + EQ_notch_width[notch - 1];
        k = EQ_notch_index[notch + 1] - EQ_notch_width[notch + 1];
        if (q == 1)
          {
            left = new;
            width = EQ_notch_index[notch] - left;
            right = left + 2 * width;

            if (EQ_notch_index[notch] - left < 5) ret = 0;
          }
        else if (q == 2)
          {
            right = new;
            width = right - EQ_notch_index[notch];
            left = right - 2 * width;

            if (right - EQ_notch_index[notch] < 5) ret = 0;
          }
        else
          {
            left = new - EQ_notch_width[notch];
            right = new + EQ_notch_width[notch];
          }
        if (left <= j || right >= k) ret = 0;
      }

    return (ret);
}


/*  This comes from the hdeq drawing area motion callback (actually the event
    box).  There are about a million things going on here.  This is basically
    the engine for the hdeq interface.  The rest of it happens in the button
    press and release handlers.  Take a look at the comments in the function 
    to see what's actually happening.  */

void hdeq_curve_motion (GdkEventMotion *event)
{
    static int     prev_x = -1, prev_y = -1, current_cursor = -1;
    int            i, j, x, y, size, diffx_fa, diffx_fb, diff_notch[2], 
                   cursor, drag, notch_flag = -1, lo, hi, clock_diff;
    float          freq, gain, s_gain;
    char           coords[20];
    clock_t        new_clock;
    static clock_t old_clock = -1;
    struct tms     buf;



    /*  Timing delay so we don't get five bazillion calls.  */

    new_clock = times (&buf);
    clock_diff = abs (new_clock - old_clock);

    if (clock_diff < MOTION_CLOCK_DIFF) return;

    old_clock = new_clock;


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
            EQ_gain_lower;

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
                gdk_gc_set_foreground (EQ_gc, &EQ_fore_color);
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
                        gain only.  */

                    if (event->state & GDK_SHIFT_MASK)
                      {
                        if (y >= 0 && y <= EQ_curve_height)
                          {
                            EQ_notch_gain[i] = (((((double) EQ_curve_height - 
                                (double) y) / (double) EQ_curve_height) * 
                                EQ_curve_range_y) + EQ_gain_lower) * 0.05;

                            drag = 1;
                            notch_flag = i;
                            EQ_notch_flag[i] = 1;

			    s_set_description (S_NOTCH_GAIN (i) ,
			       g_strdup_printf("Move notch %d", i));
                            s_set_value_ns (S_NOTCH_GAIN (i), 
                                EQ_notch_gain[i]);
                            s_set_value_ns (S_NOTCH_FLAG (i), 
                                (float) EQ_notch_flag[i]);

                            break;
                          }
                      }
                    else
                      {
                        if (x >= 0 && x <= EQ_curve_width && y >= 0 && 
                            y <= EQ_curve_height)
                          {
                            j = nearest_x (freq);
                            if (check_notch (i, j, 0))
                              {
                                EQ_notch_index[i] = nearest_x (freq);

                                EQ_notch_gain[i] = 
                                    (((((double) EQ_curve_height - 
                                    (double) y) / (double) EQ_curve_height) * 
                                    EQ_curve_range_y) + EQ_gain_lower) * 
                                    0.05;
                                EQ_notch_flag[i] = 1;

                                drag = 1;
                                notch_flag = i;

			        s_set_description (S_NOTCH_GAIN (i) ,
			            g_strdup_printf("Move notch %d", i));
                                s_set_value_ns (S_NOTCH_GAIN (i), 
                                    EQ_notch_gain[i]);
                                s_set_value_ns (S_NOTCH_FREQ (i), freq);
                                s_set_value_ns (S_NOTCH_FLAG (i), 
                                    (float) EQ_notch_flag[i]);
                              }
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
                        if (check_notch (i, j, EQ_notch_Q_drag[i]))
                          {
                            /*  Left bracket is 1, right bracket is 2.  */

                            if (EQ_notch_Q_drag[i] == 1)
                              {
                                EQ_notch_width[i] = EQ_notch_index[i] - j;
                              }
                            else
                              {
                                EQ_notch_width[i] = j - EQ_notch_index[i];
                              }

                            drag = 1;
                            notch_flag = i;

			    s_set_description (S_NOTCH_GAIN (i) ,
			       g_strdup_printf("Move notch %d", i));
			    s_set_value_ns (S_NOTCH_GAIN (i), 
                                    EQ_notch_gain[i]);
                            s_set_value_ns (S_NOTCH_Q (i), 
                                (float) EQ_notch_width[i]);
                          }
                        break;
                      }
                  }
              }


            /*  If we're dragging a notch filter...  */

            if (drag)
              {
                insert_notch ();
                set_EQ ();
                draw_EQ_curve ();
              }
            else
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
                                if (EQ_notch_drag[i])
                                  {
                                    cursor = GDK_CROSS;
                                  }
                                else
                                  {
                                    cursor = GDK_SB_H_DOUBLE_ARROW;
                                  }
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
                                cursor = GDK_CROSS;
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

                sprintf (coords, _("%ddb , %dHz - %dHz"), NINT (gain), lo, 
                    hi);
                gtk_label_set_text (l_EQ_curve_lbl, coords);
              }
          }


        prev_x = x;
        prev_y = y;

      }
}


/*  This comes from the hdeq drawing area button press callback (actually the 
    event box).  Again, many things happening here depending on the location
    of the cursor when the button is pressed.  Take a look at the comments in 
    the function to see what's actually happening.  */

void hdeq_curve_button_press (GdkEventButton *event)
{
    float               *x = NULL, *y = NULL;
    int                 diffx_fa, diffx_fb, diff_notch[2], i, j, i_start = 0, 
                        i_end = 0, size, ex, ey;
    static int          interp_pad = 5;


    ex = event->x;
    ey = event->y;

    switch (event->button)
      {

        /*  Button 1 - start drawing or end drawing unless we're over a notch
            or xover handle in which case we will be grabbing and sliding the
            handle in the X direction.  <Shift> button 1 is for grabbing and 
            sliding in the Y direction (notch/shelf filters only - look at the
            motion callback).  <Ctrl> button 1 will reset shelf and notch
            values to 0.0.  */

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
                        /*  Reset if <Ctrl> is pressed.  */

                        xover_active = 1;
                        if (event->state & GDK_CONTROL_MASK)
                          {
                            EQ_notch_flag[i] = 0;
                            EQ_notch_gain[i] = 0.0;

                            if (!i || i == NOTCHES - 1)
                              {
                                EQ_notch_width[i] = 0;
                              }
                            else
                              {
                                EQ_notch_width[i] = 5;
                              }

			    s_set_description (S_NOTCH_GAIN (i) ,
			       g_strdup_printf("Reset notch %d", i));
                            s_set_value_ns (S_NOTCH_GAIN (i), 
                                EQ_notch_gain[i]);
                            s_set_value_ns (S_NOTCH_Q (i), 
                                (float) EQ_notch_width[i]);
                            s_set_value_ns (S_NOTCH_FLAG (i), 
                                (float) EQ_notch_flag[i]);

                            insert_notch ();
                            set_EQ ();
                            draw_EQ_curve ();
                          }
                        else
                          {
                            EQ_notch_drag[i] = 1;
                          }
                        break;
                      }


                    if (i && i < NOTCHES - 1)
                      {
                        diff_notch[0] = abs (ex - EQ_notch_handle[0][0][i]);
                        diff_notch[1] = abs (ey - EQ_notch_handle[1][0][i]);

                        if (diff_notch[0] <= NOTCH_HANDLE_HALF_WIDTH &&
                            diff_notch[1] <= NOTCH_HANDLE_HALF_HEIGHT)
                          {
                            /*  Left bracket is a 1.  */

                            EQ_notch_Q_drag[i] = 1;
                            xover_active = 1;

                            break;
                          }


                        diff_notch[0] = abs (ex - EQ_notch_handle[0][2][i]);
                        diff_notch[1] = abs (ey - EQ_notch_handle[1][2][i]);

                        if (diff_notch[0] <= NOTCH_HANDLE_HALF_WIDTH &&
                            diff_notch[1] <= NOTCH_HANDLE_HALF_HEIGHT)
                          {
                            /*  Right bracket is a 2.  */

                            EQ_notch_Q_drag[i] = 2;
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
                    EQ_curve_range_y) + EQ_gain_lower) * 0.05;
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


            if (x) free (x);
            if (y) free (y);


            /*  Save state of the EQ curve.  */

            s_set_value_block (EQ_yinterp, S_EQ_GAIN(0), EQ_length);


            EQ_input_points = 0;


            /*  Replace shelf and notch areas.  */

            insert_notch ();


            /*  Set the GEQ faders and the EQ coefs.  */

            set_EQ ();


            EQ_mod = 0;


            /*  Redraw the curve.  */

            draw_EQ_curve ();
          }
        break;


      default:
        break;
      }
}


/*  This comes from the hdeq drawing area button release callback (actually 
    the event box).  Not as much going on here.  Mostly just resetting
    whatever was done in the motion and button press functions.  */

void hdeq_curve_button_release (GdkEventButton  *event)
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


        /*  Set the graphic EQ sliders based on the hand-drawn curve.  */

        geq_set_sliders (EQ_length, EQ_freq_xinterp, EQ_freq_yinterp);

        EQ_mod = 0;

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


    set_scene_warning_button ();
}


/*  Set the label in the hdeq.  */

void hdeq_curve_set_label (char *string)
{
    gtk_label_set_text (l_EQ_curve_lbl, string);
}


/*  Gets the gain values from the state functions and sets up everything.  */

void set_EQ_curve_values (int id, float value)
{
    int i;


    for (i = 0 ; i < EQ_INTERP ; i++)
      {
        EQ_yinterp[i] = s_get_value (S_EQ_GAIN (0) + i);
      }


    for (i = 0 ; i < NOTCHES ; i++)
      {
        EQ_notch_flag[i] = NINT (s_get_value (S_NOTCH_FLAG (i)));
        if (EQ_notch_flag[i])
          {
            EQ_notch_width[i] = NINT (s_get_value (S_NOTCH_Q (i)));
            EQ_notch_index[i] = nearest_x (s_get_value (S_NOTCH_FREQ (i)));
            EQ_notch_gain[i] = s_get_value (S_NOTCH_GAIN (i));
          }
      }


    /*  Replace shelf and notch areas.  */

    insert_notch ();


    /*  Set the GEQ coefs and faders.  */

    set_EQ ();


    EQ_mod = 0;


    /*  Redraw the curve.  */

    draw_EQ_curve ();
}


/*  Reset the crossovers.  */

void hdeq_set_xover ()
{
    xover_fa = pow (10.0, s_get_value (S_XOVER_FREQ(0)));
    xover_fb = pow (10.0, s_get_value (S_XOVER_FREQ(1)));
    
    hdeq_low2mid_init ();
    hdeq_mid2high_init ();
}


/*  Set the lower gain limit for the hdeq and the geq.  */

void hdeq_set_lower_gain (float gain)
{
  EQ_gain_lower = gain;

  set_scene_warning_button ();
}


/*  Set the upper gain limit for the hdeq and the geq.  */

void hdeq_set_upper_gain (float gain)
{
  EQ_gain_upper = gain;

  set_scene_warning_button ();
}


/*  Write the annotation for the compressor curves when you move the cursor in
    the curve box.  */

static void comp_write_annotation (int i, char *string)
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


/*  Draw the compressor curve (0-2).  */

void draw_comp_curve (int i)
{
    int              j, x0, y0 = 0.0, x1 = 0.0, y1 = 0.0;
    float            x, y;
    comp_settings    comp;


    if (!comp_realized[i]) return;


    /*  Clear the curve drawing area.  */

    gdk_window_clear_area (comp_drawable[i], 0, 0, comp_curve_width[i], 
        comp_curve_height[i]);
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


/*  The compressor curve expose/resize callback (0-2).  */

void comp_curve_expose (GtkWidget *widget, int i)
{
    /*  Since we're doing inclusive plots on the compressor curves we'll
        not decrement the width and height.  */

    comp_curve_width[i] = widget->allocation.width;
    comp_curve_height[i] = widget->allocation.height;

    draw_comp_curve (i);
}


/*  The compressor curve realize callback (0-2).  */

void comp_curve_realize (GtkWidget *widget, int i)
{
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


/*  The compressor curve drawing area motion callback (0-2).  */

void comp_curve_box_motion (int i, GdkEventMotion  *event)
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


/*  Leaving the box/curve, turn off highlights in the labels of the box and 
    curve.  */

void comp_box_leave (int i)
{
    gtk_widget_modify_fg ((GtkWidget *) l_comp_lbl[i], GTK_STATE_NORMAL, 
                          &comp_color[3]);
    gtk_widget_modify_fg ((GtkWidget *) l_c_curve_lbl[i], GTK_STATE_NORMAL, 
                          &comp_color[3]);
}


/*  Entering the box/curve, turn on highlights in the labels of the box and 
    curve.  */

void comp_box_enter (int i)
{
    gtk_widget_modify_fg ((GtkWidget *) l_comp_lbl[i], GTK_STATE_NORMAL, 
                          &comp_color[i]);
    gtk_widget_modify_fg ((GtkWidget *) l_c_curve_lbl[i], GTK_STATE_NORMAL, 
                          &comp_color[i]);
}


/*  Saving the current notebook page on a switch, see callbacks.c.  This saves
    us querying the GUI 10 times per second from spectrum_update.  */

void hdeq_notebook1_set_page (guint page_num)
{
    notebook1_page = page_num;
}


/*  Return the current notebook page - 0 = hdeq, 1 = geq, 2 = spectrum,
    3 = options.  */

int get_current_notebook1_page ()
{
    return (notebook1_page);
}
