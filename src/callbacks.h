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
