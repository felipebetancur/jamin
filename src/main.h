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
 *  $Id: main.h,v 1.6 2004/01/07 22:55:54 joq Exp $
 */

#ifndef MAIN_H
#define MAIN_H

/* JAMin configuration file names
 *
 * JAMIN_DIR		per-user subdirectory of $HOME
 * JAMIN_UI		GTK rc file in JAMIN_DIR or JAMIN_EXAMPLES_DIR
 * JAMIN_DEFAULT	basename of default session file
 * JAMIN_DATA_DIR	installed data (pixmaps, etc.)
 * JAMIN_PIXMAP_DIR	installed images
 * JAMIN_EXAMPLES_DIR	installed examples
 */
#define JAMIN_DIR ".jamin"
#define JAMIN_UI "jamin_ui"
#define JAMIN_DEFAULT "default.jam"
#define JAMIN_DATA_DIR PACKAGE_DATA_DIR "/jamin"
#define JAMIN_PIXMAP_DIR JAMIN_DATA_DIR "/pixmaps/"
#define JAMIN_EXAMPLES_DIR JAMIN_DATA_DIR "/examples/"

extern GtkWidget *main_window;		/* Main GTK window */
extern char *jamin_dir;			/* JAMin config directory path */
extern char *default_session;		/* default session file */

#endif
