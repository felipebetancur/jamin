/* The JACK i/o ports for each channel are defined here */


#include <stdio.h>
#include <gtk/gtk.h>
#include <math.h>
#include <string.h>
#include <jack/jack.h>

#include "io.h"
#include "process.h"
#include "support.h"
#include "main.h"
#include "gtkmeter.h"
#include "db.h"

GtkWidget *in_menu_l, *in_menu_r, *out_menu_l, *out_menu_r;
GtkWidget *in_menuitem_l, *in_menuitem_r, *out_menuitem_l, *out_menuitem_r;
GtkWidget *in_item_l, *in_item_r, *out_item_l, *out_item_r;

static jack_client_t *client;
const char **outports, **inports;
unsigned int i;



void bind_iomenu()
{
    char in_lname[256], in_rname[256], out_lname[256], out_rname[256];
//    inports = jack_get_ports(client, NULL, NULL, 0);

 //	for (i = 0; inports[i]; ++i) {
//		printf ("%s\n", inports[i]);
//	}
//  const char *port_list[];
//  port_list = jack_get_ports(client, NULL, JACK_DEFAULT_AUDIO_TYPE, 0);
/* Input ports */


	snprintf(in_lname, 255, "left_in");
	snprintf(in_rname, 255, "right_in");
	in_menuitem_l = lookup_widget(main_window, in_lname); 
	in_menuitem_r = lookup_widget(main_window, in_rname); 
	
	in_menu_l=gtk_menu_new();
	in_item_l=gtk_menu_item_new_with_label ("blah");
	gtk_menu_shell_append (GTK_MENU_SHELL (in_menu_l), in_item_l);
	gtk_widget_show (in_item_l);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (in_menuitem_l), in_menu_l);
	
	in_menu_r=gtk_menu_new();
	in_item_r=gtk_menu_item_new_with_label ("blah");
	gtk_menu_shell_append (GTK_MENU_SHELL (in_menu_r), in_item_r);
	gtk_widget_show (in_item_r);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (in_menuitem_r), in_menu_r);


/* Output ports */


	snprintf(out_lname, 255, "left_out");
	snprintf(out_rname, 255, "right_out");
	out_menuitem_l = lookup_widget(main_window, out_lname); 
	out_menuitem_r = lookup_widget(main_window, out_rname); 
	
	out_menu_l=gtk_menu_new();
	out_item_l=gtk_menu_item_new_with_label ("blah");
	gtk_menu_shell_append (GTK_MENU_SHELL (out_menu_l), out_item_l);
	gtk_widget_show (out_item_l);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (out_menuitem_l), out_menu_l);
	
	out_menu_r=gtk_menu_new();
	out_item_r=gtk_menu_item_new_with_label ("blah");
	gtk_menu_shell_append (GTK_MENU_SHELL (out_menu_r), out_item_r);
	gtk_widget_show (out_item_r);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (out_menuitem_r), out_menu_r);

    
}
