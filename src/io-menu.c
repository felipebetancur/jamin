/*
 *  iomenu.c -- GTK-2 interface for managing JACK port connections.
 *
 *  This is intended as a general, reusable interface for applications
 *  using GTK-2 and JACK.  
 *
 *  The bind_iomenu() initialization should be called before entering gtk_main().  
 *
 *  If using glade-2, define a menubar item named "jack_ports"
 *  and call iomenu_pull_down_ports() from its "activate" signal handler.
 *
 *  The current implementation still uses some JAMin global variables
 *  to access JACK client and port information.  This should be passed
 *  to initialization, instead. Until then...
 *
 *  You need these globals somewhere:
 *
 *  #define nchannels 2    -- The actual number of ports your app has
 *  extern jack_client_t *client;
 *  extern jack_port_t *input_ports[];
 *  extern jack_port_t *output_ports[];
 *  
 *  After that you need to set up the various i/o ports in 
 *
 *	iomenu_pull_down_ports()
 *
 */

/*
 *  Copyright (C) 2003 Patrick Shirkey, Jack O'Quin
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
 *  $Id: io-menu.c,v 1.14 2003/11/25 19:26:32 kotau Exp $
 */

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

static GtkWidget   *menubar_item;	/* "Ports" menubar item */
static GtkWidget   *pulldown_menu;	/* "Ports" pulldown menu */
static GtkWidget   *in_item;		/* "Connect" menubar item */
static GtkWidget   *out_item;		/* "Disconnect" menubar item */

static jack_port_t *selected_port;	/* currently selected port */

/* lists of currently registered input and output ports */
static const char **inports = NULL;
static const char **outports = NULL;


/* * * * * * * * * * * *   Callbacks   * * * * * * * * * * * * * * */

/* connect/disconnect function pointer prototype */
typedef void (*iomenu_callback)(GtkWidget *widget, char *port_name);

/* dislay error message.  Should this be a pop-up? */
static void
iomenu_error(char *fmt, ...)
{
    va_list ap;
    char buffer[300];

    va_start(ap, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, ap);
    va_end(ap);

    fprintf(stderr, "iomenu error: %s\n", buffer);
}

/* make connection */
static void
iomenu_connect(GtkWidget *widget, char *port_name)
{
    const char *selected_name = jack_port_name(selected_port);

    if (jack_port_flags(selected_port) & JackPortIsInput) {
	fprintf(stderr, "connecting port %s to %s\n",
		port_name, selected_name);
	if (jack_connect(client, port_name, selected_name)) {
	    iomenu_error("unable to connect from %s\n", port_name);
	}

    } else if (jack_port_flags(selected_port) & JackPortIsOutput) {
	fprintf(stderr, "connecting port %s to %s\n",
		selected_name, port_name);
	if (jack_connect(client, selected_name, port_name)) {
	    iomenu_error("unable to connect to %s\n", port_name);
	}
    }
}

/* break connection */
static void
iomenu_disconnect(GtkWidget *widget, char *port_name)
{
    const char *selected_name = jack_port_name(selected_port);

    if (jack_port_flags(selected_port) & JackPortIsInput) {
	fprintf(stderr, "disconnecting port %s from %s\n",
		port_name, selected_name);
	if (jack_disconnect(client, port_name, selected_name)) {
	    iomenu_error("unable to disconnect port %s\n", port_name);
	}

    } else if (jack_port_flags(selected_port) & JackPortIsOutput) {
	fprintf(stderr, "disconnecting port %s from %s\n",
		selected_name, port_name);
	if (jack_disconnect(client, selected_name, port_name)) {
	    iomenu_error("unable to disconnect port %s\n", port_name);
	}
    }
}

/* remember the local port selected */
static void
iomenu_select_port(GtkWidget *menu_item, jack_port_t *port)
{
    selected_port = port;
}

/* * * * * * * * * * * *   Menu Creation   * * * * * * * * * * * * */

GtkWidget *
iomenu_add_item(GtkWidget *menu, GtkWidget *item)
{
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
    gtk_widget_show(item);
    return item;
}

