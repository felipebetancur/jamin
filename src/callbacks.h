#include <gtk/gtk.h>


void
on_low2mid_value_changed               (GtkRange        *range,
                                        gpointer         user_data);

void
on_mid2high_value_changed              (GtkRange        *range,
                                        gpointer         user_data);

void
on_low2mid_realize                     (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_mid2high_realize                    (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_low_comp_realize                    (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_mid_comp_realize                    (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_high_comp_realize                   (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_quit_clicked                        (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_window1_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_in_trim_scale_value_changed         (GtkRange        *range,
                                        gpointer         user_data);

void
on_low2mid_lbl_realize                 (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_mid2high_lbl_realize                (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_label_Low_realize                   (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_label_Mid_realize                   (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_label_High_realize                  (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_window1_show                        (GtkWidget       *widget,
                                        gpointer         user_data);

gboolean
on_EQ_curve_configure_event            (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data);

gboolean
on_EQ_curve_expose_event               (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

void
on_EQ_curve_realize                    (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_EQ_curve_lbl_realize                (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_eqb1_realize                        (GtkWidget       *widget,
                                        gpointer         user_data);

gboolean
on_EQ_curve_event_box_motion_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data);

gboolean
on_EQ_curve_event_box_button_press_event
                                        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_EQ_curve_event_box_button_release_event
                                        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_bypass_button_toggled               (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_comp_kn_1_value_changed             (GtkRange        *range,
                                        gpointer         user_data);

void
on_comp_kn_2_value_changed             (GtkRange        *range,
                                        gpointer         user_data);

void
on_comp_kn_3_value_changed             (GtkRange        *range,
                                        gpointer         user_data);

void
on_low_knee_lbl_realize                (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_mid_knee_lbl_realize                (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_high_knee_lbl_realize               (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_geq_min_gain_spinner_value_changed  (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_geq_max_gain_spinner_value_changed  (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_out_trim_scale_value_changed        (GtkRange        *range,
                                        gpointer         user_data);

void
on_pan_scale_value_changed             (GtkRange        *range,
                                        gpointer         user_data);

gboolean
on_comp1_curve_expose_event            (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

void
on_comp1_curve_realize                 (GtkWidget       *widget,
                                        gpointer         user_data);

gboolean
on_comp2_curve_expose_event            (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

void
on_comp2_curve_realize                 (GtkWidget       *widget,
                                        gpointer         user_data);

gboolean
on_comp3_curve_expose_event            (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

void
on_comp3_curve_realize                 (GtkWidget       *widget,
                                        gpointer         user_data);

gboolean
on_low_curve_box_motion_notify_event   (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data);

gboolean
on_mid_curve_box_motion_notify_event   (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data);

gboolean
on_high_curve_box_motion_notify_event  (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data);

gboolean
on_low_curve_box_leave_notify_event    (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_mid_curve_box_leave_notify_event    (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_high_curve_box_leave_notify_event   (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_low_curve_box_leave_notify_event    (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_low_curve_box_motion_notify_event   (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data);

gboolean
on_mid_curve_box_leave_notify_event    (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_mid_curve_box_motion_notify_event   (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data);

gboolean
on_high_curve_box_leave_notify_event   (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_high_curve_box_motion_notify_event  (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data);

gboolean
on_EQ_curve_event_box_button_press_event
                                        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_EQ_curve_event_box_button_release_event
                                        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_EQ_curve_event_box_leave_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_EQ_curve_event_box_motion_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data);

gboolean
on_low_curve_box_enter_notify_event    (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

void
on_low_curve_lbl_realize               (GtkWidget       *widget,
                                        gpointer         user_data);

gboolean
on_mid_curve_box_enter_notify_event    (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

void
on_mid_curve_lbl_realize               (GtkWidget       *widget,
                                        gpointer         user_data);

gboolean
on_high_curve_box_enter_notify_event   (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

void
on_high_curve_lbl_realize              (GtkWidget       *widget,
                                        gpointer         user_data);

gboolean
on_low_comp_event_box_enter_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_low_comp_event_box_leave_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_mid_comp_event_box_enter_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_mid_comp_event_box_leave_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_high_comp_event_box_enter_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_high_comp_event_box_leave_notify_event
                                        (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

GtkWidget*
make_meter (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2);

GtkWidget*
make_mscale (gchar *widget_name, gchar *string1, gchar *string2,
		gint int1, gint int2);

void
on_autoutton1_toggled                  (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_autoutton2_toggled                  (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_autoutton3_toggled                  (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_button11_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_button12_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_pre_eq_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_post_eq_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_post_compressor_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_output_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_button10_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_lim_lh_scale_value_changed          (GtkRange        *range,
                                        gpointer         user_data);

void
on_release_val_label_realize           (GtkWidget       *widget,
                                        gpointer         user_data);

gboolean
on_low2mid_button_press_event          (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_low2mid_button_release_event        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_mid2high2_button_press_event        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_mid2high2_button_release_event      (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_hscale2_value_changed               (GtkRange        *range,
                                        gpointer         user_data);

void
on_hscale2_realize                     (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_hscale3_value_changed               (GtkRange        *range,
                                        gpointer         user_data);

void
on_hscale3_realize                     (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_hscale4_value_changed               (GtkRange        *range,
                                        gpointer         user_data);

void
on_hscale4_realize                     (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_lim_input_hscale_value_changed      (GtkRange        *range,
                                        gpointer         user_data);

void
on_lim_input_hscale_realize            (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_notebook1_realize                   (GtkWidget       *widget,
                                        gpointer         user_data);
