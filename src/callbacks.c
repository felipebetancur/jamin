#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <gtk/gtk.h>

#include "main.h"
#include "callbacks.h"
#include "callbacks_help.h"
#include "geq.h"
#include "hdeq.h"
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
#include "help.h"

/* vi:set ts=8 sts=4 sw=4: */


static char *help_ptr = general_help;


void
on_low2mid_value_changed               (GtkRange        *range,
                                        gpointer         user_data)
{
    hdeq_low2mid_set (range);
}


void
on_mid2high_value_changed              (GtkRange        *range,
                                        gpointer         user_data)
{
    hdeq_mid2high_set (range);
}


gboolean
on_low2mid_button_press_event          (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
    hdeq_low2mid_button (1);

    return FALSE;
}


gboolean
on_low2mid_button_release_event        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
    hdeq_low2mid_button (0);

    return FALSE;
}


gboolean
on_mid2high_button_press_event        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
    hdeq_mid2high_button (1);

    return FALSE;
}


gboolean
on_mid2high_button_release_event      (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
    hdeq_mid2high_button (0);

    return FALSE;
}


void
on_low2mid_realize                     (GtkWidget       *widget,
                                        gpointer         user_data)
{
    hdeq_low2mid_init ();
}


void
on_mid2high_realize                    (GtkWidget       *widget,
                                        gpointer         user_data)
{
    hdeq_mid2high_init ();
}


void
on_quit_button_clicked                        (GtkButton       *button,
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
on_window1_show                        (GtkWidget       *widget,
                                        gpointer         user_data)
{
    crossover_init ();
}


gboolean
eqb_mod (GtkAdjustment *adj, gpointer user_data)
{
    hdeq_eqb_mod ();

    return FALSE;
}


gboolean
on_EQ_curve_expose_event               (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
    hdeq_curve_exposed (widget);

    return FALSE;
}


void
on_EQ_curve_realize                    (GtkWidget       *widget,
                                        gpointer         user_data)
{
    hdeq_curve_init (widget);
}


gboolean 
on_EQ_curve_event_box_motion_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data)
{
    hdeq_curve_motion (event);

    return FALSE;
}


gboolean
on_EQ_curve_event_box_button_press_event
                                        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
    hdeq_curve_button_press (event);

    return FALSE;
}


gboolean
on_EQ_curve_event_box_button_release_event
                                        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
    hdeq_curve_button_release (event);

    return FALSE;
}


gboolean
on_EQ_curve_event_box_leave_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    hdeq_curve_set_label ("                ");

    return FALSE;
}


void
on_bypass_button_toggled               (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    global_bypass = gtk_toggle_button_get_active(togglebutton);
}




void
on_geq_min_gain_spinner_value_changed  (GtkSpinButton   *spinbutton,
                                        gpointer         user_data)
{
    float gain;


    gain = gtk_spin_button_get_value (spinbutton);

    hdeq_set_lower_gain (gain);

    geq_set_range (gain, geq_get_adjustment(0)->upper);
}


void
on_geq_max_gain_spinner_value_changed  (GtkSpinButton   *spinbutton,
                                        gpointer         user_data)
{
    float gain;


    gain = gtk_spin_button_get_value (spinbutton);

    hdeq_set_upper_gain (gain);

    geq_set_range (geq_get_adjustment(0)->lower, gain);
}


void
on_lim_out_trim_scale_value_changed        (GtkRange        *range,
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

    comp_box_leave (0);

    return FALSE;
}


gboolean
on_mid_curve_box_leave_notify_event    (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    draw_comp_curve (1);

    comp_box_leave (1);

    return FALSE;
}


gboolean
on_high_curve_box_leave_notify_event   (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    draw_comp_curve (2);

    comp_box_leave (2);

    return FALSE;
}

gboolean
on_low_curve_box_enter_notify_event    (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    comp_box_enter (0);

    return FALSE;
}


gboolean
on_mid_curve_box_enter_notify_event    (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    comp_box_enter (1);

    return FALSE;
}


gboolean
on_high_curve_box_enter_notify_event   (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    comp_box_enter (2);

    return FALSE;
}


gboolean
on_low_comp_event_box_enter_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    comp_box_enter (0);

    return FALSE;
}


gboolean
on_low_comp_event_box_leave_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    comp_box_leave (0);

    return FALSE;
}


