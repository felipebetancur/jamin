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
on_low2mid2_value_changed              (GtkRange        *range,
                                        gpointer         user_data);

void
on_low2mid2_realize                    (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_low2mid_lbl_realize                 (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_mid2high2_value_changed             (GtkRange        *range,
                                        gpointer         user_data);

void
on_mid2high2_realize                   (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_mid2high_lbl_realize                (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_low2mid_value_changed               (GtkRange        *range,
                                        gpointer         user_data);

void
on_low2mid_realize                     (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_mid2high_value_changed              (GtkRange        *range,
                                        gpointer         user_data);

void
on_mid2high_realize                    (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_low2mid_realize                     (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_mid2high2_realize                   (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_mid2high_realize                    (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_label_Low_realize                   (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_label94_realize                     (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_label68_realize                     (GtkWidget       *widget,
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
