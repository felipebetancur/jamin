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
 *  $Id: io-menu.c,v 1.5 2003/11/20 19:30:03 kotau Exp $
 */

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

GtkWidget   *menu,
	    *in_menu, 
	    *out_menu, 
	    *in_menu_l, 
	    *in_menu_r, 
	    *out_menu_l, 
	    *out_menu_r,  
	    *in_menu_ll, 
	    *in_menu_rr, 
	    *out_menu_ll, 
	    *out_menu_rr;
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

GList *group_in_l = NULL;
GList *group_in_r = NULL;
GList *group_out_l = NULL;
GList *group_out_r = NULL;

char **outports, **inports;
unsigned int i;



void bind_iomenu()
{
    char name[256];

/* Find Generic ports menuitem */

	snprintf(name, 255, "ports");
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

/* Input/Outport port menu names */

	in_menu_ll=gtk_menu_new();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (in_item_l), in_menu_ll);
	in_menu_rr=gtk_menu_new();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (in_item_r), in_menu_rr);

	out_menu_ll=gtk_menu_new();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (out_item_l), out_menu_ll);	
	out_menu_rr=gtk_menu_new();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (out_item_r), out_menu_rr);

/* populate inports menu */

/* Scan for ports  -- need to make this happen everytime the above items are opened -- */

    inports = jack_get_ports (client, NULL, NULL, JackPortIsInput);
    outports = jack_get_ports (client, NULL, NULL, JackPortIsOutput);

	    
	    for (i = 0; inports[i]; ++i) {
	    
		in_item_ll[i]=gtk_radio_menu_item_new_with_label (group_in_l, inports[i]);
		group_in_l = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (in_item_ll));		
		gtk_menu_shell_append (GTK_MENU_SHELL (in_menu_ll), in_item_ll[i]);
		gtk_widget_show (in_item_ll[i]);
	
		in_item_rr[i]=gtk_radio_menu_item_new_with_label (group_in_r, inports[i]);
		group_in_r = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (in_item_rr));	
		gtk_menu_shell_append (GTK_MENU_SHELL (in_menu_rr), in_item_rr[i]);
		gtk_widget_show (in_item_rr[i]);

	    }
	    
/* populate outports menu */

	    for (i = 0; outports[i]; ++i) {
	    
		out_item_ll[i]=gtk_radio_menu_item_new_with_label (group_out_l, outports[i]);
		group_out_l = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (in_item_ll));		
		gtk_menu_shell_append (GTK_MENU_SHELL (out_menu_ll), out_item_ll[i]);
		gtk_widget_show (out_item_ll[i]);
	
		out_item_rr[i]=gtk_radio_menu_item_new_with_label (group_out_r, outports[i]);
		group_out_r = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (in_item_ll));		
		gtk_menu_shell_append (GTK_MENU_SHELL (out_menu_rr), out_item_rr[i]);
		gtk_widget_show (out_item_rr[i]);
	    }	
	
  
    
}