gboolean
on_mid_comp_event_box_enter_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    comp_box_enter (1);

    return FALSE;
}


gboolean
on_mid_comp_event_box_leave_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    comp_box_leave (1);

    return FALSE;
}


gboolean
on_high_comp_event_box_enter_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    comp_box_enter (2);

    return FALSE;
}


gboolean
on_high_comp_event_box_leave_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    comp_box_leave (2);

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
on_autobutton1_toggled                  (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    comp_set_auto(0, gtk_toggle_button_get_active(togglebutton));
}


void
on_autobutton2_toggled                  (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    comp_set_auto(1, gtk_toggle_button_get_active(togglebutton));
}


void
on_autobutton3_toggled                  (GtkToggleButton *togglebutton,
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
on_undo_button_clicked                    (GtkButton       *button,
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


void
on_save_button_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkFileSelection    *file_selector;
    gchar               string[512];


    file_selector = 
       (GtkFileSelection *) gtk_file_selection_new ("Select a session file");

    if (getenv ("HOME") != NULL)
      {
        snprintf (string, 511, "%s/.jam/", getenv ("HOME"));
        gtk_file_selection_set_filename (file_selector, string);
      }

    gtk_file_selection_complete (file_selector, "*.jam");

    g_signal_connect (GTK_OBJECT (file_selector->ok_button),
        "clicked", G_CALLBACK (s_save_session), file_selector);

    g_signal_connect_swapped (GTK_OBJECT (file_selector->ok_button),
        "clicked", G_CALLBACK (gtk_widget_destroy), (gpointer) file_selector);

    g_signal_connect_swapped (GTK_OBJECT (file_selector->cancel_button),
        "clicked", G_CALLBACK (gtk_widget_destroy), (gpointer) file_selector);

    gtk_widget_show ((GtkWidget *) file_selector);
}


void
on_load_button_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkFileSelection    *file_selector;
    gchar               string[512];


    file_selector = 
       (GtkFileSelection *) gtk_file_selection_new ("Select a session file");

    if (getenv ("HOME") != NULL)
      {
        sprintf (string, "%s/.jam/", getenv ("HOME"));
        gtk_file_selection_set_filename (file_selector, string);
      }

    gtk_file_selection_complete (file_selector, "*.jam");

    g_signal_connect (GTK_OBJECT (file_selector->ok_button),
        "clicked", G_CALLBACK (s_load_session), file_selector);

    g_signal_connect_swapped (GTK_OBJECT (file_selector->ok_button),
        "clicked", G_CALLBACK (gtk_widget_destroy), (gpointer) file_selector);

    g_signal_connect_swapped (GTK_OBJECT (file_selector->cancel_button),
        "clicked", G_CALLBACK (gtk_widget_destroy), (gpointer) file_selector);

    gtk_widget_show ((GtkWidget *) file_selector);
}


void
on_optionmenu1_realize                 (GtkWidget       *widget,
                                        gpointer         user_data)
{
    gtk_option_menu_set_history (GTK_OPTION_MENU(widget), 1);
}


void
on_high_meter_lbl_realize              (GtkWidget       *widget,
                                        gpointer         user_data)
{

}


void
on_low_meter_lbl_realize               (GtkWidget       *widget,
                                        gpointer         user_data)
{

}


void
on_mid_meter_lbl_realize               (GtkWidget       *widget,
                                        gpointer         user_data)
{

}


gboolean
play_toggle                            (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
    transport_play();
    return FALSE;
}


gboolean
stop_toggle                            (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
    transport_stop();
    return FALSE;
}

gboolean
rewind_transport                       (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
    transport_rewind();
    return FALSE;
}


gboolean
forward_transport                      (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{

    transport_rewind();
    return FALSE;
}

void
on_boost_scale_value_changed           (GtkRange        *range,
                                        gpointer         user_data)
{
	s_set_value_ui(S_BOOST,
			gtk_range_get_adjustment(GTK_RANGE(range))->value);
}


gboolean
on_scene1_eventbox_button_press_event  (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
  select_scene (0, event->button);

  return FALSE;
}


gboolean
on_scene2_eventbox_button_press_event  (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
  select_scene (1, event->button);

  return FALSE;
}


gboolean
on_scene3_eventbox_button_press_event  (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
  select_scene (2, event->button);

  return FALSE;
}


gboolean
on_scene4_eventbox_button_press_event  (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
  select_scene (3, event->button);

  return FALSE;
}


gboolean
on_scene5_eventbox_button_press_event  (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
  select_scene (4, event->button);

  return FALSE;
}


gboolean
on_scene6_eventbox_button_press_event  (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
  select_scene (5, event->button);

  return FALSE;
}


void
on_setscene_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    set_scene ();
}


