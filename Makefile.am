## Process this file with automake to produce Makefile.in

SUBDIRS = src pixmaps examples po

EXTRA_DIST = \
	autogen.sh \
	jam-ui.glade \
	jam-ui.gladep

files-test: src/jamin
	find test/files/ -name '*.jam' -exec ./src/jamin -f \{\} \;
