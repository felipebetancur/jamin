## Process this file with automake to produce Makefile.in

if USE_OSC
CONTROLLER = controller
else
CONTROLLER =
endif

SUBDIRS = src pixmaps examples $(CONTROLLER) po
DIST_SUBDIRS = src pixmaps examples controller po

EXTRA_DIST = \
	autogen.sh \
	jam-ui.glade \
	jam-ui.gladep

files-test: src/jamin
	find test/files/ -name '*.jam' -exec ./src/jamin -f \{\} \;