void
on_clearscene_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    clear_scene ();
}


void
on_scene1_name_changed                 (GtkEditable     *editable,
                                        gpointer         user_data)
{
    set_scene_name (0, NULL);
}


void
on_scene2_name_changed                 (GtkEditable     *editable,
                                        gpointer         user_data)
{
    set_scene_name (1, NULL);
}


void
on_scene3_name_changed                 (GtkEditable     *editable,
                                        gpointer         user_data)
{
    set_scene_name (2, NULL);
}


void
on_scene4_name_changed                 (GtkEditable     *editable,
                                        gpointer         user_data)
{
    set_scene_name (3, NULL);
}


void
on_scene5_name_changed                 (GtkEditable     *editable,
                                        gpointer         user_data)
{
    set_scene_name (4, NULL);
}


void
on_scene6_name_changed                 (GtkEditable     *editable,
                                        gpointer         user_data)
{
    set_scene_name (5, NULL);
}

void
on_help_button_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
    help_message (help_help);
}


gboolean
on_EQ_curve_event_box_enter_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    help_ptr = hdeq_help;

    return FALSE;
}


gboolean
on_show_help                           (GtkWidget       *widget,
                                        GtkWidgetHelpType  help_type,
                                        gpointer         user_data)
{
    help_message (help_ptr);

    return FALSE;
}


gboolean
on_input_eventbox_enter_notify_event   (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    help_ptr = input_help;

    return FALSE;
}

gboolean
on_geq_eventbox_enter_notify_event     (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    help_ptr = geq_help;

    return FALSE;
}


gboolean
on_spectrum_eventbox_enter_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    help_ptr = spectrum_help;

    return FALSE;
}


gboolean
on_crossover_eventbox_enter_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    help_ptr = crossover_help;

    return FALSE;
}


gboolean
on_comp_curve_eventbox_enter_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    help_ptr = comp_curve_help;

    return FALSE;
}


gboolean
on_comp_eventbox_enter_notify_event    (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    help_ptr = comp_help;

    return FALSE;
}


gboolean
on_stereo_eventbox_enter_notify_event  (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    help_ptr = stereo_help;

    return FALSE;
}


gboolean
on_gain_meter_eventbox_enter_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    help_ptr = gain_meter_help;

    return FALSE;
}


gboolean
on_limiter_eventbox_enter_notify_event (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    help_ptr = limiter_help;

    return FALSE;
}


gboolean
on_boost_eventbox_enter_notify_event   (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    help_ptr = boost_help;

    return FALSE;
}


gboolean
on_output_eventbox_enter_notify_event  (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    help_ptr = output_help;

    return FALSE;
}

gboolean
on_help_button_enter_notify_event      (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    help_ptr = general_help;

    return FALSE;
}


gboolean
on_eq_options_eventbox_enter_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    help_ptr = eq_options_help;

    return FALSE;
}


gboolean
on_spectrum_options_eventbox_enter_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    help_ptr = spectrum_options_help;

    return FALSE;
}


gboolean
on_crossover_label_eventbox_enter_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    help_ptr = crossover_label_help;

    return FALSE;
}


gboolean
on_status_label_eventbox_enter_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    help_ptr = status_label_help;

    return FALSE;
}


gboolean
on_transport_controls_eventbox_enter_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    help_ptr = transport_controls_help;

    return FALSE;
}


gboolean
on_bypass_button_enter_notify_event    (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    help_ptr = bypass_help;

    return FALSE;
}


gboolean
on_load_button_enter_notify_event      (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    help_ptr = load_help;

    return FALSE;
}


gboolean
on_save_button_enter_notify_event      (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    help_ptr = save_help;

    return FALSE;
}


gboolean
on_undo_button_enter_notify_event      (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    help_ptr = undo_help;

    return FALSE;
}


gboolean
on_quit_button_enter_notify_event      (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    help_ptr = quit_help;

    return FALSE;
}

gboolean
on_scenes_eventbox_enter_notify_event  (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
    help_ptr = scenes_help;

    return FALSE;
}

