/*
 *  Copyright (C) 2003 Patrick Shirkey
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
 *  $Id: io-menu.c,v 1.8 2003/11/21 03:42:11 joq Exp $
 */

/* The JACK I/O ports for each channel are defined here */


#include <stdio.h>
#include <stdlib.h>
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

GtkWidget   *menu,
	    *in_menu, 
	    *out_menu;
//GtkWidget *in_menuitem_l, *in_menuitem_r, *out_menuitem_l, *out_menuitem_r;
GtkWidget   *menuitem, 
	    *in_item, 
	    *out_item, 
	    *in_item_l, 
	    *in_item_r, 
	    *out_item_l, 
	    *out_item_r, 
	    *in_item_ll[256], 
	    *in_item_rr[256], 
	    *out_item_ll[256], 
	    *out_item_rr[256];

void bind_iomenu()
{
    char name[256];

    /* Find Generic ports menuitem */

    snprintf(name, 255, "ports1");
    menuitem = lookup_widget(main_window, name);
	
    /* In/Out menu items */
	
    menu=gtk_menu_new();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), menu);
	
    in_item=gtk_menu_item_new_with_label ("In");
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), in_item);
    gtk_widget_show (in_item);
	
    out_item=gtk_menu_item_new_with_label ("Out");
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), out_item);
    gtk_widget_show (out_item);
	
    /* Inport l/r menu items */

    in_menu=gtk_menu_new();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (in_item), in_menu);

    in_item_l=gtk_menu_item_new_with_label ("Left");
    gtk_menu_shell_append (GTK_MENU_SHELL (in_menu), in_item_l);
    gtk_widget_show (in_item_l);
	
    in_item_r=gtk_menu_item_new_with_label ("Right");
    gtk_menu_shell_append (GTK_MENU_SHELL (in_menu), in_item_r);
    gtk_widget_show (in_item_r);
	
    /* Outport l/r menu items */

    out_menu=gtk_menu_new();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (out_item), out_menu);
	
    out_item_l=gtk_menu_item_new_with_label ("Left");
    gtk_menu_shell_append (GTK_MENU_SHELL (out_menu), out_item_l);
    gtk_widget_show (out_item_l);
	
    out_item_r=gtk_menu_item_new_with_label ("Right");
    gtk_menu_shell_append (GTK_MENU_SHELL (out_menu), out_item_r);
    gtk_widget_show (out_item_r);

}

GtkWidget   *in_menu_ll = NULL;
GtkWidget   *in_menu_rr = NULL;
GtkWidget   *out_menu_ll = NULL;
GtkWidget   *out_menu_rr = NULL;

GSList      *group_in_l = NULL;
GSList      *group_in_r = NULL;
GSList      *group_out_l = NULL;
GSList      *group_out_r = NULL;

const char **inports = NULL;
const char **outports = NULL;

void
iomenu_scan_port_names()
{
    int i;

    /* populate input ports menu with JACK output ports */
    if (in_menu_ll)
	gtk_menu_item_remove_submenu(GTK_MENU_ITEM(in_item_l));
    in_menu_ll = gtk_menu_new();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM(in_item_l), in_menu_ll);

    if (in_menu_rr)
	gtk_menu_item_remove_submenu(GTK_MENU_ITEM(in_item_r));
    in_menu_rr = gtk_menu_new();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM(in_item_r), in_menu_rr);

    if (outports)
	free(outports);
    outports = jack_get_ports (client, NULL, NULL, JackPortIsOutput);

    for (i = 0; outports[i]; ++i) {

	in_item_ll[i] =
	    gtk_radio_menu_item_new_with_label (group_in_l,
						outports[i]);
	group_in_l = gtk_radio_menu_item_get_group (
	    GTK_RADIO_MENU_ITEM (in_item_ll));		
	gtk_menu_shell_append (GTK_MENU_SHELL (in_menu_ll),
			       in_item_ll[i]);
	gtk_widget_show (in_item_ll[i]);

	in_item_rr[i] =
	    gtk_radio_menu_item_new_with_label (group_in_r,
						outports[i]);
	group_in_r = gtk_radio_menu_item_get_group (
	    GTK_RADIO_MENU_ITEM (in_item_rr));	
	gtk_menu_shell_append (GTK_MENU_SHELL (in_menu_rr),
			       in_item_rr[i]);
	gtk_widget_show (in_item_rr[i]);

    }

    /* populate output ports menu with JACK input ports */

    if (out_menu_ll)
	gtk_menu_item_remove_submenu(GTK_MENU_ITEM(out_item_l));
    out_menu_ll=gtk_menu_new();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM(out_item_l), out_menu_ll);	

    if (out_menu_rr)
	gtk_menu_item_remove_submenu(GTK_MENU_ITEM(out_item_r));
    out_menu_rr=gtk_menu_new();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM(out_item_r), out_menu_rr);

    if (inports)
	free(inports);
    inports = jack_get_ports (client, NULL, NULL, JackPortIsInput);

    for (i = 0; inports[i]; ++i) {
	    
	out_item_ll[i] =
	    gtk_radio_menu_item_new_with_label (group_out_l, inports[i]);
	group_out_l =
	    gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM(in_item_ll));
	gtk_menu_shell_append (GTK_MENU_SHELL (out_menu_ll), out_item_ll[i]);
	gtk_widget_show (out_item_ll[i]);
	
	out_item_rr[i] =
	    gtk_radio_menu_item_new_with_label (group_out_r, inports[i]);
	group_out_r =
	    gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM(in_item_ll));
	gtk_menu_shell_append (GTK_MENU_SHELL (out_menu_rr), out_item_rr[i]);
	gtk_widget_show (out_item_rr[i]);
    }	
}
