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