GtkWidget *
iomenu_connection_item(iomenu_callback handler, jack_port_t *port,
		       const char *connection_name)
{
    GtkWidget *item = gtk_check_menu_item_new_with_label(connection_name);
    
    if(!jack_port_connected_to(port, connection_name)){
	handler = iomenu_connect;
    } else {
	handler = iomenu_disconnect;
	gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (item), TRUE);
    }

    /* iomenu_select_port() must run before the other callback */
    g_signal_connect(G_OBJECT(item), "toggled",
		     G_CALLBACK(iomenu_select_port), port);
    g_signal_connect(G_OBJECT(item), "toggled",
		     G_CALLBACK(handler), (char *) connection_name);
    
    return item;
}

/* add a local JACK port to the interface
 *
 *  Creates a menu item for the `port', attaching it to the parent
 *  `menu'.  Attaches a submenu to this menu item containing a list of
 *  all the JACK ports to which this local port could possibly be
 *  connected.  Arranges for `handler' to be called if one of them is
 *  selected.  This will be iomenu_connect() or iomenu_disconnect().
 */
static void
iomenu_add_port(GtkWidget *menu, iomenu_callback handler, jack_port_t *port)
{
    GtkWidget *item;			/* menu item for port */
    GtkWidget *sub_menu;		/* connection submenu for this port */
    const char *port_name;

    if (port == NULL)			/* unregistered port? */
	return;

    port_name = jack_port_short_name(port);
    item = gtk_menu_item_new_with_label(port_name);
    sub_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), sub_menu);
    if (jack_port_flags(port) & JackPortIsInput) {
	int i;
	for (i = 0; outports[i]; ++i) {
	    iomenu_add_item(sub_menu,
			    iomenu_connection_item(handler, port,
						   outports[i]));
	}
    } else if (jack_port_flags(port) & JackPortIsOutput) {
	int i;
	for (i = 0; inports[i]; ++i) {
	    iomenu_add_item(sub_menu,
			    iomenu_connection_item(handler, port,
						   inports[i]));
	}
    }
    gtk_widget_show(sub_menu);
    iomenu_add_item(menu, item);
}

/* make an up-to-date list of JACK input and output port names
 *
 *   returns: 0 if successful, nonzero otherwise.
 */
static int
iomenu_list_jack_ports()
{
    if (client == NULL)			/* not connected to JACK? */
	return -1;

    if (inports)
	free(inports);
    inports = jack_get_ports(client, NULL, NULL, JackPortIsInput);

    if (outports)
	free(outports);
    outports = jack_get_ports(client, NULL, NULL, JackPortIsOutput);

    return (inports == NULL || outports == NULL);
}

/* called whenever the port menu pulldown is selected */
void
iomenu_pull_down_ports()
{
    int i;
    static GtkWidget *iports_menu = NULL; /* In ports menu */
    static GtkWidget *oports_menu = NULL; /* Out ports menu */

    if (iomenu_list_jack_ports() != 0)	/* not connected to JACK? */
	return;

    /* recreate port menu widgets, connect to menubar items */
    if (iports_menu) {
	gtk_widget_destroy(iports_menu);
	gtk_widget_destroy(oports_menu);
    }
    iports_menu = gtk_menu_new();
    oports_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(in_item), iports_menu);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(out_item), oports_menu);

    /* create menu items for each input port */
    for (i = 0; i < nchannels; ++i) {
	iomenu_add_port(iports_menu, iomenu_connect, input_ports[i]);
    }

    /* separate in and out ports */
//    iomenu_add_item(cports_menu, gtk_separator_menu_item_new());

    /* create menu items for each output port */
    for (i = 0; i < nchannels; ++i) {
	iomenu_add_port(oports_menu, iomenu_connect, output_ports[i]);
    }

    gtk_widget_show(iports_menu);
    gtk_widget_show(oports_menu);
}

/* initialization */
// JOQ: should pass in client, port info to avoid global variables.
void 
bind_iomenu()
{
    // JOQ: why is this needed?
    char name[256];

    /* find JACK Ports menubar item and pulldown menu */
    snprintf(name, sizeof(name), "jack_ports");
    menubar_item = lookup_widget(main_window, name);

    /* Build Ports menu with Connect and Disconnect items.  The actual
     * submenus are not created until the pulldown is selected. */
    pulldown_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menubar_item), pulldown_menu);
    in_item =
	iomenu_add_item(pulldown_menu,
			gtk_menu_item_new_with_label("In"));
    out_item =
	iomenu_add_item(pulldown_menu,
			gtk_menu_item_new_with_label("Out"));
    gtk_widget_show(pulldown_menu);
}
