/*
 *  Copyright (C) 2003 Jan C. Depner, Jack O'Quin, Steve Harris
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
 *  $Id: main.c,v 1.40 2004/01/17 20:54:30 jdepner Exp $
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <gtk/gtk.h>
#include <limits.h>
#include <errno.h>
#include <dirent.h>

#include "main.h"
#include "interface.h"
#include "resource.h"
#include "support.h"
#include "io.h"
#include "geq.h"
#include "hdeq.h"
#include "limiter-ui.h"
#include "compressor-ui.h"
#include "intrim.h"
#include "io-menu.h"
#include "process.h"
#include "spectrum.h"
#include "stereo.h"
#include "state.h"
#include "status-ui.h"
#include "scenes.h"
#include "help.h"

GtkWidget *main_window;
char *jamin_dir = NULL;
char *default_session = NULL;
char *resource_file = NULL;		/* GTK resource file */

char user_default_session[PATH_MAX];	/* user's default session name */

static gboolean update_meters(gpointer data);
gboolean spectrum_update(gpointer data);
static void set_configuration_files(void);

int main(int argc, char *argv[])
{
    char title[128];
    int spectrum_freq, sf = 0;


#ifdef ENABLE_NLS
    bindtextdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    textdomain(GETTEXT_PACKAGE);
#endif

    printf(PACKAGE " " VERSION "\n");
    printf("(c) 2003 J. Depner, S. Harris, J. O'Quin, R. Parker and P. Shirkey\n");
    printf("This is free software, and you are welcome to redistribute it\n" 
	   "under certain conditions; see the file COPYING for details.\n");

    set_configuration_files();
    gtk_init(&argc, &argv);

    spectrum_freq = io_init(argc, argv);
    if (spectrum_freq) sf = 1000 / spectrum_freq;
        
    resource_file_parse();
    state_init();
    add_pixmap_directory(JAMIN_PIXMAP_DIR);
    main_window = create_window1();

    snprintf(title, sizeof(title), PACKAGE " " VERSION);
    gtk_window_set_title ((GtkWindow *) main_window, title);

    //status_init();

    /* bind the graphic equaliser sliders to adjustments */

    bind_geq();
    bind_hdeq();
    gtk_widget_show(main_window);
    bind_intrim();
    iomenu_bind(main_window, client, input_ports, output_ports);
    bind_limiter(); 
    bind_compressors();
    bind_spectrum();
    bind_stereo();
    bind_scenes();
    s_clear_history();

    /* start I/O processing, then run GTK main loop, until "quit" */

    io_activate();


    /* start the meter update.  */

    g_timeout_add (100, update_meters, NULL);


    /* start the spectrum update unless the frequency is set to 0.  */

    if (sf) g_timeout_add (sf, spectrum_update, NULL);


    /* If the filename has been set, load it */
    s_load_session(NULL);

    gtk_main();
    io_cleanup();

    return 0;
}

static void set_configuration_files(void)
{
    DIR *dtest;
    int fd;
    char *home_dir = getenv("HOME");

    if (home_dir) {
	jamin_dir = g_strdup_printf("%s/%s/", home_dir, JAMIN_DIR);
	if ((dtest = opendir(jamin_dir))) {
	    closedir(dtest);
	} else {			/* no ~/.jamin directory */
	    if (errno == ENOTDIR) {
		fprintf(stderr, "%s exists, but its not a directory\n",
			jamin_dir);
		jamin_dir = NULL;
	    } else if (errno == ENOENT) {
		fprintf(stderr, "%s does not exist, creating it...\n",
			jamin_dir);
		if (mkdir(jamin_dir, 0755) != 0) {
		    perror("Failed to create dir");
		    jamin_dir = NULL;
		}
	    } else {
		fprintf(stderr, "Unknown error trying to stat %s\n",
			jamin_dir);
		jamin_dir = NULL;
	    }
	}
    }

    /* see if user has defined a default.jam */
    if (jamin_dir) {
	snprintf(user_default_session, PATH_MAX,
		 "%s%s", jamin_dir, JAMIN_DEFAULT);
	if ((fd = open(user_default_session, O_RDONLY)) >= 0) {
	    close(fd);
	    default_session = user_default_session;
	}
    }

    /* if not, use the system-defined default */
    if (default_session == NULL) {
	default_session = JAMIN_EXAMPLES_DIR JAMIN_DEFAULT;
    }
}

static gboolean update_meters(gpointer data)
{
    static unsigned int count = 1;

    in_meter_value(in_peak);
    out_meter_value(out_peak);
    limiter_meters_update();
    compressor_meters_update();
    /*spectrum_update();*/
    s_crossfade_ui();
    status_set_time(main_window);

    /*  Only update the remaining status once a second.  */
    if (--count == 0) {
	    status_update (main_window);
	    count = 10;
    }

    return TRUE;
}

/* vi:set ts=8 sts=4 sw=4: */
