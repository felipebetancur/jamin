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
 *  $Id: io-menu.c,v 1.12 2003/11/24 05:02:09 joq Exp $
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

static GtkWidget   *menubar_item;	/* "Ports" menubar item */
static GtkWidget   *pulldown_menu;	/* "Ports" pulldown menu */
static GtkWidget   *connect_item;	/* "Connect" menubar item */
static GtkWidget   *disconnect_item;	/* "Disconnect" menubar item */

static jack_port_t *selected_port;	/* currently selected port */

/* lists of currently registered input and output ports */
static const char **inports;
static const char **outports;


/* * * * * * * * * * * *   Callbacks   * * * * * * * * * * * * * * */

/* connect/disconnect function pointer prototype */
typedef void (*iomenu_callback)(GtkWidget *widget, char *port_name);

/* make connection */
static void
iomenu_connect(GtkWidget *widget, char *port_name)
{
    const char *selected_name = jack_port_name(selected_port);

    if (jack_port_flags(selected_port) & JackPortIsInput) {
	fprintf(stderr, "connecting port %s to %s\n",
		port_name, selected_name);
	if (jack_connect(client, port_name, selected_name)) {
	    fprintf(stderr, "unable to connect from port %s\n", port_name);
	}

    } else if (jack_port_flags(selected_port) & JackPortIsOutput) {
	fprintf(stderr, "connecting port %s to %s\n",
		selected_name, port_name);
	if (jack_connect(client, selected_name, port_name)) {
	    fprintf(stderr, "unable to connect to port %s\n", port_name);
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
	    fprintf(stderr, "unable to disconnect port %s\n", port_name);
	}

    } else if (jack_port_flags(selected_port) & JackPortIsOutput) {
	fprintf(stderr, "disconnecting port %s from %s\n",
		selected_name, port_name);
	if (jack_disconnect(client, selected_name, port_name)) {
	    fprintf(stderr, "unable to disconnect port %s\n", port_name);
	}
    }
}

static void
iomenu_select_port(GtkWidget *menu_item, jack_port_t *port)
{
    const char *port_name = jack_port_short_name(port);

    fprintf(stderr, "selected port %s\n", port_name);
    selected_port = port;
}

/* * * * * * * * * * * *   Menu Creation   * * * * * * * * * * * * */

static void
iomenu_add_connection(GtkWidget *menu, iomenu_callback handler,
		      jack_port_t *port, const char *connection_name)
{
    GtkWidget *item;

    item = gtk_menu_item_new_with_label(connection_name);
    g_signal_connect(G_OBJECT(item), "activate",
		     G_CALLBACK(iomenu_select_port), port);
    g_signal_connect(G_OBJECT(item), "activate",
		     G_CALLBACK(handler), (char *) connection_name);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
    gtk_widget_show(item);
}

static void
iomenu_add_port(GtkWidget *menu, iomenu_callback handler, jack_port_t *port)
{
    GtkWidget *item;			/* menu item for port */
    GtkWidget *connection_menu;		/* connection menu for this port */
    const char *port_name;

    if (port == NULL)			/* unregistered port? */
	return;

    port_name = jack_port_short_name(port);
    item = gtk_menu_item_new_with_label(port_name);
    connection_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), connection_menu);
    if (jack_port_flags(port) & JackPortIsInput) {
	int i;
	for (i = 0; outports[i]; ++i) {
	    iomenu_add_connection(connection_menu, handler, port, outports[i]);
	}
    } else if (jack_port_flags(port) & JackPortIsOutput) {
	int i;
	for (i = 0; inports[i]; ++i) {
	    iomenu_add_connection(connection_menu, handler, port, inports[i]);
	}
    }
    gtk_widget_show(connection_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
    gtk_widget_show(item);
}

/* make an up-to-date list of JACK input and output port names */
static void
iomenu_list_jack_ports()
{
    if (inports)
	free(inports);
    inports = jack_get_ports(client, NULL, NULL, JackPortIsInput);

    if (outports)
	free(outports);
    outports = jack_get_ports(client, NULL, NULL, JackPortIsOutput);
}

/* append a separator to the current menu */
static void
iomenu_separator(GtkWidget *menu)
{
    GtkWidget *separator = gtk_separator_menu_item_new();

    gtk_menu_shell_append(GTK_MENU_SHELL(menu), separator);
    gtk_widget_show(separator);
}

/* called whenever the port menu pulldown is selected */
void
iomenu_pull_down_ports()
{
    int i;
    static GtkWidget *cports_menu = NULL; /* connection ports menu */
    static GtkWidget *dports_menu = NULL; /* disconnection ports menu */

    if (client == NULL)			/* not connected to JACK? */
	return;

    iomenu_list_jack_ports();

    /* Recreate port menu widgets, connect to menubar items.  Tried
     * connecting a single ports_menu to both connect_item and
     * disconnect_item, but GTK complains. */
    if (cports_menu) {
	gtk_widget_destroy(cports_menu);
	gtk_widget_destroy(dports_menu);
    }
    cports_menu = gtk_menu_new();
    dports_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(connect_item), cports_menu);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(disconnect_item), dports_menu);

    /* create menu items for each input port */
    for (i = 0; i < nchannels; ++i) {
	iomenu_add_port(cports_menu, iomenu_connect, input_ports[i]);
	iomenu_add_port(dports_menu, iomenu_disconnect, input_ports[i]);
    }

    /* separate in and out ports */
    iomenu_separator(cports_menu);
    iomenu_separator(dports_menu);

    /* create menu items for each output port */
    for (i = 0; i < nchannels; ++i) {
	iomenu_add_port(cports_menu, iomenu_connect, output_ports[i]);
	iomenu_add_port(dports_menu, iomenu_disconnect, output_ports[i]);
    }

    gtk_widget_show(cports_menu);
    gtk_widget_show(dports_menu);
}

/* initialization */
// JOQ: should pass in client, port info to avoid global variables.
void 
bind_iomenu()
{
    char name[256];

    /* find JACK Ports menubar item and pulldown menu */
    snprintf(name, sizeof(name), "jack_ports");
    menubar_item = lookup_widget(main_window, name);

    /* Build Ports menu with Connect and Disconnect items.  The actual
     * submenus are not created until the pulldown is selected. */
    pulldown_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menubar_item), pulldown_menu);

    connect_item = gtk_menu_item_new_with_label("Connect");
    gtk_menu_shell_append(GTK_MENU_SHELL(pulldown_menu), connect_item);
    gtk_widget_show(connect_item);
	
    disconnect_item = gtk_menu_item_new_with_label("Disconnect");
    gtk_menu_shell_append(GTK_MENU_SHELL(pulldown_menu), disconnect_item);
    gtk_widget_show(disconnect_item);

    gtk_widget_show(pulldown_menu);
}
