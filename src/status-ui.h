#ifndef STATUS_UI_H
#define STATUS_UI_H

/* JACK status display UI functions */

void status_update(GtkWidget *main_window);
void status_set_focus (GtkWidget *main_window, char *string);
void status_set_time(GtkWidget *main_window);

#endif /* STATUS_UI_H */
