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
#include "help.h"


static GtkWidget *help_dialog;


void help_message (char *string)
{
    help_dialog = gtk_message_dialog_new ((GtkWindow *) main_window,
                                          GTK_DIALOG_DESTROY_WITH_PARENT,
                                          GTK_MESSAGE_INFO,
                                          GTK_BUTTONS_CLOSE,
                                          "%s", string);

    g_signal_connect_swapped (GTK_OBJECT (help_dialog), "response",
                           G_CALLBACK (gtk_widget_destroy),
                           GTK_OBJECT (help_dialog));

    gtk_widget_show (help_dialog);


    gdk_window_set_cursor (main_window->window, 
                           gdk_cursor_new (GDK_LEFT_PTR));
}
