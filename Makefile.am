## Process this file with automake to produce Makefile.in

SUBDIRS = src pixmaps examples po

EXTRA_DIST = \
	autogen.sh \
	jam-ui.glade \
	jam-ui.gladep

quickly:
	(cd src && make CFLAGS=-g jamin-interface.o)
	make all
