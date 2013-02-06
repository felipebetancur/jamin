/*
 *  Copyright (C) 2003 Steve Harris
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
 *  $Id: presets-ui.c,v 1.3 2013/02/06 03:42:39 kotau Exp $
 */

#include <stdio.h>
#include <gtk/gtk.h>
#include <math.h>

#include "process.h"
#include "support.h"
#include "main.h"
#include "gtkmeter.h"
#include "state.h"
#include "db.h"

//extern int global_gui; 

static GtkButton *tButton;


void presets_ui_update ()
{
	
	tButton = GTK_BUTTON(lookup_widget(presets_window, "eButton1"));
	
	if(global_gui){
		gtk_widget_show(main_window);
		gtk_button_set_label(tButton, "hide");
		gui_mode = 0;
			
	}else {
		gtk_widget_hide(main_window);
		gtk_button_set_label(tButton, "show");
		gui_mode = 1;
		
	}
	//g_print("clicked");
		
}

/* vi:set ts=8 sts=4 sw=4: */
